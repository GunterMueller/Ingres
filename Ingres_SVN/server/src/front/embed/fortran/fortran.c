
#line 6 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
/* %L language - Which language are we using for symbol table? */
/* %L merge <eqgram.my> -- get the master grammar */
/* %L fake begin */
/* %L fake end */
/* %L language begin 
** Define EQ_X_LANG where X is EUC or PL1 for the symbol table
** Also define the real host lang (if different) for gr_mechanism
*/
# define	EQ_EUC_LANG
# define	EQ_FOR_LANG
/* %L language end */

# include 	<compat.h>
# include	<er.h>
# include	<me.h>
# include	<si.h>
# include	<st.h>
# include	<cm.h>
# include	<gl.h>
# include	<sl.h>
# include	<cv.h>
# include	<iicommon.h>
# include	<equel.h>
# include	<eqlang.h>
# include	<equtils.h>
# include	<eqsym.h>
# include	<eqgen.h>
# include	<eqstmt.h>
# include	<eqscan.h>
# include	<eqgr.h>
# include	<eqfw.h>	
# include	<ereq.h>
# include	<eqtgt.h>

# define	gr_xsym	gr_nlsym  /* Overload nlsym for dyn-named EQUEL csrs */

# if defined(hp3_us5)
	# pragma OPT_LEVEL 1
# endif /* hp3_us5 */
#define	tABORT		257
#define	tAPPEND		258
#define	tCOPY		259
#define	tCREATE		260
#define	tDEFINE		261
#define	tDELETE		262
#define	tDESTROY		263
#define	tHELP		264
#define	tINDEX		265
#define	tINTEGRITY		266
#define	tMODIFY		267
#define	tPERMIT		268
#define	tPRINT		269
#define	tREGISTER		270
#define	tRELOCATE		271
#define	tRANGE		272
#define	tREMOVE		273
#define	tREPLACE		274
#define	tRETRIEVE		275
#define	tSAVE		276
#define	tSAVEPOINT		277
#define	tSET		278
#define	tSORT		279
#define	tNULL		280
#define	tUNIQUE		281
#define	tVIEW		282
#define	tWITH		283
#define	tENDLOOP		284
#define	tENDRETRIEVE		285
#define	tEXIT		286
#define	tIISTATEMENT		287
#define	tINQ_EQUEL		288
#define	tINGRES		289
#define	tPARAM		290
#define	tREPEAT		291
#define	tSET_EQUEL		292
#define	tCALL		293
#define	tIIPRINTF		294
#define	tBEG_XACT		295
#define	tEND_XACT		296
#define	tDEF_INTEGRITY		297
#define	tDEF_LINK		298
#define	tDEF_PERMIT		299
#define	tDEF_VIEW		300
#define	tDES_INTEGRITY		301
#define	tDES_PERMIT		302
#define	tDES_LINK		303
#define	tDES_TABLE		304
#define	tDIR_CONNECT		305
#define	tDIR_DISCONNECT		306
#define	tHLP_INTEGRITY		307
#define	tHLP_PERMIT		308
#define	tHLP_VIEW		309
#define	tCLS_CURSOR		310
#define	tDEC_CURSOR		311
#define	tDEL_CURSOR		312
#define	tOPN_CURSOR		313
#define	tREP_CURSOR		314
#define	tRET_CURSOR		315
#define	tFORMS		316
#define	tFORMINIT		317
#define	tADDFORM		318
#define	tFORMDATA		319
#define	tDISPLAY		320
#define	tINITIALIZE		321
#define	tFINALIZE		322
#define	tACTIVATE		323
#define	tPUTFORM		324
#define	tGETFORM		325
#define	tGETOPER		326
#define	tREDISPLAY		327
#define	tMESSAGE		328
#define	tSLEEP		329
#define	tCLEAR		330
#define	tHELPFILE		331
#define	tPROMPT		332
#define	tSUBMENU		333
#define	tVALIDATE		334
#define	tRESUME		335
#define	tENDFORMS		336
#define	tENDDISPLAY		337
#define	tBREAKDISPLAY		338
#define	tENDDATA		339
#define	tSET_FRS		340
#define	tINQ_FRS		341
#define	tPUTOPER		342
#define	tPRINTSCREEN		343
#define	tHELP_FRS		344
#define	tEND		345
#define	tIIPROMPT		346
#define	tIIMESSAGE		347
#define	tCLEARROW		348
#define	tDELETEROW		349
#define	tINSERTROW		350
#define	tGETROW		351
#define	tPUTROW		352
#define	tLOADTABLE		353
#define	tINITTABLE		354
#define	tVALIDROW		355
#define	tUNLOADTABLE		356
#define	tSCROLL		357
#define	tTABLEDATA		358
#define	tPURGETBL		359
#define	tALL		360
#define	tBY		361
#define	tFROM		362
#define	tIN		363
#define	tINTO		364
#define	tIS		365
#define	tOF		366
#define	tON		367
#define	tTO		368
#define	tWHERE		369
#define	tUNTIL		370
#define	tAT		371
#define	tNOTRIM		372
#define	tFOR		373
#define	tAS		374
#define	tFIELD		375
#define	tMENUITEM		376
#define	tCOMMAND		377
#define	tNEXT		378
#define	tSCREEN		379
#define	tUP		380
#define	tDOWN		381
#define	tOUT		382
#define	tCOLUMN		383
#define	tSCROLLUP		384
#define	tSCROLLDOWN		385
#define	tINTCONST		386
#define	tFLTCONST		387
#define	tNAME		388
#define	tSCONST		389
#define	tHOSTCODE		390
#define	tINCLUDE		391
#define	tQUOTE		392
#define	tCOMMENT		393
#define	tTERMINATE		394
#define	tWHITESPACE		395
#define	tHEXCONST		396
#define	tDECCONST		397
#define	tCOMMA		398
#define	tLPAREN		399
#define	tRPAREN		400
#define	tPERIOD		401
#define	tDEREF		402
#define	tCOLON		403
#define	tATSIGN		404
#define	tLBRACE		405
#define	tRBRACE		406
#define	tFCHAR		407
#define	tFDECLARE		408
#define	tFDECNAME		409
#define	tFCOMPLEX		410
#define	tFDOUBLE		411
#define	tFEXTERN		412
#define	tFFUNC		413
#define	tFMAP		414
#define	tFPARAM		415
#define	tFPRECISION		416
#define	tFPROC		417
#define	tFPROGRAM		418
#define	tFRECORD		419
#define	tFSEMI		420
#define	tFSTRUCT		421
#define	tFSTRCT_VAR		422
#define	tFTAG		423
#define	tFTYPE		424
#define	tFUNION		425
#define	tARUOP		426
#define	tBNDOP		427
#define	tEQOP		428
#define	tNEQOP		429
#define	tLOGUOP		430
#define	tLOGBOP		431
#define	tAGOP		432
#define	tMULOP		433
#define	tDIVOP		434
#define	tEXPOP		435
#define	tBINARYOP		436
#define	tUNARYOP		437

#line 329 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
/* %L locals - Include L locals and gr structure */
/* %L locals begin - Local variables and gr structure for C */
# include	<eqf.h>
# include	<ere1.h>

struct	gr_state {
	/* Members known to both G and L */
	SYM	*gr_sym;
	char	*gr_id;		/* Last full name used */
	i4	gr_type;
	i4	gr_flag;	/* Certain state information */
	i4	gr_func;	/* I/O Function constant */
	i4	gr_mode;	/* Mode of statement before entering block */
	i4	gr_adjective;	/* Block modifier */
	SYM	*gr_nlsym;	/* Symtab pointer to indicator var */
	char	*gr_nlid;	/* Id of indicator var */
	i4	gr_nltype;	/* Type of indicator var */
	SYM	*gr_null;	/* Generic null symbol (for "= null" assgnmts */

	/* Add L (FORTRAN) dependent members after this statement */
	i4	F_blk;		/* F block level */
	i4	F_rec;		/* F record level */
	i4	F_storage;	/* extern, static, auto, register, typedef */
	i4	F_size;		/* Size of declaration */
	i4	F_indir;	/* Indirection of variables */
	i4	F_dec;		/* In declaration or usage */
	i4	F_isfunc;	/* Is function declaration */
	i4	F_dims;		/* Number of dimensions of variable */
	i4	F_intval;	/* Value of last integer const */
	i4	F_usedec;	/* TRUE iff ## declare already */
	i4	F_intsize;	/* 2 or 4; the size of an integer */
	SYM	*F_sym;		/* Current (scanner) name-symtab ptr: for '(' */
	SYM	*F_struct;	/* Special pointer for references to structs */
};
GLOBALDEF struct gr_state _gr_local ZERO_FILL;
GLOBALDEF struct gr_state *gr = &_gr_local;

/* Make declaring names easy */
# define	F_DECLARE( name, dims, size )\
	F_declare( name, dims, size, gr->gr_type, gr->F_struct,\
	    gr->F_rec, gr->F_blk )

/* %L locals end */

#define	yyclearin	yychar = -1
#define	yyerrok		yyerrflag = 0
extern int		yychar;
extern short		yyerrflag;
#ifndef YYMAXDEPTH
#define	YYMAXDEPTH	150
#endif
#ifndef YYSTYPE
#define	YYSTYPE		int
#endif
YYSTYPE			yylval, yyval;
#define	YYERRCODE	256

#line 7548 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"


/* %L mechanism */
/* %L mechanism begin - All the stuff to do with gr_mechanism() */

GLOBALDEF GR_TYPES   gr_typetab[] = {
    { ERx("byte"),		T_INT,		1 },
    { ERx("character"),		T_CHAR,		1 },
    { ERx("complex"),		T_FLOAT,	8 },
    { ERx("dbl precision"),	T_FLOAT,	8 },
    { ERx("dbl complex"),	T_FLOAT,	16 },
    { ERx("integer"),		T_INT,		4 },
    { ERx("logical"),		T_INT,		4 },
    { ERx("real"),		T_FLOAT,	4 },
    { (char *)0,  		0,		0 }
};

/*
** Gr_mechanism - Language dependent utility called from within the grammar
**		  or from outside.
**
** Parameters:	flag       - Which mechanism to choose.
**		arg1, arg2 - Whatever is needed for the particular flag.
** Returns:	None
*/

void
gr_mechanism( flag, arg1, arg2 )
i4	flag;
PTR	arg1;			/* Extra arguments for particular cases */
PTR	arg2;
{
    register SYM	*sy;
    i4			use_mask, ret_val;
    static   i4	einit = 0;
    i4			yyequlex();

    switch (flag)
    {
      case GR_EQINIT:
	eq->eq_lang = EQ_FORTRAN;
	eq->eq_def_in = ERx("qf");
# ifdef UNIX
	eq->eq_def_out = ERx("f");
# endif	/* UNIX or NT  */
# ifdef hp9_mpe	
	eq->eq_def_out = ERx("for");
# endif	/*  hp9_mpe */
# ifdef	VMS
	eq->eq_def_out = ERx("for");
# endif	/* VMS */
# ifdef	NT_GENERIC
	eq->eq_def_out = ERx("for");
# endif	/* NT_GENERIC */
# ifdef	CMS
	eq->eq_def_out = ERx("fortran");
# endif	/* CMS */
	if (!einit)
	{
	    eq->eq_in_ext = ERx("qf");
# ifdef UNIX
	    eq->eq_out_ext = ERx("f");
# endif	/* UNIX or NT */
# ifdef hp9_mpe	
	    eq->eq_out_ext = ERx("for");
# endif	/*  hp9_mpe */
# ifdef	VMS
	    eq->eq_out_ext = ERx("for");
# endif	/* VMS */ 
# ifdef	NT_GENERIC
	    eq->eq_out_ext = ERx("for");
# endif	/* NT_GENERIC */ 
# ifdef	CMS
	    eq->eq_out_ext = ERx("copy");
# endif	/* CMS */
	    einit = 1;
	}

	gr->gr_type = T_NONE;
	gr->gr_sym = (SYM *)0;
	gr->gr_id = ERx("");
	gr->gr_flag = GR_HOSTCODE;

      /* tell the world we're Embedded QUEL! */
	dml->dm_lex = yyequlex;

	gr->F_dec = GR_F_DEC;
	gr->F_blk = 1;
	gr->F_rec = 0;
	gr->F_storage = T_NONE;
	gr->F_size = 0;
	gr->F_indir = 0;
	gr->F_dims = 0;
	gr->F_struct = (SYM *)0;
	gr->F_isfunc = 0;
	gr->F_usedec = FDEC_NONE;

	break;

      case GR_SYMINIT:
	{
	    /* Enter declarations into Equel symbol table */
	    GR_TYPES	*gt;

	    sym_init( (bool)TRUE );
	    gen_init();
	    for (gt=gr_typetab; gt->gt_id; gt++)
	    {
		sy = symDcEuc(gt->gt_id, 0, gr->F_blk,
			(syFisTYPE|syFisSYS), F_CLOSURE, SY_NORMAL);
		sym_s_btype( sy, gt->gt_rep );
		sym_s_dsize( sy, gt->gt_len );
		sym_s_indir( sy, 0 );
	    }

	  /* remember the size of an integer */
	    sy = sym_resolve( (SYM *)0, ERx("integer"), F_CLOSURE, syFisTYPE );
	    if (sy)
		gr->F_intsize = sym_g_dsize( sy );

	  /* declare "generic null" as a symbol */
	    sy = symDcEuc( ERx(".null"), 0, gr->F_blk, syFisVAR|syFisSYS, 
			  F_CLOSURE, SY_NORMAL );
	    sym_s_btype( sy, T_NUL );
	    sym_s_dsize( sy, sizeof(i4) );
	    gr->gr_null = sy;

	  /* start a scope in case user doesn't ## program units */
	    sym_s_begin( F_CLOSURE );
	    gr->F_blk++;

	}
	break;

      case GR_LOOKUP:
	/* How should we look up this name? */
	if (gr->F_dec == GR_F_USE)		/* Variable usage */
	{
	    /* 
	    ** If we have a period between name references then it may be the
	    ** C period or the Ingres period.  To resolve this we use the global
	    ** gr->F_struct and see if the specified name is a field (child) of
	    ** structure or a regular variable (including a structure parent).
	    */
	    sy = sym_resolve( gr->F_struct, (char *)arg1, 0, 
			      syFisVAR|syFisCONST );

	    /* 
	    ** We cannot set F_struct here as Yacc may look ahead and cause
	    ** it to be reset.
	    */
	    if (sy && sym_g_btype(sy) == T_STRUCT)
		*(i4 *)arg2 = tFSTRCT_VAR;
	    else
		*(i4 *)arg2 = tNAME;
	    gr->F_sym = sy;
	} else
	{
	    switch (gr->F_dec)
	    {
	      case GR_F_DEC: 
		use_mask = syFisTYPE;
		ret_val = tFTYPE;
		break;
	      case GR_F_TAG:
		use_mask = syFisTAG;
		ret_val = tFTAG;
		break;
	    }
	    if (sy = sym_resolve((SYM *)0, (char *)arg1, 0, use_mask))
		*(i4 *)arg2 = ret_val;
	    else
		*(i4 *)arg2 = tFDECNAME;
	}
	break;

      case GR_STMTFREE:
	str_reset();
	sym_f_init();		/* clear the sym stack */
	gr->gr_type = T_NONE;
	gr->gr_sym = (SYM *)0;
	gr->F_struct = (SYM *)0;
	gr->F_storage = 0;
	break;

      case GR_CLOSE:
	gr->F_blk--;		/* for the "undeclared" block */
	if (gr->F_blk > 1)
	    er_write( E_E10005_hfUNCLOSED, EQ_ERROR, 1, er_na((gr->F_blk-1)) );
	break;

      case GR_DUMP:
	{
	    register FILE	*f = eq->eq_dumpfile;

	    SIfprintf( f, ERx("GR_DUMP: gr_ \n") );
	    SIfprintf( f,
		       ERx("  sym = 0x%p, id = '%s', type = %d, flag = 0%o\n"),
		       gr->gr_sym, gr->gr_id, gr->gr_type, gr->gr_flag );
	    SIfprintf( f, ERx("  func = %d, mode = 0%o\n"),
		       gr->gr_func, gr->gr_mode, gr->F_dec );
	    SIfprintf( f, ERx("  F_blk = %d, F_rec = %d, F_storage = %d, "),
		       gr->F_blk, gr->F_rec, gr->F_storage );
	    SIfprintf( f, ERx("F_size = %d, F_indir = %d\n"), 
		       gr->F_size, gr->F_indir );
	    SIfprintf( f, ERx("  F_dec = %d, F_struct = 0x%p\n"),
		       gr->F_dec, gr->F_struct );
	    SIfprintf( f, ERx("  F_isfunc = %d, F_intsize = %d\n"),
			gr->F_isfunc, gr->F_intsize );
	    SIfprintf( f, ERx("  F_usedec = 0x%x\n"),
			gr->F_usedec );
	    SIfprintf( f, ERx("  F_sym = 0x%p\n"), gr->F_sym );
	    SIflush( f );
	}
	break;

      case GR_BLOCK:		/* Enter/exit blocks - no symbol table action */
	if (arg1) /* enter */
	    gr->F_dec = GR_F_USE;	/* On entry always expect declares */
	else		/* exit */
	  /* On Equel block exit reset decl flag - can't be immediate decls */
	    gr->F_dec = GR_F_USE;
	break;
       
      /*
      ** change the length of the type (in arg1) to the given length (in arg2).
      ** for BASIC and FORTRAN.
      */
      case GR_LENFIX:
      {
 	i2	is_int;
	register GR_TYPES *g;
	extern GR_TYPES gr_typetab[];

	is_int = STcompare((char *)arg1, ERx("integer"))==0;
	for (g=gr_typetab; g->gt_id; g++)
	{
	    if (STcompare((char *)arg1,g->gt_id) == 0 ||
		(is_int && STcompare(ERx("logical"),g->gt_id) == 0))
	    {
		g->gt_len = (i2)arg2;
	    }
	}
      }
	break;

      case GR_EQSTMT:
	{
	    i4		mode = (i4)arg1;
	    char	*kword = (char *)arg2;
	    i4		dec_message = FALSE;	/* Already complained? */

	    /* Turn off declarations flag and generate line directive */
	    gr->gr_flag &= ~GR_HOSTCODE;
	    gr->F_dec = GR_F_USE;
	    if (gr->gr_flag & GR_DECLCSR)
		gen_line( kword );
	    else
		gen_eqstmt( G_OPEN, kword );

	    /* Check if ## declare [forms] is required for certain languages */
	    if ((gr->F_usedec & FDEC_DECLARE) == 0)
	    {
		er_write( E_E10007_hfDECLARE, EQ_ERROR, 1, kword );
		gr->F_usedec |= FDEC_DECLARE;	/* stop further complaints */
		dec_message = TRUE;
	    }

	    switch (mode)
	    {
	      case GR_sREPEAT:
		gr->gr_flag |= GR_REPEAT;
		rep_begin( kword, NULL );
		/* FALL THROUGH */

	      case GR_sQUEL:
		if (gr->gr_flag & GR_RETRIEVE)
		{
		    gr->gr_flag &= ~GR_RETRIEVE;
		    er_write( E_EQ0063_grNESTQUEL, EQ_ERROR, 1, kword );
		}
		gr->gr_flag |= GR_QUEL;
		db_key( kword );
		break;

	      case GR_sFORMS:
	      /* no forms statement yet? */
		if ((gr->F_usedec & FDEC_FORMS) != FDEC_FORMS)
		{
		    if (!dec_message)
			er_write( E_E10009_hfDECFORMS, EQ_ERROR, 1, kword );
		    dec_message = TRUE;
		    gr->F_usedec |= FDEC_FORMS;	   /* stop further complaints */
		}
		/* FALL THROUGH! */

	      case GR_sLIBQ:
		gr->gr_flag |= GR_EQUEL;
		break;

	      case GR_sCURSOR:
		if (gr->gr_flag & GR_RETRIEVE)
		{
		    gr->gr_flag &= ~GR_RETRIEVE;
		    er_write( E_EQ0063_grNESTQUEL, EQ_ERROR, 1, kword );
		}
		break;
	    }
	}
	break;

      default:
	break;
    }
}
/* %L mechanism end */
short yyexca[] = {
	-1, 1,
	0, -1,
	-2, 0,
	-1, 106,
	388, 1098,
	402, 1098,
	422, 1098,
	-2, 160,
	-1, 124,
	399, 307,
	-2, 292,
	-1, 201,
	388, 1098,
	402, 1098,
	422, 1098,
	-2, 174,
	-1, 212,
	374, 254,
	399, 254,
	-2, 1098,
	-1, 236,
	399, 921,
	-2, 1098,
	-1, 237,
	399, 921,
	-2, 1098,
	-1, 246,
	399, 921,
	-2, 1098,
	-1, 369,
	388, 1098,
	402, 1098,
	422, 1098,
	-2, 281,
	-1, 387,
	388, 1098,
	402, 1098,
	422, 1098,
	-2, 332,
	-1, 406,
	388, 1098,
	402, 1098,
	422, 1098,
	-2, 555,
	-1, 509,
	388, 1098,
	402, 1098,
	422, 1098,
	-2, 588,
	-1, 569,
	400, 108,
	-2, 1098,
	-1, 586,
	428, 423,
	-2, 475,
	-1, 625,
	388, 1098,
	402, 1098,
	422, 1098,
	-2, 333,
	-1, 626,
	388, 1098,
	402, 1098,
	422, 1098,
	-2, 211,
	-1, 652,
	388, 1098,
	402, 1098,
	422, 1098,
	-2, 515,
	-1, 739,
	398, 625,
	403, 625,
	405, 625,
	-2, 1098,
	-1, 751,
	400, 877,
	-2, 874,
	-1, 772,
	399, 674,
	-2, 1098,
	-1, 773,
	399, 761,
	-2, 1098,
	-1, 774,
	388, 1098,
	402, 1098,
	422, 1098,
	-2, 919,
	-1, 775,
	388, 1098,
	402, 1098,
	422, 1098,
	-2, 919,
	-1, 776,
	399, 919,
	-2, 1098,
	-1, 777,
	388, 1098,
	402, 1098,
	422, 1098,
	-2, 588,
	-1, 778,
	388, 1098,
	402, 1098,
	422, 1098,
	-2, 919,
	-1, 780,
	399, 919,
	-2, 1098,
	-1, 782,
	388, 1098,
	402, 1098,
	422, 1098,
	-2, 919,
	-1, 826,
	398, 487,
	400, 487,
	-2, 475,
	-1, 849,
	398, 422,
	-2, 202,
	-1, 889,
	386, 486,
	388, 486,
	402, 486,
	422, 486,
	-2, 487,
	-1, 892,
	388, 1098,
	402, 1098,
	422, 1098,
	-2, 336,
	-1, 954,
	375, 615,
	383, 615,
	398, 625,
	403, 625,
	405, 625,
	-2, 1098,
	-1, 1026,
	345, 989,
	-2, 1049,
	-1, 1084,
	365, 396,
	399, 475,
	401, 475,
	426, 396,
	427, 396,
	428, 396,
	429, 396,
	433, 396,
	434, 396,
	435, 396,
	-2, 487,
	-1, 1128,
	388, 1098,
	402, 1098,
	422, 1098,
	-2, 234,
	-1, 1155,
	388, 1098,
	402, 1098,
	422, 1098,
	-2, 337,
	-1, 1192,
	399, 475,
	401, 475,
	-2, 396,
	-1, 1288,
	388, 1098,
	402, 1098,
	422, 1098,
	-2, 114,
	-1, 1405,
	345, 1048,
	-2, 1049,
	-1, 1552,
	345, 989,
	-2, 1049,
	-1, 1556,
	345, 1048,
	-2, 1049,
};

#define	YYNPROD		1116
#define	YYLAST		2944

