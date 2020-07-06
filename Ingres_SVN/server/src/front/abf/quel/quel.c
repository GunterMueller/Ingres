
#line 5 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
/*
** Copyright (c) 1984, 2004, 2008 Ingres Corporation
*/
#ifndef VMS
#include 	<stdio.h>
#endif
#include	<compat.h>
#include	<cm.h>
#include	<me.h>
#include	<si.h>
#include	<st.h>
#include	<lo.h>
#include	<er.h>
#include	<gl.h>
#include	<sl.h>
#include	<iicommon.h>
#include	<adf.h>
#ifndef ADE_CHK_TYPE
#define ADE_CHK_TYPE(itype, type) ((itype) == (type) || (itype) == -(type))
#endif
#include	<fe.h>
#include	<afe.h>
#include	<fdesc.h>
#include	<oslconf.h>
#include	<oserr.h>
#include	<osglobs.h>
#include	<osfiles.h>
#include	<ossym.h>
#include	<ostree.h>
#include	<osquery.h>

#include	<iltypes.h>
#include	<ilops.h>
#include	<ilmenu.h>

#include	<osloop.h>

#include	<osfrs.h>
#include	<frscnsts.h>
#include	<eqsym.h>
#include	<eqgen.h>
#include	<eqfw.h>

#include	<oskeyw.h>
#include	<osmem.h>

#if defined(hp3_us5)
	#pragma OPT_LEVEL 1
#endif /* hp3_us5 */

#if defined(i64_win)
#pragma optimize("", off)
#endif

# if defined(DESKTOP)
# define frs_head    osfrs_head
# define frs_error   osfrs_error
# endif

GLOBALREF	bool		yyreswd;
# define	YYRETRY		1
# define	YYRETRYTOKEN	ID

/*
NO_OPTIM = ris_us5 rs4_us5 i64_aix
*/

/**
** Name:	oslgram.y - OSL Interpreted Frame Object Translator Grammar.
**
** Description:
**	Contains the YACC grammar (LALR(1)) for the OSL interpreted frame object
**	translator independent of the data manipulation language, QUEL or SQL.
**	This file should be merged using "eqmerge" with either "quel/quel.y"
**	or "sql/sql.y" to obtain the complete grammar for OSL/QUEL or OSL/SQL,
**	respectively.  The complete grammar defines either:
**
**	quel()	OSL/QUEL interpreted frame object translator.
**	sql()	OSL/SQL interpreted frame object translator.
**
** History:
**	Revision 5.1  86/10/17	wong
**	Initial revision.
**	10-feb-89 (marian)
**		Add conversion warning messages for begin/end transaction,
**		and abort for oslsql.
**	9/90 (Mike S)
**		Add declaration for osfrs_old. Porting change 130906 from
**		griffin
**	11/24/90 (emerson)
**		Change calling sequence for osqrydot (for bug 34590).
**	01/13/91 (emerson)
**		Fixes for bug 34837.  See productions for if_expr,
**		else_if_expr, and while_expr.
**	23-jan-1991 (mgw) Bug 33236
**		Only quote single character delimiters in copy formats,
**		not multi-char delimiters such as in "text(0)nl".
**	02/24/91 (emerson)
**		Fixes for bug 36068.  See productions for callf_par_ele,
**		position_ele, and keyword_ele.  Note that:
**		(1) 6.3/02 supported CALLPROC x (y.ALL), but didn't allow
**		    any other parameters to be passed to x.
**		(2) 6.3/03/00 erroneously disallows this construction.
**		(3) 6.3/03/01 allows this construction.  It also allows
**		    other parameters to be passed to x, and it allows
**		    "z = y.ALL" in place of "y.ALL" (z is ignored).
**		(4) 6.3/01 (and possibly earlier) through 6.3/03/01
**		    allow "z = y.ALL" in the parameters of a CALLFRAME.
**		    Although the documentation says that z must be the name
**		    of the called frame, in fact z is ignored.
**		    Perhaps future releases should give a warning if z
**		    is not the name of the called frame or procedure.
**	02/25/91 (emerson)
**		Fix for bug 36084.  See production for OSLvalue.
**	04/07/91 (emerson)
**		Modifications for local procedures.
**	04/22/91 (emerson)
**		Fix SubMState logic to handle nested submenus.
**		Add support for alerters.
**	05/07/91 (emerson)
**		More modifications for local procedures:
**		Don't allow RESUME, VALIDATE, and VALIDROW
**		in a local procedure unless they're inside a DISPLAY SUBMENU.
**		Also disallow nested local procedures.
**	09-may-1991 (mgw & davel) Bug 34530
**		Re-wrote copy_null's "WITH NULLK '(' constant ')'" case to
**		properly handle all the various constant strings.
**	07/22/91 (emerson)
**		Fixes for bug 38753: When call_name is a variable rather than
**		a literal, its node doesn't have a valid n_value, and it's
**		not appropriate to call osframesym or osprocsym.
**	07/25/91 (emerson)
**		Fix for bug 38849: Don't allow a CALLPROC of a system function.
**	07/26/91 (emerson)
**		Change alertevent to dbevent (per LRC 7-3-91).
**	02-aug-1991 (davel)
**		Fix for bug 35665 - Disallow table fields in productions below
**		OSLvalue (i.e. Table fields not valid as R.H.S. argument).
**	08/02/91 (emerson)
**		Fix for bug 31342: Enclose copy formats of c0x and d0x
**		(where x represents any single character) in quotes or
**		apostrophes.
**	08/06/91 (emerson)
**		Issue a warning if the system function DBMSINFO is used
**		in an expression (bug 21745).
**	08/06/91 (emerson)
**		Allow menu explanations to be specified as global constants
**		as well as string literals (bug 35460).
**	09-aug-1991 (davel)
**		Fix for bug 35806 - correct arguments passed to oschktypes()
**		in the insrow_ele production (they were in wrong order).
**	08/15/91 (emerson)
**		Fixes for bug 35468: Created the empty disallow_repeat
**		production, and used it after OSLcol_var in several places.
**		Also refined an error message in qualification processing.
**		Also changed the qry production to reflect the fact that
**		RepeatState may now change during the parse of the qry.
**	08/18/91 (emerson)
**		Fixes for bug 38001: call_frame and call_proc weren't
**		restoring FormObj = FormSym in the case where there
**		was no assignment of the return value.
**      08/19/91 (johnr)
**              Added hp3_us5 pragma to use the highest possible
**              optimization level and avoid a compiler failure in hp-ux 8.0
**	08/26/91 (emerson)
**		Fix a serious problem in the fix (08/02/91) that DaveL and I
**		came up with for bug 35665. See OSLcol_var and OSLnocol_var.
**	10/04/91 (emerson)
**		Replaced calls to osblkSID and osblkclose by calls to
**		new osblkbreak (for bugs 34788, 36427, 40195, 40196).
**	10/08/91 (emerson)
**		Allow labels to be assigned to UNLOADTABLE statements,
**		submenu statements, and attached queries.  (SIR 40233).
**	12/14/91 (emerson)
**		Fixes for bug 40908 in ingres_int and ingres_ref.
**	12/14/91 (emerson)
**		Part of fix for bug 38940 in inittable.
**	30-dec-1991 (davel)
**		Fix bugs 37388 and 41635 - change var_colon production
**		to always use FormSym in the osdotnode() call.
**	13-apr-92  (vijay)
**		Add NO_OPTIM for ris_us5. else compile bombs.
**	29-apr-92  (vijay)
**		Oops, no optim hint cannot be a single line comment in .yf
**		files since it is grepped out and appended to the .c file
**		with comments around. Incidentally, the compile error is
**		'system	resource has been exhausted'. (?)
**	29-apr-92 (purusho)
**          Amdahl C compiler cribs about # pragma OPT_LEVEL 1 starting
**          at column 1 and hence inserted a tab character
**	24-jul-92 (sweeney)
**	    add NO_OPTIM for apl_us5 - compiler fails when building
**	    quel.o and sql.o binaries in ../quel and ../sql with:
**	    Fatal error in /usr/apollo/lib/cc
**	    Status 023
**	14-sep-92 (davel)
**		Fixed bug 46472 - ostmpbeg() and ostmpend() statements
**		were incorrectly placed in IF, ELSEIF, and WHILE statements.
**		The ostmpbeg() should be issued before the OSLcondition, and the
**		ostmpend() should be issued before the IL code (for IF
**		and WHILE) is generated.
**	09/20/92 (emerson)
**		Fixes for bugs 38299, 44004, 44056, 46646 in "var_colon: COLID".
**		Changes in the way iiosQrNext is used (for bug 46761).
**		Several changes for bugs 39582 and 34846:
**		Changed calling sequence to osqrygen; it now sets
**		the "top of loop" sid for a SELECT loop.
**		Don't generate IL_QRYBRK after calling osqrgen;
**		osqrgen now does it when necessary.
**		Pass the IL fragment that computes the query target list
**		l-values to osmknode when building a tkQUERY node.
**		Replace the n_sub field by the N_ROW bit for tablefield nodes.
**		Changed calling sequence to osqrydot again.
**		Also, the interface to osqryidchk has changed slightly.
**	27-apr-93 (essi)
**		Generate compile-time error (rather than run-time) for field
**		names that are not on the form (bug 48349). This should only
**		apply to VALIDROW, CLEAR FIELD and CLEARROW.
**      05-Aug-93 (DonC) Bugs 53779 and 53747
**              Fix to bug 48349 broke SQL/and QUEL DDL statements that use
**              the ing_name_list production. Restored the production to its
**              former self and re-implemented a fix for 48349 by introducing
**              the production ing_field_list.
**
**	Revision 6.5
**	28-apr-92 (davel)
**		Added support for 6.5 DBMS features.
**	25-aug-92 (davel)
**		Added rule for execute_procedure.
**	24-sep-92 (davel)
**		Changes required to SET statement now that SESSION is a keyword.
**	06-oct-92 (davel)
**		Add support for SET_INGRES and INQUIRE_INGRES for keyword 
**		SESSION (which is now a reserved word).
**	12-nov-92 (davel)
**		Generate IL_CHKCONNECT after SET_INGRES "SESSION".
**	07-jan-93 (sylviap)
**		Added support for the new FRS statement:
**		   inquire_forms (int_var = exists(FORM_NAME));
**		Needed something special because EXISTS is already a keyword.
**	02-feb-93 (davel)
**		Added support for WITH clause for insertrow and loadtable
**		statements (for cell attribute support).
**	09-feb-93 (davel)
**		Added support for SET_4GL and INQUIRE_4GL.
**	11-feb-93 (davel)
**		Added OFF as a reserved word, and allow ON or OFF as a valid
**		set_forms, set_ingres, or set_4gl value as a synonym for
**		1 or 0 respectively.
**	23-feb-93 (davel)
**		Fixed bug 49823 - added 'opt_ingres_str' rule.
**	25-feb-93 (davel)
**		Fixed bug 49878 (on dbevent syntax).
**	11-mar-93 (davel)
**		Add set_connection as one of the form_stmt statements, which is
**		a little goofy, but it can't be a db_stmt as an extra 
**		IL_ENDLIST gets generated.  So we lump it in with set_ingres
**		which is its next closest cousin.  Also include the 
**		set_connection rule here rather than in sql.sy.
**	24-mar-93 (davel)
**		Fix bug 50675 - several grammar errors for DBMS statement
**		support.
**	19-apr-93 (davel)
**		Fix 51331 - allow SESSION as owner in qual_table_ref (required 
**		by DECLARE GLOBAL TEMPORARY TABLE).
**	17-may-93 (davel)
**		Fix bug 51700 - allow "type of table x.y" as a record 
**		declaration.
**	27-apr-93 (essi)
**		Generate compile-time error (rather than run-time) for field
**		names that are not on the form (bug 48349). This should only
**		apply to VALIDROW, CLEAR FIELD and CLEARROW.
**	05-Aug-93 (DonC)
**		Fix to bug 48349 broke SQL/and QUEL DDL statements that use
**		the ing_name_list production. Restored the production to its
**		former self and re-implemented a fix for 48349 by introducing
**		the production ing_field_list.
**	09-sep-93 (essi)
**		Backed out changes of 27-apr-93 and 05-Aug-93. They just caused
**		more problems than solved.
**	17-jun-93 (essi)
**		Fix bug 52810. Allow keyword parameters to start the
**		parameter list too. 
**	03-aug-1993 (mgw)
**		Added handling for LONG VARCHAR(0) etc. COPY format.
**	07/28/93 (dkh) - Added support for the PURGETABLE and RESUME
**			 NEXTFIELD/PREVIOUSFIELD statements.
**    23-Aug-93 (DonC) Bug 53868
**            Add OUTOFDATAMESSAGE = BELL support.
**      30-aug-93 (huffman)
**            add include <me.h>
**    3-sep-93 (DonC)
**	      Change of 23-Aug-93 broke the compilation of constructs using
**	      set_value where the RHS contained a variable. I removed the
**	      change. 
**    09-sep-93 (essi)
**	      Backed out changes of 27-apr-93 and 05-Aug-93. They just caused
**	      more problems than solved.
**    15-sep-93 (DonC) Bug 53868
**            Add OUTOFDATAMESSAGE = BELL support the right way.
**    20-sep-93 (DonC) 
**	      Add COPY, CREATE TABLE support for BYTE(x) and BYTE VARYING(x)
**    09-sep-93 (essi)
**	      Backed out changes of 27-apr-93 and 05-Aug-93. They just caused
**	      more problems than solved.
**    22-nov-93 (robf)
**            Add enhanced support for SET SESSION statement, including
**	      ADD/DROP PRIVILEGES clauses and more WITH options.
**    09-dec-93 (lan)
**	      Fix bug #47930 - Use of LOADTABLE in a 4GL procedure is not
**	      allowed and should be caught by the 4GL compiler.
**    11-may-94 (robf)
**             Rearrange some productions between here and sql.my to
**	       avoid "rule never reduced" errors when compiling quel.my
**    09-feb-95 (chech02)
**           Added rs4_us5 for AIX 4.1.   
**    07-feb-95 (wolf) 
**	      Inclusion of stdio.h causes compiler warnings on VAX/VMS and
**	      outright errors on Alpha/VMS.  Bracket it with ifdefs.
**    22-Jun-95 (fanra01)
**            Modified the names of the calls to frs_head and frs_error.
**            These symbols conflict with ones defined already in eqgenfrs.c
**            in embed!equel.
**	  18-dec-1995 (angusm)
**		  Remove redundant force of token to lowercase in tbl_col_name
**		  rule - can cause rejection of valid token "_state" if present
**		  in both uppercase and lowercase in source (bug 71314)
**    11-jan-1996 (toumi01; from 1.1 axp_osf port)
**            Added kchin's change for axp_osf
**            11-jan-94 (kchin)
**            Added axp_osf to the NO_OPTIM list to avoid problem
**            introduced by optimizer into 4gl compiler (oslsql,osl).
**     3-dec-1996 (rodjo04)
**            bug 79308: Put back "Hack attack" that was removed 18-dec-1995
**            (bug 71314). Modified it so that when symbol "_state" was read,
**            the hack would not be applied. Added new boolean function
**            is_state(). is_state() will return TRUE if the token read is
**            "_state" (case insensitive).
**     16-Sep-1997 (rodjo04)
**         (bug 85516) Added 'NOT NULLK WITH DEFAULT' and 
**         'NOT NULLK NOT DEFAULT' to null_clause: to correct 
**         quel syntax.
**	10-may-1999 (walro03)
**		Remove obsolete version string apl_us5.
**     06-Jul-1999 (schte01)
**         Remove NO_OPTIM for axp_osf.
**	21-Apr-1999 (consi01) Bug 95433 INGCBT 186
**	    Correct a typo in the fix for bug 85516. Changed output string
**	    from "not null with default" to "not null not default".
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	16-aug-2001 (toumi01)
**	    speculative i64_aix NO_OPTIM change for beta xlc_r - FIXME !!!
**	27-dec-2001 (somsa01)
**	    Added NO_OPTIM for i64_win to avoid 4gl compiler problems
**	    (oslsql, osl).
**	15-feb-2002 (toumi01)
**	    Support keyword retry for ABF to avoid keyword problems.
**	13-jan-2005 (abbjo03)
**	    Change yyreswd to GLOBALREF since it's GLOBALDEF'd in libequel.
**	29-Jan-2007 (kiria01) b117277
**	    Changed method of recognising SET RANDOM_SEED so that the
**	    functionality can both set the seed on the DBMS and within the
**	    local context.
**      13-Dec-2007 (hanal04) Bug 119519
**          Reset FormObj for next statement at the end of execute_procedure:
**          This is in-line with a similar reset performed in call_proc: for
**          Bug 38001.
**	21-Jan-2008 (kiria01) b119806
**	    Extended grammar for postfix operators beyond IS NULL and corrected
**          the operator priority tags
*/


/* %L Configuration */
/* %L merge "oslgram.my" */
/* %L Configuration begin */
#define yyparse quel
/* %L Configuration end */

char	*osstring();
char	*oshexvchar();
OSSYM	*osframesym();
OSSYM	*osprocsym();
STATUS	osblkbreak();
OSNODE	*osblkqry();
bool	osblkutbl();
ILREF	osvalref();
ILREF	osvarref();
bool	oschktypes();
bool	oschkstr();
OSNODE	*osformall();
OSNODE	*ostabfall();
OSNODE	*osall();
OSNODE	*iiosRefGen();
VOID	osdbtle();
VOID	osdbsqltle();
VOID	osdbwith();
VOID	osdbqtle();
VOID	osdblist();
VOID	osdbsrtkey();
VOID	op_vch_change();
char	*osnodename();
OSSYM	*osnodesym();
char	*osldform();
char	*osldtable();
OSNODE	*os_ta_check();
bool	osfrs_old();
OSSYM	*osformdecl();
OSSYM	*osobjinit();
VOID	ostmpinit();
VOID	ostmpnewproc();
OSSYM	*osdefineproc();
VOID	osendproc();
VOID	oschkundefprocs();
PTR	iiIGspStartProc();
VOID	iiIGepEndProc();
VOID	frs_insrow_with(void);

/* State flags */

static bool	ProcState = FALSE;	/* source is for a proc (not a frame) */
static bool	LocalProcState = FALSE;	/* in a local procedure */
static bool	CallState = FALSE;	/* in callframe statement (bug check) */
static bool	QryState = FALSE;	/* in query retrieve */
static bool	QloopState = FALSE;	/* in query loop */
static bool	RetInState = FALSE;	/* in retrieve loop */
static bool	ActState = FALSE;	/* in activation */
static bool	RepeatState = FALSE;	/* in repeat query */
static i4	DeclareFlags = 0;	/* context-sensitive declaration flags*/
static bool	SetSession = FALSE;	/* SET_INGRES "SESSION" specified */

/*
** Keep track of what kind of submenu we're in (if any).
** Possible kinds are DISPLAY_MENU and RUN_SUBMENU.
**
** Since submenus can be nested, we need a stack.
** I use 31 of the bits of SubMState as a stack.
** Assuming submenus aren't nested more than 31 deep
** (more will be flagged as an error), SubMState will be set as follows:
**
** SubMState = 1		initially.
** SubMState = SubMState*2 + 1	when initiating a DISPLAY_MENU.
** SubMState = SubMState*2	when initiating a RUN_MENU.
** SubMState = SubMState/2	when terminating either kind of submenu.
**
** Thus,
**
** SubMState == 1		indicates we're not in either kind of submenu.
** SubMState is even		indicates we're in a RUN_MENU.
** SubMState is odd and > 1	indicates we're in a DISPLAY_MENU.
*/
static u_i4	SubMState = 1;

/*
** Keep track of whether there's a display loop that we can jump to the top of.
**
** DispLoopCount = 1 intially, for a frame.
** DispLoopCount = 0 intially, for a procedure or local procedure.
** DispLoopCount is incremented when we open a DISPLAY SUBMENU.
** DispLoopCount is decremented when we close a DISPLAY SUBMENU.
**
** Thus, there's a display loop that we can jump to the top of
** if and only if DispLoopCount > 0.
*/
static	i4	DispLoopCount = 1;

/*
** Keep track of information about the current menu or submenu.
** Each variable allows for 1 bit of information for each level of
** nested submenu, up to a depth of 31.  The low-order bit
** always represents the current submenu (as for SubMState above).
*/
static u_i4	SubMInAlert = 0;	/* In an alerter activation block? */
static u_i4	SubMGotNonAlert = 0;	/* Found non-alerter activation block?*/

static bool	AllowForm = TRUE;
static bool	FormSpecified = FALSE;

/* Symbol table entry for top routine's form */

static OSSYM	*FormSym = NULL;

/*
** Symbol table entry for "form" for current routine
** (frame, proc, or local proc)
*/
static OSSYM	*LocalFormSym = NULL;

/* Symbol table entry for query retrieve form object */

static OSSYM	*FormObj = NULL;

static OSNODE	*QryTarget = NULL;

/* Pointer to "fragment" of IL which assigns a selected row into its targets */

static PTR	TargListFrag = NULL;

/* Holding area for the label of a SELECT loop or attached query */

static char	*QueryLabel = NULL;

/* Symbol table entry for array or tablefield being operated on by
** INSERTROW, CLEARROW, etc.
*/

static OSSYM	*TableObj = NULL;

/* Name of current frame (not form) or procedure */

static char	*FrmProcName = NULL;

/* Activation Options List */

static ILREF	ActOpt[OPT_MAX] = {0, 0, 0};

/* FRS With Clause Statement Type */

static i4	FrsStmt = 0;

/* Syntax Error Flags */
static bool	OpErr = FALSE;		/* operation in statement context */

/* %L Statics */
/* %L Statics begin */
VOID	IIOSgqsGenQryStmt();
/* %L Statics end */

/* Parser Debug */
#define YYDEBUG 1

static READONLY char	_SyntaxError[]	= ERx("syntax error");
static READONLY char	_IsNull[]	= ERx("is null");
static READONLY char	_IsInteger[]	= ERx("is integer");
static READONLY char	_IsDecimal[]	= ERx("is decimal");
static READONLY char	_IsFloat[]	= ERx("is float");
static READONLY char	_IsNotNull[]	= ERx("is not null");
static READONLY char	_IsNotInteger[]	= ERx("is not integer");
static READONLY char	_IsNotDecimal[]	= ERx("is not decimal");
static READONLY char	_IsNotFloat[]	= ERx("is not float");

#line 571 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
#define YACC	1

#line 797 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
typedef union  {
	char		*st_name;
#ifdef YACC
	OSSYM		*st_sym;
	OSNODE		*st_node;
	OSTLIST		*st_tlist;
	OSSRTNODE	*st_srtlist;
	OSQRY		*st_qry;
#endif
	i4		st_nat;
} YYSTYPE;
#define	ALL		257
#define	ARRAY_OF		258
#define	BEGIN		259
#define	BYREF		260
#define	CALL		261
#define	CALLF		262
#define	CALLP		263
#define	DECLARE		264
#define	DEREFERENCE		265
#define	DISPLAY_MENU		266
#define	DO		267
#define	ELSE		268
#define	ELSEIF		269
#define	ENDIF		270
#define	ENDLOOP		271
#define	END		272
#define	ENDWHILE		273
#define	EXE_PROC		274
#define	EXIT		275
#define	FIELD		276
#define	FIELD_ENTRY		277
#define	FIELD_EXIT		278
#define	IF		279
#define	INITIALIZE		280
#define	KEY		281
#define	MODE		282
#define	NEXT		283
#define	NULLK		284
#define	ON_DBEVENT		285
#define	PROCEDURE		286
#define	PROCEDURE_RETURNING		287
#define	CLASS_OF		288
#define	REPEAT		289
#define	RETURN		290
#define	RUN_MENU		291
#define	QUALIFICATION		292
#define	SESSION		293
#define	SYSTEM		294
#define	THEN		295
#define	USER		296
#define	WHILE		297
#define	WITH		298
#define	ABORT		299
#define	APPEND		300
#define	BEGINTRANSACTION		301
#define	COPY		302
#define	CREATE		303
#define	DEFINE		304
#define	DELETE		305
#define	DESTROY		306
#define	ENDTRANSACTION		307
#define	INDEX		308
#define	INTEGRITY		309
#define	MODIFY		310
#define	PERMIT		311
#define	RANGE		312
#define	RELOCATE		313
#define	REPLACE		314
#define	RETRIEVE		315
#define	SAVE		316
#define	SAVEPOINT		317
#define	SET		318
#define	VIEW		319
#define	BY		320
#define	AT		321
#define	AS		322
#define	FROM		323
#define	INTO		324
#define	IS		325
#define	OF		326
#define	ON		327
#define	ONLY		328
#define	ORDER		329
#define	SORT		330
#define	TO		331
#define	WHERE		332
#define	UNIQUE		333
#define	UNTIL		334
#define	DEFAULT		335
#define	BELL_TOK		336
#define	OFF		337
#define	OWNER_INGRES		338
#define	TABLE		339
#define	UPDATE		340
#define	ESCAPE		341
#define	ROLLBACK		342
#define	GRANT		343
#define	GROUP		344
#define	ASC		345
#define	DESC		346
#define	USER_AUTH		347
#define	SYSTEM_USER		348
#define	SESSION_USER		349
#define	SESSION_GROUP		350
#define	SESSION_ROLE		351
#define	ADD_PRIVILEGES		352
#define	DROP_PRIVILEGES		353
#define	DIR_CONNECT		354
#define	DIR_DISCONNECT		355
#define	DIR_EXECUTE		356
#define	IMMEDIATE		357
#define	REGISTER		358
#define	REG_TABLE		359
#define	REM_TABLE		360
#define	REG_VIEW		361
#define	REM_VIEW		362
#define	REMOVE		363
#define	CLEAR		364
#define	EXISTS		365
#define	HELPFILE		366
#define	HELP_FORMS		367
#define	MESSAGE		368
#define	NOECHO		369
#define	PRINTSCREEN		370
#define	PROMPT		371
#define	REDISPLAY		372
#define	RESUME		373
#define	RESENTRY		374
#define	RESNEXT		375
#define	RESMENU		376
#define	RESNFLD		377
#define	RESPFLD		378
#define	SCREEN		379
#define	SCROLL		380
#define	SLEEP		381
#define	VALIDATE		382
#define	INQ_FORMS		383
#define	INQ_INGRES		384
#define	SET_FORMS		385
#define	SET_INGRES		386
#define	INQ_4GL		387
#define	SET_4GL		388
#define	SET_CONNECTION		389
#define	SET_RANDOM_SEED		390
#define	CLEARROW		391
#define	DELETEROW		392
#define	INITTABLE		393
#define	INSERTROW		394
#define	VALIDROW		395
#define	LOADTABLE		396
#define	UNLOADTABLE		397
#define	PURGETBL		398
#define	COLID		399
#define	ID		400
#define	FCONST		401
#define	ICONST		402
#define	SCONST		403
#define	XCONST		404
#define	DCONST		405
#define	AND		406
#define	EXP		407
#define	GTE		408
#define	LIKE		409
#define	LTE		410
#define	NOT		411
#define	NOTLIKE		412
#define	NOTEQ		413
#define	OR		414
#define	LSQBRK		415
#define	RSQBRK		416
#define	COLEQ		417
#define	LBOP		418
#define	ROP		419
#define	BOP		420
#define	BOPH		421
#define	UOP		422
#define	UOP_POSTFIX		423
#define	yyclearin	yychar = -1
#define	yyerrok		yyerrflag = 0
extern int		yychar;
extern short		yyerrflag;
#ifndef YYMAXDEPTH
#define	YYMAXDEPTH	150
#endif
YYSTYPE			yylval, yyval;
#define	YYERRCODE	256

#line 9475 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"


/* %L Routines */
/* %L Routines begin */
/* %L Routines end */

static
clrfld (expr)
OSNODE	*expr;
{
	IGgenStmt(IL_CLRFLD, (IGSID *)NULL, 1, osvalref(expr));
}

static
resumecolumn (table, column)
char	*table;
OSNODE	*column;
{
	if (column->n_token == tkID || column->n_token == tkSCONST)
	{
		char *p = (column->n_token == tkID ? column->n_value : column->n_const);

		oscolcheck(FormSym, table, p);
		IGgenStmt(IL_RESCOL, (IGSID *)NULL,
			2, IGsetConst(DB_CHA_TYPE, table),
				IGsetConst(DB_CHA_TYPE, p)
		);
		ostrfree(column);
	}
	else
	{
		IGgenStmt(IL_RESCOL, (IGSID *)NULL,
			2, IGsetConst(DB_CHA_TYPE, table), osvalref(column)
		);
	}
}

osevallist (list, func)
OSNODE		*list;
register i4	(*func)();
{
	register OSNODE	*lp;

	for (lp = list ; lp != NULL ; lp = lp->n_next)
	{
		if (lp->n_ele != NULL)
		{
			(*func)(lp->n_ele, (lp->n_next != NULL));
			lp->n_ele = NULL;	/* freed by "(*func)()" */
		}
	}

	if (list != NULL)
		ostrfree(list);
}

ostl1elm (elp)
register OSNODE *elp;
{
	IGgenStmt(IL_TL1ELM, (IGSID *)NULL, 1, osvalref(elp));
	ostrfree(elp);
}

ostl2elm (elp)
register OSNODE *elp;
{
	if (elp->n_token == tkASSIGN)
		IGgenStmt(IL_TL2ELM, (IGSID *)NULL,
			2, elp->n_lhs->s_ilref, osvalref(elp->n_rhs)
			);
	else if (elp->n_token == TLASSIGN)
		IGgenStmt(IL_TL2ELM, (IGSID *)NULL, 2, elp->n_coln, elp->n_tlexpr);
	ostrfree(elp);
}

operr ()
{
	if (!OpErr)
	{
		oscerr(QloopState ? OSEERROP : OSEINVOP, 0);
		if (QloopState)
			OpErr = TRUE;
	}
}

static
scroll ( form, table, row )
register OSNODE	*form;
register OSNODE	*table;
OSNODE		*row;
{
	if ( table->n_token == tkID && ( form == NULL ||
			(form->n_token == tkID &&
			STbcompare( FormSym->s_name, 0, form->n_value, 0,
						TRUE) == 0 ) ) )
		_VOID_ ostblcheck( FormSym, table->n_value );

	IGgenStmt( IL_SCROLL, (IGSID *)NULL,
			3, form == NULL ? 0 : osvalref( form ),
				osvalref( table ), osvalref( row )
	);
}

/* History:
**	02/90 (jhw) -- Moved out of 'IGoutput()' in "ilg!igoutput.c" where
**		bobm had put it; now ILG does not have any dependencies on OSL.
*/
static
os_output ()
{
	i4		iflags;
	char		lbuf[MAX_LOC+1];
	LOCATION	loc;
	LOINFORMATION	inf;

	/*
	** Get date on source file for 'IIAMwcWrtCommit()'.  If we can't, simply
	** pass in epoch date so that IL will just get stamped with "now" (file
	** assumed very old.)
	*/
	iflags = LO_I_LAST;
	if ( osIfile == stdin || STlcopy(osIfilname, lbuf, sizeof(lbuf)) == 0
		|| LOfroms(PATH&FILENAME, lbuf, &loc) != OK
			|| LOinfo(&loc, &iflags, &inf) != OK
				|| !(iflags & LO_I_LAST) )
	{
		inf.li_last.TM_secs = 0;
	}

	IGoutput(osFid, (osDebugIL ? osOfile : NULL), &inf.li_last, &osIGframe);
}

/*
** Name:        is_state -       Test for symbol _STATE
**
** Description:
**      This returns TRUE if the passed in string is _STATE
**      (case insensitive). Otherwise it will return FALSE.
**
*/

is_state(char *name)
{
   char buf[50];

   STcopy(name,buf);
   CVlower(buf);

   if  (STcompare(buf,"_state") == 0)
       return TRUE;
   else
       return FALSE;
}

short yyexca[] = {
	-1, 0,
	0, 2,
	-2, 11,
	-1, 1,
	0, -1,
	-2, 0,
	-1, 89,
	262, 201,
	263, 201,
	266, 174,
	274, 201,
	291, 174,
	297, 174,
	397, 174,
	-2, 128,
	-1, 90,
	262, 201,
	263, 201,
	266, 174,
	274, 201,
	291, 174,
	297, 174,
	397, 174,
	-2, 128,
	-1, 122,
	61, 68,
	44, 68,
	-2, 116,
	-1, 218,
	399, 626,
	400, 626,
	402, 626,
	403, 626,
	45, 626,
	-2, 622,
	-1, 296,
	333, 465,
	40, 465,
	-2, 525,
	-1, 363,
	262, 201,
	263, 201,
	266, 174,
	274, 201,
	291, 174,
	297, 174,
	397, 174,
	-2, 128,
	-1, 415,
	40, 911,
	-2, 869,
	-1, 419,
	40, 878,
	-2, 872,
	-1, 420,
	40, 879,
	-2, 817,
	-1, 436,
	46, 904,
	-2, 878,
	-1, 611,
	46, 909,
	-2, 878,
	-1, 687,
	332, 880,
	399, 880,
	400, 880,
	403, 880,
	-2, 899,
	-1, 753,
	331, 331,
	-2, 336,
	-1, 779,
	61, 90,
	44, 90,
	40, 90,
	-2, 120,
	-1, 836,
	40, 878,
	-2, 872,
	-1, 837,
	40, 879,
	-2, 817,
	-1, 838,
	40, 911,
	-2, 869,
	-1, 883,
	46, 804,
	-2, 878,
	-1, 892,
	332, 881,
	399, 881,
	400, 881,
	403, 881,
	-2, 900,
	-1, 1018,
	325, 593,
	407, 593,
	408, 593,
	410, 593,
	413, 593,
	60, 593,
	62, 593,
	61, 593,
	43, 593,
	45, 593,
	42, 593,
	47, 593,
	40, 600,
	-2, 872,
	-1, 1024,
	325, 613,
	407, 613,
	408, 613,
	410, 613,
	413, 613,
	60, 613,
	62, 613,
	61, 613,
	43, 613,
	45, 613,
	42, 613,
	47, 613,
	-2, 875,
	-1, 1158,
	40, 600,
	-2, 593,
	-1, 1271,
	40, 600,
	-2, 593,
	-1, 1399,
	40, 600,
	-2, 593,
};

#define	YYNPROD		917
#define	YYLAST		2369

