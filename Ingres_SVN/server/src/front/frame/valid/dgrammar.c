
#line 2 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
/*
**	dgrammar.y
**
**	Copyright (c) 1989 Ingres Corporation
**	All rights reserved.
*/

/* # include's */

# include	<compat.h>
# include	<gl.h>
# include       <me.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ft.h>
# include	<fmt.h>
# include	<adf.h>
# include	<frame.h>
# include	"dnode.h"
# include	"derive.h"
# include	<si.h>
# include	<er.h>
# include	<erfi.h>

#if defined(hp3_us5) || defined (hp8_us5) || defined (hp9_mpe)
#define __YYSCLASS
#endif /* hp3_us5 || hp8_us5 || hp9_mpe */

/**
**  Name:	dgrammar.y  -  Grammar Describing the Derivation Strings
**
**  Description:
**	This is the grammar used for parsing the derivation strings.
**	The grammar is initialized by calling IIFVdinit() in lex.c
**	(also called by IIFVvinit()) and called as IIFVdpParse().
**
**	This file defines:
**
**	IIFVdpParse	Derivation formula parser.
**
**  History:
**	06/05/89 (dkh) - Initial version.
**	06/20/89 (dkh) - Put in some more semantics checking.
**	10/02/89 (dkh) - Changed IIFVdcDeriveCheck() to IIFVcdChkDerive(),
**			 IIFVdchk to IIFVdck and IIFVdeError() to IIFVdError().
**	10/02/89 (dkh) - Changed IIFVderrflag to IIFVdeflag.
**	19-oct-1990 (mgw)
**		Fixed #include of local derive.h and dnode.h to use ""
**		instead of <>
**	06/02/01 (johnr)
**		Put in flag and added define yymaxdepth for hp3_us5.
**      02-Oct-91 (Sanjive)
**            Define yys as IIFVdyys for dra_us5 and dr6_us5 to stop multiple 
**	      defines problem due to duplicate definition of yys in grammar.y
**	03/01/92 (dkh) - Changed above fix to be more generic so that
**			 there is no possibility that either grammars in
**			 the forms system will conflict with a user's grammar.
**    04/21/92(kevinm) (jillb/jroy--DGC)
**            Undefine MIN and MAX before its definition in the grammar.
**	09-Jun-92 (fredb)
**		Added hp9_mpe to defines that solve multiple definition
**		problem in linker due to dgrammar and grammar in the same lib.
**	06/23/92 (dkh) - Added support for decimal.
**	23-Jun-92 (fredb)
**		Added hp9_mpe to code to define __YYSCLASS.
**	27-jul-1992 (thomasm) add hp8 to hp3 specific changes.
**      06-jul-92 (sweeney)
**              Apollo loader beefs about multiple global definitions.
**	09-sep-1992 (ricka)
**		VMS compiler cannot handle lines extended by using a
**		" \ ".
**	29-jan-93 (sweeney)
**		undef abs() for usl_us5 - its yacc generates a #include
**		of stdlib.h, which contains a forward reference for
**		abs() which is getting macroexpanded into garbage.
**	25-mar-93 (sweeney)
**		#define yy_yyv to be IIFVd_yyv, else it clashes with
**		the yacc'ed yy_yyv in front!st!install. I may be fixing this
**		in the wrong place, but it can't hurt. Plus a layout change.
**     03-jun-93 (pauland)
**              Added change as suggested by Tony sweeney.
**              undef abs() for usl_us5 - its yacc generates a #include
**              of stdlib.h, which contains a forward reference for
**              abs() which is getting macroexpanded into garbage.
**      20-nov-1994 (andyw)
**              Solaris 2.4/Compiler 3.00 failed with macroexpand referenced
**              function abs. changed as per Tony's previous change to usl_us5
**              Solaris 2.4/Compiler 3.00 problems
**	22-jun-95 (allmi01)
**		Added dgi_us5 support for DG-UX on Intel based platforms following dg8_us5.
**	29-jul-1997 (walro03)
**		Undefine abs() for Tandem NonStop (ts2_us5), like other
**		platforms.
##      28-aug-1997 (musro02)
##          Undef abs for sqs_ptx
**	10-may-1999 (walro03)
**	    Remove obsolete version string apl_us5.
**	10-nov-1999 (kitch01)
**		Bug 98227. Cannot use a comma in a derived field formula.
**		Added COMMA to the list of valid tokens.
**
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	20-aug-1999 (popri01)
**	    Eliminate "undef abs" for Unixware (usl_us5).
**	    It's now done in compat hdr.
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/


/* extern's */
FUNC_EXTERN	DNODE	*IIFVmnMakeNode();
FUNC_EXTERN	DNODE	*IIFVmsMathSetup();
FUNC_EXTERN	VOID	IIFVagAggSetup();
FUNC_EXTERN	VOID	IIFVcdChkDerive();
FUNC_EXTERN	void	IIFVndNegateDecimal();