short yyact[] = {
	   327,   685,  1407,   571,   593,  1603,  1604,  1528,   579,  1100,
	   336,   939,  1093,  1448,  1035,  1091,   267,  1503,  1406,  1258,
	  1036,   588,  1378,   709,  1442,  1026,   647,   361,  1173,   835,
	  1473,  1358,  1267,   859,   393,  1083,  1029,  1423,     9,  1259,
	  1607,  1262,  1245,   789,   985,  1311,   785,   912,  1184,   332,
	   794,  1022,  1169,   858,   646,  1427,   836,   812,  1082,  1079,
	  1027,   969,  1060,   461,  1066,   891,   786,  1344,   738,   947,
	   935,  1089,   899,   878,  1103,   358,   328,   827,   429,  1095,
	   811,   989,   604,   536,   609,   719,  1274,   534,   752,   574,
	  1032,   381,   185,  1031,  1037,   184,   296,   736,   659,   320,
	   722,   481,     2,   699,   706,   982,   868,   696,   318,  1165,
	   679,   675,   343,   663,   655,   643,   601,   343,   583,   608,
	   345,   854,   813,   555,   809,   801,   533,   723,   320,   769,
	   541,   394,   430,   443,   469,   690,   394,   318,   626,   680,
	   408,   564,   317,   408,   408,  1300,   352,  1304,   394,  1023,
	   539,   772,   582,  1012,   394,   422,   335,   357,   446,   394,
	   656,   468,   411,   447,  1105,   754,   575,   360,   864,  1275,
	   464,   535,   864,  1306,  1307,  1308,  1350,   550,   321,  1111,
	  1548,   634,   339,   787,   394,   339,   864,   341,  1112,  1113,
	  1295,   366,  1085,  1490,  1098,  1217,  1114,   445,  1098,  1110,
	   480,  1306,  1307,  1308,  1098,  1306,  1307,  1308,   535,   841,
	   937,  1306,  1307,  1308,   869,   913,  1109,  1295,  1098,   937,
	   705,   796,  1085,   869,  1108,  1474,  1038,   408,   869,   408,
	   408,  1011,   787,   855,   394,   394,  1025,   394,   394,   401,
	   850,   298,   500,   416,  1410,  1142,   792,   394,   587,   867,
	   616,   501,   502,   394,   394,   394,   394,   394,   394,   394,
	   394,   803,  1141,   394,   465,   462,   338,   452,  1105,   399,
	   386,   654,   491,   492,   805,   463,   787,   870,   914,   483,
	   484,  1272,   321,  1111,   864,   634,   870,  1271,   433,   867,
	  1098,   870,  1112,  1113,   394,   366,   856,  1306,  1307,  1308,
	  1114,   804,   818,  1110,   466,   321,  1111,   477,   634,   432,
	  1098,  1310,   841,  1309,   808,  1112,  1113,  1306,  1307,  1308,
	  1109,   556,  1098,   460,   761,   554,   544,  1550,  1108,  1463,
	   384,   553,  1337,  1180,   686,   581,   567,   542,   882,   586,
	   530,  1199,   572,   918,   580,  1098,   984,   544,  1030,   616,
	  1098,  1310,   841,  1309,   551,   537,   599,  1306,  1307,  1308,
	   465,   650,   575,   395,   864,   465,   462,  1105,   603,  1397,
	  1624,   651,   563,   597,   820,   578,   463,   864,   634,   598,
	  1409,   321,  1111,  1566,   634,   615,   562,  1396,   631,   611,
	   587,  1112,  1113,  1532,   366,   864,  1291,   629,   644,  1114,
	   649,  1531,  1110,   394,  1395,   466,   567,   672,   660,   628,
	   983,  1105,  1585,  1290,  1584,   984,  1489,  1491,  1488,  1109,
	  1391,  1098,  1390,  1399,  1389,   321,  1111,  1108,   634,  1159,
	  1385,  1157,   648,   557,   558,  1112,  1113,  1375,   366,   465,
	   650,   596,   395,  1114,   632,   394,  1110,   784,   882,  1044,
	   651,   346,  1361,   620,   660,   996,   394,   394,   687,  1120,
	  1004,   645,   630,  1109,   687,  1098,   994,   803,  1051,   978,
	  1048,  1108,   958,   394,   957,   394,   974,   964,  1052,   649,
	   751,   992,    95,   394,   682,   930,   395,   464,  1057,   728,
	  1055,  1058,  1054,   395,   991,   394,   394,   766,   921,   304,
	   727,   911,   307,   306,   747,  1056,   666,  1050,   366,   676,
	   771,   297,    96,   298,    95,  1359,   305,  1038,   694,   929,
	   693,   928,   728,   394,   394,   394,   394,   394,   394,   394,
	   394,   395,   394,   727,   698,   614,  1499,   920,  1410,   919,
	   708,   729,   742,   731,    96,   805,   910,   394,   909,   730,
	   905,   637,   904,   395,   725,   726,  1118,   321,  1111,   691,
	   634,   991,   395,   664,   757,  1608,   431,  1112,  1113,   366,
	   826,   826,   444,  1356,   729,  1317,   731,   402,   793,  1216,
	   732,  1227,   730,   783,   362,   589,   594,   725,   726,   815,
	  1071,   810,   721,  1006,   586,   400,   999,   343,   575,   800,
	  1223,  1224,   845,  1222,   791,   981,  1234,  1063,  1269,   746,
	   938,  1225,   923,   732,   915,  1191,   575,   784,  1270,  1236,
	   733,   906,   821,   889,   716,   482,   631,   692,   641,   734,
	   741,   907,   318,   586,   586,   629,   267,   357,   749,   826,
	   760,   394,   846,   664,   872,   824,   831,   494,   875,   758,
	   765,  1238,  1235,   394,   633,   832,   479,   474,     9,  1237,
	  1232,  1233,  1231,   862,   442,  1230,   896,   838,   449,   397,
	   437,   435,   839,   773,   410,  1357,  1228,  1219,  1214,   997,
	   979,   965,   674,   632,   961,   702,   687,  1424,   394,   987,
	   853,   795,  1107,   518,  1380,  1381,   359,   863,   887,   802,
	   630,   519,   394,   817,   902,   916,   517,   516,  1496,   666,
	   682,  1495,   185,   922,   897,   184,   296,   447,   986,   394,
	   339,   395,   931,   634,   884,   634,   941,  1383,  1384,   972,
	   973,   822,   840,   932,   934,   660,   365,  1164,   988,   770,
	   993,   949,   952,   852,   960,   959,   848,   407,   962,   950,
	   951,   445,  1250,   954,   967,   866,   395,   412,  1009,   879,
	   953,  1008,   975,  1249,   890,   888,   392,   933,  1206,  1207,
	   417,   980,   344,  1010,  1010,   464,   464,   464,   771,   464,
	   874,   464,   322,   464,   956,   894,   895,   666,   936,  1170,
	  1167,   903,  1579,   510,   326,   329,   634,   333,   334,   333,
	   395,   998,   321,   395,   589,   634,   635,   349,   350,   421,
	   333,  1444,   363,  1563,   367,   369,   373,  1247,  1248,   385,
	  1538,   387,   388,   321,   691,   945,   634,  1523,   455,   321,
	  1020,   454,   319,   737,   395,   966,   456,  1084,   395,   581,
	   586,   458,  1522,  1170,  1084,   739,   995,  1115,   580,   395,
	   976,  1439,  1073,   700,   927,   861,   715,   451,   359,   893,
	  1134,   321,  1609,  1200,  1551,  1062,   395,  1063,   733,  1132,
	   488,  1201,   764,   476,  1068,   472,   420,   375,  1039,  1074,
	   419,   606,   489,   490,   321,  1077,  1483,   671,   405,   581,
	  1105,  1453,   339,   633,   333,   333,  1003,  1153,   580,   623,
	   635,   594,  1126,  1124,  1145,   503,   644,  1015,   325,   316,
	  1021,   589,  1080,   394,   348,  1075,   377,   362,   867,  1125,
	  1078,   394,  1046,  1175,   394,  1042,   826,   418,   844,   860,
	  1064,  1282,  1148,  1150,  1133,  1076,  1192,  1069,   839,  1072,
	   648,  1136,   569,  1002,   570,   745,   860,   632,  1160,  1178,
	   988,   988,   988,  1156,  1182,   993,  1154,  1179,  1116,   331,
	   394,  1119,  1474,   394,  1122,   592,  1149,   545,  1127,   645,
	  1130,  1190,   359,  1268,   470,  1197,   394,  1177,   635,   427,
	  1152,  1226,  1239,  1135,   839,   839,  1176,  1151,   412,   315,
	   372,   619,   669,   552,  1202,  1203,  1204,  1186,   394,   378,
	   686,  1187,   354,   379,   638,   682,   355,  1013,  1014,   660,
	  1016,   339,  1017,  1582,  1018,  1213,  1171,   373,   356,   353,
	   712,  1161,   756,   373,   346,  1162,   505,   926,   676,  1255,
	   377,   585,  1583,  1580,  1172,  1509,  1424,  1486,  1464,  1193,
	   590,  1188,   504,  1346,  1189,   668,   768,   371,  1194,   515,
	   441,   396,  1198,   566,   602,   819,   267,   698,   610,   512,
	   565,  1070,   612,   613,   708,  1210,  1208,  1242,  1256,   329,
	  1212,   814,  1243,   496,  1240,   816,   561,   497,     9,   560,
	  1220,  1241,  1192,  1192,   559,  1253,  1251,   323,  1292,   498,
	   499,  1053,  1192,  1192,  1049,  1263,  1047,  1045,   543,  1302,
	  1043,   269,   540,  1254,   359,   684,  1405,  1403,  1283,  1024,
	  1273,   788,  1257,  1019,  1276,   532,  1278,  1297,   266,  1261,
	  1322,  1067,  1061,   537,   549,   799,  1280,  1312,  1313,   343,
	  1065,   807,   185,  1331,  1059,   184,   296,   798,   547,   295,
	   265,  1296,  1298,  1333,  1266,  1277,   264,  1402,  1314,  1327,
	  1260,  1287,  1192,  1284,  1279,  1034,  1033,   633,  1028,   394,
	   790,   394,   188,  1293,   186,   314,    93,   406,   658,  1168,
	  1477,  1166,   683,  1265,  1041,   309,   806,   308,   310,  1332,
	   301,   300,   299,   418,   412,  1264,  1319,  1349,  1040,   538,
	   268,   187,  1192,   418,  1320,  1339,   657,   653,  1351,  1325,
	  1174,   988,   988,   662,  1329,  1318,  1302,   661,   262,   493,
	   182,   439,   440,   293,   418,  1347,   261,   181,   418,   459,
	  1387,  1360,   180,  1348,   418,  1001,  1341,  1368,   763,  1340,
	  1000,  1354,   762,   412,   487,   759,   486,   259,   485,  1364,
	   179,   258,   178,   755,   257,  1372,  1373,   627,   177,   256,
	  1363,   176,   617,  1398,   183,  1581,  1366,  1549,  1394,  1547,
	  1498,  1393,   828,   833,   834,  1252,   750,   837,   953,  1371,
	   748,   255,  1388,  1365,   478,  1369,  1376,   175,   254,   174,
	   744,   842,  1417,   925,  1374,   333,   849,   253,   851,   548,
	  1370,  1400,  1367,   743,  1392,  1192,   857,   514,   521,  1408,
	  1192,   475,  1192,  1192,   871,   173,   520,  1404,   873,   252,
	   413,   414,   473,  1302,   172,   568,  1192,   471,   171,   892,
	  1401,   573,   735,   467,   170,   585,   585,   169,  1302,  1302,
	  1221,   684,  1414,   720,  1412,  1428,  1411,  1431,  1432,   581,
	  1062,   581,   248,  1413,   168,  1068,   457,  1465,   580,  1466,
	   580,  1435,  1426,   581,  1425,  1471,  1416,   394,  1450,  1430,
	   453,  1475,   580,  1422,  1437,  1302,   826,  1192,  1470,  1433,
	   167,   246,  1455,  1459,   988,   165,  1386,   450,   940,  1440,
	   164,   714,   448,   632,  1302,   163,  1540,  1379,  1457,  1377,
	  1382,   971,  1446,  1497,   506,  1467,   507,   508,  1468,  1215,
	   970,   968,  1485,  1434,  1335,   711,  1456,   710,   162,  1479,
	  1478,  1229,   724,  1441,  1484,  1500,  1436,   707,  1487,   704,
	  1007,   581,   242,   703,   161,   160,  1506,   159,   701,  1508,
	   580,   158,  1469,  1492,  1493,  1192,   436,  1186,   157,   697,
	   695,  1187,   237,   434,  1302,  1504,  1501,  1302,  1302,  1505,
	   156,  1302,  1081,  1517,  1502,   236,  1121,  1507,   155,  1081,
	   153,   234,   428,   152,  1196,  1195,  1512,  1129,   944,  1494,
	  1510,  1246,  1244,   990,  1205,   948,   946,   689,  1525,   955,
	  1175,   426,  1513,   425,  1005,  1519,   233,  1144,   943,   688,
	   424,  1545,   148,  1521,   147,  1515,   415,   144,   143,   142,
	   229,  1302,   141,   673,   228,   409,  1178,   140,  1534,  1536,
	  1533,   670,  1539,  1192,  1179,  1546,   139,  1544,  1183,  1602,
	   581,  1537,  1524,  1555,  1482,  1542,  1302,  1552,  1573,   580,
	  1569,  1556,   585,  1576,  1177,  1535,  1554,  1516,  1481,  1567,
	  1572,  1408,  1577,  1176,  1480,  1543,   602,  1131,  1565,  1553,
	  1362,  1574,  1419,   924,  1541,   667,   610,   404,   226,   403,
	   581,  1140,  1192,   138,  1558,   137,  1560,   136,  1592,   580,
	  1590,  1568,  1586,  1591,  1588,  1587,   652,  1598,   135,  1596,
	  1605,  1302,   642,  1599,   134,  1155,   423,   900,  1158,   898,
	   640,   390,  1594,   633,   639,   389,  1192,  1593,  1559,   581,
	  1561,   130,  1618,  1462,  1613,  1104,  1102,  1619,   580,  1302,
	  1101,  1443,  1622,  1323,  1626,  1438,  1321,  1575,  1185,  1610,
	  1117,  1611,   584,  1605,  1628,  1625,   684,  1629,   337,   581,
	   681,  1617,   581,  1302,  1631,   678,   677,  1630,   580,   917,
	  1632,   580,  1106,  1096,  1094,  1612,  1088,  1564,  1614,  1615,
	  1616,  1562,  1511,  1097,  1595,  1099,  1092,  1301,  1620,  1090,
	  1087,  1086,  1305,  1303,   418,  1429,  1299,  1294,   636,   129,
	   625,   624,   128,   127,  1352,   886,   622,   126,  1147,   881,
	   880,   877,   876,  1146,   618,   380,   125,  1529,  1527,  1472,
	  1345,   376,  1621,   418,   374,   124,   123,  1081,  1081,   370,
	   418,   368,   122,  1342,  1143,   121,   605,   865,   607,   274,
	   212,   364,   418,  1315,   120,  1316,   119,   391,   600,   118,
	   117,  1452,   398,  1571,  1518,  1451,  1330,  1328,  1128,   595,
	   351,   116,  1447,  1326,   847,   115,   591,   114,  1123,   843,
	   347,   418,   113,   112,   342,   438,   340,   111,   418,   202,
	   201,   110,   108,   107,  1338,   577,   106,   576,   105,   104,
	   330,   103,  1461,  1460,  1336,  1139,  1138,  1334,   828,  1289,
	   495,   833,  1137,   830,   829,   102,  1589,  1288,  1286,  1418,
	  1285,   825,  1415,  1281,   823,   101,   324,    99,    98,    92,
	    91,    90,    89,    88,    87,    86,    85,    84,    83,    82,
	    81,    80,    79,    78,    77,    76,    75,    74,    73,    72,
	    71,    70,    69,    68,  1324,   329,    67,    66,    65,    64,
	   509,   511,   333,   513,   513,    63,    62,    61,    60,    59,
	    58,    57,    56,   513,    55,    54,    53,  1343,    52,   522,
	   523,   524,   525,   526,   527,   528,   529,    51,  1353,   531,
	    50,    49,    48,    47,    46,    45,    44,    43,    42,    41,
	    40,    39,    38,    37,    36,    35,    34,    33,    32,    31,
	    30,    29,    28,    27,    26,    25,    24,    23,    22,    21,
	   546,    20,    19,    18,    17,    16,    15,    14,    13,   418,
	    12,    11,   383,   883,   621,   382,     8,     6,     5,     4,
	     3,     1,  1420,     0,     0,     0,     0,     0,     0,     0,
	  1081,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  1445,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  1421,     0,     0,     0,     0,     0,     0,     0,   665,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  1449,     0,     0,     0,  1454,     0,  1458,     0,     0,     0,
	     0,   713,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,   717,   718,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  1185,   740,
	     0,     0,     0,     0,     0,     0,   753,     0,     0,   665,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,   767,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	   418,     0,     0,     0,     0,     0,     0,     0,     0,   774,
	   775,   776,   777,   778,   779,   780,   781,     0,   782,     0,
	     0,     0,     0,  1557,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   797,  1514,     0,     0,     0,   333,     0,
	     0,     0,     0,     0,     0,  1520,     0,     0,     0,     0,
	     0,     0,  1578,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  1526,     0,     0,  1530,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  1449,
	  1570,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,   901,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,   908,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  1597,     0,     0,  1530,  1600,
	  1601,  1606,     0,     0,   942,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,   963,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  1623,     0,
	     0,     0,     0,     0,     0,   977,     0,     0,     0,     0,
	     0,  1627,     0,     0,  1606,    10,    97,   189,   191,   192,
	     0,   193,   194,   270,   204,     0,   207,     0,   209,   213,
	   211,   210,   214,   215,   216,   217,   218,   219,     0,     0,
	     0,     0,     0,   132,   131,   133,   203,   224,   223,     0,
	   190,   225,   221,   222,   100,   109,   205,   206,   208,   220,
	   196,   197,   198,   195,   199,   200,   271,   272,   273,   231,
	   275,   230,   227,   277,   276,   154,   235,   232,   279,   278,
	     0,     0,     0,   283,   280,   281,   166,   243,   249,   146,
	   239,   245,   250,   251,   247,   151,   150,   145,   149,   284,
	   282,     0,   244,   238,  1268,   241,   240,   285,   286,   289,
	   287,   291,   290,   288,   294,   303,   292,   260,   263,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,    95,
	     7,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,   304,    94,     0,   307,
	   306,     0,   312,     0,   302,     0,   311,   313,   297,    96,
	   298,     0,     0,   305,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  1163,
	     0,     0,     0,     0,     0,     0,     0,   665,     0,     0,
	  1181,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  1209,     0,     0,  1211,
	    10,    97,   189,   191,   192,     0,   193,   194,   270,   204,
	     0,   207,  1218,   209,   213,   211,   210,   214,   215,   216,
	   217,   218,   219,     0,     0,     0,     0,     0,   132,   131,
	   133,   203,   224,   223,   753,   190,   225,   221,   222,   100,
	   109,   205,   206,   208,   220,   196,   197,   198,   195,   199,
	   200,   271,   272,   273,   231,   275,   230,   227,   277,   276,
	   154,   235,   232,   279,   278,     0,     0,     0,   283,   280,
	   281,   166,   243,   249,   146,   239,   245,   250,   251,   247,
	   151,   150,   145,   149,   284,   282,     0,   244,   238,     0,
	   241,   240,   285,   286,   289,   287,   291,   290,   288,   294,
	   303,   292,   260,   263,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,    95,     7,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	   885,   304,    94,     0,   307,   306,     0,   312,     0,   302,
	     0,   311,   313,   297,    96,   298,     0,     0,   305,     0,
	    10,    97,   189,   191,   192,     0,   193,   194,   270,   204,
	     0,   207,     0,   209,   213,   211,   210,   214,   215,   216,
	   217,   218,   219,     0,     0,   901,     0,  1355,   132,   131,
	   133,   203,   224,   223,     0,   190,   225,   221,   222,   100,
	   109,   205,   206,   208,   220,   196,   197,   198,   195,   199,
	   200,   271,   272,   273,   231,   275,   230,   227,   277,   276,
	   154,   235,   232,   279,   278,     0,     0,     0,   283,   280,
	   281,   166,   243,   249,   146,   239,   245,   250,   251,   247,
	   151,   150,   145,   149,   284,   282,     0,   244,   238,     0,
	   241,   240,   285,   286,   289,   287,   291,   290,   288,   294,
	   303,   292,   260,   263,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,    95,     7,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   304,    94,     0,   307,   306,     0,   312,     0,   302,
	     0,   311,   313,   297,    96,   298,     0,     0,   305,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  1476
};

short yypact[] = {
	 -1000,  2464, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,   673, -1000, -1000,   541,   443,   540,
	 -1000, -1000, -1000,   597, -1000, -1000, -1000,   728, -1000, -1000,
	 -1000,   383,   743,   547, -1000, -1000,   744, -1000,   551, -1000,
	   109, -1000,   782, -1000,   635,   -75, -1000,   443, -1000, -1000,
	   477, -1000, -1000, -1000,   270,   332,   196,   178,   515,   358,
	   275,   109,   358,   358,   381, -1000,   501,   449,   658, -1000,
	 -1000, -1000,   167,   332, -1000,   196,   272,   271,   332,   270,
	   270,   265,   173,   269,   469,   178, -1000,   453,   265,  -122,
	   651,   500,   258,   491,   196,   257,   226,   178,   178,   502,
	   -75,   -75,   248,   332, -1000, -1000, -1000, -1000, -1000, -1000,
	   815, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,   612, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,   761, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,   358, -1000,   358,   358,
	 -1000, -1000, -1000,   460,   332, -1000,   332,   332, -1000, -1000,
	 -1000, -1000,   318, -1000, -1000, -1000,   332, -1000,   318, -1000,
	 -1000, -1000,   332,   332,   332,   332,   332,   332,   332,   332,
	 -1000,   167,   332, -1000, -1000, -1000,  -263,  -263, -1000,   -62,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,   332, -1000,  -256,  -256, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,   -85, -1000,   -88,   -88,
	   -88, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,   -16, -1000, -1000,   109, -1000,   109,   -61,
	 -1000, -1000,   200, -1000,   498,   200, -1000, -1000,  -173, -1000,
	 -1000, -1000, -1000, -1000, -1000,   544, -1000, -1000, -1000,   109,
	   533,   218, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	   200, -1000, -1000,   533,   507, -1000, -1000,   544, -1000, -1000,
	 -1000, -1000,   109,   -52,   749,   109, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,   529, -1000,   437,   109,   229,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   -26, -1000, -1000,
	   719, -1000,   445, -1000,   770,   701,   514, -1000, -1000, -1000,
	   719, -1000,   710, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,   288, -1000,   651, -1000,   226, -1000,   -75, -1000,
	 -1000,   118,   288, -1000, -1000, -1000, -1000,   465,   287, -1000,
	 -1000,  -206, -1000,   737,   332, -1000, -1000, -1000, -1000,   468,
	   225, -1000, -1000, -1000, -1000,   332,   332, -1000, -1000,   166,
	 -1000, -1000, -1000, -1000, -1000, -1000,   482,   651, -1000,   457,
	 -1000, -1000,   332, -1000,   142,   582,   196, -1000, -1000,   104,
	   739, -1000,   164,   739, -1000, -1000,   -21,   490, -1000, -1000,
	 -1000, -1000, -1000, -1000,    97,   332, -1000, -1000, -1000, -1000,
	 -1000,   200,   200, -1000,   781, -1000, -1000, -1000, -1000,   350,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,   332,   332,   332,   332,   332,   332,   332,   332,
	 -1000,   332,   219,  -133, -1000, -1000, -1000,  -177, -1000,   219,
	   -83, -1000, -1000,  -207, -1000, -1000,   332, -1000,   219,  -125,
	 -1000,   -99, -1000, -1000, -1000,   192, -1000,  -256, -1000, -1000,
	   190,   -86, -1000, -1000, -1000,   -27,   223, -1000,   109, -1000,
	 -1000, -1000, -1000,   531, -1000, -1000, -1000,   200, -1000, -1000,
	 -1000, -1000,   200, -1000,  -219, -1000, -1000, -1000, -1000, -1000,
	   563,   728, -1000,   -31, -1000, -1000,   744, -1000, -1000, -1000,
	   200, -1000,  -132, -1000,   584,   467, -1000,   -36, -1000,   553,
	  -137, -1000, -1000, -1000,   531, -1000, -1000,   109,   531,    44,
	 -1000,  2294,   443, -1000, -1000,   416,   609, -1000, -1000, -1000,
	 -1000, -1000,  -173,  -173, -1000, -1000,   531, -1000,   710, -1000,
	   414, -1000,   152, -1000, -1000, -1000, -1000, -1000, -1000,   234,
	 -1000, -1000,   332,   148,   101, -1000, -1000,  -150,   215,   -60,
	 -1000,   139,    98, -1000, -1000,  -150,   213,   109,   746, -1000,
	 -1000,   466, -1000,   121,    85, -1000,   -60,   -36,   -23, -1000,
	 -1000,  -142, -1000,   211,   -52,   612, -1000,   332, -1000,   651,
	 -1000,   365,   -75, -1000, -1000,    74, -1000,  -150,   -60,   286,
	  -150,   332, -1000,    77,   283, -1000, -1000,   199,   -60, -1000,
	 -1000,   341, -1000,    76,  -150, -1000,   332, -1000,   332,    69,
	   282, -1000, -1000,  -150,   206, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,    12, -1000,   329, -1000,    95,
	 -1000,    66, -1000, -1000,   178, -1000, -1000, -1000,    55, -1000,
	   281, -1000, -1000,  -150, -1000,   197, -1000, -1000, -1000, -1000,
	 -1000, -1000,   580,   196, -1000,    60, -1000, -1000, -1000,   194,
	 -1000, -1000,   372,   372,  -122,  -122,  -122,   350,  -122, -1000,
	  -122, -1000,  -122,  -226, -1000, -1000,   146, -1000,  -187,    92,
	  -263, -1000, -1000, -1000,   -83,    49,    81, -1000,  -133, -1000,
	 -1000, -1000, -1000, -1000,   481, -1000,   -88,  -133, -1000, -1000,
	 -1000, -1000,   192, -1000,   191, -1000,   190, -1000, -1000, -1000,
	 -1000, -1000,   531,   -23, -1000,   200, -1000, -1000,  -137,   -23,
	   200, -1000, -1000,  -137, -1000, -1000,  -208, -1000,   475,  -173,
	   170, -1000,  -132,  -208, -1000, -1000,   -36,   743,   200, -1000,
	 -1000,   109, -1000, -1000, -1000, -1000, -1000, -1000,   728,   334,
	 -1000, -1000, -1000, -1000, -1000, -1000,   334, -1000,  -162, -1000,
	 -1000,   109, -1000, -1000,   531, -1000,   -36,   -23, -1000, -1000,
	  -137, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   475, -1000,
	   609, -1000, -1000, -1000,   200,   200, -1000,   -23,    31, -1000,
	  -150, -1000, -1000, -1000, -1000,   -26, -1000, -1000, -1000, -1000,
	 -1000, -1000,   411, -1000, -1000,   401, -1000, -1000, -1000, -1000,
	   367, -1000,    53,   332,   531, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,    39,   759, -1000,   401, -1000,
	 -1000, -1000, -1000, -1000,   653, -1000,   -57, -1000,   488,   329,
	   329,   329,   380, -1000,    95, -1000, -1000, -1000, -1000,   332,
	 -1000,   465,   332, -1000, -1000, -1000, -1000,  -150,   280, -1000,
	   180,  -233, -1000, -1000, -1000,   332,   279, -1000, -1000,   199,
	   214,   276,   -75,   457, -1000,   162, -1000, -1000, -1000, -1000,
	   162,   429, -1000, -1000, -1000, -1000, -1000,   332, -1000,   166,
	 -1000,   178, -1000, -1000, -1000,   737, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,  -133, -1000, -1000,  -263, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,  -263, -1000,  -256, -1000, -1000,
	 -1000,  2059, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	   221, -1000, -1000, -1000,  -129, -1000, -1000, -1000, -1000,   219,
	 -1000,  -265,   146, -1000,  -256,   219, -1000,  -265,  -256, -1000,
	 -1000, -1000, -1000, -1000,   567, -1000,   -11,   728, -1000,  -241,
	 -1000,  -208,  -208,   -76,   -60, -1000, -1000, -1000, -1000, -1000,
	 -1000,    39,    -5, -1000, -1000, -1000, -1000,   109, -1000,   109,
	 -1000, -1000, -1000, -1000, -1000,   176,   518,   -52, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   334,
	 -1000, -1000, -1000, -1000,  -241, -1000, -1000,   544,   533, -1000,
	 -1000,   -52, -1000, -1000, -1000, -1000, -1000,   -98,   109,   -36,
	 -1000, -1000, -1000,   507, -1000,   764, -1000,   764, -1000,   -66,
	 -1000,  -104,   -60, -1000, -1000, -1000, -1000, -1000,   414, -1000,
	   332, -1000, -1000, -1000,   174, -1000,   277, -1000, -1000,   116,
	 -1000, -1000, -1000, -1000, -1000,   -60, -1000, -1000, -1000, -1000,
	 -1000,    52,   764,   -36, -1000,  -151, -1000, -1000, -1000, -1000,
	  -222,    39,   -60, -1000, -1000, -1000,   167, -1000,   -75,   365,
	   329,   329,   162, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	    37, -1000, -1000, -1000,   341, -1000,   306,   339,    30, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,   -60, -1000, -1000,    24,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,    22, -1000,  -150, -1000, -1000, -1000,
	 -1000, -1000,   -20,    23, -1000, -1000, -1000, -1000,   628,    92,
	  -133, -1000,   124, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,  -133, -1000, -1000,  -256, -1000,  -133, -1000,
	   146,   334, -1000, -1000, -1000,   728,   109, -1000,   407, -1000,
	 -1000, -1000, -1000, -1000,  -208, -1000,  -214,  -116, -1000,    39,
	  -238,    39,    39, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,  -232, -1000,    39, -1000, -1000,   -60, -1000,
	 -1000,   463, -1000,   -36,   423,   109,   609, -1000,   531, -1000,
	   520,   336,   -36, -1000, -1000,   756,  -101,   758,   475, -1000,
	   475,   584,   -36,   423,   728,   601, -1000, -1000, -1000,  -222,
	 -1000, -1000,   475, -1000, -1000, -1000,   332,   455, -1000, -1000,
	 -1000,  -150,   513, -1000, -1000, -1000,    39,   757,  -228, -1000,
	 -1000, -1000, -1000,   329, -1000, -1000, -1000,    18, -1000,  -235,
	 -1000, -1000, -1000, -1000, -1000, -1000,    17,   -60, -1000, -1000,
	 -1000,   429,   322,   137, -1000, -1000, -1000, -1000, -1000, -1000,
	  -180, -1000, -1000,   219, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,   728, -1000, -1000, -1000,   756,
	   475, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  -222,   755,
	 -1000, -1000,  -260, -1000,    39,  -136,   -60, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   200, -1000,   -61,
	 -1000,   584,   612, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,   454,   439, -1000, -1000,   -23,   728, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,     1,    -7,   116,   -26,
	 -1000,   551,   432, -1000, -1000,  -222, -1000, -1000, -1000,   306,
	   -26,   737, -1000, -1000, -1000, -1000, -1000, -1000,  -103,   478,
	 -1000,    92,  -133,   628,   124,    92, -1000,   109,   -36, -1000,
	  -232,   425,    39,   -17,   423,   -36, -1000, -1000,   544,   475,
	 -1000, -1000, -1000, -1000, -1000, -1000,   423,   200, -1000,   -61,
	   -52, -1000, -1000, -1000, -1000, -1000,   109, -1000,   404, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   753,   730,
	   752,    14, -1000, -1000,  -199, -1000, -1000,   -81, -1000,   475,
	 -1000,    39,   531, -1000,   200,  -222, -1000, -1000,   728, -1000,
	 -1000, -1000, -1000,   -61, -1000, -1000, -1000, -1000,   612, -1000,
	 -1000, -1000,   177, -1000, -1000,   476,   628, -1000,   628,   -23,
	 -1000,   171,   -23,  -228,   -23,    39, -1000,   533,   475, -1000,
	 -1000, -1000,   -36, -1000, -1000,   612,   -52, -1000, -1000,   -30,
	  -180,  -170, -1000, -1000, -1000, -1000, -1000,  -222, -1000,   533,
	 -1000,   612, -1000, -1000, -1000, -1000, -1000, -1000,   475, -1000,
	   -61,   475, -1000
};

