
#line 5 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#define yyparse sql
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
static bool	TableRef = FALSE;	/* SELECT target list table reference */
static OSNODE	*InsCols = NULL;	/* INSERT column name list */
static i4	InsNcols = 0;		/* Depth of above */

static bool	insert_all();
static OSNODE	*insert_column();
static OSNODE	*expand_insert();
static VOID	open_targ_IL();
static VOID	close_lhs_targ_IL();
static VOID	close_rhs_targ_IL();

VOID      IIOSgqsGenQryStmt();


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

#line 584 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
#define YACC	1
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
#define	ALTER_DEFAULT		300
#define	ALTER_PROFILE		301
#define	ALTER_TABLE		302
#define	ALTER_USER		303
#define	ALTER_LOCATION		304
#define	ALTER_SECAUDIT		305
#define	BEGINTRANSACTION		306
#define	COMMIT		307
#define	COPY		308
#define	CREATE		309
#define	CRE_DBEVENT		310
#define	CRE_PROFILE		311
#define	CRE_USER		312
#define	CRE_SECALARM		313
#define	CRE_LOCATION		314
#define	CURRENT_INST		315
#define	DELETE		316
#define	DBMS_PASSWORD		317
#define	DISABLE_SECAUDIT		318
#define	DROP		319
#define	DRP_DBEVENT		320
#define	DRP_PROFILE		321
#define	DRP_VIEW		322
#define	DRP_PROC		323
#define	DRP_LINK		324
#define	DRP_LOCATION		325
#define	DRP_SECALARM		326
#define	DRP_USER		327
#define	ENABLE_SECAUDIT		328
#define	ENDTRANSACTION		329
#define	EXECUTE_IMMED		330
#define	GET_DBEVENT		331
#define	GRANT		332
#define	INDEX		333
#define	CRE_INTEGRITY		334
#define	INSERT		335
#define	MODIFY		336
#define	CRE_PERMIT		337
#define	RAISE_DBEVENT		338
#define	REFERENCING		339
#define	REGISTER		340
#define	REGISTER_DBEVENT		341
#define	RELOCATE		342
#define	REMOVE_DBEVENT		343
#define	DRP_INTEGRITY		344
#define	DRP_PERMIT		345
#define	ROLLBACK		346
#define	SAVE		347
#define	SAVEPOINT		348
#define	SELECT		349
#define	SET		350
#define	TABLE		351
#define	UNION		352
#define	UPDATE		353
#define	CRE_VIEW		354
#define	GLOB_TEMP		355
#define	CRE_SYNONYM		356
#define	DRP_SYNONYM		357
#define	FROM_GROUP		358
#define	FROM_ROLE		359
#define	FROM_USER		360
#define	TO_GROUP		361
#define	TO_ROLE		362
#define	TO_USER		363
#define	CONNECT		364
#define	DISCONNECT		365
#define	IDENTIFIED_BY		366
#define	ANY		367
#define	ASC		368
#define	AT		369
#define	BETWEEN		370
#define	BY		371
#define	DEFAULT		372
#define	DESC		373
#define	DISTINCT		374
#define	ESCAPE		375
#define	EXISTS		376
#define	FOR		377
#define	FROM		378
#define	GROUP		379
#define	HAVING		380
#define	IN		381
#define	INTO		382
#define	IS		383
#define	JOIN		384
#define	OF		385
#define	ON		386
#define	ORDER		387
#define	SOME		388
#define	TO		389
#define	UNIQUE		390
#define	UNTIL		391
#define	VALUES		392
#define	WHERE		393
#define	WORK		394
#define	INNER_JOIN		395
#define	FULL_JOIN		396
#define	LEFT_JOIN		397
#define	RIGHT_JOIN		398
#define	ON_COMMIT		399
#define	ON_DATABASE		400
#define	ON_LOCATION		401
#define	BY_USER		402
#define	BY_GROUP		403
#define	BY_ROLE		404
#define	CHECK		405
#define	CONSTRAINT		406
#define	FOREIGN_KEY		407
#define	PRIMARY_KEY		408
#define	REFERENCES		409
#define	COMMENT_ON		410
#define	ADD		411
#define	ALTER_GROUP		412
#define	ALTER_ROLE		413
#define	CRE_GROUP		414
#define	CRE_ROLE		415
#define	CRE_RULE		416
#define	DRP_GROUP		417
#define	DRP_ROLE		418
#define	DRP_RULE		419
#define	NEW_AS		420
#define	OLD_AS		421
#define	REVOKE		422

#line 777 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
#define STARTSTMTOW(string) { osOpenwarn(ERx(string)); IGstartStmt(osscnlno(),IL_LB_NONE); }

/* Undefine these for some systems (e.g., Sun) */
#ifdef MAX
#undef MAX
#endif
#ifdef MIN
#undef MIN
#endif

#line 925 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#define	AVG		423
#define	COUNT		424
#define	MAX		425
#define	MIN		426
#define	SUM		427
#define	UNION_OP		428
#define	AS		429
#define	BELL_TOK		430
#define	OFF		431
#define	OWNER_INGRES		432
#define	USER_AUTH		433
#define	SYSTEM_USER		434
#define	SESSION_USER		435
#define	SESSION_GROUP		436
#define	SESSION_ROLE		437
#define	ADD_PRIVILEGES		438
#define	DROP_PRIVILEGES		439
#define	DIR_CONNECT		440
#define	DIR_DISCONNECT		441
#define	DIR_EXECUTE		442
#define	IMMEDIATE		443
#define	REG_TABLE		444
#define	REM_TABLE		445
#define	REG_VIEW		446
#define	REM_VIEW		447
#define	REMOVE		448
#define	CLEAR		449
#define	HELPFILE		450
#define	HELP_FORMS		451
#define	MESSAGE		452
#define	NOECHO		453
#define	PRINTSCREEN		454
#define	PROMPT		455
#define	REDISPLAY		456
#define	RESUME		457
#define	RESENTRY		458
#define	RESNEXT		459
#define	RESMENU		460
#define	RESNFLD		461
#define	RESPFLD		462
#define	SCREEN		463
#define	SCROLL		464
#define	SLEEP		465
#define	VALIDATE		466
#define	INQ_FORMS		467
#define	INQ_INGRES		468
#define	SET_FORMS		469
#define	SET_INGRES		470
#define	INQ_4GL		471
#define	SET_4GL		472
#define	SET_CONNECTION		473
#define	SET_RANDOM_SEED		474
#define	CLEARROW		475
#define	DELETEROW		476
#define	INITTABLE		477
#define	INSERTROW		478
#define	VALIDROW		479
#define	LOADTABLE		480
#define	UNLOADTABLE		481
#define	PURGETBL		482
#define	COLID		483
#define	ID		484
#define	FCONST		485
#define	ICONST		486
#define	SCONST		487
#define	XCONST		488
#define	DCONST		489
#define	AND		490
#define	EXP		491
#define	GTE		492
#define	LIKE		493
#define	LTE		494
#define	NOT		495
#define	NOTLIKE		496
#define	NOTEQ		497
#define	OR		498
#define	LSQBRK		499
#define	RSQBRK		500
#define	COLEQ		501
#define	LBOP		502
#define	ROP		503
#define	BOP		504
#define	BOPH		505
#define	UOP		506
#define	UOP_POSTFIX		507
#define	yyclearin	yychar = -1
#define	yyerrok		yyerrflag = 0
extern int		yychar;
extern short		yyerrflag;
#ifndef YYMAXDEPTH
#define	YYMAXDEPTH	150
#endif
YYSTYPE			yylval, yyval;
#define	YYERRCODE	256

#line 12445 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"


/* %L Routines */
/* %L Routines begin */

/*
** Name:	insert_all() -	Check INSERT Name List of Unexpanded "*".
**
** Description:
**	Searches through the column name list (which is a tree of COMMA nodes)
**	of an INSERT statement looking for any unexpanded "*" nodes (which
**	should have been a VALUES clause list with a ".all" object in it
**	corresponding to the "*" node rather than a sub-query object.)
**
** Input:
**	list	{OSNODE *}  Column name list for INSERT.
**
** Returns:
**	{bool}	TRUE if unexpanded "*" node found.
**		FALSE if none found.
**
** History:
**	06/87 (jhw) -- Written.
*/
static bool
insert_all (list)
register OSNODE *list;
{
	if (list->n_token == COMMA)
		return (bool)(insert_all(list->n_left) && insert_all(list->n_right));
	else
		return (bool)(list->n_token != tkID || *list->n_value != '*');
}

/*
** Name:	insert_column() -	Search for Column Name Element at Depth.
**
** Description:
**	Traverses the column name list (which is a tree of COMMA nodes) of an
**	INSERT statement for the column name element at the input depth.  This
**	is used to return the column name element corresponding to an element in
**	the VALUES clause list of the INSERT (by order.)  (The first element is
**	at the maximum depth for the tree; the last is at depth 1.)
**
** Input:
**	list	{OSNODE *}  Column name list for INSERT.
**	depth	{nat}  Depth of desired column element.
**
** Returns:
**	{OSNODE *}  The column name element at 'depth' in the list.
**
** History:
**	06/87 (jhw) -- Written.
*/
static OSNODE *
insert_column (list, depth)
register OSNODE *list;
register i4	depth;
{
	if (depth == 1)
		return list->n_token == COMMA ? list->n_right : list;
	else
		return insert_column(list->n_left, --depth);
}

/*
** Name:	expand_insert() -	Expand the ".all" Object in an Insert.
**
** Description:
**	This routine expands the (NLIST) value list for a ".all" object found in**	a VALUE clause of an INSERT statement into a (COMMA) name list and value
**	list.  The name list replaces the corresponding "*" constant node in the
**	column list (by reference.)  The original (NLIST) value list is freed,
**	(but its elements were transfered to the (COMMA) value list.)  The new
**	value list is returned.
**
** Input:
**	list	{OSNODE *}  A NLIST of the values for a ".all" in an INSERT.
**
** Output:
**	node	{OSNODE *}  The "*" constant node from the INSERT column list.
**				Replaced by the expanded name list.
**
** Returns:
**	{OSNODE *}  A COMMA list of the values.
**
** History:
**	06/87 (jhw) -- Written.
**	31-aug-1993 (mgw)
**		Fixed casting of MEcopy() args for prototyping.
*/
static OSNODE *
expand_insert (list, node)
OSNODE	*list;
OSNODE	*node;
{
	register OSNODE	*lp;
	U_ARG		nlst;
	U_ARG		vlst;

	if (list == NULL)
		return NULL;

	for (lp = list ; lp != NULL ; lp = lp->n_next)
	{
		register OSNODE *ele = lp->n_ele;
		U_ARG	name;
		U_ARG	value;

		name.u_nodep = osmkident(ele->n_tfref == NULL
						? ele->n_sym->s_name
						: ele->n_tfref->s_name,
					(OSNODE *)NULL
			);
		value.u_nodep = ele;
		nlst.u_nodep = (list == lp) ? name.u_nodep
					: osmknode(COMMA, (U_ARG*)NULL, &nlst, &name);
		vlst.u_nodep = (list == lp) ? value.u_nodep
					: osmknode(COMMA, (U_ARG*)NULL, &vlst, &value);
		lp->n_ele = NULL;	/* re-used above, save by NULLing */
	}
	/* Free (NLIST) value list */
	ostrfree(list);	/* elements save above */

	/* Copy (COMMA) name list into node */
	MEcopy((PTR)nlst.u_nodep, (u_i2)sizeof(*node), (PTR)node);
	nlst.u_nodep->n_left = nlst.u_nodep->n_right = NULL;
	ostrfree(nlst.u_nodep);

	/* Return new (COMMA) value list */
	return vlst.u_nodep;
}

/*
** Name:	open_targ_IL() -	Open IL fragment for SELECT target
**
** Description:
**	If we're in the target list of a SELECT statement (before any UNION),
**	this routine will open a fragment of IL which will represent either
**	the lhs or the rhs of a SELECT target item (we don't know which yet).
**
**	We also begin a new temp block, which will be merged into an enclosing
**	temp block by the matching close_lhs_targ_IL or close_rhs_targ_IL.
**
** History:
**	09/20/92 (emerson)
**		Written (for bug 34846).
*/
static VOID
open_targ_IL()
{
	if (QryState)
	{
		ostmpbeg();
		iiIGofOpenFragment();
	}
}

/*
** Name:	close_lhs_targ_IL() -	Close IL fragment for SELECT target lhs
**
** Description:
**	If we're in the target list of a SELECT statement (before any UNION),
**	this routine will close a fragment of IL which represents the lhs
**	of a SELECT target item and include it in TargListFrag (the fragment
**	of IL that will compute all lhs's of the current target list).
**
**	We also merge the temp block that we began when opened the IL fragment
**	into the enclosing (parent) temp block, which represents the lhs's
**	of target items.
**
**	Before doing the above merge, we "freeze" all temporaries that were
**	freed by nested temp blocks.  This prevents subsequent rhs IL from
**	reusing any of these temps.  (That could cause problems when we're
**	allocating a temp in outermost temp block associated with the SELECT,
**	because the lhs IL may get executed after the rhs IL even when it was
**	generated before).
**
** History:
**	09/20/92 (emerson)
**		Written (for bug 34846).
*/
static VOID
close_lhs_targ_IL()
{
	PTR	targ_item_frag;

	if (QryState)
	{
		ostmpfreeze();
		ostmppromote(1);
		targ_item_frag = iiIGcfCloseFragment();
		iiIGrfReopenFragment(TargListFrag);
		iiIGifIncludeFragment(targ_item_frag);
		TargListFrag = iiIGcfCloseFragment();
	}
}

/*
** Name:	close_rhs_targ_IL() -	Close IL fragment for SELECT target rhs
**
** Description:
**	If we're in the target list of a SELECT statement (before any UNION),
**	this routine will close a fragment of IL which represents the rhs
**	of a SELECT target item and include it in the "default" fragment.
**
**	We also merge the temp block that we began when opened the IL fragment
**	into the "grandparent" temp block (the 2nd-levl enclosing temp block).
**	This "grandparent" temp block represents the rhs's of target items,
**	plus temporaries that span the entire SELECT.
**
** History:
**	09/20/92 (emerson)
**		Written (for bug 34846).
*/
static VOID
close_rhs_targ_IL()
{
	PTR	targ_item_frag;

	if (QryState)
	{
		ostmppromote(2);
		targ_item_frag = iiIGcfCloseFragment();
		iiIGifIncludeFragment(targ_item_frag);
	}
}

/*
** Name:	connect_clauses() -	Emit IL_TL2ELMs for connect clause
**
** Description:	Traverse a tree of COMMA and TLASSIGN nodes, emitting
**		the IL_TL2ELM statements for the connect clauses in a
**		CONNECT or DISCONNECT statement.
**
** Input:
**	list	{OSNODE *}  A COMMA list or single TLASSIGN node.
**
** Output:
**	NONE.
**
** Returns:
**	NONE.
**
** History:
**	09/92 (davel) -- Written.
*/
static void
connect_clauses (list)
OSNODE	*list;
{
	switch (list->n_token)
	{
		case COMMA:
			connect_clauses(list->n_left);
			connect_clauses(list->n_right);
			ostrfree(list);
			break;

		case TLASSIGN:
			ostl2elm(list);
			/* ostl2elm() does its own ostrfree() */
			break;
		default:
			osuerr(OSBUG, 1, ERx("connect_clauses(default)"));
	}
}

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
	349, 471,
	481, 174,
	-2, 128,
	-1, 90,
	262, 201,
	263, 201,
	266, 174,
	274, 201,
	291, 174,
	297, 174,
	349, 471,
	481, 174,
	-2, 128,
	-1, 122,
	61, 68,
	44, 68,
	-2, 116,
	-1, 243,
	483, 1081,
	484, 1081,
	486, 1081,
	487, 1081,
	45, 1081,
	-2, 1077,
	-1, 464,
	262, 201,
	263, 201,
	266, 174,
	274, 201,
	291, 174,
	297, 174,
	349, 471,
	481, 174,
	-2, 128,
	-1, 470,
	349, 471,
	-2, 200,
	-1, 516,
	40, 1366,
	-2, 1324,
	-1, 520,
	40, 1333,
	-2, 1327,
	-1, 521,
	40, 1334,
	-2, 1272,
	-1, 535,
	46, 1359,
	-2, 1333,
	-1, 727,
	349, 471,
	-2, 173,
	-1, 813,
	46, 1364,
	-2, 1333,
	-1, 989,
	393, 1335,
	483, 1335,
	484, 1335,
	487, 1335,
	-2, 1354,
	-1, 1058,
	389, 331,
	-2, 336,
	-1, 1094,
	61, 90,
	44, 90,
	40, 90,
	-2, 120,
	-1, 1151,
	40, 1333,
	-2, 1327,
	-1, 1152,
	40, 1334,
	-2, 1272,
	-1, 1153,
	40, 1366,
	-2, 1324,
	-1, 1275,
	393, 1336,
	483, 1336,
	484, 1336,
	487, 1336,
	-2, 1355,
	-1, 1452,
	40, 1028,
	-2, 1034,
	-1, 1498,
	370, 1027,
	381, 1027,
	491, 1027,
	492, 1027,
	493, 1027,
	494, 1027,
	495, 1027,
	496, 1027,
	497, 1027,
	60, 1027,
	62, 1027,
	61, 1027,
	43, 1027,
	45, 1027,
	42, 1027,
	47, 1027,
	-2, 1366,
	-1, 1615,
	61, 498,
	40, 1028,
	-2, 1034,
	-1, 1618,
	61, 499,
	-2, 1325,
	-1, 1927,
	40, 1028,
	-2, 1327,
};

#define	YYNPROD		1372
#define	YYLAST		4395

