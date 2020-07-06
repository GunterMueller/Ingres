
#line 2 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
/*
**	GRAMMAR.y  -  Grammar describing the Validation Strings
**
**	This is the grammar used for parsing the validation strings.
**	Yacc compiles this grammar and places it the file "y.tab.c".
**	The grammar is initialized by calling yyvinit() in lex.c
**	and called as the routine yyparse().
**
**	History: 1-Aug-1981  -	written	  (JEN)
**		14-Mar-1983  -	added routine decls of v_maketree,
**				v_getlist, v_mklist - (nml)
**	04/07/87 (dkh) - Added support for ADTs.
**	08/14/87 (dkh) - ER changes.
**	06/23/89 (dkh) - Added support for derived fields.  Essentially,
**			 special macros have been added to allow two
**			 different parsers to exist in the forms system.
**			 This also allows users to pull in a standard
**			 yacc parser into their forms applications.
**	09/23/89 (dkh) - Porting changes integration.
**	10/02/89 (dkh) - Changed IIFVvchk to IIFVvck.
**	11/16/89 (dkh) - Put in support for owner.table.
**      07/02/91 (johnr) 
**		Put in YYSCLASS flag and define yymaxdepth for hp3_us5
**	03/01/92 (dkh) - Changed ingres63p/264210 fix to be more generic so that
**			 there is no possibility that either grammars in
**			 the forms system will conflict with a user's grammar.
**	09-Jun-92 (fredb)
**		Added hp9_mpe to defines that solve multiple definition
**		problem in linker due to dgrammar and grammar in the same lib.
**	06/12/92 (dkh) - Added support for decimal for 6.5.
**	06/18/92 (dkh) - Added support for owner.table in table lookup.
**	23-Jun-92 (fredb)
**		Added hp9_mpe to code to define __YYSCLASS.
**	27-jul-1992 (thomasm) - Expand hp3 specific changes to Hp8
**	28-aug-92 (sweeney)
**		Apollo loader beefs about multiple global definitions.
**	08/29/92 (dkh) - Fixed previous submission so that left off "|| \"
**			 on line 88.
**	09/02/92 (ricka)
**		VMS compiler cannot handle extended by using a        
**		" \ ".
**      18-feb-93 (sweeney) 
**              undef abs() for usl_us5 - its yacc generates a #include
**              of stdlib.h, which contains a forward reference for 
**              abs() which is getting macroexpanded into garbage.
**      25-mar-93 (sweeney) 
**		#define yy_yyv to be IIFVv_yyv, else it clashes with 
**		the yacc'ed yy_yyv in front!st!install. I may be fixing this  
**		in the wrong place, but it can't hurt. Plus a layout change.
**      03-jun-93 (pauland)
**              Change, as suggested by Tony Sweeney:
**              undef abs() for usl_us5 - its yacc generates a #include
**              of stdlib.h, which contains a forward reference for
**              abs() which is getting macroexpanded into garbage.
**	29-nov-93 (ricka)
**		One more time, VAX/VMS CANNOT handle lines extended with an
**		" \ ".
**	20-nov-1994 (andyw)
**	   	Solaris 2.4/Compiler 3.00 failed with macroexpand referenced
**		function abs. changed as per Tony's previous change to usl_us5
**	 	Solaris 2.4/Compiler 3.00 problems
**	29-jul-1997 (walro03)
**		Undefine abs() for Tandem NonStop (ts2_us5), like other
**		platforms.
**      28-aug-1997 (musro02)
**              Undef abs for sqs_ptx
**      12-Feb-1997 (mosjo01 & linke01)
**              Added the forward declaration for IIFVveError();
**              without the forward declaration, when this file is compiled
**              , got error messages "redeclaration of IIFVveError differs
**              previous declaration" and "return type (void) in IIFVveError
**              redeclaration is not compatible with the previous return type
**              (int)".  This is because IBM's yacc spits out the generated C
**              code in a different order than other platforms' yacc.
**	10-may-1999 (walro03)
**		Remove obsolete version string apl_us5.
** 
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/

# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<me.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ft.h>
# include	<fmt.h>
# include	<adf.h>
# include	<frame.h>
# include	<valid.h>
# include	<er.h>
# include	"erfv.h"

# if defined(hp3_us5) || defined(hp9_mpe) || defined (hp8_us5)
# define __YYSCLASS
# endif /* hp3_us5 || hp9_mpe || hp8_us5 */