short yyact[] = {
	   411,  1437,  1397,  1419,   416,  1389,   345,   474,  1438,   511,
	  1309,   449,  1176,  1288,   850,  1264,   955,  1289,  1057,  1373,
	  1082,   150,  1312,   996,  1125,  1181,   643,    90,   829,   730,
	   440,   830,   444,  1054,   451,   621,   787,  1041,    88,   384,
	  1128,   526,   990,   796,   696,  1127,   517,   917,   735,   723,
	   916,  1007,   795,   948,   881,  1180,  1053,   894,   439,   699,
	   816,  1042,    43,  1011,  1090,   842,   617,    86,    63,   585,
	   410,   748,   827,   728,   414,   747,   736,   722,   828,   717,
	   700,   413,   737,   710,   620,   670,   394,   880,   826,   662,
	   455,    62,   644,   893,    11,   950,   912,   810,    10,    14,
	   139,   708,   600,   147,   347,    44,   746,   725,   763,   483,
	   721,   217,   952,   371,   789,  1431,   123,  1351,  1350,  1345,
	  1172,  1257,   807,  1170,   421,   788,   395,   393,   117,    73,
	   977,   790,  1424,  1163,   824,   623,   625,    77,   626,   791,
	   624,   102,    84,   995,   956,   105,   970,   618,  1382,   622,
	  1386,   951,   957,   798,   800,   799,  1381,   623,  1230,  1231,
	   338,   405,   624,   369,  1251,   496,  1226,   104,   623,   625,
	   106,   626,   357,   624,   358,  1339,   405,   623,   623,   625,
	   522,   626,   624,   624,    29,  1333,  1444,    30,   376,   623,
	   625,   338,   626,  1317,   624,   818,   798,   800,   799,   623,
	   625,  1349,   626,  1108,   624,   417,   559,   558,   680,   815,
	   560,   871,   401,    57,   209,   209,   372,   348,  1446,   377,
	  1253,   434,   441,  1432,   446,  1246,   647,  1408,    55,  1428,
	   434,  1310,   463,   434,   465,   466,   438,   470,   407,   480,
	   405,    29,  1388,   408,    30,   378,   379,   500,  1091,   504,
	   471,   433,   819,   653,  1276,  1277,   504,  1093,  1092,  1265,
	   460,  1383,   790,   464,   790,   958,   528,  1377,  1113,  1358,
	   791,   533,   791,   750,  1256,   623,   625,  1252,   626,  1227,
	   624,   437,   565,  1401,   648,   652,   403,   649,   650,   651,
	   743,   567,   566,   798,   800,   799,   741,   405,    29,   623,
	   625,    30,   626,   447,   624,   448,   545,  1084,   529,  1275,
	   534,   706,   405,    29,   423,    68,    30,   798,   800,   799,
	   405,   424,   352,   351,   349,   350,   353,  1256,   623,   625,
	  1297,   626,   890,   624,   693,   694,   530,  1327,   209,   562,
	  1129,   402,   405,   436,   405,    29,    30,    20,    30,  1363,
	    39,  1322,    20,   744,   572,   568,   689,   564,   571,    52,
	    54,  1267,   824,   623,   625,  1140,   626,   536,   624,   405,
	    29,    29,   601,    30,    30,   478,  1320,   408,   479,   745,
	    29,  1392,  1391,    30,    76,   405,    29,   476,  1311,    30,
	    20,   580,  1112,   405,    29,  1272,   742,    30,   609,   405,
	    29,   548,   957,    30,   123,   405,    29,   505,   772,    30,
	   614,   405,    29,  1267,    29,    30,   588,    30,   804,   405,
	  1328,  1229,   595,  1329,   596,   597,  1267,   115,  1190,   599,
	   405,    29,   633,  1114,    30,  1006,   476,   605,   405,   702,
	   405,   424,   632,   640,  1064,   983,   642,   630,   582,   476,
	   818,   646,   537,   405,    29,   348,   346,    30,   342,   344,
	   667,   804,   672,   654,   549,   113,   405,    29,   441,   370,
	    30,   939,   586,  1296,   584,   424,   476,   960,   684,   690,
	   691,   476,   417,   114,  1169,   116,   790,  1169,   209,  1173,
	   681,   619,  1169,   494,   791,   446,   395,  1121,   844,   415,
	   622,   802,   805,   801,   409,   806,   803,   819,   604,   820,
	   713,   790,   395,   454,   637,   958,   395,   732,   604,   791,
	   771,   655,  1163,   392,   473,    29,   677,   707,    30,   754,
	   698,   355,   756,  1163,   758,   373,   110,   631,   961,  1123,
	   679,   415,   622,   622,   802,   805,   801,   521,   806,   803,
	   612,   415,   354,   415,   622,    61,   719,   760,   804,   424,
	   704,   705,   123,   476,  1163,    72,    71,   405,    29,   755,
	   412,    30,   380,   352,   351,   349,   350,   353,   412,   472,
	  1254,   407,   804,   711,   619,  1247,   408,   777,   734,   405,
	    29,  1120,   695,    30,   770,    53,   493,   405,   419,   352,
	   351,   420,   350,   353,   405,   424,   352,   351,   349,   350,
	   353,   405,   611,  1122,   606,    30,   417,   817,   727,   724,
	   563,    56,  1423,   712,   123,   123,   778,   123,    29,   405,
	   452,    30,   751,   453,   757,   843,   423,   847,   434,   759,
	  1163,   802,   852,   801,   400,  1337,   803,    51,   547,   124,
	   857,   856,   580,   109,   769,   767,   415,   861,   862,   764,
	   209,  1067,   113,   547,  1163,   802,  1422,   801,   848,   870,
	   803,  1319,  1318,    72,    71,   878,   879,   884,   415,   780,
	   114,    44,   415,   434,  1029,   889,   678,   415,   781,   782,
	  1364,   886,   490,  1163,   415,   415,  1338,   895,   846,   895,
	   456,   784,   851,  1052,   793,   794,    52,    54,    54,    54,
	   902,   813,   412,   888,   891,    20,   473,   491,   773,   774,
	   775,   405,   883,   489,   686,    30,  1415,   902,   622,  1335,
	   486,   405,   419,   352,   351,   420,   350,   353,  1287,  1068,
	   336,   405,    29,   602,   475,    30,  1069,   405,  1153,   405,
	    29,    30,   929,    30,   835,   892,  1451,   901,   682,   405,
	  1148,    19,   884,    30,   688,   405,   733,   462,   683,    30,
	   899,   472,  1286,   703,   991,   855,   946,   946,  1449,   921,
	  1336,   468,   685,   123,   487,   488,   962,   415,   469,  1282,
	   405,    29,   896,   475,    30,   940,  1395,   937,   123,  1436,
	   851,   832,   973,   405,    29,   675,   475,    30,   565,   668,
	   954,   352,   351,   349,   350,   353,   949,   567,   566,   687,
	   432,   965,   966,   492,   918,   417,  1314,  1313,   941,   942,
	   405,    29,   495,   475,    30,   405,   430,  1188,   475,   415,
	   884,   963,    96,   715,   674,   907,   542,   658,   964,   944,
	   656,   129,   129,   865,   864,   639,   731,  1194,   866,  1032,
	   646,   997,   998,   994,  1033,   740,   749,   749,   539,   884,
	   874,   992,   974,   672,  1038,   967,    44,    44,  1043,  1030,
	   968,   568,   978,   564,  1406,   415,   884,  1292,   415,   415,
	   969,  1055,  1055,   540,   857,  1049,    20,  1144,  1143,   541,
	   981,   607,   443,   613,  1039,  1356,  1374,   415,   415,   415,
	   546,  1066,  1348,  1072,   589,  1061,   838,   405,   584,   762,
	   475,   412,   450,  1086,  1245,  1026,  1008,   999,    20,   858,
	  1085,    20,    20,  1016,   729,  1051,   835,   457,   582,   458,
	   405,   419,   352,   351,   420,   350,   353,   459,  1056,   914,
	   661,  1028,  1027,   486,   593,   719,  1035,  1034,  1037,   835,
	  1036,   575,   576,   577,   584,   473,   664,  1047,   544,    42,
	   711,   665,     8,  1048,  1332,  1110,  1111,   456,  1062,  1100,
	   666,   663,  1076,   570,  1109,  1073,   817,  1075,   592,  1074,
	   578,   375,   538,   664,    38,   531,  1089,   516,   665,   121,
	  1098,  1087,   843,  1147,   426,   847,  1152,   666,   663,   427,
	  1095,  1102,   574,   573,   561,  1149,   120,    94,   428,  1136,
	   472,    65,  1107,   557,  1138,    91,   473,    47,  1116,  1117,
	  1426,   441,   120,   857,  1174,  1118,  1115,   399,   398,    50,
	    69,   121,   213,   503,  1154,   884,  1131,  1162,    67,   397,
	   473,   473,  1372,  1055,  1175,  1165,  1343,   857,  1200,   895,
	  1196,  1157,  1160,   415,  1164,  1137,  1150,  1145,  1139,  1135,
	  1207,  1344,   902,  1156,   415,  1155,  1368,  1166,   499,    92,
	   902,   472,   415,  1183,   975,   509,   510,  1216,  1204,  1182,
	  1233,  1016,  1016,    98,  1005,  1016,  1161,  1106,   838,  1195,
	  1080,  1046,   532,  1002,   121,   472,   472,   902,    97,  1202,
	  1197,  1201,   988,  1016,    96,   835,  1205,   835,  1219,  1208,
	  1206,   415,    65,    20,    20,   129,  1210,   512,  1016,   987,
	  1211,   986,   935,  1218,   875,   925,   337,   340,   975,   920,
	   910,   906,   339,   903,  1009,   898,  1222,   860,   783,  1013,
	   768,  1235,  1243,    49,  1221,   949,  1242,    64,   587,  1220,
	   551,   594,   362,   119,   361,    87,   103,  1234,  1232,  1236,
	   120,    99,    78,   858,  1237,   388,   389,   383,  1239,   119,
	  1240,    66,    45,   356,   703,   473,   446,  1450,    83,  1290,
	   415,   535,  1162,  1043,  1154,    98,   122,  1154,   407,  1427,
	  1165,  1284,  1162,   408,  1055,  1402,  1255,   415,  1393,  1164,
	    97,  1266,   975,   785,  1298,  1273,   107,  1300,  1283,    15,
	  1154,  1321,   415,   476,   415,  1258,  1259,  1260,  1261,  1294,
	  1293,  1262,   550,  1162,  1238,  1270,  1016,  1299,   122,  1305,
	   472,  1161,  1228,  1016,  1016,  1016,  1016,  1198,   740,  1016,
	   123,  1161,   749,  1016,   476,  1331,   422,  1325,    64,  1323,
	    95,  1326,  1315,    98,  1324,  1362,   740,   407,    80,  1136,
	   922,  1138,   408,   415,   415,   415,   749,   838,    97,   838,
	   975,  1354,  1161,  1357,   543,  1009,   404,   646,   646,   849,
	  1013,  1430,  1154,   434,  1162,  1162,  1162,  1162,  1162,   513,
	   812,   122,    85,  1365,   635,  1352,  1162,   101,   100,   859,
	  1203,   897,   858,  1360,  1361,   473,   473,   119,  1224,  1403,
	  1213,  1367,  1404,  1366,  1384,  1209,   407,  1385,  1369,  1370,
	  1442,   408,  1371,  1376,  1375,  1044,   858,  1025,   703,  1359,
	   406,  1378,   641,  1161,  1161,  1161,  1161,  1161,  1325,  1341,
	  1001,   473,  1342,   997,   998,  1161,   407,  1407,  1387,    48,
	   835,   408,  1394,  1290,  1410,   835,   641,   415,   872,  1017,
	   472,   472,  1306,   867,  1290,   924,  1290,   554,  1412,  1411,
	  1409,  1440,  1413,  1439,   347,  1414,  1418,  1416,   417,  1417,
	  1304,  1425,   527,  1279,   527,  1278,  1023,  1199,  1279,  1191,
	  1044,  1186,  1192,  1421,  1044,  1434,   472,  1380,  1141,  1059,
	  1429,  1044,  1398,  1400,   857,  1433,   407,   982,   832,   740,
	  1104,   408,  1346,  1105,  1078,  1441,  1316,  1079,    34,   415,
	  1016,  1016,  1443,  1445,  1162,  1070,  1162,  1448,  1071,  1241,
	  1059,  1045,   417,  1060,  1044,  1003,   984,   582,  1004,   985,
	  1447,  1452,  1454,  1217,  1130,  1103,   415,   857,   857,  1453,
	   415,   936,   407,  1379,   934,   933,  1178,   408,   934,   407,
	    75,  1302,  1301,   584,   408,   923,   911,   908,   924,   909,
	   909,   454,  1094,  1161,   904,  1161,   359,   905,  1000,   360,
	  1179,   473,   989,   980,  1398,  1340,  1334,   579,  1280,   697,
	  1215,  1185,  1171,  1159,  1158,   352,   351,   349,   350,   353,
	  1017,   417,  1016,  1168,  1077,  1010,   582,  1303,   993,   603,
	   926,   885,   415,   527,   877,   868,  1330,   415,   839,   417,
	   811,   497,   497,   809,   766,   627,   628,  1023,   412,   497,
	   497,   761,   584,   709,   726,   676,   472,   636,   634,  1177,
	   629,   731,   616,   525,   524,   523,   497,   405,   419,   352,
	   351,   420,   350,   353,   514,   508,   473,  1167,   507,   506,
	   417,   501,  1013,    50,    36,    58,   832,   405,    29,    35,
	   475,    30,   720,  1015,   226,  1058,   485,   484,   482,   481,
	   239,  1050,   887,   238,   236,   233,   229,   220,   583,   237,
	   417,    59,    60,   429,   218,   863,  1031,   412,   405,    29,
	  1167,   475,    30,  1091,   497,  1013,   657,   228,    74,   873,
	   461,   472,  1093,  1092,   230,   227,   405,   419,   352,   351,
	   420,   350,   353,   223,   221,   467,   832,   235,   442,   431,
	   175,   174,   173,   172,  1024,  1018,   352,   351,   349,   350,
	   353,   171,   170,   169,   927,   415,  1010,   168,   476,   473,
	   417,   167,   166,   165,   164,   163,   412,   162,   161,   160,
	   159,   158,   157,   156,  1167,   155,   154,   153,   152,  1013,
	   603,   276,  1308,  1214,  1083,   405,   419,   352,   351,   420,
	   350,   353,  1307,   858,  1101,  1081,   412,   919,   263,   251,
	   260,   262,   250,   321,   261,   306,   417,   249,   945,   327,
	   305,   269,   913,   417,   472,   405,   836,   352,   351,   837,
	   350,   353,  1420,   252,   253,   270,   603,   245,   244,   792,
	   603,   498,   268,   266,   265,   259,   858,   858,   515,   258,
	   256,   581,   254,   714,   274,   247,   246,   243,   821,   822,
	   823,   120,   205,   204,   272,   203,   412,   831,   146,   126,
	   127,   128,   335,  1420,   125,   312,   151,   202,    28,   201,
	   200,   199,   216,   275,   198,   405,   836,   352,   351,   837,
	   350,   353,   277,   278,   240,   279,   280,   285,   281,   282,
	   241,   284,  1017,   286,   197,   287,   294,   295,   296,   297,
	   298,   299,   412,   196,   195,   194,   193,   192,   191,   412,
	    27,   417,   190,   189,   188,   187,   186,   185,   184,   183,
	   182,   405,   419,   352,   351,   420,   350,   353,   405,   419,
	   352,   351,   420,   350,   353,  1017,   181,   224,   225,   283,
	   180,   288,   289,   292,   290,   293,   291,   300,   179,   303,
	   304,   311,   178,   313,   417,   257,   314,   315,   316,   317,
	   318,   319,   591,   325,   326,   328,   333,   307,   334,   322,
	   308,   323,   320,   324,   301,   302,   332,   309,   329,   310,
	   590,   330,    13,   273,   177,   176,   425,   374,   979,   396,
	   211,   212,   145,   144,   143,   932,   142,   141,   779,  1017,
	   140,    31,    24,    26,    25,   208,   786,    23,   391,   390,
	   207,    28,   386,   598,   387,   603,    33,   385,   417,   382,
	   381,   552,   331,   971,   271,    13,  1159,  1399,   352,   351,
	   349,   350,   353,   206,   138,   137,   136,   135,   134,   831,
	   133,    32,   132,   131,    31,    24,    26,    25,   130,   368,
	    23,   367,   366,    27,    28,   365,   364,   555,   953,    33,
	    81,   553,   831,    89,   363,   118,   111,   108,    82,  1159,
	  1158,   352,   351,   349,   350,   353,    32,   930,    79,    31,
	    24,    26,    25,   931,    17,    23,    46,    12,  1225,    28,
	    93,    41,     6,    37,    33,     7,    27,     5,    40,     4,
	     3,     9,     2,     1,   445,   234,   222,   219,   943,   669,
	   845,  1021,   405,    29,  1020,   475,    30,  1019,  1435,  1405,
	  1355,  1281,  1184,   660,   659,    22,    29,   671,   882,    30,
	  1396,    27,  1347,  1159,  1271,   352,   351,   349,   350,   353,
	  1269,  1268,  1014,  1248,  1295,  1022,   976,   341,   556,   776,
	   569,    18,   112,   267,   718,   215,  1187,  1040,   876,  1285,
	   841,   840,   701,  1097,  1065,   527,  1063,   972,    22,    29,
	  1291,   673,    30,  1193,   869,   435,   645,  1151,   853,  1244,
	  1146,  1263,  1012,   264,   928,   716,  1088,  1096,   518,   477,
	  1099,   752,   753,   255,   502,   248,   900,   915,   947,   765,
	    70,    16,   814,    22,    29,   615,   825,    30,  1124,   808,
	  1126,   214,   149,   148,  1132,  1133,  1134,   834,   831,   519,
	   831,    -1,   854,    -1,  1353,   520,   343,  1119,   608,   210,
	   418,   638,   833,   610,   692,  1250,  1249,   959,  1274,   232,
	   231,  1189,  1142,   938,   242,  1223,   739,   738,   797,    21,
	  1390,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  1212,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,   527,     0,     0,
	     0,   527,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   831,     0,     0,     0,     0,   831
};

short yypact[] = {
	   686, -1000, -1000, -1000, -1000,  1669,  1534,   714,  -216,   683,
	  1669, -1000,  1121,   904,  1315, -1000,  1533,  -216,   -29,  -189,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   155, -1000, -1000,
	 -1000, -1000, -1000, -1000,   898,  1120,   274,   898,  1534, -1000,
	 -1000,  1534,  -216, -1000, -1000,   898,  1111, -1000,  1704, -1000,
	 -1000,  1533, -1000,  1256, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,  1106,   769, -1000, -1000,   753,  1219, -1000, -1000,
	  1110,  1262,  1261,  1106,  1105,   898, -1000,  1106,   898,  1533,
	 -1000,   904,   280, -1000, -1000,   228, -1000, -1000,  1045,  1483,
	  1483,   999, -1000,   166, -1000, -1000,   166, -1000, -1000,   172,
	   152,   131, -1000,   753,  1106, -1000,  1106, -1000,  1445, -1000,
	 -1000,  1103,  1101, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,   999, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,    52,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   898,   711,   769,
	   898, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,   898,   898, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,   907, -1000,   123,    81,
	   775,   350,  1429,    75, -1000, -1000,   704, -1000,   505,   489,
	   -57,   -57,   579,    -6,   624, -1000,   230,   367,   628,   -57,
	   441,  -159,   -57,  -159,  -159,   457,  -159,   342,    31,   432,
	 -1000, -1000,   426,   217,  -238,  -238,  -159,  1531,    67,  1529,
	  1528,  1525,  -238,  -238,  1429,    67,  1524, -1000,   721, -1000,
	  -223,  1515,  1514,  1513,  1429,  -159,  1429,   898,   719,  -238,
	  -159,   769, -1000,  1133, -1000, -1000,   367, -1000, -1000, -1000,
	 -1000, -1000, -1000,    95, -1000, -1000, -1000, -1000,   560, -1000,
	 -1000,   560, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   675,
	 -1000, -1000, -1000, -1000, -1000,  -159,   125, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,   125, -1000, -1000, -1000, -1000, -1000,  -238, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1333, -1000,  1151,
	 -1000,   765, -1000, -1000, -1000, -1000, -1000, -1000,  -195, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	    83,   481,   -45,  1483,  -216,   -29,   -29,   -29,  -189,  1227,
	 -1000, -1000, -1000,  1669,  1097, -1000,  1045, -1000,   711,   708,
	 -1000,   907,   891, -1000, -1000,   769,   769, -1000, -1000, -1000,
	   769,   332, -1000,   118, -1000,   198,   190,   212,   190, -1000,
	  1429,  1512, -1000, -1000,   101, -1000,   147,  1429,  1429,  1510,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,    81, -1000,    52, -1000,   603, -1000, -1000, -1000,   342,
	 -1000,  -159, -1000,  1508, -1000,  1258, -1000, -1000, -1000,  1507,
	 -1000,   456,  -159, -1000,  1322,  -159, -1000, -1000, -1000, -1000,
	   -55,   624, -1000, -1000, -1000,   523, -1000,   520,   693,  -159,
	   478,  -159, -1000,   517, -1000,   474,  1505,   -57, -1000, -1000,
	   352, -1000, -1000, -1000, -1000, -1000,  -194,   431,     6,   -14,
	 -1000, -1000,   655, -1000,  1459,  1459,    39, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,    54,  1503,  -289,   101,    75, -1000,
	  1429,  -216, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1504,
	  1504,   636,   147, -1000,  -216, -1000,   366, -1000, -1000, -1000,
	 -1000, -1000, -1000,    14,   -20,   -20, -1000,   147,  -159, -1000,
	   769,  -159,  1503,  -159,   982,   159,  1501, -1000, -1000, -1000,
	   611, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,  1494,   332,  1089,  1704,   769,   120, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,  1256,  1256,  1256, -1000,   147,
	 -1000,   218, -1000, -1000, -1000, -1000,  1626,   898, -1000,  1669,
	  1669,  1087, -1000, -1000, -1000, -1000,   982,   982,   332,   940,
	  -142,   332,   332,   136, -1000,  -294, -1000,  1493,  1490, -1000,
	  1254, -1000, -1000,  1490, -1000, -1000,   168, -1000,   109, -1000,
	  1429,  1429,  1429, -1000, -1000, -1000, -1000,   321, -1000,  1316,
	   649, -1000, -1000,  1488,  -159,    98,  -159,   -57, -1000,  1243,
	   370,  -159,   518,  1265, -1000,  1086, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,  -159,  -159, -1000,   527,
	  1329, -1000, -1000, -1000, -1000, -1000, -1000,  1485,  -102,  1324,
	 -1000,   809, -1000,  1484,  -159,  -159,   322,  1481,   342, -1000,
	 -1000, -1000,   -57, -1000,     0, -1000,   382, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,  -159, -1000,  -159,  1267,
	 -1000,  1084, -1000, -1000, -1000, -1000, -1000,  1322, -1000,  -159,
	 -1000, -1000,    81, -1000,  1082, -1000,  1443, -1000,  1080, -1000,
	    75,  1436, -1000,  1079,  1435,   651,  -159,   651, -1000, -1000,
	  1078, -1000, -1000,  1224, -1000,  1434, -1000,  1074,  1480,  1613,
	 -1000, -1000, -1000, -1000, -1000, -1000,  1424, -1000,  1071, -1000,
	 -1000,  1420,   466,    71, -1000,  1045, -1000, -1000, -1000,   898,
	   898,   322, -1000,   342,   342, -1000, -1000,  -144, -1000,  1315,
	  1045,   104,   138, -1000, -1000, -1000,  1429, -1000, -1000, -1000,
	  1106,  1626,  1626,   898,  -144, -1000, -1000,   332, -1000, -1000,
	 -1000, -1000,    93,   105, -1000,  1429, -1000,   205, -1000, -1000,
	 -1000, -1000, -1000, -1000,  -281, -1000, -1000, -1000,   636,  1452,
	 -1000,  1376,    45, -1000,  1405, -1000, -1000,  1070,  1068,  1051,
	 -1000,  -258,   135, -1000, -1000,  1451, -1000,  1151,  1151, -1000,
	 -1000,   147,  1478, -1000,   541,    97,    52,    52,    52,   322,
	  1447,  1306, -1000,  1042, -1000,  1404, -1000,  1033, -1000,    35,
	 -1000,  1245, -1000, -1000,  1293, -1000, -1000, -1000, -1000,   -55,
	   644,   554,   534,  -159, -1000, -1000, -1000,   666,   322,   367,
	 -1000, -1000,  -159,  -159, -1000, -1000,   582,  -159, -1000, -1000,
	  1400, -1000,  1040,  -289, -1000,   322,   436,   637,   371, -1000,
	  -159,  -159, -1000,  1399, -1000, -1000,  1399,    39,   404, -1000,
	  1394, -1000, -1000,  1429, -1000, -1000,    14,    81, -1000, -1000,
	   -20, -1000, -1000,  1474, -1000,  1383, -1000,  1039, -1000,   -93,
	  1429, -1000,  -159, -1000,    14,   -79, -1000, -1000,  1441, -1000,
	 -1000, -1000, -1000, -1000,   -20,  1286, -1000,  1422, -1000, -1000,
	 -1000, -1000, -1000,  1414,  1291, -1000, -1000,  1379, -1000,  1036,
	 -1000, -1000, -1000,   898, -1000, -1000,  -199,   700,   691,  -146,
	    -8,    33,   636, -1000, -1000, -1000, -1000,  1106, -1000, -1000,
	 -1000,   147,   250, -1000, -1000, -1000,   213, -1000, -1000,   -60,
	 -1000,  1413, -1000, -1000, -1000,   -87,  1429,  1429,  1429, -1000,
	  1316, -1000,  1316,    41, -1000,   108, -1000, -1000, -1000,  1367,
	   574,  -159,   360,   624,  -159,   348, -1000,  -275, -1000,  1104,
	  1104,   257, -1000,  1570, -1000,  1473, -1000, -1000,    77, -1000,
	 -1000, -1000,  1462, -1000,    74,   436, -1000, -1000,  1209,  1209,
	   -57,  1104, -1000,  1461, -1000,  1360,   510, -1000, -1000,    28,
	  1358, -1000,   532,  1033,   322,   370,  1570,  1201,  1356,   436,
	 -1000,    39,  -159,  1266, -1000,  1027,  1266, -1000,  -159, -1000,
	 -1000, -1000, -1000, -1000,  1209, -1000, -1000, -1000, -1000,  -159,
	 -1000,  -159,  1281, -1000, -1000, -1000, -1000,    14, -1000,  -159,
	  1429,  1276, -1000,  1460,  1026,  1412, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   147, -1000, -1000,
	 -1000, -1000, -1000,   370, -1000, -1000,  -159,   769,  1274, -1000,
	  -132, -1000,  1196,    21, -1000, -1000, -1000, -1000, -1000, -1000,
	  -245, -1000, -1000,  1459,  1049, -1000, -1000,    52,   -60,  1188,
	 -1000, -1000,   147,   147,   147,  1151, -1000,  1151, -1000,  1398,
	 -1000,   370,  1429, -1000, -1000, -1000,   626, -1000,   185, -1000,
	 -1000,  -134, -1000,   180,  1104, -1000,   233,    80,    77,    74,
	 -1000,  1570,  1570,  1570,  1570, -1000, -1000,  1570,  -141,  1429,
	    13,  1634,    -5,  1429, -1000, -1000,   -91, -1000, -1000, -1000,
	  1354, -1000,  1458,  -275,   458,  -159,   370,   440,  -159,   564,
	 -1000, -1000,  -159, -1000,   230, -1000, -1000,   126,    73,   370,
	 -1000,  1267,  1266,  -159,  1178, -1000,  1349, -1000, -1000,  -216,
	  1331, -1000,   147, -1000, -1000,  -169,   -12, -1000, -1000,   497,
	 -1000, -1000,  1045,  1385,  -209, -1000,   337,   336,   -24,  1175,
	 -1000, -1000,   -49, -1000,   -60,  1158, -1000,    52,   -63,    20,
	   541, -1000, -1000,   624, -1000,   690,  -217,  1456, -1000, -1000,
	 -1000,   445,   361,  -227,  1455, -1000, -1000, -1000,  -274,   115,
	 -1000,   126,   286,  1308, -1000,  1010,  -297, -1000,  1381,   592,
	   157,    72, -1000,  -299, -1000, -1000, -1000, -1000, -1000,  1209,
	  -159,   584,    12,  1298, -1000, -1000,   -55,   -55,  1221, -1000,
	   291,   624,   -57, -1000, -1000, -1000, -1000, -1000,   497, -1000,
	 -1000, -1000, -1000, -1000, -1000,  1015, -1000,   -93,  1288, -1000,
	   991, -1000, -1000,   586,   586, -1000,  -146, -1000, -1000, -1000,
	 -1000,  -133,  1368, -1000, -1000, -1000,   649, -1000,    52,    52,
	    52, -1000,  1423,  1366,  -246, -1000, -1000,  -150, -1000,  1283,
	  -252, -1000,  -141,  -158,   -18,  1162, -1000,   468,  1527,  1570,
	    26,  1159, -1000,  1278, -1000,   561,   -30, -1000, -1000, -1000,
	  1265,  1265,  -159,  -159, -1000, -1000, -1000,   624,  1429, -1000,
	 -1000,  -169,   391,  -159, -1000,  -159, -1000, -1000, -1000,   410,
	  -216,  1362,   331,   287, -1000,  -270,  1350, -1000,   969, -1000,
	  1153, -1000, -1000,  -171, -1000,   370,  1247, -1000,   126,    69,
	   126, -1000,  -177,   624,  -159,   472,   436, -1000, -1000, -1000,
	 -1000, -1000,  1342, -1000, -1000, -1000,  1221,  1221,  1340, -1000,
	 -1000,  -216, -1000, -1000,  1289, -1000,   -18,  -214, -1000, -1000,
	  1527,   -39, -1000, -1000, -1000,   370,  -159,   447,  1129, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   425,   436,
	   436,  -159, -1000, -1000, -1000
};

short yypgo[] = {
	     0,  2150,   341,    71,  2149,   647,   620,   595,   142,  1136,
	    84,    35,    36,    52,  2148,    43,  2147,  2146,  2145,  2144,
	  2143,  2142,     3,    39,  2141,  2140,  2139,  2138,   992,  2137,
	    16,  2136,  2135,  2134,  2133,  1127,    70,   102,    49,  2132,
	    74,  2131,   100,   124,   286,  2130,  1286,     9,    41,    45,
	    86,    83,  2129,    30,  2128,     4,  2127,    81,     6,  2126,
	     7,  2125,  2124,  2123,     0,     8,    34,    32,  2122,  2121,
	    12,  2119,  2117,   103,  2113,  2112,  1042,  2111,  2110,    40,
	   504,  2109,  2108,    24,    97,    88,  2106,    72,    28,    78,
	    31,  2105,  2102,    60,  2101,   621,  2100,   832,    47,  2099,
	  2098,    53,   107,  2097,    50,    96,   101,  2096,  2095,  2094,
	  2093,  1043,  2092,  2091,  2090,    48,    76,  2089,  2088,  2087,
	  2086,    64,  2085,    79,    82,  2084,   110,    77,   106,    75,
	  2083,  2082,  2081,    15,  2080,  2079,    58,  2078,  2077,  2076,
	    55,    25,  2075,  2074,  2073,  2071,  2070,  2067,  2066,  2064,
	  2062,  2061,  2060,    65,    56,    33,  2059,    11,    26,    92,
	  2058,  2057,    37,    59,    80,    13,  2056,    21,   111,  2055,
	  2054,     5,  2053,  2052,  2051,  2050,   761,  2049,    90,  2048,
	  2047,  2046,    89,  2045,  2044,  2043,    63,  2042,    51,  2041,
	  2040,  2032,  2030,     2,  2028,    14,  2027,  2024,  2023,  2022,
	  2021,  2020,  2019,     1,  2018,  2017,  2014,  2011,  2010,    61,
	  2009,    85,  2008,    87,    54,    17,    22,  2007,  2006,  2005,
	  2004,  2003,  2002,  2001,  2000,  1999,  1998,  1997,    98,    62,
	  1995,  1993,    91,    67,  1579,  1079,  1992,  1428,  1991,   774,
	  1048,  1040,  1990,  1988,    94,  1987,  1986,    99,  1219,  1984,
	  1978,  1968,  1967,   653,  1966,  1575,    68,    38,  1965,  1964,
	   649,  1963,    27,  1961,  1958,  1957,  1956,  1955,  1952,  1951,
	  1949,  1948,  1943,  1942,  1940,  1938,  1937,  1936,  1935,  1934,
	    23,  1933,   572,  1924,  1922,  1921,    95,  1920,  1919,  1917,
	  1914,  1913,  1912,  1910,  1909,  1908,  1906,  1905,  1900,  1897,
	  1896,  1894,  1893,  1892,  1891,  1890,  1889,    73,  1888,    42,
	   535,    69,  1887,  1886,  1885,  1884,  1880,  1862,  1852,  1848,
	  1840,  1836,  1820,  1819,  1818,  1817,  1816,  1815,  1814,  1813,
	  1812,  1808,  1807,  1806,  1805,  1804,  1803,  1794,  1774,  1771,
	  1770,  1769,  1767,  1755,  1753,  1752,  1747,  1746,  1745,  1743,
	    29,  1742,  1741,  1740,  1739,  1738,  1735,    46,  1734,  1733,
	  1732,  1731,  1728,  1727,  1725,  1724,  1723,  1712,  1711,  1710,
	  1709,   910,   108,  1708,  1707,  1705,  1704,  1703,  1702,  1701,
	  1700,  1699,  1698,  1697,  1695,    20,  1692,  1684,  1683,  1682,
	    10,  1681,    66,    19,  1678,  1677,  1676,  1675,  1673,  1672,
	  1671,  1670,  1669,  1668,  1667,  1665,  1664,  1663,  1662,  1661,
	  1657,  1653,  1652,  1651,  1643,  1642,  1641,  1640,  1639,  1638,
	  1637,  1635,  1634,  1633,  1625,  1624,  1620,  1619,  1617,  1616,
	  1606,  1605,  1604,  1603,  1599,  1597,  1596,  1595,  1594,  1593,
	  1592,  1591,  1590,  1589,  1588,   109,  1587,    44,    93,    18,
	  1586,    57,  1585,  1584,  1583,  1582
};