short yyact[] = {
	   512,  2185,  2136,  2127,  1620,  2069,  1222,  2102,  1006,  2036,
	  2128,  2141,  1678,   632,  1148,  1080,  1187,  1966,  1431,  1914,
	  1393,  1892,  1455,  1352,  2027,  2002,   836,  1616,  1580,  1144,
	  1738,  1428,  1724,   150,  1633,  1145,  1639,  1890,   913,  1449,
	    90,  1752,   718,  1162,  1614,   485,  1036,  1747,   495,  1915,
	  1556,  1427,  1542,   571,  1457,  1528,   937,   593,  1519,  1334,
	   505,  1435,  1888,  1456,  1220,  1516,   877,   610,  1387,   669,
	  1451,  1112,  1876,  1675,  1192,  1110,  1642,  1408,  1641,  1029,
	   712,  1335,  1304,    88,  1341,   819,  1284,  1305,  1040,  1751,
	  1283,  1541,  1194,   837,  1166,  1000,  1236,   515,  1202,  1079,
	  1207,  1425,  1143,  1111,   892,  1916,  1193,   886,  1424,  1441,
	   885,  1423,   446,  1491,  1185,  1422,  1181,  1142,  1588,  1131,
	  1066,    63,  1034,  1052,   787,   511,  1028,  1041,  1053,   123,
	    43,    86,  1023,  1042,   117,   970,   646,  1001,   572,   914,
	   881,    11,   909,   878,  1141,   802,   940,  1343,  1300,   637,
	   948,   614,    44,   950,   654,  1285,   867,   583,   600,   858,
	   470,  1125,   335,   545,    62,    14,  1073,   147,   139,    10,
	  1051,  1031,  1014,   678,   936,   624,  1687,  1027,   621,   242,
	   848,   857,   472,   820,  2072,   510,  1345,  1859,   825,   827,
	    84,   828,  1122,   826,  1392,  2045,   496,  1374,  1815,  1353,
	  1429,  1104,    73,  1996,  1860,   102,  1113,  1115,  1114,   105,
	    77,  1105,  1103,   439,  1686,  2131,  2042,  2011,  1354,  1106,
	  1693,   508,   824,   494,   506,  1344,   509,  2058,   612,  1367,
	   814,  1055,  1946,   477,  1853,  1823,   458,   502,   459,  2082,
	   104,  1827,  1828,   106,   439,   533,   533,    55,   543,   544,
	  2000,   543,  1429,   533,   559,   761,   760,   569,   533,   762,
	   543,   577,   578,   543,   533,   585,   533,   587,   825,   473,
	  1994,  1978,   478,   826,  1430,   543,   533,   616,   533,   513,
	   518,   506,   813,    29,   504,    30,    30,   533,   449,   659,
	   660,   533,   889,   506,   665,   449,   675,   692,   506,    29,
	   533,   533,    30,  1674,  1857,  2052,   696,  1432,   700,   556,
	   890,  1437,   991,   116,  1684,   700,  2089,   825,   827,  2090,
	   828,   588,   826,   479,   480,   720,  1430,   707,  1558,  1622,
	   725,   825,   827,  2157,   828,   825,   826,   671,   982,    57,
	   826,   120,   666,   825,   827,   732,   828,   517,   826,   671,
	   825,   522,  2089,   825,   827,   826,   828,  2122,   826,   506,
	    29,   825,  1973,    30,   693,   693,   826,   608,   721,  1936,
	  1286,  2198,   693,   693,   506,    29,   825,   827,    30,   828,
	  2196,   826,  1589,   825,   827,  2024,   828,   823,   826,   693,
	  1683,  2060,  2105,  2104,  1113,  1115,  1114,  1436,  2121,   825,
	   827,  2060,   828,  1907,   826,   745,   589,   524,  1908,   506,
	  2075,  1357,  2130,   714,   726,  1355,  1627,   560,   825,   827,
	  1105,   828,    29,   826,  2059,    30,  1591,  1590,  1106,  1436,
	  1846,  1854,  1824,  1438,   729,  1105,  1113,  1115,  1114,   506,
	   525,   234,   234,  1106,  2186,  1521,   865,   448,   722,  1436,
	  2099,   825,   827,  1654,   828,   682,   826,  1354,   628,   554,
	  2098,   825,   827,  1159,   828,   518,   826,  2173,   537,  2097,
	   825,   827,    13,   828,    29,   826,   767,    30,    68,   506,
	   525,   453,   452,   450,   451,   454,   555,   693,  1094,  1673,
	   917,    31,    24,    26,    25,   825,   827,    23,   828,   811,
	   826,    28,  1681,  1673,   782,  1681,    33,   671,  1859,   825,
	   827,  2153,   828,  1673,   826,   816,  1700,  1967,   123,  1906,
	   769,   768,   516,   790,  2028,   825,   827,  1698,   828,   797,
	   826,   506,    29,  2101,   447,    30,   443,   445,  1012,   807,
	    29,   920,   923,    30,  1358,   832,  1891,   825,   827,   822,
	   828,  2078,   826,  2071,   821,   919,   596,  1582,   611,  2063,
	   918,   604,   922,  1739,   533,   565,   516,   548,   549,   864,
	   798,   799,   883,   774,   770,   801,   766,   773,   834,   668,
	  1988,  1139,   825,   827,  1135,   828,  1643,   826,   921,   883,
	   234,   533,  1605,   916,   753,   506,    29,  1606,   516,    30,
	   845,   846,    27,  1983,   611,    56,   803,   536,   516,  1981,
	   516,   509,   506,    29,  1968,   879,    30,  1911,   926,   533,
	   862,   671,   471,   537,  1626,   764,   533,   533,   825,   827,
	   474,   828,   910,   826,   506,  1003,   496,  1693,  1117,  1120,
	  1116,  1699,  1121,  1118,   788,   513,   585,   496,   496,   533,
	   533,  1814,  1105,  1105,  1355,   908,  1841,   671,   506,   535,
	  1106,  1106,    30,  1105,   506,   520,   453,   452,   521,   451,
	   454,  1106,   765,   986,   992,   993,   496,   927,  1105,  1007,
	   806,   808,   925,   809,  1635,   815,  1106,    72,    71,   671,
	  1158,   543,   506,    29,   911,  1826,    30,   929,   566,    22,
	    29,   506,  1989,    30,  1700,  1990,   833,   506,   825,   827,
	  1019,   828,  1038,   826,   847,  1698,  1812,  2042,  1087,  1256,
	   983,  1059,   825,   827,  1061,   828,  1063,   826,   453,   452,
	   450,   451,   454,   998,   999,   453,   452,   450,   451,   454,
	  2114,  1017,   821,  1167,   861,  2115,   912,   506,   525,   453,
	   452,   450,   451,   454,   981,   516,   895,   901,  1768,  1119,
	   628,  1065,   536,  1130,   506,    29,  2011,   123,    30,   506,
	    29,   995,   996,    30,   932,   506,    29,   516,   670,    30,
	  2042,   516,  1025,  1744,  2011,  1637,   516,   506,    29,  1182,
	   670,    30,  1693,   516,   516,   952,  1628,   953,  1496,  1693,
	  1484,   938,  2011,   954,  1406,   957,  1060,   431,   609,  1482,
	   824,  1010,  1011,   506,   535,   668,   234,    30,   767,  1132,
	   506,    29,  1406,   611,    30,  1693,  1117,  1120,  1116,  1699,
	  1121,  1118,  1693,  1749,  1093,   113,  1380,  1327,  1009,   123,
	   123,  1085,   123,  1160,   607,   525,   883,   883,  2042,   949,
	   518,   997,   729,   114,   501,   506,    29,  1750,  1173,    30,
	   506,    29,   769,   768,    30,  1157,   782,   824,  1117,  1120,
	  1116,  1178,  1121,  1118,  1847,   671,  1186,  1439,  1033,  1176,
	  1195,  1273,  1030,   883,  1636,  1154,   506,    29,  1056,  1444,
	    30,   701,   928,   902,  1191,  1064,   524,  1062,  1077,  1135,
	  2011,  1231,  1955,   533,  1008,   969,   533,  1877,   978,   900,
	  2011,  1195,  1161,  1163,  1086,   838,   770,  1074,   766,  2011,
	    72,    71,  1084,   806,  1216,  1218,  1197,   110,   733,  1748,
	    44,   533,   493,   456,  1817,   455,  1237,   673,   927,  1199,
	  1200,  1201,   533,  1232,  2011,   506,  1099,   753,   670,  1108,
	  1109,  1188,  1249,  1172,  1095,    61,   585,   990,  1693,  1255,
	   533,  1096,  1097,  1177,   973,  1259,  1004,  1262,  1088,  1089,
	  1090,   506,    29,  2013,  1693,    30,  1682,  1128,   481,  1102,
	  1092,   516,  1217,   972,  2135,   533,   735,  1272,   975,  1133,
	    51,   674,  1165,   109,  1205,  1674,  1693,   971,  1204,   883,
	   883,   974,   927,  2110,  1133,  1238,   506,   594,   533,   690,
	   595,  1239,   748,  1239,  1226,   573,  1290,  1219,  1171,  1170,
	  1526,   506,    29,  1175,   574,    30,  1228,  1268,  1269,  1287,
	   513,   824,   592,  1290,   506,  1437,   849,   516,   612,   548,
	   549,   506,    29,   506,    29,    30,  1018,    30,  1221,   506,
	   520,   453,   452,   521,   451,   454,   785,   976,  1134,   506,
	    29,   804,   670,    30,  1683,  1276,  1277,   973,  1169,   516,
	   506,  1196,   585,  1134,    30,   780,  1275,  2042,   574,   234,
	   506,  1668,  1388,  1672,    30,   516,   972,   749,   516,   516,
	  1280,   975,  1317,  1245,  1235,   506,    29,  1247,   670,    30,
	   971,   123,  1168,  1289,   974,  1359,  1328,   516,   516,   516,
	   791,  1961,   854,  1250,  1263,  1257,  1153,  1336,  1258,  1309,
	  1265,  1339,  1339,  1950,  1949,   731,   123,   506,  1266,   746,
	   670,  1351,   977,  1048,   506,  1039,   772,   113,    30,  1046,
	   124,   568,  1362,  1363,   611,   449,  1465,  1223,   681,  1787,
	    96,  1540,  1274,   928,   611,   114,  1342,  1693,  1466,  1501,
	   976,   868,  1931,   574,   873,  1331,   872,  1396,  1333,  1402,
	  1403,   824,  1221,  1394,  1395,   980,  1681,   647,  1399,  1391,
	  1400,  1951,  1306,  1150,   738,    53,  1329,  1330,  1332,   883,
	   506,    29,   841,   883,    30,  1673,   689,   620,   889,  2178,
	   883,   883,   883,  1445,   853,   506,    29,   630,  1049,    30,
	  1371,  2003,  1389,   884,   888,   891,   890,   928,   883,  1772,
	    29,   629,  2160,    30,  1360,   916,   927,  1361,  1445,  1337,
	   604,   437,  1418,  1050,   874,   977,  1420,  1765,    44,    44,
	   533,  1325,   543,   543,   543,   543,  1375,  1365,  1366,   979,
	   964,   525,   686,  1520,  1102,   551,  1517,  1930,  1445,   603,
	   958,  1488,  1530,  1364,   668,   668,  1468,  1469,  1470,   605,
	   945,  1378,   531,  1409,  1543,  1543,  1102,   687,   636,  2126,
	  1537,  1485,  1557,   685,   840,  1102,  1102,   533,   548,   549,
	  1960,   506,    29,  1545,  1546,    30,  1504,   869,   623,  1515,
	  1763,  1434,  1513,  1720,  1433,  1570,   591,  1471,   966,  1503,
	   649,  1584,  1119,   506,    29,  1664,   670,    30,   904,  1663,
	   860,  1494,  1583,  1047,   565,   889,  1416,  1551,  1493,   668,
	  1419,  1492,  1417,  1497,  1719,  1721,  1722,   640,  1440,   927,
	   884,    29,   891,   890,    30,   506,  1452,   453,   452,   450,
	   451,   454,   871,    19,   668,  1486,   639,  1489,   668,   668,
	  1487,   642,  1500,  1522,  1499,  1613,  1544,  1253,  1598,  1792,
	   638,   942,   731,  1025,   641,   691,   516,  1566,  1295,   645,
	  1625,   648,  2137,  1132,  1465,   944,   946,   516,  2070,   961,
	   960,  1523,  1567,   962,  2050,   516,  1466,  1372,   543,   543,
	  1536,  1881,  1315,  1267,   574,   859,   671,   730,  1939,  1561,
	  1618,  1153,  1882,  1474,  1186,  1667,  1547,  1650,   647,  1195,
	  1562,  1563,  1564,  1565,  1573,  1652,  1571,  1574,  1572,  1007,
	   643,  1331,   590,   905,   516,  1634,   533,  1999,  1998,  1587,
	  1585,   928,  1596,   129,   129,  1980,  1593,  1445,  1445,  1242,
	  1243,  1244,  1601,  1677,  1600,   763,   776,   555,  1979,  1661,
	  1370,   795,  1884,  1082,   611,  2138,  1069,   688,  1934,   734,
	   939,   592,  1621,  1602,   533,   742,  1071,  1241,  1150,  1281,
	   543,  1210,  1629,  1467,  1472,  1708,  1709,  1710,  1653,  1195,
	  1649,  1794,  1680,  1630,  1631,  2003,  1632,  1078,  1223,  1657,
	  1208,  1150,  1645,   542,  1237,   644,   933,  1651,  1498,   647,
	  1517,   754,  1756,  1753,   533,   576,  1695,  1896,  1209,  1899,
	  1694,  1669,   739,    96,  1670,   731,   543,  1660,  1665,  1434,
	   613,  1195,  1433,   543,   543,   543,   543,  1671,  1467,  1755,
	   741,  1543,  1746,  1102,   653,   731,  1454,   737,  1779,  1701,
	   750,  1072,  1689,  1690,   928,  1785,  1887,  1533,  1534,  1535,
	   533,   533,  1786,  1004,   668,   736,   893,  1067,   955,   663,
	  1290,  1454,   927,  1238,   743,  1712,   516,  1532,  1290,   740,
	  1081,   843,   844,   506,  1745,   453,   452,   450,   451,   454,
	   699,  1741,  1494,   516,  1742,  1219,   934,   856,  1372,  1493,
	  1754,  1454,  1492,   658,  1743,   775,   934,   927,   516,   640,
	   516,  1290,   658,   592,   896,   897,   898,  1759,   527,   547,
	  1810,  1607,   656,  1773,   906,  1246,  1774,  1845,   639,  1035,
	   924,   656,  1781,   642,  1467,  1780,   655,   528,  1539,  1784,
	   657,  1506,   638,  1320,  1302,   655,   641,  1791,  2003,   657,
	  1790,  1336,   777,   778,   779,   529,   681,  1795,   540,   546,
	  1798,   933,  1832,   744,  1797,  1805,    42,   821,   516,   516,
	   516,   933,  1153,   567,  1153,  1372,  1843,  1887,   431,     8,
	   695,  1993,  1624,  1623,  1445,  1806,   794,   705,   706,  1342,
	   611,  1829,  1445,  1807,  1808,  1677,   668,  1454,  1809,   476,
	  1331,    38,   643,  1612,   724,  1819,   597,  1831,  1212,   943,
	   723,   711,  1561,  2108,    94,  1676,   883,    50,  1836,  1833,
	  1837,  1834,   550,  1562,  1563,  1564,  1565,  1467,  1467,   759,
	   500,   499,  1887,  1467,  1852,   120,  1878,  1842,   120,  1150,
	   121,  1150,   498,  1070,    91,    65,  1850,  1477,   121,    47,
	   602,  1478,   598,  1409,  1476,  1475,  2192,   238,    67,  1872,
	  1695,  2021,  1520,  1873,  1694,  1867,  1935,  1885,  1883,  2149,
	  2068,  1530,  1894,  1921,    69,  2064,  2022,   644,   448,  1816,
	  1803,  1924,  1900,  1543,   731,   668,   668,   928,  1557,  1851,
	  1454,  1454,  1945,    92,  1886,   731,  1454,  1858,  1901,  1789,
	    65,  1788,   751,  1783,  1921,  1944,  1903,  1904,   731,  1610,
	  1318,  1336,  1902,  1926,  1578,  1319,  1414,  1969,   129,  1922,
	  1928,   121,   928,  1932,  1004,  1943,  1940,  1411,  1878,  1405,
	  1941,  1948,  1692,  1830,   668,  1947,    98,  1385,  1953,  1956,
	  1384,  1383,   438,  1962,   506,    29,  1323,   670,    30,  1313,
	  1957,    97,   440,   883,  1308,  1987,  1925,  1972,  1971,    49,
	   123,  1954,  1985,   516,  1298,  1976,  1986,  1650,  1984,  1294,
	  1734,   441,  1652,  1155,  1618,   894,  1068,  1291,   489,   490,
	   484,    64,   119,   796,  1164,   119,  1279,   120,   668,  1732,
	  1225,  1179,  1156,  1098,    83,   533,  1997,   457,  1083,   789,
	   533,  2008,  2026,  1731,  1467,   708,  2006,  1733,   463,   462,
	   103,    99,    78,  2037,    66,  1878,   680,  1878,  1735,  1736,
	    95,    45,   107,    98,    98,  1878,  1878,   785,  1878,  1203,
	    87,   916,   916,  2031,  2030,  2051,    64,   122,    97,    97,
	   533,  1924,  1924,  2161,   611,   122,  1013,   516,  2007,   727,
	  2012,  2025,  2034,  2197,  1100,  2150,   679,  1676,  2016,  2017,
	    15,  2018,  2057,  2139,  1467,  1445,   664,  1454,  1982,  2047,
	  2048,   821,  1467,  1926,  1926,  2056,  1467,  1467,  1467,  1467,
	  1467,  1467,  1467,  1835,  1825,   579,  1174,   631,   619,  1658,
	  1310,  1214,  1467,  2065,  1691,  1878,   523,  1127,    85,   835,
	  2079,  2077,   553,  2074,   503,  1692,   821,   563,   101,    80,
	    20,  1394,  1395,    39,  2083,    20,  1925,  1925,   122,  2086,
	   448,  1986,    52,    54,  2043,  2044,   100,  1454,  2061,  2190,
	  2087,  2088,  2191,  2181,   119,  1454,  2076,  1224,  2116,  1454,
	  1454,  1454,  1454,  1372,  1454,  1454,  2106,    76,   668,  1878,
	  1878,  1878,  2109,    20,  1921,  1454,  2111,   596,  2100,  2117,
	  1603,  1190,  1924,  1924,  1924,  1924,  1970,  1549,   668,  1924,
	  2062,  1189,   671,  1467,  1190,  2129,  1692,  2170,  2132,  2107,
	  2171,  1782,  2092,  2093,  2094,  2133,  2112,  1278,   842,  2113,
	   115,  1692,  1821,  2140,  1926,  1926,  1926,  1926,  1467,   516,
	  1922,  1926,  2066,   633,  2038,  2067,  1923,  2039,   883,   533,
	  1150,  1800,  2019,  1796,  2037,  2020,  2134,   516,  1659,  1559,
	   927,  1991,   516,  2155,   930,   821,  1963,  2159,  1952,  1312,
	  1878,  1910,  2158,  1502,  1413,  1410,  1454,  1925,  1925,  1925,
	  1925,  1958,  1445,  2165,  1925,    48,  2118,  2119,  2120,  1264,
	   963,  1467,  2124,  2169,   947,  2168,  1937,  2172,   756,   842,
	  1467,  1454,  1467,  2148,  2179,  2166,  1909,  1691,  2174,  1910,
	  1467,  1467,  2180,  1467,  2151,  1897,  2182,  1347,   842,  2187,
	  1924,  2167,  1855,  2162,  1150,  1856,  2188,  1840,  2189,  1150,
	   842,  1839,  2193,  2194,   842,   668,  1769,  1760,  1608,  1770,
	  1761,  1609,  1401,  1372,  1372,  1576,  1692,  1404,  1577,  2156,
	  1568,  1878,  1926,  1569,  1454,   784,  1026,  2146,  1512,  1381,
	   509,  1511,  1382,  1454,  1415,  1454,  1324,  2145,  2143,  1322,
	   927,   927,  1321,  1454,  1454,  1322,  1454,    34,  1691,  1617,
	  1467,  2096,  2091,  1311,  2183,  1467,  1312,  2163,  2085,  1692,
	  1692,  1692,  1692,  1691,  2084,  1925,  1189,  1495,  1299,  1467,
	  2081,  1297,  1296,  2177,  2010,  1297,  1923,  1923,  1692,  1292,
	   460,  2023,  1293,   461,  2014,  2001,  1977,  1838,  1804,    75,
	  1644,  1692,  1592,  1412,  1552,   683,  1386,  1377,  2176,   573,
	  2080,  2046,  2005,  2004,  1467,  1467,  1467,  1995,  1467,   667,
	  1442,  1912,  1802,  1454,  2041,  1448,  1758,  1707,  1454,  1467,
	  1706,  1705,  1704,  1702,   611,  1685,  1679,  1575,  1525,  1479,
	  1398,  1397,  1454,  1390,  1372,  1372,  1372,  1372,  1314,   951,
	  1261,  1372,  1688,  1252,  1198,   928,  1126,  1124,  1703,  1076,
	  1015,   507,  2010,  1032,   875,   431,   831,   818,   717,   516,
	   716,   715,   709,   668,   704,   703,   702,  1454,  1454,  1454,
	   697,  1454,    35,  1139,   825,   827,    50,   828,  1691,   826,
	    36,  1467,  1454,  1554,  1453,  1467,    58,   271,   677,   676,
	   296,  1866,  1113,  1115,  1114,  1538,  1448,  1923,  1923,  1923,
	  1923,  1150,  1270,   295,  1923,  2010,   293,   288,   286,  1553,
	  2010,    74,    59,    60,  2010,  2010,  2010,   257,   294,   671,
	   747,  1691,  1691,  1691,  1691,   530,   243,   246,   245,  2015,
	   863,   564,  1467,   562,   561,   747,  2009,   719,   448,   719,
	  1691,  2041,  2041,   254,  1454,  1147,   601,   599,  1454,   552,
	   251,   272,  1372,  1691,   280,   928,   928,  1718,  1619,  2010,
	  1717,  1716,  1715,  1448,  1730,  1729,  1467,  1728,  1727,   518,
	  1726,  1692,  1725,  1723,   785,  1481,  2040,  1737,  1483,   627,
	    52,    54,    54,    54,   626,   625,  1898,  1714,  1480,    20,
	  2010,  2010,  2010,  1692,  1211,  1454,   581,   264,  1234,  1233,
	   935,   931,   285,   959,   287,  1531,   967,  1507,  1508,  1509,
	  1510,   291,   662,  1505,  2009,  1923,  2041,  2041,  2041,  1818,
	   506,    29,  2041,   670,    30,  1619,  1240,   634,   282,  1454,
	  1448,  1473,  1740,   899,   263,   265,   249,   261,   855,   558,
	   532,   538,  1692,   253,   266,   667,  2010,   250,   557,   852,
	   851,   850,   252,   570,  1465,   839,   541,   247,   262,   580,
	   584,   586,  1464,   575,   259,   300,  1466,  2009,  1490,   508,
	   907,   615,  2009,   617,   509,  2041,  2009,  2009,  2009,   267,
	   887,  2010,   650,   784,   781,  1426,   661,  1421,   509,   882,
	   880,  1861,  1862,  1863,  1864,   582,   684,  1880,   876,   258,
	   283,   292,   255,  2040,  2040,   870,   805,  1893,  1869,   200,
	   719,   199,   198,   197,   196,   195,   194,   193,   192,  1619,
	   191,  2009,   829,   830,  1448,   190,   508,  1379,   189,   188,
	   187,   509,   186,  1691,   508,  1465,   185,  1463,   184,   509,
	   183,   182,   181,   180,   179,   178,  1446,  1466,   177,   513,
	   176,   175,  2009,  2009,  2009,  1691,   174,   173,   172,   171,
	   170,   169,   168,  1655,  1656,   167,   166,   988,   506,   520,
	   453,   452,   521,   451,   454,   165,   508,   164,  2040,  2040,
	  2040,   509,   163,  1955,  2040,   162,   161,   989,  1877,   160,
	   159,   158,   157,  1458,  1459,  1460,  1461,  1462,   156,   155,
	   154,   153,  1465,   152,  1691,  1919,  1975,  1974,  2009,  1813,
	  1918,  1349,  1021,  1617,  1466,  1119,  1848,  1849,  1463,  1811,
	   508,  1611,   432,  1965,  1037,   509,  1801,  1581,  1964,  1579,
	  1045,  1054,  1054,   508,  1307,   322,   310,  2040,   509,  1870,
	   319,   321,   309,  2009,  1875,  1713,   420,   320,   405,  1877,
	   308,  1338,   426,   506,  1452,   453,   452,   450,   451,   454,
	   984,   404,   328,  1301,   311,  1443,  1818,  2003,   312,  1465,
	   329,    20,   304,   303,  1458,  1459,  1460,  1461,  1462,   694,
	   327,  1466,   325,   324,   318,  1463,   710,   317,   315,   783,
	   313,  1766,  1020,   306,   305,   302,   230,   229,  1775,  1776,
	  1777,  1778,   228,    20,   227,   985,    20,    20,   226,   784,
	   225,   987,   224,   518,   509,   223,   222,   221,   220,   219,
	   218,   217,   216,   824,  1117,  1120,  1116,   518,  1121,  1118,
	   215,   214,   785,   213,   506,  1452,   453,   452,   450,   451,
	   454,  1458,  1459,  1460,  1461,  1462,  1147,   212,   211,   210,
	   209,   208,  1463,   207,  1147,   206,   205,   506,    29,   557,
	   670,    30,   204,  1465,   203,   793,   805,   792,   202,   201,
	   518,   526,   475,  1376,  2073,  1466,   497,   236,   518,   237,
	   145,   144,   143,   142,   141,   140,   233,  1101,   492,   491,
	  1992,   232,   487,   800,   488,   486,  1147,   483,   829,   482,
	   752,   506,  1452,   453,   452,   450,   451,   454,  1458,  1459,
	  1460,  1461,  1462,  2095,   805,  1589,   430,  1107,   805,   330,
	   518,   557,   557,   231,  1893,   138,   137,  1465,   136,   135,
	   134,   133,   132,   131,   130,   469,  1136,  1137,  1138,  1466,
	   468,   584,   467,   466,   956,  1146,  1463,   465,   757,  1465,
	  2032,  2033,  1599,  1350,   755,    89,   464,   118,   111,  1591,
	  1590,  1466,   108,    82,   518,    79,    17,    46,   506,  1452,
	   453,   452,   450,   451,   454,    12,  1822,   518,    93,    41,
	     6,    37,     7,     5,    40,  2053,  2144,     4,  1465,     3,
	     9,     2,     1,   513,   667,   667,  1793,   299,   786,   284,
	  1466,   268,  1458,  1459,  1460,  1461,  1462,   513,  2035,   728,
	  1529,  1555,   506,   520,   453,   452,   521,   451,   454,  1348,
	  1346,   606,  1180,  1407,   866,   539,   506,   520,   453,   452,
	   521,   451,   454,  2184,  2195,  2175,  1206,  1895,  1942,  2164,
	  2125,  2049,  1933,  1764,  1524,   652,   651,   968,   635,  1518,
	   513,  1251,  1514,   518,  1920,  1913,  1604,  1959,   513,   667,
	  1282,  1005,   506,  1615,   453,   452,   450,   451,   454,   506,
	  1151,   453,   452,  1152,   451,   454,    13,   506,   520,   453,
	   452,   521,   451,   454,   667,  1917,  1865,  2147,   667,   667,
	  1874,  1879,    32,  2123,  1871,    31,    24,    26,    25,  1560,
	   513,    23,   618,   622,   941,    28,   279,   277,   276,   278,
	    33,    20,    20,   275,   274,  2055,   244,  1868,  1696,   506,
	  1151,   453,   452,  1152,   451,   454,   506,  1452,   453,   452,
	   450,   451,   454,  1697,  1450,  1373,   442,   758,  1091,   771,
	    18,   112,   326,  1024,   513,   240,  1762,  1527,   506,  1927,
	   453,   452,   450,   451,   454,  1260,  1929,   513,  1184,  2154,
	  1183,  1002,  1550,   506,   520,   453,   452,   521,   451,   454,
	  1548,  1369,  1938,   965,  1771,  1254,   506,   520,   453,   452,
	   521,   451,   454,   534,   915,  1229,  1844,   506,  1452,   453,
	   452,   450,   451,   454,  1666,  1889,  1447,   323,  1213,  1316,
	  1022,  1215,  1586,  1594,   713,   672,    27,  1597,  1057,  1058,
	   314,   698,   307,  1288,  1303,  1340,   805,  1075,    70,    16,
	  1129,   817,  1140,  1638,  1368,  1123,  1227,  1640,   239,   149,
	   148,  1149,  1230,   513,  1248,   444,   514,   615,   810,   235,
	  1146,  1016,   519,   903,   812,    81,   994,  1356,  1905,   290,
	   289,   584,   506,   520,   453,   452,   521,   451,   454,  1767,
	  1662,    32,  1326,  1146,    31,    24,    26,    25,   301,  1820,
	    23,  1044,  1043,    21,    28,  2103,     0,     0,     0,    33,
	  1271,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,   667,     0,     0,     0,     0,     0,
	     0,     0,     0,    22,    29,     0,     0,    30,     0,  1045,
	     0,     0,     0,  1054,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  1045,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  1054,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,   584,     0,     0,
	     0,     0,     0,     0,     0,    27,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,   667,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  1595,     0,   719,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,    22,    29,     0,     0,    30,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  1646,  1647,  1648,
	     0,  1146,     0,  1146,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,   667,   667,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,   557,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,   667,   431,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  1045,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,   667,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  1799,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   557,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  1711,
	     0,     0,     0,     0,     0,     0,     0,     0,   667,     0,
	     0,     0,     0,     0,     0,     0,   333,     0,     0,   360,
	     0,     0,     0,     0,     0,     0,   331,     0,   667,     0,
	   146,   126,   127,   128,   436,     0,   125,   411,   151,  1757,
	    28,  1037,     0,     0,   241,   334,     0,     0,     0,     0,
	     0,     0,     0,     0,   336,   345,   344,   348,   349,   248,
	   347,   297,   351,   352,   380,   337,   354,   359,   357,   260,
	     0,   361,     0,   363,   365,   338,   372,   366,   374,   364,
	   370,   377,   371,   378,   298,   281,   342,   379,     0,   382,
	   381,   383,   384,   341,     0,   386,   339,   385,   340,   375,
	   376,   393,   394,   395,     0,   396,     0,     0,   397,   398,
	     0,   358,   369,     0,     0,     0,     0,     0,     0,   256,
	   273,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,    27,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,   667,     0,     0,   719,     0,
	     0,     0,     0,     0,     0,   350,     0,   343,   346,   353,
	   355,   356,   368,   367,   373,     0,  1146,   392,     0,     0,
	     0,  1146,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,   269,   270,   362,     0,   387,
	   390,   388,   391,   389,   399,   402,   403,   410,     0,   412,
	     0,   316,   413,   414,   415,   416,   417,   418,     0,   424,
	   425,   427,   434,   406,   435,   421,   407,   422,   419,   423,
	   400,   401,   433,   408,   428,   409,     0,   429,     0,   332,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   667,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  2142,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  2142,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	   557,     0,     0,     0,     0,  2029,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  2054,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  2152
};

short yypact[] = {
	  1393, -1000, -1000, -1000, -1000,  2810,  2330,  1421,  -201,  1380,
	  2810, -1000,  1860,  1626,  2101, -1000,  2326,  -201,   -10,  -147,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   471, -1000, -1000,
	 -1000, -1000, -1000, -1000,  1622,  1853,   437,  1622,  2330, -1000,
	 -1000,  2330,  -201, -1000, -1000,  1622,  1851, -1000,  2979, -1000,
	 -1000,  2326, -1000,  1952, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,  1871,  1488, -1000, -1000,  1450,  1879, -1000, -1000,
	  1850,  1980,  1962,  1871,  1849,  1622, -1000,  1871,  1622,  2326,
	 -1000,  1626,   671, -1000, -1000,    56, -1000, -1000,  1762,  3515,
	  3515,  1677, -1000,   204, -1000, -1000,   204, -1000, -1000,   250,
	   451,   449, -1000,  1450,  1871, -1000,  1871, -1000,  2229, -1000,
	 -1000,  1848,  1847, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,  1677, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   121,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1622,  1419,  1488,
	  1622, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,  1622,  1622, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,  1610, -1000,   448,   177,  1468,   560,  2683,   361, -1000,
	 -1000,  1302, -1000,   883,   175,   175,  1360,  -109,  -109,   601,
	  -109,     2,   175,   129,   214,   747,  -109,   175,   975,  -109,
	  -109,  -109,  -109,   175,   175,   175,  -109,   975,  1054,  1315,
	 -1000,   523,   973,   551,  -109,   175,  -109,   175,   912,   821,
	   973,  2683,  1021,  1028,   999,   924,   175,  1287,  -109,  -109,
	   175,  1237,   747,  -109,   292,   558,   850, -1000, -1000,   175,
	   175,   986,   733,  -190,  -190,  -109,  2320,   538,  2316,  2315,
	  2314,  -190,  -190,  2683,   538,  2312, -1000,  1435, -1000,  -259,
	  2311,  2310,  2308,  2683,  -109,  2683,  1622,  1434,  -190,  -109,
	  1488, -1000,  1891, -1000, -1000,  1020, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,  -109,   444, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	  1114, -1000,   543, -1000, -1000,  1214, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,  1189, -1000, -1000,  1189,
	 -1000, -1000, -1000, -1000, -1000, -1000,  1370, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,  -109,   736, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	   736, -1000, -1000, -1000, -1000, -1000,  -190, -1000, -1000, -1000,
	 -1000,  2305,  1162, -1000, -1000, -1000, -1000, -1000,  2114, -1000,
	  1880, -1000,  1471, -1000, -1000, -1000, -1000, -1000, -1000,  -230,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,   369,   432,    90,  3515,  -201,   -10,   -10,   -10,  -147,
	  2533, -1000, -1000, -1000,  2810,  1838, -1000,  1762, -1000,  1419,
	  1406, -1000,  1610,  1613, -1000, -1000,  1488,  1488, -1000, -1000,
	 -1000,  1488,   566, -1000,   439, -1000,   181,  -185,  -202,  -185,
	 -1000,  2683,  2307, -1000, -1000,   137, -1000,   680,  2683,  2683,
	  2306, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,   177, -1000,   121, -1000,  2305, -1000, -1000, -1000,
	   292, -1000, -1000, -1000,  1953, -1000, -1000, -1000,  -259, -1000,
	   431,   873,  2044, -1000,  1315,  1315,  2249,  2249, -1000, -1000,
	   601,  2044,  1315, -1000, -1000, -1000,   793, -1000, -1000, -1000,
	  1315,  1022,  1022,   175,   -38, -1000, -1000, -1000, -1000,   868,
	  2304,   808,  1526, -1000, -1000,  1315,  2044,  1315,  1315,  1315,
	   425,   821, -1000, -1000, -1000,   835,  1056,  1315,  -109,  1526,
	   175, -1000,   209,  1315, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   462, -1000, -1000,
	 -1000, -1000, -1000,  2044,  2080, -1000, -1000, -1000,   175, -1000,
	 -1000, -1000, -1000,  1320, -1000,   175,   175, -1000, -1000, -1000,
	  1155, -1000, -1000,   680,   985,  2110,   365, -1000, -1000,  2289,
	 -1000,  2289, -1000,  2289, -1000,   175,  1235, -1000,   175,   175,
	   871,  1004,  2106, -1000, -1000, -1000, -1000, -1000, -1000,   861,
	   922, -1000,   648,   424,   860,   784, -1000, -1000, -1000, -1000,
	 -1000,  -148,  2364,  -124,   337, -1000, -1000,  1358, -1000,  2249,
	  2249,   151,  1051,   420,  2289, -1000, -1000, -1000, -1000, -1000,
	   281,  2300,  -303,   137,   361, -1000,  2683,  -201, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,  2303,  2303,  1331, -1000,  -201,
	 -1000,   651, -1000, -1000, -1000,   857,   -62,   -62, -1000,   680,
	  -109, -1000,  1488,  -109,  2300,  -109,  1689,   767, -1000,  1527,
	  1095,  1486, -1000, -1000,  1125, -1000, -1000, -1000, -1000, -1000,
	  1218, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,  2299,   566,  1456,  1206,  1837,  2979,  1488,   430, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1952,  1952,  1952,
	 -1000,   680, -1000,   527,  2759, -1000, -1000, -1000,   216,  1622,
	 -1000,  2810,  2810,  1832, -1000, -1000, -1000, -1000,  1689,  1689,
	   566,  1681,   -55,   566,   566,   376, -1000,  -308, -1000,  2297,
	  2296, -1000,  1951, -1000, -1000,  2296, -1000, -1000,   722, -1000,
	   415, -1000,  2683,  2683,  2683, -1000, -1000, -1000, -1000,   540,
	 -1000,  2626,  1638, -1000, -1000,   401,  1315, -1000,  1831, -1000,
	   381,   206,  -109, -1000, -1000,  -109,  -109,  1315, -1000, -1000,
	 -1000,   935,   259,   696,   662,   601, -1000, -1000,  1022,  -259,
	   374, -1000, -1000, -1000, -1000,  1940,   868, -1000,   462,  -259,
	  -109,  1830,   305, -1000, -1000,  -109,  2030,  2030, -1000,   587,
	 -1000, -1000,  -109, -1000,  2294,  2249,  2249,  2249, -1000, -1000,
	 -1000, -1000,  1173,  1526,  1206, -1000, -1000, -1000, -1000,  1165,
	 -1000,  1429,   175, -1000,  1945,   175, -1000,  2030,  2030, -1000,
	   587,   773,   754,  1993, -1000,  1829, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	   175,   259,   644, -1000, -1000,   304,  2017, -1000,  2017, -1000,
	  1088,   175,  1310, -1000, -1000, -1000, -1000,  1293, -1000, -1000,
	 -1000,  -109, -1000, -1000, -1000,   175,  2293,   984,   377,   175,
	 -1000, -1000, -1000,  1296,  -109,  2290,  -109,   985,  2105,   365,
	 -1000, -1000, -1000, -1000,  2289, -1000, -1000, -1000,  1026,   292,
	   292, -1000, -1000, -1000,   175, -1000,   488, -1000,   759, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  -109,  -109,
	  2043, -1000,  1825, -1000, -1000,  1129, -1000,   330, -1000,   600,
	 -1000, -1000, -1000,  2044, -1000,  -109, -1000, -1000,   177, -1000,
	  1816, -1000,  2228, -1000,  1808, -1000,   361,  2221, -1000,  1803,
	  2217,  1346,  -109,  1346, -1000, -1000,  1793, -1000, -1000,  1944,
	  2202, -1000,  1788,  2288,  1361, -1000, -1000, -1000, -1000, -1000,
	 -1000,  2191, -1000,  1785, -1000, -1000,  2185,   852,   353, -1000,
	  1762, -1000, -1000, -1000,  1622,  1622, -1000,  1527,  1206,   292,
	 -1000,   175, -1000,   292,   292, -1000, -1000,   -70, -1000, -1000,
	  2135, -1000, -1000, -1000,  2101,  1762,   159,    60, -1000, -1000,
	 -1000,  2683, -1000, -1000, -1000,  1871,   216,   216,  1622,   -70,
	 -1000, -1000,   566, -1000, -1000, -1000, -1000,  2322,   188, -1000,
	  2683, -1000,   264, -1000, -1000, -1000, -1000, -1000, -1000,  -298,
	 -1000, -1000, -1000,  1331,  2246, -1000,  2576,   352, -1000,  2178,
	 -1000, -1000,  1780,  1779,  1776, -1000,  -269,   319, -1000, -1000,
	  2245, -1000,  1880,  1880, -1000, -1000,   680,  2283, -1000,  2584,
	   148,   121,   121,   121, -1000, -1000,   462,  2281,  2280, -1000,
	 -1000,  2030, -1000,  2030, -1000,  1315, -1000, -1000,  -109,  -109,
	  1315, -1000, -1000,  1768,   338, -1000, -1000, -1000, -1000,  -259,
	  2091, -1000,  1766,  2242,  2090, -1000,  1755,   734,   808, -1000,
	 -1000,   595,  -109,   -98, -1000, -1000,   393,   935,  2260,  -109,
	  -109,  -109,  1526, -1000,  1443, -1000,  1369, -1000, -1000,  2279,
	 -1000,   325, -1000, -1000,   316, -1000,   649,  -109,   595,   -46,
	  1315,   314, -1000,  2260,   209,  1119, -1000, -1000, -1000, -1000,
	  2089, -1000, -1000, -1000,  2017, -1000, -1000, -1000, -1000,   175,
	  1343,  -109,  -109,  -109,  -109,  2080, -1000, -1000,  2177, -1000,
	   916,   907,   403,  2260,   787, -1000, -1000,  2278, -1000, -1000,
	   591,  -109, -1000,  1199,   751, -1000, -1000, -1000, -1000,   462,
	  1340,   758, -1000,  -109,  -109, -1000,  2030,  2030,   151,  2027,
	 -1000,   286,  2075,  1025, -1000, -1000,   330,  1526,  2169, -1000,
	 -1000,  2683, -1000, -1000,   857,   177, -1000, -1000,   -62, -1000,
	 -1000,  2277, -1000,  2164, -1000,  1753, -1000,    73,  2683, -1000,
	  -109, -1000,   857,    -4, -1000, -1000,  2241, -1000, -1000, -1000,
	 -1000, -1000,   -62,  2519, -1000,  2670, -1000, -1000, -1000, -1000,
	 -1000,  1527,  1432, -1000,  2016, -1000,   224, -1000, -1000, -1000,
	  2157, -1000,  1748, -1000, -1000, -1000, -1000, -1000,  1321,  2569,
	  1622, -1000, -1000,  -157,  1399,  1398,   -80,   140,   312,  1331,
	 -1000, -1000, -1000, -1000,  1871, -1000, -1000, -1000,   680,  1060,
	 -1000, -1000, -1000,   400, -1000, -1000,   102, -1000,  2239, -1000,
	 -1000, -1000,   707,  2683,  2683,  2683, -1000,  2626, -1000,  2626,
	   -44, -1000,   196, -1000, -1000, -1000, -1000,  -109,  -109, -1000,
	 -1000, -1000, -1000, -1000, -1000,  -259,  1943,  2074, -1000, -1000,
	   305,   292,   937,  -109,   597, -1000,  1526, -1000,   587,  1526,
	 -1000,   -98, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   711,
	  1100, -1000,  2276, -1000, -1000,   175,   692, -1000,  -172,  2275,
	 -1000,   173,  2260,  2260,   334,   929,  2273, -1000,  2475, -1000,
	  2272,  2271,  1940,  2270, -1000, -1000, -1000,  2267, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  2030,  2030,
	  2030, -1000, -1000,   175,  1165, -1000, -1000, -1000, -1000,  -109,
	   932,  1554, -1000,    79, -1000,  1315,  1526, -1000,   587,  1526,
	   -46, -1000, -1000, -1000, -1000, -1000,   299,  -279, -1000, -1000,
	   576,   576,   462,   292, -1000, -1000,  1207,  2044,  2044,  2044,
	  2044,  -109, -1000,   175, -1000,  2266, -1000,  1206,  2156, -1000,
	 -1000, -1000,  -279,   914,   848,  -109,   274,  2155, -1000,   836,
	   587,   259,  -109,  -109,  -109,  -109, -1000,   462, -1000,   151,
	  -109,  2037, -1000,  1742,  2037, -1000, -1000, -1000, -1000,   576,
	 -1000, -1000, -1000, -1000,  -109,  1105, -1000,  1740,  1738,   330,
	   330, -1000, -1000, -1000, -1000, -1000,  1328,  1193, -1000,  -109,
	  2069, -1000, -1000, -1000, -1000,   857, -1000,  -109,  2683,  2067,
	 -1000,  2262,  1719,  2237, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,   680, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,   292, -1000, -1000, -1000, -1000, -1000, -1000,
	  -109,  1051,   232, -1000, -1000,   152,  1718,   505,   137,  2475,
	 -1000,  1488,  2048, -1000,   -63, -1000,  1938,   211, -1000, -1000,
	 -1000, -1000, -1000, -1000,  -246, -1000, -1000,  2249,  1792, -1000,
	 -1000,   121,   102,  1937, -1000, -1000,   680,   680,   680,  1880,
	 -1000,  1880, -1000,  2236, -1000,  2150,  2146, -1000,   172,  -259,
	 -1000, -1000,  2683, -1000, -1000, -1000,  1329, -1000,   390,  1173,
	  1173, -1000,  1100, -1000, -1000, -1000, -1000, -1000, -1000,  2260,
	  2249, -1000,   -64, -1000,  2141,  -182, -1000,  2260,   146,   163,
	 -1000,  2475,  2475,  2475,  2341,  -259,  2694,  2475, -1000,  1031,
	 -1000, -1000,  1161, -1000,  1420,  1475,    62,  2475, -1000, -1000,
	   626,  1178, -1000,  2134,  1181,  -109, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,  2017, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,  2017, -1000, -1000,
	 -1000,   773,   773, -1000, -1000, -1000, -1000,    35, -1000, -1000,
	 -1000,  2125, -1000, -1000, -1000,   133, -1000,  2261,  2655, -1000,
	 -1000,   403,   864,   292,  1099,   112,  2115,  1030, -1000, -1000,
	  -109, -1000,   523,   -66, -1000,  2044,  2044,  2044,  2044, -1000,
	  2043,  2037,  -109,   830,  2087, -1000, -1000,   286,  2633,   -44,
	  1025,   904, -1000, -1000,   706, -1000,  -201,  2085, -1000,   680,
	 -1000, -1000,    33,   130, -1000, -1000, -1000, -1000, -1000, -1000,
	   754,  2022, -1000,  2569,  2683,   320, -1000, -1000,   467,  1762,
	  2235,  -215, -1000,  1086,  1073,   125,  1922, -1000, -1000,   119,
	 -1000,   102,  2175, -1000,   121,    96,   218,  2584, -1000, -1000,
	 -1000, -1000, -1000,  1315, -1000,  1397,  -216,  2257, -1000, -1000,
	 -1000,   162,  -109,  1066,  1065, -1000,  -236,  2234, -1000, -1000,
	 -1000,  -271,   308, -1000,   666, -1000,  2408,  2253,  2252, -1000,
	 -1000,  1060, -1000, -1000, -1000,   862,   453,  2633, -1000, -1000,
	   483, -1000, -1000,  2233, -1000,  2633,  2633, -1000,  2633,  2071,
	 -1000,  1715,  2230,   341,   175,   754,    40, -1000, -1000,   175,
	  2017,  1554,    79,  1315,  1315, -1000, -1000, -1000, -1000, -1000,
	   576, -1000,  -109,  2063, -1000, -1000,   586, -1000,  2655,  2655,
	 -1000, -1000,   149, -1000, -1000, -1000,  2251, -1000, -1000, -1000,
	   209,   209,  2016,  1016,    48, -1000, -1000, -1000,  1315,   175,
	 -1000, -1000, -1000,   -71, -1000, -1000,    19, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,   453,  2633, -1000,   149,   148, -1000,
	  2260,    75,  1714, -1000,    73,  2061, -1000,  1709, -1000,  1009,
	    69, -1000,  -316, -1000,  2475,   -74, -1000,   -80, -1000, -1000,
	 -1000, -1000,    67,  2215, -1000, -1000, -1000,  1638, -1000,   121,
	   121,   121, -1000,  2250,  2219,  -247, -1000,  2215, -1000, -1000,
	  2213, -1000,  2207,  1206,  1299,  1146, -1000,   275,  2201,  2633,
	  2633,  2633, -1000,  2475, -1000,  2200,   428,   419,   409, -1000,
	    62,    49,   -91, -1000,  2475,  2249,  1439,    40,   574,  -259,
	 -1000, -1000, -1000, -1000, -1000,  2045, -1000,   372, -1000,  2655,
	  2655,  2655,  2655, -1000,   357,   100,  2655,  1993,  1993,   893,
	   462, -1000, -1000, -1000, -1000, -1000,   -83, -1000,    29,    18,
	 -1000,   311,  -279, -1000,  2683, -1000, -1000,    33,   612,  1002,
	  1094, -1000,  1917,   666, -1000, -1000,   137, -1000, -1000, -1000,
	   243,  -201,  2187, -1000, -1000, -1000,  2475,  2186,  2176, -1000,
	  2633, -1000,  -274,   293, -1000,   666, -1000, -1000, -1000, -1000,
	 -1000,  1708, -1000,  1909, -1000, -1000, -1000,  -109,   175, -1000,
	    27, -1000,  1315,  -109, -1000, -1000, -1000, -1000,  -275,   226,
	 -1000, -1000, -1000,  2168,   289,   754,  -109,   833,  1885, -1000,
	    21,     5,  2142, -1000, -1000, -1000, -1000,  2260,   -38,  2123,
	  2140, -1000, -1000,  -201,   301, -1000, -1000,  2036,   453,   -91,
	   -17,  2030,  2248, -1000, -1000, -1000, -1000,  2655, -1000,   810,
	   462,   462, -1000,  -279,  1989, -1000, -1000, -1000, -1000,   754,
	 -1000,  2633, -1000, -1000, -1000, -1000,   -40,   586,  -109, -1000,
	 -1000,   -38,  1009,   453,  1988, -1000,  1695, -1000, -1000,  1002,
	 -1000,   -40,  -104, -1000, -1000, -1000,  1907,  -113, -1000
};

