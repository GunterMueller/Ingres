
#line 6 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
/* %L language - Which language are we using for symbol table? */
/* %L merge <eqgram.my> -- get the master grammar */
/* %L fake begin */
/* %L fake end */
/* %L language begin 
** Define EQ_X_LANG where X is the EUC or PL1 for the symbol table
*/
# define	EQ_EUC_LANG
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
#define	tLBRKT		405
#define	tRBRKT		406
#define	tSEMICOL		407
#define	tARROW		408
#define	tLBRACE		409
#define	tRBRACE		410
#define	tCTYPE		411
#define	tCTYPE_I		412
#define	tCTYPE_L		413
#define	tCTYPE_S		414
#define	tCTYPE_F		415
#define	tCTYPE_D		416
#define	tCTYPE_C		417
#define	tCUNSIGNED		418
#define	tCTAG		419
#define	tCDECNAME		420
#define	tCSTORAGE		421
#define	tCEXTERN		422
#define	tCTYPEDEF		423
#define	tCSTRUCT		424
#define	tCENUM		425
#define	tCENUMTAG		426
#define	tCSTRCT_VAR		427
#define	tCFUNCVAR		428
#define	tCLPAR_STAR		429
#define	tCVARCHAR		430
#define	tARUOP		431
#define	tBNDOP		432
#define	tEQOP		433
#define	tNEQOP		434
#define	tLOGUOP		435
#define	tLOGBOP		436
#define	tAGOP		437
#define	tMULOP		438
#define	tDIVOP		439
#define	tEXPOP		440
#define	tBINARYOP		441
#define	tUNARYOP		442

#line 333 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
/* %L locals - Include L locals and gr structure */
/* %L locals begin - Local variables and gr structure for C */
# include	<eqc.h>
# include	<ere0.h>

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

	/* Add L (C) dependent members after this statement */
	i4	C_blk;		/* C block level */
	i4	C_rec;		/* C record level */
	i4	C_storage;	/* extern, static, auto, register, typedef */
	i4	C_size;		/* Size of declaration */
	i4	C_indir;	/* Indirection of variables */
	i4	C_dec;		/* In declaration or usage */
	i4	C_isfunc;	/* Is function declaration */
	i4	C_varchar;	/* Is varchar declaration */
	SYM	*C_struct;	/* Special pointer for references to structs */
	STR_TABLE *C_strvals;	/* String table for C values */
	C_VALS	*C_cvals;	/* C value structure */
};
GLOBALDEF struct gr_state _gr_local ZERO_FILL;
GLOBALDEF struct gr_state *gr = &_gr_local;

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

#line 7561 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"


/* %L mechanism */
/* %L mechanism begin - All the stuff to do with gr_mechanism() */

GLOBALDEF GR_TYPES   gr_typetab[] = {
		ERx("short"), 		T_INT, 		sizeof(short),
		ERx("int"), 		T_INT, 		sizeof(int),
		ERx("long"), 		T_INT, 		sizeof(long),
		ERx("void"), 		T_INT,		sizeof(int),
		ERx("float"), 		T_FLOAT,	sizeof(float),
		ERx("double"), 		T_FLOAT,	sizeof(double),
		ERx("char"), 		T_CHAR,		0,
		ERx("i8"),		T_INT,		sizeof(i8),
		ERx("u_i8"),		T_INT,		sizeof(i8),
		/* Stop here if not including the CL types */
		(char *)0,  		0, 		0,
		ERx("i1"), 		T_INT,		sizeof(i1),
		ERx("u_i1"), 		T_INT,		sizeof(u_i1),
		ERx("i2"), 		T_INT, 		sizeof(i2),
		ERx("u_i2"), 		T_INT, 		sizeof(u_i2),
		ERx("i4"), 		T_INT, 		sizeof(i4),
		ERx("u_i4"), 		T_INT, 		sizeof(u_i4),
		ERx("f4"), 		T_FLOAT,	sizeof(f4),
		ERx("f8"), 		T_FLOAT,	sizeof(f8),
		ERx("bool"), 		T_INT,		sizeof(bool),
		ERx("u_char"), 		T_CHAR,		0,
		ERx("STATUS"),		T_INT,		sizeof(STATUS),
		ERx("DB_DATA_VALUE"),	T_DBV,		0,
		ERx("DATE"),		T_DATE,		0,
		ERx("PTR"),		T_INT,		sizeof(PTR),
		ERx("ER_MSGID"),	T_INT,		sizeof(ER_MSGID),
		ERx("BITFLD"),		T_INT,		sizeof(BITFLD),
		ERx("VOID"), 		T_INT,		sizeof(i4),
		ERx("SIZE_TYPE"),	T_INT,		sizeof(SIZE_TYPE),
		ERx("OFFSET_TYPE"),	T_INT,		sizeof(OFFSET_TYPE),

		/*
		** Table separator for internal variables -
		** added for Kanji
		*/
		(char *)0,  0, 		0,
		ERx("ERget"), 	T_CHAR,		0,
		ERx("ERx"), 	T_CHAR,		0,
		(char *)0,  0, 		0
       };


/*{
**  Name: c_prtval - Print the sym value field. Called indirectly from symtab.
**
**  Inputs:
**	symval - Really a pointer to a C_VALS.
**
**  Outputs:
**	Returns:
**	    0 - Dummy return value.
*/