GLOBALREF	FLDTYPE	*vl_curtype;
GLOBALREF	bool	IIFVddpDebugPrint;

/*
** yacc on usl_us5 is about to spit out a #include for <stdlib.h>
** which has a prototyped forward reference for abs(int).
** Because we define abs as a macro in compat.h, this forward
** ref gets macroexpanded into "extern int ((int) > 0? (int) : -(int))"
** which is clearly garbage, and won't compile, so stomp on the macro
*/
# ifdef sqs_ptx
# undef abs
# endif

# ifdef su4_us5
# undef abs
# endif

# ifdef ts2_us5
# undef abs
# endif

/* # define's */
# define	yyparse		IIFVdpParse
# define	yyerror		IIFVdError
# define	yylex		IIFVdlex
# define	yylval		IIFVdlval
# define	yyval		IIFVdval
# define	YYSTYPE		IIFVdstype
# define	yychar		IIFVdchar
# define	yyerrflag	IIFVdeflag
# define	yyexca		IIFVdexca
# define	yyact		IIFVdact
# define	yypact		IIFVdpact
# define	yypgo		IIFVdpgo
# define	yyr1		IIFVdr1
# define	yyr2		IIFVdr2
# define	yychk		IIFVdck
# define	yydef		IIFVd1def
# define	yydebug		IIFVd2debug
# define	yynerrs		IIFVdnerrs
# define	yyv		IIFVdv
# define	yypvt		IIFVdpvt
# define	yys		IIFVds
# define	yy_yyv		IIFVd_yyv
# if defined(hp3_us5) || defined (hp8_us5) || defined (hp9_mpe)
# define	yymaxdepth	IIFVdmaxdepth
# endif /* hp3_us5, hp8_us5, hp9_mpe */

# if defined(dg8_us5) || defined(dgi_us5)
# ifdef MAX
# undef MAX
# endif /* MAX */
# ifdef MIN
# undef MIN
# endif /* MIN */
# endif /* dg8_us5 dgi_us5 */

/* static's */

#define	LPAREN		257
#define	RPAREN		258
#define	PERIOD		259
#define	COMMA		260
#define	SUM		261
#define	MAX		262
#define	MIN		263
#define	AVERAGE		264
#define	COUNT		265
#define	EXP		266
#define	PLUS		267
#define	MINUS		268
#define	TIMES		269
#define	DIV		270
#define	SCONST		271
#define	LBRAK		272
#define	RBRAK		273
#define	NAME		274
#define	SVCONST		275
#define	I4CONST		276
#define	F8CONST		277
#define	DECCONST		278
#define	UMINUS		279
#define	yyclearin	yychar = -1
#define	yyerrok		yyerrflag = 0
extern int		yychar;
extern short		yyerrflag;
#ifndef YYMAXDEPTH
#define	YYMAXDEPTH	150
#endif
YYSTYPE			yylval, yyval;
#define	YYERRCODE	256

#line 642 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"


# include	"dtokens.h"

/*
**   YYERROR -- the routine which error routine yacc calls
*/

yyerror(errmessage)
char	*errmessage;
{
	IIFVdeErr(E_FI20D2_8402, 0);
}
short yyexca[] = {
	-1, 1,
	0, -1,
	-2, 0,
};

