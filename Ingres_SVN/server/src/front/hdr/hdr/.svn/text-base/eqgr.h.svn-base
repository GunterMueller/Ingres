/*
** Copyright (c) 2004 Ingres Corporation
*/

/*
** Filename:	eqgr.h
** Purpose:	Define objects global to the grammar, and an entry point 
**		into a grammar mechanism (gr_mechanism).
**
** History:	01-jan-1985	- Written for EQUEL (ncg)
**		09-aug-1985	- Extended for ESQL (ncg)
**	25-mar-1991 (barbara)
**	    Added 'gr_flag' flags for supporting COPYHANDLER.
**	04-mar-1991 (barbara)
**	    Added another flag for COPYHANDLER; also changed names of some.
**	20-nov-1992 (Mike S)
**	    Add GR_s4GL
**	25-jun-1993 (kathryn)
**	    Added gr_flag GR_ENDIF to generate "end if" for Fortran SELECT stmt.
**	08-mar-1996 (thoda04)
**	    Cast GR_ flags to i4  for the 16 bit Windows 3.1 port.
**	4-oct-01 (inkdo01)
**	    Added define for YYEMBED to enable parser reserved word retry 
**	    logic for embedded programs.
**	15-feb-02 (toumi01)
**	    Changed YYEMBED to YYRETRY and added YYRETRYTOKEN to generalize
**	    reserved word retry so that it also works with ABF.
**
** Note:
**    - EQUEL includers must include <eqsym.h> first because of the SYM pointer
** 	in the YYSTYPE.
**    - ESQL includers (from the grammar only) must define ESQL and include
**	<sqtree.h> before compiling.
**
** Description:
**
** Grammar header file brought into both the grammar files,
** and other files that require either grammar defined constants
** or the grammar function, gr_mechanism().
** The G grammar is the main EQUEL grammar, while the L grammar is the
** language dependent module.
**
** Rules for L:
** 1. All tokens must begin with a lowercase 't'. This is to prevent
**    conflicts with constants brought in from other headers.
** 1. All langauge dependent rules must begin with the first letter of
**    the language in caps.  For example a "decl" rule for C is "Cdecl".
** 2. L must provide a gr_mechanism() to support the standard options
**    plus local L options.
** 3. L should define a table of GR_TYPES to initialize the symbol table
**    with on the GR_INIT option.
** 4. L must provide a structure, struct gr_state, that includes at least the 
**    following members to be used by G:
**	SYM	*gr_sym		-	Symbol table entry.
**	i4	gr_type		- 	Type of variable.
**	char	*gr_id		-	Variable/identifier name.
**	i4	gr_flag		-	Global state keeper.
**    A local pointer "gr" should point at an initialized structure gr_state.
** 5. All variable and name rules reduced to G should set the following
**    structure members:
**	gr_sym		-	Last symbol table entry,
**	gr_type		-	Last variable or constant type,
**	gr_id		- 	Name of last object.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/

/* Structure defining what needs to be known for L types */

typedef struct {
    char	*gt_id;			/* Name of type */
    i2		gt_rep;			/* Constant representation */
    i2		gt_len;			/* Length of type */
} GR_TYPES;

/* The L grammar should define gr_typetab[] and initialize it at startup */

# ifndef  DBG_COMMENT
    void	gr_mechanism(/*nat flag, other arguments vary*/);
# else				/* A void function cannot be put in a table */
    i4		gr_mechanism(/*nat flag, other arguments vary*/);
# endif   /* DBG_COMMENT */

/* Gr_mechanism() must provide entry points for the following constants */

# define GR_EQINIT   (i4)0 		/* Initialize Equel stuff */
# define GR_SYMINIT  (i4)1 		/* Initialize symbol table */
# define GR_LOOKUP   (i4)2 		/* Special lookup cases (C, Pascal) */
# define GR_STMTFREE (i4)3 		/* Free at least G globals after stmt */
# define GR_HOSTFREE (i4)4 		/* Free L globs after parsing hostvar */
# define GR_DUMP     (i4)5 		/* Dump at least the G globals */
# define GR_CLOSE    (i4)6 		/* Close off the grammar */
# define GR_EQSTMT   (i4)7 		/* See modes below */
# define GR_BLOCK    (i4)8 		/* Inform symtab of new scope */
# define GR_LENFIX   (i4)9 		/* Adjust size of basic types */
# define GR_NUMBER   (i4)10		/* Lookup numbers (Cobol, Pl1) */
# define GR_ERROR    (i4)11		/* Clean up after errors */
# define GR_OPERATOR (i4)12		/* Lookup operators (Pl1) */

