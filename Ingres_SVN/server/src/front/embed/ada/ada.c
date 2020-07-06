
#line 6 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#define	tSEMICOL		405
#define	tLBRACE		406
#define	tRBRACE		407
#define	tAACCESS		408
#define	tAARRAY		409
#define	tAARROW		410
#define	tAASSIGN		411
#define	tABEGIN		412
#define	tABODY		413
#define	tACASE		414
#define	tACONST		415
#define	tADECLARE		416
#define	tADECNAME		417
#define	tADELTA		418
#define	tADIGITS		419
#define	tADOTDOT		420
#define	tAFUNCTION		421
#define	tALIMITED		422
#define	tANEW		423
#define	tAOTHERS		424
#define	tAPACKAGE		425
#define	tAPIPE		426
#define	tAPRIVATE		427
#define	tAPROCEDURE		428
#define	tARECORD		429
#define	tAREC_VAR		430
#define	tARENAMES		431
#define	tARETURN		432
#define	tASEPARATE		433
#define	tASUBTYPE		434
#define	tATYPEDEF		435
#define	tAWHEN		436
#define	tAUSE		437
#define	tARUOP		438
#define	tBNDOP		439
#define	tEQOP		440
#define	tNEQOP		441
#define	tLOGUOP		442
#define	tLOGBOP		443
#define	tAGOP		444
#define	tMULOP		445
#define	tDIVOP		446
#define	tEXPOP		447
#define	tBINARYOP		448
#define	tUNARYOP		449

#line 338 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
/* %L locals - Include L locals and gr structure */
/* %L locals begin - Local variables and gr structure for ADA */
# include	<eqada.h>
# include	<ere6.h>

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

	/* Add L (ADA) dependent members after this statement */
	i4	A_type;		/* ADA base type (in declarations) */
	i4	A_blk;		/* ADA block level */
	i4	A_rec;		/* ADA record level */
	i4	A_size;		/* Size of declaration */
	i4	A_dims;		/* number of array dimensions */
	i4	A_var;		/* In declaration (AvarDEC) or use (AvarUSE) */
	i4	A_val;		/* Flags in st_value (really a i4  *) */
	i4	A_simpvar;	/* Is a simple variable */
	i4	A_err;		/* Var usage has an error */
	i4	A_with;		/* AwithFORMS or AwithEQUEL or AwithNONE */
	SYM	*A_record;	/* Special pointer for references to structs */
	SYM	*A_bytesy;	/* points to 1-byte integer entry (internal) */
	SYM	*A_shrtsy;	/* points to 2-byte integer entry (internal) */
	SYM	*A_undefsy;	/* points to "Undefined" entry (internal) */
	SYM	*A_intsy;	/* points to "integer" entry, for convenience */
	SYM	*A_fltsy;	/* points to "real" entry, for convenience */
	SYM	*A_dblsy;	/* points to "double" entry, for convenience */
	SYM	*A_chrsy;	/* points to "char" entry, for convenience */
	SYM	*A_strsy;	/* points to "string" entry, for convenience */
};
GLOBALDEF struct gr_state _gr_local ZERO_FILL;
GLOBALDEF struct gr_state *gr = &_gr_local;

/* Generate an error if no error is yet issued */
# define   ADA_ERR( x )				\
			if (!gr->A_err) {	\
			    gr->A_err = 1;	\
			    er_write x;		\
			} else			\

/* Declare variables that have been pushed on stack */
# define   ADA_DECLARE( count, typbits, typesy )		  \
		       ada_declare( count, gr->A_rec, gr->A_blk,  \
				    typbits, gr->A_dims, gr->A_val, typesy )
SYM	*ada_declare();

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

#line 7992 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"


/* %L mechanism */
/* %L mechanism begin - All the stuff to do with gr_mechanism() */