short yyr1[] = {
	     0,   221,   221,   222,   222,   223,   223,   224,   227,   227,
	   231,   230,   225,   236,   226,   238,   237,   239,   239,   234,
	   234,   234,   240,   240,   240,   235,   235,   242,   241,   241,
	   241,   241,   241,    96,    96,    96,    18,    18,    30,    30,
	    30,   179,   179,   180,   180,    29,    29,    29,    29,    29,
	   243,   243,   243,   228,   228,   244,   246,   244,   245,   247,
	   247,   248,   248,   248,   248,   250,   248,     9,     9,   251,
	   252,   252,   252,   252,   253,   253,   253,   254,   173,     6,
	     6,     6,     6,     6,   175,   175,    94,    94,    94,     4,
	     4,     4,   249,     5,   174,   174,   174,   255,     7,     7,
	     8,     8,     8,   176,   176,    95,    95,   232,   259,   257,
	   261,   257,   257,   233,   233,   258,   258,   258,   256,   256,
	    23,    23,   264,   262,   265,   262,   263,   263,   260,   266,
	   260,   267,   260,   268,   260,   269,   260,   270,   260,   260,
	   260,   260,   260,   260,   260,   271,   271,   271,   271,   277,
	   281,   285,   283,   284,   286,   286,   282,   282,   282,   282,
	   282,   287,   291,   289,   290,   288,   292,   278,   295,   294,
	   296,   296,   293,    19,    19,   279,   279,   297,   272,   272,
	   272,   272,   272,   272,   272,   303,   303,   304,   305,   301,
	   301,   302,    91,    91,    91,    92,    92,    93,    93,    93,
	    52,    52,    80,    80,   298,   306,   308,    81,    81,    81,
	    82,    82,    83,    83,   300,   299,    84,    84,    84,    85,
	    85,    85,    85,    87,    87,    88,    88,    88,    89,    89,
	    90,    72,    72,    72,   309,   273,   273,   273,   273,   273,
	    74,    77,    77,   310,   310,   312,    75,   169,   169,    73,
	    73,    76,    78,    78,    79,   313,   167,   167,   275,   275,
	   314,   315,   311,   229,   316,   316,   317,   276,   276,   276,
	   276,   276,   276,   276,   276,   276,   276,   276,   276,   276,
	   276,   276,   276,   276,   276,   276,   276,   276,   276,   276,
	   276,   276,   276,   276,   276,   318,   318,   318,   346,   321,
	   347,   322,   348,   349,   329,   351,   332,   352,   177,   177,
	   331,   331,   353,   350,   333,   334,   334,   354,   355,   355,
	   355,   356,   356,   356,   356,   356,   357,   340,   340,   358,
	   112,   113,   113,   114,   114,    20,    20,   341,   359,   343,
	   343,   360,    97,    97,   361,    97,    98,   319,   362,   106,
	   106,   107,   107,   320,   363,   345,   364,   328,   365,   327,
	   366,   105,   105,   367,   102,   102,   103,   103,   104,   344,
	   368,   323,   108,   369,   109,   109,   342,   172,   370,    99,
	    99,   100,   100,   101,   371,   371,   371,   372,   372,   373,
	   124,   124,   124,    17,    16,    16,    16,    16,    16,    16,
	   125,   125,   125,   125,   330,   110,   111,   111,   324,   375,
	   374,   122,   122,   123,   170,   336,   377,   376,   115,   115,
	   116,   325,   378,   126,   126,   127,   337,   379,   128,   128,
	   129,     3,     3,   119,   119,   120,   120,   121,   121,   121,
	   335,   380,   118,   326,   381,   338,   382,   339,   339,   130,
	   383,   307,   307,   384,   386,   384,   385,   387,   388,   388,
	   389,   389,   390,   390,   168,   391,   212,   213,   213,   214,
	   214,   194,   184,   184,   195,   195,   195,   216,   216,   216,
	   393,   393,   165,   165,   215,   215,   215,   274,   274,   274,
	   274,   274,   274,   274,   274,   274,   274,   274,   274,   274,
	   274,   274,   274,   274,   274,   274,   274,   274,   274,   274,
	   274,   395,   217,   217,   418,   418,   398,   218,   218,   419,
	   419,   411,   219,   219,   412,   420,   421,   421,   421,   397,
	   422,   208,   208,   209,   209,   185,   185,   399,   399,   423,
	   220,   220,   403,   424,   407,   425,   426,   426,   210,   210,
	   211,   196,   427,   427,   428,   404,   429,   429,   430,   430,
	   406,   197,   197,   198,   198,   182,   182,   182,   182,   431,
	   431,   431,   199,   199,   200,   200,   201,   201,   201,   202,
	   202,   203,   204,   204,   417,   186,   186,   186,   186,   186,
	   186,   187,   187,   187,   187,   187,   187,   187,   187,   187,
	   183,   189,   189,   189,   189,   190,   190,   191,   191,   192,
	   192,   193,   193,   205,   207,   206,   188,   188,   188,   188,
	   188,   188,   394,   394,   432,   433,   433,   414,   434,   416,
	   416,   136,   136,   136,    41,    41,   137,   137,   138,   138,
	   138,   138,   138,    31,    31,    31,    31,    32,    32,   178,
	   178,   157,   157,   158,   158,   159,   159,   159,   140,   140,
	   141,   141,   141,   141,   139,   139,   139,   139,   139,   139,
	   139,   139,    27,    27,    27,    27,   396,   435,    21,    21,
	   151,   151,   152,   152,   153,   134,   134,   134,    22,    22,
	   135,   135,   135,   405,   436,   143,   143,   166,   166,   156,
	   156,   156,   410,   437,   413,   413,   438,   415,   440,   415,
	   415,   415,   415,   415,   415,   415,   415,   415,   415,   415,
	   415,   415,   415,   439,   442,   441,   441,   443,   443,   443,
	   445,   163,   163,   164,   164,   164,   164,   150,   150,   148,
	   148,   148,   149,   117,    33,    33,   154,   154,   155,   155,
	   155,   155,   444,   444,   446,   450,   451,   448,   448,   447,
	   449,   452,   402,   453,   400,   401,   408,    25,    25,    25,
	    28,    28,    28,    28,    28,    24,   145,   145,   160,   160,
	   161,   161,   162,   162,   162,   144,   146,   146,   409,    26,
	    26,    26,    35,    35,    35,    35,    35,    35,    35,    36,
	    36,    86,    86,    51,    51,    50,    50,    47,    48,    37,
	    37,    37,    37,    37,    37,   147,   147,    55,    55,    55,
	    55,    55,    58,    58,    58,    58,    57,    57,    59,    59,
	    61,    60,    60,    10,    10,    11,    11,    12,    12,    13,
	    13,    13,    13,    13,    13,    14,    14,    56,    56,    56,
	   131,   454,   132,   132,   133,   133,   171,   171,     1,     1,
	    42,    49,    49,    39,   280,   280,   455,    38,    40,    40,
	    44,    45,    43,    43,    43,    46,    46,    46,     2,     2,
	    64,    64,    66,    66,    66,    67,    67,    62,    62,    63,
	    63,    65,    65,    68,    68,    71,    71,    69,    69,    70,
	    70,    70,    53,    53,   142,   142,   142,    54,    54,    34,
	    34,   392,   181,   181,    15,    15,    15
};

short yyr2[] = {
	     0,     2,     0,     1,     1,     2,     0,     3,     4,     1,
	     4,     0,     4,     2,     4,     2,     3,     1,     1,     3,
	     2,     0,     3,     2,     1,     2,     0,     1,     6,     7,
	    10,     3,     3,     3,     3,     1,     0,     2,     0,     2,
	     3,     0,     1,     0,     1,     2,     4,     5,     3,     2,
	     2,     2,     0,     2,     1,     4,     0,     5,     1,     1,
	     3,     2,     3,     3,     2,     0,     4,     4,     0,     0,
	     1,     3,     1,     3,     3,     3,     3,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     2,     2,     2,     0,     1,     1,
	     2,     2,     0,     2,     1,     1,     0,     3,     0,     4,
	     0,     2,     2,     1,     0,     1,     1,     1,     1,     1,
	     1,     1,     0,     8,     0,     5,     2,     0,     0,     0,
	     3,     0,     4,     0,     4,     0,     4,     0,     3,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     3,     2,
	     2,     0,     4,     1,     1,     1,     2,     2,     2,     1,
	     1,     2,     0,     4,     1,     2,     1,     4,     0,     3,
	     1,     1,     2,     2,     0,     1,     2,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     2,     1,     1,     2,
	     3,     3,     3,     2,     0,     3,     1,     3,     3,     3,
	     2,     0,     1,     1,     5,     1,     0,     4,     2,     0,
	     3,     1,     1,     3,     4,     4,     3,     2,     0,     3,
	     3,     1,     1,     3,     1,     1,     4,     1,     3,     1,
	     2,     2,     2,     2,     1,     1,     5,     3,     1,     1,
	     1,     5,     3,     4,     0,     1,     1,     4,     2,     1,
	     2,     3,     1,     2,     3,     0,     3,     1,     6,     6,
	     3,     3,     0,     0,     4,     0,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     2,     3,     3,     1,     3,
	     1,    10,     1,     1,     3,     1,     6,     1,     1,     0,
	     6,     1,     1,     1,     1,     2,     2,     1,     3,     5,
	     0,     1,     1,     1,     1,     1,     0,     5,     4,     1,
	     1,     1,     0,     1,     1,     1,     0,     2,     1,     1,
	     3,     1,     2,     1,     0,     2,     1,     3,     1,     3,
	     0,     3,     1,     2,     1,     3,     1,     4,     1,     4,
	     1,     4,     0,     1,     3,     0,     3,     1,     3,     3,
	     1,     2,     2,     1,     1,     0,     4,     3,     2,     3,
	     0,     1,     3,     3,     1,     1,     1,     0,     2,     1,
	     1,     2,     3,     2,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     2,     1,     1,     1,     4,     1,
	     3,     1,     3,     3,     1,     4,     1,     3,     1,     3,
	     3,     4,     1,     1,     3,     3,     4,     1,     1,     3,
	     3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     2,     1,     1,     4,     1,     4,     1,     1,     2,     1,
	     0,     3,     0,     1,     0,     4,     2,     3,     3,     0,
	     1,     3,     3,     3,     7,     1,     1,     1,     3,     3,
	     4,     1,     1,     1,     2,     3,     0,     3,     3,     0,
	     1,     0,     1,     3,     1,     3,     2,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     7,     1,     2,     1,     0,     4,     1,     2,     1,
	     0,     6,     1,     2,     9,     1,     1,     1,     0,     6,
	     1,     1,     3,     4,     3,     1,     1,     2,     4,     1,
	     1,     1,    10,     1,     3,     1,     1,     0,     1,     3,
	     3,     1,     1,     1,     1,     6,     1,     0,     1,     0,
	    11,     1,     1,     1,     3,     1,     1,     1,     1,     1,
	     1,     1,     3,     0,     2,     2,     2,     2,     0,     4,
	     0,     3,     4,     0,     7,     3,     3,     3,     3,     2,
	     1,     1,     1,     1,     1,     6,     3,     1,     1,     4,
	     1,     3,     4,     3,     0,     1,     3,     2,     0,     1,
	     3,     1,     3,     1,     3,     6,     3,     3,     2,     3,
	     2,     1,     1,     3,     1,     1,     0,     2,     1,     1,
	     1,     1,     3,     2,     1,     3,     1,     1,     1,     4,
	     6,     5,     2,     2,     2,     4,     4,     2,     2,     1,
	     0,     2,     0,     1,     3,     4,     5,     1,     3,     1,
	     2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     0,     1,     1,     1,     8,     1,     1,     1,
	     0,     1,     1,     3,     4,     1,     5,     6,     1,     0,
	     2,     5,     0,     7,     1,     1,     1,     2,     0,     0,
	     2,     2,     4,     1,     6,     2,     1,     2,     0,     5,
	     3,     3,     4,     4,     2,     3,     3,     3,     3,     3,
	     5,     5,     6,     1,     2,     2,     0,     1,     2,     1,
	     2,     1,     3,     3,     1,     5,     3,     1,     2,     1,
	     1,     1,     2,     1,     1,     1,     1,     3,     3,     3,
	     3,     3,     4,     4,     1,     1,     1,     1,     3,     1,
	     1,     1,     3,     2,     2,     1,     8,     2,     1,     1,
	     0,     1,     2,     1,     1,     1,     0,     2,     0,     3,
	     1,     3,     1,     2,     1,     2,     0,     2,     2,     2,
	     1,     1,     3,     3,     3,     3,     2,     4,     1,     1,
	     1,     1,     0,     1,     0,     3,     2,     1,     1,     3,
	     3,     2,     3,     2,     4,     1,     1,     1,     1,     1,
	     1,     1,     1,     2,     2,     2,     1,     1,     1,     1,
	     1,     1,     2,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     2,     2,     0,
	     4,     1,     3,     1,     5,     3,     1,     3,     1,     1,
	     1,     1,     3,     3,     1,     1,     0,     2,     1,     1,
	     1,     1,     1,     3,     2,     1,     3,     2,     1,     1,
	     1,     2,     1,     1,     2,     3,     1,     1,     3,     3,
	     0,     1,     2,     3,     1,     1,     2,     1,     0,     1,
	     2,     1,     1,     3,     1,     1,     1,     1,     3,     1,
	     1,     0,     1,     0,     3,     3,     6
};

short yychk[] = {
	 -4000,  -221,  -222,  -224,  -225,  -227,  -236,  -230,   286,  -223,
	  -228,  -244,  -245,   256,  -247,  -248,   -94,  -249,  -174,  -176,
	    -2,    -4,   399,   281,   276,   278,   277,   327,   285,   400,
	   403,   275,   272,   290,  -237,  -234,    40,  -231,   280,    -2,
	  -226,  -238,   286,  -229,  -244,    61,  -246,   123,    44,    -9,
	    40,    -5,    -2,    -7,    -2,   257,   -95,   402,  -255,  -255,
	  -255,   400,  -232,  -256,   259,   123,    61,  -240,    41,  -241,
	   -96,   400,   399,  -232,  -234,  -237,    -2,  -232,    61,  -250,
	  -248,   256,  -251,    -9,    -8,    46,  -233,    59,  -257,  -261,
	  -262,   256,  -235,  -242,   264,    41,  -239,    59,    44,    61,
	    46,    46,  -233,    61,  -232,  -233,  -232,    -9,  -252,  -253,
	   256,  -254,  -173,   382,   400,    -2,   257,   -23,  -258,   272,
	   125,    59,   256,  -262,  -260,   281,   276,   277,   278,  -176,
	  -271,  -272,  -273,  -274,  -275,  -276,  -277,  -278,  -279,   -42,
	  -298,  -299,  -300,  -301,  -302,  -303,   275,   -73,   -74,   -75,
	  -167,   283,  -394,  -395,  -396,  -397,  -398,  -399,  -400,  -401,
	  -402,  -403,  -404,  -405,  -406,  -407,  -408,  -409,  -410,  -411,
	  -412,  -413,  -414,  -415,  -416,  -417,  -314,  -315,  -318,  -319,
	  -320,  -321,  -322,  -323,  -324,  -325,  -326,  -327,  -328,  -329,
	  -330,  -331,  -332,  -333,  -334,  -335,  -336,  -337,  -338,  -339,
	  -340,  -341,  -342,  -343,  -344,  -345,  -281,  -293,  -297,   -43,
	   -52,  -305,  -304,   -76,   -77,  -169,   289,  -168,  -432,  -217,
	  -435,  -422,  -218,  -423,   354,   355,  -453,  -424,  -428,  -436,
	  -425,   -25,   -26,  -437,  -219,  -420,  -438,  -434,  -439,  -442,
	   301,   307,   -19,  -346,  -362,  -363,  -347,  -348,  -108,  -374,
	  -378,  -381,  -366,  -365,  -351,  -110,  -353,   372,  -354,  -356,
	  -380,  -376,  -379,  -382,  -130,  -358,  -359,  -172,  -360,  -368,
	  -364,  -283,   271,   400,   261,   290,  -391,   299,   300,   302,
	   303,   305,   306,   356,   308,   304,   310,   312,   358,   359,
	   361,   363,   360,   362,   313,   314,   315,   316,   317,   318,
	   364,   391,   392,   366,   367,  -369,  -375,   384,   387,   394,
	   396,   368,   282,   370,   373,   374,   375,   376,   377,   378,
	   389,  -377,   386,   388,   390,   380,   381,  -370,   382,   395,
	   398,  -284,   393,   383,   385,   279,  -260,    -9,  -256,  -240,
	  -241,  -180,   286,   -59,   287,   -58,   284,   -55,    45,   403,
	   404,   402,   401,   405,   400,   400,  -235,  -233,  -233,    41,
	    44,    61,    61,  -259,  -266,  -267,  -268,  -269,  -270,  -280,
	   417,    61,  -256,  -310,  -312,   280,  -257,  -256,  -256,  -256,
	  -282,  -287,  -288,   270,   -23,  -289,  -292,  -290,   268,   269,
	  -294,  -295,   400,    46,   -50,   415,  -306,   274,   263,   262,
	   294,   -64,    -2,   -44,   -46,   399,   -35,    40,    45,   -80,
	   -36,   -64,   380,   -57,   -40,   -44,   -55,   284,   -45,   400,
	   403,   -43,   -76,   -42,   400,  -313,   300,   305,   314,  -433,
	   331,  -418,   331,   -53,   -64,  -142,   400,   338,   293,  -136,
	   -53,   -64,  -419,   323,   -67,  -220,   -64,   309,   311,  -157,
	   298,   -66,   400,   403,   -44,  -178,   333,   309,   311,   319,
	   -53,  -426,   326,   -64,   -53,   -64,   -64,  -421,   324,   331,
	   -64,   -70,    -2,   -44,   -60,   402,    45,  -117,   344,   347,
	   -64,  -443,  -444,  -445,  -446,  -450,   298,   352,   353,   297,
	   266,   291,   397,   379,   276,   -97,   403,   -46,  -361,   -97,
	   -64,    40,  -109,  -111,   -64,   340,    40,    40,    40,   -97,
	   -97,   -47,   -35,  -111,    40,  -355,   276,  -357,  -118,   -71,
	   -61,   -44,   403,    40,    40,    40,   -48,   -35,   -64,   -48,
	  -256,   276,   -97,   -64,  -257,    58,  -178,   357,   -28,   308,
	   333,   339,   286,   -28,   293,   -64,  -371,    -2,   276,   339,
	  -371,   -97,  -285,  -263,    44,  -265,  -179,   258,   402,   401,
	   405,  -253,   256,    -6,   402,   327,   337,   336,   400,  -175,
	    -6,   403,   399,  -260,    -5,    -7,    -7,    -7,   -95,   -35,
	  -167,  -352,   289,   371,   315,  -311,  -228,    61,   -23,  -310,
	  -316,  -317,   280,  -282,   270,   -23,  -257,  -257,  -291,  -257,
	   -37,    40,   411,   -35,   400,   -48,   416,   -80,   -54,   -64,
	   -34,   400,   338,   -80,   -47,   -91,    40,  -392,    46,   -50,
	   -10,   -11,   407,    42,    47,    43,    45,   -35,   -35,    40,
	  -280,  -168,   -70,   -64,    40,    46,    40,    58,   -41,   399,
	   -64,    44,   -64,  -158,  -159,  -139,   -64,   281,   339,   342,
	   343,   344,   340,   308,  -157,  -392,   327,  -429,   327,  -197,
	  -198,   257,  -182,   315,   300,   305,   314,   -64,   331,  -210,
	  -211,  -196,   -64,  -145,   327,   331,    40,  -136,   334,  -392,
	   402,   -70,   327,   337,   -64,   351,   293,    -2,   -44,   350,
	   -64,   -64,   -33,   348,   349,  -445,  -447,    40,  -447,  -163,
	  -164,  -150,   400,   -44,  -311,  -311,   257,   -67,  -106,    40,
	   -51,   -50,   -43,   -47,  -349,    -2,  -122,  -123,  -170,   -38,
	  -455,  -126,  -127,   -38,  -126,  -102,    40,  -102,  -307,   298,
	  -350,    -2,   -64,   400,  -392,  -115,  -116,  -124,   -16,   -17,
	    -2,   282,   382,   276,   339,   365,  -128,  -129,    -3,    -2,
	   293,  -128,  -113,  -112,   -64,  -257,   -64,  -106,   -64,   -73,
	  -167,    40,   308,  -372,  -372,   -99,    40,   -37,    61,  -247,
	  -257,   400,   288,    -8,    -8,    -8,  -177,   369,   -23,   272,
	  -232,  -228,  -228,    61,   -37,   273,  -296,   -12,   267,   256,
	   406,   414,   -35,   -37,   -37,   -13,   -15,   -14,    60,    62,
	    61,   410,   408,   413,   325,   409,   412,   416,   -81,    40,
	   -84,    40,    46,   -84,   -92,    41,   -93,   -64,   282,   339,
	   400,   -35,   -35,   -35,    41,   -86,   -85,   -87,   -89,   -88,
	   -90,   -35,   260,   -39,   -72,   -43,   400,   403,   -44,    40,
	  -151,  -152,  -153,   -64,   400,  -208,  -209,   -64,   -53,    46,
	  -195,   332,   -64,  -137,   -68,   257,   -65,   -60,   -44,    44,
	    61,   -64,   -64,  -431,   327,   326,   331,    44,    40,  -143,
	   -64,   313,    44,  -427,    61,   325,  -160,    40,   -64,   -64,
	  -213,  -214,  -194,   400,   -64,    40,   -70,  -440,   -53,   -64,
	   332,   332,  -392,  -448,  -451,   -64,  -448,    44,    61,  -392,
	  -107,   -98,   -64,    61,    41,    44,    61,   -43,    41,    44,
	    61,    41,  -105,  -367,   298,  -103,  -104,   -98,  -105,  -383,
	    61,  -357,    46,    41,    44,    61,    40,    41,  -125,   -70,
	   364,   370,   282,    41,    44,    61,    41,   331,   -20,   400,
	   -23,  -256,  -256,  -212,  -213,  -373,   -70,  -100,  -101,   -38,
	  -286,   295,   256,  -264,   -23,   -30,    40,   298,   411,   -29,
	   339,   400,   -47,  -229,  -233,   -23,   -23,  -232,  -286,   -37,
	    41,   -35,  -147,   -55,   -40,   -44,  -181,   411,  -307,  -308,
	    41,   -85,    41,   400,    41,    44,    61,    61,    61,    41,
	  -309,  -239,  -309,    40,   -88,    46,  -280,  -280,  -280,  -213,
	    41,    44,    61,    41,    44,    61,   400,  -188,   -40,    40,
	   411,  -186,  -131,    45,  -187,  -454,   -57,   265,   400,  -205,
	  -206,  -207,  -183,   292,   399,    44,  -392,  -159,  -178,    40,
	   325,  -430,   325,   -64,  -182,  -213,  -178,  -211,   -64,   322,
	  -161,  -162,  -209,   -64,    44,    41,    61,   -51,  -213,   -65,
	  -441,   298,   332,  -154,  -155,   -64,  -154,  -449,  -452,    41,
	    44,  -449,  -164,  -148,    40,  -149,   -70,   257,   335,   342,
	    41,    44,   -47,  -123,  -124,  -127,    -3,    40,    41,    44,
	    61,  -384,  -385,  -387,   400,   -47,   -64,  -116,  -120,   -36,
	  -121,   327,   337,   336,    41,  -129,  -119,   -35,  -121,  -114,
	   -48,   272,  -311,    41,    41,    44,    61,  -256,   402,   284,
	   284,   -30,   400,   276,   400,  -307,  -229,  -229,  -233,   -56,
	   341,   284,   400,   326,   -82,   -83,   -78,   -49,   -79,   400,
	    41,   -93,   -35,   -35,   -35,   -89,   -88,   -87,   -90,   -40,
	   257,    41,   -21,   324,   323,  -153,  -134,   -64,   400,  -157,
	  -209,  -138,   -64,   400,   -12,  -392,  -186,  -188,   400,   399,
	  -188,   -10,   -11,   407,   -13,   -15,  -186,    40,    40,   415,
	    46,    40,    46,   415,   -65,  -141,   -70,   340,   257,   281,
	  -140,  -141,  -136,  -188,  -199,    40,    41,  -166,   327,   -24,
	   400,    41,    44,  -144,   325,  -214,  -195,  -186,    46,    41,
	   -65,  -163,  -154,    44,    61,  -451,  -140,   -64,   -98,    44,
	  -115,  -104,   -35,    44,  -388,    40,    61,    41,  -357,  -195,
	  -101,   -98,  -257,   -18,    44,  -243,   298,   411,    46,   400,
	   403,   404,  -447,    41,  -309,  -280,   -79,   -49,    46,  -309,
	  -309,    41,  -195,   -47,  -135,   298,    40,   400,  -185,   -31,
	   -32,   298,   411,    40,   400,  -188,    41,    41,  -186,  -186,
	  -186,  -186,  -186,  -132,  -133,   400,   -48,   400,  -189,  -190,
	  -186,   400,   400,   -48,   -27,   400,   345,   346,    41,    44,
	    40,  -200,   331,   -67,  -195,  -156,   332,   298,  -165,  -215,
	   -64,  -146,   323,  -162,   -66,  -184,   400,   257,  -195,  -155,
	   -70,   294,   293,   339,    41,  -350,    41,  -386,  -389,  -390,
	   400,   400,  -216,   330,   329,   -23,    41,   402,   335,   335,
	   400,    46,   400,   -83,   -88,  -167,  -280,   400,   400,   403,
	   -44,  -157,   284,   402,    40,   284,   335,   284,   335,   402,
	    40,    41,    44,    46,    61,   416,    41,  -191,   320,    44,
	    46,   416,  -141,   -62,   -64,  -201,   321,   -64,   257,    41,
	  -158,  -158,    44,    58,   399,  -157,   -53,  -216,    61,  -385,
	    41,    44,    61,  -393,   320,  -393,   -30,   400,  -449,    40,
	    41,   402,   298,   411,    41,    44,   402,  -133,   400,  -171,
	    -1,   400,   399,    46,  -195,   328,  -192,  -193,  -186,   400,
	  -186,   257,    46,    41,    44,  -202,   323,   -64,   257,  -215,
	   -64,  -157,   -47,  -390,   -70,   335,  -165,  -165,   -58,   -22,
	    -2,    41,   335,   335,   402,    41,    61,    46,   400,  -195,
	    44,    46,   400,  -157,   -64,  -204,   327,  -203,   -65,    41,
	    41,   -22,    41,  -171,   400,  -193,   257,  -195,   -64,   331,
	    58,   331,  -203,   -65,   -64
};

short yydef[] = {
	    -2,    -2,     6,     3,     4,     0,    21,     9,     0,     1,
	   263,    54,     0,    56,    58,    59,    68,     0,     0,   106,
	    86,    87,    88,    92,    97,    97,    97,     0,   104,   878,
	   879,    89,    90,    91,     0,     0,     0,     0,    21,    13,
	     5,    21,     0,     7,    53,     0,     0,    65,     0,    61,
	    69,    68,    93,   102,    98,    99,    64,   105,    94,    95,
	    96,   103,   114,   110,   118,   119,    26,     0,    20,    24,
	     0,    35,     0,   114,     0,     0,    15,   114,     0,    68,
	    60,     0,     0,    62,    63,     0,    12,   113,     0,    -2,
	    -2,    68,    16,     0,    27,    19,    23,    17,    18,    43,
	     0,     0,     8,    26,   114,    55,   114,    66,     0,    70,
	    72,     0,     0,    77,    78,   100,   101,   107,   108,   120,
	   121,   115,    -2,   117,   111,   129,   131,   133,   135,   137,
	   139,   140,   141,   142,   143,   144,   145,   146,   147,     0,
	   178,   179,   180,   181,   182,   183,   184,   235,   244,   110,
	   238,   239,   487,   488,   489,   490,   491,   492,   493,   494,
	   495,   496,   497,   498,   499,   500,   501,   502,   503,   504,
	   505,   506,   507,   508,   509,   510,     0,     0,   267,   268,
	   269,   270,   271,   272,   273,   274,   275,   276,   277,   278,
	   279,   280,   281,   282,   283,   284,   285,   286,   287,   288,
	   289,   290,   291,   292,   293,   294,     0,   168,   175,   860,
	     0,     0,   185,   249,   240,   246,   255,   257,    -2,   515,
	     0,     0,   520,     0,   652,   765,     0,   650,     0,     0,
	   547,     0,     0,     0,     0,   528,     0,     0,     0,     0,
	   629,   630,     0,     0,   344,   344,     0,     0,   375,     0,
	     0,     0,   344,   344,     0,     0,   311,   314,   320,   326,
	     0,     0,     0,     0,   447,     0,     0,     0,   339,   344,
	     0,   110,   177,   872,   188,   187,   650,   624,   512,   677,
	   530,   517,   539,     0,   543,   554,   694,   545,   770,   768,
	   769,   770,   790,   791,   703,   522,    -2,   706,   628,   723,
	   298,   348,   354,   300,   302,     0,     0,   422,   444,   360,
	   358,   305,   405,   312,   317,   321,   322,   323,   324,   325,
	   441,     0,   427,   446,   449,   329,   338,   344,   341,   370,
	   356,   151,   373,   409,   416,   153,   112,   127,   124,    25,
	    22,    41,    31,    32,    44,   828,   829,   822,     0,   817,
	   818,   819,   820,   821,    33,    34,    10,    14,    57,    67,
	     0,     0,     0,    -2,     0,     0,     0,     0,   106,   200,
	   864,   865,   262,     0,     0,   245,     0,   248,   244,   265,
	   149,     0,     0,   159,   160,   110,   110,   162,   166,   164,
	   110,     0,   176,     0,   874,     0,     0,     0,     0,   205,
	   189,   194,   880,   911,   870,   875,   186,     0,     0,     0,
	   798,   202,   203,   799,   800,    -2,   826,   827,   868,    -2,
	    -2,   871,   250,     0,   872,     0,   513,   518,   523,     0,
	   625,     0,   514,     0,   902,     0,    -2,   905,   906,     0,
	   631,   902,     0,   519,   537,     0,   886,   540,   541,   764,
	     0,   652,   882,   883,   911,     0,   649,   557,     0,     0,
	     0,     0,   546,   776,   788,     0,     0,     0,   526,   527,
	   705,   627,   899,   911,   901,   831,     0,   707,     0,     0,
	   743,   714,   727,   729,     0,     0,     0,   754,   755,   172,
	   262,   262,   378,   295,     0,   350,   804,   343,     0,   353,
	     0,     0,   371,   374,   406,   407,   866,   866,   866,   365,
	   365,   452,   807,   404,     0,   315,     0,   316,   440,   442,
	   895,   911,   830,     0,     0,     0,   448,   808,   332,   337,
	   110,     0,   350,     0,   150,   173,     0,   763,   767,   771,
	     0,   773,   774,   789,   724,   372,   387,   384,   385,   386,
	   387,   380,     0,     0,     0,   110,     0,    42,   823,   824,
	   825,    71,    73,    74,    79,    80,    81,    82,    83,    75,
	    76,    84,    85,   109,   130,   102,   102,   102,   138,   148,
	   251,   309,   255,   307,   465,   242,     0,     0,   237,     0,
	     0,     0,   266,   156,   157,   158,   161,   165,     0,     0,
	     0,     0,     0,     0,   873,     0,   806,   209,   218,   907,
	     0,    -2,   910,   218,   190,   191,     0,   881,     0,   877,
	     0,     0,     0,   833,   834,   835,   836,     0,   796,   802,
	     0,   256,   623,     0,   680,     0,     0,     0,   633,   634,
	   476,     0,     0,   651,   653,   657,   664,   665,   666,   667,
	   668,   669,   670,   671,   762,   884,     0,     0,   556,     0,
	   561,   562,   563,   565,   566,   567,   568,     0,     0,   544,
	   548,     0,   551,   778,     0,     0,     0,     0,     0,   900,
	   832,   708,   710,   711,     0,   715,     0,    -2,   911,   716,
	   717,   718,   719,   744,   745,   728,     0,   759,     0,   730,
	   731,   734,   737,   911,   260,   261,   296,   297,   347,     0,
	   342,   803,   345,   299,     0,   303,     0,   411,     0,   414,
	     0,     0,   423,     0,     0,   362,     0,   362,   304,   450,
	     0,   313,   326,   878,   896,     0,   418,     0,   390,     0,
	   394,   395,   396,   397,   398,   399,     0,   428,     0,   431,
	   432,     0,     0,    -2,   330,     0,   340,   369,   355,     0,
	     0,     0,   772,   410,   417,   377,   866,     0,   122,   126,
	     0,    38,     0,   132,   134,   136,     0,   308,   263,    -2,
	   114,     0,     0,     0,     0,   167,   169,     0,   170,   171,
	   837,   838,     0,     0,   811,     0,   813,     0,   839,   840,
	   841,   842,   843,   844,   913,   845,   846,   805,   452,   206,
	   214,     0,     0,   215,     0,   193,   196,     0,     0,     0,
	   876,   792,   793,   794,   795,     0,   801,   221,   222,   224,
	   229,   225,     0,   227,     0,   871,    -2,    -2,    -2,     0,
	     0,   681,   682,     0,   903,     0,   531,     0,   632,     0,
	   516,     0,   885,   538,   636,   637,   894,   891,   911,     0,
	   650,     0,   559,     0,   569,   570,   571,     0,     0,   650,
	   695,   696,     0,     0,   552,   553,     0,     0,   777,   702,
	     0,   467,     0,    -2,   471,     0,     0,   726,   712,   713,
	     0,     0,    -2,     0,   757,   756,     0,     0,     0,   738,
	     0,   352,   346,     0,   408,   866,     0,   867,   421,   866,
	     0,   443,   359,     0,   363,     0,   367,     0,   357,     0,
	     0,   318,     0,   415,     0,     0,   393,   391,     0,   400,
	   401,   402,   403,   426,     0,     0,   445,     0,   328,   335,
	   376,   262,   247,     0,   466,   388,   389,     0,   381,     0,
	   152,   154,   155,     0,   125,    28,     0,     0,     0,    38,
	     0,     0,   452,   236,   243,   263,   263,   114,   163,   810,
	   809,   812,   849,   815,   816,   869,     0,   912,   204,     0,
	   208,     0,   217,   908,   192,     0,     0,     0,     0,   797,
	     0,   234,     0,     0,   230,     0,   231,   232,   233,     0,
	     0,     0,     0,   652,     0,     0,   635,   474,   911,     0,
	     0,     0,   621,     0,   590,     0,   591,   592,    -2,   594,
	   597,   598,     0,   851,    -2,     0,   892,   654,     0,     0,
	     0,     0,   558,   573,   564,     0,   698,   549,   550,     0,
	     0,   780,   782,   784,     0,   476,     0,     0,     0,     0,
	   709,     0,     0,   720,   746,     0,   721,   752,     0,   760,
	   761,   753,   732,   733,     0,   736,   739,   740,   741,     0,
	   349,     0,     0,   412,   413,   424,   425,     0,   364,     0,
	     0,   451,   453,   459,     0,     0,   326,   419,   420,   435,
	   436,   437,   438,   439,   392,   429,   430,   433,   434,   327,
	   333,   334,   241,   476,   379,   866,     0,   110,    36,    39,
	    52,    29,    45,     0,    49,   306,   258,   259,   264,   814,
	     0,   914,   915,     0,     0,   211,   212,     0,   252,   861,
	   216,   195,   197,   198,   199,   219,   223,   220,   228,     0,
	   863,   476,     0,   678,   679,   683,   692,   685,   878,   529,
	   532,   534,   638,   878,     0,   475,     0,     0,    -2,   613,
	   618,     0,     0,     0,     0,   620,   589,     0,     0,     0,
	     0,   604,     0,     0,   893,   655,   672,   661,   662,   663,
	     0,   659,     0,   555,     0,     0,   476,   699,     0,   786,
	   775,   779,     0,   783,     0,   468,   521,   469,     0,   476,
	   704,   725,   722,     0,     0,   758,     0,   742,   351,     0,
	     0,   366,   368,   454,   456,     0,     0,   310,   319,   479,
	   382,   383,     0,     0,     0,    40,     0,     0,     0,    48,
	   847,   848,     0,   207,     0,     0,   253,     0,     0,     0,
	     0,   226,   511,   652,   684,     0,     0,     0,   533,   535,
	   536,     0,     0,     0,   642,   617,   588,   616,   585,   586,
	   587,   619,     0,     0,   853,     0,     0,   596,     0,   608,
	   605,    -2,   614,     0,   660,   673,   674,   675,   656,     0,
	     0,   578,     0,     0,   584,   693,     0,     0,   697,   482,
	   484,   652,     0,   781,   785,   470,   472,   473,   479,   747,
	   748,   749,   750,   751,   735,     0,   361,     0,     0,   460,
	     0,   457,   464,   481,   481,   123,    38,    37,    50,    51,
	    46,     0,     0,   210,   213,   254,     0,   862,     0,     0,
	     0,   676,   690,     0,     0,   643,   647,   644,   648,     0,
	     0,   850,     0,     0,     0,     0,   599,   476,     0,     0,
	     0,     0,   658,     0,   887,   580,     0,   574,   575,   572,
	   700,   701,     0,     0,   486,   766,   787,   652,     0,   455,
	   458,     0,     0,     0,   480,     0,    30,    47,   916,     0,
	   689,     0,     0,     0,   639,     0,     0,   852,     0,   855,
	   856,   858,   859,     0,   601,   476,   607,   609,   611,    -2,
	   603,   606,     0,   652,     0,   583,     0,   576,   577,   483,
	   485,   524,     0,   461,   462,   463,   477,   478,     0,   686,
	   688,   689,   645,   646,     0,   641,     0,     0,   595,   602,
	     0,     0,   615,   542,   888,   476,     0,     0,     0,   301,
	   691,   687,   640,   854,   857,   610,   612,   560,     0,     0,
	     0,     0,   579,   581,   582
};
/*
** Copyright (c) 2004 Ingres Corporation
*/
#ifndef lint
static char yaccpar_sccsid[] = "@(#)yaccpar	4.1	(Berkeley)	2/11/83";
#endif /* not lint */