#define	YYNPROD		32
#define	YYLAST		87

short yyact[] = {
	     3,     4,    32,    33,    34,    19,    20,    21,    22,    23,
	    46,    43,     5,    42,    49,    45,    47,    28,    13,     8,
	    14,    16,    17,    15,     4,    50,     6,    48,    19,    20,
	    21,    22,    23,    31,    28,     5,    35,    26,    27,    41,
	    12,    13,     8,    14,    16,    17,    15,    28,    24,    25,
	    26,    27,    28,    24,    25,    26,    27,     2,     7,    18,
	    11,    10,    29,    30,     9,    44,     1,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,    36,    37,    38,    39,    40
};

short yypact[] = {
	  -256, -1000,  -214, -1000,  -233,  -233, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,  -226, -1000,  -274, -1000, -1000,  -221, -1000,
	 -1000, -1000, -1000, -1000,  -233,  -233,  -233,  -233,  -233,  -219,
	 -1000,  -261, -1000, -1000, -1000,  -263,  -232,  -232,  -249,  -249,
	 -1000, -1000, -1000,  -257,  -264,  -253,  -231,  -259, -1000,  -234,
	 -1000
};

short yypgo[] = {
	     0,    66,    65,    64,    61,    60,    59,    58,    40,    57,
	    26
};

short yyr1[] = {
	     0,     1,     1,     9,     9,     9,     9,     9,     9,     9,
	     9,    10,    10,    10,    10,    10,    10,     7,     7,     8,
	     2,     6,     6,     6,     6,     6,     3,     3,     4,     4,
	     5,     5
};

short yyr2[] = {
	     0,     1,     1,     3,     3,     3,     3,     3,     3,     2,
	     1,     1,     1,     1,     1,     1,     1,     1,     3,     6,
	     4,     1,     1,     1,     1,     1,     1,     2,     1,     2,
	     1,     2
};

short yychk[] = {
	 -4000,    -1,    -9,   256,   257,   268,   -10,    -7,   275,    -3,
	    -4,    -5,    -8,   274,   276,   279,   277,   278,    -6,   261,
	   262,   263,   264,   265,   267,   268,   269,   270,   266,    -9,
	    -9,   259,   276,   277,   278,   257,    -9,    -9,    -9,    -9,
	    -9,   258,   274,   274,    -2,   272,   274,   269,   258,   273,
	   259
};

short yydef[] = {
	     0,    -2,     1,     2,     0,     0,    10,    11,    12,    13,
	    14,    15,    16,    17,    26,     0,    28,    30,     0,    21,
	    22,    23,    24,    25,     0,     0,     0,     0,     0,     0,
	     9,     0,    27,    29,    31,     0,     4,     5,     6,     7,
	     8,     3,    18,     0,     0,     0,     0,     0,    19,     0,
	    20
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
#line 208 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Need to do datatype compatibility checking of node
		**  and field.  Do coercion processing if necessary.
		**  Assign $1 to TREE pointer.
		*/
		IIFVcdChkDerive(yypvt[-0].node_type);
		vl_curtype->ftvalchk = (VTREE *) yypvt[-0].node_type;

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(
			    ERx("Successfully parsed a derivation string.\n"));
		}
	}
		break;
	case 2:
#line 224 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		vl_curtype->ftvalchk = NULL;

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(
				ERx("Error parsing a derivation string.\n"));
		}
	}
		break;
	case 3:
#line 236 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Do $$ = $2 assignment.
		*/
		yyval.node_type = yypvt[-1].node_type;

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(
				ERx("LPAREN expr RPAREN reduction.\n"));
		}
	}
		break;
	case 4:
#line 249 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Create new node, do datatype compatibility
		**  checking and any necessary coercions.
		*/
		yyval.node_type = IIFVmsMathSetup(PLUS_NODE, yypvt[-2].node_type, yypvt[-0].node_type);

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(ERx("expr + expr.\n"));
		}
	}
		break;
	case 5:
#line 262 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Create new node, do datatype compatibility
		**  checking and any necessary coercions.
		*/
		yyval.node_type = IIFVmsMathSetup(MINUS_NODE, yypvt[-2].node_type, yypvt[-0].node_type);

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(ERx("expr - expr.\n"));
		}
	}
		break;
	case 6:
#line 275 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Create new node, do datatype compatibility
		**  checking and any necessary coercions.
		*/
		yyval.node_type = IIFVmsMathSetup(MULT_NODE, yypvt[-2].node_type, yypvt[-0].node_type);

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(ERx("expr * expr.\n"));
		}
	}
		break;
	case 7:
#line 288 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Create new node, do datatype compatibility
		**  checking and any necessary coercions.
		*/
		yyval.node_type = IIFVmsMathSetup(DIV_NODE, yypvt[-2].node_type, yypvt[-0].node_type);

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(ERx("expr / expr.\n"));
		}
	}
		break;
	case 8:
#line 301 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Create new node, do datatype compatibility
		**  checking and any necessary coercions.
		*/
		yyval.node_type = IIFVmsMathSetup(EXP_NODE, yypvt[-2].node_type, yypvt[-0].node_type);

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(ERx("expr ** expr.\n"));
		}
	}
		break;
	case 9:
#line 314 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Create new node, and create coercion function.
		*/
		yyval.node_type = IIFVmsMathSetup(UMINUS_NODE, yypvt[-0].node_type, NULL);

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(ERx("minus expr.\n"));
		}
	}
		break;
	case 10:
#line 326 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Just assign $$ = $1.
		*/
		yyval.node_type = yypvt[-0].node_type;

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(ERx("A value reduction.\n"));
		}
	}
		break;
	case 11:
#line 340 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Just assign $$ = $1.
		*/
		yyval.node_type = yypvt[-0].node_type;

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(ERx("An attrib reduction.\n"));
		}
	}
		break;
	case 12:
#line 352 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Create node, assign datatype and assign
		**  $$ from nodemaker.
		*/
		yyval.node_type = IIFVmnMakeNode(SCONST_NODE, (YYSTYPE *)&yypvt[-0].string_type, NULL, NULL);

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(ERx("A v_string constant.\n"));
		}
	}
		break;
	case 13:
#line 365 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Just assign $$ = $1.
		*/
		yyval.node_type = IIFVmnMakeNode(ICONST_NODE, (YYSTYPE *)&yypvt[-0].I4_type, NULL, NULL);

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(ERx("An inum constant.\n"));
		}
	}
		break;
	case 14:
#line 377 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Just assign $$ = $1.
		*/
		yyval.node_type = IIFVmnMakeNode(FCONST_NODE, (YYSTYPE *)&yypvt[-0].F8_type, NULL, NULL);

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(ERx("A fnum constant.\n"));
		}
	}
		break;
	case 15:
#line 389 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		yyval.node_type = IIFVmnMakeNode(DCONST_NODE, (YYSTYPE *)&yypvt[-0].dec_type, NULL, NULL);
		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(ERx("A dnum constant.\n"));
		}
	}
		break;
	case 16:
#line 397 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Just assign $$ = $1.
		*/
		yyval.node_type = yypvt[-0].node_type;

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(
				ERx("Column aggregate to value reduction.\n"));
		}
	}
		break;
	case 17:
#line 416 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Check for field existence.
		**  If this derviation string is for a tf column,
		**  disallow it.
		**  Create node for name, just various attributes
		**  such as datatype.  Assign $$ from nodemaker.
		*/
		yyval.node_type = IIFVmnMakeNode(SFLD_NODE, NULL, yypvt[-0].node_type, NULL);

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(ERx("Got a single name.\n"));
		}
	}
		break;
	case 18:
#line 432 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  check for column existence.
		**  If this derivation string is for a simple
		**  field, disallow it.
		**  Create node for tf column, set various
		**  attributes such as datatype.  Assign
		**  $$ from nodemaker.
		*/
		yyval.node_type = IIFVmnMakeNode(TCOL_NODE, NULL, yypvt[-2].node_type, yypvt[-0].node_type);

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(ERx("Got a tf column name.\n"));
		}
	}
		break;
	case 19:
#line 451 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  check for existence of tf column.  If this
		**  derivation string is for a tf column, then
		**  disallow it.  Use node created from agg
		**  and set various attrbutes.
		**  Assign $$ from $1.
		*/
		IIFVasAggSetup(yypvt[-5].node_type, yypvt[-3].node_type, yypvt[-1].node_type);
		yyval.node_type = yypvt[-5].node_type;

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(ERx("Got a column aggregate.\n"));
		}
	}
		break;
	case 20:
#line 470 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Nothing to do.  Assign $$ value of zero.
		*/
		yyval.type_type = 0;

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(
				ERx("Got the all columns piece.\n"));
		}
	}
		break;
	case 21:
#line 485 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Create SUM node.  Assing $$ from nodemaker.
		*/
		yyval.node_type = IIFVmnMakeNode(SUM_NODE, NULL, NULL, NULL);

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(
				ERx("Got aggregate keyword SUM.\n"));
		}
	}
		break;
	case 22:
#line 498 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Create MAX node.  Assign $$ from nodemaker.
		*/
		yyval.node_type = IIFVmnMakeNode(MAX_NODE, NULL, NULL, NULL);

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(
				ERx("Got aggregate keyword MAX.\n"));
		}
	}
		break;
	case 23:
#line 511 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Create MIN node.  Assign $$ from nodemaker.
		*/
		yyval.node_type = IIFVmnMakeNode(MIN_NODE, NULL, NULL, NULL);

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(
				ERx("Got aggregate keyword MIN.\n"));
		}
	}
		break;
	case 24:
#line 524 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Create AVERAGE node.  Assign $$ from nodemaker.
		*/
		yyval.node_type = IIFVmnMakeNode(AVG_NODE, NULL, NULL, NULL);

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(
				ERx("Got aggregate keyword AVERAGE.\n"));
		}
	}
		break;
	case 25:
#line 537 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Create COUNT node.  Assign $$ from nodemaker.
		*/
		yyval.node_type = IIFVmnMakeNode(CNT_NODE, NULL, NULL, NULL);

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(
				ERx("Got aggregate keyword COUNT.\n"));
		}
	}
		break;
	case 26:
#line 552 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Create node and setup DBV constant.
		**  Assing $$ from nodemaker.
		*/
		yyval.I4_type = yypvt[-0].I4_type;

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(
				ERx("Reducing I4CONST to inum.\n"));
		}
	}
		break;
	case 27:
#line 566 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Create node and setup DBV constant.
		**  Assign $$ from nodemaker.  Do uminus operation.
		*/
		yyval.I4_type = - (yypvt[-0].I4_type);

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(
				ERx("Reducing - I4CONST to inum.\n"));
		}
	}
		break;
	case 28:
#line 582 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Create node and setup DBV constant.
		**  Assign $$ from nodemaker.
		*/
		yyval.F8_type = yypvt[-0].F8_type;

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(
				ERx("Reducing F8CONST to fnum.\n"));
		}
	}
		break;
	case 29:
#line 596 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Create node and setup DBV constant.
		**  Assign $$ from nodemaker.  Do uminus operation.
		*/
		yyval.F8_type = - (yypvt[-0].F8_type);

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(
				ERx("Reducing - F8CONST to fnum.\n"));
		}
	}
		break;
	case 30:
#line 612 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Create node and setup DBV constant.
		**  Assign $$ from nodemaker.
		*/
		yyval.dec_type = yypvt[-0].dec_type;

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(
				ERx("Reducing DECCONST to dnum.\n"));
		}
	}
		break;
	case 31:
#line 626 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/dgrammar.yf"
{
		/*
		**  Create node and setup DBV constant.
		**  Assign $$ from nodemaker.  Do uminus operation.
		*/
		IIFVndNegateDecimal(yypvt[-0].dec_type, (i4) FALSE);
		yyval.dec_type = yypvt[-0].dec_type;

		if (IIFVddpDebugPrint)
		{
			IIFVdtpDrvTracePrint(
				ERx("Reducing - DECCONST to dnum.\n"));
		}
	}
		break;
		}
		goto yystack;  /* stack new state and value */

	}