/* Modes for GR_EQSTMT */
# define GR_sQUEL	(i4)0
# define GR_sREPEAT	(i4)1
# define GR_sLIBQ	(i4)2
# define GR_sFORMS	(i4)3
# define GR_sSQL	(i4)4		/* Added for ESQL */
# define GR_sCURSOR	(i4)5		/* Added for EQUEL cursors */
# define GR_sNODB	(i4)6		/* LIBQ but NO DB stuff */
# define GR_s4GL	(i4)7		/* Added for 4GL */

/* Modes for GR_BLOCK */
# define GR_BLKFALSE	(i4)0
# define GR_BLKTRUE	(i4)1

/* Define for Yacc parser so we can use yy_dodebug */

# define YYDEBUG	1

/* 
** Define YYSTYPE for Yacc, as we do not want to use the %union tool, and
** have to %type every single rule.  Instead we typedef GR_YYSTYPE, and
** define YYSTYPE to be the same (so that Yacc will not think YYSTYPE's are
** int's); then every rule whose actions use the '$' operator will use
** something like:
**		$1.i
** The member names are very short so as not to clutter up the grammar 
** semantics.
*/

typedef union {
    i4		i;
    char	*s;
    SYM		*v;				/* L declarations and usage */
    i4		*ip;				/* Other general pointers */
    struct {		/* for FETCH with absolute/relative orientation */
	i4	intv1;				/* orientation code */
	i4	intv2;				/* orientation constant */
	SYM	*s1;
	char	*v1;				/* ptr to varname (or NULL) */
    } fet;
#ifdef ESQL					/* ESQL Grammar uses this  */
    SQNODE	*n;				/*      for SQL tree nodes */
#endif /* ESQL */
} GR_YYSTYPE;

# define	YYSTYPE		GR_YYSTYPE
# define	YYRETRY		1
# define	YYRETRYTOKEN	tNAME

/* 
** Special constant flags that can be used inside the grammar to allow 
** syntax directed parsing or other strange situations.
*/

/* Currently cannot nest Quel - remove this when we can ---> */
# define	GR_NONESTQUEL		/* End comment ----> */

/* States for gr_flag */

# define	GR_QUEL		000001	/* In Quel statement */
# define	GR_EQUEL	000002	/* In Equel statement */
# define	GR_REPEAT	000004	/* Repeat query */
# define	GR_NOSYNC	000010	/* No IIsync call */
# define	GR_RETRIEVE	000020	/* In Retrieve loop */
# define	GR_TUPSORT	000040	/* Special sort clause */
# define	GR_NOINDIR	000100	/* Do not check indirection */
# define	GR_HOSTCODE	000200	/* Just seen for 1st time */
# define	GR_SQL		000400	/* In SQL statement */
# define	GR_CURSOR	001000	/* In an EQUEL cursor statement */
# define	GR_REPLCSR	002000	/* In an EQUEL REPLACE CURSOR stmt */
# define	GR_DECLCSR	004000	/* In an EQUEL DECLARE CURSOR stmt */
# define	GR_ESQL		010000	/* In an ESQL stmt (like GR_EQUEL) */
# define	GR_PROCEXEC	020000	/* Generating EXECUTE PROCEDURE. */
# define	GR_ACTIVATE	040000	/* Seen an ACTIVATE statement */
# define	GR_HIGHX       0100000	/* Seen a high transaction id */	
# define	GR_LOWX        0200000	/* Seen a low transaction id */	
# define	GR_GETEVT      0400000	/* Generating GET EVENT */
# define	GR_CPYHNDDEF  01000000	/* Defining a COPYHANDLER */
# define	GR_COPY_PROG  02000000	/* In a COPY FROM/INTO PROG statement */
# define	GR_SEENCPYHND 04000000  /* Seen a COPY HANDLER */
# define	GR_ENDIF     010000000  /* Generate END IF after stmt */