short yypgo[] = {
	     0,  3265,  2004,   128,  3263,   990,   672,  1185,   190,  1842,
	   549,   387,   176,    75,    71,   103,  3262,  3261,  3259,  3258,
	  3252,  3250,    11,    45,  3249,  3240,  3239,  3238,  1184,  3237,
	    23,    51,    31,  3236,  3234,  2103,   125,   145,    79,    14,
	    97,  3233,   168,   351,   284,  3232,    60,    13,    42,    78,
	    48,  3231,  3229,  2285,  3228,   347,    34,  3226,    12,  3225,
	    69,    67,  3224,   153,     0,    10,    57,  1255,  3222,    26,
	    47,    93,  3221,   167,  3220,  3219,  1757,  3218,  3217,    76,
	   185,  3215,  3213,    36,   161,   144,  3212,   117,    29,   102,
	    35,  3211,  3210,   119,  3209,   605,  3208,  1375,    87,  3207,
	  3205,    84,   171,  3204,    82,   148,   172,  3203,  3202,  3201,
	  3200,  1590,  3199,  3198,  3197,    88,   127,  3195,  3194,  3193,
	  3192,   118,  3190,   132,   133,  3189,   177,   126,   170,   123,
	  3187,  3186,  3185,    37,  3184,  3176,   157,  3175,    92,  3174,
	    89,    41,  3173,  3165,  3164,  3163,  3162,  3161,  3160,  3152,
	  3151,  3150,  3148,   114,    91,    52,  3146,  1036,    38,   139,
	  3145,  3137,    55,    95,   137,    59,  3136,    33,   179,  3135,
	  3133,     7,  3132,  3131,  3130,  3129,  1353,  3128,   136,  3127,
	  3126,  3125,  3124,    62,    54,  3123,  3108,  3107,    98,    15,
	   154,    18,    64,  3106,  3105,  3104,  3103,  3099,  3098,  3097,
	  3096,  3094,  1709,  3093,   178,  3092,  3089,    70,   889,    39,
	   109,    72,    63,  3084,     4,    21,  3083,  3081,  3080,  3077,
	  3076,    25,    65,     6,  3075,     8,  3051,  3050,    86,    90,
	  3047,   155,     5,     2,    27,  3046,    49,  3045,    19,   105,
	  3044,  3042,  3041,  3039,    58,  3038,   149,  3037,   135,  3036,
	  3035,  3034,  3033,  3032,  3031,     3,  3030,  3029,  3028,    22,
	  3027,    24,   100,  3026,  3025,  3024,  3023,     1,   151,  3015,
	    73,   156,  3014,    77,  3013,   116,  3012,  3011,  3010,  3009,
	    44,  3001,    50,  3000,   162,   120,    99,   104,  2999,    81,
	  2998,     9,  2991,  2989,  2987,  2986,  2982,  2981,  2980,  2979,
	  2977,  2974,  2973,   169,   130,  2972,  2971,   164,   131,  2362,
	  1793,  2970,  2237,  2969,  1082,  1758,  1774,  2968,  2966,   141,
	  2965,  2957,   165,  1960,  2956,  2955,  2953,  2952,   993,  2948,
	  2376,   121,    83,  2947,  2946,  1140,  2945,    40,  2944,  2943,
	  2938,  2937,  2933,  2932,  2930,  2925,  2924,  2923,  2922,  2921,
	  2920,  2919,  2918,  2916,  2915,    20,  2913,   978,  2909,  2906,
	  2890,   147,  2889,  2887,  2885,  2884,  2883,  2882,  2881,  2879,
	  2878,  2877,  2876,  2875,  2874,  2873,  2872,  2871,  2870,  2869,
	  2867,  2866,   122,  2863,    68,   630,   124,  2862,  2861,  2859,
	  2858,  2857,  2855,  2854,  2852,  2846,  2845,  2843,  2841,  2840,
	  2839,  2838,  2837,  2823,  2821,  2820,  2812,  2811,  2810,  2809,
	  2808,  2807,  2806,  2805,  2802,  2800,  2798,  2794,  2792,  2787,
	  2786,  2785,  2784,  2783,  2782,    46,  2780,  2779,  2778,  2777,
	  2776,  2774,    80,  2773,  2772,  2770,  2769,  2763,  2762,  2760,
	  2758,  2754,  2753,  2752,  2751,  2742,  1129,   166,  2741,  2740,
	  2738,  2737,  2736,  2732,  2731,  2730,  2726,  2725,  2724,  2719,
	    28,  2718,  2717,  2716,  2713,    17,  2712,  2711,  2709,  2701,
	  2699,  2697,  2696,    85,  2693,  2691,  2690,  2689,  2688,  2682,
	  2681,  2680,  2679,  2676,  2675,  2672,  2667,  2665,  2656,  2655,
	  2652,  2651,  2650,  2649,  2648,  2647,  2646,  2641,  2640,  2638,
	  2635,  2634,  2633,  2632,  2631,  2630,  2628,  2626,  2622,  2620,
	  2619,  2618,  2615,  2610,  2608,  2607,  2606,  2605,  2604,  2603,
	  2602,  2601,  2599,  2595,  2592,  1673,  2591,  2590,  2589,    53,
	  2588,    16,   138,    66,  2585,   143,    74,  2580,   140,  2579,
	    43,   106,  2577,   115,   111,   108,   101,  2575,   110,   107,
	    61,    56,  2570,  2569,  2560,   142,  2558,   113,  2555,  2554,
	  2553,   180,  2548,  1722,  2547,  2546,  2545,  2542,  2541,  2540,
	  2539,    94,  2537,   163,  2534,  2533,  2529,  2528,  1659,  1619,
	  2527,  2526,  2525,  2524,  2523,  2521,  2518,  2517,   146,  2516,
	  2503,   150,  2502,  2501,  2496,  2495,  2494,  2493,  2492,  2491,
	   175,  2490,  2489,  2488,   158,    96,  2487,  2486,  2484,  2478,
	  2477,  2476,  2475,   174,  2474,  2469,  2468,  2467,    30,  2465,
	  2463,    32,  2462,  2460,  2458,  2457,  2455,  2454,  2452,  2451,
	  2450,  2447,  2444,  1706,  2441,  2440,  2439,  1750,  1752,  2437,
	  2436,  2433,  2424,   181,  2423,  2421,  2420,   159,  2419,  2418,
	  2417,  2416,  2415,  2408,  2407,  2398,  2397,  2396,  2393,  2392,
	  2385,  2380,  2379,  2378,   173,  2377,  2374,  2216
};

short yyr1[] = {
	     0,   296,   296,   297,   297,   298,   298,   299,   302,   302,
	   306,   305,   300,   311,   301,   313,   312,   314,   314,   309,
	   309,   309,   315,   315,   315,   310,   310,   317,   316,   316,
	   316,   316,   316,    96,    96,    96,    18,    18,    30,    30,
	    30,   179,   179,   180,   180,    29,    29,    29,    29,    29,
	   318,   318,   318,   303,   303,   319,   321,   319,   320,   322,
	   322,   323,   323,   323,   323,   325,   323,     9,     9,   326,
	   327,   327,   327,   327,   328,   328,   328,   329,   173,     6,
	     6,     6,     6,     6,   175,   175,    94,    94,    94,     4,
	     4,     4,   324,     5,   174,   174,   174,   330,     7,     7,
	     8,     8,     8,   176,   176,    95,    95,   307,   334,   332,
	   336,   332,   332,   308,   308,   333,   333,   333,   331,   331,
	    23,    23,   339,   337,   340,   337,   338,   338,   335,   341,
	   335,   342,   335,   343,   335,   344,   335,   345,   335,   335,
	   335,   335,   335,   335,   335,   346,   346,   346,   346,   352,
	   356,   360,   358,   359,   361,   361,   357,   357,   357,   357,
	   357,   362,   366,   364,   365,   363,   367,   353,   370,   369,
	   371,   371,   368,    19,    19,   354,   354,   372,   347,   347,
	   347,   347,   347,   347,   347,   378,   378,   379,   380,   376,
	   376,   377,    91,    91,    91,    92,    92,    93,    93,    93,
	    52,    52,    80,    80,   373,   381,   383,    81,    81,    81,
	    82,    82,    83,    83,   375,   374,    84,    84,    84,    85,
	    85,    85,    85,    87,    87,    88,    88,    88,    89,    89,
	    90,    72,    72,    72,   384,   348,   348,   348,   348,   348,
	    74,    77,    77,   385,   385,   387,    75,   169,   169,    73,
	    73,    76,    78,    78,    79,   388,   167,   167,   350,   350,
	   389,   390,   386,   304,   391,   391,   392,   351,   351,   351,
	   351,   351,   351,   351,   351,   351,   351,   351,   351,   351,
	   351,   351,   351,   351,   351,   351,   351,   351,   351,   351,
	   351,   351,   351,   351,   351,   393,   393,   393,   421,   396,
	   422,   397,   423,   424,   404,   426,   407,   427,   177,   177,
	   406,   406,   428,   425,   408,   409,   409,   429,   430,   430,
	   430,   431,   431,   431,   431,   431,   432,   415,   415,   433,
	   112,   113,   113,   114,   114,    20,    20,   416,   434,   418,
	   418,   435,    97,    97,   436,    97,    98,   394,   437,   106,
	   106,   107,   107,   395,   438,   420,   439,   403,   440,   402,
	   441,   105,   105,   442,   102,   102,   103,   103,   104,   419,
	   443,   398,   108,   444,   109,   109,   417,   172,   445,    99,
	    99,   100,   100,   101,   446,   446,   446,   447,   447,   448,
	   124,   124,   124,    17,    16,    16,    16,    16,    16,    16,
	   125,   125,   125,   125,   405,   110,   111,   111,   399,   450,
	   449,   122,   122,   123,   170,   411,   452,   451,   115,   115,
	   116,   400,   453,   126,   126,   127,   412,   454,   128,   128,
	   129,     3,     3,   119,   119,   120,   120,   121,   121,   121,
	   410,   455,   118,   401,   456,   413,   457,   414,   414,   130,
	   458,   382,   382,   459,   461,   459,   460,   462,   463,   463,
	   464,   464,   465,   465,   168,   284,   284,   284,   285,   285,
	   285,   466,   287,   287,   287,   188,   188,   222,   286,   286,
	   189,   189,   189,   278,   278,   278,   467,   469,   279,   470,
	   279,   280,   280,   471,   280,   472,   280,   280,   234,   234,
	   468,   468,   226,   226,   225,   227,   227,   229,   229,   228,
	   228,   206,   206,   206,   206,   206,   230,   230,   223,   223,
	   223,   232,   232,   257,   257,   233,   233,   288,   288,   165,
	   165,   289,   289,   235,   235,   235,   231,   231,   349,   349,
	   349,   349,   349,   349,   349,   349,   349,   349,   349,   349,
	   349,   349,   349,   349,   349,   349,   349,   349,   349,   349,
	   349,   349,   349,   349,   349,   349,   349,   349,   349,   349,
	   349,   349,   349,   349,   349,   349,   349,   349,   349,   349,
	   349,   349,   349,   349,   349,   349,   349,   236,   224,   224,
	   485,   485,   272,   272,   271,   271,   271,   271,   271,   274,
	   274,   273,   276,   276,   275,   523,   523,   502,   277,   277,
	   277,   277,   497,   292,   292,   508,   293,   293,   242,   242,
	   243,   243,   244,   244,   241,   241,   237,   237,   238,   238,
	   239,   239,   239,   239,   239,   239,   240,   240,   240,   240,
	   216,   216,   216,   521,   294,   294,   281,   281,   282,   282,
	   484,   524,   525,   525,   516,   516,   526,   268,   268,   527,
	   487,   487,   487,   487,   528,   534,   532,   530,   530,   535,
	   535,   535,   537,   537,   541,   542,   542,   543,   543,   543,
	   543,   544,   545,   545,   545,   270,   270,   270,   546,   547,
	   547,   547,   547,   547,   538,   538,   538,   538,   538,   548,
	   549,   550,   552,   539,   191,   191,   283,   258,   258,   258,
	   258,   258,   258,   194,   194,   496,   496,   496,   496,   553,
	   192,   554,   554,   555,   555,   556,   556,   557,   557,   557,
	   507,   290,   290,   291,   291,   291,   291,   522,   558,   295,
	   295,   488,   559,   560,   489,   477,   491,   562,   563,   476,
	   564,   565,   566,   566,   566,   481,   567,   568,   568,   569,
	   569,   570,   479,   572,   495,   574,   482,   575,   577,   576,
	   576,   573,   573,   573,   490,   580,   478,   581,   581,   494,
	   582,   492,   583,   584,   585,   585,   585,   585,   263,   263,
	   262,   262,   262,   262,   260,   260,   261,   261,   264,   264,
	   266,   266,   267,   265,   506,   586,   587,   587,   591,   591,
	   245,   245,   246,   246,   246,   246,   246,   246,   246,   588,
	   588,   588,   201,   201,   201,   201,   589,   589,   589,   589,
	   590,   590,   592,   592,   515,   593,   594,   594,   247,   247,
	   248,   248,   248,   248,   248,   248,   248,   595,   595,   595,
	   595,   571,   571,   511,   596,   249,   249,   250,   250,   190,
	   190,   190,   190,   597,   597,   597,   251,   251,   252,   252,
	   253,   253,   253,   254,   254,   255,   256,   256,   509,   598,
	   503,   503,   599,   503,   503,   503,   601,   503,   602,   602,
	   603,   603,   605,   605,   195,   196,   196,   196,   196,   196,
	   196,   196,   199,   199,   198,   198,   198,   197,   197,   200,
	   205,   205,   202,   202,   204,   204,   203,   493,   607,   607,
	   606,   600,   600,   600,   616,   608,   608,   617,   617,   618,
	   609,   609,   620,   620,   621,   621,   621,   621,   621,   621,
	   610,   610,   628,   628,   628,   611,   611,   504,   501,   480,
	   632,   634,   635,   636,   636,   636,   633,   633,   633,   633,
	   633,   483,   483,   641,   642,   644,   643,   643,   647,   647,
	   637,   619,   645,   604,   622,   623,   624,   625,   626,   627,
	   638,   639,   612,   614,   615,   640,   630,   631,   629,   629,
	   551,   646,   613,   613,   561,   208,   208,   208,   208,   208,
	   208,   648,   208,   208,   208,   208,   182,   182,   182,   182,
	   182,   207,   183,   183,   211,   211,   211,   211,   211,   211,
	   209,   209,   209,   209,   209,   259,   259,   212,   184,   215,
	   215,   215,   214,   214,   214,   210,   210,   210,   210,   210,
	   210,   210,   210,   210,   210,   210,   213,   213,   185,   185,
	   217,   186,   186,   218,   218,   218,   219,   219,   220,   220,
	   220,   187,   187,   221,   505,   475,   475,   475,   269,   269,
	   269,   193,   193,   193,   193,   649,   650,   474,   474,   651,
	   652,   652,   518,   653,   520,   520,   136,   136,   136,    41,
	    41,   137,   137,   138,   138,   138,   138,   138,    31,    31,
	    31,    31,    32,    32,   178,   178,   157,   157,   158,   158,
	   159,   159,   159,   140,   140,   141,   141,   141,   141,   139,
	   139,   139,   139,   139,   139,   139,   139,    27,    27,    27,
	    27,   486,   654,    21,    21,   151,   151,   152,   152,   153,
	   134,   134,   134,    22,    22,   135,   135,   135,   510,   655,
	   143,   143,   166,   166,   156,   156,   156,   512,   656,   517,
	   517,   657,   519,   659,   519,   519,   519,   519,   519,   519,
	   519,   519,   519,   519,   519,   519,   519,   519,   658,   661,
	   660,   660,   662,   662,   662,   664,   163,   163,   164,   164,
	   164,   164,   150,   150,   148,   148,   148,   149,   117,    33,
	    33,   154,   154,   155,   155,   155,   155,   663,   663,   578,
	   579,   540,   533,   533,   529,   531,   536,   500,   665,   498,
	   499,   513,    25,    25,    25,    28,    28,    28,    28,    28,
	    24,   145,   145,   160,   160,   161,   161,   162,   162,   162,
	   144,   146,   146,   514,    26,    26,    26,    35,    35,    35,
	    35,    35,    35,    35,    36,    36,    86,    86,    51,    51,
	    50,    50,    47,    48,    37,    37,    37,    37,    37,    37,
	   147,   147,    55,    55,    55,    55,    55,    58,    58,    58,
	    58,    57,    57,    59,    59,    61,    60,    60,    10,    10,
	    11,    11,    12,    12,    13,    13,    13,    13,    13,    13,
	    14,    14,    56,    56,    56,   131,   666,   132,   132,   133,
	   133,   171,   171,     1,     1,    42,    49,    49,    39,   355,
	   355,   667,    38,    40,    40,    44,    45,    43,    43,    43,
	    46,    46,    46,     2,     2,    64,    64,    66,    66,    66,
	    67,    67,    62,    62,    63,    63,    65,    65,    68,    68,
	    71,    71,    69,    69,    70,    70,    70,    53,    53,   142,
	   142,   142,    54,    54,    34,    34,   473,   181,   181,    15,
	    15,    15
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
	     1,     3,     3,     3,     2,     3,     3,     3,     3,     3,
	     2,     0,     3,     3,     2,     2,     1,     2,     7,     3,
	     1,     1,     0,     1,     1,     3,     0,     0,     2,     0,
	     4,     6,     3,     0,     4,     0,     4,     1,     1,     1,
	     1,     3,     0,     1,     2,     1,     3,     1,     4,     1,
	     3,     1,     1,     1,     1,     1,     2,     0,     2,     3,
	     0,     3,     0,     1,     3,     2,     0,     3,     0,     1,
	     3,     1,     2,     1,     1,     2,     1,     2,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
	     3,     2,     1,     2,     2,     2,     2,     3,     2,     1,
	     3,     1,     1,     3,     3,     1,     1,     2,     2,     1,
	     1,     0,     4,     1,     2,     5,     1,     2,     3,     0,
	     1,     3,     1,     1,     4,     1,     1,     3,     1,     1,
	     3,     3,     3,     2,     3,     1,     1,     1,     1,     4,
	     3,     1,     0,     6,     1,     2,     1,     3,     3,     3,
	     2,     1,     1,     0,     2,     4,     1,     1,     3,     1,
	     5,     7,     7,     4,     3,     1,     1,     1,     3,     1,
	     1,     3,     3,     2,     1,     1,     2,     1,     1,     1,
	     1,     1,     1,     3,     2,     1,     1,     1,     1,     4,
	     1,     1,     2,     5,     4,     4,     4,     6,     9,     1,
	     1,     1,     1,     1,     2,     2,     3,     1,     1,     1,
	     2,     3,     0,     1,     1,     6,     8,     8,     5,     4,
	     3,     1,     3,     3,     2,     1,     2,     1,     1,     1,
	    10,     1,     3,     1,     2,     2,     2,     6,     1,     3,
	     0,     3,     1,     1,     3,     3,     3,     1,     1,     3,
	     1,     1,     5,     5,     2,     3,     1,     3,     2,     1,
	     3,     3,     4,     1,     3,     1,     2,     1,     0,     4,
	     2,     4,     4,     0,     3,     1,     3,     2,     2,     4,
	     1,    11,     1,     1,     1,     1,     1,     1,     1,     3,
	     1,     1,     4,     1,     3,     0,     3,     0,     3,     0,
	     1,     3,     3,     3,     5,     1,     1,     2,     1,     0,
	     1,     3,     1,     2,     1,     2,     1,     2,     1,     2,
	     2,     0,     1,     1,     1,     1,     2,     2,     2,     2,
	     3,     0,     3,     0,     6,     1,     1,     2,     1,     3,
	     1,     1,     1,     2,     1,     1,     1,     2,     2,     2,
	     2,     1,     0,    10,     1,     1,     1,     1,     3,     1,
	     1,     1,     1,     1,     1,     1,     3,     0,     2,     2,
	     2,     2,     0,     4,     0,     3,     4,     0,     5,     1,
	     2,     2,     0,     4,     2,     4,     0,     4,     1,     1,
	     1,     3,     1,     1,     1,     2,     2,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     2,     1,     2,     7,     1,     0,
	     1,     2,     2,     1,     0,     3,     0,     1,     3,     1,
	     2,     0,     1,     3,     1,     1,     1,     1,     1,     1,
	     2,     0,     1,     1,     1,     3,     0,     2,     2,     3,
	     1,     1,     1,     1,     1,     0,     1,     1,     1,     1,
	     1,     3,     3,     1,     2,     2,     1,     2,     2,     4,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     2,     1,     1,     1,     1,     1,
	     1,     1,     1,     3,     1,     3,     3,     3,     3,     2,
	     1,     0,     5,     5,     5,     5,     1,     1,     1,     1,
	     1,     1,     1,     0,     3,     3,     3,     2,     3,     1,
	     1,     1,     1,     1,     4,     1,     1,     1,     1,     3,
	     1,     0,     5,     3,     1,     3,     3,     2,     3,     2,
	     4,     3,     3,     3,     4,     1,     1,     1,     1,     2,
	     3,     1,     2,     5,     1,     3,     1,     3,     3,     4,
	     4,     1,     1,     7,     2,     2,     4,     2,     2,     4,
	     0,     1,     1,     1,     1,     1,     1,     1,     3,     1,
	     1,     0,     2,     1,     1,     1,     1,     3,     2,     1,
	     3,     1,     1,     1,     4,     6,     5,     2,     2,     2,
	     4,     4,     2,     2,     1,     0,     2,     0,     1,     3,
	     4,     5,     1,     3,     1,     2,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     0,     1,     1,
	     1,     8,     1,     1,     1,     0,     1,     1,     3,     4,
	     1,     5,     6,     1,     0,     2,     5,     0,     7,     1,
	     1,     1,     2,     0,     0,     2,     2,     4,     1,     6,
	     2,     1,     2,     0,     5,     3,     3,     4,     4,     2,
	     3,     3,     3,     3,     3,     5,     5,     6,     1,     2,
	     2,     0,     1,     2,     1,     2,     1,     3,     3,     1,
	     5,     3,     1,     2,     1,     1,     1,     2,     1,     1,
	     1,     1,     3,     3,     3,     3,     3,     4,     4,     1,
	     1,     1,     1,     3,     1,     1,     1,     3,     2,     2,
	     1,     8,     2,     1,     1,     0,     1,     2,     1,     1,
	     1,     0,     2,     0,     3,     1,     3,     1,     2,     1,
	     2,     0,     2,     2,     2,     1,     1,     3,     3,     3,
	     3,     2,     4,     1,     1,     1,     1,     0,     1,     0,
	     3,     2,     1,     1,     3,     3,     2,     3,     2,     4,
	     1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
	     2,     1,     1,     1,     1,     1,     1,     2,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     2,     2,     0,     4,     1,     3,     1,     5,
	     3,     1,     3,     1,     1,     1,     1,     3,     3,     1,
	     1,     0,     2,     1,     1,     1,     1,     1,     3,     2,
	     1,     3,     2,     1,     1,     1,     2,     1,     1,     2,
	     3,     1,     1,     3,     3,     0,     1,     2,     3,     1,
	     1,     2,     1,     0,     1,     2,     1,     1,     3,     1,
	     1,     1,     1,     3,     1,     1,     0,     1,     0,     3,
	     3,     6
};