GLOBALDEF GR_ADTYPES   gr_typetab[] = {
  /* first 3 are fake; just for internal use */
 {ERx(".byte"),		T_INT,	sizeof(char),	EQ_ADALSYS|EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX},
 {ERx(".short"),	T_INT,	sizeof(short),	EQ_ADALSYS|EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX},
 {ERx(".undef"),	T_UNDEF, sizeof(int), 	EQ_ADALSYS|EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX},
  /* now the real ones */
 {ERx("character"),	T_CHAR,	0,   		EQ_ADALSYS|EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX},
 {ERx("string"),	T_CHAR,	0,   		EQ_ADALSYS|EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX},
  /* size of booleans is 1 byte on VMS */
 {ERx("boolean"),	T_INT,	sizeof(char),	EQ_ADALSYS|EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX},
#if defined(axp_osf)
 {ERx("short_short_integer"),T_INT,sizeof(char),EQ_ADVADS|EQ_ADGNAT          },
#else /* axp_osf */
 {ERx("tiny_integer"),	T_INT,	sizeof(char),	EQ_ADVADS|EQ_ADGNAT	     },
#endif /* axp_osf */
 {ERx("short_short_integer"),T_INT,	sizeof(char),		  EQ_ADVAX   },
 {ERx("short_integer"),	T_INT, 	sizeof(char),	EQ_ADALSYS		     },
 {ERx("short_integer"),	T_INT, 	sizeof(short),	EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX },
 {ERx("integer"), 	T_INT, 	sizeof(short),	EQ_ADALSYS		     },
 {ERx("integer"), 	T_INT, 	sizeof(int),	EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX },
#if defined(axp_osf)
 {ERx("long_integer"),  T_INT,  sizeof(long),   EQ_ADVADS|EQ_ADGNAT          },
#endif /* axp_osf */
 {ERx("long_integer"), 	T_INT, 	sizeof(int),	EQ_ADALSYS		     },
 {ERx("natural"), 	T_INT, 	sizeof(int),   	EQ_ADALSYS|EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX},
 {ERx("positive"), 	T_INT, 	sizeof(int),   	EQ_ADALSYS|EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX},
 {ERx("short_float"), 	T_FLOAT,sizeof(float),	EQ_ADVADS|EQ_ADGNAT	     },
 {ERx("float"), 	T_FLOAT,sizeof(float),	EQ_ADALSYS|EQ_ADVAX	     },
#if defined(axp_osf)
 {ERx("float"),         T_FLOAT,sizeof(float),  EQ_ADVADS|EQ_ADGNAT          },
 {ERx("long_float"),    T_FLOAT,sizeof(double), EQ_ADALSYS|EQ_ADVAX|EQ_ADVADS|EQ_ADGNAT},
 {ERx("long_long_float"),T_FLOAT,sizeof(double),EQ_ADVADS|EQ_ADGNAT          },
#else /* axp_osf */
 {ERx("float"),	 	T_FLOAT,sizeof(double),	EQ_ADVADS|EQ_ADGNAT  	     },
 {ERx("long_float"),	T_FLOAT,sizeof(double),	EQ_ADALSYS|EQ_ADVAX	     },
#endif /* axp_osf */
 {ERx("f_float"), 	T_FLOAT,sizeof(float), 	EQ_ADVAX		     },
 {ERx("d_float"), 	T_FLOAT,sizeof(double),	EQ_ADVAX		     },

  /* table separator */
 {(char *)0,  		0, 	0, 		0			     },

  /* quadruple is unsupported, and generates an error */
 {ERx("long_long_float"),T_FLOAT, 2*sizeof(double), 	  EQ_ADVAX	     },

  /* table separator */
 {(char *)0,  		0, 	0, 		0			     },

  /* now the constants */
 {ERx("max_int"),	T_INT,	sizeof(int),	EQ_ADALSYS|EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX},
 {ERx("min_int"),	T_INT,	sizeof(int),	EQ_ADALSYS|EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX},
 {ERx("true"),		T_INT,	sizeof(char),	EQ_ADALSYS|EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX},
 {ERx("false"),		T_INT,	sizeof(char),	EQ_ADALSYS|EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX},

  /* table terminator */
 {(char *)0,			0,	0,		0		     }
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
    static   i4	einit = 0;
    i4			yyequlex();
    static   char	*patch_extension = "--"; /* Patched after SYMINIT */

    switch (flag)
    {
      case GR_EQINIT:
	eq->eq_lang = EQ_ADA;
	eq->eq_def_in = ERx("qa");
	eq->eq_def_out = patch_extension;
	if (!einit)
	{
	    eq->eq_in_ext = ERx("qa");
	    eq->eq_out_ext = patch_extension;
	    einit = 1;
	}

      /* tell the world we're Embedded QUEL! */
	dml->dm_lex = yyequlex;

	gr->gr_type = T_NONE;
	gr->gr_sym = (SYM *)0;
	gr->gr_id = ERx("");
	gr->gr_flag = GR_HOSTCODE;

      /* ADA specific flags */
	gr->A_var = AvarDEC;
	gr->A_blk = 1;
	gr->A_rec = 0;
	gr->A_size = 0;
	gr->A_dims = 0;
	gr->A_record = (SYM *)0;
	gr->A_with = AwithNONE;

	ecs_clear();
	ada_init();
	break;

      case GR_SYMINIT:
	/* Patch up extensions as Ada command line flags -a may change them */
	if (eq->eq_def_out == patch_extension)
	    eq->eq_def_out = (eq->eq_config & (EQ_ADVAX|EQ_ADALSYS)) ?
				ERx("ada") : ERx("a");
	if (eq->eq_out_ext == patch_extension)
	    eq->eq_out_ext = (eq->eq_config & (EQ_ADVAX|EQ_ADALSYS)) ?
				ERx("ada") : ERx("a");

	{
	    /* Enter declarations into Equel symbol table */
	    GR_ADTYPES	*gt;

	    sym_init( (bool)TRUE );		/* ignore case */

	  /* enter the types */
	    for (gt=gr_typetab; gt->gt_id; gt++)
	    {
		if ((eq->eq_config & gt->gt_flags) == 0)	
		    continue;			/* Skip if wrong Ada config */

		sy = symDcEuc(gt->gt_id, 0, gr->A_blk,
		    (syFisTYPE|syFisSYS|syFisBASE), A_CLOSURE, SY_NORMAL);
		if (sy)
		{
		    sym_s_btype( sy, gt->gt_rep );
		    sym_s_dsize( sy, gt->gt_len );
		    sym_s_indir( sy, 0 );
		}
	    }

	  /* skip the separator and go on with the unsupported types */
	    for (gt++; gt->gt_id; gt++)
	    {
		if ((eq->eq_config & gt->gt_flags) == 0)	
		    continue;			/* Skip if wrong Ada config */

		sy = symDcEuc(gt->gt_id, 0, gr->A_blk,
		    (syFisTYPE|syFisSYS|syFisBASE), A_CLOSURE, SY_NORMAL);
		if (sy)
		{
		    sym_s_btype( sy, gt->gt_rep );
		    sym_s_dsize( sy, gt->gt_len );
		    sym_s_indir( sy, 0 );
		    sym_s_vlue( sy, AvalUNSUPP );
		}
	    }

	  /* skip the separator and go on with the constants */
	    for (gt++; gt->gt_id; gt++)
	    {
		if ((eq->eq_config & gt->gt_flags) == 0)
		    continue;			/* Skip if wrong Ada config */

		sy = symDcEuc(gt->gt_id, 0, gr->A_blk,
		    (syFisCONST|syFisSYS), A_CLOSURE, SY_NORMAL);
		if (sy)
		{
		    sym_s_btype( sy, gt->gt_rep );
		    sym_s_dsize( sy, gt->gt_len );
		    sym_s_indir( sy, 0 );
		}
	    }
	}

      /* remember them for convenience */
	gr->A_bytesy =
		sym_resolve( (SYM *)0, ERx(".byte"), A_CLOSURE, syFisTYPE );
	gr->A_shrtsy =
		sym_resolve( (SYM *)0, ERx(".short"), A_CLOSURE, syFisTYPE );
	gr->A_undefsy =
		sym_resolve( (SYM *)0, ERx(".undef"), A_CLOSURE, syFisTYPE );
	gr->A_strsy =
		sym_resolve( (SYM *)0, ERx("string"), A_CLOSURE, syFisTYPE );
	gr->A_intsy =
		sym_resolve( (SYM *)0, ERx("integer"), A_CLOSURE, syFisTYPE );
	if (eq->eq_config & (EQ_ADVAX|EQ_ADALSYS))
	{
	    gr->A_fltsy =
		sym_resolve( (SYM *)0, ERx("float"), A_CLOSURE, syFisTYPE );
	    gr->A_dblsy =
		sym_resolve((SYM *)0, ERx("long_float"), A_CLOSURE, syFisTYPE);
	}
	else
	{
#if defined(axp_osf)
            gr->A_fltsy =
                sym_resolve( (SYM *)0, ERx("float"), A_CLOSURE, syFisTYPE );
            gr->A_dblsy =
                sym_resolve((SYM *)0, ERx("long_float"), A_CLOSURE, syFisTYPE);
#else /* axp_osf */
	    gr->A_fltsy =
		sym_resolve((SYM *)0, ERx("short_float"), A_CLOSURE, syFisTYPE);
	    gr->A_dblsy =
		sym_resolve((SYM *)0, ERx("float"), A_CLOSURE, syFisTYPE);
#endif /* axp_osf */
	}
	gr->A_chrsy =
		sym_resolve( (SYM *)0, ERx("character"), A_CLOSURE, syFisTYPE);
	sym_s_vlue( gr->A_chrsy, AvalCHAR );

      /* note enumerated names -- these can't be type-coerced */
	{
	    SYM		*boolsy;

	    boolsy =
		sym_resolve( (SYM *)0, ERx("boolean"), A_CLOSURE, syFisTYPE );
	    sym_s_vlue( boolsy, AvalENUM );

	    sy = sym_resolve( (SYM *)0, ERx("true"), A_CLOSURE, syFisCONST );
	    sym_s_vlue( sy, AvalENUM );
	    sym_s_type( sy, boolsy );

	    sy = sym_resolve( (SYM *)0, ERx("false"), A_CLOSURE, syFisCONST );
	    sym_s_vlue( sy, AvalENUM );
	    sym_s_type( sy, boolsy );
	}

      /* declare "generic null" as a symbol */
	sy = symDcEuc( ERx(".null"), 0, gr->A_blk, syFisVAR|syFisSYS, 
		      A_CLOSURE, SY_NORMAL );
	sym_s_btype( sy, T_NUL );
	sym_s_dsize( sy, sizeof(i4) );
	gr->gr_null = sy;
	break;

      case GR_LOOKUP:
	/* How should we look up this name? */
	if (gr->A_var == AvarUSE)		/* Variable usage */
	{
	    /*
	    ** If we have a dot between name references then it may be the ADA
	    ** dot or the INGRES dot.  To resolve this we use the global
	    ** gr->A_record and see if the specified name is a member (child)
	    ** of record or a regular variable (including a record parent).
	    **
	    ** If this is the first component of a reference then gr->A_record
	    ** is NULL and we will be looking it up independently, otherwise
	    ** gr->A_record points to who the parent of the new member is.
	    **
	    ** MUST check for syFisCONST as IN params
	    ** and CONSTANT objects are syFisCONST but can be RECORDs.
	    */
	    sy = sym_resolve( gr->A_record, (char *)arg1, A_CLOSURE,
			      syFisVAR|syFisCONST );
	    if (sy)		/* It might have been a forward reference */
		_VOID_ symExtType( sy );
	    /*
	    ** We cannot set gr->A_record here as YACC may look ahead and cause
	    ** it to be reset.
	    */

	    if (sy == (SYM *)0)
		*(i4 *)arg2 = tNAME;
	    else if (sym_g_btype(sy) == T_STRUCT ||
	    	     ((i4)sym_g_vlue(sy) & AvalACCESS))
		*(i4 *)arg2 = tAREC_VAR;
	} else		/* In declare mode */
	    *(i4 *)arg2 = tADECNAME;
	break;

      case GR_NUMBER:		/* are we in a label declaration? */
	break;

      case GR_STMTFREE:
	gr->A_dims = 0;
	gr->A_val = 0;

	ada_reset();		/* clear the "name" stack */
	str_reset();
	gr->gr_type = T_NONE;
	gr->gr_sym = (SYM *)0;
	gr->A_record = (SYM *)0;
	gr->A_size = gr->A_rec = 0;
	break;

      case GR_ERROR:
	break;

      case GR_CLOSE:
	if (gr->A_blk > 1)
	    er_write( E_E60011_haUNCLOSED, EQ_ERROR, 1, er_na((gr->A_blk-1)) );
	break;

      case GR_DUMP:
	{
	    char		*trBaseType();
	    register FILE	*f = eq->eq_dumpfile;

	    SIfprintf( f, ERx("GR_DUMP: gr_ \n") );
	    SIfprintf( f,
			ERx("  sym = 0x%p, id = '%s', type = %s, flag = 0%o\n"),
			gr->gr_sym, gr->gr_id, trBaseType(gr->gr_type),
			gr->gr_flag );
	    SIfprintf( f, ERx("  func = %d, mode = 0%o\n"),
			gr->gr_func, gr->gr_mode );
	    SIfprintf( f, ERx("  A_type = %s, A_blk = %d, A_rec = %d\n"),
			trBaseType(gr->A_type), gr->A_blk, gr->A_rec );
	    SIfprintf( f, ERx("  A_size = %d, A_dims = %d\n"),
			gr->A_size, gr->A_dims );
	    SIfprintf( f, ERx("  A_var = %d, A_val = 0x%x\n"),
			gr->A_var, gr->A_val );
	    SIfprintf( f, ERx("  A_simpvar = %d, A_err = %d, A_with = 0x%x\n"),
			gr->A_simpvar, gr->A_err, gr->A_with );
	    SIfprintf( f, ERx("  A_record = 0x%p\n"), gr->A_record );
	    SIflush( f );
	}
	break;

      case GR_BLOCK:	/* Enter/exit an EQUEL block - no symbol table stuff */
	gr->A_var = AvarUSE;
	break;

      case GR_EQSTMT:
	{
	    i4		mode = (i4)arg1;
	    char	*kword = (char *)arg2;
	    static i4	in_include_err = FALSE;	/* Already complained */

	    /* Turn off declarations flag and generate line directive */
	    gr->gr_flag &= ~GR_HOSTCODE;
	    gr->A_var = AvarUSE;
	    if (inc_is_nested() == SC_INC && !in_include_err)
	    {
		er_write( E_E60006_haINCPKG, EQ_ERROR, 1, kword );
		in_include_err = TRUE;
	    }

	    /* Check if ## WITH {EQUEL|EQUEL_FORMS}; was used */
	    if (mode == GR_sFORMS && (gr->A_with & AwithFORMS) == 0)
	    {
		er_write( E_E60015_haWITHEQUEL, EQ_ERROR, 2,
						    ERx("EQUEL_FORMS"), kword );
		gr->A_with |= (AwithEQUEL|AwithFORMS);	/* No more complaints */
	    } else if ((gr->A_with & AwithEQUEL) == 0)
	    {
		er_write( E_E60015_haWITHEQUEL, EQ_ERROR, 2,
						    ERx("EQUEL"), kword );
		gr->A_with |= (AwithEQUEL|AwithFORMS);	/* No more complaints */
	    }

	    if (gr->gr_flag & GR_DECLCSR)
		gen_line( kword );
	    else
		gen_eqstmt( G_OPEN, kword );

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
	-1, 106,
	388, 1180,
	430, 1180,
	-2, 160,
	-1, 124,
	399, 307,
	-2, 292,
	-1, 206,
	388, 1180,
	430, 1180,
	-2, 174,
	-1, 217,
	388, 1180,
	430, 1180,
	-2, 254,
	-1, 241,
	399, 921,
	-2, 1180,
	-1, 242,
	399, 921,
	-2, 1180,
	-1, 251,
	399, 921,
	-2, 1180,
	-1, 384,
	388, 1180,
	430, 1180,
	-2, 281,
	-1, 403,
	388, 1180,
	430, 1180,
	-2, 332,
	-1, 422,
	388, 1180,
	430, 1180,
	-2, 555,
	-1, 524,
	388, 1180,
	430, 1180,
	-2, 588,
	-1, 586,
	400, 108,
	-2, 1180,
	-1, 603,
	440, 423,
	-2, 475,
	-1, 641,
	388, 1180,
	430, 1180,
	-2, 333,
	-1, 642,
	388, 1180,
	430, 1180,
	-2, 211,
	-1, 668,
	388, 1180,
	430, 1180,
	-2, 515,
	-1, 753,
	388, 1180,
	430, 1180,
	-2, 625,
	-1, 765,
	400, 877,
	-2, 874,
	-1, 786,
	399, 674,
	-2, 1180,
	-1, 787,
	399, 761,
	-2, 1180,
	-1, 788,
	388, 1180,
	430, 1180,
	-2, 919,
	-1, 789,
	388, 1180,
	430, 1180,
	-2, 919,
	-1, 790,
	399, 919,
	-2, 1180,
	-1, 791,
	388, 1180,
	430, 1180,
	-2, 588,
	-1, 792,
	388, 1180,
	430, 1180,
	-2, 919,
	-1, 794,
	399, 919,
	-2, 1180,
	-1, 796,
	388, 1180,
	430, 1180,
	-2, 919,
	-1, 832,
	398, 487,
	400, 487,
	-2, 475,
	-1, 855,
	398, 422,
	-2, 202,
	-1, 896,
	386, 486,
	388, 486,
	402, 486,
	430, 486,
	-2, 487,
	-1, 899,
	388, 1180,
	430, 1180,
	-2, 336,
	-1, 961,
	375, 615,
	383, 615,
	388, 1180,
	430, 1180,
	-2, 625,
	-1, 1073,
	365, 396,
	399, 475,
	401, 475,
	438, 396,
	439, 396,
	440, 396,
	441, 396,
	445, 396,
	446, 396,
	447, 396,
	-2, 487,
	-1, 1115,
	388, 1180,
	430, 1180,
	-2, 234,
	-1, 1140,
	388, 1180,
	430, 1180,
	-2, 337,
	-1, 1171,
	399, 475,
	401, 475,
	-2, 396,
	-1, 1300,
	388, 1180,
	430, 1180,
	-2, 114,
};

#define	YYNPROD		1196
#define	YYLAST		3277

short yyact[] = {
	   342,   553,   588,  1747,   803,  1700,  1089,  1449,   273,   596,
	  1750,   609,  1726,  1560,  1423,   324,   700,  1080,  1045,  1716,
	  1622,  1426,  1717,  1443,  1594,  1557,  1082,  1431,  1039,   946,
	  1234,   841,  1034,  1505,  1499,   351,  1041,  1059,   604,  1037,
	  1424,   724,  1589,   311,  1385,   272,  1410,  1469,   376,   663,
	  1530,  1412,   569,   278,   307,  1440,   552,  1025,  1152,   476,
	   865,  1285,  1720,   409,   662,   306,  1448,   864,  1480,  1040,
	  1163,   347,  1321,  1445,  1241,  1223,   548,  1460,   321,   547,
	  1456,  1071,  1411,   898,  1078,   976,   802,   995,   954,   320,
	   620,   942,   445,   752,  1068,   906,  1084,   833,   884,   335,
	    94,   343,   991,   807,   625,   333,   766,  1028,   396,   733,
	  1354,   874,   358,   842,   808,  1244,  1242,   358,   373,   721,
	   714,   750,   988,   694,   678,   711,   690,   671,   335,   624,
	  1092,   410,   496,   659,   333,   736,   410,   360,  1038,   600,
	   424,   860,   617,   424,   424,  1017,  1235,   783,   410,   459,
	   818,   737,  1055,   484,   410,   599,   705,   642,  1310,   410,
	  1427,   367,     2,   462,   332,  1314,   695,   427,  1243,   438,
	   479,   446,   786,   768,  1018,   581,   672,   483,   350,   372,
	  1305,  1094,   461,   495,   410,  1087,  1316,  1317,  1318,   375,
	   870,  1484,  1316,  1317,  1318,   336,  1098,  1074,   650,  1531,
	  1545,   592,  1640,   870,  1195,  1099,  1100,   354,   381,   847,
	   720,   338,   354,  1101,  1728,   329,  1072,   577,   944,  1414,
	   873,  1288,   875,  1360,   920,  1470,   460,   944,   870,   875,
	   356,   875,   424,  1305,   424,   424,  1654,   861,  1468,   410,
	   410,  1087,   410,   410,   873,  1437,   580,  1087,  1316,  1317,
	  1318,  1074,   410,  1097,   670,   870,  1759,  1561,   410,   410,
	   410,   410,   410,   410,   410,   410,  1087,   632,   410,   515,
	   417,  1420,  1760,  1316,  1317,  1318,  1087,  1559,   516,   517,
	  1598,   556,   557,  1316,  1317,  1318,  1029,   573,   584,   506,
	   507,   330,   402,  1087,  1320,   847,  1319,   876,   467,   921,
	  1316,  1317,  1318,   330,   876,   410,   876,   576,   415,   330,
	   498,   499,   862,  1046,   567,  1094,  1728,  1087,  1320,   847,
	  1319,   551,  1599,   448,  1316,  1317,  1318,   449,  1049,   336,
	  1098,   327,   650,   572,   575,   328,   799,   742,   326,  1099,
	  1100,  1032,   381,  1050,   475,   338,   492,  1101,   741,  1029,
	   598,  1418,  1452,  1452,   603,   330,   597,   568,  1159,   545,
	  1246,  1642,   563,   562,   480,   666,  1520,   411,  1094,   570,
	   399,  1347,   573,   824,   561,   667,   400,   574,   615,   330,
	   338,  1087,   336,  1098,  1252,   650,  1035,  1097,   554,   743,
	   619,   745,  1099,  1100,   814,   381,   870,   744,   338,   555,
	  1101,   823,   739,   740,   647,   613,  1462,  1582,  1254,   698,
	   645,  1463,   595,   566,   660,   614,   665,   565,   631,   410,
	   888,   627,   330,   687,  1414,  1416,   900,   775,   555,   589,
	   318,  1094,  1246,   925,  1087,  1417,   338,   632,   919,   550,
	  1097,   993,   644,   584,   549,   336,  1098,  1049,   650,   746,
	  1551,  1645,   735,  1550,   826,  1099,  1100,   374,   381,   909,
	  1742,   338,  1050,  1101,   480,   666,   338,   411,   480,   477,
	   992,   410,   410,   661,   330,   667,  1415,   664,   327,   478,
	   338,   525,   328,   338,   338,   326,   650,   809,   410,  1481,
	   410,   314,   325,   636,   323,   765,  1177,  1087,   410,   338,
	   989,   990,   479,  1097,   408,   990,  1015,  1247,   870,  1014,
	   410,   410,   353,   697,   998,   742,   665,   648,  1414,  1416,
	   481,  1418,   338,   888,  1447,   785,   741,   997,   354,  1417,
	   338,   610,   381,   856,   646,  1672,  1625,   411,   410,   410,
	   410,   410,   410,   410,   410,   410,   910,   410,  1430,   330,
	   338,  1414,  1416,   708,  1418,   630,  1751,   591,   411,   809,
	   410,  1246,  1417,   336,  1098,   338,   650,   743,  1149,   745,
	  1415,   338,   653,  1099,  1100,   744,  1546,  1205,   681,  1247,
	   739,   740,   330,   698,   674,   810,   812,   832,   832,   480,
	   477,  1240,   800,   437,   706,   679,  1201,  1202,   650,  1200,
	   478,   674,   369,  1415,   411,   338,   370,  1203,   801,   592,
	   603,   338,   338,   358,   651,  1087,  1404,   338,   371,   368,
	  1096,   411,   411,   374,   815,   411,   707,   746,  1396,   816,
	   336,   411,   780,   650,   338,   338,   681,  1392,   338,   896,
	   336,   481,   647,   851,   338,   333,   338,   338,   645,   603,
	   603,   336,   701,   681,   334,   832,   338,   410,  1049,   755,
	   748,   817,   878,   760,   338,   338,   881,   338,   592,   410,
	   870,  1382,   772,  1050,   679,   372,   774,   763,  1368,   779,
	   852,   689,  1452,   411,   903,   330,   411,   686,   809,   650,
	  1002,   984,   838,  1197,   771,   338,   338,  1279,  1032,   338,
	   411,  1402,   338,   410,   374,   830,   837,  1283,  1247,   787,
	   386,  1274,  1280,   338,   338,   784,   338,   410,  1282,   411,
	   971,   361,   341,   344,   937,   348,   349,   348,   338,  1107,
	   761,   462,   338,   410,   374,   364,   365,   806,   348,   697,
	   378,   928,   382,   384,   388,   918,   338,   401,   923,   403,
	   404,  1646,   994,   828,   999,   822,   648,   894,   411,  1691,
	   336,  1690,   423,   948,   938,   854,   359,   846,   550,   756,
	  1462,   338,   646,   904,   858,   338,   338,   674,  1544,   338,
	  1543,  1398,  1192,  1397,   872,  1554,   967,  1016,  1016,   479,
	   479,   479,   785,   479,   460,   479,   974,   479,   890,   897,
	   960,   880,   885,   895,   901,   902,   997,  1054,   943,   336,
	  1098,  1144,   650,  1142,   447,   868,   963,  1366,  1327,  1099,
	  1100,   809,   702,   702,  1010,  1000,  1105,   348,   348,   418,
	   643,  1058,   965,   936,   964,   935,  1194,  1027,   518,   381,
	   927,  1003,   926,  1073,   985,   598,   603,   338,  1043,   819,
	  1073,   597,  1012,   917,  1102,   916,  1051,   562,   706,  1036,
	  1062,   952,  1053,  1043,   416,   912,  1121,   911,   561,  1005,
	   987,  1051,   562,   945,   973,   930,   377,   605,   610,   922,
	   982,   827,   730,   561,   939,   941,   972,   709,   497,   354,
	   657,  1001,  1052,  1562,  1563,   509,   598,   494,   747,   913,
	  1119,   489,   597,  1042,  1564,  1138,   558,  1052,   592,   914,
	   458,   968,  1130,   660,   464,   413,   649,   453,  1042,   451,
	   410,   426,   717,   702,   612,  1056,  1056,   390,   410,   751,
	  1154,   410,   650,   832,   433,  1019,  1020,  1721,  1022,  1021,
	  1023,   753,  1024,  1171,  1111,  1033,  1065,  1387,  1388,  1009,
	  1390,  1391,   979,   980,  1228,  1685,  1060,   994,   994,   994,
	   388,  1069,   999,  1161,  1057,  1227,   388,   410,  1112,  1501,
	   410,  1061,   661,   605,   602,   651,   664,  1184,  1185,  1157,
	  1669,  1139,   410,   606,  1106,  1103,  1630,  1204,  1217,  1155,
	  1109,  1114,   336,  1158,  1156,   650,  1120,   618,  1136,  1122,
	  1617,   626,  1117,  1123,   410,   628,   629,  1616,   338,  1496,
	   715,   648,   344,   934,   956,   959,   533,  1165,   867,  1225,
	  1226,  1063,   957,   958,   534,   697,   961,  1066,   470,   532,
	   531,   469,   729,   466,  1722,  1249,   471,  1643,  1250,  1178,
	  1562,   473,  1170,   747,  1113,  1147,  1146,  1179,  1590,   699,
	  1591,  1166,  1151,  1233,   778,   491,   503,  1236,   436,  1180,
	  1181,  1182,   435,  1167,  1168,  1286,  1251,  1591,   504,   505,
	   487,  1171,  1171,   622,  1133,  1135,  1176,  1253,  1538,  1220,
	   421,  1171,  1171,  1094,  1221,  1510,  1043,   354,  1292,  1188,
	  1312,  1186,  1190,   639,  1051,   562,  1141,  1218,   651,   392,
	   610,   340,  1302,   331,   605,  1043,   561,  1332,  1255,  1256,
	  1229,  1219,   675,  1051,   562,  1231,   358,   363,   377,   929,
	  1422,  1198,   554,   873,  1295,   561,   866,  1341,  1294,  1290,
	  1052,   850,  1008,   759,   866,  1248,   346,  1171,  1531,  1470,
	  1232,  1042,   387,  1578,   410,   485,   410,  1175,  1343,  1052,
	   443,   966,   844,  1337,   969,   649,   684,   845,   674,  1169,
	  1042,  1262,  1296,   380,  1047,  1306,  1308,   981,  1275,  1276,
	  1269,  1171,  1303,   651,   859,   354,   986,   317,  1688,   994,
	   994,   869,   393,   727,   428,   770,   394,  1312,  1342,   520,
	   361,   933,  1329,   392,  1689,  1686,  1299,  1603,  1394,  1481,
	  1541,  1521,  1172,   683,  1356,   519,  1004,   834,   839,   840,
	  1361,  1335,   843,  1246,   782,   412,   457,   583,   825,  1351,
	   582,   530,   821,   820,   579,   848,   578,  1367,  1467,   348,
	   855,  1403,   857,  1358,  1339,  1432,  1405,  1330,  1406,  1364,
	   863,  1429,   511,  1357,  1428,  1425,   512,  1048,   877,  1044,
	   940,   280,   879,   527,  1558,  1409,  1373,   432,   513,   514,
	  1413,  1644,  1556,   899,  1408,  1407,  1378,  1376,  1381,   602,
	   602,   960,  1370,  1324,  1238,   699,  1374,  1395,  1383,  1466,
	  1450,  1379,  1380,  1289,   279,  1377,   277,  1307,  1266,  1436,
	  1451,  1258,  1419,  1446,  1474,  1273,  1272,  1322,  1323,  1749,
	  1748,  1758,  1757,  1465,  1746,  1171,  1727,  1471,  1725,  1656,
	  1171,  1441,  1171,  1171,  1444,  1278,  1271,   633,  1349,  1442,
	  1281,   947,  1277,  1455,  1270,  1312,  1171,  1253,  1458,  1268,
	  1453,  1454,  1439,  1438,  1434,  1435,  1043,  1267,  1265,  1264,
	  1312,  1312,  1263,   817,  1051,   562,  1145,  1261,  1260,   598,
	  1259,   598,  1457,  1359,  1371,   597,   561,   597,  1522,  1257,
	  1523,   805,   313,   598,  1459,   312,  1464,   410,   276,   597,
	  1507,   275,  1532,   832,  1171,   455,   456,  1312,   274,  1421,
	  1052,   994,  1031,  1030,   586,   474,   587,  1375,  1527,  1064,
	  1528,  1042,  1487,  1312,  1067,   337,  1245,  1239,  1237,  1482,
	  1552,  1494,   798,  1026,  1512,   797,   193,   608,   192,  1555,
	   649,   191,   845,  1191,  1516,   190,   189,   188,   187,  1524,
	  1503,   186,   310,   309,  1473,   271,   308,  1569,  1570,  1588,
	   428,  1479,  1698,   635,  1587,  1461,  1573,  1574,  1575,  1284,
	  1134,   813,   571,  1539,  1597,  1596,  1571,   654,  1581,  1291,
	   648,  1287,   811,  1450,   316,   315,   270,  1165,   269,   845,
	   845,   185,   184,  1451,  1568,  1583,   602,  1547,    93,  1585,
	  1286,  1567,  1513,   560,  1548,   536,   673,   669,   598,  1566,
	   618,  1118,   585,  1542,   597,  1595,   564,  1602,   590,  1153,
	   626,  1166,  1171,   677,  1457,  1127,   529,   676,  1483,   267,
	  1565,   508,   182,  1312,   304,   535,  1312,  1312,  1600,  1611,
	  1312,  1580,  1568,  1043,  1592,  1490,  1577,  1576,  1579,  1572,
	  1140,  1051,   562,  1586,   266,  1497,  1485,   181,  1488,  1489,
	  1584,   180,  1007,   561,  1514,  1154,   777,  1593,  1006,   691,
	   434,   776,  1492,  1606,  1525,  1601,  1637,   502,   773,   501,
	   264,   500,   179,  1164,   263,   178,   769,  1052,  1312,   262,
	  1619,   699,   177,   261,   713,   176,  1607,   183,  1042,  1613,
	   723,  1647,  1615,  1687,  1609,  1641,  1413,  1401,  1651,  1652,
	  1639,  1553,  1400,  1595,  1157,  1628,  1230,   764,  1638,  1631,
	  1540,   762,  1648,  1626,  1155,  1636,  1662,   260,  1158,  1156,
	   493,   175,   422,   259,   174,  1633,   758,  1171,  1595,  1634,
	  1635,   258,  1568,   757,   598,   490,   173,  1661,   257,  1649,
	   597,  1441,  1312,  1679,   488,  1650,  1682,  1659,   428,  1442,
	  1253,  1660,  1658,  1653,   172,   486,   171,  1345,   749,  1043,
	  1655,   482,  1657,  1673,  1675,   170,   169,  1051,   562,  1199,
	  1693,  1678,   734,   253,  1683,  1680,   168,   472,   468,   561,
	   167,   251,   165,  1399,  1393,  1595,   598,  1701,  1171,   465,
	  1692,   164,   597,   728,  1694,  1705,   428,   463,   163,  1695,
	  1632,  1704,  1711,  1052,  1350,   834,  1301,  1697,   839,  1312,
	  1703,  1386,  1699,  1696,  1042,  1384,  1043,  1389,  1723,  1729,
	  1707,  1718,  1712,   978,  1051,   562,  1193,   977,  1604,  1171,
	  1709,   975,   598,   726,   725,  1618,   561,   162,   597,  1207,
	  1372,  1737,  1736,  1724,  1312,   932,   738,   439,   722,  1334,
	   344,  1731,   719,  1013,   247,   718,   161,   348,  1745,  1743,
	  1052,  1740,   160,  1413,  1212,   429,   430,   159,   716,  1753,
	  1353,  1042,  1756,  1312,   598,   158,  1718,  1214,  1764,  1754,
	   597,  1363,  1766,  1762,  1761,  1768,  1450,   452,  1769,   598,
	  1770,  1595,  1767,  1744,  1413,   597,  1451,  1413,  1771,  1446,
	  1763,  1765,   157,   712,  1568,   710,   242,   450,   156,  1216,
	  1213,  1755,   241,   155,  1664,   153,  1666,  1215,  1210,  1211,
	  1209,  1674,   239,  1208,   444,   152,  1174,  1534,  1173,   951,
	  1476,  1549,   434,  1224,  1206,  1222,   338,   996,  1183,   955,
	   953,   704,   434,  1671,   962,   442,   441,  1011,   238,   950,
	   703,   440,   148,   147,   521,   431,   522,   523,   144,   143,
	   142,   234,   141,   434,   688,   233,   425,   434,   140,   649,
	   685,   139,  1162,   434,  1715,  1629,  1537,  1627,   407,  1519,
	  1536,  1535,  1369,   414,   931,   682,   420,   231,   419,   138,
	   137,   136,   668,   135,   658,   134,   907,  1143,   905,   656,
	   406,  1491,   655,   405,  1706,  1070,   454,   130,  1093,  1108,
	  1091,  1498,  1070,  1090,  1730,  1500,  1333,  1732,  1733,  1734,
	  1116,  1495,  1331,  1104,   601,   352,   696,  1738,   693,   692,
	  1526,   510,   924,  1095,  1085,  1083,  1077,  1670,  1668,  1605,
	  1129,  1478,  1086,  1088,  1081,  1735,  1311,  1079,  1076,  1075,
	  1315,  1313,  1486,  1309,  1304,   652,   129,   641,   640,   128,
	   127,  1362,   893,   638,   126,  1132,   887,   886,   883,   882,
	  1131,   634,   395,   125,  1623,  1621,  1529,  1355,  1506,   391,
	   389,   124,  1511,   123,  1515,   385,   524,   526,   383,   528,
	   528,   122,  1352,  1128,   121,   621,   871,   623,   285,   528,
	   217,   379,   120,   119,   616,   537,   538,   539,   540,   541,
	   542,   543,   544,  1164,   118,   546,   117,  1509,  1677,  1612,
	  1137,  1508,  1340,  1338,  1115,   611,   366,   116,  1504,  1336,
	   853,   115,   607,   114,  1110,   849,   362,   113,   112,   357,
	   355,   111,   207,   206,   110,   108,   107,   594,   106,   593,
	   105,   104,   559,   345,   103,  1518,  1517,  1150,  1346,  1126,
	  1125,  1344,  1124,   836,   835,   102,  1702,  1300,  1298,   691,
	  1475,  1297,   831,  1472,  1293,   829,   101,   339,    99,    98,
	    92,    91,  1610,    90,    89,    88,    87,    86,    85,    84,
	    83,    82,    81,    80,    79,    78,    77,    76,   713,    75,
	    74,    73,    72,    71,    70,   723,    69,    68,    67,    66,
	    65,    64,    63,    62,    61,    60,    59,    58,    57,    56,
	    55,    54,    53,    52,    51,    50,    49,    48,    47,    46,
	    45,    44,    43,  1070,  1070,    42,    41,    40,    39,  1608,
	    38,    37,    36,   348,    35,    34,    33,    32,    31,  1325,
	  1614,  1326,    30,    29,    28,    27,    26,    25,    24,    23,
	    22,    21,    20,    19,    18,    17,   680,  1620,    16,    15,
	  1624,    14,    13,    12,    11,   398,   889,   637,   397,     9,
	  1665,     8,  1667,     6,     5,     4,     3,     1,  1348,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  1681,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,   731,   732,
	     0,     0,     0,     0,     0,     0,     0,     0,  1328,     0,
	     0,     0,     0,     0,     0,   754,     0,     0,     0,     0,
	     0,     0,   767,     0,     0,   680,     0,     0,  1708,     0,
	     0,  1506,  1676,     0,     0,     0,     0,     0,   781,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,   788,   789,   790,   791,   792,
	   793,   794,   795,  1739,   796,     0,     0,     0,     0,     0,
	     0,     0,     0,   434,     0,     0,     0,   804,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  1710,
	     0,     0,  1624,  1713,  1714,  1719,     0,     0,     0,     0,
	     0,     0,     0,   434,     0,     0,     0,     0,     0,     0,
	   434,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,   434,     0,     0,     0,     0,     0,     0,  1741,
	     0,  1477,     0,     0,     0,     0,     0,  1070,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  1752,     0,     0,
	  1719,   434,     0,     0,     0,     0,     0,     0,   434,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  1502,     0,
	     0,     0,     0,     0,   908,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,   915,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	   949,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  1493,     0,     0,     0,   970,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	   983,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   434,     0,     0,     0,     0,    10,    97,   194,   196,
	   197,     0,   198,   199,   281,   209,     0,   212,     0,   214,
	   218,   216,   215,   219,   220,   221,   222,   223,   224,     0,
	  1430,     0,     0,   322,   132,   131,   133,   208,   229,   228,
	     0,   195,   230,   226,   227,   100,   109,   210,   211,   213,
	   225,   201,   202,   203,   200,   204,   205,   282,   283,   284,
	   236,   286,   235,   232,   288,   287,   154,   240,   237,   290,
	   289,     0,     0,     0,   294,   291,   292,   166,   248,   254,
	   146,   244,   250,   255,   256,   252,   151,   150,   145,   149,
	   295,   293,     0,   249,   243,  1470,   246,   245,   296,   297,
	   300,   298,   302,   301,   299,   305,   319,   303,   265,   268,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   318,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	    95,     7,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  1663,    96,     0,  1148,     0,     0,
	     0,     0,  1252,     0,     0,   680,  1433,   330,  1160,     0,
	     0,   327,     0,     0,     0,   328,     0,     0,   326,     0,
	     0,  1684,     0,   329,   314,   325,     0,   323,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  1187,     0,     0,  1189,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  1196,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   434,     0,     0,    10,    97,   194,   196,
	   197,   767,   198,   199,   281,   209,     0,   212,     0,   214,
	   218,   216,   215,   219,   220,   221,   222,   223,   224,     0,
	  1430,     0,     0,   322,   132,   131,   133,   208,   229,   228,
	     0,   195,   230,   226,   227,   100,   109,   210,   211,   213,
	   225,   201,   202,   203,   200,   204,   205,   282,   283,   284,
	   236,   286,   235,   232,   288,   287,   154,   240,   237,   290,
	   289,     0,     0,     0,   294,   291,   292,   166,   248,   254,
	   146,   244,   250,   255,   256,   252,   151,   150,   145,   149,
	   295,   293,     0,   249,   243,     0,   246,   245,   296,   297,
	   300,   298,   302,   301,   299,   305,   319,   303,   265,   268,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   318,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	    95,     7,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   908,     0,  1365,     0,    96,     0,     0,     0,     0,
	     0,     0,  1252,     0,     0,     0,  1433,   330,     0,     0,
	     0,   327,     0,     0,     0,   328,     0,     0,   326,     0,
	     0,     0,     0,   329,   314,   325,     0,   323,    10,    97,
	   194,   196,   197,     0,   198,   199,   281,   209,     0,   212,
	     0,   214,   218,   216,   215,   219,   220,   221,   222,   223,
	   224,     0,     0,     0,     0,   322,   132,   131,   133,   208,
	   229,   228,     0,   195,   230,   226,   227,   100,   109,   210,
	   211,   213,   225,   201,   202,   203,   200,   204,   205,   282,
	   283,   284,   236,   286,   235,   232,   288,   287,   154,   240,
	   237,   290,   289,     0,     0,     0,   294,   291,   292,   166,
	   248,   254,   146,   244,   250,   255,   256,   252,   151,   150,
	   145,   149,   295,   293,     0,   249,   243,   892,   246,   245,
	   296,   297,   300,   298,   302,   301,   299,   305,   319,   303,
	   265,   268,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,   318,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,    95,     7,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,    96,     0,   891,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,   330,
	     0,     0,     0,   327,     0,     0,     0,   328,     0,     0,
	   326,     0,     0,     0,     0,   329,   314,   325,     0,   323,
	     0,     0,     0,     0,  1533,    10,    97,   194,   196,   197,
	     0,   198,   199,   281,   209,     0,   212,     0,   214,   218,
	   216,   215,   219,   220,   221,   222,   223,   224,     0,     0,
	     0,     0,   322,   132,   131,   133,   208,   229,   228,     0,
	   195,   230,   226,   227,   100,   109,   210,   211,   213,   225,
	   201,   202,   203,   200,   204,   205,   282,   283,   284,   236,
	   286,   235,   232,   288,   287,   154,   240,   237,   290,   289,
	     0,     0,     0,   294,   291,   292,   166,   248,   254,   146,
	   244,   250,   255,   256,   252,   151,   150,   145,   149,   295,
	   293,     0,   249,   243,     0,   246,   245,   296,   297,   300,
	   298,   302,   301,   299,   305,   319,   303,   265,   268,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,   318,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,    95,
	     7,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,    96,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,   330,     0,     0,     0,
	   327,     0,     0,     0,   328,     0,     0,   326,     0,     0,
	     0,     0,   329,   314,   325,     0,   323
};

short yypact[] = {
	 -1000,  2839, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   735,   265,   733,
	 -1000,    34,    34,   774,    34,    34,    34,   892, -1000, -1000,
	 -1000,   377,   909,   750,    34,    34,   344,    34,   752,    34,
	   133,    34,   445,    34,   818,   -36,    34,   265,    34,    34,
	   215, -1000, -1000, -1000,   516,   297,   465,   430,   707,   373,
	   522,   133,   373,   373,   545, -1000,   683,   233,   829, -1000,
	 -1000, -1000,   415,   297, -1000,   465,   520,   518,   297,   516,
	   516,   511,   297,   515,   645,   430, -1000,   653,   511,   203,
	   822,   695,   502,   673,   465,   498,   489,   430,   430,   688,
	   -36,   -36,   496,   297, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,   984, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,   263,    34, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,    34,   924, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,   373, -1000,   373,   373, -1000, -1000, -1000,   148,   297,
	 -1000,   297,   297, -1000, -1000, -1000, -1000,   641, -1000, -1000,
	 -1000,   297, -1000,   641, -1000, -1000, -1000,   297,   297,   297,
	   297,   297,   297,   297,   297, -1000,   415,   297, -1000, -1000,
	 -1000, -1000, -1000,    41, -1000, -1000, -1000,  -114,    23,    -6,
	    -6, -1000, -1000, -1000, -1000,    34, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,   297, -1000, -1000, -1000,   -90,    12,
	     8, -1000,    -6,  -114, -1000, -1000, -1000,  -126, -1000, -1000,
	   757, -1000,   -83,  -110, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,  -142, -1000,    34,
	 -1000,   133, -1000,   133,    26,    34, -1000,   211, -1000,   374,
	   211, -1000, -1000,    34, -1000, -1000, -1000, -1000, -1000, -1000,
	   737, -1000,    34, -1000,   133,   732,   510, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,   211,    34, -1000,   732,   699,
	    34, -1000,   737, -1000,    34,    34, -1000,   133,    36,   912,
	   133,    34, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,   723, -1000,   606,   133,   491, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,   -22, -1000, -1000,   868, -1000,   311, -1000,
	   928,   865,   314, -1000, -1000, -1000,   868, -1000,   293, -1000,
	 -1000, -1000, -1000, -1000,  -142, -1000, -1000, -1000,   525, -1000,
	   822, -1000,   489, -1000,   -36, -1000, -1000,   487,   525, -1000,
	 -1000, -1000, -1000,   622,   524, -1000, -1000,  -228, -1000,   900,
	 -1000, -1000, -1000, -1000,   644,   483, -1000, -1000, -1000, -1000,
	   297,   297, -1000, -1000,    14, -1000, -1000, -1000, -1000, -1000,
	 -1000,   657,   822, -1000,   553, -1000, -1000,   297, -1000,   369,
	   770,   465, -1000, -1000,   330,   902, -1000,   294,   902, -1000,
	 -1000,    82,   672, -1000, -1000, -1000, -1000, -1000, -1000,   232,
	   297, -1000, -1000, -1000, -1000, -1000,   211,   211, -1000,   949,
	 -1000, -1000, -1000, -1000,   326, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   297,   297,   297,
	   297,   297,   297,   297,   297, -1000,   297,   -79,  -114, -1000,
	 -1000,   757, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   297,
	 -1000, -1000, -1000,   757,   -41, -1000, -1000, -1000,   422,  -114,
	     5, -1000,   757, -1000, -1000, -1000, -1000, -1000,   450,    13,
	 -1000, -1000,    53,   482, -1000,   133,    34,    34,    34, -1000,
	   729,    34, -1000, -1000,   211, -1000, -1000, -1000, -1000,   211,
	 -1000,  -231, -1000, -1000,    34, -1000,   766,   892,    34,   262,
	 -1000,    34,   344, -1000, -1000, -1000,   211, -1000,  -128,    34,
	   772,   630, -1000,   270, -1000,   758,  -136,    34, -1000, -1000,
	   729,    34, -1000,   133,   729,   119, -1000,  2652,   265, -1000,
	 -1000,   244,   245, -1000, -1000, -1000, -1000, -1000,    34,    34,
	 -1000, -1000,   729, -1000,   293, -1000,   169, -1000,   467, -1000,
	 -1000, -1000, -1000, -1000, -1000,   512, -1000, -1000,   297,   455,
	   345, -1000, -1000,  -141,   480,    30,   442,   341, -1000, -1000,
	  -141,   476,   133,   910, -1000, -1000,   625, -1000,   435,   324,
	 -1000,    30,   270,   108, -1000, -1000,  -143, -1000,   474,    36,
	   263, -1000,   297, -1000,   822, -1000,   638,   -36, -1000, -1000,
	   434, -1000,  -141,    30,   513,  -141,   297, -1000,   320,   488,
	 -1000, -1000,   192,    30, -1000, -1000,   564, -1000,  -141, -1000,
	   297, -1000,   297,   291,   446, -1000, -1000,  -141,   471, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   102,
	 -1000,    81, -1000,   128, -1000,   425, -1000, -1000,   430, -1000,
	 -1000, -1000,   290, -1000,   443, -1000, -1000,  -141, -1000,   470,
	 -1000, -1000, -1000, -1000, -1000, -1000,   769,   465, -1000,   424,
	 -1000, -1000, -1000,   453, -1000, -1000,   120,   120,   203,   203,
	   203,   326,   203, -1000,   203, -1000,   203,   -68,   -25, -1000,
	 -1000,  -131,    57, -1000, -1000,    23, -1000, -1000, -1000, -1000,
	   422,   422, -1000, -1000, -1000,  -114, -1000,    57, -1000, -1000,
	   450, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   729,   108,
	 -1000,   211, -1000, -1000,  -136,   108,   211, -1000, -1000,  -136,
	 -1000, -1000,  -191, -1000,   254,    34,   440, -1000,  -128,  -191,
	 -1000, -1000,   270,   909,   211, -1000, -1000,   133, -1000,    34,
	    34, -1000, -1000, -1000,   892,   300, -1000, -1000, -1000,    34,
	 -1000, -1000,   543, -1000,    34, -1000, -1000,   133, -1000, -1000,
	   729, -1000,   270,   108, -1000, -1000,  -136, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,   254, -1000,   804, -1000,    34,
	 -1000,   211,   211, -1000,   108,   413, -1000,  -141, -1000, -1000,
	 -1000, -1000,   -22, -1000, -1000, -1000, -1000, -1000, -1000,   242,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   236, -1000,    78,
	   297,   729,    34, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	    34, -1000,    59,   922, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,   825, -1000,    98, -1000,   664,    81,    81,    81,   589,
	 -1000,   128, -1000, -1000, -1000, -1000,   297, -1000,   622,   297,
	 -1000, -1000, -1000, -1000,  -141,   384, -1000,   437,  -236, -1000,
	 -1000,   297,   295, -1000, -1000,   192,   210,  1414,   -36,   553,
	 -1000,   407, -1000, -1000, -1000, -1000,   407,   631, -1000, -1000,
	 -1000, -1000, -1000,   297, -1000,    14, -1000,   430, -1000, -1000,
	 -1000,   900, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,   -25,   -25,   160, -1000, -1000,
	 -1000,   422, -1000,    -6, -1000, -1000,    -6,    88,   -28, -1000,
	    57, -1000, -1000, -1000, -1000, -1000,     3, -1000, -1000, -1000,
	 -1000,    23,   757,   289, -1000, -1000,  -114,  -211, -1000,    57,
	   -28, -1000, -1000,   764,    34,   300,   892,    34,  -263, -1000,
	  -191,  -191,  -121,    30, -1000, -1000, -1000, -1000, -1000, -1000,
	    59,    -4, -1000, -1000, -1000, -1000,   133, -1000,   133, -1000,
	 -1000, -1000, -1000, -1000,   419,   711,    36, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,   300, -1000,    34,    34,
	 -1000,  -263, -1000, -1000,   737,   163,    34, -1000,    36, -1000,
	 -1000, -1000, -1000, -1000,   -71,   133,   270, -1000,   699,    34,
	   925, -1000,   925, -1000,    16, -1000,   -57,    30, -1000, -1000,
	    34, -1000, -1000,   169, -1000,   297, -1000, -1000, -1000,   418,
	 -1000, -1000, -1000, -1000,    30, -1000, -1000, -1000, -1000, -1000,
	   278,   925,   270, -1000,  -134, -1000, -1000, -1000, -1000,  -253,
	    59,    30, -1000, -1000,   415, -1000,   -36,   638,    81,    81,
	   407, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   271, -1000,
	 -1000, -1000,   564, -1000,   559,   562,   237,    34, -1000, -1000,
	 -1000, -1000, -1000, -1000,    30, -1000, -1000,   228, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,   383, -1000,  -141, -1000, -1000, -1000, -1000, -1000,
	   312,   216, -1000, -1000,    -6, -1000,    -6, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,   165,  -167, -1000, -1000,   754, -1000,
	 -1000,  2470, -1000, -1000, -1000,  -218,  -218, -1000,  -182, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   -38, -1000, -1000,
	 -1000,   268,  -131,  -131, -1000, -1000,  -167,   941, -1000, -1000,
	 -1000,    38, -1000, -1000,     6, -1000,    41,  -114, -1000,  -189,
	    57,   794,  2470,   300, -1000, -1000, -1000,   892,   133, -1000,
	   209, -1000, -1000, -1000,  -191, -1000,  -210,  -145, -1000,    59,
	  -245,    59,    59, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,  -197, -1000,    59, -1000, -1000,    30, -1000,
	 -1000,   621, -1000,   270,   581,   133,   804,    34,   729, -1000,
	   714,    97,   270,    34, -1000,   919,   -76,   921,   254, -1000,
	   254,   772,   270,   581,   892,   777, -1000, -1000, -1000,  -253,
	 -1000, -1000,   254, -1000, -1000, -1000,   297, -1000,  -141,   705,
	 -1000, -1000,    34,    59,   920,  -172, -1000, -1000, -1000, -1000,
	    81, -1000, -1000, -1000,   380, -1000,  -240, -1000, -1000, -1000,
	 -1000, -1000, -1000,   176,    30, -1000, -1000, -1000,   631,    64,
	   386, -1000, -1000, -1000, -1000, -1000, -1000,    -6,  -140,  -163,
	 -1000, -1000, -1000, -1000, -1000,   507, -1000, -1000, -1000,   941,
	   654,  -131, -1000,  2240, -1000,    -6,    -6, -1000, -1000, -1000,
	 -1000,  2470, -1000, -1000,    -6,    -6,    -6, -1000, -1000,   370,
	 -1000, -1000, -1000,   798,   -62, -1000,    -6,     2, -1000,  -114,
	    41, -1000, -1000, -1000, -1000,  -163, -1000, -1000, -1000, -1000,
	 -1000,  -114, -1000, -1000,   685, -1000,   794,    57, -1000,  -108,
	 -1000,  2470,   892, -1000, -1000, -1000,   919,   254, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,  -253,   917, -1000, -1000,  -259,
	 -1000,    59,  -162,    30, -1000, -1000, -1000, -1000,    34, -1000,
	 -1000, -1000,    34, -1000,   211, -1000,    26, -1000,   772,   263,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   619,
	   612, -1000, -1000,   108,   892, -1000,    34, -1000, -1000,    34,
	 -1000, -1000, -1000,   136,   -22, -1000,   752,   598, -1000, -1000,
	  -253, -1000, -1000, -1000,   559,   -22,   900, -1000, -1000, -1000,
	 -1000, -1000, -1000,   -81,   651, -1000,    50, -1000,   352, -1000,
	   165, -1000, -1000, -1000, -1000, -1000, -1000,  -108, -1000, -1000,
	 -1000,  2240, -1000, -1000, -1000, -1000, -1000,   -38,  -193, -1000,
	 -1000, -1000, -1000,   757,  -131,  -167, -1000,  -131,   668, -1000,
	 -1000, -1000,  -108,    57,    -6, -1000, -1000, -1000, -1000, -1000,
	 -1000,   133,   270, -1000,  -197,   592,    59,   135,   581,   270,
	    34,    34,   737,   254, -1000, -1000, -1000, -1000, -1000, -1000,
	   581,   211, -1000,    26,    36, -1000, -1000, -1000,   133, -1000,
	   567, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	   915,   895,   914,   361,  -140, -1000, -1000, -1000,    -6,  -108,
	    57, -1000, -1000, -1000, -1000, -1000,   -25, -1000, -1000,   -25,
	 -1000, -1000, -1000,   177, -1000,   254, -1000,    59,   729, -1000,
	   211,  -253, -1000, -1000,   892, -1000, -1000, -1000,    34,    26,
	 -1000,    34,    34,    34,   263, -1000, -1000, -1000,   549, -1000,
	 -1000,   648, -1000, -1000, -1000,   -28,  -222,    -6, -1000, -1000,
	 -1000, -1000,   108, -1000,   423,   108,  -172,   108,    59, -1000,
	   732,   254, -1000, -1000, -1000,   270, -1000, -1000,   263,    36,
	 -1000, -1000,    60, -1000,  2470,  -120, -1000,   132, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,  -253,    34,   732, -1000,   263,
	 -1000, -1000, -1000,  2240,   -61, -1000,  -154, -1000, -1000, -1000,
	  -163, -1000, -1000,   254, -1000,  -108,    -6,   268,   132, -1000,
	 -1000,   165,    26, -1000, -1000, -1000, -1000, -1000,   254, -1000,
	 -1000, -1000
};

short yypgo[] = {
	     0,  2167,   160,  2166,  2165,  2164,  2163,  2161,  2159,   108,
	  2158,  2157,  2156,  2155,  2154,  2153,  2152,  2151,  2149,  2148,
	  2145,  2144,  2143,  2142,  2141,  2140,  2139,  2138,  2137,  2136,
	  2135,  2134,  2133,  2132,  2128,  2127,  2126,  2125,  2124,  2122,
	  2121,  2120,  2118,  2117,  2116,  2115,  2112,  2111,  2110,  2109,
	  2108,  2107,  2106,  2105,  2104,  2103,  2102,  2101,  2100,  2099,
	  2098,  2097,  2096,  2095,  2094,  2093,  2092,  2091,  2090,  2089,
	  2088,  2087,  2086,  2084,  2083,  2082,  2081,  2080,  2079,  2077,
	  2076,  2075,  2074,  2073,  2072,  2071,  2070,  2069,  2068,  2067,
	  2066,  2065,  2064,  2063,  2061,  2060,  2059,   164,  2058,  2057,
	  1142,   167,    31,  2056,   620,  1042,  2055,   191,  2054,  2053,
	    35,   118,  2052,    97,   557,   111,  2051,  2050,  2048,   130,
	  2047,    68,   512,  2046,    26,    17,     9,    60,     0,  2045,
	   101,  2044,     2,  2043,   104,  2042,  2041,  2040,  2039,  2038,
	  2036,  2035,  2034,  2033,  2031,    71,  2030,  2029,  2028,  2027,
	    62,  2026,  2025,  2024,  2023,  2022,  2021,  2020,  2019,  2018,
	   137,    38,   141,  2017,  2016,  2015,  2014,    94,  2013,  2012,
	  2011,    11,  2010,  2009,    83,   155,  2008,    33,   426,   113,
	  2007,  2006,  2005,  2004,  2003,   161,    48,  2002,  2001,  1999,
	  1998,  1997,  1996,  1994,  1984,   142,    29,  1983,  1982,  1981,
	    90,    67,  1980,  1978,  1977,   129,  1976,   158,  1975,  1974,
	  1973,  1972,    34,  1971,  1968,  1965,  1963,  1961,  1960,   927,
	   110,  1959,  1957,  1956,  1955,    50,    20,  1954,  1953,  1952,
	  1951,  1950,  1949,  1948,    98,  1947,   216,  1946,  1112,    72,
	  1945,  1944,  1943,  1942,  1941,  1940,  1939,  1938,  1937,   157,
	     6,  1936,  1935,    81,  1934,  1933,  1932,  1931,   165,  1930,
	  1929,  1928,  1927,  1926,  1924,  1923,  1922,  1919,  1918,  1917,
	  1916,    84,  1915,    96,  1914,    16,  1913,  1912,  1909,  1908,
	   123,  1906,    91,  1905,   139,  1904,  1903,  1902,  1901,  1896,
	  1895,  1893,  1890,  1888,  1887,  1883,  1882,  1880,  1727,  1879,
	  1878,    95,  1877,  1876,   438,  1875,  1215,  1874,   133,    59,
	    64,    49,    63,  1873,  1872,  1871,   171,  1870,   132,  1869,
	  1868,  1867,  1602,  1866,  1865,  1864,  1862,  1861,  1860,  1857,
	  1856,  1855,  1854,    19,    22,  1852,    70,  1851,  1850,  1848,
	  1846,  1845,  1844,   254,   126,  1842,  1841,  1840,  1839,  1838,
	  1835,  1833,   169,  1832,  1831,  1830,  1829,  1828,   147,  1827,
	    41,  1826,  1825,   183,  1824,  1821,   156,   153,  1820,   122,
	    88,  1819,   102,    87,  1818,    93,  1817,  1815,    75,  1813,
	  1811,  1809,  1808,  1806,    92,  1805,  1804,  1802,  1795,  1793,
	  1792,  1253,  1788,  1787,  1786,  1785,   125,  1783,  1782,  1767,
	   120,  1755,  1748,  1747,  1742,  1736,  1216,  1735,  1734,  1221,
	   172,  1733,  1732,   119,  1728,   151,  1726,  1719,  1717,   149,
	  1714,  1713,  1711,    85,  1707,  1706,  1703,  1697,  1695,    44,
	  1691,  1680,  1678,  1677,  1673,  1671,  1669,  1664,  1662,  1661,
	  1660,  1658,  1657,  1656,   109,  1653,  1652,   135,  1649,  1646,
	   174,  1645,  1641,   177,  1638,   121,  1636,  1635,  1634,  1624,
	  1618,   145,  1616,  1615,  1613,  1611,  1606,  1604,  1603,  1601,
	  1600,  1597,  1591,   182,  1587,   106,  1586,  1582,  1581,  1580,
	  1577,  1575,  1573,  1567,  1565,   173,  1563,  1562,  1559,  1556,
	  1555,  1554,  1552,  1551,  1550,  1549,  1548,  1547,  1541,  1538,
	  1536,  1532,  1531,  1527,  1524,  1504,  1502,  1501,  1499,  1497,
	  1493,   124,    58,  1489,  1477,   127,  1476,   166,   176,  1468,
	   100,  1462,  1461,  1458,  1456,    65,    54,    53,    56,    86,
	   138,    27,    14,    47,    24,     1,     4,  1455,    52,    15,
	   152,  1454,  1452,  1451,    89,  1449,  1445,  1444,  1177,  1442,
	  1441,   114,  1439,    77,    61,  1435,     8,    79,  1434,    39,
	  1432,  1429,    42,    32,  1426,  1164,    78,  1425,    45,  1423,
	  1422,  1421,  1418,  1417,  1416,  1415,  1411,  1408,  1406,  1405,
	    57,    30,  1403,    43,    76,  1402,  1398,  1397,    74,   116,
	   168,   115,    10,    13,  1396,    46,    80,   103,   107,  1383,
	  1382,  1379,   146,  1378,  1371,  1368,  1365,  1362,  1361,  1359,
	  1350,  1348,  1347,  1342,  1339,  1338,  1337,  1333,  1332,    55,
	    51,  1329,  1324,  1322,  1320,    82,  1316,    23,  1315,  1314,
	    73,    21,    66,  1309,    18,    37,     5,     7,  1308,    12,
	  1306,  1304,  1302,     3,  1301,  1300,  1299,  1296,  1295,  1291,
	  1289,  1288,  1286,  1284,  1283,  1279,  1274,  1265,  1264,  1262,
	    25,  1261,  1254,  1251,    28,    69,    36,  1249,  1247,  1245,
	  1244,  1241,  1235,  1228,    40,  1395,   217,  1226,   150,  1224,
	  1223,  1222,   175,  1220,  1218,  1217
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
	   391,   391,   304,   304,   402,   402,    13,    13,    12,    12,
	   279,   233,   517,   343,   510,   518,   416,     8,     8,     7,
	     7,   519,   519,   521,   521,   523,   523,   525,   527,   527,
	   526,   545,   545,   534,   534,   534,   534,   544,   538,   536,
	   542,   542,   540,   540,   552,   552,   555,   554,   558,   558,
	   558,   558,   560,   560,   524,   564,   565,   565,   522,   522,
	   567,   569,   568,   570,   520,   520,   520,   520,   520,   520,
	   520,   520,   571,   571,   556,   556,   583,   579,   579,   580,
	   559,   586,   586,   587,   587,   587,   588,   589,   596,   596,
	   590,   597,   582,   598,   599,   581,   581,   602,   572,   573,
	   573,   573,   603,   606,   608,   608,   609,   609,   609,   609,
	   609,   609,   609,   610,   616,   617,   618,   618,   619,   619,
	   611,   621,   612,   612,   622,   623,   623,   613,   626,   627,
	   627,   627,   627,   627,   629,   629,   632,   632,   634,   634,
	   633,   635,   636,   630,   638,   638,   639,   641,   641,   643,
	   643,   645,   645,   614,   615,   604,   605,   649,   649,   574,
	   575,   576,   653,   577,   657,   658,   659,   659,   660,   660,
	   661,   662,   578,   663,   530,   530,   664,   654,   654,   665,
	   665,   666,   666,   666,   666,   666,   667,   667,   668,   668,
	   669,   669,   670,   671,   655,   655,   532,   532,   674,   674,
	   674,   592,   592,   592,   592,   620,   595,   595,   625,   625,
	   625,   625,   529,   647,   600,   642,   563,   531,   549,   637,
	   557,   584,   585,   672,   624,   594,   593,   533,   541,   561,
	   528,   651,   551,   539,   648,   631,   601,   646,   562,   548,
	   644,   650,   673,   537,   591,   628,   656,   543,   547,   553,
	   535,   566,   550,   652,   546,   607,   640,   238,   128,   128,
	   675,   676,   676,   676,   677,   679,   679,   682,   683,   683,
	   685,   684,   680,   681,   678,   678
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
	     1,     1,     2,     1,     1,     2,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,    10,     4,     6,
	     9,     0,     2,     0,     1,     1,     1,     4,     0,     0,
	     1,     1,     0,     3,     1,     3,     1,     5,     0,     1,
	     2,     1,     0,     1,     2,     1,     1,     1,     1,     1,
	     2,     2,     2,     2,     1,     1,     1,     1,     1,     1,
	     1,     1,     6,     6,     1,     3,     1,     0,     1,     2,
	     1,     0,     1,     1,     1,     1,     4,     3,     0,     1,
	     1,     1,     1,     1,     4,     0,     1,     1,     5,     1,
	     1,     1,     2,     5,     0,     1,     1,     1,     1,     1,
	     1,     1,     1,     2,     1,     2,     1,     3,     1,     1,
	     1,     4,     1,     1,     2,     2,     2,     4,     1,     1,
	     2,     1,     2,     2,     1,     2,     5,     1,     1,     1,
	     1,     0,     0,     9,     1,     2,     4,     1,     3,     1,
	     1,     1,     3,     2,     2,     4,     7,     0,     1,     5,
	     3,     2,     9,     7,     2,     0,     1,     3,     1,     2,
	     1,     1,     2,     1,     0,     1,     1,     0,     1,     1,
	     2,     1,     1,     1,     1,     2,     1,     1,     5,     5,
	     1,     1,     4,     9,     0,     2,     1,     2,     2,     2,
	     1,     1,     1,     1,     1,     1,     1,     2,     1,     2,
	     1,     2,     0,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     2,     2,     1,
	     0,     2,     3,     2,     1,     1,     2,     2,     2,     1,
	     1,     1,     1,     1,     1,     0
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
	   -93,   -94,   -95,  -519,  -520,   390,   405,   257,   -96,   -98,
	   295,  -103,  -129,  -142,  -144,  -146,  -148,  -151,  -152,   296,
	  -153,  -156,  -159,  -163,  -168,  -170,  -180,  -192,  -193,  -197,
	  -198,  -209,  -213,  -216,  -217,  -228,  -241,  -245,  -246,  -251,
	  -294,   285,   284,   286,  -305,  -313,  -315,  -317,  -319,  -337,
	  -339,  -345,  -347,  -348,  -349,   338,   330,  -351,  -353,   339,
	   337,   336,  -385,  -388,   316,  -389,  -392,  -398,  -401,  -403,
	  -404,  -405,  -418,  -432,  -435,  -438,   327,  -440,  -443,  -449,
	  -451,  -456,  -458,  -462,  -467,  -469,  -484,  -487,  -490,  -492,
	  -502,  -503,  -506,  -483,  -521,  -522,  -571,  -572,  -573,  -574,
	  -575,  -576,  -577,  -578,   258,   291,   259,   260,   262,   263,
	   304,   301,   302,   303,   305,   306,  -154,  -155,   287,   265,
	   297,   298,   267,   299,   269,   272,   271,  -202,   270,   273,
	   274,   275,   276,   277,   278,   300,   293,   294,   289,   288,
	   292,  -321,   313,  -341,  -346,   312,   310,   318,  -357,  -387,
	   317,  -390,  -394,   344,   331,   347,   346,  -408,   328,   343,
	   332,  -439,   335,  -445,   329,   333,   334,  -460,  -465,  -468,
	  -471,  -486,  -488,  -491,  -494,   358,  -504,  -508,   359,  -523,
	  -524,  -567,  -568,  -556,  -603,  -604,  -605,  -652,  -527,  -653,
	  -663,   264,   307,   308,   309,  -203,   311,   315,   314,   320,
	   319,   325,   326,   341,   324,   340,   348,   349,   351,   354,
	   350,   353,   352,   357,  -505,   355,  -525,  -526,  -564,  -569,
	  -570,  -583,  -606,  -607,   434,  -537,  -541,  -548,   373,   356,
	  -544,  -566,   283,   437,  -539,   435,   428,   421,   425,   433,
	   417,   368,   -97,  -250,   389,  -128,   386,  -675,   402,   -99,
	   368,  -104,  -128,  -130,  -104,  -143,   362,  -145,  -104,  -104,
	  -145,  -110,  -283,  -122,   283,  -157,  -111,  -158,  -128,   389,
	  -160,   281,  -164,   367,  -104,  -104,  -181,  -185,   275,   258,
	   262,   274,  -150,  -111,   360,  -145,  -186,   366,  -104,  -199,
	  -105,   399,  -104,  -214,  -104,  -215,   265,  -100,  -104,  -218,
	  -219,  -221,   281,   364,   368,  -229,    -9,   -10,   -13,   406,
	   412,  -104,   -97,  -104,  -104,  -295,  -297,  -298,   289,  -312,
	  -128,   389,  -306,   399,  -298,  -316,   399,  -318,   399,  -320,
	  -323,   373,  -322,   389,  -128,  -340,   399,  -101,  -105,  -322,
	  -322,  -350,  -238,   389,  -675,   379,   375,   360,  -352,  -298,
	  -354,  -361,  -362,   321,  -386,  -384,  -316,   399,  -352,  -316,
	  -393,   399,  -399,   399,  -298,  -306,  -306,  -406,   399,  -419,
	  -312,  -473,  -128,  -433,   399,  -436,   388,  -318,  -441,   378,
	   375,   383,  -442,   388,  -406,  -450,  -309,   387,   397,  -128,
	   386,   438,  -452,  -453,  -367,   323,  -457,   375,  -459,   399,
	  -463,   382,  -316,  -470,   399,  -363,  -318,   399,  -318,  -318,
	  -493,  -495,  -497,   368,   380,   381,    -9,    -9,  -507,   399,
	  -298,   258,   262,   274,   275,  -150,  -145,  -145,  -104,   281,
	   265,  -322,  -322,  -322,  -298,   333,  -298,  -391,  -298,  -391,
	  -409,   389,   388,   375,   383,  -391,  -409,  -298,  -298,  -298,
	  -298,  -298,  -298,  -298,  -298,  -384,  -298,  -557,  -584,   403,
	   398,  -583,  -528,  -535,   365,   405,  -535,  -535,  -104,  -298,
	  -565,  -525,  -526,  -527,  -548,   405,   405,  -535,  -583,  -538,
	  -538,  -549,  -539,   413,  -528,   417,   417,  -676,  -677,  -679,
	   388,  -682,  -683,  -685,   430,  -100,  -105,  -105,  -132,   403,
	  -100,  -114,   398,  -147,  -149,  -150,  -126,  -250,  -128,  -175,
	  -284,  -285,  -104,  -128,  -161,   367,  -104,  -169,  -105,  -171,
	   368,  -182,  -114,  -161,  -186,  -171,  -194,  -195,  -104,  -171,
	  -200,  -208,   374,  -204,  -205,  -134,  -104,  -161,  -104,  -104,
	  -101,  -196,   401,  -219,  -230,  -105,  -130,   -11,  -242,   370,
	  -247,  -248,  -249,  -178,  -161,  -250,  -119,  -128,  -179,  -122,
	   389,   369,  -252,  -101,  -105,  -296,  -299,   399,  -307,  -308,
	  -128,  -309,  -310,  -311,  -312,   438,   387,   397,  -314,  -514,
	  -343,  -515,  -518,  -516,   290,  -238,  -509,  -510,  -511,  -518,
	  -298,   342,  -324,   275,   291,  -338,   373,  -128,  -342,  -343,
	  -344,  -238,  -278,  -279,  -280,  -517,  -281,  -271,   290,  -104,
	  -275,  -676,   398,  -355,  -365,  -366,  -367,  -363,    -9,   400,
	  -395,  -396,  -397,  -238,  -400,   388,  -402,   398,  -407,  -412,
	   438,  -413,  -414,  -238,  -360,  -420,  -421,   283,  -434,   388,
	   399,  -298,  -298,  -444,  -446,   438,  -447,  -415,  -416,   388,
	   389,   334,   323,   375,   383,   377,   435,   386,  -453,  -454,
	  -455,   376,  -375,   388,  -298,  -352,   400,  -464,  -466,   363,
	  -316,   400,  -472,  -473,  -474,  -128,  -475,  -298,  -485,  -489,
	   283,   400,  -485,  -496,  -450,   345,  -498,  -500,   382,  -352,
	   400,  -298,   265,  -358,   389,  -128,  -410,  -410,  -298,  -298,
	  -298,  -298,  -298,  -298,  -298,  -298,  -298,  -579,  -585,   415,
	  -583,  -528,  -529,  -536,  -298,  -608,  -590,  -597,  -551,   399,
	  -539,  -542,  -539,  -550,   389,  -538,  -528,  -529,  -678,   399,
	  -680,  -681,  -682,   388,   360,  -684,   401,   399,  -101,  -106,
	  -111,  -112,  -128,  -113,  -104,  -131,  -133,  -111,  -134,  -104,
	  -104,  -102,  -179,  -104,  -114,  -114,  -258,   440,  -104,  -165,
	   365,  -110,  -145,  -172,  -175,  -104,   271,  -104,  -185,  -114,
	  -162,   365,   440,  -104,  -201,  -127,   362,   388,  -107,  -114,
	   400,  -206,  -207,   365,  -115,   365,   440,  -104,  -102,  -104,
	  -101,  -102,  -232,  -233,  -234,  -517,  -235,  -237,   404,   -12,
	    -2,   407,   345,  -243,  -111,   -97,  -128,  -249,  -174,  -104,
	  -178,  -175,  -175,  -102,  -111,  -300,  -301,  -303,  -298,   290,
	   377,   400,   398,   387,   397,  -298,   400,   398,   400,  -304,
	   365,   440,   399,  -239,  -277,   403,   400,   398,   400,  -304,
	   399,  -325,  -105,   281,   388,   400,   398,   400,  -239,  -107,
	  -114,  -107,  -282,  -115,   361,   399,  -196,  -104,  -150,  -298,
	  -356,  -381,  -366,  -368,  -370,  -371,   376,   384,   385,   377,
	  -375,   388,  -364,    -9,   400,   398,  -304,  -239,   398,  -304,
	  -298,   400,   398,  -415,  -239,  -422,  -423,  -424,  -426,   388,
	   389,  -304,  -419,  -298,   400,   398,  -304,   399,  -369,   398,
	   403,  -372,   389,   360,  -128,  -373,  -376,   399,   386,  -128,
	   400,  -318,   400,   398,  -304,   399,  -499,  -501,   363,  -316,
	   400,  -359,   399,  -411,   389,   386,  -128,  -461,  -450,  -461,
	  -461,  -358,  -461,  -461,  -461,  -580,  -582,  -559,  -598,   417,
	  -599,  -600,   409,  -602,  -563,   411,  -580,  -559,  -530,  -664,
	  -665,  -666,  -520,  -568,  -667,  -634,   256,  -565,  -668,   390,
	   405,  -527,  -544,  -528,  -535,  -540,  -551,  -540,  -539,  -635,
	  -530,  -678,  -102,  -107,  -114,  -115,  -107,  -114,  -167,  -111,
	  -105,  -253,  -236,  -128,   442,  -260,  -261,  -270,  -271,  -262,
	  -125,  -264,  -124,  -272,  -273,  -274,  -266,   438,  -265,  -250,
	  -291,  -292,  -119,  -293,   372,  -276,  -104,   444,   387,   396,
	   397,   404,  -126,  -284,  -286,   386,  -160,   289,  -105,  -162,
	  -166,  -167,  -111,  -107,  -160,  -183,  -105,  -195,  -104,  -110,
	  -119,  -128,  -205,  -119,  -135,  -137,  -138,  -104,  -210,  -105,
	  -102,  -231,  -240,  -107,  -114,  -107,  -115,  -238,  -126,  -174,
	  -104,  -107,   400,  -302,   398,  -304,  -308,  -515,  -298,   326,
	  -238,  -511,  -512,  -513,  -128,  -309,  -310,  -311,  -312,   280,
	  -298,  -102,  -335,  -336,  -104,  -271,  -111,  -344,  -280,  -236,
	  -105,  -128,   280,  -382,  -383,   322,  -369,   398,   375,   383,
	  -372,  -372,  -372,  -374,   388,   389,  -396,  -298,  -400,  -298,
	  -413,  -304,   398,  -425,   399,   440,  -298,   398,  -447,  -448,
	   389,   386,   387,   397,  -128,   367,   400,  -417,   389,   386,
	   384,   385,   330,   376,   343,   383,   375,  -128,    -9,  -455,
	  -373,  -373,  -377,  -378,  -379,   388,   389,   334,   323,  -475,
	  -476,  -444,  -318,  -360,  -581,  -602,  -581,  -586,  -656,  -587,
	   431,  -588,  -589,  -590,  -591,  -594,   272,   419,  -597,  -535,
	  -535,  -531,   412,  -666,   405,  -528,  -528,  -609,  -649,  -610,
	  -611,  -612,  -598,  -613,  -614,  -615,  -651,  -616,  -621,  -589,
	  -622,  -626,  -647,  -648,   422,  -551,  -591,  -623,  -628,   408,
	   423,  -624,   429,   418,  -552,  -554,  -556,  -543,   432,  -654,
	  -665,  -545,  -531,  -108,   364,  -127,  -113,  -116,  -118,  -119,
	  -120,  -104,  -110,  -134,  -254,   443,  -167,  -236,  -167,  -255,
	  -207,  -263,  -125,  -257,  -258,  -259,   445,   446,   447,   441,
	   439,  -239,  -236,  -236,  -107,  -105,  -105,   399,  -238,  -273,
	  -119,  -287,  -128,  -289,  -104,  -130,  -173,  -161,  -184,  -111,
	  -187,  -171,  -145,  -196,  -136,  -122,  -139,   442,  -105,  -107,
	  -114,  -200,  -211,  -104,  -220,  -222,   279,  -220,  -234,  -236,
	   280,  -239,  -244,  -104,  -301,  -298,   399,  -239,   400,  -326,
	  -220,  -107,  -114,  -282,  -115,  -236,  -384,    -9,  -370,  -372,
	  -372,  -373,   400,  -423,  -428,  -429,  -430,   388,   389,  -427,
	   388,   389,   400,  -437,  -128,  -239,   400,   400,   398,  -304,
	  -477,  -480,   389,  -128,   400,  -535,  -535,  -657,  -658,  -592,
	  -595,  -625,  -620,  -539,   386,   438,   387,   397,   389,  -595,
	   438,  -601,   366,  -532,  -674,  -669,  -631,    -2,  -670,  -671,
	   280,  -531,  -672,   416,  -566,  -566,  -650,   427,  -617,  -618,
	  -619,  -583,  -620,  -627,  -629,  -630,  -631,   256,  -632,  -637,
	  -556,  -634,   414,  -580,  -580,  -595,  -596,  -588,  -595,  -625,
	  -553,  -555,   400,   405,  -557,  -539,  -655,  -673,   427,  -533,
	   345,  -532,  -109,  -119,  -128,  -117,  -122,  -105,  -104,  -119,
	  -121,   280,  -167,  -107,  -107,  -236,  -256,  -253,  -236,  -236,
	  -107,  -114,  -236,  -238,  -239,  -288,   388,  -107,  -114,  -212,
	  -290,   388,  -105,  -174,  -176,  -177,  -104,  -102,  -188,  -191,
	   371,  -104,  -150,  -119,  -107,  -104,  -121,  -140,  -141,  -122,
	   442,   280,  -126,  -126,  -201,  -107,  -114,  -212,  -110,  -223,
	  -225,   361,  -126,  -298,  -304,  -327,  -328,  -330,   373,  -336,
	  -236,   280,  -372,   400,   398,   440,   400,  -239,  -378,  -380,
	   389,   386,  -128,  -478,   399,  -535,  -659,  -660,  -662,   417,
	  -593,   420,   386,   387,   397,  -596,  -580,  -533,  -674,  -535,
	  -535,  -532,  -529,  -535,  -535,  -535,  -553,  -584,   345,  -630,
	  -632,  -535,   405,  -539,  -557,  -593,  -554,  -558,  -561,  -562,
	   363,   382,  -533,  -665,  -534,  -539,  -546,  -547,   388,   430,
	  -110,  -121,  -126,   280,  -236,  -267,  -225,  -239,  -104,  -145,
	  -114,  -132,  -189,  -127,  -104,  -150,   388,   388,  -107,  -110,
	  -104,  -224,  -226,  -227,  -104,   400,  -512,  -329,  -186,  -331,
	   388,  -429,  -431,  -309,  -312,  -310,  -311,  -128,  -360,  -479,
	   283,  -481,   442,   386,  -661,   401,   399,  -592,  -534,  -533,
	  -538,  -536,  -536,  -619,   429,  -528,  -633,  -580,  -595,  -559,
	  -562,  -534,  -535,  -105,  -107,  -114,  -107,  -114,  -268,   388,
	  -269,  -236,   400,  -212,  -107,  -177,  -104,  -190,  -161,  -126,
	  -212,  -114,  -132,  -196,  -105,   388,   280,  -482,   283,   280,
	   400,   398,  -660,  -535,  -534,  -664,  -635,  -581,  -560,  -563,
	  -636,  -536,  -123,  -124,  -125,  -126,  -236,  -102,  -114,  -110,
	  -104,  -132,  -226,  -104,  -104,  -332,  -333,  -334,  -275,  -104,
	  -150,   388,   386,  -536,  -531,  -638,  -639,  -640,   436,  -535,
	  -107,  -124,  -107,  -107,  -107,  -236,  -171,  -126,  -107,  -114,
	  -334,  -104,   400,  -532,  -533,  -639,  -641,  -643,  -645,  -646,
	  -592,   424,  -104,  -171,  -333,  -533,  -637,  -642,  -644,   410,
	   426,  -593,  -126,  -534,  -535,  -627,  -643,  -592,  -132,  -536,
	  -636,  -126
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
	    94,    95,    96,   939,   940,   937,   938,    97,  1180,   457,
	   103,  1180,  1180,   156,  1180,  1180,    -2,   417,   169,   172,
	   173,  1180,   461,   192,  1180,  1180,  1180,  1180,     0,  1180,
	   258,  1180,  1180,  1180,    -2,   319,  1180,  1180,  1180,  1180,
	  1180,   502,   503,   504,     0,  1180,     0,     0,     0,  1180,
	     0,     0,  1180,  1180,  1180,   577,     0,  1180,   593,   642,
	   643,   644,   910,  1180,   651,     0,     0,     0,  1180,     0,
	     0,     0,  1180,   741,   746,     0,   751,   754,     0,  1180,
	     0,   787,   794,   798,     0,   811,   894,     0,     0,     0,
	     0,     0,   871,  1180,   941,   942,   984,   985,   986,   987,
	   988,   989,   990,   991,   101,     0,   105,   131,   153,   161,
	   162,   163,   164,   165,   170,   171,    -2,  1180,   183,   188,
	   190,   196,   200,   215,   244,   246,   252,    -2,   257,   278,
	   284,   287,   323,   329,   331,   345,   489,   506,   516,   518,
	   520,  1180,   554,  1180,  1180,   570,   572,   574,  1180,  1180,
	   650,    -2,    -2,   664,   668,   670,   672,     0,   713,   739,
	   744,    -2,   753,     0,   775,   777,   786,  1180,  1180,  1180,
	  1180,  1180,  1180,  1180,  1180,   861,   910,  1180,   831,   943,
	   944,   978,   979,     0,  1019,  1020,  1021,     0,     0,     0,
	     0,   178,   179,   180,   181,  1180,   523,   560,   568,   585,
	   647,   654,   657,   676,   750,   762,   791,   797,   805,   808,
	   834,   837,   843,   846,  1180,   868,   945,   946,     0,     0,
	     0,   994,     0,     0,  1173,   958,   958,     0,  1093,   865,
	     0,   975,     0,     0,   996,  1175,  1163,  1148,  1159,  1171,
	  1153,    99,    98,   484,   485,   486,   478,     0,  1179,  1180,
	   456,     0,   475,     0,   132,  1180,   155,   157,   476,  1180,
	   159,   168,   416,  1180,   459,   182,   184,   185,   487,   186,
	     0,   460,  1180,   191,   197,     0,     0,   216,   218,   219,
	   220,   221,   222,   223,   434,   243,  1180,   451,     0,     0,
	  1180,   449,     0,   277,    -2,  1180,   282,     0,   473,   307,
	     0,  1180,   306,   290,   291,   304,   320,    13,    12,   926,
	   927,   322,   328,    -2,     0,     0,   490,   491,   492,   873,
	   874,   872,   505,  1180,   880,   517,  1180,   519,  1180,   521,
	     0,   528,    -2,   544,   545,   558,  1180,   566,  1180,   569,
	   571,   573,   575,   576,     0,   578,   581,   579,   580,   878,
	   598,   591,   894,   594,     0,   648,   908,  1180,   649,   652,
	   655,  1180,   663,     0,   925,   669,   671,  1180,   675,   715,
	   875,   876,   877,   738,     0,     0,   745,   748,   752,   755,
	  1180,  1180,   758,   759,     0,   774,   888,   889,   890,   891,
	   882,     0,   776,   778,     0,   602,   785,  1180,   789,  1180,
	   800,     0,   803,   806,  1180,   838,   892,  1180,   838,   841,
	   844,  1180,   854,   849,   852,   853,   860,   862,   866,  1180,
	  1180,   102,   154,   285,   288,   175,   176,   177,   255,     0,
	   272,   522,   559,   567,    -2,   584,   646,   653,   920,   656,
	   681,   677,   678,   679,   680,   749,   681,  1180,  1180,  1180,
	  1180,  1180,  1180,  1180,  1180,   863,  1180,   997,     0,  1140,
	  1141,     0,  1132,   959,  1150,  1170,  1081,  1092,   256,  1180,
	   974,   976,   977,     0,     0,   980,   982,  1022,  1024,     0,
	     0,   958,     0,  1138,  1132,   981,   983,  1178,  1195,     0,
	  1184,  1185,     0,  1189,  1190,     0,    -2,  1180,  1180,   436,
	   350,  1180,   437,   158,   166,   167,   481,   479,   480,   418,
	   419,     0,   422,    -2,  1180,   452,   447,   417,  1180,  1180,
	   455,  1180,  1180,   224,   225,   226,   245,   247,     0,  1180,
	   267,     0,   439,     0,   260,   264,   263,  1180,   279,   280,
	   350,  1180,   453,     0,   350,   316,   289,     0,  1180,   324,
	   330,    -2,    -2,   339,   340,   341,   342,   343,  1180,  1180,
	   483,   458,   350,   346,  1180,   488,  1180,   494,     0,   508,
	   510,   511,   512,   513,   514,     0,   884,   886,    -2,     0,
	     0,   913,   933,     0,     0,   396,     0,     0,   897,   934,
	     0,     0,     0,   526,   529,   553,     0,   557,     0,     0,
	   563,   396,     0,     0,   410,   930,     0,   413,     0,   415,
	  1180,  1177,  1180,   638,   597,   599,   614,   596,   645,   909,
	     0,   659,     0,   396,     0,     0,  1180,   924,     0,   686,
	   687,   688,     0,   396,   712,   714,     0,   717,     0,   742,
	  1180,   756,  1180,     0,   763,   764,   765,     0,   692,   695,
	   696,   697,   698,   699,   700,   701,   936,   883,   779,   604,
	   781,  1180,   784,    -2,   788,     0,   793,   795,     0,   802,
	   799,   809,     0,   812,   813,    -2,   814,     0,   832,     0,
	   840,   893,   835,   847,   850,   851,   857,     0,   856,     0,
	   870,   830,   271,   590,   586,   587,    -2,    -2,    -2,    -2,
	    -2,    -2,    -2,   836,    -2,   845,    -2,     0,   998,  1142,
	   995,     0,  1094,  1080,   864,     0,  1025,  1010,  1011,  1152,
	   962,   962,   960,   961,  1172,     0,  1061,  1094,  1181,  1194,
	  1195,  1183,  1186,  1192,  1193,  1187,  1191,  1188,   350,     0,
	   106,   107,    -2,   109,     0,     0,   134,   135,   136,     0,
	   133,   152,  1180,   477,  1180,  1180,   461,   406,     0,  1180,
	   446,   195,     0,   461,   201,    -2,   203,   228,   217,  1180,
	  1180,   440,   441,   251,   417,  1180,   438,   266,   259,  1180,
	   454,   262,     0,   448,  1180,   442,   443,   273,   283,   474,
	   350,   318,     0,     0,   310,   931,     0,  1180,   315,    11,
	    14,   928,   929,   321,   325,  1180,    -2,   211,   335,    -2,
	   210,   212,   213,   344,     0,     0,   495,     0,   498,   499,
	   500,   507,  1180,   885,   887,   881,   911,  1180,   912,  1180,
	   922,   923,   935,   917,  1180,   398,   895,  1180,   896,  1180,
	  1180,   350,  1180,   527,   556,   561,  1180,   562,   565,   408,
	  1180,   409,  1180,   444,   445,   932,   390,   388,   389,   879,
	   582,   640,   600,   604,   605,     0,  1180,  1180,  1180,     0,
	   613,    -2,   592,   595,   658,  1180,  1180,   662,     0,  1180,
	   667,   673,  1180,   690,     0,   716,   718,   727,     0,   722,
	   723,  1180,     0,   757,   760,     0,  1180,  1180,     0,     0,
	   603,   625,   616,   617,   618,   621,   625,     0,   623,   624,
	   792,   801,   810,  1180,   816,     0,   848,     0,   859,   855,
	   869,   715,   589,   682,   683,   684,   685,   790,   918,   796,
	   804,   807,   833,   842,   867,  1015,  1015,  1001,  1012,  1000,
	  1013,     0,  1134,     0,  1017,  1136,     0,  1001,     0,  1095,
	  1096,  1099,  1101,  1102,  1103,  1104,     0,  1106,  1107,  1058,
	  1059,     0,     0,  1077,  1075,   948,     0,     0,   957,  1097,
	   951,  1182,   100,     0,  1180,  1180,   417,  1180,   348,   349,
	  1180,  1180,     0,    -2,   400,   361,   362,   382,   383,   363,
	  1180,  1180,   384,   385,   386,   387,     0,   402,     0,   465,
	   466,   467,   468,   469,     0,  1180,   373,   375,   470,   472,
	   471,   394,   482,   420,   421,   424,  1180,   427,  1180,  1180,
	   189,   193,   194,   198,   205,    -2,  1180,   248,   249,   253,
	   268,   269,   261,   265,   148,   139,     0,   143,     0,  1180,
	   294,   305,   294,   308,   316,   309,  1180,   396,   326,   334,
	    -2,   347,   493,  1180,   501,  1180,   509,   914,   915,     0,
	   397,   898,   899,   901,   396,   903,   904,   905,   906,   907,
	     0,   294,     0,   547,     0,   550,   551,   564,   411,   412,
	  1180,    -2,   414,   637,   910,   641,     0,   614,  1180,  1180,
	   625,   609,   610,   612,   619,   620,   660,   661,     0,   666,
	   689,   691,     0,   720,     0,     0,     0,  1180,   766,   767,
	   768,   769,   770,   771,   396,   773,   693,     0,   702,   703,
	   704,   705,   706,   707,   708,   709,   710,   711,   780,   782,
	   783,   622,     0,   627,     0,   630,   631,   632,   633,   815,
	  1180,     0,   858,   583,     0,  1016,     0,   999,  1085,  1002,
	  1166,  1003,  1004,  1005,     0,     0,  1164,  1145,     0,  1018,
	  1079,     0,  1137,  1100,  1105,  1132,  1132,  1023,     0,  1026,
	  1027,  1028,  1029,  1030,  1031,  1032,  1078,     0,  1040,  1042,
	  1043,     0,     0,     0,  1151,  1034,     0,  1008,  1048,  1133,
	  1154,     0,  1165,  1144,     0,   964,     0,     0,  1167,  1114,
	  1098,     0,     0,  1180,   126,   127,   110,   118,   112,   113,
	    -2,   125,   130,   137,  1180,   399,     0,     0,   352,  1180,
	   359,  1180,  1180,   356,   357,   358,   403,   404,   405,   407,
	   401,   391,   366,     0,   371,  1180,   372,  1180,   396,   393,
	   425,   431,   429,     0,   462,     0,   211,  1180,   350,   229,
	   237,  1180,     0,  1180,   138,     0,   145,     0,  1180,   140,
	  1180,   267,     0,   462,   417,   297,   295,   317,   311,   312,
	   313,   314,  1180,   338,   496,   497,  1180,   902,     0,   530,
	   525,   546,  1180,  1180,   444,     0,   639,   601,   606,   607,
	  1180,   608,   665,   719,     0,   728,     0,   731,   732,   721,
	   724,   725,   740,     0,   396,   772,   694,   626,     0,  1180,
	   819,   818,   822,   823,   839,   992,   993,     0,     0,     0,
	  1121,  1122,  1123,  1124,  1126,     0,  1128,  1130,  1125,  1008,
	     0,     0,  1156,     0,  1116,     0,     0,  1120,  1110,  1111,
	  1155,     0,  1132,  1143,     0,     0,     0,  1161,  1033,     0,
	  1036,  1038,  1039,     0,  1049,  1051,     0,     0,  1054,     0,
	     0,  1057,  1139,  1073,  1074,     0,  1044,  1009,  1045,  1046,
	   963,     0,  1169,   966,   968,   949,     0,     0,  1162,   953,
	  1147,   952,   417,   128,   129,   111,     0,  1180,   115,   116,
	   117,   450,   353,   351,   367,   354,     0,   360,   364,   365,
	   368,  1180,   376,   396,   392,   426,   430,   428,  1180,   432,
	   463,   464,  1180,   199,   204,   206,   208,   214,   240,  1180,
	   238,   231,   232,   233,   227,   250,   144,   146,   147,     0,
	     0,   149,   142,     0,   417,   274,  1180,   275,   286,  1180,
	   296,   435,   327,     0,  1180,   524,   536,     0,   533,   548,
	   549,   552,   611,   726,     0,  1180,   715,   747,   628,   629,
	   634,   635,   636,   824,     0,  1083,  1084,  1086,  1088,  1091,
	     0,  1146,  1127,  1129,  1131,  1007,  1014,   953,  1117,  1118,
	  1119,     0,   958,   959,   959,  1076,  1035,     0,     0,  1050,
	  1055,  1052,  1053,     0,     0,     0,   965,     0,   969,   971,
	  1149,  1158,   953,  1115,     0,   954,   955,   956,  1174,  1168,
	   104,   120,     0,   355,     0,   381,  1180,     0,   462,     0,
	  1180,  1180,   242,  1180,   235,   236,   150,   151,   141,   270,
	   462,   293,   298,   300,   302,   916,   900,   531,     0,   532,
	   534,   729,   730,   733,   734,   735,   736,   737,   743,   817,
	     0,   829,     0,     0,     0,  1090,  1089,  1006,     0,   953,
	     0,  1108,  1109,  1037,  1047,  1061,  1015,  1060,  1041,   972,
	   970,  1062,   959,     0,   123,  1180,   369,  1180,   350,   380,
	   377,   378,   395,   433,   417,   207,   209,   230,  1180,     0,
	   276,  1180,  1180,  1180,  1180,   535,   825,   826,     0,   827,
	   820,     0,  1087,   959,  1112,     0,     0,     0,   967,   973,
	  1082,   950,     0,   121,     0,     0,     0,     0,  1180,   187,
	     0,  1180,   299,   301,   303,     0,   538,   540,  1180,   542,
	   543,   828,     0,   947,     0,     0,  1064,     0,  1176,  1056,
	   119,   122,   124,   370,   374,   379,  1180,     0,   537,  1180,
	   541,   542,   821,     0,     0,  1065,     0,  1067,  1069,  1070,
	  1071,  1157,   241,  1180,   539,   953,     0,     0,     0,  1135,
	  1160,     0,     0,   959,  1062,  1066,  1068,  1072,  1180,  1113,
	  1063,   239
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
#line 403 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 3:
#line 409 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 440 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_flag &= ~GR_EQUEL;
		    gen_eqstmt( G_CLOSE, (char *)0 );
		}
		break;
	case 5:
#line 445 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if ((gr->gr_flag & GR_DECLCSR) == 0)
			gen_eqstmt( G_CLOSE, (char *)0 );
		    gr->gr_flag &= ~(GR_QUEL|GR_REPEAT|GR_NOSYNC|
				     GR_CURSOR|GR_DECLCSR|GR_REPLCSR);
		}
		break;
	case 6:
#line 452 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_flag &= ~GR_EQUEL;
		    gen_eqstmt( G_CLOSE, (char *)0 );
		}
		break;
	case 7:
#line 457 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (*(yypvt[-0].s) == 'i')	/* "include" or "<EOF>" */
			inc_push_file();
		    else
			inc_pop_file();
		}
		break;
	case 10:
#line 466 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 522 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    lbl_exit_block();
		    gr_mechanism( GR_BLOCK, GR_BLKFALSE ); /* Symtab updates */
		}
		break;
	case 12:
#line 528 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 540 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 97:
#line 670 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    arg_int_add( IIxactABORT );
		    gen_call( IIXACT );
		    gr->gr_flag |= GR_NOSYNC;      /* No IIsync after this */
		}
		break;
	case 99:
#line 679 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 101:
#line 691 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 102:
#line 695 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-0].s );
		}
		break;
	case 103:
#line 703 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    arg_int_add( IIxactBEGIN );
		    gen_call( IIXACT );
		    gr->gr_flag |= GR_NOSYNC;      /* No IIsync after this */
		}
		break;
	case 105:
#line 717 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 126:
#line 750 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 129:
#line 757 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 785 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 149:
#line 816 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		    if (STbcompare(yypvt[-1].s, 0, ERx("not"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
						    ERx("CREATE"), ERx("NOT") );
		}
		break;
	case 150:
#line 825 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( ERx("default") );
		    if (STbcompare(yypvt[-0].s, 0, ERx("default"), 0, TRUE) != 0)
			er_write(E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
						ERx("CREATE"), ERx("DEFAULT") );
		}
		break;
	case 151:
#line 833 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 851 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 154:
#line 855 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-0].s );
		}
		break;
	case 161:
#line 875 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 162:
#line 879 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 163:
#line 885 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 164:
#line 889 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 165:
#line 894 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 170:
#line 918 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 171:
#line 923 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 172:
#line 930 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    arg_int_add( IIxactCOMMIT );
		    gen_call( IIXACT );
		    gr->gr_flag |= GR_NOSYNC;      /* No IIsync after this */
		}
		break;
	case 173:
#line 942 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 178:
#line 952 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 179:
#line 958 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 180:
#line 962 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 181:
#line 966 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 183:
#line 976 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		}
		break;
	case 186:
#line 984 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 188:
#line 996 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 190:
#line 1006 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 196:
#line 1025 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 200:
#line 1042 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 203:
#line 1049 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 215:
#line 1079 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 218:
#line 1087 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 219:
#line 1091 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 220:
#line 1095 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 221:
#line 1099 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 238:
#line 1125 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 243:
#line 1139 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1, yypvt[-1].s );
		}
		break;
	case 244:
#line 1144 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 246:
#line 1154 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 252:
#line 1170 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 255:
#line 1205 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 1215 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 257:
#line 1220 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 266:
#line 1237 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 1255 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 1275 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-2].s );
		    db_key( ERx("unique") );
		    db_key( ERx("index") );
		}
		break;
	case 272:
#line 1281 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-1].s );
		    db_key( ERx("index") );
		}
		break;
	case 278:
#line 1305 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 279:
#line 1310 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 1323 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( ERx("index") );
		}
		break;
	case 284:
#line 1333 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 285:
#line 1337 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-0].s );
		}
		break;
	case 287:
#line 1355 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 288:
#line 1359 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-0].s );
		}
		break;
	case 293:
#line 1371 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 1;
		}
		break;
	case 294:
#line 1375 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 0;
		}
		break;
	case 295:
#line 1380 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 304:
#line 1420 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 1431 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 1454 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 308:
#line 1460 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_func = IINEXTGET;
		}
		break;
	case 309:
#line 1464 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_func = IITUPGET;
		}
		break;
	case 313:
#line 1473 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 314:
#line 1478 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    ret_add( gr->gr_sym, gr->gr_id, gr->gr_nlsym, gr->gr_nlid,
				(char *)0, 0 );
		}
		break;
	case 315:
#line 1484 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    er_write( E_EQ0065_grRETREPEAT, EQ_ERROR, 0 );
		}
		break;
	case 317:
#line 1490 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_flag &= ~GR_TUPSORT;
		}
		break;
	case 318:
#line 1495 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_flag |= GR_TUPSORT;
		}
		break;
	case 323:
#line 1509 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 324:
#line 1514 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 326:
#line 1520 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    /* Bug #6749 - Need a space for run-time */
		    db_op(ERx(" "));
		}
		break;
	case 329:
#line 1532 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 331:
#line 1542 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		    gr->gr_func = DB_STRING;
		}
		break;
	case 340:
#line 1558 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_func = DB_ID;		/* ON tablename */
		}
		break;
	case 343:
#line 1565 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 1585 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sQUEL, yypvt[-0].s );
		}
		break;
	case 348:
#line 1603 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 0;
		}
		break;
	case 349:
#line 1607 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 1;
		}
		break;
	case 350:
#line 1611 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 0;
		}
		break;
	case 355:
#line 1623 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 361:
#line 1643 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = yypvt[-0].i;		/* Not really an expression */
		}
		break;
	case 362:
#line 1647 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 3;
		}
		break;
	case 375:
#line 1681 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 380:
#line 1692 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (STbcompare(yypvt[-0].s, 0, ERx("only"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
						    ERx("WHERE"), ERx("ONLY") );
		    db_key( yypvt[-0].s );
		}
		break;
	case 382:
#line 1710 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 0;
		}
		break;
	case 383:
#line 1714 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    /* Note that gr_sym will be left around after second name */
		    yyval.i = gr->gr_sym ? 2 : 1;
		}
		break;
	case 387:
#line 1723 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 1744 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = gr->gr_sym ? (char *)0 : yypvt[-0].s;
		    if (gr->gr_flag & GR_REPLCSR)
			er_write( E_EQ0413_csRANGEREPL, EQ_ERROR, 3, yypvt[-1].s,
							yypvt[-0].s, ecs_namecsr() );
		}
		break;
	case 389:
#line 1751 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = (char *)0;
		    if (gr->gr_flag & GR_REPLCSR)
			er_write( E_EQ0413_csRANGEREPL, EQ_ERROR, 3, yypvt[-1].s,
							yypvt[-0].s, ecs_namecsr() );
		}
		break;
	case 390:
#line 1759 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 1778 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 1796 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_var( DB_REG, gr->gr_sym, gr->gr_id, gr->gr_nlsym,
			    gr->gr_nlid, (char *)0 );
		}
		break;
	case 394:
#line 1803 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (gr->gr_flag & GR_REPEAT)
			rep_param();
		    else
			er_write( E_EQ0376_repARG, EQ_ERROR, 0 );
		}
		break;
	case 395:
#line 1814 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 1834 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_nlsym = (SYM *)0;	/* Initialize state vars */
		    gr->gr_nlid = (char *)0;
		    gr->gr_nltype = T_NONE; 
		}
		break;
	case 397:
#line 1840 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 1867 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 1880 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 400:
#line 1885 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 401:
#line 1890 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 402:
#line 1895 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 403:
#line 1900 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 404:
#line 1904 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 405:
#line 1908 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 406:
#line 1913 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 407:
#line 1918 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_op( ERx("!=") );
		}
		break;
	case 409:
#line 1930 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (gr->gr_flag & GR_REPLCSR)
			ecs_chkupd( NULL, NULL, NULL );		/* check csr */
		}
		break;
	case 414:
#line 1941 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 415:
#line 1946 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (gr->gr_flag & GR_REPLCSR)
			ecs_chkupd( NULL, yypvt[-0].s, gr->gr_sym );
		}
		break;
	case 421:
#line 1986 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_flag &= ~GR_CPYHNDDEF;
		}
		break;
	case 423:
#line 1992 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 2017 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 2030 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_sconst( yypvt[-0].s );
		}
		break;
	case 429:
#line 2036 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 2064 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 434:
#line 2076 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 435:
#line 2081 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 436:
#line 2086 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 437:
#line 2091 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_op(ERx(", "));	/* Need a space for run-time */
		}
		break;
	case 438:
#line 2096 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 439:
#line 2101 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 440:
#line 2112 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_op( ERx("=") );
		}
		break;
	case 441:
#line 2116 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_op( ERx("=") );
		}
		break;
	case 442:
#line 2128 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_op( ERx("=") );
		    er_write( E_EQ0071_grASSGNOP, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 443:
#line 2133 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_op( ERx("=") );
		}
		break;
	case 445:
#line 2147 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_op( ERx("=") );
		    er_write( E_EQ0071_grASSGNOP, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 448:
#line 2167 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 449:
#line 2172 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 450:
#line 2177 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 451:
#line 2182 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 452:
#line 2187 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 453:
#line 2192 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 454:
#line 2197 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 455:
#line 2202 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 456:
#line 2207 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 458:
#line 2213 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 459:
#line 2218 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 460:
#line 2223 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 464:
#line 2234 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 2262 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 471:
#line 2269 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 472:
#line 2276 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_hexconst(yypvt[-0].s);
		}
		break;
	case 475:
#line 2290 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 2325 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 480:
#line 2334 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 2356 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_sconst( yypvt[-0].s );
		}
		break;
	case 485:
#line 2365 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 486:
#line 2369 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 2390 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 2418 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_int_add( IIutEXE );
		    arg_str_add( ARG_CHAR, (char *)0 );
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_call( IIUTSYS );
		}
		break;
	case 489:
#line 2425 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    arg_int_add( IIutPROG );
		}
		break;
	case 490:
#line 2431 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_call( IIUTSYS );
		}
		break;
	case 492:
#line 2438 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 494:
#line 2445 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_int_add( IIutARG );
		}
		break;
	case 497:
#line 2453 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IIUTSYS );
		}
		break;
	case 499:
#line 2459 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 2463 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 501:
#line 2468 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_int_add( IIutARG );
		}
		break;
	case 502:
#line 2483 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 2507 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 2548 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    gen_call( IIEXIT );
		}
		break;
	case 505:
#line 2561 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( QAPRINTF );
		}
		break;
	case 506:
#line 2566 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		}
		break;
	case 510:
#line 2576 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (gr->gr_sym)
			arg_var_add( gr->gr_sym, gr->gr_id );
		    else
			arg_str_add( ARG_CHAR, gr->gr_id );
		}
		break;
	case 511:
#line 2583 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_str_add( ARG_INT, yypvt[-0].s );
		}
		break;
	case 512:
#line 2587 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_str_add( ARG_FLOAT, yypvt[-0].s );
		}
		break;
	case 513:
#line 2591 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_str_add( ARG_PACK, yypvt[-0].s );
		}
		break;
	case 514:
#line 2595 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 515:
#line 2607 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IIINGOPEN );
		}
		break;
	case 516:
#line 2612 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    arg_int_add( eq->eq_lang );
		}
		break;
	case 518:
#line 2627 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    gr->gr_func = IIEQINQ;
		}
		break;
	case 520:
#line 2642 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s );
		    gr->gr_func = IIEQSET;
		}
		break;
	case 521:
#line 2659 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 2672 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    ecs_declcsr( yypvt[-0].s, gr->gr_sym );   /* sets current cursor */
		    ecs_query( NULL, ECS_START_QUERY );	/* start saving query */
		}
		break;
	case 523:
#line 2679 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_flag |= GR_CURSOR|GR_DECLCSR;
		    gr_mechanism( GR_EQSTMT, GR_sCURSOR, yypvt[-0].s );
		    /* $$.s = $1.s; */
		}
		break;
	case 524:
#line 2688 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 2716 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (yypvt[-0].i)
			ecs_csrset( NULL, ECS_SORTED );
		}
		break;
	case 526:
#line 2723 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 527:
#line 2727 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		    ecs_csrset( NULL, ECS_UNIQUE );
		}
		break;
	case 529:
#line 2736 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    ecs_csrset( NULL, ECS_REPEAT );
		    gr->gr_flag |= GR_REPEAT;
		    rep_begin( ERx("repeat"), ecs_id_curcsr() );
		}
		break;
	case 530:
#line 2751 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 0;
		}
		break;
	case 531:
#line 2755 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    ecs_csrset( NULL, yypvt[-1].i );
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 532:
#line 2763 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 533:
#line 2770 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 534:
#line 2781 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 2792 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 2821 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 1;
		}
		break;
	case 537:
#line 2825 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 2;
		}
		break;
	case 542:
#line 2841 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 2888 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    er_write( E_EQ0411_csUPDALL, EQ_ERROR, 1, ecs_namecsr() );
		}
		break;
	case 544:
#line 2895 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_xsym = (SYM *)0;
		}
		break;
	case 545:
#line 2900 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 2940 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 2958 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 2969 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    _VOID_ ecs_colupd( NULL, ERx(""), ECS_ADD|ECS_ISWILD );
		}
		break;
	case 552:
#line 2973 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_key( yypvt[-0].s );
		    _VOID_ ecs_colupd( NULL, yypvt[-2].s, ECS_ADD|ECS_CONSTANT );
		}
		break;
	case 553:
#line 2991 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    ecs_open( yypvt[-1].s, gr->gr_id, gr->gr_sym, gr->gr_xsym?TRUE:FALSE );
		    gr->gr_xsym = (SYM *)0;	/* Borrowed from gr_nlsym */
		}
		break;
	case 554:
#line 2998 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_flag |= GR_CURSOR;
		    gr_mechanism( GR_EQSTMT, GR_sCURSOR, yypvt[-0].s );
		}
		break;
	case 555:
#line 3005 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_id = (char *)0;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 556:
#line 3010 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (STbcompare(yypvt[-0].s, 0, ERx("readonly"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					ERx("OPEN CURSOR"), ERx("READONLY") );
		    gr->gr_id = ERx(" for readonly ");
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 557:
#line 3018 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 3038 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    ecs_eretrieve( NULL, 0 );	/* Didn't use an SQLDA! */
		}
		break;
	case 559:
#line 3044 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    ecs_retrieve( yypvt[-0].s, gr->gr_sym != (SYM *)0, 0, 0,
					(SYM *) NULL, (char *) NULL );
		    gr->gr_func = IICSRETRIEVE;
		}
		break;
	case 560:
#line 3052 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_flag |= GR_CURSOR;
		    gr_mechanism( GR_EQSTMT, GR_sCURSOR, yypvt[-0].s );
		}
		break;
	case 565:
#line 3074 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (gr->gr_sym)
		    {
			ecs_get( gr->gr_id, gr->gr_sym, gr->gr_type,
				 gr->gr_nlid, gr->gr_nlsym, (char *)0 );
		    }
		}
		break;
	case 566:
#line 3091 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    ecs_replace( FALSE, NULL, FALSE );
		}
		break;
	case 567:
#line 3097 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    ecs_replace( TRUE, yypvt[-0].s, gr->gr_sym != (SYM *)0 );
		    gr->gr_flag |= GR_REPLCSR;
		}
		break;
	case 568:
#line 3104 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_flag |= GR_CURSOR;
		    gr_mechanism( GR_EQSTMT, GR_sCURSOR, yypvt[-0].s );
		}
		break;
	case 569:
#line 3117 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    ecs_delete( yypvt[-0].s, (char *)0, gr->gr_sym != (SYM *)0 );
		}
		break;
	case 570:
#line 3123 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_flag |= GR_CURSOR;
		    gr_mechanism( GR_EQSTMT, GR_sCURSOR, yypvt[-0].s );
		}
		break;
	case 571:
#line 3136 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    ecs_close( yypvt[-0].s, gr->gr_sym != (SYM *)0 );
		}
		break;
	case 572:
#line 3142 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_flag |= GR_CURSOR;
		    gr_mechanism( GR_EQSTMT, GR_sCURSOR, yypvt[-0].s );
		}
		break;
	case 573:
#line 3162 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IIADDFORM );
		    gr->gr_flag &= ~GR_NOINDIR;
		}
		break;
	case 574:
#line 3168 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    gr->gr_flag |= GR_NOINDIR;
		}
		break;
	case 575:
#line 3174 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 3193 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
# ifndef EQ_ADD_FNAME
		    er_write( E_EQ0061_grINTVAR, EQ_ERROR, 1, yypvt[-0].s );
# endif /* EQ_ADD_FNAME */
		}
		break;
	case 577:
#line 3209 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 3230 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-1].s );
		    gen_call( IICLRSCR );
		}
		break;
	case 579:
#line 3235 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IICLRFLDS );
		}
		break;
	case 581:
#line 3241 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-1].s );
		    gr->gr_func = IIFLDCLEAR;
		}
		break;
	case 582:
#line 3309 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 3342 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 3361 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 3380 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    gr->gr_func = IIDISPFRM;
		}
		break;
	case 586:
#line 3386 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    frs_mode( ERx("DISPLAY/INITTABLE"), yypvt[-0].s );
		}
		break;
	case 587:
#line 3390 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 3407 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_str_add( ARG_CHAR, ERx("f") );
		}
		break;
	case 589:
#line 3412 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    sc_eat( NULL, SC_NEST|SC_SEEN, ERx(")"), '(', ')' );
		    er_write( E_EQ0051_grDISPTL, EQ_ERROR, 0 );
		}
		break;
	case 591:
#line 3440 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 3455 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_if_goto( IIFSETIO, C_EQ, 0, L_FDEND,
			lbl_current(LBL_FORM) );
		    gr->gr_func = IISETFIELD;	/* In case of I/O */
		    gr->gr_mode = LBLmDISPLAY;
		}
		break;
	case 597:
#line 3499 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 3509 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_flag &= ~GR_ACTIVATE;
		}
		break;
	case 601:
#line 3517 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_else( G_ELSE, IIRETVAL, C_0, 0, L_ACTELSE, 
			lbl_current(LBL_INTR), L_ACTEND, lbl_current(LBL_FORM));
		    gr->gr_mode = LBLmDISPLAY;
		}
		break;
	case 602:
#line 3524 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 3535 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    /* Reset activate argument list */
		    act_args( FALSE, (char *)0, (char *)0, 0, (SYM *)0 );
		}
		break;
	case 604:
#line 3540 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    act_args( FALSE, (char *)0, (char *)0, 0, (SYM *)0 );
		}
		break;
	case 607:
#line 3553 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    act_add( ACT_FIELD, lbl_current(LBL_INTR), yypvt[-2].i, yypvt[-0].ip );
		    gr->gr_adjective = LBLaFIELD;
		}
		break;
	case 608:
#line 3558 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    act_add( ACT_MENU, lbl_current(LBL_INTR), 0, yypvt[-1].ip );
		    gr->gr_adjective = LBLaMITEM;
		}
		break;
	case 609:
#line 3563 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    act_add( ACT_SCROLL, lbl_current(LBL_INTR), sclUP, yypvt[-0].ip );
		    gr->gr_adjective = LBLaSCROLL;
		}
		break;
	case 610:
#line 3568 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    act_add( ACT_SCROLL,lbl_current(LBL_INTR), sclDOWN, yypvt[-0].ip );
		    gr->gr_adjective = LBLaSCROLL;
		}
		break;
	case 611:
#line 3573 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    i4	cur_intr = lbl_current( LBL_INTR );

		    act_add( ACT_COLUMN, cur_intr, yypvt[-3].i, yypvt[-1].ip );
		    act_add( ACT_COLUMN, cur_intr, yypvt[-3].i, yypvt[-0].ip );
		    gr->gr_adjective = LBLaCOLUMN;
		}
		break;
	case 612:
#line 3581 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    /* Old-style syntax - no longer supported */
                    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1,
                                            ERx("ACTIVATE COMMAND") );
		}
		break;
	case 614:
#line 3589 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 0;
		}
		break;
	case 615:
#line 3593 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 3608 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.ip = act_string( yypvt[-0].s );
		}
		break;
	case 617:
#line 3612 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.ip = act_string( yypvt[-0].s );
		}
		break;
	case 618:
#line 3616 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 3629 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 620:
#line 3633 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 621:
#line 3644 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 3685 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 3704 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 3718 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 3734 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 0;
		}
		break;
	case 626:
#line 3738 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 1;
		}
		break;
	case 629:
#line 3746 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    /* It will check for errors */
		    act_args( TRUE, yypvt[-2].s, yypvt[-0].s, gr->gr_type, gr->gr_sym );
		}
		break;
	case 630:
#line 3752 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 631:
#line 3756 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
	    	}
		break;
	case 632:
#line 3760 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
	    	}
		break;
	case 633:
#line 3764 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
	    	}
		break;
	case 634:
#line 3769 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = NULL;
	    	}
		break;
	case 635:
#line 3775 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_INT;
		    gr->gr_sym = NULL;
	    	}
		break;
	case 636:
#line 3781 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 3806 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_goto( G_TERM, L_FDEND, lbl_current(LBL_FORM) );
		}
		break;
	case 638:
#line 3811 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 3829 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_if_goto( IIFSETIO, C_EQ, 0, L_FDEND,
			lbl_current(LBL_FORM) );
		    gr->gr_func = IIRETFIELD;
		}
		break;
	case 642:
#line 3845 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 3866 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 3886 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    gen_call( IIENDFORMS );
		}
		break;
	case 645:
#line 3916 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    i4	cur_form = lbl_current( LBL_FORM );

		    gen_loop( G_CLOSE, L_FDBEG, L_FDEND, cur_form, IIFNAMES,
			C_0, 0 );
		    gen_label( G_LOOP, L_FDEND, cur_form );
		    gen_call( IIENDFRM );
		}
		break;
	case 646:
#line 3926 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 3939 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 648:
#line 3944 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (yypvt[-0].i)
			er_write( E_EQ0077_grISUNSUPP, EQ_ERROR, 1,
							    ERx("FORMDATA") );
		}
		break;
	case 650:
#line 3960 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    gr->gr_func = IIFORMINIT;
		}
		break;
	case 651:
#line 3973 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( eq->eq_lang );
		    gen_call( IIFORMS );
		}
		break;
	case 652:
#line 3989 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		}
		break;
	case 653:
#line 3994 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_if( G_OPEN, IIFSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IIRETFIELD;
		}
		break;
	case 654:
#line 4000 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 655:
#line 4020 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1,
						    ERx("GETOPER") );
		}
		break;
	case 656:
#line 4027 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_if( G_OPEN, IIFSETIO, C_NOTEQ, 0 );
		}
		break;
	case 657:
#line 4032 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 661:
#line 4042 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
			gen_call( IIGETQRY );
		}
		break;
	case 662:
#line 4047 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 4074 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( 0 );		/* Dummy type right now */
		}
		break;
	case 665:
#line 4080 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 4091 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 667:
#line 4105 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IIHELPFILE );
		}
		break;
	case 668:
#line 4110 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 669:
#line 4122 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( QAMESSAGE );
		}
		break;
	case 670:
#line 4127 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 671:
#line 4139 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( QAPROMPT );
		}
		break;
	case 672:
#line 4144 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 673:
#line 4164 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    frs_close();
		}
		break;
	case 675:
#line 4171 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    frs_head();
		}
		break;
	case 676:
#line 4176 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    frs_inqset( FRSinq );
		}
		break;
	case 677:
#line 4182 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 678:
#line 4186 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 679:
#line 4190 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 680:
#line 4194 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 683:
#line 4203 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    frs_parentname( yypvt[-0].s, T_CHAR, (PTR)0 );
		}
		break;
	case 684:
#line 4207 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    frs_parentname( yypvt[-0].s, T_INT, (PTR)0 );
		}
		break;
	case 685:
#line 4211 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 4237 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    er_write( E_EQ0077_grISUNSUPP, EQ_ERROR, 1,
						ERx("INQUIRE_FRS") );
		    YYERROR;
		}
		break;
	case 691:
#line 4249 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    frs_iqvar( gr->gr_id, gr->gr_type, gr->gr_sym,
				gr->gr_nlid, gr->gr_nlsym );
		}
		break;
	case 692:
#line 4255 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    frs_constant( yypvt[-0].s, (char *)0, T_NONE, (SYM *)0 );
		}
		break;
	case 693:
#line 4259 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    er_write( E_EQ0054_grNOPARENS, EQ_ERROR, 0 );
		    frs_constant( yypvt[-2].s, (char *)0, T_NONE, (SYM *)0 );
		}
		break;
	case 694:
#line 4264 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    /* Should be an error */
		    frs_constant( yypvt[-3].s, yypvt[-1].s, gr->gr_type, gr->gr_sym );
		}
		break;
	case 695:
#line 4271 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 696:
#line 4275 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 697:
#line 4279 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 698:
#line 4283 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 699:
#line 4287 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 700:
#line 4291 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 701:
#line 4295 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 702:
#line 4300 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 703:
#line 4306 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_INT;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 704:
#line 4312 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 705:
#line 4318 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 706:
#line 4324 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 707:
#line 4330 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 708:
#line 4336 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 709:
#line 4342 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 710:
#line 4348 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 711:
#line 4354 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 4427 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IIMESSAGE ); 
    		}
		break;
	case 713:
#line 4432 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
            	    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
    	    	    gr->gr_func = IIMESSAGE;
    		}
		break;
	case 716:
#line 4455 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    EQFW3glgen();	
		}
		break;
	case 717:
#line 4460 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    EQFWopen(gr->gr_func);
		}
		break;
	case 721:
#line 4475 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    EQFWwcoption(yypvt[-2].s, yypvt[-0].s);
		}
		break;
	case 722:
#line 4480 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 723:
#line 4484 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 724:
#line 4489 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 725:
#line 4493 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 730:
#line 4518 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		EQFWsuboption(yypvt[-2].s, gr->gr_type, gr->gr_id, (PTR)gr->gr_sym);
	}
		break;
	case 731:
#line 4523 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
	    	    yyval.s = yypvt[-0].s;
	        }
		break;
	case 732:
#line 4527 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 733:
#line 4533 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_INT;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 734:
#line 4539 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_CHAR;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 735:
#line 4545 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_FLOAT;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 736:
#line 4551 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_PACKASCHAR;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 737:
#line 4557 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (!gr->gr_sym)
		    {
			gr->gr_type = T_CHAR;
		    }	
		}
		break;
	case 738:
#line 4571 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IIPRNSCR );
		}
		break;
	case 739:
#line 4576 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 740:
#line 4581 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    /* Should check that $2.s is "file" */
		    if (STbcompare(yypvt[-3].s, 0, ERx("file"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-3].s,
					ERx("PRINTSCREEN"), ERx("FILE") );
		}
		break;
	case 741:
#line 4588 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		}
		break;
	case 742:
#line 4593 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 743:
#line 4613 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IINPROMPT );
		}
		break;
	case 744:
#line 4618 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    gr->gr_func = IINPROMPT;
		}
		break;
	case 745:
#line 4624 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (STbcompare(yypvt[-0].s, 0, ERx("noecho"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
						ERx("PROMPT"), ERx("NOECHO") );
		    arg_int_add( 1 );
		}
		break;
	case 746:
#line 4631 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_int_add( 0 );
		}
		break;
	case 747:
#line 4636 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 4675 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		}
		break;
	case 749:
#line 4680 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_if( G_OPEN, IIFSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IISETFIELD;
		}
		break;
	case 750:
#line 4686 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 751:
#line 4698 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    gen_call( IIREDISP );
		}
		break;
	case 752:
#line 4719 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 4732 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 755:
#line 4739 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 4764 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IIRESFLD );
		}
		break;
	case 757:
#line 4768 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IIRESCOL );
		}
		break;
	case 758:
#line 4772 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 4838 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 760:
#line 4853 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    frs_close();
		}
		break;
	case 762:
#line 4860 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    frs_inqset( FRSset );
		}
		break;
	case 764:
#line 4874 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    er_write( E_EQ0077_grISUNSUPP, EQ_ERROR, 1,
						    ERx("SET_FRS") );
		    YYERROR;
		}
		break;
	case 768:
#line 4886 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    frs_setval( form_sconst(yypvt[-0].s), T_CHAR, (SYM *)0,
				(char *)0, (SYM *)0 );
		}
		break;
	case 769:
#line 4891 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    frs_setval( yypvt[-0].s, T_INT, (SYM *)0, (char *)0, (SYM *)0 );
		}
		break;
	case 770:
#line 4895 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    frs_setval(ERx("0"), T_INT, (SYM *)0, (char *)0, (SYM *)0);
		}
		break;
	case 771:
#line 4900 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    frs_setval(ERx("0"), T_INT, (SYM *)0, (char *)0, (SYM *)0);
		}
		break;
	case 772:
#line 4905 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 4937 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    frs_setval( ERx("1"), T_INT, (SYM *)0, (char *)0,
				(SYM *)0 );
		}
		break;
	case 774:
#line 4964 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IISLEEP );
		}
		break;
	case 775:
#line 4969 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 776:
#line 5002 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 5018 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 5034 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_else( G_ELSE, IIRETVAL, C_0, 0, L_ACTELSE, 
			lbl_current(LBL_INTR), L_ACTEND, 
			lbl_current(LBL_FORM) );
		    gr->gr_mode = LBLmSUBMENU;
		}
		break;
	case 781:
#line 5042 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_mode = LBLmSUBMENU;	/* Not LBLmDISPLAY */
		}
		break;
	case 782:
#line 5046 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_mode = LBLmSUBMENU;	/* Not LBLmDISPLAY */
		}
		break;
	case 783:
#line 5051 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    act_add( ACT_MENU, lbl_current(LBL_INTR), 0, yypvt[-1].ip );
		}
		break;
	case 785:
#line 5069 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 5083 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 787:
#line 5088 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_func = IICHKFRM;
		}
		break;
	case 788:
#line 5092 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_func = IIVALFLD;
		}
		break;
	case 789:
#line 5114 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 790:
#line 5120 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IITCLRCOL;
		}
		break;
	case 791:
#line 5126 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( cmCLEARR );
		}
		break;
	case 793:
#line 5133 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IITCLRROW );
		}
		break;
	case 794:
#line 5137 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IITCLRROW );
		}
		break;
	case 795:
#line 5158 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    /* Close inner and deleterow if blocks */
		    gen_if( G_CLOSE, IITDELROW, C_0, 0 );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 796:
#line 5165 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLRET;	/* May be an Out list */
		}
		break;
	case 797:
#line 5171 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( cmDELETER );
		}
		break;
	case 799:
#line 5178 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (eq->eq_flags & EQ_VERSWARN)
			er_write( E_EQ0078_grSTYUNSUPP, EQ_WARN, 1,
						    ERx("DELETEROW ... OUT") );
		}
		break;
	case 800:
#line 5185 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_int_add( FALSE );
		    gen_if( G_OPEN, IITDELROW, C_NOTEQ, 0 );
		}
		break;
	case 801:
#line 5190 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (eq->eq_flags & EQ_VERSWARN)
			er_write( E_EQ0078_grSTYUNSUPP, EQ_WARN, 1,
						    ERx("DELETEROW ... IN") );
		}
		break;
	case 802:
#line 5197 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_int_add( TRUE );	/* In list */
		    gen_if( G_OPEN, IITDELROW, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 803:
#line 5215 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 804:
#line 5221 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLRET;
		}
		break;
	case 805:
#line 5227 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( cmGETR );
		}
		break;
	case 806:
#line 5245 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IITFILL );
		    gen_if( G_CLOSE, IITBINIT, C_0, 0 );
		}
		break;
	case 807:
#line 5251 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_if( G_OPEN, IITBINIT, C_NOTEQ, 0 );
		}
		break;
	case 808:
#line 5256 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 812:
#line 5265 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_call( IITHIDECOL );
		}
		break;
	case 816:
#line 5282 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{ gr->gr_sym = (SYM *)0; }
		break;
	case 817:
#line 5284 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 5299 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 0;		/* Paren not stuff present */
		}
		break;
	case 820:
#line 5303 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    id_add( yypvt[-2].s );
		    id_add( yypvt[-1].s );
		    id_add( yypvt[-0].s );
		    yyval.i = 1;		/* Paren stuff present */
		}
		break;
	case 821:
#line 5310 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 5320 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    id_add( form_sconst(yypvt[-0].s) );
		}
		break;
	case 823:
#line 5324 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 5346 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 0;		/* No NULL clause */
		}
		break;
	case 825:
#line 5350 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    id_add( ERx(" with null") );
		    yyval.i = 1;		/* With NULL clause */
		}
		break;
	case 826:
#line 5355 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (yypvt[-0].i == 0)
			id_add( ERx(" not null") );
		    else
			id_add( ERx(" not null with default") );
		    yyval.i = 1;		/* With NULL clause */
		}
		break;
	case 827:
#line 5364 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (STbcompare(yypvt[-1].s, 0, ERx("not"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
						ERx("INITTABLE"), ERx("NOT") );
		}
		break;
	case 828:
#line 5371 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 1;
		    if (STbcompare(yypvt[-0].s, 0, ERx("default"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					    ERx("INITTABLE"), ERx("DEFAULT") );
		}
		break;
	case 829:
#line 5378 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 0;
		}
		break;
	case 830:
#line 5393 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 5405 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmPURGE );
		}
		break;
	case 832:
#line 5428 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    /* Close inner and insertrow if blocks */
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITINSERT, C_0, 0 );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 833:
#line 5436 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gen_if( G_OPEN, IITINSERT, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 834:
#line 5443 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( cmINSERTR );
		}
		break;
	case 835:
#line 5466 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBACT, C_0, 0 );
		}
		break;
	case 836:
#line 5472 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_int_add( TRUE );		/* Loading */
		    gen_if( G_OPEN, IITBACT, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 837:
#line 5479 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 840:
#line 5487 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    frs_inqset(FRSload);
		    frs_object(ERx("row"));
		}
		break;
	case 841:
#line 5504 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 842:
#line 5510 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 843:
#line 5516 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( cmPUTR );
		}
		break;
	case 844:
#line 5542 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    /* Close inner and scroll if blocks */
		    gen_if( G_CLOSE, IITSCROLL, C_0, 0 );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 845:
#line 5549 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_int_add( rowNONE );
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		}
		break;
	case 846:
#line 5555 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( cmSCROLL );
		}
		break;
	case 847:
#line 5561 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_if( G_OPEN, IITSCROLL, C_NOTEQ, 0 );
		}
		break;
	case 848:
#line 5565 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if ((yypvt[-1].i || yypvt[-0].i) && (eq->eq_flags & EQ_VERSWARN))
			er_write( E_EQ0078_grSTYUNSUPP, EQ_WARN, 1,
							ERx("SCROLL IN/OUT") );
		}
		break;
	case 849:
#line 5572 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gen_call( IITBSMODE );
		    arg_int_add( FALSE );	/* Ignore In flag */
		}
		break;
	case 851:
#line 5580 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_int_add( rowEND );
		}
		break;
	case 852:
#line 5585 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gen_call( IITBSMODE );
		}
		break;
	case 853:
#line 5590 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gen_call( IITBSMODE );
		}
		break;
	case 854:
#line 5596 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 0;
		}
		break;
	case 855:
#line 5600 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 1;
		}
		break;
	case 856:
#line 5605 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_func = IITCOLRET;
		}
		break;
	case 857:
#line 5610 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_int_add( FALSE );
		    arg_int_add( rowNONE );
		    gen_if( G_OPEN, IITSCROLL, C_NOTEQ, 0 );
		    yyval.i = 0;
		}
		break;
	case 858:
#line 5617 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 1;
		}
		break;
	case 859:
#line 5622 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_int_add( TRUE );	/* In list */
		    arg_int_add( rowNONE );
		    gen_if( G_OPEN, IITSCROLL, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 860:
#line 5644 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 5655 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 5685 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 5696 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IITBCECOLEND );
		}
		break;
	case 864:
#line 5701 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 5716 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		}
		break;
	case 866:
#line 5737 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 5754 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{	
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLVAL;
		    yyval.s = yypvt[-3].s;
		}
		break;
	case 868:
#line 5761 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s );
		    arg_int_add( cmVALIDR );
		}
		break;
	case 870:
#line 5768 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IITVALROW );
		}
		break;
	case 871:
#line 5772 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IITVALROW );
		}
		break;
	case 872:
#line 5784 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = form_sconst(yypvt[-0].s);
		}
		break;
	case 873:
#line 5792 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 874:
#line 5796 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 5819 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 877:
#line 5828 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 5847 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 879:
#line 5851 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 882:
#line 5868 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 883:
#line 5872 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 5889 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 885:
#line 5893 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 5910 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 887:
#line 5914 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 5931 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_str_add( ARG_INT, yypvt[-0].s );
		}
		break;
	case 889:
#line 5935 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    arg_int_add( 0 );
		}
		break;
	case 890:
#line 5940 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    arg_int_add( 0 );
		}
		break;
	case 891:
#line 5945 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 5978 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 900:
#line 5982 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 6009 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 902:
#line 6017 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 6039 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_int_add( 0 );		/* No null indicator */
		    arg_str_add( ARG_INT, yypvt[-0].s );
		    gr->gr_type = T_INT;
		}
		break;
	case 904:
#line 6045 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_int_add( 0 );		/* No null indicator */
		    arg_str_add( ARG_FLOAT, yypvt[-0].s );
		    gr->gr_type = T_FLOAT;
		}
		break;
	case 905:
#line 6051 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_int_add( 0 );		/* No null indicator */
		    arg_str_add( ARG_PACK, yypvt[-0].s );
		    gr->gr_type = T_PACKASCHAR;
		}
		break;
	case 906:
#line 6057 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_int_add( 0 );		/* No null indicator */
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gr->gr_type = T_CHAR;
		}
		break;
	case 907:
#line 6072 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{	
		    arg_int_add( 0 );
		    arg_var_add( gr->gr_null, ERx("null") ); 
		}
		break;
	case 908:
#line 6088 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 1;
		}
		break;
	case 909:
#line 6092 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 1;
		}
		break;
	case 910:
#line 6096 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 0;
		}
		break;
	case 915:
#line 6107 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 916:
#line 6111 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 6132 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
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
#line 6145 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_int_add( rowCURR );
		}
		break;
	case 921:
#line 6153 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    if (lbl_gen(LBLmDISPLAY, FALSE) == LBL_NOLEV)
			er_write( E_EQ0056_grFORMNAME, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 922:
#line 6162 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    er_write( E_EQ0071_grASSGNOP, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 930:
#line 6228 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IIPARSET );
		}
		break;
	case 931:
#line 6233 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IIPARRET );
		}
		break;
	case 932:
#line 6238 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    db_send();
		    sc_eat( id_add, SC_STRIP|SC_NEST|SC_SEEN, ERx(")"),
								    '(', ')' );
		    id_free();
		    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1, ERx("PARAM") );
		}
		break;
	case 933:
#line 6252 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IIPARRET );
		}
		break;
	case 934:
#line 6257 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_call( IIPARSET );
		}
		break;
	case 935:
#line 6262 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    sc_eat( id_add, SC_STRIP|SC_NEST|SC_SEEN, ERx(")"),
								    '(', ')' );
		    id_free();
		    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1, ERx("PARAM") );
		}
		break;
	case 936:
#line 6277 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 937:
#line 6289 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if ((gr->gr_flag & GR_HOSTCODE) == 0)
		    {
			gen_line( ERx("host code") );
			gr->gr_flag |= GR_HOSTCODE;
		    }
		    gen_host( G_H_CODE, yypvt[-0].s );
		    gr->A_var = AvarDEC;	/* Maybe dec's to follow */
		}
		break;
	case 958:
#line 6365 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->A_blk++;
		    sym_s_begin( A_CLOSURE );	/* ADA closure is always 0 */
		    gr->A_var = AvarDEC;	/* On entry expect declares */
		    gen_do_indent( 1 );
		}
		break;
	case 959:
#line 6374 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (gr->A_blk > 1)
			_VOID_ sym_s_end( gr->A_blk-- );
		  /*
		  ** On ADA block exit set decl flag
		  ** -- may be immediate decl names.
		  ** QUEL statements start with a keyword which will switch
		  ** to USE mode.
		  */
		    gr->A_var = AvarDEC;
		    gen_do_indent( -1 );
		}
		break;
	case 966:
#line 6404 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->A_val = 0;		/* not really needed */
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 967:
#line 6411 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    ADA_DECLARE( yypvt[-4].i, (yypvt[-2].i ? syFisCONST:syFisVAR), yypvt[-1].v );
		}
		break;
	case 968:
#line 6417 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 1;		/* default is IN */
		}
		break;
	case 969:
#line 6421 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 1;
		}
		break;
	case 970:
#line 6425 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 0;
		}
		break;
	case 971:
#line 6429 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 0;
		}
		break;
	case 973:
#line 6437 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    sc_eat( gen_code, SC_BACK|SC_NEST, ERx(");"), '(', ')' );
		}
		break;
	case 975:
#line 6447 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    sc_eat( gen_code, SC_NEST, ERx(")"), '(', ')' );
		    gen_host( G_H_NEWLINE, NULL );
		    er_write( E_E60013_haTYPUNSUPP, EQ_ERROR, 1,
							    ERx("SEPARATE") );
		}
		break;
	case 980:
#line 6469 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_NEWLINE, NULL );
		}
		break;
	case 981:
#line 6475 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (STbcompare(yypvt[-0].s,0,ERx("equel"),0,TRUE)==0)
		    {
			gr->A_with |= AwithEQUEL;
			gen_declare( FALSE );
		    } else if (STbcompare(yypvt[-0].s,0,ERx("equel_forms"),0,TRUE)==0)
		    {
			gr->A_with |= AwithEQUEL|AwithFORMS;
			gen_declare( TRUE );
		    } else
		    {
			gen_host( G_H_KEY, yypvt[-1].s );
			gen_host( G_H_KEY, yypvt[-0].s );
			sc_eat( gen_code, SC_BACK, ERx(";"), '(', ')' );
			gen_host( G_H_OP, ERx(";") );
		    }
		}
		break;
	case 982:
#line 6496 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_NEWLINE, NULL );
		}
		break;
	case 983:
#line 6502 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if ((STbcompare(yypvt[-0].s,0,ERx("equel"),0,TRUE) != 0) &&
		        (STbcompare(yypvt[-0].s,0,ERx("equel_forms"),0,TRUE) != 0))
		    {
			gen_host( G_H_KEY, yypvt[-1].s );
			gen_host( G_H_KEY, yypvt[-0].s );
			sc_eat( gen_code, SC_BACK, ERx(";"), '(', ')' );
			gen_host( G_H_OP, ERx(";") );
		    }
		}
		break;
	case 992:
#line 6546 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    SYM	*sy;

		    sy = ADA_DECLARE(yypvt[-5].i, (yypvt[-3].i ? syFisCONST:syFisVAR), yypvt[-2].v);
		    if (sy)
		    {
			if ((i4)sym_g_vlue(sy) & AvalCHAR)
			{
			    /*
			    ** The following #if-block mimics what 5.0 did
			    ** (except that it complains on multi-dimensional
			    ** arrays too).  The #else-block simply decrements
			    ** the dims-count, allowing a properly indexed
			    ** (multi-)array of chars to work.  It would require
			    ** such arrays to be coerced to STRINGs.
			    ** I don't see why this wouldn't work, but I seem
			    ** to have taken pains in 5.0 to disallow it.
			    ** If I have time I'll check this out.  In the
			    ** meantime, better safe than sorry. (mrw)
			    */
#if 1
			    if (sym_g_dims(sy) != 1)
				er_write( E_E60004_haCHARDIMS, EQ_ERROR, 1,
							    sym_str_name(sy) );
			    else
				sym_s_dims( sy, 0 );
#else
			    if (sym_g_dims(sy) == 0)
				er_write( E_E60004_haCHARDIMS, EQ_ERROR, 1,
							    sym_str_name(sy) );
			    else
				sym_s_dims( sy, sym_g_dims(sy)-1 );
#endif
			}
			if (yypvt[-3].i && !yypvt[-1].i)
			    er_write( E_E60009_haNOTINITED, EQ_ERROR, 1,
							    sym_str_name(sy) );
		    }
		}
		break;
	case 993:
#line 6587 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    SYM	*sy;

		    sy = ADA_DECLARE(yypvt[-5].i, (yypvt[-3].i ? syFisCONST:syFisVAR), yypvt[-2].v);
		    if (sy)
		    {
			if ((i4)sym_g_vlue(sy) & AvalCHAR)
			{
			    /*
			    ** See the comment block above for info on
			    ** this #if-#else block. (mrw)
			    */
#if 1
			    sym_s_dims( sy, 0 );	/* not an array */
#else
			    i4	dims = sym_g_dims(sy);

			    if (dims > 0)
				sym_s_dims( sy, dims-1 );
#endif
			}
			if (yypvt[-3].i && !yypvt[-1].i)
			    er_write( E_E60009_haNOTINITED, EQ_ERROR, 1,
							    sym_str_name(sy) );
		    }
		}
		break;
	case 994:
#line 6616 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 1;
		}
		break;
	case 995:
#line 6620 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = yypvt[-2].i + 1;
		}
		break;
	case 996:
#line 6626 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    ada_push( yypvt[-0].s );
		}
		break;
	case 997:
#line 6632 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 0;
		}
		break;
	case 998:
#line 6636 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 1;
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
#line 6649 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    register SYM	*sy;

		    sy = sym_resolve( (SYM *)0, yypvt[-0].s, A_CLOSURE, syFisTYPE );
		    if (!sy)
		    {
			er_write( E_E60003_haBADTYPE, EQ_ERROR, 1, yypvt[-0].s );
			gr->A_type = T_UNDEF;
			sy = gr->A_undefsy;
		    } else
		    {
			gr->A_type = sym_g_btype(sy);
			gr->A_size = sym_g_dsize(sy);
			if (((i4)sym_g_vlue(sy)) & AvalUNSUPP)
			    er_write( E_E60013_haTYPUNSUPP, EQ_ERROR, 1, yypvt[-0].s );
		    }
		    yyval.v = sy;
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1007:
#line 6683 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (yypvt[-1].i > 0 && yypvt[-1].i <= 6)
			yyval.v = gr->A_fltsy;
		    else if (yypvt[-1].i >= 7 && yypvt[-1].i <= 16)
			yyval.v = gr->A_dblsy;
		    else
		    {
			er_write( E_E60005_haDIGITS, EQ_ERROR, 1, yypvt[-1].i );
			yyval.v = gr->A_fltsy;
		    }
		}
		break;
	case 1011:
#line 6707 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    sc_eat( gen_code, SC_SEEN|SC_NEST|SC_STRIP, ERx(")"),
								    '(', ')' );
		    gen_host( G_H_OP, ERx(")") );
		}
		break;
	case 1013:
#line 6720 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->A_dims++;
		}
		break;
	case 1014:
#line 6726 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.v = yypvt[-0].v;
		}
		break;
	case 1015:
#line 6732 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 0;
		}
		break;
	case 1016:
#line 6736 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 1;
		}
		break;
	case 1017:
#line 6742 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    sc_eat( gen_code, SC_BACK, ERx(";"), '(', ')' );
		}
		break;
	case 1018:
#line 6757 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    ADA_DECLARE( yypvt[-4].i, syFisCONST, gr->A_intsy );
		}
		break;
	case 1023:
#line 6780 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (yypvt[-0].v)		/* non-enums */
		    {
			register SYM	*sy;

			sy = ADA_DECLARE( 1, syFisTYPE, yypvt[-0].v );
		      /* access types will have a btype of T_UNDEF */
			if (gr->A_val & AvalACCESS)
			    sym_s_btype( sy, T_FORWARD );
		    }
		}
		break;
	case 1026:
#line 6808 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.v = (SYM *)0;	/* flag as enum -- already declared */
		}
		break;
	case 1034:
#line 6827 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    /*
		    ** The name being declared is that of the type
		    ** (LHS of "full_type_decl").
		    ** Sets $$.v for Aenum_end.
		    */
		    gr->A_type = T_INT;
		    gr->A_val |= AvalENUM;
		    yyval.v = ADA_DECLARE( 1, syFisTYPE, gr->A_bytesy );
		}
		break;
	case 1035:
#line 6840 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		  /* Uses $0.v from Aenum_begin */
		    gr->A_type = T_INT;
		    if (yypvt[-1].i > 0)
			ADA_DECLARE( yypvt[-1].i, syFisCONST, yypvt[-2].v );
		}
		break;
	case 1037:
#line 6851 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = yypvt[-2].i + yypvt[-0].i;
		}
		break;
	case 1038:
#line 6857 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 1;
		}
		break;
	case 1039:
#line 6861 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.i = 0;
		}
		break;
	case 1040:
#line 6871 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->A_type = T_INT;
		    switch (yypvt[-0].i)
		    {
		      case 1:
			yyval.v = gr->A_bytesy;
			break;
		      case 2:
			yyval.v = gr->A_shrtsy;
			break;
		      case 4:
		      default:
			yyval.v = gr->A_intsy;
			break;
		    }
		}
		break;
	case 1041:
#line 6891 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    i4		m, n;

		    m = ada_sizeof( yypvt[-2].i );
		    n = ada_sizeof( yypvt[-0].i );
		    yyval.i = max( m, n );
		}
		break;
	case 1042:
#line 6906 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.v = yypvt[-0].v;
		}
		break;
	case 1043:
#line 6910 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.v = gr->A_fltsy;
		    er_write( E_E60013_haTYPUNSUPP, EQ_ERROR, 1, ERx("FIXED") );
		}
		break;
	case 1047:
#line 6937 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->A_rec--;
		    gr->A_type = T_STRUCT;
		    gen_host( G_H_KEY|G_H_OUTDENT, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		  /* { $$.v = $1.v; } */
		}
		break;
	case 1048:
#line 6947 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    register SYM	*sy;

		    sy = yyval.v = symDcEuc( ERx(""), gr->A_rec, gr->A_blk,
						syFisTAG, A_CLOSURE, SY_TAG );
		    if (sy)
			sym_s_btype( sy, T_STRUCT );
		    gr->A_rec++;
		    gr->A_val = 0;
		}
		break;
	case 1056:
#line 6979 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    ADA_DECLARE( yypvt[-4].i, syFisVAR, yypvt[-2].v );
		}
		break;
	case 1058:
#line 6986 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_flag |= GR_HOSTCODE;
		    gen_host( G_H_CODE, yypvt[-0].s );
		    gr->A_var = AvarDEC;	/* Maybe dec's to follow */
		}
		break;
	case 1061:
#line 7000 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_NEWLINE|G_H_INDENT, NULL );
		}
		break;
	case 1062:
#line 7007 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_OUTDENT, NULL );
		}
		break;
	case 1073:
#line 7042 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.v = yypvt[-0].v;
		    gr->A_val |= AvalACCESS;
		}
		break;
	case 1074:
#line 7054 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.v = yypvt[-0].v;
		}
		break;
	case 1075:
#line 7064 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    register SYM	*sy;

		    sy = ADA_DECLARE( 1, syFisTYPE|syFisFORWARD, (SYM *)0 );
		    sym_s_btype( sy, T_FORWARD );
		}
		break;
	case 1076:
#line 7075 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    register SYM	*sy;

		    sy = ADA_DECLARE( 1, syFisTYPE|syFisFORWARD, (SYM *)0 );
		    sym_s_btype( sy, T_FORWARD );
		}
		break;
	case 1079:
#line 7092 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    ADA_DECLARE( 1, syFisTYPE, yypvt[-1].v );
		}
		break;
	case 1083:
#line 7140 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    SYM		*sy, *typsy;

		    if (yypvt[-1].v)
		    {
			i4		dims = gr->A_dims;

		      /* Fake a syntax error if "CONSTANT" appears */
			if (yypvt[-4].i > 0)
			    er_write( E_EQ0244_yySYNWRD, EQ_ERROR, 1,
							    ERx("CONSTANT") );
			if (yypvt[-6].i > 1)
			    er_write( E_E60010_haRNMMANY, EQ_ERROR, 0 );

		      /* ADA_DECLARE uses A_dims, which is set by Arename_var */
			dims = gr->A_dims;
			gr->A_dims = 0;
			sy = ADA_DECLARE( yypvt[-6].i, syFisVAR, sym_g_type(yypvt[-1].v) );

			typsy = yypvt[-3].v;
			if (typsy)
			{
			  /* Base type, dims, and element size must match */
			    if (sym_g_btype(yypvt[-1].v) != sym_g_btype(typsy) ||
			        sym_g_dims(yypvt[-1].v) != (sym_g_dims(typsy)+dims) ||
			        sym_g_dsize(yypvt[-1].v) != sym_g_dsize(typsy))
			    {
				er_write(E_E6000F_haRNMCOMPAT, EQ_ERROR, 0);
			    }

			    /*
			    ** A subscripted rename-var is not an array
			    ** even if the renamed-object is.
			    */
			    if (sy && dims && sym_g_dims(yypvt[-1].v))
				sym_s_dims( sy, 0 );
			}
		    } else
			ADA_DECLARE( yypvt[-6].i, syFisVAR, gr->A_undefsy );
		    id_free();		/* rename var name */
		}
		break;
	case 1084:
#line 7184 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    SYM		*sy;
		    i4		stat;
		    char	*name;

		    stat = symRsEuc( &sy, A_CLOSURE, syFisVAR|syFisCONST );
		    name = str_add( STRNULL, id_getname() );
		    /* id_free is done above in Arename_decl */

		    gen_host( G_H_KEY, name );

		    switch (stat)
		    {
		      case syL_OK:
			yyval.v = sy;
			break;
		      case syL_NO_NAMES:
		      case syL_BAD_REF:
		      case syL_NOT_FOUND:
		      case syL_RECURSIVE:
		      default:
			er_write( E_E6000E_haRNMBAD, EQ_ERROR, 1, name );
			yyval.v = (SYM *)0;
			gr->A_dims = 0;
			break;
		    }
		}
		break;
	case 1085:
#line 7214 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    sym_f_init();
		    gr->A_dims = 0;
		}
		break;
	case 1089:
#line 7226 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->A_dims = 1;
		    sc_eat( id_add, SC_SEEN|SC_NEST, ERx(")"), '(', ')' );
		}
		break;
	case 1090:
#line 7233 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    id_add( yypvt[-0].s );
		    gr->A_dims = 0;
		}
		break;
	case 1091:
#line 7240 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    id_add( yypvt[-0].s );
		    sym_fpush( sym_find(yypvt[-0].s) );
		}
		break;
	case 1093:
#line 7255 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    sc_eat( gen_code, SC_BACK, ERx(";"), '(', ')' );
		    er_write( E_E60016_haUNSUPP, EQ_ERROR, 1, ERx("FOR") );
		}
		break;
	case 1099:
#line 7277 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 1100:
#line 7281 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 1116:
#line 7348 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 1117:
#line 7352 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 1121:
#line 7369 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->A_type = T_INT;
		    yyval.v = gr->A_intsy;
		}
		break;
	case 1122:
#line 7374 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->A_type = T_FLOAT;
		    yyval.v = gr->A_fltsy;
		}
		break;
	case 1123:
#line 7379 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->A_type = T_CHAR;
		    yyval.v = gr->A_chrsy;
		}
		break;
	case 1124:
#line 7384 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->A_type = T_INT;
		    yyval.v = gr->A_bytesy;
		}
		break;
	case 1125:
#line 7391 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    char	buf[4];

		    buf[0] = '\'';
		    buf[1] = *(yypvt[-0].s);
		    buf[2] = '\'';
		    buf[3] = '\0';
		    gen_host( G_H_OP, buf );
		}
		break;
	case 1126:
#line 7403 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    i4		i = 0;
		    /*
		    ** All integer constants (and subranges) are size(integer)
		    ** on VMS.
		    */
		    gen_host( G_H_KEY, yypvt[-0].s );
		    CVan( yypvt[-0].s, &i );
		    yyval.i = i;
		}
		break;
	case 1127:
#line 7414 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    i4		i = 0;

		    CVan( yypvt[-0].s, &i );
		    if (*(yypvt[-1].s) == '-')		/* tARUOP is only + and - */
		    {
			gen_host( G_H_OP, ERx("-") );
			yyval.i = -i;
		    } else
			yyval.i = i;
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1128:
#line 7429 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    /*
		    ** All float constants (and subranges) are size(float)
		    ** on VMS.
		    */
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1129:
#line 7437 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (*(yypvt[-1].s) == '-')		/* tARUOP is only + and - */
			gen_host( G_H_OP, ERx("-") );
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1130:
#line 7443 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1131:
#line 7447 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (*(yypvt[-1].s) == '-')		/* tARUOP is only + and - */
			gen_host( G_H_OP, ERx("-") );
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1132:
#line 7455 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_NEWLINE, NULL );
		}
		break;
	case 1133:
#line 7465 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1134:
#line 7470 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1135:
#line 7475 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 1136:
#line 7480 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 1137:
#line 7485 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host(G_H_KEY|G_H_OUTDENT|G_H_INDENT|G_H_NEWLINE, yypvt[-0].s);
		}
		break;
	case 1138:
#line 7490 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1139:
#line 7495 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1140:
#line 7500 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 1141:
#line 7505 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 1142:
#line 7510 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1143:
#line 7515 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1144:
#line 7520 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1145:
#line 7525 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1146:
#line 7530 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 1147:
#line 7535 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY|G_H_OUTDENT|G_H_INDENT, yypvt[-0].s );
		}
		break;
	case 1148:
#line 7540 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1149:
#line 7545 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1150:
#line 7550 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		  /* since "=" also goes to tIS */
		    if (STbcompare(yypvt[-0].s, 0, ERx("is"), 0, TRUE))
		    {
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
							ERx("ADA"), ERx("IS") );
			gen_host( G_H_KEY, ERx("is") );
		    } else
			gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1151:
#line 7562 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1152:
#line 7567 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 1153:
#line 7572 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1154:
#line 7577 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1155:
#line 7582 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1156:
#line 7587 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1157:
#line 7592 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1158:
#line 7597 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1159:
#line 7602 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->A_var = AvarDEC;	/* On entry expect declares */
		}
		break;
	case 1160:
#line 7608 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 1161:
#line 7613 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1162:
#line 7618 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host(G_H_KEY|G_H_INDENT|G_H_OUTDENT|G_H_NEWLINE, yypvt[-0].s);
		}
		break;
	case 1163:
#line 7623 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1164:
#line 7628 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1165:
#line 7633 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY|G_H_INDENT|G_H_NEWLINE, yypvt[-0].s );
		}
		break;
	case 1166:
#line 7638 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1167:
#line 7643 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1168:
#line 7648 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->A_var = AvarDEC;	/* Back to DEC mode */
		}
		break;
	case 1169:
#line 7654 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 1170:
#line 7659 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->A_val = 0;
		    gen_host( G_H_OP|G_H_NEWLINE, yypvt[-0].s );
		}
		break;
	case 1171:
#line 7665 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1172:
#line 7670 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_SCONST, yypvt[-0].s );
		}
		break;
	case 1173:
#line 7675 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->A_var = AvarDEC;
		}
		break;
	case 1174:
#line 7681 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->A_var = AvarDEC;	/* Back to DEC mode */
		}
		break;
	case 1175:
#line 7687 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->A_var = AvarDEC;
		}
		break;
	case 1176:
#line 7693 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 1177:
#line 7718 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    SYM		*sy;

		    gr->gr_type = T_UNDEF;		/* Default */
		    gr->gr_id = str_add( STRNULL, id_getname() );
		    id_free();
		    if ((sy = yypvt[-0].v) == (SYM *)0)
		    {
			ADA_ERR( (E_E60012_haUNDEC, EQ_ERROR, 1, gr->gr_id) );
			if (gr->A_simpvar)
			{
			    sy = symDcEuc( gr->gr_id, 0, gr->A_blk,
					syFisVAR, A_CLOSURE, SY_NORMAL );
			    if (sy)
				sym_s_btype( sy, T_UNDEF );
			}
		    } else	/* Known to EQUEL -- check usage */
		    {
			/* Must be a real variable - not constant or enum */
			if (!syBitAnd(sym_g_useof(sy), syFisVAR))
			{
			    ADA_ERR((E_E6000A_haNOTVAR, EQ_ERROR, 1,gr->gr_id));
			} else if (sym_g_btype(sy) == T_STRUCT)
			{
			  /*
			  ** Cannot be a structure - this can't really happen as
			  ** structures affect parsing.
			  */
			    ADA_ERR( (E_E60014_haVARRECORD, EQ_ERROR, 1,
								gr->gr_id) );
			} else
			    gr->gr_type = sym_g_btype(sy);
		    }
		    gr->gr_sym = sy;
		}
		break;
	case 1178:
#line 7768 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (yypvt[-0].v)
		    {
			if (gr->A_err)
			    gr->gr_type = T_UNDEF;
			else
			    gr->gr_type = sym_g_btype( yypvt[-0].v );
		    } else
			gr->gr_type = T_NONE;
		    gr->gr_sym = yypvt[-0].v;
		    gr->gr_id = str_add( STRNULL, id_getname() );
		    id_free();
		}
		break;
	case 1179:
#line 7782 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->gr_type = T_NONE;
		    gr->gr_sym = (SYM *)0;
		    gr->gr_id = str_add( STRNULL, yypvt[-0].s );
		}
		break;
	case 1180:
#line 7790 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    gr->A_record = (SYM *)0;
		    gr->A_var = AvarUSE;
		    gr->A_simpvar = TRUE;
		    gr->A_err = 0;
		}
		break;
	case 1181:
#line 7807 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.v = yypvt[-1].v;
		}
		break;
	case 1182:
#line 7811 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.v = yypvt[-1].v;
		    gr->A_simpvar = FALSE;
		}
		break;
	case 1183:
#line 7816 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    yyval.v = yypvt[-0].v;
		    gr->A_simpvar = FALSE;
		}
		break;
	case 1184:
#line 7829 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    id_add( yypvt[-0].s );
		    /*
		    ** MUST check for syFisCONST as IN params
		    ** and CONSTANT objects are syFisCONST but can be vars.
		    **
		    ** $$.v set for $0 of Ause_paren
		    */
	    	    yyval.v = sym_resolve( (SYM *)0, yypvt[-0].s, A_CLOSURE,
				        syFisVAR|syFisCONST );
		    /* Reset here, because YACC looks ahead for left paren */
		    gr->A_record = (SYM *)0;
		}
		break;
	case 1188:
#line 7870 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    /*
		    ** Whether or not this is an array reference, eat
		    ** up the left paren!  Otherwise we will get a syntax
		    ** error at the left paren.  There is a symbol table
		    ** entry but we may not know it as an array.
		    */
		    if (gr->A_record && !sym_g_dims(gr->A_record))
			ADA_ERR( (E_E60002_haCANTINDIR, EQ_ERROR, 1,
								id_getname()) );
		    sc_eat( id_add, SC_SEEN|SC_NEST, ERx(")"), '(', ')' );
		}
		break;
	case 1189:
#line 7883 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (gr->A_record && sym_g_dims(gr->A_record))
			ADA_ERR( (E_E60001_haMUSTINDIR, EQ_ERROR, 1,
								id_getname()) );
		}
		break;
	case 1190:
#line 7890 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    id_add( yypvt[-0].s );
		    /*
		    ** MUST check for syFisCONST as IN params
		    ** and CONSTANT objects are syFisCONST but can be RECORDs.
		    */
		    gr->A_record = sym_resolve( gr->A_record, yypvt[-0].s, A_CLOSURE,
						syFisVAR|syFisCONST );
		}
		break;
	case 1191:
#line 7901 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    id_add( yypvt[-0].s );
		}
		break;
	case 1192:
#line 7907 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    SYM		*sy;

		    id_add( yypvt[-0].s );
		    /*
		    ** MUST check for syFisCONST as IN params
		    ** and CONSTANT objects are syFisCONST but are vars.
		    ** Use A_record as the parent pointer.
		    */
		    sy = sym_resolve( gr->A_record, yypvt[-0].s,  A_CLOSURE,
				      syFisVAR|syFisCONST );
		    if (sy == (SYM *)0)
		    {
			ADA_ERR( (E_E6000D_haRECORDREF, EQ_ERROR, 1,
								id_getname()) );
		    }
		    /* Reset here, because YACC looks ahead for left paren */
		    gr->A_record = (SYM *)0;
		    yyval.v = sy;		/* Set for $0 of Ause_paren */
		}
		break;
	case 1193:
#line 7929 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    SYM		*sy;

		    id_add( yypvt[-0].s );
		    /*
		    ** Inspect A_record, and its children if it is a record,
		    ** or its type if it is an access type to a scalar type.
		    */
		    if ((sy = gr->A_record) == (SYM *)0)
		    {
			ADA_ERR( (E_E6000B_haBADRECALL, EQ_ERROR, 1,
								id_getname()) );
			yyval.v = (SYM *)0;
		    } else if (sym_g_btype(sy) == T_STRUCT)
		    {
		       ADA_ERR((E_E6000C_haSCALRECALL, EQ_ERROR, 1,
								id_getname()));
		       yyval.v = (SYM *)0;
		    } else 	/* Must be a scalar access type */
		    {
			/*
			** An access type was used, without a record as its
			** final type. Eg: type ip is access INTEGER;
			**		   I: ip;
			**		   sleep I.ALL
			*/
			yyval.v = sy;
		    }
		}
		break;
	case 1194:
#line 7967 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    if (yypvt[-1].v)    /* Variable found - are these our parens */
		    {
			if (sym_g_dims(yypvt[-1].v))	
			{
			    yyval.i = TRUE;
			    gr->A_simpvar = FALSE;	/* non-Simpvar */
			    sc_eat( id_add, SC_SEEN|SC_NEST, ERx(")"),
								    '(', ')' );
			} else
			    SC_PTR--;
		    } else
			SC_PTR--;
		}
		break;
	case 1195:
#line 7982 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/ada/ada.yf"
{
		    /* Check to see dimensions needed on last element */
		    if (yypvt[-0].v && sym_g_dims(yypvt[-0].v))
		    {
			ADA_ERR( (E_E60001_haMUSTINDIR, EQ_ERROR, 1,
								id_getname()) );
		    }
		}
		break;
		}
	return 0; /* Success */

	}