i4
c_prtval(symval)
i4	*symval;
{
    register	C_VALS	*cv = (C_VALS *)symval;

    trPrval(ERx("cv_addr:  0x%p "), cv);
    if (cv) {
	trPrval(ERx("cv_flags: %d "), cv->cv_flags);
	trPrval(ERx("cv_dims: '%s' "), cv->cv_dims);
    }
    return 0;
}

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
    C_VALS		*cv;
    i4			use_mask, ret_val;
    static   i4	einit = 0;
    i4			yyequlex();

    switch (flag)
    {
      case GR_EQINIT:
	eq->eq_lang = EQ_C;
	eq->eq_def_in = ERx("qc");
	eq->eq_def_out = ERx("c");
	if (!einit)
	{
	    eq->eq_in_ext = ERx("qc");
	    eq->eq_out_ext = ERx("c");
	    einit = 1;
	}

      /* tell the world we're Embedded QUEL! */
	dml->dm_lex = yyequlex;

	gr->gr_type = T_NONE;
	gr->gr_sym = (SYM *)0;
	gr->gr_id = ERx("");
	gr->gr_flag = GR_HOSTCODE;
	gr->C_dec = GR_C_DEC;
	gr->C_blk = 1;
	gr->C_rec = 0;
	gr->C_storage = T_NONE;
	gr->C_size = 0;
	gr->C_indir = 0;
	gr->C_struct = (SYM *)0;
	gr->C_isfunc = 0;
	gr->C_varchar = 0;
	gr->C_cvals = (C_VALS *)0;
	if (gr->C_strvals)
	    str_free(gr->C_strvals, (char *)0);
	else
	    gr->C_strvals = str_new(ID_MAXLEN);

	ecs_clear();
	break;

      case GR_SYMINIT:
	{
	    /* Enter declarations into Equel symbol table */
	    GR_TYPES	*gt;
	    i4		maxinit = 1;	/* 2 for CL types */
	    extern i4	(*sym_prtval)();

	    /* CL types are used too */
	    if (eq->eq_flags & EQ_COMPATLIB)
		maxinit = 2;
	    sym_init( (bool)FALSE );
	    sym_prtval = c_prtval;		/* Prt routine for C_VALS */
	    for (gt = gr_typetab; maxinit; maxinit--, gt++)
	    {
		for (; gt->gt_id; gt++)
		{
		    sy = symDcEuc(gt->gt_id, 0, gr->C_blk,
			(syFisTYPE|syFisSYS), C_CLOSURE, SY_NORMAL);
		    sym_s_btype( sy, gt->gt_rep );
		    sym_s_dsize( sy, gt->gt_len );
		    sym_s_indir( sy, 0 );
		}
	    }
	    /* Declare "generic null" as a symbol */
	    sy = symDcEuc( ERx(".null"), 0, gr->C_blk, syFisVAR|syFisSYS,
			    C_CLOSURE, SY_NORMAL );
	    sym_s_btype( sy, T_NUL );
	    sym_s_dsize( sy, sizeof(i4) );
	    gr->gr_null = sy;

	    if (eq->eq_flags & EQ_COMPATLIB)
	    {
		static C_VALS	cv_local;

		cv_local.cv_flags = CvalFUNCVAR;
		cv_local.cv_dims[0] = '\0';
		/* Continue to declare internal variables */
		for (; gt->gt_id; gt++)
		{
		    sy = symDcEuc(gt->gt_id, 0, gr->C_blk,
			(syFisVAR|syFisSYS), C_CLOSURE, SY_NORMAL);
		    sym_s_btype( sy, gt->gt_rep );
		    sym_s_dsize( sy, gt->gt_len );
		    sym_s_vlue( sy, &cv_local);
		}
	    }
	}
	break;

      case GR_LOOKUP:
	/* How should we look up this name? */
	if (gr->C_dec == GR_C_USE)		/* Variable usage */
	{
	    /* 
	    ** If we have a period between name references then it may be the
	    ** C period or the Ingres period.  To resolve this we use the global
	    ** gr->C_struct and see if the specified name is a field (child) of
	    ** structure or a regular variable (including a structure parent).
	    */
	    sy = sym_resolve( gr->C_struct, (char *)arg1, C_CLOSURE, 
			      syFisVAR|syFisCONST );

	    /* 
	    ** We cannot set C_struct here as Yacc may look ahead and cause
	    ** it to be reset.
	    */
	    if (sy)
	    {
		if (sym_g_btype(sy) == T_STRUCT)
		    *(i4 *)arg2 = tCSTRCT_VAR;
		else if ((cv = (C_VALS *)sym_g_vlue(sy)) != NULL &&
			  cv->cv_flags == CvalFUNCVAR)
		    *(i4 *)arg2 = tCFUNCVAR;
		else
		    *(i4 *)arg2 = tNAME;
	    }
	    else
		*(i4 *)arg2 = tNAME;
	} 
	else
	{
	    switch (gr->C_dec)
	    {
	      case GR_C_DEC: 
		use_mask = syFisTYPE;
		ret_val = tCTYPE;
		break;
	      case GR_C_TAG:
		use_mask = syFisTAG;
		ret_val = tCTAG;
		/*
		** (Bug 10459) Reset the flag here too (as well as in the rules)
		** to allow constructs such as:
		**	struct abc abc;
		** where "abc" is a known tag.
		** If the flag is not reset, then this is returned as:
		**	tCSTRUCT tCTAG tCTAG
		** because YACC needs to look ahead to resolve between using
		** Cstr_dectag and Cstr_withtag.
		*/
		gr->C_dec = GR_C_DEC;
		break;
	    }
	    if (sy = sym_resolve((SYM *)0, (char *)arg1, C_CLOSURE, use_mask))
	    {
		if (ret_val == tCTAG && sym_g_btype(sy) == T_INT)
		    ret_val = tCENUMTAG;
		*(i4 *)arg2 = ret_val;
	    }
	    else
		*(i4 *)arg2 = tCDECNAME;
	}
	break;

      case GR_STMTFREE:
	str_reset();
	sym_f_init();		/* clear the sym stack */
	gr->gr_type = T_NONE;
	gr->gr_sym = (SYM *)0;
	gr->C_struct = (SYM *)0;
	gr->C_storage = 0;
	break;

      case GR_CLOSE:
	if (gr->C_blk > 1)
	    er_write( E_E00005_hcUNCLOSED, EQ_ERROR, 1, er_na((gr->C_blk-1)) );
	break;

      case GR_DUMP:
	{
	    register FILE	*f = eq->eq_dumpfile;

	    SIfprintf( f, ERx("GR_DUMP: gr_ \n") );
	    SIfprintf( f, ERx("  sym = 0x%p, id = '%s', type = %d, flag = 0%o\n"),
		       gr->gr_sym, gr->gr_id, gr->gr_type, gr->gr_flag );
	    SIfprintf( f, ERx("  func = %d, mode = 0%o\n"),
		       gr->gr_func, gr->gr_mode, gr->C_dec );
	    SIfprintf( f, ERx("  C_blk = %d, C_rec = %d, C_storage = %d, "),
		       gr->C_blk, gr->C_rec, gr->C_storage );
	    SIfprintf( f, ERx("C_size = %d, C_indir = %d\n"), 
		       gr->C_size, gr->C_indir );
	    SIfprintf( f, ERx("  C_dec = %d, C_struct = 0x%p\n"),
		       gr->C_dec, gr->C_struct );
	    SIfprintf( f, ERx("  C_isfunc = %d, C_varchar = %d\n"), 
		       gr->C_isfunc, gr->C_varchar );
	    SIflush( f );
	}
	break;

      case GR_BLOCK:		/* Enter/exit a block - inform symbol table */
	if (arg1)	
	{
	    gen_host( G_H_OP|G_H_NEWLINE|G_H_INDENT, ERx("{") );
	    if (!gr->C_isfunc)	/* Otherwise already done */
	    {
		gr->C_blk++;
		sym_s_begin( C_CLOSURE ); /* C closure is always 0 */
	    }
	    gr->C_isfunc = 0;		/* No more functions till next */
	    gr->C_dec = GR_C_DEC;	/* On entry always expect declares */
	}
	else
	{
	    gen_host( G_H_OP|G_H_OUTDENT|G_H_NEWLINE, ERx("}") );
	    _VOID_ sym_s_end( gr->C_blk-- );
	    /* On Equel block exit reset decl flag - can't be immediate decls */
	    gr->C_dec = GR_C_USE;
	}
	break;
       
      /* Change length of int's (in arg1) to the given length (in arg2) */
      case GR_LENFIX:
	{
	    register GR_TYPES	*gt;

	    if (STcompare((char *)arg1, ERx("int")) != 0)
		return;
	    /* Do twice because of CL types in the middle */
	    for (gt = gr_typetab; gt->gt_id; gt++)
	    {
		if (STcompare(gt->gt_id, ERx("int")) == 0)
		    gt->gt_len = (i2)arg2;
	    }
	    for (gt++; gt->gt_id; gt++)
	    {
		if (STcompare(gt->gt_id, ERx("nat")) == 0)
		    gt->gt_len = (i2)arg2;
	    }
	}
	break;

      case GR_EQSTMT:
	{
	    i4		mode = (i4)arg1;
	    char	*kword = (char *)arg2;

	    /* Turn off declarations flag and generate line directive */
	    gr->gr_flag &= ~GR_HOSTCODE;
	    gr->C_dec = GR_C_USE;
	    if (gr->gr_flag & GR_DECLCSR)
		gen_line( kword );
	    else
		gen_eqstmt( G_OPEN, kword );

	    /* Check if ## declare [forms] is required for certain languages */

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

	      case GR_sLIBQ:
	      case GR_sFORMS:
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
	-1, 107,
	388, 1053,
	427, 1053,
	429, 1053,
	438, 1053,
	440, 1053,
	-2, 160,
	-1, 125,
	399, 307,
	-2, 292,
	-1, 202,
	388, 1053,
	427, 1053,
	429, 1053,
	438, 1053,
	440, 1053,
	-2, 174,
	-1, 213,
	374, 254,
	399, 254,
	-2, 1053,
	-1, 214,
	388, 257,
	402, 257,
	420, 257,
	427, 257,
	428, 257,
	429, 257,
	438, 257,
	440, 257,
	-2, 968,
	-1, 237,
	399, 921,
	-2, 1053,
	-1, 238,
	399, 921,
	-2, 1053,
	-1, 247,
	399, 921,
	-2, 1053,
	-1, 270,
	407, 1041,
	-2, 1053,
	-1, 272,
	407, 1041,
	-2, 1053,
	-1, 275,
	407, 1041,
	-2, 1053,
	-1, 377,
	388, 1053,
	427, 1053,
	429, 1053,
	438, 1053,
	440, 1053,
	-2, 281,
	-1, 395,
	388, 1053,
	427, 1053,
	429, 1053,
	438, 1053,
	440, 1053,
	-2, 332,
	-1, 414,
	388, 1053,
	427, 1053,
	429, 1053,
	438, 1053,
	440, 1053,
	-2, 555,
	-1, 528,
	388, 1053,
	427, 1053,
	429, 1053,
	438, 1053,
	440, 1053,
	-2, 588,
	-1, 570,
	409, 1000,
	-2, 1002,
	-1, 576,
	409, 1019,
	-2, 1018,
	-1, 606,
	400, 108,
	-2, 1053,
	-1, 623,
	433, 423,
	-2, 475,
	-1, 661,
	388, 1053,
	427, 1053,
	429, 1053,
	438, 1053,
	440, 1053,
	-2, 333,
	-1, 662,
	388, 1053,
	427, 1053,
	429, 1053,
	438, 1053,
	440, 1053,
	-2, 211,
	-1, 688,
	388, 1053,
	427, 1053,
	429, 1053,
	438, 1053,
	440, 1053,
	-2, 515,
	-1, 771,
	398, 625,
	403, 625,
	409, 625,
	-2, 1053,
	-1, 783,
	400, 877,
	-2, 874,
	-1, 813,
	399, 674,
	-2, 1053,
	-1, 814,
	399, 761,
	-2, 1053,
	-1, 815,
	388, 1053,
	427, 1053,
	429, 1053,
	438, 1053,
	440, 1053,
	-2, 919,
	-1, 816,
	388, 1053,
	427, 1053,
	429, 1053,
	438, 1053,
	440, 1053,
	-2, 919,
	-1, 817,
	399, 919,
	-2, 1053,
	-1, 818,
	388, 1053,
	427, 1053,
	429, 1053,
	438, 1053,
	440, 1053,
	-2, 588,
	-1, 819,
	388, 1053,
	427, 1053,
	429, 1053,
	438, 1053,
	440, 1053,
	-2, 919,
	-1, 821,
	399, 919,
	-2, 1053,
	-1, 823,
	388, 1053,
	427, 1053,
	429, 1053,
	438, 1053,
	440, 1053,
	-2, 919,
	-1, 857,
	398, 487,
	400, 487,
	-2, 475,
	-1, 880,
	398, 422,
	-2, 202,
	-1, 920,
	386, 486,
	388, 486,
	402, 486,
	427, 486,
	428, 486,
	429, 486,
	438, 486,
	440, 486,
	-2, 487,
	-1, 923,
	388, 1053,
	427, 1053,
	429, 1053,
	438, 1053,
	440, 1053,
	-2, 336,
	-1, 985,
	375, 615,
	383, 615,
	398, 625,
	403, 625,
	409, 625,
	-2, 1053,
	-1, 1090,
	365, 396,
	399, 475,
	401, 475,
	431, 396,
	432, 396,
	433, 396,
	434, 396,
	438, 396,
	439, 396,
	440, 396,
	-2, 487,
	-1, 1141,
	388, 1053,
	427, 1053,
	429, 1053,
	438, 1053,
	440, 1053,
	-2, 234,
	-1, 1167,
	388, 1053,
	427, 1053,
	429, 1053,
	438, 1053,
	440, 1053,
	-2, 337,
	-1, 1198,
	399, 475,
	401, 475,
	-2, 396,
	-1, 1283,
	283, 114,
	398, 114,
	400, 114,
	-2, 1053,
};

#define	YYNPROD		1100
#define	YYLAST		2930

short yyact[] = {
	   335,  1566,   608,   629,  1565,  1099,  1106,   616,  1500,  1179,
	   720,   970,   624,   890,   401,   683,   743,  1097,  1422,   682,
	   340,  1428,   469,  1453,  1190,  1369,   889,   369,   344,   866,
	  1250,  1088,  1015,   366,  1109,   827,  1403,  1056,  1569,  1000,
	   437,  1019,  1085,  1115,   966,   770,   978,   930,   909,   346,
	   867,   513,  1101,  1095,   336,  1407,   645,  1338,   922,   640,
	   389,   846,   858,  1075,   943,   271,   556,   270,   801,   275,
	   489,   752,     9,   768,   695,   328,  1060,   274,   784,   740,
	   899,   733,   755,  1012,   329,   730,   714,   710,   698,   269,
	   691,   885,   273,   679,   644,   620,  1305,   507,   451,   756,
	   326,   477,   810,     2,   724,   600,   324,   619,   272,   438,
	   637,   662,  1294,   351,   813,   323,   360,   277,   351,   715,
	   840,  1298,   807,   430,  1089,  1045,   353,   786,   343,   326,
	   692,   898,   402,   476,   454,   324,   895,   402,  1454,   368,
	  1289,   416,   611,   596,   416,   416,   349,   895,   612,   402,
	   895,  1300,  1301,  1302,  1104,   402,   365,   419,  1468,   602,
	   402,  1300,  1301,  1302,   455,  1518,   895,  1104,  1091,   552,
	   592,   472,   593,  1264,  1300,  1301,  1302,  1197,   453,   347,
	   761,  1104,   602,  1289,   603,   402,  1222,   347,  1300,  1301,
	  1302,   760,   592,   592,   593,   593,   872,  1104,  1304,   872,
	  1303,   944,   551,   488,  1300,  1301,  1302,   330,  1104,   409,
	  1058,   327,   576,   575,   968,  1300,  1301,  1302,   900,   900,
	   424,   426,   968,   520,   521,   886,   900,   330,   416,   829,
	   416,   416,   762,   331,   764,   402,   402,   460,   402,   402,
	   763,   519,   499,   500,   394,   758,   759,   407,   402,   491,
	   492,   804,   505,   331,   402,   402,   402,   402,   402,   402,
	   402,   402,   652,   845,   402,  1111,   441,   881,   806,   945,
	   511,   825,   330,   849,  1392,   508,   508,   367,   440,   327,
	  1116,   803,   670,   806,   630,   485,   901,   901,   754,  1117,
	  1118,  1044,   374,   887,   901,   330,   468,  1119,   331,   373,
	   577,   739,   402,   549,  1120,  1122,  1124,  1123,  1125,  1126,
	  1121,   898,   602,   582,   933,   392,   842,  1520,   330,   330,
	   420,   331,   367,  1127,  1104,  1344,   473,   686,  1091,   403,
	  1114,  1443,   579,   581,  1176,   580,   553,   687,   557,  1331,
	   559,   560,   330,   618,   331,   331,   569,   623,   327,   617,
	   473,   470,   570,   839,   511,   511,   511,   511,   511,   511,
	   511,   471,   690,   635,   330,  1017,   330,  1474,   331,   913,
	  1473,   685,   633,   330,   514,   639,   609,  1104,  1304,   872,
	  1303,   615,  1073,   330,  1300,  1301,  1302,   634,   647,   949,
	   331,   565,   331,   651,  1016,   474,   667,   403,   330,   331,
	   802,   934,   665,   718,  1186,   652,   680,   330,   664,   331,
	   330,   402,  1261,   403,  1404,   707,  1584,  1111,  1232,   895,
	   684,   367,  1394,  1531,   331,   588,   330,  1073,   681,  1063,
	   666,   327,  1116,   331,   670,  1503,   331,  1228,  1229,   656,
	  1227,  1117,  1118,   706,   374,   669,   668,   330,  1230,  1119,
	   670,   347,   331,   330,  1022,  1469,  1120,  1122,  1124,  1123,
	  1125,  1126,  1121,   330,   402,   402,  1389,  1021,   374,   802,
	   330,  1477,   330,   331,   717,  1127,  1104,  1111,   514,   331,
	   701,   402,  1114,   402,   426,  1380,  1376,  1204,   783,   331,
	   694,   402,  1014,   711,   426,   472,   331,   727,   331,   612,
	  1366,   895,   632,   402,   402,   829,   831,   330,   833,  1352,
	   473,   686,   606,   403,   607,   426,   832,   913,   732,   426,
	  1026,   687,   718,   670,   742,   426,   330,   403,  1008,   812,
	   835,   995,   961,   331,   725,   628,   330,   671,   650,   834,
	   330,   699,   402,   402,   402,   402,   402,   402,   402,   402,
	   830,   402,   331,   952,   673,   685,   800,  1042,   420,  1239,
	  1041,   655,   331,    95,   808,   362,   331,   402,  1021,   363,
	   330,   439,  1241,   330,   674,   553,  1550,   942,  1549,   529,
	    96,   364,   361,  1267,   317,   312,   310,   311,   315,   316,
	   313,   314,  1350,   511,   778,   793,   331,   319,   318,   331,
	   728,  1311,   330,  1072,  1243,  1240,   773,   857,   857,   766,
	   410,   403,  1242,  1237,  1238,  1236,  1221,   578,  1235,   790,
	   792,   699,   781,  1113,   330,  1467,   797,  1466,   331,  1233,
	   623,   330,   805,   351,   330,   403,   473,   470,   726,  1039,
	   855,   862,   408,   327,  1116,   612,   670,   471,   330,   877,
	   331,  1029,   330,  1117,  1118,   814,   876,   331,  1011,   920,
	   331,   851,   667,   327,   863,   324,   670,   367,   665,   623,
	   623,   365,   824,   969,   331,   857,  1224,   402,   331,   330,
	   903,   474,   327,  1076,   906,   325,   852,  1073,  1104,   402,
	  1077,   954,   918,   843,   670,   844,   666,   694,   330,   893,
	  1052,  1053,   927,   850,   347,   331,   330,   330,   928,   330,
	   946,  1054,   669,   668,   749,  1120,  1122,  1124,  1123,  1125,
	  1126,  1121,   402,  1382,   331,  1381,   334,   337,   717,   341,
	   342,   341,   331,   331,  1127,   331,   402,   879,   490,   357,
	   358,   694,   341,   871,   371,   677,   375,   377,   381,   883,
	   455,   393,   402,   395,   396,   765,   937,   869,   897,   972,
	   379,   915,   870,   853,   453,   953,   938,   403,   963,   965,
	  1018,   984,  1023,   921,   919,   910,   925,   926,   798,   884,
	   330,   709,  1171,   420,  1169,   604,   894,   987,   625,   400,
	   671,   694,   947,   701,   563,   502,   990,   967,  1013,   993,
	   585,   583,   584,  1014,   487,   586,   331,   327,   962,   330,
	   670,   905,  1005,   482,  1043,  1043,   472,   472,   472,   812,
	   472,  1010,   472,   330,   472,   725,   341,   341,   976,   991,
	   721,   420,  1034,   450,   721,   331,  1024,   522,   457,   998,
	   403,   997,   989,   701,   988,   403,   405,  1025,  1006,   331,
	  1028,   789,   445,   330,   811,   964,   779,   443,   330,   317,
	   312,   310,   311,   315,   316,   313,   314,   330,  1090,  1037,
	   618,   623,   319,   318,   403,  1090,   617,  1128,   418,   331,
	   956,  1219,  1027,  1079,   331,   774,   511,   330,  1009,   403,
	   403,  1147,   960,   331,   959,   996,   951,   330,   950,  1035,
	   992,  1086,   330,   330,   802,  1033,   564,  1071,  1138,  1070,
	  1080,  1069,  1074,   331,  1062,   736,  1083,  1137,  1145,  1068,
	   618,  1048,   941,   331,   940,  1146,   617,  1165,   331,   331,
	   721,  1064,  1149,  1139,  1067,  1157,   403,   680,   924,   936,
	  1082,   935,   670,  1154,   402,  1570,   452,  1371,  1372,   330,
	  1066,   684,   402,  1544,  1181,   402,   381,   857,  1078,   681,
	  1374,  1375,   381,  1160,  1162,  1135,  1129,  1198,  1185,  1184,
	   622,   669,   668,  1183,   429,   331,  1182,  1003,  1004,   626,
	  1166,  1018,  1018,  1018,  1168,  1188,  1023,  1164,   426,  1148,
	  1193,   402,  1163,   638,   402,  1143,  1172,   646,  1132,  1081,
	  1424,   648,   649,   403,  1084,  1140,   402,   415,   337,   769,
	  1192,  1231,  1244,  1207,  1208,  1209,   330,   426,   717,  1528,
	   330,   771,   870,  1177,   426,   537,  1211,  1212,   402,   354,
	  1173,  1508,  1174,   538,  1495,   711,   426,  1133,   536,   535,
	  1178,  1494,   331,   425,   719,  1087,   331,  1419,  1194,  1134,
	  1161,  1195,  1087,  1571,   352,  1260,   330,  1247,   734,   958,
	  1142,  1203,  1248,  1218,   732,   426,   892,   330,   870,   870,
	   748,   742,   426,  1245,  1215,  1213,  1217,   459,  1521,  1052,
	  1156,   765,   331,   796,   370,   625,   630,  1246,  1198,  1198,
	   484,  1196,  1225,   331,  1278,  1205,   480,   463,  1198,  1198,
	   462,  1258,  1259,  1206,   642,   464,  1256,  1296,  1046,  1047,
	   466,  1049,  1286,  1050,  1461,  1051,   612,  1282,   383,   428,
	   413,  1433,   659,   427,  1262,  1071,  1285,  1070,  1268,  1069,
	  1290,  1292,  1062,  1316,  1131,  1273,  1266,  1068,  1274,   761,
	  1275,  1287,   351,  1272,  1279,  1325,   511,   374,   347,  1064,
	   760,   671,  1067,  1321,  1308,  1269,  1327,   385,  1271,   327,
	  1116,  1255,   670,  1326,  1198,  1313,   630,  1314,  1066,  1117,
	  1118,   402,  1254,   402,  1270,  1323,   317,   312,   310,   311,
	   315,   316,   313,   314,   898,   333,   322,  1312,   426,   319,
	  1319,   762,   625,   764,   356,   370,   875,  1032,  1198,   763,
	  1329,   891,   891,  1277,   758,   759,  1018,  1018,  1333,   777,
	  1342,   339,  1291,  1454,  1296,  1335,   496,  1341,  1348,   478,
	  1202,   435,  1306,  1307,   704,  1378,  1252,  1253,   497,   498,
	   859,   864,   865,   380,   671,   868,  1357,   347,  1363,  1364,
	   386,  1547,  1360,   746,   387,  1355,  1354,   788,   873,  1365,
	   984,  1362,   341,   880,   524,   882,   354,   957,  1388,  1367,
	   385,  1345,  1548,   888,  1361,  1087,  1087,  1545,  1481,  1340,
	   523,   902,  1358,  1404,  1464,   904,  1444,  1397,  1351,  1199,
	   555,  1309,   703,  1310,   809,   510,   923,   515,  1343,  1198,
	   404,   516,   622,   622,  1198,  1334,  1198,  1198,   719,  1391,
	  1390,  1386,   534,   517,   518,   980,   983,   601,   449,  1296,
	  1198,  1396,   599,   981,   982,   848,  1383,   985,  1402,   598,
	  1393,   847,  1359,   597,  1296,  1296,  1410,   595,  1379,  1332,
	  1399,  1405,  1356,   618,   663,   618,   531,   594,   591,   617,
	  1445,   617,  1446,   187,   971,   828,  1406,   618,   307,   826,
	   562,   402,  1430,   617,  1455,   276,  1450,   857,  1198,   590,
	  1436,  1296,  1447,  1413,  1435,  1018,  1439,  1451,  1036,   509,
	   669,   668,  1065,  1420,  1061,  1265,  1059,  1296,   841,  1426,
	  1442,  1462,  1437,   574,  1475,   573,  1416,   426,   572,   571,
	  1193,   306,  1448,  1263,  1057,  1055,   838,  1465,   568,   837,
	   567,   618,   305,   304,   265,   189,   188,   617,  1480,  1417,
	  1192,   186,   504,  1471,   185,  1198,    94,  1457,  1408,   587,
	  1411,  1412,   321,    11,  1478,   693,  1296,   689,  1180,  1296,
	  1296,   697,  1489,  1296,  1415,   696,  1479,   263,   501,  1484,
	   183,   301,   262,   182,   181,  1491,  1487,  1031,   795,  1030,
	  1414,   447,   448,   794,   495,   791,   494,   260,  1181,  1400,
	  1421,   493,   180,   259,   179,   787,  1087,  1504,   258,  1515,
	   178,  1493,  1185,  1184,   257,  1470,  1497,  1183,   467,  1449,
	  1182,  1296,  1463,  1512,  1514,  1198,  1516,  1506,  1513,   177,
	   184,  1511,   618,  1509,   622,  1546,  1519,  1425,   617,  1538,
	  1296,   653,  1496,  1537,  1541,  1532,  1385,  1517,   638,  1144,
	  1534,  1476,  1384,  1485,  1542,  1257,   782,  1539,   646,   780,
	   256,   486,   176,  1153,   255,   618,   175,  1198,  1552,   506,
	   512,   617,  1554,   776,   254,   775,  1523,   483,  1525,  1482,
	  1553,  1560,   414,  1533,   174,   253,   481,  1167,  1296,  1561,
	   173,   431,   540,   558,  1567,   554,   561,  1556,  1198,  1573,
	   479,   618,  1558,  1578,  1063,   172,   605,   617,  1579,  1582,
	  1488,   767,   610,  1296,   475,   533,   171,   170,  1226,   753,
	  1191,   249,   169,  1586,   539,   465,  1587,   618,   719,  1111,
	   618,  1589,  1567,   617,  1588,  1296,   617,  1590,   461,   168,
	   247,   166,  1377,   327,  1116,   458,   670,  1572,   165,  1530,
	  1574,  1575,  1576,  1117,  1118,   747,   374,   895,   456,   330,
	  1580,  1119,   164,  1524,  1510,  1526,  1370,  1368,  1120,  1122,
	  1124,  1123,  1125,  1126,  1121,  1111,  1373,  1002,  1220,  1001,
	   999,   745,  1540,   744,   163,   331,  1234,  1127,  1104,   327,
	  1116,  1555,   670,   757,  1114,   741,   738,  1522,  1040,  1117,
	  1118,   243,   374,   737,   162,   330,   161,  1119,   160,   735,
	   159,   444,  1557,   158,  1120,  1122,  1124,  1123,  1125,  1126,
	  1121,   731,  1577,   399,  1543,   729,   421,   422,   406,   238,
	   442,   331,   157,  1127,  1104,   237,   156,   154,    95,   235,
	  1114,   436,   153,  1201,  1200,   859,  1284,  1581,   864,   975,
	  1472,   446,  1251,  1249,  1020,    96,  1210,   979,   977,   317,
	   312,   310,   311,   315,   316,   313,   314,   723,  1387,   986,
	   434,   433,   319,   318,  1038,   234,   503,   974,  1072,   722,
	   432,   330,   149,   148,   423,   145,   144,   143,   230,   142,
	  1120,  1122,  1124,  1123,  1125,  1126,  1121,   708,  1318,   337,
	   229,   417,   141,   705,   140,  1189,   341,   331,  1564,  1127,
	   525,  1507,   526,   527,  1460,  1505,  1459,  1458,  1353,   955,
	  1337,   702,   412,   227,   411,   139,   528,   530,   138,   532,
	   532,  1347,   137,   688,   136,   678,   135,   931,  1170,   532,
	   670,   929,   676,   398,   675,   541,   542,   543,   544,   545,
	   546,   547,   548,   330,   397,   550,   131,  1110,  1108,  1107,
	  1423,  1317,  1120,  1122,  1124,  1123,  1125,  1126,  1121,  1418,
	  1315,  1130,   621,   345,   716,   713,   712,   948,  1112,   331,
	  1102,  1127,  1100,  1094,  1529,  1527,  1483,  1103,  1105,  1098,
	  1295,  1096,  1093,   566,  1092,  1299,  1297,  1409,  1293,  1288,
	   672,   130,   661,   660,   129,   128,  1346,   917,   658,   127,
	  1159,   912,   911,   908,   907,  1158,   654,   388,   126,  1501,
	  1499,  1452,  1339,   384,   382,   125,   124,   378,   376,   123,
	  1336,  1155,   122,   641,   896,   643,   282,   213,   372,   121,
	   120,   636,   119,   118,  1432,  1536,  1490,  1401,  1431,  1324,
	  1322,  1141,   631,   359,   117,  1427,  1320,   878,   116,   627,
	   115,  1136,   874,   355,   114,   113,   350,   348,   112,   203,
	   202,   111,   109,   108,   614,   107,   613,   106,   105,   338,
	   104,  1441,  1440,  1330,  1152,  1429,  1151,  1328,  1150,  1434,
	   861,  1438,   860,   103,  1551,  1283,  1281,  1398,  1280,   856,
	  1395,  1276,   700,   854,   102,   332,   100,    99,    93,    92,
	    91,    90,    89,    88,    87,    86,    85,    84,    83,    82,
	  1191,    81,    80,    79,    78,    77,    76,    75,    74,    73,
	    72,    71,    70,    69,    68,    67,    66,    65,    64,    63,
	    62,    61,    60,    59,    58,    57,    56,    55,    54,    53,
	    52,    51,    50,    49,    48,   750,   751,    47,    46,    45,
	    44,    43,    42,    41,    40,    39,    38,    37,    36,    35,
	    34,    33,   772,    32,    31,    30,    29,    28,    27,   785,
	    26,    25,   700,    24,    23,  1486,    22,    21,    20,   341,
	    19,    18,    17,    16,    15,   799,  1492,    14,    13,    12,
	   391,   914,   657,   390,     8,     6,     5,     4,     3,     1,
	     0,     0,     0,  1498,     0,     0,  1502,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   815,   816,   817,   818,   819,   820,   821,
	   822,     0,   823,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  1429,  1535,     0,     0,     0,     0,   836,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  1559,     0,     0,  1502,  1562,  1563,  1568,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  1583,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  1585,     0,     0,  1568,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,   932,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	   939,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   973,     0,     0,     0,    10,    98,   190,
	   192,   193,   320,   194,   195,   278,   205,   994,   208,     0,
	   210,   214,   212,   211,   215,   216,   217,   218,   219,   220,
	     0,     0,     0,  1007,     0,   133,   132,   134,   204,   225,
	   224,     0,   191,   226,   222,   223,   101,   110,   206,   207,
	   209,   221,   197,   198,   199,   196,   200,   201,   279,   280,
	   281,   232,   283,   231,   228,   285,   284,   155,   236,   233,
	   287,   286,     0,     0,     0,   291,   288,   289,   167,   244,
	   250,   147,   240,   246,   251,   252,   248,   152,   151,   146,
	   150,   292,   290,     0,   245,   239,     0,   242,   241,   293,
	   294,   297,   295,   299,   298,   296,   302,   309,   300,   261,
	   264,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,    95,     7,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,    96,     0,
	    97,   916,   317,   312,   310,   311,   315,   316,   313,   314,
	     0,   308,   303,   267,   266,   319,   318,     0,     0,     0,
	     0,   268,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  1175,     0,     0,     0,     0,
	     0,     0,     0,   700,     0,     0,  1187,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  1214,     0,     0,  1216,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  1223,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,   785,
	    10,    98,   190,   192,   193,   320,   194,   195,   278,   205,
	     0,   208,     0,   210,   214,   212,   211,   215,   216,   217,
	   218,   219,   220,     0,     0,     0,     0,     0,   133,   132,
	   134,   204,   225,   224,     0,   191,   226,   222,   223,   101,
	   110,   206,   207,   209,   221,   197,   198,   199,   196,   200,
	   201,   279,   280,   281,   232,   283,   231,   228,   285,   284,
	   155,   236,   233,   287,   286,     0,     0,     0,   291,   288,
	   289,   167,   244,   250,   147,   240,   246,   251,   252,   248,
	   152,   151,   146,   150,   292,   290,     0,   245,   239,     0,
	   242,   241,   293,   294,   297,   295,   299,   298,   296,   302,
	   309,   300,   261,   264,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,    95,     7,     0,     0,     0,     0,
	     0,     0,   932,     0,  1349,     0,     0,     0,     0,     0,
	     0,    96,     0,    97,   589,   317,   312,   310,   311,   315,
	   316,   313,   314,     0,   308,   303,   267,   266,   319,   318,
	     0,     0,     0,     0,   268,    10,    98,   190,   192,   193,
	   320,   194,   195,   278,   205,     0,   208,     0,   210,   214,
	   212,   211,   215,   216,   217,   218,   219,   220,     0,     0,
	     0,     0,     0,   133,   132,   134,   204,   225,   224,     0,
	   191,   226,   222,   223,   101,   110,   206,   207,   209,   221,
	   197,   198,   199,   196,   200,   201,   279,   280,   281,   232,
	   283,   231,   228,   285,   284,   155,   236,   233,   287,   286,
	     0,     0,     0,   291,   288,   289,   167,   244,   250,   147,
	   240,   246,   251,   252,   248,   152,   151,   146,   150,   292,
	   290,     0,   245,   239,     0,   242,   241,   293,   294,   297,
	   295,   299,   298,   296,   302,   309,   300,   261,   264,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,    95,
	     7,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  1456,     0,     0,     0,    96,     0,    97,     0,
	   317,   312,   310,   311,   315,   316,   313,   314,     0,   308,
	   303,   267,   266,   319,   318,     0,     0,     0,     0,   268
};

short yypact[] = {
	 -1000,  2499, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   818,   296,
	   817, -1000,  -195,  -195,   849,  -195,  -195,  -195,   954, -1000,
	 -1000, -1000,   665,   975,   827,  -195,  -195,   307,  -195,   829,
	  -195,    69,  -195,   495,  -195,   876,   -94,  -195,   296,  -195,
	  -195,   500, -1000, -1000, -1000,   447,   222,   243,   211,   747,
	   618,   479,    69,   618,   618,   654, -1000,   744,   614,   900,
	 -1000, -1000, -1000,   172,   222, -1000,   243,   458,   453,   222,
	   447,   447,   434,   547,   439,   689,   211, -1000,   722,   434,
	   -36,   896,   721,   414,   708,   243,   405,   339,   211,   211,
	   848,   -94,   -94,   396,   222,   448,   448,   -33, -1000, -1000,
	 -1000,  1029, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,   -83,  -195, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,  -195,   989, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   618, -1000,   618,
	   618, -1000, -1000, -1000,   246,   222, -1000,   222,   222, -1000,
	 -1000, -1000, -1000,   650, -1000, -1000, -1000,   222, -1000,   650,
	 -1000, -1000, -1000,   222,   222,   222,   222,   222,   222,   222,
	   222, -1000,   172,   222, -1000,  -228, -1000,  -261, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,   395, -1000, -1000, -1000,
	 -1000, -1000,   -29, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,   222, -1000, -1000, -1000,   -74,  -207,  -120,   218, -1000,
	   -80,   -99, -1000, -1000,   388, -1000, -1000, -1000, -1000, -1000,
	 -1000,  2324, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  -245,
	 -1000,   386,  -195, -1000,    69, -1000,    69,   -27,  -195, -1000,
	   247, -1000,   -38,   247, -1000, -1000,  -195, -1000, -1000, -1000,
	 -1000, -1000, -1000,   825, -1000,  -195, -1000,    69,   798,   718,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   247,  -195,
	 -1000,   798,   730,  -195, -1000,   825, -1000,  -195,  -195, -1000,
	    69,     4,   979,    69,  -195, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,   752, -1000,   421,    69,   346, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,   -60, -1000, -1000,   407, -1000,
	   501, -1000,  1007,   933,    70, -1000, -1000, -1000,   407, -1000,
	   232, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	   532, -1000,   896, -1000,   339, -1000,   -94, -1000, -1000,   200,
	   532, -1000, -1000,  -195, -1000,   670,   517, -1000, -1000,  -130,
	 -1000,   960, -1000, -1000, -1000, -1000, -1000,   682,   315, -1000,
	 -1000, -1000, -1000,   222,   222, -1000, -1000,  -143, -1000, -1000,
	 -1000, -1000, -1000, -1000,   695,   896, -1000,   633, -1000, -1000,
	   222, -1000,   485,   846,   243, -1000, -1000,   456,   964, -1000,
	   451,   964, -1000, -1000,   250,   701, -1000, -1000, -1000, -1000,
	 -1000, -1000,   378,   222,   -33,   506, -1000, -1000, -1000,  -152,
	  -137,  -246,   -33, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	   247,   247, -1000,  1019, -1000, -1000, -1000, -1000,   465, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,   222,   222,   222,   222,   222,   222,   222,   222, -1000,
	   222, -1000, -1000,   506,  -149, -1000,   506,   506, -1000,   506,
	   506, -1000,   119, -1000, -1000, -1000,   222,   -56, -1000, -1000,
	 -1000,   -93,   -93, -1000,   -93, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,  -150, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,  -115, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,    69,  -195,  -195,  -195, -1000,
	   782,  -195, -1000, -1000,   247, -1000, -1000, -1000, -1000,   247,
	 -1000,  -237, -1000, -1000,  -195, -1000,   831,   954,  -195,    -4,
	 -1000,  -195,   307, -1000, -1000, -1000,   247, -1000,  -140,  -195,
	   840,   678, -1000,   101, -1000,   819,  -146,  -195, -1000, -1000,
	   782,  -195, -1000,    69,   782,   113, -1000,  2021,   296, -1000,
	 -1000,   277,   168, -1000, -1000, -1000, -1000, -1000,  -195,  -195,
	 -1000, -1000,   782, -1000,   232, -1000,    24, -1000,   541, -1000,
	 -1000, -1000, -1000, -1000, -1000,   369, -1000, -1000,   222,   524,
	   177, -1000, -1000,  -164,   311,   -14,   498,   153, -1000, -1000,
	  -164,   292,    69,   976, -1000, -1000,   671, -1000,   494,   132,
	 -1000,   -14,   101,    19, -1000, -1000,  -147, -1000,   274,     4,
	   -83,   222, -1000,   896, -1000,   929,   -94, -1000, -1000,   444,
	 -1000,  -164,   -14,   502,  -164,   222, -1000,   131,   497, -1000,
	 -1000,   816,   -14, -1000, -1000,   589, -1000,  -164, -1000,   222,
	 -1000,   222,   128,   490, -1000, -1000,  -164,   259, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   400, -1000,     5,
	 -1000,    68, -1000,   436, -1000, -1000,   211, -1000, -1000, -1000,
	   120, -1000,   484, -1000, -1000,  -164, -1000,   252, -1000, -1000,
	 -1000, -1000, -1000, -1000,   834,   243, -1000,   432, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	   240, -1000, -1000,   171,   171,   -36,   -36,   -36,   465,   -36,
	 -1000,   -36, -1000,   -36, -1000,   218, -1000, -1000, -1000, -1000,
	   314, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  -210, -1000,
	 -1000,  1308, -1000, -1000, -1000, -1000,   -23, -1000, -1000, -1000,
	 -1000,   282,  -268,   782,    19, -1000,   247, -1000, -1000,  -146,
	    19,   247, -1000, -1000,  -146, -1000, -1000,  -107, -1000,  -175,
	  -195,   748, -1000,  -140,  -107, -1000, -1000,   101,   975,   247,
	 -1000, -1000,    69, -1000,  -195,  -195, -1000, -1000, -1000,   954,
	   305, -1000, -1000, -1000,  -195, -1000, -1000,   553, -1000,   304,
	 -1000, -1000,    69, -1000, -1000,   782, -1000,   101,    19, -1000,
	 -1000,  -146,  -195, -1000, -1000, -1000, -1000, -1000, -1000,  -175,
	 -1000,   865, -1000,  -195, -1000,   247,   247, -1000,    19,   384,
	 -1000,  -164, -1000, -1000, -1000, -1000,   -60, -1000, -1000, -1000,
	 -1000,  -195, -1000,     8, -1000, -1000, -1000, -1000,  -195, -1000,
	 -1000,   138, -1000,   124,   222,   782,  -195, -1000, -1000, -1000,
	  -195, -1000, -1000, -1000,  -195, -1000,  1263,   999, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,   898, -1000,    89, -1000,   720,
	     5,     5,     5,   638, -1000,    68, -1000, -1000, -1000,  -195,
	   222, -1000,   670,   222, -1000, -1000,  -195, -1000,  -164,   483,
	 -1000,   217,  -247, -1000, -1000,   222,   278, -1000, -1000,   816,
	    51,   229,   -94,   633, -1000,   169, -1000, -1000, -1000, -1000,
	   169,   838, -1000, -1000, -1000, -1000, -1000,   222, -1000,  -143,
	 -1000,   211, -1000, -1000, -1000, -1000, -1000,   -23,   960, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,     2, -1000,  -260, -1000,   173,
	 -1000, -1000, -1000, -1000, -1000,   765, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,   282, -1000, -1000, -1000, -1000, -1000,
	   839,  -195,  1411,   954,  -195,  -296, -1000,  -107,  -107,   -54,
	   -14, -1000, -1000, -1000, -1000, -1000, -1000,  1263,  1217, -1000,
	 -1000, -1000, -1000,    69, -1000,    69, -1000, -1000, -1000, -1000,
	 -1000,   202,   105,     4, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,   305, -1000,  -195,  -195, -1000,  -296, -1000, -1000,
	   825,   -84,  -195, -1000,     4, -1000, -1000, -1000, -1000, -1000,
	   -96,    69,   101, -1000, -1000,   730,  -195,   990, -1000,   990,
	 -1000,   -35, -1000,    45,   -14, -1000, -1000,  -195, -1000, -1000,
	    24, -1000,   222, -1000, -1000, -1000,   193, -1000, -1000, -1000,
	 -1000,   -14, -1000, -1000, -1000, -1000, -1000,   109,   990,   101,
	 -1000,  -139, -1000, -1000, -1000, -1000,  -277,  1263,   -14, -1000,
	 -1000,   172, -1000,   -94,   929,     5,     5,   169, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,   100, -1000, -1000, -1000,   589,
	 -1000,   559,   572,    86,  -195, -1000, -1000, -1000, -1000, -1000,
	 -1000,   -14, -1000, -1000,    85, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   325,
	 -1000,  -164, -1000, -1000, -1000, -1000, -1000,  1339,    66, -1000,
	 -1000, -1000,  -210,  -157, -1000, -1000, -1000, -1000,    71, -1000,
	 -1000, -1000, -1000, -1000, -1000,    22,   305, -1000, -1000, -1000,
	   954,    69, -1000,   134, -1000, -1000, -1000, -1000,  -107, -1000,
	  -253,  -234, -1000,  1263,  -267,  1263,  1263, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,  -250, -1000,  1263,
	 -1000,  -195,   -14, -1000, -1000,   659, -1000,   101,   612,    69,
	   865,  -195,   782, -1000,   750,    61,   101,  -195, -1000,   993,
	  -104,   996,  -175, -1000,  -175,   840,   101,   612,   954,   852,
	 -1000, -1000, -1000,  -277, -1000, -1000,  -175, -1000, -1000, -1000,
	   222, -1000,  -164,   741, -1000, -1000,  -195,  1263,   994,  -264,
	 -1000, -1000, -1000, -1000,     5, -1000, -1000, -1000,   227, -1000,
	  -275, -1000, -1000, -1000, -1000, -1000, -1000,    55,   -14, -1000,
	 -1000, -1000,   838,   -19,    72, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,   693, -1000, -1000,   954, -1000, -1000, -1000,   993,
	  -175, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  -277,   988,
	 -1000, -1000,  -287, -1000,  1263,  -223,   -14, -1000, -1000, -1000,
	 -1000,  -195, -1000, -1000, -1000,  -195, -1000,   247, -1000,   -27,
	 -1000,   840,   -83, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,   653,   646, -1000, -1000,    19,   954, -1000,  -195,
	 -1000, -1000,  -195, -1000, -1000, -1000,    35,   -60, -1000,   829,
	   643, -1000, -1000,  -277, -1000, -1000, -1000,   559,   -60,   960,
	 -1000, -1000, -1000, -1000, -1000, -1000,  -118,   692, -1000,    69,
	   101, -1000,  -250,   631,  1263,    23,   612,   101,  -195,  -195,
	   825,  -175, -1000, -1000, -1000, -1000, -1000, -1000,   612,   247,
	 -1000,   -27,     4, -1000, -1000, -1000,    69, -1000,   565, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   987,   958,
	   982,   178,   257, -1000,  -175, -1000,  1263,   782, -1000,   247,
	  -277, -1000, -1000,   954, -1000, -1000, -1000,  -195,   -27, -1000,
	  -195,  -195,  -195,   -83, -1000, -1000, -1000,   557, -1000, -1000,
	   667,    19, -1000,   773,    19,  -264,    19,  1263, -1000,   798,
	  -175, -1000, -1000, -1000,   101, -1000, -1000,   -83,     4, -1000,
	 -1000,    16, -1000, -1000, -1000, -1000, -1000,  -277,  -195,   798,
	 -1000,   -83, -1000, -1000, -1000, -1000,  -175, -1000,   -27,  -175,
	 -1000
};

short yypgo[] = {
	     0,  2069,   103,  2068,  2067,  2066,  2065,  2064,    72,    60,
	  2063,  2062,  2061,  2060,  2059,  2058,  2057,  2054,  2053,  2052,
	  2051,  2050,  2048,  2047,  2046,  2044,  2043,  2041,  2040,  2038,
	  2037,  2036,  2035,  2034,  2033,  2031,  2030,  2029,  2028,  2027,
	  2026,  2025,  2024,  2023,  2022,  2021,  2020,  2019,  2018,  2017,
	  2014,  2013,  2012,  2011,  2010,  2009,  2008,  2007,  2006,  2005,
	  2004,  2003,  2002,  2001,  2000,  1999,  1998,  1997,  1996,  1995,
	  1994,  1993,  1992,  1991,  1990,  1989,  1988,  1987,  1986,  1985,
	  1984,  1983,  1982,  1981,  1979,  1978,  1977,  1976,  1975,  1974,
	  1973,  1972,  1971,  1970,  1969,  1968,  1967,   115,  1966,  1965,
	  1233,   157,    29,  1964,   623,   177,  1963,    55,  1961,  1960,
	    28,    33,  1959,    62,   142,    80,  1958,  1957,  1956,    34,
	  1955,    36,    49,  1954,     5,    17,     7,    13,     0,  1953,
	    54,  1952,     2,  1950,    56,  1948,  1947,  1946,  1944,  1943,
	  1942,  1941,  1940,  1939,  1938,    20,  1937,  1936,  1935,  1934,
	    38,  1933,  1932,  1931,  1930,  1929,  1928,  1927,  1926,  1925,
	   126,    12,    91,  1924,  1923,  1922,  1921,    42,  1920,  1919,
	  1918,     3,  1917,  1916,    58,   107,  1915,    21,   938,    50,
	  1914,  1913,  1912,  1911,  1910,   116,    27,  1909,  1908,  1906,
	  1905,  1904,  1903,  1902,  1901,   110,    11,  1900,  1899,  1898,
	    59,    26,  1897,  1896,  1895,    94,  1894,   112,  1893,  1892,
	  1891,  1890,    18,  1889,  1888,  1887,  1886,  1885,  1884,  1118,
	    57,  1883,  1882,  1881,  1880,    23,     8,  1879,  1878,  1877,
	  1876,  1875,  1874,  1873,    48,  1872,   124,  1871,    74,    96,
	  1870,  1869,  1868,  1867,  1866,  1865,  1864,  1863,  1862,   111,
	     6,  1861,  1860,    31,  1859,  1858,  1857,  1856,   121,  1855,
	  1854,  1852,  1851,  1850,  1849,  1848,  1847,  1846,  1845,  1844,
	  1843,    53,  1842,    52,  1840,    10,  1838,  1837,  1836,  1835,
	    86,  1834,    44,  1833,    95,  1832,  1831,  1830,  1829,  1821,
	  1820,  1819,  1818,  1817,  1816,  1814,  1804,  1803,  1551,  1802,
	  1801,    47,  1798,  1797,    64,  1796,  1290,  1795,    93,    22,
	    19,    15,    14,  1794,  1793,  1792,   109,  1788,    70,  1785,
	  1784,  1783,  1542,  1782,  1781,  1779,  1778,  1777,  1776,  1775,
	  1774,  1771,  1768,     4,     1,  1765,    24,  1764,  1763,  1762,
	  1761,  1760,  1757,   362,    87,  1749,  1748,  1747,  1746,  1745,
	  1744,  1743,   123,  1742,  1740,  1739,  1737,  1735,   102,  1734,
	    16,  1731,  1730,   203,  1729,  1727,   104,   101,  1718,    83,
	    46,  1717,    32,    41,  1716,    45,  1714,  1713,    30,  1712,
	  1710,  1709,  1704,  1703,    40,  1702,  1701,  1699,  1697,  1696,
	  1695,  1336,  1692,  1690,  1689,  1685,    85,  1681,  1673,  1671,
	    81,  1670,  1669,  1668,  1666,  1664,  1308,  1663,  1661,  1302,
	   114,  1658,  1656,    79,  1655,    99,  1653,  1646,  1644,    98,
	  1643,  1641,  1640,    39,  1639,  1638,  1637,  1636,  1627,    25,
	  1626,  1624,  1622,  1618,  1615,  1608,  1605,  1602,  1601,  1600,
	  1599,  1598,  1585,  1582,    71,  1581,  1579,    82,  1578,  1577,
	   125,  1576,  1574,   133,  1571,    73,  1565,  1560,  1550,  1546,
	  1545,   291,  1544,  1537,  1535,  1534,  1533,  1526,  1524,  1522,
	  1521,  1520,  1519,   134,  1516,    78,  1515,  1512,  1511,  1507,
	  1506,  1496,  1495,  1490,  1489,   127,  1474,  1470,  1468,  1465,
	  1464,  1463,  1462,  1461,  1457,  1456,  1455,  1454,  1453,  1449,
	  1448,  1447,  1444,  1443,  1442,  1441,  1440,  1438,  1437,  1435,
	  1431,    88,     9,  1428,  1427,    90,  1425,   119,   130,    43,
	  1423,  1422,  1419,  1416,  1414,  1412,    51,  1411,  1343,  1406,
	  1405,  1404,    66,    89,    67,  1280,    65,   108,    92,    77,
	    69,  1403,  1402,  1400,  1399,  1398,  1396,  1395,    37,    68,
	  1394,  1393,    35,  1391,  1389,   120,  1388,  1385,  1383,  1378,
	  1376,  1375,    76,  1374,  1372,    97,  1369,   122,  1285,  1368,
	    84,  1359,    61,  1355,  1350,  1349,  1348,  1345,   117,    75,
	  1338,  1337,  1327,  1323,  1321,  1319,  1315,  1312,    63,   105,
	  1307
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
	   391,   391,   304,   304,   402,   402,    13,    12,   279,   233,
	   517,   343,   510,   518,    74,   118,   137,   265,   477,   519,
	   519,   519,   519,   519,   519,   519,   519,   202,     2,   520,
	   521,   521,   522,     8,     8,     7,   523,   523,   523,   523,
	   523,   524,   524,   524,   524,   524,   524,   531,   527,   525,
	   525,   528,   528,   528,   528,   528,   528,   528,   533,   541,
	   541,   541,   541,   541,   541,   541,   541,   541,   541,   541,
	   541,   541,   541,   541,   541,   541,   534,   536,   542,   543,
	   543,   543,   545,   544,   546,   547,   547,   548,   548,   550,
	   551,   537,   538,   539,   540,   553,   554,   556,   557,   558,
	   555,   559,   561,   560,   560,   562,   562,   562,   563,   563,
	   563,   563,   563,   563,   563,   563,   563,   563,   563,   563,
	   564,   535,   535,   549,   532,   532,   565,   565,   565,   565,
	   566,   567,   568,   570,   570,   571,   571,   569,   572,   572,
	   529,   573,   576,   574,   574,   575,   575,   552,   552,   577,
	   577,   577,   577,   577,   577,   577,   530,   530,   578,   238,
	   128,   579,   579,   579,   580,   580,   581,   582,   584,   583,
	   583,   585,   587,   587,   590,   586,   589,   588,   588,   526
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
	     1,     0,     1,     1,     1,     0,     1,     1,     1,     1,
	     2,     1,     1,     2,     2,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     2,     3,     1,
	     2,     0,     1,     1,     1,     1,     3,     3,     2,     1,
	     1,     1,     2,     1,     1,     2,     1,     1,     1,     1,
	     1,     2,     2,     2,     2,     2,     2,     2,     1,     1,
	     1,     1,     1,     1,     2,     2,     2,     2,     2,     2,
	     1,     1,     2,     2,     3,     1,     3,     2,     1,     1,
	     1,     0,     1,     3,     1,     1,     3,     3,     1,     1,
	     1,     3,     3,     2,     3,     1,     1,     0,     1,     1,
	     3,     1,     1,     1,     2,     3,     1,     1,     1,     2,
	     1,     2,     1,     2,     1,     2,     1,     2,     1,     1,
	     1,     0,     1,     1,     1,     3,     2,     2,     2,     1,
	     3,     1,     1,     0,     2,     1,     1,     1,     0,     2,
	     3,     2,     1,     1,     0,     1,     1,     1,     2,     1,
	     2,     1,     2,     1,     1,     1,     3,     1,     2,     1,
	     1,     3,     1,     2,     1,     1,     1,     2,     1,     1,
	     2,     3,     1,     5,     1,     3,     1,     1,     1,     1
};

short yychk[] = {
	 -4000,    -1,    -2,    -3,    -4,    -5,    -6,   391,    -7,    -8,
	   256,  -520,   -14,   -15,   -16,   -17,   -18,   -19,   -20,   -21,
	   -22,   -23,   -24,   -25,   -26,   -27,   -28,   -29,   -30,   -31,
	   -32,   -33,   -34,   -35,   -36,   -37,   -38,   -39,   -40,   -41,
	   -42,   -43,   -44,   -45,   -46,   -47,   -48,   -49,   -50,   -51,
	   -52,   -53,   -54,   -55,   -56,   -57,   -58,   -59,   -60,   -61,
	   -62,   -63,   -64,   -65,   -66,   -67,   -68,   -69,   -70,   -71,
	   -72,   -73,   -74,   -75,   -76,   -77,   -78,   -79,   -80,   -81,
	   -82,   -83,   -84,   -85,   -86,   -87,   -88,   -89,   -90,   -91,
	   -92,   -93,   -94,   -95,  -523,   390,   407,   409,   257,   -96,
	   -98,   295,  -103,  -129,  -142,  -144,  -146,  -148,  -151,  -152,
	   296,  -153,  -156,  -159,  -163,  -168,  -170,  -180,  -192,  -193,
	  -197,  -198,  -209,  -213,  -216,  -217,  -228,  -241,  -245,  -246,
	  -251,  -294,   285,   284,   286,  -305,  -313,  -315,  -317,  -319,
	  -337,  -339,  -345,  -347,  -348,  -349,   338,   330,  -351,  -353,
	   339,   337,   336,  -385,  -388,   316,  -389,  -392,  -398,  -401,
	  -403,  -404,  -405,  -418,  -432,  -435,  -438,   327,  -440,  -443,
	  -449,  -451,  -456,  -458,  -462,  -467,  -469,  -484,  -487,  -490,
	  -492,  -502,  -503,  -506,  -483,  -524,  -527,  -528,  -529,  -530,
	   258,   291,   259,   260,   262,   263,   304,   301,   302,   303,
	   305,   306,  -154,  -155,   287,   265,   297,   298,   267,   299,
	   269,   272,   271,  -202,   270,   273,   274,   275,   276,   277,
	   278,   300,   293,   294,   289,   288,   292,  -321,   313,  -341,
	  -346,   312,   310,   318,  -357,  -387,   317,  -390,  -394,   344,
	   331,   347,   346,  -408,   328,   343,   332,  -439,   335,  -445,
	   329,   333,   334,  -460,  -465,  -468,  -471,  -486,  -488,  -491,
	  -494,   358,  -504,  -508,   359,  -531,   423,   422,   430,  -533,
	  -534,  -536,  -537,  -538,  -539,  -540,  -573,  -578,   264,   307,
	   308,   309,  -203,   311,   315,   314,   320,   319,   325,   326,
	   341,   324,   340,   348,   349,   351,   354,   350,   353,   352,
	   357,  -505,   355,   421,  -541,  -542,  -553,  -576,   420,   356,
	   413,   414,   412,   417,   418,   415,   416,   411,   425,   424,
	   261,  -521,   368,   -97,  -250,   389,  -128,   386,  -579,  -570,
	   402,   428,   -99,   368,  -104,  -128,  -130,  -104,  -143,   362,
	  -145,  -104,  -104,  -145,  -110,  -283,  -122,   283,  -157,  -111,
	  -158,  -128,   389,  -160,   281,  -164,   367,  -104,  -104,  -181,
	  -185,   275,   258,   262,   274,  -150,  -111,   360,  -145,  -186,
	   366,  -104,  -199,  -105,   399,  -104,  -214,  -104,  -215,   265,
	  -100,  -104,  -218,  -219,  -221,   281,   364,   368,  -229,    -9,
	   -10,   -13,   409,  -104,   -97,  -104,  -104,  -295,  -297,  -298,
	   289,  -312,  -128,   389,  -306,   399,  -298,  -316,   399,  -318,
	   399,  -320,  -323,   373,  -322,   389,  -128,  -340,   399,  -101,
	  -105,  -322,  -322,  -350,  -238,   389,  -579,   379,   375,   360,
	  -352,  -298,  -354,  -361,  -362,   321,  -386,  -384,  -316,   399,
	  -352,  -316,  -393,   399,  -399,   399,  -298,  -306,  -306,  -406,
	   399,  -419,   399,  -312,  -473,  -128,  -433,   399,  -436,   388,
	  -318,  -441,   378,   375,   383,  -442,   388,  -406,  -450,  -309,
	   387,   397,  -128,   386,   431,  -452,  -453,  -367,   323,  -457,
	   375,  -459,   399,  -463,   382,  -316,  -470,   399,  -363,  -318,
	   399,  -318,  -318,  -493,  -495,  -497,   368,   380,   381,    -9,
	    -9,  -507,   399,  -298,  -525,  -532,  -528,  -565,  -533,  -566,
	  -568,  -570,  -528,  -526,   407,   258,   262,   274,   275,  -150,
	  -145,  -145,  -104,   281,   265,  -322,  -322,  -322,  -298,   333,
	  -298,  -391,  -298,  -391,  -409,   389,   388,   375,   383,  -391,
	  -409,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -384,
	  -298,   430,   430,  -532,  -568,  -535,  -532,  -532,  -535,  -532,
	  -532,  -535,  -574,   399,  -104,   420,  -298,  -543,  -545,   420,
	   426,  -554,  -556,  -557,  -558,   420,   419,   420,   399,   412,
	   415,   413,   412,   413,   414,   412,   417,  -522,    -2,   410,
	  -571,  -580,   438,   440,  -581,  -582,   388,  -583,  -585,  -587,
	  -589,  -590,   427,   429,   399,  -100,  -105,  -105,  -132,   403,
	  -100,  -114,   398,  -147,  -149,  -150,  -126,  -250,  -128,  -175,
	  -284,  -285,  -104,  -128,  -161,   367,  -104,  -169,  -105,  -171,
	   368,  -182,  -114,  -161,  -186,  -171,  -194,  -195,  -104,  -171,
	  -200,  -208,   374,  -204,  -205,  -134,  -104,  -161,  -104,  -104,
	  -101,  -196,   401,  -219,  -230,  -105,  -130,   -11,  -242,   370,
	  -247,  -248,  -249,  -178,  -161,  -250,  -119,  -128,  -179,  -122,
	   389,   369,  -252,  -101,  -105,  -296,  -299,   399,  -307,  -308,
	  -128,  -309,  -310,  -311,  -312,   431,   387,   397,  -314,  -514,
	  -343,  -515,  -518,  -516,   290,  -238,  -509,  -510,  -511,  -518,
	  -298,   342,  -324,   275,   291,  -338,   373,  -128,  -342,  -343,
	  -344,  -238,  -278,  -279,  -280,  -517,  -281,  -271,   290,  -104,
	  -275,   398,  -355,  -365,  -366,  -367,  -363,    -9,   400,  -395,
	  -396,  -397,  -238,  -400,   388,  -402,   398,  -407,  -412,   431,
	  -413,  -414,  -238,  -360,  -420,  -421,   283,  -434,   388,   399,
	  -298,  -298,  -444,  -446,   431,  -447,  -415,  -416,   388,   389,
	   334,   323,   375,   383,   377,   386,  -453,  -454,  -455,   376,
	  -375,   388,  -298,  -352,   400,  -464,  -466,   363,  -316,   400,
	  -472,  -473,  -474,  -128,  -475,  -298,  -485,  -489,   283,   400,
	  -485,  -496,  -450,   345,  -498,  -500,   382,  -352,   400,  -298,
	  -526,  -549,   398,   433,   403,  -567,   420,  -567,  -526,   265,
	  -358,   389,  -128,  -410,  -410,  -298,  -298,  -298,  -298,  -298,
	  -298,  -298,  -298,  -298,  -578,   420,  -575,  -552,  -577,   386,
	   431,   387,   397,   389,   420,   411,  -298,  -544,  -546,   409,
	  -555,  -559,   409,  -555,  -555,   413,  -572,  -584,  -586,   388,
	  -589,  -572,  -570,  -101,  -106,  -111,  -112,  -128,  -113,  -104,
	  -131,  -133,  -111,  -134,  -104,  -104,  -102,  -179,  -104,  -114,
	  -114,  -258,   433,  -104,  -165,   365,  -110,  -145,  -172,  -175,
	  -104,   271,  -104,  -185,  -114,  -162,   365,   433,  -104,  -201,
	  -127,   362,   388,  -107,  -114,   400,  -206,  -207,   365,  -115,
	   365,   433,  -104,  -102,  -104,  -101,  -102,  -232,  -233,  -234,
	  -517,  -235,  -237,   404,   -12,    -2,   410,  -243,  -111,   -97,
	  -128,  -249,  -174,  -104,  -178,  -175,  -175,  -102,  -111,  -300,
	  -301,  -303,  -298,   290,   377,   400,   398,   387,   397,  -298,
	   400,   398,   400,  -304,   365,   433,   399,  -239,  -277,   403,
	   400,   398,   400,  -304,   399,  -325,  -105,   281,   388,   400,
	   398,   400,  -239,  -107,  -114,  -107,  -282,  -115,   361,   399,
	  -196,  -104,  -150,  -298,  -356,  -381,  -366,  -368,  -370,  -371,
	   376,   384,   385,   377,  -375,   388,  -364,    -9,   400,   398,
	  -304,  -239,   398,  -304,  -298,   400,   398,  -415,  -239,  -422,
	  -423,  -424,  -426,   388,   389,  -304,  -419,  -298,   400,   398,
	  -304,   399,  -369,   398,   403,  -372,   389,   360,  -128,  -373,
	  -376,   399,   386,  -128,   400,  -318,   400,   398,  -304,   399,
	  -499,  -501,   363,  -316,   400,  -565,  -569,  -572,  -359,   399,
	  -411,   389,   386,  -128,  -461,  -450,  -461,  -461,  -358,  -461,
	  -461,  -461,   386,   387,   397,  -547,  -548,  -550,   420,  -560,
	  -562,  -563,    -8,   256,  -533,  -564,  -537,  -538,  -539,  -540,
	  -534,  -536,   430,   405,  -572,  -588,   401,   408,  -589,  -102,
	  -107,  -114,  -115,  -107,  -114,  -167,  -111,  -105,  -253,  -236,
	  -128,   435,  -260,  -261,  -270,  -271,  -262,  -125,  -264,  -124,
	  -272,  -273,  -274,  -266,   431,  -265,  -250,  -291,  -292,  -119,
	  -293,   372,  -276,  -104,   437,  -519,   387,   396,   397,   404,
	   411,   417,   412,   414,   413,   415,   416,   430,  -126,  -284,
	  -286,   386,  -160,   289,  -105,  -162,  -166,  -167,  -111,  -107,
	  -160,  -183,  -105,  -195,  -104,  -110,  -119,  -128,  -205,  -119,
	  -135,  -137,  -138,  -104,  -519,  -210,  -105,  -102,  -231,  -240,
	  -107,  -114,  -107,  -115,  -238,  -126,  -174,  -104,  -107,   400,
	  -302,   398,  -304,  -308,  -515,  -298,   326,  -238,  -511,  -512,
	  -513,  -128,  -309,  -310,  -311,  -312,   280,  -298,  -102,  -335,
	  -336,  -104,  -271,  -111,  -344,  -280,  -236,  -105,  -128,   280,
	  -382,  -383,   322,  -369,   398,   375,   383,  -372,  -372,  -372,
	  -374,   388,   389,  -396,  -298,  -400,  -298,  -413,  -304,   398,
	  -425,   399,   433,  -298,   398,  -447,  -448,   389,   386,   387,
	   397,  -128,   367,   400,  -417,   389,   386,   384,   385,   330,
	   376,   343,   383,   375,  -128,    -9,  -455,  -373,  -373,  -377,
	  -378,  -379,   388,   389,   334,   323,  -475,  -476,  -444,  -318,
	  -360,   410,  -549,  -551,   433,  -561,  -562,   410,  -532,  -533,
	  -537,  -538,  -539,  -540,  -588,  -572,  -108,   364,  -127,  -113,
	  -116,  -118,  -119,  -120,  -104,  -519,  -110,  -134,  -254,   436,
	  -167,  -236,  -167,  -255,  -207,  -263,  -125,  -257,  -258,  -259,
	   438,   439,   440,   434,   432,  -239,  -236,  -236,  -107,  -105,
	  -105,   399,  -238,  -273,  -119,  -287,  -128,  -289,  -104,  -130,
	  -173,  -161,  -184,  -111,  -187,  -171,  -145,  -196,  -136,  -122,
	  -139,   435,  -105,  -107,  -114,  -200,  -211,  -104,  -220,  -222,
	   279,  -220,  -234,  -236,   280,  -239,  -244,  -104,  -301,  -298,
	   399,  -239,   400,  -326,  -220,  -107,  -114,  -282,  -115,  -236,
	  -384,    -9,  -370,  -372,  -372,  -373,   400,  -423,  -428,  -429,
	  -430,   388,   389,  -427,   388,   389,   400,  -437,  -128,  -239,
	   400,   400,   398,  -304,  -477,  -480,  -519,   389,  -128,   400,
	  -548,  -552,   431,  -526,   400,  -109,  -119,  -128,  -117,  -122,
	  -105,  -104,  -119,  -121,   280,  -167,  -107,  -107,  -236,  -256,
	  -253,  -236,  -236,  -107,  -114,  -236,  -238,  -239,  -288,   388,
	  -107,  -114,  -212,  -290,   388,  -105,  -174,  -176,  -177,  -104,
	  -102,  -188,  -191,   371,  -104,  -150,  -119,  -107,  -104,  -121,
	  -140,  -141,  -122,   435,   280,  -126,  -126,  -201,  -107,  -114,
	  -212,  -110,  -223,  -225,   361,  -126,  -298,  -304,  -327,  -328,
	  -330,   373,  -336,  -236,   280,  -372,   400,   398,   433,   400,
	  -239,  -378,  -380,   389,   386,  -128,  -478,   399,  -110,  -121,
	  -126,   280,  -236,  -267,  -225,  -239,  -104,  -145,  -114,  -132,
	  -189,  -127,  -104,  -150,   388,   388,  -107,  -110,  -104,  -224,
	  -226,  -227,  -104,   400,  -512,  -329,  -186,  -331,   388,  -429,
	  -431,  -309,  -312,  -310,  -311,  -128,  -360,  -479,   283,  -481,
	   435,   386,  -105,  -107,  -114,  -107,  -114,  -268,   388,  -269,
	  -236,   400,  -212,  -107,  -177,  -104,  -190,  -161,  -126,  -212,
	  -114,  -132,  -196,  -105,   388,   280,  -482,   283,   280,   400,
	   398,  -123,  -124,  -125,  -126,  -236,  -102,  -114,  -110,  -104,
	  -132,  -226,  -104,  -104,  -332,  -333,  -334,  -275,  -104,  -150,
	   388,   386,  -107,  -124,  -107,  -107,  -107,  -236,  -171,  -126,
	  -107,  -114,  -334,  -104,   400,  -104,  -171,  -333,  -126,  -132,
	  -126
};

short yydef[] = {
	     2,    -2,     1,     3,     4,     5,     6,     7,     8,     9,
	    10,   951,    15,    16,    17,    18,    19,    20,    21,    22,
	    23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
	    33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
	    43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
	    53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
	    63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
	    73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
	    83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
	    93,    94,    95,    96,   955,   953,   954,   949,    97,  1053,
	   457,   103,  1053,  1053,   156,  1053,  1053,    -2,   417,   169,
	   172,   173,  1053,   461,   192,  1053,  1053,  1053,  1053,     0,
	  1053,   258,  1053,  1053,  1053,    -2,   319,  1053,  1053,  1053,
	  1053,  1053,   502,   503,   504,     0,  1053,     0,     0,     0,
	  1053,     0,     0,  1053,  1053,  1053,   577,     0,  1053,   593,
	   642,   643,   644,   910,  1053,   651,     0,     0,     0,  1053,
	     0,     0,     0,  1053,   741,   746,     0,   751,   754,     0,
	  1053,     0,   787,   794,   798,     0,   811,   894,     0,     0,
	     0,     0,     0,   871,  1053,  1053,     0,     0,   959,   960,
	   101,     0,   105,   131,   153,   161,   162,   163,   164,   165,
	   170,   171,    -2,  1053,   183,   188,   190,   196,   200,   215,
	   244,   246,   252,    -2,    -2,   278,   284,   287,   323,   329,
	   331,   345,   489,   506,   516,   518,   520,  1053,   554,  1053,
	  1053,   570,   572,   574,  1053,  1053,   650,    -2,    -2,   664,
	   668,   670,   672,     0,   713,   739,   744,    -2,   753,     0,
	   775,   777,   786,  1053,  1053,  1053,  1053,  1053,  1053,  1053,
	  1053,   861,   910,  1053,   831,   961,   963,   964,   966,  1053,
	    -2,  1053,    -2,  1053,  1053,    -2,  1064,  1077,   178,   179,
	   180,   181,  1053,   523,   560,   568,   585,   647,   654,   657,
	   676,   750,   762,   791,   797,   805,   808,   834,   837,   843,
	   846,  1053,   868,   967,   978,  1001,  1017,     0,     0,   865,
	   979,   980,   981,   982,   983,   990,   991,   995,   998,  1015,
	  1062,     0,    99,    98,   484,   485,   486,   478,  1080,     0,
	  1082,     0,  1053,   456,     0,   475,     0,   132,  1053,   155,
	   157,   476,  1053,   159,   168,   416,  1053,   459,   182,   184,
	   185,   487,   186,     0,   460,  1053,   191,   197,     0,     0,
	   216,   218,   219,   220,   221,   222,   223,   434,   243,  1053,
	   451,     0,     0,  1053,   449,     0,   277,    -2,  1053,   282,
	     0,   473,   307,     0,  1053,   306,   290,   291,   304,   320,
	    13,    12,   926,   322,   328,    -2,     0,     0,   490,   491,
	   492,   873,   874,   872,   505,  1053,   880,   517,  1053,   519,
	  1053,   521,     0,   528,    -2,   544,   545,   558,  1053,   566,
	  1053,   569,   571,   573,   575,   576,  1079,   578,   581,   579,
	   580,   878,   598,   591,   894,   594,     0,   648,   908,  1053,
	   649,   652,   655,  1053,   663,     0,   925,   669,   671,  1053,
	   675,   715,   934,   875,   876,   877,   738,     0,     0,   745,
	   748,   752,   755,  1053,  1053,   758,   759,     0,   774,   888,
	   889,   890,   891,   882,     0,   776,   778,     0,   602,   785,
	  1053,   789,  1053,   800,     0,   803,   806,  1053,   838,   892,
	  1053,   838,   841,   844,  1053,   854,   849,   852,   853,   860,
	   862,   866,  1053,  1053,     0,   969,   970,  1044,  1053,  1049,
	     0,  1052,     0,   958,  1099,   102,   154,   285,   288,   175,
	   176,   177,   255,     0,   272,   522,   559,   567,    -2,   584,
	   646,   653,   920,   656,   681,   677,   678,   679,   680,   749,
	   681,  1053,  1053,  1053,  1053,  1053,  1053,  1053,  1053,   863,
	  1053,   962,   965,   971,     0,   972,  1042,   973,   974,   975,
	   976,   977,     0,  1063,   256,   947,  1053,     0,   997,   999,
	    -2,     0,     0,  1013,     0,  1016,    -2,  1061,  1078,   988,
	   992,   993,   989,   984,   985,   986,   987,   948,   950,   952,
	  1054,  1058,  1055,  1056,  1084,  1085,  1086,     0,  1089,  1058,
	  1092,  1053,  1096,  1094,  1083,     0,    -2,  1053,  1053,   436,
	   350,  1053,   437,   158,   166,   167,   481,   479,   480,   418,
	   419,     0,   422,    -2,  1053,   452,   447,   417,  1053,  1053,
	   455,  1053,  1053,   224,   225,   226,   245,   247,     0,  1053,
	   267,     0,   439,     0,   260,   264,   263,  1053,   279,   280,
	   350,  1053,   453,     0,   350,   316,   289,     0,  1053,   324,
	   330,    -2,    -2,   339,   340,   341,   342,   343,  1053,  1053,
	   483,   458,   350,   346,  1053,   488,  1053,   494,     0,   508,
	   510,   511,   512,   513,   514,     0,   884,   886,    -2,     0,
	     0,   913,   931,     0,     0,   396,     0,     0,   897,   932,
	     0,     0,     0,   526,   529,   553,     0,   557,     0,     0,
	   563,   396,     0,     0,   410,   928,     0,   413,     0,   415,
	  1053,  1053,   638,   597,   599,   614,   596,   645,   909,     0,
	   659,     0,   396,     0,     0,  1053,   924,     0,   686,   687,
	   688,     0,   396,   712,   714,     0,   717,     0,   742,  1053,
	   756,  1053,     0,   763,   764,   765,     0,   692,   695,   696,
	   697,   698,   699,   700,   701,   883,   779,   604,   781,  1053,
	   784,    -2,   788,     0,   793,   795,     0,   802,   799,   809,
	     0,   812,   813,    -2,   814,     0,   832,     0,   840,   893,
	   835,   847,   850,   851,   857,     0,   856,     0,   870,   830,
	   956,  1053,  1043,  1046,  1047,  1048,  1051,  1058,   957,   271,
	   590,   586,   587,    -2,    -2,    -2,    -2,    -2,    -2,    -2,
	   836,    -2,   845,    -2,  1076,  1051,  1060,  1065,  1066,  1067,
	     0,  1069,  1071,  1073,  1074,  1075,   864,   996,     0,  1004,
	  1011,     0,  1021,  1012,  1014,   994,  1081,  1087,  1090,  1088,
	  1058,     0,     0,   350,     0,   106,   107,    -2,   109,     0,
	     0,   134,   135,   136,     0,   133,   152,  1053,   477,  1053,
	  1053,   461,   406,     0,  1053,   446,   195,     0,   461,   201,
	    -2,   203,   228,   217,  1053,  1053,   440,   441,   251,   417,
	  1053,   438,   266,   259,  1053,   454,   262,     0,   448,  1053,
	   442,   443,   273,   283,   474,   350,   318,     0,     0,   310,
	   929,     0,  1053,   315,    11,    14,   927,   321,   325,  1053,
	    -2,   211,   335,    -2,   210,   212,   213,   344,     0,     0,
	   495,     0,   498,   499,   500,   507,  1053,   885,   887,   881,
	   911,  1053,   912,  1053,   922,   923,   933,   917,  1053,   398,
	   895,  1053,   896,  1053,  1053,   350,  1053,   527,   556,   561,
	  1053,   562,   565,   408,  1053,   409,  1053,   444,   445,   930,
	   390,   388,   389,   879,   582,   640,   600,   604,   605,     0,
	  1053,  1053,  1053,     0,   613,    -2,   592,   595,   658,  1053,
	  1053,   662,     0,  1053,   667,   673,  1053,   690,     0,   716,
	   718,   727,     0,   722,   723,  1053,     0,   757,   760,     0,
	  1053,  1053,     0,     0,   603,   625,   616,   617,   618,   621,
	   625,     0,   623,   624,   792,   801,   810,  1053,   816,     0,
	   848,     0,   859,   855,   869,  1045,  1050,  1057,   715,   589,
	   682,   683,   684,   685,   790,   918,   796,   804,   807,   833,
	   842,   867,  1068,  1070,  1072,     0,  1005,  1008,  1009,     0,
	  1023,  1053,  1026,  1027,  1028,     0,  1030,  1032,  1034,  1036,
	  1038,  1039,  1040,  1059,     0,  1091,  1097,  1098,  1058,   100,
	     0,  1053,  1053,   417,  1053,   348,   349,  1053,  1053,     0,
	    -2,   400,   361,   362,   382,   383,   363,  1053,  1053,   384,
	   385,   386,   387,     0,   402,     0,   465,   466,   467,   468,
	   469,     0,  1053,   373,   375,   937,   470,   472,   471,   394,
	   939,   940,   941,   942,   943,   944,   945,   946,   482,   420,
	   421,   424,  1053,   427,  1053,  1053,   189,   193,   194,   198,
	   205,    -2,  1053,   248,   249,   253,   268,   269,   261,   265,
	   148,   139,     0,   143,   936,     0,  1053,   294,   305,   294,
	   308,   316,   309,  1053,   396,   326,   334,    -2,   347,   493,
	  1053,   501,  1053,   509,   914,   915,     0,   397,   898,   899,
	   901,   396,   903,   904,   905,   906,   907,     0,   294,     0,
	   547,     0,   550,   551,   564,   411,   412,  1053,    -2,   414,
	   637,   910,   641,     0,   614,  1053,  1053,   625,   609,   610,
	   612,   619,   620,   660,   661,     0,   666,   689,   691,     0,
	   720,     0,     0,     0,  1053,   766,   767,   768,   769,   770,
	   771,   396,   773,   693,     0,   702,   703,   704,   705,   706,
	   707,   708,   709,   710,   711,   780,   782,   783,   622,     0,
	   627,     0,   630,   631,   632,   633,   815,  1053,     0,   858,
	   583,  1003,     0,     0,  1010,  1020,  1024,  1022,     0,  1029,
	  1031,  1033,  1035,  1037,  1095,     0,  1053,   126,   127,   110,
	   118,   112,   113,    -2,   125,   935,   130,   137,  1053,   399,
	     0,     0,   352,  1053,   359,  1053,  1053,   356,   357,   358,
	   403,   404,   405,   407,   401,   391,   366,     0,   371,  1053,
	   372,  1053,   396,   393,   425,   431,   429,     0,   462,     0,
	   211,  1053,   350,   229,   237,  1053,     0,  1053,   138,     0,
	   145,     0,  1053,   140,  1053,   267,     0,   462,   417,   297,
	   295,   317,   311,   312,   313,   314,  1053,   338,   496,   497,
	  1053,   902,     0,   530,   525,   546,  1053,  1053,   444,     0,
	   639,   601,   606,   607,  1053,   608,   665,   719,     0,   728,
	     0,   731,   732,   721,   724,   725,   740,     0,   396,   772,
	   694,   626,     0,  1053,   819,   818,   938,   822,   823,   839,
	  1006,  1007,     0,  1025,  1093,   417,   128,   129,   111,     0,
	  1053,   115,   116,   117,   450,   353,   351,   367,   354,     0,
	   360,   364,   365,   368,  1053,   376,   396,   392,   426,   430,
	   428,  1053,   432,   463,   464,  1053,   199,   204,   206,   208,
	   214,   240,  1053,   238,   231,   232,   233,   227,   250,   144,
	   146,   147,     0,     0,   149,   142,     0,   417,   274,  1053,
	   275,   286,  1053,   296,   435,   327,     0,  1053,   524,   536,
	     0,   533,   548,   549,   552,   611,   726,     0,  1053,   715,
	   747,   628,   629,   634,   635,   636,   824,     0,   104,   120,
	     0,   355,     0,   381,  1053,     0,   462,     0,  1053,  1053,
	   242,  1053,   235,   236,   150,   151,   141,   270,   462,   293,
	   298,   300,   302,   916,   900,   531,     0,   532,   534,   729,
	   730,   733,   734,   735,   736,   737,   743,   817,     0,   829,
	     0,     0,     0,   123,  1053,   369,  1053,   350,   380,   377,
	   378,   395,   433,   417,   207,   209,   230,  1053,     0,   276,
	  1053,  1053,  1053,  1053,   535,   825,   826,     0,   827,   820,
	     0,     0,   121,     0,     0,     0,     0,  1053,   187,     0,
	  1053,   299,   301,   303,     0,   538,   540,  1053,   542,   543,
	   828,     0,   119,   122,   124,   370,   374,   379,  1053,     0,
	   537,  1053,   541,   542,   821,   241,  1053,   539,     0,  1053,
	   239
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
#line 377 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 3:
#line 383 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 414 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_flag &= ~GR_EQUEL;
		    gen_eqstmt( G_CLOSE, (char *)0 );
		}
		break;
	case 5:
#line 419 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if ((gr->gr_flag & GR_DECLCSR) == 0)
			gen_eqstmt( G_CLOSE, (char *)0 );
		    gr->gr_flag &= ~(GR_QUEL|GR_REPEAT|GR_NOSYNC|
				     GR_CURSOR|GR_DECLCSR|GR_REPLCSR);
		}
		break;
	case 6:
#line 426 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_flag &= ~GR_EQUEL;
		    gen_eqstmt( G_CLOSE, (char *)0 );
		}
		break;
	case 7:
#line 431 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if (*(yypvt[-0].s) == 'i')	/* "include" or "<EOF>" */
			inc_push_file();
		    else
			inc_pop_file();
		}
		break;
	case 10:
#line 440 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 496 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    lbl_exit_block();
		    gr_mechanism( GR_BLOCK, GR_BLKFALSE ); /* Symtab updates */
		}
		break;
	case 12:
#line 502 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 514 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 97:
#line 644 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    arg_int_add( IIxactABORT );
		    gen_call( IIXACT );
		    gr->gr_flag |= GR_NOSYNC;      /* No IIsync after this */
		}
		break;
	case 99:
#line 653 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 101:
#line 665 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 102:
#line 669 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-0].s );
		}
		break;
	case 103:
#line 677 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    arg_int_add( IIxactBEGIN );
		    gen_call( IIXACT );
		    gr->gr_flag |= GR_NOSYNC;      /* No IIsync after this */
		}
		break;
	case 105:
#line 691 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 126:
#line 724 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 129:
#line 731 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 759 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 149:
#line 790 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		    if (STbcompare(yypvt[-1].s, 0, ERx("not"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
						    ERx("CREATE"), ERx("NOT") );
		}
		break;
	case 150:
#line 799 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( ERx("default") );
		    if (STbcompare(yypvt[-0].s, 0, ERx("default"), 0, TRUE) != 0)
			er_write(E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
						ERx("CREATE"), ERx("DEFAULT") );
		}
		break;
	case 151:
#line 807 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 825 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 154:
#line 829 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-0].s );
		}
		break;
	case 161:
#line 849 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 162:
#line 853 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 163:
#line 859 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 164:
#line 863 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 165:
#line 868 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 170:
#line 892 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 171:
#line 897 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 172:
#line 904 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    arg_int_add( IIxactCOMMIT );
		    gen_call( IIXACT );
		    gr->gr_flag |= GR_NOSYNC;      /* No IIsync after this */
		}
		break;
	case 173:
#line 916 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 178:
#line 926 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 179:
#line 932 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 180:
#line 936 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 181:
#line 940 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 183:
#line 950 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		}
		break;
	case 186:
#line 958 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 188:
#line 970 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 190:
#line 980 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 196:
#line 999 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 200:
#line 1016 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 203:
#line 1023 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 215:
#line 1053 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 218:
#line 1061 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 219:
#line 1065 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 220:
#line 1069 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 221:
#line 1073 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 238:
#line 1099 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 243:
#line 1113 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1, yypvt[-1].s );
		}
		break;
	case 244:
#line 1118 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 246:
#line 1128 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 252:
#line 1144 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 255:
#line 1179 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 1189 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 257:
#line 1194 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 266:
#line 1211 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 1229 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 1249 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-2].s );
		    db_key( ERx("unique") );
		    db_key( ERx("index") );
		}
		break;
	case 272:
#line 1255 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-1].s );
		    db_key( ERx("index") );
		}
		break;
	case 278:
#line 1279 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 279:
#line 1284 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 1297 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( ERx("index") );
		}
		break;
	case 284:
#line 1307 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 285:
#line 1311 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-0].s );
		}
		break;
	case 287:
#line 1329 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 288:
#line 1333 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-0].s );
		}
		break;
	case 293:
#line 1345 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 1;
		}
		break;
	case 294:
#line 1349 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 0;
		}
		break;
	case 295:
#line 1354 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 304:
#line 1394 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 1405 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 1428 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 308:
#line 1434 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_func = IINEXTGET;
		}
		break;
	case 309:
#line 1438 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_func = IITUPGET;
		}
		break;
	case 313:
#line 1447 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 314:
#line 1452 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    ret_add( gr->gr_sym, gr->gr_id, gr->gr_nlsym, gr->gr_nlid,
				(char *)0, 0 );
		}
		break;
	case 315:
#line 1458 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    er_write( E_EQ0065_grRETREPEAT, EQ_ERROR, 0 );
		}
		break;
	case 317:
#line 1464 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_flag &= ~GR_TUPSORT;
		}
		break;
	case 318:
#line 1469 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_flag |= GR_TUPSORT;
		}
		break;
	case 323:
#line 1483 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 324:
#line 1488 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 326:
#line 1494 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    /* Bug #6749 - Need a space for run-time */
		    db_op(ERx(" "));
		}
		break;
	case 329:
#line 1506 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 331:
#line 1516 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		    gr->gr_func = DB_STRING;
		}
		break;
	case 340:
#line 1532 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_func = DB_ID;		/* ON tablename */
		}
		break;
	case 343:
#line 1539 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 1559 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 348:
#line 1577 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 0;
		}
		break;
	case 349:
#line 1581 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 1;
		}
		break;
	case 350:
#line 1585 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 0;
		}
		break;
	case 355:
#line 1597 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 361:
#line 1617 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = yypvt[-0].i;		/* Not really an expression */
		}
		break;
	case 362:
#line 1621 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 3;
		}
		break;
	case 375:
#line 1655 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 380:
#line 1666 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if (STbcompare(yypvt[-0].s, 0, ERx("only"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
						    ERx("WHERE"), ERx("ONLY") );
		    db_key( yypvt[-0].s );
		}
		break;
	case 382:
#line 1684 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 0;
		}
		break;
	case 383:
#line 1688 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    /* Note that gr_sym will be left around after second name */
		    yyval.i = gr->gr_sym ? 2 : 1;
		}
		break;
	case 387:
#line 1697 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 1718 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = gr->gr_sym ? (char *)0 : yypvt[-0].s;
		    if (gr->gr_flag & GR_REPLCSR)
			er_write( E_EQ0413_csRANGEREPL, EQ_ERROR, 3, yypvt[-1].s,
							yypvt[-0].s, ecs_namecsr() );
		}
		break;
	case 389:
#line 1725 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = (char *)0;
		    if (gr->gr_flag & GR_REPLCSR)
			er_write( E_EQ0413_csRANGEREPL, EQ_ERROR, 3, yypvt[-1].s,
							yypvt[-0].s, ecs_namecsr() );
		}
		break;
	case 390:
#line 1733 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 1752 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 1770 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_var( DB_REG, gr->gr_sym, gr->gr_id, gr->gr_nlsym,
			    gr->gr_nlid, (char *)0 );
		}
		break;
	case 394:
#line 1777 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if (gr->gr_flag & GR_REPEAT)
			rep_param();
		    else
			er_write( E_EQ0376_repARG, EQ_ERROR, 0 );
		}
		break;
	case 395:
#line 1788 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 1808 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_nlsym = (SYM *)0;	/* Initialize state vars */
		    gr->gr_nlid = (char *)0;
		    gr->gr_nltype = T_NONE; 
		}
		break;
	case 397:
#line 1814 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 1841 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 1854 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 400:
#line 1859 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 401:
#line 1864 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 402:
#line 1869 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 403:
#line 1874 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 404:
#line 1878 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 405:
#line 1882 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 406:
#line 1887 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 407:
#line 1892 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_op( ERx("!=") );
		}
		break;
	case 409:
#line 1904 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if (gr->gr_flag & GR_REPLCSR)
			ecs_chkupd( NULL, NULL, NULL );		/* check csr */
		}
		break;
	case 414:
#line 1915 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 415:
#line 1920 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if (gr->gr_flag & GR_REPLCSR)
			ecs_chkupd( NULL, yypvt[-0].s, gr->gr_sym );
		}
		break;
	case 421:
#line 1960 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_flag &= ~GR_CPYHNDDEF;
		}
		break;
	case 423:
#line 1966 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 1991 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 2004 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_sconst( yypvt[-0].s );
		}
		break;
	case 429:
#line 2010 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 2038 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 434:
#line 2050 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 435:
#line 2055 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 436:
#line 2060 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 437:
#line 2065 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_op(ERx(", "));	/* Need a space for run-time */
		}
		break;
	case 438:
#line 2070 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 439:
#line 2075 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 440:
#line 2086 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_op( ERx("=") );
		}
		break;
	case 441:
#line 2090 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_op( ERx("=") );
		}
		break;
	case 442:
#line 2102 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_op( ERx("=") );
		    er_write( E_EQ0071_grASSGNOP, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 443:
#line 2107 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_op( ERx("=") );
		}
		break;
	case 445:
#line 2121 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_op( ERx("=") );
		    er_write( E_EQ0071_grASSGNOP, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 448:
#line 2141 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 449:
#line 2146 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 450:
#line 2151 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 451:
#line 2156 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 452:
#line 2161 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 453:
#line 2166 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 454:
#line 2171 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 455:
#line 2176 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 456:
#line 2181 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 458:
#line 2187 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 459:
#line 2192 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 460:
#line 2197 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 464:
#line 2208 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 2236 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 471:
#line 2243 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 472:
#line 2250 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_hexconst(yypvt[-0].s);
		}
		break;
	case 475:
#line 2264 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 2299 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 480:
#line 2308 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 2330 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_sconst( yypvt[-0].s );
		}
		break;
	case 485:
#line 2339 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 486:
#line 2343 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 2364 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 2392 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_int_add( IIutEXE );
		    arg_str_add( ARG_CHAR, (char *)0 );
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_call( IIUTSYS );
		}
		break;
	case 489:
#line 2399 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    arg_int_add( IIutPROG );
		}
		break;
	case 490:
#line 2405 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_call( IIUTSYS );
		}
		break;
	case 492:
#line 2412 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 494:
#line 2419 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_int_add( IIutARG );
		}
		break;
	case 497:
#line 2427 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( IIUTSYS );
		}
		break;
	case 499:
#line 2433 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 2437 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 501:
#line 2442 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_int_add( IIutARG );
		}
		break;
	case 502:
#line 2457 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 2481 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 2522 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    gen_call( IIEXIT );
		}
		break;
	case 505:
#line 2535 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( QAPRINTF );
		}
		break;
	case 506:
#line 2540 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		}
		break;
	case 510:
#line 2550 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if (gr->gr_sym)
			arg_var_add( gr->gr_sym, gr->gr_id );
		    else
			arg_str_add( ARG_CHAR, gr->gr_id );
		}
		break;
	case 511:
#line 2557 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_str_add( ARG_INT, yypvt[-0].s );
		}
		break;
	case 512:
#line 2561 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_str_add( ARG_FLOAT, yypvt[-0].s );
		}
		break;
	case 513:
#line 2565 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_str_add( ARG_PACK, yypvt[-0].s );
		}
		break;
	case 514:
#line 2569 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 515:
#line 2581 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( IIINGOPEN );
		}
		break;
	case 516:
#line 2586 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    arg_int_add( eq->eq_lang );
		}
		break;
	case 518:
#line 2601 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    gr->gr_func = IIEQINQ;
		}
		break;
	case 520:
#line 2616 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    gr->gr_func = IIEQSET;
		}
		break;
	case 521:
#line 2633 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 2646 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    ecs_declcsr( yypvt[-0].s, gr->gr_sym );   /* sets current cursor */
		    ecs_query( NULL, ECS_START_QUERY );	/* start saving query */
		}
		break;
	case 523:
#line 2653 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_flag |= GR_CURSOR|GR_DECLCSR;
		    gr_mechanism( GR_EQSTMT, GR_sCURSOR, yypvt[-0].s );
		    /* $$.s = $1.s; */
		}
		break;
	case 524:
#line 2662 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 2690 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if (yypvt[-0].i)
			ecs_csrset( NULL, ECS_SORTED );
		}
		break;
	case 526:
#line 2697 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 527:
#line 2701 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		    ecs_csrset( NULL, ECS_UNIQUE );
		}
		break;
	case 529:
#line 2710 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    ecs_csrset( NULL, ECS_REPEAT );
		    gr->gr_flag |= GR_REPEAT;
		    rep_begin( ERx("repeat"), ecs_id_curcsr() );
		}
		break;
	case 530:
#line 2725 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 0;
		}
		break;
	case 531:
#line 2729 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    ecs_csrset( NULL, yypvt[-1].i );
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 532:
#line 2737 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 533:
#line 2744 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 534:
#line 2755 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 2766 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 2795 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 1;
		}
		break;
	case 537:
#line 2799 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 2;
		}
		break;
	case 542:
#line 2815 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 2862 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    er_write( E_EQ0411_csUPDALL, EQ_ERROR, 1, ecs_namecsr() );
		}
		break;
	case 544:
#line 2869 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_xsym = (SYM *)0;
		}
		break;
	case 545:
#line 2874 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 2914 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 2932 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 2943 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    _VOID_ ecs_colupd( NULL, ERx(""), ECS_ADD|ECS_ISWILD );
		}
		break;
	case 552:
#line 2947 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key( yypvt[-0].s );
		    _VOID_ ecs_colupd( NULL, yypvt[-2].s, ECS_ADD|ECS_CONSTANT );
		}
		break;
	case 553:
#line 2965 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    ecs_open( yypvt[-1].s, gr->gr_id, gr->gr_sym, gr->gr_xsym?TRUE:FALSE );
		    gr->gr_xsym = (SYM *)0;	/* Borrowed from gr_nlsym */
		}
		break;
	case 554:
#line 2972 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_flag |= GR_CURSOR;
		    gr_mechanism( GR_EQSTMT, GR_sCURSOR, yypvt[-0].s );
		}
		break;
	case 555:
#line 2979 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_id = (char *)0;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 556:
#line 2984 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if (STbcompare(yypvt[-0].s, 0, ERx("readonly"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					ERx("OPEN CURSOR"), ERx("READONLY") );
		    gr->gr_id = ERx(" for readonly ");
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 557:
#line 2992 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 3012 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    ecs_eretrieve( NULL, 0 );	/* Didn't use an SQLDA! */
		}
		break;
	case 559:
#line 3018 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    ecs_retrieve( yypvt[-0].s, gr->gr_sym != (SYM *)0, 0, 0,
					(SYM *) NULL, (char *) NULL );
		    gr->gr_func = IICSRETRIEVE;
		}
		break;
	case 560:
#line 3026 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_flag |= GR_CURSOR;
		    gr_mechanism( GR_EQSTMT, GR_sCURSOR, yypvt[-0].s );
		}
		break;
	case 565:
#line 3048 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if (gr->gr_sym)
		    {
			ecs_get( gr->gr_id, gr->gr_sym, gr->gr_type,
				 gr->gr_nlid, gr->gr_nlsym, (char *)0 );
		    }
		}
		break;
	case 566:
#line 3065 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    ecs_replace( FALSE, NULL, FALSE );
		}
		break;
	case 567:
#line 3071 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    ecs_replace( TRUE, yypvt[-0].s, gr->gr_sym != (SYM *)0 );
		    gr->gr_flag |= GR_REPLCSR;
		}
		break;
	case 568:
#line 3078 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_flag |= GR_CURSOR;
		    gr_mechanism( GR_EQSTMT, GR_sCURSOR, yypvt[-0].s );
		}
		break;
	case 569:
#line 3091 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    ecs_delete( yypvt[-0].s, (char *)0, gr->gr_sym != (SYM *)0 );
		}
		break;
	case 570:
#line 3097 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_flag |= GR_CURSOR;
		    gr_mechanism( GR_EQSTMT, GR_sCURSOR, yypvt[-0].s );
		}
		break;
	case 571:
#line 3110 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    ecs_close( yypvt[-0].s, gr->gr_sym != (SYM *)0 );
		}
		break;
	case 572:
#line 3116 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_flag |= GR_CURSOR;
		    gr_mechanism( GR_EQSTMT, GR_sCURSOR, yypvt[-0].s );
		}
		break;
	case 573:
#line 3136 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( IIADDFORM );
		    gr->gr_flag &= ~GR_NOINDIR;
		}
		break;
	case 574:
#line 3142 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    gr->gr_flag |= GR_NOINDIR;
		}
		break;
	case 575:
#line 3148 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 3167 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
# ifndef EQ_ADD_FNAME
		    er_write( E_EQ0061_grINTVAR, EQ_ERROR, 1, yypvt[-0].s );
# endif /* EQ_ADD_FNAME */
		}
		break;
	case 577:
#line 3183 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 3204 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-1].s );
		    gen_call( IICLRSCR );
		}
		break;
	case 579:
#line 3209 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( IICLRFLDS );
		}
		break;
	case 581:
#line 3215 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-1].s );
		    gr->gr_func = IIFLDCLEAR;
		}
		break;
	case 582:
#line 3283 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 3316 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 3335 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 3354 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    gr->gr_func = IIDISPFRM;
		}
		break;
	case 586:
#line 3360 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    frs_mode( ERx("DISPLAY/INITTABLE"), yypvt[-0].s );
		}
		break;
	case 587:
#line 3364 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 3381 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_str_add( ARG_CHAR, ERx("f") );
		}
		break;
	case 589:
#line 3386 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    sc_eat( NULL, SC_NEST|SC_SEEN, ERx(")"), '(', ')' );
		    er_write( E_EQ0051_grDISPTL, EQ_ERROR, 0 );
		}
		break;
	case 591:
#line 3414 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 3429 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_if_goto( IIFSETIO, C_EQ, 0, L_FDEND,
			lbl_current(LBL_FORM) );
		    gr->gr_func = IISETFIELD;	/* In case of I/O */
		    gr->gr_mode = LBLmDISPLAY;
		}
		break;
	case 597:
#line 3473 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 3483 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_flag &= ~GR_ACTIVATE;
		}
		break;
	case 601:
#line 3491 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_else( G_ELSE, IIRETVAL, C_0, 0, L_ACTELSE, 
			lbl_current(LBL_INTR), L_ACTEND, lbl_current(LBL_FORM));
		    gr->gr_mode = LBLmDISPLAY;
		}
		break;
	case 602:
#line 3498 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 3509 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    /* Reset activate argument list */
		    act_args( FALSE, (char *)0, (char *)0, 0, (SYM *)0 );
		}
		break;
	case 604:
#line 3514 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    act_args( FALSE, (char *)0, (char *)0, 0, (SYM *)0 );
		}
		break;
	case 607:
#line 3527 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    act_add( ACT_FIELD, lbl_current(LBL_INTR), yypvt[-2].i, yypvt[-0].ip );
		    gr->gr_adjective = LBLaFIELD;
		}
		break;
	case 608:
#line 3532 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    act_add( ACT_MENU, lbl_current(LBL_INTR), 0, yypvt[-1].ip );
		    gr->gr_adjective = LBLaMITEM;
		}
		break;
	case 609:
#line 3537 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    act_add( ACT_SCROLL, lbl_current(LBL_INTR), sclUP, yypvt[-0].ip );
		    gr->gr_adjective = LBLaSCROLL;
		}
		break;
	case 610:
#line 3542 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    act_add( ACT_SCROLL,lbl_current(LBL_INTR), sclDOWN, yypvt[-0].ip );
		    gr->gr_adjective = LBLaSCROLL;
		}
		break;
	case 611:
#line 3547 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    i4	cur_intr = lbl_current( LBL_INTR );

		    act_add( ACT_COLUMN, cur_intr, yypvt[-3].i, yypvt[-1].ip );
		    act_add( ACT_COLUMN, cur_intr, yypvt[-3].i, yypvt[-0].ip );
		    gr->gr_adjective = LBLaCOLUMN;
		}
		break;
	case 612:
#line 3555 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    /* Old-style syntax - no longer supported */
                    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1,
                                            ERx("ACTIVATE COMMAND") );
		}
		break;
	case 614:
#line 3563 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 0;
		}
		break;
	case 615:
#line 3567 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 3582 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.ip = act_string( yypvt[-0].s );
		}
		break;
	case 617:
#line 3586 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.ip = act_string( yypvt[-0].s );
		}
		break;
	case 618:
#line 3590 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 3603 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 620:
#line 3607 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 621:
#line 3618 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 3659 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 3678 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 3692 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 3708 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 0;
		}
		break;
	case 626:
#line 3712 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 1;
		}
		break;
	case 629:
#line 3720 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    /* It will check for errors */
		    act_args( TRUE, yypvt[-2].s, yypvt[-0].s, gr->gr_type, gr->gr_sym );
		}
		break;
	case 630:
#line 3726 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 631:
#line 3730 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
	    	}
		break;
	case 632:
#line 3734 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
	    	}
		break;
	case 633:
#line 3738 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
	    	}
		break;
	case 634:
#line 3743 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = NULL;
	    	}
		break;
	case 635:
#line 3749 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_INT;
		    gr->gr_sym = NULL;
	    	}
		break;
	case 636:
#line 3755 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 3780 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_goto( G_TERM, L_FDEND, lbl_current(LBL_FORM) );
		}
		break;
	case 638:
#line 3785 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 3803 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_if_goto( IIFSETIO, C_EQ, 0, L_FDEND,
			lbl_current(LBL_FORM) );
		    gr->gr_func = IIRETFIELD;
		}
		break;
	case 642:
#line 3819 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 3840 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 3860 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    gen_call( IIENDFORMS );
		}
		break;
	case 645:
#line 3890 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    i4	cur_form = lbl_current( LBL_FORM );

		    gen_loop( G_CLOSE, L_FDBEG, L_FDEND, cur_form, IIFNAMES,
			C_0, 0 );
		    gen_label( G_LOOP, L_FDEND, cur_form );
		    gen_call( IIENDFRM );
		}
		break;
	case 646:
#line 3900 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 3913 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 648:
#line 3918 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if (yypvt[-0].i)
			er_write( E_EQ0077_grISUNSUPP, EQ_ERROR, 1,
							    ERx("FORMDATA") );
		}
		break;
	case 650:
#line 3934 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    gr->gr_func = IIFORMINIT;
		}
		break;
	case 651:
#line 3947 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( eq->eq_lang );
		    gen_call( IIFORMS );
		}
		break;
	case 652:
#line 3963 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		}
		break;
	case 653:
#line 3968 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_if( G_OPEN, IIFSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IIRETFIELD;
		}
		break;
	case 654:
#line 3974 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 655:
#line 3994 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1,
						    ERx("GETOPER") );
		}
		break;
	case 656:
#line 4001 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_if( G_OPEN, IIFSETIO, C_NOTEQ, 0 );
		}
		break;
	case 657:
#line 4006 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 661:
#line 4016 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
			gen_call( IIGETQRY );
		}
		break;
	case 662:
#line 4021 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 4048 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( 0 );		/* Dummy type right now */
		}
		break;
	case 665:
#line 4054 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 4065 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 667:
#line 4079 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( IIHELPFILE );
		}
		break;
	case 668:
#line 4084 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 669:
#line 4096 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( QAMESSAGE );
		}
		break;
	case 670:
#line 4101 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 671:
#line 4113 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( QAPROMPT );
		}
		break;
	case 672:
#line 4118 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 673:
#line 4138 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    frs_close();
		}
		break;
	case 675:
#line 4145 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    frs_head();
		}
		break;
	case 676:
#line 4150 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    frs_inqset( FRSinq );
		}
		break;
	case 677:
#line 4156 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 678:
#line 4160 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 679:
#line 4164 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 680:
#line 4168 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 683:
#line 4177 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    frs_parentname( yypvt[-0].s, T_CHAR, (PTR)0 );
		}
		break;
	case 684:
#line 4181 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    frs_parentname( yypvt[-0].s, T_INT, (PTR)0 );
		}
		break;
	case 685:
#line 4185 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 4211 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    er_write( E_EQ0077_grISUNSUPP, EQ_ERROR, 1,
						ERx("INQUIRE_FRS") );
		    YYERROR;
		}
		break;
	case 691:
#line 4223 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    frs_iqvar( gr->gr_id, gr->gr_type, gr->gr_sym,
				gr->gr_nlid, gr->gr_nlsym );
		}
		break;
	case 692:
#line 4229 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    frs_constant( yypvt[-0].s, (char *)0, T_NONE, (SYM *)0 );
		}
		break;
	case 693:
#line 4233 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    er_write( E_EQ0054_grNOPARENS, EQ_ERROR, 0 );
		    frs_constant( yypvt[-2].s, (char *)0, T_NONE, (SYM *)0 );
		}
		break;
	case 694:
#line 4238 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    /* Should be an error */
		    frs_constant( yypvt[-3].s, yypvt[-1].s, gr->gr_type, gr->gr_sym );
		}
		break;
	case 695:
#line 4245 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 696:
#line 4249 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 697:
#line 4253 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 698:
#line 4257 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 699:
#line 4261 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 700:
#line 4265 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 701:
#line 4269 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 702:
#line 4274 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 703:
#line 4280 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_INT;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 704:
#line 4286 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 705:
#line 4292 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 706:
#line 4298 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 707:
#line 4304 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 708:
#line 4310 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 709:
#line 4316 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 710:
#line 4322 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 711:
#line 4328 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 4401 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( IIMESSAGE ); 
    		}
		break;
	case 713:
#line 4406 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
            	    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
    	    	    gr->gr_func = IIMESSAGE;
    		}
		break;
	case 716:
#line 4429 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    EQFW3glgen();	
		}
		break;
	case 717:
#line 4434 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    EQFWopen(gr->gr_func);
		}
		break;
	case 721:
#line 4449 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    EQFWwcoption(yypvt[-2].s, yypvt[-0].s);
		}
		break;
	case 722:
#line 4454 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 723:
#line 4458 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 724:
#line 4463 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 725:
#line 4467 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 730:
#line 4492 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		EQFWsuboption(yypvt[-2].s, gr->gr_type, gr->gr_id, (PTR)gr->gr_sym);
	}
		break;
	case 731:
#line 4497 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
	    	    yyval.s = yypvt[-0].s;
	        }
		break;
	case 732:
#line 4501 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 733:
#line 4507 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_INT;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 734:
#line 4513 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_CHAR;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 735:
#line 4519 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_FLOAT;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 736:
#line 4525 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_PACKASCHAR;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 737:
#line 4531 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if (!gr->gr_sym)
		    {
			gr->gr_type = T_CHAR;
		    }	
		}
		break;
	case 738:
#line 4545 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( IIPRNSCR );
		}
		break;
	case 739:
#line 4550 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 740:
#line 4555 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    /* Should check that $2.s is "file" */
		    if (STbcompare(yypvt[-3].s, 0, ERx("file"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-3].s,
					ERx("PRINTSCREEN"), ERx("FILE") );
		}
		break;
	case 741:
#line 4562 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		}
		break;
	case 742:
#line 4567 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 743:
#line 4587 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( IINPROMPT );
		}
		break;
	case 744:
#line 4592 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    gr->gr_func = IINPROMPT;
		}
		break;
	case 745:
#line 4598 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if (STbcompare(yypvt[-0].s, 0, ERx("noecho"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
						ERx("PROMPT"), ERx("NOECHO") );
		    arg_int_add( 1 );
		}
		break;
	case 746:
#line 4605 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_int_add( 0 );
		}
		break;
	case 747:
#line 4610 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 4649 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		}
		break;
	case 749:
#line 4654 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_if( G_OPEN, IIFSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IISETFIELD;
		}
		break;
	case 750:
#line 4660 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 751:
#line 4672 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    gen_call( IIREDISP );
		}
		break;
	case 752:
#line 4693 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 4706 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 755:
#line 4713 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 4738 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( IIRESFLD );
		}
		break;
	case 757:
#line 4742 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( IIRESCOL );
		}
		break;
	case 758:
#line 4746 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 4812 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 760:
#line 4827 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    frs_close();
		}
		break;
	case 762:
#line 4834 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    frs_inqset( FRSset );
		}
		break;
	case 764:
#line 4848 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    er_write( E_EQ0077_grISUNSUPP, EQ_ERROR, 1,
						    ERx("SET_FRS") );
		    YYERROR;
		}
		break;
	case 768:
#line 4860 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    frs_setval( form_sconst(yypvt[-0].s), T_CHAR, (SYM *)0,
				(char *)0, (SYM *)0 );
		}
		break;
	case 769:
#line 4865 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    frs_setval( yypvt[-0].s, T_INT, (SYM *)0, (char *)0, (SYM *)0 );
		}
		break;
	case 770:
#line 4869 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    frs_setval(ERx("0"), T_INT, (SYM *)0, (char *)0, (SYM *)0);
		}
		break;
	case 771:
#line 4874 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    frs_setval(ERx("0"), T_INT, (SYM *)0, (char *)0, (SYM *)0);
		}
		break;
	case 772:
#line 4879 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 4911 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    frs_setval( ERx("1"), T_INT, (SYM *)0, (char *)0,
				(SYM *)0 );
		}
		break;
	case 774:
#line 4938 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( IISLEEP );
		}
		break;
	case 775:
#line 4943 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 776:
#line 4976 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 4992 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 5008 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_else( G_ELSE, IIRETVAL, C_0, 0, L_ACTELSE, 
			lbl_current(LBL_INTR), L_ACTEND, 
			lbl_current(LBL_FORM) );
		    gr->gr_mode = LBLmSUBMENU;
		}
		break;
	case 781:
#line 5016 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_mode = LBLmSUBMENU;	/* Not LBLmDISPLAY */
		}
		break;
	case 782:
#line 5020 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_mode = LBLmSUBMENU;	/* Not LBLmDISPLAY */
		}
		break;
	case 783:
#line 5025 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    act_add( ACT_MENU, lbl_current(LBL_INTR), 0, yypvt[-1].ip );
		}
		break;
	case 785:
#line 5043 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 5057 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 787:
#line 5062 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_func = IICHKFRM;
		}
		break;
	case 788:
#line 5066 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_func = IIVALFLD;
		}
		break;
	case 789:
#line 5088 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 790:
#line 5094 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IITCLRCOL;
		}
		break;
	case 791:
#line 5100 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( cmCLEARR );
		}
		break;
	case 793:
#line 5107 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( IITCLRROW );
		}
		break;
	case 794:
#line 5111 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( IITCLRROW );
		}
		break;
	case 795:
#line 5132 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    /* Close inner and deleterow if blocks */
		    gen_if( G_CLOSE, IITDELROW, C_0, 0 );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 796:
#line 5139 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLRET;	/* May be an Out list */
		}
		break;
	case 797:
#line 5145 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( cmDELETER );
		}
		break;
	case 799:
#line 5152 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if (eq->eq_flags & EQ_VERSWARN)
			er_write( E_EQ0078_grSTYUNSUPP, EQ_WARN, 1,
						    ERx("DELETEROW ... OUT") );
		}
		break;
	case 800:
#line 5159 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_int_add( FALSE );
		    gen_if( G_OPEN, IITDELROW, C_NOTEQ, 0 );
		}
		break;
	case 801:
#line 5164 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if (eq->eq_flags & EQ_VERSWARN)
			er_write( E_EQ0078_grSTYUNSUPP, EQ_WARN, 1,
						    ERx("DELETEROW ... IN") );
		}
		break;
	case 802:
#line 5171 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_int_add( TRUE );	/* In list */
		    gen_if( G_OPEN, IITDELROW, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 803:
#line 5189 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 804:
#line 5195 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLRET;
		}
		break;
	case 805:
#line 5201 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( cmGETR );
		}
		break;
	case 806:
#line 5219 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( IITFILL );
		    gen_if( G_CLOSE, IITBINIT, C_0, 0 );
		}
		break;
	case 807:
#line 5225 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_if( G_OPEN, IITBINIT, C_NOTEQ, 0 );
		}
		break;
	case 808:
#line 5230 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 812:
#line 5239 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_call( IITHIDECOL );
		}
		break;
	case 816:
#line 5256 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{ gr->gr_sym = (SYM *)0; }
		break;
	case 817:
#line 5258 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 5273 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 0;		/* Paren not stuff present */
		}
		break;
	case 820:
#line 5277 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    id_add( yypvt[-2].s );
		    id_add( yypvt[-1].s );
		    id_add( yypvt[-0].s );
		    yyval.i = 1;		/* Paren stuff present */
		}
		break;
	case 821:
#line 5284 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 5294 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    id_add( form_sconst(yypvt[-0].s) );
		}
		break;
	case 823:
#line 5298 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 5320 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 0;		/* No NULL clause */
		}
		break;
	case 825:
#line 5324 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    id_add( ERx(" with null") );
		    yyval.i = 1;		/* With NULL clause */
		}
		break;
	case 826:
#line 5329 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if (yypvt[-0].i == 0)
			id_add( ERx(" not null") );
		    else
			id_add( ERx(" not null with default") );
		    yyval.i = 1;		/* With NULL clause */
		}
		break;
	case 827:
#line 5338 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if (STbcompare(yypvt[-1].s, 0, ERx("not"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
						ERx("INITTABLE"), ERx("NOT") );
		}
		break;
	case 828:
#line 5345 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 1;
		    if (STbcompare(yypvt[-0].s, 0, ERx("default"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					    ERx("INITTABLE"), ERx("DEFAULT") );
		}
		break;
	case 829:
#line 5352 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 0;
		}
		break;
	case 830:
#line 5367 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 5379 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmPURGE );
		}
		break;
	case 832:
#line 5402 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    /* Close inner and insertrow if blocks */
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITINSERT, C_0, 0 );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 833:
#line 5410 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gen_if( G_OPEN, IITINSERT, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 834:
#line 5417 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( cmINSERTR );
		}
		break;
	case 835:
#line 5440 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBACT, C_0, 0 );
		}
		break;
	case 836:
#line 5446 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_int_add( TRUE );		/* Loading */
		    gen_if( G_OPEN, IITBACT, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 837:
#line 5453 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 840:
#line 5461 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    frs_inqset(FRSload);
		    frs_object(ERx("row"));
		}
		break;
	case 841:
#line 5478 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 842:
#line 5484 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 843:
#line 5490 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( cmPUTR );
		}
		break;
	case 844:
#line 5516 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    /* Close inner and scroll if blocks */
		    gen_if( G_CLOSE, IITSCROLL, C_0, 0 );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 845:
#line 5523 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_int_add( rowNONE );
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		}
		break;
	case 846:
#line 5529 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( cmSCROLL );
		}
		break;
	case 847:
#line 5535 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_if( G_OPEN, IITSCROLL, C_NOTEQ, 0 );
		}
		break;
	case 848:
#line 5539 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if ((yypvt[-1].i || yypvt[-0].i) && (eq->eq_flags & EQ_VERSWARN))
			er_write( E_EQ0078_grSTYUNSUPP, EQ_WARN, 1,
							ERx("SCROLL IN/OUT") );
		}
		break;
	case 849:
#line 5546 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gen_call( IITBSMODE );
		    arg_int_add( FALSE );	/* Ignore In flag */
		}
		break;
	case 851:
#line 5554 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_int_add( rowEND );
		}
		break;
	case 852:
#line 5559 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gen_call( IITBSMODE );
		}
		break;
	case 853:
#line 5564 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gen_call( IITBSMODE );
		}
		break;
	case 854:
#line 5570 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 0;
		}
		break;
	case 855:
#line 5574 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 1;
		}
		break;
	case 856:
#line 5579 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_func = IITCOLRET;
		}
		break;
	case 857:
#line 5584 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_int_add( FALSE );
		    arg_int_add( rowNONE );
		    gen_if( G_OPEN, IITSCROLL, C_NOTEQ, 0 );
		    yyval.i = 0;
		}
		break;
	case 858:
#line 5591 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 1;
		}
		break;
	case 859:
#line 5596 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_int_add( TRUE );	/* In list */
		    arg_int_add( rowNONE );
		    gen_if( G_OPEN, IITSCROLL, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 860:
#line 5618 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 5629 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 5659 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 5670 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( IITBCECOLEND );
		}
		break;
	case 864:
#line 5675 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 5690 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 866:
#line 5711 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 5728 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{	
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLVAL;
		    yyval.s = yypvt[-3].s;
		}
		break;
	case 868:
#line 5735 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( cmVALIDR );
		}
		break;
	case 870:
#line 5742 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( IITVALROW );
		}
		break;
	case 871:
#line 5746 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( IITVALROW );
		}
		break;
	case 872:
#line 5758 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = form_sconst(yypvt[-0].s);
		}
		break;
	case 873:
#line 5766 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 874:
#line 5770 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 5793 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 877:
#line 5802 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 5821 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 879:
#line 5825 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 882:
#line 5842 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 883:
#line 5846 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 5863 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 885:
#line 5867 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 5884 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 887:
#line 5888 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 5905 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_str_add( ARG_INT, yypvt[-0].s );
		}
		break;
	case 889:
#line 5909 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    arg_int_add( 0 );
		}
		break;
	case 890:
#line 5914 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    arg_int_add( 0 );
		}
		break;
	case 891:
#line 5919 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 5952 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 900:
#line 5956 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 5983 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 902:
#line 5991 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 6013 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_int_add( 0 );		/* No null indicator */
		    arg_str_add( ARG_INT, yypvt[-0].s );
		    gr->gr_type = T_INT;
		}
		break;
	case 904:
#line 6019 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_int_add( 0 );		/* No null indicator */
		    arg_str_add( ARG_FLOAT, yypvt[-0].s );
		    gr->gr_type = T_FLOAT;
		}
		break;
	case 905:
#line 6025 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_int_add( 0 );		/* No null indicator */
		    arg_str_add( ARG_PACK, yypvt[-0].s );
		    gr->gr_type = T_PACKASCHAR;
		}
		break;
	case 906:
#line 6031 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_int_add( 0 );		/* No null indicator */
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gr->gr_type = T_CHAR;
		}
		break;
	case 907:
#line 6046 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{	
		    arg_int_add( 0 );
		    arg_var_add( gr->gr_null, ERx("null") ); 
		}
		break;
	case 908:
#line 6062 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 1;
		}
		break;
	case 909:
#line 6066 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 1;
		}
		break;
	case 910:
#line 6070 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 0;
		}
		break;
	case 915:
#line 6081 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 916:
#line 6085 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 6106 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
#line 6119 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_int_add( rowCURR );
		}
		break;
	case 921:
#line 6127 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    if (lbl_gen(LBLmDISPLAY, FALSE) == LBL_NOLEV)
			er_write( E_EQ0056_grFORMNAME, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 922:
#line 6136 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    er_write( E_EQ0071_grASSGNOP, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 927:
#line 6178 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_flag &= ~GR_HOSTCODE;
		}
		break;
	case 928:
#line 6202 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( IIPARSET );
		}
		break;
	case 929:
#line 6207 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_call( IIPARRET );
		}
		break;
	case 930:
#line 6212 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_send();
		    sc_eat(id_add,SC_STRIP|SC_NEST|SC_SEEN, ERx(")"), '(', ')');
		    arg_str_add( ARG_RAW, id_getname() );
		    id_free();
		    arg_str_add( ARG_RAW, ERx("(int *)0") );
		}
		break;
	case 931:
#line 6228 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if (yypvt[-0].i)
			er_write( E_EQ0055_grFMPARAM, EQ_ERROR, 0 );
		    gen_call( gr->gr_func );
		}
		break;
	case 932:
#line 6235 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if (yypvt[-0].i)
			er_write( E_EQ0055_grFMPARAM, EQ_ERROR, 0 );
		    gen_call( gr->gr_func );
		}
		break;
	case 933:
#line 6242 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
		    sc_eat(id_add,SC_STRIP|SC_NEST|SC_SEEN, ERx(")"), '(', ')');
		    arg_str_add( ARG_RAW, id_getname() );
		    id_free();
		    arg_str_add( ARG_RAW, ERx("(int *)0") );
		}
		break;
	case 934:
#line 6286 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    sc_eat(id_add,SC_STRIP|SC_NEST|SC_SEEN, ERx(")"), '(', ')');
		    arg_str_add( ARG_RAW, id_getname() );
		    id_free();
		    gen_call( IIMESSAGE );
		}
		break;
	case 935:
#line 6314 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 936:
#line 6319 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 937:
#line 6324 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 938:
#line 6329 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    id_add( yypvt[-0].s );
		}
		break;
	case 947:
#line 6363 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		    db_key( yypvt[-0].s );
		}
		break;
	case 949:
#line 6374 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_BLOCK, GR_BLKTRUE );
		    /* On C block entry - expect declares */
		    gr->C_dec = GR_C_DEC;
		}
		break;
	case 950:
#line 6381 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 952:
#line 6387 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr_mechanism( GR_BLOCK, GR_BLKFALSE );
		    /* On C block exit - expect declares */
		    gr->C_dec = GR_C_DEC;
		}
		break;
	case 953:
#line 6399 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if ((gr->gr_flag & GR_HOSTCODE) == 0)
		    {
			gen_line( ERx("host code") );
			gr->gr_flag |= GR_HOSTCODE;
		    }
		    gen_host( G_H_CODE, yypvt[-0].s );
		    gr->C_dec = GR_C_DEC;	/* Maybe dec's to follow */
		}
		break;
	case 955:
#line 6414 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->C_rec = 0;
		    gr->C_size = 0;
		    gr->C_indir = 0;
		    gr->C_dec = GR_C_DEC;	/* Maybe dec's to follow */
		    gr->gr_type = T_INT;
		    gr->C_storage = T_NONE;
		    gr->C_struct = (SYM *)0;
		    gr->C_varchar = 0;
		    gr->C_cvals = (C_VALS *)0;
		}
		break;
	case 961:
#line 6449 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->C_dec = GR_C_DEC;	/* Dec's to follow */
		    gr->gr_type = T_INT;
		    gr->C_size = sizeof(i4);
		}
		break;
	case 962:
#line 6455 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->C_dec = GR_C_DEC;	/* Dec's to follow */
		    gr->gr_type = T_INT;
		    gr->C_size = sizeof(i4);
		    gr->C_varchar = 1;
		}
		break;
	case 963:
#line 6462 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->C_storage = T_TYPEDEF;
		    gr->C_dec = GR_C_DEC;	/* Dec's to follow */
		    gr->gr_type = T_INT;
		    gr->C_size = sizeof(i4);
		}
		break;
	case 964:
#line 6470 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );  
		    gr->C_dec = GR_C_DEC;	/* Dec's to follow */
		    gr->gr_type = T_INT;
		    gr->C_size = sizeof(i4);
		    gr->C_storage = T_EXTERN;
		}
		break;
	case 965:
#line 6478 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );  
		    gr->C_dec = GR_C_DEC;	/* Dec's to follow */
		    gr->gr_type = T_INT;
		    gr->C_size = sizeof(i4);
		    gr->C_storage = T_EXTERN;
		    gr->C_varchar = 1;
		}
		break;
	case 966:
#line 6487 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->C_varchar = 1;
		}
		break;
	case 967:
#line 6499 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    /* Verify -c flag for GLOBALREF and GLOBALDEF */
		    if (eq->eq_flags & EQ_COMPATLIB)
		    {
			if (STbcompare(yypvt[-0].s, 0, ERx("global"), 6, TRUE) == 0)
			{
			    char	tbuf[80];

			    /*
			    **  Must copy the string before we can change it
			    **  since $1.s is a string constant and is not
			    **  updateable (found on alpha).
			    */
			    STcopy(yypvt[-0].s, tbuf);
			    CVupper(tbuf);
			    gen_host(G_H_KEY, tbuf);
			}
			else
			{
			    gen_host(G_H_KEY, yypvt[-0].s);
			}
		    }
		    else
			gen_host(G_H_KEY, yypvt[-0].s);  
		}
		break;
	case 968:
#line 6527 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );  
		    gr->C_dec = GR_C_DEC;	/* Dec's to follow */
		    gr->gr_type = T_INT;
		    gr->C_size = sizeof(i4);
		}
		break;
	case 978:
#line 6551 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    SYM		*sy;

		    sy = sym_resolve( (SYM *)0, yypvt[-0].s, C_CLOSURE, syFisTYPE );
		    if (sy)
		    {
			gr->C_size = sym_g_dsize( sy );
			gr->C_indir = sym_g_indir( sy );
			gr->gr_type = sym_g_btype( sy );
			/*
			** May need to save value info for cases like
			** typdef char BUF[10];
			** ## BUF	b;  - should have dimension 10.
			*/
			gr->C_cvals = (C_VALS *)sym_g_vlue(sy);

			/*
			** This "if" is made use of only in Cdec_var
			** where we set it only for typedefs of structs.
			** We use it there only, so we know to set C_struct here
			** so that when we get into Cdec_var for the Cdecl_list
			** of this declaration we'll have a symbol table type
			** entry to point to for the declared variable.
			*/
			if (gr->gr_type == T_STRUCT)
			    gr->C_struct = sy;
		    }
		    gr->C_dec = GR_C_DEC;	/* Dec's to follow */
		}
		break;
	case 979:
#line 6582 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 980:
#line 6587 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 981:
#line 6592 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 982:
#line 6597 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 983:
#line 6602 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = ERx("int");
		}
		break;
	case 984:
#line 6607 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 985:
#line 6613 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 986:
#line 6619 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 987:
#line 6625 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 988:
#line 6631 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-1].s;
		}
		break;
	case 989:
#line 6637 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-1].s;
		}
		break;
	case 990:
#line 6643 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 991:
#line 6648 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 992:
#line 6653 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = ERx("double");
		}
		break;
	case 993:
#line 6659 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = ERx("i8");
		}
		break;
	case 994:
#line 6665 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-2].s );
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = ERx("u_i8");
		}
		break;
	case 995:
#line 6672 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 996:
#line 6709 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->C_size = sizeof(i4);
		    gr->C_indir = 0;
		    gr->gr_type = T_INT;
		}
		break;
	case 997:
#line 6716 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->C_size = sizeof(i4);
		    gr->C_indir = 0;
		    gr->gr_type = T_INT;
		}
		break;
	case 998:
#line 6723 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->C_dec = GR_C_TAG;	/* Get ready for enum tag */
		    gen_host( G_H_KEY, yypvt[-0].s );
		    if (gr->C_varchar == 1)
		    {
			er_write( E_E00008_hcVCHIGNRE, EQ_ERROR, 0 );
			gr->C_varchar = 0;
		    }
		}
		break;
	case 999:
#line 6734 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    SYM		*sy;

		    gen_host( G_H_KEY, yypvt[-0].s );
		    /* Declare the tag and mark as integer */
		    sy = symDcEuc( yypvt[-0].s, gr->C_rec, gr->C_blk, syFisTAG,
			             C_CLOSURE, SY_TAG );
		    if (sy)
			sym_s_btype( sy, T_INT );
		    gr->C_dec = GR_C_DEC;
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
#line 6746 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    /*
		    ** Probably will be redeclaration to compiler but
		    ** we will ignore it.
		    */
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->C_dec = GR_C_DEC;
		}
		break;
	case 1001:
#line 6755 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->C_dec = GR_C_DEC;
		}
		break;
	case 1002:
#line 6760 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->C_dec = GR_C_DEC;
		}
		break;
	case 1003:
#line 6766 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_NEWLINE|G_H_OUTDENT, (char *)0 );
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 1004:
#line 6772 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_OP|G_H_NEWLINE|G_H_INDENT, yypvt[-0].s );
		}
		break;
	case 1007:
#line 6780 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host(G_H_KEY, yypvt[-0].s);
		}
		break;
	case 1009:
#line 6786 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    SYM		*sy;

		    gen_host(G_H_KEY, yypvt[-0].s);
		    /* Not recorded as child of anything -- orphans */
		    if (sy = symDcEuc(yypvt[-0].s, 0, gr->C_blk,
				syFisORPHAN|syFisCONST, C_CLOSURE, SY_NORMAL))
		    {
			sym_s_btype( sy, T_INT );
			sym_s_dsize(sy, sizeof(i4));
		    }
		}
		break;
	case 1010:
#line 6800 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 1011:
#line 6829 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_type = T_STRUCT;
		}
		break;
	case 1012:
#line 6834 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_type = T_STRUCT;
		}
		break;
	case 1013:
#line 6839 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_type = T_STRUCT;
		    gr->C_struct = yypvt[-0].v;
		}
		break;
	case 1014:
#line 6845 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    /* Repeated structure tag - legal in C */
		    gr->gr_type = T_STRUCT;
		}
		break;
	case 1015:
#line 6851 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->C_dec = GR_C_TAG;		/* Get ready for tag */
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1016:
#line 6857 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->C_struct  = symDcEuc( yypvt[-0].s, gr->C_rec, gr->C_blk, 
			syFisTAG, C_CLOSURE, SY_TAG );
		    gr->C_dec = GR_C_DEC;
		}
		break;
	case 1017:
#line 6864 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->C_struct = symDcEuc( ERx(""), gr->C_rec, gr->C_blk, 
			syFisTAG, C_CLOSURE, SY_TAG );
		    gr->C_dec = GR_C_DEC;
		}
		break;
	case 1018:
#line 6871 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.v = sym_resolve((SYM *)0, yypvt[-0].s, C_CLOSURE, syFisTAG);
		    gr->C_dec = GR_C_DEC;
		}
		break;
	case 1019:
#line 6878 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->C_struct = symDcEuc( yypvt[-0].s, gr->C_rec, gr->C_blk, 
			syFisTAG, C_CLOSURE, SY_TAG );
		    gr->C_dec = GR_C_DEC;
		}
		break;
	case 1021:
#line 6889 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_OP|G_H_NEWLINE|G_H_INDENT, yypvt[-0].s );
		    gr->C_rec++;
		    /* 
		    ** Push the struct entry in case there are nested structs,
		    ** so that variables of this struct type can be set.
		    ** The struct pointer id popped on the right brace.
		    */
		    sym_fpush( gr->C_struct );
		    /*
		    ** Save info about outer vchar. 
		    ** Note that C_struct may be null if redecl.
		    */
		    if (gr->C_struct && gr->C_varchar == 1)
			sym_s_btype( gr->C_struct, T_VCH ); 
		    gr->C_varchar = 0;
		}
		break;
	case 1022:
#line 6908 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->C_rec--;
		    gr->C_struct = (SYM *) sym_fpop();
		    gen_host( G_H_KEY|G_H_OUTDENT, yypvt[-0].s );
		    if (gr->C_struct)	/* Could be null if redecl */
		    {
			if (sym_g_btype(gr->C_struct) == T_VCH)
			{
			    gr->C_varchar = 1;
			    sym_s_btype( gr->C_struct, T_STRUCT ); 
			}
			else
			    gr->C_varchar = 0;    /* Reset nested varchars */
		    }
		}
		break;
	case 1025:
#line 6936 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_type = T_NONE;
		    gr->C_struct = (SYM *)0;
		    gr->C_size = 0;
		    gr->C_indir = 0;
		    gr->C_varchar = 0;
		}
		break;
	case 1027:
#line 6945 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_OP|G_H_NEWLINE, ERx("...DECLARE ERROR;") );
		    gr->gr_type = T_NONE;
		    gr->C_struct = (SYM *)0;
		    gr->C_size = 0;
		    gr->C_indir = 0;
		    gr->C_varchar = 0;
		}
		break;
	case 1040:
#line 6968 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->C_varchar = 1;
		}
		break;
	case 1041:
#line 6973 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    /*
		    ** Varchar storage class ONLY allowed with variable decls.
		    ** Disallow with structure tags (and no vars) but give
		    ** error.
		    */
		    if (gr->C_varchar == 1)
		    {
			er_write( E_E00008_hcVCHIGNRE, EQ_ERROR, 0 );
			gr->C_varchar = 0;
		    }
		}
		break;
	case 1043:
#line 6988 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1046:
#line 6996 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    /* Now strip off any C initialization clause */
		    sc_eat( gen_code, SC_NEST|SC_BACK, ERx(";,"), '{', '}' );
		}
		break;
	case 1047:
#line 7002 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    /* Now strip off any C bitfield clause */
		    sc_eat( gen_code, SC_NEST|SC_BACK, ERx(";,"), '(', ')' );
		}
		break;
	case 1048:
#line 7008 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if ((eq->eq_flags & EQ_COMPATLIB) &&
			    STbcompare(yypvt[-0].s, 0, ERx("ZERO_FILL"), 0, 0)==0)
			;
		    else
			er_write( E_E00009_hcMISSEQUAL, EQ_ERROR, 0 );
		}
		break;
	case 1050:
#line 7018 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    /* Enter declarations into Equel symbol table */
		    SYM		*sy;
		    C_VALS	*cv;
		    i4		cindir;
		    i4		type_bit;
		    i4		gr_type = gr->gr_type;
		    i4		csize = gr->C_size;

		    cindir = yypvt[-2].i + yypvt[-0].i + gr->C_indir;

		    /*
		    ** If there is already a value field from a type like:
		    **	 typedef char BUF[10]; BUF b;
		    ** then we have the value field in C_cvals.  Otherwise
		    ** build one from the current type. Find ONLY the
		    ** dimensions - should not include the ptrs, so as not
		    ** to err on decls like:
		    **	  char	*buf[10];
		    */
		    if ((cv = gr->C_cvals) == NULL && gr_type == T_CHAR &&
			cindir == yypvt[-0].i && cindir > 0)
		    {
			char	*cpopen =  STrchr(id_getname(), '[');
			char	*cpclose = STrchr(cpopen, ']');
			i4	len;

			if (cpopen && cpclose)
			{
			    /* Does not check embedded comments */
			    CMnext(cpopen);	/* Skip first '[' */
			    *cpclose = '\0';	/* Toss last ']' */

			    for (; CMwhite(cpopen); CMnext(cpopen))
				;
			    if (len = STtrmwhite(cpopen))
			    {
				/* +2 for cv_flags and null byte */
				cv = (C_VALS *)str_space(gr->C_strvals, len+2);
				cv->cv_flags = CvalDIMS;
				STcopy(cpopen, cv->cv_dims);
			    }
			}
		    }
		    id_free();	/* In case of Cdec_arrlist */

		    /*
		    ** Chars are pointers to buffers or they are one byte.
		    ** Don't decrement indirection on outermost level of
		    ** typedefs -- we'll do the decrement when we actually
		    ** declare such a variable.  For example:
		    ** We want to leave indirection at 2 for 's2'
		    ** 		typedef char *s2; 
		    ** so that 's' will get its indirection decremented in
		    ** 		s2   s;
		    */
		    if (gr_type == T_CHAR &&
			(gr->C_storage != T_TYPEDEF || gr->C_rec != 0))
		    {
			if (cindir)
			    cindir--;
			else
			{
			    gr_type = T_INT;
			    csize = 1;
			}
		    }
		    if (gr->C_storage==T_TYPEDEF && gr->C_rec==0)
			type_bit = syFisTYPE;
		    else {
			type_bit = syFisVAR;
		      /* member names can't be external! */
			if (gr->C_storage==T_EXTERN && gr->C_rec==0)
			    type_bit |= syFisFORWARD;
		    }

		    if (sy = symDcEuc(yypvt[-1].s, gr->C_rec, gr->C_blk, type_bit, 
				       C_CLOSURE, SY_NORMAL))
		    {
			sym_s_indir( sy, cindir ); 
			sym_s_btype( sy, gr_type ); 
			sym_s_dsize( sy, csize );
			sym_s_vlue( sy, cv );
			if (gr_type == T_STRUCT && gr->C_struct)
			{ 
			   /* Assign type entry for structure variables */
			    sym_type_euc( sy, gr->C_struct );
			    /*
			    ** See the comment in Ctype_dec about why we set
			    ** the btype to T_STRUCT here.  Note that this
			    ** test is "if this is a typedef".
			    */
			    if (gr->C_storage == T_TYPEDEF)
				sym_s_btype( sy, T_STRUCT );
			    if (gr->C_varchar == 1) 
			    { 
				SYM	*sib1, *sib2; 

				if (sib1 = syStrInit(gr->C_struct))
				{
				    if (!(sym_g_btype(sib1) == T_INT) ||
					!(sym_g_dsize(sib1) == 2))
				    {
					er_write(E_E00008_hcVCHIGNRE, 
						EQ_ERROR, 0);
				    }
				    else
				    {
					if (sib2 = syStrNext(sib1))
					{
					    /* 
					    ** Next entry must be single
					    ** dimensioned variable (ie,
					    ** indirection = 0).  Set the
					    ** value field.
					    */
					    if (sym_g_btype(sib2) == T_CHAR &&
						sym_g_indir(sib2) == 0 &&
						sym_g_vlue(sib2))
					    {
						sym_s_btype( sy, T_VCH );
						sym_s_vlue(sy,sym_g_vlue(sib2));
						sym_type_euc( sy, NULL );
					    }
					    else
						er_write( E_E00008_hcVCHIGNRE, 
							EQ_ERROR, 0 );
					}
					else		/* No 2nd child */
					{
					    er_write( E_E00008_hcVCHIGNRE, 
						    EQ_ERROR, 0 );
					}
				    }
				}
				else		/* No child */
				{
				    er_write(E_E00008_hcVCHIGNRE, EQ_ERROR, 0);
				}
			    }
			}
			else if (gr->C_varchar == 1)	/* Vchr must be strct */
			    er_write( E_E00008_hcVCHIGNRE, EQ_ERROR, 0 );
		    }
		}
		break;
	case 1051:
#line 7165 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1052:
#line 7171 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if (yypvt[-0].i)
		    {
			gen_host( G_H_OP, id_getname() );
			id_free();
		    }
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1053:
#line 7181 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 0;
		}
		break;
	case 1054:
#line 7185 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = yypvt[-1].i + yypvt[-0].i;
		}
		break;
	case 1055:
#line 7190 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 1;
		    id_add( yypvt[-0].s );
		}
		break;
	case 1056:
#line 7195 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 2;
		    id_add( yypvt[-0].s );
		}
		break;
	case 1057:
#line 7201 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    if (yypvt[-0].i)
			gen_host( G_H_OP, id_getname() );
		    /*
		    ** If the array list eater changes then change the
		    ** cv_dims stuff in the Cdec_var rule.
		    */
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1058:
#line 7212 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.i = 0;
		}
		break;
	case 1059:
#line 7216 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		  /* Eat up the array dimension and save it - don't nest them */
		    sc_eat( id_add, SC_SEEN|SC_NEST, ERx("]"), '[', ']' );
		    yyval.i = yypvt[-1].i +1;
		}
		break;
	case 1060:
#line 7223 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    SYM		*sy;

		    if (gr->gr_type == T_CHAR)
			gen_host( G_H_SCONST|G_H_NEWLINE, yypvt[-0].s );
		    else
			gen_host( G_H_KEY|G_H_NEWLINE, yypvt[-0].s );

		    if (gr->gr_type != T_NONE)
		    {
			if (sy = symDcEuc(yypvt[-2].s, 0, gr->C_blk, syFisCONST, 
			                   C_CLOSURE, SY_NORMAL))
			{
			    sym_s_btype( sy, gr->gr_type );
			    sym_s_dsize( sy, gr->C_size );
			}
		    } 
		    else
			er_write( E_E00001_hcDEFINE, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 1061:
#line 7245 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_OP, ERx("#") );
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1062:
#line 7253 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->C_dec = GR_C_DEC;	/* Dec's to follow */
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1063:
#line 7259 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    sc_eat( NULL, SC_SEEN|SC_NEST, ERx(")"), '(', ')' );
		    er_write( E_E00002_hcIGNCAST, EQ_ERROR, 0 );
		}
		break;
	case 1065:
#line 7266 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1066:
#line 7270 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1067:
#line 7275 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_type = T_INT;
		    gr->C_size = sizeof(i4);
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1068:
#line 7281 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    char	ibuf[50];
		    char	*s_int = yypvt[-0].s;

		    if (*(yypvt[-1].s) == '-')
		    {
			ibuf[0] = '-';
			ibuf[1] = '\0';
			s_int = str_add( STRNULL, STcat(ibuf, s_int) );
		    }
		    gr->gr_type = T_INT;
		    gr->C_size = sizeof(i4);
		    yyval.s = s_int;
		}
		break;
	case 1069:
#line 7297 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_type = T_FLOAT;
		    gr->C_size = sizeof(f8);
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1070:
#line 7303 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    char	fbuf[50];
		    char	*s_flt = yypvt[-0].s;

		    if (*(yypvt[-1].s) == '-')
		    {
			fbuf[0] = '-';
			fbuf[1] = '\0';
			s_flt = str_add( STRNULL, STcat(fbuf, s_flt) );
		    }
		    gr->gr_type = T_FLOAT;
		    gr->C_size = sizeof(f8);
		    yyval.s = s_flt;
		}
		break;
	case 1071:
#line 7318 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_type = T_FLOAT;
		    gr->C_size = sizeof(f8);
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1072:
#line 7324 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    char	dbuf[50];
		    char	*s_dec = yypvt[-0].s;

		    if (*(yypvt[-1].s) == '-')
		    {
			dbuf[0] = '-';
			dbuf[1] = '\0';
			s_dec = str_add( STRNULL, STcat(dbuf, s_dec) );
		    }
		    gr->gr_type = T_FLOAT;
		    gr->C_size = sizeof(f8);
		    yyval.s = s_dec;
		}
		break;
	case 1073:
#line 7339 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_type = T_CHAR;
		    gr->C_size = STlength( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1074:
#line 7345 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_type = T_NONE;
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1075:
#line 7350 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gr->gr_type = T_NONE;
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1078:
#line 7363 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    char	ch;

		    gen_host( G_H_NEWLINE, (char *)0 );
		    gen_host( G_H_OP, yypvt[-1].s );
		    sc_eat( gen_code, SC_SEEN|SC_NEST, ERx(")"), '(', ')' );
		    if (*SC_PTR == ';')
		    {
			SC_PTR++;	/* swallow the semicolon */
			gen_host( G_H_OP|G_H_NEWLINE, ERx(";") );
			er_write( E_E00006_hcNOFUNCDEC, EQ_ERROR, 1, yypvt[-1].s );
		    } else
		    {
			gen_host( G_H_NEWLINE, (char *)0 );
			if (gr->C_blk > 1)
			{
			    er_write(E_E00007_hcNESTFUNCDEF, EQ_ERROR, 1, yypvt[-1].s);
			} else {
			    gr->C_blk++;
			    sym_s_begin( C_CLOSURE );	/* Closure */
			    gr->C_isfunc = 1;
			}
		    }
		}
		break;
	case 1079:
#line 7392 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    SYM		*sy;

		    if ((sy = yypvt[-0].v) == (SYM *)0 )	/* Enter undefined */
		    {
			er_write(E_EQ0057_grHOSTVAR, EQ_ERROR, 1, id_getname());
			sy = symDcEuc(id_getname(), 0, gr->C_blk, syFisVAR, 
			                   C_CLOSURE, SY_NORMAL);
			if (sy)
			    sym_s_btype( sy, T_UNDEF );
		    }
		    else if  (!(sym_g_useof(sy) & syFisVAR) || 
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
	case 1080:
#line 7418 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
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
	case 1081:
#line 7432 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    i4		cindir;

		    cindir = yypvt[-2].i + yypvt[-0].i;
		    if (cindir > 0 && !yypvt[-1].v)	
		    {	
			/* Indirection but no var */
			er_write( E_EQ0502_hNOVAR, EQ_ERROR, 2, ERx("C"), 
				id_getname() );
		    }
		    else if (yypvt[-1].v && sym_g_indir(yypvt[-1].v) != cindir && 
			     (gr->gr_flag & GR_NOINDIR) == 0) 
		    {
			/* Illegal indirection */
			er_write( E_E00003_hcINDIR, EQ_ERROR, 3, id_getname(), 
				  er_na(cindir), er_na(sym_g_indir(yypvt[-1].v)) );
		    }
		    yyval.v = yypvt[-1].v;
		}
		break;
	case 1082:
#line 7452 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    id_add( yypvt[-0].s );
		    yyval.v = (SYM *)0;
		}
		break;
	case 1083:
#line 7457 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    /*
		    ** Function support - allow usages such as:
		    ** 	## message ERget(QBF_FETCHING, 1234)
		    ** or:
		    ** 	## activate menuitem ERx(QBF_SAVE)
		    ** Behave like system function "variables".
		    */
		    /* Eat up func args and allow nesting */
		    id_add(yypvt[-1].s);
		    sc_eat(id_add, SC_SEEN|SC_NEST, ERx(")"), '(', ')' );
	    	    yyval.v = sym_resolve((SYM *)0, yypvt[-1].s, C_CLOSURE, syFisVAR);
		}
		break;
	case 1084:
#line 7472 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{ 
		    yyval.v = yypvt[-0].v;
		}
		break;
	case 1085:
#line 7476 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.v = yypvt[-0].v;
		}
		break;
	case 1086:
#line 7481 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    id_add( yypvt[-0].s );
	    	    yyval.v = sym_resolve((SYM *)0, yypvt[-0].s, C_CLOSURE, 
				       syFisVAR|syFisCONST);
		    gr->C_struct = (SYM *)0;
		}
		break;
	case 1087:
#line 7499 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    yyval.v = yypvt[-0].v;	    
		    gr->C_struct = (SYM *)0; 	/* For next struct usage */
		}
		break;
	case 1088:
#line 7505 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    SYM		*sy;

		    id_add( yypvt[-0].s );
		    /* Use C_struct as the parent pointer */
	    	    sy = sym_resolve( gr->C_struct, yypvt[-0].s, C_CLOSURE, 
					syFisVAR|syFisCONST );
		    if (!sy)
			er_write( E_E00004_hcSTRUCTREF, EQ_ERROR, 1, 
				id_getname() );
		    yyval.v = sy;
		}
		break;
	case 1093:
#line 7525 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    id_add( yypvt[-0].s );
		}
		break;
	case 1094:
#line 7530 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    id_add( yypvt[-0].s );
		}
		break;
	case 1096:
#line 7537 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    id_add( yypvt[-0].s );
		    /* Use C_struct as the parent pointer and set to new one */
	    	    gr->C_struct = sym_resolve( gr->C_struct, yypvt[-0].s, C_CLOSURE, 
					        syFisVAR|syFisCONST );
		}
		break;
	case 1097:
#line 7545 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    id_add( yypvt[-0].s );
		}
		break;
	case 1098:
#line 7549 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    id_add( yypvt[-0].s );
		}
		break;
	case 1099:
#line 7554 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/c/c.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		    gen_host( G_H_NEWLINE, (char *)0 );
		}
		break;
		}
	return 0; /* Success */

	}