/*
NO_OPTIM = dg8_us5
NO_OPTIM = nc4_us5
** History:   
**	16-apr-1990 (kimman)
**		Adding iyacc.par for iyacc (parser for iyacc output)
**	21-apr-1992 (Kevinm)
**		Added dg8_us5 to NO_OPTIM. 
**	26-apr-93 (tyler)
**		Added 'static' to local variable declarations in
**		order to allow individual programs to use multiple
**		YACC-generated parsers.
**	29-apr-93 (tyler)
**		Removed previous change.
**	19-aug-93 (brett)
**		Change yydebug to i4  so it matches fleas/hdr/osglob.h.
**	13-sep-93 (dianeh)
**		Removed NO_OPTIM setting for obsolete PC/RT (rtp_us5)
**		and HP (hp9_mpe).
**	13-oct-93 (brett)
**		Add declaration for yyparse, yycase is now fixed to allow this.
**	15-oct-93 (brett)
**		Correct the mistake of declaring yyparse to nat.  This is usually
**		done with a define in the .y file, although that reeks havoc on
**		DOS with prototypes.
**	15-jun-95 (popri01)
**		Added NO_OPTIM for nc4_us5 (NCR C Development Toolkit - 2.03.01)
**		to eliminate esqlc syntax error in dbutil/duf/duc/ducomom on
**		statement of form: "CREATE PROCEDURE bob(char)".
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	4-oct-01 (inkdo01)
**	    add parser reserved word retry logic.
**	15-feb-02 (toumi01)
**	    generalize parser reserved word retry logic to support abf.
*/

# define YYFLAG -1000
# define YYERROR goto yyerrlab
# define YYACCEPT return(0)
# define YYABORT return(1)

/*	parser for yacc output	*/

#ifdef YYDEBUG
i4  yydebug = 0; /* 1 for debugging */
#endif
YYSTYPE yyv[YYMAXDEPTH]; /* where the values are stored */
int yychar = -1; /* current input token number */
int yynerrs = 0;  /* number of errors */
short yyerrflag = 0;  /* error recovery flag */
#ifdef YYRETRY
GLOBALREF bool	yyreswd;
#endif

YYSTYPE *yypvt; /* Moved to global space from yyparse*/


yyparse() {

	short yys[YYMAXDEPTH];
	short yyj, yym;
	register short yystate, *yyps, yyn;
	register YYSTYPE *yypv;
	register short *yyxi;

	yystate = 0;
	yychar = -1;
	yynerrs = 0;
	yyerrflag = 0;
	yyps= &yys[-1];
	yypv= &yyv[-1];

 yystack:    /* put a state and value onto the stack */

#ifdef YYDEBUG
	if( yydebug  ) printf( "state %d, char 0%o\n", yystate, yychar );
#endif
		if( ++yyps> &yys[YYMAXDEPTH] ) { yyerror( "yacc stack overflow" ); return(1); }
		*yyps = yystate;
		++yypv;
		*yypv = yyval;

 yynewstate:

	yyn = yypact[yystate];

	if( yyn<= YYFLAG ) goto yydefault; /* simple state */

	if( yychar<0 ) if( (yychar=yylex())<0 ) yychar=0;
	if( (yyn += yychar)<0 || yyn >= YYLAST ) goto yydefault;

	if( yychk[ yyn=yyact[ yyn ] ] == yychar ){ /* valid shift */
		yychar = -1;
		yyval = yylval;
		yystate = yyn;
		if( yyerrflag > 0 ) --yyerrflag;
		goto yystack;
		}

 yydefault:
	/* default state action */

	if( (yyn=yydef[yystate]) == -2 ) {
		if( yychar<0 ) if( (yychar=yylex())<0 ) yychar = 0;
		/* look through exception table */

		for( yyxi=yyexca; (*yyxi!= (-1)) || (yyxi[1]!=yystate) ; yyxi += 2 ) ; /* VOID */

		while( *(yyxi+=2) >= 0 ){
			if( *yyxi == yychar ) break;
			}
		if( (yyn = yyxi[1]) < 0 ) return(0);   /* accept */
		}

	if( yyn == 0 ){ /* error */
		/* error ... attempt to resume parsing */

		switch( yyerrflag ){

		case 0:   /* brand new error */

#ifdef YYRETRY
			if (yyreswd)
			{
			    yyreswd = FALSE;
			    yychar = YYRETRYTOKEN;
			    goto yynewstate;
			}
#endif
			yyerror( "syntax error" );
		yyerrlab:
			++yynerrs;

		case 1:
		case 2: /* incompletely recovered error ... try again */

			yyerrflag = 3;

			/* find a state where "error" is a legal shift action */

			while ( yyps >= yys ) {
			   yyn = yypact[*yyps] + YYERRCODE;
			   if( yyn>= 0 && yyn < YYLAST && yychk[yyact[yyn]] == YYERRCODE ){
			      yystate = yyact[yyn];  /* simulate a shift of "error" */
			      goto yystack;
			      }
			   yyn = yypact[*yyps];

			   /* the current yyps has no shift onn "error", pop stack */

#ifdef YYDEBUG
			   if( yydebug ) printf( "error recovery pops state %d, uncovers %d\n", *yyps, yyps[-1] );
#endif
			   --yyps;
			   --yypv;
			   }

			/* there is no state on the stack with an error shift ... abort */

	yyabort:
			return(1);


		case 3:  /* no shift yet; clobber input char */

#ifdef YYDEBUG
			if( yydebug ) printf( "error recovery discards char %d\n", yychar );
#endif

			if( yychar == 0 ) goto yyabort; /* don't discard EOF, quit */
			yychar = -1;
			goto yynewstate;   /* try again in the same state */

			}

		}

	/* reduction by production yyn */

#ifdef YYDEBUG
		if( yydebug ) printf("reduce %d\n",yyn);
#endif
		yyps -= yyr2[yyn];
		yypvt = yypv;
		yypv -= yyr2[yyn];
		yyval = yypv[1];
		yym=yyn;
			/* consult goto table to find next state */
		yyn = yyr1[yyn];
		yyj = yypgo[yyn] + *yyps + 1;
		if( yyj>=YYLAST || yychk[ yystate = yyact[yyj] ] != -yyn ) yystate = yyact[yypgo[yyn]];
		switch(yym){
			
	case 1:
#line 957 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			oschkundefprocs(FormSym);
			if (osErrcnt == 0 || osDebugIL)
			{
				os_output();
			}
			oserrsummary(FormSym);
			if (osErrcnt == 0)
			{
				osdepwrite();
			}
			osFree();
		}
		break;
	case 2:
#line 971 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			oscerr(OSEEMPTY, 0);
		}
		break;
	case 7:
#line 997 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			register IGSID	*initSID;

			IGpopSID();		/* throw away display SID */
			osendblock();
			initSID = IGpopSID();
			IGsetSID(IGpopSID());	/* menu SID */
			IGendMenu(IL_BEGMENU, initSID);	/* end main menu */
			iiIGepEndProc(LocalFormSym->s_rdesc);
			osendproc(LocalFormSym);
		}
		break;
	case 8:
#line 1034 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGsetSID(IGtopSID());	/* display SID */
			IGgenStmt(IL_DISPLOOP, (IGSID *)NULL, 0);
		}
		break;
	case 9:
#line 1039 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			ostabsinit(FormSym);
			IGgenStmt(IL_INITIALIZE, (IGSID *)NULL, 0);

			IGsetSID(IGtopSID());	/* display SID */
			IGgenStmt(IL_DISPLOOP, (IGSID *)NULL, 0);
		}
		break;
	case 10:
#line 1048 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			ostabsinit(FormSym);
			IGgenStmt(IL_INITIALIZE, (IGSID *)NULL, 0);
		}
		break;
	case 11:
#line 1070 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			OSSYM	*framesym;
			PTR	rdesc;

			iiIGstmtInit();
			IGstartStmt(osscnlno(), IL_LB_NONE);
			FrmProcName = osFrm;
			rdesc = iiIGspStartProc(0, NULL);
			ostmpinit();
			ostmpnewproc();
			framesym = osobjinit(osFrm, osForm, OSFRAME);
			FormObj = FormSym = LocalFormSym =
				osformdecl(osForm, TRUE, &framesym->s_dbdt);
			LocalFormSym->s_rdesc = rdesc;

			IGpushSID(IGinitSID());	/* menu SID */
			IGgenStmt(IL_DISPLAY, IGtopSID(), 0);
			IGpushSID(IGinitSID());	/* initialization loop SID */
			IGsetSID(IGtopSID());
			IGpushSID(IGinitSID());	/* display loop SID */
			osblock(LP_DISPLAY, IGtopSID(),
					iiIG_string(ERx("$DISPLAY")));
			IGbgnMenu();
		}
		break;
	case 12:
#line 1116 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_RETPROC, (IGSID *)NULL, 1, (ILREF)0);
			iiIGepEndProc(LocalFormSym->s_rdesc);
			osendproc(LocalFormSym);
		}
		break;
	case 13:
#line 1123 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			OSSYM	*procsym;
			PTR	rdesc;
            CVlower(yypvt[-0].st_name);
			iiIGstmtInit();
			IGstartStmt(osscnlno(), IL_LB_NONE);
			ProcState = TRUE;
			FrmProcName = osFrm;
			if (*osFrm != EOS
				&& STbcompare(osFrm, 0, yypvt[-0].st_name, 0, TRUE) != 0)
			{
				osuerr( OSXPROC, 2, (PTR)osFrm, (PTR)yypvt[-0].st_name );
			}
			rdesc = iiIGspStartProc(0, NULL);
			ostmpinit();
			ostmpnewproc();
			procsym = osobjinit(osFrm, yypvt[-0].st_name, OSPROC);
			FormObj = FormSym = LocalFormSym =
				osformdecl(yypvt[-0].st_name, FALSE, &procsym->s_dbdt);
			LocalFormSym->s_rdesc = rdesc;
			DispLoopCount = 0;
		}
		break;
	case 14:
#line 1147 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_RETPROC, (IGSID *)NULL, 1, (ILREF)0);
			iiIGepEndProc(LocalFormSym->s_rdesc);
			osendproc(LocalFormSym);
		}
		break;
	case 15:
#line 1154 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			ILREF	ilref;
                       
            CVlower(yypvt[-0].st_name);
			IGstartStmt(osscnlno(), IL_LB_NONE);
			LocalProcState = TRUE;
			FrmProcName = yypvt[-0].st_name;
			LocalFormSym = osdefineproc(yypvt[-0].st_name, FormSym);
			LocalFormSym->s_rdesc = iiIGspStartProc(
					IGsetConst(DB_CHA_TYPE, yypvt[-0].st_name),
					LocalFormSym->s_parent->s_rdesc);
			ostmpnewproc();
			DispLoopCount	= 0;
			SubMState	= 1;
			SubMInAlert	= 0;
			SubMGotNonAlert	= 0;
		}
		break;
	case 16:
#line 1173 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_INITIALIZE, (IGSID *)NULL, 0);
		}
		break;
	case 25:
#line 1211 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			DeclareFlags &= ~FDF_LOCAL;
		}
		break;
	case 27:
#line 1218 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			DeclareFlags |= FDF_LOCAL;
		}
		break;
	case 28:
#line 1241 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( yypvt[-5].st_node != NULL )
			{
				i4 flags = DeclareFlags;

				if (yypvt[-2].st_nat)
					flags |= FDF_ARRAY;

				(VOID)osdeclare( LocalFormSym, yypvt[-5].st_node->n_name,
						yypvt[-5].st_node->n_attr, yypvt[-1].st_name,
						(char *)NULL, (char *)NULL,
						yypvt[-0].st_name, flags,
						(bool)yypvt[-3].st_nat
				);
				ostrfree(yypvt[-5].st_node);
			}
		}
		break;
	case 29:
#line 1260 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( yypvt[-6].st_node != NULL && yypvt[-1].st_name != NULL )
			{
				i4 flags = DeclareFlags;

				if (yypvt[-3].st_nat)
					flags |= FDF_ARRAY;

				(VOID)osdeclare( LocalFormSym, yypvt[-6].st_node->n_name,
						yypvt[-6].st_node->n_attr, yypvt[-1].st_name,
						(char *)NULL, (char *)NULL,
						yypvt[-0].st_name, flags,
						(bool)yypvt[-4].st_nat
				);
				ostrfree(yypvt[-6].st_node);
			}
		}
		break;
	case 30:
#line 1279 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (yypvt[-9].st_node != NULL)
			{
				i4 flags = DeclareFlags;

				if (yypvt[-6].st_nat)
					flags |= FDF_ARRAY;

				(VOID)osdeclare( LocalFormSym, yypvt[-9].st_node->n_name,
						yypvt[-9].st_node->n_attr, yypvt[-5].st_name,
						yypvt[-3].st_name, yypvt[-2].st_name,
						yypvt[-0].st_name, flags,
						(bool)yypvt[-7].st_nat
				);
				ostrfree(yypvt[-9].st_node);
			}
		}
		break;
	case 31:
#line 1297 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (yypvt[-2].st_node != NULL)
			{
				(VOID)osdeclare( LocalFormSym, yypvt[-2].st_node->n_name,
						yypvt[-2].st_node->n_attr, ERx("none"),
						(char *)NULL, (char *)NULL,
						(char *)NULL, DeclareFlags,
						(bool)TRUE
				);
				ostrfree(yypvt[-2].st_node);
			}
		}
		break;
	case 32:
#line 1316 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyerror(_SyntaxError);
			_VOID_ ossymundef(yypvt[-2].st_node->n_name, LocalFormSym);
			ostrfree(yypvt[-0].st_node);
			ostrfree(yypvt[-2].st_node);
		}
		break;
	case 33:
#line 1324 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (ProcState || LocalProcState)
			{
				oscerr(E_OS016F_InProcOrLocalProc,
						1, ERget(_ColumnDecl));
				yyval.st_node = NULL;
			}
			else
			{
				u_ptr[0].u_cp = yypvt[-2].st_name;
				u_ptr[1].u_cp = yypvt[-0].st_name;
				yyval.st_node = osmknode(ATTR, &u_ptr[0], &u_ptr[1],
							(U_ARG*)NULL);
			}
		}
		break;
	case 34:
#line 1340 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (ProcState || LocalProcState)
			{
				oscerr(E_OS016F_InProcOrLocalProc,
						1, ERget(_ColumnDecl));
				yyval.st_node = NULL;
			}
			else
			{
				u_ptr[0].u_cp = yypvt[-2].st_name;
				u_ptr[1].u_cp = yypvt[-0].st_name;
				yyval.st_node = osmknode(ATTR, &u_ptr[0], &u_ptr[1],
							(U_ARG*)NULL);
			}
		}
		break;
	case 35:
#line 1356 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_cp = yypvt[-0].st_name;
			u_ptr[1].u_cp = NULL;
			yyval.st_node = osmknode(ATTR, &u_ptr[0], &u_ptr[1], (U_ARG*)NULL);
		}
		break;
	case 36:
#line 1363 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = NULL;
		}
		break;
	case 37:
#line 1367 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 38:
#line 1372 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = NULL;
		}
		break;
	case 39:
#line 1376 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = ERx("with null");
		}
		break;
	case 40:
#line 1380 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = ERx("not null");
		}
		break;
	case 41:
#line 1385 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_nat = FALSE;
		}
		break;
	case 42:
#line 1389 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_nat = TRUE;
		}
		break;
	case 43:
#line 1394 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_nat = FALSE;
		}
		break;
	case 44:
#line 1398 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			/*
			** Local procedures within local procedures
			** can be allowed by changing the #if 1 below to #if 0.
			*/
#if 1
			if (LocalProcState)
			{
				oscerr(E_OS016D_NestedLocalProc, 0);
			}
#endif
			yyval.st_nat = TRUE;
		}
		break;
	case 45:
#line 1413 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = osldtable(yypvt[-0].st_name, (char *)NULL);
		}
		break;
	case 46:
#line 1417 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = osldtable(yypvt[-0].st_name, yypvt[-2].st_name);
		}
		break;
	case 47:
#line 1421 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = osldform(yypvt[-2].st_name, yypvt[-0].st_name);
		}
		break;
	case 48:
#line 1425 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = osldform(FormSym->s_name, yypvt[-0].st_name);
		}
		break;
	case 49:
#line 1429 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
			if (!osw_compare(ERx("form"), yypvt[-1].st_name))
				yyerror(_SyntaxError);
			yyval.st_name = osldform(yypvt[-0].st_name, (char*)NULL);
		}
		break;
	case 55:
#line 1454 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			SubMInAlert &= ~1;
		}
		break;
	case 56:
#line 1458 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			i4	line;

			line = osscnlno();
			oscerr(OSEHEADER, 1, (PTR)&line);

			SubMInAlert &= ~1;

		}
		break;
	case 58:
#line 1477 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (ActState)
				IGsetSID(IGpopSID() /* operation SID */);
			ActState = FALSE;
		}
		break;
	case 61:
#line 1510 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGactivate( OLMENU, (PTR)osvalref(yypvt[-1].st_node), yypvt[-0].st_name,
					IGtopSID() /* operation SID */
			);
			SubMGotNonAlert |= 1;
		}
		break;
	case 62:
#line 1517 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( yypvt[-1].st_name != NULL )
				IGactivate( OLKEY, yypvt[-1].st_name, yypvt[-0].st_name,
						IGtopSID()/* operation SID */
				);
			else
			{ /* no FRSkey */
				i4	line = osscnlno();
				oscerr(OSEHEADER, 1, (PTR)&line);
			}
			SubMGotNonAlert |= 1;
		}
		break;
	case 63:
#line 1530 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			register char	*cp;

			if ( yypvt[-0].st_name == NULL &&
				      (cp = STindex(yypvt[-1].st_name, ERx("."), 0)) == NULL )
			{ /* field */
				if ( !osw_compare(_All, yypvt[-1].st_name) )
				{
					osfldcheck(FormSym, yypvt[-1].st_name);
				}
				IGactivate(yypvt[-2].st_nat ? OLFLD_ENTRY : OLFIELD, yypvt[-1].st_name,
						(PTR)NULL,
						IGtopSID() /* operation SID */
				);
			}
			else
			{ /* should be table field . column */
				char	buf[OSBUFSIZE];
				char	*table;
				char	*column;

				if ( yypvt[-0].st_name != NULL )
				{
					table = yypvt[-1].st_name;
					column = yypvt[-0].st_name;
				}
				else
				{
					_VOID_ STlcopy(yypvt[-1].st_name, buf, cp - yypvt[-1].st_name);
					table = iiIG_string(buf);
					STcopy(cp+1, buf);
					column = iiIG_string(buf);
				}
				if ( !osw_compare(_All, column) )
					oscolcheck(FormSym, table, column);
				else
					_VOID_ ostblcheck(FormSym, table);
				IGactivate( yypvt[-2].st_nat ? OLCOL_ENTRY : OLCOLUMN, table,
						column,
						IGtopSID() /* operation SID */
				);
			}
			SubMGotNonAlert |= 1;
		}
		break;
	case 64:
#line 1575 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( !ActState )
			{ /* 1st activation in list */
				IGpushSID(IGinitSID()); /* operations SID */
				IGstartStmt(osscnlno(), IL_LB_MENU);
				ActState = TRUE;
			}
			if ( yypvt[-1].st_nat != -1 )
			{
				if ( yypvt[-1].st_nat == OLALERTER )
				{
					if ( yypvt[-0].st_node != NULL )
					{
						yyerror(_SyntaxError);
					}
					if ( ( SubMState & 1 ) == 0 )
					{ /* In RUN_MENU */
						oscerr(E_OS017D_BabSubmAct, 0);
					}
					SubMInAlert |= 1;
				}
				else if ( yypvt[-1].st_nat == OLTIMEOUT )
				{
					if ( yypvt[-0].st_node != NULL
						&& yypvt[-0].st_node->n_token != tkICONST )
					{
						yyerror(_SyntaxError);
					}
					SubMGotNonAlert |= 1;
				}

				IGactivate( yypvt[-1].st_nat, yypvt[-0].st_node, (PTR)NULL,
					IGtopSID() /* operations SID */
				);
			}
		}
		break;
	case 65:
#line 1611 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{ i4  line = osscnlno();
				oscerr(OSEHEADER, 1, (PTR)&line);}
		break;
	case 67:
#line 1616 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = (PTR)ActOpt;
		}
		break;
	case 68:
#line 1620 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = NULL;
		}
		break;
	case 69:
#line 1625 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			ActOpt[OPT_EXPL]  = 0;
			ActOpt[OPT_VAL]   = FRS_UF;
			ActOpt[OPT_ACTIV] = FRS_UF;
		}
		break;
	case 74:
#line 1637 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( ActOpt[OPT_VAL] == FRS_UF ) 
			{
				if ( *(yypvt[-0].st_name+1) == EOS )
				{
					if ( *yypvt[-0].st_name == '0' )
					{
						ActOpt[OPT_VAL] = FRS_NO;
					}
					else if ( *yypvt[-0].st_name == '1' )
					{
						ActOpt[OPT_VAL] = FRS_YES;
					}
				}
				if ( ActOpt[OPT_VAL] == FRS_UF )
				{
					frs_error( E_EQ0306_actARGS, EQ_ERROR,
						2, ERx("validate"), yypvt[-0].st_name );
				}
			}
		}
		break;
	case 75:
#line 1659 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( yypvt[-2].st_nat != 0 )
			{
				if ( yypvt[-2].st_nat != tkSCONST )
				{
					yyerror(_SyntaxError);
				}
				else if ( ActOpt[OPT_EXPL] == 0 )
				{
					ActOpt[OPT_EXPL] = yypvt[-0].st_nat;
				}
			}
		}
		break;
	case 76:
#line 1673 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( yypvt[-2].st_nat != 0 )
			{
				if ( yypvt[-2].st_nat != tkICONST )
				{
					yyerror(_SyntaxError);
				}
				else if ( ActOpt[OPT_ACTIV] == FRS_UF )
				{
					if ( *(yypvt[-0].st_name+1) == EOS )
					{
						if ( *yypvt[-0].st_name == '0' )
						{
							ActOpt[OPT_ACTIV] =
							    FRS_NO;
						}
						else if ( *yypvt[-0].st_name == '1' )
						{
							ActOpt[OPT_ACTIV] =
							    FRS_YES;
						}
					}
					if ( ActOpt[OPT_ACTIV] == FRS_UF )
					{
						frs_error( E_EQ0306_actARGS,
							   EQ_ERROR, 2,
							   ERx("activate"),
							   yypvt[-0].st_name );
					}
				}
			}
		}
		break;
	case 77:
#line 1707 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( ActOpt[OPT_VAL] != FRS_UF )
			{
				yyerror(_SyntaxError);
			}
		}
		break;
	case 78:
#line 1715 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (osw_compare(_Explanation, yypvt[-0].st_name))
			{
				if ( ActOpt[OPT_EXPL] != 0 )
				{
					yyerror(_SyntaxError);
				}
				yyval.st_nat = tkSCONST;
			}
			else if (osw_compare(_Activate, yypvt[-0].st_name))
			{
				if ( ActOpt[OPT_ACTIV] != FRS_UF )
				{
					yyerror(_SyntaxError);
				}
				yyval.st_nat = tkICONST;
			}
			else
			{
				yyerror(ERx("VALIDATE, EXPLANATION, ACTIVATE"));
				yyval.st_nat = 0;
			}
		}
		break;
	case 79:
#line 1741 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 80:
#line 1745 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = ERx("1");
		}
		break;
	case 81:
#line 1749 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = ERx("0");
		}
		break;
	case 82:
#line 1753 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = ERx("2");
		}
		break;
	case 83:
#line 1757 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			static READONLY char	_True[]		= ERx("true"),
						_False[]	= ERx("false");

			if ( osw_compare(_True, yypvt[-0].st_name) )
				yyval.st_name = ERx("1");
			else if ( osw_compare(_False, yypvt[-0].st_name) )
				yyval.st_name = ERx("0");
			else
			{
				yyerror(_SyntaxError);
				yyval.st_name = ERx("0");
			}
		}
		break;
	case 84:
#line 1773 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_nat = IGsetConst( DB_CHR_TYPE,
					 osstring( FALSE, yypvt[-0].st_name, osldml ) );
		}
		break;
	case 85:
#line 1778 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			OSSYM	*sym;

			sym = osfld( FormSym, yypvt[-0].st_name, OSOBJUNDEF );
			yyval.st_nat = sym->s_ilref;
			if ( sym->s_kind != OSACONST )
			{
				yyerror( _SyntaxError );
			}
		}
		break;
	case 86:
#line 1792 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (!ActState)
			{ /* 1st activation in list */
				IGpushSID(IGinitSID()); /* operations SID */
				IGstartStmt(osscnlno(), IL_LB_MENU);
				ActState = TRUE;
			}
			yyval.st_node = osmkconst(tkSCONST, osstring( FALSE, yypvt[-0].st_name, osldml ));
		}
		break;
	case 87:
#line 1802 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (!ActState)
			{ /* 1st activation in list */
				IGpushSID(IGinitSID()); /* operations SID */
				IGstartStmt(osscnlno(), IL_LB_MENU);
				ActState = TRUE;
			}
			yyval.st_node = osmkconst(tkSCONST, yypvt[-0].st_name);
		}
		break;
	case 88:
#line 1812 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (!ActState)
			{ /* 1st activation in list */
				IGpushSID(IGinitSID()); /* operations SID */
				IGstartStmt(osscnlno(), IL_LB_MENU);
				ActState = TRUE;
			}

			yyval.st_node = osvalnode(FormSym, yypvt[-0].st_name);
			if ( yyval.st_node->n_sym->s_kind != OSACONST )
				yyerror(_SyntaxError);
		}
		break;
	case 89:
#line 1826 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 90:
#line 1830 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 91:
#line 1834 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 92:
#line 1840 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (!ActState)
			{ /* 1st activation in list */
				IGpushSID(IGinitSID()); /* operations SID */
				IGstartStmt(osscnlno(), IL_LB_MENU);
				ActState = TRUE;
			}
		}
		break;
	case 93:
#line 1850 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			register i4	keyval;

			if ((keyval = frsck_key(yypvt[-0].st_name)) == 0)
			{
				yyerror(ERx("FRSKEYn"));
				yyval.st_name = NULL;
			}
			else if (keyval < 0)
			{
				char	key_max[24];

				CVna(Fsi_KEY_PF_MAX, key_max);
				frs_error(E_EQ0157_fsNUM, EQ_ERROR,
						2, ERx("FRSKEY"), key_max
					);
				yyval.st_name = iiIG_string(ERx("1"));
			}
			else
			{
				char	buf[24];

				CVna(keyval, buf);
				yyval.st_name = iiIG_string(buf);
			}
		}
		break;
	case 94:
#line 1879 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_nat = FALSE;
		}
		break;
	case 95:
#line 1883 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_nat = FALSE;
		}
		break;
	case 96:
#line 1887 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_nat = TRUE;
		}
		break;
	case 97:
#line 1892 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( ( SubMState & 1 ) == 0 ) /* In RUN_MENU */
			{
				oscerr( E_OS017D_BabSubmAct, 0 );
			}

			if (!ActState)
			{ /* 1st activation in list */
				IGpushSID(IGinitSID()); /* operations SID */
				IGstartStmt(osscnlno(), IL_LB_MENU);
				ActState = TRUE;
			}
		}
		break;
	case 98:
#line 1907 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 99:
#line 1911 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = _All;
		}
		break;
	case 100:
#line 1916 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 101:
#line 1920 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = _All;
		}
		break;
	case 102:
#line 1924 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = NULL;
		}
		break;
	case 103:
#line 1930 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( osw_compare(ERx("timeout"), yypvt[-0].st_name) )
			{
				yyval.st_nat = OLTIMEOUT;
			}
			else
			{
				yyerror(ERx("TIMEOUT"));
				yyval.st_nat = -1;
			}
		}
		break;
	case 104:
#line 1942 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_nat = OLALERTER;
		}
		break;
	case 105:
#line 1947 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkconst(tkICONST, yypvt[-0].st_name);
		}
		break;
	case 106:
#line 1951 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 107:
#line 1979 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( !( ProcState || LocalProcState ) || SubMState > 1 )
			{
				IGstartStmt(osscnlno(), IL_LB_NONE);
				IGgenStmt( IL_GOTO,
					IGtopSID() /* display/menu loop SID */,
					0
				);
			}
		}
		break;
	case 108:
#line 1998 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{ ostmpbeg(); osclrrefs(FormSym); }
		break;
	case 109:
#line 1999 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osgenputs();
			ostmpend();
		}
		break;
	case 110:
#line 2003 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{ ostmpbeg(); osclrrefs(FormSym); }
		break;
	case 111:
#line 2004 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osgenputs();
			ostmpend();
		}
		break;
	case 116:
#line 2018 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{oscerr(OSESEMIC, 0);}
		break;
	case 120:
#line 2028 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 121:
#line 2032 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = ERx("}");
		}
		break;
	case 122:
#line 2038 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{operr();}
		break;
	case 124:
#line 2040 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{operr();}
		break;
	case 129:
#line 2048 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{yyerror(_SyntaxError);}
		break;
	case 130:
#line 2048 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{YYERROR;}
		break;
	case 131:
#line 2050 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{yyerror(_SyntaxError);}
		break;
	case 132:
#line 2050 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{YYERROR;}
		break;
	case 133:
#line 2052 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{yyerror(_SyntaxError);}
		break;
	case 134:
#line 2052 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{YYERROR;}
		break;
	case 135:
#line 2054 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{yyerror(_SyntaxError);}
		break;
	case 136:
#line 2054 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{YYERROR;}
		break;
	case 137:
#line 2056 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{yyerror(_SyntaxError);}
		break;
	case 138:
#line 2056 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{YYERROR;}
		break;
	case 142:
#line 2065 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osdbflush();
			IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
		}
		break;
	case 148:
#line 2095 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{

			/* Special case:  Generate code for procedure calls
			** even if the procedure is not defined at this time.
			*/
			if ( !osiserr(yypvt[-2].st_node) )
			{
				if (yypvt[-2].st_node->n_flags & N_READONLY)
				{
					oscerr(OSASNOVAR, 1, osnodename(yypvt[-2].st_node));
				}
				else if (  (  yypvt[-0].st_node->n_token == tkPCALL
				           && (  yypvt[-0].st_node->n_psym == NULL
					      || yypvt[-0].st_node->n_psym->s_kind == OSUNDEF
					      )
				           )
				        || oschktypes(yypvt[-0].st_node->n_type, yypvt[-2].st_node->n_type)
				        )
				{ /* generate assignment */
					if (AFE_NULLABLE_MACRO(yypvt[-0].st_node->n_type)
					  && !AFE_NULLABLE_MACRO(yypvt[-2].st_node->n_type))
					{
						if (yypvt[-0].st_node->n_token == tkNULL)
							oscerr(OSNULLEXPR, 0);
						else
							oswarn(OSNULLCHK, 0);
					}

					osevalexpr(yypvt[-0].st_node, osnodesym(yypvt[-2].st_node));
					osvardisplay(yypvt[-2].st_node);
				}
				else
				{
					oscerr(OSASNTYPES, 0);
				}
			}
			ostrfree(yypvt[-2].st_node);
			ostrfree(yypvt[-0].st_node);

			FormObj = FormSym;
		}
		break;
	case 149:
#line 2152 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGsetSID(IGpopSID());	/* end SID */
		}
		break;
	case 150:
#line 2169 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			register IGSID	*elseSID = IGpopSID();

			IGgenStmt(IL_GOTO, IGtopSID() /* end SID */, 0);
			IGsetSID(elseSID);
		}
		break;
	case 151:
#line 2204 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{ostmpbeg();}
		break;
	case 152:
#line 2205 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			ILREF	refer;

			refer = osvalref(yypvt[-1].st_node);
			osgenputs();
			ostmpend();
			IGgenStmt(IL_IF, IGtopSID() /* else SID */, 1, refer);
		}
		break;
	case 153:
#line 2227 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGpushSID(IGinitSID());		/* end SID */
			IGpushSID(IGinitSID());		/* else SID */
		}
		break;
	case 155:
#line 2241 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{yyerror(ERx("THEN"));}
		break;
	case 158:
#line 2252 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{ /* Error! */
			i4	line = osscnlno();

			oscerr(OSENDIF, 2, (PTR)&line, yypvt[-0].st_name);
			YYERROR;
		}
		break;
	case 160:
#line 2262 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{ /* Error! */
			i4	line = osscnlno();

			oscerr(OSENDIF, 2, (PTR)&line, yypvt[-0].st_name);
			YYERROR;
		}
		break;
	case 161:
#line 2282 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			register IGSID	*elseSID = IGpopSID();

			IGgenStmt(IL_GOTO, IGtopSID() /* end SID */, 0);
			IGsetSID(elseSID);
		}
		break;
	case 162:
#line 2317 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{ostmpbeg();}
		break;
	case 163:
#line 2318 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			ILREF	refer;

			refer = osvalref(yypvt[-1].st_node);
			osgenputs();
			ostmpend();
			IGgenStmt(IL_IF, IGtopSID() /* else SID */, 1, refer);
		}
		break;
	case 164:
#line 2340 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGpushSID(IGinitSID());		/* new else SID */
		}
		break;
	case 166:
#line 2363 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 167:
#line 2388 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			register IGSID	*endSID = IGpopSID();

			IGgenStmt(IL_GOTO, IGpopSID() /* while SID */, 0);
			IGsetSID(endSID);
			osendblock();
		}
		break;
	case 168:
#line 2429 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{ostmpbeg();}
		break;
	case 169:
#line 2430 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			ILREF	refer;

			refer = osvalref(yypvt[-1].st_node);
			osgenputs();
			ostmpend();
			IGgenStmt(IL_IF, IGtopSID() /* end SID */, 1, refer);
		}
		break;
	case 171:
#line 2440 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{yyerror(ERx("DO"));}
		break;
	case 172:
#line 2457 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGpushSID(IGinitSID());		/* while SID */
			IGsetSID(IGtopSID());
			IGstartStmt(osscnlno(), IL_LB_WHILE);
			IGpushSID(IGinitSID());		/* end SID */
			osblock(LP_WHILE, IGtopSID(), yypvt[-1].st_name);
		}
		break;
	case 173:
#line 2471 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-1].st_name;
		}
		break;
	case 174:
#line 2476 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = NULL;
		}
		break;
	case 175:
#line 2493 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (osblkbreak(LP_NONDISPLAY, (char *)NULL) != OK)
			{
				oscerr(OSENDLOOP, 0);
			}
		}
		break;
	case 176:
#line 2500 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			_VOID_ osblkbreak(LP_LABEL, yypvt[-0].st_name);
		}
		break;
	case 177:
#line 2505 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 184:
#line 2528 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			_VOID_ osblkbreak(LP_NONE, (char *)NULL);
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_EXIT, (IGSID *)NULL, 0);
		}
		break;
	case 185:
#line 2547 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			DB_DT_ID	rettype;

			_VOID_ osblkbreak(LP_NONE, (char *)NULL);
			rettype = LocalFormSym->s_type;
			if (rettype != DB_NODT && osCompat > 5)
				oswarn(OSRETTYPE, 1, FrmProcName);
			if (ProcState || LocalProcState)
			{
				IGgenStmt(IL_RETPROC, (IGSID *)NULL,
					1, (ILREF)0);
			}
			else
			{
				IGgenStmt(IL_RETFRM, (IGSID *)NULL,
					1, (ILREF)0);
			}
		}
		break;
	case 186:
#line 2566 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			DB_DT_ID	rettype;

			_VOID_ osblkbreak(LP_NONE, (char *)NULL);
			rettype = LocalFormSym->s_type;
			if ( rettype == DB_NODT )
				oscerr(OSNORETYPE, 1, FrmProcName);

			if ( yypvt[-0].st_node->n_type != DB_NODT )
			{ /* valid return expression */
				if ( !oschktypes(yypvt[-0].st_node->n_type, rettype) )
					oscerr(OSBADRET, 1, FrmProcName);
				else if ( !AFE_NULLABLE(rettype) )
				{
					if ( yypvt[-0].st_node->n_token == tkNULL )
						oscerr(OSNULLRET, 1,
								FrmProcName);
					else if ( AFE_NULLABLE(yypvt[-0].st_node->n_type) )
						oswarn(OSNULRETWARN, 1,
								FrmProcName);
				}

				/*
				** more dangerous magic: character constants
				** must be of type varchar. Bug 44485.
				*/
				op_vch_change(yypvt[-0].st_node);

				if (ProcState || LocalProcState)
				{
					IGgenStmt(IL_RETPROC, (IGSID *)NULL,
						1, osvarref(yypvt[-0].st_node));
				}
				else
				{
					IGgenStmt(IL_RETFRM, (IGSID *)NULL,
						1, osvarref(yypvt[-0].st_node));
				}
			}
		}
		break;
	case 187:
#line 2608 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 188:
#line 2620 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 189:
#line 2631 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_CALSYS, (IGSID *)NULL, 1, 0);
		}
		break;
	case 190:
#line 2635 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_CALSYS, (IGSID *)NULL, 1, osvalref(yypvt[-0].st_node));
		}
		break;
	case 191:
#line 2647 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			ossubsys(yypvt[-1].st_node, yypvt[-0].st_node);
		}
		break;
	case 192:
#line 2652 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 193:
#line 2656 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 194:
#line 2660 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 195:
#line 2665 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 196:
#line 2672 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 197:
#line 2680 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( AFE_NULLABLE_MACRO(yypvt[-0].st_node->n_type) )
				oswarn( yypvt[-0].st_node->n_token == tkNULL
					? OSNULLEXPR : OSNULVCHK, 0
				);
			u_ptr[0].u_cp = ERx(":=");
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(ASNOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 198:
#line 2691 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( AFE_NULLABLE_MACRO(yypvt[-0].st_node->n_type) )
				oswarn( yypvt[-0].st_node->n_token == tkNULL
					? OSNULLEXPR : OSNULVCHK, 0
				);
			u_ptr[0].u_cp = ERx(":=");
			u_ptr[1].u_nodep = osmkident( yypvt[-2].st_name, (OSNODE *)NULL );
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(ASNOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 199:
#line 2702 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( AFE_NULLABLE_MACRO(yypvt[-0].st_node->n_type) )
				oswarn( yypvt[-0].st_node->n_token == tkNULL
					? OSNULLEXPR : OSNULVCHK, 0
				);
			u_ptr[0].u_cp = ERx(":=");
			u_ptr[1].u_nodep = osmkident( yypvt[-2].st_name, (OSNODE *)NULL );
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(ASNOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 200:
#line 2721 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (yypvt[-1].st_node->n_flags & N_READONLY)
				yyval.st_node = NULL;
			else
				yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 201:
#line 2728 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			yyval.st_node = NULL;
		}
		break;
	case 202:
#line 2734 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 203:
#line 2738 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkident( yypvt[-0].st_name, (OSNODE *)NULL );
			yyval.st_node->n_type = DB_CHA_TYPE;	/* special case */
		}
		break;
	case 204:
#line 2765 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			register OSNODE	*node;

			if (yypvt[-2].st_node->n_token == tkID)
			{
				u_ptr[0].u_symp = osframesym(yypvt[-2].st_node->n_value, TRUE);
			}
			else
			{
				u_ptr[0].u_symp = (OSSYM *)NULL;
			}
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-1].st_node;
			node = osmknode(tkFCALL, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
			if (yypvt[-4].st_node == NULL)
			{
				if ( node->n_type != DB_NODT && osCompat > 5
						&& osUser )
					oswarn(OSEXPRETVAL, 1, node->n_fsym->s_name);
				/* build param structure and call frame */
				osgencall(node, (OSSYM *)NULL);
			}
			else
			{ /* assignment */
				register OSSYM	*var = osnodesym(yypvt[-4].st_node);

				if (node->n_type != DB_NODT)
					node->n_fsym->s_ref |= OS_OBJREF;
				else
				{
					register OSSYM	*frame = node->n_fsym;

					if ( frame != NULL )
					{
					    if ( frame->s_kind == OSUNDEF )
						frame->s_ref |= OS_OBJREF;
					    else
						oscerr( OSNORETVAL,
							1, frame->s_name
						);
					}
				}

				if (osiserr(yypvt[-4].st_node))
					var = NULL;
				else if (node->n_type != DB_NODT
				  && !oschktypes(node->n_type, yypvt[-4].st_node->n_type) )
				{
					oscerr(OSASNTYPES, 0);
					var = NULL;
				}

				/* build param structure and call frame */
				osgencall(node, var);
				if (var != NULL)
					osvardisplay(yypvt[-4].st_node);
				ostrfree(yypvt[-4].st_node);
			}
			FormObj = FormSym;
			ostrfree(node);
		}
		break;
	case 205:
#line 2828 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			FrsStmt = IIDISPFRM;
		}
		break;
	case 206:
#line 2833 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{CallState = TRUE;}
		break;
	case 207:
#line 2834 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			CallState = FALSE;
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 208:
#line 2839 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 209:
#line 2843 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 210:
#line 2848 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-2].st_node;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(COMMA, (U_ARG*)NULL, &u_ptr[0], &u_ptr[1]);
		}
		break;
	case 211:
#line 2854 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 212:
#line 2859 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 213:
#line 2863 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			/* ID [ '.' ID ] assign_op ... */
			/* save flag since 'osvarref()' frees node */
			bool		byref = yypvt[-0].st_node->n_token == tkBYREF;

			if (yypvt[-2].st_node->n_token != ATTR
			  || (yypvt[-0].st_node->n_token == DOTALL && yypvt[-2].st_node->n_attr != NULL))
			{
				osuerr(OSNULLCONST, 1, ERx("attribute"));
			}

			if (yypvt[-0].st_node->n_token == DOTALL)
			{
				yyval.st_node = yypvt[-0].st_node;
				ostrfree(yypvt[-2].st_node);
			}
			else
			{
				yyval.st_node = osmkassign(
					IGsetConst(DB_CHA_TYPE,
						yypvt[-2].st_node->n_attr == NULL
						? yypvt[-2].st_node->n_name : yypvt[-2].st_node->n_attr),
					osvarref(yypvt[-0].st_node), byref
				);
			}
		}
		break;
	case 214:
#line 2900 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			register OSNODE	*node;
			register OSSYM	*var;

			u_ptr[0].u_symp = (OSSYM *)NULL;
			u_ptr[1].u_nodep = yypvt[-1].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			node = osmknode(tkEXEPROC, &u_ptr[0], &u_ptr[1], 
								&u_ptr[2]);
			if ( yypvt[-3].st_node == NULL 
			  || osiserr(yypvt[-3].st_node) || yypvt[-3].st_node->n_flags & N_READONLY
			   )
			{
				var = (OSSYM *)NULL;
			}
			else if ( !oschktypes(node->n_type, yypvt[-3].st_node->n_type) )
			{
				oscerr(OSASNTYPES, 0);
				var = (OSSYM *)NULL;
			}
			else
			{
				var = osnodesym(yypvt[-3].st_node);
			}
			osgencall(node, var);
			if (var != NULL)
				osvardisplay(yypvt[-3].st_node);
                        FormObj = FormSym;
			ostrfree(yypvt[-3].st_node);
			ostrfree(node);
		}
		break;
	case 215:
#line 2974 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			register OSNODE	*node;
			char	*proc_name;
              
      			if (yypvt[-1].st_node->n_token == tkID)
			{
				CVlower(yypvt[-1].st_node->n_value);
				proc_name = yypvt[-1].st_node->n_value;
				u_ptr[0].u_symp = osprocsym(proc_name, TRUE);
			}
			else
			{
				proc_name = NULL;
				u_ptr[0].u_symp = (OSSYM *)NULL;
			}
			u_ptr[1].u_nodep = yypvt[-1].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			node = osmknode(tkPCALL, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
			/*
			** If osmknode set node->n_token to something other
			** than tkPCALL, it's because it (osmknode) decided
			** that call_name represented a system (ADF) function.
			** This is an error:  CALLPROC may only be used
			** to call a procedure.
			*/
			if (node->n_token != tkPCALL)
			{
				oscerr( E_OS0267_CallProcSysFunc, 1,
					proc_name );
			}
			/* Type Check */
			else if (yypvt[-3].st_node == NULL)
			{
				if (node->n_type != DB_NODT && osCompat > 5
						&& osUser)
					oswarn(OSEXPRETVAL, 1, node->n_psym->s_name);
				/* build param structure and call procedure */
				osgencall(node, (OSSYM *)NULL);
			}
			else
			{
				register OSSYM	*var = osnodesym(yypvt[-3].st_node);

				if (node->n_type != DB_NODT)
				{
					node->n_psym->s_ref |= OS_OBJREF;
				}
				else
				{
					register OSSYM	*proc = node->n_psym;

					if ( proc != NULL )
					{
					    if ( proc->s_kind == OSUNDEF )
						proc->s_ref |= OS_OBJREF;
					    else
						oscerr( OSNORETVAL,
							1, proc->s_name
						);
					}
				}

				if (osiserr(yypvt[-3].st_node) || yypvt[-3].st_node->n_flags & N_READONLY)
				{
					var = NULL;
				}
				else if (node->n_type != DB_NODT
				  && !oschktypes(node->n_type, yypvt[-3].st_node->n_type) )
				{
					oscerr(OSASNTYPES, 0);
					var = NULL;
				}

				/* Generate code for procedure call */
				osgencall(node, var);

				if (var != NULL)
					osvardisplay(yypvt[-3].st_node);
				ostrfree(yypvt[-3].st_node);
			}
			FormObj = FormSym;
			ostrfree(node);
		}
		break;
	case 216:
#line 3059 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 217:
#line 3063 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 218:
#line 3067 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 219:
#line 3073 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-2].st_node;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(COMMA, (U_ARG *)NULL, &u_ptr[0], &u_ptr[1]);
		}
		break;
	case 220:
#line 3079 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			yyval.st_node = osmknode(COMMA, (U_ARG *)NULL, &u_ptr[0], &u_ptr[1]);
		}
		break;
	case 221:
#line 3086 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 222:
#line 3090 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 223:
#line 3096 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-2].st_node;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode( COMMA, (U_ARG*)NULL, &u_ptr[0],
					&u_ptr[1]
			);
		}
		break;
	case 224:
#line 3104 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 225:
#line 3109 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 226:
#line 3113 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( yypvt[-1].st_node->n_flags & N_READONLY )
			{
				oscerr(OSFNARG, 0);
				u_ptr[0].u_symp = ossymundef(ERx("*BYREF"),
						FormSym);
			}
			else
			{
				osmarkput(yypvt[-1].st_node);
				u_ptr[0].u_symp = osnodesym(yypvt[-1].st_node);
			}
			yyval.st_node = osmknode( tkBYREF, (U_ARG*)NULL, &u_ptr[0], (U_ARG*)NULL);
		}
		break;
	case 227:
#line 3128 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(DOTALL, &u_ptr[0],
					(U_ARG*)NULL, (U_ARG*)NULL);
		}
		break;
	case 228:
#line 3136 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-2].st_node;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode( COMMA, (U_ARG *)NULL, &u_ptr[0],
					&u_ptr[1]
			);
		}
		break;
	case 229:
#line 3144 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 230:
#line 3149 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			/*
			** Note that this rule is used for a CALLPROC.
			** We don't generate code to do an assign if r.h.s.
			** is of the form x.ALL; we simply ignore the l.h.s.
			** (as is currently done for x.ALL in a CALLFRAME;
			** perhaps we should verify that l.h.s. is the name
			** of the called frame or procedure for both CALLFRAME
			** and CALLPROC).
			*/
			if (yypvt[-0].st_node->n_token == DOTALL)
			{
				yyval.st_node = yypvt[-0].st_node;
				ostrfree(yypvt[-1].st_node);
			}
			else
			{
				/* save flag since 'osvarref()' frees node */
				bool	byref = yypvt[-0].st_node->n_token == tkBYREF;

				yyval.st_node = osmkassign(osvalref(yypvt[-1].st_node), osvarref(yypvt[-0].st_node),
						byref);
			}
		}
		break;
	case 231:
#line 3175 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkconst( tkSCONST, yypvt[-1].st_name );
		}
		break;
	case 232:
#line 3179 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkconst( tkSCONST, yypvt[-1].st_name );
		}
		break;
	case 233:
#line 3183 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (yypvt[-1].st_node->n_type != DB_NODT && !oschkstr(yypvt[-1].st_node->n_type))
				oscerr(OSNOTSTR, 1, ERget(_PrmKeyWord));
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 235:
#line 3208 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osqrygen( yypvt[-0].st_node, OS_QRYSTMT, !ProcState,
				  (IGSID *)NULL, (IGSID *)NULL );
			ostrfree(yypvt[-0].st_node);
		}
		break;
	case 236:
#line 3246 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGSID	*initcodeSID;

			DispLoopCount -= 1;

			_VOID_ IGpopSID(); 	/* discard MENULOOP SID */
			initcodeSID = IGpopSID();

			IGsetSID(IGpopSID());	/* BEGIN-DISPLAY SID */
			IGendMenu(IL_BEGDISPMU, initcodeSID);

			IGsetSID(IGpopSID());	/* end submenu SID */
			osendblock();		/* sub-menu */
			osendblock();		/* query */

			IGgenStmt(IL_POPSUBMU, (IGSID *)NULL, 0);
			IGsetSID(IGpopSID());	/* end query SID */
			ostrfree(yypvt[-4].st_node);
		}
		break;
	case 237:
#line 3285 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			register IGSID	*endwhileSID = IGpopSID();

			iiosQrNext(yypvt[-2].st_node, endwhileSID, (bool)FALSE);
			IGsetSID(IGpopSID() /* end query SID */);
			osendblock();
			ostrfree(yypvt[-2].st_node);

			QloopState = FALSE;
			OpErr = FALSE;
		}
		break;
	case 238:
#line 3297 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			register OSNODE *qry;

			u_ptr[0].u_symp = FormSym;
			u_ptr[1].u_nodep = osmknode( VALUE, &u_ptr[0],
						(U_ARG *)NULL, (U_ARG *)NULL );
			u_ptr[1].u_nodep->n_flags |= N_READONLY;
			u_ptr[0].u_p = TargListFrag;
			u_ptr[2].u_qrynodep = yypvt[-0].st_qry;
			qry = osmknode(tkQUERY,&u_ptr[0],&u_ptr[1],&u_ptr[2]);
			TargListFrag = NULL;
			osqrygen( qry, OS_QRYSTMT, !ProcState,
				  (IGSID *)NULL, (IGSID *)NULL );
			ostrfree(qry);
		}
		break;
	case 239:
#line 3327 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGSID		*qrySID;
			register OSNODE	*qry = osblkqry(&qrySID);

			IGstartStmt(osscnlno(), IL_LB_NONE);
			iiosQrNext( qry, (IGSID *)NULL,
				    (bool)(qry == NULL || SubMState > 1) );
		}
		break;
	case 240:
#line 3350 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			SubMState |= 1;
			DispLoopCount += 1;

			IGpushSID(IGinitSID());		/* end retrieve SID */
			osqrygen( yypvt[-0].st_node, OS_SUBMENU, !ProcState,
				  IGtopSID(), (IGSID *)NULL );
			IGpushSID(IGinitSID());		/* end submenu SID */
			osblock(LP_QUERY, IGtopSID(), QueryLabel, yypvt[-0].st_node);
			IGpushSID(IGinitSID());		/* menu SID */

			IGgenStmt(IL_GOTO, IGtopSID(), 0);
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 241:
#line 3366 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			QueryLabel = yypvt[-4].st_name;
			yyval.st_node = yypvt[-2].st_node;
		}
		break;
	case 242:
#line 3371 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			QueryLabel = NULL;
			yyval.st_node = yypvt[-2].st_node;
		}
		break;
	case 243:
#line 3395 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGsetSID(IGtopSID());		/* menu SID */
			IGstartStmt(osscnlno(), IL_LB_WHILE);
			IGgenStmt(IL_MENULOOP, (IGSID *)NULL, 0);
		}
		break;
	case 244:
#line 3401 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGpushSID(IGinitSID());		/* init code SID */
			IGsetSID(IGtopSID());
			IGstartStmt(osscnlno(), IL_LB_WHILE);

			IGpushSID(IGinitSID());		/* menu SID */
			IGbgnMenu();
			osblock(LP_DISPLAY, IGtopSID(), (char *) NULL);

			IGsetSID(IGtopSID());		/* menu SID */
			IGgenStmt(IL_MENULOOP, (IGSID *)NULL, 0);
		}
		break;
	case 245:
#line 3415 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGpushSID(IGinitSID());		/* init code SID */
			IGsetSID(IGtopSID());
			IGstartStmt(osscnlno(), IL_LB_WHILE);

			IGpushSID(IGinitSID());		/* menu SID */
			IGbgnMenu();
			osblock(LP_DISPLAY, IGtopSID(), (char *) NULL);
		}
		break;
	case 246:
#line 3439 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGSID	*endsid, *topsid;

			u_ptr[0].u_symp = FormSym;
			u_ptr[1].u_nodep = osmknode( VALUE, &u_ptr[0],
						(U_ARG *)NULL, (U_ARG *)NULL );
			u_ptr[1].u_nodep->n_flags |= N_READONLY;
			u_ptr[0].u_p = TargListFrag;
			u_ptr[2].u_qrynodep = yypvt[-0].st_qry;
			yyval.st_node = osmknode(tkQUERY, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
			TargListFrag = NULL;
			endsid = IGinitSID();		/* end query SID */
			IGpushSID(endsid);
			topsid = IGinitSID();		/* query loop SID */
			IGpushSID(topsid);
			osqrygen( yyval.st_node, OS_QRYLOOP, !ProcState,
				  endsid, topsid );
			osblock(LP_QWHILE, endsid, QueryLabel, yyval.st_node);

			QloopState = TRUE;
		}
		break;
	case 247:
#line 3462 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			QueryLabel = yypvt[-3].st_name;
			yyval.st_qry = yypvt[-1].st_qry;
		}
		break;
	case 248:
#line 3467 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			QueryLabel = NULL;
			yyval.st_qry = yypvt[-1].st_qry;
		}
		break;
	case 249:
#line 3485 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 250:
#line 3489 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yypvt[-1].st_node->n_child = yypvt[-0].st_node;
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 251:
#line 3502 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_p = TargListFrag;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_qrynodep = yypvt[-0].st_qry;
			yyval.st_node = osmknode(tkQUERY, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
			TargListFrag = NULL;
		}
		break;
	case 252:
#line 3511 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 253:
#line 3515 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yypvt[-1].st_node->n_child = yypvt[-0].st_node;
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 254:
#line 3522 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_p = TargListFrag;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_qrynodep = yypvt[-0].st_qry;
			yyval.st_node = osmknode(tkQUERY, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
			TargListFrag = NULL;
		}
		break;
	case 255:
#line 3538 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{RepeatState = TRUE;}
		break;
	case 256:
#line 3539 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yypvt[-0].st_qry->qn_repeat = RepeatState; /*may have been set FALSE*/
			yyval.st_qry = yypvt[-0].st_qry;

			FormObj = FormSym;
			RepeatState = FALSE;
		}
		break;
	case 257:
#line 3547 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yypvt[-0].st_qry->qn_repeat = FALSE;
			yyval.st_qry = yypvt[-0].st_qry;

			FormObj = FormSym;
		}
		break;
	case 258:
#line 3590 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			register IGSID	*initcodeSID;

			DispLoopCount -= 1;

			_VOID_ IGpopSID(); 	/* discard menuloop SID */
			initcodeSID = IGpopSID();

			IGsetSID(IGpopSID());	/* BEGINMENU SID */
			IGendMenu(IL_BEGDISPMU, initcodeSID);

			IGsetSID(IGpopSID());	/* end submenu SID */
			osendblock();		/* sub-menu */
			osendblock();		/* sub-menu end */

			IGgenStmt(IL_POPSUBMU, (IGSID *)NULL, 0);
		}
		break;
	case 259:
#line 3635 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGsetSID(IGpopSID());	/* menu SID */
			IGendMenu(IL_BEGSUBMU, IGpopSID());/* submenu top SID */
			IGsetSID(IGpopSID());	/* end submenu SID */
			osendblock();		/* sub-menu end */
		}
		break;
	case 260:
#line 3655 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			SubMState |= 1;
			DispLoopCount += 1;

			IGpushSID(IGinitSID());		/* end submenu SID */
			osblock(LP_DISPMENU, IGtopSID(), yypvt[-2].st_name);
			IGpushSID(IGinitSID());		/* menu SID */
			IGgenStmt(IL_GOTO, IGtopSID(), 0);
		}
		break;
	case 261:
#line 3674 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGpushSID(IGinitSID());		/* end submenu SID */
			osblock(LP_RUNMENU, IGtopSID(), yypvt[-2].st_name);
		}
		break;
	case 262:
#line 3681 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( SubMState > 0x7FFFFFFF )
			{
				SubMState &= 0x7FFFFFFF;
				oscerr( E_OS017C_SubmTooDeep, 0 );
			}
			SubMState	<<= 1;
			SubMInAlert	<<= 1;
			SubMGotNonAlert	<<= 1;
		}
		break;
	case 263:
#line 3694 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( ( SubMGotNonAlert & 1 ) == 0 )
			{
				oscerr( E_OS017F_MissingActs, 0 );
			}
			SubMState	>>= 1;
			SubMInAlert	>>= 1;
			SubMGotNonAlert	>>= 1;
		}
		break;
	case 264:
#line 3728 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			register IGSID	*menuSID;

			/* re-arrange the stack here. */
			menuSID = IGpopSID();		/* menu SID */

			IGpushSID(IGinitSID());		/* submenu loop SID */
			IGsetSID(IGtopSID());

			IGpushSID(menuSID);		/* re-push menu SID */
			IGgenStmt(IL_SUBMENU, (IGSID *)NULL, 0);
			IGbgnMenu();
		}
		break;
	case 265:
#line 3742 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			register IGSID	*menuSID;

			menuSID = IGinitSID();		/* menu SID */
			IGgenStmt(IL_GOTO, menuSID, 0);
			IGpushSID(IGinitSID());		/* submenu loop SID */
			IGsetSID(IGtopSID());
			IGpushSID(menuSID);		/* push menu SID */
			IGgenStmt(IL_SUBMENU, (IGSID *)NULL, 0);
			IGbgnMenu();
		}
		break;
	case 266:
#line 3755 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGpushSID(IGinitSID());		/* menu SID */
		}
		break;
	case 295:
#line 3816 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_CLRSCR, (IGSID *)NULL, 0);
		}
		break;
	case 296:
#line 3820 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_CLRALL, (IGSID *)NULL, 0);
		}
		break;
	case 297:
#line 3824 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osevallist(yypvt[-0].st_node, clrfld);
		}
		break;
	case 298:
#line 3829 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (ProcState)
				oscerr(OSINPROC, 1, yypvt[-0].st_name);
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 299:
#line 3849 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_HLPFILE, (IGSID *)NULL,
					2, osvalref(yypvt[-1].st_node), osvalref(yypvt[-0].st_node)
				);
		}
		break;
	case 300:
#line 3856 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 301:
#line 3875 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_HLPFORMS, (IGSID *)NULL,
				2, osvalref(yypvt[-5].st_node), osvalref(yypvt[-1].st_node)
				);
		}
		break;
	case 302:
#line 3882 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 303:
#line 3887 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (!osw_compare(_Subject, yypvt[-0].st_name))
				yyerror(ERx("SUBJECT"));
		}
		break;
	case 304:
#line 3905 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_MESSAGE, (IGSID *)NULL, 1, osvalref(yypvt[-1].st_node));
		}
		break;
	case 305:
#line 3910 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			FrsStmt = IIMESSAGE;
		}
		break;
	case 306:
#line 3932 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( !(yypvt[-5].st_node->n_flags & N_READONLY))
			{
				if (!oschkstr(yypvt[-5].st_node->n_type))
					oscerr(OSVARPSTR, 0);
				else
				{
					IGgenStmt(yypvt[-2].st_nat ? IL_NEPROMPT : IL_PROMPT,
						(IGSID*)NULL, 2,
						yypvt[-5].st_node->n_ilref, osvalref(yypvt[-1].st_node)
						);
					osvardisplay(yypvt[-5].st_node);
				}
			}
			else
			{
				ostrfree(yypvt[-1].st_node);
			}
			ostrfree(yypvt[-5].st_node);
			FormObj = FormSym;
		}
		break;
	case 307:
#line 3955 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			FrsStmt = IINPROMPT;
		}
		break;
	case 308:
#line 3960 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_nat = TRUE;
		}
		break;
	case 309:
#line 3964 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_nat = FALSE;
		}
		break;
	case 310:
#line 3981 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_PRNTSCR, (IGSID *)NULL, 1, osvalref(yypvt[-1].st_node));
		}
		break;
	case 311:
#line 3985 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_PRNTSCR, (IGSID *)NULL, 1, 0);
		}
		break;
	case 312:
#line 3990 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 313:
#line 3996 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (!osw_compare(_File, yypvt[-0].st_name))
				yyerror(ERx("FILE"));
		}
		break;
	case 314:
#line 4013 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (ProcState)
				oscerr(OSINPROC, 1, yypvt[-0].st_name);
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_REDISPLAY, (IGSID *)NULL, 0);
		}
		break;
	case 315:
#line 4047 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (DispLoopCount > 0)
			{
				_VOID_ osblkbreak(LP_DISPLAY, (char *)NULL);
			}
		}
		break;
	case 316:
#line 4054 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (DispLoopCount > 0)
			{
				_VOID_ osblkbreak(LP_DISPLAY, (char *)NULL);
			}
		}
		break;
	case 317:
#line 4062 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (DispLoopCount == 0)
			{
				oscerr(E_OS016E_NotInDisplayLoop, 1, yypvt[-0].st_name);
			}
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 318:
#line 4071 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			register char	*cp;

			if (yypvt[-1].st_node->n_token == tkID &&
				(cp = STindex(yypvt[-1].st_node->n_value, ERx("."), 0)) != NULL)
			{
				char	buf[OSBUFSIZE];
				char	*table;
				char	*column;

				STlcopy(yypvt[-1].st_node->n_value, buf, cp - yypvt[-1].st_node->n_value);
				table = iiIG_string(buf);
				STcopy(cp+1, buf);
				column = iiIG_string(buf);
				resumecolumn(table, osmkident(column, (OSNODE *)NULL));
			}
			else
			{
				if (yypvt[-1].st_node->n_token == tkID)
				{
					osfldcheck(FormSym, yypvt[-1].st_node->n_value);
				}
				IGgenStmt(IL_RESFLD, (IGSID *)NULL, 1, osvalref(yypvt[-1].st_node));
			}
		}
		break;
	case 319:
#line 4097 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			resumecolumn(yypvt[-3].st_name, yypvt[-1].st_node);
		}
		break;
	case 320:
#line 4101 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_RESUME, (IGSID *)NULL, 0);
		}
		break;
	case 321:
#line 4106 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (DispLoopCount == 0)
			{
				oscerr(E_OS016E_NotInDisplayLoop, 1, yypvt[-0].st_name);
			}
			IGgenStmt(IL_RESENTRY, (IGSID *)NULL, 0);
		}
		break;
	case 322:
#line 4114 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (DispLoopCount == 0)
			{
				oscerr(E_OS016E_NotInDisplayLoop, 1, yypvt[-0].st_name);
			}
			IGgenStmt(IL_RESNEXT, (IGSID *)NULL, 0);
		}
		break;
	case 323:
#line 4122 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (DispLoopCount == 0)
			{
				oscerr(E_OS016E_NotInDisplayLoop, 1, yypvt[-0].st_name);
			}
			IGgenStmt(IL_RESMENU, (IGSID *)NULL, 0);
		}
		break;
	case 324:
#line 4130 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (DispLoopCount == 0)
			{
				oscerr(E_OS016E_NotInDisplayLoop, 1, yypvt[-0].st_name);
			}
			IGgenStmt(IL_RESNFLD, (IGSID *)NULL, 0);
		}
		break;
	case 325:
#line 4138 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (DispLoopCount == 0)
			{
				oscerr(E_OS016E_NotInDisplayLoop, 1, yypvt[-0].st_name);
			}
			IGgenStmt(IL_RESPFLD, (IGSID *)NULL, 0);
		}
		break;
	case 326:
#line 4147 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			/*
			** RESUME with operands is not allowed
			** inside an alerter activation.
			*/
			if ( SubMInAlert & 1 )
			{
				oscerr( E_OS017E_BadResInAlert, 0 );
			}
		}
		break;
	case 327:
#line 4172 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( yypvt[-2].st_node == NULL )
				scroll( (OSNODE *)NULL, yypvt[-3].st_node, yypvt[-0].st_node );
			else
				scroll( yypvt[-3].st_node, yypvt[-2].st_node, yypvt[-0].st_node );
		}
		break;
	case 328:
#line 4179 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			char	*dir;
			bool	up;
			OSNODE	*row;
			char	buf[16];

			if (( yypvt[-0].st_name == NULL && yypvt[-1].st_node->n_token != tkID ) )
			{
				yyerror( ERx("TO, UP, DOWN") );
				ostrfree(yypvt[-2].st_node);
				ostrfree(yypvt[-1].st_node);
				break;
			}
			dir = ( yypvt[-0].st_name == NULL ) ? yypvt[-1].st_node->n_value : yypvt[-0].st_name;
			if ( !(up = osw_compare( ERx("up"), dir )) &&
				!osw_compare( ERx("down"), dir) )
			{
				yyerror( ERx("UP, DOWN") );
				ostrfree(yypvt[-2].st_node);
				ostrfree(yypvt[-1].st_node);
				break;
			}
			row = osmkconst( tkICONST,
				iiIG_string( STprintf(buf, ERx("%d"),
							up ? rowUP : rowDOWN
					)
				)
			);
			if ( yypvt[-0].st_name == NULL )
			{
				if ( ProcState )
					oscerr( OSINPROC, 1, 
						ERx("SCROLL <table field>") );
				scroll( (OSNODE *)NULL, yypvt[-2].st_node, row );
			}
			else
				scroll( yypvt[-2].st_node, yypvt[-1].st_node, row );
		}
		break;
	case 329:
#line 4219 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt( osscnlno(), IL_LB_NONE );
		}
		break;
	case 330:
#line 4224 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 331:
#line 4229 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 332:
#line 4233 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( ProcState )
				oscerr( OSINPROC, 1, ERx("SCROLL <table field>") );
			yyval.st_node = NULL;
		}
		break;
	case 333:
#line 4240 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 334:
#line 4244 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			char	buf[16];

			yyval.st_node = osmkconst( tkICONST,
				iiIG_string( STprintf(buf, ERx("%d"), rowEND) )
			);
		}
		break;
	case 335:
#line 4253 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 336:
#line 4257 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = NULL;
		}
		break;
	case 337:
#line 4273 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_SLEEP, (IGSID *)NULL, 1, osvalref(yypvt[-0].st_node));
		}
		break;
	case 338:
#line 4278 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 339:
#line 4300 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGSID	*xSID;

			IGgenStmt(IL_VALIDATE, (xSID = IGinitSID()), 0);
			if (DispLoopCount > 0)
			{
				_VOID_ osblkbreak(LP_DISPLAY, (char *)NULL);
			}
			IGsetSID(xSID);
		}
		break;
	case 340:
#line 4311 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGSID	*xSID;

			if (yypvt[-0].st_node->n_token == tkID)
				osfldcheck(FormSym, yypvt[-0].st_node->n_value);
			IGgenStmt(IL_VALFLD, (xSID = IGinitSID()), 1, osvalref(yypvt[-0].st_node));
			if (DispLoopCount > 0)
			{
				_VOID_ osblkbreak(LP_DISPLAY, (char *)NULL);
			}
			IGsetSID(xSID);
		}
		break;
	case 341:
#line 4325 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (ProcState)
			{
				oscerr(OSINPROC, 1, yypvt[-0].st_name);
			}
			if (DispLoopCount == 0)
			{
				oscerr(E_OS016E_NotInDisplayLoop, 1, yypvt[-0].st_name);
			}
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 342:
#line 4349 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osvalnode(FormSym, yypvt[-1].st_name);
			if (yypvt[-0].st_node != NULL)
				yyval.st_node = osarraynode(yyval.st_node, yypvt[-0].st_node, TRUE);
			yyval.st_node = os_ta_check(FormSym, yyval.st_node);
			if (!osiserr(yyval.st_node))
				TableObj = osnodesym(yyval.st_node);
		}
		break;
	case 343:
#line 4358 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{ /* variable with a ':' */
			yyval.st_node = yypvt[-0].st_node;
			if (!oschkstr(yypvt[-0].st_node->n_type))
				oscerr(OSNOTSTR, 1, ERget(_TblFldName));
			else if ( AFE_NULLABLE_MACRO(yypvt[-0].st_node->n_type) )
				oswarn(OSNULVCHK, 0);
		}
		break;
	case 344:
#line 4365 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{AllowForm = FALSE;}
		break;
	case 345:
#line 4366 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{ /* variable - could be tblfld or array */
			yyval.st_node = os_ta_check(FormSym, yypvt[-0].st_node);
			if (!osiserr(yyval.st_node))
				TableObj = osnodesym(yyval.st_node);
		}
		break;
	case 346:
#line 4374 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (yypvt[-0].st_node->n_token == tkID && TableObj != NULL)
			{
				if (ossympeek(yypvt[-0].st_node->n_value, TableObj) != NULL)
				{
				     /* Hack Put back  - bug 79308 */

                     if (TableObj->s_type == DB_DMY_TYPE)
                     {
                         /*  HACK ATTACK - must be lowercase
                         **  in our string constants table.
                         */

                         if ( !is_state(yypvt[-0].st_node->n_value) )
                         {
                             CVlower(yypvt[-0].st_node->n_value);
                         }

                     }
				}
				else if (TableObj->s_type == DB_DMY_TYPE)
				{
					oscerr( OSNOTMEM,
						2, yypvt[-0].st_node->n_value, TableObj->s_name
					);
					yypvt[-0].st_node->n_flags |= N_ERROR;
				}
				else
				{
					/* tablefield, but no such column. */
					oscerr( OSNOCOL,
						3, TableObj->s_parent->s_name,
							TableObj->s_name,
							yypvt[-0].st_node->n_value
					);
					yypvt[-0].st_node->n_flags |= N_ERROR;
				}

			}
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 347:
#line 4433 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osrowops(FormSym, (IGSID*)NULL, IL_CLRROW, yypvt[-1].st_node);
			if (yypvt[-0].st_node != NULL)
				osevallist(yypvt[-0].st_node, ostl1elm);
			IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
			TableObj = NULL;
		}
		break;
	case 348:
#line 4442 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 349:
#line 4447 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 350:
#line 4451 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 351:
#line 4456 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 352:
#line 4463 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 353:
#line 4483 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osrowops(FormSym, (IGSID*)NULL, IL_DELROW, yypvt[-0].st_node);
			TableObj = NULL;
		}
		break;
	case 354:
#line 4489 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 355:
#line 4506 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (yypvt[-0].st_node == NULL)
			{
				oscerr(E_OS0270_NoPurgeTableName, 0);
			}
			if (yypvt[-0].st_node->n_token == tkID && (yypvt[-1].st_node == NULL ||
				(yypvt[-1].st_node->n_token == tkID &&
				  STbcompare(FormSym->s_name, 0, yypvt[-1].st_node->n_value,
				  0, TRUE) == 0)))
			{
				(void) ostblcheck(FormSym, yypvt[-0].st_node->n_value);
			}
			IGgenStmt(IL_PURGETBL, (IGSID *) NULL, 2,
				yypvt[-1].st_node == NULL ? 0 : osvalref(yypvt[-1].st_node), osvalref(yypvt[-0].st_node));

		}
		break;
	case 356:
#line 4524 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 357:
#line 4537 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (osiserr(yypvt[-2].st_node))
			{
				; /* We've already output an error */
			}
			else if (  (yypvt[-2].st_node->n_token == TABLE && yypvt[-2].st_node->n_flags & N_ROW)
				|| (yypvt[-2].st_node->n_token == VALUE && yypvt[-2].st_node->n_sexpr != NULL)
				|| (yypvt[-2].st_node->n_token == ARRAYREF) )
			{
			    	oscerr(OSLDTBLIDX, 0);
			}
			else if (yypvt[-1].st_node == NULL)
			{
				/* LOADTABLE must have a list */
			    	oscerr(OSLDTBLEMP, 0);
			}
			else
			{
				if (yypvt[-0].st_node == NULL)
				{
				    osrowops(FormSym, (IGSID*)NULL, 
					IL_LOADTABLE, yypvt[-2].st_node);
				    osevallist(yypvt[-1].st_node, ostl2elm);
				    IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
				}
				else
				{
				    osrowops(FormSym, (IGSID*)NULL, 
					IL_LOADTABLE|ILM_HAS_CELL_ATTR, yypvt[-2].st_node);
				    osevallist(yypvt[-1].st_node, ostl2elm);
				    IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
				    osevallist(yypvt[-0].st_node, frs_gen);
				    IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
				}
				TableObj = NULL;
			}
		}
		break;
	case 358:
#line 4576 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (ProcState)			/* Fix bug #47930 */
				oscerr(OSINPROC, 1, yypvt[-0].st_name);
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 359:
#line 4592 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (yypvt[-0].st_node == NULL)
			{
			    osrowops(FormSym, (IGSID*)NULL, IL_INSROW, yypvt[-2].st_node);
			    if (yypvt[-1].st_node != NULL)
				osevallist(yypvt[-1].st_node, ostl2elm);
			    IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
			}
			else
			{
			    osrowops(FormSym, (IGSID*)NULL, 
				IL_INSROW|ILM_HAS_CELL_ATTR, yypvt[-2].st_node);
			    if (yypvt[-1].st_node != NULL)
				osevallist(yypvt[-1].st_node, ostl2elm);
			    IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
			    osevallist(yypvt[-0].st_node, frs_gen);
			    IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
			}
			TableObj = NULL;
		}
		break;
	case 360:
#line 4614 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 361:
#line 4619 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 362:
#line 4623 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 363:
#line 4628 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			frs_insrow_with();
		}
		break;
	case 364:
#line 4633 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 365:
#line 4637 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 366:
#line 4642 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 367:
#line 4649 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 368:
#line 4657 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( yypvt[-2].st_node->n_token == tkID && TableObj != NULL
				&& !osiserr(yypvt[-2].st_node) )
			{
				OSSYM 	*sym = ossympeek(yypvt[-2].st_node->n_value, TableObj);

				if (yypvt[-2].st_node->n_flags & N_READONLY)
				{
					oscerr(OSASNOVAR, 1, osnodename(yypvt[-2].st_node));
				}
				else if ( sym != NULL )
				{
					if (!oschktypes(yypvt[-0].st_node->n_type,sym->s_type))
					{
						oscerr(OSASNTYPES, 0);
					}
					else if ( AFE_NULLABLE_MACRO(yypvt[-0].st_node->n_type)
					  && !AFE_NULLABLE_MACRO(sym->s_type) )
					{
						if (yypvt[-0].st_node->n_token == tkNULL)
							oscerr(OSNULLEXPR, 0);
						else
							oswarn(OSNULLCHK, 0);
					}
				}
			}
			yyval.st_node = osmkassign(osvalref(yypvt[-2].st_node), osvarref(yypvt[-0].st_node), FALSE);
		}
		break;
	case 369:
#line 4705 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGSID	*xSID = IGinitSID();;

			osrowops(FormSym, xSID, IL_VALROW, yypvt[-1].st_node);
			if (yypvt[-0].st_node != NULL)
				osevallist(yypvt[-0].st_node, ostl1elm);
			IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
			if (DispLoopCount > 0)
			{
				_VOID_ osblkbreak(LP_DISPLAY, (char *)NULL);
			}
			IGsetSID(xSID);
			TableObj = NULL;
		}
		break;
	case 370:
#line 4721 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (ProcState)
			{
				oscerr(OSINPROC, 1, yypvt[-0].st_name);
			}
			if (DispLoopCount == 0)
			{
				oscerr(E_OS016E_NotInDisplayLoop, 1, yypvt[-0].st_name);
			}
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 371:
#line 4757 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			register char *table;
			ILREF	ref1, ref2;

			/* save table name since 'osvalref()' frees node */
			table = (yypvt[-1].st_node->n_token == tkID) ? yypvt[-1].st_node->n_value : NULL;
			ref1 = osvalref(yypvt[-1].st_node);
			if (yypvt[-0].st_node == NULL)
				ref2 = IGsetConst(DB_CHA_TYPE, ERx("f"));
			else
				ref2 = osvalref(yypvt[-0].st_node);
			IGgenStmt(IL_INITTAB, (IGSID *)NULL, 2, ref1, ref2);
			if (table != NULL)
				oshidcol(FormSym, table);
			IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
		}
		break;
	case 372:
#line 4775 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (yypvt[-0].st_node->n_token == tkID)
				_VOID_ ostblcheck(FormSym, yypvt[-0].st_node->n_value);
			else
				oswarn(OSTABINIT, 0);
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 373:
#line 4784 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (ProcState)
				oscerr(OSINPROC, 1, yypvt[-0].st_name);
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 374:
#line 4791 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (yypvt[-0].st_node != NULL && yypvt[-0].st_node->n_token == tkID)
				yypvt[-0].st_node->n_value =
				 iiIG_string(frsck_mode(ERx("INITTABLE"), yypvt[-0].st_node->n_value));
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 375:
#line 4798 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 376:
#line 4810 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (yypvt[-3].st_sym != NULL)
			{
				osclrunld(yypvt[-3].st_sym);
				IGgenStmt(IL_GOTO, IGpopSID() /*unload SID*/,0);
			}
			IGsetSID(IGpopSID());	/* end SID */
			osendblock();
		}
		break;
	case 377:
#line 4821 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGSID	*endSID = IGtopSID();
			OSSYM *sym = NULL;

			if ( !osiserr(yypvt[-1].st_node) )
			{
				OSSYM	*osunload();

				/* There's an IGpushSID hidden in here. */
				sym = osunload(TableObj, endSID, yypvt[-1].st_node);
			}
			if (sym != NULL)
				osgenunldcols(sym);
			osevallist(yypvt[-0].st_node, ostl2elm);
			IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
			osgenputs();
			yyval.st_sym = sym;
			TableObj = NULL;
		}
		break;
	case 378:
#line 4842 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (osblkutbl())
				oscerr(OSNESTUNLD, 0);
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGpushSID(IGinitSID());	/* end SID () */
			osblock(LP_UNLOADTBL, IGtopSID(), yypvt[-1].st_name);
		}
		break;
	case 379:
#line 4851 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 380:
#line 4855 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 381:
#line 4860 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 382:
#line 4867 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 383:
#line 4875 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( !osiserr(yypvt[-0].st_node)
			  && yypvt[-0].st_node->n_token == tkID
			  && TableObj != NULL
			  && !osiserr(yypvt[-2].st_node) )
			{
				OSSYM 	*sym = ossympeek(yypvt[-0].st_node->n_value, TableObj);

				if ( sym != NULL )
				{
					if (!oschktypes(sym->s_type,yypvt[-2].st_node->n_type))
						oscerr(OSASNTYPES, 0);
					else if (AFE_NULLABLE_MACRO(sym->s_type)
					    && !AFE_NULLABLE_MACRO(yypvt[-2].st_node->n_type))
						oswarn(OSNULLCHK, 0);
				}
			}
			osmarkput(yypvt[-2].st_node);
			u_ptr[1].u_symp = osnodesym(yypvt[-2].st_node);
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(tkASSIGN, (U_ARG*)NULL, &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 384:
#line 4915 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			frs_object(yypvt[-0].st_name);
		}
		break;
	case 385:
#line 4919 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			frs_object(yypvt[-0].st_name);
		}
		break;
	case 386:
#line 4923 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			frs_object(yypvt[-0].st_name);
		}
		break;
	case 389:
#line 4931 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (yypvt[-0].st_node->n_token == tkID)
				frs_parentname(yypvt[-0].st_node->n_value, T_CHAR, (PTR)NULL);
			else if ( yypvt[-0].st_node->n_token == tkICONST )
			{ /* should be row number */
				frs_parentname( yypvt[-0].st_node->n_const, T_INT, (PTR)NULL );
			}
			else
			{ /* variable reference */
				frs_parentname(ERx(""), osfrs_basetype(yypvt[-0].st_node->n_type),
					(PTR)osnodesym(yypvt[-0].st_node)
				);
			}
			ostrfree(yypvt[-0].st_node);
		}
		break;
	case 390:
#line 4948 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osfrs_constant(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 391:
#line 4952 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osfrs_constant(yypvt[-1].st_name, (OSNODE *)NULL);
		}
		break;
	case 392:
#line 4956 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osfrs_constant(yypvt[-2].st_name, yypvt[-1].st_node);
		}
		break;
	case 393:
#line 4962 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (osfrs_old())
				yyerror(_SyntaxError);
			yyval.st_name = yypvt[-1].st_name;
		}
		break;
	case 394:
#line 4969 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 395:
#line 4973 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 396:
#line 4977 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 397:
#line 4981 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 398:
#line 4985 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 399:
#line 4989 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 400:
#line 4994 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 401:
#line 4998 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkconst( tkSCONST, yypvt[-0].st_name );
		}
		break;
	case 402:
#line 5002 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkconst( tkSCONST, yypvt[-0].st_name );
		}
		break;
	case 403:
#line 5006 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkconst( tkSCONST, yypvt[-0].st_name );
		}
		break;
	case 404:
#line 5024 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (yypvt[-0].st_node != NULL && yypvt[-0].st_node->n_token == tkID)
				yypvt[-0].st_node->n_value =
				  iiIG_string(frsck_mode(ERx("MODE"), yypvt[-0].st_node->n_value));
			IGgenStmt(IL_MODE, (IGSID *)NULL, 1, osvalref(yypvt[-0].st_node));
		}
		break;
	case 405:
#line 5032 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 406:
#line 5037 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 407:
#line 5041 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;	/* special case */
		}
		break;
	case 408:
#line 5061 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			frs_head();
			osevallist(yypvt[-1].st_node, frs_gen);
			IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
		}
		break;
	case 409:
#line 5068 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			frs_inqset(FRSinq);
		}
		break;
	case 410:
#line 5074 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			frs_style();	/* recognize style semantically */
		}
		break;
	case 411:
#line 5079 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (yypvt[-0].st_node != NULL)
			{
				u_ptr[0].u_nodep = yypvt[-0].st_node;
				u_ptr[1].u_nodep = NULL;
				u_ptr[2].u_nodep = NULL;
				yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
			}
			else
			{
				yyval.st_node = NULL;
			}
		}
		break;
	case 412:
#line 5093 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (yypvt[-0].st_node != NULL)
			{
				u_ptr[0].u_nodep = yypvt[-0].st_node;
				u_ptr[1].u_nodep = yypvt[-2].st_node;
				u_ptr[2].u_nodep = NULL;
				yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
			}
			else
			{
				yyval.st_node = yypvt[-2].st_node;
			}
		}
		break;
	case 413:
#line 5108 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			OSNODE 	*n = NULL;

			if ( yypvt[-2].st_sym != NULL && yypvt[-0].st_node != NULL )
			{
				if ( yypvt[-0].st_node->n_token == FRSCONST )
                		{ /* `new' style */
					yypvt[-0].st_node->n_inqvar = yypvt[-2].st_sym;
					n = yypvt[-0].st_node;
				}
				else
				{ /* `old' style */
					n = osmkassign( yypvt[-2].st_sym->s_ilref,
							osvalref(yypvt[-0].st_node), FALSE
					);
				}
			}
			yyval.st_node = n;
		}
		break;
	case 414:
#line 5129 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			OSSYM 	*sym = osnodesym(yypvt[-0].st_node);

			frs_iqset(sym);
			osmarkput(yypvt[-0].st_node);

			if (osiserr(yypvt[-0].st_node))
				yyval.st_sym = NULL;
			else
				yyval.st_sym = sym;
		}
		break;
	case 415:
#line 5156 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			frs_head();
			osevallist(yypvt[-1].st_node, frs_gen);
			IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
		}
		break;
	case 416:
#line 5163 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			frs_inqset(FRSset);
		}
		break;
	case 417:
#line 5169 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			frs_style();	/* recognize style semantically */
		}
		break;
	case 418:
#line 5174 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 419:
#line 5181 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 420:
#line 5189 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (yypvt[-2].st_node == NULL)
				ostrfree(yypvt[-0].st_node);
			else
			{
				if (yypvt[-2].st_node->n_token == FRSCONST)
				{ /* `new' style */
					yypvt[-2].st_node->n_setref = osfrs_setval(yypvt[-0].st_node);
				}
				else
				{ /* `old' style */
					yypvt[-2].st_node = osmkassign(osvalref(yypvt[-2].st_node), osvarref(yypvt[-0].st_node), FALSE);
				}
			}
			yyval.st_node = yypvt[-2].st_node;
		}
		break;
	case 421:
#line 5213 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_INQING, (IGSID *)NULL, 0);
			osevallist(yypvt[-1].st_node, ostl2elm);
			IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
		}
		break;
	case 422:
#line 5220 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 423:
#line 5225 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 424:
#line 5232 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 425:
#line 5240 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (yypvt[-2].st_node->n_flags & N_READONLY)
			{
				yyval.st_node = NULL;
			}
			else
			{
				osmarkput(yypvt[-2].st_node);
				u_ptr[1].u_symp = osnodesym(yypvt[-2].st_node);
				u_ptr[2].u_nodep = osmkconst( tkSCONST, yypvt[-0].st_name );
				yyval.st_node = osmknode(tkASSIGN, (U_ARG*)NULL,
						&u_ptr[1], &u_ptr[2]
				);
			}
		}
		break;
	case 426:
#line 5264 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_SETING, (IGSID *)NULL, 0);
			osevallist(yypvt[-1].st_node, ostl2elm);
			IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
			if (SetSession)
			{
				IGgenStmt(IL_CHKCONNECT, (IGSID *)NULL, 0);
			}
		}
		break;
	case 427:
#line 5275 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			SetSession = FALSE;
		}
		break;
	case 428:
#line 5281 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 429:
#line 5288 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 430:
#line 5296 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkassign( IGsetConst(DB_CHA_TYPE, yypvt[-2].st_name),
						osvarref(yypvt[-0].st_node), FALSE
			);
		}
		break;
	case 431:
#line 5303 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 432:
#line 5307 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			SetSession = TRUE;
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 433:
#line 5314 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 434:
#line 5318 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 435:
#line 5324 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 436:
#line 5328 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 437:
#line 5334 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkconst(tkICONST, ERx("1"));
		}
		break;
	case 438:
#line 5338 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkconst(tkICONST, ERx("0"));
		}
		break;
	case 439:
#line 5342 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
                     	yyval.st_node = osmkconst(tkICONST, ERx("2"));
		}
		break;
	case 440:
#line 5356 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		IGgenStmt(IL_SETING, (IGSID *)NULL, 0);
		u_ptr[0].u_nodep = yypvt[-0].st_node;
		u_ptr[1].u_nodep = NULL;
		u_ptr[2].u_nodep = NULL;
		u_ptr[0].u_nodep = osmknode(NLIST, &u_ptr[0], 
						&u_ptr[1], &u_ptr[2]);
		osevallist(u_ptr[0].u_nodep, ostl2elm); 
		IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
		IGgenStmt(IL_CHKCONNECT, (IGSID *)NULL, 0);
	}
		break;
	case 441:
#line 5369 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		IGstartStmt(osscnlno(), IL_LB_NONE);
	}
		break;
	case 442:
#line 5375 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		yyval.st_node = osmkassign( IGsetConst(DB_CHA_TYPE, 
				ERx("connection_name")), osvarref(yypvt[-0].st_node), FALSE);
	}
		break;
	case 443:
#line 5390 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_INQ4GL, (IGSID *)NULL, 0);
			osevallist(yypvt[-1].st_node, ostl2elm);
			IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
		}
		break;
	case 444:
#line 5397 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 445:
#line 5412 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_SET4GL, (IGSID *)NULL, 0);
			osevallist(yypvt[-1].st_node, ostl2elm);
			IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
		}
		break;
	case 446:
#line 5419 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 447:
#line 5436 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(QUEL ? IL_SET : IL_SETSQL, (IGSID *)NULL, 0);
			osdbstr(ERx("random_seed"));
			osdbflush();
			IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_SET_RANDOM, (IGSID *)NULL, 1, 0);
		}
		break;
	case 448:
#line 5445 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (tkIS_CONST_MACRO(yypvt[-0].st_node->n_token)) {
				IGgenStmt(QUEL ? IL_SET : IL_SETSQL, (IGSID *)NULL, 0);
				osdbstr(ERx("random_seed "));
				/*
				** We could do 'osdbeval($2);' to achieve what we want
				** except it will quietly free the $2 expr that we need
				** for the IL_SET_RANDOM.
				*/
				osdbstr(yypvt[-0].st_node->n_const);
				osdbflush();
				IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
				IGstartStmt(osscnlno(), IL_LB_NONE);
			}
			IGgenStmt(IL_SET_RANDOM, (IGSID *)NULL, 1, osvalref(yypvt[-0].st_node));
		}
		break;
	case 449:
#line 5463 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 450:
#line 5482 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{_VOID_ EQFWopen( FrsStmt );}
		break;
	case 451:
#line 5483 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			frs_optgen();
		}
		break;
	case 454:
#line 5489 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{yyerror(_SyntaxError);}
		break;
	case 457:
#line 5494 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			EQFWwcoption(yypvt[-2].st_name, yypvt[-0].st_name);
		}
		break;
	case 462:
#line 5505 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( yypvt[-0].st_node->n_token == tkSCONST ||
					yypvt[-0].st_node->n_token == tkICONST ||
					yypvt[-0].st_node->n_token == tkFCONST ||
					yypvt[-0].st_node->n_token == tkDCONST )
			{ /* a constant */
				EQFWsuboption( yypvt[-2].st_name, yypvt[-0].st_node->n_type, yypvt[-0].st_node->n_const, (PTR)NULL );
			}
			else if ( yypvt[-0].st_node->n_token == tkID )
			{ /* an ID constant */
				EQFWsuboption( yypvt[-2].st_name, DB_CHR_TYPE, yypvt[-0].st_node->n_value, (PTR)NULL );
			}
			else
			{ /* a variable reference */
				OSSYM *sym = osnodesym(yypvt[-0].st_node);

				if ( AFE_NULLABLE_MACRO(yypvt[-0].st_node->n_type) )
					oswarn(OSNULVCHK, 0);
				EQFWsuboption( yypvt[-2].st_name,
					AFE_NULLABLE_MACRO(yypvt[-0].st_node->n_type)
						? ( - yypvt[-0].st_node->n_type ) : yypvt[-0].st_node->n_type,
						sym->s_name, (PTR)sym
				);
			}
		}
		break;
	case 463:
#line 5531 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
				EQFWsuboption( yypvt[-2].st_name, DB_CHR_TYPE, yypvt[-0].st_name, (PTR)NULL );
			}
		break;
	case 464:
#line 5550 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_qry = osmkqry(DB_QUEL, osmkquery(tkQUERY,
							yypvt[-5].st_nat ? ERx("unique"):NULL,
							(PTR)yypvt[-3].st_tlist, (PTR)yypvt[-1].st_node,
							(OSNODE *)NULL
						),
					yypvt[-0].st_srtlist);
		}
		break;
	case 465:
#line 5560 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( !CallState && FormObj == NULL )
			{ /* Bug check:  (for JupBug #9133.) */
				oscerr(OSBUG, 1, ERx("quel(missing query object)"));
			}
			else if ( FormObj != NULL  &&  FormObj != FormSym
				&&  FormObj->s_kind != OSTABLE
				&&  ( FormObj->s_kind != OSVAR
				  || (FormObj->s_flags & (FDF_RECORD|FDF_ARRAY))
						== 0 ) )
			{
				if ( FormObj->s_kind == OSUNDEF )
					oscerr( OSNOQRYOBJ, 2, FormObj->s_name,
						FormSym->s_name
					);
				else if ( FormObj->s_parent->s_kind != OSFORM )
					oscerr( OSNOTCUR, 2,
						FormObj->s_parent->s_name,
						FormSym->s_name
					);
				else
					oscerr( OSNOQRYOBJ, 2, FormObj->s_name,
						FormObj->s_parent->s_name
					);

				FormObj = NULL;
			}
			QryState = TRUE;
		}
		break;
	case 466:
#line 5603 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = yypvt[-0].st_tlist;
			QryState = FALSE;
		}
		break;
	case 467:
#line 5613 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_tlist = yypvt[-0].st_tlist;
		}
		break;
	case 468:
#line 5617 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_tlist = osaddtlist(yypvt[-2].st_tlist, yypvt[-0].st_tlist);
		}
		break;
	case 469:
#line 5622 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_tlist = (yypvt[-2].st_node != NULL) ? osmaketle(yypvt[-2].st_node, yypvt[-0].st_node, (OSNODE*)NULL) : NULL;
		}
		break;
	case 470:
#line 5626 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    register OSNODE	*symn;
		    register bool	all;

		    all = osw_compare(_All, yypvt[-0].st_name);
		    if (QryState)
		    {
			if (yypvt[-2].st_node != NULL)
			    oscerr(OSQRYTFALL, 1, ERx(".all"));
			if (all)
			{ /* generate target list */
			    if ((yyval.st_tlist = ostlqryall(QryTarget, FormObj, yypvt[-3].st_name)) == NULL)
				oscerr(OSBADQRYALL, 2, ERx(".all"),
						FormSym->s_name
				);
			}
			else
			{
			    register OSNODE	*attr;

			    u_ptr[0].u_cp = yypvt[-3].st_name;
			    u_ptr[1].u_cp = yypvt[-0].st_name;
			    attr =
			     osmknode(ATTR, &u_ptr[0], &u_ptr[1], (U_ARG*)NULL);
			    if (FormObj == NULL)
			    {
				yyval.st_tlist =
				  osmaketle(attr, (OSNODE*)NULL, (OSNODE*)NULL);
			    }
			    else
			    {
				u_ptr[0].u_symp = osqryidchk(FormObj, yypvt[-0].st_name);
				yyval.st_tlist = osmaketle( osmknode(VALUE, &u_ptr[0],
						    (U_ARG*)NULL, (U_ARG*)NULL
						),
						attr, (OSNODE *)NULL
				);
			    }
			}
		    }
		    else
		    { /* non-retrieve DML QUEL statement. */
			if (all && RetInState && yypvt[-2].st_node == NULL)
			{ /* retrieve into */
			    u_ptr[0].u_cp = yypvt[-3].st_name;
			    u_ptr[1].u_cp = yypvt[-0].st_name;
			    yyval.st_tlist = osmaketle( osmknode(ATTR, &u_ptr[0], &u_ptr[1],
							(U_ARG*)NULL
					    ),
				  	    (OSNODE *)NULL, (OSNODE *)NULL
			    );
			}
			else if (yypvt[-2].st_node != NULL || ossymlook(yypvt[-3].st_name, FormSym) != NULL)
			{ /* assume it's a table field */
			    if (all)
			    {	/* generate target list and get column values */
				yyval.st_tlist = ostlall(ostabfall(FormSym, yypvt[-3].st_name, yypvt[-2].st_node));
			    }
			    else
			    {
				symn = ostvalnode(FormSym, yypvt[-3].st_name,(OSNODE*)NULL,yypvt[-0].st_name);
				if (symn->n_tfref->s_parent->s_kind == OSTABLE)
				    ostfval(symn);
			    }
			}
			else if (STbcompare(FormSym->s_name,0, yypvt[-3].st_name,0, TRUE) == 0)
			{ /* form . field reference */
			    if (all)
			    {	/* generate target list and get values */
				if ((yyval.st_tlist = ostlall(osformall(FormSym))) == NULL)
				    oscerr(OSBADQRYALL, 2, ERx(".all"),
						FormSym->s_name
				    );
			    }
			    else
			    {
				symn = osvalnode(FormSym, yypvt[-0].st_name);
				if (symn->n_token == VALUE &&
					(symn->n_sym->s_kind == OSFIELD ||
						symn->n_sym->s_kind == OSVAR) )
				    osfldval(symn);
			    }
			}
			else
			{ /* assume it's a relation (or range) */
			    u_ptr[0].u_cp = yypvt[-3].st_name;
			    u_ptr[1].u_cp = yypvt[-0].st_name;
			    symn = osmknode(ATTR, &u_ptr[0], &u_ptr[1],
					(U_ARG*)NULL
				);
			    if (all)
				yyval.st_tlist =
				  osmaketle(symn, (OSNODE*)NULL, (OSNODE*)NULL);
			}
			if (!all)
			{
			    u_ptr[0].u_cp = yypvt[-0].st_name;
			    yyval.st_tlist = osmaketle(osmkident(yypvt[-0].st_name, (OSNODE *) NULL), symn,
					(OSNODE *)NULL
				);
			}
		    } /* end non-retrieve DML target list element */
		}
		break;
	case 471:
#line 5731 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    if (!QryState)
		    {
			yyval.st_node = yypvt[-0].st_node;
		    }
		    else
		    { /* target list in query retrieve */
			if (yypvt[-0].st_node->n_token == VALUE)
			{
			    oscerr(OSDYNQRY, 0);
			    yyval.st_node = yypvt[-0].st_node;
			}
			else
			{
			    if (FormObj == NULL)
			    {
				yyval.st_node = yypvt[-0].st_node;
			    }
			    else
			    {
				u_ptr[0].u_symp =
					    osqryidchk(FormObj, yypvt[-0].st_node->n_value);
				yyval.st_node = osmknode(VALUE, &u_ptr[0],
					    (U_ARG *)NULL, (U_ARG *)NULL
				);
				ostrfree(yypvt[-0].st_node);
			    }
			}
		    }
		}
		break;
	case 472:
#line 5763 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 473:
#line 5767 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 474:
#line 5776 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 475:
#line 5780 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    if (yypvt[-1].st_node->n_type != DB_NODT && !oschkstr(yypvt[-1].st_node->n_type))
			oscerr(OSNOTSTRX, 0);
		    yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 476:
#line 5786 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_node = NULL;
		}
		break;
	case 477:
#line 5795 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yypvt[-0].st_srtlist->srt_type = SRT_SORT;
		    yyval.st_srtlist = yypvt[-0].st_srtlist;
		}
		break;
	case 478:
#line 5800 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yypvt[-0].st_srtlist->srt_type = SRT_ORDER;
		    yyval.st_srtlist = yypvt[-0].st_srtlist;
		}
		break;
	case 479:
#line 5805 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_srtlist = NULL;
		}
		break;
	case 482:
#line 5814 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_srtlist = yypvt[-0].st_srtlist;
		}
		break;
	case 483:
#line 5818 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    /* Maintain sort list as circular linked list */
		    yypvt[-0].st_srtlist->srt_next = yypvt[-2].st_srtlist->srt_next;
		    yypvt[-2].st_srtlist->srt_next = yypvt[-0].st_srtlist;
		    yyval.st_srtlist = yypvt[-0].st_srtlist;
		}
		break;
	case 484:
#line 5835 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_srtlist = osmksrtnode(yypvt[-0].st_node, (OSNODE *)NULL);
		}
		break;
	case 485:
#line 5839 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_srtlist = osmksrtnode(yypvt[-2].st_node, yypvt[-0].st_node);
		}
		break;
	case 486:
#line 5843 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_srtlist = osmksrtnode(yypvt[-1].st_node, osmkident(yypvt[-0].st_name, (OSNODE *) NULL));
		}
		break;
	default:
		    goto label_500;
	}

label_500:
	switch (yym) {
	case 511:
#line 5897 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IIOSgqsGenQryStmt(IL_APPEND, RepeatState);
		    osdbeval(yypvt[-4].st_node);
		    osdbstr(ERx("("));
		    osevaltlist(yypvt[-2].st_tlist, osdbqtle);
		    osdbstr(ERx(")"));
		    if (yypvt[-0].st_node != NULL)
		    { /* append qualification */
			osqwhere(yypvt[-0].st_node);
		    }
		    RepeatState = FALSE;
		}
		break;
	case 512:
#line 5911 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IGstartStmt(osscnlno(), IL_LB_NONE);
		    yyval.st_nat = FALSE;
		}
		break;
	case 513:
#line 5916 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IGstartStmt(osscnlno(), IL_LB_NONE);
		    RepeatState = TRUE;
		    yyval.st_nat = TRUE;
		}
		break;
	case 516:
#line 5938 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IIOSgqsGenQryStmt(IL_DELETE, RepeatState);
		    osdbeval(yypvt[-1].st_node);
		    if (yypvt[-0].st_node != NULL)
		    { /* delete qualification */
			osqwhere(yypvt[-0].st_node);
		    }
		    RepeatState = FALSE;
		}
		break;
	case 517:
#line 5949 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IGstartStmt(osscnlno(), IL_LB_NONE);
		    yyval.st_nat = FALSE;
		}
		break;
	case 518:
#line 5954 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IGstartStmt(osscnlno(), IL_LB_NONE);
		    RepeatState = TRUE;
		    yyval.st_nat = TRUE;
		}
		break;
	case 521:
#line 5975 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IIOSgqsGenQryStmt(IL_REPLACE, RepeatState);
		    osdbeval(yypvt[-4].st_node);
		    osdbstr(ERx("("));
		    osevaltlist(yypvt[-2].st_tlist, osdbqtle);
		    osdbstr(ERx(")"));
		    if (yypvt[-0].st_node != NULL)
		    { /* replace qualification */
			osqwhere(yypvt[-0].st_node);
		    }
		    RepeatState = FALSE;
		}
		break;
	case 522:
#line 5989 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IGstartStmt(osscnlno(), IL_LB_NONE);
		    yyval.st_nat = FALSE;
		}
		break;
	case 523:
#line 5994 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IGstartStmt(osscnlno(), IL_LB_NONE);
		    RepeatState = TRUE;
		    yyval.st_nat = TRUE;
		}
		break;
	case 524:
#line 6018 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IGgenStmt(IL_RETINTO, (IGSID *)NULL, 0);
		    osdbeval(yypvt[-6].st_node);
		    osdbstr(ERx("("));
		    osevaltlist(yypvt[-4].st_tlist, osdbqtle);
		    osdbstr(ERx(")"));
		    if (yypvt[-2].st_node != NULL)
		    { /* retrieve qualification */
			osqwhere(yypvt[-2].st_node);
		    }
		    if (yypvt[-1].st_srtlist != NULL)
		    { /* retrieve sort specification */
			osdbstr(yypvt[-1].st_srtlist->srt_type == SRT_SORT
					? ERx(" sort by ") : ERx(" order by ")
			);
			osevalsklist(yypvt[-1].st_srtlist, osdbsrtkey, osldml);
		    }
		    osdbwith(yypvt[-0].st_tlist);	/* optional retrieve into with clause */
		    RetInState = FALSE;
		}
		break;
	case 525:
#line 6040 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IGstartStmt(osscnlno(), IL_LB_NONE);
		    RetInState = TRUE;
		}
		break;
	case 529:
#line 6064 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IGgenStmt(IL_CREATE, (IGSID *)NULL, 0);
		    osdbeval(yypvt[-4].st_node);
		    osdbstr(ERx("("));
		    osevaltlist(yypvt[-2].st_tlist, osdbtle); /* create target list */
		    osdbstr(ERx(")"));
		    osdbwith(yypvt[-0].st_tlist);	/* optional create with clause */
		}
		break;
	case 530:
#line 6074 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 531:
#line 6079 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_tlist = yypvt[-0].st_tlist;
		}
		break;
	case 532:
#line 6083 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_tlist = osaddtlist(yypvt[-2].st_tlist, yypvt[-0].st_tlist);
		}
		break;
	case 533:
#line 6089 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_tlist = osmaketle( yypvt[-3].st_node, yypvt[-1].st_node, osmkident(yypvt[-0].st_name, (OSNODE *)NULL) );
		}
		break;
	case 534:
#line 6093 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_tlist = osmaketle(yypvt[-2].st_node, yypvt[-0].st_node, (OSNODE*)NULL);
		}
		break;
	case 535:
#line 6098 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 536:
#line 6102 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 537:
#line 6116 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IGgenStmt(IL_DESTROY, (IGSID *)NULL, 0);
		    osevallist(yypvt[-0].st_node, osdblist);
		}
		break;
	case 538:
#line 6121 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IGgenStmt(yypvt[-2].st_nat, (IGSID *)NULL, 0);
		    osdbeval(yypvt[-1].st_node);
		    if (yypvt[-0].st_node == NULL)
			osdbstr(ERx(" all"));
		    else
		    {
			osdbstr(ERx(" "));
			osevallist(yypvt[-0].st_node, osdblist);
		    }
		}
		break;
	case 539:
#line 6134 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 540:
#line 6139 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_nat = IL_DESINTEG;
		}
		break;
	case 541:
#line 6143 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_nat = IL_DESPERM;
		}
		break;
	case 542:
#line 6157 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IGgenStmt(IL_INDEX, (IGSID *)NULL, 0);
		    if (yypvt[-8].st_nat)
			osdbstr(ERx(" unique "));
		    osdbstr(ERx(" on "));
		    osdbeval(yypvt[-6].st_node);
		    osdbstr(ERx(" is "));
		    osdbeval(yypvt[-4].st_node);
		    osdbstr(ERx("("));
		    osdbeval(yypvt[-2].st_node);
		    osdbstr(ERx(")"));
		    osdbwith(yypvt[-0].st_tlist);	/* optional index with clause */
		}
		break;
	case 543:
#line 6172 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 544:
#line 6192 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IGgenStmt(IL_RANGE, (IGSID *)NULL, 0);
		    osevaltlist(yypvt[-0].st_tlist, osdbtle);
		}
		break;
	case 545:
#line 6198 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 548:
#line 6206 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_tlist = yypvt[-0].st_tlist;
		}
		break;
	case 549:
#line 6210 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_tlist = osaddtlist(yypvt[-2].st_tlist, yypvt[-0].st_tlist);
		}
		break;
	case 550:
#line 6215 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_tlist = osmaketle(yypvt[-2].st_node, yypvt[-0].st_node, (OSNODE *)NULL);
		}
		break;
	case 551:
#line 6220 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    /*
		    ** Check that the range variable name
		    ** is not the same as a field name.
		    */
		    if (yypvt[-0].st_node->n_token == tkID &&
			    ossymlook(yypvt[-0].st_node->n_value, FormSym) != NULL)
			oscerr(OSRNGFLD, 1, yypvt[-0].st_node->n_value);
		    yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 554:
#line 6242 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 555:
#line 6253 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IGgenStmt(IL_INTEGRITY, (IGSID *)NULL, 0);
		    osdbeval(yypvt[-2].st_node);
		    osqtraverse(yypvt[-0].st_node);
		}
		break;
	case 560:
#line 6283 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IGgenStmt(IL_PERMIT, (IGSID *)NULL, 0);
		    osevallist(yypvt[-8].st_node, osdblist);	/* permit list */
		    osdbstr(ERx(" on "));
		    osdbeval(yypvt[-6].st_node);	/* relation name */
		    if (yypvt[-5].st_node != NULL)
		    { /* permit columns */
			osdbstr(ERx("("));
			osevallist(yypvt[-5].st_node, osdblist);
			osdbstr(ERx(")"));
		    }
		    /* permit who */
		    osdbstr(ERx(" to "));
		    osdbeval(yypvt[-4].st_node);

		    if (yypvt[-3].st_node != NULL)
		    { /* permit terminal */
			osdbstr(ERx(" at "));
			osdbeval(yypvt[-3].st_node);
		    }
		    if (yypvt[-2].st_node != NULL)
		    { /* permit time */
			osdbstr(ERx(" from "));
			osdbeval(yypvt[-2].st_node);
		    }
		    if (yypvt[-1].st_node != NULL)
		    { /* permit day */
			osdbstr(ERx(" on "));
			osdbeval(yypvt[-1].st_node);
		    }
		    if (yypvt[-0].st_node != NULL)
		    { /* permit qualification */
			osqwhere(yypvt[-0].st_node);
		    }
		}
		break;
	case 561:
#line 6320 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 562:
#line 6324 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    u_ptr[0].u_nodep = osmkident(yypvt[-0].st_name, (OSNODE *) NULL);
		    u_ptr[1].u_nodep = NULL;
		    u_ptr[2].u_nodep = NULL;
		    yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 563:
#line 6332 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    u_ptr[0].u_nodep = osmkident(yypvt[-0].st_name, (OSNODE *) NULL);
		    u_ptr[1].u_nodep = NULL;
		    u_ptr[2].u_nodep = NULL;
		    yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 564:
#line 6339 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    u_ptr[0].u_nodep = osmkident(yypvt[-0].st_name, (OSNODE *) NULL);
		    u_ptr[1].u_nodep = yypvt[-2].st_node;
		    u_ptr[2].u_nodep = NULL;
		    yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 565:
#line 6347 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 566:
#line 6351 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 567:
#line 6355 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 568:
#line 6359 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 572:
#line 6368 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 573:
#line 6372 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_node = NULL;
		}
		break;
	case 574:
#line 6377 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 575:
#line 6381 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *) NULL);
		}
		break;
	case 576:
#line 6386 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 577:
#line 6390 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *) NULL);
		}
		break;
	case 578:
#line 6394 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_node = NULL;
		}
		break;
	case 579:
#line 6399 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    u_ptr[0].u_cp = yypvt[-1].st_name;
		    u_ptr[1].u_nodep = yypvt[-2].st_node;
		    u_ptr[2].u_nodep = yypvt[-0].st_node;
		    yyval.st_node = osmknode(ASSOCOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 580:
#line 6406 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_node = NULL;
		}
		break;
	case 581:
#line 6411 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    u_ptr[0].u_cp = ERx(":");
		    u_ptr[1].u_nodep = yypvt[-2].st_node;
		    u_ptr[2].u_nodep = yypvt[-0].st_node;
		    yyval.st_node = osmknode(ASSOCOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 582:
#line 6419 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    u_ptr[0].u_cp = yypvt[-1].st_name;
		    u_ptr[1].u_nodep = yypvt[-2].st_node;
		    u_ptr[2].u_nodep = yypvt[-0].st_node;
		    yyval.st_node = osmknode(ASSOCOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 583:
#line 6426 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_node = NULL;
		}
		break;
	case 584:
#line 6438 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    IGgenStmt(IL_VIEW, (IGSID *)NULL, 0);
		    osdbeval(yypvt[-4].st_node);
		    osdbstr(ERx("("));
		    osevaltlist(yypvt[-2].st_tlist, osdbqtle);
		    osdbstr(ERx(")"));
		    if (yypvt[-0].st_node != NULL)
		    { /* view qualification */
			osqwhere(yypvt[-0].st_node);
		    }
		}
		break;
	case 585:
#line 6458 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    u_ptr[0].u_cp = yypvt[-1].st_name;
		    u_ptr[1].u_nodep = yypvt[-2].st_node;
		    u_ptr[2].u_nodep = yypvt[-0].st_node;
		    yyval.st_node = osmknode(DML|OP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 586:
#line 6465 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    u_ptr[0].u_cp = yypvt[-1].st_name;
		    u_ptr[1].u_nodep = yypvt[-2].st_node;
		    u_ptr[2].u_nodep = yypvt[-0].st_node;
		    yyval.st_node = osmknode(DML|OP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 587:
#line 6472 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    u_ptr[0].u_cp = _Exp;
		    u_ptr[1].u_nodep = yypvt[-2].st_node;
		    u_ptr[2].u_nodep = yypvt[-0].st_node;
		    yyval.st_node = osmknode(DML|OP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 588:
#line 6479 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    u_ptr[0].u_nodep = yypvt[-1].st_node;
		    yyval.st_node = osmknode(PARENS, (U_ARG *) NULL,
						&u_ptr[0], (U_ARG *) NULL);
		}
		break;
	case 589:
#line 6485 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    u_ptr[0].u_cp = _Minus;
		    u_ptr[1].u_nodep = yypvt[-0].st_node;
		    yyval.st_node = osmknode(DML|UNARYOP, &u_ptr[0], &u_ptr[1],
						(U_ARG *) NULL);
		}
		break;
	case 590:
#line 6492 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 591:
#line 6498 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 592:
#line 6502 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 593:
#line 6506 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    /* QUEL expressions may have relation references
		    ** either directly or through range variables.
		    ** So check that a identifier is undefined first,
		    ** and then assume it's a relation reference.
		    */
		    if (ossymlook(yypvt[-0].st_name, FormSym) != NULL)
		    { /* it's a field */
			yyval.st_node = osvalnode(FormSym, yypvt[-0].st_name);
			if (yyval.st_node->n_sym->s_kind == OSFIELD)
			    osfldval(yyval.st_node);
		    }
		    else
		    { /* treat it as a relation (or range variable) */
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		    }
		}
		break;
	case 594:
#line 6524 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    if (yypvt[-0].st_node->n_sym->s_kind == OSFIELD)
			osfldval(yypvt[-0].st_node);
		    yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 595:
#line 6534 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_node = ostvalnode(FormSym, yypvt[-5].st_name, yypvt[-3].st_node, yypvt[-0].st_name);
		    if (yyval.st_node->n_tfref->s_parent->s_kind == OSTABLE)
			ostfval(yyval.st_node);
		}
		break;
	case 596:
#line 6540 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    if (ossymlook(yypvt[-2].st_name, FormSym) != NULL)
		    { /* assume it's a table field */
			yyval.st_node = ostvalnode(FormSym, yypvt[-2].st_name, (OSNODE *)NULL, yypvt[-0].st_name);
			if (yyval.st_node->n_tfref->s_parent->s_kind == OSTABLE)
			    ostfval(yyval.st_node);
		    }
		    else
		    { /* relation.column reference */
			u_ptr[0].u_cp = yypvt[-2].st_name;
			u_ptr[1].u_cp = yypvt[-0].st_name;
			yyval.st_node = osmknode(ATTR, &u_ptr[0], &u_ptr[1],
					(U_ARG *)NULL
			);
		    }
		}
		break;
	case 597:
#line 6557 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    if (yypvt[-0].st_node->n_tfref->s_parent->s_kind == OSTABLE)
			ostfval(yypvt[-0].st_node);
		    yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 598:
#line 6563 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    if (yypvt[-0].st_node->n_tfref->s_parent->s_kind == OSTABLE)
			ostfval(yypvt[-0].st_node);
		    yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 599:
#line 6570 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    u_ptr[0].u_nodep = yypvt[-1].st_node;
		    yyval.st_node = osmkident(yypvt[-3].st_name,  osmknode(PARENS, (U_ARG *)NULL,
						&u_ptr[0], (U_ARG *)NULL
					)
			);
		}
		break;
	case 600:
#line 6579 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 601:
#line 6584 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    u_ptr[0].u_nodep = yypvt[-2].st_node;
		    u_ptr[1].u_nodep = yypvt[-1].st_node;
		    u_ptr[2].u_nodep = yypvt[-0].st_node;
		    yyval.st_node = osmknode(AGGLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 602:
#line 6591 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    u_ptr[0].u_nodep = yypvt[-3].st_node;
		    u_ptr[1].u_nodep = yypvt[-2].st_node;
		    u_ptr[2].u_nodep = osmkident(yypvt[-1].st_name, yypvt[-0].st_node);
		    yyval.st_node = osmknode(AGGLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 603:
#line 6598 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    u_ptr[0].u_nodep = yypvt[-2].st_node;
		    u_ptr[1].u_nodep = yypvt[-0].st_node;
		    yyval.st_node = osmknode(COMMA, (U_ARG *)NULL, &u_ptr[0], &u_ptr[1]);
		}
		break;
	case 604:
#line 6604 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_node = NULL;
		}
		break;
	case 605:
#line 6609 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 606:
#line 6614 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    u_ptr[0].u_cp = yypvt[-2].st_name;
		    u_ptr[1].u_cp = yypvt[-0].st_name;
		    yyval.st_node = osmknode(ATTR, &u_ptr[0], &u_ptr[1], (U_ARG *)NULL);
		}
		break;
	case 607:
#line 6621 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 608:
#line 6625 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_node = NULL;
		}
		break;
	case 609:
#line 6630 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 610:
#line 6634 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    u_ptr[0].u_nodep = yypvt[-2].st_node;
		    u_ptr[1].u_nodep = yypvt[-0].st_node;
		    yyval.st_node = osmknode(COMMA, (U_ARG *)NULL,
						&u_ptr[0], &u_ptr[1]);
		}
		break;
	case 611:
#line 6642 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 612:
#line 6647 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    u_ptr[0].u_cp = yypvt[-2].st_name;
		    u_ptr[1].u_cp = yypvt[-0].st_name;
		    yyval.st_node = osmknode(ATTR, &u_ptr[0], &u_ptr[1], (U_ARG *)NULL);
		}
		break;
	case 613:
#line 6655 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osvalnode(FormSym, yypvt[-0].st_name);
		}
		break;
	case 614:
#line 6660 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = ostvalnode(FormSym, yypvt[-2].st_name, (OSNODE *)NULL, yypvt[-0].st_name);
		}
		break;
	case 615:
#line 6665 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = ostvalnode(FormSym, yypvt[-5].st_name, yypvt[-3].st_node, yypvt[-0].st_name);
		}
		break;
	case 616:
#line 6679 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    u_ptr[0].u_nodep = yypvt[-1].st_node;
		    yyval.st_node =
		      osmknode(PARENS, (U_ARG*)NULL, &u_ptr[0], (U_ARG*)NULL);
		}
		break;
	case 617:
#line 6685 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    u_ptr[0].u_cp = yypvt[-1].st_name;
		    u_ptr[1].u_nodep = yypvt[-2].st_node;
		    u_ptr[2].u_nodep = yypvt[-0].st_node;
		    yyval.st_node = osmknode(DML|LOGOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 618:
#line 6692 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    u_ptr[0].u_cp = yypvt[-1].st_name;
		    u_ptr[1].u_nodep = yypvt[-0].st_node;
		    u_ptr[2].u_nodep = NULL;
		    yyval.st_node = osmknode(DML|LOGOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 619:
#line 6699 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    u_ptr[0].u_cp = yypvt[-1].st_name;
		    u_ptr[1].u_nodep = yypvt[-2].st_node;
		    u_ptr[2].u_nodep = yypvt[-0].st_node;
		    yyval.st_node = osmknode(DML|RELOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 620:
#line 6706 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    u_ptr[0].u_cp = yypvt[-0].st_name;
		    u_ptr[1].u_nodep = yypvt[-1].st_node;
		    u_ptr[2].u_nodep = NULL;
		    yyval.st_node = osmknode(DML|RELOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 621:
#line 6713 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 622:
#line 6732 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_ABORT, (IGSID *)NULL, 0);
		}
		break;
	case 623:
#line 6736 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_ABORT, (IGSID *)NULL, 0);
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 624:
#line 6742 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( SQL )
			{
				/* not supported in future SQL... */
				oswarn(OSXACTWRN, 1, ERx("ABORT TRANSACTION"));

				/* and not part of Open SQL. */
				osOpenwarn(ERx("ABORT TRANSACTION"));
			}
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 627:
#line 6768 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt( QUEL ? IL_SAVEPOINT : IL_SAVPT_SQL,
					(IGSID *)NULL, 0
			);
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 628:
#line 6776 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( SQL )
				osOpenwarn(ERx("SAVEPOINT"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 629:
#line 6790 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( SQL )
			{
				/* not supported in future SQL... */
				oswarn(OSXACTWRN, 1, ERx("BEGIN TRANSACTION"));

				/* and not part of Open SQL. */
				osOpenwarn(ERx("BEGIN TRANSACTION"));
			}
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_BEGTRANS, (IGSID *)NULL, 0);
		}
		break;
	case 630:
#line 6803 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( SQL )
			{
				/* not supported in future SQL... */
				oswarn(OSXACTWRN, 1, ERx("END TRANSACTION"));

				/* and not part of Open SQL. */
				osOpenwarn(ERx("END TRANSACTION"));
			}
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_ENDTRANS, (IGSID *)NULL, 0);
		}
		break;
	case 631:
#line 6832 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 632:
#line 6836 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_cp = ERx(":");
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(DML|OP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 633:
#line 6843 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_cp = ERx(":");
			u_ptr[1].u_nodep = yypvt[-1].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(DML|OP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 634:
#line 6853 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
    	    yyval.st_node = osmkident( yypvt[-0].st_name, (OSNODE *)NULL );
    	}
		break;
	case 635:
#line 6857 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
            u_ptr[0].u_nodep = osmkident( yypvt[-2].st_name, (OSNODE *)NULL );
            u_ptr[1].u_nodep = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
    	    u_ptr[1].u_nodep->n_type = DB_CHA_TYPE;  /* see ingres_name def. */
            yyval.st_node = osmknode(DOT|DML, (U_ARG*)NULL, &u_ptr[0], &u_ptr[1]);
        }
		break;
	case 636:
#line 6874 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 637:
#line 6878 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 638:
#line 6890 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 639:
#line 6894 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			char	buf[OSBUFSIZE];

			_VOID_ STprintf(buf, ERx("%s(%s)"), yypvt[-3].st_name, yypvt[-1].st_name);
			yyval.st_node = osmkident( iiIG_string(buf), (OSNODE *)NULL );
		}
		break;
	case 640:
#line 6901 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			char	buf[OSBUFSIZE];

			_VOID_ STprintf(buf, ERx("%s(%s,%s)"), yypvt[-5].st_name, yypvt[-3].st_name, yypvt[-1].st_name);
			yyval.st_node = osmkident( iiIG_string(buf), (OSNODE *)NULL );
		}
		break;
	case 641:
#line 6908 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			char	buf[OSBUFSIZE];
			(VOID) STprintf(buf, ERx("%s %s(%s)"), yypvt[-4].st_name, yypvt[-3].st_name, yypvt[-1].st_name );
			yyval.st_node = osmkident( iiIG_string(buf), (OSNODE *)NULL );
		}
		break;
	case 642:
#line 6914 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			char	buf[OSBUFSIZE];
			(VOID) STprintf(buf, ERx("%s %s"), yypvt[-1].st_name, yypvt[-0].st_name );
			yyval.st_node = osmkident( iiIG_string(buf), (OSNODE *)NULL );
		}
		break;
	case 643:
#line 6925 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = ERx("with null");
		}
		break;
	case 644:
#line 6929 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = ERx("not null");
		}
		break;
	case 645:
#line 6933 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
                        if ( SQL )
                                osOpenwarn(ERx("... WITH DEFAULT"));
                        yyval.st_name = ERx("not null with default");
                }
		break;
	case 646:
#line 6939 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
                        if ( SQL )
                                osOpenwarn(ERx("... WITH DEFAULT"));
                        yyval.st_name = ERx("not null not default");
                }
		break;
	case 647:
#line 6948 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( SQL )
				osOpenwarn(ERx("... WITH DEFAULT"));
			yyval.st_name = ERx("with default");
		}
		break;
	case 648:
#line 6954 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( SQL )
				osOpenwarn(ERx("... NOT DEFAULT"));
			yyval.st_name = ERx("not default");
		}
		break;
	case 649:
#line 6969 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_nat = TRUE;
		}
		break;
	case 650:
#line 6973 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_nat = FALSE;
		}
		break;
	case 651:
#line 7099 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = yypvt[-0].st_tlist;
		}
		break;
	case 652:
#line 7103 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = NULL;
		}
		break;
	case 653:
#line 7109 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = yypvt[-0].st_tlist;
		}
		break;
	case 654:
#line 7113 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = osaddtlist(yypvt[-2].st_tlist, yypvt[-0].st_tlist);
		}
		break;
	case 655:
#line 7119 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			OSNODE *t1 = yypvt[-0].st_node;
			OSNODE *t2 = (OSNODE *) NULL;

			if (yypvt[-1].st_nat)
			{
				t1 = osmkident(ERx("unique"), (OSNODE *) NULL);
				t2 = yypvt[-0].st_node;
			}
			yyval.st_tlist = osmaketle(yypvt[-3].st_node, t1, t2);
		}
		break;
	case 656:
#line 7131 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[1].u_nodep = yypvt[-1].st_node;
			yyval.st_tlist = osmaketle( yypvt[-4].st_node,
				osmknode(PARENS, (U_ARG*)NULL, &u_ptr[1],
					(U_ARG*)NULL),
				(OSNODE*)NULL
			);
		}
		break;
	case 657:
#line 7140 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = osmaketle(yypvt[-0].st_node, (OSNODE *) NULL, (OSNODE *) NULL);
		}
		break;
	case 658:
#line 7145 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-2].st_node;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(COMMA, (U_ARG *)NULL, &u_ptr[0], &u_ptr[1]);
		}
		break;
	case 659:
#line 7151 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 660:
#line 7156 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (yypvt[-0].st_name == NULL)
				yyval.st_node = yypvt[-1].st_node;
			else
			{
				u_ptr[0].u_nodep = yypvt[-1].st_node;
				u_ptr[1].u_nodep = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
				yyval.st_node = osmknode(BLANK, (U_ARG *)NULL,
							&u_ptr[0], &u_ptr[1]
				);
			}
		}
		break;
	case 661:
#line 7169 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 662:
#line 7173 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 663:
#line 7177 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 664:
#line 7183 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 665:
#line 7188 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;
		}
		break;
	case 666:
#line 7193 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;
		}
		break;
	case 667:
#line 7198 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;
		}
		break;
	case 668:
#line 7203 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;
		}
		break;
	case 669:
#line 7208 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;
		}
		break;
	case 670:
#line 7213 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;
		}
		break;
	case 671:
#line 7218 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;
		}
		break;
	case 672:
#line 7224 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = NULL;
		}
		break;
	case 673:
#line 7228 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			/* We'd better be in QUEL. */
			if (!osw_compare(ERx("asc"), yypvt[-0].st_name)
			 && !osw_compare(ERx("desc"), yypvt[-0].st_name))
				yyerror(_SyntaxError);
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 674:
#line 7236 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 675:
#line 7240 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 676:
#line 7263 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			ILREF	fres = 0;

			/* pass file directly as semantic value (w/o '\\') */
			fres = osvarref(yypvt[-1].st_node);

			IGgenStmt(QUEL ? IL_COPY :IL_COPY_SQL, (IGSID*)NULL, 0);
			osdbeval(yypvt[-6].st_node);
			osdbstr(ERx("("));
			if (yypvt[-4].st_tlist != NULL)
				osevaltlist(yypvt[-4].st_tlist, osdbtle); /* copy target list */
			osdbstr(ERx(")"));
			osdbstr(yypvt[-2].st_name);
			osdbflush();
			IGgenStmt(IL_DBVAR, (IGSID *)NULL, 1, fres);/* string val */
			osdbwith(yypvt[-0].st_tlist);	/* optional with clause */
		}
		break;
	case 677:
#line 7282 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( SQL )
				osOpenwarn(ERx("COPY"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 678:
#line 7289 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 679:
#line 7293 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 680:
#line 7298 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = NULL;
		}
		break;
	case 681:
#line 7302 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = yypvt[-0].st_tlist;
		}
		break;
	case 682:
#line 7307 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = yypvt[-0].st_tlist;
		}
		break;
	case 683:
#line 7311 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = osaddtlist(yypvt[-2].st_tlist, yypvt[-0].st_tlist);
		}
		break;
	case 684:
#line 7316 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = osmaketle(yypvt[-3].st_node, yypvt[-1].st_node, yypvt[-0].st_node);
		}
		break;
	case 685:
#line 7321 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;

			/*
			** Hack for bug 31342:
			**
			** The INGRES DBMS requires that a copy format
			** of c0<delim> or d0<delim> be enclosed
			** in quotes (QUEL) or apostrophes (SQL),
			** in the case where <delim> is a 1-character
			** delimiter, so we force quotes or apostrophes
			** in precisely those 2 cases.
			**
			** A cleaner approach would have been to emit
			** exactly what the user entered: that is, to
			** restore the enclosing quotes or apostrophes
			** if the copy format came in as an SCONST.
			** However, that could break existing applications.
			** For example, if the user specifies a copy format
			** of 'char(0)', we currently strip off the apostrophes,
			** which produces a legal format: char(0).  If we were
			** to restore the apostrophes, the DBMS would reject
			** the copy format.
			*/
			if (yypvt[-0].st_node->n_token == tkID)
			{
				char	*format_string = yypvt[-0].st_node->n_value;

				if (  STlength(format_string) == 3
				   && format_string[1] == '0'
				   && (  format_string[0] == 'c'
				      || format_string[0] == 'd'
				      )
				   )
				{
					char	buf[OSBUFSIZE];

					(VOID) STprintf(buf, ERx("%s%s%s"),
							osQuote, format_string,
							osQuote);
					yyval.st_node = osmkident( iiIG_string(buf),
							(OSNODE *)NULL );
				}
			}
		}
		break;
	case 686:
#line 7367 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			char	buf[OSBUFSIZE];

			if (yypvt[-0].st_name == NULL)
			{
				(VOID) STprintf(buf, ERx("%s(%s)"), yypvt[-4].st_name, yypvt[-2].st_name);
			}
			else if (STlength(yypvt[-0].st_name) == 1)
			{
				(VOID) STprintf(buf, ERx("%s(%s)%s%s%s"),
						yypvt[-4].st_name, yypvt[-2].st_name, osQuote, yypvt[-0].st_name, osQuote);
			}
			else
			{
				(VOID) STprintf(buf, ERx("%s(%s)%s"),
						yypvt[-4].st_name, yypvt[-2].st_name, yypvt[-0].st_name);
			}
			yyval.st_node = osmkident( iiIG_string(buf), (OSNODE *)NULL );
		}
		break;
	case 687:
#line 7388 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			char	buf[OSBUFSIZE];

			if (yypvt[-0].st_name == NULL)
			{
				(VOID) STprintf(buf, ERx("%s %s(%s)"),
				                yypvt[-5].st_name, yypvt[-4].st_name, yypvt[-2].st_name);
			}
			else if (STlength(yypvt[-0].st_name) == 1)
			{
				(VOID) STprintf(buf, ERx("%s %s(%s)%s%s%s"),
						yypvt[-5].st_name, yypvt[-4].st_name, yypvt[-2].st_name, osQuote, yypvt[-0].st_name,
				                osQuote);
			}
			else
			{
				(VOID) STprintf(buf, ERx("%s %s(%s)%s"),
						yypvt[-5].st_name, yypvt[-4].st_name, yypvt[-2].st_name, yypvt[-0].st_name);
			}
			yyval.st_node = osmkident( iiIG_string(buf), (OSNODE *)NULL );
		}
		break;
	case 688:
#line 7411 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 689:
#line 7415 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = NULL;
		}
		break;
	case 690:
#line 7420 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkident( ERx("with null"), (OSNODE *)NULL );
		}
		break;
	case 691:
#line 7424 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			/* 
			 * Bug 34530 - re-written to produce the correct
			 * "with null(...)" string for all the various
			 * constant types.
			 */

			char	buf[OSBUFSIZE]; /* put "with null (...)" here */
			char	xcstr[2];	/* the "X" in "(X'3939')" */
			i4	ntk;		/* token type of $4 */
			char	*qstr;		/* osQuote in string, else "" */

			xcstr[0] = xcstr[1] = EOS;
			ntk = yypvt[-1].st_node->n_token;
			qstr = osQuote;

			if ( ntk == tkXCONST )
				xcstr[0] = 'X';
			else if ( ntk == tkFCONST 
			       || ntk == tkICONST 
			       || ntk == tkDCONST 
				)
				qstr = ERx("");
			else if ( ntk != tkSCONST )
			{
				/* syntax error */
				oscerr(E_OS0132_BadWithNull, 1, yypvt[-1].st_node->n_const);
			}

			STprintf(buf, ERx("with null(%s%s%s%s)"), xcstr,
								  qstr,
								  yypvt[-1].st_node->n_const,
								  qstr);
			yyval.st_node = osmkident( iiIG_string(buf), (OSNODE *)NULL );
		}
		break;
	case 692:
#line 7460 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 693:
#line 7475 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(QUEL ? IL_MODIFY : IL_SQLMODIFY, (IGSID*)NULL, 0);
			osdbeval(yypvt[-5].st_node);
			osdbstr(ERx(" to "));
			osdbeval(yypvt[-3].st_node);
			if (yypvt[-2].st_nat)
				osdbstr(ERx(" unique "));
			if (yypvt[-1].st_srtlist != NULL)
			{ /* modify key list */
				osdbstr(ERx(" on "));
				osevalsklist(yypvt[-1].st_srtlist, osdbsrtkey, osldml);
			}
			if (yypvt[-0].st_tlist != NULL)
			{ /* modify fill list */
				osdbstr(ERx(" where "));
				osevaltlist(yypvt[-0].st_tlist, osdbtle);
			}
		}
		break;
	case 694:
#line 7495 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( SQL )
				osOpenwarn(ERx("MODIFY"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 695:
#line 7502 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 696:
#line 7506 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 697:
#line 7511 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_srtlist = yypvt[-0].st_srtlist;
		}
		break;
	case 698:
#line 7515 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_srtlist = NULL;
		}
		break;
	case 699:
#line 7520 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = NULL;
		}
		break;
	case 700:
#line 7524 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = yypvt[-0].st_tlist;
		}
		break;
	case 701:
#line 7528 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = yypvt[-0].st_tlist;
		}
		break;
	case 702:
#line 7540 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_RELOCATE, (IGSID *)NULL, 0);
			osdbeval(yypvt[-2].st_node);
			osdbstr(ERx(" to "));
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 703:
#line 7548 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( SQL )
				osOpenwarn(ERx("RELOCATE"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 704:
#line 7562 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_SAVE, (IGSID *)NULL, 0);
			osdbeval(yypvt[-4].st_node);
			osdbstr(ERx(" until "));
			osdbeval(yypvt[-2].st_node);
			osdbstr(ERx(" "));
			osdbeval(yypvt[-1].st_node);
			osdbstr(ERx(" "));
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 705:
#line 7573 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_SAVE, (IGSID *)NULL, 0);
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 706:
#line 7579 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( SQL )
				osOpenwarn(ERx("SAVE"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 707:
#line 7603 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 708:
#line 7607 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osdbeval(yypvt[-1].st_node);
			osdbstr(ERx(" "));
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 710:
#line 7614 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osdbeval(yypvt[-1].st_node);
			osdbstr(ERx(" on"));
		}
		break;
	case 711:
#line 7620 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osdbeval(yypvt[-1].st_node);
			osdbstr(ERx(" off"));
		}
		break;
	case 712:
#line 7626 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osdbeval(yypvt[-2].st_node);
			osdbstr(ERx(" on "));
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 713:
#line 7633 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osdbeval(yypvt[-2].st_node);
			osdbstr(ERx(" "));
			osdbeval(yypvt[-1].st_node);
			osdbstr(ERx(" "));
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 715:
#line 7646 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osOpenwarn(ERx("SET ROLE"));
			osdbeval(yypvt[-1].st_node);
			osdbstr(ERx(" session role"));
		}
		break;
	case 716:
#line 7654 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osOpenwarn(ERx("SET GROUP"));
			osdbstr(ERx(" group session group"));
		}
		break;
	case 717:
#line 7661 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osOpenwarn(ERx("SET GROUP"));
			osdbstr(ERx(" group "));
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 718:
#line 7669 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osOpenwarn(ERx("SET USER AUTHORIZATION"));
			osdbstr(ERx(" user authorization "));
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 719:
#line 7677 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osOpenwarn(ERx("SET USER AUTHORIZATION"));
			osdbstr(ERx(" user authorization "));
			osdbstr(yypvt[-0].st_name);
		}
		break;
	case 720:
#line 7685 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osdbeval(yypvt[-3].st_node);
			osdbstr(ERx(" "));
			osdbeval(yypvt[-2].st_node);
			osdbstr(ERx(" where "));
			osevaltlist(yypvt[-0].st_tlist, osdbtle);	/* lock list */
		}
		break;
	case 721:
#line 7694 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osdbeval(yypvt[-3].st_node);
			osdbstr(ERx(" session where "));
			osevaltlist(yypvt[-0].st_tlist, osdbtle);	/* lock list */
		}
		break;
	case 722:
#line 7700 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osdbeval(yypvt[-4].st_node);
			osdbstr(ERx(" on "));
			osdbeval(yypvt[-2].st_node);
			osdbstr(ERx(" where "));
			osevaltlist(yypvt[-0].st_tlist, osdbtle);	/* lock list */
		}
		break;
	case 723:
#line 7709 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(QUEL ? IL_SET : IL_SETSQL, (IGSID *)NULL, 0);
		}
		break;
	case 724:
#line 7716 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(QUEL ? IL_SET : IL_SETSQL, (IGSID *)NULL, 0);
			osdbstr(ERx("session"));
		}
		break;
	case 725:
#line 7723 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osdbwith(yypvt[-0].st_tlist);
		}
		break;
	case 730:
#line 7739 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osdbwith(yypvt[-0].st_tlist);
		}
		break;
	case 731:
#line 7747 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = yypvt[-0].st_tlist;
		}
		break;
	case 732:
#line 7752 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = osaddtlist(yypvt[-2].st_tlist, yypvt[-0].st_tlist);
		}
		break;
	case 733:
#line 7759 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = osmaketle(yypvt[-2].st_node, yypvt[-0].st_node, (OSNODE *)NULL);
		}
		break;
	case 734:
#line 7764 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = osmaketle(yypvt[-0].st_node, (OSNODE *) NULL,
						(OSNODE *)NULL);
		}
		break;
	case 735:
#line 7769 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[1].u_nodep = yypvt[-1].st_node;
			yyval.st_tlist = osmaketle( yypvt[-4].st_node,
				osmknode(PARENS, (U_ARG*)NULL, &u_ptr[1],
					(U_ARG*)NULL),
				(OSNODE*)NULL
			);
		}
		break;
	case 736:
#line 7779 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = osmaketle(yypvt[-2].st_node,  yypvt[-0].st_node,
					(OSNODE *)NULL);
		}
		break;
	case 737:
#line 7786 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 738:
#line 7790 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (!oschkstr(yypvt[-1].st_node->n_type))
				oscerr(OSNOTSTR, 1, ERget(_FrsName));
			else if ( AFE_NULLABLE_MACRO(yypvt[-1].st_node->n_type) )
				oswarn(OSNULVCHK, 0);
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 739:
#line 7799 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{yyval.st_node=yypvt[-0].st_node;}
		break;
	case 740:
#line 7802 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{ yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;	/* special case */
			}
		break;
	case 741:
#line 7806 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{ yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;	/* special case */
			}
		break;
	case 742:
#line 7812 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = osmkident(yypvt[-1].st_name, (OSNODE*)NULL);
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode( BLANK, (U_ARG*)NULL,
					&u_ptr[0], &u_ptr[1]);
		}
		break;
	case 743:
#line 7820 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
                        if (osChkSQL && yypvt[-0].st_node->n_token == tkID
			  && STbcompare(yypvt[-0].st_node->n_value, 0, ERx("autocommit"), 0,
					FALSE) != 0
				)
			{
				char	buf[OSBUFSIZE];

				osOpenwarn( STprintf( buf, ERx("SET %s"),
							yypvt[-0].st_node->n_value
						)
				);
			}
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 744:
#line 7837 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = ERx("system user");
		}
		break;
	case 745:
#line 7841 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = ERx("session user");
		}
		break;
	case 746:
#line 7846 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = yypvt[-0].st_tlist;
		}
		break;
	case 747:
#line 7850 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = osaddtlist(yypvt[-2].st_tlist, yypvt[-0].st_tlist);
		}
		break;
	case 748:
#line 7855 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = osmaketle(yypvt[-2].st_node, yypvt[-0].st_node, (OSNODE *)NULL);
		}
		break;
	case 749:
#line 7859 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = osmaketle(yypvt[-2].st_node, osmkident(yypvt[-0].st_name, (OSNODE *) NULL),
						(OSNODE *)NULL);
		}
		break;
	case 750:
#line 7864 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = osmaketle(yypvt[-2].st_node, osmkident(yypvt[-0].st_name, (OSNODE *) NULL),
						(OSNODE *)NULL);
		}
		break;
	case 751:
#line 7869 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_tlist = osmaketle(yypvt[-2].st_node, osmkident(yypvt[-0].st_name, (OSNODE *) NULL),
						(OSNODE *)NULL);
		}
		break;
	case 754:
#line 7888 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{  osdbstr(ERx(" add privileges "));}
		break;
	case 755:
#line 7891 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{  osdbstr(ERx(" drop privileges "));}
		break;
	case 756:
#line 7894 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
	    osdbeval(yypvt[-0].st_node);
	}
		break;
	case 759:
#line 7902 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
	    osdbstr(ERx("("));
	}
		break;
	case 760:
#line 7907 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
	    osdbstr(ERx(")"));
	}
		break;
	case 761:
#line 7912 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
	    osdbstr(ERx(","));
	}
		break;
	case 762:
#line 7931 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_DIREXIMM, (IGSID *)NULL, 0);
			osdbeval(yypvt[-1].st_node);
			osdbwith(yypvt[-0].st_tlist);	/* optional with clause */
		}
		break;
	case 763:
#line 7938 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			/*
			**  In Quel, error recovery is awful for unrecognized
			**  2nd words of double-keywords.  So it's much
			**  prettier if we catch it here.  This should be fixed.
			*/
			if (QUEL)
				yyerror(_SyntaxError);
			else
				osOpenwarn(ERx("DIRECT EXECUTE IMMEDIATE"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 764:
#line 7959 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( SQL )
				osOpenwarn(ERx("DIRECT CONNECT"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_DIRCONN, (IGSID *)NULL, 0);
			osdbwith(yypvt[-0].st_tlist);	/* optional with clause */
		}
		break;
	case 765:
#line 7976 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( SQL )
				osOpenwarn(ERx("DIRECT DISCONNECT"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_DIRDIS, (IGSID *)NULL, 0);
		}
		break;
	case 766:
#line 8002 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGgenStmt(IL_REGISTER, (IGSID *)NULL, 0);

			/* <empty>, TABLE, INDEX, VIEW, or PROCEDURE(SQL) */
			if (yypvt[-7].st_name != NULL)
			{
				osdbstr(yypvt[-7].st_name);
				osdbstr(ERx(" "));
			}

			/* object name */
			osdbeval(yypvt[-6].st_node);

			/* the optional ON <name> */
			if (yypvt[-5].st_node != NULL)
			{
				osdbstr(ERx(" on "));
				osdbeval(yypvt[-5].st_node);
			}

			/* the optional formatlist */
			if (yypvt[-4].st_tlist != NULL)
			{
				osdbstr(ERx("("));
				if (QUEL)
					osevaltlist(yypvt[-4].st_tlist, osdbtle);
				else
					osevaltlist(yypvt[-4].st_tlist, osdbsqltle);
				osdbstr(ERx(")"));
			}

			osdbstr(ERx(" as "));
			osdbstr(yypvt[-2].st_name);

			/* optional FROM clause */
			if (yypvt[-1].st_node != NULL)
			{
				osdbstr(ERx(" from "));
				osdbeval(yypvt[-1].st_node);
			}

			osdbwith(yypvt[-0].st_tlist);	/* optional WITH clause */
		}
		break;
	case 767:
#line 8047 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( SQL )
				osOpenwarn(ERx("REGISTER"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 768:
#line 8054 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			yyval.st_name = ERx("table");
		}
		break;
	case 769:
#line 8059 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			osOpenwarn(ERx("REGISTER"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
			yyval.st_name = ERx("view");
		}
		break;
	case 770:
#line 8066 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = NULL;
		}
		break;
	case 771:
#line 8070 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 772:
#line 8074 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = ERx("unique index");
		}
		break;
	case 773:
#line 8078 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 774:
#line 8082 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 775:
#line 8087 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( !osw_compare(ERx("import"), yypvt[-0].st_name)
					&& !osw_compare(ERx("link"), yypvt[-0].st_name) )
			{
				yyerror(_SyntaxError);
			}
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 776:
#line 8097 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 777:
#line 8101 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 778:
#line 8107 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		yyval.st_tlist = NULL;
	}
		break;
	case 779:
#line 8111 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		yyval.st_tlist = yypvt[-1].st_tlist;
	}
		break;
	case 780:
#line 8117 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		yyval.st_tlist = yypvt[-0].st_tlist;
	}
		break;
	case 781:
#line 8121 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		yyval.st_tlist = osaddtlist(yypvt[-2].st_tlist, yypvt[-0].st_tlist);
	}
		break;
	case 782:
#line 8127 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		yyval.st_tlist = yypvt[-0].st_tlist;
	}
		break;
	case 783:
#line 8131 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		if ( yypvt[-1].st_tlist->tl_dim == NULL )
			yypvt[-1].st_tlist->tl_dim = yypvt[-0].st_node;
		else
		{
			OSNODE *tmp;

			u_ptr[0].u_nodep = yypvt[-1].st_tlist->tl_dim;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yypvt[-1].st_tlist->tl_dim = osmknode( BLANK, (U_ARG *)NULL,
						&u_ptr[0], &u_ptr[1]
			);
		}
		yyval.st_tlist = yypvt[-1].st_tlist;
	}
		break;
	case 784:
#line 8147 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		yyval.st_tlist = osmaketle(yypvt[-0].st_node, (OSNODE *)NULL, (OSNODE *)NULL);
	}
		break;
	case 785:
#line 8152 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = osmkident(yypvt[-1].st_name, (OSNODE *) NULL);
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode( BLANK, (U_ARG *) NULL,
						&u_ptr[0], &u_ptr[1]
			);
		}
		break;
	case 786:
#line 8161 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 787:
#line 8165 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 788:
#line 8177 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_REMOVE, (IGSID *)NULL, 0);

			/* <empty>, TABLE, INDEX, or VIEW */
			if (yypvt[-1].st_name != NULL)
			{
				osdbstr(yypvt[-1].st_name);
				osdbstr(ERx(" "));
			}

			/* object name */
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 789:
#line 8193 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( SQL )
				osOpenwarn(ERx("REMOVE"));
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 790:
#line 8199 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = ERx("table");
		}
		break;
	case 791:
#line 8203 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( SQL )
				osOpenwarn(ERx("REMOVE"));
			yyval.st_name = ERx("view");
		}
		break;
	case 792:
#line 8236 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_cp = yypvt[-1].st_name;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(OP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 793:
#line 8243 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_cp = yypvt[-1].st_name;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(OP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 794:
#line 8250 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_cp = _Exp;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(OP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 795:
#line 8257 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 796:
#line 8261 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_cp = _UnaryMinus;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node =
			  osmknode(UNARYOP, &u_ptr[0], &u_ptr[1], (U_ARG*)NULL);
		}
		break;
	case 797:
#line 8268 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			register OSNODE	*proc;
			char	*proc_name;

			if (yypvt[-3].st_node->n_token == tkID)
			{
				proc_name = yypvt[-3].st_node->n_value;
				u_ptr[0].u_symp = osprocsym(yypvt[-3].st_node->n_value, TRUE);
			}
			else
			{
				proc_name = NULL;
				u_ptr[0].u_symp = (OSSYM *)NULL;
			}
			u_ptr[1].u_nodep = yypvt[-3].st_node;
			u_ptr[2].u_nodep = yypvt[-1].st_node;
			proc = osmknode(tkPCALL, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
			if ( proc->n_token == tkPCALL )
			{
				if ( proc->n_psym != NULL )
				{
					if ( proc->n_psym->s_kind == OSUNDEF
						|| proc->n_type != DB_NODT )
					{
						proc->n_psym->s_ref |=
							OS_OBJREF;
					}
					else
					{
						oscerr( OSNORETVAL,
							1, proc->n_proc->n_const
						);
					}
				}
			}
			else	/* osmknode determined that the specified
				** procedure is a system (ADF) function.
				** See if it's DBMSINFO.
				*/
			{
				if ( STbcompare(proc_name, 0,
					ERx("dbmsinfo"), 0, TRUE) == 0 )
				{
					oswarn(E_OS0269_BadDbmsinfo);
				}
			}

			yyval.st_node = proc;
		}
		break;
	case 798:
#line 8318 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 799:
#line 8324 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			/* Translate strings */
			if ( yypvt[-0].st_node->n_token == tkSCONST )
				yypvt[-0].st_node->n_const = osstring( FALSE, yypvt[-0].st_node->n_const, osldml);
			else if ( yypvt[-0].st_node->n_token == tkXCONST )
				yypvt[-0].st_node->n_const = oshexvchar(yypvt[-0].st_node->n_const);	/* SQL only */
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 800:
#line 8333 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 801:
#line 8339 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 802:
#line 8343 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 803:
#line 8357 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 804:
#line 8361 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 805:
#line 8366 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 806:
#line 8370 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 807:
#line 8376 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( yypvt[-0].st_node->n_token == tkNULL ||
				( yypvt[-0].st_node->n_type != DB_NODT &&
					!oschkstr(yypvt[-0].st_node->n_type) ) )
			{ /* not a string! */
				oscerr(OSNOTSTRX, 0);
			}
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 808:
#line 8388 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( !ADE_CHK_TYPE(yypvt[-0].st_node->n_type, DB_INT_TYPE) &&
					yypvt[-0].st_node->n_type != DB_NODT )
				oscerr(OSNOTINT, 0);
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 809:
#line 8403 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 810:
#line 8407 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_cp = yypvt[-1].st_name;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(LOGOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 811:
#line 8414 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_cp = _Not;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(LOGOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 812:
#line 8421 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_cp = yypvt[-1].st_name;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(RELOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 813:
#line 8428 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_cp = yypvt[-0].st_name;
			u_ptr[1].u_nodep = yypvt[-1].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(RELOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 814:
#line 8435 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_cp = yypvt[-2].st_name;
			u_ptr[1].u_nodep = yypvt[-3].st_node;
			u_ptr[2].u_nodep = yypvt[-1].st_node;

			yyval.st_node = osmknode(RELOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
			if ( yypvt[-0].st_node != NULL )
			{ /* SQL only because ESCAPE is not a keyword in QUEL */
				u_ptr[0].u_cp = _Escape;
				u_ptr[1].u_nodep = yyval.st_node;
				u_ptr[2].u_nodep = yypvt[-0].st_node;
				yyval.st_node = osmknode(RELOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
			}
		}
		break;
	case 815:
#line 8451 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			/* Translate strings */
			if ( yypvt[-0].st_node->n_token == tkSCONST )
				yypvt[-0].st_node->n_const = osstring( TRUE, yypvt[-0].st_node->n_const, osldml);
			else if ( yypvt[-0].st_node->n_token == tkXCONST )
				yypvt[-0].st_node->n_const = oshexvchar(yypvt[-0].st_node->n_const);  /* SQL only */
			else
			{
				/* not a string! */
				oscerr(OSNOTSTRX, 0);
			}
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 816:
#line 8465 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( yypvt[-0].st_node->n_token == tkNULL
			  || ( yypvt[-0].st_node->n_type != DB_NODT && !oschkstr(yypvt[-0].st_node->n_type) ) )
			{
				/* not a string! */
				oscerr(OSNOTSTRX, 0);
			}
			if ( QUEL )
			{ /* Must encode dynamic value for pattern-match */
				U_ARG   encode;
				U_ARG   utmp;

				/* Note:  ADE_PMENCODE operates on its argument
				** translating pattern-match characters (and escapes)
				** into the internal pattern-match representation.
				** So, variables must be copied into a temporary
				** before applying ADE_PMENCODE and the LIKE operators.
				*/
                		encode.u_cp = ERx("varchar");
				utmp.u_nodep = yypvt[-0].st_node;
				utmp.u_nodep = osmknode(UNARYOP, &encode,
						&utmp, (U_ARG *)NULL);

				encode.u_cp = _PMEncode;
				yyval.st_node = osmknode(UNARYOP, &encode,
						&utmp, (U_ARG *)NULL);
			}
			else
			{
				yyval.st_node = yypvt[-0].st_node;
			}
		}
		break;
	case 817:
#line 8508 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkconst(tkSCONST, yypvt[-0].st_name);
		}
		break;
	case 818:
#line 8512 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (QUEL)
				yyerror(_SyntaxError);
			yyval.st_node = osmkconst(tkXCONST, yypvt[-0].st_name);
		}
		break;
	case 819:
#line 8518 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkconst(tkICONST, yypvt[-0].st_name);
		}
		break;
	case 820:
#line 8522 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkconst(tkFCONST, yypvt[-0].st_name);
		}
		break;
	case 821:
#line 8526 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkconst(tkDCONST, yypvt[-0].st_name);
		}
		break;
	case 822:
#line 8531 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 823:
#line 8535 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			char    buf[OSBUFSIZE];

			buf[0] = '-'; STcopy( yypvt[-0].st_name, buf + 1 );
			yyval.st_node = osmkconst(tkICONST, iiIG_string(buf));
		}
		break;
	case 824:
#line 8542 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			char    buf[OSBUFSIZE];

			buf[0] = '-'; STcopy( yypvt[-0].st_name, buf + 1 );
			yyval.st_node = osmkconst(tkFCONST, iiIG_string(buf));
		}
		break;
	case 825:
#line 8549 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			char    buf[OSBUFSIZE];

			buf[0] = '-'; STcopy( yypvt[-0].st_name, buf + 1 );
			yyval.st_node = osmkconst(tkDCONST, iiIG_string(buf));
		}
		break;
	case 826:
#line 8557 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 827:
#line 8561 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkconst(tkNULL, yypvt[-0].st_name);
		}
		break;
	case 828:
#line 8566 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 829:
#line 8570 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkconst(tkNULL, yypvt[-0].st_name);
		}
		break;
	case 830:
#line 8575 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkconst(tkSCONST, yypvt[-0].st_name);
		}
		break;
	case 831:
#line 8580 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkconst(tkICONST, yypvt[-0].st_name);
		}
		break;
	case 832:
#line 8584 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			char	buf[64];

			buf[0] = '-'; STcopy( yypvt[-0].st_name, buf + 1 );
			yyval.st_node = osmkconst(tkICONST, iiIG_string(buf));
		}
		break;
	case 833:
#line 8593 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = _Mul;
		}
		break;
	case 834:
#line 8597 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = _Div;
		}
		break;
	case 835:
#line 8603 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = _Plus;
		}
		break;
	case 836:
#line 8607 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = _Minus;
		}
		break;
	case 837:
#line 8613 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = _And;
		}
		break;
	case 838:
#line 8617 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = _Or;
		}
		break;
	case 839:
#line 8623 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = _Less;
		}
		break;
	case 840:
#line 8627 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = _Greater;
		}
		break;
	case 841:
#line 8631 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = _Equal;
		}
		break;
	case 842:
#line 8635 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = _LessEqual;
		}
		break;
	case 843:
#line 8639 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = _GreatrEqual;
		}
		break;
	case 844:
#line 8643 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = _NotEqual;
		}
		break;
	case 845:
#line 8649 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = _Like;
		}
		break;
	case 846:
#line 8653 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = _NotLike;
		}
		break;
	case 847:
#line 8663 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkconst(tkSCONST, yypvt[-0].st_name);
		}
		break;
	case 848:
#line 8667 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkconst(tkXCONST, yypvt[-0].st_name);
		}
		break;
	case 849:
#line 8671 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 850:
#line 8692 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-1].st_node;
			yyval.st_node = osmknode(NPRED, &u_ptr[0], (U_ARG*)NULL, (U_ARG*)NULL);
		}
		break;
	case 851:
#line 8698 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( RepeatState )
			{
				oswarn( E_OS015B_QualRepeat, 0 );
				RepeatState = FALSE;
			}
		}
		break;
	case 852:
#line 8707 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 853:
#line 8714 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 854:
#line 8722 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_cp = yypvt[-4].st_name;
			u_ptr[1].u_cp = yypvt[-2].st_name;
			u_ptr[2].u_symp = yypvt[-0].st_sym;
			yyval.st_node = osmknode(PREDE, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 855:
#line 8729 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			register OSSYM	*sym;

			if (QUEL)
				oscerr(OSQUALID, 0);
			u_ptr[0].u_cp = yypvt[-2].st_name;
			u_ptr[1].u_cp = NULL;
			u_ptr[2].u_symp = yypvt[-0].st_sym;
			yyval.st_node = osmknode(PREDE, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 856:
#line 8741 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			register OSSYM  *sym;

			/* Get the symbol table entry, if possible */
			sym = osfld(FormSym, yypvt[-0].st_name, OSRHSUNDEF);
			if ( sym->s_kind == OSVAR )
				oscerr(OSHIDPRED, 1, yypvt[-0].st_name);
			else if ( sym->s_kind != OSFIELD )
				oscerr(OSBADQUAL, 1, yypvt[-0].st_name);
			yyval.st_sym = sym;
		}
		break;
	case 857:
#line 8753 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			register OSSYM  *sym;

			/* 
			** ostab will issue an error message if this isn't a
			** valid tablefield/column pair.
			*/
			sym = ostab(FormSym, yypvt[-2].st_name, yypvt[-0].st_name);
			if ( sym->s_kind == OSHIDCOL)
				oscerr(OSHIDCOLPRED, 2, yypvt[-2].st_name, yypvt[-0].st_name);
			yyval.st_sym = sym;
		}
		break;
	case 858:
#line 8768 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 859:
#line 8773 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 860:
#line 8795 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{ /* Note:  FORM was allowed */
			OSSYM	*idsym;

			IGstartStmt(osscnlno(), IL_LB_NONE);
			idsym = osqryobjchk(FormSym, yypvt[-0].st_node);
			if ( idsym != NULL )
			{
				FormObj = idsym;
				if ( FormObj != FormSym
					       && FormObj->s_kind != OSTABLE )
					QryTarget = yypvt[-0].st_node;
				yyval.st_node = iiosRefGen(yypvt[-0].st_node, TRUE /* assign */);
			}
			else
			{
				yyval.st_node = os_lhs(yypvt[-0].st_node);
				FormObj = osnodesym(yyval.st_node);
			}
		}
		break;
	case 861:
#line 8816 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_cp = yypvt[-0].st_name;
			u_ptr[1].u_cp = NULL;
			yyval.st_node = osmknode(ATTR, &u_ptr[0], &u_ptr[1], (U_ARG*)NULL);
			FormObj = NULL;
		}
		break;
	case 862:
#line 8823 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_cp = yypvt[-2].st_name;
			u_ptr[1].u_cp = yypvt[-0].st_name;
			yyval.st_node = osmknode(ATTR, &u_ptr[0], &u_ptr[1], (U_ARG*)NULL);
			FormObj = NULL;
		}
		break;
	case 863:
#line 8836 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{ /* Note:  FORM was allowed */
			yyval.st_node = osall(FormSym, yypvt[-2].st_node);
		}
		break;
	case 866:
#line 8853 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{AllowForm = FALSE;}
		break;
	case 867:
#line 8854 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = os_lhs(yypvt[-0].st_node);
		}
		break;
	case 868:
#line 8916 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 869:
#line 8920 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 870:
#line 8942 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (  yypvt[-0].st_node->n_token == VALUE
			   && yypvt[-0].st_node->n_sym->s_kind == OSTABLE )
			{  /* table field illegal in this context */
				oscerr (E_OS0268_BadTblFld, 1, 
					yypvt[-0].st_node->n_sym->s_name);
			}
			yyval.st_node = osval(yypvt[-0].st_node);
		}
		break;
	case 871:
#line 8953 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( FormSpecified )
			{
				ostrfree(yypvt[-0].st_node);
				yypvt[-0].st_node = osvalnode(FormSym, FormSym->s_name);
				FormSpecified = FALSE;
			}
			if (  yypvt[-0].st_node->n_token == VALUE
			   && yypvt[-0].st_node->n_sym->s_kind == OSTABLE )
			{  /* table field illegal in this context */
				oscerr (E_OS0268_BadTblFld, 1, 
			   		yypvt[-0].st_node->n_sym->s_name);
			}
			yyval.st_node = osval(yypvt[-0].st_node);
		}
		break;
	case 872:
#line 8975 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( !AllowForm || STbcompare( FormSym->s_name, 0,
							yypvt[-0].st_name, 0, TRUE) != 0 )
			{ /* form not allowed or not the form */
				yyval.st_node = osvalnode(FormSym, yypvt[-0].st_name);
				AllowForm = TRUE;
				FormSpecified = FALSE;
			}
			else
			{ /* special case:  form name */
				u_ptr[0].u_symp = FormSym;
				yyval.st_node = osmknode( VALUE, &u_ptr[0],
						(U_ARG *)NULL, (U_ARG *)NULL
				);
				yyval.st_node->n_flags |= N_READONLY;
				FormSpecified = TRUE;
			}
		}
		break;
	case 873:
#line 8994 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( yypvt[-2].st_node->n_token != VALUE || yypvt[-2].st_node->n_sym != FormSym )
			{
				if ( FormSpecified )
				{
					oscerr(E_OS025A_BadFormRef, 0);
					FormSpecified = FALSE;
				}
				yyval.st_node = osdotnode(FormSym, yypvt[-2].st_node, yypvt[-0].st_name);
			}
			else
			{ /* <form> . ID */
				OSSYM	*sym;

				if ( (sym = ossympeek(yypvt[-0].st_name, FormSym))
							!= NULL
						&& sym->s_kind == OSTABLE )
				{ /* <form> . <tblfld> */
					ostrfree(yypvt[-2].st_node);
					yyval.st_node = osvalnode(FormSym, yypvt[-0].st_name);
				}
				else if ( (sym =
					ossympeek(FormSym->s_name, FormSym))
							!= NULL
						&& sym->s_kind == OSTABLE )
				{ /* <tblfld> . <col> */
					ostrfree(yypvt[-2].st_node);
					yyval.st_node = osdotnode(FormSym,
							osvalnode(FormSym,
								FormSym->s_name
							), yypvt[-0].st_name
					);
					FormSpecified = FALSE;
				}
				else
				{
					oscerr( OSNOTBLFLD,
							2, yypvt[-0].st_name, yypvt[-2].st_node->n_sym->s_name
					);
					ostrfree(yypvt[-2].st_node);
					yyval.st_node = osvalnode(FormSym, yypvt[-0].st_name);
				}
			}
		}
		break;
	case 874:
#line 9039 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( yypvt[-1].st_node->n_token == VALUE && yypvt[-1].st_node->n_sym == FormSym )
			{ /* cannot be form */
				ostrfree(yypvt[-1].st_node);
				yypvt[-1].st_node = osvalnode(FormSym, FormSym->s_name);
				FormSpecified = FALSE;
			}
			yyval.st_node = osarraynode(yypvt[-1].st_node, yypvt[-0].st_node, FALSE);
		}
		break;
	case 875:
#line 9050 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osvalnode(FormSym, yypvt[-0].st_name);
		}
		break;
	case 876:
#line 9054 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osdotnode(FormSym, yypvt[-2].st_node, yypvt[-0].st_name);
		}
		break;
	case 877:
#line 9058 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osarraynode(yypvt[-1].st_node, yypvt[-0].st_node, TRUE);
		}
		break;
	case 878:
#line 9082 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 879:
#line 9086 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 880:
#line 9102 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;	/* special case */
		}
		break;
	case 881:
#line 9107 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (!oschkstr(yypvt[-1].st_node->n_type))
				oscerr(OSNOTSTR, 1, ERget(_FrsName));
			else if ( AFE_NULLABLE_MACRO(yypvt[-1].st_node->n_type) )
				oswarn(OSNULVCHK, 0);
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 882:
#line 9128 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 883:
#line 9132 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkconst(tkSCONST, yypvt[-0].st_name);
		}
		break;
	case 884:
#line 9136 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (yypvt[-1].st_node == NULL || !oschkstr(yypvt[-1].st_node->n_type))
				oscerr(OSNOTSTR, 1, ERget(_FrsName));
			else if ( AFE_NULLABLE_MACRO(yypvt[-1].st_node->n_type) )
				oswarn(OSNULVCHK, 0);
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 885:
#line 9152 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 886:
#line 9159 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 887:
#line 9171 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 888:
#line 9175 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-2].st_node;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(COMMA, (U_ARG*)NULL, &u_ptr[0], &u_ptr[1]);
		}
		break;
	case 889:
#line 9182 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-1].st_node;
			yyval.st_node = osmknode(PARENS, (U_ARG*)NULL, &u_ptr[0], (U_ARG*)NULL);
		}
		break;
	case 890:
#line 9187 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 891:
#line 9212 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 892:
#line 9216 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if (yypvt[-1].st_node == NULL || !ADE_CHK_TYPE(yypvt[-1].st_node->n_type, DB_INT_TYPE))
				oscerr(OSNOTINT, 0);
			else if ( AFE_NULLABLE_MACRO(yypvt[-1].st_node->n_type) )
				oswarn(OSNULVCHK, 0);
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 893:
#line 9232 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 894:
#line 9239 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 895:
#line 9247 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 896:
#line 9251 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( !oschkstr(yypvt[-1].st_node->n_type) )
				oscerr(OSNOTSTR, 0);
			else if ( AFE_NULLABLE_MACRO(yypvt[-1].st_node->n_type) )
				oswarn(OSNULVCHK, 0);
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 897:
#line 9261 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 898:
#line 9265 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 899:
#line 9285 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;	/* special case */
		}
		break;
	case 900:
#line 9290 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( !ADE_CHK_TYPE(yypvt[-1].st_node->n_type, DB_INT_TYPE) &&
					!oschkstr(yypvt[-1].st_node->n_type) )
				oscerr(OSNOTREF, 0);
			else if ( AFE_NULLABLE_MACRO(yypvt[-1].st_node->n_type) )
				oswarn(OSNULVCHK, 0);
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 901:
#line 9299 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 902:
#line 9306 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
    	    yyval.st_node = yypvt[-0].st_node;
        }
		break;
	case 903:
#line 9310 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
            u_ptr[0].u_nodep = yypvt[-2].st_node;
    	    u_ptr[0].u_nodep->n_type = DB_CHA_TYPE; /* see ingres_name def. */
            u_ptr[1].u_nodep = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
    	    u_ptr[1].u_nodep->n_type = DB_CHA_TYPE;  /* see ingres_name def. */
            yyval.st_node = osmknode(DOT|DML, (U_ARG*)NULL, &u_ptr[0], &u_ptr[1]);
        }
		break;
	case 904:
#line 9320 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 905:
#line 9324 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 906:
#line 9328 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 907:
#line 9334 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
    	    yyval.st_node = yypvt[-0].st_node;
        }
		break;
	case 908:
#line 9338 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
	    char    buf[OSBUFSIZE];

	    _VOID_ STprintf(buf, ERx("%s.%s"), yypvt[-2].st_name, yypvt[-0].st_name);
	    yyval.st_node = osmkident( iiIG_string(buf), (OSNODE *)NULL);
	    yyval.st_node->n_type = DB_CHA_TYPE;		/* special case */
        }
		break;
	case 909:
#line 9348 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 910:
#line 9352 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 911:
#line 9406 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			if ( RepeatState )
			{
				oswarn( E_OS015A_BadRepeat, 0 );
				RepeatState = FALSE;
			}
		}
		break;
	case 912:
#line 9419 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_nat = 1;
		}
		break;
	case 913:
#line 9423 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_nat = 0;
		}
		break;
	case 914:
#line 9437 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
			yyval.st_name = (yypvt[-1].st_nat == 0) ? _IsNull : _IsNotNull;
		}
		break;
	case 915:
#line 9442 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    if ( SQL )
		    {
			if (osw_compare(ERx("integer"), yypvt[-0].st_name))
				yyval.st_name = (yypvt[-1].st_nat == 0) ? _IsInteger : _IsNotInteger;
			else if (osw_compare(ERx("decimal"), yypvt[-0].st_name))
				yyval.st_name = (yypvt[-1].st_nat == 0) ? _IsDecimal : _IsNotDecimal;
			else if (osw_compare(ERx("float"), yypvt[-0].st_name))
				yyval.st_name = (yypvt[-1].st_nat == 0) ? _IsFloat : _IsNotFloat;
			else
				yyerror(ERx("IS datatype error"));
		    }
		    else
			yyerror(ERx("IS datatype error"));
		}
		break;
	case 916:
#line 9459 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/quel/quel.yf"
{
		    if ( SQL )
		    {
			if (osw_compare(ERx("integer"), yypvt[-1].st_name))
				yyval.st_name = (yypvt[-4].st_nat == 0) ? _IsInteger : _IsNotInteger;
			else if (osw_compare(ERx("decimal"), yypvt[-1].st_name))
				yyval.st_name = (yypvt[-4].st_nat == 0) ? _IsDecimal : _IsNotDecimal;
			else if (osw_compare(ERx("float"), yypvt[-1].st_name))
				yyval.st_name = (yypvt[-4].st_nat == 0) ? _IsFloat : _IsNotFloat;
			else
				yyerror(ERx("IS datatype error"));
		    }
		    else
			yyerror(ERx("IS datatype error"));
		}
		break;
		}
		goto yystack;  /* stack new state and value */

	}