FUNC_EXTERN	VTREE	*vl_maketree();
FUNC_EXTERN	VTREE	*vl_mkattrtree();
FUNC_EXTERN	VTREE	*vl_mkcoltree();
FUNC_EXTERN	VALROOT *vl_mklist();
FUNC_EXTERN	VALROOT *vl_getlist();
FUNC_EXTERN	VALROOT *IIFVotg_OwnerTableGetlist();
FUNC_EXTERN	void	IIFVndNegateDecimal();
FUNC_EXTERN	void	IIFVsdsScanDelimString();
void    IIFVveError();

/*
** yacc on usl_us5 is about to spit out a #include for <stdlib.h>
** which has a prototyped forward reference for abs(int).
** Because we define abs as a macro in compat.h, this forward
** ref gets macroexpanded into "extern int ((int) > 0? (int) : -(int))"
** which is clearly garbage, and won't compile, so stomp the macro.
*/
# ifdef sqs_ptx 
# undef abs
# endif

# ifdef usl_us5 
# undef abs
# endif

# ifdef su4_us5 
# undef abs
# endif

# ifdef ts2_us5 
# undef abs
# endif

/*
**  Special defintions to enable having two
**  yacc parsers in the forms system.
*/
# define	yyparse		IIFVvpParse
# define	yyerror		IIFVveError
# define	yylex		IIFVvlex
# define	yylval		IIFVvlval
# define	yyval		IIFVvval
# define	YYSTYPE		IIFVvstype
# define	yychar		IIFVvchar
# define	yyerrflag	IIFVverrflag
# define	yyexca		IIFVvexca
# define	yyact		IIFVvact
# define	yypact		IIFVvpact
# define	yypgo		IIFVvpgo
# define	yyr1		IIFVvr1
# define	yyr2		IIFVvr2
# define	yychk		IIFVvck
# define	yydef		IIFVv1def
# define	yydebug		IIFVv2debug
# define	yynerrs		IIFVvnerrs
# define	yyv		IIFVvv
# define	yypvt		IIFVvpvt
# define	yys		IIFVvs
# define	yy_yyv		IIFVv_yyv
# if defined(hp3_us5) || defined(hp9_mpe) || defined(hp8_us5)
# define	yymaxdepth	IIFVvmaxdepth
# endif /* hp3_us5 || hp9_mpe || hp8_us5 */

#define	LPAREN		257
#define	RPAREN		258
#define	IN		259
#define	PERIOD		260
#define	COMMA		261
#define	LBRAK		262
#define	RBRAK		263
#define	IS		264
#define	NAME		265
#define	SCONST		266
#define	SVCONST		267
#define	I4CONST		268
#define	F8CONST		269
#define	DECCONST		270
#define	LBOP		271
#define	NOT		272
#define	LUOP		273
#define	UMINUS		274
#define	RELOP		275
#define	ANULL		276
#define	LIKE		277
#define	yyclearin	yychar = -1
#define	yyerrok		yyerrflag = 0
extern int		yychar;
extern short		yyerrflag;
#ifndef YYMAXDEPTH
#define	YYMAXDEPTH	150
#endif
YYSTYPE			yylval, yyval;
#define	YYERRCODE	256

#line 420 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"


# include	"tokens.h"

/*
**   YYERROR -- the routine which error routine yacc calls
*/
VOID
yyerror(errmessage)
char	*errmessage;
{
	vl_par_error(ERx("PARSER"), errmessage);
}
short yyexca[] = {
	-1, 1,
	0, -1,
	-2, 0,
};

