
#line 60 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
# include <compat.h>
# include <si.h>
# include <me.h>
# include <st.h>
# include <cm.h>
# include <er.h>
# include <lo.h>
# include <pm.h>
# include "cr.h"

/*
** yacc on usl_us5 is about to spit out a #include for <stdlib.h>
** which has a prototyped forward reference for abs(int).
** Because we define abs as a macro in compat.h, this forward
** ref gets macroexpanded into "extern int ((int) > 0? (int) : -(int))"
** which is clearly garbage, and won't compile. So stomp on the macro
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

STATUS IIUGhfHtabFind();
STATUS IIUGheHtabEnter();

GLOBALREF PTR tab;	/* hash table */

/*
** Converts a PATTERN to a SYMBOL by replacing instances of "%" with "$". 
*/
static char *
pattern_to_symbol( char *pattern )
{
	char *c, *symbol = STalloc( pattern );
	
	for( c = symbol; *c != EOS; CMnext( c ) )
	{
		if( *c == '%' )
			*c = '$';
	}
	return( symbol );
}



#line 113 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
typedef union 
{
	f8	real;
	char	*string;
	i4	integer;
	CR_NODE	*node;
} YYSTYPE;
#define	STR_CON		257
#define	EXEC_TEXT		258
#define	EXEC_TEXT_STR		259
#define	SYMBOL		260
#define	PATTERN		261
#define	COMPARE_OP		262
#define	LOGIC_OP		263
#define	NUM_CON		264
#define	BOOL_CON		265
#define	REQUIRES		266
#define	VALID		267
#define	MIN		268
#define	MAX		269
#define	PRIME		270
#define	POWER2		271
#define	IF		272
#define	ELSE		273
#define	ELSEIF		274
#define	ENDIF		275
#define	UNDEFINED		276
#define	DIRECTORY		277
#define	FILESPEC		278
#define	SIGNED_INT		279
#define	DECIMAL		280
#define	SIZETYPE		281
#define	SUM		282
#define	yyclearin	yychar = -1
#define	yyerrok		yyerrflag = 0
extern int		yychar;
extern short		yyerrflag;
#ifndef YYMAXDEPTH
#define	YYMAXDEPTH	150
#endif
YYSTYPE			yylval, yyval;
#define	YYERRCODE	256

#line 595 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"


short yyexca[] = {
	-1, 1,
	0, -1,
	-2, 0,
};

#define	YYNPROD		43
#define	YYLAST		272

short yyact[] = {
	    18,    48,    42,    40,    41,    43,    44,    70,    71,    31,
	    55,    78,    49,    50,    45,    46,    47,    18,    60,    61,
	    38,    81,    64,     2,    68,    69,    77,     5,    51,    18,
	     6,    59,    58,    57,    30,    20,     3,    25,    29,    37,
	    56,    23,    26,    24,    23,    75,    24,    28,    22,     9,
	    15,     1,     4,    32,    21,    39,    72,     0,     0,     0,
	    14,     0,    32,     0,     0,     0,    54,     0,    27,     0,
	     0,     0,     0,     0,     0,     0,    35,    36,     0,    62,
	     0,    53,     0,    32,    33,    34,    65,    63,    66,    67,
	     0,     0,     0,     0,     0,     0,    73,     0,    74,     0,
	     0,     0,     0,     0,     0,    80,    32,    76,    83,    82,
	    79,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,    10,    12,    13,
	    16,     0,     0,     0,    17,    11,    10,    12,    13,    16,
	     0,     0,     7,    17,    11,     0,     8,    16,     0,     0,
	     0,    17,    19,    52,    52,     8,    10,    12,    13,    16,
	    52,    19,     0,    17,    11,     0,     0,     0,     0,    19,
	     0,     0,     0,     0,     0,     8,     0,     0,     0,     0,
	     0,    19
};