short yychk[] = {
	 -4000,  -296,  -297,  -299,  -300,  -302,  -311,  -305,   286,  -298,
	  -303,  -319,  -320,   256,  -322,  -323,   -94,  -324,  -174,  -176,
	    -2,    -4,   483,   281,   276,   278,   277,   386,   285,   484,
	   487,   275,   272,   290,  -312,  -309,    40,  -306,   280,    -2,
	  -301,  -313,   286,  -304,  -319,    61,  -321,   123,    44,    -9,
	    40,    -5,    -2,    -7,    -2,   257,   -95,   486,  -330,  -330,
	  -330,   484,  -307,  -331,   259,   123,    61,  -315,    41,  -316,
	   -96,   484,   483,  -307,  -309,  -312,    -2,  -307,    61,  -325,
	  -323,   256,  -326,    -9,    -8,    46,  -308,    59,  -332,  -336,
	  -337,   256,  -310,  -317,   264,    41,  -314,    59,    44,    61,
	    46,    46,  -308,    61,  -307,  -308,  -307,    -9,  -327,  -328,
	   256,  -329,  -173,   466,   484,    -2,   257,   -23,  -333,   272,
	   125,    59,   256,  -337,  -335,   281,   276,   277,   278,  -176,
	  -346,  -347,  -348,  -349,  -350,  -351,  -352,  -353,  -354,   -42,
	  -373,  -374,  -375,  -376,  -377,  -378,   275,   -73,   -74,   -75,
	  -167,   283,  -474,  -475,  -476,  -477,  -478,  -479,  -480,  -481,
	  -482,  -483,  -484,  -485,  -486,  -487,  -488,  -489,  -490,  -491,
	  -492,  -493,  -494,  -495,  -496,  -497,  -498,  -499,  -500,  -501,
	  -502,  -503,  -504,  -505,  -506,  -507,  -508,  -509,  -510,  -511,
	  -512,  -513,  -514,  -515,  -516,  -517,  -518,  -519,  -520,  -521,
	  -522,  -389,  -390,  -393,  -394,  -395,  -396,  -397,  -398,  -399,
	  -400,  -401,  -402,  -403,  -404,  -405,  -406,  -407,  -408,  -409,
	  -410,  -411,  -412,  -413,  -414,  -415,  -416,  -417,  -418,  -419,
	  -420,  -356,  -368,  -372,   -43,   -52,  -380,  -379,   -76,   -77,
	  -169,   289,  -168,  -651,  -193,  -649,  -650,  -564,   304,  -581,
	  -572,  -635,  -567,  -575,  -641,  -524,   364,  -654,  -528,  -559,
	   314,  -580,  -562,  -583,  -606,  -582,  -574,  -553,  -292,   440,
	   441,  -665,  -634,   365,  -195,  -196,  -198,  -199,  -197,  -200,
	  -632,   330,  -586,  -527,  -293,  -598,  -655,  -596,  -656,   -25,
	   -26,  -593,  -526,  -657,  -653,  -658,  -661,   306,   329,  -294,
	  -558,   -19,  -421,  -437,  -438,  -422,  -423,  -108,  -449,  -453,
	  -456,  -441,  -440,  -426,  -110,  -428,   456,  -429,  -431,  -455,
	  -451,  -454,  -457,  -130,  -433,  -434,  -172,  -435,  -443,  -439,
	  -358,   271,   484,   261,   290,  -284,   299,   310,   320,   341,
	   343,   338,   331,   412,   301,   300,   413,   305,   302,   303,
	   410,   307,   308,   414,   311,   415,   416,   313,   356,   312,
	   264,   316,   442,   318,   324,   319,   322,   418,   417,   357,
	   325,   327,   321,   419,   323,   344,   345,   326,   328,   332,
	   309,   335,   334,   336,   337,   342,   340,   444,   446,   448,
	   445,   447,   422,   346,   347,   348,   350,   353,   354,   449,
	   475,   476,   450,   451,  -444,  -450,   468,   471,   478,   480,
	   452,   282,   454,   457,   458,   459,   460,   461,   462,   473,
	  -452,   470,   472,   474,   464,   465,  -445,   466,   479,   482,
	  -359,    40,  -466,   477,   467,   469,   279,  -335,    -9,  -331,
	  -315,  -316,  -180,   286,   -59,   287,   -58,   284,   -55,    45,
	   487,   488,   486,   485,   489,   484,   484,  -310,  -308,  -308,
	    41,    44,    61,    61,  -334,  -341,  -342,  -343,  -344,  -345,
	  -355,   501,    61,  -331,  -385,  -387,   280,  -332,  -331,  -331,
	  -331,  -357,  -362,  -363,   270,   -23,  -364,  -367,  -365,   268,
	   269,  -369,  -370,   484,    46,   -50,   499,  -381,   274,   263,
	   262,   294,   -64,    -2,   -44,   -46,   483,   -35,    40,    45,
	   -80,   -36,   -64,   464,   -57,   -40,   -44,   -55,   284,   -45,
	   484,   487,   -43,   -76,   -42,   484,  -388,   316,   335,   353,
	  -652,   389,   -53,   -64,  -142,   484,   432,   293,   -53,  -269,
	   298,  -565,   -67,   -64,   -64,  -573,  -578,  -579,   438,   439,
	  -563,   -67,  -636,  -637,   457,   484,  -551,   -53,  -576,   -64,
	  -573,  -642,  -644,  -638,  -645,   351,   484,  -525,   394,   -64,
	   -53,  -529,  -532,    40,   429,  -560,   -67,   -64,   -64,  -563,
	   -53,  -607,  -534,  -136,   -53,   -64,   -53,   -64,  -529,  -532,
	   378,  -157,   298,   -66,   484,   487,   -44,  -633,  -638,  -639,
	  -604,  -640,  -637,   286,   257,   296,  -277,   293,   -71,   257,
	   -61,   -44,   487,   -67,  -268,   -53,   -64,   -53,  -205,  -202,
	   285,  -204,  -203,   386,  -600,  -612,  -614,  -615,  -204,   400,
	   386,  -633,   -47,   -35,  -587,  -245,   257,  -246,   349,   335,
	   316,   353,   340,   409,   484,   351,  -178,   390,   382,   386,
	   -53,  -249,  -250,   257,  -190,   349,   335,   353,   316,   -64,
	   -64,   -53,  -592,   332,  -525,   -64,   -70,    -2,   -44,   -60,
	   486,    45,  -117,   379,   433,   -64,  -662,  -663,  -664,  -578,
	  -579,   298,  -231,   -53,   -53,   297,   266,   291,   481,   463,
	   276,   -97,   487,   -46,  -436,   -97,   -64,    40,  -109,  -111,
	   -64,   353,    40,    40,    40,   -97,   -97,   -47,  -111,    40,
	  -430,   276,  -432,  -118,   -71,    40,    40,    40,   -48,   -35,
	   -64,   -48,  -331,   276,   -97,   -64,  -332,    58,  -288,  -188,
	   387,   352,   -64,   484,   355,   443,   351,   333,   -28,   333,
	   390,   351,   286,   -28,   293,   -64,  -446,    -2,   276,   351,
	  -446,   -97,  -360,  -284,   349,  -338,    44,  -340,  -179,   258,
	   486,   485,   489,  -328,   256,    -6,   486,   386,   431,   430,
	   484,  -175,    -6,   487,   483,  -335,    -5,    -7,    -7,    -7,
	   -95,   -35,  -167,  -427,    40,   289,   455,  -386,  -303,    61,
	   -23,  -385,  -391,  -392,   280,  -357,   270,   -23,  -332,  -332,
	  -366,  -332,   -37,    40,   495,   -35,   484,   -48,   500,   -80,
	   -54,   -64,   -34,   484,   432,   -80,   -47,   -91,    40,  -473,
	    46,   -50,   -10,   -11,   491,    42,    47,    43,    45,   -35,
	   -35,    40,  -355,  -168,   -70,    46,   -69,   -71,   484,  -566,
	   411,   319,    44,  -157,  -157,  -529,  -529,  -573,  -561,  -157,
	  -568,  -569,  -570,   411,   319,  -577,  -157,  -643,  -647,   383,
	   298,  -643,  -551,  -646,  -214,   484,  -272,  -271,   293,   429,
	  -523,   484,   298,   296,   366,    40,  -530,  -533,  -535,  -540,
	  -537,  -538,  -539,   -64,   405,  -548,  -549,  -552,   406,   390,
	   408,   407,  -287,    40,   349,  -561,  -157,  -157,  -157,  -584,
	   484,  -600,    58,   -41,   483,   377,  -157,  -554,  -533,  -555,
	  -540,  -287,  -231,  -158,  -159,  -139,   -64,   281,   351,   346,
	   332,   379,   353,   333,  -157,  -473,   -65,   -60,   -44,  -473,
	    44,  -599,  -231,   351,   286,  -601,  -613,  -551,  -613,   315,
	  -588,  -201,   386,  -202,   400,   285,   401,    44,  -591,   484,
	   -63,    40,   -63,   -63,  -136,   333,   -53,  -231,   389,  -597,
	   386,   385,   389,    44,   389,  -145,   386,  -594,  -247,   257,
	  -248,   349,   335,   316,   353,   340,   409,   484,   484,   389,
	   391,  -473,   486,   -70,   386,   431,   -64,   437,   293,    -2,
	   -44,   436,   -64,   -64,   -33,   434,   435,  -664,  -529,  -529,
	  -163,  -164,  -150,   484,   -44,  -226,  -225,   378,   484,   -63,
	  -386,  -386,   257,   -67,  -106,    40,   -51,   -50,   -43,   -47,
	  -424,    -2,  -122,  -123,  -170,   -38,  -667,  -126,  -127,   -38,
	  -126,  -102,    40,  -102,  -382,   298,  -425,    -2,   -64,   484,
	  -115,  -116,  -124,   -16,   -17,    -2,   282,   466,   276,   351,
	   376,  -128,  -129,    -3,    -2,   293,  -128,  -113,  -112,   -64,
	  -332,   -64,  -106,   -64,   -73,  -167,  -285,    40,   349,   371,
	   257,   351,   333,  -447,  -447,   -99,    40,   -37,    41,  -286,
	  -189,   374,   257,    61,  -322,  -332,   484,   288,    -8,    -8,
	    -8,  -177,   453,   -23,   272,  -307,  -303,  -303,    61,   -37,
	   273,  -371,   -12,   267,   256,   490,   498,   -35,   -37,   -37,
	   -13,   -15,   -14,    60,    62,    61,   494,   492,   497,   383,
	   493,   496,   500,   -81,    40,   -84,    40,    46,   -84,   -92,
	    41,   -93,   -64,   282,   351,   484,   -35,   -35,   -35,    41,
	   -86,   -85,   -87,   -89,   -88,   -90,   -35,   260,   -39,   -72,
	   -43,   484,   487,   -44,   484,  -157,    61,   484,   484,   257,
	   -64,  -533,  -540,  -533,  -157,  -538,  -571,   484,   406,   406,
	  -573,  -647,   -71,   484,    46,  -271,   -65,   -71,   -64,    61,
	  -276,  -275,   484,  -151,  -152,  -153,   -64,  -531,  -536,    41,
	    44,  -531,  -536,  -541,  -138,   -64,   484,  -540,    40,  -529,
	  -529,  -529,  -188,  -157,  -287,  -286,  -263,  -262,   335,   353,
	   316,  -608,   279,   -53,    46,   -53,  -531,  -536,  -531,  -541,
	  -192,   399,  -223,   393,    44,    61,  -473,   -53,  -571,  -137,
	   -68,   257,   -65,  -602,  -603,  -604,  -605,   -64,   -60,  -536,
	  -589,   389,   361,   362,   363,  -268,   315,  -246,   -62,   -64,
	  -136,  -242,    40,   383,  -143,   -64,   342,  -231,  -190,   -64,
	  -160,    40,   -64,  -588,    44,  -591,   -63,   377,   -70,   -70,
	  -659,   -53,   -64,   393,   393,  -473,  -533,  -533,    44,    61,
	  -473,   350,  -227,  -229,  -228,  -231,    40,   429,  -107,   -98,
	   -64,    61,    41,    44,    61,   -43,    41,    44,    61,    41,
	  -105,  -442,   298,  -103,  -104,   -98,  -105,  -458,    61,  -432,
	    46,    41,    44,    61,    40,    41,  -125,   -70,   449,   454,
	   282,    41,    44,    61,    41,   389,   -20,   484,   -23,  -331,
	  -331,  -188,  -285,  -286,  -165,  -289,   -70,  -136,  -448,   -70,
	  -100,  -101,   -38,  -361,   295,   256,  -278,    42,  -279,  -469,
	  -339,   -23,   -30,    40,   298,   495,   -29,   351,   484,   -47,
	  -304,  -308,   -23,   -23,  -307,  -361,   -37,    41,   -35,  -147,
	   -55,   -40,   -44,  -181,   495,  -382,  -383,    41,   -85,    41,
	   484,    41,    44,    61,    61,    61,    41,  -384,  -314,  -384,
	    40,   -88,    46,  -355,  -355,  -355,   -65,    40,    40,  -531,
	  -531,  -157,   -64,   -64,  -157,    61,   484,  -274,  -273,   -71,
	    44,    61,    41,    44,    61,  -157,  -532,  -535,  -540,  -532,
	  -540,  -542,  -543,  -544,  -545,  -546,  -547,   -31,   -32,   298,
	   372,  -191,   405,  -548,  -549,  -550,   495,   409,    40,   484,
	  -538,  -210,    40,   495,  -208,  -214,   376,  -131,    45,  -209,
	  -182,  -207,   484,  -666,   -55,  -259,  -212,  -184,   423,   424,
	   425,   426,   427,   367,   292,   284,   296,   -44,  -533,  -533,
	  -533,  -287,    41,  -585,    44,   386,   385,   378,   382,    40,
	  -609,  -619,   484,  -616,   484,  -192,  -532,  -555,  -540,  -532,
	  -556,  -557,  -544,  -545,  -546,  -157,   484,  -210,   -44,  -159,
	  -178,    40,    44,  -536,  -551,  -590,   298,   -67,   -67,   -67,
	   -67,    44,    41,   386,  -241,   392,  -222,   349,  -243,  -244,
	   -64,    42,  -210,  -178,  -251,    40,   429,  -161,  -162,  -283,
	   -64,  -595,   378,   358,   359,   360,  -248,   -65,  -660,   298,
	   393,  -154,  -155,   -64,  -154,  -531,  -531,  -164,  -148,    40,
	  -149,   -70,   257,   372,   346,  -281,  -282,   -64,    42,    44,
	  -206,   384,   395,   396,   397,   398,  -229,  -287,    41,    44,
	   -47,  -123,  -124,  -127,    -3,    40,    41,    44,    61,  -459,
	  -460,  -462,   484,   -47,   -64,  -116,  -120,   -36,  -121,   386,
	   431,   430,    41,  -129,  -119,   -35,  -121,  -114,   -48,   272,
	  -386,  -285,    41,    44,  -235,   368,   373,   -44,    41,    44,
	    61,  -467,   382,    44,  -280,   484,  -234,  -208,   -46,    40,
	  -214,  -331,   486,   284,   284,   -30,   484,   276,   484,  -382,
	  -304,  -304,  -308,   -56,   375,   284,   484,   385,   -82,   -83,
	   -78,   -49,   -79,   484,    41,   -93,   -35,   -35,   -35,   -89,
	   -88,   -87,   -90,   -40,   257,   -67,   -67,   -71,    46,    44,
	  -275,   -70,   -21,   382,   378,  -153,  -134,   -64,   484,  -287,
	  -287,  -543,   372,   484,   284,  -270,   -44,  -259,   -58,    40,
	  -551,   484,   284,   372,   486,    40,    41,   -12,  -208,  -210,
	  -210,   -10,   -11,   491,   -13,   -14,  -186,  -185,   381,   495,
	   370,   -15,    40,  -208,    40,    40,    40,    40,  -531,  -531,
	  -531,   -53,  -262,   -67,  -610,  -628,  -629,  -630,  -631,   402,
	   371,   403,   404,  -620,  -621,  -622,  -623,  -624,  -625,  -626,
	  -627,   349,   335,   353,   316,   364,   365,  -617,  -618,   484,
	  -157,  -287,  -287,  -557,   484,  -473,  -141,   -70,   353,   257,
	   281,  -140,  -141,   -65,  -605,   332,   -64,   -53,    40,  -286,
	    41,    44,  -166,   386,  -252,   389,   -67,   -24,   484,    41,
	    44,  -144,   383,  -138,  -571,   -67,   -67,   -67,   -67,   -65,
	  -163,  -154,    44,    61,  -140,   -64,  -223,    44,    61,    61,
	  -229,  -228,    41,  -295,   298,   -98,    44,  -115,  -104,   -35,
	    44,  -463,    40,    61,    41,  -432,  -289,  -473,  -101,   -98,
	  -225,  -468,   484,  -470,   499,    46,    61,   429,  -208,  -332,
	   -18,    44,  -318,   298,   495,    46,   484,   487,   488,  -529,
	    41,  -384,  -355,   -79,   -49,    46,  -384,  -384,    41,    41,
	    41,   484,  -273,   -47,  -135,   298,    40,   484,  -157,  -157,
	  -270,  -210,  -529,   298,   495,    41,    44,   486,  -210,    41,
	    41,  -208,  -208,  -208,  -208,  -220,    40,  -207,  -187,   257,
	   388,  -213,  -212,   -61,  -218,    40,  -211,    45,  -209,  -217,
	  -208,   370,   381,  -222,    42,  -183,   374,   257,  -183,  -132,
	  -133,   484,  -215,  -208,  -550,  -260,   339,    41,  -611,   338,
	  -533,  -536,  -536,  -192,  -192,   -27,   484,   368,   373,    41,
	    44,   484,    40,  -237,  -238,  -236,  -239,  -224,    45,    40,
	  -240,   -39,   -46,   -55,  -259,   -40,  -184,   484,  -244,  -156,
	   393,   298,  -165,  -253,   369,   -64,   257,    41,  -146,   378,
	  -162,   -66,  -258,   -31,   -32,  -191,   298,  -155,   -70,   294,
	   293,   351,    41,  -282,  -211,    40,  -236,   -46,   -43,  -230,
	   386,   405,  -425,    41,  -461,  -464,  -465,   484,   484,  -223,
	    44,  -280,   -48,    42,  -471,  -472,   -23,    41,   486,   372,
	   372,   484,    46,   484,   -83,   -88,  -167,  -355,   484,   484,
	   487,   -44,  -157,   284,   486,    40,    41,  -540,   372,   372,
	   486,    41,  -221,   349,    40,    40,   -56,  -211,  -221,   -10,
	   -11,   491,  -211,   490,    41,  -648,  -211,  -211,  -211,    41,
	    44,    46,    61,    41,    44,  -551,  -223,  -261,   484,   -53,
	  -621,  -618,  -157,  -157,  -141,  -290,  -291,   -64,    41,    44,
	   -10,   -11,   491,  -239,  -239,    46,    40,  -158,  -158,  -254,
	   378,   -64,   257,  -157,   -53,  -194,   -32,  -191,   298,   495,
	   372,  -211,  -210,   484,    61,  -460,    41,    44,    61,  -232,
	   379,   484,   500,  -208,  -234,   484,   -46,   -30,   484,  -531,
	    40,    41,   486,  -531,    41,    41,  -189,  -221,  -221,    41,
	    44,    41,  -211,  -211,  -211,  -208,    41,    41,    41,    41,
	  -133,   484,  -171,    -1,   484,   483,  -215,  -529,   274,  -261,
	   429,   -69,    41,    44,   368,   373,   -64,  -238,  -239,  -239,
	  -239,    41,   257,  -216,  -239,  -256,   386,  -255,   -65,  -191,
	   495,   298,   -47,  -465,   -70,   372,  -233,   380,   371,    46,
	   -58,   -22,    -2,    41,  -208,    41,    41,  -219,  -211,    61,
	    46,  -533,   -53,   484,  -157,  -291,    41,    44,  -223,   -64,
	   389,    58,    41,  -210,  -257,  -214,    42,    41,   -22,  -225,
	    41,    44,  -171,   484,  -531,  -264,    40,  -239,   389,  -255,
	   -65,    44,  -223,  -211,  -266,  -267,   484,   -64,  -214,  -232,
	    41,    44,    61,  -233,  -267,  -265,   484,    46,   484
};

short yydef[] = {
	    -2,    -2,     6,     3,     4,     0,    21,     9,     0,     1,
	   263,    54,     0,    56,    58,    59,    68,     0,     0,   106,
	    86,    87,    88,    92,    97,    97,    97,     0,   104,  1333,
	  1334,    89,    90,    91,     0,     0,     0,     0,    21,    13,
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
	   238,   239,   538,   539,   540,   541,   542,   543,   544,   545,
	   546,   547,   548,   549,   550,   551,   552,   553,   554,   555,
	   556,   557,   558,   559,   560,   561,   562,   563,   564,   565,
	   566,   567,   568,   569,   570,   571,   572,   573,   574,   575,
	   576,   577,   578,   579,   580,   581,   582,   583,   584,   585,
	   586,     0,     0,   267,   268,   269,   270,   271,   272,   273,
	   274,   275,   276,   277,   278,   279,   280,   281,   282,   283,
	   284,   285,   286,   287,   288,   289,   290,   291,   292,   293,
	   294,     0,   168,   175,  1315,     0,     0,   185,   249,   240,
	   246,   255,   257,    -2,     0,     0,  1070,     0,     0,   773,
	     0,   955,     0,   773,     0,   653,     0,     0,     0,     0,
	     0,     0,     0,     0,   919,     0,     0,     0,     0,  1107,
	  1220,     0,     0,   611,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  1105,     0,     0,     0,     0,     0,     0,
	     0,   833,   653,     0,     0,     0,     0,  1084,  1085,     0,
	     0,     0,     0,   344,   344,     0,     0,   375,     0,     0,
	     0,   344,   344,     0,     0,   311,   314,   320,   326,     0,
	     0,     0,     0,   447,     0,     0,     0,   339,   344,     0,
	   110,   177,  1327,   188,   187,   528,  1079,  1071,  1072,  1073,
	  1074,  1075,  1076,   750,     0,     0,   763,   952,   756,   767,
	   963,   651,  1132,   742,   775,   747,   782,   920,   780,   765,
	     0,   613,     0,   951,   894,   901,   897,   898,   899,   900,
	   904,   905,   906,   902,   903,   907,   908,   909,   950,   805,
	   659,   616,   879,  1149,   854,  1158,  1225,  1223,  1224,  1225,
	  1245,  1246,   835,   656,  1161,  1083,  1178,   644,   738,   298,
	   348,   354,   300,   302,     0,     0,   422,   444,   360,   358,
	   305,   405,   312,   317,   321,   322,   323,   324,   325,   441,
	     0,   427,   446,   449,   329,   338,   344,   341,   370,   356,
	   151,   471,     0,   373,   409,   416,   153,   112,   127,   124,
	    25,    22,    41,    31,    32,    44,  1283,  1284,  1277,     0,
	  1272,  1273,  1274,  1275,  1276,    33,    34,    10,    14,    57,
	    67,     0,     0,     0,    -2,     0,     0,     0,     0,   106,
	    -2,  1319,  1320,   262,     0,     0,   245,     0,   248,   244,
	   265,   149,     0,     0,   159,   160,   110,   110,   162,   166,
	   164,   110,     0,   176,     0,  1329,     0,     0,     0,     0,
	   205,   189,   194,  1335,  1366,  1325,  1330,   186,     0,     0,
	     0,  1253,   202,   203,  1254,  1255,    -2,  1281,  1282,  1323,
	    -2,    -2,  1326,   250,     0,  1327,   471,   614,   617,   645,
	     0,  1080,  1065,  1357,     0,    -2,  1360,  1361,  1353,  1067,
	     0,     0,   751,  1341,  1107,  1107,     0,     0,  1209,  1210,
	   773,   748,  1107,   953,   954,   970,     0,   990,   766,   768,
	  1107,     0,     0,     0,     0,   980,   972,   650,   652,   591,
	     0,     0,     0,  1214,   666,  1107,   743,  1107,  1107,  1107,
	     0,     0,   918,   665,  1086,  1357,     0,  1107,     0,     0,
	     0,  1219,     0,  1107,  1337,  1338,  1366,   948,   956,   957,
	   958,   959,   960,   981,   973,   985,   607,     0,   609,   610,
	  1350,  1366,  1285,   880,   881,   657,   882,   884,     0,   910,
	   911,   912,   913,   915,   886,     0,     0,   923,   982,   983,
	   915,   947,  1064,  1262,   821,   806,   809,   810,   812,  1345,
	   814,  1345,   816,  1345,   818,     0,     0,  1104,     0,     0,
	     0,     0,   855,   856,   857,   859,   860,   861,   862,     0,
	  1231,  1243,     0,     0,   654,  1160,  1082,  1354,  1366,  1356,
	  1286,     0,  1162,     0,     0,  1198,  1169,  1182,  1184,     0,
	     0,     0,   502,   536,  1345,   172,   262,   262,   378,   295,
	     0,   350,  1259,   343,     0,   353,     0,     0,   371,   374,
	   406,   407,  1321,  1321,  1321,   365,   365,   452,   404,     0,
	   315,     0,   316,   440,   442,     0,     0,     0,   448,  1263,
	   332,   337,   110,     0,   350,     0,   150,    -2,   464,     0,
	     0,   476,   777,   778,     0,  1218,   895,   896,  1222,  1226,
	     0,  1228,  1229,  1244,  1179,   372,   387,   384,   385,   386,
	   387,   380,     0,     0,   482,     0,     0,   110,     0,    42,
	  1278,  1279,  1280,    71,    73,    74,    79,    80,    81,    82,
	    83,    75,    76,    84,    85,   109,   130,   102,   102,   102,
	   138,   148,   251,   309,   471,   255,   307,   242,     0,     0,
	   237,     0,     0,     0,   266,   156,   157,   158,   161,   165,
	     0,     0,     0,     0,     0,     0,  1328,     0,  1261,   209,
	   218,  1362,     0,    -2,  1365,   218,   190,   191,     0,  1336,
	     0,  1332,     0,     0,     0,  1288,  1289,  1290,  1291,     0,
	  1251,  1257,   471,   256,  1078,     0,  1107,  1352,  1068,   749,
	     0,     0,     0,   745,   776,     0,     0,  1107,   949,   994,
	   755,     0,   852,   759,     0,   773,   770,   961,   966,     0,
	     0,   962,   964,   965,   991,  1034,   590,   592,     0,     0,
	     0,     0,     0,   605,   606,  1135,     0,     0,   667,  1212,
	   669,   670,     0,  1211,     0,     0,     0,     0,   703,   699,
	   700,   702,  1107,     0,   482,   741,   744,   774,   746,     0,
	   783,   926,     0,  1088,  1089,     0,   764,     0,     0,   721,
	  1212,     0,   520,  1106,  1108,  1112,  1119,  1120,  1121,  1122,
	  1123,  1124,  1125,  1126,  1217,  1339,   608,  1346,  1366,  1351,
	     0,   852,     0,   914,   916,     0,   921,   992,   922,   984,
	     0,     0,   915,   822,   823,   824,   825,     0,   807,   808,
	   813,     0,   815,   817,   664,     0,   619,     0,     0,     0,
	   863,   864,   865,     0,     0,  1233,     0,   821,   836,   809,
	   838,   840,   841,   842,  1345,   844,   845,   846,     0,     0,
	     0,  1355,  1287,  1163,  1165,  1166,     0,  1170,     0,    -2,
	  1366,  1171,  1172,  1173,  1174,  1199,  1200,  1183,     0,     0,
	  1185,  1186,  1189,  1192,  1366,     0,   503,     0,   537,     0,
	   260,   261,   296,   297,   347,     0,   342,  1258,   345,   299,
	     0,   303,     0,   411,     0,   414,     0,     0,   423,     0,
	     0,   362,     0,   362,   304,   450,     0,   313,   326,  1333,
	     0,   418,     0,   390,     0,   394,   395,   396,   397,   398,
	   399,     0,   428,     0,   431,   432,     0,     0,    -2,   330,
	     0,   340,   369,   355,     0,     0,   466,     0,   482,     0,
	   475,     0,  1227,   410,   417,   377,  1321,     0,   465,   467,
	   487,   480,   481,   122,   126,     0,    38,     0,   132,   134,
	   136,     0,   308,   263,    -2,   114,     0,     0,     0,     0,
	   167,   169,     0,   170,   171,  1292,  1293,     0,     0,  1266,
	     0,  1268,     0,  1294,  1295,  1296,  1297,  1298,  1299,  1368,
	  1300,  1301,  1260,   452,   206,   214,     0,     0,   215,     0,
	   193,   196,     0,     0,     0,  1331,  1247,  1248,  1249,  1250,
	     0,  1256,   221,   222,   224,   229,   225,     0,   227,     0,
	  1326,    -2,    -2,    -2,  1358,  1066,     0,     0,     0,   754,
	  1340,     0,  1212,     0,   762,  1107,   758,   851,     0,     0,
	  1107,   967,   968,     0,     0,   593,   594,   595,   596,     0,
	   598,   602,     0,     0,  1136,  1137,     0,  1107,     0,  1215,
	  1216,     0,     0,   673,   674,  1093,  1333,     0,     0,     0,
	     0,     0,     0,   663,     0,   474,     0,   788,   790,   791,
	   793,   931,   924,  1087,     0,   779,     0,     0,     0,   724,
	  1107,     0,   612,     0,     0,  1105,  1347,   658,   883,   885,
	  1091,  1092,  1349,   887,   888,   889,   890,   892,   893,     0,
	   831,     0,     0,     0,     0,   819,   820,   811,     0,  1342,
	     0,     0,     0,     0,  1105,  1150,  1151,   867,   858,  1157,
	     0,     0,  1232,     0,     0,   837,   843,   832,   655,     0,
	  1181,  1167,  1168,     0,     0,    -2,     0,     0,     0,     0,
	  1193,     0,   504,   505,   507,   509,     0,     0,     0,   352,
	   346,     0,   408,  1321,     0,  1322,   421,  1321,     0,   443,
	   359,     0,   363,     0,   367,     0,   357,     0,     0,   318,
	     0,   415,     0,     0,   393,   391,     0,   400,   401,   402,
	   403,   426,     0,     0,   445,     0,   328,   335,   376,   262,
	   247,     0,     0,   470,   527,   529,   531,   719,   388,   389,
	     0,   381,     0,   152,   154,   155,   486,   483,   484,     0,
	     0,   125,    28,     0,     0,     0,    38,     0,     0,   452,
	   236,   243,   263,   263,   114,   163,  1265,  1264,  1267,  1304,
	  1270,  1271,  1324,     0,  1367,   204,     0,   208,     0,   217,
	  1363,   192,     0,     0,     0,     0,  1252,     0,   234,     0,
	     0,   230,     0,   231,   232,   233,  1069,     0,     0,   771,
	   772,   757,   760,   761,   769,     0,  1033,   597,   599,   601,
	     0,     0,     0,     0,     0,   660,     0,   668,     0,     0,
	  1213,   672,   675,   677,   678,   679,   680,   681,   682,     0,
	     0,   688,     0,   690,   691,     0,     0,   701,     0,  1097,
	   671,     0,     0,     0,     0,  1023,     0,  1045,     0,  1000,
	     0,     0,    -2,     0,  1020,  1021,  1022,     0,  1006,  1007,
	  1008,  1009,  1010,  1011,  1306,  1025,  1026,  1027,     0,     0,
	     0,   473,   472,     0,     0,   784,   785,   786,   787,     0,
	   941,     0,   971,     0,  1090,  1107,     0,   722,     0,     0,
	   723,   725,   727,   728,   729,   718,     0,   518,    -2,  1109,
	     0,     0,     0,     0,   993,   804,     0,   826,   827,   828,
	   829,     0,  1344,     0,   615,     0,   625,   482,     0,   620,
	   622,   623,   878,  1153,     0,     0,     0,     0,  1235,  1237,
	  1239,   852,     0,     0,     0,     0,   839,     0,  1164,     0,
	     0,  1175,  1201,     0,  1176,  1207,  1208,  1187,  1188,     0,
	  1191,  1194,  1195,  1196,     0,   520,   646,     0,     0,     0,
	     0,   511,   512,   513,   514,   515,     0,   740,   349,     0,
	     0,   412,   413,   424,   425,     0,   364,     0,     0,   451,
	   453,   459,     0,     0,   326,   419,   420,   435,   436,   437,
	   438,   439,   392,   429,   430,   433,   434,   327,   333,   334,
	   241,   469,   468,     0,   532,   533,   534,  1366,   379,  1321,
	     0,   479,     0,   489,   488,    -2,     0,   497,    -2,     0,
	  1023,   110,    36,    39,    52,    29,    45,     0,    49,   306,
	   258,   259,   264,  1269,     0,  1369,  1370,     0,     0,   211,
	   212,     0,   252,  1316,   216,   195,   197,   198,   199,   219,
	   223,   220,   228,     0,  1318,     0,     0,   969,     0,     0,
	   603,   604,     0,  1133,  1134,  1138,  1147,  1140,  1333,  1107,
	  1107,   676,  1102,   705,  1098,   684,   685,   686,   687,     0,
	   692,   704,  1099,  1103,     0,     0,   694,     0,     0,     0,
	  1037,     0,     0,     0,     0,     0,     0,     0,  1051,     0,
	  1048,  1039,     0,   999,  1013,  1013,     0,  1031,   695,   696,
	     0,   795,   789,     0,   946,     0,   942,   943,   944,   988,
	   989,   986,   987,   930,   932,   934,   935,   936,   937,   938,
	   939,   974,   975,   976,   977,   978,   979,   925,   927,   929,
	   715,     0,     0,   726,   720,   519,  1110,  1127,  1116,  1117,
	  1118,     0,  1114,  1348,   891,     0,  1343,     0,     0,   477,
	   618,     0,  1154,     0,   872,     0,     0,  1241,  1230,  1234,
	     0,  1238,     0,   712,   834,   847,   848,   849,   850,  1159,
	  1180,  1177,     0,     0,     0,  1197,   643,     0,     0,     0,
	   506,   517,   510,   737,     0,   351,     0,     0,   366,   368,
	   454,   456,     0,     0,   310,   319,   530,   535,   382,   383,
	   520,   485,   500,     0,     0,     0,   493,   495,     0,     0,
	     0,     0,    40,     0,     0,     0,    48,  1302,  1303,     0,
	   207,     0,   471,   253,     0,     0,     0,     0,   226,   752,
	   753,  1032,   600,  1107,  1139,     0,     0,     0,   661,   662,
	   683,     0,     0,     0,     0,  1094,     0,     0,  1036,   998,
	  1035,   995,   996,   997,  1038,  1041,     0,     0,     0,  1061,
	  1062,  1304,  1046,  1047,  1042,     0,  1054,     0,  1019,  1043,
	     0,  1049,  1052,     0,  1001,     0,     0,  1012,     0,     0,
	  1308,     0,     0,  1030,     0,   520,   797,   792,   917,     0,
	   940,     0,     0,  1107,  1107,  1115,  1128,  1129,  1130,  1111,
	     0,   830,     0,     0,   626,   628,   629,   587,     0,     0,
	   635,   588,  1325,   636,   637,   638,     0,    -2,   621,  1148,
	     0,     0,  1152,   874,     0,   868,   869,   866,  1107,     0,
	  1236,  1240,   706,   707,   708,   709,     0,  1202,  1203,  1204,
	  1205,  1206,  1190,   647,   648,     0,   649,     0,     0,   508,
	     0,     0,     0,   361,     0,     0,   460,     0,   457,   522,
	     0,   490,     0,   492,     0,     0,   123,    38,    37,    50,
	    51,    46,     0,     0,   210,   213,   254,   471,  1317,     0,
	     0,     0,  1131,  1145,     0,     0,   689,     0,  1100,  1101,
	     0,  1096,     0,   482,  1013,     0,  1040,     0,     0,     0,
	     0,     0,  1017,     0,  1044,     0,     0,     0,     0,  1305,
	     0,     0,     0,  1024,  1031,   697,     0,   797,     0,  1353,
	   933,   928,   716,   717,  1113,     0,   731,   733,   624,     0,
	     0,     0,     0,   633,     0,     0,   642,  1155,  1156,   877,
	     0,   870,   871,  1221,  1242,   710,   713,   714,     0,     0,
	  1102,     0,   516,   739,     0,   455,   458,     0,     0,   526,
	     0,   501,     0,   494,   496,   498,   499,    30,    47,  1371,
	     0,  1144,     0,   693,  1095,  1058,     0,     0,     0,  1018,
	     0,  1055,  1014,  1015,  1016,  1050,  1002,  1003,  1005,  1004,
	  1307,     0,  1310,  1311,  1313,  1314,  1029,     0,     0,   794,
	     0,   945,  1107,     0,   734,   735,   736,   627,   630,   631,
	   632,   634,   589,     0,   641,   520,     0,     0,     0,   711,
	     0,     0,     0,   461,   462,   463,   478,     0,     0,     0,
	     0,  1141,  1143,  1144,     0,  1060,  1059,     0,  1056,     0,
	     0,     0,   799,   796,   730,   732,   639,     0,   853,     0,
	     0,     0,   301,   525,   521,   523,   491,  1146,  1142,   520,
	  1053,     0,  1309,  1312,   698,   781,     0,   640,     0,   873,
	   875,     0,   522,  1057,     0,   800,     0,   876,   524,   526,
	   798,     0,     0,  1063,   801,   802,     0,     0,   803
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
#line 1120 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1134 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			oscerr(OSEEMPTY, 0);
		}
		break;
	case 7:
#line 1160 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1197 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGsetSID(IGtopSID());	/* display SID */
			IGgenStmt(IL_DISPLOOP, (IGSID *)NULL, 0);
		}
		break;
	case 9:
#line 1202 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			ostabsinit(FormSym);
			IGgenStmt(IL_INITIALIZE, (IGSID *)NULL, 0);

			IGsetSID(IGtopSID());	/* display SID */
			IGgenStmt(IL_DISPLOOP, (IGSID *)NULL, 0);
		}
		break;
	case 10:
#line 1211 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			ostabsinit(FormSym);
			IGgenStmt(IL_INITIALIZE, (IGSID *)NULL, 0);
		}
		break;
	case 11:
#line 1233 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1279 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_RETPROC, (IGSID *)NULL, 1, (ILREF)0);
			iiIGepEndProc(LocalFormSym->s_rdesc);
			osendproc(LocalFormSym);
		}
		break;
	case 13:
#line 1286 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1310 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_RETPROC, (IGSID *)NULL, 1, (ILREF)0);
			iiIGepEndProc(LocalFormSym->s_rdesc);
			osendproc(LocalFormSym);
		}
		break;
	case 15:
#line 1317 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1336 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_INITIALIZE, (IGSID *)NULL, 0);
		}
		break;
	case 25:
#line 1374 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			DeclareFlags &= ~FDF_LOCAL;
		}
		break;
	case 27:
#line 1381 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			DeclareFlags |= FDF_LOCAL;
		}
		break;
	case 28:
#line 1404 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1423 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1442 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1460 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1479 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyerror(_SyntaxError);
			_VOID_ ossymundef(yypvt[-2].st_node->n_name, LocalFormSym);
			ostrfree(yypvt[-0].st_node);
			ostrfree(yypvt[-2].st_node);
		}
		break;
	case 33:
#line 1487 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1503 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1519 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-0].st_name;
			u_ptr[1].u_cp = NULL;
			yyval.st_node = osmknode(ATTR, &u_ptr[0], &u_ptr[1], (U_ARG*)NULL);
		}
		break;
	case 36:
#line 1526 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = NULL;
		}
		break;
	case 37:
#line 1530 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 38:
#line 1535 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = NULL;
		}
		break;
	case 39:
#line 1539 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = ERx("with null");
		}
		break;
	case 40:
#line 1543 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = ERx("not null");
		}
		break;
	case 41:
#line 1548 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_nat = FALSE;
		}
		break;
	case 42:
#line 1552 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_nat = TRUE;
		}
		break;
	case 43:
#line 1557 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_nat = FALSE;
		}
		break;
	case 44:
#line 1561 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1576 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = osldtable(yypvt[-0].st_name, (char *)NULL);
		}
		break;
	case 46:
#line 1580 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = osldtable(yypvt[-0].st_name, yypvt[-2].st_name);
		}
		break;
	case 47:
#line 1584 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = osldform(yypvt[-2].st_name, yypvt[-0].st_name);
		}
		break;
	case 48:
#line 1588 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = osldform(FormSym->s_name, yypvt[-0].st_name);
		}
		break;
	case 49:
#line 1592 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
			if (!osw_compare(ERx("form"), yypvt[-1].st_name))
				yyerror(_SyntaxError);
			yyval.st_name = osldform(yypvt[-0].st_name, (char*)NULL);
		}
		break;
	case 55:
#line 1617 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			SubMInAlert &= ~1;
		}
		break;
	case 56:
#line 1621 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			i4	line;

			line = osscnlno();
			oscerr(OSEHEADER, 1, (PTR)&line);

			SubMInAlert &= ~1;

		}
		break;
	case 58:
#line 1640 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (ActState)
				IGsetSID(IGpopSID() /* operation SID */);
			ActState = FALSE;
		}
		break;
	case 61:
#line 1673 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGactivate( OLMENU, (PTR)osvalref(yypvt[-1].st_node), yypvt[-0].st_name,
					IGtopSID() /* operation SID */
			);
			SubMGotNonAlert |= 1;
		}
		break;
	case 62:
#line 1680 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1693 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1738 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1774 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{ i4  line = osscnlno();
				oscerr(OSEHEADER, 1, (PTR)&line);}
		break;
	case 67:
#line 1779 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = (PTR)ActOpt;
		}
		break;
	case 68:
#line 1783 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = NULL;
		}
		break;
	case 69:
#line 1788 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			ActOpt[OPT_EXPL]  = 0;
			ActOpt[OPT_VAL]   = FRS_UF;
			ActOpt[OPT_ACTIV] = FRS_UF;
		}
		break;
	case 74:
#line 1800 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1822 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1836 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1870 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( ActOpt[OPT_VAL] != FRS_UF )
			{
				yyerror(_SyntaxError);
			}
		}
		break;
	case 78:
#line 1878 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1904 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 80:
#line 1908 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = ERx("1");
		}
		break;
	case 81:
#line 1912 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = ERx("0");
		}
		break;
	case 82:
#line 1916 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = ERx("2");
		}
		break;
	case 83:
#line 1920 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1936 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_nat = IGsetConst( DB_CHR_TYPE,
					 osstring( FALSE, yypvt[-0].st_name, osldml ) );
		}
		break;
	case 85:
#line 1941 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1955 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1965 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1975 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 1989 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 90:
#line 1993 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 91:
#line 1997 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 92:
#line 2003 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 2013 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 2042 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_nat = FALSE;
		}
		break;
	case 95:
#line 2046 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_nat = FALSE;
		}
		break;
	case 96:
#line 2050 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_nat = TRUE;
		}
		break;
	case 97:
#line 2055 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 2070 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 99:
#line 2074 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = _All;
		}
		break;
	case 100:
#line 2079 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 101:
#line 2083 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = _All;
		}
		break;
	case 102:
#line 2087 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = NULL;
		}
		break;
	case 103:
#line 2093 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 2105 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_nat = OLALERTER;
		}
		break;
	case 105:
#line 2110 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkconst(tkICONST, yypvt[-0].st_name);
		}
		break;
	case 106:
#line 2114 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 107:
#line 2142 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 2161 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{ ostmpbeg(); osclrrefs(FormSym); }
		break;
	case 109:
#line 2162 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osgenputs();
			ostmpend();
		}
		break;
	case 110:
#line 2166 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{ ostmpbeg(); osclrrefs(FormSym); }
		break;
	case 111:
#line 2167 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osgenputs();
			ostmpend();
		}
		break;
	case 116:
#line 2181 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{oscerr(OSESEMIC, 0);}
		break;
	case 120:
#line 2191 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 121:
#line 2195 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = ERx("}");
		}
		break;
	case 122:
#line 2201 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{operr();}
		break;
	case 124:
#line 2203 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{operr();}
		break;
	case 129:
#line 2211 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{yyerror(_SyntaxError);}
		break;
	case 130:
#line 2211 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{YYERROR;}
		break;
	case 131:
#line 2213 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{yyerror(_SyntaxError);}
		break;
	case 132:
#line 2213 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{YYERROR;}
		break;
	case 133:
#line 2215 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{yyerror(_SyntaxError);}
		break;
	case 134:
#line 2215 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{YYERROR;}
		break;
	case 135:
#line 2217 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{yyerror(_SyntaxError);}
		break;
	case 136:
#line 2217 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{YYERROR;}
		break;
	case 137:
#line 2219 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{yyerror(_SyntaxError);}
		break;
	case 138:
#line 2219 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{YYERROR;}
		break;
	case 142:
#line 2228 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbflush();
			IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
		}
		break;
	case 148:
#line 2258 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 2315 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGsetSID(IGpopSID());	/* end SID */
		}
		break;
	case 150:
#line 2332 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			register IGSID	*elseSID = IGpopSID();

			IGgenStmt(IL_GOTO, IGtopSID() /* end SID */, 0);
			IGsetSID(elseSID);
		}
		break;
	case 151:
#line 2367 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{ostmpbeg();}
		break;
	case 152:
#line 2368 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			ILREF	refer;

			refer = osvalref(yypvt[-1].st_node);
			osgenputs();
			ostmpend();
			IGgenStmt(IL_IF, IGtopSID() /* else SID */, 1, refer);
		}
		break;
	case 153:
#line 2390 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGpushSID(IGinitSID());		/* end SID */
			IGpushSID(IGinitSID());		/* else SID */
		}
		break;
	case 155:
#line 2404 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{yyerror(ERx("THEN"));}
		break;
	case 158:
#line 2415 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{ /* Error! */
			i4	line = osscnlno();

			oscerr(OSENDIF, 2, (PTR)&line, yypvt[-0].st_name);
			YYERROR;
		}
		break;
	case 160:
#line 2425 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{ /* Error! */
			i4	line = osscnlno();

			oscerr(OSENDIF, 2, (PTR)&line, yypvt[-0].st_name);
			YYERROR;
		}
		break;
	case 161:
#line 2445 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			register IGSID	*elseSID = IGpopSID();

			IGgenStmt(IL_GOTO, IGtopSID() /* end SID */, 0);
			IGsetSID(elseSID);
		}
		break;
	case 162:
#line 2480 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{ostmpbeg();}
		break;
	case 163:
#line 2481 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			ILREF	refer;

			refer = osvalref(yypvt[-1].st_node);
			osgenputs();
			ostmpend();
			IGgenStmt(IL_IF, IGtopSID() /* else SID */, 1, refer);
		}
		break;
	case 164:
#line 2503 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGpushSID(IGinitSID());		/* new else SID */
		}
		break;
	case 166:
#line 2526 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 167:
#line 2551 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			register IGSID	*endSID = IGpopSID();

			IGgenStmt(IL_GOTO, IGpopSID() /* while SID */, 0);
			IGsetSID(endSID);
			osendblock();
		}
		break;
	case 168:
#line 2592 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{ostmpbeg();}
		break;
	case 169:
#line 2593 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			ILREF	refer;

			refer = osvalref(yypvt[-1].st_node);
			osgenputs();
			ostmpend();
			IGgenStmt(IL_IF, IGtopSID() /* end SID */, 1, refer);
		}
		break;
	case 171:
#line 2603 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{yyerror(ERx("DO"));}
		break;
	case 172:
#line 2620 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGpushSID(IGinitSID());		/* while SID */
			IGsetSID(IGtopSID());
			IGstartStmt(osscnlno(), IL_LB_WHILE);
			IGpushSID(IGinitSID());		/* end SID */
			osblock(LP_WHILE, IGtopSID(), yypvt[-1].st_name);
		}
		break;
	case 173:
#line 2634 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-1].st_name;
		}
		break;
	case 174:
#line 2639 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = NULL;
		}
		break;
	case 175:
#line 2656 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (osblkbreak(LP_NONDISPLAY, (char *)NULL) != OK)
			{
				oscerr(OSENDLOOP, 0);
			}
		}
		break;
	case 176:
#line 2663 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			_VOID_ osblkbreak(LP_LABEL, yypvt[-0].st_name);
		}
		break;
	case 177:
#line 2668 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 184:
#line 2691 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			_VOID_ osblkbreak(LP_NONE, (char *)NULL);
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_EXIT, (IGSID *)NULL, 0);
		}
		break;
	case 185:
#line 2710 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 2729 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 2771 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 188:
#line 2783 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 189:
#line 2794 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_CALSYS, (IGSID *)NULL, 1, 0);
		}
		break;
	case 190:
#line 2798 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_CALSYS, (IGSID *)NULL, 1, osvalref(yypvt[-0].st_node));
		}
		break;
	case 191:
#line 2810 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			ossubsys(yypvt[-1].st_node, yypvt[-0].st_node);
		}
		break;
	case 192:
#line 2815 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 193:
#line 2819 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 194:
#line 2823 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 195:
#line 2828 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 196:
#line 2835 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 197:
#line 2843 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 2854 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 2865 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 2884 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (yypvt[-1].st_node->n_flags & N_READONLY)
				yyval.st_node = NULL;
			else
				yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 201:
#line 2891 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			yyval.st_node = NULL;
		}
		break;
	case 202:
#line 2897 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 203:
#line 2901 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident( yypvt[-0].st_name, (OSNODE *)NULL );
			yyval.st_node->n_type = DB_CHA_TYPE;	/* special case */
		}
		break;
	case 204:
#line 2928 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 2991 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			FrsStmt = IIDISPFRM;
		}
		break;
	case 206:
#line 2996 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{CallState = TRUE;}
		break;
	case 207:
#line 2997 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			CallState = FALSE;
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 208:
#line 3002 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 209:
#line 3006 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 210:
#line 3011 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-2].st_node;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(COMMA, (U_ARG*)NULL, &u_ptr[0], &u_ptr[1]);
		}
		break;
	case 211:
#line 3017 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 212:
#line 3022 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 213:
#line 3026 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 3063 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 3137 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 3222 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 217:
#line 3226 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 218:
#line 3230 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 219:
#line 3236 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-2].st_node;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(COMMA, (U_ARG *)NULL, &u_ptr[0], &u_ptr[1]);
		}
		break;
	case 220:
#line 3242 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			yyval.st_node = osmknode(COMMA, (U_ARG *)NULL, &u_ptr[0], &u_ptr[1]);
		}
		break;
	case 221:
#line 3249 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 222:
#line 3253 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 223:
#line 3259 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-2].st_node;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode( COMMA, (U_ARG*)NULL, &u_ptr[0],
					&u_ptr[1]
			);
		}
		break;
	case 224:
#line 3267 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 225:
#line 3272 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 226:
#line 3276 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 3291 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(DOTALL, &u_ptr[0],
					(U_ARG*)NULL, (U_ARG*)NULL);
		}
		break;
	case 228:
#line 3299 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-2].st_node;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode( COMMA, (U_ARG *)NULL, &u_ptr[0],
					&u_ptr[1]
			);
		}
		break;
	case 229:
#line 3307 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 230:
#line 3312 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 3338 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkconst( tkSCONST, yypvt[-1].st_name );
		}
		break;
	case 232:
#line 3342 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkconst( tkSCONST, yypvt[-1].st_name );
		}
		break;
	case 233:
#line 3346 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (yypvt[-1].st_node->n_type != DB_NODT && !oschkstr(yypvt[-1].st_node->n_type))
				oscerr(OSNOTSTR, 1, ERget(_PrmKeyWord));
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 235:
#line 3371 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osqrygen( yypvt[-0].st_node, OS_QRYSTMT, !ProcState,
				  (IGSID *)NULL, (IGSID *)NULL );
			ostrfree(yypvt[-0].st_node);
		}
		break;
	case 236:
#line 3409 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 3448 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 3460 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 3490 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGSID		*qrySID;
			register OSNODE	*qry = osblkqry(&qrySID);

			IGstartStmt(osscnlno(), IL_LB_NONE);
			iiosQrNext( qry, (IGSID *)NULL,
				    (bool)(qry == NULL || SubMState > 1) );
		}
		break;
	case 240:
#line 3513 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 3529 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			QueryLabel = yypvt[-4].st_name;
			yyval.st_node = yypvt[-2].st_node;
		}
		break;
	case 242:
#line 3534 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			QueryLabel = NULL;
			yyval.st_node = yypvt[-2].st_node;
		}
		break;
	case 243:
#line 3558 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGsetSID(IGtopSID());		/* menu SID */
			IGstartStmt(osscnlno(), IL_LB_WHILE);
			IGgenStmt(IL_MENULOOP, (IGSID *)NULL, 0);
		}
		break;
	case 244:
#line 3564 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 3578 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 3602 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 3625 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			QueryLabel = yypvt[-3].st_name;
			yyval.st_qry = yypvt[-1].st_qry;
		}
		break;
	case 248:
#line 3630 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			QueryLabel = NULL;
			yyval.st_qry = yypvt[-1].st_qry;
		}
		break;
	case 249:
#line 3648 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 250:
#line 3652 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yypvt[-1].st_node->n_child = yypvt[-0].st_node;
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 251:
#line 3665 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_p = TargListFrag;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_qrynodep = yypvt[-0].st_qry;
			yyval.st_node = osmknode(tkQUERY, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
			TargListFrag = NULL;
		}
		break;
	case 252:
#line 3674 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 253:
#line 3678 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yypvt[-1].st_node->n_child = yypvt[-0].st_node;
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 254:
#line 3685 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_p = TargListFrag;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_qrynodep = yypvt[-0].st_qry;
			yyval.st_node = osmknode(tkQUERY, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
			TargListFrag = NULL;
		}
		break;
	case 255:
#line 3701 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{RepeatState = TRUE;}
		break;
	case 256:
#line 3702 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yypvt[-0].st_qry->qn_repeat = RepeatState; /*may have been set FALSE*/
			yyval.st_qry = yypvt[-0].st_qry;

			FormObj = FormSym;
			RepeatState = FALSE;
		}
		break;
	case 257:
#line 3710 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yypvt[-0].st_qry->qn_repeat = FALSE;
			yyval.st_qry = yypvt[-0].st_qry;

			FormObj = FormSym;
		}
		break;
	case 258:
#line 3753 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 3798 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGsetSID(IGpopSID());	/* menu SID */
			IGendMenu(IL_BEGSUBMU, IGpopSID());/* submenu top SID */
			IGsetSID(IGpopSID());	/* end submenu SID */
			osendblock();		/* sub-menu end */
		}
		break;
	case 260:
#line 3818 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 3837 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGpushSID(IGinitSID());		/* end submenu SID */
			osblock(LP_RUNMENU, IGtopSID(), yypvt[-2].st_name);
		}
		break;
	case 262:
#line 3844 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 3857 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 3891 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 3905 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 3918 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGpushSID(IGinitSID());		/* menu SID */
		}
		break;
	case 295:
#line 3979 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_CLRSCR, (IGSID *)NULL, 0);
		}
		break;
	case 296:
#line 3983 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_CLRALL, (IGSID *)NULL, 0);
		}
		break;
	case 297:
#line 3987 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osevallist(yypvt[-0].st_node, clrfld);
		}
		break;
	case 298:
#line 3992 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (ProcState)
				oscerr(OSINPROC, 1, yypvt[-0].st_name);
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 299:
#line 4012 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_HLPFILE, (IGSID *)NULL,
					2, osvalref(yypvt[-1].st_node), osvalref(yypvt[-0].st_node)
				);
		}
		break;
	case 300:
#line 4019 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 301:
#line 4038 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_HLPFORMS, (IGSID *)NULL,
				2, osvalref(yypvt[-5].st_node), osvalref(yypvt[-1].st_node)
				);
		}
		break;
	case 302:
#line 4045 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 303:
#line 4050 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (!osw_compare(_Subject, yypvt[-0].st_name))
				yyerror(ERx("SUBJECT"));
		}
		break;
	case 304:
#line 4068 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_MESSAGE, (IGSID *)NULL, 1, osvalref(yypvt[-1].st_node));
		}
		break;
	case 305:
#line 4073 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			FrsStmt = IIMESSAGE;
		}
		break;
	case 306:
#line 4095 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 4118 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			FrsStmt = IINPROMPT;
		}
		break;
	case 308:
#line 4123 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_nat = TRUE;
		}
		break;
	case 309:
#line 4127 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_nat = FALSE;
		}
		break;
	case 310:
#line 4144 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_PRNTSCR, (IGSID *)NULL, 1, osvalref(yypvt[-1].st_node));
		}
		break;
	case 311:
#line 4148 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_PRNTSCR, (IGSID *)NULL, 1, 0);
		}
		break;
	case 312:
#line 4153 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 313:
#line 4159 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (!osw_compare(_File, yypvt[-0].st_name))
				yyerror(ERx("FILE"));
		}
		break;
	case 314:
#line 4176 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (ProcState)
				oscerr(OSINPROC, 1, yypvt[-0].st_name);
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_REDISPLAY, (IGSID *)NULL, 0);
		}
		break;
	case 315:
#line 4210 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (DispLoopCount > 0)
			{
				_VOID_ osblkbreak(LP_DISPLAY, (char *)NULL);
			}
		}
		break;
	case 316:
#line 4217 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (DispLoopCount > 0)
			{
				_VOID_ osblkbreak(LP_DISPLAY, (char *)NULL);
			}
		}
		break;
	case 317:
#line 4225 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (DispLoopCount == 0)
			{
				oscerr(E_OS016E_NotInDisplayLoop, 1, yypvt[-0].st_name);
			}
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 318:
#line 4234 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 4260 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			resumecolumn(yypvt[-3].st_name, yypvt[-1].st_node);
		}
		break;
	case 320:
#line 4264 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_RESUME, (IGSID *)NULL, 0);
		}
		break;
	case 321:
#line 4269 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (DispLoopCount == 0)
			{
				oscerr(E_OS016E_NotInDisplayLoop, 1, yypvt[-0].st_name);
			}
			IGgenStmt(IL_RESENTRY, (IGSID *)NULL, 0);
		}
		break;
	case 322:
#line 4277 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (DispLoopCount == 0)
			{
				oscerr(E_OS016E_NotInDisplayLoop, 1, yypvt[-0].st_name);
			}
			IGgenStmt(IL_RESNEXT, (IGSID *)NULL, 0);
		}
		break;
	case 323:
#line 4285 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (DispLoopCount == 0)
			{
				oscerr(E_OS016E_NotInDisplayLoop, 1, yypvt[-0].st_name);
			}
			IGgenStmt(IL_RESMENU, (IGSID *)NULL, 0);
		}
		break;
	case 324:
#line 4293 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (DispLoopCount == 0)
			{
				oscerr(E_OS016E_NotInDisplayLoop, 1, yypvt[-0].st_name);
			}
			IGgenStmt(IL_RESNFLD, (IGSID *)NULL, 0);
		}
		break;
	case 325:
#line 4301 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (DispLoopCount == 0)
			{
				oscerr(E_OS016E_NotInDisplayLoop, 1, yypvt[-0].st_name);
			}
			IGgenStmt(IL_RESPFLD, (IGSID *)NULL, 0);
		}
		break;
	case 326:
#line 4310 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 4335 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( yypvt[-2].st_node == NULL )
				scroll( (OSNODE *)NULL, yypvt[-3].st_node, yypvt[-0].st_node );
			else
				scroll( yypvt[-3].st_node, yypvt[-2].st_node, yypvt[-0].st_node );
		}
		break;
	case 328:
#line 4342 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 4382 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt( osscnlno(), IL_LB_NONE );
		}
		break;
	case 330:
#line 4387 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 331:
#line 4392 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 332:
#line 4396 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( ProcState )
				oscerr( OSINPROC, 1, ERx("SCROLL <table field>") );
			yyval.st_node = NULL;
		}
		break;
	case 333:
#line 4403 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 334:
#line 4407 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			char	buf[16];

			yyval.st_node = osmkconst( tkICONST,
				iiIG_string( STprintf(buf, ERx("%d"), rowEND) )
			);
		}
		break;
	case 335:
#line 4416 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 336:
#line 4420 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = NULL;
		}
		break;
	case 337:
#line 4436 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_SLEEP, (IGSID *)NULL, 1, osvalref(yypvt[-0].st_node));
		}
		break;
	case 338:
#line 4441 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 339:
#line 4463 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 4474 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 4488 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 4512 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 4521 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{ /* variable with a ':' */
			yyval.st_node = yypvt[-0].st_node;
			if (!oschkstr(yypvt[-0].st_node->n_type))
				oscerr(OSNOTSTR, 1, ERget(_TblFldName));
			else if ( AFE_NULLABLE_MACRO(yypvt[-0].st_node->n_type) )
				oswarn(OSNULVCHK, 0);
		}
		break;
	case 344:
#line 4528 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{AllowForm = FALSE;}
		break;
	case 345:
#line 4529 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{ /* variable - could be tblfld or array */
			yyval.st_node = os_ta_check(FormSym, yypvt[-0].st_node);
			if (!osiserr(yyval.st_node))
				TableObj = osnodesym(yyval.st_node);
		}
		break;
	case 346:
#line 4537 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 4596 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osrowops(FormSym, (IGSID*)NULL, IL_CLRROW, yypvt[-1].st_node);
			if (yypvt[-0].st_node != NULL)
				osevallist(yypvt[-0].st_node, ostl1elm);
			IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
			TableObj = NULL;
		}
		break;
	case 348:
#line 4605 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 349:
#line 4610 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 350:
#line 4614 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 351:
#line 4619 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 352:
#line 4626 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 353:
#line 4646 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osrowops(FormSym, (IGSID*)NULL, IL_DELROW, yypvt[-0].st_node);
			TableObj = NULL;
		}
		break;
	case 354:
#line 4652 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 355:
#line 4669 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 4687 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 357:
#line 4700 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 4739 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (ProcState)			/* Fix bug #47930 */
				oscerr(OSINPROC, 1, yypvt[-0].st_name);
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 359:
#line 4755 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 4777 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 361:
#line 4782 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 362:
#line 4786 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 363:
#line 4791 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			frs_insrow_with();
		}
		break;
	case 364:
#line 4796 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 365:
#line 4800 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 366:
#line 4805 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 367:
#line 4812 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 368:
#line 4820 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 4868 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 4884 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 4920 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 4938 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (yypvt[-0].st_node->n_token == tkID)
				_VOID_ ostblcheck(FormSym, yypvt[-0].st_node->n_value);
			else
				oswarn(OSTABINIT, 0);
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 373:
#line 4947 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (ProcState)
				oscerr(OSINPROC, 1, yypvt[-0].st_name);
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 374:
#line 4954 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (yypvt[-0].st_node != NULL && yypvt[-0].st_node->n_token == tkID)
				yypvt[-0].st_node->n_value =
				 iiIG_string(frsck_mode(ERx("INITTABLE"), yypvt[-0].st_node->n_value));
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 375:
#line 4961 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 376:
#line 4973 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 4984 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 5005 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (osblkutbl())
				oscerr(OSNESTUNLD, 0);
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGpushSID(IGinitSID());	/* end SID () */
			osblock(LP_UNLOADTBL, IGtopSID(), yypvt[-1].st_name);
		}
		break;
	case 379:
#line 5014 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 380:
#line 5018 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 381:
#line 5023 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 382:
#line 5030 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 383:
#line 5038 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 5078 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			frs_object(yypvt[-0].st_name);
		}
		break;
	case 385:
#line 5082 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			frs_object(yypvt[-0].st_name);
		}
		break;
	case 386:
#line 5086 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			frs_object(yypvt[-0].st_name);
		}
		break;
	case 389:
#line 5094 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 5111 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osfrs_constant(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 391:
#line 5115 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osfrs_constant(yypvt[-1].st_name, (OSNODE *)NULL);
		}
		break;
	case 392:
#line 5119 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osfrs_constant(yypvt[-2].st_name, yypvt[-1].st_node);
		}
		break;
	case 393:
#line 5125 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (osfrs_old())
				yyerror(_SyntaxError);
			yyval.st_name = yypvt[-1].st_name;
		}
		break;
	case 394:
#line 5132 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 395:
#line 5136 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 396:
#line 5140 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 397:
#line 5144 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 398:
#line 5148 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 399:
#line 5152 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 400:
#line 5157 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 401:
#line 5161 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkconst( tkSCONST, yypvt[-0].st_name );
		}
		break;
	case 402:
#line 5165 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkconst( tkSCONST, yypvt[-0].st_name );
		}
		break;
	case 403:
#line 5169 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkconst( tkSCONST, yypvt[-0].st_name );
		}
		break;
	case 404:
#line 5187 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (yypvt[-0].st_node != NULL && yypvt[-0].st_node->n_token == tkID)
				yypvt[-0].st_node->n_value =
				  iiIG_string(frsck_mode(ERx("MODE"), yypvt[-0].st_node->n_value));
			IGgenStmt(IL_MODE, (IGSID *)NULL, 1, osvalref(yypvt[-0].st_node));
		}
		break;
	case 405:
#line 5195 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 406:
#line 5200 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 407:
#line 5204 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;	/* special case */
		}
		break;
	case 408:
#line 5224 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			frs_head();
			osevallist(yypvt[-1].st_node, frs_gen);
			IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
		}
		break;
	case 409:
#line 5231 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			frs_inqset(FRSinq);
		}
		break;
	case 410:
#line 5237 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			frs_style();	/* recognize style semantically */
		}
		break;
	case 411:
#line 5242 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 5256 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 5271 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 5292 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 5319 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			frs_head();
			osevallist(yypvt[-1].st_node, frs_gen);
			IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
		}
		break;
	case 416:
#line 5326 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			frs_inqset(FRSset);
		}
		break;
	case 417:
#line 5332 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			frs_style();	/* recognize style semantically */
		}
		break;
	case 418:
#line 5337 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 419:
#line 5344 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 420:
#line 5352 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 5376 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_INQING, (IGSID *)NULL, 0);
			osevallist(yypvt[-1].st_node, ostl2elm);
			IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
		}
		break;
	case 422:
#line 5383 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 423:
#line 5388 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 424:
#line 5395 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 425:
#line 5403 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 5427 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 5438 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			SetSession = FALSE;
		}
		break;
	case 428:
#line 5444 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 429:
#line 5451 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 430:
#line 5459 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkassign( IGsetConst(DB_CHA_TYPE, yypvt[-2].st_name),
						osvarref(yypvt[-0].st_node), FALSE
			);
		}
		break;
	case 431:
#line 5466 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 432:
#line 5470 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			SetSession = TRUE;
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 433:
#line 5477 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 434:
#line 5481 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 435:
#line 5487 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 436:
#line 5491 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 437:
#line 5497 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkconst(tkICONST, ERx("1"));
		}
		break;
	case 438:
#line 5501 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkconst(tkICONST, ERx("0"));
		}
		break;
	case 439:
#line 5505 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
                     	yyval.st_node = osmkconst(tkICONST, ERx("2"));
		}
		break;
	case 440:
#line 5519 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 5532 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		IGstartStmt(osscnlno(), IL_LB_NONE);
	}
		break;
	case 442:
#line 5538 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		yyval.st_node = osmkassign( IGsetConst(DB_CHA_TYPE, 
				ERx("connection_name")), osvarref(yypvt[-0].st_node), FALSE);
	}
		break;
	case 443:
#line 5553 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_INQ4GL, (IGSID *)NULL, 0);
			osevallist(yypvt[-1].st_node, ostl2elm);
			IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
		}
		break;
	case 444:
#line 5560 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 445:
#line 5575 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_SET4GL, (IGSID *)NULL, 0);
			osevallist(yypvt[-1].st_node, ostl2elm);
			IGgenStmt(IL_ENDLIST, (IGSID *)NULL, 0);
		}
		break;
	case 446:
#line 5582 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 447:
#line 5599 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 5608 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 5626 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 450:
#line 5645 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{_VOID_ EQFWopen( FrsStmt );}
		break;
	case 451:
#line 5646 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			frs_optgen();
		}
		break;
	case 454:
#line 5652 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{yyerror(_SyntaxError);}
		break;
	case 457:
#line 5657 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			EQFWwcoption(yypvt[-2].st_name, yypvt[-0].st_name);
		}
		break;
	case 462:
#line 5668 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
#line 5694 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
				EQFWsuboption( yypvt[-2].st_name, DB_CHR_TYPE, yypvt[-0].st_name, (PTR)NULL );
			}
		break;
	case 464:
#line 5726 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_qry = osmkqry(DB_SQL, yypvt[-1].st_qry, yypvt[-0].st_srtlist);
		}
		break;
	case 465:
#line 5731 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_qry = osmkquery(PARENS, (char *)NULL, (PTR)yypvt[-1].st_qry,
				(PTR)NULL, (OSNODE*)NULL,
				(OSNODE*)NULL, (OSNODE*)NULL
			    );
		}
		break;
	case 466:
#line 5738 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_qry = osmkquery(OP, yypvt[-1].st_name, (PTR)yypvt[-2].st_qry, (PTR)yypvt[-0].st_qry,
				(PTR)NULL, (OSNODE*)NULL,
				(OSNODE*)NULL, (OSNODE*)NULL
			    );
		}
		break;
	case 467:
#line 5745 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_qry = yypvt[-0].st_qry;
		}
		break;
	case 468:
#line 5750 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_qry = osmkquery(PARENS, (char *)NULL, (PTR)yypvt[-1].st_qry,
				(PTR)NULL, (OSNODE*)NULL,
				(OSNODE*)NULL, (OSNODE*)NULL
			    );
		}
		break;
	case 469:
#line 5757 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_qry = osmkquery(OP, yypvt[-1].st_name, (PTR)yypvt[-2].st_qry, (PTR)yypvt[-0].st_qry,
				(PTR)NULL, (OSNODE*)NULL,
				(OSNODE*)NULL, (OSNODE*)NULL
			    );
		}
		break;
	case 470:
#line 5764 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_qry = yypvt[-0].st_qry;
		}
		break;
	case 471:
#line 5768 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( FormObj == NULL )
			{ /* Either a CALLFRAME parameter or a bug */
				if ( !CallState )
				{ /* Bug check: (for JupBug #9133.) */
					osuerr( OSBUG, 1,
						ERx("sql(missing query object)")
					);
				}
			}
			else if ( FormObj != FormSym
				&&  FormObj->s_kind != OSTABLE
				&&  ( FormObj->s_kind != OSVAR
					&& FormObj->s_kind != OSGLOB
					&& FormObj->s_kind != OSRATTR
				  || (FormObj->s_flags & (FDF_RECORD|FDF_ARRAY))
						== 0 ) )
			{
				if ( FormObj->s_kind == OSUNDEF 
					&& FormObj->s_parent->s_kind != OSFORM )
				{
					oscerr( OSNOTCUR, 2,
						FormObj->s_parent->s_name,
						FormSym->s_name
					);
				}
				else
				{
					oscerr( OSNOQRYOBJ, 2,
						FormObj->s_name,
						FormSym->s_name
					);
				}

				FormObj = NULL;
			}
			QryState = TRUE;

			/*
			** We're about to start generating the IL to:
			** (1) assign a selected row into the appropriate
			** targets (recomputing the target l-values), and
			** (2) compute the values of any variables in
			** any DML expressions in the target list.
			** We want to redirect (1) into a "fragment"
			** which can be emitted where appropriate
			** (after the initial QRYSINGLE and after each QRYNEXT).
			** Unfortunately, (1) and (2) may be interspersed.
			** So we'll redirect each contiguous piece
			** of (1) and (2) into a smaller fragment of IL.
			** As each of these smaller fragments is completed,
			** it will be included in either:
			** (1) The fragment TargListFrag (set "empty" below), or
			** (2) The "default" fragment.
			**
			** We also begin a temp block that will contain
			** temporaries in (1).
			*/
			ostmpbeg();
			iiIGofOpenFragment();
			TargListFrag = iiIGcfCloseFragment();
		}
		break;
	case 472:
#line 5841 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_qry = osmkquery(PARENS, (char*)NULL, (PTR)yypvt[-1].st_qry,
				(PTR)NULL, (OSNODE*)NULL,
				(OSNODE*)NULL, (OSNODE*)NULL
			    );
		}
		break;
	case 473:
#line 5848 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_qry = osmkquery(OP, yypvt[-1].st_name, (PTR)yypvt[-2].st_qry, (PTR)yypvt[-0].st_qry, (OSNODE*)NULL,
				(OSNODE*)NULL, (OSNODE*)NULL
			    );
		}
		break;
	case 474:
#line 5854 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_qry = yypvt[-0].st_qry;
		}
		break;
	case 475:
#line 5860 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = ERx("union all");
		}
		break;
	case 476:
#line 5864 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 477:
#line 5870 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = NULL;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_qrynodep = yypvt[-0].st_qry;
			yyval.st_node = osmknode(tkQUERY, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 478:
#line 5880 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_qry = osmkquery(tkQUERY, yypvt[-6].st_name, (PTR)yypvt[-5].st_tlist,
				(PTR)yypvt[-2].st_node, yypvt[-3].st_node, yypvt[-1].st_node, yypvt[-0].st_node
			    );
		}
		break;
	case 479:
#line 5886 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (TableRef)
				yyerror(ERx("FROM"));
			yyval.st_qry = osmkquery(tkQUERY, yypvt[-2].st_name, (PTR)yypvt[-1].st_tlist,
				(PTR)NULL, (OSNODE*)NULL,
				(OSNODE*)NULL, (OSNODE*)NULL
			    );
		}
		break;
	case 480:
#line 5897 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			TableRef = FALSE;
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 481:
#line 5902 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			TableRef = FALSE;
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 482:
#line 5907 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			TableRef = FALSE;
			yyval.st_name = NULL;
		}
		break;
	case 483:
#line 5914 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			open_targ_IL();
			TableRef = TRUE;
			if (QryState)
			{
				yyval.st_tlist = ostlqryall(QryTarget, FormObj, ERx("*"));
				if (yyval.st_tlist == NULL)
				{
					oscerr( OSBADQRYALL, 2,
						ERx("*"), FormSym->s_name );
				}
			}
			else
			{
				yyval.st_tlist = osmaketle( osmkident( ERx("*"),
							   (OSNODE *)NULL ),
						(OSNODE *)NULL,
						(OSNODE *)NULL );
			}
			close_lhs_targ_IL();
		}
		break;
	case 484:
#line 5936 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = yypvt[-0].st_tlist;
		}
		break;
	case 485:
#line 5940 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			oscerr(OSINTO, 0);
			yyval.st_tlist = yypvt[-2].st_tlist;
		}
		break;
	case 486:
#line 5947 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (QryState)
			{
				QryState = FALSE;

				/*
				** We've just about finished generating the IL
				** to assign a selected row into the appropriate
				** targets (recomputing the target l-values).
				** All that's left to do is to end the tmp block
				** associated with this IL, which may emit IL
				** (e.g. IL_RELEASEOBJ instructions).  This IL
				** *must* go into the same fragment that already
				** contains the IL to recompute target l-values.
				**
				** We also "freeze" the temporaries that have
				** generated so far in the enclosing temp block
				** (which *must* be the one that spans the
				** entire SELECT statement).  This prevents
				** IL within the SELECT loop or submenu
				** from reusing temps that were used in
				** the IL to recompute SELECT target l-values
				** (which will get emitted wherever a NEXT
				** is encountered).
				*/
				iiIGrfReopenFragment(TargListFrag);
				ostmpend();
				ostmpfreeze();
				TargListFrag = iiIGcfCloseFragment();
			}
		}
		break;
	case 487:
#line 5980 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{ open_targ_IL(); }
		break;
	case 488:
#line 5981 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = yypvt[-0].st_tlist;
		}
		break;
	case 489:
#line 5984 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{ open_targ_IL(); }
		break;
	case 490:
#line 5985 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = osaddtlist(yypvt[-3].st_tlist, yypvt[-0].st_tlist);
		}
		break;
	case 491:
#line 5990 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			oscerr(OSQRYTFALL, 1, ERx(".*"));
			ostrfree(yypvt[-3].st_node);
			yyval.st_tlist = NULL;
			close_lhs_targ_IL();
		}
		break;
	case 492:
#line 5997 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			TableRef = TRUE;
			if (QryState)
			{
				yyval.st_tlist = ostlqryall(QryTarget, FormObj, yypvt[-2].st_name);
				if (yyval.st_tlist == NULL)
				{
					oscerr( OSBADQRYALL, 2,
						ERx("*"), FormSym->s_name );
				}
			}
			else
			{ /* non-query select DB statment */
				u_ptr[0].u_cp = yypvt[-2].st_name;
				u_ptr[1].u_cp = ERx("*");
				yyval.st_tlist = osmaketle( osmknode( ATTR,
							  &u_ptr[0], &u_ptr[1],
							  (U_ARG*)NULL ),
						(OSNODE *)NULL,
						(OSNODE *)NULL );
			} /* end non-query DB statement */
			close_lhs_targ_IL();
		}
		break;
	case 493:
#line 6020 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{ close_lhs_targ_IL(); open_targ_IL(); }
		break;
	case 494:
#line 6021 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			close_rhs_targ_IL();
			yyval.st_tlist = ( (yypvt[-3].st_node != NULL) ? osmaketle(yypvt[-3].st_node, yypvt[-0].st_node, (OSNODE*)NULL)
					    : NULL );
		}
		break;
	case 495:
#line 6026 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{ close_rhs_targ_IL(); open_targ_IL(); }
		break;
	case 496:
#line 6027 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			close_lhs_targ_IL();
			yyval.st_tlist = ( (yypvt[-0].st_node != NULL) ? osmaketle(yypvt[-0].st_node, yypvt[-3].st_node, (OSNODE*)NULL)
					    : NULL );
		}
		break;
	case 497:
#line 6033 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (!QryState)
			{
				yyval.st_tlist = osmaketle( yypvt[-0].st_node,
						(OSNODE *)NULL,
						(OSNODE *)NULL );
			}
			else if (  yypvt[-0].st_node->n_token == VALUE
				|| yypvt[-0].st_node->n_token == ARRAYREF
				|| (  yypvt[-0].st_node->n_token == DOT
				   && yypvt[-0].st_node->n_right->n_token == VALUE
				   )
				)
			{ /* field or variable */
				oscerr(E_OS015C_BadColonInSelect, 0);
				ostrfree(yypvt[-0].st_node);
				yyval.st_tlist = NULL;
			}
			else if ( ! (  yypvt[-0].st_node->n_token == DOT
				    || yypvt[-0].st_node->n_token == ATTR
				    || (  yypvt[-0].st_node->n_token == tkID
				       && yypvt[-0].st_node->n_left == NULL
				       )
				    )
				)
			{
				oscerr(OSSELLIST, 0);
				ostrfree(yypvt[-0].st_node);
				yyval.st_tlist = NULL;
			}
			else if (FormObj == NULL)
			{
				yyval.st_tlist = osmaketle( yypvt[-0].st_node,
						(OSNODE *)NULL,
						(OSNODE *)NULL );
			}
			else
			{ /* database column name */
				register OSSYM	*sym;
				register char	*attr;
				register OSNODE	*dml_obj;
				register OSNODE	*ele;

				if (yypvt[-0].st_node->n_token == tkID)
				{
					attr = yypvt[-0].st_node->n_value;
				}
				else if (yypvt[-0].st_node->n_token == ATTR)
				{
					attr = yypvt[-0].st_node->n_attr;
				}
				else if (yypvt[-0].st_node->n_right->n_token == DOT)
				{
					attr = yypvt[-0].st_node->n_right->n_right->n_value;
				}
				else /* single DOT node */
				{
					attr = yypvt[-0].st_node->n_right->n_value;
				}
				sym = osqryidchk(FormObj, attr);

				if (  yypvt[-0].st_node->n_token == ATTR
				   || yypvt[-0].st_node->n_token == DOT
				   || STequal(yypvt[-0].st_node->n_value, sym->s_name)
				   )
				{
					dml_obj = yypvt[-0].st_node;
				}
				else
				{
					dml_obj = NULL;
					ostrfree(yypvt[-0].st_node);
				}
				if (  FormObj == FormSym 
				   || FormObj->s_kind == OSTABLE
				   )
				{
					u_ptr[0].u_symp = sym;
					ele = osmknode( VALUE,
							&u_ptr[0],
							(U_ARG*)NULL,
							(U_ARG*)NULL );
				}
				else
				{
					ele = os_lhs(osqrydot(QryTarget, sym));
				}
				yyval.st_tlist = osmaketle(ele, dml_obj, (OSNODE *)NULL);
			}
			close_lhs_targ_IL();
		}
		break;
	case 498:
#line 6126 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (!QryState || FormObj == NULL)
			{
				yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			}
			else
			{ /* target list in query select */
				register OSSYM	*sym;

				sym = osqryidchk(FormObj, yypvt[-0].st_name);

				if (  FormObj == FormSym
				   || FormObj->s_kind == OSTABLE )
				{
					u_ptr[0].u_symp = sym;
					yyval.st_node =  osmknode( VALUE, &u_ptr[0],
							(U_ARG *)NULL,
							(U_ARG *)NULL );
				}
				else
				{
					yyval.st_node = os_lhs(osqrydot(QryTarget, sym));
				}
			} /* end query select target list */
		}
		break;
	case 499:
#line 6152 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (!QryState || FormObj != FormSym)
			{
				oscerr(E_OS015C_BadColonInSelect, 0);
				yyval.st_node = os_lhs(yypvt[-0].st_node);
			}
			else if (yypvt[-0].st_node->n_token == VALUE)
			{
				yyval.st_node = os_lhs(yypvt[-0].st_node);
				/*
				** If we're selecting into the form,
				** but the target for this particular
				** column is a tablefield cell, then
				** generate the necessary IL_PUTROW.
				** Note that we *don't* generate an
				** IL_PUTFORM for a simple field;
				** that's handled via the query's
				** PUTFORM string.  (See osquery.c).
				*/
				if (yyval.st_node->n_tfref != NULL)
				{
					osvardisplay(yyval.st_node);
				}
			}
			else
			{	/*
				** Handle the case where we're looking at
				** a DOT or ARRAYREF node.  Note that we're
				** selecting into the form (FormObj == FormSym).
				** We call os_lhs($1) to emit IL to build
				** a placeholder temp representing the
				** ultimate LHS.  Then we allocate a true temp
				** and emit IL to assign it to the ultimate LHS.
				** We return the true temp, which is what
				** we'll select into.
				**
				** Important note: before allocationg the true
				** temp, we must "freeze" all temporaries that
				** were freed by nested temp blocks.
				** Otherwise, the true temp may overlay one of
				** those nested temps, but at run time the
				** nested temp will be use *between* the time
				** of the actual query and the time of the
				** IL_ASSIGN below.  (Phew!)
				*/
				OSNODE	*tmpnode;	/* placeholder temp */
				OSNODE	*datnode;	/* true temp */
				OSSYM	*sym;
				char	*name = osnodesym(yypvt[-0].st_node)->s_name;

				tmpnode = os_lhs(yypvt[-0].st_node);
				ostmpfreeze();
				sym = osdatalloc( tmpnode->n_type,
						  (i4)tmpnode->n_length,
						  (i2)tmpnode->n_prec );
				sym->s_name = name;
				u_ptr[0].u_symp = sym;

				datnode = osmknode( VALUE, &u_ptr[0],
						    (U_ARG *)NULL,
						    (U_ARG *)NULL );

				IGgenStmt( IL_ASSIGN, (IGSID *)NULL, 3,
					   tmpnode->n_ilref, datnode->n_ilref,
					   oschkcoerce( datnode->n_type,
							tmpnode->n_type ) );
				ostrfree(tmpnode);
				yyval.st_node = datnode;
			}
		}
		break;
	default:
		    goto label_500;
	}

label_500:
	switch (yym) {
	case 502:
#line 6238 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 503:
#line 6242 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 504:
#line 6248 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 505:
#line 6253 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 506:
#line 6257 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    u_ptr[0].u_nodep = yypvt[-2].st_node;
		    u_ptr[1].u_nodep = yypvt[-0].st_node;
		    yyval.st_node = osmknode(COMMA, (U_ARG *)NULL, &u_ptr[0], &u_ptr[1]);
		}
		break;
	case 507:
#line 6264 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 508:
#line 6269 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    u_ptr[0].u_nodep = yypvt[-3].st_node;
		    u_ptr[1].u_nodep = osmkident(yypvt[-2].st_name, (OSNODE *)NULL);
		    u_ptr[0].u_nodep = 
			osmknode(BLANK, (OSNODE *)NULL, &u_ptr[0], &u_ptr[1]);
		    u_ptr[1].u_nodep = yypvt[-1].st_node;
		    yyval.st_node = osmknode(BLANK, (OSNODE *)NULL, &u_ptr[0], &u_ptr[1]);
		    if (yypvt[-0].st_node != NULL)
		    {
			u_ptr[0].u_nodep = yyval.st_node;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(BLANK, (OSNODE *)NULL, &u_ptr[0], 
								&u_ptr[1]);
		    }
		}
		break;
	case 509:
#line 6286 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 510:
#line 6290 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    u_ptr[0].u_nodep = yypvt[-1].st_node;
		    yyval.st_node = osmknode(PARENS, (U_ARG*)NULL, &u_ptr[0], 
								(U_ARG*)NULL);
		}
		break;
	case 511:
#line 6297 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 512:
#line 6301 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = iiIG_string(ERx("inner join"));
		}
		break;
	case 513:
#line 6305 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = iiIG_string(ERx("full join"));
		}
		break;
	case 514:
#line 6309 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = iiIG_string(ERx("left join"));
		}
		break;
	case 515:
#line 6313 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = iiIG_string(ERx("right join"));
		}
		break;
	case 516:
#line 6319 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    u_ptr[0].u_nodep = osmkident(yypvt[-1].st_name, (OSNODE *)NULL);
		    u_ptr[1].u_nodep = yypvt[-0].st_node;
		    u_ptr[2].u_nodep = osmknode(BLANK, (OSNODE *)NULL, &u_ptr[0], &u_ptr[1]);
		    yyval.st_node = osmknode(ONCLAUSE, (OSNODE *)NULL, &u_ptr[2]  , (OSNODE *)NULL);


		}
		break;
	case 517:
#line 6328 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 518:
#line 6336 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 519:
#line 6340 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osval(yypvt[-1].st_node);
			if (yypvt[-1].st_node->n_type != DB_NODT && !oschkstr(yypvt[-1].st_node->n_type))
				oscerr(OSNOTSTRX, 0);
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 520:
#line 6347 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 521:
#line 6356 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 522:
#line 6360 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			 yyval.st_node = NULL;
		}
		break;
	case 523:
#line 6368 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 524:
#line 6372 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-2].st_node;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(COMMA, (U_ARG*)NULL,
						&u_ptr[0], &u_ptr[1]);
		}
		break;
	case 525:
#line 6384 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 526:
#line 6388 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 527:
#line 6397 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yypvt[-0].st_srtlist->srt_type = SRT_ORDER;
			yyval.st_srtlist = yypvt[-0].st_srtlist;
		}
		break;
	case 528:
#line 6402 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_srtlist = NULL;
		}
		break;
	case 529:
#line 6407 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_srtlist = yypvt[-0].st_srtlist;
		}
		break;
	case 530:
#line 6411 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			/* Maintain sort list as circular linked list */
			yypvt[-0].st_srtlist->srt_next = yypvt[-2].st_srtlist->srt_next;
			yypvt[-2].st_srtlist->srt_next = yypvt[-0].st_srtlist;
			yyval.st_srtlist = yypvt[-0].st_srtlist;
		}
		break;
	case 531:
#line 6422 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_srtlist = osmksrtnode(yypvt[-0].st_node, (OSNODE *)NULL);
		}
		break;
	case 532:
#line 6426 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_srtlist = osmksrtnode(yypvt[-1].st_node, yypvt[-0].st_node);
		}
		break;
	case 533:
#line 6431 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 534:
#line 6435 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 535:
#line 6439 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (yypvt[-1].st_node == NULL || !oschkstr(yypvt[-1].st_node->n_type))
				oscerr(OSNOTSTR, 0);
			osval(yypvt[-1].st_node);
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 536:
#line 6452 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
    	    yyval.st_node = yypvt[-0].st_node;
    	}
		break;
	case 537:
#line 6456 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    u_ptr[0].u_nodep = yypvt[-1].st_node;
	    u_ptr[1].u_nodep = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
	    yyval.st_node = osmknode(BLANK, (U_ARG *) NULL, &u_ptr[0], &u_ptr[1]);
    	}
		break;
	case 587:
#line 6539 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 588:
#line 6545 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 589:
#line 6549 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osall(FormSym, yypvt[-2].st_node);
		}
		break;
	case 590:
#line 6573 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    /* bypass all invalid arg checking for now */
		    IGstartStmt(osscnlno(), IL_LB_NONE);
		    IGgenStmt(IL_CONNECT, (IGSID *)NULL, 1, osvalref(yypvt[-1].st_node));
		    connect_clauses(yypvt[-0].st_node);
		}
		break;
	case 591:
#line 6580 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    IGstartStmt(osscnlno(), IL_LB_NONE);
		    IGgenStmt(IL_CONNECT, (IGSID *)NULL, 1, osvalref(yypvt[-0].st_node));
		}
		break;
	case 592:
#line 6587 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 593:
#line 6591 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    u_ptr[0].u_nodep = yypvt[-1].st_node;
		    u_ptr[1].u_nodep = yypvt[-0].st_node;
		    yyval.st_node = osmknode(COMMA, (U_ARG *)NULL, &u_ptr[0], &u_ptr[1]);
		}
		break;
	case 594:
#line 6598 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    yyval.st_node = osmkassign(
				IGsetConst(DB_CHA_TYPE, ERx("ii_sess")),
				osvalref(yypvt[-0].st_node), FALSE);
		}
		break;
	case 595:
#line 6604 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    yyval.st_node = osmkassign(
				IGsetConst(DB_CHA_TYPE, ERx("ii_conn")),
				osvalref(yypvt[-0].st_node), FALSE);
		}
		break;
	case 596:
#line 6610 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    yyval.st_node = osmkassign(
				IGsetConst(DB_CHA_TYPE, ERx("ii_user")),
				osvalref(yypvt[-0].st_node), FALSE);
		}
		break;
	case 597:
#line 6624 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    if ( !osw_compare(ERx("options"), yypvt[-2].st_name) )
		    {
			yyerror(_SyntaxError);
			yyval.st_node = NULL;
		    }
		    else
		    {
			yyval.st_node = yypvt[-0].st_node;
		    }
		}
		break;
	case 598:
#line 6636 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 599:
#line 6642 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 600:
#line 6646 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    u_ptr[0].u_nodep = yypvt[-2].st_node;
		    u_ptr[1].u_nodep = yypvt[-0].st_node;
		    yyval.st_node = osmknode(COMMA, (U_ARG *)NULL, &u_ptr[0], &u_ptr[1]);
		}
		break;
	case 601:
#line 6653 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    yyval.st_node = osmkassign(		
				IGsetConst(DB_CHA_TYPE, ERx("ii_flag")),
				osvalref(yypvt[-0].st_node), FALSE);
		}
		break;
	case 602:
#line 6661 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 603:
#line 6665 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    u_ptr[0].u_nodep = yypvt[-2].st_node;
		    u_ptr[1].u_nodep = yypvt[-0].st_node;
		    yyval.st_node = osmknode(COMMA, (U_ARG *)NULL, &u_ptr[0], &u_ptr[1]);
		}
		break;
	case 604:
#line 6672 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    yyval.st_node = osmkassign(		
				IGsetConst(DB_CHA_TYPE, yypvt[-2].st_name),
				osvarref(yypvt[-0].st_node), FALSE);
		}
		break;
	case 607:
#line 6692 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			/* bypass all invalid arg checking for now */
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_DISCONNECT, (IGSID *)NULL, 0);
			if (yypvt[-0].st_node != NULL)
				connect_clauses(yypvt[-0].st_node);
		}
		break;
	case 608:
#line 6702 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    yyval.st_node = osmkassign(
				IGsetConst(DB_CHA_TYPE, ERx("ii_sess")),
				osvalref(yypvt[-0].st_node), FALSE);
		}
		break;
	case 609:
#line 6708 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    yyval.st_node = osmkassign(
				IGsetConst(DB_CHA_TYPE, ERx("ii_conn")),
				osvalref(yypvt[-0].st_node), FALSE);
		}
		break;
	case 610:
#line 6714 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		    yyval.st_node = osmkassign(
				IGsetConst(DB_CHA_TYPE, ERx("ii_all")),
				(ILREF)0, FALSE);
		}
		break;
	case 611:
#line 6720 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 612:
#line 6732 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IIOSgqsGenQryStmt(IL_DELETEFRM, RepeatState);
			osdbeval(yypvt[-1].st_node);
			if (yypvt[-0].st_node != NULL)
			{ /* delete qualification */
				osqwhere(yypvt[-0].st_node);
			}
			RepeatState = FALSE;
		}
		break;
	case 613:
#line 6743 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			yyval.st_nat = FALSE;
		}
		break;
	case 614:
#line 6748 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			RepeatState = TRUE;
			yyval.st_nat = TRUE;
		}
		break;
	case 615:
#line 6763 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IIOSgqsGenQryStmt(IL_INSERT, RepeatState);
			osdbeval(yypvt[-2].st_node);
			if (yypvt[-1].st_node != NULL)
			{ /* insert column list */
				if (yypvt[-0].st_node->n_token == tkQUERY && !insert_all(yypvt[-1].st_node->n_left))
				    oscerr(OSINSSTAR, 0);   /* "*" with sub-query */
				else if (yypvt[-0].st_node->n_token != tkQUERY && InsNcols > 0)
					oscerr(OSINSLISTS, 0);	/* unbalanced */
				osdbeval(yypvt[-1].st_node);	/* column name list */
			}
			if (yypvt[-0].st_node->n_token != tkQUERY)
			{
				osdbstr(ERx(" values "));
				osqtraverse(yypvt[-0].st_node);
			}
			else
			{
				osdbstr(ERx(" "));
				osdbqry(yypvt[-0].st_node->n_query);
				yypvt[-0].st_node->n_query = NULL;
				ostrfree(yypvt[-0].st_node);
			}
			RepeatState = FALSE;
		}
		break;
	case 616:
#line 6790 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			yyval.st_nat = FALSE;
		}
		break;
	case 617:
#line 6795 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			RepeatState = TRUE;
			yyval.st_nat = TRUE;
		}
		break;
	case 618:
#line 6802 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = InsCols = yypvt[-1].st_node;
			yyval.st_node = osmknode(PARENS, (U_ARG*)NULL, &u_ptr[0], (U_ARG*)NULL);
		}
		break;
	case 619:
#line 6807 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = InsCols = NULL;
		}
		break;
	case 620:
#line 6812 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			InsNcols = 1;
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 621:
#line 6817 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			InsNcols += 1;
			u_ptr[0].u_nodep = yypvt[-2].st_node;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(COMMA, (U_ARG*)NULL, &u_ptr[0], &u_ptr[1]);
		}
		break;
	case 622:
#line 6825 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 623:
#line 6830 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(ERx("*"), (OSNODE *)NULL);
		}
		break;
	case 624:
#line 6835 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-1].st_node;
			yyval.st_node = osmknode(PARENS, (U_ARG*)NULL, &u_ptr[0], (U_ARG*)NULL);
		}
		break;
	case 625:
#line 6840 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 626:
#line 6845 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 627:
#line 6849 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-2].st_node;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(COMMA, (U_ARG*)NULL, &u_ptr[0], &u_ptr[1]);
		}
		break;
	case 628:
#line 6856 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			register OSNODE	*col_n;

			if (InsCols == NULL || InsNcols == 0 ||
			 (col_n = insert_column(InsCols, InsNcols--)) == NULL ||
			   col_n->n_token != tkID || *col_n->n_value != '*')
			{
				oscerr(OSINSALL, 0);	/* no "*" in column list */
				ostrfree(yypvt[-0].st_node);
				yyval.st_node = osmkident(ERx("*.all"), (OSNODE *)NULL);
			}
			else
			{
				yyval.st_node = expand_insert(yypvt[-0].st_node, col_n);
			}
		}
		break;
	case 629:
#line 6873 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			register OSNODE	*col_n;

			if (InsCols != NULL)
			{
				if (InsNcols == 0)
					oscerr(OSINSLISTS, 0);	/* unbalanced */
				else if ((col_n=insert_column(InsCols,InsNcols--)) != NULL
				  && col_n->n_token == tkID && *col_n->n_value == '*')
					oscerr(OSINSSTAR, 0);
				yyval.st_node = yypvt[-0].st_node;
			}
		}
		break;
	case 630:
#line 6888 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-1].st_name;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(DML|OP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 631:
#line 6895 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-1].st_name;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(DML|OP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 632:
#line 6902 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = _Exp;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(DML|OP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 633:
#line 6909 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = _Minus;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(DML|UNARYOP, &u_ptr[0], &u_ptr[1],
				(U_ARG*)NULL
				 );
		}
		break;
	case 634:
#line 6917 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-1].st_node;
			yyval.st_node = osmknode(PARENS, (U_ARG*)NULL, &u_ptr[0], (U_ARG*)NULL);
		}
		break;
	case 635:
#line 6922 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 636:
#line 6927 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 637:
#line 6931 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 638:
#line 6935 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 639:
#line 6939 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-1].st_node;
			yyval.st_node = osmkident(yypvt[-3].st_name, osmknode(PARENS, (U_ARG*)NULL,
							 &u_ptr[0], (U_ARG*)NULL
					   )
			 );
		}
		break;
	case 640:
#line 6949 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-2].st_node;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(COMMA, (U_ARG*)NULL, &u_ptr[0], &u_ptr[1]);
		}
		break;
	case 641:
#line 6955 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 642:
#line 6959 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 643:
#line 6973 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IIOSgqsGenQryStmt(IL_UPDATE, RepeatState);
			osdbeval(yypvt[-4].st_node);
			if (yypvt[-3].st_node != NULL)
			{
				osOpenwarn(ERx("UPDATE ... FROM"));
				osdbstr(ERx(" from "));
				osdbeval(yypvt[-3].st_node);
			}
			osdbstr(ERx(" set "));
			osevaltlist(yypvt[-1].st_tlist, osdbqtle);
			if (yypvt[-0].st_node != NULL)
			{ /* update qualification */
				osqwhere(yypvt[-0].st_node);
			}
			RepeatState = FALSE;
		}
		break;
	case 644:
#line 6992 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			yyval.st_nat = FALSE;
		}
		break;
	case 645:
#line 6997 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			RepeatState = TRUE;
			yyval.st_nat = TRUE;
		}
		break;
	case 646:
#line 7004 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = yypvt[-0].st_tlist;
		}
		break;
	case 647:
#line 7008 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = osaddtlist(yypvt[-2].st_tlist, yypvt[-0].st_tlist);
		}
		break;
	case 648:
#line 7013 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = osmaketle(yypvt[-2].st_node, yypvt[-0].st_node, (OSNODE *)NULL);
		}
		break;
	case 649:
#line 7017 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = yypvt[-0].st_node != NULL ? ostlall(yypvt[-0].st_node) : NULL;
		}
		break;
	case 650:
#line 7036 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_COMMIT, (IGSID *)NULL, 0);
		}
		break;
	case 651:
#line 7041 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 654:
#line 7056 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_ROLLBACK, (IGSID *)NULL, 0);
		}
		break;
	case 655:
#line 7060 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("ROLLBACK ... TO"));
			IGgenStmt(IL_ROLLBACK, (IGSID *)NULL, 0);
			osdbstr(ERx(" to "));
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 656:
#line 7068 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 657:
#line 7090 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
    	    u_ptr[0].u_nodep = yypvt[-0].st_node;
    	    u_ptr[1].u_nodep = NULL;
    	    u_ptr[2].u_nodep = NULL;
    	    yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
    	}
		break;
	case 658:
#line 7097 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
    	    u_ptr[0].u_nodep = yypvt[-0].st_node;
    	    u_ptr[1].u_nodep = yypvt[-2].st_node;
    	    u_ptr[2].u_nodep = NULL;
    	    yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
	}
		break;
	case 659:
#line 7106 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 660:
#line 7135 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbwith(yypvt[-0].st_tlist);
	}
		break;
	case 661:
#line 7140 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbqry(yypvt[-1].st_qry);
	    osdbwith(yypvt[-0].st_tlist);
	}
		break;
	case 662:
#line 7146 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbqry(yypvt[-1].st_qry);
	    osdbwith(yypvt[-0].st_tlist);
	}
		break;
	case 663:
#line 7151 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbqry(yypvt[-1].st_qry);
	    osdbwith(yypvt[-0].st_tlist);
	}
		break;
	case 664:
#line 7157 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		IGgenStmt(IL_CRTTABLE, (IGSID *)NULL, 0);
		osdbeval(yypvt[-0].st_node);
	}
		break;
	case 665:
#line 7163 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		osdbeval(yypvt[-0].st_node);
	}
		break;
	case 666:
#line 7168 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		osOpenwarn(ERx("CREATE TABLE ... AS SELECT"));
		osdbstr(ERx(" as "));
	}
		break;
	case 674:
#line 7184 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		osdbstr(ERx(" "));
		osdbeval(yypvt[-0].st_node);
	}
		break;
	case 681:
#line 7199 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		osdbstr(ERx(" "));
		osdbstr(yypvt[-0].st_name);
	}
		break;
	case 682:
#line 7206 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		osdbstr(ERx(" "));
		osdbstr(yypvt[-0].st_name);
	}
		break;
	case 683:
#line 7211 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		osdbstr(ERx(" with default "));
		osdbeval(yypvt[-0].st_node);
	}
		break;
	case 684:
#line 7216 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		osdbstr(ERx(" default "));
		osdbeval(yypvt[-0].st_node);
	}
		break;
	case 685:
#line 7223 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		if ( AFE_NULLABLE_MACRO(yypvt[-0].st_node->n_type) )
			oswarn(OSNULVCHK, 0);
		yyval.st_node = yypvt[-0].st_node;
	}
		break;
	case 686:
#line 7229 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 687:
#line 7233 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		yyval.st_node = yypvt[-0].st_node;
	}
		break;
	case 688:
#line 7238 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		osdbstr(yypvt[-0].st_name);
	}
		break;
	case 689:
#line 7245 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		osdbstr(ERx(" check ("));
		osqtraverse(yypvt[-1].st_node);
		osdbstr(ERx(")"));
	}
		break;
	case 694:
#line 7258 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		osdbstr(ERx(" check ("));
		osqtraverse(yypvt[-1].st_node);
		osdbstr(ERx(")"));
	}
		break;
	case 699:
#line 7271 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		osdbstr(ERx(" unique"));
	}
		break;
	case 700:
#line 7276 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		osdbstr(ERx(" primary key"));
	}
		break;
	case 701:
#line 7281 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		osdbstr(ERx(" references "));
	}
		break;
	case 702:
#line 7286 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		osdbstr(ERx(" foreign key "));
	}
		break;
	case 703:
#line 7291 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		osdbstr(ERx(" constraint "));
	}
		break;
	case 704:
#line 7298 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		if ( !osw_compare(ERx("system_maintained"), yypvt[-0].st_name) )
			yyerror(_SyntaxError);
		yyval.st_name = ERx(" not system_maintained");
	}
		break;
	case 705:
#line 7304 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		if ( !osw_compare(ERx("system_maintained"), yypvt[-0].st_name) )
			yyerror(_SyntaxError);
		yyval.st_name = ERx(" with system_maintained");
	}
		break;
	case 706:
#line 7313 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    yyval.st_tlist = osmaketle(yypvt[-2].st_node, yypvt[-1].st_node, yypvt[-0].st_node);
	}
		break;
	case 707:
#line 7318 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 708:
#line 7322 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 709:
#line 7326 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 710:
#line 7330 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			char	buf[OSBUFSIZE];

			_VOID_ STprintf(buf, ERx("%s %s"), yypvt[-1].st_name, yypvt[-0].st_name);
			yyval.st_node = osmkident( iiIG_string(buf), (OSNODE *)NULL);
		}
		break;
	case 711:
#line 7337 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			char	buf[OSBUFSIZE];

			_VOID_ STprintf(buf, ERx("%s %s %s"), yypvt[-2].st_name, yypvt[-1].st_name, yypvt[-0].st_name);
			yyval.st_node = osmkident( iiIG_string(buf), (OSNODE *)NULL);
		}
		break;
	case 712:
#line 7344 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 713:
#line 7350 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 714:
#line 7354 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 715:
#line 7382 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbstr(yypvt[-1].st_name);
	    osdbwith(yypvt[-0].st_tlist);
	}
		break;
	case 716:
#line 7388 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbqry(yypvt[-2].st_qry);
	    osdbstr(yypvt[-1].st_name);
	    osdbwith(yypvt[-0].st_tlist);
	}
		break;
	case 717:
#line 7395 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbqry(yypvt[-2].st_qry);
	    osdbstr(yypvt[-1].st_name);
	    osdbwith(yypvt[-0].st_tlist);
	}
		break;
	case 718:
#line 7402 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbqry(yypvt[-2].st_qry);
	    osdbstr(yypvt[-1].st_name);
	    osdbwith(yypvt[-0].st_tlist);
	}
		break;
	case 719:
#line 7409 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		STARTSTMTOW("DECLARE GLOBAL TEMPORARY TABLE");
		IGgenStmt(IL_DGTT, (IGSID *)NULL, 0);
		osdbeval(yypvt[-0].st_node);
	}
		break;
	case 720:
#line 7416 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		if ( !osw_compare(ERx("preserve"), yypvt[-1].st_name) 
		  || !osw_compare(ERx("rows"), yypvt[-0].st_name)
		   )
			yyerror(_SyntaxError);
		yyval.st_name = ERx(" on commit preserve rows ");
	}
		break;
	case 730:
#line 7448 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(yypvt[-8].st_nat ? IL_CRTUINDEX : IL_CRTINDEX, (IGSID *)NULL, 0);
			osdbeval(yypvt[-6].st_node);
			osdbstr(ERx(" on "));
			osdbeval(yypvt[-4].st_node);
			osdbstr(ERx("("));
			osevalsklist(yypvt[-2].st_srtlist, osdbsrtkey, osldml);
			osdbstr(ERx(")"));
			osdbwith(yypvt[-0].st_tlist);
		}
		break;
	case 731:
#line 7460 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_srtlist = yypvt[-0].st_srtlist;
		}
		break;
	case 732:
#line 7464 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			/* Maintain sort list as circular linked list */
			yypvt[-0].st_srtlist->srt_next = yypvt[-2].st_srtlist->srt_next;
			yypvt[-2].st_srtlist->srt_next = yypvt[-0].st_srtlist;
			yyval.st_srtlist = yypvt[-0].st_srtlist;
		}
		break;
	case 733:
#line 7472 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_srtlist = osmksrtnode(yypvt[-0].st_node, (OSNODE *)NULL);
		}
		break;
	case 734:
#line 7476 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_srtlist = osmksrtnode(yypvt[-1].st_node, osmkident(yypvt[-0].st_name, (OSNODE *) NULL));
		}
		break;
	case 735:
#line 7480 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_srtlist = osmksrtnode(yypvt[-1].st_node, osmkident(yypvt[-0].st_name, (OSNODE *) NULL));
		}
		break;
	case 736:
#line 7484 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_srtlist = osmksrtnode(yypvt[-1].st_node, yypvt[-0].st_node);
		}
		break;
	case 737:
#line 7497 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_CRTVIEW, (IGSID *)NULL, 0);
			osdbeval(yypvt[-4].st_node);
			if (yypvt[-3].st_node != NULL)
				osdbeval(yypvt[-3].st_node);	/* column name list (with "()") */
			osdbstr(ERx(" as "));
			osdbqry(yypvt[-1].st_qry);
			if (yypvt[-0].st_nat != NULL)
				osdbstr(ERx(" with check option"));
		}
		break;
	case 738:
#line 7509 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 739:
#line 7514 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (!osw_compare(ERx("option"), yypvt[-0].st_name))
				yyerror(_SyntaxError);
			yyval.st_nat = TRUE;
		}
		break;
	case 740:
#line 7520 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_nat = FALSE;
		}
		break;
	case 742:
#line 7536 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			STARTSTMTOW("CREATE GROUP");
			IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
			osdbstr(ERx("create group "));
		}
		break;
	case 743:
#line 7544 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osevallist(yypvt[-0].st_node, osdblist);
		}
		break;
	case 744:
#line 7559 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			STARTSTMTOW("CREATE LOCATION");
			IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
			osdbstr(ERx("create location "));
			osdbeval(yypvt[-1].st_node);
			osdbwith(yypvt[-0].st_tlist);
		}
		break;
	case 745:
#line 7576 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			STARTSTMTOW("ALTER LOCATION");
			IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
			osdbstr(ERx("alter location "));
			osdbeval(yypvt[-1].st_node);
			osdbwith(yypvt[-0].st_tlist);
		}
		break;
	case 746:
#line 7593 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
					osdbwith(yypvt[-0].st_tlist);	
				}
		break;
	case 747:
#line 7599 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			STARTSTMTOW("CREATE ROLE");
			IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
			osdbstr(ERx("create role "));
		}
		break;
	case 748:
#line 7607 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osevallist(yypvt[-0].st_node, osdblist);
		}
		break;
	case 750:
#line 7626 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			STARTSTMTOW("ALTER GROUP");
			IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
			osdbstr(ERx("alter group "));
		}
		break;
	case 751:
#line 7634 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osevallist(yypvt[-0].st_node, osdblist);
		}
		break;
	case 752:
#line 7640 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (!osw_compare(ERx("users"), yypvt[-3].st_name) )
				yyerror(_SyntaxError);
			osdbstr(ERx(" add users ("));
			osevallist(yypvt[-1].st_node, osdblist);   /* name list w/o "()") */
			osdbstr(ERx(")"));
		}
		break;
	case 753:
#line 7648 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (!osw_compare(ERx("users"), yypvt[-3].st_name) )
				yyerror(_SyntaxError);
			osdbstr(ERx(" drop users ("));
			osevallist(yypvt[-1].st_node, osdblist);   /* name list w/o "()") */
			osdbstr(ERx(")"));
		}
		break;
	case 754:
#line 7656 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbstr(ERx(" drop all"));
		}
		break;
	case 756:
#line 7673 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			STARTSTMTOW("ALTER TABLE");
			IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
			osdbstr(ERx("alter table "));
		}
		break;
	case 757:
#line 7682 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbwith(yypvt[-0].st_tlist);
		}
		break;
	case 759:
#line 7688 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbstr(ERx(" add "));
		}
		break;
	case 760:
#line 7693 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbstr(ERx(" add constraint "));
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 761:
#line 7699 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbstr(ERx(" drop constraint "));
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 762:
#line 7715 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
					osdbwith(yypvt[-0].st_tlist);	
				}
		break;
	case 763:
#line 7721 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			STARTSTMTOW("ALTER ROLE");
			IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
			osdbstr(ERx("alter role "));
		}
		break;
	case 764:
#line 7743 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		IGgenStmt(IL_CREATEUSER, (IGSID *)NULL, 0);
		osdbeval(yypvt[-1].st_node);
		osdbwith(yypvt[-0].st_tlist);
	}
		break;
	case 765:
#line 7751 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		STARTSTMTOW("CREATE USER");
	}
		break;
	case 767:
#line 7777 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		STARTSTMTOW("ALTER USER");
		IGgenStmt(IL_ALTERUSER, (IGSID *)NULL, 0);
	}
		break;
	case 768:
#line 7783 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{osdbeval(yypvt[-0].st_node);}
		break;
	case 769:
#line 7784 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		osdbwith(yypvt[-0].st_tlist);
	}
		break;
	case 770:
#line 7788 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		/* No user name */
		osdbwith(yypvt[-0].st_tlist);
	}
		break;
	case 774:
#line 7819 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		osdbeval(yypvt[-1].st_node);
		osdbwith(yypvt[-0].st_tlist);
	}
		break;
	case 775:
#line 7826 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		osOpenwarn(ERx("CREATE PROFILE"));
		IGstartStmt(osscnlno(), IL_LB_NONE);
		IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
		osdbstr(ERx("create profile "));
	}
		break;
	case 776:
#line 7848 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{osdbwith(yypvt[-0].st_tlist);}
		break;
	case 777:
#line 7852 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		osOpenwarn(ERx("ALTER PROFILE"));
		IGstartStmt(osscnlno(), IL_LB_NONE);
		IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
		osdbstr(ERx("alter profile "));
		osdbeval(yypvt[-0].st_node);
	}
		break;
	case 778:
#line 7860 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		if (!osw_compare(ERx("profile"), yypvt[-0].st_name) )
			yyerror(_SyntaxError);
		osOpenwarn(ERx("ALTER PROFILE"));
		IGstartStmt(osscnlno(), IL_LB_NONE);
		IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
		osdbstr(ERx("alter default profile "));
	}
		break;
	case 779:
#line 7879 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
		osdbstr(ERx("create synonym "));
		osdbeval(yypvt[-2].st_node);
		osdbstr(ERx(" for "));
		osdbeval(yypvt[-0].st_node);
	}
		break;
	case 780:
#line 7889 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		STARTSTMTOW("CREATE SYNONYM");
	}
		break;
	case 781:
#line 7919 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		IGgenStmt(IL_CREATERULE, (IGSID *)NULL, 0);
		osdbeval( yypvt[-9].st_node );
		osdbstr( ERx(" after ") );
		osevallist( yypvt[-7].st_node, osdblist );
		osdbstr( ERx(" on ") );
	 	osdbeval ( yypvt[-5].st_node );
		if ( yypvt[-4].st_node != NULL )
		{
			osdbstr( ERx(" referencing ") );
			osdbeval( yypvt[-4].st_node );
		}
		if ( yypvt[-3].st_node != NULL )
		{
			osqwhere(yypvt[-3].st_node);
		}
		osdbstr(ERx(" execute procedure "));
		osdbeval( yypvt[-1].st_node );
		if ( yypvt[-0].st_node != NULL )
		{
			osdbstr( "(" );
			osevallist(yypvt[-0].st_node, osdblist);
			osdbstr( ERx(")") );
		}
	}
		break;
	case 782:
#line 7946 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{STARTSTMTOW("CREATE RULE")}
		break;
	case 783:
#line 7950 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		if ( !osw_compare(ERx("after"), yypvt[-0].st_name) )
			yyerror(_SyntaxError);
	}
		break;
	case 788:
#line 7963 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
    	    u_ptr[0].u_nodep = yypvt[-0].st_node;
	    u_ptr[1].u_nodep = NULL;
    	    u_ptr[2].u_nodep = NULL;
    	    yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
	}
		break;
	case 789:
#line 7970 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    u_ptr[0].u_nodep = yypvt[-0].st_node;
	    u_ptr[1].u_nodep = yypvt[-2].st_node;
    	    u_ptr[2].u_nodep = NULL;
    	    yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
	}
		break;
	case 790:
#line 7979 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
	}
		break;
	case 791:
#line 7983 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
	}
		break;
	case 792:
#line 7987 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    u_ptr[0].u_nodep = yypvt[-1].st_node;
    	    u_ptr[1].u_nodep = osmkident(yypvt[-3].st_name, (OSNODE *)NULL);
    	    u_ptr[2].u_nodep =
    		osmknode(PARENS, (U_ARG*)NULL, &u_ptr[0], (U_ARG*)NULL);
	    yyval.st_node = osmknode(BLANK, (U_ARG*)NULL, &u_ptr[1], &u_ptr[2]);
	}
		break;
	case 793:
#line 7995 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
	}
		break;
	case 794:
#line 8001 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    u_ptr[0].u_nodep = yypvt[-1].st_node;
	    u_ptr[1].u_nodep = yypvt[-0].st_node;
	    yyval.st_node = osmknode( BLANK, (U_ARG*)NULL, &u_ptr[0], &u_ptr[1] );
	}
		break;
	case 795:
#line 8007 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    yyval.st_node = NULL;
	}
		break;
	case 796:
#line 8013 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		if ( osw_compare(ERx("old"), yypvt[-2].st_name) )
			yyval.st_node = osmkident(" old as ", osmkident(yypvt[-0].st_name,(OSNODE*)NULL));
		else if ( osw_compare(ERx("new"), yypvt[-2].st_name) )
			yyval.st_node = osmkident(" new as ", osmkident(yypvt[-0].st_name,(OSNODE*)NULL));
		else
		{
			yyerror(_SyntaxError);
			yyval.st_node = NULL;
		}
	}
		break;
	case 797:
#line 8025 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    yyval.st_node = NULL;
	}
		break;
	case 798:
#line 8031 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    yyval.st_node = yypvt[-1].st_node;
	}
		break;
	case 799:
#line 8035 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    yyval.st_node = NULL;
	}
		break;
	case 800:
#line 8041 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
    	    u_ptr[0].u_nodep = yypvt[-0].st_node;
	    u_ptr[1].u_nodep = NULL;
    	    u_ptr[2].u_nodep = NULL;
    	    yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
	}
		break;
	case 801:
#line 8048 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    u_ptr[0].u_nodep = yypvt[-0].st_node;
	    u_ptr[1].u_nodep = yypvt[-2].st_node;
    	    u_ptr[2].u_nodep = NULL;
    	    yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
	}
		break;
	case 802:
#line 8057 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    u_ptr[0].u_cp = ERx("=");
	    u_ptr[1].u_nodep = osmkident(yypvt[-2].st_name, (OSNODE *)NULL);
	    u_ptr[2].u_nodep = yypvt[-0].st_node;
	    yyval.st_node = osmknode(ASSOCOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
	}
		break;
	case 803:
#line 8066 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    u_ptr[0].u_nodep = osmkident(yypvt[-2].st_name, (OSNODE *)NULL);
	    u_ptr[1].u_nodep = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
	    yyval.st_node = osmknode(DML|DOT, (U_ARG*)NULL, &u_ptr[0], &u_ptr[1]);
	}
		break;
	case 805:
#line 8093 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    		STARTSTMTOW("GRANT")
			IGgenStmt(IL_GRANT, (IGSID *)NULL, 0);
		}
		break;
	case 806:
#line 8099 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osevallist(yypvt[-0].st_node, osdblist);	/* grant list */
		}
		break;
	case 807:
#line 8103 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbstr(ERx("all"));
		}
		break;
	case 808:
#line 8108 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (!osw_compare(ERx("privileges"), yypvt[-0].st_name))
				yyerror(_SyntaxError);
		}
		break;
	case 810:
#line 8115 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 811:
#line 8122 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 812:
#line 8130 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 813:
#line 8134 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-1].st_name, yypvt[-0].st_node);
		}
		break;
	case 814:
#line 8138 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 815:
#line 8142 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-1].st_name, yypvt[-0].st_node);
		}
		break;
	case 816:
#line 8146 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 817:
#line 8150 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-1].st_name, yypvt[-0].st_node);
		}
		break;
	case 818:
#line 8154 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 819:
#line 8160 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbstr(ERx(" "));
			osdbstr(yypvt[-1].st_name);
			osevallist(yypvt[-0].st_node, osdblist);
		}
		break;
	case 820:
#line 8166 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx(
				"GRANT/REVOKE ... ON CURRENT INSTALLATION"));
			osdbstr(ERx(" on current installation"));
		}
		break;
	case 822:
#line 8174 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 823:
#line 8178 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("GRANT/REVOKE ... ON DATABASE"));
			yyval.st_name = ERx("on database ");
		}
		break;
	case 824:
#line 8183 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("GRANT/REVOKE ... ON DBEVENT"));
			yyval.st_name = ERx("on dbevent ");
		}
		break;
	case 825:
#line 8188 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("GRANT/REVOKE ... ON LOCATION"));
			yyval.st_name = ERx("on location ");
		}
		break;
	case 826:
#line 8194 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbstr(ERx(" to "));
			osevallist(yypvt[-0].st_node, osdblist);
		}
		break;
	case 827:
#line 8199 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("GRANT ... TO GROUP"));
			osdbstr(ERx(" to group "));
			osevallist(yypvt[-0].st_node, osdblist);
		}
		break;
	case 828:
#line 8205 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("GRANT ... TO ROLE"));
			osdbstr(ERx(" to role "));
			osevallist(yypvt[-0].st_node, osdblist);
		}
		break;
	case 829:
#line 8211 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbstr(ERx(" to user "));
			osevallist(yypvt[-0].st_node, osdblist);
		}
		break;
	case 830:
#line 8217 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (!osw_compare(ERx("option"), yypvt[-0].st_name))
				yyerror(_SyntaxError);
			osdbstr(ERx(" with grant option "));
		}
		break;
	case 832:
#line 8225 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (!osw_compare(ERx("option"), yypvt[-1].st_name))
				yyerror(_SyntaxError);
			osdbstr(ERx(" grant option for "));
		}
		break;
	case 835:
#line 8251 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
			osdbstr(ERx(" revoke "));
		}
		break;
	case 836:
#line 8258 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osevallist(yypvt[-0].st_node, osdblist);	/* revoke list */
		}
		break;
	case 837:
#line 8262 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbstr(ERx("all"));
		}
		break;
	case 838:
#line 8267 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 839:
#line 8274 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 840:
#line 8282 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 841:
#line 8286 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 842:
#line 8290 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 843:
#line 8294 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-1].st_name, yypvt[-0].st_node);
		}
		break;
	case 844:
#line 8298 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 845:
#line 8302 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 846:
#line 8306 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 847:
#line 8312 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbstr(ERx(" from "));
			osevallist(yypvt[-0].st_node, osdblist);
		}
		break;
	case 848:
#line 8317 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("REVOKE ... FROM GROUP"));
			osdbstr(ERx(" from group "));
			osevallist(yypvt[-0].st_node, osdblist);
		}
		break;
	case 849:
#line 8323 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("REVOKE ... FROM ROLE"));
			osdbstr(ERx(" from role "));
			osevallist(yypvt[-0].st_node, osdblist);
		}
		break;
	case 850:
#line 8329 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbstr(ERx(" from user "));
			osevallist(yypvt[-0].st_node, osdblist);
		}
		break;
	case 851:
#line 8335 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (osw_compare(ERx("cascade"), yypvt[-0].st_name))
				osdbstr(ERx(" cascade"));
			else if (osw_compare(ERx("restrict"), yypvt[-0].st_name))
				osdbstr(ERx(" restrict"));
			else
				yyerror(_SyntaxError);
		}
		break;
	case 853:
#line 8358 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_CRTPERMIT, (IGSID *)NULL, 0);
			osevallist(yypvt[-8].st_node, osdblist);	/* permit list */
			osdbstr(ERx(" on "));
			osdbfrom(yypvt[-6].st_node);	/* table correlation name */
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
	case 854:
#line 8395 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			STARTSTMTOW("CREATE PERMIT");
		}
		break;
	case 855:
#line 8400 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 856:
#line 8404 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 857:
#line 8412 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 858:
#line 8419 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = osmkident(yypvt[-2].st_node, (OSNODE *)NULL);
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 859:
#line 8427 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 860:
#line 8431 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 861:
#line 8435 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 862:
#line 8439 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 866:
#line 8448 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 867:
#line 8452 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 868:
#line 8457 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 869:
#line 8461 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 870:
#line 8466 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 871:
#line 8470 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 872:
#line 8474 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 873:
#line 8479 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-1].st_name;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(ASSOCOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 874:
#line 8486 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 875:
#line 8491 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = ERx(":");
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(ASSOCOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 876:
#line 8499 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-1].st_name;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(ASSOCOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 877:
#line 8506 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 878:
#line 8518 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_CRTINTEG, (IGSID *)NULL, 0);
			osdbfrom(yypvt[-2].st_node);	/* table correlation name */
			osdbstr(ERx(" is "));
			osqtraverse(yypvt[-0].st_node);
		}
		break;
	case 879:
#line 8526 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			STARTSTMTOW("CREATE INTEGRITY");
		}
		break;
	case 880:
#line 8544 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
			osdbstr(yypvt[-1].st_name);
			if (yypvt[-0].st_node->n_next != NULL)
				osOpenwarn(ERx("DROP ... name_list"));
			osevallist(yypvt[-0].st_node, osdblist);
		}
		break;
	case 881:
#line 8553 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
			osdbstr(yypvt[-1].st_name);
			if (yypvt[-0].st_node->n_next != NULL)
				osOpenwarn(ERx("DROP ... name_list"));
			osevallist(yypvt[-0].st_node, osdblist);
		}
		break;
	case 882:
#line 8562 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
			osdbstr(yypvt[-1].st_name);
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 884:
#line 8569 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
			osdbstr(yypvt[-1].st_name);
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 885:
#line 8576 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
			osdbstr(yypvt[-3].st_name);
			osdbstr(yypvt[-2].st_name);
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
	case 886:
#line 8590 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{ osdbstr(ERx(" ")); }
		break;
	case 892:
#line 8599 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{osdbeval(yypvt[-0].st_node); osdbstr(ERx(" "));}
		break;
	case 893:
#line 8602 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{ osdbeval(yypvt[-0].st_node); osdbstr(ERx(" "));}
		break;
	case 894:
#line 8605 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("DROP LINK"));
			yyval.st_name = ERx("drop link ");
		}
		break;
	case 895:
#line 8611 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = ERx("drop table ");
		}
		break;
	case 896:
#line 8615 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = ERx("drop index ");
		}
		break;
	case 897:
#line 8619 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = ERx("drop view ");
		}
		break;
	case 898:
#line 8623 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("DROP ROLE"));
			yyval.st_name = ERx("drop role ");
		}
		break;
	case 899:
#line 8628 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("DROP GROUP"));
			yyval.st_name = ERx("drop group ");
		}
		break;
	case 900:
#line 8633 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("DROP SYNONYM"));
			yyval.st_name = ERx("drop synonym ");
		}
		break;
	case 901:
#line 8638 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("DROP object"));
			yyval.st_name = ERx("drop ");
		}
		break;
	case 902:
#line 8644 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("DROP RULE"));
			yyval.st_name = ERx("drop rule ");
		}
		break;
	case 903:
#line 8649 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("DROP PROCEDURE"));
			yyval.st_name = ERx("drop procedure ");
		}
		break;
	case 904:
#line 8655 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("DROP LOCATION"));
			yyval.st_name = ERx("drop location ");
		}
		break;
	case 905:
#line 8660 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("DROP USER"));
			yyval.st_name = ERx("drop user ");
		}
		break;
	case 906:
#line 8665 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("DROP PROFILE"));
			yyval.st_name = ERx("drop profile ");
		}
		break;
	case 907:
#line 8671 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("DROP INTEGRITY"));
			yyval.st_name = ERx("drop integrity ");
		}
		break;
	case 908:
#line 8676 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("DROP PERMIT"));
			yyval.st_name = ERx("drop permit ");
		}
		break;
	case 909:
#line 8682 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("DROP SECURITY_ALARM"));
			yyval.st_name = ERx("drop security_alarm ");
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
			osdbstr(ERx("drop security_alarm "));
		}
		break;
	case 910:
#line 8692 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 911:
#line 8696 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = ERx("on dbevent ");
		}
		break;
	case 912:
#line 8701 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 913:
#line 8705 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 914:
#line 8711 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = ERx("on table ");
		}
		break;
	case 915:
#line 8715 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = ERx("on ");
		}
		break;
	case 916:
#line 8720 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = ERx("on procedure ");
		}
		break;
	case 918:
#line 8746 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{osdbstr(ERx(" "));}
		break;
	case 920:
#line 8750 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("CREATE SECURITY_ALARM"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
			osdbstr(ERx("create security_alarm "));
		}
		break;
	case 924:
#line 8763 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbstr(ERx(" if "));
		}
		break;
	case 929:
#line 8774 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (osw_compare(ERx("success"), yypvt[-0].st_name))
				osdbstr(ERx("success "));
			else if (osw_compare(ERx("failure"), yypvt[-0].st_name))
				osdbstr(ERx("failure "));
			else
				yyerror(_SyntaxError);
		}
		break;
	case 945:
#line 8805 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
				osdbstr(" raise dbevent ");
				osdbeval(yypvt[-1].st_node);
			       	if (yypvt[-0].st_node != NULL)
			       	{ /* optional event text */
				   osdbstr(ERx(" "));
				   osdbveval(yypvt[-0].st_node);
				   osdbstr(ERx(" "));
			       	}
			    }
		break;
	case 950:
#line 8837 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("ENABLE SECURITY_AUDIT"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
			osdbstr(ERx("enable security_audit "));
		}
		break;
	case 951:
#line 8845 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("DISABLE SECURITY_AUDIT"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
			osdbstr(ERx("disable security_audit "));
		}
		break;
	case 952:
#line 8853 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("ALTER SECURITY_AUDIT"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
			osdbstr(ERx("alter security_audit "));
		}
		break;
	case 954:
#line 8861 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{ osdbstr(ERx("resume "));}
		break;
	case 963:
#line 8886 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("COMMENT ON"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
			osdbstr(ERx("comment on "));
		}
		break;
	case 968:
#line 8901 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbstr(ERx(" is "));
			osdbveval(yypvt[-0].st_node);
		}
		break;
	case 969:
#line 8906 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbstr(ERx(" with short_remark = "));
			osdbveval(yypvt[-0].st_node);
		}
		break;
	case 970:
#line 8915 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbstr(yypvt[-0].st_name);
	}
		break;
	case 971:
#line 8920 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    if (!osw_compare(ERx("when"), yypvt[-0].st_name))
		yyerror(_SyntaxError);
	    osdbstr(ERx(" when "));
	}
		break;
	case 972:
#line 8927 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    if (!osw_compare(ERx("column"), yypvt[-0].st_name))
		yyerror(_SyntaxError);
	    osdbstr(ERx("column "));
	}
		break;
	case 973:
#line 8934 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbstr(ERx("all "));
	}
		break;
	case 974:
#line 8939 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbstr(ERx("select "));
	}
		break;
	case 975:
#line 8944 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbstr(ERx("insert "));
	}
		break;
	case 976:
#line 8949 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbstr(ERx("update "));
	}
		break;
	case 977:
#line 8954 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbstr(ERx("delete "));
	}
		break;
	case 978:
#line 8959 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbstr(ERx("connect "));
	}
		break;
	case 979:
#line 8963 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbstr(ERx("disconnect "));
	}
		break;
	case 980:
#line 8968 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbstr(ERx("table "));
	}
		break;
	case 981:
#line 8973 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbstr(ERx("procedure "));
	}
		break;
	case 982:
#line 8978 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbstr(yypvt[-0].st_name);
	}
		break;
	case 983:
#line 8983 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		osdbstr(" on database ");
	}
		break;
	case 984:
#line 8988 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		osdbstr(" on current installation ");
	}
		break;
	case 985:
#line 8993 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbstr(ERx("user "));
	}
		break;
	case 986:
#line 8998 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbstr(ERx("by group "));
	}
		break;
	case 987:
#line 9003 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbstr(ERx("by role "));
	}
		break;
	case 988:
#line 9008 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbstr(ERx("by user "));
	}
		break;
	case 989:
#line 9012 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbstr(ERx("by "));
	}
		break;
	case 990:
#line 9017 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbeval(yypvt[-0].st_node);
	}
		break;
	case 991:
#line 9022 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbeval(yypvt[-0].st_node);
	}
		break;
	case 994:
#line 9030 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbwith(yypvt[-0].st_tlist);
		}
		break;
	case 995:
#line 9046 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-1].st_name;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(DML|OP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 996:
#line 9053 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-1].st_name;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(DML|OP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 997:
#line 9060 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = _Exp;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(DML|OP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 998:
#line 9067 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-1].st_node;
			yyval.st_node = osmknode(PARENS, (U_ARG*)NULL, &u_ptr[0], (U_ARG*)NULL);
		}
		break;
	case 999:
#line 9072 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = _Minus;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(DML|UNARYOP, &u_ptr[0], &u_ptr[1],
					(U_ARG*)NULL
			 );
		}
		break;
	default:
		    if (yy__case10(yym)) YYERROR;
		    break;
	}
	goto yystack; /* stack new stuff */
}


yy__case10(yym)
short yym;
{
#ifdef YYERROR
#undef YYERROR
#endif /* YYERROR */
#define YYERROR return 1

	switch (yym) {
	case 1000:
#line 9080 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1001:
#line 9097 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			/* Note:  COUNT already recognized by
			** <agg_name> so just look for 'c'.
			*/
			if ( CMcmpnocase(yypvt[-2].st_name, ERx("c")) != 0 )
			{
				yyerror( _SyntaxError );
			}
		}
		break;
	case 1002:
#line 9106 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			TableRef = TRUE;
			yyval.st_node = osmkident(yypvt[-4].st_name, osmkident(ERx("(*)"), (OSNODE*)NULL));
		}
		break;
	case 1003:
#line 9111 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			TableRef = TRUE;
			u_ptr[0].u_nodep = (yypvt[-2].st_name == NULL) ? yypvt[-1].st_node : osmkident(yypvt[-2].st_name, yypvt[-1].st_node);
			yyval.st_node = osmkident(yypvt[-4].st_name, osmknode(PARENS, (U_ARG*)NULL,
							&u_ptr[0], (U_ARG*)NULL
					)
			 );
		}
		break;
	case 1004:
#line 9120 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			TableRef = TRUE;
			u_ptr[0].u_nodep = (yypvt[-2].st_name == NULL) ? yypvt[-1].st_node : osmkident(yypvt[-2].st_name, yypvt[-1].st_node);
			yyval.st_node = osmkident(yypvt[-4].st_name, osmknode(PARENS, (U_ARG*)NULL,
							&u_ptr[0], (U_ARG*)NULL
					)
			 );
		}
		break;
	case 1005:
#line 9129 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			TableRef = TRUE;
			if ( yypvt[-1].st_node->n_token != ATTR && ( yypvt[-1].st_node->n_token != tkID
			   || yypvt[-1].st_node->n_left != NULL ) )
			{
				osOpenwarn(
				      ERx("<aggregate>(DISTINCT <expression>)")
				);
			}
			u_ptr[0].u_nodep = osmkident(yypvt[-2].st_name, yypvt[-1].st_node);
			yyval.st_node = osmkident(yypvt[-4].st_name, osmknode(PARENS, (U_ARG*)NULL,
							&u_ptr[0], (U_ARG*)NULL
					   )
			 );
		}
		break;
	case 1006:
#line 9146 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1007:
#line 9150 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1008:
#line 9154 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1009:
#line 9158 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1010:
#line 9162 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1011:
#line 9167 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1012:
#line 9172 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1013:
#line 9176 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = NULL;
		}
		break;
	case 1014:
#line 9181 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-1].st_name;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(DML|OP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1015:
#line 9188 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-1].st_name;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(DML|OP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1016:
#line 9195 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = _Exp;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(DML|OP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1017:
#line 9202 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = _Minus;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(DML|UNARYOP, &u_ptr[0], &u_ptr[1],
					(U_ARG*)NULL
			 );
		}
		break;
	case 1018:
#line 9210 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-1].st_node;
			yyval.st_node = osmknode(PARENS, (U_ARG*)NULL, &u_ptr[0], (U_ARG*)NULL);
		}
		break;
	case 1019:
#line 9215 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1020:
#line 9221 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1021:
#line 9225 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1022:
#line 9229 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1023:
#line 9233 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (yypvt[-0].st_node->n_token == DOT)
				TableRef = TRUE;
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1024:
#line 9239 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-1].st_node;
			yyval.st_node = osmkident(yypvt[-3].st_name, osmknode(PARENS, (U_ARG*)NULL,
							  &u_ptr[0], (U_ARG*)NULL
					)
			 );
		}
		break;
	case 1025:
#line 9248 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
                        yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
                }
		break;
	case 1026:
#line 9252 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
                        yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
                }
		break;
	case 1027:
#line 9265 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1028:
#line 9271 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			/* DBMSINFO is the only function in Open SQL. */
			if ( STbcompare(yypvt[-0].st_name, 0, ERx("dbmsinfo"), 0, TRUE) != 0 )
	    			osOpenwarn(yypvt[-0].st_name);
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1029:
#line 9279 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-2].st_node;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(COMMA, (U_ARG*)NULL, &u_ptr[0], &u_ptr[1]);
		}
		break;
	case 1030:
#line 9285 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1031:
#line 9289 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 1032:
#line 9296 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
    	    u_ptr[0].u_nodep = osmkident(yypvt[-4].st_name, (OSNODE *)NULL);
    	    u_ptr[0].u_nodep->n_type = DB_CHA_TYPE;  /* see ingres_name def. */
    	    u_ptr[1].u_nodep = osmkident(yypvt[-2].st_name, (OSNODE *)NULL);
    	    u_ptr[1].u_nodep->n_type = DB_CHA_TYPE;
    	    u_ptr[2].u_nodep = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
    	    u_ptr[2].u_nodep->n_type = DB_CHA_TYPE;
	    u_ptr[1].u_nodep = osmknode(DML|DOT, (U_ARG*)NULL, 
		                        &u_ptr[1], &u_ptr[2]);
    	    yyval.st_node = osmknode(DML|DOT, (U_ARG*)NULL, &u_ptr[0], &u_ptr[1] );
    	}
		break;
	case 1033:
#line 9308 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
    	    u_ptr[0].u_nodep = osmkident(yypvt[-2].st_name, (OSNODE *)NULL);
    	    u_ptr[0].u_nodep->n_type = DB_CHA_TYPE;  /* see ingres_name def. */
    	    u_ptr[1].u_nodep = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
    	    u_ptr[1].u_nodep->n_type = DB_CHA_TYPE;
    	    yyval.st_node = osmknode(DML|DOT, (U_ARG*)NULL, &u_ptr[0], &u_ptr[1]);
    	}
		break;
	case 1034:
#line 9316 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
    	    yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
    	}
		break;
	case 1035:
#line 9330 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-1].st_node;
			yyval.st_node = osmknode(PARENS, (U_ARG*)NULL, &u_ptr[0],
					(U_ARG*)NULL
			);
		}
		break;
	case 1036:
#line 9337 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-1].st_name;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(DML|LOGOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1037:
#line 9344 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-1].st_name;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(DML|LOGOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1038:
#line 9352 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-1].st_name;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
           	yyval.st_node = osmknode(DML|RELOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1039:
#line 9359 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-0].st_name;
			u_ptr[1].u_nodep = yypvt[-1].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(DML|RELOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1040:
#line 9366 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-2].st_name;
			u_ptr[1].u_nodep = yypvt[-3].st_node;
			u_ptr[2].u_nodep = yypvt[-1].st_node;
			yyval.st_node = osmknode(DML|RELOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
			if ( yypvt[-0].st_node != NULL )
			{
				u_ptr[0].u_cp = _Escape;
				u_ptr[1].u_nodep = yyval.st_node;
				u_ptr[2].u_nodep = yypvt[-0].st_node;
				yyval.st_node = osmknode(DML|RELOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
			}
		}
		break;
	case 1041:
#line 9380 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-1].st_name;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(DML|RELOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1042:
#line 9387 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-1].st_name;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(DML|RELOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1043:
#line 9394 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-1].st_name;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(DML|RELOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1044:
#line 9401 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-1].st_node;
			yyval.st_node = osmkident(yypvt[-3].st_name, osmknode(PARENS, (U_ARG*)NULL,
							  &u_ptr[0], (U_ARG*)NULL)
				 );
		}
		break;
	case 1045:
#line 9408 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1046:
#line 9413 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( yypvt[-0].st_node->n_type != DB_NODT && !oschkstr(yypvt[-0].st_node->n_type) )
				oscerr(OSNOTSTRX, 0);
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1047:
#line 9419 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1048:
#line 9424 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1049:
#line 9428 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = ERx("not between");
		}
		break;
	case 1050:
#line 9433 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-1].st_name;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(DML|LOGOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1051:
#line 9441 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1052:
#line 9445 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = ERx("not in");
		}
		break;
	case 1053:
#line 9450 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-3].st_node;
			u_ptr[1].u_nodep = yypvt[-1].st_node;
			u_ptr[0].u_nodep =
			   osmknode(COMMA, (U_ARG*)NULL, &u_ptr[0], &u_ptr[1]);
			yyval.st_node = osmknode(PARENS, (U_ARG*)NULL, &u_ptr[0], (U_ARG*)NULL);
		}
		break;
	case 1054:
#line 9458 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1055:
#line 9462 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-1].st_node;
			yyval.st_node = osmknode(PARENS, (U_ARG*)NULL, &u_ptr[0], (U_ARG*)NULL);
		}
		break;
	case 1056:
#line 9468 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1057:
#line 9472 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-2].st_node;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(COMMA, (U_ARG*)NULL, &u_ptr[0], &u_ptr[1]);
		}
		break;
	case 1058:
#line 9483 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-1].st_node;
			yyval.st_node = osmknode( PARENS, (U_ARG *)NULL, &u_ptr[0],
					(U_ARG *)NULL
			);
		}
		break;
	case 1059:
#line 9490 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-1].st_node;
			yyval.st_node = osmkident(yypvt[-3].st_name, osmknode( PARENS, (U_ARG *)NULL,
							&u_ptr[0], (U_ARG *)NULL
						)
			);
		}
		break;
	case 1060:
#line 9498 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-1].st_node;
			yyval.st_node = osmkident(yypvt[-3].st_name, osmknode( PARENS, (U_ARG *)NULL,
							&u_ptr[0], (U_ARG *)NULL
						)
                        );
                }
		break;
	case 1061:
#line 9507 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1062:
#line 9511 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1063:
#line 9517 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = NULL;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_qrynodep =
			osmkquery( tkQUERY, yypvt[-5].st_name,
				(PTR)osmaketle(yypvt[-4].st_node, (OSNODE*)NULL,(OSNODE*)NULL),
				(PTR)yypvt[-2].st_node, yypvt[-3].st_node, yypvt[-1].st_node, yypvt[-0].st_node
			);
			yyval.st_node = osmknode(tkQUERY, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1064:
#line 9540 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			ILREF       fres = 0;

			if ( AFE_NULLABLE_MACRO(yypvt[-0].st_node->n_type) )
			{
				oswarn(OSNULVCHK, 0);
			}
			IGstartStmt(osscnlno(), IL_LB_NONE);
			fres = osvarref(yypvt[-0].st_node);
			IGgenStmt(IL_EXIMMEDIATE, (IGSID *)NULL, 0);
			IGgenStmt(IL_DBVAL, (IGSID *)NULL, 1, fres);
		}
		break;
	case 1065:
#line 9590 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
			osdbstr(yypvt[-1].st_name);
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 1066:
#line 9596 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_DBSTMT, (IGSID *)NULL, 0);
			osdbstr(ERx("raise dbevent "));
			osdbeval(yypvt[-2].st_node);
			if (yypvt[-1].st_node != NULL)
			{ /* optional event text */
				osdbstr(ERx(" "));
				osdbveval(yypvt[-1].st_node);
				osdbstr(ERx(" "));
			}
			osdbwith(yypvt[-0].st_tlist);
		}
		break;
	case 1067:
#line 9609 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (yypvt[-0].st_node != NULL)
			{
				ILREF	ilref;

				ilref = osvalref(yypvt[-0].st_node);
				IGgenStmt(IL_GETEVENT, (IGSID *)NULL, 1, ilref);
			}
		}
		break;
	case 1068:
#line 9621 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( osw_compare(ERx("wait"), yypvt[-0].st_name) )
			{
				yyval.st_node = osmkconst(tkICONST, iiIG_string(ERx("-1")));
			}
			else if ( osw_compare(ERx("nowait"), yypvt[-0].st_name) )
			{
				yyval.st_node = osmkconst(tkICONST, iiIG_string(ERx("0")));
			}
			else
			{
				yyerror(_SyntaxError);
				yyval.st_node = NULL;
			}
		}
		break;
	case 1069:
#line 9637 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( osw_compare(ERx("wait"), yypvt[-2].st_name) )
			{
				yyval.st_node = yypvt[-0].st_node;
			}
			else
			{
				yyerror(_SyntaxError);
				yyval.st_node = NULL;
			}
		}
		break;
	case 1070:
#line 9649 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkconst(tkICONST, iiIG_string(ERx("0")));
		}
		break;
	case 1071:
#line 9655 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			yyval.st_name = ERx("create dbevent ");
		}
		break;
	case 1072:
#line 9660 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			yyval.st_name = ERx("drop dbevent ");
		}
		break;
	case 1073:
#line 9665 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			yyval.st_name = ERx("register dbevent ");
		}
		break;
	case 1074:
#line 9670 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			yyval.st_name = ERx("remove dbevent ");
		}
		break;
	case 1075:
#line 9677 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 1076:
#line 9683 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 1077:
#line 9702 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_ABORT, (IGSID *)NULL, 0);
		}
		break;
	case 1078:
#line 9706 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_ABORT, (IGSID *)NULL, 0);
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 1079:
#line 9712 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1082:
#line 9738 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt( QUEL ? IL_SAVEPOINT : IL_SAVPT_SQL,
					(IGSID *)NULL, 0
			);
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 1083:
#line 9746 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( SQL )
				osOpenwarn(ERx("SAVEPOINT"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 1084:
#line 9760 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1085:
#line 9773 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1086:
#line 9802 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1087:
#line 9806 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = ERx(":");
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(DML|OP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1088:
#line 9813 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = ERx(":");
			u_ptr[1].u_nodep = yypvt[-1].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(DML|OP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1089:
#line 9823 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
    	    yyval.st_node = osmkident( yypvt[-0].st_name, (OSNODE *)NULL );
    	}
		break;
	case 1090:
#line 9827 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
            u_ptr[0].u_nodep = osmkident( yypvt[-2].st_name, (OSNODE *)NULL );
            u_ptr[1].u_nodep = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
    	    u_ptr[1].u_nodep->n_type = DB_CHA_TYPE;  /* see ingres_name def. */
            yyval.st_node = osmknode(DOT|DML, (U_ARG*)NULL, &u_ptr[0], &u_ptr[1]);
        }
		break;
	case 1091:
#line 9844 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1092:
#line 9848 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 1093:
#line 9860 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1094:
#line 9864 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			char	buf[OSBUFSIZE];

			_VOID_ STprintf(buf, ERx("%s(%s)"), yypvt[-3].st_name, yypvt[-1].st_name);
			yyval.st_node = osmkident( iiIG_string(buf), (OSNODE *)NULL );
		}
		break;
	case 1095:
#line 9871 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			char	buf[OSBUFSIZE];

			_VOID_ STprintf(buf, ERx("%s(%s,%s)"), yypvt[-5].st_name, yypvt[-3].st_name, yypvt[-1].st_name);
			yyval.st_node = osmkident( iiIG_string(buf), (OSNODE *)NULL );
		}
		break;
	case 1096:
#line 9878 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			char	buf[OSBUFSIZE];
			(VOID) STprintf(buf, ERx("%s %s(%s)"), yypvt[-4].st_name, yypvt[-3].st_name, yypvt[-1].st_name );
			yyval.st_node = osmkident( iiIG_string(buf), (OSNODE *)NULL );
		}
		break;
	case 1097:
#line 9884 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			char	buf[OSBUFSIZE];
			(VOID) STprintf(buf, ERx("%s %s"), yypvt[-1].st_name, yypvt[-0].st_name );
			yyval.st_node = osmkident( iiIG_string(buf), (OSNODE *)NULL );
		}
		break;
	case 1098:
#line 9895 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = ERx("with null");
		}
		break;
	case 1099:
#line 9899 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = ERx("not null");
		}
		break;
	case 1100:
#line 9903 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
                        if ( SQL )
                                osOpenwarn(ERx("... WITH DEFAULT"));
                        yyval.st_name = ERx("not null with default");
                }
		break;
	case 1101:
#line 9909 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
                        if ( SQL )
                                osOpenwarn(ERx("... WITH DEFAULT"));
                        yyval.st_name = ERx("not null not default");
                }
		break;
	case 1102:
#line 9918 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( SQL )
				osOpenwarn(ERx("... WITH DEFAULT"));
			yyval.st_name = ERx("with default");
		}
		break;
	case 1103:
#line 9924 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( SQL )
				osOpenwarn(ERx("... NOT DEFAULT"));
			yyval.st_name = ERx("not default");
		}
		break;
	case 1104:
#line 9939 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_nat = TRUE;
		}
		break;
	case 1105:
#line 9943 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_nat = FALSE;
		}
		break;
	case 1106:
#line 10069 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = yypvt[-0].st_tlist;
		}
		break;
	case 1107:
#line 10073 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = NULL;
		}
		break;
	case 1108:
#line 10079 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = yypvt[-0].st_tlist;
		}
		break;
	case 1109:
#line 10083 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = osaddtlist(yypvt[-2].st_tlist, yypvt[-0].st_tlist);
		}
		break;
	case 1110:
#line 10089 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1111:
#line 10101 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[1].u_nodep = yypvt[-1].st_node;
			yyval.st_tlist = osmaketle( yypvt[-4].st_node,
				osmknode(PARENS, (U_ARG*)NULL, &u_ptr[1],
					(U_ARG*)NULL),
				(OSNODE*)NULL
			);
		}
		break;
	case 1112:
#line 10110 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = osmaketle(yypvt[-0].st_node, (OSNODE *) NULL, (OSNODE *) NULL);
		}
		break;
	case 1113:
#line 10115 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-2].st_node;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(COMMA, (U_ARG *)NULL, &u_ptr[0], &u_ptr[1]);
		}
		break;
	case 1114:
#line 10121 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1115:
#line 10126 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1116:
#line 10139 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 1117:
#line 10143 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 1118:
#line 10147 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 1119:
#line 10153 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1120:
#line 10158 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;
		}
		break;
	case 1121:
#line 10163 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;
		}
		break;
	case 1122:
#line 10168 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;
		}
		break;
	case 1123:
#line 10173 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;
		}
		break;
	case 1124:
#line 10178 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;
		}
		break;
	case 1125:
#line 10183 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;
		}
		break;
	case 1126:
#line 10188 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;
		}
		break;
	case 1127:
#line 10194 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = NULL;
		}
		break;
	case 1128:
#line 10198 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			/* We'd better be in QUEL. */
			if (!osw_compare(ERx("asc"), yypvt[-0].st_name)
			 && !osw_compare(ERx("desc"), yypvt[-0].st_name))
				yyerror(_SyntaxError);
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1129:
#line 10206 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1130:
#line 10210 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1131:
#line 10233 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1132:
#line 10252 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( SQL )
				osOpenwarn(ERx("COPY"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 1133:
#line 10259 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1134:
#line 10263 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1135:
#line 10268 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = NULL;
		}
		break;
	case 1136:
#line 10272 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = yypvt[-0].st_tlist;
		}
		break;
	case 1137:
#line 10277 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = yypvt[-0].st_tlist;
		}
		break;
	case 1138:
#line 10281 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = osaddtlist(yypvt[-2].st_tlist, yypvt[-0].st_tlist);
		}
		break;
	case 1139:
#line 10286 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = osmaketle(yypvt[-3].st_node, yypvt[-1].st_node, yypvt[-0].st_node);
		}
		break;
	case 1140:
#line 10291 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1141:
#line 10337 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1142:
#line 10358 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1143:
#line 10381 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1144:
#line 10385 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = NULL;
		}
		break;
	case 1145:
#line 10390 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident( ERx("with null"), (OSNODE *)NULL );
		}
		break;
	case 1146:
#line 10394 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1147:
#line 10430 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 1148:
#line 10445 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1149:
#line 10465 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( SQL )
				osOpenwarn(ERx("MODIFY"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 1150:
#line 10472 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1151:
#line 10476 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 1152:
#line 10481 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_srtlist = yypvt[-0].st_srtlist;
		}
		break;
	case 1153:
#line 10485 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_srtlist = NULL;
		}
		break;
	case 1154:
#line 10490 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = NULL;
		}
		break;
	case 1155:
#line 10494 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = yypvt[-0].st_tlist;
		}
		break;
	case 1156:
#line 10498 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = yypvt[-0].st_tlist;
		}
		break;
	case 1157:
#line 10510 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_RELOCATE, (IGSID *)NULL, 0);
			osdbeval(yypvt[-2].st_node);
			osdbstr(ERx(" to "));
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 1158:
#line 10518 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( SQL )
				osOpenwarn(ERx("RELOCATE"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 1159:
#line 10532 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1160:
#line 10543 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_SAVE, (IGSID *)NULL, 0);
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 1161:
#line 10549 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( SQL )
				osOpenwarn(ERx("SAVE"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
		}
		break;
	case 1162:
#line 10573 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 1163:
#line 10577 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbeval(yypvt[-1].st_node);
			osdbstr(ERx(" "));
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 1165:
#line 10584 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbeval(yypvt[-1].st_node);
			osdbstr(ERx(" on"));
		}
		break;
	case 1166:
#line 10590 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbeval(yypvt[-1].st_node);
			osdbstr(ERx(" off"));
		}
		break;
	case 1167:
#line 10596 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbeval(yypvt[-2].st_node);
			osdbstr(ERx(" on "));
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 1168:
#line 10603 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbeval(yypvt[-2].st_node);
			osdbstr(ERx(" "));
			osdbeval(yypvt[-1].st_node);
			osdbstr(ERx(" "));
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 1170:
#line 10616 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("SET ROLE"));
			osdbeval(yypvt[-1].st_node);
			osdbstr(ERx(" session role"));
		}
		break;
	case 1171:
#line 10624 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("SET GROUP"));
			osdbstr(ERx(" group session group"));
		}
		break;
	case 1172:
#line 10631 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("SET GROUP"));
			osdbstr(ERx(" group "));
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 1173:
#line 10639 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("SET USER AUTHORIZATION"));
			osdbstr(ERx(" user authorization "));
			osdbeval(yypvt[-0].st_node);
		}
		break;
	case 1174:
#line 10647 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("SET USER AUTHORIZATION"));
			osdbstr(ERx(" user authorization "));
			osdbstr(yypvt[-0].st_name);
		}
		break;
	case 1175:
#line 10655 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbeval(yypvt[-3].st_node);
			osdbstr(ERx(" "));
			osdbeval(yypvt[-2].st_node);
			osdbstr(ERx(" where "));
			osevaltlist(yypvt[-0].st_tlist, osdbtle);	/* lock list */
		}
		break;
	case 1176:
#line 10664 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbeval(yypvt[-3].st_node);
			osdbstr(ERx(" session where "));
			osevaltlist(yypvt[-0].st_tlist, osdbtle);	/* lock list */
		}
		break;
	case 1177:
#line 10670 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbeval(yypvt[-4].st_node);
			osdbstr(ERx(" on "));
			osdbeval(yypvt[-2].st_node);
			osdbstr(ERx(" where "));
			osevaltlist(yypvt[-0].st_tlist, osdbtle);	/* lock list */
		}
		break;
	case 1178:
#line 10679 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(QUEL ? IL_SET : IL_SETSQL, (IGSID *)NULL, 0);
		}
		break;
	case 1179:
#line 10686 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(QUEL ? IL_SET : IL_SETSQL, (IGSID *)NULL, 0);
			osdbstr(ERx("session"));
		}
		break;
	case 1180:
#line 10693 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbwith(yypvt[-0].st_tlist);
		}
		break;
	case 1185:
#line 10709 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osdbwith(yypvt[-0].st_tlist);
		}
		break;
	case 1186:
#line 10717 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = yypvt[-0].st_tlist;
		}
		break;
	case 1187:
#line 10722 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = osaddtlist(yypvt[-2].st_tlist, yypvt[-0].st_tlist);
		}
		break;
	case 1188:
#line 10729 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = osmaketle(yypvt[-2].st_node, yypvt[-0].st_node, (OSNODE *)NULL);
		}
		break;
	case 1189:
#line 10734 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = osmaketle(yypvt[-0].st_node, (OSNODE *) NULL,
						(OSNODE *)NULL);
		}
		break;
	case 1190:
#line 10739 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[1].u_nodep = yypvt[-1].st_node;
			yyval.st_tlist = osmaketle( yypvt[-4].st_node,
				osmknode(PARENS, (U_ARG*)NULL, &u_ptr[1],
					(U_ARG*)NULL),
				(OSNODE*)NULL
			);
		}
		break;
	case 1191:
#line 10749 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = osmaketle(yypvt[-2].st_node,  yypvt[-0].st_node,
					(OSNODE *)NULL);
		}
		break;
	case 1192:
#line 10756 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 1193:
#line 10760 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (!oschkstr(yypvt[-1].st_node->n_type))
				oscerr(OSNOTSTR, 1, ERget(_FrsName));
			else if ( AFE_NULLABLE_MACRO(yypvt[-1].st_node->n_type) )
				oswarn(OSNULVCHK, 0);
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 1194:
#line 10769 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{yyval.st_node=yypvt[-0].st_node;}
		break;
	case 1195:
#line 10772 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{ yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;	/* special case */
			}
		break;
	case 1196:
#line 10776 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{ yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;	/* special case */
			}
		break;
	case 1197:
#line 10782 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = osmkident(yypvt[-1].st_name, (OSNODE*)NULL);
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode( BLANK, (U_ARG*)NULL,
					&u_ptr[0], &u_ptr[1]);
		}
		break;
	case 1198:
#line 10790 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1199:
#line 10807 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = ERx("system user");
		}
		break;
	case 1200:
#line 10811 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = ERx("session user");
		}
		break;
	case 1201:
#line 10816 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = yypvt[-0].st_tlist;
		}
		break;
	case 1202:
#line 10820 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = osaddtlist(yypvt[-2].st_tlist, yypvt[-0].st_tlist);
		}
		break;
	case 1203:
#line 10825 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = osmaketle(yypvt[-2].st_node, yypvt[-0].st_node, (OSNODE *)NULL);
		}
		break;
	case 1204:
#line 10829 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = osmaketle(yypvt[-2].st_node, osmkident(yypvt[-0].st_name, (OSNODE *) NULL),
						(OSNODE *)NULL);
		}
		break;
	case 1205:
#line 10834 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = osmaketle(yypvt[-2].st_node, osmkident(yypvt[-0].st_name, (OSNODE *) NULL),
						(OSNODE *)NULL);
		}
		break;
	case 1206:
#line 10839 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_tlist = osmaketle(yypvt[-2].st_node, osmkident(yypvt[-0].st_name, (OSNODE *) NULL),
						(OSNODE *)NULL);
		}
		break;
	case 1209:
#line 10858 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{  osdbstr(ERx(" add privileges "));}
		break;
	case 1210:
#line 10861 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{  osdbstr(ERx(" drop privileges "));}
		break;
	case 1211:
#line 10864 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbeval(yypvt[-0].st_node);
	}
		break;
	case 1214:
#line 10872 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbstr(ERx("("));
	}
		break;
	case 1215:
#line 10877 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbstr(ERx(")"));
	}
		break;
	case 1216:
#line 10882 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    osdbstr(ERx(","));
	}
		break;
	case 1217:
#line 10901 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGgenStmt(IL_DIREXIMM, (IGSID *)NULL, 0);
			osdbeval(yypvt[-1].st_node);
			osdbwith(yypvt[-0].st_tlist);	/* optional with clause */
		}
		break;
	case 1218:
#line 10908 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1219:
#line 10929 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( SQL )
				osOpenwarn(ERx("DIRECT CONNECT"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_DIRCONN, (IGSID *)NULL, 0);
			osdbwith(yypvt[-0].st_tlist);	/* optional with clause */
		}
		break;
	case 1220:
#line 10946 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( SQL )
				osOpenwarn(ERx("DIRECT DISCONNECT"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
			IGgenStmt(IL_DIRDIS, (IGSID *)NULL, 0);
		}
		break;
	case 1221:
#line 10972 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1222:
#line 11017 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( SQL )
				osOpenwarn(ERx("REGISTER"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1223:
#line 11024 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			IGstartStmt(osscnlno(), IL_LB_NONE);
			yyval.st_name = ERx("table");
		}
		break;
	case 1224:
#line 11029 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			osOpenwarn(ERx("REGISTER"));
			IGstartStmt(osscnlno(), IL_LB_NONE);
			yyval.st_name = ERx("view");
		}
		break;
	case 1225:
#line 11036 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = NULL;
		}
		break;
	case 1226:
#line 11040 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1227:
#line 11044 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = ERx("unique index");
		}
		break;
	case 1228:
#line 11048 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1229:
#line 11052 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1230:
#line 11057 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( !osw_compare(ERx("import"), yypvt[-0].st_name)
					&& !osw_compare(ERx("link"), yypvt[-0].st_name) )
			{
				yyerror(_SyntaxError);
			}
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1231:
#line 11067 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 1232:
#line 11071 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1233:
#line 11077 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		yyval.st_tlist = NULL;
	}
		break;
	case 1234:
#line 11081 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		yyval.st_tlist = yypvt[-1].st_tlist;
	}
		break;
	case 1235:
#line 11087 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		yyval.st_tlist = yypvt[-0].st_tlist;
	}
		break;
	case 1236:
#line 11091 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		yyval.st_tlist = osaddtlist(yypvt[-2].st_tlist, yypvt[-0].st_tlist);
	}
		break;
	case 1237:
#line 11097 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		yyval.st_tlist = yypvt[-0].st_tlist;
	}
		break;
	case 1238:
#line 11101 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1239:
#line 11117 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
		yyval.st_tlist = osmaketle(yypvt[-0].st_node, (OSNODE *)NULL, (OSNODE *)NULL);
	}
		break;
	case 1240:
#line 11122 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = osmkident(yypvt[-1].st_name, (OSNODE *) NULL);
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode( BLANK, (U_ARG *) NULL,
						&u_ptr[0], &u_ptr[1]
			);
		}
		break;
	case 1241:
#line 11131 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 1242:
#line 11135 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1243:
#line 11147 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1244:
#line 11163 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( SQL )
				osOpenwarn(ERx("REMOVE"));
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1245:
#line 11169 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = ERx("table");
		}
		break;
	case 1246:
#line 11173 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( SQL )
				osOpenwarn(ERx("REMOVE"));
			yyval.st_name = ERx("view");
		}
		break;
	case 1247:
#line 11206 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-1].st_name;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(OP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1248:
#line 11213 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-1].st_name;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(OP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1249:
#line 11220 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = _Exp;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(OP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1250:
#line 11227 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 1251:
#line 11231 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = _UnaryMinus;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node =
			  osmknode(UNARYOP, &u_ptr[0], &u_ptr[1], (U_ARG*)NULL);
		}
		break;
	case 1252:
#line 11238 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1253:
#line 11288 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1254:
#line 11294 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			/* Translate strings */
			if ( yypvt[-0].st_node->n_token == tkSCONST )
				yypvt[-0].st_node->n_const = osstring( FALSE, yypvt[-0].st_node->n_const, osldml);
			else if ( yypvt[-0].st_node->n_token == tkXCONST )
				yypvt[-0].st_node->n_const = oshexvchar(yypvt[-0].st_node->n_const);	/* SQL only */
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1255:
#line 11303 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1256:
#line 11309 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1257:
#line 11313 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 1258:
#line 11327 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1259:
#line 11331 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 1260:
#line 11336 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 1261:
#line 11340 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 1262:
#line 11346 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1263:
#line 11358 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( !ADE_CHK_TYPE(yypvt[-0].st_node->n_type, DB_INT_TYPE) &&
					yypvt[-0].st_node->n_type != DB_NODT )
				oscerr(OSNOTINT, 0);
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1264:
#line 11373 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 1265:
#line 11377 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-1].st_name;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(LOGOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1266:
#line 11384 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = _Not;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(LOGOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1267:
#line 11391 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-1].st_name;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(RELOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1268:
#line 11398 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-0].st_name;
			u_ptr[1].u_nodep = yypvt[-1].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(RELOP, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1269:
#line 11405 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1270:
#line 11421 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1271:
#line 11435 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1272:
#line 11478 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkconst(tkSCONST, yypvt[-0].st_name);
		}
		break;
	case 1273:
#line 11482 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (QUEL)
				yyerror(_SyntaxError);
			yyval.st_node = osmkconst(tkXCONST, yypvt[-0].st_name);
		}
		break;
	case 1274:
#line 11488 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkconst(tkICONST, yypvt[-0].st_name);
		}
		break;
	case 1275:
#line 11492 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkconst(tkFCONST, yypvt[-0].st_name);
		}
		break;
	case 1276:
#line 11496 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkconst(tkDCONST, yypvt[-0].st_name);
		}
		break;
	case 1277:
#line 11501 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1278:
#line 11505 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			char    buf[OSBUFSIZE];

			buf[0] = '-'; STcopy( yypvt[-0].st_name, buf + 1 );
			yyval.st_node = osmkconst(tkICONST, iiIG_string(buf));
		}
		break;
	case 1279:
#line 11512 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			char    buf[OSBUFSIZE];

			buf[0] = '-'; STcopy( yypvt[-0].st_name, buf + 1 );
			yyval.st_node = osmkconst(tkFCONST, iiIG_string(buf));
		}
		break;
	case 1280:
#line 11519 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			char    buf[OSBUFSIZE];

			buf[0] = '-'; STcopy( yypvt[-0].st_name, buf + 1 );
			yyval.st_node = osmkconst(tkDCONST, iiIG_string(buf));
		}
		break;
	case 1281:
#line 11527 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1282:
#line 11531 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkconst(tkNULL, yypvt[-0].st_name);
		}
		break;
	case 1283:
#line 11536 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1284:
#line 11540 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkconst(tkNULL, yypvt[-0].st_name);
		}
		break;
	case 1285:
#line 11545 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkconst(tkSCONST, yypvt[-0].st_name);
		}
		break;
	case 1286:
#line 11550 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkconst(tkICONST, yypvt[-0].st_name);
		}
		break;
	case 1287:
#line 11554 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			char	buf[64];

			buf[0] = '-'; STcopy( yypvt[-0].st_name, buf + 1 );
			yyval.st_node = osmkconst(tkICONST, iiIG_string(buf));
		}
		break;
	case 1288:
#line 11563 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = _Mul;
		}
		break;
	case 1289:
#line 11567 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = _Div;
		}
		break;
	case 1290:
#line 11573 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = _Plus;
		}
		break;
	case 1291:
#line 11577 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = _Minus;
		}
		break;
	case 1292:
#line 11583 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = _And;
		}
		break;
	case 1293:
#line 11587 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = _Or;
		}
		break;
	case 1294:
#line 11593 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = _Less;
		}
		break;
	case 1295:
#line 11597 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = _Greater;
		}
		break;
	case 1296:
#line 11601 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = _Equal;
		}
		break;
	case 1297:
#line 11605 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = _LessEqual;
		}
		break;
	case 1298:
#line 11609 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = _GreatrEqual;
		}
		break;
	case 1299:
#line 11613 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = _NotEqual;
		}
		break;
	case 1300:
#line 11619 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = _Like;
		}
		break;
	case 1301:
#line 11623 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = _NotLike;
		}
		break;
	case 1302:
#line 11633 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkconst(tkSCONST, yypvt[-0].st_name);
		}
		break;
	case 1303:
#line 11637 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkconst(tkXCONST, yypvt[-0].st_name);
		}
		break;
	case 1304:
#line 11641 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 1305:
#line 11662 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-1].st_node;
			yyval.st_node = osmknode(NPRED, &u_ptr[0], (U_ARG*)NULL, (U_ARG*)NULL);
		}
		break;
	case 1306:
#line 11668 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( RepeatState )
			{
				oswarn( E_OS015B_QualRepeat, 0 );
				RepeatState = FALSE;
			}
		}
		break;
	case 1307:
#line 11677 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1308:
#line 11684 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1309:
#line 11692 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-4].st_name;
			u_ptr[1].u_cp = yypvt[-2].st_name;
			u_ptr[2].u_symp = yypvt[-0].st_sym;
			yyval.st_node = osmknode(PREDE, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1310:
#line 11699 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1311:
#line 11711 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1312:
#line 11723 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1313:
#line 11738 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1314:
#line 11743 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1315:
#line 11765 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1316:
#line 11786 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-0].st_name;
			u_ptr[1].u_cp = NULL;
			yyval.st_node = osmknode(ATTR, &u_ptr[0], &u_ptr[1], (U_ARG*)NULL);
			FormObj = NULL;
		}
		break;
	case 1317:
#line 11793 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_cp = yypvt[-2].st_name;
			u_ptr[1].u_cp = yypvt[-0].st_name;
			yyval.st_node = osmknode(ATTR, &u_ptr[0], &u_ptr[1], (U_ARG*)NULL);
			FormObj = NULL;
		}
		break;
	case 1318:
#line 11806 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{ /* Note:  FORM was allowed */
			yyval.st_node = osall(FormSym, yypvt[-2].st_node);
		}
		break;
	case 1321:
#line 11823 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{AllowForm = FALSE;}
		break;
	case 1322:
#line 11824 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = os_lhs(yypvt[-0].st_node);
		}
		break;
	case 1323:
#line 11886 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1324:
#line 11890 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1325:
#line 11912 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1326:
#line 11923 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1327:
#line 11945 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1328:
#line 11964 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1329:
#line 12009 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1330:
#line 12020 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osvalnode(FormSym, yypvt[-0].st_name);
		}
		break;
	case 1331:
#line 12024 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osdotnode(FormSym, yypvt[-2].st_node, yypvt[-0].st_name);
		}
		break;
	case 1332:
#line 12028 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osarraynode(yypvt[-1].st_node, yypvt[-0].st_node, TRUE);
		}
		break;
	case 1333:
#line 12052 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1334:
#line 12056 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1335:
#line 12072 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;	/* special case */
		}
		break;
	case 1336:
#line 12077 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (!oschkstr(yypvt[-1].st_node->n_type))
				oscerr(OSNOTSTR, 1, ERget(_FrsName));
			else if ( AFE_NULLABLE_MACRO(yypvt[-1].st_node->n_type) )
				oswarn(OSNULVCHK, 0);
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 1337:
#line 12098 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 1338:
#line 12102 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkconst(tkSCONST, yypvt[-0].st_name);
		}
		break;
	case 1339:
#line 12106 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (yypvt[-1].st_node == NULL || !oschkstr(yypvt[-1].st_node->n_type))
				oscerr(OSNOTSTR, 1, ERget(_FrsName));
			else if ( AFE_NULLABLE_MACRO(yypvt[-1].st_node->n_type) )
				oswarn(OSNULVCHK, 0);
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 1340:
#line 12122 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1341:
#line 12129 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1342:
#line 12141 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1343:
#line 12145 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-2].st_node;
			u_ptr[1].u_nodep = yypvt[-0].st_node;
			yyval.st_node = osmknode(COMMA, (U_ARG*)NULL, &u_ptr[0], &u_ptr[1]);
		}
		break;
	case 1344:
#line 12152 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-1].st_node;
			yyval.st_node = osmknode(PARENS, (U_ARG*)NULL, &u_ptr[0], (U_ARG*)NULL);
		}
		break;
	case 1345:
#line 12157 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 1346:
#line 12182 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1347:
#line 12186 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if (yypvt[-1].st_node == NULL || !ADE_CHK_TYPE(yypvt[-1].st_node->n_type, DB_INT_TYPE))
				oscerr(OSNOTINT, 0);
			else if ( AFE_NULLABLE_MACRO(yypvt[-1].st_node->n_type) )
				oswarn(OSNULVCHK, 0);
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 1348:
#line 12202 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = yypvt[-2].st_node;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1349:
#line 12209 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			u_ptr[0].u_nodep = yypvt[-0].st_node;
			u_ptr[1].u_nodep = NULL;
			u_ptr[2].u_nodep = NULL;
			yyval.st_node = osmknode(NLIST, &u_ptr[0], &u_ptr[1], &u_ptr[2]);
		}
		break;
	case 1350:
#line 12217 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1351:
#line 12221 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( !oschkstr(yypvt[-1].st_node->n_type) )
				oscerr(OSNOTSTR, 0);
			else if ( AFE_NULLABLE_MACRO(yypvt[-1].st_node->n_type) )
				oswarn(OSNULVCHK, 0);
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 1352:
#line 12231 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1353:
#line 12235 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = NULL;
		}
		break;
	case 1354:
#line 12255 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
			yyval.st_node->n_type = DB_CHA_TYPE;	/* special case */
		}
		break;
	case 1355:
#line 12260 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( !ADE_CHK_TYPE(yypvt[-1].st_node->n_type, DB_INT_TYPE) &&
					!oschkstr(yypvt[-1].st_node->n_type) )
				oscerr(OSNOTREF, 0);
			else if ( AFE_NULLABLE_MACRO(yypvt[-1].st_node->n_type) )
				oswarn(OSNULVCHK, 0);
			yyval.st_node = yypvt[-1].st_node;
		}
		break;
	case 1356:
#line 12269 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = yypvt[-0].st_node;
		}
		break;
	case 1357:
#line 12276 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
    	    yyval.st_node = yypvt[-0].st_node;
        }
		break;
	case 1358:
#line 12280 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
            u_ptr[0].u_nodep = yypvt[-2].st_node;
    	    u_ptr[0].u_nodep->n_type = DB_CHA_TYPE; /* see ingres_name def. */
            u_ptr[1].u_nodep = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
    	    u_ptr[1].u_nodep->n_type = DB_CHA_TYPE;  /* see ingres_name def. */
            yyval.st_node = osmknode(DOT|DML, (U_ARG*)NULL, &u_ptr[0], &u_ptr[1]);
        }
		break;
	case 1359:
#line 12290 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 1360:
#line 12294 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 1361:
#line 12298 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_node = osmkident(yypvt[-0].st_name, (OSNODE *)NULL);
		}
		break;
	case 1362:
#line 12304 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
    	    yyval.st_node = yypvt[-0].st_node;
        }
		break;
	case 1363:
#line 12308 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
	    char    buf[OSBUFSIZE];

	    _VOID_ STprintf(buf, ERx("%s.%s"), yypvt[-2].st_name, yypvt[-0].st_name);
	    yyval.st_node = osmkident( iiIG_string(buf), (OSNODE *)NULL);
	    yyval.st_node->n_type = DB_CHA_TYPE;		/* special case */
        }
		break;
	case 1364:
#line 12318 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1365:
#line 12322 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = yypvt[-0].st_name;
		}
		break;
	case 1366:
#line 12376 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			if ( RepeatState )
			{
				oswarn( E_OS015A_BadRepeat, 0 );
				RepeatState = FALSE;
			}
		}
		break;
	case 1367:
#line 12389 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_nat = 1;
		}
		break;
	case 1368:
#line 12393 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_nat = 0;
		}
		break;
	case 1369:
#line 12407 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
{
			yyval.st_name = (yypvt[-1].st_nat == 0) ? _IsNull : _IsNotNull;
		}
		break;
	case 1370:
#line 12412 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	case 1371:
#line 12429 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/abf/sql/sql.yf"
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
	return 0; /* Success */

	}