#define	YYNPROD		41
#define	YYLAST		109

short yyact[] = {
	     3,     4,    16,    18,    19,    59,    23,    20,    17,    15,
	    11,     4,    16,    18,    19,    52,     5,    50,    17,    15,
	    11,    49,    16,    18,    19,    58,     5,    61,    17,    15,
	    11,    36,    16,    18,    19,    42,    34,    33,    17,    16,
	    18,    19,    38,    39,    40,    17,    54,    25,    20,    35,
	    26,    53,    46,    48,    47,    60,    45,    37,    14,    13,
	    12,    10,     7,    29,    28,    27,     2,    24,    32,    31,
	    30,    21,    22,     9,     8,    51,     6,     1,     0,     0,
	     0,     0,     0,     0,     0,    44,    43,    41,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,    57,    56,    55
};

short yypact[] = {
	  -256, -1000,  -264, -1000,  -246,  -246, -1000,  -269,  -215,  -229,
	  -228, -1000, -1000, -1000, -1000,  -203, -1000,  -226, -1000, -1000,
	  -246,  -223, -1000,  -236, -1000, -1000,  -204,  -211,  -207,  -208,
	 -1000, -1000, -1000, -1000,  -255,  -252, -1000,  -214, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,  -219, -1000,  -266,  -241, -1000,
	  -271, -1000, -1000, -1000,  -205, -1000, -1000, -1000, -1000, -1000,
	  -238, -1000
};

short yypgo[] = {
	     0,    77,    60,    59,    58,    66,    76,    62,    61,    75,
	    74,    73,    67,    65,    64,    63
};

short yyr1[] = {
	     0,     1,     1,     5,     5,     5,     5,     6,     6,     6,
	     6,     6,     6,    10,    11,     7,     7,     7,     7,     7,
	     9,     8,     8,    12,    12,    13,    13,    14,    14,    14,
	    14,    14,    14,     2,     2,     3,     3,     4,     4,    15,
	    15
};

short yyr2[] = {
	     0,     1,     1,     3,     3,     2,     1,     3,     2,     3,
	     3,     4,     3,     2,     2,     1,     1,     1,     1,     1,
	     1,     1,     3,     3,     5,     1,     1,     1,     1,     1,
	     3,     3,     3,     1,     2,     1,     2,     1,     2,     1,
	     3
};

short yychk[] = {
	 -4000,    -1,    -5,   256,   257,   272,    -6,    -7,   -10,   -11,
	    -8,   266,    -2,    -3,    -4,   265,   268,   274,   269,   270,
	   271,    -5,    -5,   275,   -12,   262,   265,   -13,   -14,   -15,
	    -2,    -3,    -4,   266,   264,   277,   259,   260,   268,   269,
	   270,    -5,   258,    -7,    -8,   260,   263,   261,   261,   276,
	   272,    -9,   267,   265,   265,    -2,    -3,    -4,   266,   276,
	   260,   265
};

short yydef[] = {
	     0,    -2,     1,     2,     0,     0,     6,     0,     0,     0,
	    15,    16,    17,    18,    19,    21,    33,     0,    35,    37,
	     0,     0,     5,     0,     8,    14,     0,     0,    25,    26,
	    27,    28,    29,    39,     0,     0,    13,     0,    34,    36,
	    38,     4,     3,     7,    15,     0,     9,     0,     0,    10,
	     0,    12,    20,    22,    23,    30,    31,    32,    40,    11,
	     0,    24
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
#line 190 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		vl_curtype->ftvalchk = yypvt[-0].tree_type;
	}
		break;
	case 2:
#line 194 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		vl_curtype->ftvalchk = NULL;
		vl_par_error(ERx("PARSER"), ERget(E_FV001E_syntax_error));
	}
		break;
	case 3:
#line 201 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.tree_type = yypvt[-1].tree_type;
	}
		break;
	case 4:
#line 205 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.tree_type = vl_maketree((YYSTYPE *)(&yypvt[-1].type_type), vBOP, vLOGICAL, (VTREE *)yypvt[-2].tree_type, (VTREE *)yypvt[-0].tree_type);
	}
		break;
	case 5:
#line 209 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.tree_type = vl_maketree((YYSTYPE *)(&yypvt[-1].type_type), vUOP, vLOGICAL, (VTREE *)NULL, (VTREE *)yypvt[-0].tree_type);
	}
		break;
	case 6:
#line 213 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.tree_type = yypvt[-0].tree_type;
	}
		break;
	case 7:
#line 219 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.tree_type = vl_maketree((YYSTYPE *)(&yypvt[-1].type_type), vBOP, vLOGICAL, (VTREE *)yypvt[-2].tree_type, (VTREE *)yypvt[-0].tree_type);
	}
		break;
	case 8:
#line 223 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		/*
		**  Turn off delim id scanning since we've successfully
		**  handle a table lookup statement.
		*/
		IIFVsdsScanDelimString(FALSE);
		yyval.tree_type = vl_maketree((YYSTYPE *)NULL, vLIST, vLOGICAL, (VTREE *)yypvt[-1].tree_type, (VTREE *)yypvt[-0].root_type);
	}
		break;
	case 9:
#line 232 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.tree_type = vl_maketree((YYSTYPE *)NULL, vLIST, vLOGICAL, (VTREE *)yypvt[-2].tree_type, (VTREE *)yypvt[-1].root_type);
	}
		break;
	case 10:
#line 236 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.tree_type = vl_maketree((YYSTYPE *)(&yypvt[-0].type_type), vUOP, vLOGICAL,
			(VTREE *) NULL, (VTREE *) yypvt[-2].tree_type);
	}
		break;
	case 11:
#line 241 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yypvt[-0].type_type = v_opNOTNL;
		yyval.tree_type = vl_maketree((YYSTYPE *)(&yypvt[-0].type_type), vUOP, vLOGICAL,
			(VTREE *) NULL, (VTREE *) yypvt[-3].tree_type);
	}
		break;
	case 12:
#line 247 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.tree_type = vl_maketree((YYSTYPE *)(&yypvt[-1].type_type), vBOP, vLOGICAL,
			(VTREE *) yypvt[-2].tree_type, (VTREE *) yypvt[-0].tree_type);
	}
		break;
	case 13:
#line 254 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		/*
		**  Turn on delimited id scanning in the scanner since
		**  we may be dealing with a table lookup validation.
		**  syntax.  This is necesssary to differentiate between
		**  scanning for a double quoted string constant with
		**  quel escapes versus delimited escapes.
		*/
		IIFVsdsScanDelimString(TRUE);
		yyval.tree_type = yypvt[-1].tree_type;
	}
		break;
	case 14:
#line 268 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		/*
		**  Turn off delimited id scanning since we are now
		**  handling a specific list as part of the validation
		**  string.
		*/
		IIFVsdsScanDelimString(FALSE);
		yyval.tree_type = yypvt[-1].tree_type;
	}
		break;
	case 15:
#line 280 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.tree_type = yypvt[-0].tree_type;
	}
		break;
	case 16:
#line 284 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.tree_type = vl_maketree((YYSTYPE *)(&yypvt[-0].string_type), vCONST, vSTRING, (VTREE *)NULL, (VTREE *)NULL);
	}
		break;
	case 17:
#line 288 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.tree_type = vl_maketree((YYSTYPE *)(&yypvt[-0].I4_type), vCONST, vINT, (VTREE *)NULL, (VTREE *)NULL);
	}
		break;
	case 18:
#line 292 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.tree_type = vl_maketree((YYSTYPE *)(&yypvt[-0].F8_type), vCONST, vFLOAT, (VTREE *)NULL, (VTREE *)NULL);
	}
		break;
	case 19:
#line 296 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.tree_type = vl_maketree((YYSTYPE *)(&yypvt[-0].dec_type), vCONST, vDEC, (VTREE *)NULL, (VTREE *)NULL);
	}
		break;
	case 20:
#line 302 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.tree_type = vl_maketree((YYSTYPE *)(&yypvt[-0].string_type), vCONST, vVCHSTRING,
			(VTREE *) NULL, (VTREE *) NULL);
	}
		break;
	case 21:
#line 311 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.tree_type = vl_mkattrtree(yypvt[-0].name_type, vATTR, (FIELD **)NULL);
	}
		break;
	case 22:
#line 315 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.tree_type = vl_mkcoltree(yypvt[-2].name_type, yypvt[-0].name_type);
	}
		break;
	case 23:
#line 321 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		/*
		**  Calling vl_getlist() to make sure that the names
		**  are valid.  vl_getlist() will return immediately
		**  after checking the names if vl_syntax is TRUE.
		*/
		yyval.root_type = vl_getlist(yypvt[-2].name_type, yypvt[-0].name_type, NULL);
	}
		break;
	case 24:
#line 330 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		/*
		**  Calling vl_getlist() to make sure that the names
		**  are valid.  vl_getlist() will return immediately
		**  after checking the names if vl_syntax is TRUE.
		*/
		yyval.root_type = vl_getlist(yypvt[-2].name_type, yypvt[-0].name_type, yypvt[-4].name_type);
	}
		break;
	case 25:
#line 341 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.root_type = yypvt[-0].root_type;
	}
		break;
	case 26:
#line 345 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.root_type = yypvt[-0].root_type;
	}
		break;
	case 27:
#line 350 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.root_type = vl_mklist(vINT);
		vl_addlist((YYSTYPE *)(&yypvt[-0].I4_type), vINT);
	}
		break;
	case 28:
#line 355 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.root_type = vl_mklist(vFLOAT);
		vl_addlist((YYSTYPE *)(&yypvt[-0].F8_type), vFLOAT);
	}
		break;
	case 29:
#line 360 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.root_type = vl_mklist(vDEC);
		vl_addlist((YYSTYPE *)(&yypvt[-0].dec_type), vDEC);
	}
		break;
	case 30:
#line 365 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		vl_addlist((YYSTYPE *)(&yypvt[-0].I4_type), vINT);
	}
		break;
	case 31:
#line 369 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		vl_addlist((YYSTYPE *)(&yypvt[-0].F8_type), vFLOAT);
	}
		break;
	case 32:
#line 373 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		vl_addlist((YYSTYPE *)(&yypvt[-0].dec_type), vDEC);
	}
		break;
	case 33:
#line 379 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.I4_type = yypvt[-0].I4_type;
	}
		break;
	case 34:
#line 383 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yypvt[-0].I4_type = - (yypvt[-0].I4_type);
		yyval.I4_type = yypvt[-0].I4_type;
	}
		break;
	case 35:
#line 390 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.F8_type = yypvt[-0].F8_type;
	}
		break;
	case 36:
#line 394 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yypvt[-0].F8_type = - (yypvt[-0].F8_type);
		yyval.F8_type = yypvt[-0].F8_type;
	}
		break;
	case 37:
#line 401 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.dec_type = yypvt[-0].dec_type;
	}
		break;
	case 38:
#line 405 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		IIFVndNegateDecimal(yypvt[-0].dec_type, (i4) TRUE);
		yyval.dec_type = yypvt[-0].dec_type;
	}
		break;
	case 39:
#line 411 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		yyval.root_type = vl_mklist(vSTRING);
		vl_addlist((YYSTYPE *)(&yypvt[-0].string_type), vSTRING);
	}
		break;
	case 40:
#line 416 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/frame/valid/grammar.yf"
{
		vl_addlist((YYSTYPE *)(&yypvt[-0].string_type), vSTRING);
	}
		break;
		}
		goto yystack;  /* stack new state and value */

	}