short yypact[] = {
	 -1000,  -237,   -22,   -40,   -24, -1000, -1000,     8, -1000,     1,
	 -1000, -1000, -1000, -1000,    -5, -1000, -1000, -1000,   -23,     7,
	 -1000,    -6,   -31,   -23,   -23,   -23,   -23,    -2,  -241,  -265,
	   -13,   -31,  -252,    -5,    -5, -1000, -1000, -1000,    -1, -1000,
	   -28,   -29,   -30, -1000, -1000, -1000, -1000, -1000,  -242, -1000,
	 -1000,   -40,   -31,    -2,   -19,   -11, -1000,   -11,   -11,   -99,
	 -1000, -1000,  -266, -1000, -1000, -1000, -1000, -1000,   -40, -1000,
	   -40,     5,   -18, -1000,  -264,   -31, -1000,   -40, -1000,   -20,
	 -1000,   -40,  -266, -1000
};

short yypgo[] = {
	     0,    27,    25,    30,    49,    60,    50,    34,    56,    55,
	    54,    52,    51
};

short yyr1[] = {
	     0,    12,    12,    11,     1,     1,     2,     2,     3,     3,
	     3,     3,     3,     3,     4,     4,     4,     5,     5,     5,
	     6,     6,     6,     6,     7,     7,     7,    10,    10,     9,
	     9,     9,     9,     9,     9,     9,     9,     9,     9,     9,
	     9,     8,     8
};

short yyr2[] = {
	     0,     0,     5,     2,     1,     6,     3,     6,     1,     1,
	     1,     1,     1,     1,     1,     3,     3,     1,     3,     3,
	     1,     1,     3,     4,     3,     3,     3,     0,     3,     3,
	     3,     5,     1,     1,     1,     1,     1,     2,     2,     1,
	     1,     1,     3
};

short yychk[] = {
	 -4000,   -12,   260,    58,   -11,    -1,    -3,   272,   276,    -4,
	   257,   265,   258,   259,    -5,    -6,   260,   264,    40,   282,
	    59,   -10,    40,    43,    45,    42,    47,    -4,    40,    44,
	    -7,    40,    -3,    -5,    -5,    -6,    -6,    41,   261,    -9,
	   268,   269,   267,   270,   271,   279,   280,   281,   266,   277,
	   278,    41,   263,    -4,    -7,   262,    41,    61,    61,    61,
	   260,   261,    -1,    -7,    41,    -3,    -3,    -3,   123,    -2,
	   273,   274,    -8,    -1,    -1,    40,   125,    44,   275,    -7,
	    -1,    41,    -1,    -2
};

short yydef[] = {
	     1,    -2,     0,     0,     0,    27,     4,     0,     8,     9,
	    10,    11,    12,    13,    14,    17,    20,    21,     0,     0,
	     2,     3,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,    15,    16,    18,    19,    22,     0,    28,
	     0,     0,     0,    32,    33,    34,    35,    36,     0,    39,
	    40,     0,     0,     9,     0,     0,    23,     0,     0,     0,
	    37,    38,     0,    25,    24,    26,    29,    30,     0,     5,
	     0,     0,     0,    41,     0,     0,    31,     0,     6,     0,
	    42,     0,     0,     7
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
			
	case 2:
#line 175 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *s;

		/* check hash table for previously referenced symbol */
		if( IIUGhfHtabFind( tab, yypvt[-3].string, (PTR *) &s ) == FAIL )
		{
			/* no symbol found - create hash table entry */
			s = CRnode();
			s->symbol = yypvt[-3].string;
			if( IIUGheHtabEnter( tab, yypvt[-3].string, (PTR) s ) == FAIL )
			{
				SIfprintf( stderr,
					ERx( "\n\nIIUGheHtabEnter failed.\n\n" ) );
				PCexit( FAIL );
			}
		}
		/* copy predicate to hash table entry */
		if( yypvt[-1].node != NULL )
			MEcopy( (PTR) yypvt[-1].node, sizeof( CR_NODE ), (PTR) s ); 
		/* restore symbol name (not contained in predicate) */
		s->symbol = yypvt[-3].string;
	}
		break;
	case 3:
#line 202 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		yypvt[-1].node->constraint= yypvt[-0].node;
		yyval.node = yypvt[-1].node;
	}
		break;
	case 4:
#line 210 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{ yyval.node = yypvt[-0].node; }
		break;
	case 5:
#line 213 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->type = ND_COND1;
		n->child0 = yypvt[-3].node;
		n->child1 = CRnode();
		n->child1->type = ND_COND2;
		n->child1->child0 = yypvt[-1].node;
		n->child1->child1 = yypvt[-0].node;
		yyval.node = n;
	}
		break;
	case 6:
#line 228 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{ yyval.node = yypvt[-1].node; }
		break;
	case 7:
#line 231 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->type = ND_COND1;
		n->child0 = yypvt[-3].node;
		n->child1 = CRnode();
		n->child1->type = ND_COND2;
		n->child1->child0 = yypvt[-1].node;
		n->child1->child1 = yypvt[-0].node;
		yyval.node = n;
	}
		break;
	case 8:
#line 247 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->type = ND_UNDEFINED;
		yyval.node = n;
	}
		break;
	case 9:
#line 254 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{ yyval.node = yypvt[-0].node; }
		break;
	case 10:
#line 257 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->type = ND_STR_VAL;
		n->str_val = yypvt[-0].string;
		yyval.node = n;
	}
		break;
	case 11:
#line 266 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->type = ND_BOOL_VAL;
		n->num_val = yypvt[-0].real;
		yyval.node = n;
	}
		break;
	case 12:
#line 275 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->type = ND_EXEC_TEXT;
		n->str_val = yypvt[-0].string;
		yyval.node = n;
	}
		break;
	case 13:
#line 284 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->type = ND_EXEC_TEXT_STR;
		n->str_val = yypvt[-0].string;
		yyval.node = n;
	}
		break;
	case 14:
#line 295 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{ yyval.node = yypvt[-0].node; }
		break;
	case 15:
#line 298 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->type = ND_MATH_OP;

		n->str_val = yypvt[-1].string;
		n->child0 = yypvt[-2].node; 
		n->child1 = yypvt[-0].node;
		yyval.node = n;
	}
		break;
	case 16:
#line 310 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->type = ND_MATH_OP;

		n->str_val = yypvt[-1].string;
		n->child0 = yypvt[-2].node; 
		n->child1 = yypvt[-0].node;
		yyval.node = n;
	}
		break;
	case 17:
#line 324 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{ yyval.node = yypvt[-0].node; }
		break;
	case 18:
#line 327 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->type = ND_MATH_OP;

		n->str_val = yypvt[-1].string;
		n->child0 = yypvt[-2].node; 
		n->child1 = yypvt[-0].node;
		yyval.node = n;
	}
		break;
	case 19:
#line 339 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->type = ND_MATH_OP;

		n->str_val = yypvt[-1].string;
		n->child0 = yypvt[-2].node; 
		n->child1 = yypvt[-0].node;
		yyval.node = n;
	}
		break;
	case 20:
#line 354 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *s;
		CR_NODE *n = CRnode();	

		/* check hash table for already referenced symbol */
		if( IIUGhfHtabFind( tab, yypvt[-0].string, (PTR *) &s ) == FAIL )
		{
			/* no symbol found - forward reference */
			s = CRnode();
			s->symbol = yypvt[-0].string;
			if( IIUGheHtabEnter( tab, yypvt[-0].string, (PTR) s ) == FAIL )
			{
				SIfprintf( stderr,
					ERx( "\n\nIIUGheHtabEnter failed.\n\n" ) );
				PCexit( FAIL );
			}
		}
		n->type = ND_SYM_VAL;
		n->sym_val = s;
		n->sym_val->reference = TRUE;
		yyval.node = n;
	}
		break;
	case 21:
#line 378 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->type = ND_NUM_VAL;
		n->num_val = yypvt[-0].real;
		yyval.node = n;
	}
		break;
	case 22:
#line 387 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{ yyval.node = yypvt[-1].node; }
		break;
	case 23:
#line 390 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *s;
		CR_NODE *n = CRnode();

		switch( yypvt[-3].integer )
		{
			case SUM:
			{
				CR_NODE *s;
				char *p = pattern_to_symbol( yypvt[-1].string );

				/*
				** If the pattern/symbol is not found,
				** don't create it in this case, since
				** the SUM() function doesn't imply
				** existence. 
				*/
				if( IIUGhfHtabFind( tab, p, (PTR *) &s )
					== OK )
				{
					s->reference = TRUE;
				}
				else
					MEfree( p );
				n->reference = TRUE;
				n->type = ND_SUM; 
				n->str_val = yypvt[-1].string;
				break;
			}
		}
		n->symbol = yypvt[-1].string;
		yyval.node = n;
	}
		break;
	case 24:
#line 427 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{ yyval.node = yypvt[-1].node; }
		break;
	case 25:
#line 430 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->type = ND_LOGIC_OP;	
		n->str_val = yypvt[-1].string;
		n->child0 = yypvt[-2].node;
		n->child1 = yypvt[-0].node;
		yyval.node = n;
	}
		break;
	case 26:
#line 441 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->type = ND_COMPARE_OP;	
		n->str_val = yypvt[-1].string;
		n->child0 = yypvt[-2].node;
		n->child1 = yypvt[-0].node;
		yyval.node = n;
	}
		break;
	case 27:
#line 454 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{ yyval.node = NULL; }
		break;
	case 28:
#line 457 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{		
		yypvt[-0].node->constraint = yypvt[-2].node;
		yyval.node = yypvt[-0].node;
	}
		break;
	case 29:
#line 466 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->type = ND_MIN;
		n->child0 = yypvt[-0].node;
		yyval.node = n;
	}
		break;
	case 30:
#line 475 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->type = ND_MAX;
		n->child0 = yypvt[-0].node;
		yyval.node = n;
	}
		break;
	case 31:
#line 484 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->type = ND_SET_MEMBER;
		n->list = yypvt[-1].node;
		yyval.node = n;
	}
		break;
	case 32:
#line 493 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->type = ND_PRIME;
		yyval.node = n;
	}
		break;
	case 33:
#line 501 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->type = ND_POWER2;
		yyval.node = n;
	}
		break;
	case 34:
#line 509 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->type = ND_SIGNED_INT;
		yyval.node = n;
	}
		break;
	case 35:
#line 517 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->type = ND_DECIMAL;
		yyval.node = n;
	}
		break;
	case 36:
#line 525 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
                CR_NODE *n = CRnode();

                n->type = ND_SIZETYPE;
                yyval.node = n;
        }
		break;
	case 37:
#line 533 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->reference = TRUE;
		n->type = ND_REQUIRES;
		n->str_val = yypvt[-0].string;
		yyval.node = n;
	}
		break;
	case 38:
#line 543 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode(), *s;
		char *p = pattern_to_symbol( yypvt[-0].string );

		/*
		** If the pattern/symbol is not found,
		** don't create it in this case, since
		** the SUM() function doesn't imply
		** existence. 
		*/
		if( IIUGhfHtabFind( tab, p, (PTR *) &s )
			== OK )
		{
			s->reference = TRUE;
		}
		else
			MEfree( p );
		n->reference = TRUE;
		n->type = ND_REQUIRES;
		n->str_val = yypvt[-0].string;
		yyval.node = n;
	}
		break;
	case 39:
#line 567 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->type = ND_DIRECTORY;
		yyval.node = n;
	}
		break;
	case 40:
#line 575 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		CR_NODE *n = CRnode();

		n->type = ND_FILESPEC;
		yyval.node = n;
	}
		break;
	case 41:
#line 586 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{ yyval.node = yypvt[-0].node; }
		break;
	case 42:
#line 589 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/config/crparse.yf"
{
		yypvt[-0].node->list = yypvt[-2].node;	
		yyval.node = yypvt[-0].node;
	}
		break;
		}
		goto yystack;  /* stack new state and value */

	}