short yypgo[] = {
	     0,  1901,   102,  1900,  1899,  1898,  1897,  1896,    36,    91,
	  1895,  1894,  1893,  1892,  1891,  1890,  1888,  1887,  1886,  1885,
	  1884,  1883,  1882,  1881,  1879,  1878,  1877,  1876,  1875,  1874,
	  1873,  1872,  1871,  1870,  1869,  1868,  1867,  1866,  1865,  1864,
	  1863,  1862,  1861,  1860,  1859,  1858,  1857,  1856,  1855,  1854,
	  1853,  1852,  1851,  1850,  1847,  1838,  1836,  1835,  1834,  1832,
	  1831,  1830,  1829,  1828,  1827,  1826,  1825,  1819,  1818,  1817,
	  1816,  1813,  1812,  1811,  1810,  1809,  1808,  1807,  1806,  1805,
	  1804,  1803,  1802,  1801,  1800,  1799,  1798,  1797,  1796,  1795,
	  1794,  1793,  1792,  1791,  1790,  1789,  1788,   142,  1787,  1786,
	   990,   162,    29,  1785,   692,   615,  1784,    55,  1783,  1782,
	    10,    75,  1781,    77,    89,   106,  1780,  1779,  1778,    74,
	  1777,    37,   266,  1776,    12,    15,     8,    33,     0,  1775,
	    76,  1774,     3,  1773,    84,  1772,  1767,  1766,  1765,  1764,
	  1763,  1762,  1761,  1760,  1759,    49,  1758,  1757,  1756,  1755,
	    40,  1753,  1752,  1751,  1750,  1749,  1747,  1746,  1744,  1743,
	   120,    21,   121,  1742,  1740,  1739,  1738,    59,  1737,  1736,
	  1735,     4,  1734,  1733,    65,   152,  1732,    13,   859,    56,
	  1731,  1730,  1729,  1728,  1727,   146,    27,  1726,  1725,  1724,
	  1723,  1721,  1720,  1719,  1718,   116,    11,  1716,  1714,  1711,
	    82,    53,  1710,  1709,  1708,   119,  1707,   145,  1706,  1705,
	  1704,  1703,    24,  1702,  1701,  1699,  1696,  1695,  1694,   877,
	    67,  1691,  1690,  1689,  1688,    30,     7,  1687,  1686,  1685,
	  1684,  1683,  1682,  1681,    73,  1680,    35,  1679,    98,    45,
	  1678,  1677,  1676,  1675,  1674,  1673,  1672,  1671,  1670,   138,
	     9,  1669,  1668,    58,  1667,  1666,  1665,  1663,   147,  1662,
	  1661,  1660,  1659,  1657,  1656,  1655,  1653,  1652,  1651,  1647,
	  1646,    71,  1644,    79,  1643,     1,  1642,  1639,  1636,  1635,
	   110,  1630,    70,  1628,   118,  1622,  1620,  1616,  1615,  1613,
	  1611,  1610,  1606,  1605,  1601,  1595,  1594,  1591,  1586,  1590,
	  1589,    72,  1588,  1587,    47,  1584,  1051,  1582,   115,    63,
	    54,    26,    34,  1578,  1576,  1567,   132,  1565,   101,  1563,
	  1559,  1558,  1167,  1557,  1555,  1553,  1550,  1544,  1538,  1535,
	  1524,  1521,  1519,     5,     6,  1518,    48,  1516,  1511,  1507,
	  1505,  1504,  1503,   271,   111,  1502,  1500,  1499,  1498,  1497,
	  1496,  1494,   155,  1492,  1490,  1489,  1488,  1486,   129,  1484,
	    23,  1483,  1481,   200,  1479,  1477,   135,   134,  1476,   105,
	    69,  1475,    44,    81,  1474,    68,  1473,  1472,    42,  1471,
	  1469,  1468,  1465,  1464,    78,  1463,  1462,  1461,  1460,  1458,
	  1455,  1059,  1450,  1443,  1442,  1440,   107,  1439,  1438,  1436,
	   103,  1431,  1428,  1427,  1425,  1424,  1050,  1423,  1422,  1049,
	   151,  1420,  1419,   104,  1417,   127,  1412,  1411,  1408,   133,
	  1407,  1405,  1401,    61,  1400,  1399,  1391,  1390,  1389,    22,
	  1387,  1386,  1385,  1382,  1381,  1380,  1377,  1376,  1375,  1371,
	  1370,  1360,  1346,  1344,    85,  1342,  1333,   100,  1330,  1327,
	   153,  1324,  1323,   161,  1322,    97,  1318,  1317,  1314,  1312,
	  1309,   231,  1305,  1301,  1293,  1287,  1280,  1279,  1278,  1277,
	  1274,  1271,  1270,   158,  1266,    88,  1265,  1261,  1260,  1259,
	  1258,  1257,  1255,  1254,  1251,   165,  1249,  1248,  1244,  1243,
	  1242,  1241,  1240,  1238,  1237,  1236,  1235,  1234,  1232,  1230,
	  1228,  1225,  1222,  1217,  1216,  1213,  1210,  1209,  1208,  1207,
	  1203,   113,    28,  1200,  1197,   114,  1196,  1191,  1190,  1189,
	    39,  1188,  1185,  1182,  1181,  1180,  1178,   123,  1177,   124,
	    94,    57,  1176,    80,  1175,  1174,  1173,    32,   139,  1172,
	   109,  1171,  1170,  1169,    52,    31,   160,  1168,  1166,  1165,
	    93,    90,  1164,  1162,    14,    83,    43,    25,    19,    87,
	  1160,    66,    60,  1158,  1156,  1155,  1150,  1147,  1146,  1140,
	  1139,  1138,  1137,  1134,  1131,  1130,   993,  1125,  1124,   125,
	   149,    50,    62,    64,  1122,    86,  1121,    51,  1118,  1115,
	   126,    46,  1113,  1111,  1109,  1107,    20,    41,  1106,    17,
	    18,     2,  1102,  1101,  1100,   150,   130,  1098,  1097,  1096,
	  1094,  1091,   782,  1087,  1084,  1079,   122,  1076,  1075,  1071,
	  1061,   141,  1060,  1055,  1053
};

short yyr1[] = {
	     0,     1,     1,     2,     2,     2,     2,     2,     2,     2,
	     2,     9,    10,    11,    11,     3,     3,     3,     3,     3,
	     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
	     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
	     3,     3,     3,     3,     4,     4,     4,     4,     4,     4,
	     4,     4,     5,     5,     5,     5,     5,     5,     6,     6,
	     6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
	     6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
	     6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
	     6,     6,     6,     6,     6,     6,     6,    14,    14,    96,
	    15,    98,    98,    16,    17,   103,   106,   106,   106,   112,
	   112,   113,   116,   116,   116,   116,   116,   116,   117,   117,
	   117,   123,   123,   120,   120,   118,   108,   108,   109,   109,
	    18,   129,   130,   130,   131,   131,   133,   133,   134,   135,
	   135,   135,   138,   137,   136,   136,   136,   136,   136,   139,
	   140,   141,    19,   142,   142,   143,   143,    20,    20,    20,
	    20,   144,   144,   146,   146,   148,   147,   147,    21,    21,
	   151,   152,    22,    23,   153,   153,   153,   153,   154,   155,
	   155,   155,    24,   156,   157,   157,   158,    25,   159,    26,
	   163,   164,   164,   166,   166,    27,   168,   169,   169,    28,
	   170,   172,   172,   172,   173,   173,   176,   176,   177,   177,
	   174,   174,   178,   178,    29,   180,   181,   181,   185,   185,
	   185,   185,   185,   185,   182,   182,   182,   183,   183,   184,
	   184,   187,   187,   187,   187,   188,   188,   188,   191,   189,
	   189,   190,   190,    30,   192,    31,   193,   194,   194,   195,
	   195,    32,   197,    33,   198,   198,   202,   203,   199,   199,
	   204,   204,   205,   205,   206,   206,   200,   201,   201,   201,
	    34,   209,   209,   210,   210,   211,   211,    35,   213,   214,
	   214,   214,   215,    36,   216,   216,    37,   217,   217,   218,
	   221,   221,   221,   220,   220,   222,   223,   223,   224,   224,
	   226,   226,   227,   227,    38,   228,   219,   219,   230,   230,
	   232,   232,   234,   234,   235,   237,   237,   231,   240,   229,
	   229,    39,    39,   241,   242,   243,   244,   243,    40,   245,
	    41,   246,   247,   247,   247,   247,   247,   247,   247,   247,
	   248,   249,   249,   249,    42,   251,   252,   252,   102,   102,
	   102,   167,   167,   167,   167,   167,   255,   255,   255,   256,
	   256,   236,   236,   261,   261,   261,   261,   261,   261,   261,
	   261,   261,   264,   265,   262,   266,   267,   267,   269,   269,
	   268,   268,   260,   260,   270,   270,   270,   270,   271,   271,
	   275,   272,   274,   274,   276,   273,   239,   239,   277,   254,
	   253,   259,   125,   263,   263,   263,   258,   257,   101,   101,
	   278,   278,   280,   280,   280,   281,   110,   110,   283,   175,
	   175,   284,   284,   285,   286,   286,   286,   286,   286,   287,
	   288,   288,   289,   289,   150,   225,   132,   114,   127,   208,
	   162,   162,   115,   115,   282,   282,   165,   165,   207,   105,
	   121,   186,   161,   196,   107,   171,    99,    99,   179,   122,
	   160,   160,   212,   212,   290,   124,   124,   124,   124,   124,
	   291,   293,   292,   100,   100,   104,   145,   145,   250,   126,
	   126,   149,   149,   119,    97,    97,    97,   111,    43,   294,
	   295,   297,   297,   296,   299,   300,   300,   301,   303,   303,
	   303,   302,    44,    45,    46,    47,   305,   306,   307,   307,
	   308,   308,   308,   308,   308,    48,   313,    49,   315,    50,
	   317,    51,   319,   321,   320,   326,   324,   324,   323,   323,
	   327,   327,   328,   330,   331,   331,   329,   329,   332,   332,
	   333,   333,   334,   334,   322,   322,   325,   335,   335,   336,
	   336,   336,   336,    52,   337,   338,   338,   338,    53,   339,
	   341,   340,   340,   342,   342,   344,    54,   345,   346,    55,
	   347,    56,   348,    57,   349,   350,   350,    58,    59,    59,
	    59,   351,    60,   353,   353,   357,   358,   358,   358,   359,
	   359,   354,   361,   361,   362,   364,   364,   355,   355,   365,
	   365,   366,   367,   369,   369,   368,   368,   370,   370,   370,
	   370,   370,   370,   370,   371,   371,   372,   372,   372,   374,
	   374,   375,   375,   376,   376,   373,   373,   377,   377,   378,
	   379,   379,   379,   379,   380,   380,   380,   356,   381,   382,
	   382,   383,    61,    62,    63,    64,   385,   387,   386,    65,
	   388,    66,    67,   389,   390,    68,   392,   394,   393,   395,
	   395,   396,   397,    69,   398,   399,   400,    70,   401,    71,
	   403,    72,   404,    73,   405,   406,   408,   409,   409,   409,
	   409,   410,   410,   411,   411,   411,   407,   407,   412,   412,
	   413,   414,   415,   415,   415,   416,   416,   416,   416,   416,
	   416,   416,   417,   417,   417,   417,   417,   417,   417,   417,
	   417,   417,    74,   418,   360,   360,   420,   421,   422,   422,
	   423,   424,   426,   426,   427,   427,   425,   425,   428,   428,
	   429,   430,   430,   431,   431,   431,   431,   431,    75,   432,
	   433,   433,   434,    76,   435,   436,   436,   437,    77,   438,
	   439,    78,    79,   440,   441,   441,   441,   441,   441,   442,
	    80,   443,   445,   444,   444,   446,   446,   447,   448,   448,
	   448,   448,   448,   448,    81,   449,    82,   451,   452,   452,
	   453,   454,   454,   455,   455,    83,   456,   457,   457,    84,
	   458,   460,   459,   459,   459,    85,   462,   465,   463,   463,
	   464,   464,   466,    86,   467,   468,    87,   469,   471,   470,
	   470,   470,   472,   472,   474,   474,   476,   475,   477,   478,
	   478,   478,   480,   480,   479,   479,   479,   481,   482,   482,
	    95,   483,    88,   484,   486,    89,   487,   488,   485,   485,
	   489,    90,   490,   491,    91,   492,   494,   493,   493,   495,
	   496,   496,   497,   497,   498,   498,   500,   499,   499,   501,
	    92,   502,    93,   503,   504,   505,    94,   506,   508,   507,
	   507,   507,   312,   298,   298,   419,   419,   473,   352,   352,
	   314,   314,   309,   309,   310,   310,   311,   311,   450,   450,
	   450,   450,   363,   363,   363,   318,   318,   509,   509,   511,
	   511,   511,   512,   512,   512,   512,   512,   513,   384,   384,
	   384,   316,   316,   514,   514,   515,   515,   516,   461,   461,
	   391,   391,   304,   304,   402,   402,   517,   519,   522,   518,
	   518,   518,   525,   523,   524,   524,   533,   521,   527,   536,
	   535,   535,   529,   529,    13,    12,   279,   233,   538,   539,
	   540,   541,   541,   543,   542,   544,   545,   545,   343,   510,
	   547,   546,    74,   118,   118,   137,   137,   265,   265,   477,
	   477,   416,   285,     8,     8,     7,     7,   549,   549,   548,
	   548,   548,   548,   548,   552,   555,   560,   560,   556,   558,
	   557,   557,   562,   562,   562,   563,   563,   563,   563,   564,
	   566,   566,   550,   550,   568,   569,   572,   574,   571,   571,
	   576,   576,   577,   577,   573,   573,   575,   575,   582,   583,
	   584,   586,   587,   587,   580,   531,   531,   585,   585,   551,
	   589,   589,   592,   592,   590,   593,   567,   567,   595,   595,
	   591,   565,   598,   598,   600,   600,   600,   597,   599,   520,
	   570,   581,   559,   537,   532,   534,   579,   579,   602,   601,
	   578,   561,   603,   528,   526,   588,   604,   554,   594,   530,
	   530,   530,   530,   596,   553,   553,   605,   605,   607,   606,
	   608,   608,   608,   608,   609,   609,   609,   609,   610,   610,
	   610,   611,   611,   611,   611,   238,   128,   612,   613,   614,
	   614,   614,   616,   616,   616,   619,   620,   615,   618,   617,
	   617,   621,   622,   622,   624,   623
};

short yyr2[] = {
	     0,     2,     0,     1,     1,     1,     1,     1,     1,     1,
	     1,     3,     1,     0,     2,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     2,     2,
	     5,     1,     2,     1,     8,     1,     1,     1,     0,     1,
	     3,     4,     1,     1,     1,     2,     2,     2,     0,     5,
	     2,     1,     2,     4,     6,     1,     1,     1,     1,     1,
	     6,     1,     1,     3,     1,     1,     1,     3,     4,     1,
	     2,     4,     3,     1,     2,     1,     2,     2,     0,     2,
	     2,     2,     4,     1,     2,     1,     0,     2,     3,     2,
	     1,     1,     1,     1,     1,     1,     1,     1,     2,     1,
	     1,     1,     1,     1,     1,     2,     2,     2,     1,     1,
	     1,     1,     2,     1,     1,     1,     1,    10,     1,     5,
	     1,     1,     0,     1,     1,     4,     1,     0,     3,     7,
	     1,     1,     1,     1,     2,     0,     1,     3,     1,     3,
	     1,     0,     2,     2,     7,     1,     1,     3,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     3,     0,     1,
	     4,     2,     2,     2,     0,     2,     2,     0,     1,     8,
	     0,     4,     0,     2,     1,     3,     1,     1,     3,     3,
	     5,     4,     1,     5,     1,     2,     2,     1,     0,     3,
	     1,     3,     2,     1,     0,     2,     2,     0,     2,     2,
	     8,     3,     2,     0,     3,     2,     4,     2,     1,     2,
	     2,     1,     1,     4,     1,     2,     7,     1,     2,     2,
	     1,     1,     0,     3,     0,     1,     1,     0,     1,     3,
	     1,     3,     1,     3,     2,     5,     1,     0,     3,     3,
	     1,     3,     3,     3,     3,     1,     0,     2,     0,     0,
	     1,     4,     2,     1,     1,     1,     0,     4,     2,     1,
	     3,     1,     0,     1,     3,     2,     2,     3,     4,     1,
	     1,     1,     1,     1,     4,     1,     1,     3,     2,     2,
	     0,     3,     2,     3,     3,     4,     1,     1,     1,     0,
	     1,     1,     1,     1,     3,     3,     2,     3,     3,     5,
	     7,     2,     2,     1,     7,     1,     0,     2,     1,     3,
	     1,     0,     1,     1,     1,     1,     1,     1,     2,     2,
	     2,     2,     3,     2,     1,     5,     0,     2,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     3,     3,
	     1,     3,     3,     1,     3,     1,     1,     0,     2,     1,
	     3,     3,     1,     1,     1,     2,     3,     1,     3,     1,
	     1,     0,     2,     4,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     0,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     0,     1,     1,
	     1,     0,     0,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     3,     1,     1,     3,     1,     1,
	     1,     1,     3,     1,     1,     1,     1,     1,     3,     1,
	     1,     1,     1,     3,     1,     1,     3,     3,     1,     1,
	     1,     1,     1,     1,     1,     2,     1,     3,     1,     3,
	     1,     1,     1,     1,     1,     3,     1,     2,     1,     2,
	     1,     2,     2,     1,     6,     1,     1,     2,     1,     2,
	     0,     2,     2,     1,     1,     2,     0,     4,     1,     3,
	     1,     2,     1,     1,     1,     1,     3,     1,     3,     3,
	     1,     1,     3,     3,     1,     0,     2,     1,     2,     2,
	     1,     3,     3,     1,     3,     2,     2,     2,     1,     2,
	     1,     2,     1,     2,     1,     1,     1,     1,     2,     2,
	     2,     2,     4,     5,     2,     1,     1,     1,     0,     1,
	     0,     1,     3,     0,     1,     1,     0,     1,     0,     1,
	     2,     4,     1,     1,     0,     1,     3,     3,     3,     2,
	     2,     4,     2,     1,     0,     1,     1,     1,     1,     1,
	     1,     2,     3,     1,     1,     0,     3,     1,     3,     3,
	     1,     1,     1,     1,     1,     1,     1,     2,     0,     2,
	     0,     1,     1,     1,     1,     3,     2,     1,     1,     2,
	     1,     1,     2,     2,     1,     2,     2,     1,     3,     1,
	     3,     3,     2,     2,     1,     5,     3,     4,     1,     2,
	     1,     2,     1,     4,     3,     1,     1,     1,     1,     1,
	     1,     0,     2,     1,     1,     1,     1,     1,     1,     3,
	     2,     3,     1,     3,     4,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     3,     1,     1,     0,     2,     1,     1,     3,
	     2,     3,     1,     1,     1,     1,     3,     0,     1,     3,
	     3,     1,     1,     1,     1,     1,     1,     1,     2,     1,
	     5,     0,     1,     8,     1,     1,     0,     2,     2,     2,
	     1,     1,     2,     1,     0,     1,     2,     3,     1,     1,
	     4,     3,     1,     1,     1,     1,     3,     3,     1,     1,
	     1,     1,     2,     1,     2,     1,     2,     1,     1,     2,
	     4,     1,     3,     3,     1,     2,     1,     0,     2,     2,
	     4,     1,     3,     2,     0,     3,     4,     1,     0,     2,
	     0,     2,     1,     2,     4,     1,     2,     4,     1,     2,
	     3,     0,     1,     1,     1,     3,     0,     6,     1,     0,
	     3,     5,     1,     1,     0,     2,     2,     2,     2,     0,
	     3,     1,     3,     4,     1,     3,     3,     1,     0,     4,
	     1,     2,     4,     1,     2,     3,     1,     2,     3,     1,
	     1,     1,     1,     1,     0,     2,     1,     0,     2,     1,
	     2,     1,     2,     2,     3,     1,     2,     4,     1,     3,
	     2,     0,     1,     1,     1,     1,     1,     1,     1,     3,
	     0,     2,     1,     2,     1,     2,     1,     2,     1,     1,
	     1,     1,     1,     2,     0,     3,     3,     1,     3,     3,
	     6,     3,     2,     1,     1,     1,     1,     1,     1,     2,
	     0,     3,     3,     1,     3,     3,     6,     2,     1,     0,
	     1,     0,     1,     1,     1,     0,     5,     0,     0,     1,
	     1,     1,     2,     3,     5,     3,     2,     2,     1,     1,
	     0,     2,     0,     1,     1,     1,     1,     1,     3,     1,
	     4,     1,     1,     2,     2,     1,     0,     1,     1,     1,
	     1,     3,     4,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     2,     0,     1,     1,
	     1,     1,     1,     1,     7,     3,     1,     1,     0,     0,
	     1,     3,     1,     1,     1,     1,     1,     1,     1,     8,
	     0,     1,     1,     1,     4,     4,     0,     0,     0,     2,
	     2,     2,     0,     1,     1,     3,     1,     3,     2,     2,
	     3,     3,     0,     1,     1,     0,     1,     0,     1,     2,
	     2,     3,     1,     2,     4,     0,     0,     1,     1,     3,
	     2,     6,     1,     3,     6,     1,     1,     0,     0,     0,
	     1,     1,     1,     1,     1,     1,     1,     2,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     2,     2,     1,     1,     2,     4,     1,     3,     1,     3,
	     1,     1,     1,     1,     1,     2,     1,     2,     1,     2,
	     2,     1,     1,     1,     1,     1,     1,     2,     0,     2,
	     3,     1,     0,     1,     2,     1,     1,     1,     1,     1,
	     2,     2,     1,     2,     1,     1
};

short yychk[] = {
	 -4000,    -1,    -2,    -3,    -4,    -5,    -6,   391,    -7,    -8,
	   256,   -14,   -15,   -16,   -17,   -18,   -19,   -20,   -21,   -22,
	   -23,   -24,   -25,   -26,   -27,   -28,   -29,   -30,   -31,   -32,
	   -33,   -34,   -35,   -36,   -37,   -38,   -39,   -40,   -41,   -42,
	   -43,   -44,   -45,   -46,   -47,   -48,   -49,   -50,   -51,   -52,
	   -53,   -54,   -55,   -56,   -57,   -58,   -59,   -60,   -61,   -62,
	   -63,   -64,   -65,   -66,   -67,   -68,   -69,   -70,   -71,   -72,
	   -73,   -74,   -75,   -76,   -77,   -78,   -79,   -80,   -81,   -82,
	   -83,   -84,   -85,   -86,   -87,   -88,   -89,   -90,   -91,   -92,
	   -93,   -94,   -95,  -548,   408,   390,   420,   257,   -96,   -98,
	   295,  -103,  -129,  -142,  -144,  -146,  -148,  -151,  -152,   296,
	  -153,  -156,  -159,  -163,  -168,  -170,  -180,  -192,  -193,  -197,
	  -198,  -209,  -213,  -216,  -217,  -228,  -241,  -245,  -246,  -251,
	  -294,   285,   284,   286,  -305,  -313,  -315,  -317,  -319,  -337,
	  -339,  -345,  -347,  -348,  -349,   338,   330,  -351,  -353,   339,
	   337,   336,  -385,  -388,   316,  -389,  -392,  -398,  -401,  -403,
	  -404,  -405,  -418,  -432,  -435,  -438,   327,  -440,  -443,  -449,
	  -451,  -456,  -458,  -462,  -467,  -469,  -484,  -487,  -490,  -492,
	  -502,  -503,  -506,  -483,  -550,  -551,  -552,  -517,  -553,   258,
	   291,   259,   260,   262,   263,   304,   301,   302,   303,   305,
	   306,  -154,  -155,   287,   265,   297,   298,   267,   299,   269,
	   272,   271,  -202,   270,   273,   274,   275,   276,   277,   278,
	   300,   293,   294,   289,   288,   292,  -321,   313,  -341,  -346,
	   312,   310,   318,  -357,  -387,   317,  -390,  -394,   344,   331,
	   347,   346,  -408,   328,   343,   332,  -439,   335,  -445,   329,
	   333,   334,  -460,  -465,  -468,  -471,  -486,  -488,  -491,  -494,
	   358,  -504,  -508,   359,  -568,  -569,  -588,  -554,  -518,  -603,
	   264,   307,   308,   309,  -203,   311,   315,   314,   320,   319,
	   325,   326,   341,   324,   340,   348,   349,   351,   354,   350,
	   353,   352,   357,  -505,   355,  -570,  -530,   419,   421,  -523,
	  -524,  -525,   415,   356,   407,   424,   411,   410,  -528,  -534,
	  -526,   417,   413,   418,  -549,   316,   368,   -97,  -250,   389,
	  -128,   386,  -612,  -613,   -99,   368,  -104,  -128,  -130,  -104,
	  -143,   362,  -145,  -104,  -104,  -145,  -110,  -283,  -122,   283,
	  -157,  -111,  -158,  -128,   389,  -160,   281,  -164,   367,  -104,
	  -104,  -181,  -185,   275,   258,   262,   274,  -150,  -111,   360,
	  -145,  -186,   366,  -104,  -199,  -105,   399,  -104,  -214,  -104,
	  -215,   265,  -100,  -104,  -218,  -219,  -221,   281,   364,   368,
	  -229,    -9,   -10,   -13,   405,  -104,   -97,  -104,  -104,  -295,
	  -297,  -298,   289,  -312,  -128,   389,  -306,   399,  -298,  -316,
	   399,  -318,   399,  -320,  -323,   373,  -322,   389,  -128,  -340,
	   399,  -101,  -105,  -322,  -322,  -350,  -238,   389,  -612,   379,
	   375,   360,  -352,  -298,  -354,  -361,  -362,   321,  -386,  -384,
	  -316,   399,  -352,  -316,  -393,   399,  -399,   399,  -298,  -306,
	  -306,  -406,   399,  -419,   399,  -312,  -473,  -128,  -433,   399,
	  -436,   388,  -318,  -441,   378,   375,   383,  -442,   388,  -406,
	  -450,  -309,   387,   397,  -128,   386,   426,  -452,  -453,  -367,
	   323,  -457,   375,  -459,   399,  -463,   382,  -316,  -470,   399,
	  -363,  -318,   399,  -318,  -318,  -493,  -495,  -497,   368,   380,
	   381,    -9,    -9,  -507,   399,  -298,   258,   262,   274,   275,
	  -150,  -145,  -145,  -104,   281,   265,  -322,  -322,  -322,  -298,
	   333,  -298,  -391,  -298,  -391,  -409,   389,   388,   375,   383,
	  -391,  -409,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,
	  -384,  -298,  -589,  -590,  -559,   434,  -555,  -559,  -519,  -605,
	  -602,  -606,   399,  -607,   409,  -104,  -298,  -571,  -576,  -578,
	   433,  -531,  -576,   416,   410,  -527,   409,  -527,  -527,  -614,
	  -615,  -617,   402,   388,  -621,  -622,  -624,   422,  -100,  -105,
	  -105,  -132,   403,  -100,  -114,   398,  -147,  -149,  -150,  -126,
	  -250,  -128,  -175,  -284,  -285,  -104,  -128,   421,  -161,   367,
	  -104,  -169,  -105,  -171,   368,  -182,  -114,  -161,  -186,  -171,
	  -194,  -195,  -104,  -171,  -200,  -208,   374,  -204,  -205,  -134,
	  -104,  -161,  -104,  -104,  -101,  -196,   401,  -219,  -230,  -105,
	  -130,   -11,  -242,   370,  -247,  -248,  -249,  -178,  -161,  -250,
	  -119,  -128,  -179,  -122,   389,   369,  -252,  -101,  -105,  -296,
	  -299,   399,  -307,  -308,  -128,  -309,  -310,  -311,  -312,   426,
	   387,   397,  -314,  -514,  -343,  -515,  -546,  -516,  -547,  -238,
	   290,  -509,  -510,  -511,  -546,  -298,   342,  -324,   275,   291,
	  -338,   373,  -128,  -342,  -343,  -344,  -238,  -278,  -279,  -280,
	  -538,  -281,  -271,  -539,  -104,  -275,   290,   398,  -355,  -365,
	  -366,  -367,  -363,    -9,   400,  -395,  -396,  -397,  -238,  -400,
	   388,  -402,   398,  -407,  -412,   426,  -413,  -414,  -238,  -360,
	  -420,  -421,   283,  -298,  -434,   388,   399,  -298,  -298,  -444,
	  -446,   426,  -447,  -415,  -416,   388,   389,   334,   323,   375,
	   383,   377,   414,   386,  -453,  -454,  -455,   376,  -375,   388,
	  -298,  -352,   400,  -464,  -466,   363,  -316,   400,  -472,  -473,
	  -474,  -128,  -475,  -298,  -485,  -489,   283,   400,  -485,  -496,
	  -450,   345,  -498,  -500,   382,  -352,   400,  -298,   265,  -358,
	   389,  -128,  -410,  -410,  -298,  -298,  -298,  -298,  -298,  -298,
	  -298,  -298,  -298,  -581,   398,  -591,  -561,   409,  -593,  -556,
	  -560,  -561,   423,  -520,  -581,  -605,   428,  -298,  -572,  -577,
	  -581,  -579,  -580,   386,   426,   399,  -532,  -574,   413,  -529,
	   399,  -533,  -531,  -616,  -619,   399,  -618,  -621,   388,  -623,
	   401,   399,  -101,  -106,  -111,  -112,  -128,  -113,  -104,  -131,
	  -133,  -111,  -134,  -104,  -104,  -102,  -179,  -104,  -114,  -114,
	  -258,   428,  -104,  -165,   365,  -110,  -145,  -172,  -175,  -104,
	   271,  -104,  -185,  -114,  -162,   365,   428,  -104,  -201,  -127,
	   362,   388,  -107,  -114,   400,  -206,  -207,   365,  -115,   365,
	   428,  -104,  -102,  -104,  -101,  -102,  -232,  -233,  -234,  -538,
	  -235,  -237,   404,   -12,    -2,   406,  -243,  -111,   -97,  -128,
	  -249,  -174,  -104,  -178,  -175,  -175,  -102,  -111,  -300,  -301,
	  -303,  -298,   290,   377,   400,   398,   387,   397,  -298,   400,
	   398,   400,  -304,   365,   428,   399,  -239,  -277,   403,   400,
	   398,   400,  -304,   399,  -325,  -105,   281,   388,   400,   398,
	   400,  -239,  -107,  -114,  -107,  -282,  -115,   361,   399,  -196,
	  -104,  -150,  -298,  -356,  -381,  -366,  -368,  -370,  -371,   376,
	   384,   385,   377,  -375,   388,  -364,    -9,   400,   398,  -304,
	  -239,   398,  -304,  -298,   400,   398,  -415,  -239,  -422,  -423,
	  -424,  -426,   388,   389,   400,  -304,  -419,  -298,   400,   398,
	  -304,   399,  -369,   398,   403,  -372,   389,   360,  -128,  -373,
	  -376,   399,   386,  -128,   400,  -318,   400,   398,  -304,   399,
	  -499,  -501,   363,  -316,   400,  -359,   399,  -411,   389,   386,
	  -128,  -461,  -450,  -461,  -461,  -358,  -461,  -461,  -461,  -592,
	  -591,  -590,  -587,  -580,  -594,   423,  -557,  -562,  -563,    -8,
	   256,  -550,  -551,  -564,  -565,  -554,  -596,  -530,   425,  -559,
	  -521,  -535,  -606,  -604,   400,  -608,  -579,  -609,   389,  -610,
	   426,   387,   397,  -611,   411,   409,   424,   407,   410,  -573,
	  -582,  -584,  -561,   386,  -527,  -575,  -583,  -586,  -561,  -529,
	  -620,   399,  -616,  -102,  -107,  -114,  -115,  -107,  -114,  -167,
	  -111,  -105,  -253,  -236,  -128,   430,  -260,  -261,  -270,  -271,
	  -262,  -125,  -264,  -124,  -272,  -273,  -274,  -266,   426,  -265,
	  -250,  -291,  -292,  -119,  -293,   372,  -276,  -104,   432,   424,
	   407,   387,   396,   397,   404,  -126,  -284,  -286,   386,  -160,
	   289,  -105,  -162,  -166,  -167,  -111,  -107,  -160,  -183,  -105,
	  -195,  -104,  -110,  -119,  -128,  -205,  -119,  -135,  -137,  -138,
	  -104,   424,   407,  -210,  -105,  -102,  -231,  -240,  -107,  -114,
	  -107,  -115,  -238,  -126,  -174,  -104,  -107,   400,  -302,   398,
	  -304,  -308,  -515,  -298,   326,  -540,  -541,   389,  -543,  -544,
	   388,  -238,  -511,  -512,  -513,  -128,  -309,  -310,  -311,  -312,
	   280,  -298,  -102,  -335,  -336,  -104,  -271,  -111,  -344,  -280,
	  -236,  -105,  -128,   280,  -540,  -382,  -383,   322,  -369,   398,
	   375,   383,  -372,  -372,  -372,  -374,   388,   389,  -396,  -298,
	  -400,  -298,  -413,  -304,   398,  -425,   399,   428,  -298,   398,
	  -447,  -448,   389,   386,   387,   397,  -128,   367,   400,  -417,
	   389,   386,   384,   385,   330,   376,   343,   383,   375,  -128,
	    -9,  -455,  -373,  -373,  -377,  -378,  -379,   388,   389,   334,
	   323,  -475,  -476,  -444,  -318,  -360,  -591,  -559,  -558,  -520,
	  -566,  -555,  -597,  -531,  -522,  -536,    -2,  -537,   345,   387,
	   397,   416,   410,  -581,  -585,   434,  -587,  -533,  -581,  -585,
	  -531,  -108,   364,  -127,  -113,  -116,  -118,  -119,  -120,  -104,
	   424,   407,  -110,  -134,  -254,   431,  -167,  -236,  -167,  -255,
	  -207,  -263,  -125,  -257,  -258,  -259,   433,   434,   435,   429,
	   427,  -239,  -236,  -236,  -107,  -105,  -105,   399,  -238,  -273,
	  -119,  -287,  -128,  -289,  -104,  -130,  -173,  -161,  -184,  -111,
	  -187,  -171,  -145,  -196,  -136,  -122,  -139,   430,  -105,  -107,
	  -114,  -200,  -211,  -104,  -220,  -222,   279,  -220,  -234,  -236,
	   280,  -239,  -244,  -104,  -301,  -298,   399,   398,  -545,   399,
	  -239,   400,  -326,  -220,  -107,  -114,  -282,  -115,  -236,  -384,
	    -9,  -370,  -372,  -372,  -373,   400,  -423,  -428,  -429,  -430,
	   388,   389,  -427,   388,   389,   400,  -437,  -128,  -239,   400,
	   400,   398,  -304,  -477,  -480,   424,   407,   389,  -128,   400,
	  -537,  -562,  -567,  -595,  -591,  -598,  -600,  -601,    -8,   256,
	   414,  -582,  -531,  -583,  -587,  -109,  -119,  -128,  -117,  -122,
	  -105,  -104,  -119,  -121,   280,  -167,  -107,  -107,  -236,  -256,
	  -253,  -236,  -236,  -107,  -114,  -236,  -238,  -239,  -288,   388,
	  -107,  -114,  -212,  -290,   388,  -105,  -174,  -176,  -177,  -104,
	  -102,  -188,  -191,   371,  -104,  -150,  -119,  -107,  -104,  -121,
	  -140,  -141,  -122,   430,   280,  -126,  -126,  -201,  -107,  -114,
	  -212,  -110,  -223,  -225,   361,  -126,  -298,  -542,  -544,  -304,
	  -327,  -328,  -330,   373,  -336,  -236,   280,  -372,   400,   398,
	   428,   400,  -239,  -378,  -380,   389,   386,  -128,  -478,   399,
	  -554,  -556,  -581,  -599,  -520,  -597,  -110,  -121,  -126,   280,
	  -236,  -267,  -225,  -239,  -104,  -145,  -114,  -132,  -189,  -127,
	  -104,  -150,   388,   388,  -107,  -110,  -104,  -224,  -226,  -227,
	  -104,   400,   400,  -545,  -512,  -329,  -186,  -331,   388,  -429,
	  -431,  -309,  -312,  -310,  -311,  -128,  -360,  -479,   283,  -481,
	   430,   386,  -557,  -591,  -537,  -600,  -557,  -105,  -107,  -114,
	  -107,  -114,  -268,   388,  -269,  -236,   400,  -212,  -107,  -177,
	  -104,  -190,  -161,  -126,  -212,  -114,  -132,  -196,  -105,   388,
	   280,  -482,   283,   280,   400,   398,  -558,  -596,  -599,  -123,
	  -124,  -125,  -126,  -236,  -102,  -114,  -110,  -104,  -132,  -226,
	  -104,  -104,  -332,  -333,  -334,  -275,  -104,  -150,   388,   386,
	  -537,  -537,  -107,  -124,  -107,  -107,  -107,  -236,  -171,  -126,
	  -107,  -114,  -334,  -104,   400,  -554,  -601,  -104,  -171,  -333,
	  -126,  -132,  -126
};

short yydef[] = {
	     2,    -2,     1,     3,     4,     5,     6,     7,     8,     9,
	    10,    15,    16,    17,    18,    19,    20,    21,    22,    23,
	    24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
	    34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
	    44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
	    54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
	    64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
	    74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
	    84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
	    94,    95,    96,   975,   977,   973,   974,    97,  1098,   457,
	   103,  1098,  1098,   156,  1098,  1098,    -2,   417,   169,   172,
	   173,  1098,   461,   192,  1098,  1098,  1098,  1098,     0,  1098,
	   258,  1098,  1098,  1098,    -2,   319,  1098,  1098,  1098,  1098,
	  1098,   502,   503,   504,     0,  1098,     0,     0,     0,  1098,
	     0,     0,  1098,  1098,  1098,   577,     0,  1098,   593,   642,
	   643,   644,   910,  1098,   651,     0,     0,     0,  1098,     0,
	     0,     0,  1098,   741,   746,     0,   751,   754,     0,  1098,
	     0,   787,   794,   798,     0,   811,   894,     0,     0,     0,
	     0,     0,   871,  1098,   979,   980,   981,   982,   983,   101,
	     0,   105,   131,   153,   161,   162,   163,   164,   165,   170,
	   171,    -2,  1098,   183,   188,   190,   196,   200,   215,   244,
	   246,   252,    -2,   257,   278,   284,   287,   323,   329,   331,
	   345,   489,   506,   516,   518,   520,  1098,   554,  1098,  1098,
	   570,   572,   574,  1098,  1098,   650,    -2,    -2,   664,   668,
	   670,   672,     0,   713,   739,   744,    -2,   753,     0,   775,
	   777,   786,  1098,  1098,  1098,  1098,  1098,  1098,  1098,  1098,
	   861,   910,  1098,   831,  1002,  1003,     0,     0,   927,     0,
	   178,   179,   180,   181,  1098,   523,   560,   568,   585,   647,
	   654,   657,   676,   750,   762,   791,   797,   805,   808,   834,
	   837,   843,   846,  1098,   868,  1008,  1025,  1065,  1067,   929,
	   930,   931,  1062,   865,  1050,  1069,     0,  1072,     0,     0,
	     0,  1063,  1055,  1064,   976,   978,    99,    98,   484,   485,
	   486,   478,  1096,     0,  1098,   456,     0,   475,     0,   132,
	  1098,   155,   157,   476,  1098,   159,   168,   416,  1098,   459,
	   182,   184,   185,   487,   186,     0,   460,  1098,   191,   197,
	     0,     0,   216,   218,   219,   220,   221,   222,   223,   434,
	   243,  1098,   451,     0,     0,  1098,   449,     0,   277,    -2,
	  1098,   282,     0,   473,   307,     0,  1098,   306,   290,   291,
	   304,   320,    13,    12,   944,   322,   328,    -2,     0,     0,
	   490,   491,   492,   873,   874,   872,   505,  1098,   880,   517,
	  1098,   519,  1098,   521,     0,   528,    -2,   544,   545,   558,
	  1098,   566,  1098,   569,   571,   573,   575,   576,  1095,   578,
	   581,   579,   580,   878,   598,   591,   894,   594,     0,   648,
	   908,  1098,   649,   652,   655,  1098,   663,     0,   925,   669,
	   671,  1098,   675,   715,  1098,   875,   876,   877,   738,     0,
	     0,   745,   748,   752,   755,  1098,  1098,   758,   759,     0,
	   774,   888,   889,   890,   891,   882,     0,   776,   778,     0,
	   602,   785,  1098,   789,  1098,   800,     0,   803,   806,  1098,
	   838,   892,  1098,   838,   841,   844,  1098,   854,   849,   852,
	   853,   860,   862,   866,  1098,  1098,   102,   154,   285,   288,
	   175,   176,   177,   255,     0,   272,   522,   559,   567,    -2,
	   584,   646,   653,   920,   656,   681,   677,   678,   679,   680,
	   749,   681,  1098,  1098,  1098,  1098,  1098,  1098,  1098,  1098,
	   863,  1098,  1029,     0,  1035,  1052,   988,     0,  1049,  1074,
	     0,  1076,  1058,     0,  1078,   256,  1098,  1006,  1012,     0,
	  1060,  1007,  1026,  1070,  1071,   942,   938,  1025,   932,  1097,
	  1102,     0,  1101,  1107,  1109,     0,  1112,  1114,     0,    -2,
	  1098,  1098,   436,   350,  1098,   437,   158,   166,   167,   481,
	   479,   480,   418,   419,     0,   422,    -2,   972,  1098,   452,
	   447,   417,  1098,  1098,   455,  1098,  1098,   224,   225,   226,
	   245,   247,     0,  1098,   267,     0,   439,     0,   260,   264,
	   263,  1098,   279,   280,   350,  1098,   453,     0,   350,   316,
	   289,     0,  1098,   324,   330,    -2,    -2,   339,   340,   341,
	   342,   343,  1098,  1098,   483,   458,   350,   346,  1098,   488,
	  1098,   494,     0,   508,   510,   511,   512,   513,   514,     0,
	   884,   886,    -2,     0,     0,   913,   958,     0,     0,   396,
	   960,     0,     0,   897,   959,     0,     0,     0,   526,   529,
	   553,     0,   557,     0,     0,   563,   396,     0,     0,   410,
	   946,     0,   413,     0,   415,  1098,   949,  1098,   638,   597,
	   599,   614,   596,   645,   909,     0,   659,     0,   396,     0,
	     0,  1098,   924,     0,   686,   687,   688,     0,   396,   712,
	   714,     0,   717,     0,     0,   742,  1098,   756,  1098,     0,
	   763,   764,   765,     0,   692,   695,   696,   697,   698,   699,
	   700,   701,   971,   883,   779,   604,   781,  1098,   784,    -2,
	   788,     0,   793,   795,     0,   802,   799,   809,     0,   812,
	   813,    -2,   814,     0,   832,     0,   840,   893,   835,   847,
	   850,   851,   857,     0,   856,     0,   870,   830,   271,   590,
	   586,   587,    -2,    -2,    -2,    -2,    -2,    -2,    -2,   836,
	    -2,   845,    -2,     0,  1051,  1030,  1022,  1061,     0,     0,
	     0,   986,   987,   940,     0,     0,     0,   864,     0,  1009,
	  1013,  1010,  1011,  1056,     0,  1024,     0,     0,  1054,   933,
	   943,   935,   942,  1099,  1103,  1105,  1102,  1110,  1108,  1111,
	  1115,  1113,   350,     0,   106,   107,    -2,   109,     0,     0,
	   134,   135,   136,     0,   133,   152,  1098,   477,  1098,  1098,
	   461,   406,     0,  1098,   446,   195,     0,   461,   201,    -2,
	   203,   228,   217,  1098,  1098,   440,   441,   251,   417,  1098,
	   438,   266,   259,  1098,   454,   262,     0,   448,  1098,   442,
	   443,   273,   283,   474,   350,   318,     0,     0,   310,   947,
	     0,  1098,   315,    11,    14,   945,   321,   325,  1098,    -2,
	   211,   335,    -2,   210,   212,   213,   344,     0,     0,   495,
	     0,   498,   499,   500,   507,  1098,   885,   887,   881,   911,
	  1098,   912,  1098,   922,   923,     0,   917,  1098,   398,   895,
	  1098,   896,  1098,  1098,   350,  1098,   527,   556,   561,  1098,
	   562,   565,   408,  1098,   409,  1098,   444,   445,     0,   390,
	   388,   389,   879,   582,   640,   600,   604,   605,     0,  1098,
	  1098,  1098,     0,   613,    -2,   592,   595,   658,  1098,  1098,
	   662,     0,  1098,   667,   673,  1098,   690,     0,   716,   718,
	   727,     0,   722,   723,   962,  1098,     0,   757,   760,     0,
	  1098,  1098,     0,     0,   603,   625,   616,   617,   618,   621,
	   625,     0,   623,   624,   792,   801,   810,  1098,   816,     0,
	   848,     0,   859,   855,   869,   715,   589,   682,   683,   684,
	   685,   790,   918,   796,   804,   807,   833,   842,   867,  1031,
	  1032,     0,  1040,  1023,     0,  1068,    -2,   990,   992,   993,
	   994,   995,   996,   997,   998,  1000,  1047,  1025,  1073,   985,
	   928,     0,  1077,  1075,  1066,  1079,  1080,  1081,  1082,  1083,
	     0,  1084,  1086,  1088,     0,  1091,  1092,  1093,  1094,  1004,
	  1014,  1027,  1022,  1057,  1025,  1005,  1016,  1027,  1025,   936,
	  1104,  1106,  1100,   100,     0,  1098,  1098,   417,  1098,   348,
	   349,  1098,  1098,     0,    -2,   400,   361,   362,   382,   383,
	   363,  1098,  1098,   384,   385,   386,   387,     0,   402,     0,
	   465,   466,   467,   468,   469,     0,  1098,   373,   375,   967,
	   968,   470,   472,   471,   394,   482,   420,   421,   424,  1098,
	   427,  1098,  1098,   189,   193,   194,   198,   205,    -2,  1098,
	   248,   249,   253,   268,   269,   261,   265,   148,   139,     0,
	   143,   965,   966,     0,  1098,   294,   305,   294,   308,   316,
	   309,  1098,   396,   326,   334,    -2,   347,   493,  1098,   501,
	  1098,   509,   914,   915,     0,   961,     0,   951,   952,   956,
	   955,   397,   898,   899,   901,   396,   903,   904,   905,   906,
	   907,     0,   294,     0,   547,     0,   550,   551,   564,   411,
	   412,  1098,    -2,   414,   948,   637,   910,   641,     0,   614,
	  1098,  1098,   625,   609,   610,   612,   619,   620,   660,   661,
	     0,   666,   689,   691,     0,   720,     0,     0,     0,  1098,
	   766,   767,   768,   769,   770,   771,   396,   773,   693,     0,
	   702,   703,   704,   705,   706,   707,   708,   709,   710,   711,
	   780,   782,   783,   622,     0,   627,     0,   630,   631,   632,
	   633,   815,  1098,     0,   858,   583,  1033,  1034,     0,     0,
	  1036,  1001,     0,  1007,   926,   937,   941,   939,  1053,  1085,
	  1087,  1089,  1090,     0,  1018,  1028,  1025,   934,     0,  1019,
	  1022,  1098,   126,   127,   110,   118,   112,   113,    -2,   125,
	   963,   964,   130,   137,  1098,   399,     0,     0,   352,  1098,
	   359,  1098,  1098,   356,   357,   358,   403,   404,   405,   407,
	   401,   391,   366,     0,   371,  1098,   372,  1098,   396,   393,
	   425,   431,   429,     0,   462,     0,   211,  1098,   350,   229,
	   237,  1098,     0,  1098,   138,     0,   145,     0,  1098,   140,
	  1098,   267,     0,   462,   417,   297,   295,   317,   311,   312,
	   313,   314,  1098,   338,   496,   497,  1098,     0,   953,   957,
	   902,     0,   530,   525,   546,  1098,  1098,   444,     0,   639,
	   601,   606,   607,  1098,   608,   665,   719,     0,   728,     0,
	   731,   732,   721,   724,   725,   740,     0,   396,   772,   694,
	   626,     0,  1098,   819,   818,   969,   970,   822,   823,   839,
	     0,   991,   988,  1037,  1038,    -2,  1042,  1047,  1045,  1046,
	  1059,  1015,  1020,  1017,  1021,   417,   128,   129,   111,     0,
	  1098,   115,   116,   117,   450,   353,   351,   367,   354,     0,
	   360,   364,   365,   368,  1098,   376,   396,   392,   426,   430,
	   428,  1098,   432,   463,   464,  1098,   199,   204,   206,   208,
	   214,   240,  1098,   238,   231,   232,   233,   227,   250,   144,
	   146,   147,     0,     0,   149,   142,     0,   417,   274,  1098,
	   275,   286,  1098,   296,   435,   327,     0,     0,   956,  1098,
	   524,   536,     0,   533,   548,   549,   552,   611,   726,     0,
	  1098,   715,   747,   628,   629,   634,   635,   636,   824,     0,
	   984,     0,     0,     0,     0,     0,   104,   120,     0,   355,
	     0,   381,  1098,     0,   462,     0,  1098,  1098,   242,  1098,
	   235,   236,   150,   151,   141,   270,   462,   293,   298,   300,
	   302,   916,   950,   954,   900,   531,     0,   532,   534,   729,
	   730,   733,   734,   735,   736,   737,   743,   817,     0,   829,
	     0,     0,    -2,  1039,     0,  1043,    -2,     0,   123,  1098,
	   369,  1098,   350,   380,   377,   378,   395,   433,   417,   207,
	   209,   230,  1098,     0,   276,  1098,  1098,  1098,  1098,   535,
	   825,   826,     0,   827,   820,     0,     0,  1041,     0,     0,
	   121,     0,     0,     0,     0,  1098,   187,     0,  1098,   299,
	   301,   303,     0,   538,   540,  1098,   542,   543,   828,     0,
	     0,     0,   119,   122,   124,   370,   374,   379,  1098,     0,
	   537,  1098,   541,   542,   821,   999,  1044,   241,  1098,   539,
	     0,  1098,   239
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
#line 380 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 3:
#line 386 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if ((gr->gr_flag & GR_NOSYNC) == 0)
		    {
			if (gr->gr_flag & GR_REPEAT)
			    rep_close( IISYNCUP, NULL, FALSE );
			else
			{
			    if (gr->gr_flag & GR_COPY_PROG)
			    /*
			    ** If COPY FROM/INTO PROGRAM check that handler
			    ** has been declared.
			    */
			    {
				db_send();
				if ((gr->gr_flag & GR_SEENCPYHND) == 0)
				{
				    er_write(E_EQ0091_COPYHANDLER, EQ_ERROR, 0);
				    eqtl_init(IISETSQL);
				    eqtl_add(ERx("0"), (PTR)0, (PTR)0, (PTR)0,
					T_NONE, ERx("copyhandler"));
				}
				eqgen_tl(IISETSQL);
				gr->gr_flag &= ~(GR_SEENCPYHND|GR_COPY_PROG);
			    }
			    db_close( IISYNCUP );
			}
		    }
		    gr->gr_flag &= ~(GR_QUEL | GR_REPEAT | GR_NOSYNC);
		    gen_eqstmt( G_CLOSE, (char *)0 );
		}
		break;
	case 4:
#line 417 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_flag &= ~GR_EQUEL;
		    gen_eqstmt( G_CLOSE, (char *)0 );
		}
		break;
	case 5:
#line 422 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if ((gr->gr_flag & GR_DECLCSR) == 0)
			gen_eqstmt( G_CLOSE, (char *)0 );
		    gr->gr_flag &= ~(GR_QUEL|GR_REPEAT|GR_NOSYNC|
				     GR_CURSOR|GR_DECLCSR|GR_REPLCSR);
		}
		break;
	case 6:
#line 429 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_flag &= ~GR_EQUEL;
		    gen_eqstmt( G_CLOSE, (char *)0 );
		}
		break;
	case 7:
#line 434 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (*(yypvt[-0].s) == 'i')	/* "include" or "<EOF>" */
			inc_push_file();
		    else
			inc_pop_file();
		}
		break;
	case 10:
#line 443 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    /*
		    ** Cursors implement their own recovery;
		    ** they just throw away whatever BE commands
		    ** they've been buffering.
		    ** QUEL statements make sure there's a syntax
		    ** error in the BE commands and then flush it.
		    ** QUEL and EQUEL commands (except for DECLARE CURSOR)
		    ** then close off the host-language block.  DECLARE CURSOR
		    ** is different as it does not generate any code, and thus
		    ** never started a host-language block.
		    */
		    if (gr->gr_flag & GR_CURSOR)
		    {
			db_op( ERx("...ERR") );
			db_send();
			ecs_query( NULL, ECS_STOP_QUERY );	/* stop query */
			if (gr->gr_flag & GR_REPEAT)
			{
			    rep_close( IICSQUERY, ecs_id_curcsr(), FALSE );
			    ecs_query( NULL, ECS_END_QUERY ); 	/* end query */
			}
		    } else
		    {
			if (gr->gr_flag & GR_QUEL)
			{
			    db_op( ERx("...ERR") );
			    if (gr->gr_flag & GR_REPEAT)
				rep_close( IISYNCUP, NULL, FALSE );
			    else
				db_close( IISYNCUP );
			}
		    }

		    if (gr->gr_flag & (GR_QUEL|GR_EQUEL|GR_CURSOR))
			if ((gr->gr_flag & GR_DECLCSR) == 0)
			    gen_eqstmt( G_CLOSE, (char *)0 );

		    arg_free();
		    id_free();
		    gen_host( G_H_NEWLINE, (char *)0 );
		    gr_mechanism( GR_ERROR );
		    gr->gr_flag &= ~(GR_QUEL|GR_REPEAT|GR_NOSYNC|
				     GR_CURSOR|GR_DECLCSR|GR_REPLCSR);
		}
		break;
	case 11:
#line 499 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    lbl_exit_block();
		    gr_mechanism( GR_BLOCK, GR_BLKFALSE ); /* Symtab updates */
		}
		break;
	case 12:
#line 505 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    lbl_enter_block();
		    lbl_set_mode( gr->gr_mode );
		    lbl_adset( LBL_NOLEV, gr->gr_adjective );
		    gr->gr_mode = LBLmNONE;
		    gr->gr_adjective = LBLaNONE;
		    gr_mechanism( GR_BLOCK, GR_BLKTRUE ); /* Symtab updates */
		    gr->gr_flag &= ~(GR_QUEL|GR_EQUEL);
		}
		break;
	case 14:
#line 517 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 97:
#line 647 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    arg_int_add( IIxactABORT );
		    gen_call( IIXACT );
		    gr->gr_flag |= GR_NOSYNC;      /* No IIsync after this */
		}
		break;
	case 99:
#line 656 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 101:
#line 668 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 102:
#line 672 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-0].s );
		}
		break;
	case 103:
#line 680 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    arg_int_add( IIxactBEGIN );
		    gen_call( IIXACT );
		    gr->gr_flag |= GR_NOSYNC;      /* No IIsync after this */
		}
		break;
	case 105:
#line 694 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 126:
#line 727 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 129:
#line 734 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_sym)
		    {
			db_var( DB_STRING, gr->gr_sym, gr->gr_id, (SYM *)0,
				(char *)0, (char *)0 );
			if (gr->gr_type != T_CHAR && gr->gr_type != T_UNDEF)
			    er_write( E_EQ0067_grSTRVAR, EQ_ERROR, 1,
								gr->gr_id );
		    }
		    else if (!STbcompare(gr->gr_id, 0, ERx("program"), 0, TRUE))
		    {
			db_key(gr->gr_id);
			gr->gr_flag |= GR_COPY_PROG;
		    }
		    else
		    {
			er_write( E_EQ0066_grSTR, EQ_ERROR, 1, gr->gr_id );
			db_sconst( gr->gr_id );
		    }
		}
		break;
	case 131:
#line 762 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 149:
#line 793 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		    if (STbcompare(yypvt[-1].s, 0, ERx("not"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
						    ERx("CREATE"), ERx("NOT") );
		}
		break;
	case 150:
#line 802 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( ERx("default") );
		    if (STbcompare(yypvt[-0].s, 0, ERx("default"), 0, TRUE) != 0)
			er_write(E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
						ERx("CREATE"), ERx("DEFAULT") );
		}
		break;
	case 151:
#line 810 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( ERx("default") );
		    if (STbcompare(yypvt[-1].s, 0, ERx("not"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
						    ERx("CREATE"), ERx("NOT") );
		    else
		        if (STbcompare(yypvt[-0].s, 0, ERx("default"), 0, TRUE) != 0)
		  	    er_write(E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
						ERx("CREATE"), ERx("DEFAULT") );
		}
		break;
	case 153:
#line 828 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 154:
#line 832 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-0].s );
		}
		break;
	case 161:
#line 852 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 162:
#line 856 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 163:
#line 862 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 164:
#line 866 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 165:
#line 871 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 170:
#line 895 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 171:
#line 900 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 172:
#line 907 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    arg_int_add( IIxactCOMMIT );
		    gen_call( IIXACT );
		    gr->gr_flag |= GR_NOSYNC;      /* No IIsync after this */
		}
		break;
	case 173:
#line 919 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 178:
#line 929 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 179:
#line 935 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 180:
#line 939 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 181:
#line 943 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 183:
#line 953 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		}
		break;
	case 186:
#line 961 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 188:
#line 973 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 190:
#line 983 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 196:
#line 1002 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 200:
#line 1019 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 203:
#line 1026 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 215:
#line 1056 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 218:
#line 1064 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 219:
#line 1068 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 220:
#line 1072 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 221:
#line 1076 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 238:
#line 1102 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 243:
#line 1116 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1, yypvt[-1].s );
		}
		break;
	case 244:
#line 1121 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 246:
#line 1131 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 252:
#line 1147 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 255:
#line 1182 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (   (STbcompare(yypvt[-1].s, 0, ERx("table"), 0, TRUE) != 0)
			&& (STbcompare(yypvt[-1].s, 0, ERx("view"), 0, TRUE) != 0)
		       )
		    {
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
				  ERx("REGISTER"), ERx("TABLE") );
		    }
		}
		break;
	case 256:
#line 1192 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 257:
#line 1197 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 266:
#line 1214 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (  (STbcompare(yypvt[-0].s, 0, ERx("import"), 0, TRUE) != 0)
			&&(STbcompare(yypvt[-0].s, 0, ERx("link"), 0, TRUE) != 0) 
		       )
		    {
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					ERx("REGISTER"), ERx("LINK") );
		        db_key( ERx("link") );
		    }
		    else
		    {
			db_key( yypvt[-0].s );
		    }
		}
		break;
	case 269:
#line 1232 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_sym)
		    {
			db_var( DB_STRING, gr->gr_sym, gr->gr_id, (SYM *)0,
				(char *)0, (char *)0 );
			if (gr->gr_type != T_CHAR && gr->gr_type != T_UNDEF )
			    er_write( E_EQ0067_grSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
		    }
		    else
		    {
			db_key( gr->gr_id );
		    }
		}
		break;
	case 271:
#line 1252 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-2].s );
		    db_key( ERx("unique") );
		    db_key( ERx("index") );
		}
		break;
	case 272:
#line 1258 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-1].s );
		    db_key( ERx("index") );
		}
		break;
	case 278:
#line 1282 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 279:
#line 1287 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (   (STbcompare(yypvt[-1].s, 0, ERx("table"), 0, TRUE) != 0)
			&& (STbcompare(yypvt[-1].s, 0, ERx("view"), 0, TRUE) != 0)
		       )
		    {
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
				    ERx("REMOVE"), ERx("TABLE") );
		    }
		}
		break;
	case 282:
#line 1300 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( ERx("index") );
		}
		break;
	case 284:
#line 1310 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 285:
#line 1314 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-0].s );
		}
		break;
	case 287:
#line 1332 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 288:
#line 1336 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-0].s );
		}
		break;
	case 293:
#line 1348 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 1;
		}
		break;
	case 294:
#line 1352 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 0;
		}
		break;
	case 295:
#line 1357 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 304:
#line 1397 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    /* Close retrieve loop */
		    gen_loop( G_CLOSE, L_RETBEG, L_RETFLUSH,
			lbl_current(LBL_RET), IINEXTGET, C_0, 0 );
		    gen_call( IIFLUSH );
		    gen_label( G_NOLOOP, L_RETEND, lbl_current(LBL_RET) );
		    gr->gr_flag |= GR_NOSYNC;      /* No IIsync after this */
		    gr->gr_flag &= ~GR_RETRIEVE;   /* Done with Ret block */
		}
		break;
	case 305:
#line 1408 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	cur_ret;

		    cur_ret = lbl_next( LBL_RET );
		    gr->gr_mode = LBLmRETLOOP;
		    if (gr->gr_flag & GR_REPEAT)
		    {
			gr->gr_flag &= ~GR_REPEAT;
			rep_close( IIRETINIT, NULL, FALSE );
		    }
		    else
			db_close( IIRETINIT );
		    gen_if_goto( IIERRTEST, C_NOTEQ, 0, L_RETEND, cur_ret );
		    gen_label( G_LOOP, L_RETBEG, cur_ret );
		    gen_loop( G_OPEN, L_RETBEG, L_RETFLUSH, cur_ret,
			      gr->gr_func, C_NOTEQ, 0 );
		    ret_close();
		    gen_if_goto( IIERRTEST, C_NOTEQ, 0, L_RETBEG, cur_ret );
		    gr->gr_flag &= ~GR_QUEL;
		    gr->gr_flag |= GR_RETRIEVE;	   /* Prevent nesting Quel */
		}
		break;
	case 306:
#line 1431 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 308:
#line 1437 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_func = IINEXTGET;
		}
		break;
	case 309:
#line 1441 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_func = IITUPGET;
		}
		break;
	case 313:
#line 1450 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 314:
#line 1455 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    ret_add( gr->gr_sym, gr->gr_id, gr->gr_nlsym, gr->gr_nlid,
				(char *)0, 0 );
		}
		break;
	case 315:
#line 1461 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    er_write( E_EQ0065_grRETREPEAT, EQ_ERROR, 0 );
		}
		break;
	case 317:
#line 1467 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_flag &= ~GR_TUPSORT;
		}
		break;
	case 318:
#line 1472 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_flag |= GR_TUPSORT;
		}
		break;
	case 323:
#line 1486 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 324:
#line 1491 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 326:
#line 1497 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    /* Bug #6749 - Need a space for run-time */
		    db_op(ERx(" "));
		}
		break;
	case 329:
#line 1509 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 331:
#line 1519 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		    gr->gr_func = DB_STRING;
		}
		break;
	case 340:
#line 1535 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_func = DB_ID;		/* ON tablename */
		}
		break;
	case 343:
#line 1542 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_sym)
		    {
			db_var(gr->gr_func, gr->gr_sym, gr->gr_id, (SYM *)0, 
				(char *)0, (char *)0);
			if (gr->gr_type != T_CHAR && gr->gr_type != T_INT &&
			     gr->gr_type != T_UNDEF )
			    er_write( E_EQ0060_grINTSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
		    }
		    else
			db_key( gr->gr_id );
		}
		break;
	case 345:
#line 1562 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 348:
#line 1580 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 0;
		}
		break;
	case 349:
#line 1584 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 1;
		}
		break;
	case 350:
#line 1588 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 0;
		}
		break;
	case 355:
#line 1600 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 361:
#line 1620 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = yypvt[-0].i;		/* Not really an expression */
		}
		break;
	case 362:
#line 1624 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 3;
		}
		break;
	case 375:
#line 1658 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 380:
#line 1669 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (STbcompare(yypvt[-0].s, 0, ERx("only"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
						    ERx("WHERE"), ERx("ONLY") );
		    db_key( yypvt[-0].s );
		}
		break;
	case 382:
#line 1687 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 0;
		}
		break;
	case 383:
#line 1691 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    /* Note that gr_sym will be left around after second name */
		    yyval.i = gr->gr_sym ? 2 : 1;
		}
		break;
	case 387:
#line 1700 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_flag & GR_REPEAT)
		    {
			if (gr->gr_nlsym)
			    rep_add(gr->gr_sym,gr->gr_id,gr->gr_nlsym,
					gr->gr_nlid,(char *)0);
			else
			    rep_add( gr->gr_sym,gr->gr_id,(SYM *)0,
					(char *)0,(char *)0 );
		    }
		}
		break;
	case 388:
#line 1721 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = gr->gr_sym ? (char *)0 : yypvt[-0].s;
		    if (gr->gr_flag & GR_REPLCSR)
			er_write( E_EQ0413_csRANGEREPL, EQ_ERROR, 3, yypvt[-1].s,
							yypvt[-0].s, ecs_namecsr() );
		}
		break;
	case 389:
#line 1728 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = (char *)0;
		    if (gr->gr_flag & GR_REPLCSR)
			er_write( E_EQ0413_csRANGEREPL, EQ_ERROR, 3, yypvt[-1].s,
							yypvt[-0].s, ecs_namecsr() );
		}
		break;
	case 390:
#line 1736 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_flag & GR_CURSOR)
		    {
			/*
			** Variables aren't table names so we don't add them.
			** Note that this can give us cursors w/o any table
			** names.  This shouldn't cause any trouble.  Note
			** that gr_sym will be pointing at the table name if
			** it was specified by a variable.
			*/
			if (!gr->gr_sym && ((gr->gr_flag & GR_REPLCSR) == 0))
			    ecs_addtab( NULL, gr->gr_id );
		    }
		}
		break;
	case 391:
#line 1755 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_sym)
		    {
			db_var( DB_REG, gr->gr_sym, gr->gr_id, gr->gr_nlsym,
				gr->gr_nlid, (char *)0 );
		    }
		    else
		    {
			if (gr->gr_nlsym)	/* Ignore indicator */
			    er_write( E_EQ0072_grNONULL, EQ_ERROR, 0 );
			db_key( gr->gr_id );
		    }
		}
		break;
	case 392:
#line 1773 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_var( DB_REG, gr->gr_sym, gr->gr_id, gr->gr_nlsym,
			    gr->gr_nlid, (char *)0 );
		}
		break;
	case 394:
#line 1780 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_flag & GR_REPEAT)
			rep_param();
		    else
			er_write( E_EQ0376_repARG, EQ_ERROR, 0 );
		}
		break;
	case 395:
#line 1791 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_type == T_CHAR)
		    {
			db_var( DB_NOTRIM, gr->gr_sym, gr->gr_id, gr->gr_nlsym,
				gr->gr_nlid, (char *)0 );
		    }
		    else 
		    {
			if (gr->gr_type != T_UNDEF)
			    er_write( E_EQ0067_grSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
			db_key( gr->gr_id );
		    }
		}
		break;
	case 396:
#line 1811 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_nlsym = (SYM *)0;	/* Initialize state vars */
		    gr->gr_nlid = (char *)0;
		    gr->gr_nltype = T_NONE; 
		}
		break;
	case 397:
#line 1817 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    SYM		*tmpsym;
		    char	*tmpid;
		    i4		tmptype;
		    /*
		    ** Assign indicator info to correct gr_state location
		    */
		    tmpsym = gr->gr_nlsym;
		    tmpid = gr->gr_nlid;
		    tmptype = gr->gr_nltype;
		    gr->gr_nlsym = gr->gr_sym;
		    gr->gr_nlid = gr->gr_id;
		    gr->gr_nltype = gr->gr_type;
		    gr->gr_sym = tmpsym;
		    gr->gr_id = tmpid;
		    gr->gr_type = tmptype;
		    if (gr->gr_nltype != T_UNDEF)	/* Don't cascade errs */
		    {
			if (gr->gr_nltype != T_INT
					    || sym_g_dsize(gr->gr_nlsym) != 2)
			    er_write( E_EQ0073_grINDVAR, EQ_ERROR, 1,
								gr->gr_nlid );
		    }
		}
		break;
	case 398:
#line 1844 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    /* 
		    ** Save away variable information 
		    */
		    gr->gr_nlsym = gr->gr_sym;
		    gr->gr_nlid = gr->gr_id;
		    gr->gr_nltype = gr->gr_type;
		}
		break;
	case 399:
#line 1857 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 400:
#line 1862 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 401:
#line 1867 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 402:
#line 1872 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 403:
#line 1877 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 404:
#line 1881 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 405:
#line 1885 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 406:
#line 1890 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 407:
#line 1895 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_op( ERx("!=") );
		}
		break;
	case 409:
#line 1907 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_flag & GR_REPLCSR)
			ecs_chkupd( NULL, NULL, NULL );		/* check csr */
		}
		break;
	case 414:
#line 1918 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 415:
#line 1923 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_flag & GR_REPLCSR)
			ecs_chkupd( NULL, yypvt[-0].s, gr->gr_sym );
		}
		break;
	case 421:
#line 1963 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_flag &= ~GR_CPYHNDDEF;
		}
		break;
	case 423:
#line 1969 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_sym)
		    {
			if (gr->gr_type == T_CHAR)
			    db_var( DB_ID, gr->gr_sym, gr->gr_id, (SYM *)0,
				    (char *)0, (char *)0 );
			else
			{
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0062_grNAME, EQ_ERROR, 1,
								    gr->gr_id );
			    db_key( gr->gr_id );
			}
		    }
		    else	/* No variable - just a name */
		    {
			if (   gr->gr_flag & GR_COPY_PROG
			    && STbcompare(gr->gr_id, 0, ERx("copyhandler"), 0,
				TRUE) == 0)
			    gr->gr_flag |= GR_CPYHNDDEF;
			db_key( gr->gr_id );
		    }
		}
		break;
	case 424:
#line 1994 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		    if (gr->gr_flag & GR_CPYHNDDEF)	/* Turned off above */
		    {
			eqtl_init(IISETSQL);
			eqtl_add(yypvt[-0].s, (PTR)0, (PTR)0, (PTR)0, T_INT,
				ERx("copyhandler"));
			gr->gr_flag |= GR_SEENCPYHND;
		    }
		}
		break;
	case 427:
#line 2007 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_sconst( yypvt[-0].s );
		}
		break;
	case 429:
#line 2013 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_sym)
		    {
			db_var( DB_STRING, gr->gr_sym, gr->gr_id, (SYM *)0,
				(char *)0, (char *)0 );
			if (gr->gr_type != T_INT && gr->gr_type != T_CHAR &&
						    gr->gr_type != T_UNDEF )
			    er_write( E_EQ0060_grINTSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
		    }
		    else
		    {
			db_key( gr->gr_id );
			if (gr->gr_flag & GR_CPYHNDDEF)
			{
			    eqtl_init(IISETSQL);
			    eqtl_add(gr->gr_id, (PTR)0, (PTR)0, (PTR)0, T_NONE,
					ERx("copyhandler"));
			    gr->gr_flag |= GR_SEENCPYHND;
			}
		    }
		}
		break;
	case 430:
#line 2041 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 434:
#line 2053 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 435:
#line 2058 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 436:
#line 2063 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 437:
#line 2068 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_op(ERx(", "));	/* Need a space for run-time */
		}
		break;
	case 438:
#line 2073 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 439:
#line 2078 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 440:
#line 2089 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_op( ERx("=") );
		}
		break;
	case 441:
#line 2093 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_op( ERx("=") );
		}
		break;
	case 442:
#line 2105 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_op( ERx("=") );
		    er_write( E_EQ0071_grASSGNOP, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 443:
#line 2110 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_op( ERx("=") );
		}
		break;
	case 445:
#line 2124 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_op( ERx("=") );
		    er_write( E_EQ0071_grASSGNOP, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 448:
#line 2144 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 449:
#line 2149 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 450:
#line 2154 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 451:
#line 2159 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 452:
#line 2164 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 453:
#line 2169 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 454:
#line 2174 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 455:
#line 2179 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 456:
#line 2184 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 458:
#line 2190 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 459:
#line 2195 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 460:
#line 2200 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 464:
#line 2211 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (  (STbcompare(yypvt[-0].s, 0, ERx("asc"), 0, TRUE) != 0)
			&&(STbcompare(yypvt[-0].s, 0, ERx("desc"), 0, TRUE) != 0)
		       )
		    {
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					ERx("REGISTER INDEX"), ERx("ASC") );
			db_key( ERx("asc") );
		    }
		    else
		    {
			db_key( yypvt[-0].s );
		    }
		}
		break;
	case 470:
#line 2239 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 471:
#line 2246 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 472:
#line 2253 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_hexconst(yypvt[-0].s);
		}
		break;
	case 475:
#line 2267 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_sym)
		    {
			if (gr->gr_type == T_CHAR)
			    db_var( DB_ID, gr->gr_sym, gr->gr_id, (SYM *)0,
				    (char *)0, (char *)0 );
			else
			{
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0062_grNAME, EQ_ERROR, 1,
								    gr->gr_id );
			    db_key( gr->gr_id );
			}
		    }
		    else	/* No variable - just a name */
		    {
			/* If tuple sort - send same name as used in tlist */
			if (gr->gr_flag & GR_TUPSORT)
			    db_attname( gr->gr_id, 0 );
			else
			    db_key( gr->gr_id );
		    }
		    yyval.s = gr->gr_id;		/* for cursor rules */
		}
		break;
	case 478:
#line 2302 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 480:
#line 2311 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_sym && gr->gr_type == T_INT)
			db_var( DB_STRING, gr->gr_sym, gr->gr_id, (SYM *)0,
				(char *)0, (char *)0 );
		    else
		    {
			if (gr->gr_type != T_UNDEF)
			    er_write( E_EQ0059_grINT, EQ_ERROR, 1, gr->gr_id );
			db_key( gr->gr_id );
		    }
		}
		break;
	case 483:
#line 2333 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_sconst( yypvt[-0].s );
		}
		break;
	case 485:
#line 2342 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 486:
#line 2346 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4		db_type;

		    if (gr->gr_sym)
		    {
			db_type = gr->gr_type == T_CHAR ? DB_ID : DB_STRING;
			db_var( db_type, gr->gr_sym, gr->gr_id, (SYM *)0,
				(char *)0, (char *)0 );
			if (gr->gr_type != T_INT && gr->gr_type != T_CHAR &&
			     gr->gr_type != T_UNDEF )
			    er_write( E_EQ0060_grINTSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
		    }
		    else
			db_key( gr->gr_id );
		}
		break;
	case 487:
#line 2367 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_sym && gr->gr_type == T_CHAR)
			db_var( DB_ID, gr->gr_sym, gr->gr_id, (SYM *)0,
				(char *)0, (char *)0 );
		    else
		    {
			if (gr->gr_type != T_UNDEF)
			    er_write( E_EQ0064_grQRSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
			db_key( gr->gr_id );
		    }
		}
		break;
	case 488:
#line 2395 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_int_add( IIutEXE );
		    arg_str_add( ARG_CHAR, (char *)0 );
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_call( IIUTSYS );
		}
		break;
	case 489:
#line 2402 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    arg_int_add( IIutPROG );
		}
		break;
	case 490:
#line 2408 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_call( IIUTSYS );
		}
		break;
	case 492:
#line 2415 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 494:
#line 2422 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_int_add( IIutARG );
		}
		break;
	case 497:
#line 2430 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( IIUTSYS );
		}
		break;
	case 499:
#line 2436 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	default:
		    goto label_500;
	}

label_500:
	switch (yym) {
	case 500:
#line 2440 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 501:
#line 2445 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_int_add( IIutARG );
		}
		break;
	case 502:
#line 2460 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	ret_no;

		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    if ((ret_no = lbl_gen(LBLmRETLOOP, TRUE)) == LBL_NOLEV)
			er_write( E_EQ0058_grINLOOP, EQ_ERROR, 2, yypvt[-0].s,
							    ERx("RETRIEVE") );
		    else
		    {
			gen_call( IIBREAK );
			gen_goto( G_IF, L_RETEND, 
				  lbl_value(LBLmRETLOOP, ret_no) );
		    }
		}
		break;
	case 503:
#line 2484 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	stop, stop_num, stop_mode;

		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    stop = lbl_gen(LBLmRETLOOP|LBLmFORMDATA|LBLmTBLOOP, TRUE);
		    if (stop != LBL_NOLEV)
		    {
			stop_mode = lbl_get_mode( stop );
			stop_num = lbl_value( stop_mode, stop );

			switch ( stop_mode )
			{
			  case LBLmRETLOOP:
			    gen_call( IIBREAK );
			    gen_goto( G_IF, L_RETEND, stop_num );
			    break;

			  case LBLmFORMDATA:
			    gen_goto( G_IF, L_FDEND, stop_num );
			    break;

			  case LBLmTBUNLD:
			  case LBLmTBDATA:
			    gen_goto( G_IF, L_TBEND, stop_num );
			    lbl_used( LBL_TBL );
			    break;
			}
		    }
		    else
			er_write( E_EQ0058_grINLOOP, EQ_ERROR, 2, yypvt[-0].s,
						    ERx("RETRIEVE/DISPLAY") );
		}
		break;
	case 504:
#line 2525 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    gen_call( IIEXIT );
		}
		break;
	case 505:
#line 2538 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( QAPRINTF );
		}
		break;
	case 506:
#line 2543 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		}
		break;
	case 510:
#line 2553 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_sym)
			arg_var_add( gr->gr_sym, gr->gr_id );
		    else
			arg_str_add( ARG_CHAR, gr->gr_id );
		}
		break;
	case 511:
#line 2560 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_str_add( ARG_INT, yypvt[-0].s );
		}
		break;
	case 512:
#line 2564 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_str_add( ARG_FLOAT, yypvt[-0].s );
		}
		break;
	case 513:
#line 2568 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_str_add( ARG_PACK, yypvt[-0].s );
		}
		break;
	case 514:
#line 2572 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 515:
#line 2584 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( IIINGOPEN );
		}
		break;
	case 516:
#line 2589 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    arg_int_add( eq->eq_lang );
		}
		break;
	case 518:
#line 2604 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    gr->gr_func = IIEQINQ;
		}
		break;
	case 520:
#line 2619 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    gr->gr_func = IIEQSET;
		}
		break;
	case 521:
#line 2636 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_send();			/* flush query to cursor */
		    ecs_query( NULL, ECS_STOP_QUERY );	/* stop saving query */
		    if (gr->gr_flag & GR_REPEAT)
		    {
			rep_close( IICSQUERY, ecs_id_curcsr(), FALSE );
		      /* rep_close changed the output state. */
			ecs_query( NULL, ECS_END_QUERY ); 	/* end query */
		    }
		}
		break;
	case 522:
#line 2649 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    ecs_declcsr( yypvt[-0].s, gr->gr_sym );   /* sets current cursor */
		    ecs_query( NULL, ECS_START_QUERY );	/* start saving query */
		}
		break;
	case 523:
#line 2656 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_flag |= GR_CURSOR|GR_DECLCSR;
		    gr_mechanism( GR_EQSTMT, GR_sCURSOR, yypvt[-0].s );
		    /* $$.s = $1.s; */
		}
		break;
	case 524:
#line 2665 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (yypvt[-2].i)
		    {
			/*
			** If the where clause is in a variable, then the
			** cursor is updatable.  Additionally, if there wasn't
			** an update clause, then pretend we saw
			** "for update of (string_var)" since one may have been
			** tacked onto the end of the where-variable.
			*/
			ecs_csrset( NULL, ECS_UPDATE|ECS_WHERE );
			switch (yypvt[-0].i)
			{
			  case 1:	/* Update clause, but no update list */
			    break;
			  case 0:	/* No update clause */
			    ecs_csrset( NULL, ECS_UPDVAR );
			    /* FALL THROUGH! */
			  case 2:	/* Update clause with update list */
			    ecs_fixupd( NULL );
			    break;
			}
		    } else if (yypvt[-0].i == 2)
			ecs_fixupd( NULL );
		}
		break;
	case 525:
#line 2693 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (yypvt[-0].i)
			ecs_csrset( NULL, ECS_SORTED );
		}
		break;
	case 526:
#line 2700 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 527:
#line 2704 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		    ecs_csrset( NULL, ECS_UNIQUE );
		}
		break;
	case 529:
#line 2713 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    ecs_csrset( NULL, ECS_REPEAT );
		    gr->gr_flag |= GR_REPEAT;
		    rep_begin( ERx("repeat"), ecs_id_curcsr() );
		}
		break;
	case 530:
#line 2728 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 0;
		}
		break;
	case 531:
#line 2732 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    ecs_csrset( NULL, yypvt[-1].i );
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 532:
#line 2740 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 533:
#line 2747 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 534:
#line 2758 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (STbcompare(yypvt[-0].s, 0, ERx("update"), 0, TRUE) != 0)
		    {
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					ERx("DECLARE CURSOR"), ERx("UPDATE") );
			db_key( ERx("update") );
		    } else
			db_key( yypvt[-0].s );
		    yyval.i = ECS_UPDATE;
		}
		break;
	case 535:
#line 2769 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (STbcompare(yypvt[-1].s, 0, ERx("direct"), 0, TRUE) == 0)
		    {
			yyval.i = ECS_UPDATE|ECS_DIRECTU;
			db_key( yypvt[-1].s );
		    } else if (!STbcompare(yypvt[-1].s, 0, ERx("deferred"), 0, TRUE))
		    {
			yyval.i = ECS_UPDATE|ECS_DEFERRU;
			db_key( yypvt[-1].s );
		    } else
		    {
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
				    ERx("DECLARE CURSOR"), ERx("DEFERRED") );
			yyval.i = ECS_UPDATE|ECS_DEFERRU;
			db_key( ERx("deferred") );
		    }
		    if (STbcompare(yypvt[-0].s, 0, ERx("update"), 0, TRUE))
		    {
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
				    ERx("DECLARE CURSOR"), ERx("UPDATE") );
			db_key( ERx("update") );
		    } else
			db_key( yypvt[-0].s );
		}
		break;
	case 536:
#line 2798 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 1;
		}
		break;
	case 537:
#line 2802 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 2;
		}
		break;
	case 542:
#line 2818 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4		stat;		/* status of this column */
		    i4		has_var;	/* csr had a var in tgt list? */

		    /*
		    ** Variables in the update list are always allowed;
		    ** they always match any name in the target list.
		    */
		    if (gr->gr_sym)
		    {
		      /* Note that the cursor has a var in the update list. */
			ecs_csrset( NULL, ECS_UPDVAR );
		    } else	/* Column is just a plain name. */
		    {
			has_var =
			   ecs_colupd( NULL, ERx(""),
					    ECS_CHK|ECS_ISWILD) & ECS_EXISTS;
			stat = ecs_colupd( NULL, yypvt[-0].s, ECS_CHK );

		      /* Column not mentioned in target list? */
			if (!(stat & ECS_EXISTS))
			{
#if 0
			  /* No vars in target list means an error. */
			    if (!has_var)
				er_write( E_EQ0406_csUPDXIST, EQ_ERROR, 2,
							yypvt[-0].s, ecs_namecsr() );
#endif
			  /* Add it in any case. */
			    _VOID_ ecs_colupd( NULL, yypvt[-0].s, ECS_ADD|ECS_UPD );
			}
#if 0
			else if (stat & ECS_EXPR)
			    er_write( E_EQ0403_csUPDEXPR, EQ_ERROR, 2, yypvt[-0].s,
							ecs_namecsr() );
			else if (stat & ECS_MANY)
			    er_write( E_EQ0404_csUPDMANY, EQ_ERROR, 2, yypvt[-0].s,
							ecs_namecsr() );
			else if (stat & ECS_CONSTANT)
			    er_write( E_EQ0405_csUPDCONST, EQ_ERROR, 2, yypvt[-0].s,
							ecs_namecsr() );
#endif
			else
			    _VOID_ ecs_colupd( NULL, yypvt[-0].s, ECS_CHK|ECS_UPD );
		    }
		}
		break;
	case 543:
#line 2865 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    er_write( E_EQ0411_csUPDALL, EQ_ERROR, 1, ecs_namecsr() );
		}
		break;
	case 544:
#line 2872 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_xsym = (SYM *)0;
		}
		break;
	case 545:
#line 2877 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_sym)
		    {
			if (gr->gr_type != T_CHAR) 
			{
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0067_grSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
			    gr->gr_sym = (SYM *)0;
			}
		    }
		    /* xsym used by csr_open rule */
		    gr->gr_xsym = gr->gr_sym;
		    yyval.s = gr->gr_id;
		}
		break;
	case 549:
#line 2917 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    switch (yypvt[-0].i)
		    {
		      case 0:
			_VOID_ ecs_colupd( NULL, yypvt[-2].s, ECS_ADD|ECS_CONSTANT );
			break;
		      case 1:
			_VOID_ ecs_colupd( NULL, yypvt[-2].s, ECS_ADD );
			break;
		      case 2:
			_VOID_ ecs_colupd( NULL, ERx(""), ECS_ADD|ECS_ISWILD );
			break;
		      default:
			_VOID_ ecs_colupd( NULL, yypvt[-2].s, ECS_ADD|ECS_EXPR );
			break;
		    }
		}
		break;
	case 550:
#line 2935 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    /*
		    ** "id.all" is allowed, but is like a variable replacing
		    ** a portion of the target list.
		    */
		    if (yypvt[-0].s)
			_VOID_ ecs_colupd( NULL, yypvt[-0].s, ECS_ADD );
		    else
			_VOID_ ecs_colupd( NULL, ERx(""), ECS_ADD|ECS_ISWILD );
		}
		break;
	case 551:
#line 2946 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    _VOID_ ecs_colupd( NULL, ERx(""), ECS_ADD|ECS_ISWILD );
		}
		break;
	case 552:
#line 2950 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key( yypvt[-0].s );
		    _VOID_ ecs_colupd( NULL, yypvt[-2].s, ECS_ADD|ECS_CONSTANT );
		}
		break;
	case 553:
#line 2968 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    ecs_open( yypvt[-1].s, gr->gr_id, gr->gr_sym, gr->gr_xsym?TRUE:FALSE );
		    gr->gr_xsym = (SYM *)0;	/* Borrowed from gr_nlsym */
		}
		break;
	case 554:
#line 2975 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_flag |= GR_CURSOR;
		    gr_mechanism( GR_EQSTMT, GR_sCURSOR, yypvt[-0].s );
		}
		break;
	case 555:
#line 2982 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_id = (char *)0;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 556:
#line 2987 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (STbcompare(yypvt[-0].s, 0, ERx("readonly"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					ERx("OPEN CURSOR"), ERx("READONLY") );
		    gr->gr_id = ERx(" for readonly ");
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 557:
#line 2995 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (!gr->gr_sym || gr->gr_type != T_CHAR)
		    {
			if (gr->gr_type != T_UNDEF)
			    er_write( E_EQ0064_grQRSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
		    }
		}
		break;
	case 558:
#line 3015 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    ecs_eretrieve( NULL, 0 );	/* Didn't use an SQLDA! */
		}
		break;
	case 559:
#line 3021 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    ecs_retrieve( yypvt[-0].s, gr->gr_sym != (SYM *)0, 0, 0,
					(SYM *) NULL, (char *) NULL );
		    gr->gr_func = IICSRETRIEVE;
		}
		break;
	case 560:
#line 3029 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_flag |= GR_CURSOR;
		    gr_mechanism( GR_EQSTMT, GR_sCURSOR, yypvt[-0].s );
		}
		break;
	case 565:
#line 3051 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_sym)
		    {
			ecs_get( gr->gr_id, gr->gr_sym, gr->gr_type,
				 gr->gr_nlid, gr->gr_nlsym, (char *)0 );
		    }
		}
		break;
	case 566:
#line 3068 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    ecs_replace( FALSE, NULL, FALSE );
		}
		break;
	case 567:
#line 3074 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    ecs_replace( TRUE, yypvt[-0].s, gr->gr_sym != (SYM *)0 );
		    gr->gr_flag |= GR_REPLCSR;
		}
		break;
	case 568:
#line 3081 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_flag |= GR_CURSOR;
		    gr_mechanism( GR_EQSTMT, GR_sCURSOR, yypvt[-0].s );
		}
		break;
	case 569:
#line 3094 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    ecs_delete( yypvt[-0].s, (char *)0, gr->gr_sym != (SYM *)0 );
		}
		break;
	case 570:
#line 3100 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_flag |= GR_CURSOR;
		    gr_mechanism( GR_EQSTMT, GR_sCURSOR, yypvt[-0].s );
		}
		break;
	case 571:
#line 3113 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    ecs_close( yypvt[-0].s, gr->gr_sym != (SYM *)0 );
		}
		break;
	case 572:
#line 3119 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_flag |= GR_CURSOR;
		    gr_mechanism( GR_EQSTMT, GR_sCURSOR, yypvt[-0].s );
		}
		break;
	case 573:
#line 3139 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( IIADDFORM );
		    gr->gr_flag &= ~GR_NOINDIR;
		}
		break;
	case 574:
#line 3145 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    gr->gr_flag |= GR_NOINDIR;
		}
		break;
	case 575:
#line 3151 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_sym)
		    {
			arg_var_add( gr->gr_sym, gr->gr_id );
			if (gr->gr_type == T_CHAR)
			{
# ifndef EQ_ADD_FNAME
			    er_write( E_EQ0061_grINTVAR, EQ_ERROR, 1,
								    gr->gr_id );
# endif /* EQ_ADD_FNAME */
			}
			else if (gr->gr_type != T_INT)
			    er_write( E_EQ0061_grINTVAR, EQ_ERROR, 1,
								    gr->gr_id );
		    }	
		    else 		/* Error already sent if not variable */
			arg_str_add( ARG_CHAR, gr->gr_id );
		}
		break;
	case 576:
#line 3170 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
# ifndef EQ_ADD_FNAME
		    er_write( E_EQ0061_grINTVAR, EQ_ERROR, 1, yypvt[-0].s );
# endif /* EQ_ADD_FNAME */
		}
		break;
	case 577:
#line 3186 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	form_no;

		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    if ((form_no = lbl_gen(LBLmDISPLAY, TRUE)) != LBL_NOLEV)
			gen_goto( G_IF, L_FDEND, 
				  lbl_value(LBLmDISPLAY, form_no) );
		    else
			er_write( E_EQ0058_grINLOOP, EQ_ERROR, 2, yypvt[-0].s,
							    ERx("DISPLAY") );
		}
		break;
	case 578:
#line 3207 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-1].s );
		    gen_call( IICLRSCR );
		}
		break;
	case 579:
#line 3212 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( IICLRFLDS );
		}
		break;
	case 581:
#line 3218 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-1].s );
		    gr->gr_func = IIFLDCLEAR;
		}
		break;
	case 582:
#line 3286 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	cur_form = lbl_current( LBL_FORM );

		    gen_label( G_NOLOOP, L_MUINIT, cur_form );
		    act_close( cur_form );
		    gen_goto( G_TERM, L_FDINIT, cur_form );
		    gen_label( G_NOLOOP, L_FDEND, cur_form );
		    if (yypvt[-3].i)				/* display submenu */
			gen_call( IIENDNEST );
		    else				/* normal display */
			gen_call( IIENDFRM );
		    lbl_exit_block();
		}
		break;
	case 583:
#line 3319 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	cur_form;

		    cur_form = lbl_next( LBL_FORM );
		    lbl_enter_block();
		    gr->gr_mode = LBLmDISPLAY;
		    lbl_set_mode( gr->gr_mode );
		  /* mode is DISPLAY, and adjective is DISPLAY */
		    lbl_adset( LBL_NOLEV, LBLaDISPLAY );
		    lbl_a_reset();

		    act_open();
		    gen_if_goto( IIDISPFRM, C_EQ, 0, L_FDEND, cur_form );
		    gen_goto( G_TERM, L_MUINIT, cur_form );
		    gen_label( G_NOLOOP, L_FDINIT, cur_form );
		    yyval.i = 0;
		}
		break;
	case 584:
#line 3338 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	cur_form;

		    cur_form = lbl_next( LBL_FORM );
		  /* mode is DISPLAY, but adjective is NESTMU */
		    lbl_enter_block();
		    gr->gr_mode = LBLmDISPLAY;
		    lbl_set_mode( gr->gr_mode );
		    lbl_adset( LBL_NOLEV, LBLaNESTMU );
		    lbl_a_reset();

		    act_open();
		    gen_if_goto( IINESTMU, C_EQ, 0, L_FDEND, cur_form );
		    gen_goto( G_TERM, L_MUINIT, cur_form );
		    gen_label( G_NOLOOP, L_FDINIT, cur_form );
		    yyval.i = 1;
		}
		break;
	case 585:
#line 3357 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    gr->gr_func = IIDISPFRM;
		}
		break;
	case 586:
#line 3363 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    frs_mode( ERx("DISPLAY/INITTABLE"), yypvt[-0].s );
		}
		break;
	case 587:
#line 3367 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_sym)
		    {
			if (gr->gr_type == T_CHAR)
			    arg_var_add( gr->gr_sym, gr->gr_id );
			else
			{
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0062_grNAME, EQ_ERROR, 1,
								    gr->gr_id );
			    arg_str_add( ARG_CHAR, gr->gr_id );
			}
		    }
		    else
			frs_mode( ERx("DISPLAY/INITTABLE"), gr->gr_id );
		}
		break;
	case 588:
#line 3384 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_str_add( ARG_CHAR, ERx("f") );
		}
		break;
	case 589:
#line 3389 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    sc_eat( NULL, SC_NEST|SC_SEEN, ERx(")"), '(', ')' );
		    er_write( E_EQ0051_grDISPTL, EQ_ERROR, 0 );
		}
		break;
	case 591:
#line 3417 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	func = IIRUNFRM;
		    i4	cur_form = lbl_current( LBL_FORM );

		    gen_label( G_LOOP, L_FDBEG, cur_form );
		    if (yypvt[-1].i)	/* disp_head was DISPLAY SUBMENU */
			func = IIRUNNEST;
		    gen_loop( G_OPEN, L_FDBEG, L_FDFINAL, cur_form, func, 
			      C_NOTEQ, 0 );
		}
		break;
	case 594:
#line 3432 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_if_goto( IIFSETIO, C_EQ, 0, L_FDEND,
			lbl_current(LBL_FORM) );
		    gr->gr_func = IISETFIELD;	/* In case of I/O */
		    gr->gr_mode = LBLmDISPLAY;
		}
		break;
	case 597:
#line 3476 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	cur_form = lbl_current( LBL_FORM );

		    gen_else( G_OPEN, II0, C_0, 0, L_0, 0, L_0, 0 ); 
		    gen_goto( G_IF, L_FDEND, cur_form );
		    gen_else( G_CLOSE, IIRETVAL, C_0, 0, L_0, 0, 
			      L_ACTEND, cur_form );
		    gr->gr_flag |= GR_ACTIVATE;
		}
		break;
	case 598:
#line 3486 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_flag &= ~GR_ACTIVATE;
		}
		break;
	case 601:
#line 3494 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_else( G_ELSE, IIRETVAL, C_0, 0, L_ACTELSE, 
			lbl_current(LBL_INTR), L_ACTEND, lbl_current(LBL_FORM));
		    gr->gr_mode = LBLmDISPLAY;
		}
		break;
	case 602:
#line 3501 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	cur_intr = lbl_next( LBL_INTR );

		    gen_else( G_OPEN, IIRETVAL, C_EQ, cur_intr, L_ACTELSE,
			cur_intr, L_ACTEND, lbl_current(LBL_FORM) );
		    gr->gr_mode = LBLmDISPLAY;
		    /* Reset activate argument list */
		    act_args( FALSE, (char *)0, (char *)0, 0, (SYM *)0 );
		}
		break;
	case 603:
#line 3512 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    /* Reset activate argument list */
		    act_args( FALSE, (char *)0, (char *)0, 0, (SYM *)0 );
		}
		break;
	case 604:
#line 3517 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    act_args( FALSE, (char *)0, (char *)0, 0, (SYM *)0 );
		}
		break;
	case 607:
#line 3530 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    act_add( ACT_FIELD, lbl_current(LBL_INTR), yypvt[-2].i, yypvt[-0].ip );
		    gr->gr_adjective = LBLaFIELD;
		}
		break;
	case 608:
#line 3535 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    act_add( ACT_MENU, lbl_current(LBL_INTR), 0, yypvt[-1].ip );
		    gr->gr_adjective = LBLaMITEM;
		}
		break;
	case 609:
#line 3540 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    act_add( ACT_SCROLL, lbl_current(LBL_INTR), sclUP, yypvt[-0].ip );
		    gr->gr_adjective = LBLaSCROLL;
		}
		break;
	case 610:
#line 3545 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    act_add( ACT_SCROLL,lbl_current(LBL_INTR), sclDOWN, yypvt[-0].ip );
		    gr->gr_adjective = LBLaSCROLL;
		}
		break;
	case 611:
#line 3550 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	cur_intr = lbl_current( LBL_INTR );

		    act_add( ACT_COLUMN, cur_intr, yypvt[-3].i, yypvt[-1].ip );
		    act_add( ACT_COLUMN, cur_intr, yypvt[-3].i, yypvt[-0].ip );
		    gr->gr_adjective = LBLaCOLUMN;
		}
		break;
	case 612:
#line 3558 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    /* Old-style syntax - no longer supported */
                    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1,
                                            ERx("ACTIVATE COMMAND") );
		}
		break;
	case 614:
#line 3566 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 0;
		}
		break;
	case 615:
#line 3570 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (STbcompare(yypvt[-0].s, 0, ERx("after"), 0, TRUE) == 0)
		    {
			yyval.i = 0;
		    }	
		    else 
		    {
			if (STbcompare(yypvt[-0].s, 0, ERx("before"), 0, TRUE) != 0)
		            er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
				    ERx("ACTIVATE"), ERx("BEFORE") );
			yyval.i = 1;
		    }
		}
		break;
	case 616:
#line 3585 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.ip = act_string( yypvt[-0].s );
		}
		break;
	case 617:
#line 3589 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.ip = act_string( yypvt[-0].s );
		}
		break;
	case 618:
#line 3593 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_sym)
		    {
			yyval.ip = act_var( gr->gr_sym, gr->gr_id );
			if (gr->gr_type != T_CHAR && gr->gr_type != T_UNDEF)
			    er_write( E_EQ0067_grSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
		    }
		    else
			yyval.ip = act_string( gr->gr_id );
		}
		break;
	case 619:
#line 3606 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 620:
#line 3610 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 621:
#line 3621 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    char	*s = yypvt[-1].s;
		    i4		keynum = 1;

		    if (STbcompare(s, 7, ERx("timeout"), 7, TRUE) == 0)
		    {
			act_add(ACT_TIMEOUT, lbl_current(LBL_INTR),0,(i4 *)0);
			/* Is this the only act_elm in this ACTIVATE? */
			if (gr->gr_adjective == LBLaNONE)
			    gr->gr_adjective = LBLaTIME;
			if (yypvt[-0].i == 1)
			{
			    er_write( E_EQ0309_actTARGET, EQ_ERROR, 1, 
				    ERx("TIMEOUT") );
			}
		    }
		    else if (STbcompare(s, 6, ERx("frskey"), 6, TRUE) == 0)
		    {
			if ((CVan(s+6, &keynum) == OK) && (keynum >= 0))
		        {
			    act_add( ACT_FRSKEY, lbl_current(LBL_INTR), 
					keynum, (i4 *)0);
			    gr->gr_adjective = LBLaFRSKEY;
			}
		        else
			{
			    er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, s,
				    ERx("ACTIVATE"), ERx("FRSKEY1") );
			}
		    }
		    else if (STbcompare(s, 8, ERx("control_"), 0, TRUE) == 0)
		    {
			er_write( E_EQ0070_grMISSWORD, EQ_ERROR, 2,
				  ERx("COMMAND"), ERx("ACTIVATE") );
		    }
		    else
		    {
			er_write( E_EQ0301_actBAD, EQ_ERROR, 1, s );
		    }
		}
		break;
	case 622:
#line 3662 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    char	*s = yypvt[-2].s;

		    /*
		    ** ACTIVATE FRSKEY number
		    */
		    if (STbcompare(s, 0, ERx("frskey"), 0, TRUE) == 0)
		    {
			act_add( ACT_FRSKEY, lbl_current(LBL_INTR),
					yypvt[-1].i, (i4 *)0);
			gr->gr_adjective = LBLaFRSKEY;
		    }
		    else
		    {
			er_write( E_EQ0301_actBAD, EQ_ERROR, 1, s );
		    }
		}
		break;
	case 623:
#line 3681 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4		keynum;

		    if ((CVan(yypvt[-0].s, &keynum) == OK) && (keynum >= 0))
		    {
		        yyval.i = keynum;	
		    }
		    else
		    {
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
			    ERx("ACTIVATE"), ERx("FRSKEY1") );
		    }
		}
		break;
	case 624:
#line 3695 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_sym)
		    {
			if (gr->gr_type != T_INT && gr->gr_type != T_UNDEF)
			    er_write( E_EQ0061_grINTVAR, EQ_ERROR, 1,
								gr->gr_id );
		    }
		    else
		    {
			gr->gr_type = T_INT;
		    }
		    act_num(ERx("frskey"), gr->gr_id, gr->gr_type, gr->gr_sym);
		    yyval.i = 1;
		}
		break;
	case 625:
#line 3711 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 0;
		}
		break;
	case 626:
#line 3715 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 1;
		}
		break;
	case 629:
#line 3723 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    /* It will check for errors */
		    act_args( TRUE, yypvt[-2].s, yypvt[-0].s, gr->gr_type, gr->gr_sym );
		}
		break;
	case 630:
#line 3729 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 631:
#line 3733 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
	    	}
		break;
	case 632:
#line 3737 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
	    	}
		break;
	case 633:
#line 3741 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
	    	}
		break;
	case 634:
#line 3746 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = NULL;
	    	}
		break;
	case 635:
#line 3752 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_INT;
		    gr->gr_sym = NULL;
	    	}
		break;
	case 636:
#line 3758 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = gr->gr_id;
		    if (gr->gr_sym)
		    {
			if (gr->gr_type != T_CHAR && gr->gr_type != T_INT &&
				gr->gr_type != T_UNDEF)
			    er_write( E_EQ0060_grINTSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
		    }
		}
		break;
	case 637:
#line 3783 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_goto( G_TERM, L_FDEND, lbl_current(LBL_FORM) );
		}
		break;
	case 638:
#line 3788 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	func = IIRUNFRM;
		    i4	cur_form = lbl_current( LBL_FORM );

		    if (lbl_adget(LBL_NOLEV) == LBLaNESTMU)
			func = IIRUNNEST;
		    gen_loop( G_CLOSE, L_FDBEG, L_FDFINAL, cur_form, 
			      func, C_0, 0 );
		    if ((gr->gr_flag & GR_ACTIVATE) == 0)
			gen_if_goto( IIFRISTIME,C_NOTEQ, 0, L_FDEND, cur_form );
		    gen_label( G_LOOP, L_FDFINAL, cur_form );
		    gen_if_goto( IICHKFRM, C_EQ, 0, L_FDBEG, cur_form );
		}
		break;
	case 641:
#line 3806 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_if_goto( IIFSETIO, C_EQ, 0, L_FDEND,
			lbl_current(LBL_FORM) );
		    gr->gr_func = IIRETFIELD;
		}
		break;
	case 642:
#line 3822 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	form_no;

		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    if ((form_no = lbl_gen(LBLmFORMDATA, TRUE)) != LBL_NOLEV)
			gen_goto( G_IF, L_FDEND, 
				  lbl_value(LBLmFORMDATA, form_no) );
		    else
			er_write( E_EQ0058_grINLOOP, EQ_ERROR, 2, yypvt[-0].s,
							    ERx("FORMDATA") );
		}
		break;
	case 643:
#line 3843 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	form_no;

		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    if ((form_no = lbl_gen(LBLmDISPLAY, TRUE)) != LBL_NOLEV)
			gen_goto( G_IF, L_FDFINAL, 
				  lbl_value(LBLmDISPLAY, form_no) );
		    else
			er_write( E_EQ0058_grINLOOP, EQ_ERROR, 2, yypvt[-0].s,
							    ERx("DISPLAY") );
		}
		break;
	case 644:
#line 3863 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    gen_call( IIENDFORMS );
		}
		break;
	case 645:
#line 3893 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	cur_form = lbl_current( LBL_FORM );

		    gen_loop( G_CLOSE, L_FDBEG, L_FDEND, cur_form, IIFNAMES,
			C_0, 0 );
		    gen_label( G_LOOP, L_FDEND, cur_form );
		    gen_call( IIENDFRM );
		}
		break;
	case 646:
#line 3903 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	cur_form;

		    cur_form = lbl_next( LBL_FORM );
		    arg_str_add( ARG_CHAR, ERx("Names") );
		    gen_if_goto( IIDISPFRM, C_EQ, 0, L_FDEND, cur_form);
		    gen_loop( G_OPEN, L_FDBEG, L_FDEND, cur_form, IIFNAMES, 
			      C_NOTEQ, 0 );
		    gr->gr_func = IIFDATA;		/* In case of I/O */
		    gr->gr_mode = LBLmFORMDATA;		/* In case of block */
		}
		break;
	case 647:
#line 3916 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 648:
#line 3921 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (yypvt[-0].i)
			er_write( E_EQ0077_grISUNSUPP, EQ_ERROR, 1,
							    ERx("FORMDATA") );
		}
		break;
	case 650:
#line 3937 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    gr->gr_func = IIFORMINIT;
		}
		break;
	case 651:
#line 3950 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( eq->eq_lang );
		    gen_call( IIFORMS );
		}
		break;
	case 652:
#line 3966 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		}
		break;
	case 653:
#line 3971 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_if( G_OPEN, IIFSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IIRETFIELD;
		}
		break;
	case 654:
#line 3977 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 655:
#line 3997 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1,
						    ERx("GETOPER") );
		}
		break;
	case 656:
#line 4004 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_if( G_OPEN, IIFSETIO, C_NOTEQ, 0 );
		}
		break;
	case 657:
#line 4009 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 661:
#line 4019 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
			gen_call( IIGETQRY );
		}
		break;
	case 662:
#line 4024 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_sym)
		    {
			if (gr->gr_nlsym != (SYM *)0)
			    arg_var_add( gr->gr_nlsym, gr->gr_nlid );
			else
			    arg_int_add( 0 );
			arg_var_add( gr->gr_sym, gr->gr_id );
			if (gr->gr_type != T_INT && gr->gr_type != T_UNDEF)
			    er_write( E_EQ0061_grINTVAR, EQ_ERROR, 1,
								    gr->gr_id );
		    }
		    /* else error already sent */
		}
		break;
	case 664:
#line 4051 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( 0 );		/* Dummy type right now */
		}
		break;
	case 665:
#line 4057 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (STbcompare(yypvt[-3].s, 0, ERx("subject"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-3].s,
					ERx("HELP_FRS"), ERx("SUBJECT") );
		    else if (STbcompare(yypvt[-1].s, 0, ERx("file"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
					ERx("HELP_FRS"), ERx("FILE") );
		    gen_call( IIFRSHELP );
		}
		break;
	case 666:
#line 4068 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 667:
#line 4082 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( IIHELPFILE );
		}
		break;
	case 668:
#line 4087 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 669:
#line 4099 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( QAMESSAGE );
		}
		break;
	case 670:
#line 4104 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 671:
#line 4116 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( QAPROMPT );
		}
		break;
	case 672:
#line 4121 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 673:
#line 4141 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    frs_close();
		}
		break;
	case 675:
#line 4148 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    frs_head();
		}
		break;
	case 676:
#line 4153 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    frs_inqset( FRSinq );
		}
		break;
	case 677:
#line 4159 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 678:
#line 4163 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 679:
#line 4167 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 680:
#line 4171 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 683:
#line 4180 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    frs_parentname( yypvt[-0].s, T_CHAR, (PTR)0 );
		}
		break;
	case 684:
#line 4184 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    frs_parentname( yypvt[-0].s, T_INT, (PTR)0 );
		}
		break;
	case 685:
#line 4188 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_sym)
		    {
			if (gr->gr_type != T_CHAR && gr->gr_type != T_INT)
			{
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0062_grNAME, EQ_ERROR, 1,
								    gr->gr_id );
			    frs_parentname( gr->gr_id, T_CHAR, (PTR)0 );
			}
			else
			    frs_parentname( gr->gr_id, gr->gr_type,(PTR)(gr->gr_sym) );
		    }
		    else
			frs_parentname( gr->gr_id, T_CHAR, (PTR)0 );
		}
		break;
	case 687:
#line 4214 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    er_write( E_EQ0077_grISUNSUPP, EQ_ERROR, 1,
						ERx("INQUIRE_FRS") );
		    YYERROR;
		}
		break;
	case 691:
#line 4226 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    frs_iqvar( gr->gr_id, gr->gr_type, gr->gr_sym,
				gr->gr_nlid, gr->gr_nlsym );
		}
		break;
	case 692:
#line 4232 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    frs_constant( yypvt[-0].s, (char *)0, T_NONE, (SYM *)0 );
		}
		break;
	case 693:
#line 4236 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    er_write( E_EQ0054_grNOPARENS, EQ_ERROR, 0 );
		    frs_constant( yypvt[-2].s, (char *)0, T_NONE, (SYM *)0 );
		}
		break;
	case 694:
#line 4241 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    /* Should be an error */
		    frs_constant( yypvt[-3].s, yypvt[-1].s, gr->gr_type, gr->gr_sym );
		}
		break;
	case 695:
#line 4248 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 696:
#line 4252 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 697:
#line 4256 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 698:
#line 4260 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 699:
#line 4264 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 700:
#line 4268 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 701:
#line 4272 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 702:
#line 4277 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 703:
#line 4283 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_INT;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 704:
#line 4289 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 705:
#line 4295 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 706:
#line 4301 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 707:
#line 4307 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 708:
#line 4313 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 709:
#line 4319 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 710:
#line 4325 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 711:
#line 4331 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = gr->gr_id;
		    if (gr->gr_sym)
		    {
			if (gr->gr_type != T_CHAR && gr->gr_type != T_INT)
			{
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0062_grNAME, EQ_ERROR, 1,
								    gr->gr_id );
			    gr->gr_type = T_CHAR;
			    gr->gr_sym = (SYM *)0;
			}
		    }
		    else
			gr->gr_type = T_CHAR;
		}
		break;
	case 712:
#line 4404 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( IIMESSAGE ); 
    		}
		break;
	case 713:
#line 4409 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
            	    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
    	    	    gr->gr_func = IIMESSAGE;
    		}
		break;
	case 716:
#line 4432 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    EQFW3glgen();	
		}
		break;
	case 717:
#line 4437 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    EQFWopen(gr->gr_func);
		}
		break;
	case 721:
#line 4452 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    EQFWwcoption(yypvt[-2].s, yypvt[-0].s);
		}
		break;
	case 722:
#line 4457 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 723:
#line 4461 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 724:
#line 4466 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 725:
#line 4470 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 730:
#line 4495 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		EQFWsuboption(yypvt[-2].s, gr->gr_type, gr->gr_id, (PTR)gr->gr_sym);
	}
		break;
	case 731:
#line 4500 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
	    	    yyval.s = yypvt[-0].s;
	        }
		break;
	case 732:
#line 4504 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 733:
#line 4510 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_INT;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 734:
#line 4516 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_CHAR;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 735:
#line 4522 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_FLOAT;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 736:
#line 4528 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_PACKASCHAR;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 737:
#line 4534 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (!gr->gr_sym)
		    {
			gr->gr_type = T_CHAR;
		    }	
		}
		break;
	case 738:
#line 4548 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( IIPRNSCR );
		}
		break;
	case 739:
#line 4553 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 740:
#line 4558 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    /* Should check that $2.s is "file" */
		    if (STbcompare(yypvt[-3].s, 0, ERx("file"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-3].s,
					ERx("PRINTSCREEN"), ERx("FILE") );
		}
		break;
	case 741:
#line 4565 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		}
		break;
	case 742:
#line 4570 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 743:
#line 4590 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( IINPROMPT );
		}
		break;
	case 744:
#line 4595 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    gr->gr_func = IINPROMPT;
		}
		break;
	case 745:
#line 4601 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (STbcompare(yypvt[-0].s, 0, ERx("noecho"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
						ERx("PROMPT"), ERx("NOECHO") );
		    arg_int_add( 1 );
		}
		break;
	case 746:
#line 4608 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_int_add( 0 );
		}
		break;
	case 747:
#line 4613 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    /* Null indicator argument */
		    if (gr->gr_nlsym)
			arg_var_add( gr->gr_nlsym, gr->gr_nlid );
		    else
			arg_int_add( 0 );

		    /* Prompt buffer argument */
		    if (gr->gr_sym)
		    {
			if ((gr->gr_type == T_CHAR) || (gr->gr_type == T_DBV))
			{
			    arg_var_add( gr->gr_sym, gr->gr_id );
			} else
			{
			    arg_str_add( ARG_CHAR, gr->gr_id );
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0067_grSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
			}
		    } else
		    {
			arg_str_add( ARG_CHAR, gr->gr_id );
			if (gr->gr_type != T_UNDEF)
			    er_write( E_EQ0067_grSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
		    }
		}
		break;
	case 748:
#line 4652 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		}
		break;
	case 749:
#line 4657 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_if( G_OPEN, IIFSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IISETFIELD;
		}
		break;
	case 750:
#line 4663 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 751:
#line 4675 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    gen_call( IIREDISP );
		}
		break;
	case 752:
#line 4696 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	form_no;

		    if ((form_no = lbl_gen(LBLmDISPLAY, TRUE)) != LBL_NOLEV)
		    {
			gen_goto( G_IF, L_FDBEG, 
				  lbl_value(LBLmDISPLAY, form_no) );
		    } else
			er_write( E_EQ0058_grINLOOP, EQ_ERROR, 2, yypvt[-1].s,
							    ERx("DISPLAY") );
		}
		break;
	case 753:
#line 4709 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 755:
#line 4716 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4		form_no;

		    /* No RESUME NEXT unless we're in a DISPLAY loop. */
		    if ((form_no = lbl_gen(LBLmDISPLAY, FALSE)) != LBL_NOLEV)
		    {
			switch (lbl_adget(form_no))
			{
			  /* 
			  ** RESUME should not be allowed on activate SCROLL,
			  ** FRSKEY or MENUITEM; however we don't give an
			  ** error on LBLaSCROLL, LBLaFRSKEY and LBLaMITEM 
			  ** for backward compatibility reasons.
			  */ 
			  case LBLaTIME:
			    er_write( E_EQ0307_actRESNEXT, EQ_WARN, 1,
						ERx("TIMEOUT") );
			    break;
			  default:
			    break;
			}
			gen_call( IIRESNEXT );
		    }
		}
		break;
	case 756:
#line 4741 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( IIRESFLD );
		}
		break;
	case 757:
#line 4745 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( IIRESCOL );
		}
		break;
	case 758:
#line 4749 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4		stop;

		    /* find the nearest DISPLAY or SUBMENU block, if any */
		    stop = lbl_gen( LBLmDISPLAY|LBLmSUBMENU, FALSE );
   
		    if (STbcompare(yypvt[-0].s, 0, ERx("entry"), 0, TRUE) == 0)
		    {
			/* Warning if nearest block is not a DISPLAY block */
			if ((stop != LBL_NOLEV)
				&& (lbl_get_mode(stop) == LBLmSUBMENU))
			    er_write( E_EQ0058_grINLOOP, EQ_WARN, 2,
					ERx("RESUME ENTRY"), ERx("DISPLAY") );
			else
			    gen_call( IIFRRERESENTRY );
		    }
		    else if (STbcompare(yypvt[-0].s, 0, ERx("nextfield"), 0,
			TRUE) == 0)
		    {
			/* Warning if nearest block is not a DISPLAY block */
			if ((stop != LBL_NOLEV)
				&& (lbl_get_mode(stop) == LBLmSUBMENU))
			{
			    er_write( E_EQ0058_grINLOOP, EQ_WARN, 2,
					ERx("RESUME NEXTFIELD"),
					ERx("DISPLAY") );
			}
			else
			{
			    arg_int_add( IIFRresNEXT );
			    gen_call( IIFRNXTPRVFLD );
			}
		    }
		    else if (STbcompare(yypvt[-0].s, 0, ERx("previousfield"), 0,
			TRUE) == 0)
		    {
			/* Warning if nearest block is not a DISPLAY block */
			if ((stop != LBL_NOLEV)
				&& (lbl_get_mode(stop) == LBLmSUBMENU))
			{
			    er_write( E_EQ0058_grINLOOP, EQ_WARN, 2,
					ERx("RESUME PREVIOUSFIELD"),
					ERx("DISPLAY") );
			}
			else
			{
			    arg_int_add( IIFRresPREV);
			    gen_call( IIFRNXTPRVFLD );
			}
		    }
		    else
		    {
			if (STbcompare(yypvt[-0].s, 0, ERx("menu"), 0, TRUE) != 0)
			    er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
						ERx("RESUME"), ERx("MENU") );
			/* Error if nearest block is not a DISPLAY block */
		        if ((stop != LBL_NOLEV)
				&& (lbl_get_mode(stop) == LBLmSUBMENU))
			    er_write( E_EQ0058_grINLOOP, EQ_ERROR, 2,
					ERx("RESUME MENU"), ERx("DISPLAY") );
			else
			    gen_call( IIRESMU );
		    }
		 }
		break;
	case 759:
#line 4815 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 760:
#line 4830 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    frs_close();
		}
		break;
	case 762:
#line 4837 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    frs_inqset( FRSset );
		}
		break;
	case 764:
#line 4851 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    er_write( E_EQ0077_grISUNSUPP, EQ_ERROR, 1,
						    ERx("SET_FRS") );
		    YYERROR;
		}
		break;
	case 768:
#line 4863 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    frs_setval( form_sconst(yypvt[-0].s), T_CHAR, (SYM *)0,
				(char *)0, (SYM *)0 );
		}
		break;
	case 769:
#line 4868 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    frs_setval( yypvt[-0].s, T_INT, (SYM *)0, (char *)0, (SYM *)0 );
		}
		break;
	case 770:
#line 4872 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    frs_setval(ERx("0"), T_INT, (SYM *)0, (char *)0, (SYM *)0);
		}
		break;
	case 771:
#line 4877 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    frs_setval(ERx("0"), T_INT, (SYM *)0, (char *)0, (SYM *)0);
		}
		break;
	case 772:
#line 4882 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    /* 
		    ** Only use null indicator information if we have
		    ** a valid user variable
		    */
		    if (gr->gr_sym)
		    {
			if (gr->gr_type == T_FLOAT || gr->gr_type == T_UNDEF)
			{
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0060_grINTSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
			    frs_setval( gr->gr_id, T_CHAR, (SYM *)0, 
				        (char *)0, (SYM *)0 );
			} else
			    frs_setval( gr->gr_id, gr->gr_type, gr->gr_sym,
					gr->gr_nlid, gr->gr_nlsym );
		    }
		    else
		    {
		        if (STbcompare(gr->gr_id,0,ERx("off"),0,TRUE)==0)
			    frs_setval( ERx("0"), T_INT, (SYM *)0, (char *)0,
					(SYM *)0 );
			else if (STbcompare(gr->gr_id,0,ERx("bell"),0,TRUE)==0)
			    frs_setval( ERx("2"), T_INT, (SYM *)0, (char *)0,
					(SYM *)0 );
		      	else
			    frs_setval( gr->gr_id, T_CHAR, (SYM *)0, (char *)0,
				    	(SYM *)0 );
		    }
		}
		break;
	case 773:
#line 4914 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    frs_setval( ERx("1"), T_INT, (SYM *)0, (char *)0,
				(SYM *)0 );
		}
		break;
	case 774:
#line 4941 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( IISLEEP );
		}
		break;
	case 775:
#line 4946 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 776:
#line 4979 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	cur_form = lbl_current( LBL_FORM );

		    gen_else( G_OPEN, II0, C_0, 0, L_0, 0, L_0, 0 ); 
		    gen_goto( G_IF, L_FDEND, cur_form );
		    gen_else( G_CLOSE, IIRETVAL, C_0, 0, L_0, 0, 
			      L_ACTEND, cur_form );
		    gen_goto( G_TERM, L_FDEND, cur_form );
		    gen_label( G_NOLOOP, L_MUINIT, cur_form );
		    gen_call( IIMUONLY );
		    act_close( cur_form );
		    gen_goto( G_TERM, L_FDBEG, cur_form );
		    gen_label( G_NOLOOP, L_FDEND, cur_form );
		}
		break;
	case 777:
#line 4995 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	cur_form = lbl_next( LBL_FORM );

		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    gen_goto( G_TERM, L_MUINIT, cur_form );
		    gen_label( G_NOLOOP, L_FDBEG, cur_form );
		    arg_int_add( 0 );
		    gen_call( IIRUNMU );
		    lbl_a_reset();
		    act_open();
		}
		break;
	case 780:
#line 5011 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_else( G_ELSE, IIRETVAL, C_0, 0, L_ACTELSE, 
			lbl_current(LBL_INTR), L_ACTEND, 
			lbl_current(LBL_FORM) );
		    gr->gr_mode = LBLmSUBMENU;
		}
		break;
	case 781:
#line 5019 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_mode = LBLmSUBMENU;	/* Not LBLmDISPLAY */
		}
		break;
	case 782:
#line 5023 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_mode = LBLmSUBMENU;	/* Not LBLmDISPLAY */
		}
		break;
	case 783:
#line 5028 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    act_add( ACT_MENU, lbl_current(LBL_INTR), 0, yypvt[-1].ip );
		}
		break;
	case 785:
#line 5046 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	form_no;

		    gen_if( G_OPEN, gr->gr_func, C_EQ, 0 );
		    if ((form_no = lbl_gen(LBLmDISPLAY, TRUE)) != LBL_NOLEV)
			gen_goto( G_NOTERM, L_FDBEG, 
				  lbl_value(LBLmDISPLAY, form_no) );
		    else
			er_write( E_EQ0058_grINLOOP, EQ_ERROR, 2, yypvt[-1].s,
							    ERx("DISPLAY") );
		    gen_if( G_CLOSE, gr->gr_func, C_EQ, 0 );
		}
		break;
	case 786:
#line 5060 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 787:
#line 5065 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_func = IICHKFRM;
		}
		break;
	case 788:
#line 5069 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_func = IIVALFLD;
		}
		break;
	case 789:
#line 5091 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 790:
#line 5097 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IITCLRCOL;
		}
		break;
	case 791:
#line 5103 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( cmCLEARR );
		}
		break;
	case 793:
#line 5110 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( IITCLRROW );
		}
		break;
	case 794:
#line 5114 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( IITCLRROW );
		}
		break;
	case 795:
#line 5135 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    /* Close inner and deleterow if blocks */
		    gen_if( G_CLOSE, IITDELROW, C_0, 0 );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 796:
#line 5142 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLRET;	/* May be an Out list */
		}
		break;
	case 797:
#line 5148 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( cmDELETER );
		}
		break;
	case 799:
#line 5155 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (eq->eq_flags & EQ_VERSWARN)
			er_write( E_EQ0078_grSTYUNSUPP, EQ_WARN, 1,
						    ERx("DELETEROW ... OUT") );
		}
		break;
	case 800:
#line 5162 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_int_add( FALSE );
		    gen_if( G_OPEN, IITDELROW, C_NOTEQ, 0 );
		}
		break;
	case 801:
#line 5167 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (eq->eq_flags & EQ_VERSWARN)
			er_write( E_EQ0078_grSTYUNSUPP, EQ_WARN, 1,
						    ERx("DELETEROW ... IN") );
		}
		break;
	case 802:
#line 5174 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_int_add( TRUE );	/* In list */
		    gen_if( G_OPEN, IITDELROW, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 803:
#line 5192 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 804:
#line 5198 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLRET;
		}
		break;
	case 805:
#line 5204 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( cmGETR );
		}
		break;
	case 806:
#line 5222 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( IITFILL );
		    gen_if( G_CLOSE, IITBINIT, C_0, 0 );
		}
		break;
	case 807:
#line 5228 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_if( G_OPEN, IITBINIT, C_NOTEQ, 0 );
		}
		break;
	case 808:
#line 5233 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 812:
#line 5242 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_call( IITHIDECOL );
		}
		break;
	case 816:
#line 5259 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{ gr->gr_sym = (SYM *)0; }
		break;
	case 817:
#line 5261 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_str_add( ARG_CHAR,  id_getname() );
		    id_free();
		    gen_call( IITHIDECOL );

		  /* Can't use literals and a variable too. */
		    if (gr->gr_sym && (yypvt[-1].i || yypvt[-0].i))
			er_write( E_EQ0074_grMISVAR, EQ_ERROR, 2, gr->gr_id,
							    ERx("INITTABLE") );
		}
		break;
	case 819:
#line 5276 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 0;		/* Paren not stuff present */
		}
		break;
	case 820:
#line 5280 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    id_add( yypvt[-2].s );
		    id_add( yypvt[-1].s );
		    id_add( yypvt[-0].s );
		    yyval.i = 1;		/* Paren stuff present */
		}
		break;
	case 821:
#line 5287 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    id_add( yypvt[-4].s );
		    id_add( yypvt[-3].s );
		    id_add( yypvt[-2].s );
		    id_add( yypvt[-1].s );
		    id_add( yypvt[-0].s );
		    yyval.i = 1;		/* Paren stuff present */
		}
		break;
	case 822:
#line 5297 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    id_add( form_sconst(yypvt[-0].s) );
		}
		break;
	case 823:
#line 5301 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_sym)
		    {
			if (gr->gr_type == T_CHAR)
			    arg_var_add( gr->gr_sym, gr->gr_id );
			else
			{
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0062_grNAME, EQ_ERROR, 1,
								    gr->gr_id );
			    id_add( gr->gr_id );
			}
		    }
		    else
			id_add( gr->gr_id );
		}
		break;
	case 824:
#line 5323 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 0;		/* No NULL clause */
		}
		break;
	case 825:
#line 5327 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    id_add( ERx(" with null") );
		    yyval.i = 1;		/* With NULL clause */
		}
		break;
	case 826:
#line 5332 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (yypvt[-0].i == 0)
			id_add( ERx(" not null") );
		    else
			id_add( ERx(" not null with default") );
		    yyval.i = 1;		/* With NULL clause */
		}
		break;
	case 827:
#line 5341 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (STbcompare(yypvt[-1].s, 0, ERx("not"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
						ERx("INITTABLE"), ERx("NOT") );
		}
		break;
	case 828:
#line 5348 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 1;
		    if (STbcompare(yypvt[-0].s, 0, ERx("default"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					    ERx("INITTABLE"), ERx("DEFAULT") );
		}
		break;
	case 829:
#line 5355 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 0;
		}
		break;
	case 830:
#line 5370 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    /*
		    **  Add dummy argument for IItbsetio statement
		    **  which is not used by purgetable.
		    */
		    arg_int_add( 0 );
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 831:
#line 5382 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmPURGE );
		}
		break;
	case 832:
#line 5405 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    /* Close inner and insertrow if blocks */
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITINSERT, C_0, 0 );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 833:
#line 5413 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gen_if( G_OPEN, IITINSERT, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 834:
#line 5420 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( cmINSERTR );
		}
		break;
	case 835:
#line 5443 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBACT, C_0, 0 );
		}
		break;
	case 836:
#line 5449 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_int_add( TRUE );		/* Loading */
		    gen_if( G_OPEN, IITBACT, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 837:
#line 5456 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 840:
#line 5464 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    frs_inqset(FRSload);
		    frs_object(ERx("row"));
		}
		break;
	case 841:
#line 5481 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 842:
#line 5487 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 843:
#line 5493 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( cmPUTR );
		}
		break;
	case 844:
#line 5519 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    /* Close inner and scroll if blocks */
		    gen_if( G_CLOSE, IITSCROLL, C_0, 0 );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 845:
#line 5526 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_int_add( rowNONE );
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		}
		break;
	case 846:
#line 5532 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( cmSCROLL );
		}
		break;
	case 847:
#line 5538 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_if( G_OPEN, IITSCROLL, C_NOTEQ, 0 );
		}
		break;
	case 848:
#line 5542 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if ((yypvt[-1].i || yypvt[-0].i) && (eq->eq_flags & EQ_VERSWARN))
			er_write( E_EQ0078_grSTYUNSUPP, EQ_WARN, 1,
							ERx("SCROLL IN/OUT") );
		}
		break;
	case 849:
#line 5549 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gen_call( IITBSMODE );
		    arg_int_add( FALSE );	/* Ignore In flag */
		}
		break;
	case 851:
#line 5557 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_int_add( rowEND );
		}
		break;
	case 852:
#line 5562 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gen_call( IITBSMODE );
		}
		break;
	case 853:
#line 5567 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gen_call( IITBSMODE );
		}
		break;
	case 854:
#line 5573 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 0;
		}
		break;
	case 855:
#line 5577 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 1;
		}
		break;
	case 856:
#line 5582 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_func = IITCOLRET;
		}
		break;
	case 857:
#line 5587 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_int_add( FALSE );
		    arg_int_add( rowNONE );
		    gen_if( G_OPEN, IITSCROLL, C_NOTEQ, 0 );
		    yyval.i = 0;
		}
		break;
	case 858:
#line 5594 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 1;
		}
		break;
	case 859:
#line 5599 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_int_add( TRUE );	/* In list */
		    arg_int_add( rowNONE );
		    gen_if( G_OPEN, IITSCROLL, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 860:
#line 5621 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	cur_table = lbl_current( LBL_TBL );

		    /* Close tabledata loop */
		    gen_loop( G_CLOSE, L_TBBEG, L_TBEND, cur_table, IITDATA,
			      C_0, 0 );
		    if (lbl_is_used(LBL_TBL))
			gen_label( G_LOOP, L_TBEND, cur_table );
		    gen_call( IITDATEND );
		}
		break;
	case 861:
#line 5632 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	cur_table = lbl_next( LBL_TBL );

		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    if (lbl_gen(LBLmFORMDATA, FALSE) == LBL_NOLEV)
			er_write( E_EQ0058_grINLOOP, EQ_ERROR, 2, yypvt[-0].s,
							    ERx("FORMDATA") );
		    gr->gr_mode = LBLmTBDATA;
		    gen_loop( G_OPEN, L_TBBEG, L_TBEND, cur_table, IITDATA, 
			      C_NOTEQ, 0 );
		}
		break;
	case 862:
#line 5662 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	cur_table = lbl_current( LBL_TBL );

		    /* Close unloadtable loop */
		    gen_loop( G_CLOSE, L_TBBEG, L_TBEND, cur_table, IITUNLOAD,
			      C_0, 0 );
		    gen_label( G_LOOP, L_TBEND, cur_table );
		    gen_call( IITUNEND );
		}
		break;
	case 863:
#line 5673 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( IITBCECOLEND );
		}
		break;
	case 864:
#line 5678 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	cur_table = lbl_next( LBL_TBL );

		  /* Nested unloadtable? */
		    if (lbl_gen(LBLmTBUNLD,FALSE) != LBL_NOLEV)
			er_write( E_EQ0068_grUNLOADTB, EQ_ERROR, 0 );
		    gr->gr_mode = LBLmTBUNLD;
		    arg_int_add( FALSE );		/* Unload flag */
		    gen_if_goto( IITBACT, C_EQ, 0, L_TBEND, cur_table );
		    gen_loop( G_OPEN, L_TBBEG, L_TBEND, cur_table, IITUNLOAD, 
			      C_NOTEQ, 0 );
		    gr->gr_func = IITCOLRET;
		}
		break;
	case 865:
#line 5693 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 866:
#line 5714 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4	stop_level;

		    arg_int_add( 0 );
		    gen_if( G_OPEN, IITVALVAL, C_EQ, 0 );
		    if ((stop_level = lbl_gen(LBLmDISPLAY, TRUE)) == LBL_NOLEV)
			er_write( E_EQ0058_grINLOOP, EQ_ERROR, 2, yypvt[-1].s,
							    ERx("DISPLAY") );
		    else
			gen_goto( G_NOTERM, L_FDBEG, 
				  lbl_value(LBL_FORM, stop_level) );
		    /* Close inner and validrow if blocks */
		    gen_if( G_CLOSE, IITVALVAL, C_0, 0 );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 867:
#line 5731 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{	
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLVAL;
		    yyval.s = yypvt[-3].s;
		}
		break;
	case 868:
#line 5738 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( cmVALIDR );
		}
		break;
	case 870:
#line 5745 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( IITVALROW );
		}
		break;
	case 871:
#line 5749 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( IITVALROW );
		}
		break;
	case 872:
#line 5761 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = form_sconst(yypvt[-0].s);
		}
		break;
	case 873:
#line 5769 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 874:
#line 5773 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_sym)
		    {
			if (gr->gr_type == T_CHAR)
			    arg_var_add( gr->gr_sym, gr->gr_id );
			else
			{
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0062_grNAME, EQ_ERROR, 1,
								    gr->gr_id );
			    arg_str_add( ARG_CHAR, gr->gr_id );
			}
		    }
		    else
		    {
			arg_str_add( ARG_CHAR, gr->gr_id );
		    }
		}
		break;
	case 875:
#line 5796 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 877:
#line 5805 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_sym && gr->gr_type == T_CHAR)
			arg_var_add( gr->gr_sym, gr->gr_id );
		    else
		    {
			arg_str_add( ARG_CHAR, gr->gr_id );
			if (gr->gr_type != T_UNDEF)
			    er_write( E_EQ0067_grSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
		    }
		}
		break;
	case 878:
#line 5824 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 879:
#line 5828 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 882:
#line 5845 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 883:
#line 5849 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    char	ibuf[50];
		    char	*s_int = yypvt[-0].s;

		    if (*(yypvt[-1].s) == '-')
		    {
			ibuf[0] = '-';
			ibuf[1] = '\0';
			s_int = str_add( STRNULL, STcat(ibuf, s_int) );
		    }
		    yyval.s = s_int;
		}
		break;
	case 884:
#line 5866 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 885:
#line 5870 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    char	fbuf[50];
		    char	*s_flt = yypvt[-0].s;

		    if (*(yypvt[-1].s) == '-')
		    {
			fbuf[0] = '-';
			fbuf[1] = '\0';
			s_flt = str_add( STRNULL, STcat(fbuf, s_flt) );
		    }
		    yyval.s = s_flt;
		}
		break;
	case 886:
#line 5887 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 887:
#line 5891 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    char	dbuf[50];
		    char	*s_dec = yypvt[-0].s;

		    if (*(yypvt[-1].s) == '-')
		    {
			dbuf[0] = '-';
			dbuf[1] = '\0';
			s_dec = str_add( STRNULL, STcat(dbuf, s_dec) );
		    }
		    yyval.s = s_dec;
		}
		break;
	case 888:
#line 5908 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_str_add( ARG_INT, yypvt[-0].s );
		}
		break;
	case 889:
#line 5912 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    arg_int_add( 0 );
		}
		break;
	case 890:
#line 5917 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    arg_int_add( 0 );
		}
		break;
	case 891:
#line 5922 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_sym && gr->gr_type == T_INT)
			arg_var_add( gr->gr_sym, gr->gr_id );
		    else
		    {
			arg_int_add( 0 );
			if (gr->gr_type != T_UNDEF)
			    er_write( E_EQ0059_grINT, EQ_ERROR, 1, gr->gr_id );
		    }
		}
		break;
	case 899:
#line 5955 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 900:
#line 5959 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    switch ( gr->gr_func )
		    {
		      case IISETFIELD:
		      case IITCOLSET:
			arg_push();
			arg_int_add( TRUE );
			gen_call( IIPUTOPER );
			switch (gr->gr_type)
			{
			  case T_INT:
			  case T_DBV:
			  case T_UNDEF:
			    break;
			  default:
			    er_write( E_EQ0053_grFMINTFUNC, EQ_ERROR, 1, yypvt[-5].s );
			    break;
			}
		 	break;

		      default:
			er_write( E_EQ0052_grFMFUNC, EQ_ERROR, 1, yypvt[-5].s );
			break;
		    }
		    gen_call( gr->gr_func );
		}
		break;
	case 901:
#line 5986 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 902:
#line 5994 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    /* Null indicator argument to IIput */
		    if (gr->gr_nlsym)
		    {
			arg_var_add( gr->gr_nlsym, gr->gr_nlid );
		    }
		    else
		    {
			arg_int_add( 0 );
		    }
		    /* Data argument to IIput */
		    if (gr->gr_sym)
		    {
			arg_var_add( gr->gr_sym, gr->gr_id );
		    }
		    else
		    {
			arg_str_add( ARG_CHAR, gr->gr_id );
			er_write( E_EQ0057_grHOSTVAR, EQ_ERROR, 1, gr->gr_id );
		    }
		}
		break;
	case 903:
#line 6016 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_int_add( 0 );		/* No null indicator */
		    arg_str_add( ARG_INT, yypvt[-0].s );
		    gr->gr_type = T_INT;
		}
		break;
	case 904:
#line 6022 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_int_add( 0 );		/* No null indicator */
		    arg_str_add( ARG_FLOAT, yypvt[-0].s );
		    gr->gr_type = T_FLOAT;
		}
		break;
	case 905:
#line 6028 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_int_add( 0 );		/* No null indicator */
		    arg_str_add( ARG_PACK, yypvt[-0].s );
		    gr->gr_type = T_PACKASCHAR;
		}
		break;
	case 906:
#line 6034 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_int_add( 0 );		/* No null indicator */
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gr->gr_type = T_CHAR;
		}
		break;
	case 907:
#line 6049 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{	
		    arg_int_add( 0 );
		    arg_var_add( gr->gr_null, ERx("null") ); 
		}
		break;
	case 908:
#line 6065 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 1;
		}
		break;
	case 909:
#line 6069 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 1;
		}
		break;
	case 910:
#line 6073 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 0;
		}
		break;
	case 915:
#line 6084 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 916:
#line 6088 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    switch ( gr->gr_func )
		    {
		      case IIRETFIELD:
		      case IITCOLRET:
			arg_push();
			arg_int_add( TRUE );
			gen_call( IIGETOPER );
			if (yypvt[-5].i != T_INT && yypvt[-5].i != T_DBV && yypvt[-5].i != T_UNDEF)
			    er_write( E_EQ0053_grFMINTFUNC, EQ_ERROR, 1, yypvt[-3].s );
		 	break;

		      default:
			er_write( E_EQ0052_grFMFUNC, EQ_ERROR, 1, yypvt[-3].s );
			break;
		    }
		    gen_call( gr->gr_func );
		}
		break;
	case 917:
#line 6109 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->gr_nlsym)
			arg_var_add( gr->gr_nlsym, gr->gr_nlid );
		    else
			arg_int_add( 0 );
		    arg_var_add( gr->gr_sym, gr->gr_id );
		    yyval.i = gr->gr_type;
		}
		break;
	case 919:
#line 6122 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_int_add( rowCURR );
		}
		break;
	case 921:
#line 6130 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    if (lbl_gen(LBLmDISPLAY, FALSE) == LBL_NOLEV)
			er_write( E_EQ0056_grFORMNAME, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 922:
#line 6139 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    er_write( E_EQ0071_grASSGNOP, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 927:
#line 6187 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->F_usedec = FDEC_NONE|FDEC_BLOCK;
		}
		break;
	case 928:
#line 6193 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->F_usedec = FDEC_NONE;
		}
		break;
	case 938:
#line 6221 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    sym_s_begin( F_CLOSURE );
		  /* 1: predefined. 2: global "DECLARE" block. 3: PROC block */
		    if (++gr->F_blk > 3)
			er_write( E_E10006_hfNESTFUNCDEF, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 939:
#line 6231 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    /* gr->F_blk will always be >= 2 */
		    if (gr->F_blk > 1)
			_VOID_ sym_s_end( gr->F_blk-- );
		}
		break;
	case 941:
#line 6240 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 943:
#line 6247 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    sc_eat( gen_code, SC_NEST|SC_SEEN, ERx(")"), '(', ')' );
		}
		break;
	case 945:
#line 6263 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_flag &= ~GR_HOSTCODE;
		}
		break;
	case 946:
#line 6289 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
#ifdef VMS
		    arg_str_add( ARG_RAW, ERx("IIxintrans") );
#endif
		    gen_call( IIPARSET );
		}
		break;
	case 947:
#line 6298 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
#ifdef VMS
		    arg_str_add( ARG_RAW, ERx("IIxouttrans") );
#endif
		    gen_call( IIPARRET );
		}
		break;
	case 949:
#line 6310 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_send();
		}
		break;
	case 951:
#line 6319 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 953:
#line 6326 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    SYM		*sy;

		    sy = sym_resolve( (SYM *)0, ERx("character"), 
			F_CLOSURE, syFisTYPE );
		    arg_var_add( sy, str_add(STRNULL, id_getname()) );
		    id_free();
		}
		break;
	case 954:
#line 6337 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    SYM		*sy;

		    sy = sym_resolve( (SYM *)0, ERx("integer"),
			F_CLOSURE, syFisTYPE );
		    arg_var_add( sy, str_add(STRNULL, id_getname()) );
		    id_free();
		}
		break;
	case 955:
#line 6348 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    id_add( yypvt[-0].s );
		}
		break;
	case 957:
#line 6355 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    sc_eat( id_add, SC_NEST|SC_SEEN, ERx(")"), '(', ')' );
		}
		break;
	case 958:
#line 6369 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (yypvt[-0].i)
			er_write( E_EQ0055_grFMPARAM, EQ_ERROR, 0 );
#ifdef VMS
		    arg_str_add( ARG_RAW, ERx("IIxouttrans") );
#endif
		    gen_call( gr->gr_func );
		}
		break;
	case 959:
#line 6380 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (yypvt[-0].i)
			er_write( E_EQ0055_grFMPARAM, EQ_ERROR, 0 );
#ifdef VMS
		    arg_str_add( ARG_RAW, ERx("IIxintrans") );
#endif
		    gen_call( gr->gr_func );
		}
		break;
	case 961:
#line 6394 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 0;		/* No error */
		    switch (gr->gr_func)
		    {
		      case IIRETFIELD:
			gr->gr_func = IIRF_PARAM;
			break;

		      case IISETFIELD:
			gr->gr_func = IISF_PARAM;
			break;

		      case IITCOLRET:	
			gr->gr_func = IITRC_PARAM;
			break;
    
		      case IITCOLSET:	
			gr->gr_func = IITSC_PARAM;
			break;

		      case IICSRETRIEVE:
			gr->gr_func = IICSPARGET;
			break;
    
		      default:
			yyval.i = 1;	/* Error in function */
			break;
		    }
		}
		break;
	case 962:
#line 6434 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_call( IIMESSAGE );
		}
		break;
	case 963:
#line 6458 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 964:
#line 6462 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 965:
#line 6467 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 966:
#line 6471 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 967:
#line 6476 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 968:
#line 6480 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 969:
#line 6485 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    id_add( yypvt[-0].s );
		}
		break;
	case 970:
#line 6489 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    id_add(yypvt[-0].s);
		}
		break;
	case 971:
#line 6497 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 972:
#line 6507 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 973:
#line 6519 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if ((gr->gr_flag & GR_HOSTCODE) == 0)
		    {
			gen_line( ERx("host code") );
			gr->gr_flag |= GR_HOSTCODE;
		    }
		    gen_host( G_H_CODE, yypvt[-0].s );
		    gr->F_dec = GR_F_DEC;	/* Maybe dec's to follow */
		}
		break;
	case 975:
#line 6534 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->F_rec = 0;
		    gr->F_dims = 0;
		    gr->F_size = 0;
		    gr->F_indir = 0;
		    gr->F_dec = GR_F_DEC;	/* Maybe dec's to follow */
		    gr->gr_type = T_INT;
		    gr->F_storage = T_NONE;
		    gr->F_struct = (SYM *)0;
		    gen_host( G_H_NEWLINE, (char *)0 );
		}
		break;
	case 976:
#line 6546 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (gr->F_usedec & FDEC_BLOCK)	/* ## subroutine foo */
		    {
			if (gr->F_usedec & FDEC_DECLARE)
			    er_write( E_E10008_hfREDECLARE, EQ_ERROR, 0 );
			else
			{
			    gr->F_usedec = FDEC_DECLARE | FDEC_BLOCK | yypvt[-0].i;
			    gen_declare(yypvt[-0].i == FDEC_FORMS);
			}
		    } else	/* new declaration just overrides old one */
		    {
			gr->F_usedec = FDEC_DECLARE | yypvt[-0].i;
			gen_declare(yypvt[-0].i == FDEC_FORMS);

		      /* end old scope and start new one */
			_VOID_ sym_s_end( gr->F_blk );
			sym_s_begin( F_CLOSURE );
		    }

		    gr->gr_flag |= GR_HOSTCODE;
		    gr->F_dec = GR_F_DEC;	/* Maybe dec's to follow */
		}
		break;
	case 977:
#line 6572 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = FDEC_NONE;
		}
		break;
	case 978:
#line 6576 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = FDEC_FORMS;
		}
		break;
	case 985:
#line 6681 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    register SYM	*sy;

		  /* if we got tFTAG then we'll get a symtab error here */
		    sy = yyval.v = symDcEuc( yypvt[-1].s, gr->F_rec, gr->F_blk, syFisTAG,
			    F_CLOSURE, SY_TAG );
		    gr->gr_type = T_STRUCT;
		    if (sy)
			sym_s_btype( sy, T_STRUCT );
		    gr->F_dec = GR_F_DEC;
		}
		break;
	case 986:
#line 6695 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 987:
#line 6699 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 988:
#line 6706 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->F_rec++;
		    gen_host( G_H_INDENT | G_H_NEWLINE, (char *) NULL );
		}
		break;
	case 989:
#line 6713 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->F_rec--;
		    gen_host( G_H_OUTDENT | G_H_NEWLINE, (char *) NULL );
		}
		break;
	case 992:
#line 6725 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->F_dims = 0;
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
#line 6745 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    /* BUG 2259 - Explicitly set type of nested struct */
		    gr->gr_type = T_STRUCT;
		}
		break;
	case 1006:
#line 6767 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		  /* gr->F_intval is set by Fopt_star_len_comma */
		    gr->F_dims = (gr->F_intval ? 1 : 0);
		    gr->F_intval = 0;
		}
		break;
	case 1007:
#line 6775 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->F_dims = 0;		/* not an array */
		  /* gr->F_intval is set by Fopt_star_len */
		    if (gr->F_intval)		/* *size */
			gr->F_size = gr->F_intval;
		    gr->F_intval = 0;
		}
		break;
	case 1008:
#line 6785 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->F_intval = 0;
		}
		break;
	case 1009:
#line 6789 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_OP, ERx(" ") );	/* last token was OP */
		}
		break;
	case 1011:
#line 6796 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->F_intval = yypvt[-0].i;
		}
		break;
	case 1020:
#line 6836 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		  /*           name  dims                size  */
		    F_DECLARE( yypvt[-2].s, yypvt[-1].i, (gr->F_intval ? 1 : gr->F_dims) );
		    gr->F_intval = 0;
		}
		break;
	case 1021:
#line 6844 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		  /*           name  dims    size        */
		    F_DECLARE( yypvt[-2].s, yypvt[-0].i, gr->F_size );
		    gr->F_intval = 0;
		}
		break;
	case 1022:
#line 6852 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.i = 0;
		}
		break;
	case 1024:
#line 6859 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    sc_eat( gen_code, SC_SEEN|SC_NEST, ERx(")"), '(', ')' );
		    yyval.i = 1;
		}
		break;
	case 1028:
#line 6871 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    sc_eat( gen_code, SC_SEEN, ERx("/"), '/', '/' );
		}
		break;
	case 1029:
#line 6877 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_type = T_INT;
		}
		break;
	case 1034:
#line 6891 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->F_dec = GR_F_DEC;
		}
		break;
	case 1035:
#line 6897 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->F_dec = GR_F_TAG;
		}
		break;
	case 1040:
#line 6911 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		  /*           name  dims size */
		    F_DECLARE( yypvt[-1].s, yypvt[-0].i, 0 );
		}
		break;
	case 1047:
#line 6938 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_INDENT | G_H_NEWLINE, (char *) NULL );
		}
		break;
	case 1048:
#line 6944 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_OUTDENT | G_H_NEWLINE, (char *) NULL );
		}
		break;
	case 1049:
#line 6950 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_NEWLINE, (char *) NULL );
		}
		break;
	case 1050:
#line 6960 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->F_size = 0;		/* for IIvmsret! */
		    gr->F_indir = 0;
		    gr->F_dims = 0;
		    gr->gr_type = T_CHAR;
		    gr->F_dec = GR_F_DEC;
		    gr->F_intval = 0;
		}
		break;
	case 1051:
#line 6972 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1052:
#line 6978 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1053:
#line 6984 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1054:
#line 6990 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->F_dec = GR_F_DEC;
		}
		break;
	case 1055:
#line 6997 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_NEWLINE, (char *)0 );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->F_dec = GR_F_DEC;
		}
		break;
	case 1056:
#line 7005 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    CVan( yypvt[-0].s, &gr->F_intval );
		}
		break;
	case 1057:
#line 7010 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    i4		i;

		    CVan( yypvt[-0].s, &gr->F_intval );
		    if (*(yypvt[-1].s) == '-')
		    {
			gen_host( G_H_OP, ERx("-") );
			gr->F_intval = -gr->F_intval;
		    }
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1058:
#line 7024 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 1059:
#line 7030 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1060:
#line 7036 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 1061:
#line 7042 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1062:
#line 7049 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gen_host( G_H_OP, ERx(" ") );
		}
		break;
	case 1063:
#line 7056 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_NEWLINE, (char *)0 );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->F_dec = GR_F_DEC;
		}
		break;
	case 1064:
#line 7064 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_NEWLINE, (char *)0 );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->F_dec = GR_F_DEC;
		}
		break;
	case 1065:
#line 7072 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->F_dec = GR_F_TAG;		/* expect a tag */
		}
		break;
	case 1066:
#line 7079 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 1067:
#line 7085 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->F_dec = GR_F_TAG;		/* expect a tag */
		}
		break;
	case 1068:
#line 7092 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    register SYM	*sy;

		    gen_host( G_H_KEY, yypvt[-0].s );
		    sy = sym_resolve( (SYM *)0, yypvt[-0].s, F_CLOSURE, syFisTAG );
		    gr->F_struct = sy;
		    gr->gr_type = T_STRUCT;
		    gr->F_dec = GR_F_DEC;
		}
		break;
	case 1069:
#line 7104 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    SYM		*sy;

		    sy = sym_resolve( (SYM *)0, yypvt[-0].s, F_CLOSURE, syFisTYPE );
		    if (!sy)
			er_write( E_E10001_hfBADTYPE, EQ_ERROR, 1, yypvt[-0].s );
		    else
		    {
			gr->F_size = sym_g_dsize( sy );
			gr->F_indir = sym_g_indir( sy );
			gr->gr_type = sym_g_btype( sy );
		    }
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->F_dec = GR_F_DEC;
		    gr->F_intval = 0;
		}
		break;
	case 1070:
#line 7121 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->F_size = 8;
		    gr->F_indir = 0;
		    gr->gr_type = T_FLOAT;
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->F_dec = GR_F_DEC;
		    gr->F_intval = 0;
		}
		break;
	case 1071:
#line 7131 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->F_size = 8;
		    gr->F_indir = 0;
		    gr->gr_type = T_FLOAT;
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->F_dec = GR_F_DEC;
		    er_write( E_E10001_hfBADTYPE, EQ_ERROR, 1,
							ERx("DOUBLE COMPLEX") );
		    gr->F_intval = 0;
		}
		break;
	case 1072:
#line 7143 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->F_size = 8;
		    gr->F_indir = 0;
		    gr->gr_type = T_FLOAT;
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->F_dec = GR_F_DEC;
		    er_write( E_E10001_hfBADTYPE, EQ_ERROR, 1, yypvt[-0].s );
		    gr->F_intval = 0;
		}
		break;
	case 1073:
#line 7155 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1078:
#line 7171 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gen_host( G_H_OP, ERx("=") );
		}
		break;
	case 1079:
#line 7178 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    register SYM	*sy;

		    if (gr->gr_type != T_NONE)
		    {
			if (sy=symDcEuc(yypvt[-2].s, 0, gr->F_blk, syFisCONST,
			    F_CLOSURE, SY_NORMAL))
			{
			    sym_s_btype( sy, gr->gr_type );
			    sym_s_dsize( sy, gr->F_size );
			}
		    } else
			er_write( E_E10002_hfDEFINE, EQ_ERROR, 1, yypvt[-2].s );
		}
		break;
	case 1080:
#line 7195 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_type = T_INT;
		    gr->F_size = gr->F_intsize;
		}
		break;
	case 1081:
#line 7200 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_type = T_FLOAT;
		    gr->F_size = sizeof(f4);	/* real constants are real*4 */
		}
		break;
	case 1082:
#line 7205 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_type = T_CHAR;
		    gr->F_size = STlength( yypvt[-0].s );
		    gen_host( G_H_SCONST, yypvt[-0].s );
		}
		break;
	case 1083:
#line 7211 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->gr_type = T_NONE;
		}
		break;
	case 1084:
#line 7217 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1085:
#line 7221 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (*(yypvt[-1].s) == '-')
			gen_host( G_H_OP, ERx("-") );
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1086:
#line 7227 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1087:
#line 7231 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (*(yypvt[-1].s) == '-')
			gen_host( G_H_OP, ERx("-") );
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1088:
#line 7240 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1089:
#line 7244 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1090:
#line 7249 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1095:
#line 7267 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    SYM		*sy;

		    gr->gr_type = T_UNDEF;		/* Error default */
		    if ((sy = yypvt[-0].v) == (SYM *)0)	/* Enter undefined */
		    {
			er_write( E_EQ0057_grHOSTVAR, EQ_ERROR, 1,
								id_getname() );
		    	sy = symDcEuc( id_getname(), 0, gr->F_blk, 
			       syFisVAR, F_CLOSURE, SY_NORMAL );
		    	if (sy)
			    sym_s_btype( sy, T_UNDEF );
		    } else if (!syBitAnd(sym_g_useof(sy),syFisVAR) ||
			      sym_g_btype(sy) == T_STRUCT)
			er_write( E_EQ0057_grHOSTVAR, EQ_ERROR, 1,
								id_getname() );
		    if (sy)
			gr->gr_type = sym_g_btype( sy );
		    gr->gr_sym = sy;
		    gr->gr_id = str_add( STRNULL, id_getname() );
		    id_free();
		}
		break;
	case 1096:
#line 7297 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    if (yypvt[-0].v)
			gr->gr_type = sym_g_btype( yypvt[-0].v );
		    else
			gr->gr_type = T_NONE;
		    gr->gr_sym = yypvt[-0].v;
		    gr->gr_id = str_add( STRNULL, id_getname() );
		    id_free();
		}
		break;
	case 1097:
#line 7310 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    register SYM	*sy = yypvt[-0].v;

		    if (sy)
		    {
		      /* Check to see if subscription was required */
			if (sym_g_btype(sy)==T_CHAR)
			{
			  /* not enough dimensions */
			    if (gr->F_dims < (i4)sym_g_dims(sy))
			    {
				er_write( E_E10003_hfINDIR, EQ_ERROR, 3,
								id_getname(),
				    er_na(gr->F_dims), er_na(sym_g_dims(sy)) );
			    }
			  /* too many dimensions+substringing */
			    else if (gr->F_dims > 
				     (i4)sym_g_dims(sy) + sym_g_dsize(sy)
				    )
			    {
				er_write( E_E10003_hfINDIR, EQ_ERROR, 3,
				    id_getname(), er_na(gr->F_dims),
				    er_na(sym_g_dims(sy)) );
			    }
			} else if (gr->F_dims != sym_g_dims(sy))
			    er_write( E_E10003_hfINDIR, EQ_ERROR, 3,
				id_getname(), er_na(gr->F_dims),
				er_na(sym_g_dims(sy)) );
			
		    }
		    gr->F_struct = (SYM *)0;	/* Reset for next time */
		    sym_f_init();
		    gr->F_dims = 0;
		    yyval.v = yypvt[-0].v;
		}
		break;
	case 1098:
#line 7348 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->F_struct = (SYM *)0;
		    sym_f_init();
		    gr->F_dims = 0;
		    gr->F_dec = GR_F_USE;
		}
		break;
	case 1099:
#line 7364 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.v = yypvt[-1].v;
		}
		break;
	case 1100:
#line 7368 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    yyval.v = yypvt[-1].v;
		}
		break;
	case 1101:
#line 7372 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    id_add( yypvt[-0].s );
		    yyval.v = (SYM *)0;
		}
		break;
	case 1105:
#line 7388 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    register SYM	*sy = yypvt[-1].v;	/* MUST BE SET */

		    if (sy)
		    {
			register int	btype;

			/* 
			** Is this an Equel array reference?  Base type
			** [Equel type is defined] and
			** (is array or char string).
			*/
			btype = sym_g_btype(sy);
			if (btype!=T_NONE &&		/* defined */
			    (sym_g_dims(sy) ||		/* array */
				(btype==T_CHAR && F_issubstring())) /* string */
			   )
			{
			    sc_eat( id_add, SC_SEEN|SC_NEST, ERx(")"),
								    '(', ')' );
			    gr->F_dims = 1;
			} 
			else
			{
			    SC_PTR--;
			}
		    } 
		    else
		    {
			SC_PTR--;
		    }
		    yyval.v = yypvt[-1].v;
		}
		break;
	case 1106:
#line 7424 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    register SYM	*sy = yypvt[-1].v;	/* MUST BE SET */

		    if (sy)
		    {
			register int	btype;

			/* 
			** Is this an Equel substring reference?
			** Base type is CHAR, and either not an array
			** or already indexed.
			*/
			btype = sym_g_btype(sy);
			if (btype==T_CHAR &&
			    (!sym_g_dims(sy) || gr->F_dims) && F_issubstring())
			{
			    sc_eat( id_add, SC_SEEN|SC_NEST, ERx(")"),
								    '(', ')' );
			} 
			else
			{
			    SC_PTR--;
			}
		    } 
		    else
		    {
			SC_PTR--;
		    }
		    yyval.v = yypvt[-1].v;
		}
		break;
	case 1107:
#line 7457 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    SYM		*sy;
		    i2		stat;

		    id_add( yypvt[-0].s );
		    yyval.v = sym_resolve( gr->F_struct, yypvt[-0].s, 0, 
			      syFisVAR|syFisCONST );
		}
		break;
	case 1108:
#line 7482 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    SYM		*sy;

		    id_add( yypvt[-0].s );
		    /* Use F_struct as the parent pointer */
		    sy = sym_resolve( gr->F_struct, yypvt[-0].s, F_CLOSURE, syFisVAR );
		    if (sy == (SYM *)0)
			er_write( E_E10004_hfSTRUCTREF, EQ_ERROR, 1,
								id_getname() );
		  /* Reset here, because it looks ahead for left paren */
		    gr->F_struct = (SYM *)0;
		    yyval.v = sy;		/* Set for $0 of Use_paren */
		}
		break;
	case 1111:
#line 7500 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    gr->F_dims = 0;
		}
		break;
	case 1113:
#line 7506 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    register SYM	*sy = gr->F_struct;

		    /* 
		    ** Is this an Equel array reference?  Base type 
		    ** (Equel type is defined) and has array bit set.
		    ** Whether or not this is an array reference, eat
		    ** up the left paren!  Otherwise we will get a syntax
		    ** error at the left paren.  There is a symbol table
		    ** entry but we may not know it as an array.
		    */
		    if (sy)	/* "must be true" */
		    {
			if (sym_g_btype(sy) != T_NONE)
			{
			    if (sym_g_dims(sy))
				gr->F_dims = 1;
			    else
				er_write( E_E10003_hfINDIR, EQ_ERROR, 3,
				    id_getname(), er_na(gr->F_dims +1),
				    er_na(sym_g_dims(sy)) );
			}
		    }
		    sc_eat( id_add, SC_SEEN|SC_NEST, ERx(")"), '(', ')' );
		}
		break;
	case 1114:
#line 7534 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    id_add( yypvt[-0].s );
		    gr->F_struct = sym_resolve(gr->F_struct, yypvt[-0].s, F_CLOSURE, 
				syFisVAR);
		}
		break;
	case 1115:
#line 7542 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortran/fortran.yf"
{
		    id_add( yypvt[-0].s );
		}
		break;
		}
	return 0; /* Success */

	}
