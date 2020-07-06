
#line 6 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
/*
** Copyright (c) 2004, 2008 Ingres Corporation
*/

/* %L language - Which language are we using for symbol table? */
/* %L merge <esqlgram.my> -- get the master grammar */
/* %L fake begin */
/* %L fake end */
/* %L language begin 
** Define EQ_X_LANG where X is EUC or PL1 for the symbol table
** Also define the real host lang (if different) for gr_mechanism
*/
# define	EQ_EUC_LANG
# define	EQ_FOR_LANG
/* %L language end */

# include	<compat.h>
# include	<er.h>
# include	<si.h>
# include	<st.h>
# include	<me.h>
# include	<cm.h>
# include	<gl.h>
# include	<sl.h>
# include	<cv.h>
# include	<iicommon.h>
# include	<fe.h>			/* for FE_MAXNAME */
/* Files from EQUEL preprocessor */
# include	<equel.h>
# include	<eqlang.h>
# include	<equtils.h>
# include	<eqsym.h>
# include	<eqgen.h>
# include	<eqstmt.h>
# include	<eqscan.h>
# include	<ereq.h>
# include	<eqgr.h>
# include	<eqfw.h>	
# include	<eqtgt.h>	
# include	<g4consts.h>
# include	<g4defs.h>

/* Special ESQL overlay */
# include	<eqesql.h>

# if defined(hp3_us5)
	# pragma OPT_LEVEL 1
# endif /* hp3_us5 */

static bool   set_role=FALSE;

#define	tABORT		257
#define	tCOPY		258
#define	tHELP		259
#define	tMODIFY		260
#define	tPRINT		261
#define	tREGISTER		262
#define	tRELOCATE		263
#define	tREMOVE		264
#define	tSAVE		265
#define	tSAVEPOINT		266
#define	tSET		267
#define	tCOLLATE		268
#define	tCOMMIT		269
#define	tROLLBACK		270
#define	tREVOKE		271
#define	tBYREF		272
#define	tDISTINCT		273
#define	tOF		274
#define	tNULL		275
#define	tNOT		276
#define	tLIKE		277
#define	tESCAPE		278
#define	tEXCLUDING		279
#define	tGRANT		280
#define	tUSING		281
#define	tFIRST		282
#define	tKEYSET		283
#define	tOFFSET		284
#define	tABSOLUTE		285
#define	tLAST		286
#define	tPRIOR		287
#define	tRELATIVE		288
#define	tEXECUTE		289
#define	tIMMED		290
#define	tONLY		291
#define	tDESCRIBE		292
#define	tPREPARE		293
#define	tDESCRIPTOR		294
#define	tREPEAT		295
#define	tENDSELECT		296
#define	tINDICATOR		297
#define	tBEGIN		298
#define	tEND		299
#define	tANY		300
#define	tAS		301
#define	tAVG		302
#define	tADD		303
#define	tASYMMETRIC		304
#define	tBETWEEN		305
#define	tCASE		306
#define	tCAST		307
#define	tCOUNT		308
#define	tCREATE		309
#define	tDELETE		310
#define	tDROP		311
#define	tELSE		312
#define	tEXISTS		313
#define	tFULL		314
#define	tGROUP		315
#define	tHAVING		316
#define	tINDEX		317
#define	tINNER		318
#define	tINSERT		319
#define	tINTEGRITY		320
#define	tINTERVAL		321
#define	tJOIN		322
#define	tLEFT		323
#define	tMAX		324
#define	tMIN		325
#define	tNATURAL		326
#define	tORDER		327
#define	tPERMIT		328
#define	tRIGHT		329
#define	tSUBSTRING		330
#define	tSYMMETRIC		331
#define	tSELECT		332
#define	tSUM		333
#define	tTABLE		334
#define	tTHEN		335
#define	tUNION		336
#define	tUNIQUE		337
#define	tUPDATE		338
#define	tVALUES		339
#define	tCURRVAL		340
#define	tNEXTVAL		341
#define	tVIEW		342
#define	tWHEN		343
#define	tRESTRICT		344
#define	tCASCADE		345
#define	tGLOBAL		346
#define	tTEMPORARY		347
#define	tSESSION		348
#define	tPRESERVE		349
#define	tROWS		350
#define	tSYSTEM_MAINTAINED		351
#define	tAUTHORIZATION		352
#define	tSCHEMA		353
#define	tCONSTRAINT		354
#define	tFOREIGN		355
#define	tPRIMARY		356
#define	tREFERENCES		357
#define	tDEFAULT		358
#define	tCHECK		359
#define	tKEY		360
#define	tUSER		361
#define	tPUBLIC		362
#define	tDATAHANDLER		363
#define	tCACHE		364
#define	tCYCLE		365
#define	tINCREMENT		366
#define	tMAXVALUE		367
#define	tMINVALUE		368
#define	tNOCACHE		369
#define	tNOCYCLE		370
#define	tNOMAXVALUE		371
#define	tNOMINVALUE		372
#define	tNOORDER		373
#define	tRESTART		374
#define	tSTART		375
#define	tSYSTEM_USER		376
#define	tSESSION_USER		377
#define	tCURRENT_USER		378
#define	tINITIAL_USER		379
#define	tUNCOMMITTED		380
#define	tCOMMITTED		381
#define	tREPEATABLE		382
#define	tSERIALIZABLE		383
#define	tREAD		384
#define	tPARTITION		385
#define	tCALL		386
#define	tCLOSE		387
#define	tCONNECT		388
#define	tCONTINUE		389
#define	tCURRENT		390
#define	tCURSOR		391
#define	tDECLARE		392
#define	tDISCONNECT		393
#define	tEND_EXEC		394
#define	tFETCH		395
#define	tGOTO		396
#define	tIDENTIFIED		397
#define	tOPEN		398
#define	tPROCEDURE		399
#define	tSECTION		400
#define	tSTOP		401
#define	tWHENEVER		402
#define	tIIPRINTF		403
#define	tINQ_INGRES		404
#define	tSET_INGRES		405
#define	tSET_CONNECT		406
#define	tREM_SYS_USER		407
#define	tREM_SYS_PASS		408
#define	tENDEXECUTE		409
#define	tRESULT		410
#define	tROW		411
#define	tFORMS		412
#define	tFORMINIT		413
#define	tADDFORM		414
#define	tFORMDATA		415
#define	tDISPLAY		416
#define	tINITIALIZE		417
#define	tFINALIZE		418
#define	tACTIVATE		419
#define	tPUTFORM		420
#define	tGETFORM		421
#define	tGETOPER		422
#define	tREDISPLAY		423
#define	tMESSAGE		424
#define	tSLEEP		425
#define	tCLEAR		426
#define	tHELPFILE		427
#define	tPROMPT		428
#define	tSUBMENU		429
#define	tVALIDATE		430
#define	tRESUME		431
#define	tENDFORMS		432
#define	tENDDISPLAY		433
#define	tBREAKDISPLAY		434
#define	tENDDATA		435
#define	tENDLOOP		436
#define	tSET_FRS		437
#define	tINQ_FRS		438
#define	tPUTOPER		439
#define	tPRINTSCREEN		440
#define	tHELP_FRS		441
#define	tIIPROMPT		442
#define	tIIMESSAGE		443
#define	tCLEARROW		444
#define	tDELETEROW		445
#define	tINSERTROW		446
#define	tGETROW		447
#define	tPUTROW		448
#define	tLOADTABLE		449
#define	tINITTABLE		450
#define	tVALIDROW		451
#define	tUNLOADTABLE		452
#define	tSCROLL		453
#define	tTABLEDATA		454
#define	tPURGETBL		455
#define	tALL		456
#define	tBY		457
#define	tFROM		458
#define	tIN		459
#define	tINTO		460
#define	tIS		461
#define	tON		462
#define	tTO		463
#define	tWHERE		464
#define	tUNTIL		465
#define	tAT		466
#define	tNOTRIM		467
#define	tWITH		468
#define	tFOR		469
#define	tIF		470
#define	tSQL		471
#define	tFIELD		472
#define	tMENUITEM		473
#define	tCOMMAND		474
#define	tNEXT		475
#define	tSCREEN		476
#define	tUP		477
#define	tDOWN		478
#define	tOUT		479
#define	tCOLUMN		480
#define	tSCROLLUP		481
#define	tSCROLLDOWN		482
#define	tINTCONST		483
#define	tFLTCONST		484
#define	tNAME		485
#define	tSCONST		486
#define	tHEXCONST		487
#define	tDECCONST		488
#define	tUCONST		489
#define	tDTCONST		490
#define	tIVCONST		491
#define	tHOSTCODE		492
#define	tINCLUDE		493
#define	tEOFINC		494
#define	tQUOTE		495
#define	tCOMMENT		496
#define	tTERMINATE		497
#define	tWHITESPACE		498
#define	tDELIMID		499
#define	tATICK		500
#define	tCOMMA		501
#define	tLPAREN		502
#define	tRPAREN		503
#define	tPERIOD		504
#define	tCOLON		505
#define	tDOLLAR		506
#define	tREMOVEROW		507
#define	tSETROW		508
#define	tCALLFRAME		509
#define	tCALLPROC		510
#define	tINQ_4GL		511
#define	tSET_4GL		512
#define	tINSERTROW_4GL		513
#define	tDESCRIBE_4GL		514
#define	tGETROW_4GL		515
#define	tALT_GROUP		516
#define	tALT_LOCATION		517
#define	tALT_ROLE		518
#define	tALT_TABLE		519
#define	tALT_USER		520
#define	tALT_DEFAULT		521
#define	tALT_SEQUENCE		522
#define	tBEG_DECLARE		523
#define	tBEG_EXCLUDE		524
#define	tEND_EXCLUDE		525
#define	tBEG_XACT		526
#define	tCOM_ON		527
#define	tCRE_DBEVENT		528
#define	tCRE_LINK		529
#define	tCRE_ROLE		530
#define	tCRE_DOM		531
#define	tCRE_SEQUENCE		532
#define	tCRE_RULE		533
#define	tCRE_LOCATION		534
#define	tCRE_SEC_ALARM		535
#define	tCRE_USER		536
#define	tDESC_FORM		537
#define	tDESC_INPUT		538
#define	tDIS_SEC_AUDIT		539
#define	tDROP_DBEVENT		540
#define	tDROP_LINK		541
#define	tDROP_ROLE		542
#define	tDROP_RULE		543
#define	tDROP_SEQUENCE		544
#define	tDROP_LOCATION		545
#define	tDROP_SEC_ALARM		546
#define	tDROP_USER		547
#define	tENA_SEC_AUDIT		548
#define	tEND_XACT		549
#define	tGET_ATTR		550
#define	tGET_GLOBAL		551
#define	tGET_DATA		552
#define	tGET_DBEVENT		553
#define	tSET_ATTR		554
#define	tSET_GLOBAL		555
#define	tSETROW_DEL		556
#define	tCLEAR_ARRAY		557
#define	tHLP_INTEGRITY		558
#define	tHLP_PERMIT		559
#define	tHLP_VIEW		560
#define	tHLP_ALL		561
#define	tDIR_EXECUTE		562
#define	tDIR_CONNECT		563
#define	tDIR_DISCONNECT		564
#define	tDROP_DOM		565
#define	tRAISE_DBEVENT		566
#define	tREM_DBEVENT		567
#define	tREG_DBEVENT		568
#define	tONCOMMIT		569
#define	tPUT_DATA		570
#define	tONDATABASE		571
#define	tCRE_SYNONYM		572
#define	tDROP_SYNONYM		573
#define	tALT_SEC_AUDIT		574
#define	tSEND_USEREVENT		575
#define	tCRE_PROFILE		576
#define	tALT_PROFILE		577
#define	tDROP_PROFILE		578
#define	tDBMS_PASSWORD		579
#define	tREAD_ONLY		580
#define	tREAD_WRITE		581
#define	tISO_LEVEL		582
#define	tSET_TRANS		583
#define	tEACH_ROW		584
#define	tEACH_STATEMENT		585
#define	tWTIME		586
#define	tWLOCAL		587
#define	tWOTIME		588
#define	tFPERCENT		589
#define	tFBYTE		590
#define	tFCHAR		591
#define	tFCOMPLEX		592
#define	tFDOUBLE		593
#define	tFINTEGER		594
#define	tFLOGICAL		595
#define	tFMAP		596
#define	tFPARAM		597
#define	tFPRECISION		598
#define	tFREAL		599
#define	tFRECORD		600
#define	tFSTRUCT		601
#define	tOR		602
#define	tAND		603
#define	tLT		604
#define	tGT		605
#define	tNE		606
#define	tLE		607
#define	tGE		608
#define	tEQOP		609
#define	tPLUS		610
#define	tMINUS		611
#define	tSTAR		612
#define	tSLASH		613
#define	tEXP		614
#define	tUNARYOP		615

#line 988 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
	

/* 
** SQL-specific support from the original SQL grammar.
*/

/*
** ESQL-specific support for the SQL statements that are embedded (not FRS).
**
** This information is only valid on a per-statement level, and should be 
** zero-ed out after each statement. See GR_STMTFREE.
*/

/* structure definition has been moved to front.hdr.hdr.eqesql.h */
GLOBALDEF struct esq_struct _esq_struct ZERO_FILL;
GLOBALDEF struct esq_struct *esq = &_esq_struct;
GLOBALDEF char	 session_temp[100];

/* Flag bits for esq->flag */
# define	ESQ_CURSOR	0x001	/* declaring or using a cursor */
# define	ESQ_DISTINCT	0x002
# define	ESQ_NOPRT	0x004
# define	ESQ_DECLCSR	0x008	/* declaring a cursor */
# define	ESQ_REG_SELECT	0x010	/* A non-repeat SELECT */
# define	ESQ_DESC_SEL	0x020	/* A dynamic SELECT using DESCRIPTOR */
# define        ESQ_FROM_CHK    0x040    /* In FROM clause of csr select */
# define	ESQ_CRE_SCHEMA	0x080	/* In CREATE SCHEMA statement */
# define	ESQ_PROC_BYREF	0x100	/* DBproc call is passing parms byref */
# define	ESQ_UNION_SEL	0x200	/* check target list */
# define	ESQ_WITHABLE_CONSTRAINT 0x400 /* just parsed unique/fkey constr */
# define	ESQ_PROC_RESULT	0x800	/* DBproc call has "result row" */

/* Macro to tell if we are using or declaring a cursor */
# define	ESQ_CSRDEC(x)	(x->flag & ESQ_CURSOR)

/* Macro to tell if we are DECLARING a cursor */
# define	ESQ_DCLCSR(x) \
	(((x)->flag & (ESQ_CURSOR|ESQ_DECLCSR)) == (ESQ_CURSOR|ESQ_DECLCSR))

/* Flag bits returned by csr_select_sub and friends */
# define CSR_STATIC	0x00	/* WHERE clause static (or missing) */
# define CSR_NO_FROM	0x01	/* No FROM clause */
# define CSR_NO_WHERE	0x02	/* No WHERE clause */
# define CSR_WHERE_VAR	0x04	/* if there was a WHERE clause in a variable */
# define CSR_UPDATE	0x08	/* if there was an UPDATE clause */
# define CSR_UPD_LIST	0x10	/* if an UPDATE list in the UPDATE clause */
# define CSR_UPDATE_LIST (CSR_UPDATE|CSR_UPD_LIST) /* UPD_LIST implies UPDATE */

/* Convenience string comparison macro: ignore case, check entire string */
# define ESQ_EQSTR(a, b)	(STbcompare((a), 0, (b), 0, TRUE) == 0)

/* Flag bits returned by dec_nullfmt and children */
# define CRE_OK		0x00	/* FIPS and OPEN-SQL compliant */
# define CRE_WDEF	0x01	/* WITH DEFAULT is non-FIPS and non-OPEN SQL */
# define CRE_NDEF	0x02	/* NOT DEFAULT is non-FIPS and non-OPEN SQL */
# define CRE_WSYS	0x04	/* WITH SYSTEM_MAINTAINED non-FIPS, non-OPEN */
# define CRE_NSYS	0x08	/* NOT SYSTEM_MAINTAINED non-FIPS, non-OPEN */
# define CRE_WNUL	0x10	/* WITH NULL is non-FIPS */
# define CRE_OBJKEY	0x20	/* OBJECT_KEY is non-FIPS non-OPEN SQL */
# define CRE_TBLKEY	0x40	/* TABLE_KEY is non-FIPS non-OPEN SQL */
# define CRE_NFIPS	0x80	/* non-FIPS */
# define CRE_CONWTH	0x100	/* constraint WITH clause */

/* Flag bits returned by from_list */
# define FRM_OK		0x00	/* FIPS and OPEN-SQL compliant */
# define FRM_JOIN	0x01	/* FROM with Outer Join non-Fips, non-OPEN SQL*/

/* Stucture for EXEC 4GL INSERTROW */
typedef struct insrow4gl_parms
{
    char *rownumber;		/* String for rownumber */
    char *row;			/* String for object to insert */
    char *row_state;		/* String for state of new row */
} INSROW4GL_PARMS;
static VOID set_insrow_param(
   char *name, char *value, i4  type, SYM *symbol, INSROW4GL_PARMS *param);


#line 1069 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
/* %L locals - Include L locals and global gr structure */
/* %L locals begin - local variables and gr structure for FORTRAN */

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
	i4      gr_rcnt;        /* count of result row entries (for RPPs) */

	/* Add L (FORTRAN) dependent members after this statement */
# define	F_BLOCK		1	/* No scope in ESQL */
	i4	F_rec;		/* F record level */
	i4	F_size;		/* Size of declaration */
	i4	F_dims;		/* Number of dimensions of variable */
	i4	F_intsize;	/* 2 or 4; the size of an integer */
	SYM	*F_struct;	/* Special pointer for references to structs */
};
GLOBALDEF struct gr_state _gr_local ZERO_FILL;
GLOBALDEF struct gr_state *gr = &_gr_local;
struct gr_state tempgr;

/* Where do FORTRAN array indices start? */
# define F_ARR_BASE	1

/* How do you index a FORTRAN array? */
# define F_ARR_EXPR	ERx("(%d)")

/* %L locals end */

/* %L fake begin - Dummy declaration of gr to allow compilation */
/* %L fake end */

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

#line 16677 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"


/* %L mechanism */
/* %L mechanism begin - All the stuff to do with gr_mechanism() */

GLOBALDEF GR_TYPES	gr_typetab[] = {
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
gr_mechanism( flag, arg1, arg2, arg3 )
i4	flag;
PTR	arg1;			/* Extra arguments for particular cases */
PTR	arg2;
PTR	arg3;
{
    register SYM	*sy;
    i4			use_mask, ret_val;
    static   i4	einit = 0;
  /* DML routines */
    extern	i4	gen__sqltab[];
    i4			scStrIsCont(),
			esq_repeat(),
			yyesqlex(),
			sc_iscomment();

    switch (flag)
    {
      case GR_EQINIT:

	eq->eq_lang = EQ_FORTRAN;
	eq->eq_def_in = ERx("sf");
# ifdef	NT_GENERIC
	eq->eq_def_out = ERx("for");
# endif	/* NT_GENERIC */
# ifdef	UNIX
	eq->eq_def_out = ERx("f");
# endif	/* UNIX */
# ifdef	hp9_mpe
	eq->eq_def_out = ERx("for");
# endif	/* hp9_mpe */
# ifdef	VMS
	eq->eq_def_out = ERx("for");
# endif	/* VMS */
# ifdef	CMS
	eq->eq_def_out = ERx("fortran");
# endif	/* CMS */
# ifdef	DGC_AOS
	eq->eq_def_out = ERx("f77");
# endif	/* DGC_AOS */
	if (!einit)
	{
	    eq->eq_in_ext = ERx("sf");
# ifdef	UNIX
	    eq->eq_out_ext = ERx("f");
# endif	/* UNIX */
# ifdef	hp9_mpe
	    eq->eq_out_ext = ERx("for");
# endif	/* hp9_mpe */
# ifdef	VMS
	    eq->eq_out_ext = ERx("for");
# endif	/* VMS */
# ifdef	NT_GENERIC
	    eq->eq_out_ext = ERx("for");
# endif	/* NT_GENERIC */
# ifdef	CMS
	    eq->eq_out_ext = ERx("copy");
# endif	/* CMS */
# ifdef	DGC_AOS
	    eq->eq_out_ext = ERx("f77");
# endif	/* DGC_AOS */
	    einit = 1;
	}

	eq->eq_sql_quote = '\'';
	eq->eq_quote_esc = '\'';
	eq->eq_host_quote = '\'';

	gr->gr_type = T_NONE;
	gr->gr_sym = (SYM *)0;
	gr->gr_id = ERx("");
	gr->gr_flag = GR_HOSTCODE;
	gr->F_intsize = sizeof(i4);

      /* tell the world we're Embedded QUEL! */
	dml->dm_lang = DML_ESQL;
	dml->dm_exec = DML_HOST;
	dml->dm_gentab = gen__sqltab;
	dml->dm_lex = yyesqlex;
	dml->dm_strcontin = scStrIsCont;
	dml->dm_iscomment = sc_iscomment;
	dml->dm_repeat = esq_repeat; 	/* Provide hook for REPEATED queries */

      /* FORTRAN specific flags */
	gr->F_rec = 0;
	gr->F_size = 0;
	gr->F_dims = 0;
	gr->F_struct = (SYM *)0;

	esq->inc = 0;
	break;

      case GR_SYMINIT:
	sym_init( (bool)TRUE );
	gen_init();

      /* declare "generic null" as a symbol */
	sy = symDcEuc( ERx(".null"), 0, F_BLOCK, syFisVAR|syFisSYS, 
		      F_CLOSURE, SY_NORMAL );
	sym_s_btype( sy, T_NUL );
	sym_s_dsize( sy, sizeof(i4) );
	gr->gr_null = sy;
	break;

      case GR_LOOKUP:
	break;

      case GR_STMTFREE:
	str_reset();
	sym_f_init();		/* clear the sym stack */
	gr->gr_type = T_NONE;
	gr->gr_sym = (SYM *)0;
	gr->F_struct = (SYM *)0;
	esq->flag = 0;
	break;

      case GR_DUMP:
	{
	    register FILE	*f = eq->eq_dumpfile;

	    SIfprintf( f, ERx("GR_DUMP: gr_ \n") );
	    SIfprintf( f,
		       ERx("  sym = 0x%p, id = '%s', type = %d, flag = 0%o\n"),
		       gr->gr_sym, gr->gr_id, gr->gr_type, gr->gr_flag );
	    SIfprintf( f, ERx("  func = %d, mode = 0%o\n"),
		       gr->gr_func, gr->gr_mode );
	    SIfprintf( f, ERx("  F_rec = %d, F_size = %d, F_dims = %d\n"),
		       gr->F_rec, gr->F_size, gr->F_dims );
	    SIfprintf( f, ERx("  F_struct = 0x%p F_intsize = %d\n"),
		       gr->F_struct, gr->F_intsize );
	    SIfprintf( f, ERx("ESQ: esq_ \n") );
	    SIfprintf( f, ERx("  flag = 0%o,"), esq->flag );
	    SIfprintf( f, ERx(" inc = 0%o, dml_exec = 0%o\n"), 
		       esq->inc, dml->dm_exec );
	    SIflush( f );
	}
	break;

      case GR_LENFIX:
      /*
      ** change the length of the type (in arg1) to the given length (in arg2).
      ** for BASIC and FORTRAN.
      */
	if (STbcompare((char *)arg1, 0, ERx("integer"), 0, TRUE) == 0)
	    /* get lint truncation warning if sizeof ptr > int, but 
	       code is still valid. */
	    gr->F_intsize = (i4)arg2;
	else 
	    gr->F_intsize = sizeof(i4);
	break;

      case GR_EQSTMT:
	{
	    /* get lint truncation warning if sizeof ptr > int, but 
	       code is still valid. */
	    i4		mode = (i4)arg1;
	    char	*kword = (char *)arg2;
	    char	*kw2 = (char *)arg3;
	    char	buf[50];

	    /* Turn off declarations flag and generate line directive */
	    gr->gr_flag &= ~GR_HOSTCODE;

	    if ((mode==GR_sSQL || mode==GR_sREPEAT
		 || mode==GR_sNODB || mode==GR_sFORMS || mode==GR_s4GL)
		 && kw2 != (char *)0)
	    {
		STprintf( buf, ERx("%s %s"), kword, kw2 );
		kword = buf;
	    }
	    if ((esq->flag & ESQ_NOPRT) == 0)
		gen_eqstmt( G_OPEN, kword );

	    /* 
	    ** Check if SQLCA is required for certain languages.
	    ** If the language requires all externals are defined for type
	    ** checking then make sure the user did an INCLUDE SQLCA.
	    */
	    if ((esq->inc & sqcaSQL) == 0)
	    {
		er_write( E_EQ0503_hSQLCA, EQ_ERROR, 1, kword );
		esq->inc |= sqcaSQL;
	    }

	    /* Check that the correct EXEC was used */
	    if (mode == GR_sFORMS && dml->dm_exec != (DML_EXEC|DML__FRS))
		er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("FRS"), kword );
	    else if (mode == GR_s4GL && dml->dm_exec != (DML_EXEC|DML__4GL))
		er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("4GL"), kword );
	    else if (mode != GR_sFORMS && mode != GR_s4GL &&
		     dml->dm_exec != (DML_EXEC|DML__SQL))
		er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"), kword );
	    /* Stay in EXEC mode but turn off SQL or FRS */
	    dml->dm_exec = DML_EXEC;

	    switch (mode)
	    {

	      case GR_sREPEAT:
		if (gr->gr_flag & GR_RETRIEVE)
		    er_write( E_EQ0063_grNESTQUEL, EQ_ERROR, 1, kword );
		gr->gr_flag |= GR_REPEAT|GR_SQL;
		/*
		** rep_begin doesn't use the first arg, but for safety's sake,
		** send only the keyword part, not the "repeat".
		** definitely don't db_key( "repeat keyword" ), though.
		*/
		if (kw2)
		    kword = kw2;
		rep_begin( kword, NULL );
		db_key( kword );
		break;
	      case GR_sSQL:		/* Reset SQL information */
		if (gr->gr_flag & GR_RETRIEVE)
		    er_write( E_EQ0063_grNESTQUEL, EQ_ERROR, 1, kword );
		gr->gr_flag |= GR_SQL;
		esq_init();
		db_key( kword );
		break;
	      case GR_sLIBQ:
		if (gr->gr_flag & GR_RETRIEVE)
		    er_write( E_EQ0063_grNESTQUEL, EQ_ERROR, 1, kword );
		gr->gr_flag |= GR_ESQL;
		break;
	      case GR_sNODB:
	      case GR_sFORMS:
	      case GR_s4GL:
		gr->gr_flag |= GR_ESQL;
		break;
	    }
	}
	break;

      default:
	break;
    }
}
/* %L mechanism end */
/*{
** Name:	set_insrow_param -- Set a parameter for INSERTROW
**
** Description:
**	EXEC 4GL INSERTROW takes a fixed set of parameters.  Here,
**	we chech a parameter for validity, and save it away if it's correct.	
**
** Inputs:
**	name	char *		Name string for parameter.
**	value	char *		Value string for parameter.
**	type	i4		Data type for parameter.
**	symbol	SYM *		Symbol for parameter
**
** Outputs:
**	param	INSROW4GL_PARAMS *	Parameter structure.
**
** History:
**	11/92 (Mike S) Initial version
*/
static VOID
set_insrow_param(char *name, char *value, i4  type, SYM *symbol,
INSROW4GL_PARMS *param)
{
    /* Fill in our structure, if we can */
    if (STbcompare(name, 0, ERx("rownumber"), 0, TRUE) == 0)
    {
	if (type != T_UNDEF) /* Don't cascade errs */
	{
	    if (type != T_INT)
		er_write( E_EQ0061_grINTVAR, EQ_ERROR, 1, value );
	}
	param->rownumber = value;
    }
    else if (STbcompare(name, 0, ERx("row"), 0, TRUE) == 0)
    {
	if (type != T_UNDEF) /* Don't cascade errs */
	{
	    if (type != T_INT || (symbol != NULL && sym_g_dsize(symbol) != 4))
		er_write( E_EQ0518_grOBJVAR, EQ_ERROR, 1, value );
	}
	param->row = value;
    }
    else if (STbcompare(name, 0, ERx("row_state"), 0, TRUE)== 0)
    {
	if (type != T_UNDEF) /* Don't cascade errs */
	{
	    if (type != T_INT)
		er_write( E_EQ0061_grINTVAR, EQ_ERROR, 1, value );
	}
	param->row_state = value;
    }
    else
    {
	er_write( E_EQ0081_TL_ATTR_UNKNOWN, EQ_ERROR, 1, name);
    }
}
short yyexca[] = {
	-1, 1,
	0, -1,
	-2, 0,
	-1, 164,
	497, 829,
	-2, 831,
	-1, 379,
	458, 811,
	-2, 810,
	-1, 503,
	504, 238,
	-2, 1306,
	-1, 661,
	502, 984,
	-2, 977,
	-1, 663,
	502, 984,
	-2, 982,
	-1, 721,
	485, 555,
	499, 555,
	591, 555,
	593, 555,
	594, 555,
	598, 555,
	-2, 552,
	-1, 924,
	256, 1276,
	391, 1276,
	504, 1470,
	-2, 1468,
	-1, 925,
	256, 1277,
	391, 1277,
	504, 1471,
	-2, 1469,
	-1, 975,
	504, 238,
	-2, 236,
	-1, 1200,
	504, 238,
	-2, 414,
	-1, 1426,
	503, 2050,
	-2, 2087,
	-1, 1457,
	609, 2140,
	-2, 1309,
	-1, 1566,
	485, 2375,
	-2, 2073,
	-1, 1567,
	485, 2375,
	-2, 2073,
	-1, 1568,
	485, 2375,
	-2, 2073,
	-1, 1570,
	485, 2375,
	-2, 2073,
	-1, 1572,
	485, 2375,
	-2, 2073,
	-1, 1577,
	485, 2375,
	-2, 2073,
	-1, 1608,
	504, 2371,
	-2, 2369,
	-1, 1609,
	504, 2372,
	-2, 2370,
	-1, 1740,
	463, 1276,
	501, 1276,
	504, 1276,
	-2, 998,
	-1, 1839,
	314, 1317,
	318, 1317,
	322, 1317,
	323, 1317,
	329, 1317,
	-2, 523,
	-1, 1844,
	485, 555,
	499, 555,
	591, 555,
	593, 555,
	594, 555,
	598, 555,
	-2, 548,
	-1, 1845,
	485, 555,
	499, 555,
	591, 555,
	593, 555,
	594, 555,
	598, 555,
	-2, 550,
	-1, 1933,
	497, 1787,
	501, 1787,
	-2, 1776,
	-1, 2048,
	456, 1276,
	483, 1276,
	504, 1276,
	-2, 876,
	-1, 2062,
	485, 1200,
	499, 1200,
	591, 1200,
	593, 1200,
	594, 1200,
	598, 1200,
	-2, 497,
	-1, 2068,
	502, 782,
	-2, 1276,
	-1, 2089,
	504, 1183,
	-2, 1181,
	-1, 2115,
	502, 781,
	-2, 1227,
	-1, 2317,
	458, 1276,
	501, 1276,
	504, 1276,
	-2, 922,
	-1, 2343,
	502, 1276,
	-2, 1282,
	-1, 2423,
	280, 1305,
	284, 1305,
	289, 1305,
	309, 1305,
	315, 1305,
	316, 1305,
	327, 1305,
	336, 1305,
	395, 1305,
	468, 1305,
	469, 1305,
	497, 1305,
	503, 1305,
	569, 1305,
	-2, 1309,
	-1, 2582,
	458, 776,
	502, 782,
	-2, 1276,
	-1, 2583,
	458, 777,
	-2, 720,
	-1, 2595,
	299, 2264,
	-2, 2321,
	-1, 2664,
	504, 1276,
	-2, 1293,
	-1, 2665,
	504, 1277,
	-2, 1294,
	-1, 2840,
	314, 1317,
	318, 1317,
	322, 1317,
	323, 1317,
	329, 1317,
	-2, 524,
	-1, 2847,
	485, 555,
	499, 555,
	591, 555,
	593, 555,
	594, 555,
	598, 555,
	-2, 528,
	-1, 3138,
	501, 442,
	503, 442,
	-2, 444,
	-1, 3139,
	501, 443,
	503, 443,
	-2, 445,
	-1, 3210,
	336, 596,
	485, 555,
	499, 555,
	591, 555,
	593, 555,
	594, 555,
	598, 555,
	-2, 528,
	-1, 3297,
	596, 2321,
	-2, 2320,
	-1, 3318,
	339, 1362,
	-2, 1378,
	-1, 3500,
	339, 1363,
	-2, 1379,
	-1, 3594,
	299, 2264,
	-2, 2321,
	-1, 3598,
	299, 2320,
	-2, 2321,
};

#define	YYNPROD		2400
#define	YYLAST		10630

short yyact[] = {
	  2237,  3729,  2696,  3711,  2603,  3299,  3730,  3713,  3574,  3291,
	  2554,  3501,  2650,  3016,  3401,  1788,  3485,  3173,   980,  2604,
	  3392,  1911,   661,  1553,  2087,  2990,  2249,  1651,  2225,  2708,
	  2888,  1152,  3333,  3091,  2107,  3498,  3064,  1119,  3313,  2595,
	  2145,  3298,  2676,  3219,  2555,  2070,  2101,  3215,  1231,  3224,
	  1191,  2269,  1102,  2114,  3133,  3220,  3380,  3065,  1285,  2287,
	  2681,  3137,  2419,  3381,  2558,  3140,  2149,  2655,  2668,  3493,
	   722,  2995,  2991,  2140,  2110,  3280,  2214,  2086,  2958,  1378,
	   555,  1466,  1462,  1752,  1461,  2910,  1460,  1463,  1465,   952,
	  2760,  2815,  2552,  2460,  2074,  2776,  2752,  3013,  3212,  2702,
	  3080,  3339,  2649,  2414,   503,  2146,  2057,  2088,  2633,  2610,
	  1281,  2969,  2082,  2704,  2714,  2192,  2596,  2498,  1912,  2424,
	  3218,  2971,  2578,  1840,  2359,  2461,  2352,  1708,  1969,  2825,
	  1526,  2347,   585,   587,  1764,   500,  2692,  2699,  2265,  2305,
	  2211,  2262,  2266,  1552,  2719,  2073,  2732,  2747,  2283,  1343,
	  1554,  1937,  2698,  3049,  1817,   581,  1075,  1584,  3002,  2687,
	  1103,   635,  3094,  2234,  2629,  1999,  2098,   559,   559,   559,
	  2577,   580,   586,   559,   559,  2154,  2570,  2063,   586,   559,
	   596,   580,   559,   605,  2060,   559,   580,   832,  1506,   559,
	   580,   580,  2017,   559,   559,   580,   630,  1579,  1950,  1916,
	   870,  1009,  1413,   649,  1974,   649,  1926,  1331,  3134,   662,
	   613,  1886,   605,  1874,   878,   678,  1851,   586,   580,   586,
	   683,   684,   586,  2288,  2365,  1758,   697,   983,  1695,   882,
	   719,  1761,   653,  2335,  1865,  2089,  2718,  1660,   956,  1291,
	  1640,  2826,  1917,  2223,  1289,  1626,  1885,  1606,  1592,  1588,
	  1533,  1503,  1499,  1476,  1342,  1500,  1452,   880,  1391,  1386,
	   658,  1459,  2064,  2264,  1411,   579,   584,  1488,   857,  1375,
	  1427,  1393,   584,   607,  1368,   597,  1494,   584,  1363,  1585,
	   610,  1966,  1330,  1501,   624,   625,  1290,   703,  1065,   628,
	  1902,  1319,   745,  1325,   984,  2769,  2415,   710,   709,  1269,
	   656,  2351,  1091,  2222,  1090,  2679,   584,  3209,  1364,  1071,
	  1256,   584,   681,   584,  2346,  2703,   584,  1241,  1198,   659,
	   679,  1456,   600,  1675,   660,  1527,   657,  1634,   629,   835,
	   605,  1472,  3050,   633,   634,   723,  2733,  1617,  1061,   508,
	  1443,  1590,  1729,   678,   733,   580,   559,   559,   580,   945,
	   559,   559,   559,  2129,   580,   580,  2611,   648,   678,   648,
	   678,   704,   721,   922,   636,  1394,  1557,   843,   558,  2130,
	  1358,  1424,  1011,  1348,  1829,   742,  3144,   708,  2336,   671,
	   951,   655,  1182,   612,  2778,   720,   787,  1123,    63,  1171,
	   557,   569,    51,  1644,    48,  1625,  1602,  1578,  3003,  1564,
	  1203,   794,   595,   921,  1196,   598,   582,   582,  1491,  1366,
	   841,   855,  1429,   582,   604,   620,   582,  1216,   582,   504,
	   949,   582,  1060,  2630,   584,   582,   582,  1121,  1473,  1586,
	   582,  2142,  1603,   508,   508,  1153,   837,   723,   513,   941,
	  1653,  3435,   944,  1938,   580,   603,  3322,   582,   955,   957,
	  3252,   979,   582,   582,   582,  2425,  3231,   582,   513,  3230,
	  1219,   705,  1089,  2710,  1702,   582,  3158,  2874,   698,  1043,
	  1666,  1667,  2559,  2560,  2561,  2456,  2730,  2729,   975,  2403,
	  2401,  2384,  2381,   577,  1192,   723,   508,  1666,  1667,  2559,
	  2560,  2561,  2151,  1998,  1997,  1996,  1900,  1895,   962,  1228,
	  1035,  2711,  2710,  2567,  2710,  2731,  1702,  3281,  2880,  3300,
	   884,  1720,   663,  2871,  2097,   892,   893,   747,  1702,  3341,
	  3123,   901,   902,  1138,  1611,  1620,  2121,  2120,  1782,  1783,
	  1610,   639,   608,  3194,   573,  1153,  1063,   570,  1020,  3753,
	  1653,   590,   590,   821,  1374,  3193,  3186,   566,   953,  3281,
	  3633,   568,  1085,   609,  2395,   859,   861,   623,  1702,  1586,
	  2217,   626,   627,   839,   838,   582,   513,   513,   926,   942,
	   943,  1666,  1667,  2559,  2560,  2561,   499,  2260,  1068,   837,
	   582,   789,   748,   582,   748,  1653,   583,  1666,  1667,   582,
	   582,  1939,   594,  1621,   606,   494,   493,  2099,   508,  1064,
	   650,   513,  2734,   513,   605,  2711,  2710,  1458,   580,   508,
	   712,   713,   714,  1154,   762,  1818,  1819,  1820,  1823,  1158,
	   837,  1188,  2410,   748,  1972,  1666,  1667,  2559,  2560,  2561,
	  1576,   680,   513,   682,  3191,  1388,   685,   805,  1124,  1227,
	  3240,   954,   513,  1015,  1089,  1106,  1702,   816,   831,  1016,
	  2467,  1017,  2553,  1971,  1970,   507,   851,   954,   513,  2734,
	  2397,  1164,  2398,  2396,   804,  1666,  1667,  2559,  2560,  2561,
	  2175,   510,   586,   513,  1138,  2215,   606,  1153,  1170,   582,
	  1064,   743,   748,  1124,  2164,   511,  2568,  1242,  2621,  2623,
	  2620,  2619,  1167,  2624,  1057,   744,  3667,  3668,   584,  3669,
	  3670,   513,  1172,  1702,  1264,  1728,   839,   838,  1753,  1246,
	  1240,  1224,  2882,   513,   492,   699,  1257,   700,   946,   947,
	   948,  3259,  1219,  2094,  3258,   566,  2092,  2095,  2093,  2109,
	   513,   701,   618,  1621,   513,  1727,  1200,   513,  1243,   790,
	  1702,   513,  1210,   513,   606,   512,  1153,   839,   838,   507,
	   507,  1284,  1286,  1666,  1667,  2559,  2560,  2561,   513,  1219,
	  1165,  3192,   566,   778,  1458,  1237,   584,   507,  1292,  3456,
	  1546,   510,  1063,  1236,  2711,  2710,  1521,   563,   606,   564,
	   562,   513,   507,  3015,   565,   511,  1069,  1282,  1193,  1194,
	  1910,   513,   512,   976,  2736,  2729,  3702,  1084,  2559,  1207,
	  1219,  3455,   507,  3683,  2723,  2724,  2726,  2727,  2728,  2725,
	  1666,  1667,  2559,  2560,  2561,  2625,  2626,  3652,   837,  1470,
	  1453,  1454,   513,  1471,  1666,  1667,  2559,  2560,  2561,  1058,
	  3649,  1062,  1666,  1667,  2559,  2560,  2561,  1265,  1450,   582,
	   513,  3577,  2559,   582,  1666,  1667,  2559,  2560,  2561,  1666,
	  1667,  1666,  1667,  2559,  2560,  2561,  3477,  1161,   837,  1470,
	  3465,   748,   677,  1471,   510,  3464,  3446,   508,  1280,   743,
	   748,  2390,  1099,  1175,  1098,  3264,  1132,  1153,   511,  1458,
	   513,  3102,   566,   744,   507,   512,  1157,  1667,  1156,   513,
	  1438,  1175,  1197,   513,   837,  1470,  3454,   748,  1089,  1471,
	  1702,   513,  3584,   702,  3583,   586,  1515,   582,  1666,  1667,
	  2559,  2560,  2561,  3531,   507,  3530,   513,  3440,  3253,  3439,
	  1215,  1132,  1088,  2363,  3009,   507,  1168,  2941,  3251,  1174,
	  3250,  3066,   507,  1163,   586,  1100,  1977,  1101,  1070,  1666,
	  1667,  2559,  2560,  2561,  1235,  1469,  1468,  1195,  2934,  2932,
	   559,   580,   580,   580,  1088,  1976,  1088,  1702,   513,  1327,
	  1162,  1666,  1667,  2559,  2560,  2561,   666,  2393,   513,  3000,
	  1388,   606,  1485,  1531,  1173,   837,  1470,  1453,  1454,  1484,
	  1471,  1877,  1878,  2936,  1309,  1469,  1468,  1244,   837,  1470,
	  1249,   748,  2935,  1471,  1259,  1320,   618,   513,  2924,   584,
	  1551,   513,  1273,  1272,  1361,  1226,  1248,  1271,  1185,  1270,
	   513,  1247,  1184,  1245,  1183,   588,   743,   748,   837,  1509,
	  1258,  1469,  1468,  1510,  1525,   510,  2922,  1283,   957,  1287,
	   744,  2918,   705,  1211,  1433,  1206,   513,  1365,  1223,   511,
	   513,  2907,   510,  1189,  1377,  1536,  1537,  1538,  1547,  3107,
	   920,  1201,  1190,  2926,  2931,  2925,   511,  2930,  2533,  1561,
	  1913,  1516,   513,   512,  1328,  2517,  1524,  1062,  1981,  1208,
	  1442,  1962,  1856,  1856,   837,   833,  2144,  1945,  2615,   834,
	  3107,  1220,   723,  2523,  2605,  2522,  1221,   559,  1481,  1508,
	   837,  1470,  2159,   748,  1202,  1471,   513,  1485,  2161,  1485,
	  1702,  1326,  1469,  1468,  1484,  1560,  1484,  3262,  2232,  1855,
	  1855,  2161,   513,   837,   833,  1469,  1468,  1365,   834,  3242,
	  1267,  1365,  2723,  2724,  2726,  2727,  2728,  2725,  1307,   510,
	  1315,   563,  1089,   564,   562,   513,   606,  1322,   565,  2906,
	   582,   582,  1159,   511,  1160,   839,   838,  1654,  1976,   513,
	   512,  1012,   510,  1670,  2427,  1671,  2426,  1674,  1690,  1692,
	  1295,  2393,  1696,  2392,  1698,   793,   511,  1359,   580,   582,
	   580,  2643,  1522,  1705,  1629,  2588,  1709,   580,  2380,  1495,
	  2378,  1124,  1197,   507,   777,   748,   582,   582,   582,   924,
	   724,  2185,   586,  2183,   743,   748,  1477,   706,  1723,  2572,
	  1489,   839,   838,   925,   513,   940,  1559,  1558,   744,   513,
	   512,   580,   580,  1360,   513,   662,   678,  1469,  1468,  2029,
	   958,  2028,   959,  1421,  1092,  1751,   513,  2571,  1616,  1622,
	  1755,   510,  1093,  2024,  1437,  2023,  1763,  1765,  1766,  1868,
	   839,   838,  1365,   650,  2289,   511,  2021,   485,  2020,   823,
	  1416,  1409,   512,  2014,  1718,  2013,  1681,  1724,   563,  1092,
	   564,   562,  1700,   510,  1703,   565,   658,  1093,  1813,  1432,
	  1679,  1172,  1434,  1435,  1747,  1179,  1445,   511,   760,   606,
	   606,  2455,   719,   513,  1843,  1844,   584,  1845,  1846,  1847,
	  1734,  1672,  1332,  1759,  1676,  1691,  2177,  2007,  1478,  2006,
	  1995,  3568,  1994,  1749,  1726,   957,   957,  1347,   621,  1992,
	  1638,  1512,   705,   743,   748,  1092,  1589,  2152,  1535,  1513,
	   763,  1641,   485,  1093,  1518,  1811,  2435,   744,  1768,   606,
	   618,  1446,  1722,   513,   606,  1346,  1706,  2602,  1049,  1048,
	  1054,  1053,  1050,  1051,  1347,  1789,  1979,  1052,  1044,  1043,
	   510,   724,  1958,  1798,  1957,  2123,  1848,  1219,  1647,   510,
	   566,   808,  2033,  1543,   511,   888,  1787,   510,   566,   563,
	   513,   564,   562,   511,  2654,   987,   565,  2027,  1032,  1869,
	  1274,   511,  1808,   888,  2144,  1595,   761,  1826,  2012,   578,
	   743,   748,  1756,  1757,  1936,  1467,  1935,  1806,  2867,  1565,
	  2866,  2865,  1593,   582,   744,   582,  1685,  1899,  1486,  1898,
	   513,  1897,   582,  1896,   721,  1580,   748,  1632,   606,  1132,
	   513,  1025,  1089,  1684,  1689,  1686,   578,   582,  1688,  1699,
	  2000,   743,   748,   807,  1814,   513,  1893,   720,  1892,  1605,
	  1601,   705,  1984,  2121,  2120,   744,   582,   582,  1858,  1735,
	  1741,   513,  1643,   808,  1965,  1642,   808,  1859,  1860,  1089,
	  2458,  1662,  1668,  1178,  1180,   563,  1092,   564,   562,  1631,
	  2653,  1960,   565,   563,  1096,   564,   562,   808,   485,  1744,
	   565,  1914,  1967,  1745,  1175,  1646,  1894,   743,   748,  1904,
	  2837,  1905,  1906,  1731,  1777,  1623,  1092,   705,  1492,   510,
	   566,   744,  1089,  1824,  1093,  1422,  1839,   513,  1704,  1773,
	  1655,  1903,  1774,   511,  1047,  1505,   606,   582,   663,   513,
	  1791,  1943,  1349,  1490,  1479,  1384,   580,  1089,   756,   507,
	  2201,  1940,   858,  1486,  1955,  1486,   712,   713,   714,  1959,
	  1748,  1673,   580,  1948,  1841,  1310,  1964,  2036,  2037,  1299,
	  2448,  1853,  1081,  2061,   510,   896,  1345,  2127,   895,  2126,
	  2125,  1711,  1932,   891,  1712,   877,  1710,   874,   511,   854,
	  1185,   705,   848,   815,  1184,   699,  1183,  2664,   566,   513,
	  1983,   508,  1012,  1862,   818,  1863,   899,   771,   510,   810,
	   774,  2665,   712,   713,   714,   981,  1742,   513,   512,   765,
	  2453,  2383,   511,  1861,   887,   563,  1717,   564,   562,  1658,
	  1659,  2304,   565,   743,   748,  1046,  1039,   743,   748,  1769,
	  2049,   507,  1982,  2594,  1963,   743,   748,   744,  1946,  2008,
	  1907,   744,  1942,   513,  1890,  1786,  2052,   513,  1909,   744,
	  1350,  2015,  1802,  1417,  1816,   513,  1831,  2022,   559,   666,
	  1063,  2055,  2025,  1832,  2143,  1941,  2030,  1615,  1371,  1347,
	   563,  1083,   564,   562,  1089,   588,  2599,   565,  2158,  1354,
	  1353,  1080,  1790,  1871,   823,  1882,  1219,   858,  2343,  1365,
	  2038,  2219,  2914,   563,  2043,   564,   562,  2598,  1980,   513,
	   565,  2141,   511,  1888,   563,   758,   564,   562,   807,   566,
	   605,   565,  2209,  2210,  2916,  2917,  2156,  1889,   485,  1365,
	  1953,  1954,  1049,  1048,  1054,  1053,  1050,  1051,  1106,  1045,
	  2589,  1052,  1044,  1043,  2247,  1359,  3426,  2251,  1924,  1921,
	  2637,   689,   699,  2010,   700,  2284,   705,   580,  1725,   567,
	   510,   566,  1947,  1334,  2638,   678,   678,  1809,   701,  2216,
	  2301,   507,  1827,   688,   511,  1877,  1878,  2031,  1993,   510,
	  1956,   582,  1219,  2005,   510,   566,  3119,  2009,   588,   580,
	   678,  1242,   908,   511,   678,   678,  1681,   582,   511,  3721,
	   580,  1652,  2303,  2399,   563,   580,   564,   562,  1477,   510,
	  1679,   565,   678,  1246,   584,   510,  3120,  2400,  2216,   580,
	  1657,  2201,   510,   511,  2342,   696,  2041,  2186,  1257,   511,
	  2212,  2190,  1608,  1988,  2194,  1807,   511,  2912,  2913,   606,
	  2042,   510,  1243,   512,   695,  1313,  1609,   705,   566,  2997,
	  2998,   957,  2278,  2279,  2999,   511,  2259,  1709,  2216,  1314,
	   782,   513,   719,   506,   826,  3673,   563,   825,   564,   562,
	  2034,  2035,   827,   565,  1790,  2201,  1282,   829,   672,  2248,
	   608,  1408,  2003,   957,  1496,  1497,  2004,  2442,  2443,  2306,
	   563,  2189,   564,   562,  2327,  2217,   510,   565,  2313,  2329,
	  1333,   748,   608,  1810,  1810,  2040,  2053,   578,   578,  2285,
	   511,  2298,  1292,  2334,  1219,  1405,  3157,   507,  3174,  1089,
	  3517,  3632,  2503,  1805,   705,   705,  1804,  2187,   563,  2188,
	   564,   562,   746,  2502,  2193,   565,  1403,  2202,  1808,   511,
	  2203,  2131,  2132,   909,  2134,  1808,  2135,   563,  3617,   564,
	   562,  2136,  1404,  2150,   565,   582,  2133,   506,   506,  1803,
	  1597,  1598,  1407,  2001,  2220,  1062,  2226,  2002,  2254,  2142,
	  2239,  2172,  3591,  3015,  2169,   506,  2170,  3503,  2168,  2160,
	  2412,  2250,  2413,  2253,  2360,  2182,   576,  2226,  2500,  2501,
	   506,  2420,   582,  1175,   721,  2290,  2180,  3516,  3515,  3413,
	  2422,  2267,   563,  3364,   564,   562,  3229,  2267,  2274,   565,
	   506,  2204,  2205,  2206,  1928,  1931,   575,   720,  2221,   996,
	  2218,  1399,  1929,  1930,   582,  2263,  1933,  2316,  1790,  2201,
	  2232,  3095,  1398,  1412,  2955,   582,   505,  2244,  2324,  2992,
	   582,  2281,  2252,  2986,  2018,  1414,  2879,  2820,  2312,  2257,
	  2811,   706,  2798,   578,   582,  1628,  1595,  2537,  2538,   705,
	  2345,  2326,  2465,  2534,  2464,   630,  2463,  2466,  1369,   781,
	   574,   559,  2307,  2291,  1400,  2280,  1402,  2438,  2439,  2440,
	  1339,  2173,  1401,  1336,   588,  3095,  2148,  1396,  1397,  2542,
	  1338,  1244,   506,  2056,  1249,  1340,   997,   582,  1743,  2051,
	  2019,  1891,  2565,  1830,  2270,  1327,  1259,  1796,  2321,  2549,
	  1248,  1772,  2356,  2550,  2357,  1247,  2330,  1245,   507,  2310,
	  2452,  1649,   506,  1645,  2366,  2364,  2341,  2372,  2340,   588,
	   505,   505,  1258,   506,  1630,  1624,  2267,  2373,  3518,  1383,
	   506,  1341,  1323,  1320,  2349,  1311,  1273,  1272,  2355,  2354,
	  1250,  1271,  2350,  1270,  1213,  2445,  1074,   994,   903,   580,
	   900,   889,   820,   725,  2377,   995,  2370,  2371,   676,  2375,
	   993,   992,  2367,  2368,   593,  3757,  2421,   906,   779,  1337,
	  1120,  3742,  1028,   505,  1219,  2591,  1365,  2493,   618,  3685,
	  2524,  2525,  2267,  2416,  2495,  2226,  1377,  2539,  1406,  3654,
	  2496,  3629,  2405,  1026,  2608,  2402,  3572,  3536,   559,   559,
	  1328,  2404,  2411,  2661,   559,  1219,  3463,  3357,  3234,  2654,
	  2163,   705,  2592,  1292,  1408,  1407,  2428,  2434,  1441,  2429,
	  2673,  2674,   850,  1732,  2436,  2446,  2451,  2449,  2459,  3095,
	  2628,   678,  2437,   580,   510,   566,   846,  1326,  1716,  1192,
	  1365,  1092,  2494,   957,  1775,  2469,   866,  1365,   511,  1093,
	   678,  1550,  2530,  2506,  1365,   725,   510,  1696,  2566,  3462,
	   867,   868,  2509,  2504,  2753,  2507,  1288,  1709,  1219,  1910,
	   511,  2515,  2720,   606,  1254,  3581,  2631,   512,  2635,  2738,
	  2519,  2521,  2586,  2587,  1674,   505,   578,  2528,  2717,   507,
	   507,  2531,  2520,  1809,  2527,   559,   559,  2529,   559,  2462,
	  1809,  2656,  2532,   725,  1381,  2653,  2666,  2213,  2526,  2784,
	  1120,  2547,   792,   907,  2797,  1495,   791,  2802,  1810,  2514,
	  1763,  2054,   578,  2808,  2284,   580,  1477,  2707,  2510,  2512,
	  2513,  1884,  1431,  1489,   510,   566,  1317,  2817,  1302,  2796,
	   563,  2774,   564,   562,   769,  2569,  3117,   565,   511,  1262,
	  2260,  1810,  3722,  2782,   513,  3684,   556,  3420,  2662,  2756,
	  1901,  1251,   563,   963,   564,   562,  3066,   559,   559,   565,
	  3535,   608,   719,  1164,  2430,  2584,  2585,   719,  2850,  2851,
	  1759,   506,   732,  1681,   582,  2590,  2829,  2593,  2779,  1987,
	  2838,  2831,  1920,   578,  2754,  2167,  1474,  1679,  2839,  1449,
	  1733,   578,   735,  2609,  2816,  1205,   578,   734,  2613,  2627,
	  2731,  2801,  2793,  2742,   755,  2741,   672,  2319,  2777,   957,
	  2640,  1676,  2636,  2641,  2749,  1420,  2794,  1332,  2408,  1535,
	  2644,  2606,  3363,  1589,  1641,  2648,  2768,  2612,   582,  1282,
	   563,  1661,   564,   562,  2659,  2979,  1548,   565,   675,   632,
	  2389,  3460,  2567,  1648,   844,  2786,  2433,   802,  1555,  3647,
	  2651,  2651,  3263,  1808,  1082,   517,  2651,  2873,   582,  2832,
	  2833,  1810,  2809,  2315,  2567,  2671,  2418,  2799,  2285,  2712,
	  2713,  2735,  2318,   991,  2201,  2743,  2388,  1467,  2682,  2338,
	  2139,   989,  2344,  1019,  2348,  2348,   554,  2328,  2226,  2226,
	  1118,   783,  3214,  2302,   721,   973,  2758,  2772,  2737,   721,
	  1881,   666,  2353,  2353,  1274,  1583,  1833,  3200,  2787,  2788,
	  3646,  2789,   971,  1467,  1467,  1467,  1467,   720,  2859,  2781,
	   972,   728,   720,  2771,  2276,   588,  1834,  1835,   608,  3653,
	  2267,  2770,  2240,  2197,  2238,   621,  2200,  2806,  2226,   670,
	  1467,  2804,   582,  1467,  2773,   580,   580,   580,  1467,  2061,
	   582,  1670,  2953,  2810,  2734,  2271,  2652,  2652,  2317,  2199,
	  2792,  2951,  2652,  2898,   735,  2903,  2904,  1018,  2823,  2943,
	  2944,  2785,   511,  1306,  1204,  2885,   735,   974,  2853,   512,
	  1277,  1279,  1278,  1519,  2260,  2896,  2840,  2856,  2198,  2586,
	  1528,  1529,  2988,  1233,  2243,  2242,  2863,   582,  3606,  2694,
	  2824,  3182,   582,  2295,  2952,  2876,  1485,  2293,  2294,  2857,
	  2196,  2884,  2962,  1484,  2861,   588,  2962,  2974,  3335,  2651,
	  2651,   669,  2957,  2886,  1841,  2568,  2964,  2897,  2921,  2366,
	  2849,   618,  2320,  1853,  2900,  2901,  2322,  2323,  1932,  2946,
	  2947,  2948,  2902,  2905,  2923,  2605,  3336,  2568,  2260,  2417,
	  2996,  2927,  2908,  2887,  2331,  2195,   727,  2200,  2891,  3020,
	  1685,  2892,   510,   508,   563,   917,   564,   562,  1539,  1910,
	  2937,   565,  3609,   918,  3060,  3046,   511,  1684,  3181,  1686,
	   670,  2700,  1688,   512,  2121,  2120,  2942,  2039,  2920,   510,
	   566,  1599,  3021,  3022,  3023,   664,  3057,  3057,  2970,  3004,
	  2877,   606,  1540,   511,   665,   961,   931,   510,   666,  1352,
	   512,   666,   806,  1508,  2940,   510,  2938,   506,  2299,  2899,
	  2376,   511,  2939,   669,  1146,  2652,  2652,  2978,   512,   511,
	  2954,  2883,  2363,  2949,  2156,  1685,  3043,   602,  3207,  1670,
	  3029,  3073,  3076,  1106,  2720,  3206,  2963,  2972,  3108,  3092,
	   699,  3048,   700,   566,  1686,   670,  1146,  1688,  3047,  2284,
	  3028,  2994,  3093,   510,  3017,  1910,   701,  1426,   563,  3205,
	   564,   562,   513,  3056,  3056,   565,  3204,   511,   616,   616,
	   582,   582,   582,  3083,   512,   706,  3061,   617,   617,  3321,
	  1457,  2693,  2952,  3150,  3006,   563,  3007,   564,   562,  3005,
	   614,   614,   565,  1482,  3001,  1545,   615,   615,   705,   506,
	  3084,  1517,  3045,   563,  1511,   564,   562,  2890,  1809,  3044,
	   565,   563,  2975,   564,   562,   559,  1138,  3146,   565,  2959,
	  1092,  1684,   886,  2959,  3159,  3160,  3034,  1709,  1093,  3152,
	  3171,  2226,   719,  3042,  3168,  3169,   356,   753,  3153,  3138,
	  3058,  3115,  3059,  3292,  3135,  3190,  3139,  2966,  1077,   879,
	  3225,  3118,  3052,  3052,  3077,  3085,  3071,  3086,  3074,   563,
	  3151,   564,   562,   872,  1636,  1186,   565,  1793,  2670,  3217,
	  3097,  1918,  3078,   999,  3155,  3088,  2282,  1824,   723,   998,
	   725,  2680,  2705,  3106,  3233,  1614,  1367,  1296,  1294,  1562,
	  3149,  3698,   932,  2962,  1780,  3147,  3164,  3124,  3127,  3126,
	  3110,  2855,   985,  2285,  1730,  2212,  3129,  2306,  2983,  3132,
	  1059,  1332,   738,  2745,   618,   687,   927,  1217,  1482,  3008,
	  1482,  1824,  1824,  2962,   814,   928,  3090,  2962,  3035,   506,
	  3165,  3172,  2232,  3039,  3266,  3267,  3161,  3239,   930,   673,
	  3616,  3582,  3067,  3068,   929,  3176,  3579,  3184,  2201,  3179,
	  3177,  3199,   510,  3180,  2360,  3202,  3046,  2066,  1790,  3416,
	  3063,  2226,  2226,  1685,   721,  2783,   511,  2896,  2465,   507,
	  2464,  3079,  2463,  2466,  3087,  3235,  3037,   822,  2678,  3244,
	  1544,  1689,  1686,  1837,   730,  1688,   852,   720,   856,   860,
	   862,   770,  1990,   764,     2,  2962,  2962,  2962,  3236,  2962,
	   642,  1842,  2176,   559,  1627,  3271,  3272,  3273,  3246,  3274,
	  3290,  3245,  3307,  3308,  3310,  3303,   589,   773,  1824,   678,
	  2174,  2844,  2267,  3248,  3296,  2843,   966,  2267,  3130,  2842,
	  2845,  3286,  1486,  1073,  3256,  1072,  2846,  1067,  3175,  1066,
	  3269,  1607,  1040,   967,  1229,  3141,   965,  3269,  1596,  1038,
	  2618,   969,  2616,  1594,   678,  3318,  2614,   919,   563,  1591,
	   564,   562,  1037,  2622,  3255,   565,  2617,   582,  3297,  3295,
	  3277,  2147,  2607,  2706,  1587,   506,  3254,  1036,  3323,  3330,
	  3276,  2970,  3285,  3337,  3196,  2634,  2155,  2762,  2157,  2632,
	  2162,  1604,  2739,  1106,  3203,  1042,  2763,  3356,  3293,  3351,
	  2635,   937,  2153,  3301,   938,  2753,  3302,  1600,   968,  2761,
	  2959,  1041,   939,   580,  1034,  2764,  1033,  3350,   935,  2717,
	  2061,  2717,  1674,  3237,  3294,  1010,   936,   898,  3358,  2717,
	  2251,  1754,  3150,  3150,  3150,  2993,  2601,  2600,  2597,  3287,
	  2959,  3374,  2226,  3150,  2959,  1031,  3393,  3265,   890,  1030,
	  1670,  3397,  1029,  1027,  3399,  1024,  1023,   495,  2817,  3408,
	  3408,    20,  1507,  2026,   307,  2765,  3146,  3146,  3146,  2032,
	  2766,   306,  3330,  1701,  3409,  3376,  3346,  3146,  3152,  3152,
	  3152,   305,  3344,  3407,  3407,  1502,   436,   559,  3352,  3152,
	   591,  1118,   897,   304,   719,  3375,  1498,  3379,  3382,  3383,
	  3421,  3361,   830,   303,  1873,   651,  3366,  3365,  3387,  3151,
	  3151,  3151,  2959,  2959,  2959,  2462,  2959,  3369,  3104,  3360,
	  3151,  1681,  3411,  3370,  3368,  2816,  3367,  1493,   894,   302,
	  3342,   718,  3394,   301,   784,  1679,   772,  3434,  3289,  3149,
	  3149,  3149,  2016,  3431,  3147,  3147,  3147,  3430,   505,  3400,
	  3149,  3354,  3423,  3406,  3406,  3147,  2777,  3355,  1352,  1676,
	   812,   813,   775,  3389,   300,  1487,   299,  2518,  2011,  1480,
	   298,  3402,  3402,   297,  1475,  3432,   506,   428,   885,  3311,
	   296,  3481,  3418,   295,   294,  1455,  3482,  1451,   425,  3415,
	  1448,   876,   293,   161,   160,   159,   158,   157,   507,   156,
	  3384,   155,  3385,  3269,   154,  2962,   153,   152,  3428,   559,
	   151,   150,   149,  3398,   148,  1973,   147,  1978,  1362,  1385,
	   423,  3500,  3269,  3457,  3452,   507,   721,  3447,  2465,  3343,
	  2464,  3451,  2463,  2466,  3324,  3026,  3327,  2670,  3466,  3458,
	   873,   291,   474,  3036,  3469,  3349,  3038,  3040,   582,   720,
	   422,  2216,  3484,  3476,  3269,  3269,  3269,  3269,  3483,  3479,
	  3486,  3487,   290,  3150,  3150,  3523,  3444,  1824,  1824,  1824,
	  3491,  2652,   289,  3527,  2952,  3495,  3496,  1986,  1824,  3507,
	  3374,  3513,  1440,  3374,  3374,  2717,  1985,  2420,  3374,  1439,
	  3519,  3488,   865,  1436,  3141,  3141,  3141,  3146,  3146,   864,
	   420,   863,  3553,  1467,  3537,  3141,  3410,   288,  3490,  3152,
	  3152,  3105,   419,  3538,  3376,   287,  1430,  3376,  3376,  3138,
	  3225,  1670,  3376,  3567,  3539,  2226,  3139,  3540,   418,   582,
	   286,  3565,  3545,  3546,  3375,   417,   285,  3375,  3375,  3217,
	  3151,  3151,  3375,   292,  2475,  3580,  3461,  1106,  1106,  2651,
	  2929,  3459,  3261,  2928,  2505,  2106,  1841,   506,   506,  1425,
	  1423,  3563,  3562,  2962,   416,   853,   508,  2128,  2128,  3564,
	  3149,  3149,  1562,   284,  2952,  3147,  3147,  3555,   415,  2962,
	   283,  3548,  1419,   414,  1418,  3596,   849,  3549,   559,  3576,
	  2476,   282,   413,   559,  2348,   847,  2353,   281,   845,   280,
	  3604,  1410,   842,   411,   840,  3608,  3550,  3551,   279,  3589,
	   278,   723,  3533,  3594,  2470,  3595,  3588,  3598,  3597,  1392,
	  3318,  3601,  2959,  3603,   409,   277,   828,   824,   276,   407,
	   274,  1824,  3150,  2919,   819,  3150,  3615,   273,  3393,  3614,
	  3618,  1382,   817,  3408,  3620,   272,  3631,  3448,  3641,  3630,
	  3643,  2251,  3624,  1118,  2067,  2652,  2911,  3638,  3636,  2909,
	  2915,  3639,  3445,  1952,  2454,  1951,  3146,  3407,  2479,  3146,
	  1949,  3502,  1380,  3644,  1379,  2462,   271,  3648,  3152,   269,
	  2481,  3152,  1395,  1376,  3511,  1373,  2106,  2124,   403,  2472,
	  2473,  2477,  2471,  1372,  2474,   270,   268,  3634,  1824,  1824,
	  3635,  3578,  1086,  3663,  3528,  3662,  3661,  2106,  3529,  3151,
	  3660,   513,  3151,  1370,  1086,   267,   809,   266,   398,   264,
	  3664,  3505,   262,   396,  3508,  3141,  3541,  3510,  3390,  1824,
	  1824,   261,  3626,  2432,  2431,  3374,  3374,  3406,  1923,  3149,
	  1086,  3257,  3149,  1086,  3147,  2499,  1166,  3147,  2497,  3700,
	  3696,  1975,  3556,  2441,  1927,  3402,  1177,  1925,  3712,  3269,
	  3706,  3552,  3707,   559,  3717,  1357,  3709,  2444,   801,  3376,
	  3376,  2226,  3557,   800,  3559,  3718,  2122,  2652,   395,  3150,
	  2959,  3408,  3725,  1922,  3726,  1356,  3727,  3732,  3691,  3375,
	  3375,   799,   257,  1351,   256,  3724,  2959,   796,  2245,  1214,
	   255,  3736,  3017,  3737,  3325,  3407,   254,   788,   251,  3442,
	  3733,  1989,  2255,  3146,   871,   146,   145,   144,   143,   142,
	   141,   140,  3747,   139,  3374,  3152,  2272,  3331,  3746,   138,
	  3681,  3682,  3712,  3750,   137,  3571,  3732,  3754,   136,   135,
	  2694,   134,  3599,   133,  3723,  3733,   132,   131,  2292,   130,
	  3502,   129,   128,  1275,  3748,  1824,  3151,   127,  3376,  3733,
	  3450,   126,   125,  1467,   124,  2308,   123,  1824,  3715,  3715,
	  1824,   122,   121,   120,   119,  3406,   118,   117,  3375,   116,
	   115,   114,   507,   113,  1738,   112,  3149,  2391,  3494,  3494,
	  3494,  3147,  2046,  3402,  3141,   111,   110,  3141,   109,   108,
	   107,  1685,  3610,  2409,   106,   637,   201,   200,   199,  1225,
	  1542,   362,   674,   212,  2184,  2423,  1639,   516,  1684,  3741,
	  1686,   670,  2700,  1688,  2646,  1824,  1635,  3243,  3651,  2895,
	  3241,  1973,  1973,  1973,  2894,  2893,  1978,  1915,  1344,   786,
	  1737,   785,  3715,  3658,   250,  1079,   515,   105,   502,  1739,
	   501,  2166,  2165,  1824,  1824,  3756,  2652,  2047,  3665,  1613,
	  1056,  2652,   498,   497,   103,   198,   197,  2478,  2492,  1335,
	   780,   246,   650,  2406,  1329,   776,   245,   244,   242,  3425,
	  1324,  3319,  2730,  2729,   241,  1318,   768,  1818,  1819,  1820,
	  1823,  1824,  1824,  1321,   767,   766,   240,   243,  1457,  2511,
	  2511,  2511,  2881,  1316,   239,  2233,   759,  3674,  3675,   238,
	  1312,  2731,   757,  2697,  1824,  2695,  3332,  3502,  3716,  3716,
	  2701,  3031,  2675,  2691,  2690,  2511,  2689,  2688,  2511,  2683,
	  2394,  1887,  2693,  2511,   923,  1740,   237,  1883,  1308,   754,
	   236,  3141,  1824,  2048,  1110,  3699,  2864,  2382,  1872,   511,
	  1870,  1301,  1876,  1875,  1111,  3433,   512,   511,   507,  3228,
	  3436,  3437,  3232,  2872,   512,  1824,   606,  2106,  2387,  2106,
	  2106,   910,  3719,   912,  2097,  2875,   913,  2670,  1824,   752,
	  1305,  2870,  2386,  3497,   914,  1880,   751,  1086,  1086,  1113,
	   917,  3033,  3716,   911,  1304,  3227,   510,   566,   918,  2869,
	  2385,  1541,  1879,  2106,  2106,  2111,  2103,  1303,  1109,   916,
	   511,   750,  1112,   382,  1300,   749,   513,   512,   235,  1866,
	  2115,  2379,  2116,  1864,  1298,   740,  1297,   739,  2117,  2113,
	   234,  2652,  2694,  1115,  1114,  3740,   883,  2097,  3710,  2121,
	  2120,   563,  3316,   564,   562,  3315,  3314,  3312,   565,   563,
	  3027,   564,   562,  2669,  1110,   915,   565,  2099,  2667,  1120,
	   650,  3499,  1064,  3607,  1111,  2734,  1107,  1164,  2191,  1105,
	  2694,  1104,  3014,  2171,  1619,  1818,  1819,  1820,  1823,  1618,
	   836,  2658,   561,  2217,  3019,  3605,  1665,  1665,  1276,  3561,
	  1118,  1118,   711,  1685,   707,  2663,  1118,   228,  1821,  1113,
	  3178,  1116,   563,  2337,   564,   562,  2579,  1801,  3575,   565,
	  1684,  1800,  1686,   670,  2700,  1688,  1799,  3185,  1109,  1825,
	  2099,  1268,  1112,   650,  1812,  1064,  1266,  1797,  2106,  2106,
	  1263,  1685,   227,  1795,  1794,  1261,   225,  1776,  1818,  1819,
	  1820,  1823,  1252,  1115,  1114,   970,   221,  1650,  1684,   220,
	  1686,   670,  2700,  1688,   218,  2807,  2311,   506,   219,  3032,
	  3602,  2800,  1762,  1760,   964,  1234,  2108,   217,   216,  1120,
	   214,   213,  3391,  3611,  2795,  1750,   606,   960,  2423,   513,
	  3388,  3154,  1219,  2094,  2068,   566,  2092,  2095,  2093,  2109,
	  2300,  1222,   211,   456,  2657,   359,   358,   357,   511,   209,
	  2286,   606,  1736,   654,   513,   512,  1212,  1209,   652,   208,
	  2723,  2724,  2726,  2727,  2728,  2725,  1666,  1667,  2559,  2560,
	  2561,  3625,  3103,  2744,  2693,  1694,   668,  3099,  2740,  1118,
	  1693,   177,   226,  2314,  1771,  1239,  1767,  1238,   640,   686,
	   222,  2827,  2541,  1532,  2546,  1219,  2094,   510,   566,  2092,
	  2095,  2093,  2109,   667,  2545,  2544,  2137,  2750,  1581,  1118,
	  1118,   511,  2693,   443,   354,  1702,  1523,   513,   512,  1108,
	   353,  1530,   352,   351,   664,  1117,   350,  2090,   349,   348,
	   347,   346,   345,   665,  1770,   344,   343,  2868,   640,   513,
	   563,   342,   564,   562,  2540,  2045,   666,   565,  2790,  2791,
	  1785,   934,   669,   933,   341,   203,   592,   176,   165,  1666,
	  1667,  2097,   737,  1549,  1828,   640,  3575,  3701,   736,   233,
	   731,   670,    -1,   978,   645,   977,   729,  2812,  2813,   232,
	   505,    -1,   646,  1520,   726,   231,  2821,  2119,  2118,  1973,
	  1973,  2987,  2684,  2685,  2686,  2105,  2104,  2112,  2581,  2102,
	  2072,   650,  2071,   563,  2973,   564,   562,  2217,  2576,   668,
	   565,  2968,  2967,  2574,  2965,  2573,  2100,  3590,  3478,  2847,
	  2848,  3288,  1666,  1667,  2985,   881,   881,  2984,  2858,   881,
	  2096,   881,   881,  2085,  2084,  1117,   667,   881,   881,   647,
	  2083,  3051,  2075,  2069,  2099,  2065,  3072,   650,  2716,  1064,
	  3749,   644,  2722,  2235,  3621,  3347,  2721,   664,  2933,  3082,
	  3081,  3340,  1818,  1819,  1820,  1823,   665,  3666,  2231,  2230,
	   618,  2229,  2694,  2228,  2227,  2709,  2224,  1482,  1511,   666,
	  3731,  3728,  3560,  3223,  3221,   669,  2374,  1854,  1852,  3422,
	  3211,  2852,  2369,  1850,  1849,  2268,   643,  3697,  3645,   510,
	  3424,  2106,  2106,  3213,   670,  2106,  2121,  2120,  2854,  2889,
	  3238,  3640,  3419,   511,  3642,  3558,  2694,  3208,  3417,  2106,
	  2841,  2362,  1838,  2106,  2642,   510,  1857,   650,  2178,  1076,
	  3573,  2106,  2551,  1685,  2059,   392,   506,  2106,  2106,   511,
	  3441,  1908,  1818,  1819,  1820,  1823,   249,   104,  2835,  2834,
	  1684,  2358,  1686,   670,  2700,  1688,  1836,   717,   716,   881,
	   715,   229,  3148,   506,  3145,   881,  3373,   881,  3136,  1219,
	  2094,   510,   566,  2092,  2095,  2093,  2109,  1685,  2297,  2296,
	  1746,   210,   204,   202,  2277,   511,   640,  2780,   640,  2275,
	  1199,   513,   512,   631,  1684,   196,  1686,   670,  2700,  1688,
	  1719,   195,   193,   194,   192,   563,  1680,   564,   562,  1683,
	  3514,  3334,   565,  1687,  1682,  1678,  1677,  1541,  2044,  2677,
	  2956,  2775,  1721,  1169,   329,   191,   190,  2050,  1118,   950,
	  3122,   563,   510,   564,   562,  2976,  2759,  2261,   565,  1715,
	  1714,  3121,  2757,  2258,  1713,   622,   511,   188,  1187,  1219,
	  3055,   510,  3053,  1181,  3054,   619,   187,  1230,   215,  1781,
	  1792,  2106,  2106,   694,  2693,   511,  1665,  1665,  1665,  2819,
	  2097,  2106,   512,  3170,  2106,  2818,  2138,   563,  1784,   564,
	   562,   705,  1255,   693,   565,  3405,  2814,  1779,  2106,   692,
	   691,  2325,  1778,  3101,  1253,   690,  1666,  1667,   606,   224,
	  1707,  3534,  3362,  3116,  3011,  2755,  1176,   611,  2693,   186,
	   185,  3018,   184,   183,  2751,   601,  2217,   599,   182,   180,
	  1697,  3024,  3025,   179,   181,   705,  2091,  3030,  1137,  1136,
	  1135,  1134,  1133,  1664,  1664,  1131,   725,  1130,   563,  1129,
	   564,   562,   606,  1128,  1127,   565,  1126,  1125,  1656,  2097,
	  1122,   189,   178,  3714,  1514,   905,   650,   563,   904,   564,
	   562,  3062,   640,   317,   565,   175,   173,  1087,  3069,  3070,
	   172,  1818,  1819,  1820,  1823,  2241,  1666,  1667,  2827,  1155,
	   171,   170,  2827,   572,  3195,  2694,   571,   169,  2246,   168,
	  1097,  1095,  2106,  1094,  3098,  2217,   167,   166,   102,   101,
	   100,    99,  2256,  3111,    98,    97,  3113,  2106,    96,    95,
	  2106,    94,  2106,    93,    92,    91,    90,  2273,    89,    88,
	  1293,    87,    86,    85,   640,  3128,  2684,  2685,  2686,    84,
	  1541,    83,  2099,    82,    81,   650,  1541,    80,    79,    78,
	  2106,    77,  2106,    76,  2106,    75,  1685,   640,    74,    73,
	  1818,  1819,  1820,  1823,   640,  2309,    72,  1973,   640,    71,
	    70,    69,  3162,  1684,    68,  1686,   670,  2700,  1688,  3166,
	    67,    66,    65,    64,    62,    61,    60,    59,  1219,  2094,
	  3260,   566,  2092,  2095,  2093,  2109,    58,  3197,  3198,  2487,
	    57,    56,  2097,   560,   560,   560,    55,   606,    54,   560,
	   560,   512,    53,  2489,  3210,   560,    52,    50,   560,    49,
	    47,   560,  2106,  2106,  2106,   560,  2106,    46,    45,   560,
	   560,    44,    43,  2106,    42,    41,  2106,    40,   638,    39,
	  2106,  2106,  2106,  2361,    38,  2491,  2488,    37,  2217,    36,
	    35,    34,    33,  2490,  2485,  2486,  2484,    32,  2483,  2482,
	    31,   640,    30,    29,    28,    27,    26,  1219,  2094,    25,
	   566,  2092,  2095,  2093,  2109,  1118,  2480,    24,   513,    23,
	    22,   491,  1022,   490,    18,  2099,   606,  2693,   650,    16,
	   512,    15,    14,    13,    12,    11,    10,     9,  1118,     8,
	  1218,  1138,     7,  1818,  1819,  1820,  1823,     6,     5,     4,
	     3,     1,     0,     0,     0,     0,     0,     0,  1464,     0,
	     0,  2878,  3270,     0,     0,     0,     0,  1146,     0,  3275,
	   506,  1483,     0,     0,     0,  2106,     0,     0,     0,     0,
	     0,     0,  2106,  2106,     0,     0,     0,     0,     0,     0,
	  2106,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   509,  1144,  1145,  1141,  1142,  1143,  1149,
	  1150,  1147,  1148,  1151,  1140,  1139,     0,     0,     0,     0,
	  2557,  3304,   560,   560,     0,     0,   560,   560,   560,     0,
	     0,     0,  2106,     0,  3320,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  3328,     0,     0,     0,
	  1219,  2094,     0,   566,  2092,  2095,  2093,  2109,     0,     0,
	     0,  1754,     0,     0,     0,     0,     0,     0,     0,     0,
	  1118,  2106,  2106,   512,     0,  2106,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  1483,     0,  1483,  1118,
	     0,     0,  3353,     0,  2548,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  1293,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  3371,     0,   505,     0,     0,   506,     0,     0,     0,
	     0,     0,     0,     0,     0,  3453,     0,  3386,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  2106,     0,     0,     0,     0,     0,     0,     0,
	  3412,     0,     0,  2106,     0,     0,     0,     0,  2106,     0,
	     0,  2557,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,   641,     0,     0,     0,     0,
	     0,     0,     0,  1664,  1664,  1664,     0,     0,     0,     0,
	     0,  1118,     0,     0,     0,     0,     0,  1534,     0,     0,
	     0,     0,     0,     0,  1086,     0,     0,     0,     0,     0,
	     0,     0,     0,  3521,     0,     0,     0,     0,     0,  2106,
	     0,     0,     0,     0,     0,   641,     0,     0,     0,     0,
	     0,     0,  1556,     0,     0,     0,     0,     0,  1233,     0,
	     0,     0,     0,     0,     0,     0,  3467,     0,     0,  3468,
	     0,     0,   641,     0,     0,     0,     0,  3471,  3473,  3474,
	     0,     0,  1118,  1118,     0,     0,     0,     0,   641,   641,
	     0,     0,   641,     0,   641,   641,     0,  2767,     0,     0,
	   641,   641,     0,     0,     0,     0,     0,     0,     0,   725,
	     0,  2106,     0,     0,     0,     0,     0,  2106,  3504,     0,
	  2106,  3506,  2106,  1086,  2106,  1541,     0,  3512,     0,     0,
	     0,     0,   640,     0,     0,     0,  2106,     0,     0,  3522,
	     0,     0,  3525,     0,     0,     0,     0,  1232,     0,     0,
	     0,  1541,     0,     0,  3532,     0,     0,     0,     0,     0,
	     0,  1260,  2106,     0,     0,  2106,     0,     0,     0,     0,
	     0,     0,     0,     0,  3542,     0,     0,     0,     0,     0,
	     0,  2828,  3543,     0,  2828,     0,  2830,     0,     0,  2830,
	  1118,     0,     0,  2836,     0,  1118,  1663,  1663,  1669,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  3554,     0,     0,     0,     0,     0,  2106,     0,     0,  2106,
	     0,     0,   641,     0,     0,     0,     0,     0,   641,     0,
	   641,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  2106,     0,     0,  2062,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  3587,     0,     0,     0,     0,  3592,     0,     0,     0,     0,
	     0,  2106,     0,     0,     0,     0,  3600,     0,     0,     0,
	     0,     0,     0,   641,     0,   641,     0,     0,     0,     0,
	     0,     0,     0,     0,  3612,     0,     0,     0,     0,     0,
	  2557,  2557,  2557,  2106,  2960,     0,     0,     0,  2960,     0,
	     0,     0,  3623,  2557,     0,     0,     0,     0,  2557,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  3637,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  1118,     0,     0,     0,     0,
	     0,  2106,     0,     0,     0,     0,  2945,     0,     0,     0,
	     0,  3655,     0,  2106,  3657,     0,     0,     0,     0,     0,
	     0,     0,     0,   640,   640,   640,     0,   640,     0,   640,
	     0,     0,  3671,     0,   640,     0,     0,  2556,  3676,     0,
	     0,     0,     0,     0,     0,  2977,   560,     0,     0,  3679,
	  3680,     0,     0,     0,     0,     0,     0,     0,  1541,  3686,
	  3687,     0,     0,  3689,     0,  3692,     0,  3694,     0,     0,
	     0,     0,   640,  1665,  1665,     0,     0,     0,     0,     0,
	     0,  3703,     0,     0,  3705,     0,     0,     0,  3708,     0,
	     0,     0,     0,  2557,     0,     0,     0,     0,     0,   641,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  3012,     0,     0,     0,     0,     0,
	     0,  3012,     0,     0,     0,     0,     0,     0,     0,  3735,
	     0,  3012,     0,     0,     0,  3738,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  3743,  3744,
	     0,     0,     0,     0,     0,     0,     0,   641,     0,     0,
	     0,   641,     0,     0,     0,  3751,     0,     0,     0,     0,
	  3142,     0,     0,     0,  3755,     0,     0,     0,   641,     0,
	     0,     0,     0,   560,   641,     0,     0,     0,     0,   641,
	     0,   641,     0,     0,     0,   641,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  2556,  3189,
	     0,     0,     0,  3112,     0,     0,  3114,     0,     0,     0,
	     0,     0,     0,     0,  3125,     0,     0,     0,     0,     0,
	     0,   641,     0,     0,     0,  2960,     0,     0,   641,     0,
	  3131,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  1086,  1086,  1086,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  2960,     0,     0,     0,  2960,
	     0,     0,  3163,     0,     0,  1541,     0,     0,     0,  3167,
	     0,     0,     0,     0,     0,     0,     0,     0,   641,     0,
	     0,     0,     0,     0,     0,     0,     0,  3012,  3012,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   641,     0,     0,  1293,   641,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  2557,     0,
	     0,     0,     0,     0,     0,  2557,     0,  2960,  2960,  2960,
	     0,  2960,  2557,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,   640,     0,  1663,  1663,  1663,  2557,
	  2557,     0,  2207,  2208,  1815,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  2097,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  2562,     0,  2563,  2564,     0,     0,     0,     0,     0,     0,
	     0,  2407,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,   640,     0,     0,     0,
	     0,     0,  2115,     0,  2116,     0,  2575,  2580,     0,     0,
	  2117,     0,     0,     0,     0,     0,   641,     0,     0,     0,
	     0,  2121,  2120,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  2557,     0,     0,     0,     0,     0,  2099,
	  1464,     0,   650,     0,  1064,     0,     0,  2557,     0,     0,
	     0,     0,     0,     0,  2557,     0,     0,  1818,  1819,  1820,
	  1823,  3305,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  2333,     0,     0,  1464,  1464,  1464,  1464,
	     0,  2339,     0,     0,   641,   641,  3329,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  3378,  3142,
	  3142,  3142,     0,  1464,     0,     0,  1464,     0,   881,     0,
	  3142,  1464,     0,     0,     0,     0,     0,  2556,  2556,  2556,
	     0,  2961,     0,     0,     0,  2961,  3404,  3404,     0,     0,
	  2556,     0,  2715,     0,     0,  2556,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  2108,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  2557,
	     0,     0,     0,     0,  1219,  2094,  2068,   566,  2092,  2095,
	  2093,  2109,     0,     0,     0,     0,     0,     0,  2557,     0,
	   511,  3372,     0,   606,  2557,     0,   513,   512,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  2557,     0,     0,
	     0,     0,     0,     0,     0,  1293,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  1086,  1293,     0,     0,     0,     0,     0,  2960,     0,  2557,
	  2557,  2557,  2557,     0,     0,     0,  2557,  2557,     0,     0,
	  2557,  2557,  2557,     0,     0,     0,     0,     0,     0,     0,
	  1664,  1664,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  3427,     0,     0,     0,     0,  3429,     0,
	  2556,  1822,   563,     0,   564,   562,     0,     0,     0,   565,
	     0,  3427,     0,  3443,     0,     0,     0,     0,     0,     0,
	     0,  1666,  1667,     0,     0,     0,  2557,     0,     0,     0,
	     0,     0,     0,  2557,     0,     0,  2557,     0,     0,   641,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  2543,   560,     0,     0,  3472,  1534,     0,
	  3142,     0,     0,     0,     0,     0,     0,  3378,     0,     0,
	  3378,  3378,     0,     0,     0,  3378,     0,  3143,     0,  3489,
	     0,     0,     0,     0,     0,     0,     0,  3492,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  1650,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  2960,     0,     0,     0,     0,
	     0,     0,     0,  3526,     0,     0,     0,     0,     0,     0,
	     0,  2960,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   560,  2961,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  2557,     0,
	     0,     0,  3544,  2557,     0,     0,  3547,     0,   640,     0,
	     0,     0,  2961,     0,     0,     0,  2961,     0,     0,     0,
	     0,     0,     0,   641,  2062,  2950,     0,     0,     0,     0,
	  3012,     0,     0,     0,     0,     0,     0,     0,     0,  2332,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  2672,  2557,   641,     0,     0,     0,  3627,     0,     0,
	  2981,  2982,     0,     0,     0,     0,     0,     0,     0,  3142,
	     0,     0,  3142,     0,     0,  2556,     0,     0,     0,     0,
	  3404,     0,  2556,     0,  2961,  2961,  2961,     0,  2961,  2556,
	     0,     0,     0,     0,  2557,  3593,     0,     0,  2748,     0,
	     0,     0,     0,     0,  2557,     0,  2556,  2556,     0,     0,
	     0,     0,     0,  2557,  2557,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	   641,   641,   641,     0,   641,     0,   641,  2557,     0,     0,
	  2557,   641,     0,     0,     0,     0,     0,     0,     0,     0,
	  1650,  1650,  3378,  3378,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  1483,     0,     0,     0,     0,
	     0,  2822,     0,     0,  2557,     0,     0,     0,  3650,   641,
	     0,     0,     0,     0,  3075,   640,     0,  3089,     0,     0,
	  2556,     0,     0,     0,     0,  2557,     0,     0,     0,     0,
	     0,  3096,     0,     0,  2556,     0,  3142,     0,  3404,     0,
	     0,  2556,  3672,  2860,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  3678,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  2557,     0,
	  3688,  3378,     0,  3690,     0,  3693,     0,  3695,     0,     0,
	     0,     0,     0,     0,     0,  3377,  3143,  3143,  3143,     0,
	     0,     0,     0,     0,     0,     0,     0,  3143,  3305,     0,
	     0,  2535,  2536,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  3403,  3403,     0,     0,   560,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  3201,  3734,     0,     0,     0,
	     0,     0,     0,     0,     0,  3739,  2556,     0,     0,     0,
	     0,     0,     0,     0,     0,  3226,     0,     0,     0,     0,
	     0,     0,     0,  3745,     0,  2556,     0,     0,  2583,     0,
	     0,  2556,     0,     0,     0,  3752,     0,     0,     0,     0,
	     0,     0,     0,     0,  2556,     0,     0,     0,     0,     0,
	     0,   982,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  2961,     0,  2556,  2556,  2556,  2556,
	     0,     0,     0,  2556,  2556,     0,     0,  2556,  2556,  2556,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   641,     0,     0,   560,   560,     0,     0,     0,  2660,
	   560,     0,     0,     0,     0,     0,  3278,     0,     0,  3279,
	     0,     0,     0,  3282,  3283,  3284,     0,     0,     0,     0,
	     0,     0,     0,  2556,     0,     0,     0,   641,     0,     0,
	  2556,   641,     0,  2556,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  2746,     0,     0,  3143,     0,     0,
	     0,     0,   640,     0,  3377,     0,     0,  3377,  3377,     0,
	   641,     0,  3377,     0,     0,     0,     0,     0,     0,     0,
	   641,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  3041,   560,   560,   641,   560,     0,     0,     0,     0,     0,
	     0,     0,     0,  1663,  1663,     0,  1464,     0,  3338,     0,
	     0,     0,  2961,  2803,     0,  2562,  3345,  2805,     0,     0,
	   560,     0,     0,  3348,     0,     0,     0,     0,  2961,     0,
	     0,     0,     0,     0,   641,     0,     0,     0,     0,     0,
	     0,   641,     0,     0,     0,     0,   641,     0,   641,     0,
	     0,     0,     0,   881,     0,  2556,     0,     0,     0,     0,
	  2556,   641,     0,   560,   560,  2062,   741,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,   795,   797,   798,     0,
	     0,     0,     0,     0,   795,     0,     0,     0,     0,   811,
	     0,     0,     0,     0,  3156,     0,     0,     0,     0,  2556,
	     0,     0,     0,     0,  3628,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,   875,     0,  3143,     0,     0,  3143,
	     0,     0,     0,     0,     0,     0,     0,  3403,     0,  3188,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  2556,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  2556,  3216,     0,     0,     0,     0,     0,     0,     0,
	  2556,  2556,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,   641,     0,  2556,     0,     0,  2556,     0,     0,
	     0,     0,     0,     0,     0,     0,  3470,     0,   641,  3377,
	  3377,  3475,     0,     0,     0,     0,     0,   986,   988,     0,
	   990,     0,     0,     0,     0,     0,     0,     0,     0,   990,
	     0,  2556,     0,     0,     0,  1001,  1002,  1003,  1004,  1005,
	  1006,  1007,  1008,  3268,     0,  1013,     0,  1014,     0,     0,
	     0,     0,  2556,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  2097,  3143,     0,  3403,     0,     0,     0,     0,
	     0,     0,  3524,     0,     0,     0,     0,     0,     0,  2980,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  2556,  1021,     0,  3377,   640,
	     0,     0,     0,     0,     0,     0,     0,     0,  2115,     0,
	  2116,     0,   640,     0,     0,     0,  2117,     0,     0,     0,
	     0,     0,     0,     0,     0,  3317,     0,  2121,  2120,     0,
	     0,     0,     0,     0,     0,     0,     0,  3326,     0,     0,
	     0,     0,     0,     0,     0,  2099,     0,     0,   650,     0,
	  1064,     0,     0,  3449,     0,  3226,  1464,  3566,     0,     0,
	     0,     0,     0,  1818,  1819,  1820,  1823,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  3585,     0,     0,  3586,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  3359,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,   641,     0,     0,     0,     0,
	  1968,     0,     0,     0,     0,     0,     0,  3100,     0,   484,
	   486,   487,     0,     0,  3109,     0,     0,   488,   489,  3619,
	     0,     0,  3622,     0,  2108,   560,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  1219,  2094,  2068,   566,  2092,  2095,  2093,  2109,     0,     0,
	     0,     0,   641,     0,     0,     0,   511,  3414,     0,   606,
	  1702,     0,   513,   512,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  3656,     0,     0,     0,     0,     0,
	     0,   560,     0,     0,     0,     0,  3183,     0,  3187,     0,
	     0,     0,     0,   496,     0,   514,     0,   518,   519,   520,
	   521,   522,     0,   523,   524,   525,  3677,   526,   527,   528,
	   529,   530,   531,   532,   533,   534,   535,   536,   537,   538,
	   539,   540,   541,   542,     0,   543,   544,   545,   546,   547,
	   548,   549,   550,   551,     0,     0,   552,   553,   563,     0,
	   564,   562,     0,     0,     0,   565,     0,  3480,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  1666,  1667,     0,
	     0,     0,     0,     0,  3720,     0,     0,     0,     0,     0,
	     0,     0,   641,     0,     0,     0,     0,     0,     0,     0,
	  1355,     0,     0,     0,     0,     0,     0,  3509,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  1387,     0,     0,  1389,  1390,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  1415,     0,
	   795,     0,     0,     0,     0,     0,  1428,     0,     0,     0,
	  1387,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,   803,     0,     0,     0,   795,  1447,     0,     0,
	  2097,  2232,     0,     0,     0,     0,     0,     0,     0,   560,
	     0,     0,     0,     0,     0,     0,     0,     0,  3306,  3309,
	   869,  1504,  3216,     0,     0,  2081,     0,  2078,     0,     0,
	     0,  2111,  2103,  2076,     0,     0,     0,     0,  2236,     0,
	     0,     0,     0,     0,     0,     0,  2115,     0,  2116,  2080,
	  2079,     0,     0,     0,  2117,  2113,     0,   666,  2077,     0,
	     0,     0,     0,     0,     0,  2121,  2120,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  2099,     0,     0,   650,     0,  1064,     0,
	  3317,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  1818,  1819,  1820,  1823,     0,     0,     0,     0,  3613,
	     0,     0,     0,     0,     0,     0,  1822,     0,     0,     0,
	     0,     0,     0,  1566,  1567,  1568,  1569,  1570,  1571,  1572,
	  1573,     0,     0,     0,     0,  1577,     0,     0,     0,     0,
	     0,     0,  1000,  1582,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  3395,  3396,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  3659,  2108,   560,     0,     0,     0,  2097,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  1219,  2094,
	  2068,   566,  2092,  2095,  2093,  2109,     0,     0,     0,   641,
	     0,     0,     0,     0,   511,     0,     0,   606,     0,  1055,
	   513,   512,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  2115,     0,  2116,  1078,     0,     0,     0,
	     0,  2117,     0,     0,     0,     0,  3704,     0,     0,     0,
	     0,     0,  2121,  2120,     0,     0,   641,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  2099,     0,     0,   650,     0,  1064,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  2097,  2232,     0,  1818,  1819,
	  1820,  1823,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,   560,   563,     0,   564,   562,
	  2081,     0,  2078,   565,     0,     0,  2111,  2103,  2076,     0,
	     0,     0,     0,  2236,     0,  1666,  1667,  3520,     0,     0,
	     0,  2115,     0,  2116,  2080,  2079,     0,     0,     0,  2117,
	  2113,     0,     0,  2077,     0,     0,     0,     0,     0,     0,
	  2121,  2120,  1232,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  2099,     0,
	     0,   650,     0,  1064,     0,     0,     0,     0,     0,  2108,
	     0,     0,     0,     0,     0,     0,  1818,  1819,  1820,  1823,
	     0,     0,     0,     0,     0,  1219,  2094,  2068,   566,  2092,
	  2095,  2093,  2109,     0,     0,     0,     0,     0,     0,     0,
	  1867,   511,     0,     0,   606,     0,     0,   513,   512,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  1822,  1822,  1822,     0,  1919,
	     0,     0,     0,     0,     0,     0,  1822,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  2108,     0,     0,
	     0,     0,  1944,     0,   560,     0,     0,     0,     0,   560,
	     0,     0,     0,  1219,  2094,  2068,   566,  2092,  2095,  2093,
	  2109,     0,  1961,   563,     0,   564,   562,     0,     0,   511,
	   565,     0,   606,     0,     0,   513,   512,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  2097,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  2081,     0,  2078,     0,     0,   641,  2111,  2103,  2076,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,   641,
	     0,     0,  2115,     0,  2116,  2080,  2079,     0,     0,     0,
	  2117,  2113,     0,     0,  2077,     0,     0,     0,     0,     0,
	     0,  2121,  2120,  1444,     0,     0,     0,     0,     0,     0,
	     0,   563,     0,   564,   562,     0,     0,     0,   565,  2099,
	     0,     0,   650,     0,  1064,     0,     0,  1504,     0,     0,
	  1666,  1667,     0,     0,     0,     0,     0,  1818,  1819,  1820,
	  1823,     0,     0,     0,     0,     0,     0,    17,   164,   319,
	   454,   361,   364,   367,   369,   370,   374,   375,   376,     0,
	   318,   223,   372,     0,     0,     0,     0,     0,     0,   560,
	     0,   356,     0,     0,     0,     0,     0,     0,     0,     0,
	   248,     0,     0,   335,   363,     0,   334,   205,     0,     0,
	  2647,     0,     0,     0,     0,     0,  1822,  1822,     0,     0,
	   323,   333,   441,     0,     0,     0,     0,     0,     0,     0,
	   360,     0,     0,     0,     0,     0,     0,     0,  1563,     0,
	     0,     0,     0,   458,     0,     0,     0,     0,  2108,   378,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  1574,  1575,     0,     0,  1219,  2094,  2068,   566,  2092,  2095,
	  2093,  2109,     0,     0,     0,     0,     0,     0,     0,     0,
	   511,     0,     0,   606,     0,     0,   513,   512,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,   381,   230,   457,
	     0,     0,     0,   332,   383,     0,   379,  1612,     0,   380,
	   365,     0,     0,   391,   386,   387,   389,   390,     0,     0,
	   206,     0,     0,   263,   397,   393,   460,   459,  1633,     0,
	  1637,   463,   461,   265,   275,   404,   410,   253,   400,   406,
	   465,   412,   408,   259,   258,   252,   247,   260,   464,   462,
	     0,   405,   399,   402,   401,   466,   467,   470,   468,   472,
	   471,   469,   475,   483,   473,   421,   424,     0,     0,     0,
	     0,     0,   563,     0,   564,   562,     0,     0,     0,   565,
	     0,     0,     0,     0,     0,  1822,     0,     0,  1822,     0,
	     0,  1666,  1667,  2058,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,    21,   162,    19,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,   433,   438,
	   477,   476,   431,   435,   432,   427,   430,   309,   310,   308,
	   313,   314,   316,   312,   163,     0,     0,   174,   440,   322,
	   325,   320,   321,   327,   373,   324,   326,   330,   394,   336,
	   340,   444,   442,   445,   446,   452,   448,   451,   449,   355,
	   207,   478,   429,   384,   385,   479,   437,   439,   426,   480,
	   481,   482,   455,   339,   337,   338,   447,   366,   371,   368,
	     0,   388,     0,   328,   453,   311,   434,   331,   315,   450,
	     0,     0,     0,     0,   377,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	    17,   164,   319,   454,   361,   364,   367,   369,   370,   374,
	   375,   376,     0,   318,   223,   372,     0,     0,     0,     0,
	     0,     0,  1822,     0,   356,     0,     0,     0,     0,     0,
	     0,     0,     0,   248,     0,     0,   335,   363,     0,   334,
	   205,     0,     0,  2179,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   323,   333,   441,     0,     0,     0,     0,
	     0,     0,     0,   360,     0,     0,     0,     0,     0,  2447,
	     0,     0,     0,     0,     0,  2450,   458,     0,     0,     0,
	     0,     0,   378,     0,     0,     0,     0,  2457,     0,     0,
	  1387,     0,  2468,     0,     0,     0,     0,     0,     0,     0,
	     0,  1934,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  1428,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	   381,   230,   457,     0,     0,     0,   332,   383,     0,   379,
	  2516,     0,   380,   365,     0,     0,   391,   386,   387,   389,
	   390,     0,     0,   206,     0,     0,   263,   397,   393,   460,
	   459,  1504,     0,     0,   463,   461,   265,   275,   404,   410,
	   253,   400,   406,   465,   412,   408,   259,   258,   252,   247,
	   260,   464,   462,     0,   405,   399,   402,   401,   466,   467,
	   470,   468,   472,   471,   469,   475,   483,   473,   421,   424,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,    21,   162,    19,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   433,   438,   477,   476,   431,   435,   432,   427,   430,
	   309,   310,   308,   313,   314,   316,   312,   163,     0,     0,
	   174,   440,   322,   325,   320,   321,   327,   373,   324,   326,
	   330,   394,   336,   340,   444,   442,   445,   446,   452,   448,
	   451,   449,   355,   207,   478,   429,   384,   385,   479,   437,
	   439,   426,   480,   481,   482,   455,   339,   337,   338,   447,
	   366,   371,   368,     0,   388,     0,   328,   453,   311,   434,
	   331,   315,   450,     0,     0,     0,     0,   377,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	    17,   164,   319,   454,   361,   364,   367,   369,   370,   374,
	   375,   376,     0,   318,   223,   372,     0,     0,     0,     0,
	     0,     0,     0,     0,   356,     0,     0,  2181,     0,     0,
	     0,     0,     0,   248,     0,     0,   335,   363,     0,   334,
	   205,     0,     0,  1991,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   323,   333,   441,     0,     0,     0,     0,
	     0,     0,     0,   360,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,   458,     0,     0,     0,
	     0,     0,   378,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	   381,   230,   457,     0,     0,     0,   332,   383,     0,   379,
	     0,     0,   380,   365,     0,     0,   391,   386,   387,   389,
	   390,     0,     0,   206,     0,     0,   263,   397,   393,   460,
	   459,  1867,     0,  2862,   463,   461,   265,   275,   404,   410,
	   253,   400,   406,   465,   412,   408,   259,   258,   252,   247,
	   260,   464,   462,     0,   405,   399,   402,   401,   466,   467,
	   470,   468,   472,   471,   469,   475,   483,   473,   421,   424,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,    21,   162,    19,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   433,   438,   477,   476,   431,   435,   432,   427,   430,
	   309,   310,   308,   313,   314,   316,   312,   163,     0,     0,
	   174,   440,   322,   325,   320,   321,   327,   373,   324,   326,
	   330,   394,   336,   340,   444,   442,   445,   446,   452,   448,
	   451,   449,   355,   207,   478,   429,   384,   385,   479,   437,
	   439,   426,   480,   481,   482,   455,   339,   337,   338,   447,
	   366,   371,   368,     0,   388,     0,   328,   453,   311,   434,
	   331,   315,   450,     0,     0,     0,     0,   377,     0,     0,
	    17,   164,   319,   454,   361,   364,   367,   369,   370,   374,
	   375,   376,     0,   318,   223,   372,     0,     0,     0,     0,
	     0,     0,     0,     0,   356,     0,     0,     0,     0,     0,
	     0,     0,     0,   248,     0,     0,   335,   363,     0,   334,
	   205,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  2508,   323,   333,   441,     0,     0,     0,     0,
	     0,     0,     0,   360,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,   458,     0,     0,     0,
	     0,     0,   378,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	   381,   230,   457,     0,     0,     0,   332,   383,     0,   379,
	     0,     0,   380,   365,     0,     0,   391,   386,   387,   389,
	   390,     0,     0,   206,     0,     0,   263,   397,   393,   460,
	   459,     0,     0,     0,   463,   461,   265,   275,   404,   410,
	   253,   400,   406,   465,   412,   408,   259,   258,   252,   247,
	   260,   464,   462,     0,   405,   399,   402,   401,   466,   467,
	   470,   468,   472,   471,   469,   475,   483,   473,   421,   424,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  2639,     0,     0,
	     0,     0,     0,     0,     0,     0,    21,   162,    19,     0,
	  2645,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   433,   438,   477,   476,   431,   435,   432,   427,   430,
	   309,   310,   308,   313,   314,   316,   312,   163,     0,     0,
	   174,   440,   322,   325,   320,   321,   327,   373,   324,   326,
	   330,   394,   336,   340,   444,   442,   445,   446,   452,   448,
	   451,   449,   355,   207,   478,   429,   384,   385,   479,   437,
	   439,   426,   480,   481,   482,   455,   339,   337,   338,   447,
	   366,   371,   368,  2097,   388,     0,   328,   453,   311,   434,
	   331,   315,   450,     0,     0,     0,     0,   377,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  2081,     0,
	  2078,     0,     0,     0,  2111,  2103,  2076,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  3249,  2115,
	     0,  2116,  2080,  2079,     0,     0,     0,  2117,  2113,     0,
	     0,  2077,     0,     0,     0,  2097,     0,     0,  2121,  2120,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  2099,     0,     0,   650,
	  2081,  1064,  2078,     0,     0,     0,  2111,  2103,  2076,     0,
	     0,     0,     0,     0,  1818,  1819,  1820,  1823,     0,     0,
	     0,  2115,     0,  2116,  2080,  2079,     0,     0,     0,  2117,
	  2113,     0,   666,  2077,     0,     0,     0,     0,     0,     0,
	  2121,  2120,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  2099,     0,
	     0,   650,     0,  1064,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  1818,  1819,  1820,  1823,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  2108,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  1219,  2094,  2068,   566,  2092,  2095,  2093,  2109,     0,
	     0,     0,     0,     0,     0,     0,     0,   511,     0,     0,
	   606,     0,     0,   513,   512,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  2108,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  1219,  2094,  2068,   566,  2092,  2095,  2093,
	  2109,     0,     0,     0,     0,     0,     0,     0,     0,   511,
	     0,     0,   606,     0,     0,   513,   512,     0,     0,     0,
	     0,     0,     0,  2097,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,   563,
	     0,   564,   562,     0,     0,     0,   565,     0,  2081,     0,
	  2078,     0,     0,     0,  2111,  2103,  2076,     0,  1666,  1667,
	  3222,     0,     0,  2989,     0,     0,     0,     0,     0,  2115,
	     0,  2116,  2080,  2079,     0,     0,     0,  2117,  2113,     0,
	     0,  2077,     0,     0,     0,     0,     0,     0,  2121,  2120,
	     0,     0,     0,     0,  3438,     0,     0,     0,     0,     0,
	     0,   563,     0,   564,   562,  2097,  2099,     0,   565,   650,
	     0,  1064,     0,     0,     0,     0,     0,     0,  3010,     0,
	  1666,  1667,     0,     0,  1818,  1819,  1820,  1823,     0,     0,
	  2081,     0,  2078,     0,     0,     0,  2111,  2103,  2076,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  2115,     0,  2116,  2080,  2079,     0,     0,     0,  2117,
	  2113,     0,     0,  2077,     0,     0,     0,     0,     0,     0,
	  2121,  2120,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  2099,     0,
	     0,   650,     0,  1064,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,   618,     0,  1818,  1819,  1820,  1823,
	     0,     0,     0,     0,     0,  2108,     0,     0,     0,     0,
	     0,     0,     0,     0,  2097,     0,     0,     0,     0,     0,
	     0,  1219,  2094,  2068,   566,  2092,  2095,  2093,  2109,     0,
	     0,     0,     0,     0,     0,     0,     0,   511,     0,  2081,
	   606,  2078,     0,   513,   512,  2111,  2103,  2076,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  2115,     0,  2116,  2080,  2079,     0,     0,     0,  2117,  2113,
	     0,     0,  2077,     0,     0,     0,     0,     0,     0,  2121,
	  2120,     0,  2260,     0,     0,     0,     0,  2108,  3569,  3570,
	     0,     0,     0,     0,     0,     0,     0,  2099,     0,     0,
	   650,     0,  1064,  1219,  2094,  2582,   566,  2092,  2095,  2093,
	  2109,     0,     0,     0,     0,  1818,  1819,  1820,  1823,   511,
	     0,     0,   606,  1702,     0,   513,   512,     0,     0,   563,
	     0,   564,   562,  2097,     0,     0,   565,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  1666,  1667,
	     0,     0,     0,     0,     0,     0,     0,     0,  2081,     0,
	  2078,  3247,     0,     0,  2111,  2103,  2076,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  2115,
	     0,  2116,  2080,  2079,     0,     0,     0,  2117,  2113,     0,
	     0,  2077,     0,     0,     0,     0,     0,     0,  2121,  2120,
	     0,     0,     0,     0,     0,     0,  2108,     0,     0,     0,
	     0,   563,     0,   564,   562,     0,  2099,     0,   565,   650,
	     0,  1064,  1219,  2094,  2068,   566,  2092,  2095,  2093,  2109,
	  1666,  1667,     0,     0,  1818,  1819,  1820,  1823,   511,     0,
	     0,   606,     0,     0,   513,   512,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  2108,     0,     0,     0,     0,
	   563,     0,   564,   562,     0,     0,     0,   565,     0,     0,
	     0,  1219,  2094,  2068,   566,  2092,  2095,  2093,  2109,  1666,
	  1667,     0,     0,     0,     0,     0,     0,   511,     0,     0,
	   606,     0,     0,   513,   512,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,   563,
	     0,   564,   562,     0,     0,     0,   565,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  1666,  1667
};

short yypact[] = {
	 -1000,  9104, -1000, -1000, -1000, -1000, -1000, -1000,   750,   750,
	   750, -1000, -1000, -1000, -1000, -1000,   750,   750,   110, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,   750,   286,   750,  2045,   750,   750,   750,   750,
	   750, -1000,   750,   750,   750, -1000,   750,   750,   750,   750,
	   750,   750,   750,   750,   750,   750,   750,   750,   750,   750,
	   750,   750,   750, -1000,   750,   750,   750,   750,   750,   750,
	   750,   750,   750, -1000, -1000,   750,   750, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,  2086, -1000,  1883,  1739,  1739,  1739,  1565,
	  2222,  2177,  1739,  1739, -1000, -1000,  1669,  2177,  1739,  1391,
	  2222,  1739,  1761,  1899,  1739,  2222,  2468,  2183,  1739,  2222,
	  2222, -1000,  1739,  1739,  2222,   667,  1981,   644,   644,  1808,
	 -1000,    61,  4000, -1000,  4000, -1000, -1000, -1000,  3974, -1000,
	  1946,  2177,  1980,  1663,  1391,  1304,  2177,  2222,  2177,  1391,
	  1391,  2177,  2645,  1268,  1339,  1391,  1249,  1716,    30,  2177,
	   865,  2265,  2116,  2641,   384,  2546,  1944,  1194,   930,  1097,
	  1866,  1085,  1097,  1088,  1088,   699,  1564, -1000,  2092,  1932,
	   644,   253, -1000,  1830,   709,   384,   384,  2030, -1000, -1000,
	 -1000,   750,   384, -1000,  1196, -1000,  1087,   384,  1085,  1085,
	  1071,    98,  1082,  1657,  1172, -1000,  1372,  1071,   630,  2025,
	  1754,  1070,  1733,  1196,  1067,  1175,  1172,  1172,  1773,   750,
	  2575,  1065,   384,  1063,   -47,   -47,  1102,  1656,   -47,  1061,
	   -47,   -47,  1056,  1053,  1084,  1655,   -47,   -47, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,  1653,  1823, -1000,  1438,
	 -1000, -1000, -1000,  3666, -1000, -1000, -1000, -1000, -1000,  2177,
	 -1000, -1000,   704, -1000,  2616,  2372, -1000, -1000, -1000,  2612,
	 -1000,  2794,  1391, -1000,  2222,  1739,  1739,  2222,  1391,  1739,
	  1739,  1739,    86,  2222,  2222, -1000, -1000,  1391, -1000,  1391,
	 -1000,  2371,   644,  1890, -1000, -1000, -1000,  2719, -1000, -1000,
	  2178, -1000, -1000,   379, -1000, -1000, -1000, -1000, -1000, -1000,
	   865, -1000,  1132, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,  2630, -1000, -1000,   946,   384, -1000,   384, -1000,
	 -1000, -1000, -1000,  1665, -1000, -1000, -1000,   384, -1000,  1665,
	 -1000,   750, -1000,   384,   384,   384,   384,   384,   384,   384,
	   384, -1000,  1080,   384, -1000,   384, -1000, -1000,   -47, -1000,
	 -1000, -1000, -1000, -1000,   -47, -1000,   -47, -1000, -1000, -1000,
	 -1000,  2168, -1000,  2222, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,   384, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,  1122, -1000, -1000,   750,   208,
	  2639,   236,    73, -1000,   282, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,  1651, -1000,  2560,  1211,  1050,  2043, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,  1164,  1249, -1000,   921, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1163,
	  1808, -1000,  1808, -1000, -1000, -1000, -1000,  3794, -1000,  4650,
	    31,    31, -1000,  2527, -1000, -1000,    31,    31, -1000,  1928,
	 -1000,  1928, -1000, -1000,   174,   958,  1902, -1000,   958, -1000,
	 -1000,   631,  1899,  2177, -1000,   173, -1000,  2222, -1000,  1808,
	   174,  1001, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  2566,
	  1391, -1000,    86, -1000,  4650,  1760,  1808,  1808,   174,   174,
	 -1000,   746, -1000,  2114,  2114, -1000, -1000,  1808, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,    70,  1649,   631, -1000, -1000,   174, -1000, -1000,
	 -1000,  2648,  1681,  2648, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,  2177, -1000,  1915, -1000,   153,  -110,   631, -1000, -1000,
	    61,   174, -1000,  1899,  1883, -1000,  3974,  1645,  1888, -1000,
	  1795, -1000, -1000,  2385,  1213, -1000, -1000,  1874, -1000, -1000,
	 -1000, -1000, -1000,   667,   956, -1000, -1000, -1000,   631, -1000,
	 -1000,  2198, -1000, -1000, -1000, -1000, -1000, -1000,  1971,   373,
	    31,  2525, -1000, -1000, -1000, -1000, -1000,  1787, -1000, -1000,
	 -1000, -1000, -1000,   236,  2604, -1000, -1000,   236,  2603,  1047,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1860,
	  2225, -1000,    98, -1000,    62, -1000,  1043,  1640,  1340, -1000,
	   630, -1000, -1000, -1000,  1858,    62, -1000,   630, -1000,  1637,
	 -1000,    96, -1000, -1000,  1395, -1000, -1000, -1000, -1000, -1000,
	  1674, -1000,  1636, -1000,  1971, -1000,  1054, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,  1158, -1000,  1184, -1000,   384,  2025,
	 -1000,  1030, -1000,  2575,  1158, -1000, -1000,    62,  2602, -1000,
	  1563,  1157, -1000, -1000,   -67, -1000,  1826, -1000,  1634,  1023,
	 -1000, -1000, -1000,   196, -1000, -1000,   384,   384, -1000, -1000,
	  1582, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1722,  1721,
	  2025, -1000,  1540, -1000, -1000, -1000,   384, -1000,  1140,  1956,
	   931, -1000, -1000, -1000,  1002,  1854,  1854, -1000,   531,  1854,
	  1854, -1000, -1000, -1000,   591,  1729, -1000, -1000, -1000,  2575,
	 -1000, -1000,   750, -1000,   828,   384,  1929,   335, -1000, -1000,
	 -1000, -1000, -1000, -1000,  1926, -1000, -1000,    62,  2602,  1022,
	   505,    62,  1021,   996, -1000,  1379,  1379, -1000, -1000,   384,
	  1013,   535,   996, -1000,  2177,  2222, -1000, -1000, -1000, -1000,
	 -1000,  2494, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   174,
	  2236, -1000, -1000,   272, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,  2177,  1899, -1000, -1000, -1000, -1000, -1000,
	   631, -1000,   631,   631, -1000, -1000, -1000, -1000,  2246,   874,
	  2222,  2222,  2278,  2368, -1000, -1000,   631, -1000,   631,   631,
	 -1000, -1000,  2114,  2721, -1000,  2488, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,   266,   865,  1978,  1772,
	 -1000,   384, -1000,  2175,  1681, -1000,   721,   750, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,   384,   384,   384,   384,   384,   384,   384,   384,   750,
	   750, -1000,   127,   384, -1000,   505, -1000,   505, -1000, -1000,
	 -1000,   384, -1000, -1000, -1000,  2113, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,  -184,  -184,   900,  1455, -1000,
	  2357,  -180,  -180, -1000, -1000, -1000, -1000,  1327, -1000, -1000,
	 -1000, -1000, -1000,   -68, -1000, -1000,   750, -1000, -1000,  2601,
	  1156, -1000,   228,   228,   993, -1000,  1630,  1550, -1000,  1304,
	 -1000, -1000,  1629,  1550, -1000, -1000, -1000,   750,  2576,   750,
	    62,    62, -1000, -1000, -1000, -1000,  1739,  1808, -1000, -1000,
	  1618,  1618, -1000, -1000, -1000,   174,  1987,  1616, -1000, -1000,
	 -1000, -1000,   631, -1000, -1000,   -24, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  -169,
	 -1000, -1000,  4650, -1000,  1086,  1808,  1808,  1974,   -23,   -23,
	  1681, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,  1391, -1000,  1391, -1000,  1069,  2477,  1391, -1000,
	 -1000,  1391, -1000,  1391, -1000, -1000,   174,  2222,   237,  2222,
	 -1000,  1902,  1391, -1000,  1808,  1391,  2222,  2469, -1000, -1000,
	 -1000,  2566, -1000, -1000, -1000, -1000,  2331, -1000, -1000,  1758,
	 -1000,  2177, -1000, -1000, -1000,  2525,   -58,  1391,  2525,  1274,
	 -1000,   231,   201,  2633,    73,  1919, -1000, -1000, -1000,  1883,
	  3460,  2222, -1000, -1000,  4107,  1391,   174, -1000, -1000, -1000,
	   174,   916,  1883, -1000,  1391, -1000, -1000, -1000,   225,  1391,
	  1808,  1808, -1000, -1000,  2525,  1391,  1391,  1391,    70,   631,
	  1606, -1000, -1000, -1000, -1000, -1000,   174, -1000, -1000,   174,
	  1765, -1000,  1249,  2615,   -56,  1398, -1000,   174, -1000, -1000,
	  2582,  1602, -1000,  1420, -1000,  2470, -1000,  3521,  1844,   631,
	  1598,  1598, -1000, -1000, -1000,    30, -1000,  2122,  2146, -1000,
	  2726,  1761, -1000,  1391,  1391, -1000,  1391,  1391,   778,   947,
	 -1000,   228,   228, -1000,    73,  1156,    73, -1000,   905, -1000,
	 -1000,   496, -1000,  2103, -1000,   630, -1000, -1000,  1853, -1000,
	  1208, -1000,  1133, -1000, -1000, -1000, -1000,  1596,   935, -1000,
	   228, -1000, -1000,  -112,   910, -1000, -1000, -1000, -1000,   906,
	 -1000,  -113, -1000, -1000, -1000, -1000, -1000,  1887,  1006,  1006,
	 -1000, -1000,  2333,  1877, -1000,   557,  2587,   384,  1922, -1000,
	 -1000, -1000, -1000, -1000, -1000,  1184, -1000,  2025, -1000,  1521,
	   750, -1000,   893, -1000,   -18,   228,    73, -1000,  1131,   -18,
	   384, -1000,   574,  1127, -1000, -1000,  1582,   228, -1000, -1000,
	  1225, -1000,   -18, -1000,    98,   851, -1000,   -18,   969, -1000,
	   384,   568,  1123, -1000,   -18,   952, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	   981, -1000,   168, -1000,   453, -1000,   843, -1000, -1000,   747,
	 -1000, -1000, -1000,   565, -1000,  1121, -1000, -1000,   -18, -1000,
	   940, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1920,
	   931, -1000, -1000,  8774, -1000,   806, -1000, -1000, -1000,    62,
	 -1000,   799, -1000,  -114,  -115,  -116, -1000,   228,   928, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1459,  1378,
	 -1000, -1000, -1000, -1000,    73,   796, -1000,   -18,    73,    62,
	   886, -1000, -1000, -1000, -1000, -1000, -1000,   752, -1000,   -18,
	  1539, -1000,  1595,   745, -1000,   -18, -1000, -1000,   732, -1000,
	   -18,   875,   718, -1000,   -18,   384,   860, -1000, -1000, -1000,
	 -1000, -1000, -1000,  1902,  1902,    31,    31, -1000,  1808,  2353,
	 -1000,  1321,   958,   631, -1000,  3468, -1000, -1000, -1000, -1000,
	 -1000, -1000,   631, -1000, -1000, -1000, -1000, -1000, -1000,  1594,
	 -1000,  2222, -1000,  2633,  1843, -1000,  1304, -1000, -1000, -1000,
	  1588, -1000,  7951, -1000, -1000, -1000, -1000,   853, -1000, -1000,
	 -1000, -1000, -1000, -1000,  1074,  1074,   630,   630,   630,   721,
	   630, -1000,   630, -1000, -1000, -1000, -1000,   630, -1000, -1000,
	 -1000,   631, -1000,  2080, -1000,  1464, -1000,   883,  1464,  1581,
	   883,  1551, -1000, -1000,  -117, -1000, -1000, -1000, -1000,   815,
	  1464,   883,   609, -1000, -1000,  1717, -1000,   180, -1000, -1000,
	 -1000, -1000, -1000,  2104, -1000,   236, -1000, -1000,    88, -1000,
	 -1000, -1000, -1000,  1576, -1000,   166, -1000,   794, -1000, -1000,
	 -1000,   166,  8424, -1000, -1000, -1000,   750,  2576, -1000,   690,
	 -1000,   228, -1000, -1000,   174, -1000,   174,  1739, -1000, -1000,
	  3794,  2199, -1000, -1000,   174, -1000, -1000,   174,   -23,   -23,
	   -23, -1000, -1000, -1000,  1681,  1681, -1000, -1000, -1000, -1000,
	    31,    31, -1000,  1829,  1554, -1000,  2636, -1000, -1000, -1000,
	  1391,   174,  2182,  7710, -1000, -1000,  2174,   174, -1000,  2172,
	  2250,  2246, -1000,   397, -1000, -1000,  -169,  1808,   832,  2177,
	   174, -1000, -1000, -1000,  7710, -1000, -1000,   397, -1000, -1000,
	   174, -1000, -1000,  2241, -1000,  1520, -1000, -1000,   634,  1808,
	  2206,   237,   631,  1554,   634,  2134, -1000,  1304,  1304, -1000,
	  1570, -1000,    73,  2592,   882,   631,  2222,  1568, -1000, -1000,
	 -1000,   631, -1000,   397,  1391,  1391, -1000,  2264,  2379,  2230,
	    31, -1000,  1110, -1000, -1000, -1000, -1000, -1000,  1971,  1567,
	   397, -1000,  1902,  1554, -1000,   174, -1000,  1971,  2073,  1391,
	  4107, -1000, -1000,  1391,  1391, -1000,  1249, -1000,   174,  2222,
	  2088, -1000, -1000, -1000,  2222,  2385, -1000, -1000, -1000, -1000,
	 -1000,  1391,  1213, -1000, -1000,  1681, -1000, -1000,  2222,  1844,
	 -1000, -1000, -1000,  1193, -1000, -1000,  1419, -1000,  1086,  1086,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,  1086,  1086,  1012,   174,
	 -1000,   174, -1000, -1000, -1000, -1000,  1391, -1000,   631,  2406,
	 -1000,  1317, -1000, -1000,  2525,  2525, -1000, -1000, -1000, -1000,
	  2429, -1000,  1971,   634,  2630, -1000,  2388,   236, -1000, -1000,
	 -1000, -1000, -1000, -1000,   677, -1000,  -127, -1000, -1000, -1000,
	 -1000, -1000,  1100, -1000, -1000,  -128, -1000, -1000, -1000,  2069,
	 -1000,  1983, -1000, -1000,   388,   660, -1000,    69, -1000, -1000,
	  1298,  -129, -1000,    62,  -130, -1000, -1000,    96, -1000,  1395,
	   137,  1006, -1000,  1477, -1000, -1000, -1000, -1000,  2397,   634,
	  2193,  2470,  7710, -1000,  -154,   653, -1000,    73, -1000, -1000,
	    73,  1904, -1000,  2028, -1000,   825, -1000,  1742,   168,   168,
	   168,  1382, -1000,   453,  2575, -1000,    62,  1128, -1000, -1000,
	 -1000, -1000,  1563,   384, -1000, -1000,    62, -1000,   -18,  1099,
	 -1000,   779,  -134, -1000, -1000,   384,   959, -1000,   196,   375,
	   384, -1000, -1000,  1582,  3116,  4423,  2575,  1540, -1000,   646,
	 -1000, -1000, -1000, -1000, -1000,   646,  1483, -1000, -1000, -1000,
	 -1000, -1000,   384, -1000,  1582, -1000,   747, -1000, -1000, -1000,
	 -1000,   750, -1000, -1000, -1000,   492,   607,   607,   607, -1000,
	    62, -1000, -1000, -1000, -1000, -1000, -1000,    62,   384, -1000,
	   562, -1000,    62, -1000,    62,  1379,   582, -1000,   -18,   -18,
	 -1000,  1379,   505, -1000,  1379,   505, -1000,   -47, -1000,   384,
	   505,   555, -1000,  1558,  1213,  1213,  1391,  1391, -1000, -1000,
	 -1000, -1000, -1000, -1000,   667,  1712, -1000, -1000, -1000, -1000,
	  1279, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   631,
	 -1000,   -69,   351, -1000, 10018, -1000, 10018, 10018, -1000, -1000,
	 -1000,   230,   174,  2191, -1000, -1000,   725, -1000, -1000, -1000,
	 -1000,   697, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	  9899,  9780,   174,   174, -1000, -1000,   228,   228,   673, -1000,
	  1234, -1000,   174, -1000,  1760, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,  1826, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  2222,  1126,
	   748,  -184, -1000,   -54, -1000, -1000,   596,  -184, -1000,  1551,
	   575,   205,  1208,   883, -1000,  -190,   596, -1000, -1000, -1000,
	 -1000, -1000,  1464, -1000,  1245, -1000,   750,  2587,   236, -1000,
	 -1000, -1000, -1000,   669,  1550, -1000, -1000, -1000, -1000,   750,
	  8081, -1000, -1000, -1000,    62, -1000, -1000,  1014,  1014,   397,
	 -1000, -1000,  1092,  1014, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,   827,  1681, -1000, -1000, -1000, -1000, -1000,  1391,
	  1391, -1000, -1000, -1000,  2720,  4200,  1086, -1000, -1000, -1000,
	  1391, -1000,  2222,  -101, -1000,  7710,  7415, -1000, -1000, -1000,
	 -1000, -1000, -1000,  3616,   518,   174, -1000,    31, -1000,  1391,
	 -1000, -1000, -1000, -1000, -1000,  1945,  1391,  1342, -1000, -1000,
	  1681,  2683,   237,  1391,  -101, -1000,  1391,  1886,  1974,  2787,
	 -1000, -1000,   631, -1000,  1923, -1000, -1000,   634,  1971,  2630,
	 -1000,  2194,  2525,  1069,  2429,  7710,  2701,  1877, -1000, -1000,
	 -1000, -1000,    73,  1808,  1739,  1739, -1000,  1739,   631, -1000,
	   631, -1000, -1000,   397,   397, -1000, -1000,   174,  2397, -1000,
	  2320,  -169, -1000,  1391,  1547,  1808,  2204, -1000, -1000,  1391,
	  1213,  2525,  1391,   882,  2222,  1545,   631, -1000, -1000, -1000,
	   631, -1000,   397,   397, -1000, -1000,  1336, -1000, -1000,  1542,
	 -1000,   397, -1000,  1681,  1844, -1000, -1000,   174,   232, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,   631, -1000,  -169,   631,
	 -1000,   631, -1000,  -169,   631, -1000,  1014,  1014,  1026, -1000,
	  -169,  1761,  2717, -1000,   454,   597,  1317,  1391,  1391,  2627,
	   777,  1974, -1000,   454,  2175,  1681, -1000, -1000, -1000,   905,
	 -1000,   384,  1270, -1000,   915,   -66, -1000,  2049,  -142, -1000,
	 -1000, -1000, -1000,  1208,  4469,  1541, -1000, -1000,   -90, -1000,
	 -1000,   229, -1000,  1395, -1000, -1000, -1000, -1000, -1000,   228,
	 -1000, -1000, -1000, -1000,  2627,  1974,  2333, -1000, -1000,  2501,
	  1974,  -101, -1000,   228,  2313, -1000, -1000,   644, -1000, -1000,
	    73, -1000,   649, -1000,  2575,  1521,   168,   168,   646, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   637,   538,
	 -1000, -1000, -1000,  1225, -1000,  1332,  1219,   528,    62, -1000,
	 -1000, -1000,   228, -1000, -1000, -1000, -1000, -1000,   523, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   228, -1000,
	 -1000,   495, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   552, -1000,   -18,
	 -1000, -1000, -1000, -1000, -1000,   463,   489, -1000, -1000, -1000,
	 -1000,   228, -1000, -1000,   480, -1000, -1000, -1000,   -18, -1000,
	 -1000, -1000, -1000,  1539,   505,   535, -1000, -1000, -1000, -1000,
	   424, -1000, -1000, -1000,   -18, -1000, -1000,    31,    31, -1000,
	 -1000,   631, -1000, -1000,  2222,  2222,  2222, -1000, 10018, 10018,
	   186,  1391, 10018, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,    55, -1000, -1000,   237,  3709, -1000, -1000,  2191,
	  3709, -1000, -1000,  2558, -1000,   234,  2305, -1000,  7710, -1000,
	   143,  1977,  1213, -1000, 10018, 10018, -1000, -1000,    62,  1538,
	 -1000,  1391,   750,   631, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,  1534,  -184, -1000, -1000, -1000, -1000, -1000,  1464,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,  1346, -1000, -1000, -1000, -1000,   466,  1464, -1000,
	 -1000,  -180,   883, -1000,  -190,  -180, -1000, -1000, -1000,  2560,
	    73,   947,   421, -1000, -1000, -1000, -1000,   750, -1000,   397,
	  1468,  4577, -1000, -1000, -1000, -1000,   397, -1000,  1391, -1000,
	    31,    31,    31, -1000, -1000, -1000,   397,   237, -1000,  1086,
	  1899,  1086,   237, -1000, -1000,  3684, -1000,  1086, -1000, -1000,
	  4156,  1086,  1681,  3804, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,  2461,  2400,  4126,  4126, -1000, -1000,  2398,
	 -1000, -1000, -1000, -1000, -1000,  1883,   397,   469,  7710,  7710,
	 -1000, -1000, -1000,     3,   237,   200,  1971,  2175,  9698,  2479,
	  2095, 10018, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	  2659, -1000, -1000,  1734, -1000, 10018,  2659,  2376,  1103,   397,
	   396, -1000, -1000, -1000,  1808,   547, -1000,  1255,   397, -1000,
	 -1000,   397, -1000,  1468, -1000,  1870,  1310,   -46,   882,   631,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1520, -1000,   634,
	   454, -1000,  2175, -1000,   634,   631, -1000, -1000,  1829,  2627,
	 -1000,  -101, -1000,   865, -1000, -1000, -1000,   631,   631,   631,
	 -1000, -1000,  5691,  2627,  1899, -1000,  1411, -1000,  -143, -1000,
	 -1000, -1000,    31,    31, -1000, -1000,  1971,   397,  1468,  2246,
	   631, -1000, -1000, -1000,   397, -1000,  -169,  -169,  1997,  1413,
	 -1000, -1000, -1000, -1000,  1739,   232, -1000, -1000,  1086,  2267,
	  1086,   276,   397,   397, -1000, -1000,  1391,  2117, -1000, 10018,
	  2406,  1761, -1000,  2454,  2447,  2423,  2416,  2525, -1000,   454,
	 -1000, -1000,  2097, -1000,   317, -1000, -1000,  3709, -1000,  9398,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1501,
	 -1000,  -150, -1000,  -153, -1000,   384, -1000, -1000,  1715, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,  2097,  3709, -1000,  7710,
	 -1000,  3709,   136, -1000,   617,   375, -1000, -1000,  -154, -1000,
	   750, -1000, -1000, -1000,   168, -1000,   384, -1000, -1000,   427,
	 -1000,  -159, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   415,
	   228, -1000,   -18, -1000, -1000, -1000,  1483,   238,   605, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,  2041, -1000, -1000,
	   372, -1000,   -47,  1391,  1391,  1681, -1000, -1000, -1000, -1000,
	  -123, -1000, -1000, -1000, -1000, -1000, -1000,    55, -1000,  3709,
	  3709,  3709, -1000,  3709,    55, -1000, -1000, -1000,  2305, -1000,
	 10018, -1000, -1000, 10018, -1000,   172, -1000, 10018, 10018, 10018,
	 -1000,   329,   222,   228, -1000,   174, -1000, -1000,    31, -1000,
	  2554,   748, -1000,  1464, -1000,   -87, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,  1464, -1000,   596, -1000, -1000, -1000,
	 -1000, -1000,  1839, -1000, -1000, -1000,   397, -1000, -1000, -1000,
	 -1000,  1739,  1739,  1391, -1000, -1000,   174,  1775,  1391,   237,
	 -1000, -1000,  2472,  -163, -1000,  3794, -1000, -1000,  2323,  1681,
	  3804,   397, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,    31, -1000, -1000,
	  2691,  1086, -1000,  1391, -1000,  2308, -1000, -1000,   -99, -1000,
	 -1000,  1877, 10018, -1000, -1000,   241,   174,  2169, -1000,  9460,
	 10018, -1000, -1000, -1000, -1000,  2479, -1000,  9460, -1000,  -123,
	 -1000, -1000,   174,  1580, -1000, -1000,   241,   237, -1000,  1808,
	 -1000, -1000, -1000, -1000,  2683,   578,  1391,  1714, -1000, -1000,
	 -1000,  2666,  1681,  2320,  1391, -1000,  1964,  1498, -1000, -1000,
	 -1000, -1000,  2222, -1000, -1000,  2787, -1000, -1000, -1000,  7951,
	  2429,  1069, -1000, -1000,  2097, -1000,   397, -1000, -1000, -1000,
	  -123,  5691,  5691,  5691,   228, -1000,   228, -1000, -1000, -1000,
	    31, -1000,  6977, -1000,  1844,  1391, -1000, -1000,   225,  1739,
	  1739,  1808, -1000,  1391, -1000, -1000, -1000,  1336,  4046,  4046,
	 -1000, -1000,  7710,  2525, -1000,   397, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,  1494, -1000, -1000,  1681,
	 -1000, -1000, -1000, -1000, -1000, -1000,  1014, -1000, -1000, -1000,
	  2695,  -123,  2066,  1761, -1000, -1000, -1000, -1000, -1000,  1391,
	  2525,  1760, -1000,  1251, -1000, -1000, -1000, -1000,   631, -1000,
	   298, -1000, -1000,   631, -1000,   -69,   222, -1000,   384,  -168,
	 -1000, -1000,   384, -1000,   416,  1808,   631,  -101,   631,  -123,
	   644, -1000,    62, -1000, -1000, -1000,  2705, -1000, -1000,   363,
	 -1000,  1332,   411,  1826, -1000,   505, -1000, -1000, -1000, -1000,
	 -1000,  1973,  1713,   362, -1000,   357, -1000, -1000, -1000,  3709,
	 -1000,    55, -1000, -1000,    55, -1000, -1000, -1000,   214,  -123,
	 10018, -1000,   -39,    55,    55, 10018,  1554,   353,  1883,  1681,
	  1391,  -132, -1000, -1000, -1000,   883, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,  1468,   631,  4577, -1000, -1000, -1000, -1000,
	 -1000,  1086,   631, -1000,  1808,  1808,  1808,  1086,  1014,   631,
	 -1000, -1000,  1472,   397, -1000,  3804,   237, -1000,  3766,  1681,
	  2344,  3474,   397,  1894,  1633, -1000, -1000,  2470,  -123, -1000,
	    61, -1000, -1000,  2376,   237,  -140, 10018,   237,   -39,  1526,
	 -1000,   174, -1000, -1000, -1000,   174, -1000,   412, -1000,   237,
	  1808, -1000,  1898,  1704, -1000,    61, -1000, -1000,  2627, -1000,
	 -1000, -1000,  5691,  7612, -1000, -1000, -1000, -1000, -1000,    55,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   -39, -1000,  3794,
	  3794,   631, -1000,  1468, -1000, -1000, -1000, -1000, -1000,  1468,
	 -1000, -1000, -1000,  4577,  4577, -1000, -1000, -1000,   242, -1000,
	  -101,  1391, -1000, -1000, -1000,   397,   865, -1000,  7710,   174,
	 -1000,   174, -1000,  1086,   317, -1000, -1000,  3709, -1000, 10018,
	   186, 10018,  1391,   800, -1000, -1000,   384,   384, -1000,  3804,
	  1703, -1000,  1086,  3709,   338,   228, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	  2681,  1797,  2676,   401, -1000, -1000, -1000, -1000, -1000, 10018,
	  -123, -1000, 10018, -1000, -1000,    15,  2720, -1000, -1000,  1467,
	   397, -1000, -1000,   748,  1464,  2554,   -87,   748, -1000,   174,
	 -1000,   237,  1775, -1000,  1086, -1000, -1000,  1014, -1000,  2259,
	 -1000, -1000,  1839,  2338, -1000, -1000,  3804,  1086, -1000,   237,
	 -1000,  1681,  1894,  2308, -1000, -1000, -1000,  2675,  1443,  2501,
	 -1000, -1000, -1000, 10018,  -140, -1000, 10018,   237,  1526,   239,
	 -1000,  1698,  2666, -1000,  1877,  1416,    45, -1000, -1000, -1000,
	 -1000,  5691, -1000, -1000,  5691,   631,   631,  1391, -1000, -1000,
	 -1000, -1000,  3772,  1413, -1000, -1000,  -101,  1391, -1000,  1391,
	  2283, -1000,  2099, -1000, -1000, -1000,  -123, -1000,   384, -1000,
	 -1000, -1000,   327,   631, -1000,   174,  -123, -1000,   314, -1000,
	 -1000,  2171, -1000, -1000,  1696,  -123,    55, -1000, 10018,   237,
	   174, -1000, -1000,  1681, -1000, -1000,  2309, -1000, -1000,  4577,
	 -1000, -1000,  -169, -1000, -1000,    92, -1000,   397, -1000,  1360,
	 -1000,  3804,  3804,   237, -1000, -1000, -1000, -1000, -1000,  -123,
	 10018,   631,  -123, -1000,   237,   237, -1000,  4577,  4577,   300,
	 -1000, -1000,  1882,  1686,    55,   -39, -1000, -1000,   397,  2525,
	   397, -1000,   397, -1000,  2694,  2610, -1000, -1000, -1000,  3804,
	  1086,  1086, -1000, -1000,   293, -1000,    55, -1000,  1681,   237,
	  2554, -1000,  2554,   397,   174,  4355,  4434, -1000, -1000, -1000,
	 -1000, -1000,  1839, -1000, -1000, -1000,  3804,  -123, 10018, -1000,
	 -1000, -1000, -1000, -1000,  1294,  1879, -1000, -1000,  5691, -1000,
	  4046,  1391, -1000,  1391, -1000,  1391,   540, -1000, -1000, -1000,
	 -1000,   631, -1000, -1000,   237, -1000,  -132,   -87, -1000, -1000,
	   397, -1000, -1000, -1000, -1000, -1000,  4577, -1000, -1000, -1000,
	  -123, -1000,  1678,    55,   237, -1000, -1000, -1000,   631, -1000,
	 -1000,   540,    31, -1000,  1086, -1000, -1000, -1000, -1000,  4355,
	   397, -1000,    34, -1000, -1000,   540, -1000, -1000, -1000,   237,
	 -1000, -1000,  4577,  1672, -1000, -1000, -1000, -1000
};

short yypgo[] = {
	     0,  4961,  3002,  4960,  4959,  4958,  4957,  4952,  4949,  7420,
	  4947,  4946,  4945,  4944,  4943,  4942,  4941,  4939,  4934,  4933,
	  4932,  4931,  4930,  4929,  4927,  4919,  4916,  4915,  4914,  4913,
	  4912,  4910,  4907,  4902,  4901,  4900,  4899,  4897,  4894,  4889,
	  4887,  4885,  4884,  4882,  4881,  4878,  4877,  4870,   394,  4869,
	  4867,   392,  4866,  4862,  4858,  4856,  4851,  4850,  4846,  4837,
	  4836,  4835,  4834,   388,  4833,  4832,  4831,  4830,  4824,  4821,
	  4820,  4819,  4816,  4809,  4808,  4805,  4803,  4801,  4799,  4798,
	  4797,  4794,  4793,  4791,  4789,  4783,  4782,  4781,  4779,  4778,
	  4776,  4775,  4774,  4773,  4771,  4769,  4768,  4765,  4764,  4761,
	  4760,  4759,  4758,  4757,   223,   552,  4756,  4753,  4751,   262,
	  2014,  4750,   304,   302,  4749,   368,   364,  4747,  4746,  4743,
	  4741,    94,   427,  4740,   414,  4739,   134,   384,   140,     0,
	  4735,   137,   325,  4730,  3026,  4727,  4726,   393,   102,  4725,
	  4723,  4718,   336,  4686,  4715,  4714,    31,   155,  4712,  4711,
	  4710,   387,    58,  4708,  4707,   483,   332,  4706,  4704,   237,
	  4703,  4699,  4697,  4287,  4695,   296,  4692,  4691,  4690,  4689,
	  4688,   675,  4684,   161,  4683,  4680,    26,  3054,    28,   162,
	  4679,  4678,  4677,   322,  4675,  2737,    89,  4674,   115,    96,
	    97,  4673,   389,   243,  4672,  4670,   318,  4669,  4667,  4666,
	  4665,  4663,  4662,  4661,    21,   383,    23,  4660,   127,   104,
	  4659,  4655,  4654,  4652,  4651,   419,   235,  4650,  4649,  4647,
	  4646,    91,    67,    27,    14,   112,  3564,  2967,  4645,    24,
	  4643,  4642,  4638,  4635,  4633,   122,   310,    22,   862,   326,
	   300,   379,   110,    15,  4629,    17,  4623,  4620,  4619,  4618,
	  4617,  4616,  4615,  4613,   415,  4608,   382,  4607,  4605,   420,
	  4604,  4603,  4602,  4601,  4600,  4599,  4597,   141,  4596,    90,
	   148,  4590,    48,  4589,   380,  4586,    50,  4585,  4584,  4583,
	   404,  4582,   263,   103,    54,  4581,    95,    76,    42,   305,
	    60,    74,    80,  4579,   323,  4576,  4575,   113,    99,  4574,
	   315,   303,    36,  4573,    57,    32,  4571,  4570,  4569,  4566,
	   136,  4564,  4563,  4562,  4561,   132,  4560,   133,  4555,  4553,
	  4550,  4549,  4547,  4544,    70,  4543,  3010,  4542,   154,  4541,
	  4540,  4539,  4538,  4528,   142,    61,    56,    63,    65,  4526,
	    10,    64,    44,   376,   165,  4524,    34,  4522,   107,    77,
	    46,  4521,  4520,  4518,  4517,  3011,   149,  4516,  4511,  4509,
	  4508,   124,  3915,  4507,   156,  4506,  2994,   254,  4501,   208,
	    98,  4500,  4495,   227,   143,   106,   294,   150,  4494,   184,
	  4492,  4490,     8,  4489,   340,  4488,   344,   244,   286,  4486,
	   239,  4484,    62,    30,  4482,   224,   123,  4481,  4480,  4478,
	  4477,   307,  4475,  4474,  4472,  4471,   118,    59,  4470,    55,
	   238,  4469,   295,   120,    43,  4468,    47,  4463,  4460,  4458,
	  4457,   138,  4455,    51,  4454,  4453,  4452,  4451,  4450,  4449,
	   216,  4448,  4447,  4446,  4444,  4443,    49,  4442,  4441,     1,
	     6,  4440,  4436,    29,  4435,  4434,  4433,  4431,  4429,  4428,
	   236,   101,   163,   145,   114,  4427,  4421,  1912,   144,   100,
	  4420,  4419,  4416,  4415,   146,  4414,  4413,  4412,    33,  4408,
	  4406,    92,   177,  4405,  4403,    45,    78,  4402,  4400,  4394,
	  4393,  4390,   166,  4387,  4384,  4381,  4378,  4377,  4376,  4375,
	  4374,  4373,  4372,  4371,   121,   111,    75,  4368,   170,  4364,
	  4362,   176,  4360,  4359,  4358,  4357,  4356,  4355,    53,  4351,
	  4348,  4347,   329,  4345,  4344,  4343,  4341,  4339,  4336,  4335,
	  4333,  4332,  4330,   288,  4329,  4328,  4323,  4322,   422,  4318,
	   241,  4317,  4316,  4315,  4314,  4313,  4311,  4305,  4304,  4301,
	  4296,  4295,  4292,  4291,  4290,  4289,  4288,  4286,  4283,   130,
	  4282,  4281,  4280,  4276,  4274,  4273,  4268,  4266,  4265,  4264,
	  4254,  4253,   250,  4252,  4250,  4249,  4247,  4246,  4245,  4244,
	   317,   324,   319,  4243,  4242,  4241,  4240,  4238,  4237,  4235,
	   228,  4233,  4232,   147,  4231,  4219,  4218,  4217,  4216,  4213,
	   381,   417,  4212,  4210,  4209,  4207,  4206,  4205,  4203,  4202,
	  4201,  4200,  4191,  4190,  4187,    37,  4185,  4184,  4182,    20,
	    52,  4181,    83,  4180,  4178,  4177,  4175,   225,   139,  4174,
	  4173,   231,  4172,  4171,  4168,  4166,  4165,  4164,  4159,  4156,
	  4155,  4152,  4147,  4146,  4145,  4144,  4143,  4142,  4140,  4137,
	  4136,  4134,  4131,  4129,   299,   374,   298,   297,   301,   126,
	  4127,   233,  4126,  4121,  4117,   378,  4113,   129,   314,   131,
	  4110,  4108,  4107,  4104,   377,  4102,  4098,  4095,  4094,  4092,
	  1739,   551,  4091,   135,   337,  4089,  4084,  4083,  4082,   160,
	  4081,  4079,  4078,  4076,    12,    13,    11,  4073,  4071,  4068,
	    68,  4063,  4060,  4057,    38,  4056,    69,  4055,  4052,    35,
	  4048,     7,     3,     2,  4045,  4040,  4037,  4036,  4035,  6811,
	  4034,  4033,   234,  4031,  4029,  4028,  4025,  4024,  4023,    18,
	  4021,  4017,  4012,  4010,  4009,  4005,  4004,  3996,  3995,  3992,
	  3991,  3990,   369,  3989,   375,  3985,  3978,  3973,  3972,  3971,
	  3970,  3969,  3965,  3963,  3962,  3961,  3960,   371,  3958,   213,
	  3957,  3956,  3950,  3949,  3948,  3947,   229,  3946,   246,   363,
	  3944,   211,  3941,  3940,  3939,   159,  3937,  3936,  3934,  3933,
	  3932,  3931,  3930,  3925,   153,  3923,   152,  3922,  3920,  3919,
	  3916,  3914,  3003,  3913,  3912,  3907,  3027,  3906,  3905,  3904,
	  3903,  3896,  3895,   291,   207,   517,  3894,  3001,  3890,   293,
	   187,   292,  3888,  3887,  3886,  3885,  3884,   282,  3883,  4090,
	  3881,  3880,  3879,   290,  3876,   400,   342,  3875,  3874,  3873,
	  3872,  3870,  3869,  3862,  3861,   242,  3860,  3858,   338,  3857,
	  3856,   327,  3855,  3854,  3851,  3849,  3848,   119,  3847,   199,
	  3845,  3844,  3840,  3839,  3837,    93,   125,  3836,  3834,  3827,
	  3826,   240,  3824,  3823,  3822,  3821,  3820,  3819,  3818,  3817,
	  3816,  3815,  3814,  3810,  3809,  3808,  3806,  3805,  3795,  3793,
	  3791,  3790,  3789,  3787,  3786,  3784,  3783,  3782,  3781,  3776,
	  3774,  3772,  3771,  3767,  3762,  3761,  3759,  3757,  3756,  3753,
	  3751,  3749,  3748,  3744,  3739,  3733,  3731,  3730,  3729,  3728,
	  3727,  3726,  3725,   200,  3724,  3721,  3718,  3717,  3716,   401,
	  3710,  3707,   373,  3704,  3703,  3702,  3701,  3695,  3693,  3688,
	   366,  3686,    79,  3683,  3678,   411,  3677,  3675,   370,   367,
	  3667,   281,   206,  3664,   128,   204,  3663,   202,  3661,  3658,
	   117,  3655,   151,  3651,  3648,  3644,  3643,   201,  3641,  3633,
	  3632,  3629,   372,  2712,  3628,  2481,  3627,  3626,   274,  3625,
	  3623,  3606,  3605,  2934,  3603,  3598,  2473,   399,  3597,  3595,
	   269,  3593,   365,  3592,  3590,  3589,  3586,  3584,  3582,  3580,
	   198,  3575,  3574,  3573,  3570,  3569,    85,  3566,  3557,    88,
	    81,  3555,  3552,  3551,  3547,  3544,  3543,  3540,   268,  3539,
	  3538,  3537,  3536,  3535,   258,  3534,  3529,   271,  3524,  3520,
	  3518,  3514,  3513,   410,  3512,  3511,   264,  3509,  3508,  3507,
	  3505,  3502,   353,  3501,  3496,  3494,  3493,  3492,  3490,  3488,
	  3483,  3475,  3474,  3470,  3469,   270,  3464,  3463,  3462,  3461,
	  3460,  3456,  3455,  3453,  3446,   412,  3445,  3440,  3438,  3426,
	  3425,  3422,  3417,  3411,  3410,  3409,  3403,  3402,  3399,  3396,
	  3392,  3387,  3382,  3372,  3360,  3352,  3351,  3350,  3330,  3329,
	   259,  3328,   278,   308,    87,    86,    84,    82,   409,  3326,
	  3324,  3322,  3321,  3320,  3317,  3316,  3314,  3311,  3309,  3307,
	  3306,  3305,  3304,  3303,  3302,  3301,  3300,  3298,  3297,   256,
	   321,  3295,   261,  3294,   214,  3293,  3290,  3288,  3287,   397,
	  2832,  3284,   253,  3283,  3280,  2859,  3279,  3278,   197,  3277,
	  3276,  3275,   267,   255,  3274,  3252,   192,  3243,   408,  3239,
	  3238,  3237,   276,   283,  3213,  3206,   252,  3203,  3202,  3196,
	  3195,   251,  3191,  3181,   188,  3179,  3174,   257,  3173,  3172,
	  3171,  3167,  3166,  3165,  1687,  3163,  1666,  3162,  3159,  3155,
	     4,   157,    73,    39,    25,     9,   279,   105,   116,    72,
	  3148,  3147,  3146,  3145,  3134,  3126,  3124,  3121,  3117,  3112,
	  3105,  3101,  3100,  3099,   396,   398,  3098,   356,    66,   175,
	   108,  3096,   164,  3095,   109,   158,  3087,  3084,   249,    40,
	  3082,  3081,  3079,    19,    71,  3078,    16,    41,     5,  3076,
	  3073,  3072,   341,  3069,  3066,   248,  3063,  3062,  3060,  3059,
	  3058,  3052,   247,  3051,  5013,  4046,   331,   428,   309,  3049,
	  3047,   395,  3045,  3043,   245,  3030,  3014,  3012
};

short yyr1[] = {
	     0,     1,     1,     2,     3,     3,     3,     3,     4,     4,
	     4,     4,     4,     4,     4,     4,     4,     4,     5,     5,
	    18,    19,    19,    21,    21,     8,     8,     8,     8,     8,
	     8,     8,     8,     8,     8,     8,     8,     8,     8,     8,
	     8,     8,     8,     8,     8,     8,     8,     8,     8,     8,
	     8,     8,     8,     8,     8,     8,     8,     8,     8,     8,
	     8,     8,     8,     8,     8,     8,     8,     8,     8,     8,
	     8,     8,     8,     8,     8,     8,     8,     8,     8,     8,
	     8,     8,     8,     8,     8,     8,     8,     8,    10,    10,
	    10,    10,    11,    11,    11,    11,    11,    11,    11,    11,
	    11,    11,    11,    11,    11,    12,    23,   103,    24,   106,
	   107,   107,   108,   111,   111,    25,   114,    26,   117,   118,
	   118,   118,   119,   119,   119,    27,   120,    28,   123,   125,
	   125,   125,   125,   130,   130,   130,    29,   133,   134,   134,
	   105,   105,   105,    30,   136,   136,   135,   135,   137,    32,
	   139,   139,   140,   141,   144,   145,   145,   145,    35,   148,
	    46,   149,   122,   122,   150,   150,   151,   151,   151,   151,
	   151,   151,   151,   151,   151,   151,   151,   151,   151,   151,
	   153,   153,    38,   172,    36,   174,   175,   175,   175,   176,
	   176,    37,   180,    39,   181,   181,   182,   182,   184,   184,
	   183,   187,   187,   189,    40,   191,    41,   194,    42,   195,
	    43,   197,   198,   198,   198,   205,   205,   205,   205,   199,
	   199,   199,   196,   196,   207,   207,   200,   200,   200,   201,
	   201,   202,   202,   203,   203,    79,   210,   210,   215,   215,
	   211,   211,   213,   219,   214,   214,   220,   220,   221,   221,
	   224,   224,   224,   224,   224,   224,   228,   228,   228,   217,
	   230,   234,   234,   231,   231,   236,   236,   236,   236,   232,
	   232,   232,   232,   233,   233,   233,   244,   245,   218,   246,
	   247,   212,   212,   248,   248,   250,    70,   249,   249,    44,
	    44,   251,   252,   254,   255,   253,   253,   256,   256,   256,
	    45,   258,   258,   257,   260,   260,   264,   265,   266,   266,
	   267,   261,   261,   268,   268,   269,   269,   269,   269,   269,
	   269,   262,   262,   263,   263,   271,   259,   259,   259,   259,
	   274,   273,    47,   275,    48,   278,   185,   185,   277,   280,
	   280,   280,   281,   281,   285,   285,   286,   286,   126,   287,
	   287,   287,   287,   287,   287,   287,   287,   287,   287,   287,
	   288,   288,   127,   127,   294,   294,   294,   295,   296,   300,
	   300,   299,   302,   302,   302,   304,   305,   306,   306,   307,
	   307,   307,   307,   297,   298,   298,   303,   309,   301,   308,
	   128,   128,    49,   311,    51,    50,   313,   312,    52,   314,
	   315,   315,   317,   316,    53,   321,    53,   323,    53,    53,
	   318,   318,   319,   319,   320,   320,   320,   322,   322,    57,
	   325,    59,   327,   326,   326,   326,   326,   326,   326,   326,
	    60,    61,    65,   329,   329,   330,   330,   331,   331,   332,
	   333,   333,   335,   335,   338,   338,   336,   336,   336,   336,
	   336,   339,   339,   339,   339,   339,   337,   337,   337,   345,
	   345,   345,   345,   345,   347,   347,   347,   347,    84,   351,
	   351,   352,   352,   353,   354,   360,   359,   358,   358,   361,
	    14,   363,   365,   372,   372,   373,   373,   376,   374,   374,
	   374,   377,   375,   375,   378,   378,   379,   379,   379,   379,
	   371,   371,   381,   381,   382,   382,   364,   364,   383,   385,
	   366,   366,   386,   387,   387,   389,   388,   388,   346,   391,
	   367,   356,   356,   394,   394,   395,   395,   395,   400,   400,
	   402,   402,   403,   403,   396,   396,   398,   398,   398,   398,
	   398,   399,   399,   399,   405,   405,   355,   355,   355,   355,
	   355,   355,   355,   355,   401,   401,   192,   192,   204,   204,
	   204,   392,   392,   408,   408,   410,   410,   121,   121,   121,
	   121,   121,   208,   393,   393,   368,   368,   283,   283,   413,
	   413,   414,   284,   369,   369,   370,   370,   418,   418,   419,
	   419,   416,   416,   282,   282,   421,   421,   334,   422,   424,
	   425,   425,   430,   430,   431,   432,   432,   433,   433,   434,
	   434,   435,   435,   436,   436,   436,   436,   426,   426,   427,
	   428,   429,   429,   429,   438,   438,   439,   439,   441,   440,
	   440,   437,   437,   237,   193,   193,   193,   193,   193,   442,
	   442,   442,   442,   442,   445,   445,   445,   445,   445,   450,
	   450,   450,   450,   450,   450,   450,   450,   455,   455,   455,
	   455,   451,   451,   451,   447,   447,   459,   459,   459,   446,
	   446,   446,   462,   462,   463,   465,   465,   448,   449,   467,
	   467,   467,   467,   468,   468,   452,   454,   469,   470,   470,
	   470,   362,   362,   471,   471,   471,   471,   471,   472,   472,
	   472,   472,   473,   473,   409,   409,   476,   476,   476,   476,
	   475,   475,   477,   477,   477,   477,   477,   477,   477,   225,
	   225,   225,   225,   225,   225,   225,   225,   225,   225,   481,
	   481,   483,   484,   485,   485,   485,   486,   486,   486,   487,
	   478,   489,   489,   492,   493,   493,   495,   491,   497,   497,
	   498,   494,   494,   474,   474,   474,   474,   500,   453,   502,
	   502,   502,   502,   501,   501,   501,   480,   479,   479,   479,
	   479,   479,   479,   479,   479,   504,   504,   504,   350,   505,
	   505,   505,   505,   349,   349,   506,   509,   509,   507,   508,
	   508,   156,   156,   156,   229,   229,   348,    85,    86,   513,
	   515,   516,   516,   514,   514,   514,   514,    87,   517,   519,
	   519,   520,   521,   521,   518,   518,   518,   522,    88,   524,
	   324,   324,   324,   526,   526,   525,   525,   525,   527,    22,
	    22,   529,    33,   531,   532,   532,    58,   533,   533,   533,
	   533,   533,   533,   533,   533,   533,   533,   533,   533,   533,
	   533,   533,   533,   533,   533,   534,   535,   535,   535,   535,
	   539,   556,   540,   555,   557,   557,   542,   543,   541,   552,
	   536,   536,   558,   537,   559,   537,   560,   537,   537,   551,
	   551,   561,   561,   562,   562,   538,   538,   563,   563,   544,
	   545,   546,   547,   548,   550,   554,   553,   553,    77,   564,
	   565,   565,   566,   566,   566,   569,   568,   568,   570,   570,
	   570,   570,   570,   570,   570,   570,   570,   567,   567,   567,
	   567,   567,   573,   573,   573,   549,   549,   132,   132,    81,
	   574,    31,    34,   575,   124,   124,   576,   576,   579,   579,
	   580,   581,   581,   581,   581,   581,   581,   581,   581,   581,
	   583,   583,   582,   582,   582,   584,   584,   584,   577,   577,
	   578,   578,   578,    62,    63,   585,   586,   586,   586,   589,
	   589,   590,   590,   590,   590,   590,   590,   590,   590,   590,
	   590,   590,   590,   588,   591,   591,   240,   239,   423,   572,
	   571,   587,   587,   587,   587,   587,   592,   592,   592,   270,
	   270,   270,   270,   270,    64,   594,   594,   594,   594,   595,
	   597,   598,   598,   598,   596,    66,    66,   599,   604,   604,
	   600,   600,   606,   606,   601,   601,   607,   607,   607,   602,
	   602,   608,   608,   609,   603,   603,   603,    68,   611,   612,
	   612,    69,   613,    71,   614,   272,   272,   272,    72,   615,
	   619,   619,   619,   619,   616,   616,   620,   620,   621,   621,
	   621,   622,   617,   618,   618,   623,   623,   623,   623,   623,
	    74,   624,   624,   625,   625,   626,   626,    73,   627,    75,
	   628,    76,   629,   629,   630,   630,   630,   630,   630,    78,
	    78,   631,    78,   632,    78,    80,    80,   633,   634,   635,
	   636,   638,    82,    82,    82,    82,   637,   640,   640,   642,
	   642,   642,   644,   644,   644,   644,   644,   645,   643,   643,
	   643,   648,   648,   649,   649,   650,   650,   650,   650,   650,
	   650,   650,   650,   650,   650,   650,   650,   647,   647,   639,
	   639,   639,   639,   639,   639,   639,   639,   639,   639,   639,
	   639,   653,   653,   653,   654,   641,   641,   641,   641,   651,
	   651,   655,   655,   658,   658,   659,   660,   660,   660,   660,
	   660,   656,   656,   656,   328,   482,   482,   482,   482,   482,
	   661,   661,   216,   147,    83,   663,   663,   664,   664,   646,
	   666,   666,   666,   666,   662,   665,   112,   206,   443,   152,
	   380,   380,   458,   460,   461,   159,   162,   488,   177,   279,
	   164,   511,   146,   113,   499,   490,   223,   456,   466,   341,
	   417,   276,   242,   407,   411,   464,   158,   291,   241,   142,
	   109,   160,   227,   161,   510,   168,   169,   166,   167,   170,
	   178,   179,   243,   415,   186,   420,   444,   165,   605,   226,
	   593,   157,   110,   342,   357,   340,   154,   503,   496,   292,
	   404,   667,   235,   406,   412,   412,   412,   155,   293,   668,
	   668,   129,   238,   238,   669,   669,   670,   671,   209,   209,
	   652,   652,   171,   290,   290,   290,   143,   115,   115,   104,
	   104,   163,   530,   530,   530,   657,   657,   672,   672,   672,
	   672,   672,   672,   672,   672,   222,   673,   673,   673,   344,
	   344,   344,   674,   676,   675,   188,   188,   397,   397,   190,
	   190,   678,   173,   173,   116,   610,   610,   679,   680,   680,
	   680,   680,   680,   681,   681,   681,   681,   681,   681,   681,
	   681,   681,   682,   682,   682,   682,   682,   682,   682,   682,
	   138,   138,   138,   684,   684,   684,   684,   685,   685,   686,
	   687,   687,   688,   688,   683,   689,   689,   691,   691,   690,
	   692,   692,   693,   693,   694,   694,   694,   695,   699,   699,
	   699,   697,   698,   698,   700,   700,   702,   702,   701,   701,
	   704,   704,   696,   696,    89,   705,   706,   708,   707,   710,
	   711,   711,   712,   714,   714,   714,   713,    90,   715,   715,
	   716,   721,   721,   720,   720,   722,   722,   723,   723,   724,
	   724,   718,   726,   731,   727,   733,   735,   728,   729,   738,
	   736,   739,   737,   740,   730,   725,   725,   741,   742,   742,
	   719,   743,   744,   744,   717,   717,   745,   746,   746,   748,
	   748,   750,   749,   751,   751,   751,   751,    91,   752,   754,
	   755,   755,   755,   753,   753,    93,   757,   757,   759,   759,
	   760,   760,   760,   758,   758,   761,   761,   761,   762,   762,
	   763,   763,   289,   289,   764,   764,   765,   765,   765,   765,
	   765,   770,   310,   771,   771,   771,   771,   766,   772,   768,
	   768,   768,   768,   773,   703,   775,   774,   774,   774,   774,
	   774,   774,   774,   767,   767,   769,   769,   131,   776,   776,
	   776,   776,    92,   777,   777,   778,   778,   778,   778,    94,
	   779,   780,   780,   780,   780,   780,    95,   781,   783,   783,
	    99,   785,    96,   787,   788,   788,   788,   789,   791,   790,
	   782,   792,   792,   793,   794,   794,   794,   784,   784,    97,
	   796,   797,   798,   798,   799,   799,   799,    98,   802,   100,
	   100,   100,   805,   805,   803,   804,   786,   806,   806,   807,
	   808,   808,   808,   808,   101,   810,   811,   811,   812,   812,
	   812,   812,   812,    54,   814,   815,   815,   815,   816,   816,
	    55,   817,    13,    13,   818,   819,   819,   821,   821,   822,
	   823,   824,   824,   820,   820,   820,   826,   827,   528,   528,
	   828,   828,    15,    15,   832,   829,   834,   833,   835,   836,
	   836,   836,   836,   836,   825,   825,   838,   838,   839,   837,
	   840,   840,   841,   843,   842,   844,   844,   831,   831,   847,
	   848,   830,   830,   849,   850,   850,   852,   851,    67,   853,
	   855,   856,   856,   854,   857,   857,    56,    56,    56,   858,
	   859,   860,   861,   861,    16,    16,    16,    16,    16,    16,
	    16,    16,    16,    16,    16,    16,    16,    16,    16,    16,
	    16,    16,    16,    16,    16,    16,    16,    16,    16,    16,
	    16,    16,    16,    16,    16,    16,    16,    16,    16,    16,
	    16,    16,    16,    16,    16,   903,   904,   384,   384,   905,
	   862,   906,   907,   907,   863,   864,   864,   864,   908,   865,
	   866,   910,   911,   913,   912,   912,   912,   912,   912,   914,
	   914,   867,   915,   915,   919,   920,   920,   920,   920,   920,
	   920,   921,   921,   916,   923,   923,   924,   926,   926,   917,
	   917,   927,   927,   928,   929,   931,   930,   930,   932,   932,
	   932,   932,   932,   932,   932,   933,   933,   934,   934,   934,
	   934,   936,   936,   937,   937,   938,   938,   935,   935,   939,
	   939,   940,   941,   941,   941,   941,   943,   943,   943,   918,
	   944,   945,   945,   946,   102,   868,   869,   870,   871,   948,
	   949,   872,   950,   873,   874,   874,   951,   954,   875,   876,
	   956,   957,   958,   877,   959,   878,   961,   880,   962,   963,
	   965,   966,   966,   966,   966,   966,   966,   967,   967,   968,
	   968,   968,   968,   964,   964,   969,   969,   970,   971,   972,
	   972,   972,   973,   973,   973,   973,   973,   973,   973,   973,
	   973,   973,   974,   974,   974,   974,   974,   974,   974,   974,
	   974,   974,   974,   879,   975,   881,   976,   922,   922,   977,
	   978,   979,   979,   980,   981,   983,   983,   984,   984,   982,
	   982,   985,   985,   986,   987,   987,   987,   988,   988,   988,
	   988,   988,   988,   988,   988,   882,   991,   992,   992,   993,
	   883,   994,   995,   995,   996,   884,   884,   997,   999,   885,
	   886,  1000,  1001,  1001,  1001,  1001,  1001,  1002,   887,  1003,
	  1005,  1004,  1006,  1006,  1007,  1008,  1008,  1008,  1008,  1008,
	  1008,  1008,  1008,  1008,   888,  1009,   889,  1010,  1012,  1011,
	  1011,  1013,  1014,  1015,  1015,  1016,  1016,   890,  1017,  1018,
	  1018,   891,  1019,  1021,  1020,  1020,  1020,   892,  1023,  1026,
	  1024,  1024,  1025,  1025,  1027,   893,   893,  1028,  1029,   894,
	  1030,  1032,  1031,  1031,  1031,  1033,  1033,  1034,  1034,  1036,
	  1035,  1037,  1040,  1040,  1040,  1038,  1038,  1038,  1039,  1039,
	  1039,  1041,  1042,  1042,   902,  1043,   895,   895,  1044,  1046,
	   896,   896,  1047,  1048,  1045,  1045,  1049,   897,   897,  1050,
	  1051,   898,  1052,  1054,  1053,  1053,  1055,  1056,  1056,  1057,
	  1057,  1058,  1058,  1060,  1059,  1059,  1061,   899,  1062,   900,
	  1063,  1063,  1064,  1065,   901,  1066,  1068,  1067,  1067,  1067,
	   747,   909,   909,   925,   925,   925,   998,  1069,  1069,  1070,
	  1070,  1070,   846,   947,   947,   947,   952,  1071,  1071,  1072,
	  1072,  1073,  1022,  1022,   955,   955,   942,   942,   960,   960,
	   795,   801,  1074,   709,   709,   709,   734,   734,   809,   809,
	   809,   800,  1075,   989,   989,   989,  1076,   990,   990,   990,
	  1077,   732,   732,   732,   732,   845,   845,   845,   845,   845,
	   953,  1078,  1078,    17,    17,    17,    17,    17,    17,    17,
	    17,    17,    17,    17,    17,    17,    17,    17,  1079,  1094,
	  1097,  1097,  1095,  1095,  1098,  1098,  1099,  1099,  1099,  1099,
	  1101,  1100,  1100,  1096,  1096,  1080,  1103,  1081,  1105,  1082,
	  1106,  1108,  1107,  1107,  1111,  1111,  1083,  1113,  1084,  1114,
	  1116,  1117,  1085,  1120,  1121,  1121,  1122,  1086,  1124,  1125,
	  1125,  1126,  1087,  1127,  1088,  1129,  1130,  1131,  1131,  1132,
	  1089,  1134,  1135,  1135,  1136,  1090,  1137,  1139,  1138,  1138,
	  1140,  1140,  1091,  1142,  1092,  1143,  1145,  1093,  1146,  1128,
	  1104,  1115,  1115,  1119,  1147,  1148,  1148,  1144,  1109,  1109,
	  1118,  1118,  1118,  1118,  1118,  1110,  1112,  1141,  1149,  1149,
	  1149,  1149,  1102,  1102,  1102,  1102,  1102,  1123,  1133,  1133,
	   129,   129,   129,   129,   763,   763,   763,  1037,  1037,   973,
	   681,     9,    20,   813,   813,   813,   813,   813,     7,     6,
	  1150,  1152,  1151,  1151,  1153,  1153,  1153,  1153,  1153,  1153,
	  1153,  1155,  1161,  1162,  1164,  1163,  1163,  1168,  1170,  1170,
	  1170,  1170,  1170,  1171,  1173,  1173,  1154,  1154,  1175,  1176,
	  1181,  1181,  1182,  1178,  1178,  1185,  1185,  1186,  1186,  1179,
	  1179,  1183,  1183,  1189,  1190,  1191,  1193,  1194,  1194,  1187,
	  1195,  1195,  1192,  1192,  1156,  1197,  1197,  1200,  1200,  1198,
	  1174,  1174,  1202,  1202,  1199,  1172,  1205,  1205,  1207,  1204,
	  1206,  1169,  1177,  1188,  1166,  1165,  1209,  1209,  1210,  1210,
	  1208,  1184,  1167,  1196,  1160,  1201,  1180,  1180,  1180,  1180,
	  1180,  1180,  1180,  1203,  1157,  1157,  1211,  1212,  1212,  1216,
	  1215,  1217,  1217,  1217,  1217,  1218,  1218,  1218,  1218,  1213,
	  1214,  1158,  1158,  1219,  1220,  1220,  1159,  1221,  1221,  1222,
	  1222,  1223,  1223,  1224,  1225,  1225,   677,   756,  1226,   512,
	   457,  1227,   523,   523,  1229,  1230,  1230,   390,   343,  1228,
	  1228,  1232,  1233,  1231,  1231,  1234,  1236,  1237,  1237,  1235
};

short yyr2[] = {
	     0,     2,     0,     1,     1,     1,     1,     1,     2,     2,
	     2,     1,     1,     1,     1,     1,     2,     2,     3,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     2,     3,     1,     3,     1,
	     4,     2,     2,     1,     1,     3,     1,     2,     1,     1,
	     2,     1,     1,     1,     1,     3,     1,     3,     1,     2,
	     3,     3,     3,     0,     1,     1,     3,     1,     0,     1,
	     1,     2,     1,     3,     1,     2,     5,     5,     1,     1,
	     5,     5,     1,     1,     1,     3,     5,     5,     3,     1,
	     3,     1,     1,     0,     1,     2,     2,     3,     3,     3,
	     2,     2,     2,     1,     1,     1,     1,     1,     1,     1,
	     1,     4,     3,     2,     5,     1,     2,     5,     7,     0,
	     2,     2,     1,     2,     3,     2,     1,     1,     3,     5,
	     8,     1,     3,     2,     5,     2,     3,     1,     4,     1,
	    10,     2,     1,     3,     1,     1,     1,     1,     1,     1,
	     1,     1,     3,     0,     1,     3,     2,     2,     2,     2,
	     0,     8,     0,     4,     0,     5,     2,     4,     1,     1,
	     1,     1,     2,     2,     0,     3,     1,     3,     3,     3,
	     1,     2,     2,     1,     1,     1,     3,     4,     6,     6,
	     1,     1,     2,     1,     3,     1,     4,     1,     1,     1,
	     1,     1,     1,     0,     4,     7,     1,     1,     4,     1,
	     1,     0,     2,     1,     1,     0,     4,     2,     1,     2,
	     3,     2,     2,     1,     1,     1,     2,     1,     1,     1,
	     7,     1,     0,     1,     1,     0,     2,     1,     1,     3,
	     1,     2,     0,     1,     3,     1,     1,     1,     1,     1,
	     1,     2,     0,     3,     0,     1,     2,     2,     2,     3,
	     1,     2,     4,     1,     1,     2,     1,     3,     4,     3,
	     5,     7,     1,     3,     1,     3,     1,     2,     3,     2,
	     3,     4,     5,     4,     7,     5,     8,     6,     5,     7,
	     0,     2,     1,     2,     1,     1,     1,     4,     2,     6,
	     3,     4,     0,     2,     4,     1,     2,     1,     1,     1,
	     1,     2,     2,     2,     1,     2,     2,     1,     1,     1,
	     0,     1,     3,     1,     6,     3,     1,     2,     5,     4,
	     3,     1,     1,     3,     3,     0,     6,     0,     6,     7,
	     1,     2,     0,     1,     1,     3,     3,     1,     1,     2,
	     1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     4,     1,     2,     2,     2,     4,     3,     1,
	     1,     3,     1,     1,     1,     1,     3,     5,     3,     2,
	     2,     1,     1,     1,     1,     1,     2,     1,     2,     1,
	     1,     1,     1,     1,     2,     3,     5,     7,     2,     1,
	     2,     1,     1,     5,     5,     1,     4,     1,     3,     3,
	     3,     8,     4,     1,     2,     0,     2,     1,     0,     1,
	     1,     1,     1,     1,     1,     3,     3,     1,     3,     3,
	     0,     2,     1,     3,     1,     6,     0,     3,     2,     2,
	     0,     2,     1,     1,     3,     1,     2,     2,     5,     1,
	     4,     2,     0,     1,     3,     1,     5,     4,     0,     3,
	     0,     3,     1,     3,     1,     3,     1,     2,     2,     2,
	     2,     2,     4,     0,     1,     3,     1,     3,     3,     5,
	     3,     5,     1,     3,     1,     0,     1,     2,     2,     2,
	     0,     3,     0,     1,     3,     1,     3,     5,     3,     1,
	     5,     3,     1,     2,     0,     0,     2,     0,     3,     1,
	     3,     2,     2,     0,     2,     0,     5,     1,     1,     1,
	     1,     1,     1,     1,     3,     1,     3,     2,     4,     5,
	     1,     3,     2,     3,     4,     1,     2,     0,     2,     1,
	     1,     1,     3,     3,     1,     3,     3,     0,     3,     1,
	     1,     0,     5,     3,     1,     3,     1,     2,     2,     1,
	     1,     0,     1,     1,     1,     3,     3,     2,     3,     1,
	     1,     1,     1,     1,     4,     4,     5,     3,     4,     1,
	     1,     1,     1,     1,     1,     1,     2,     1,     1,     1,
	     1,     2,     2,     0,     6,     7,     0,     1,     1,     5,
	     3,     3,     2,     1,     3,     1,     3,     4,     2,     2,
	     5,     3,     6,     1,     1,     3,     5,     3,     0,     1,
	     1,     3,     1,     1,     1,     1,     1,     1,     3,     1,
	     2,     2,     1,     1,     3,     1,     3,     2,     2,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     3,     3,     1,     0,     3,     5,     0,     5,     2,     1,
	     3,     1,     2,     2,     1,     2,     4,     2,     1,     2,
	     4,     0,     2,     3,     4,     5,     4,     2,     2,     1,
	     1,     1,     1,     1,     1,     0,     7,     2,     3,     5,
	     7,     6,     8,     5,     5,     2,     1,     1,     2,     1,
	     1,     1,     1,     1,     1,     3,     3,     1,     3,     1,
	     1,     1,     2,     2,     2,     2,     5,     2,     2,     3,
	     0,     0,     1,     3,     3,     3,     1,     2,     3,     2,
	     0,     0,     2,     1,     1,     1,     1,     3,     2,     3,
	     1,     1,     1,     0,     2,     0,     2,     3,     1,     1,
	     3,     1,     2,     1,     1,     0,     1,     4,     5,     2,
	     1,     1,     1,     2,     2,     2,     2,     2,     2,     2,
	     3,     3,     2,     3,     2,     1,     0,     1,     1,     1,
	     1,     0,     4,     2,     0,     2,     1,     2,     1,     1,
	     1,     1,     0,     3,     0,     3,     0,     3,     1,     1,
	     1,     1,     3,     1,     1,     1,     1,     1,     3,     1,
	     1,     1,     1,     1,     1,     1,     1,     3,     7,     1,
	     3,     0,     1,     1,     2,     1,     1,     3,     1,     1,
	     1,     1,     1,     4,     1,     1,     4,     3,     3,     2,
	     2,     0,     1,     1,     1,     1,     0,     1,     1,     2,
	     1,     1,     8,     2,     1,     1,     1,     0,     1,     3,
	     4,     1,     1,     1,     2,     2,     2,     2,     2,     3,
	     4,     6,     0,     5,     2,     1,     2,     2,     1,     1,
	     1,     1,     1,     1,     6,     1,     1,     2,     1,     1,
	     3,     1,     1,     4,     1,     1,     1,     1,     5,     1,
	     2,     5,     1,     1,     0,     1,     1,     1,     1,     1,
	     1,     2,     2,     3,     3,     0,     1,     1,     1,     2,
	     2,     1,     2,     1,     1,     1,     1,     2,     2,     1,
	     1,     1,     1,     1,     1,     8,     3,     2,     1,     0,
	     0,     2,     1,     3,     2,     1,     0,     2,     2,     2,
	     0,     1,     3,     2,     2,     2,     0,     8,     4,     1,
	     1,     2,     1,     4,     1,     0,     1,     1,     6,     2,
	     0,     1,     1,     1,     0,     3,     1,     3,     1,     3,
	     1,     1,     2,     0,     2,     1,     3,     1,     3,     3,
	     8,     3,     2,     0,     3,     2,     4,     2,     1,     4,
	     1,     2,     2,     1,     0,     1,     1,     1,     1,     1,
	     2,     0,     4,     0,     5,     4,     2,     1,     1,     3,
	     1,     0,     4,     3,     4,     3,     1,     2,     2,     1,
	     3,     0,     5,     5,     1,     1,     1,     1,     2,     2,
	     0,     1,     3,     3,     1,     0,     2,     1,     1,     2,
	     1,     1,     1,     1,     1,     1,     3,     1,     1,     0,
	     1,     3,     2,     1,     1,     1,     5,     1,     1,     2,
	     2,     2,     3,     4,     2,     1,     1,     1,     1,     0,
	     1,     2,     2,     1,     3,     3,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     2,     1,     2,     1,     3,     1,     1,     2,
	     2,     2,     2,     1,     1,     1,     1,     1,     1,     1,
	     0,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     2,     2,     1,     1,     0,
	     1,     1,     1,     3,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     3,     1,     1,     1,     1,     1,     1,     1,     1,     3,
	     3,     3,     3,     3,     1,     1,     1,     3,     1,     0,
	     1,     2,     2,     1,     1,     1,     0,     0,     1,     0,
	     1,     1,     1,     0,     2,     1,     3,     1,     3,     1,
	     2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     3,     3,     1,     1,     1,     1,     1,     1,
	     2,     4,     7,     1,     1,     1,     1,     1,     3,     3,
	     1,     3,     0,     1,     5,     1,     1,     3,     5,     3,
	     0,     2,     1,     3,     2,     2,     2,     3,     0,     1,
	     1,     2,     6,     5,     1,     3,     1,     1,     1,     3,
	     1,     3,     0,     4,     3,     1,     1,     1,     3,     1,
	     1,     3,     3,     1,     1,     1,     1,     3,     2,     3,
	     6,     0,     1,     0,     1,     0,     1,     0,     1,     0,
	     1,     1,     2,     1,     2,     1,     0,     4,     2,     0,
	     4,     0,     4,     0,     4,     2,     0,     2,     1,     3,
	     1,     1,     1,     1,     2,     0,     1,     1,     1,     1,
	     3,     1,     3,     1,     1,     1,     1,     4,     2,     1,
	     2,     2,     0,     1,     0,     5,     2,     4,     1,     1,
	     1,     1,     1,     1,     3,     3,     6,     8,     1,     1,
	     1,     2,     0,     1,     1,     2,     1,     1,     1,     1,
	     1,     0,     3,     2,     5,     1,     3,     2,     2,     1,
	     2,     2,     1,     2,     1,     2,     1,     1,     1,     1,
	     1,     1,     1,     2,     2,     1,     2,     2,     1,     1,
	     1,     1,     3,     2,     0,     1,     3,     1,     3,     2,
	     1,     2,     1,     1,     1,     0,     3,     1,     0,     4,
	     2,     1,     2,     1,     0,     2,     1,     3,     2,     1,
	     3,     1,     3,     4,     1,     1,     1,     1,     1,     2,
	     1,     3,     1,     3,     1,     1,     1,     2,     1,     2,
	     2,     2,     1,     1,     1,     1,     3,     1,     3,     3,
	     1,     1,     2,     1,     3,     1,     1,     2,     1,     3,
	     2,     2,     1,     4,     1,     2,     3,     4,     0,     2,
	     3,     1,     2,     3,     1,     3,     4,     1,     1,     0,
	     3,     3,     2,     1,     2,     2,     2,     3,     1,     3,
	     2,     2,     4,     5,     2,     2,     2,     2,     1,     0,
	     2,     7,     3,     3,     0,     1,     1,     3,     3,     1,
	     2,     2,     1,     0,     4,     1,     1,     0,     3,     2,
	     2,     0,     4,     2,     1,     3,     1,     2,     2,     3,
	     1,     0,     2,     2,     1,     1,     2,     1,     3,     1,
	     1,     2,     1,     1,     2,     2,     2,     2,     2,     1,
	     2,     2,     2,     1,     2,     2,     2,     2,     2,     2,
	     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
	     2,     1,     2,     2,     2,     2,     2,     2,     2,     2,
	     2,     1,     1,     2,     2,     3,     2,     0,     2,     2,
	     2,     1,     1,     1,     1,     2,     2,     2,     2,     5,
	     6,     1,     1,     2,     1,     1,     1,     1,     0,     1,
	     1,     4,     6,     3,     1,     1,     1,     1,     1,     1,
	     0,     1,     0,     1,     4,     0,     1,     1,     0,     1,
	     0,     1,     2,     4,     1,     1,     1,     3,     3,     3,
	     2,     2,     4,     2,     1,     0,     1,     1,     1,     1,
	     1,     1,     1,     2,     3,     1,     1,     0,     3,     1,
	     3,     3,     1,     1,     1,     1,     1,     1,     1,     2,
	     0,     3,     0,     1,     1,     1,     1,     1,     3,     2,
	     1,     2,     1,     1,     2,     2,     2,     1,     1,     2,
	     1,     5,     3,     4,     1,     2,     1,     4,     3,     1,
	     1,     1,     1,     1,     1,     1,     1,     0,     2,     1,
	     1,     1,     1,     1,     1,     1,     3,     2,     3,     1,
	     3,     4,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     2,     1,     3,     1,     1,     0,     2,
	     1,     1,     3,     2,     3,     1,     1,     1,     1,     3,
	     0,     1,     3,     3,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     2,     1,     5,     0,     1,
	     8,     1,     1,     0,     2,     2,     2,     2,     1,     1,
	     2,     1,     0,     1,     2,     3,     1,     1,     4,     3,
	     1,     1,     1,     3,     3,     1,     1,     1,     1,     1,
	     1,     1,     2,     1,     2,     1,     2,     2,     1,     1,
	     2,     4,     1,     1,     3,     3,     1,     2,     1,     0,
	     2,     2,     4,     1,     3,     2,     0,     3,     4,     1,
	     0,     2,     0,     2,     1,     2,     2,     4,     1,     2,
	     4,     1,     2,     3,     0,     1,     1,     1,     3,     0,
	     6,     1,     1,     1,     1,     0,     3,     5,     0,     2,
	     2,     2,     2,     0,     3,     1,     3,     3,     4,     1,
	     3,     3,     3,     1,     0,     4,     1,     2,     2,     4,
	     1,     2,     3,     1,     2,     3,     1,     1,     1,     1,
	     1,     0,     2,     1,     0,     2,     1,     3,     1,     2,
	     3,     3,     3,     1,     2,     4,     1,     3,     2,     0,
	     1,     1,     3,     1,     2,     0,     3,     1,     3,     3,
	     6,     3,     1,     1,     2,     0,     3,     1,     3,     3,
	     6,     2,     1,     0,     1,     0,     1,     1,     1,     0,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
	     2,     1,     1,     1,     2,     2,     1,     1,     2,     2,
	     1,     1,     1,     1,     1,     2,     1,     1,     1,     1,
	     3,     0,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     3,     2,
	     1,     1,     2,     3,     1,     3,     3,     3,     3,     1,
	     1,     4,     1,     0,     2,     2,     1,     4,     1,     2,
	     3,     1,     1,     3,     1,     3,     5,     1,     4,     1,
	     1,     5,     4,     1,     1,     3,     3,     5,     1,     1,
	     3,     3,     3,     1,     2,     2,     3,     1,     3,     3,
	     4,     1,     1,     3,     3,     2,     3,     1,     1,     3,
	     1,     3,     5,     1,     4,     1,     5,     3,     1,     5,
	     1,     1,     1,     1,     1,     3,     0,     1,     0,     1,
	     1,     1,     1,     1,     1,     3,     3,     3,     1,     1,
	     1,     1,     2,     1,     1,     1,     1,     2,     1,     1,
	     1,     1,     1,     1,     1,     1,     2,     1,     1,     1,
	     1,     1,     0,     2,     1,     2,     1,     1,     1,     3,
	     3,     4,     0,     2,     1,     1,     1,     1,     1,     1,
	     1,     7,     3,     0,     0,     1,     3,     1,     1,     1,
	     1,     1,     2,     8,     0,     1,     1,     1,     3,     4,
	     0,     2,     0,     0,     2,     2,     2,     0,     1,     1,
	     3,     1,     3,     2,     2,     3,     3,     0,     1,     1,
	     0,     1,     0,     1,     2,     2,     3,     1,     2,     3,
	     0,     1,     1,     3,     2,     6,     1,     3,     6,     0,
	     0,     0,     1,     1,     1,     1,     1,     2,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     2,     1,     2,     1,     2,     4,     1,     1,     3,     1,
	     3,     1,     1,     1,     1,     1,     2,     1,     2,     1,
	     1,     2,     1,     1,     1,     1,     5,     2,     4,     1,
	     1,     1,     1,     1,     1,     0,     1,     2,     1,     2,
	     2,     1,     1,     2,     2,     1,     0,     2,     2,     1,
	     2,     2,     0,     1,     3,     2,     1,     0,     1,     1
};

short yychk[] = {
	 -4000,    -1,    -2,    -3,    -4,    -5,    -6,    -7,    -8,   -10,
	   -11,   -12,   -13,   -14,   -15,   -16,   -17,   256,   -18,   494,
	 -1150,   492,   -22,   -23,   -24,   -25,   -26,   -27,   -28,   -29,
	   -30,   -31,   -32,   -33,   -34,   -35,   -36,   -37,   -38,   -39,
	   -40,   -41,   -42,   -43,   -44,   -45,   -46,   -47,   -48,   -49,
	   -50,   -51,   -52,   -53,   -54,   -55,   -56,   -57,   -58,   -59,
	   -60,   -61,   -62,   -63,   -64,   -65,   -66,   -67,   -68,   -69,
	   -70,   -71,   -72,   -73,   -74,   -75,   -76,   -77,   -78,   -79,
	   -80,   -81,   -82,   -83,   -84,   -85,   -86,   -87,   -88,   -89,
	   -90,   -91,   -92,   -93,   -94,   -95,   -96,   -97,   -98,   -99,
	  -100,  -101,  -102,  -818,  -363,  -829,  -862,  -863,  -864,  -865,
	  -866,  -867,  -868,  -869,  -870,  -871,  -872,  -873,  -874,  -875,
	  -876,  -877,  -878,  -879,  -880,  -881,  -882,  -883,  -884,  -885,
	  -886,  -887,  -888,  -889,  -890,  -891,  -892,  -893,  -894,  -895,
	  -896,  -897,  -898,  -899,  -900,  -901,  -902, -1079, -1080, -1081,
	 -1082, -1083, -1084, -1085, -1086, -1087, -1088, -1089, -1090, -1091,
	 -1092, -1093,   493,   523,   257,  -529,  -103,  -106,  -114,  -117,
	  -120,  -123,  -133,  -136,   526,  -139,  -531,  -575,  -148,  -174,
	  -180,  -172,  -181,  -191,  -194,  -195,  -197,  -251,  -257,  -149,
	  -275,  -277,  -311,  -313,  -312,  -314,  -318,  -814,  -817,  -858,
	  -859,  -860,  -325,  -533,  -327,   296,   409,   549,  -585,  -594,
	  -329,  -599,  -853,  -611,  -613,  -249,  -614,  -615,  -627,  -624,
	  -628,  -629,  -564,   270,  -210,  -633,  -574,  -637,  -662,  -351,
	   387,  -513,  -517,  -524,  -705,  -715,  -752,  -757,  -779,  -781,
	  -787,  -796,  -802,  -785,  -803,  -804,  -810,   435,   289,  -365,
	  -833,  -906,   434,   426,  -908,  -910,  -913,  -915,   433,   432,
	   436,  -948,  -950,   412,  -951,   422,  -956,  -959,  -961,  -975,
	  -962,  -976,  -991,  -994,  -997,   423, -1000, -1003, -1009, -1010,
	 -1017, -1019, -1023, -1028, -1030, -1044, -1047, -1050, -1052, -1062,
	 -1063, -1066, -1043, -1094, -1103, -1105, -1106, -1113, -1114, -1120,
	 -1124, -1127, -1129, -1134, -1137, -1142, -1143, -1146,   518,   516,
	   517,   574,   522,   519,   520,   577,   521,  -140,   269,   258,
	   530,   531,   528,   309,   534,   529,   535,   532,   572,  -278,
	   536,   576,   392,   310,   295,   292,   538,   563,   564,   562,
	   539,  -534,  -539,  -540,  -541,  -542,  -543,  -544,  -545,  -546,
	  -547,  -548,  -550,  -552,  -554,   548,   280,  -595,  -596,  -597,
	   319,   260,  -855,   293,   261,   399,   566,   262,   568,   263,
	   264,   567,   271,   533,   265,   266,   267,   583,   338,   395,
	   398,   386,  -718,   393,   552,   553,   403,   404,   570,   405,
	   406,   402,  -372,   414,   537,  -919,  -949,   413,  -954,   441,
	   427,   443,   442,  -965,   424,   440,   428,  -999,   431, -1005,
	   425, -1012,   430, -1021, -1026, -1029, -1032, -1046, -1048, -1051,
	 -1054,   454, -1064, -1068,   455, -1097,   557,   514, -1108,   551,
	   515,   511,   513,   507,   575,   512, -1139,   555,   508,   556,
	   527,   311,   541,  -555,   540,   542,   543,   565,   545,   547,
	   578,   546,   544,   573,   259,   561,  -598,   388,   332,   416,
	   415,   421,   438,   420,   437,   429,   444,   445,   447,   450,
	   446,   449,   448,   453, -1065,   451,   510,   509,   550,   554,
	   558,   559,   560,   452,    -9,   497,    -9,    -9,    -9,    -9,
	   -19,   -21,   604,   486,   485, -1151,    -9,  -819,  -820,   290,
	  -673,  -826,  -827,  -209,  -215,  -457,  -670,  -671,  -216, -1224,
	   485,   499,   506,   505,    -9,  -830,  -849,   410,    -9,    -9,
	    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,
	    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,
	    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,
	    -9,    -9,    -9,    -9,   400,  -292,   463,  -104,  -115,  -129,
	  -143,  -669,   594,   591,   593,   598,   486,  -670,  -671,  -104,
	  -115,  -118,  -119,  -116,   485,   431,   401,  -155,   468,  -121,
	  -129,  -147,  -216,  -124,  -121,  -315,  -129,  -317,   348,  -134,
	  -115,  -134,  -532,   485,  -124,  -104,  -129,  -121,  -104,  -182,
	  -183,  -184,  -185,  -109,  -124,  -129,   502,  -186,   462,  -115,
	  -121,  -198,  -205,  -206,   332,   338,   310,   319,   456,  -252,
	  -254,   352,  -258,  -115,  -121,  -121,  -115,  -115,  -121,  -315,
	  -129,  -319,   458,  -673,  -673,  -173,  -116,  -861,  -143,  -512,
	 -1225, -1224,  -326,   456,   411,   334,   342,   399,  -328,  -129,
	   361,  -326,  -586,  -206,  -589,  -590,  -240,  -239,  -423,  -572,
	  -571,  -237,  -129,  -301,   310,   319,   332,   289,   262,   338,
	   357,  -241,   460,  -185,  -854,   458,   485,  -238,  -129,  -209,
	  -124,  -121,  -124,  -129,  -129,  -124,  -565,   280,   485,   463,
	  -211,  -217,  -218,  -230,  -246,   485,   466,  -129,  -530,   483,
	   485,   499,  -171,  -407,  -317,   485,   315,  -663,  -664,  -646,
	  -647,  -665,   580,   581,   582,  -352,  -353,  -354,  -355,  -129,
	  -147,  -315,  -324,  -209,   486,  -457,  -514,   391,   256,  -518,
	  -366,  -522,   256,  -386,   281,   460,  -525,  -527,   281,  -706,
	  -708,  -709,  -734,   485,   499,  -801,  -457,  -795,   486,  -716,
	  -720,  -727,  -733,   301,  -753,   460,   334,  -777,   501,  -780,
	   348,   456,  -734,   390,  -782,   502,  -788,  -789,  -791,   468,
	  -797,   502,  -782,  -786,   502,  -786,  -805,   485,  -795,  -457,
	  -811,   485,   276,   399,  -366,  -834,  -835,  -673,  -907,  -512,
	   486,   476,   472,   456,  -909,  -709,  -911,  -709,  -709,  -916,
	  -923,  -924,   417,    -9,  -909,  -952,  -953,   502,   281,  -957,
	   502,  -709,  -797,  -797,  -963,   502,  -734,  -992,   502,  -995,
	   485,  -998,  -953,   502, -1001,   475,   472,   480, -1002,   485,
	  -963,  -732,  -800,   484,   488,  -512,  -809,   483,   611,   610,
	 -1011, -1013, -1014,  -929,   419, -1018,   472, -1020,   502, -1024,
	   479,  -952,  -953, -1031,   502,  -925,  -953,  -998,   502,  -998,
	  -953,  -998,  -953, -1053, -1055, -1057,   463,   477,   478,    -9,
	  -903,  -904,   298, -1067,   502,  -709, -1095,   502, -1104, -1115,
	 -1147,  -809,  -756, -1225, -1104, -1107, -1110,   502,   281,   485,
	 -1115,   502, -1104, -1104, -1130,   502,   502, -1138, -1110,   502,
	   485, -1104, -1104,   485,  -141,  -144,   334,   480,   334,   485,
	   315,   337,   317,   320,   328,   399,   353,   334,   342,  -185,
	   346,  -324,  -759,  -760,   485,   499,  -216,   310,   319,   338,
	   332,   334,   290,  -535,  -536,   334,   342,   317,   320,   328,
	  -238,  -121,  -104,  -104,  -121,  -129,  -115,  -115,  -115,  -259,
	  -273,  -274,  -186,   462,   571,  -121,  -410,  -121,  -238,  -238,
	  -604,   334,  -673,   463,  -619,   337,   317,   334,   399,   342,
	  -630,   334,   342,   317,   399,  -209,  -215,  -519,  -520,  -324,
	  -719,   463,  -709,  -373,  -376,   282,  -709,   429,  -709,  -955,
	  -709,  -966,   486,   485,   472,   480,   334,   411,  -955,  -966,
	    -9,  -709,  -709,  -709,  -709,  -709,  -709,  -709,  -709,  -947,
	  -953,  -952,   502,  -709,  -709, -1104, -1104, -1104,   399,   315,
	  -121,  -709,   -20, -1152, -1153,   299, -1154, -1155, -1156, -1157,
	 -1158, -1159,   256, -1175, -1176, -1160, -1196, -1211, -1219,   494,
	 -1221, -1177, -1180,   601,   600,   597,   493,   392,   591,   590,
	   594,   595,   599,   593,   592,    -9,  -821,   486,  -512,   281,
	  -528,  -828,  -512,  -346,   363,  -523, -1229, -1230,   505,   504,
	 -1227, -1228, -1232, -1233,   485,  -364,  -383,   298,    -9,  -832,
	   460,   502,   411,   497,  -530,  -105,  -177,  -135,  -116,   501,
	  -112,  -113,   303,   311,  -107,  -108,   311,  -111,  -112,  -113,
	  -116,  -116,  -610,  -679,  -680,  -681,  -222,  -683,   485,   334,
	   270,   280,   338,   315,   360,   359,   317,   601,  -457,  -605,
	   385,  -122,  -150,  -151,  -152,  -154,  -157,  -158,  -160,  -161,
	  -162,  -164,  -165,  -166,  -167,  -168,  -169,  -170,   301,   375,
	   374,   366,   367,   368,   364,   365,   327,   371,   372,   369,
	   370,   373,  -146,   504,  -146,  -125,  -112,  -113,  -146,  -105,
	  -105,  -109,  -116,  -142,   461,  -173,  -177,  -186,  -183,  -279,
	   505,  -192,  -121,  -116,  -196,  -109,  -199,  -177,   462,   274,
	   463,  -253,  -256,   -48,   -51,   -63,   309,  -255,  -129,  -259,
	  -122,  -276,   469,  -173,  -173,  -196,  -280,  -109,  -196,  -320,
	  -209,  -215,   348,  -815,   460,   281,  -815,  -173, -1227,  -587,
	  -186,  -274,  -588,   485,  -177,  -109,  -591,   279,  -163,   483,
	  -591,  -124,  -600,  -116,  -605,  -857,  -512,   486,   609,  -177,
	  -250,  -272,  -143,  -457,  -616,  -109,  -186,  -292,  -566,  -568,
	  -206,  -570,  -129,  -423,  -240,  -239,  -237,  -571,  -572,  -301,
	   485,   463,  -631,  -212,   469,  -231,  -236,  -237,  -239,  -240,
	  -143,  -634,   465,  -638,  -129,  -317,  -640,  -254,  -642,  -644,
	  -112,  -113,  -646,  -647,  -171,  -177,  -666,   382,   384,   383,
	  -356,  -242,   458,  -401,  -146,  -152,  -146,  -401,   469,  -387,
	  -388,  -390,  -346, -1225,   294,  -528,   294,  -707,  -710,   502,
	  -717,  -745,   468,  -721,  -726,  -731,   348,  -734,  -754,  -756,
	   502,   485,  -778,   485,   499,  -732,  -783,   468,  -792,  -793,
	  -756,  -790,  -732,   485,  -798,  -799,  -512,  -800,  -801,  -806,
	  -807,  -794,  -795,   485,   348,  -812,   389,   485,   396,   386,
	   401,   485,  -367,  -356,  -836,   502,   281,   501,  -912,   338,
	   456,  -914,  -457,   486,   485,  -709,  -917,  -927,  -928,  -929,
	  -925,  -903, -1071, -1072, -1073,  -756, -1078,   294,  -958,   485,
	  -960,   501,  -964,  -969,   611,  -970,  -971,  -756,  -922,  -977,
	  -978,   468,  -993,   485,   502, -1069, -1070,  -709,   439,  -709,
	  -709, -1004, -1006, -1007,  -972,  -973,   485,   486,   430,   419,
	   472,   480,   474,   334,   350,   313,   596,   483,   483, -1013,
	 -1015, -1016,   473,  -937,   485,  -709,  -909,   503, -1025, -1027,
	   459,  -952,   503, -1033,  -747, -1034,  -457, -1035,  -709, -1045,
	 -1049,   468, -1045,   503, -1045, -1045, -1056,  -732,   299, -1058,
	 -1060,   479,  -903,  -384,    -9,  -909,   503,  -709, -1096,   460,
	   503, -1098, -1099,   485,   486, -1101, -1100,  -457,   272, -1102,
	 -1075, -1076, -1077, -1074,  -809,  -989,  -990,  -795,   611,   610,
	   484,   488, -1226, -1227,   460, -1111, -1112, -1073, -1078,   502,
	 -1116, -1118,  -457,  -809,  -989,  -990,  -795, -1121, -1122, -1073,
	   502, -1128,   502, -1131, -1132, -1133,   485,   486, -1135, -1136,
	 -1123, -1133, -1140, -1141,  -709,   502, -1144, -1149,  -800,   484,
	   488,  -457, -1128,  -124,  -145,  -129,  -147,   317,  -280,   347,
	  -515,   504,  -410,  -553,  -315,  -186,  -549,  -132,   344,   345,
	  -551,  -206,  -561,  -562,  -163,  -115,  -121,  -121,  -121,   390,
	   334,  -177,  -856,  -815,   269,   317,   504,  -324,   458,  -526,
	   469,  -719,  -374,  -206,  -377,   273,  -163,  -920,   486,   485,
	   384,   338,  -457,    -9,  -967,  -967,  -709,  -709,  -709,  -709,
	  -709,  -709,  -709,  -709,    -9,    -9,   503,  -709, -1109, -1118,
	 -1109,  -556,  -709,   392, -1161, -1166,   613, -1197, -1198, -1166,
	 -1212, -1213, -1215,   502, -1216,   485, -1220,   485,   486,   334,
	 -1178, -1185, -1184,   612, -1181, -1184, -1222, -1223,   485,   499,
	   598,   592,    -9,  -822,   294,   501,  -344,  -674,  -675,  -676,
	   297,   505,  -344,   502,   485, -1231, -1234, -1236,   485,  -209,
	   485, -1231,  -384,    -9,  -831,  -847,   298,    -9,  -756,  -850,
	  -851,  -390,  -115,  -116,  -137,   485,  -137,  -109,   456,   485,
	  -177,  -223,  -171,   609,  -223,  -151,  -153,  -171,  -155,  -155,
	  -159,   457,  -156,  -163,  -226,  -227,   610,   611,  -156,  -163,
	  -129,  -129,  -126,  -127,  -129,  -294,  -131,  -295,  -296,  -297,
	  -309,  -298,  -299,  -308,   354,   337,   356,  -303,   359,   355,
	  -129,  -131,  -129,  -576,  -579,  -580,  -129,  -175,  -129,  -109,
	  -121,  -110,   503,  -121,  -142,  -129,  -173,  -207,  -208,  -129,
	  -192,  -205,  -256,  -260,  -264,  -265,   470,  -124,  -152,  -316,
	   569,  -281,  -126,  -129,  -152,   464,  -209,   504,   504,  -816,
	   281,  -523,   294,   471,  -292,  -410,  -592,   390,   334,   399,
	   485,  -410,  -590,  -238,  -109,  -109,  -330,   358,  -196,  -292,
	  -606,  -129,  -612,   483,  -457,  -129,  -173,  -173,  -617,  -152,
	  -620,  -621,  -622,  -129,  -126,  -129,  -129,  -567,  -186,  -274,
	  -177,  -569,   485,  -109,  -109,   469,  -632,  -530,  -213,  -219,
	   289,  -248,   584,   585,  -232,  -177,  -241,  -242,  -243,  -186,
	   274,  -109,  -247,   295,  -635,  -636,   485,  -639,  -186,  -652,
	  -653,  -654,  -655,  -171,   486,   483,  -317,  -671,  -406,  -155,
	   464,  -407,  -641,  -129,  -482,  -143,  -343,  -328,   376,   377,
	   378,  -661, -1224,   379,  -216,  -643,  -406,  -155,  -177,  -645,
	   485,  -645,  -664,   384,   380,   381,  -357,   267,  -394,  -395,
	  -396,  -109,  -355,  -129,  -129,  -129,  -129,  -129,  -222,  -424,
	  -425,  -430,  -431,  -109,  -432,   332,   295,  -389,   501,  -344,
	  -344, -1228,  -523,  -523,  -711,  -712,  -714,  -709,   334,   474,
	  -746,  -747,  -748,  -457,  -749,  -743,  -744,   485,   486,  -722,
	  -728,   397,  -732,  -755,   468,  -758,  -761,  -762,   485,   499,
	   501,   485,   503,   501,  -344,   609,   503,   501,   503,   501,
	   609,   463,  -813,   505,   483,   485,   486,  -813,  -368,  -412,
	   336,  -204,  -406,   503,  -673,  -838,  -839,  -825,   294,  -709,
	   460,  -912,  -918,  -944,  -928,  -930,  -932,  -933,   473,   481,
	   482,   474,  -937,   485,    -9,   503,   501,  -942,   461,   609,
	  -344,  -523,   501,  -942,  -709,   503,   501,  -972,  -344,  -979,
	  -980,  -981,  -983,   485,   486,  -942,  -734,   503,   501,  -942,
	   502,  -709,   503,   501,  -942,   502,  -931,   501,    -9,  -934,
	   486,   485,   456,  -457,  -935,  -938,   502,   483,  -457,   503,
	  -998,   503,   501,  -942,   502, -1059, -1061,   459,  -952,  -905,
	    -2,   299,   503, -1073,   503,   501,   609,   609,   609,  -344,
	   502,   484,   488,   484,   488,  -523,   503,   501,  -942,  -523,
	 -1112, -1117,   502,   503,   501,  -942, -1125, -1126,   485,   485,
	   503,   501,  -942,   503,   501,  -942, -1148,   502,   503,   501,
	  -942, -1141, -1145,   502,  -142,  -142,  -146,  -146,  -173,   334,
	  -759,   485,   499,  -173,  -177,  -537,   334,   399,   485,  -121,
	  -177,   485,  -121,  -816,   468,  -209,   485,  -375,   612,  -378,
	  -379,  -129,  -362,  -472,  -109,  -473,  -227,  -226,   485,  -474,
	  -475,  -500,  -502,  -453,  -121,  -477,   308,   333,   302,   325,
	   324,   300,  -225,  -478,  -479,  -480,  -349,  -229,  -348,  -216,
	  -163,  -143,   487,   489,   484,   488,  -481,   275,  -482,   358,
	  -488,  -350,  -503,   307,  -506,  -507,  -457,  -346,   467,   490,
	  -291,   306,  -505,   330,  -508,   321,   323,   329,  -510,  -511,
	   341,   340,  -921,   502,  -968,   486,   485,   483,  -457, -1022,
	  -732, -1022, -1022,  -920, -1022, -1022, -1022,  -557,  -177,   400,
	 -1162, -1167,   485, -1188,   501, -1199, -1167, -1201,   485, -1188,
	 -1212,   609,   502, -1179, -1189, -1191, -1167, -1186, -1188,   483,
	 -1187,   502, -1182,   483,   504,  -823,  -824,   281,  -386,  -828,
	  -674,  -677, -1226,   485, -1235,   504, -1237,   502,  -385,   299,
	  -384,    -9,  -831,   503,  -852,   501,  -344,  -109,  -109,  -104,
	  -679,  -682,  -188,  -109,  -179,   456,   411,   334,   389,   360,
	   337,   275,  -109,  -109,  -156,  -156,  -156,  -163,  -163,  -146,
	  -146,  -128,  -310,   468,  -287,  -171,  -291,   321,  -294,  -129,
	  -109,  -300,  -301,  -193,  -442,  -178,  -109,  -445,  -446,  -447,
	  -448,  -449,   276,  -362,  -452,  -466,   313,  -129,   360,  -109,
	   360,  -130,   345,   344,  -132,  -110,  -177,  -223,  -173,  -176,
	  -109,  -178,  -183,  -109,  -193,  -110,  -177,  -196,  -261,  -235,
	   343,  -266,  -267,   485,  -282,  -421,  -334,  -109,  -422,  -423,
	  -116,   349,  -110,  -177,  -282,  -321,   390,  -323,  -209,  -209,
	   485,  -523,   294,  -270,  -129,  -328,  -593,  -407,  -104,   362,
	  -410,   485,  -110,  -238,  -238,   339,  -331,  -332,  -334,   339,
	  -601,  -129,   263,  -146,   501,  -618,  -242,   485,  -110,  -177,
	  -142,  -625,  -109,  -242,  -573,   390,  -410,   485,   399,   334,
	  -238,  -570,  -238,  -238,  -530,  -214,  -109,  -121,   399,  -121,
	  -236,  -238,  -143,  -163,  -121,  -651,  -655,  -656,  -171,  -163,
	  -112,  -113,  -129,   485,  -171,  -651,  -658,  -659,  -171,  -658,
	 -1228,  -648,  -649,  -171,  -648,  -644,  -109,  -109,  -358,  -361,
	  -208,  -177,  -397,   326,  -282,  -395,  -109,  -401,  -401,  -426,
	  -412,  -165,  -367,  -282,  -433,  -376,   332,  -388,   503,  -713,
	   501,   609,  -750,   501,   609,  -723,  -729,  -736,   407,   457,
	   483,  -457,   503,   501,  -763,   485,   594,   591,   593,   485,
	   499,   609,  -793,   609,  -799,  -807,  -808,  -809,  -795,  -457,
	   485,  -813,   483,   485,  -283,  -165,  -282,   456,   273,  -392,
	  -407,  -193,  -222,  -457,  -837,   609,   503,   501,  -523,  -523,
	   460,  -945,  -946,   418,  -931,   501,   472,   480,  -934,  -934,
	  -934,  -936,   485,   486,  -926,  -903, -1072,  -709,   422,  -958,
	  -709,  -970,  -942,   501,  -982,   502,   609,  -709,   501, -1070,
	  -845,  -846,  -512, -1075, -1076, -1077, -1074,   275,  -709, -1007,
	 -1008,   486,   483,   484,   488,   338,   384,   485,  -457,   462,
	   503,  -974,   486,   485,   483,   481,   482,   426,   473,   440,
	   480,   472,  -457,  -903, -1016,  -935,  -935,  -939,  -940,  -941,
	   485,   486,   430,   419, -1035, -1036, -1004,  -998,    -9, -1099,
	 -1100,  -457, -1100, -1100, -1073, -1112,  -709,   503, -1119, -1147,
	 -1122, -1123,   503,   501,  -942,  -942, -1132, -1102, -1136, -1102,
	 -1104, -1141, -1102,   503,   485,  -143,  -143,  -129,  -129,  -315,
	  -538,  -563,  -206,  -163,  -558,  -559,  -560,  -562,  -177,  -223,
	  -146,  -380,  -471,   301,  -340,  -342,  -226,  -227,  -341,   612,
	   613,   614,  -362,  -362,  -362,  -340,  -501,   273,   456,  -109,
	  -501,   502,   502,  -489,  -491,  -362,  -497,  -498,  -235,  -110,
	  -362,  -504,   485,  -143,  -109,  -109,  -344,  -344,   502,   486,
	  -109,  -276,  -922,  -410,   497, -1163, -1168, -1170, -1154, -1156,
	 -1171, -1172,   589, -1160, -1203,   336, -1166, -1200, -1199, -1198,
	 -1194, -1187, -1166, -1215, -1214,   503, -1217, -1209, -1218,   486,
	   485,   483, -1210,   484,   488,   610,   611,  -758, -1188, -1192,
	   613, -1194, -1183, -1190, -1193, -1167, -1222,   485,   499,    -9,
	  -825,  -387,  -391,   502, -1234,    -9,  -848,   299,  -851,  -138,
	  -684,  -109,  -115,   456,   360,  -222,  -138,  -110,  -672,  -530,
	  -143,  -129,  -147,  -457,   485,   499,  -138,  -689,  -690,  -691,
	  -171,  -109,  -163,  -129,  -129,  -770,  -288,  -293,   268,  -289,
	  -171,  -290,  -109,  -764,   586,   587,   588,  -765,  -766,  -767,
	  -768,  -769,  -310,   468,   276,  -773,  -703,  -775,  -776,  -131,
	   358,  -772,  -298,  -300,  -297,  -171,  -238,  -121,  -443,  -444,
	   603,   602,  -193,  -193,  -454,  -362,  -469,  -423,  -450,  -458,
	  -178,  -462,  -467,   604,   605,   609,   606,   607,   608,   277,
	   276,   305,  -464,  -142,   459,  -450,   276,  -109,  -146,  -238,
	  -577,   460,   458,  -580,  -581,  -171,  -143,  -583,  -163,  -179,
	  -110,  -187,  -189,  -129,  -208,  -200,   463,  -262,  -159,  -268,
	  -269,   332,   310,   319,   338,   388,   393,  -177,  -173,  -412,
	  -282,  -367,  -373,   350,  -152,  -285,  -286,  -126,  -127,  -283,
	  -322,  -193,  -222,   274,  -204,  -523,  -173,  -104,  -104,  -104,
	  -110,  -110,  -109,  -283,  -188,  -607,  -223,  -129,   485,  -173,
	  -623,  -121,  -129,  -143,  -621,  -143,  -617,  -626,  -129,  -270,
	  -410,   485,  -110,  -110,  -220,  -221,  -222,  -129,  -233,  -244,
	   485,  -110,  -163,  -651,  -109,  -657,  -530,  -457,  -177,  -223,
	  -177,  -223,  -138,  -138,  -359,  -360,  -177,   464,  -204,  -223,
	  -395,  -398,   322,   318,   314,   323,   329,  -110,  -110,  -282,
	  -129,  -129,  -427,  -369,  -415,   284,  -430,  -159,  -110,  -374,
	  -163,  -712,  -709,  -749,  -751,   486,   485,   483,  -457,  -724,
	  -730,   579,  -737,   408,   609,  -735,  -761,  -289,   502,   485,
	   598,  -784,   483,  -512,  -794,  -344,  -369,  -159,  -393,  -411,
	   316,  -159,   348,  -840,  -841,  -843,   272,  -839,  -673,  -523,
	  -947,  -903,  -932,  -934,  -934,  -935,   502,   503,  -980,  -985,
	  -986,  -987,   485,   486,   350,  -984,   485,   486,   503,  -996,
	  -756,  -344,   503,  -344,   503,   503,   501,  -942, -1037, -1040,
	   594,   591,   486,  -457,   485,   503,   503,  -942, -1126, -1118,
	 -1144,   503,  -942,  -146,  -146,  -177,  -121,  -121,  -121,  -379,
	  -362,  -340,  -508,  -129,  -472,  -110,  -110,  -409,  -476,  -109,
	  -227,  -226,  -475,  -501,  -409,  -490,   299,  -492,  -493,  -495,
	  -235,  -494,  -498,  -499,   312,  -193,  -110,  -177,  -464,   458,
	  -143,  -362,  -362,  -512,  -483,  -484,   485,  -509,  -129,    -9,
	 -1164, -1169,   485, -1173, -1161, -1204, -1199,   483,   484,   488,
	   503, -1189, -1195, -1185, -1188, -1192, -1195,  -364,  -523,   503,
	    -9,  -110,  -177,  -190,  -678,   485,  -685,  -225,  -110,  -668,
	  -129,  -146,  -146,  -146,  -110,  -110,  -171,  -692,  -186,  -690,
	  -110,  -771,   485,   317,  -679,  -109,  -171,  -289,  -171,  -109,
	  -171,  -163,  -765,   275,   358,   351,   275,   358,   351,  -774,
	  -156,   275,  -482,   486,   488,   484,  -349,  -129,  -774,  -776,
	   276,  -292,  -110,  -109,  -302,  -304,   462,  -193,  -193,  -110,
	  -110,  -356,  -470,  -206,  -377,  -362,  -206,  -453,  -452,  -109,
	  -459,  -460,  -461,   304,   331,  -458,  -464,  -109,  -452,  -362,
	   277,  -468,  -243,  -178,  -179,   485,  -362,  -334,  -110,  -578,
	  -143,  -457,   485,  -582,  -155,  -171,  -583,   502,  -129,  -143,
	  -179,  -110,  -177,  -110,  -177,  -190,  -201,   466,  -209,   456,
	   486,  -263,  -271,   566,  -270,  -177,  -267,  -421,  -110,  -374,
	  -282,  -177,  -128,  -284,  -369,  -324,  -333,  -335,  -336,  -337,
	  -338,  -109,  -227,  -226,  -343,  -345,  -346,  -225,  -347,  -348,
	  -129,  -349,  -350,  -284,  -602,  -186,  -163,   485,   609,  -146,
	  -146,  -618,  -110,  -177,  -190,  -549,  -110,  -177,  -223,  -223,
	  -234,  -204,  -235,  -245,   485,  -104,  -657,  -659,  -660,  -657,
	  -317,   411,   334,  -143,  -649,  -650,   270,  -143,  -163,  -227,
	  -206,   358,   485,   269,   257,  -457,  -109,  -110,  -110,  -361,
	   390,  -362,  -396,  -109,   322,   322,   322,   322,  -400,  -401,
	  -110,  -428,  -370,  -417,   395,  -416,  -163,  -229,  -413,  -414,
	  -409,  -434,   612,  -435,  -436,  -129,  -362,  -725,  -741,   485,
	   609,   609,  -738,  -719,   483,  -370,  -413,  -193,  -408,  -409,
	   504,  -842,   502,  -844,  -845,  -846,  -837,    -9,  -934,  -709,
	   503,   501,   609,   503,  -344,  -942,  -940,  -943,   486,   483,
	  -457, -1038,   502,   411,   503, -1104,  -129,  -129,  -163,  -471,
	  -110,  -409,  -409,  -409,  -409,  -110,  -494,  -495,  -362,  -362,
	  -496,   335,  -362,  -362,  -362,  -242,  -152,  -344,  -485,  -109,
	  -146, -1165,   299, -1168, -1174, -1202, -1199, -1205, -1207, -1208,
	   596, -1190, -1194,  -684,  -110,  -177,  -143,  -129,  -129,  -143,
	  -129,  -109,  -693,  -694,  -695,  -697,  -698,  -163,  -605,  -238,
	  -110,   317,   609,  -610,  -289,  -171,  -163,  -289,  -110,  -177,
	  -146,  -171,  -238,  -305,  -306,   310,   338,  -204,  -362,  -451,
	  -456,   278,  -452,  -109,  -454,  -362,  -459,  -463,  -362,  -109,
	  -468,  -243,  -451,  -110,  -173,  -179,  -129,   483,  -176,  -163,
	  -188,  -189,  -202,   458,   485,  -121,  -269,  -375,  -283,  -286,
	  -370,  -110,  -177,  -339,  -340,  -341,  -342,  -226,  -227,  -338,
	  -336,  -337,  -338,  -338,  -344,  -344,  -110,  -338,  -603,  -406,
	  -155,  -608,  -609,  -129,  -612,  -143,  -143,  -129,  -173,  -129,
	  -221,  -224,  -225,  -226,  -227,  -228,  -121,  -229,  -129,  -224,
	  -193,  -152,  -110,   485,  -163,  -138,   274,  -399,  -186,  -404,
	   281,  -129,  -429,  -276,  -418,  -171,   475,  -177,  -190,  -177,
	  -146,  -223,  -152,  -742,  -719,   609,  -740,  -739,  -709,   503,
	   501,  -371,  -155,  -177,  -673,  -512,   503,  -986,  -988,  -809,
	  -795,  -989,  -990,  -457,   485,   390,   358,  -922,  -845, -1039,
	   468, -1041,   276,   483,   503,   503,  -476,  -110,  -110,  -496,
	  -362,  -110,  -177,  -110,  -110,  -362,  -287,   503,  -486,  -292,
	  -163,  -129, -1160, -1162, -1188, -1206, -1169, -1204,  -190,  -177,
	  -225,  -690,  -177,  -696,  -155,  -696,  -696,  -171,  -699,  -688,
	  -684,  -686,  -109,   485,  -110,  -289,  -110,  -290,  -289,  -163,
	  -289,  -109,  -110,  -304,  -307,   345,   344,   267,   485,  -392,
	  -143,  -457,  -110,  -443,  -362,  -110,  -177,  -468,  -109,  -109,
	   503,   501,  -110,  -173,  -203,   462,   483,  -272,  -284,  -335,
	  -337,  -109,  -110,  -110,  -177,  -610,  -610,  -177,  -190,  -190,
	  -225,  -225,  -109,  -129,  -110,  -324,  -193,  -109,  -402,  -109,
	  -437,  -171,  -416,  -414,  -436,  -340,  -362,  -129,   501,  -709,
	  -709,  -289,   483,  -381,  -382,  -171,  -409,   503,  -344,   275,
	 -1042,   468,   275,   503,   501,  -362,  -362,  -110,  -276,  -288,
	  -487,   485,  -110,  -177, -1163, -1199, -1165, -1207, -1163,  -109,
	  -110,  -694,  -171,  -699,  -684,  -667,   339,  -687,  -684,   334,
	  -289,  -171,  -110,  -163,  -302,  -305,   275,   485,  -393,  -362,
	  -443,  -465,  -362,  -110,  -468,  -584,  -225,  -227,  -226,   483,
	  -176,  -204,   485,   505,  -338,  -338,  -609,  -110,  -224,  -245,
	  -405,  -129,  -403,  -129,  -237,  -419,   411,   350,  -719,   503,
	  -177,  -109,   503,   358,   483,  -110,  -362,  -110,  -109,  -163,
	 -1164, -1203, -1206,  -685,  -223,  -109,  -455,   604,   605,   607,
	   608,  -110,  -177,   485,  -289,  -289,  -110,  -362,  -177,  -110,
	  -110,  -225,  -225,   503,   463,   483,  -110,  -110,  -177,  -110,
	  -177,  -152,  -110,  -177,  -110,  -177,  -243,  -420,   291,  -289,
	  -382,  -171,   503,  -110,  -163,  -110, -1165, -1165,  -110,  -686,
	  -700,  -702,  -703,  -701,   358,  -225,  -109,  -701,  -684,  -289,
	  -362,   485,   463,  -338,  -224,  -129,  -129,  -129,  -438,  -439,
	  -440,  -441,  -129,  -206,  -177,  -110, -1160, -1208,  -110,  -177,
	  -704,  -225,   483,  -110,  -110,  -177,  -440,  -129,  -146,  -171,
	  -702,  -110,  -177,   505,  -439,  -110,  -225,   483
};

short yydef[] = {
	     2,    -2,     1,     3,     4,     5,     6,     7,     0,     0,
	     0,    11,    12,    13,    14,    15,     0,     0,     0,    19,
	  2252,  2248,    25,    26,    27,    28,    29,    30,    31,    32,
	    33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
	    43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
	    53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
	    63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
	    73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
	    83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
	    93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
	   103,   104,     0,     0,     0,  1651,     0,     0,     0,     0,
	     0,  1679,     0,     0,     0,  1683,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  1701,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  1711,  1712,     0,     0,  2113,  2114,  2115,
	  2116,  2117,  2118,  2119,  2120,  2121,  2122,  2123,  2124,  2125,
	  2126,  2127,    20,     0,    -2,     0,     0,     0,     0,     0,
	     0,     0,   138,   138,   931,   149,   835,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,   302,     0,
	     0,   334,     0,     0,     0,     0,   412,     0,     0,  1323,
	  1667,  2375,     0,   836,     0,   430,   431,   963,     0,  1004,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,   901,  1089,     0,     0,     0,     0,     0,     0,
	     0,     0,   510,   825,     0,  1413,  1464,  1524,  1535,     0,
	  1544,     0,     0,     0,     0,     0,     0,  1804,  1604,   510,
	     0,  2375,  1724,     0,     0,     0,     0,  1755,  1805,  1806,
	  1807,     0,     0,  1813,     0,  1818,     0,     0,     0,     0,
	     0,     0,  1908,  1913,     0,  1919,  1922,     0,  2375,     0,
	  1959,  1966,  1970,     0,  1984,  2055,     0,     0,     0,     0,
	     0,  2049,     0,     0,  2375,  2375,     0,     0,  2375,     0,
	  2375,  2375,     0,     0,     0,     0,  2375,  2375,   107,   109,
	   116,   118,   126,   128,   137,   144,     0,     0,   833,     0,
	   159,   185,   192,     0,   207,   209,   303,   161,   333,     0,
	   393,   396,     0,   410,     0,  1594,  1601,  1669,  1670,     0,
	   420,   856,   840,   841,   842,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,   422,   965,  1005,  1006,     0,
	   433,  1019,     0,  1660,  1042,   288,  1044,  1050,  1078,  1080,
	  1084,  1083,   899,     0,  1097,   930,  1106,  1194,   469,    -2,
	     0,  1395,     0,  1530,  1537,  1543,  1560,  1568,  1541,  1574,
	  1575,  1585,   485,  1721,  1731,     0,     0,  1812,  2075,  1820,
	  1824,  1826,  1874,     0,  1876,  1906,  1911,  2075,  1921,     0,
	  1945,     0,  1958,     0,     0,     0,     0,     0,     0,     0,
	     0,  2038,  2065,     0,  2005,     0,  2146,  2148,  2375,  2157,
	  2159,  2163,  2168,  2173,  2375,  2181,  2375,  2193,  2195,  2198,
	   152,   855,   860,     0,   868,   866,   889,   890,   891,   892,
	   893,   894,   869,   895,  1009,  1014,  1010,  1421,   483,  1744,
	  1810,  1817,  1830,  1918,  1930,  1948,  1963,  1969,  1978,  1981,
	  2009,  2013,  2020,  2023,     0,  2046,  2130,  2131,  2151,  2187,
	  1011,  1012,  1013,  2043,     8,  2241,     9,    10,    16,    17,
	  2242,    21,    22,    23,    24,     0,   105,  1602,     0,  2375,
	  1613,  2375,  2386,    -2,     0,  1308,  1278,  1279,   239,  2392,
	  1276,  1277,     0,  2373,   506,     0,     0,     0,  1674,  1675,
	  1676,  1677,  1678,  1680,  1681,  1682,  1684,  1685,  1686,  1687,
	  1688,  1689,  1690,  1691,  1692,  1693,  1694,  1695,  1696,  1697,
	  1698,  1699,  1700,  1702,  1703,  1704,  1705,  1706,  1707,  1708,
	  1709,  1710,  1713,  1714,     0,     0,  1259,     0,  1289,  1287,
	  1288,  1271,  2230,  2231,  2232,  2233,  1286,  1274,  1275,     0,
	     0,   117,   119,   121,   122,   123,   124,     0,  1267,   163,
	   569,     0,  1183,     0,   934,   935,   401,     0,   402,     0,
	   139,     0,   832,   834,     0,     0,     0,   191,  1323,   193,
	   196,   197,     0,     0,   336,   401,  1230,     0,  1244,     0,
	   223,     0,   212,   214,   215,   216,   217,   218,  1197,   289,
	     0,   293,     0,   301,   163,     0,  1323,  1323,   223,   223,
	   401,     0,   413,     0,     0,  1666,  1322,  1323,  1672,  1673,
	  2392,  2374,   419,   423,   424,   425,   426,   427,   428,   429,
	  1174,   421,   995,   966,   968,   969,   971,   972,   974,   975,
	   976,    -2,   979,    -2,   986,   987,   988,   989,   990,   633,
	   388,     0,  1228,  1020,  1658,  2375,     0,  1041,  1272,   285,
	  1045,  1054,  1077,     0,     0,  1081,     0,     0,  1090,  1091,
	   281,   240,   241,     0,     0,   260,   279,  1096,   929,  1292,
	  1293,  1294,  1101,     0,  1111,  1282,  1223,  1184,  1185,  1187,
	  1188,     0,  1137,  1138,  1195,   468,   471,   472,   522,   401,
	     0,    -2,   797,   820,   821,   822,   798,     0,   806,   807,
	   814,   815,   816,  2375,     0,   512,   818,  2375,   828,     0,
	  1396,  1397,  2083,  2084,  2085,  2086,  2087,  2081,  2080,  1445,
	  1411,  1414,     0,  1425,  2375,  1463,     0,     0,     0,  1529,
	  2375,  1532,  1533,  1534,  1538,  2375,  1542,  2375,  1546,     0,
	  1559,  2375,  1567,  1540,     0,  1569,  1570,  1571,  1572,  1573,
	     0,  1586,     0,  1627,   522,  1625,  1629,  1628,  1720,  1722,
	  1723,  1725,  1728,  1726,  1727,  2051,  1738,  1732,     0,  1760,
	  1753,  2055,  1756,     0,  1811,  1814,  1815,  2375,  2111,  1819,
	     0,  2079,  1825,  1873,  2375,  1829,  1878,  1905,     0,     0,
	  1912,  1915,  1916,     0,  1920,  1923,     0,     0,  1926,  1927,
	     0,  1944,  2101,  2102,  2103,  2104,  2091,  2088,     0,     0,
	  1946,  1949,     0,  1952,  1764,  1957,     0,  1961,     0,  1972,
	     0,  1975,  1976,  1979,     0,  2014,  2014,  2053,     0,  2014,
	  2014,  2017,  2018,  2021,  2375,  2031,  2026,  2029,  2030,     0,
	  2039,  1717,     0,  2044,     0,     0,  2143,     0,  2145,  2200,
	  2201,  2202,  2204,  2392,     0,  2149,  2152,  2375,  2111,     0,
	     0,  2375,     0,     0,  2174,     0,     0,  2185,  2188,     0,
	     0,     0,     0,   145,     0,     0,   153,   154,   933,  1458,
	   183,     0,   195,   205,   211,   287,   291,   335,   397,   223,
	     0,   800,  1466,     0,    -2,    -2,  1472,   411,   434,   470,
	   484,  1733,  1671,     0,     0,   857,   858,   859,   870,   871,
	   839,   844,   843,   845,   846,   847,   848,   849,   926,     0,
	     0,     0,     0,  1244,   330,   852,   854,   565,  1007,  1008,
	  1017,  1018,  1661,     0,  1049,     0,  1072,  1051,  1052,  1053,
	  1082,  1085,  1086,  1087,  1088,    -2,     0,     0,     0,   823,
	  1408,     0,  1440,   488,     0,   487,  1750,     0,  1809,  1816,
	  2074,  1837,  1831,  1832,  1833,  1834,  1835,  1836,  1917,  1837,
	  1947,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  2063,  2375,     0,  2129,  2208,  2175,  2208,   863,   867,
	   861,     0,    18,  2249,  2253,     0,  2254,  2255,  2256,  2257,
	  2258,  2259,  2260,  2276,  2277,     0,     0,     0,     0,  2362,
	     0,  2283,  2280,  2334,  2333,  2346,  2363,     0,  2322,  2336,
	  2337,  2338,  2339,     0,  2341,  1603,  1609,  1607,  1608,  1616,
	  1614,  1618,  1309,  1309,     0,  1615,  2382,     0,  2385,     0,
	  2380,  2381,  2389,     0,  1182,   480,  1717,     0,  1647,     0,
	  2375,  2375,  1653,  2250,   830,   106,     0,   140,   142,  1208,
	     0,     0,  1196,  1213,   108,     0,  1213,     0,   113,   114,
	   115,   120,  1324,  1325,  1327,  1329,  1331,  1332,  1333,  1334,
	  1335,  1336,  1337,  1338,  1339,  1340,  1341,  2240,  1305,     0,
	  1248,   125,   162,   164,     0,     0,     0,     0,     0,     0,
	     0,   173,   174,   175,   176,   177,   178,   179,  1199,  1256,
	  1251,  1226,  1231,  1233,  1206,  1210,  1247,  1237,  1238,  1235,
	  1236,  1239,     0,  1212,     0,   127,     0,     0,     0,   136,
	   143,   937,   158,     0,  1229,   182,     0,     0,     0,     0,
	  1209,     0,   556,   206,  1323,     0,     0,     0,   219,   220,
	   221,   290,   295,   297,   298,   299,     0,   292,   294,   305,
	   160,     0,  1221,   392,   395,     0,     0,     0,     0,   404,
	    -2,     0,     0,  1598,  2386,     0,  1600,  1668,  2379,     0,
	     0,     0,   967,   983,     0,     0,     0,   985,   980,  1291,
	     0,   223,     0,  1016,     0,  1663,  1664,  1665,     0,     0,
	  1323,  1323,  1046,  1047,     0,     0,     0,     0,   921,   902,
	   903,   906,   908,   909,   910,   911,   912,   914,   915,     0,
	     0,  1093,     0,     0,     0,     0,   263,   265,   267,   268,
	     0,     0,  1098,  1139,  1103,     0,  1105,     0,  1120,  1109,
	     0,     0,  1114,  1115,  1116,     0,  1189,     0,     0,  1193,
	     0,     0,  1222,     0,     0,   554,     0,     0,     0,   511,
	   513,  1309,  1309,  2392,  2386,   826,  2386,  1394,     0,  1399,
	  1407,     0,  1446,  1415,  1412,  2375,  1423,  1424,  1462,  1459,
	     0,  1522,  1523,  1525,  1527,  1531,  1536,     0,     0,  1551,
	  1309,  1545,  1549,  1548,     0,  1562,  1564,  1565,  1566,     0,
	  1577,     0,  1554,  1555,  1556,  1584,  1588,     0,     0,     0,
	  1592,  1587,   575,   560,  1626,     0,  1634,     0,     0,  1734,
	  1735,  1736,  1737,  1739,  1740,  1738,  1800,  1759,  1761,  1775,
	     0,  1808,     0,  2067,     0,  1309,  2386,  2112,     0,     0,
	     0,  2078,     0,  1843,  1844,  1845,     0,  1309,  1875,  1877,
	     0,  1880,     0,  1909,     0,     0,  2057,     0,     0,  1924,
	     0,     0,  1931,  1932,     0,  1849,  1852,  1853,  1854,  1855,
	  1856,  1857,  1858,  1859,  1860,  1861,  2239,  2089,  2090,  1950,
	     0,  1953,     0,  1956,  1787,  1960,     0,  1965,  1967,     0,
	  1974,  1971,  1982,     0,  1985,  1986,    -2,  1987,     0,  2006,
	     0,  2016,  2007,  2054,  2010,  2011,  2024,  2027,  2028,  2034,
	     0,  2033,  2037,     0,  1716,     0,  2048,  2004,  2128,  2375,
	  2132,     0,  2134,     0,  2080,     0,  2139,    -2,     0,  2142,
	  2223,  2224,  2225,  2226,  2092,  2096,  2100,  2082,     0,     0,
	  2093,  2097,  2377,  2378,  2386,     0,  2154,     0,  2386,  2375,
	     0,  2160,  2210,  2211,  2212,  2213,  2214,     0,  2164,     0,
	     0,  2172,     0,     0,  2177,     0,  2228,  2229,     0,  2182,
	     0,  2206,     0,  2190,     0,     0,     0,  2207,  2218,  2219,
	  2220,  2221,  2197,     0,     0,     0,     0,   194,  1323,     0,
	   799,     0,  1323,   853,   896,     0,   850,   925,   927,   928,
	   851,   879,   880,   881,   883,   884,   326,   327,   328,     0,
	   331,     0,  1659,  1598,     0,  1071,     0,   808,   809,   819,
	     0,  1409,     0,   489,   490,   491,   486,  1752,  1745,  1746,
	  1747,  1748,  1749,  1743,  1828,  1929,    -2,    -2,    -2,  1750,
	    -2,  2012,    -2,  2022,  2040,  2041,  2064,    -2,  2150,  2209,
	  2186,   864,  2042,     0,  2263,     0,  2324,  2304,     0,     0,
	  2344,     0,  2347,  2359,     0,  2349,  2361,  2364,  2365,     0,
	     0,  2287,     0,  2331,  2282,     0,  2367,     0,    -2,    -2,
	  2340,  2342,  1605,     0,  1617,  2375,  1620,  1310,     0,  2392,
	  1314,  1313,  1621,     0,  2383,  2384,  2393,  2397,  2396,  1307,
	  2390,  2391,     0,   508,  1622,  1717,     0,  1647,  1624,     0,
	  1654,  1309,  1290,   141,     0,   148,     0,     0,   111,   112,
	     0,  1316,  1330,  1216,     0,   165,   166,   180,     0,     0,
	     0,  1205,   170,   791,     0,     0,  1249,  1232,   171,   172,
	   568,   571,   129,   390,     0,   362,     0,   364,   365,   366,
	     0,     0,     0,     0,   387,   384,     0,     0,   389,     0,
	   133,     0,   400,     0,   936,   938,     0,  1323,   189,     0,
	     0,   198,  1252,   337,     0,   557,   208,     0,   224,   572,
	   223,   213,   296,   312,   304,     0,   307,   332,     0,     0,
	     0,     0,   342,   572,     0,   405,   407,     0,     0,  1593,
	     0,  1595,  2386,     0,     0,   991,     0,     0,   996,   997,
	    -2,   992,   970,     0,     0,     0,   432,     0,     0,     0,
	  1021,  1022,     0,  1039,  1040,  1273,   286,  1043,  1063,     0,
	     0,  1056,  1058,  1060,  1061,  1073,  1079,     0,     0,     0,
	     0,   904,   905,     0,     0,   900,     0,  1092,   244,     0,
	     0,   282,   283,   284,     0,     0,   269,   270,   271,   272,
	  1242,     0,     0,   280,  1095,     0,  1100,  1102,  1140,  1159,
	  1143,  1144,  1145,  1280,  1147,  1148,  1159,  1281,     0,     0,
	  1263,  1104,  1107,  1155,  1156,  1157,  1158,  1175,  1176,  1177,
	  1178,  1179,  2392,  1180,  1181,  1108,     0,     0,     0,     0,
	  1117,     0,  1186,  1190,  1191,  1192,     0,  1254,   521,    -2,
	   525,     0,   534,   547,    -2,    -2,   553,   803,   804,   805,
	   617,   600,   522,     0,   607,   605,     0,  2375,   515,   516,
	   517,  2387,   817,   827,     0,  1400,     0,  1403,  1404,  1405,
	  1444,  1447,  1448,  2050,  1449,     0,  1441,  1442,  1443,  1417,
	  1416,     0,  1422,  1457,     0,     0,  1473,     0,  1478,  1479,
	     0,     0,  1550,  2375,     0,  1547,  1561,  2375,  1576,     0,
	     0,     0,  1590,     0,  2244,  2246,  2247,  1591,   577,     0,
	  1264,   562,     0,  1630,     0,     0,  1636,  2386,  1635,  2052,
	  2386,     0,  1741,  1802,  1762,     0,  1766,     0,     0,     0,
	     0,     0,  1774,    -2,  1758,  2066,  2375,     0,  2076,  2077,
	  2071,  2110,     0,     0,  1823,  1827,  2375,  1847,     0,  1879,
	  1881,  1890,     0,  1885,  1886,     0,     0,  2056,     0,  2375,
	     0,  1925,  1928,     0,     0,     0,     0,     0,  1765,  1787,
	  1777,  1778,  1779,  1780,  1783,  1787,     0,  1785,  1786,  1964,
	  1973,  1983,     0,  1989,     0,  2025,     0,  2036,  2032,  1715,
	  1718,     0,  2047,  2144,  2133,     0,     0,     0,     0,  2222,
	  2375,  2094,  2098,  2095,  2099,  2147,  2153,  2375,     0,  2215,
	     0,  2158,  2375,  2162,  2375,     0,     0,  2169,     0,     0,
	  2176,     0,     0,  2180,     0,     0,  2227,  2375,  2189,     0,
	     0,     0,  2194,     0,     0,     0,     0,     0,   338,   399,
	  1467,  1468,  1469,   837,     0,     0,   872,   874,    -2,   878,
	     0,   329,   566,  1662,  1038,   237,   824,   482,   492,   493,
	   494,   569,    -2,   692,     0,   699,     0,     0,    -2,   702,
	   703,   765,     0,   765,   710,   711,     0,   759,   760,   761,
	   762,     0,   712,   713,   714,   715,   716,   717,   718,    -2,
	   719,   720,   721,   722,   723,   724,   725,   726,   727,   728,
	     0,     0,     0,     0,   783,   784,  1309,  1309,     0,   729,
	     0,  1207,     0,  1257,     0,    -2,   779,   780,   789,   790,
	  1234,  1211,  1878,  1751,  1838,  1839,  1840,  1841,  1842,  1962,
	  2072,  1968,  1977,  1980,  2008,  2019,  2045,   862,     0,     0,
	     0,     0,  2332,     0,  2323,  2305,  2297,     0,  2335,     0,
	     0,     0,     0,  2278,  2289,  2302,  2297,  2284,  2288,  2285,
	  2286,  2299,     0,  2281,     0,  1606,     0,  1634,  2375,  1619,
	  1311,  1312,  2376,     0,     0,  2399,  2395,  2398,   507,     0,
	     0,  1649,  1623,  1652,  2375,  1656,  1657,     0,     0,     0,
	  1326,  1328,     0,     0,  1344,  1345,  1346,  1347,  1348,  1349,
	  1315,  1241,     0,     0,   167,   168,   169,   792,   793,     0,
	     0,   130,   391,  1491,   360,  1482,     0,  1227,   363,  1517,
	     0,   368,     0,   383,   634,     0,     0,   639,   640,   641,
	   642,   643,  1240,     0,     0,     0,  1218,   569,   385,     0,
	   386,   131,   134,   135,   132,     0,     0,     0,   184,   186,
	     0,     0,     0,     0,   204,   222,     0,     0,   322,     0,
	  1262,   306,   308,   310,  1323,   593,   595,     0,   522,   485,
	   398,     0,   339,     0,   577,     0,     0,   560,   415,   416,
	  1599,  1596,  2386,  1323,  1287,     0,  1001,     0,  1003,  1250,
	   993,   994,   973,     0,     0,   435,   436,     0,   577,   439,
	  1316,  1026,  1025,     0,     0,  1323,     0,  1062,  1055,     0,
	     0,     0,     0,     0,     0,     0,   920,    -2,   923,   924,
	   919,   907,     0,     0,  1094,   235,     0,   242,   243,   273,
	   264,     0,   278,     0,  1159,  1142,  1160,     0,  1151,  1154,
	  1171,  1172,  1173,    -2,  1149,  1150,  1161,  1163,     0,  1162,
	  2388,  1118,  1121,  1124,  1119,  1110,     0,     0,   560,   477,
	     0,     0,     0,  1318,     0,  1317,     0,     0,     0,   583,
	     0,     0,   602,     0,   488,     0,   606,   514,  1398,     0,
	  1406,     0,     0,  1451,     0,  1419,  1418,     0,     0,  1426,
	  1460,  1461,  1465,     0,  1482,  1480,  2234,  2235,     0,  1526,
	  1528,  2375,  1552,     0,  1563,  1578,  1579,  1580,  1581,  1309,
	  1583,  1589,  2243,  2245,   583,     0,   576,  1265,  1266,   574,
	     0,   558,   559,    -2,  1643,  1639,  1632,     0,  1633,  1729,
	  2386,  1799,  2065,  1803,     0,  1775,     0,     0,  1787,  1770,
	  1771,  1773,  1781,  1782,  1754,  1757,  2068,  2069,     0,     0,
	  1822,  1846,  1848,     0,  1883,     0,     0,     0,  2375,  2058,
	  2059,  2061,  1309,  2106,  2107,  2108,  2109,  2062,     0,  1933,
	  1934,  1935,  1936,  1937,  1938,  1939,  1940,  1941,  1309,  1943,
	  1850,     0,  1862,  1863,  1864,  1865,  1866,  1867,  1868,  1869,
	  1870,  1871,  1872,  1951,  1954,  1955,  1784,     0,  1789,     0,
	  1792,  1793,  1794,  1795,  1988,     0,     0,  2035,  1719,  2135,
	  2136,  1309,  2137,  2138,     0,  2155,  2216,  2156,     0,  2203,
	  2165,  2166,  2167,     0,     0,     0,  2178,  2179,  2183,  2184,
	     0,  2191,  2217,  2192,     0,   150,   151,   155,     0,   897,
	   838,   885,   886,   887,     0,     0,     0,   882,     0,     0,
	     0,     0,     0,  1201,   693,   694,   695,   696,   697,  1255,
	  1253,  1219,     0,   700,   701,     0,     0,   763,   764,   765,
	     0,   757,   758,     0,   741,     0,   751,   748,     0,   767,
	     0,     0,    -2,    -2,     0,     0,   794,   795,  2375,     0,
	   778,     0,     0,   865,  2251,    -2,  2265,  2267,  2268,  2269,
	  2270,  2271,     0,  2274,  2319,  2343,  2262,  2306,  2307,     0,
	  2314,  2298,  2309,  2348,  2345,  2360,  2350,  2351,  2352,  2353,
	  2354,  2326,     0,  2355,  2357,  2328,  2329,     0,     0,  2293,
	  2303,  2300,  2279,  2291,  2302,  2300,  2368,  2369,  2370,   506,
	  2386,  1612,     0,   519,  2394,   509,  1648,     0,  1655,     0,
	  1319,     0,  1353,  1354,  1355,  1356,     0,   110,  1269,  1297,
	  1298,     0,     0,  1304,    -2,    -2,     0,     0,  1365,  1366,
	  1370,     0,     0,   567,   570,     0,   348,     0,  1268,   349,
	  1482,     0,     0,  1483,  1283,  1284,  1285,  1484,  1486,  1487,
	  1488,  1489,  1490,  1491,     0,  1499,     0,  1502,  1515,     0,
	  1504,  1518,  1519,  1520,  1521,     0,     0,   372,     0,     0,
	  1198,  1246,   637,     0,     0,     0,   522,   688,     0,   666,
	     0,     0,   678,   649,   650,   651,   652,   653,   654,   655,
	  1240,  1202,   673,     0,  1225,     0,     0,     0,     0,     0,
	     0,   958,   959,   939,   952,   941,   942,   943,     0,   190,
	   199,     0,   201,  1319,   225,   230,     0,   324,     0,   311,
	   313,   315,   316,   317,   318,   319,   320,     0,   394,     0,
	     0,   597,   488,   403,     0,   343,   344,   346,   390,   583,
	   406,   417,   418,     0,   408,  1597,   964,   999,  1000,  1002,
	   978,   981,     0,   583,  1030,  1024,     0,  1023,     0,  1048,
	  1064,  1065,   569,  1067,  1057,  1059,  1063,     0,  1319,   926,
	   917,   918,   913,   916,     0,   246,     0,     0,   560,     0,
	   276,   266,  1099,  1141,     0,  1152,  1295,  1296,     0,     0,
	     0,  1125,     0,     0,   473,   474,     0,  1263,   475,     0,
	    -2,     0,   536,     0,     0,     0,     0,    -2,   535,     0,
	   549,   551,   585,   619,     0,  1243,   601,     0,   603,     0,
	   608,  1401,  1402,  1450,  1452,  1453,  1454,  1455,  1456,  1436,
	  1420,     0,  1428,     0,  1429,     0,  1474,  1475,     0,  1481,
	  2236,  1539,  1557,  1558,  1553,  1582,   585,     0,   520,     0,
	  1224,     0,     0,  1638,     0,  2375,  1642,  1637,     0,  1730,
	     0,  1763,  1767,  1768,     0,  1769,     0,  1821,  1882,     0,
	  1891,     0,  1894,  1895,  1896,  1884,  1887,  1888,  1907,     0,
	  1309,  2105,     0,  1942,  1851,  1788,     0,     0,  1995,  1991,
	  2237,  2238,  1992,  1993,  1994,  2015,  2141,     0,  2170,  2171,
	     0,  2205,  2375,     0,     0,     0,   873,   875,   877,   495,
	   496,   499,   788,   498,   691,   698,   753,     0,   705,     0,
	     0,     0,   709,     0,     0,   740,  1215,   742,   751,   744,
	     0,   747,   749,     0,  1214,     0,   768,     0,     0,     0,
	   775,     0,     0,  1309,   730,   733,   732,   785,   787,  1742,
	     0,     0,  2272,  2310,  2275,     0,  2308,  2327,  2356,  2358,
	  2366,  2290,  2295,  2301,     0,  2294,  2297,  1610,  1611,   518,
	  1650,   146,     0,  1350,  1320,  1321,     0,  1357,   147,  1342,
	  1270,     0,     0,     0,  1343,  1364,     0,     0,     0,     0,
	   181,  1492,  1333,  1341,  1495,     0,   361,   350,  1482,     0,
	  1482,     0,  1485,  1497,  1503,  1513,  1498,  1505,  1514,  1500,
	  1506,  1507,  1508,  1509,  1510,  1511,  1512,     0,  1501,  1516,
	     0,     0,   367,     0,   370,     0,   375,   635,   636,   638,
	   685,   560,     0,   689,   690,   663,     0,   765,   647,     0,
	     0,   667,   668,  1203,  1204,   666,   672,     0,   670,   671,
	   656,   679,     0,     0,   683,   684,   663,     0,   371,  1323,
	   960,   961,   962,   940,     0,   947,   948,     0,   944,   945,
	   946,   189,     0,  1316,     0,   203,   232,     0,   226,   227,
	   228,   300,     0,   325,   321,     0,   309,   594,   596,     0,
	   577,     0,   347,   340,   585,   409,     0,   440,    -2,    -2,
	     0,     0,     0,     0,  1309,   457,  1309,   459,   460,   461,
	   462,   463,     0,   438,  1036,     0,  1027,  1028,     0,     0,
	     0,  1323,  1074,     0,  1075,   898,   245,     0,     0,     0,
	   259,   261,     0,     0,   277,     0,  1153,  1164,  1165,  1166,
	  1167,  1168,  1169,  1170,  1122,  1123,     0,  1127,  1128,     0,
	  1130,  1131,  1132,  1133,  1134,  1135,     0,  1112,  1113,   478,
	     0,   479,   543,     0,   537,   538,   539,   540,   527,     0,
	    -2,   621,   620,     0,  1220,   584,   591,   592,   618,   579,
	  1319,   604,   609,   610,   611,   569,   614,  1410,     0,     0,
	  1433,  1431,     0,  1427,     0,   500,   578,   573,   561,   563,
	     0,  1640,  2375,  1641,  1645,  1646,  1643,  1801,  1772,     0,
	  1889,     0,     0,  1878,  1914,  2375,  1790,  1791,  1796,  1797,
	  1798,  1998,     0,     0,  2199,     0,   156,   157,   888,     0,
	   754,     0,   707,   708,     0,   756,   743,   745,     0,   752,
	     0,  1258,     0,     0,     0,     0,     0,     0,   736,     0,
	     0,     0,  2325,  2266,  2263,  2311,  2312,    -2,  2316,  2319,
	  2330,  2292,  2296,  1319,     0,     0,  1302,  1303,  1299,  1301,
	  1300,     0,  1369,  1372,  1392,  1392,  1392,     0,    -2,  1371,
	  1367,  1493,     0,     0,   351,  1482,     0,   353,  1482,     0,
	     0,  1482,     0,   373,     0,   377,   378,   562,   687,   644,
	     0,  1217,   645,     0,     0,     0,     0,     0,     0,     0,
	   681,     0,   648,   677,   932,   954,   949,     0,   187,     0,
	  1323,   202,   234,     0,   229,  1045,   314,   598,   583,   345,
	   582,   437,     0,     0,   451,   452,   453,   454,   455,     0,
	   444,   445,   449,   450,   456,   458,   464,     0,  1015,     0,
	     0,  1029,  1031,  1319,  1037,  1066,  1068,  1069,  1070,  1319,
	   247,   248,   250,     0,     0,   253,   254,   255,   569,   249,
	   262,     0,  1146,  1126,  1129,     0,     0,   526,     0,     0,
	  1260,   530,   599,   631,     0,   587,   588,     0,   581,     0,
	     0,     0,     0,  1435,  1438,  1437,     0,     0,  1430,  1482,
	     0,   481,     0,     0,     0,  1309,  2070,  1892,  1893,  1897,
	  1898,  1899,  1900,  1901,  1902,  1903,  1904,  1910,  2060,  1990,
	     0,  2003,     0,     0,  2161,  2196,   704,   706,   755,     0,
	   750,   769,     0,   773,   774,     0,   360,   796,   731,     0,
	     0,   786,  2261,     0,     0,     0,     0,     0,  1351,     0,
	  1358,     0,     0,  1374,     0,  1375,  1376,  1378,  1381,     0,
	    -2,  1380,     0,     0,  1496,   352,  1482,     0,   358,     0,
	   355,     0,   372,     0,   376,   379,   380,     0,     0,   574,
	   661,   662,   646,     0,     0,   669,     0,     0,     0,     0,
	   950,     0,   189,   200,   560,     0,     0,   323,   341,   441,
	   446,     0,   448,   465,     0,  1034,  1035,     0,  1033,  1076,
	   251,   252,     0,   274,  1136,   476,   541,     0,   529,     0,
	     0,   632,     0,   580,   612,   616,   613,   615,     0,  1434,
	  1432,  1476,     0,   501,   502,   504,   564,  1631,     0,  1999,
	  2000,     0,  2001,  1996,     0,   746,     0,   771,     0,     0,
	   738,   739,   734,     0,    -2,  2313,     0,  2317,    -2,     0,
	  1368,  1373,     0,  1377,  1379,     0,  1261,     0,  1360,     0,
	   357,  1482,  1482,     0,   369,   374,   381,   382,   686,   664,
	     0,   674,   675,   680,     0,     0,   955,     0,     0,     0,
	   188,   210,     0,     0,     0,     0,  1032,   256,     0,     0,
	     0,   544,     0,   532,   623,     0,   589,   590,  1439,  1482,
	     0,     0,  1644,  2002,     0,   770,     0,   766,     0,     0,
	     0,  2315,     0,     0,     0,     0,     0,   657,   658,   659,
	   660,  1359,     0,  1494,   354,   359,  1482,   665,     0,   682,
	   953,   956,   957,   951,     0,     0,   447,   466,     0,   257,
	     0,     0,   542,     0,   531,     0,     0,   586,  1245,  1477,
	   503,     0,  1997,   772,     0,   735,     0,     0,  1352,  1393,
	     0,  1384,  1386,  1387,   728,  1388,     0,  1383,  1361,   356,
	   676,   233,     0,     0,     0,   275,   545,   533,   622,   624,
	   626,     0,   629,   630,     0,   737,  2273,  2318,  1382,     0,
	     0,  1390,     0,   467,   258,     0,   627,   629,   628,     0,
	  1385,  1389,     0,     0,   625,   505,  1391,   231
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
#line 1123 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* 
		    ** Note that FRS blocks fall through to this rule too, so
		    ** if you add actions here, do it at block_stmt too.
		    */
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 3:
#line 1134 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_func = 0;	/* No current function */
		}
		break;
	case 8:
#line 1147 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_CHREOS) /* Turn EOS checking Off */
			esq_eoscall(IImodNOCEOS);
		    if ((gr->gr_flag & GR_NOSYNC) == 0)
		    {
			if (gr->gr_flag & GR_REPEAT)
			    rep_close( IISYNCUP, NULL, FALSE );
			else
			{
			    if (gr->gr_flag & GR_COPY_PROG)
			    /* 
			    ** If COPY FROM/INTO PROGRAM, check that handler has
			    ** been declared.
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
		    gr->gr_flag &= ~(GR_SQL|GR_REPEAT|GR_NOSYNC);
		    if ((gr->gr_flag & GR_PROCEXEC) == 0)
			gen_sqlca( sqcaGEN );
		    else
			gr->gr_flag &= ~GR_PROCEXEC;
		    gen_eqstmt( G_CLOSE, (char *)0 );
                    if (eq->eq_flags & EQ_ENDIF)
                    {
                        gen_if( G_CLOSE, 0, C_0, 0 );
                        eq->eq_flags &= ~EQ_ENDIF;
                    }
		}
		break;
	case 9:
#line 1189 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			_VOID_ eqck_tgt( 0, 0 );	/* clear */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			gen_sqlca( sqcaGEN );
			gen_eqstmt( G_CLOSE, (char *)0 );
		    }
		    gr->gr_flag &= ~(GR_SQL|GR_REPEAT);
                    if (eq->eq_flags & EQ_ENDIF)
                    {
                        gen_if( G_CLOSE, 0, C_0, 0 );
                        eq->eq_flags &= ~EQ_ENDIF;
                    }
		}
		break;
	case 10:
#line 1205 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_CHREOS) /* Turn EOS checking Off */
			esq_eoscall(IImodNOCEOS);
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			if (yypvt[-1].i)
			{
			    
			    /* GET DBEVENT must not generate DBEVENT condition */
		            if ((gr->gr_flag & GR_GETEVT) == 0)
			    {
			        gen_sqlca( sqcaGEN );
			    }
			    else
			    {
				gen_sqlca( sqcaGETEVT );
				gr->gr_flag &= ~GR_GETEVT;
			    }
			}
			gen_eqstmt( G_CLOSE, (char *)0 );
		    }
		    gr->gr_flag &= ~(GR_ESQL);
                    if (eq->eq_flags & EQ_ENDIF)
                    {
                        gen_if( G_CLOSE, 0, C_0, 0 );
                        eq->eq_flags &= ~EQ_ENDIF;
                    }
		}
		break;
	case 11:
#line 1234 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
    		    gen_eqstmt( G_CLOSE, (char *)0 );
                    if (eq->eq_flags & EQ_ENDIF)
                    {
                        gen_if( G_CLOSE, 0, C_0, 0 );
                        eq->eq_flags &= ~EQ_ENDIF;
                    }
		}
		break;
	case 12:
#line 1243 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if ((gr->gr_flag & GR_NOSYNC) == 0)
		    {
			db_close( IISYNCUP );
		    }
		    gr->gr_flag &= ~GR_NOSYNC;
		    esq->flag &= ~ESQ_REG_SELECT;
		    esq->flag &= ~ESQ_DESC_SEL;
		    gen_sqlca( sqcaGEN );
		    gen_eqstmt( G_CLOSE, (char *)0 );
                    if (gr->gr_flag & GR_ENDIF)
                    {
                        gen_if( G_CLOSE, 0, C_0, 0 );
                        gr->gr_flag &= ~GR_ENDIF;
                    }
		}
		break;
	case 13:
#line 1260 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			_VOID_ eqck_tgt( 0, 0 );	/* clear */
		    gr->gr_flag &= ~(GR_ESQL|GR_REPEAT);
		    esq->flag &= ~ESQ_REG_SELECT;
		    gen_sqlca( sqcaGEN );
		    gen_eqstmt( G_CLOSE, (char *)0 );
		    if (gr->gr_flag & GR_ENDIF)
		    {
			gen_if( G_CLOSE, 0, C_0, 0 );
			gr->gr_flag &= ~GR_ENDIF;
		    }
		}
		break;
	case 14:
#line 1274 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			_VOID_ eqck_tgt( 0, 0 );	/* clear */
		    if (eq->eq_flags & EQ_CHREOS) /* Turn EOS checking Off */
			esq_eoscall(IImodNOCEOS);
		    if ((gr->gr_flag & GR_NOSYNC) == 0)
		    {
			if (gr->gr_flag & GR_REPEAT)
			    rep_close( IISYNCUP, NULL, FALSE );
		    }
		    gr->gr_flag &= ~(GR_ESQL|GR_REPEAT|GR_NOSYNC);
		    if ((gr->gr_flag & GR_PROCEXEC) == 0)
			gen_sqlca( sqcaGEN );
		    else
			gr->gr_flag &= ~GR_PROCEXEC;
		    gen_eqstmt( G_CLOSE, (char *)0 );
		    if (gr->gr_flag & GR_ENDIF)
		    {
			gen_if( G_CLOSE, 0, C_0, 0 );
			gr->gr_flag &= ~GR_ENDIF;
		    }
		}
		break;
	case 15:
#line 1297 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* The stmts have their own host_term because of blocks */
		    gr->gr_flag &= ~GR_ESQL;
		    gen_eqstmt( G_CLOSE, (char *)0 );
                    if (eq->eq_flags & EQ_ENDIF)
                    {
                        gen_if( G_CLOSE, 0, C_0, 0 );
                        eq->eq_flags &= ~EQ_ENDIF;
                    }
		}
		break;
	case 16:
#line 1308 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_flag &= ~GR_ESQL;
		    gen_eqstmt( G_CLOSE, (char *)0 );
                    if (eq->eq_flags & EQ_ENDIF)
                    {
                        gen_if( G_CLOSE, 0, C_0, 0 );
                        eq->eq_flags &= ~EQ_ENDIF;
                    }
		}
		break;
	case 17:
#line 1318 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (   (gr->gr_flag & GR_NOSYNC) == 0
			&& (gr->gr_flag & GR_SQL))
		    {
			db_op( ERx("\"...SYNTAX ERROR") );
			if (gr->gr_flag & GR_REPEAT)
			    rep_close( IISYNCUP, NULL, FALSE );
			else
			    db_close( IISYNCUP );
		    }
		    arg_free();
		    id_free();
		    gen_host( G_H_NEWLINE, (char *)0 );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    if ((esq->flag & ESQ_NOPRT) == 0 && 
			    (gr->gr_flag & (GR_SQL|GR_ESQL)))
			gen_eqstmt( G_CLOSE, (char *)0 );
		    if (esq->flag & ESQ_REG_SELECT)
			esl_clean();
		    gr->gr_flag &=
		       ~(GR_SQL|GR_ESQL|GR_REPEAT|GR_NOSYNC|GR_PROCEXEC);
		    gr_mechanism( GR_ERROR );
		}
		break;
	case 18:
#line 1367 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* 
		    ** Strip to end of line, but ensure scanner is synchronized
		    ** via 'lex_newline' - see yylex routines.	This is because
		    ** the newline is the EXEC trigger.
		    */
		    sc_reset();
		    if (   yypvt[-1].s
			&& inc_parse_name(yypvt[-1].s, gr->gr_adjective) == SC_INC)
			inc_push_file();
		    gr->gr_adjective = 0;
		}
		break;
	case 19:
#line 1380 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* 
		    ** Better be "<EOF>" else Yacc will produce syntax error.
		    ** File closed by scanner - allowed because of statement
		    ** terminator.
		    */
		    dml->dm_exec = DML_HOST;
		}
		break;
	case 20:
#line 1390 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
                    /* Accept either EXEC SQL | 4GL INCLUDE */

                    if ((dml->dm_exec != (DML_EXEC|DML__SQL)) &&
                        (dml->dm_exec != (DML_EXEC|DML__4GL)))
                        er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"),
                                                            ERx("INCLUDE") );
		}
		break;
	case 21:
#line 1400 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (ESQ_EQSTR(yypvt[-0].s, ERx("sqlca")))
		    {
			gen_sqlca( sqcaSQL );
			esq->inc |= sqcaSQL;
			yyval.s = (char *)0;
			if (eq->eq_flags & EQ_FIPS)
			    er_write( E_EQ050A_FIPS_EXTSN, EQ_WARN,
					    1, ERx("INCLUDE SQLCA") );
		    } else if (ESQ_EQSTR(yypvt[-0].s, ERx("sqlda")))
		    {
			gen_sqlca( sqcaSQLDA );
			esq->inc |= sqcaSQLDA;
			yyval.s = (char *)0;
			if (eq->eq_flags & EQ_FIPS)
			    er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
					    1, ERx("INCLUDE SQLDA") );
		    } else
		    {
			yyval.s = yypvt[-0].s;
			if (eq->eq_flags & EQ_FIPS)
			    er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
					    1, ERx("INCLUDE") );
		    }
		    gr->gr_adjective = FALSE;
		}
		break;
	case 22:
#line 1427 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Eat the name till right angle bracket */
		    sc_eat(id_add, SC_STRIP, ERx(">"), '<', '>');
		    yyval.s = str_add(STRNULL, id_getname());
		    id_free();
		    gr->gr_adjective = TRUE;
		}
		break;
	case 23:
#line 1436 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 24:
#line 1440 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 92:
#line 1536 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 93:
#line 1540 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 94:
#line 1544 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 95:
#line 1548 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 96:
#line 1552 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 97:
#line 1556 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 98:
#line 1560 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 99:
#line 1564 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 100:
#line 1568 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 101:
#line 1572 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 102:
#line 1576 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 103:
#line 1580 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 104:
#line 1584 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 106:
#line 1602 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
				    EQ_WARN, 1, ERx("ALTER ROLE"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("ALTER ROLE"));
		}
		break;
	case 107:
#line 1612 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 108:
#line 1624 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
				    EQ_WARN, 1, ERx("ALTER GROUP"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("ALTER GROUP"));
		}
		break;
	case 109:
#line 1634 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 111:
#line 1640 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 112:
#line 1646 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
    		    if (STbcompare(yypvt[-0].s, 0, ERx("users"), 5, TRUE) != 0)
    		    {
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
				     ERx("ALTER GROUP"), ERx("USERS") );
		    }
		    db_key( ERx("users") );
		}
		break;
	case 115:
#line 1664 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
				    EQ_WARN, 1, ERx("ALTER LOCATION"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("ALTER LOCATION"));
		}
		break;
	case 116:
#line 1674 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 117:
#line 1686 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
				    EQ_WARN, 1, ERx("ALTER SECURITY_AUDIT"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
				    EQ_WARN, 1, ERx("ALTER SECURITY_AUDIT"));
		}
		break;
	case 118:
#line 1696 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 122:
#line 1705 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{ db_key(yypvt[-0].s);}
		break;
	case 123:
#line 1707 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{ db_key(yypvt[-0].s);}
		break;
	case 124:
#line 1709 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{ db_key(yypvt[-0].s);}
		break;
	case 125:
#line 1716 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
				    EQ_WARN, 1, ERx("ALTER SEQUENCE"));
		}
		break;
	case 126:
#line 1723 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 127:
#line 1736 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
				    EQ_WARN, 1, ERx("ALTER TABLE"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("ALTER TABLE"));
		}
		break;
	case 128:
#line 1746 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 134:
#line 1757 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( ERx("cascade") );
		}
		break;
	case 135:
#line 1761 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( ERx("restrict") );
		}
		break;
	case 136:
#line 1776 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
				    EQ_WARN, 1, ERx("ALTER USER"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("ALTER USER"));
		}
		break;
	case 137:
#line 1786 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 143:
#line 1807 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
				    EQ_WARN, 1, ERx("ALTER PROFILE"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("ALTER PROFILE"));
		}
		break;
	case 144:
#line 1817 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
	    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
	}
		break;
	case 145:
#line 1822 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
	    if (!ESQ_EQSTR(yypvt[-0].s, ERx("profile")))
		er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
				    ERx("ALTER DEFAULT"), ERx("PROFILE") );
	    gr_mechanism( GR_EQSTMT, GR_sSQL, "alter default profile ", NULL );
	}
		break;
	case 148:
#line 1835 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("privileges")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					    ERx("ALTER USER/PROFILE"), ERx("PRIVILEGES") );
		    db_key( ERx("privileges") );
		}
		break;
	case 149:
#line 1849 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 1, 
				  ERx("COMMENT ON"));
	 	    if (eq->eq_flags & EQ_FIPS) 
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1, 
				  ERx("COMMENT ON "));
		}
		break;
	case 152:
#line 1862 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
			gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 153:
#line 1867 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 154:
#line 1872 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 158:
#line 1887 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
				    EQ_WARN, 1, ERx("CREATE ROLE"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("CREATE ROLE"));
		}
		break;
	case 159:
#line 1897 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 160:
#line 1907 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
				    EQ_WARN, 1, ERx("CREATE SEQUENCE"));
		}
		break;
	case 161:
#line 1914 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 182:
#line 1948 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
				    EQ_WARN, 1, ERx("CREATE GROUP"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("CREATE GROUP"));
		}
		break;
	case 183:
#line 1958 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		}
		break;
	case 185:
#line 1971 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
				    EQ_WARN, 1, ERx("CREATE DOMAIN"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("CREATE DOMAIN"));
		}
		break;
	case 192:
#line 1997 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
				    EQ_WARN, 1, ERx("CREATE DBEVENT"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("CREATE DBEVENT"));
		}
		break;
	case 193:
#line 2027 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("CREATE INDEX"));
		}
		break;
	case 194:
#line 2034 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    STprintf( esq->sbuf, ERx("%s %s"), yypvt[-2].s, yypvt[-1].s );
		    gr_mechanism( GR_EQSTMT, GR_sSQL, esq->sbuf, yypvt[-0].s );
		}
		break;
	case 195:
#line 2039 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		}
		break;
	case 205:
#line 2066 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("CREATE INTEGRITY"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("CREATE INTEGRITY"));
		}
		break;
	case 207:
#line 2086 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("CREATE LOCATION"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("CREATE LOCATION"));
		}
		break;
	case 209:
#line 2105 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("CREATE LINK"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("CREATE LINK"));
		}
		break;
	case 211:
#line 2127 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("CREATE PERMIT"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("CREATE PERMIT"));
		}
		break;
	case 212:
#line 2138 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 213:
#line 2142 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 219:
#line 2153 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 220:
#line 2157 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 221:
#line 2161 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 226:
#line 2174 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 227:
#line 2179 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 228:
#line 2184 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_sconst( yypvt[-0].s );
		}
		break;
	case 229:
#line 2190 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 231:
#line 2198 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-7].s );
		    db_key( yypvt[-6].s );
		    db_op( yypvt[-5].s );
		    db_key( yypvt[-4].s );
		    db_key( yypvt[-3].s );
		    db_key( yypvt[-2].s );
		    db_op( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 233:
#line 2211 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-3].s );
		    db_key( yypvt[-2].s );
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 236:
#line 2235 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, NULL );
		    db_key( yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("CREATE RULE"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("CREATE RULE"));
		}
		break;
	case 237:
#line 2248 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-3].s, NULL );
		    db_key( yypvt[-2].s );
		    db_op( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("CREATE RULE"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("CREATE RULE"));
		}
		break;
	case 243:
#line 2270 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 260:
#line 2299 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (ESQ_EQSTR(yypvt[-0].s, ERx("before")))
			db_key( ERx("before") );
		    else if (ESQ_EQSTR(yypvt[-0].s, ERx("after")))
			db_key( ERx("after") );
		    else
		    {
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
				    ERx("CREATE RULE"), ERx("AFTER") );
			db_key( ERx("after") );
		    }
		}
		break;
	case 262:
#line 2314 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = CSR_STATIC;
		}
		break;
	case 276:
#line 2337 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("referencing")))
		    {
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
				    ERx("CREATE RULE"), ERx("REFERENCING") );
		    }
		    db_key( ERx("referencing") );
		}
		break;
	case 277:
#line 2347 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (   !ESQ_EQSTR(yypvt[-0].s, ERx("old"))
			&& !ESQ_EQSTR(yypvt[-0].s, ERx("new"))
		       )
		    {
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
				    ERx("CREATE RULE"), ERx("OLD") );
			db_key( ERx("old") );
		    }	
		    else
		    {
			db_key( yypvt[-0].s );
		    }
		}
		break;
	case 279:
#line 2365 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 280:
#line 2370 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 282:
#line 2376 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 285:
#line 2401 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( IIPROC_CREATE );
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gen_call( IIPROCINIT );

		    db_key( yypvt[-0].s );

		    /* gobble up the tokens until we see the END */
		    sc_tokeat(SC_NEST, tEND, tBEGIN, tCASE, tEND);
		}
		break;
	case 287:
#line 2414 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("CREATE PROCEDURE"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("CREATE PROCEDURE"));
		}
		break;
	case 288:
#line 2424 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("PROCEDURE"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("PROCEDURE"));
		}
		break;
	case 291:
#line 2443 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
 		    if (eq->eq_flags & EQ_COMMON)
 			er_write( E_EQ0504_OPEN_WARN,
 				EQ_WARN, 1, ERx("CREATE SCHEMA") );
		    esq->flag |= ESQ_CRE_SCHEMA;
		}
		break;
	case 293:
#line 2454 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		    if (gr->gr_func == tSET)
		    	erec_setup( 1 );
		}
		break;
	case 303:
#line 2486 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
				EQ_WARN, 1, ERx("CREATE SECURITY_ALARM"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
				EQ_WARN, 1, ERx("CREATE SECURITY_ALARM"));
		}
		break;
	case 307:
#line 2503 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 310:
#line 2511 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (ESQ_EQSTR(yypvt[-0].s, ERx("failure")))
		    {
		        db_key( ERx("failure") );
		    }
		    else
		    {
		        if (!ESQ_EQSTR(yypvt[-0].s, ERx("success")))
			    er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
				ERx("CREATE SECURITY_ALARM"), ERx("SUCCESS") );
		        db_key( ERx("success") );
		    }
		}
		break;
	case 315:
#line 2532 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 316:
#line 2536 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 317:
#line 2540 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 318:
#line 2544 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 319:
#line 2548 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 320:
#line 2552 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 325:
#line 2563 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{db_key( yypvt[-0].s);}
		break;
	case 326:
#line 2567 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			/* FIPS allows ON to be followed only by <tablename> */
			STprintf( esq->sbuf, ERx("%s %s"), ERx("ON"), yypvt[-0].s );
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, esq->sbuf, ERx("CREATE/DROP SECURITY_ALARM") );
		    }
		}
		break;
	case 327:
#line 2577 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			/* FIPS allows ON to be followed only by <tablename> */
			STprintf( esq->sbuf, ERx("%s %s"), ERx("ON"), yypvt[-0].s );
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, esq->sbuf, ERx("CREATE/DROP SECURITY_ALARM") );
		    }
		}
		break;
	case 328:
#line 2587 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS && yypvt[-0].i == 1)
		    {
			/* List of tables is non-FIPS */
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
			    2, ERx("ON <table_list>"), ERx("CREATE/DROP SECURITY_ALARM") );
		    }
		}
		break;
	case 329:
#line 2596 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("installation")))
		    {
			er_write( E_EQ006A_grxBADWORD, EQ_ERROR, 2, yypvt[-0].s,
				ERx("GRANT"));
		    }
		    /* ON CURRENT INSTALLATION is non-FIPS */
		    else if (eq->eq_flags & EQ_FIPS)
		    {
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2,
				ERx("ON CURRENT INSTALLATION"), ERx("CREATE/DROP SECURITY_ALARM") );
		    }
		}
		break;
	case 330:
#line 2613 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{ 
			db_key(yypvt[-0].s);
		}
		break;
	case 331:
#line 2618 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
			db_key(yypvt[-1].s);
			db_key(yypvt[-0].s);
		}
		break;
	case 333:
#line 2630 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
				EQ_WARN, 1, ERx("CREATE SYNONYM"));
		}
		break;
	case 335:
#line 2644 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (esq->flag & ESQ_CRE_SCHEMA)	/* in CREATE SCHEMA */
		    {
			db_key( yypvt[-1].s );
			db_key( yypvt[-0].s );
		    }
		    else
		    {
		    	gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
			if (eq->eq_flags & EQ_FIPS)
			    er_write( E_EQ0505_FIPS_STATMT,
				EQ_WARN, 1, ERx("CREATE TABLE"));
		    }
		    esq->flag &= ~ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 337:
#line 2662 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				    2, ERx("location"), ERx("CREATE TABLE") );
		}
		break;
	case 338:
#line 2678 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* With clause is non-FIPS compliant */
		    if (yypvt[-0].i == 1 && eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
					2, ERx("WITH"), ERx("CREATE TABLE") );
		}
		break;
	case 339:
#line 2686 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
		    {
			if (yypvt[-1].i & CRE_TBLKEY)
			    er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 1,
				ERx("CREATE TABLE ... TABLE_KEY"));
 			else if (yypvt[-1].i & CRE_WDEF)
 			    er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 1, 
 				ERx("CREATE TABLE ... WITH DEFAULT"));
 			else if (yypvt[-1].i & CRE_NDEF)
 			    er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 1, 
 				ERx("CREATE TABLE ... NOT DEFAULT"));
			else if (yypvt[-1].i & CRE_OBJKEY)
			    er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 1,
				ERx("CREATE TABLE ... OBJECT_KEY"));
			else if (yypvt[-1].i & CRE_WSYS)
			    er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 1, 
				ERx("CREATE TABLE ... WITH SYSTEM_MAINTAINED"));
			else if (yypvt[-1].i & CRE_NSYS)
			    er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 1, 
				ERx("CREATE TABLE ... NOT SYSTEM_MAINTAINED"));
			else if (yypvt[-1].i & CRE_CONWTH)
			    er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 1, 
				ERx("CREATE TABLE ... UNIQUE/FKEY WITH ..."));
		    }
		    if (eq->eq_flags & EQ_FIPS)
		    {
			if (yypvt[-1].i & CRE_WNUL)
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2, 
				ERx("WITH NULL"), ERx("CREATE TABLE") );
			else if (yypvt[-1].i & CRE_OBJKEY)
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2,
				ERx("OBJECT_KEY"), ERx("CREATE TABLE") ); 
			else if (yypvt[-1].i & CRE_TBLKEY)
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2,
				ERx("TABLE_KEY"), ERx("CREATE TABLE") ); 
			else if (yypvt[-1].i & CRE_NDEF)
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2, 
				ERx("NOT DEFAULT"), ERx("CREATE TABLE") );
			else if (yypvt[-1].i & CRE_WDEF)
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2, 
				ERx("WITH DEFAULT"), ERx("CREATE TABLE") );
			else if (yypvt[-1].i & CRE_WSYS)
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2, 
			     ERx("WITH SYSTEM_MAINTAINED"),ERx("CREATE TABLE"));
			else if (yypvt[-1].i & CRE_NSYS)
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2, 
			      ERx("NOT SYSTEM_MAINTAINED"),ERx("CREATE TABLE"));
			else if (yypvt[-1].i & CRE_CONWTH)
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2, 
				ERx("UNIQUE/REFERENCES WITH ..."),
				ERx("CREATE TABLE"));
			else if (yypvt[-1].i & CRE_NFIPS)
			    er_write( E_EQ050C_FIPS_DATTYP, EQ_WARN, 1,
			      ERx("CREATE TABLE"));
		    }
		}
		break;
	case 340:
#line 2745 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2, 
			        ERx("AS SELECT"), ERx("CREATE TABLE") );
		}
		break;
	case 341:
#line 2752 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2, 
			        ERx("AS SELECT"), ERx("CREATE TABLE") );
		}
		break;
	case 342:
#line 2783 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;		/* Info on dec_nullfmt */
		}
		break;
	case 343:
#line 2787 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-2].i | yypvt[-0].i;		/* Info on dec_nullfmt */
		}
		break;
	case 344:
#line 2792 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 345:
#line 2796 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-2].i | yypvt[-0].i;
		}
		break;
	case 346:
#line 2801 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 347:
#line 2805 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-1].i;
		}
		break;
	case 348:
#line 2810 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-1].i;
}
		break;
	case 349:
#line 2815 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if ( ESQ_EQSTR(yypvt[-1].s, ERx("table_key")) )
		    {
			yyval.i = CRE_TBLKEY;
		    }
		    else if ( ESQ_EQSTR(yypvt[-1].s, ERx("object_key")) )
		    {
			yyval.i = CRE_OBJKEY;
		    }
		    else if ( !ESQ_EQSTR(yypvt[-1].s, ERx("smallint"))
				&& !ESQ_EQSTR(yypvt[-1].s, ERx("integer"))
				&& !ESQ_EQSTR(yypvt[-1].s, ERx("float")) )
		    {
			yyval.i = CRE_NFIPS;
		    }
		    else
		    {
			yyval.i = yypvt[-0].i;
		    }
		}
		break;
	case 350:
#line 2836 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 351:
#line 2840 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 352:
#line 2844 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 353:
#line 2849 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 354:
#line 2853 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 355:
#line 2857 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 356:
#line 2861 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 357:
#line 2866 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 358:
#line 2870 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if ( !ESQ_EQSTR(yypvt[-4].s, ERx("char"))
				&& !ESQ_EQSTR(yypvt[-4].s, ERx("varchar"))
				&& !ESQ_EQSTR(yypvt[-4].s, ERx("float")) )
		    {
			yyval.i = CRE_NFIPS;
		    }
		    else
		    {
		    	yyval.i = yypvt[-0].i;
		    }
		}
		break;
	case 359:
#line 2885 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 362:
#line 2893 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    esq->flag |= ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 363:
#line 2897 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    esq->flag |= ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 367:
#line 2906 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 375:
#line 2923 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 377:
#line 2934 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 378:
#line 2942 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 379:
#line 2951 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 380:
#line 2959 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 381:
#line 2967 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-1].s);
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 382:
#line 2976 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* This clause must be "no action". */
		    if (!ESQ_EQSTR(yypvt[-1].s, ERx("no")) ||
			!ESQ_EQSTR(yypvt[-0].s, ERx("action")))
		     er_write(E_EQ006A_grxBADWORD, EQ_ERROR, 2, yypvt[-1].s,
			ERx("referential action"));

		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-1].s);
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 384:
#line 2994 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 385:
#line 3002 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-1].s);
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 386:
#line 3012 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-1].s);
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 387:
#line 3022 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 388:
#line 3031 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 389:
#line 3040 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 393:
#line 3061 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("CREATE USER"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("CREATE USER"));
		}
		break;
	case 394:
#line 3077 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* UNION views are non-FIPS */
		    if (eq->eq_flags & EQ_FIPS && yypvt[-1].i == 1)
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2, 
			        ERx("UNION"), ERx("CREATE VIEW") );
		}
		break;
	case 396:
#line 3093 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("CREATE PROFILE"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("CREATE PROFILE"));
		}
		break;
	case 397:
#line 3104 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (esq->flag & ESQ_CRE_SCHEMA)	/* in CREATE SCHEMA */
		    {
			db_key( yypvt[-1].s );
			db_key( yypvt[-0].s );
		    }
		    else
		    {
		        gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
			if (eq->eq_flags & EQ_FIPS)
			    er_write( E_EQ0505_FIPS_STATMT,
				EQ_WARN, 1, ERx("CREATE VIEW"));
		    }
		}
		break;
	case 399:
#line 3130 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-3].s, NULL );
		    db_key( yypvt[-2].s );
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
			    EQ_WARN, 1, ERx("DECLARE GLOBAL TEMPORARY TABLE") );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
			    EQ_WARN, 1, ERx("DECLARE GLOBAL TEMPORARY TABLE") );
		}
		break;
	case 400:
#line 3144 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    STpolycat(3, yypvt[-2].s, yypvt[-1].s, yypvt[-0].s, session_temp);
		    yyval.s = session_temp;
		}
		break;
	case 401:
#line 3149 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 402:
#line 3154 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 403:
#line 3159 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-2].s );
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 404:
#line 3178 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_flag &= ~GR_NOSYNC;	/* error output ok now */
		    if (yypvt[-2].i)
			gr_mechanism(GR_EQSTMT, GR_sREPEAT, ERx("repeat"),
					ERx("delete"));
		    else
			gr_mechanism( GR_EQSTMT, GR_sSQL, ERx("delete"), NULL );
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );		/* owner.tablename */
		}
		break;
	case 405:
#line 3189 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_flag &= ~GR_NOSYNC;	/* error output ok now */
		    if (yypvt[-3].i)
			gr_mechanism(GR_EQSTMT, GR_sREPEAT, ERx("repeat"),
					ERx("delete"));
		    else
			gr_mechanism( GR_EQSTMT, GR_sSQL, ERx("delete"), NULL );
		    db_key( yypvt[-2].s );
		    db_key( yypvt[-1].s );		/* owner.tablename */
		    db_key( yypvt[-0].s );
		}
		break;
	case 407:
#line 3203 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_flag &= ~GR_NOSYNC;	/* error output ok now */
		    if (yypvt[-3].i)
			gr_mechanism(GR_EQSTMT, GR_sREPEAT, ERx("repeat"),
					ERx("delete"));
		    else
			gr_mechanism( GR_EQSTMT, GR_sSQL, ERx("delete"), NULL );
		    db_key( yypvt[-2].s );
		    db_key( yypvt[-1].s );	/* owner.tablename */
		    db_key( yypvt[-0].s );
		}
		break;
	case 409:
#line 3217 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* gr->gr_flag &= ~GR_NOSYNC;*/ /* error output ok now */
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, ERx("delete"),
					yypvt[-4].s );	/* owner.tablename */
		    esq_init();
		    ecs_delete( yypvt[-0].s, yypvt[-4].s, gr->gr_sym != (SYM *)0 );
		    if( yypvt[-6].i )
			er_write( E_EQ0423_csBADREPEAT, EQ_ERROR, 1,
								ERx("DELETE") );
		}
		break;
	case 410:
#line 3229 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_flag |= GR_NOSYNC;	/* no error output yet */
		    yyval.i = 0;
		}
		break;
	case 411:
#line 3234 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_flag |= GR_NOSYNC;	/* no error output yet */
		    if (eq->eq_flags & EQ_FIPS) 
			    er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1, 
			        ERx("REPEATED DELETE") );
		    yyval.i = 1;
		}
		break;
	case 412:
#line 3244 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    er_write( E_EQ0070_grMISSWORD, EQ_ERROR, 2, ERx("FROM"),
								ERx("DELETE") );
		    yyval.s = ERx("from");
		}
		break;
	case 414:
#line 3252 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 415:
#line 3258 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    STprintf( esq->sbuf, ERx("%s.%s"), yypvt[-2].s, yypvt[-0].s );
		    yyval.s = esq->sbuf;
		}
		break;
	case 416:
#line 3264 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    STprintf( esq->sbuf, ERx("%s.%s"), yypvt[-2].s, yypvt[-0].s );
		    yyval.s = esq->sbuf;
		}
		break;
	case 418:
#line 3271 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if ( gr->gr_flag & GR_REPEAT )
		    {
			db_op( ERx("...SYNTAX ERROR") );
			er_write( E_EQ0378_repWHERE, EQ_ERROR, 0 );
		    }
		}
		break;
	case 419:
#line 3285 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
				    EQ_WARN, 1, ERx("DISABLE SECURITY_AUDIT"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
				    EQ_WARN, 1, ERx("DISABLE SECURITY_AUDIT"));
		}
		break;
	case 420:
#line 3295 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 421:
#line 3304 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
				    EQ_WARN, 1, ERx("ENABLE SECURITY_AUDIT"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
				    EQ_WARN, 1, ERx("ENABLE SECURITY_AUDIT"));
		}
		break;
	case 422:
#line 3314 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 423:
#line 3319 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 424:
#line 3323 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 425:
#line 3327 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 426:
#line 3331 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 427:
#line 3335 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 430:
#line 3354 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  ret_no;

		    gr->gr_flag |= GR_NOSYNC;
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-0].s, NULL );
		    if ((ret_no = lbl_gen(LBLmRETLOOP, TRUE)) == LBL_NOLEV)
			er_write( E_EQ0058_grINLOOP, EQ_ERROR, 2, yypvt[-0].s,
								ERx("SELECT") );
		    else
		    {
			gen_call( IIBREAK );
			gen_goto( G_IF, L_RETEND, 
				  lbl_value(LBLmRETLOOP, ret_no) );
		    }
		    if (eq->eq_flags & EQ_FIPS) 
			    er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1, 
			        ERx("ENDSELECT") );
		    yyval.i = 1;
		}
		break;
	case 431:
#line 3384 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4 ret_no;

		    gr->gr_flag |= GR_NOSYNC;
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-0].s, NULL );
		    if ((ret_no = lbl_gen(LBLmEXECLOOP, TRUE)) == LBL_NOLEV)
			er_write( E_EQ0058_grINLOOP, EQ_ERROR, 2, yypvt[-0].s,
							ERx("EXECUTE PROCEDURE") );
		    else
		    {
			gen_call( IIBREAK );
			gen_goto( G_IF, L_RETEND, 
				  lbl_value(LBLmEXECLOOP, ret_no) );
		    }
		    if (eq->eq_flags & EQ_FIPS) 
			    er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1, 
			        ERx("ENDEXECUTE") );
		    yyval.i = 1;
		}
		break;
	case 433:
#line 3412 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 434:
#line 3416 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-1].s, yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS) 
			    er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1, 
			        ERx("REPEATED INSERT") );
		}
		break;
	case 435:
#line 3424 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 439:
#line 3434 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		    erec_setup( 1 );
		}
		break;
	case 442:
#line 3451 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-0].i > 1)
			er_write( E_EQ0132_sqILLSTRUCTEXPR, EQ_ERROR, 0 );
		}
		break;
	case 446:
#line 3463 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		  /* return the max num of struct elems on one side */
		    yyval.i = (yypvt[-2].i > yypvt[-0].i) ? yypvt[-2].i : yypvt[-0].i;
		}
		break;
	case 447:
#line 3468 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
 		  /* return the max num of struct elems on one side */
 		    yyval.i = (yypvt[-4].i > yypvt[-1].i) ? yypvt[-4].i : yypvt[-1].i;
 		}
		break;
	case 448:
#line 3473 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-1].i;	/* Might be a structure */
		}
		break;
	case 449:
#line 3477 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;	/* Might be a structure */
		}
		break;
	case 450:
#line 3481 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;	/* Might be a structure */
		}
		break;
	case 456:
#line 3493 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = erec_vars();
		    erec_use( IISETDOM, gr->gr_flag & GR_REPEAT, ERx(",") );
		    erec_setup( 1 );		/* end of variable */
		    if ( (gr->gr_type == T_STRUCT) && (eq->eq_flags & EQ_FIPS) )
			er_write( E_EQ050D_FIPS_VAR, EQ_WARN, 0 );
		}
		break;
	case 457:
#line 3501 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;		/* Not a structure */
		}
		break;
	case 458:
#line 3505 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    erec_use( IISETDOM, gr->gr_flag & GR_REPEAT, ERx(",") );
		    erec_setup( 1 );
		}
		break;
	case 460:
#line 3513 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (   !ESQ_EQSTR(yypvt[-0].s, ERx("dbmsinfo")) 
		        && eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0510_OPEN_FUNC, EQ_WARN, 1, yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS) 
			er_write( E_EQ0509_FIPS_FUNC, EQ_WARN, 1, yypvt[-0].s );
		}
		break;
	case 462:
#line 3522 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    er_write( E_EQ0129_sqVARCOLON, EQ_ERROR, 0 );
		    YYERROR;
		}
		break;
	case 464:
#line 3529 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-1].s;
		}
		break;
	case 465:
#line 3533 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 466:
#line 3537 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-4].s;
		}
		break;
	case 467:
#line 3542 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-6].s;
		}
		break;
	case 469:
#line 3560 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    gr->gr_func = tUPDATE;
		}
		break;
	case 470:
#line 3565 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-1].s, yypvt[-0].s );
		    gr->gr_func = tUPDATE;
		    if (eq->eq_flags & EQ_FIPS) 
			    er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1, 
			        ERx("REPEATED UPDATE") );
		}
		break;
	case 473:
#line 3579 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_flag & GR_REPEAT)
		    {
			/*
			** Disallow repeated cursor, but leave as regular
			** query (for generation).
			*/
			er_write( E_EQ0423_csBADREPEAT, EQ_ERROR, 1,
								ERx("UPDATE") );
		    }
		    else if (yypvt[-3].i != CSR_NO_FROM)
		    {
			/*
			** Disallow FROM clause 
			*/
			er_write( E_EQ0429_csBADFROM, EQ_ERROR, 1, 
								ERx("UPDATE") );
		    }	
		    else			
		    {
			db_send();
			ecs_colrslv( TRUE );	    /* check all column names */
  						    /* check legality */
			ecs_replace( TRUE, yypvt[-0].s, gr->gr_sym != (SYM *)0 ); 
						    /* generate IIcsERplace */
			ecs_replace( FALSE, yypvt[-0].s, gr->gr_sym != (SYM *)0 );
			gr->gr_flag |= GR_NOSYNC;
			if (yypvt[-4].s)
			    er_write( E_EQ0424_csILLCORRELNAME, EQ_ERROR, 2,
						yypvt[-4].s, ERx("UPDATE") );
		    }
		    esq->flag &= ~ESQ_CURSOR;
		}
		break;
	case 474:
#line 3616 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    ecs_colrslv( FALSE );	/* toss all the column names */
						/* check for FROM clause */
		    if ((yypvt[-3].i != CSR_NO_FROM) && (eq->eq_flags & EQ_COMMON))
			er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 1, 
				ERx("UPDATE ... FROM"));
		    if (yypvt[-3].i != CSR_NO_FROM && eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2, 
			        ERx("FROM"), ERx("UPDATE") );
		}
		break;
	case 476:
#line 3631 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    esq->flag |= ESQ_CURSOR;
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 480:
#line 3699 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-0].i)
		    {
		      /* SELECT LOOP - gen WHILE */
			/* Close retrieve loop */
			gen_loop( G_CLOSE, L_RETBEG, L_RETFLUSH,
				    lbl_current(LBL_RET), IINEXTGET, C_0, 0 );
			gen_call( IIFLUSH );
			gen_label( G_NOLOOP, L_RETEND, lbl_current(LBL_RET) );

                        /* b114309 */
                        gr->gr_nlid = (char *)0;
                        gr->gr_nlsym = (SYM *)0;

			gr->gr_flag &= ~GR_RETRIEVE;/* Done with SELECT loop */
		    } else
		    {
		      /* SINGLETON SELECT - gen IF */
			if (eq->eq_flags & EQ_CHREOS)
			    esq_eoscall(IImodNOCEOS);
			/* Open retrieve loop */
			/* gen_if( G_OPEN, IIERRTEST, C_EQ, 0 ); */
			gen_if( G_OPEN, IINEXTGET, C_NOTEQ, 0 );
			if (eq->eq_flags & EQ_CHRPAD)
			    esq_sqmods(IImodCPAD);
			/* Generate variable retrieval in loop */
			ret_close();
			if (eq->eq_flags & EQ_CHRPAD)
			    esq_sqmods(IImodNOCPAD);
			gen_if( G_CLOSE, IINEXTGET, C_0, 0 );
			gen_call( IISQFLUSH );
			/* gen_if( G_CLOSE, IIERRTEST, C_0, 0 ); */

                        /* b114309 */
                        gr->gr_nlid = (char *)0;
                        gr->gr_nlsym = (SYM *)0;
		    }
		}
		break;
	case 482:
#line 3743 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* don't set until after call to gr_mechanism */ 
		}
		break;
	case 483:
#line 3748 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    esq->flag |= ESQ_REG_SELECT;
		    esl_query( ESL_START_QUERY );
		    /* special case for singleton select */
		    if (eq->eq_flags & EQ_ENDIF)
		    {
			gr->gr_flag |= GR_ENDIF;
			eq->eq_flags &= ~EQ_ENDIF;
		    }		
		}
		break;
	case 484:
#line 3760 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-1].s, yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS) 
			    er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1, 
			        ERx("REPEATED SELECT") );
		}
		break;
	case 487:
#line 3771 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 490:
#line 3778 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* DISTINCT makes a cursor Read-Only */
		    if (ESQ_DCLCSR(esq))
			ecs_csrset( NULL, ECS_UNIQUE );
		}
		break;
	case 491:
#line 3785 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 492:
#line 3790 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s ); /* db_key 'cause we want spaces around it */
		}
		break;
	case 496:
#line 3801 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2,
			    ERx("<= result_column_name>"), ERx("SELECT") );
		}
		break;
	case 497:
#line 3807 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			if (yypvt[-0].s)
			{
			    if (*yypvt[-0].s)		/* name */
			    {
				if (esq->flag & ESQ_UNION_SEL)
				    /* check and flag */
				    _VOID_ eqck_tgt( 3, 3 );
				else
				    _VOID_ eqck_tgt( 2, 3 );	/* fill */
			    }
			    else		/* expression */
			    {
				if (esq->flag & ESQ_UNION_SEL)
				    /* check and flag */
				    _VOID_ eqck_tgt( 3, 2 );
				else
				    _VOID_ eqck_tgt( 2, 2 );	/* fill */
			    }
			}
			else			/* constant */
			{
			    if (esq->flag & ESQ_UNION_SEL)
				/* check and flag */
				_VOID_ eqck_tgt( 3, 1 );
			    else
				_VOID_ eqck_tgt( 2, 1 );	/* fill */
			}
		    }
		}
		break;
	case 498:
#line 3840 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2,
			    ERx("<result_column_name AS>"), ERx("SELECT") );
		}
		break;
	case 499:
#line 3846 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			if (esq->flag & ESQ_UNION_SEL)
			    _VOID_ eqck_tgt( 3, 2 );	/* check and flag */
			else
			    _VOID_ eqck_tgt( 2, 2 );	/* fill */
		    }
		}
		break;
	default:
		    goto label_500;
	}

label_500:
	switch (yym) {
	case 506:
#line 3878 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_flag & GR_REPEAT)
		    {
			gr->gr_flag &= ~GR_REPEAT;
			/* Dump repeated query */
			rep_close( IIRETINIT, NULL, TRUE );
		    }
		    else
		    {
			db_close( IIRETINIT );
		    }
		    /* Add all the INTO variables and indicators */
		    if ((esq->flag & ESQ_DESC_SEL) == 0)
			erec_use( IIRETDOM, gr->gr_flag & GR_REPEAT, NULL );

		    if (esq->flag & ESQ_REG_SELECT)
		    {
			esl_query( ESL_SNGL_STOP_QUERY );   /* Dump the query */
			esq->flag &= ~ESQ_REG_SELECT;
		    }
		    yyval.i = 0;
		}
		break;
	case 507:
#line 3901 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 508:
#line 3907 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4		cur_ret;

		    if (eq->eq_flags & EQ_CHREOS)
			 esq_eoscall(IImodNOCEOS);
		    if (gr->gr_flag & GR_REPEAT)
		    {
			gr->gr_flag &= ~GR_REPEAT;
			/* Dump repeated query */
			rep_close( IIRETINIT, NULL, FALSE );
		    } else
			db_close( IIRETINIT );

		    /* Add all the INTO variables and indicators */
		    if ((esq->flag & ESQ_DESC_SEL) == 0)
			erec_use( IIRETDOM, gr->gr_flag & GR_REPEAT, NULL );

		    if (esq->flag & ESQ_REG_SELECT)
		    {
			esl_query( ESL_BULK_STOP_QUERY );   /* Dump the query */
			esq->flag &= ~ESQ_REG_SELECT;
		    }

		    /* Here's generic block-begin code */
		    _VOID_ lbl_next( LBL_RET );
		    lbl_enter_block();
		    lbl_set_mode( LBLmRETLOOP );
		    lbl_adset( LBL_NOLEV, gr->gr_adjective );
		    gr->gr_mode = 0;
		    gr->gr_adjective = 0;
		    gr->gr_flag &= ~(GR_SQL|GR_ESQL);

		    /* Here's SELECT-specific block-begin code */
		    cur_ret = lbl_current( LBL_RET );
		    gen_if_goto( IIERRTEST, C_NOTEQ, 0, L_RETEND, cur_ret );
		    /* Open retrieve loop */
		    gen_label( G_LOOP, L_RETBEG, cur_ret );
		    gen_loop( G_OPEN, L_RETBEG, L_RETFLUSH, cur_ret,
			IINEXTGET, C_NOTEQ, 0 );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		    if (esq->flag & ESQ_DESC_SEL)
			/* Descriptor retrieval */
			erec_desc( NULL );
		    else
			ret_close();		/* Variable retrieval */
		    if (eq->eq_flags & EQ_CHRPAD)
			 esq_sqmods(IImodNOCPAD);
		    gen_if_goto( IIERRTEST, C_NOTEQ, 0, L_RETBEG, cur_ret );
		    gr->gr_flag &= ~GR_SQL;
		    gr->gr_flag |= GR_RETRIEVE; /* Prevent nesting queries */
		    if (eq->eq_flags & EQ_FIPS) 
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2, 
			        ERx("BEGIN"), ERx("SELECT") );
		}
		break;
	case 509:
#line 3965 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    lbl_exit_block();
		}
		break;
	case 510:
#line 3985 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* error must have INTO clause before FROM clause */
		    er_write( E_EQ0070_grMISSWORD, EQ_ERROR, 2, ERx("INTO"),
							ERx("SELECT/FETCH") );
		    YYERROR;
		}
		break;
	case 511:
#line 3992 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    erec_setup( 0 );	/* Done with list */
		}
		break;
	case 512:
#line 3997 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    erec_setup( 1 );	/* Initialize struct indicators stuff */
		}
		break;
	case 515:
#line 4005 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    erec_setup( 0 );	/* Done with one element */
		}
		break;
	case 516:
#line 4010 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if ( (gr->gr_type == T_STRUCT) && (eq->eq_flags & EQ_FIPS) )
			er_write( E_EQ050D_FIPS_VAR, EQ_WARN, 0 );
		}
		break;
	case 518:
#line 4017 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
                    char        *argname = NULL;

                    if ((  (argname = str_add( STRNULL, id_getname())) != NULL)
			&& (*argname != EOS))
                        id_free();
                    else
                        argname = ERx("0");

                    if (gr->gr_func == tUPDATE || gr->gr_func == tEXECUTE)
                    {
                        gr->gr_id = yypvt[-2].s;
                        yyval.s = argname;
                    }
                    else
                    {
                        erec_mem_add( yypvt[-2].s, (SYM *) 0, T_HDLR );
                        erec_arg_add( argname );
			yyval.s = yypvt[-2].s;
                    }
                }
		break;
	case 519:
#line 4041 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    sc_eat(id_add,SC_STRIP|SC_NEST|SC_SEEN,ERx(")"), '(', ')');
		}
		break;
	case 520:
#line 4056 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-3].i == CSR_NO_FROM)
		    {
			/*
			** If there isn't a FROM clause, then the WHERE,
			** GROUP BY, and HAVING clauses are illegal.
			** We accept them in order to give a good error message.
			** For DECLARE CURSOR, the GROUP BY and HAVING clauses
			** will mark the cursor as read-only, but that's okay.
			*/
			yyval.i = CSR_NO_FROM;
			if (eq->eq_flags & EQ_FIPS) 
			    er_write( E_EQ0507_FIPS_NOTOP, EQ_WARN, 2, 
			        ERx("FROM"), ERx("SELECT") );
			else if (yypvt[-2].i != CSR_NO_WHERE || yypvt[-1].i || yypvt[-0].i)
			    er_write( E_EQ0133_sqNOFROM, EQ_ERROR, 0 );
		    } else
		    {
		      /* No WHERE clause is same as static clause after this */
			if (yypvt[-2].i == CSR_NO_WHERE)
			    yyval.i = CSR_STATIC;
			else
			    yyval.i = yypvt[-2].i;
		    }
		}
		break;
	case 521:
#line 4096 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-0].i & FRM_JOIN)	
		    {
		        if (eq->eq_flags & EQ_COMMON)
			    er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 1,
				ERx("SELECT/UPDATE ... FROM ... JOIN"));
		        if (eq->eq_flags & EQ_FIPS) 
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2, 
			        ERx("FROM ... JOIN"), ERx("SELECT/UPDATE") );
		    }
		    yyval.i = CSR_STATIC;
		}
		break;
	case 522:
#line 4109 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = CSR_NO_FROM;
		}
		break;
	case 523:
#line 4114 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 524:
#line 4118 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-2].i | yypvt[-0].i;
		}
		break;
	case 525:
#line 4123 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 526:
#line 4127 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = FRM_JOIN;
		}
		break;
	case 527:
#line 4131 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-2].i;
		}
		break;
	case 529:
#line 4137 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-1].s;
		}
		break;
	case 534:
#line 4148 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = FRM_OK;
		}
		break;
	case 535:
#line 4152 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-1].i;
		}
		break;
	case 536:
#line 4157 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 537:
#line 4161 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 538:
#line 4166 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 539:
#line 4171 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 540:
#line 4176 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 546:
#line 4189 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_addtab( NULL, yypvt[-0].s );
		    yyval.s = (char *)0;
		}
		break;
	case 547:
#line 4195 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_addtab( NULL, yypvt[-2].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 548:
#line 4201 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    char tbuf[100];     /* buffer to hold owner.tablename */

		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_addtab( NULL, STpolycat(3,yypvt[-2].s,yypvt[-1].s,yypvt[-0].s,tbuf) );
		    yyval.s = (char *)0;
		}
		break;
	case 549:
#line 4209 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    char tbuf[100];     /* buffer to hold owner.tablename */
		
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
		        ecs_addtab( NULL, STpolycat(3,yypvt[-4].s,yypvt[-3].s,yypvt[-2].s,tbuf) );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 550:
#line 4218 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    char tbuf[100];     /* buffer to hold owner.tablename */

		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_addtab( NULL, STpolycat(3,yypvt[-2].s,yypvt[-1].s,yypvt[-0].s,tbuf) );
		    yyval.s = (char *)0;
		}
		break;
	case 551:
#line 4227 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    char tbuf[100];     /* buffer to hold owner.tablename */
		
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
		        ecs_addtab( NULL, STpolycat(3,yypvt[-4].s,yypvt[-3].s,yypvt[-2].s,tbuf) );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 552:
#line 4235 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_addtab( NULL, yypvt[-0].s );
		    yyval.s = (char *)0;
		}
		break;
	case 553:
#line 4241 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
		        ecs_addtab( NULL, yypvt[-2].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 558:
#line 4261 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = CSR_STATIC;
		}
		break;
	case 559:
#line 4265 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = CSR_WHERE_VAR;
		    if ( gr->gr_flag & GR_REPEAT )
		    {
			db_op( ERx("...SYNTAX ERROR") );
			er_write( E_EQ0378_repWHERE, EQ_ERROR, 0 );
		    }
		}
		break;
	case 560:
#line 4274 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = CSR_NO_WHERE;
		}
		break;
	case 561:
#line 4280 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* GROUP BY makes a cursor Read-Only */
		    if (ESQ_DCLCSR(esq))
			ecs_csrset( NULL, ECS_GROUP );
		    yyval.i = 1;
		}
		break;
	case 562:
#line 4287 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 563:
#line 4292 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 564:
#line 4296 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 565:
#line 4301 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 566:
#line 4305 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 567:
#line 4311 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 568:
#line 4316 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 569:
#line 4320 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 570:
#line 4325 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 571:
#line 4330 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 572:
#line 4335 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (esq->flag & ESQ_CURSOR)
		    {
			ecs_colupd( NULL, yypvt[-0].s, ECS_CHK );
			if (gr->gr_func == tUPDATE)
			    ecs_coladd( yypvt[-0].s ); /* might be a cursor */
		    }
		}
		break;
	case 573:
#line 4346 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* HAVING makes a cursor Read-Only */
		    if (ESQ_DCLCSR(esq))
			ecs_csrset( NULL, ECS_HAVING );
		    yyval.i = 1;
		}
		break;
	case 574:
#line 4353 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 587:
#line 4379 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
	if (!ESQ_EQSTR(yypvt[-0].s, ERx("first")))
	    er_write( E_EQ0244_yySYNWRD, EQ_ERROR, 1, yypvt[-0].s );
    }
		break;
	case 588:
#line 4384 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
	db_key( yypvt[-0].s );
    }
		break;
	case 589:
#line 4389 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
	db_key( yypvt[-0].s );
    }
		break;
	case 590:
#line 4393 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
	db_key( yypvt[-0].s );
    }
		break;
	case 593:
#line 4406 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;	/* UNION info for CREATE VIEW rule */
		}
		break;
	case 594:
#line 4410 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 1;		/* UNION info for CREATE VIEW rule */
		}
		break;
	case 595:
#line 4415 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;		/* UNION info used by CREATE VIEW */
		}
		break;
	case 596:
#line 4419 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-2].i;	/* UNION info used by CREATE VIEW */
		}
		break;
	case 599:
#line 4446 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-4].i == CSR_NO_FROM)
		    {
			/*
			** If there isn't a FROM clause, then the FOR UPDATE
			** clause is illegal.
			** We accept it in order to give a good error message.
			*/
			if (yypvt[-0].i != CSR_STATIC)
			    er_write( E_EQ0133_sqNOFROM, EQ_ERROR, 0 );
			yyval.i = yypvt[-4].i;
		    } else
		    {
			yyval.i = yypvt[-4].i | yypvt[-0].i;
		    }
		}
		break;
	case 600:
#line 4471 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 601:
#line 4475 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-2].i == CSR_NO_FROM || yypvt[-0].i == CSR_NO_FROM)
			yyval.i = CSR_NO_FROM;
		    else
			yyval.i = yypvt[-2].i | yypvt[-0].i; /* MRW: probably just return $3.i */
		    ecs_csrset( NULL, ECS_UNION );
		}
		break;
	case 602:
#line 4491 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 603:
#line 4495 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-1].i;	/* MRW: probably can just return 0 */
		}
		break;
	case 605:
#line 4502 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 606:
#line 4506 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_flag |= GR_REPEAT;
		    rep_begin( ERx("repeat"), ecs_id_curcsr() );
		    ecs_csrset( NULL, ECS_REPEAT );
		    db_key( yypvt[-0].s );
		    /* 
		    ** No need for FIPS message because ecs_csrset will
		    ** give a general message.
		    */
		}
		break;
	case 609:
#line 4521 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s ); /* db_key 'cause we want spaces around it */
		    _VOID_ ecs_colupd( NULL, ERx(""), ECS_ADD|ECS_ISWILD );
		}
		break;
	case 613:
#line 4539 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-0].s)
		    {
			if (*yypvt[-0].s)
			    _VOID_ ecs_colupd( NULL, yypvt[-2].s, ECS_ADD );
			else
			    _VOID_ ecs_colupd( NULL, yypvt[-2].s, ECS_ADD|ECS_EXPR );
		    } else
			_VOID_ ecs_colupd( NULL, yypvt[-2].s, ECS_ADD|ECS_CONSTANT );

		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2,
			    ERx("<= result_column_name>"), ERx("SELECT") );
		}
		break;
	case 614:
#line 4554 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-0].s)
		    {
			if (*yypvt[-0].s)
			{
			    _VOID_ ecs_colupd( NULL, yypvt[-0].s, ECS_ADD );
			    if (eq->eq_flags & EQ_FIPS)
			    {
				if (esq->flag & ESQ_UNION_SEL)
			    	    /* check and flag */
				    _VOID_ eqck_tgt( 3, 3 );
				else
				    _VOID_ eqck_tgt( 2, 3 );	/* fill */
			    }
			}
			else
			{
			    _VOID_ ecs_colupd( NULL, yypvt[-0].s, ECS_ADD|ECS_EXPR );
			    if (eq->eq_flags & EQ_FIPS)
			    {
				if (esq->flag & ESQ_UNION_SEL)
			    	    /* check and flag */
				    _VOID_ eqck_tgt( 3, 2 );
				else
				    _VOID_ eqck_tgt( 2, 2 );	/* fill */
			    }
			}
		    } else
		    {
			_VOID_ ecs_colupd( NULL, ERx(""),
						ECS_ADD|ECS_CONSTANT );
			if (eq->eq_flags & EQ_FIPS)
			{
			    if (esq->flag & ESQ_UNION_SEL)
			        /* check and flag */
				_VOID_ eqck_tgt( 3, 1 );
			    else
				_VOID_ eqck_tgt( 2, 1 );	/* fill */
			}
		    }
		}
		break;
	case 615:
#line 4596 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-2].s)
		    {
			if (*yypvt[-2].s)
			    _VOID_ ecs_colupd( NULL, yypvt[-0].s, ECS_ADD );
			else
			    _VOID_ ecs_colupd( NULL, yypvt[-0].s, ECS_ADD|ECS_EXPR );
		    } else
			_VOID_ ecs_colupd( NULL, yypvt[-0].s, ECS_ADD|ECS_CONSTANT );

		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2,
			    ERx("<result_column_name AS>"), ERx("SELECT") );
		}
		break;
	case 616:
#line 4611 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    STprintf( esq->sbuf, ERx("%s.%s"), yypvt[-2].s, yypvt[-0].s );
		    _VOID_ ecs_colupd( NULL, str_add(STRNULL, esq->sbuf),
				ECS_ADD|ECS_ISWILD );
		    if (eq->eq_flags & EQ_FIPS)
		    {
			if (esq->flag & ESQ_UNION_SEL)
			    _VOID_ eqck_tgt( 3, 2 );	/* check and flag */
			else
			    _VOID_ eqck_tgt( 2, 2 );	/* fill */
		    }
		}
		break;
	case 618:
#line 4626 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    ecs_csrset( NULL, ECS_SORTED );
		}
		break;
	case 621:
#line 4635 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = CSR_STATIC;
		}
		break;
	case 622:
#line 4639 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
		    {
			if (yypvt[-3].i == ECS_DIRECTU) 
			    er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 1, 
				ERx("DECLARE CURSOR ... FOR DIRECT UPDATE"));
			else if (yypvt[-3].i == ECS_DEFERRU)
			    er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 1, 
				ERx("DECLARE CURSOR ... FOR DEFERRED UPDATE"));
		    }
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2,
				ERx("FOR UPDATE"), ERx("DECLARE CURSOR") );
		    ecs_csrset( NULL, yypvt[-3].i );
		    yyval.i = CSR_UPDATE_LIST;
		}
		break;
	case 623:
#line 4657 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
		    {
			if (yypvt[-1].i == ECS_DIRECTU) 
			    er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 1, 
				ERx("DECLARE CURSOR ... FOR DIRECT UPDATE"));
			else if (yypvt[-1].i == ECS_DEFERRU)
			    er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 1, 
				ERx("DECLARE CURSOR ... FOR DEFERRED UPDATE"));
		    }
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2,
				ERx("FOR UPDATE"), ERx("DECLARE CURSOR") );
		    ecs_csrset( NULL, yypvt[-1].i );
		    yyval.i = CSR_UPDATE;
		}
		break;
	case 628:
#line 4691 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    ecs_addtab( NULL, yypvt[-1].s );
		}
		break;
	case 629:
#line 4696 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4		stat;		/* status of this column */
		    i4		is_wild;	/* csr had var|* in tgt list? */

		    /*
		    ** Variables in the update list are not allowed in ESQL.
		    */

		    is_wild = ecs_colupd(NULL, ERx(""), ECS_CHK|ECS_ISWILD) 
				    & ECS_EXISTS;
		    stat = ecs_colupd( NULL, yypvt[-0].s, ECS_CHK );

		  /* Column not mentioned in target list? */
		    if (!(stat & ECS_EXISTS))
		    {
#if 0
		      /* No '*'s in target list means an error. */
			if (!is_wild)
			    er_write( E_EQ0406_csUPDXIST, EQ_ERROR, 2, yypvt[-0].s,
								ecs_namecsr() );
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
		break;
	case 630:
#line 4735 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    er_write( E_EQ0411_csUPDALL, EQ_ERROR, 1, ecs_namecsr() );
		}
		break;
	case 631:
#line 4740 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = ECS_UPDATE;
		}
		break;
	case 632:
#line 4744 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (ESQ_EQSTR(yypvt[-0].s, ERx("direct")))
			yyval.i = ECS_DIRECTU;
		    else if (ESQ_EQSTR(yypvt[-0].s, ERx("deferred")))
			yyval.i = ECS_DEFERRU;
		    else
		    {
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
						ERx("UPDATE"), ERx("DIRECT") );
			yyval.i = ECS_DIRECTU;
		    }
		}
		break;
	case 633:
#line 4758 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 647:
#line 4786 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-0].i == 1)
		    {
		    	if (eq->eq_flags & EQ_FIPS)
				er_write( E_EQ0511_FIPS_SUBCLAUSE, EQ_WARN,0);
		    	if (eq->eq_flags & EQ_COMMON)
				er_write( E_EQ0512_OPEN_CLAUSE, EQ_WARN,2,
				ERx("GROUP BY/HAVING"), ERx("SELECT-Subqry"));
		    }
	        }
		break;
	case 648:
#line 4797 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-3].i == 1)
		    {
		    	if (eq->eq_flags & EQ_FIPS) 
				er_write( E_EQ0511_FIPS_SUBCLAUSE, EQ_WARN,0);
		    	if (eq->eq_flags & EQ_COMMON)
				er_write( E_EQ0512_OPEN_CLAUSE, EQ_WARN,2,
				ERx("GROUP BY/HAVING"), ERx("SELECT-Subqry"));
		    }
		}
		break;
	case 649:
#line 4809 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 650:
#line 4813 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 651:
#line 4817 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 652:
#line 4821 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_op( yypvt[-0].s );
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("<>")))
		    {
		        if (eq->eq_flags & EQ_FIPS)
			    er_write( E_EQ0508_FIPS_OP, EQ_WARN, 1, yypvt[-0].s );
			if (eq->eq_flags & EQ_COMMON)
			    er_write( E_EQ0513_OPEN_OP, EQ_WARN, 1, yypvt[-0].s );
		    }		
		}
		break;
	case 653:
#line 4832 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 654:
#line 4836 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 655:
#line 4840 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 656:
#line 4844 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 657:
#line 4851 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 658:
#line 4855 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 659:
#line 4859 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 660:
#line 4863 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 662:
#line 4882 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym)
		    {
			db_var( DB_STRING, gr->gr_sym, gr->gr_id,
				(SYM *)0, (char *)0, (char *)0 );
			if (gr->gr_type != T_CHAR && gr->gr_type != T_WCHAR &&
				gr->gr_type != T_UNDEF)
			    er_write( E_EQ0067_grSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
		    }
		    else	/* Error already from host_setvar */
			db_key( gr->gr_id );
		}
		break;
	case 684:
#line 4938 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 685:
#line 4944 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-1].i;		/* bug #58171 */
		}
		break;
	case 686:
#line 4950 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-3].i == CSR_NO_FROM)
		    {
			if (eq->eq_flags & EQ_FIPS) 
			    er_write( E_EQ0507_FIPS_NOTOP, EQ_WARN, 2, 
			        ERx("FROM"), ERx("SELECT") );
			/*
			** If there isn't a FROM clause, then the WHERE,
			** GROUP BY, and HAVING clauses are illegal.
			** We accept them in order to give a good error message.
			*/
			if (yypvt[-2].i != CSR_NO_WHERE || yypvt[-1].i || yypvt[-0].i)
			    er_write( E_EQ0133_sqNOFROM, EQ_ERROR, 0 );
		    }
		    yyval.i = yypvt[-1].i || yypvt[-0].i;
		}
		break;
	case 691:
#line 4985 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 692:
#line 4989 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 698:
#line 5001 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 699:
#line 5005 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 700:
#line 5009 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 701:
#line 5013 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 702:
#line 5019 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = ERx("");
		    /* functions make a cursor Read-Only */
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_csrset( NULL, ECS_FUNCTION );
		}
		break;
	case 703:
#line 5026 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 710:
#line 5044 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 711:
#line 5048 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 712:
#line 5054 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = NULL;
		}
		break;
	case 713:
#line 5058 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 714:
#line 5062 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (   !ESQ_EQSTR(yypvt[-0].s, ERx("dbmsinfo")) 
		        && eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0510_OPEN_FUNC, EQ_WARN, 1, yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS) 
			er_write( E_EQ0509_FIPS_FUNC, EQ_WARN, 1, yypvt[-0].s );
		    yyval.s = ERx("");
		}
		break;
	case 715:
#line 5071 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 716:
#line 5075 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 717:
#line 5079 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 718:
#line 5083 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 721:
#line 5090 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_hexconst( yypvt[-0].s );
		}
		break;
	case 722:
#line 5094 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_uconst( yypvt[-0].s );
		}
		break;
	case 723:
#line 5098 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 724:
#line 5102 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 726:
#line 5107 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 728:
#line 5112 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 729:
#line 5117 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_dtconst( yypvt[-0].s );
		}
		break;
	case 732:
#line 5125 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("year")) &&
			!ESQ_EQSTR(yypvt[-0].s, ERx("month")) &&
			!ESQ_EQSTR(yypvt[-0].s, ERx("day")) &&
			!ESQ_EQSTR(yypvt[-0].s, ERx("hour")) &&
			!ESQ_EQSTR(yypvt[-0].s, ERx("minute")) &&
			!ESQ_EQSTR(yypvt[-0].s, ERx("second")))
			er_write( E_EQ0233_scILIT, EQ_ERROR, 0);

		    db_key( yypvt[-0].s );
		}
		break;
	case 739:
#line 5146 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("month")) &&
			!ESQ_EQSTR(yypvt[-0].s, ERx("hour")) &&
			!ESQ_EQSTR(yypvt[-0].s, ERx("minute")) &&
			!ESQ_EQSTR(yypvt[-0].s, ERx("second")))
			er_write( E_EQ0233_scILIT, EQ_ERROR, 0);

		    db_key( yypvt[-0].s );
		}
		break;
	case 756:
#line 5182 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0509_FIPS_FUNC, EQ_WARN, 1, ERx("ANY"));
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0510_OPEN_FUNC, EQ_WARN, 1, ERx("ANY"));
		}
		break;
	case 757:
#line 5190 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_op( yypvt[-0].s );
		}
		break;
	case 758:
#line 5196 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_op( yypvt[-0].s );
		}
		break;
	case 759:
#line 5202 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 760:
#line 5206 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 761:
#line 5210 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 762:
#line 5214 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 763:
#line 5220 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 764:
#line 5224 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 767:
#line 5234 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-1].s;
		}
		break;
	case 768:
#line 5238 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 769:
#line 5242 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-4].s;
		}
		break;
	case 770:
#line 5247 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-6].s;
		}
		break;
	case 771:
#line 5251 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-5].s;
		}
		break;
	case 772:
#line 5255 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-7].s;
		}
		break;
	case 773:
#line 5259 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-4].s, ERx("position")))
			er_write( E_EQ006A_grxBADWORD, EQ_ERROR, 2, yypvt[-4].s,
				ERx("query"));
		    yyval.s = yypvt[-4].s;
		}
		break;
	case 774:
#line 5266 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-4].s, ERx("trim")))
			er_write( E_EQ006A_grxBADWORD, EQ_ERROR, 2, yypvt[-4].s,
				ERx("query"));
		    yyval.s = yypvt[-4].s;
		}
		break;
	case 778:
#line 5278 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-1].s;
		}
		break;
	case 779:
#line 5283 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 780:
#line 5288 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 781:
#line 5293 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 782:
#line 5298 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 786:
#line 5309 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 787:
#line 5313 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 794:
#line 5340 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_flag & GR_REPEAT )
			rep_param();
		    db_var( DB_REG, gr->gr_sym, gr->gr_id,
			    gr->gr_nlsym, gr->gr_nlid, (char *)0 );
		    if (gr->gr_flag & GR_REPEAT )
			rep_add( gr->gr_sym, gr->gr_id,
				gr->gr_nlsym, gr->gr_nlid, (char *)0 );
		    if ( (gr->gr_type == T_STRUCT) && (eq->eq_flags & EQ_FIPS) )
			er_write( E_EQ050D_FIPS_VAR, EQ_WARN, 0 );
		}
		break;
	case 795:
#line 5352 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_flag & GR_REPEAT )
			rep_param();
			/* 60688 */
		    if (gr->gr_flag & GR_REPEAT )
			rep_add( gr->gr_sym, gr->gr_id,
				gr->gr_nlsym, gr->gr_nlid, yypvt[-1].s );
                    else
		        db_var( DB_HDLR, gr->gr_sym, gr->gr_id,
			    gr->gr_nlsym, gr->gr_nlid, yypvt[-1].s );
		}
		break;
	case 796:
#line 5371 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_flag & GR_REPEAT )
			rep_param();
		    db_var( DB_NOTRIM, gr->gr_sym, gr->gr_id,
			    gr->gr_nlsym, gr->gr_nlid, (char *)0 );
		    if (gr->gr_flag & GR_REPEAT )
			rep_add( gr->gr_sym, gr->gr_id,
				gr->gr_nlsym, gr->gr_nlid, (char *)0 );
		    if (gr->gr_type != T_CHAR && gr->gr_type != T_UNDEF)
		    {
			er_write( E_EQ0067_grSTRVAR, EQ_ERROR, 1, gr->gr_id );
			gr->gr_sym = (SYM *)0;	/* Will be sent in db_key */
		    }
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0510_OPEN_FUNC, EQ_WARN, 1, yypvt[-4].s );
		    if (eq->eq_flags & EQ_FIPS) 
			er_write( E_EQ0509_FIPS_FUNC, EQ_WARN, 1, yypvt[-4].s );
		}
		break;
	case 797:
#line 5409 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-1].s, yypvt[-0].s );
		    esq_init();
		    ecs_close( yypvt[-0].s, gr->gr_sym != (SYM *)0 );
		}
		break;
	case 798:
#line 5426 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_send();
		    if (eq->eq_flags & EQ_CHREOS) /* Turn EOS check Off */
		        esq_eoscall(IImodNOCEOS);
		    ecs_query( NULL, ECS_STOP_QUERY );
		    if (gr->gr_flag & GR_REPEAT )
		    {
			rep_close( IICSQUERY, ecs_id_curcsr(), FALSE );
			ecs_query( NULL, ECS_END_QUERY );
		    }
		    esq->flag &= ~(ESQ_CURSOR|ESQ_FROM_CHK);
		    esq->flag |= ESQ_NOPRT;
		}
		break;
	case 799:
#line 5441 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		  /* Don't print it */
		    esq->flag |= (ESQ_NOPRT|ESQ_CURSOR
				  |ESQ_DECLCSR|ESQ_FROM_CHK);
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-2].s, yypvt[-1].s );
		    esq->flag &= ~ESQ_NOPRT;
		    ecs_declcsr( yypvt[-1].s, gr->gr_sym );
		    if (yypvt[-0].i > 0)
			ecs_csrset( yypvt[-1].s, yypvt[-0].i );
		    ecs_query( NULL, ECS_START_QUERY );
		}
		break;
	case 800:
#line 5454 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 801:
#line 5468 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 802:
#line 5472 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = ECS_KEYSET;
                }
		break;
	case 803:
#line 5477 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    ecs_csrset( NULL, ECS_DYNAMIC );
		}
		break;
	case 804:
#line 5481 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    ecs_csrset( NULL, ECS_DYNAMIC );
		}
		break;
	case 805:
#line 5485 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /*
		    ** csr_select_sub returns a bit mask:
		    ** CSR_STATIC	(0x00)	WHERE clause static or missing
		    ** CSR_NO_FROM	(0x01)	No FROM clause
		    ** CSR_WHERE_VAR	(0x04)	a WHERE clause in a variable
		    ** CSR_UPDATE	(0x08)	an UPDATE clause
		    ** CSR_UPD_LIST	(0x10)	an UPDATE list in UPDATE clause
		    ** CSR_UPDATE_LIST	(0x18)	UPD_LIST always implies UPDATE
		    */
		    if (yypvt[-0].i & CSR_WHERE_VAR)	/* WHERE clause in a variable */
		    {
			/*
			** If the where clause is in a variable, then the
			** cursor is updatable.	 Additionally, if there wasn't
			** an update clause, then pretend we saw
			** "for update of (string_var)" since one may have been
			** tacked onto the end of the where-variable.
			*/
			ecs_csrset( NULL, ECS_UPDATE|ECS_WHERE );
			switch (yypvt[-0].i & CSR_UPDATE_LIST)
			{
			  case CSR_UPDATE:/* Update clause but no update list */
			    break;
			  case CSR_STATIC:	/* No update clause */
			    ecs_csrset( NULL, ECS_UPDVAR );
			    /* FALL THROUGH! */
			  case CSR_UPD_LIST:/* Update clause with update list */
			  case CSR_UPDATE_LIST:
			    ecs_fixupd( NULL );
			    break;
			}
		    } else if (yypvt[-0].i & CSR_UPD_LIST)	/* UPDATE list */
			ecs_fixupd( NULL );
		}
		break;
	case 806:
#line 5521 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    ecs_csrset( NULL, ECS_ERR );
		}
		break;
	case 807:
#line 5560 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    ecs_eretrieve( NULL, yypvt[-0].i );
		    esq->flag &= ~ESQ_CURSOR;
		}
		break;
	case 808:
#line 5570 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-2].s, yypvt[-0].s );
		    esq_init();
		    ecs_retrieve( yypvt[-0].s, gr->gr_sym != (SYM *)0, yypvt[-1].fet.intv1,
					yypvt[-1].fet.intv2, yypvt[-1].fet.s1, yypvt[-1].fet.v1 );
		    if (eq->eq_flags & EQ_CHRPAD)
		  	esq_sqmods(IImodCPAD);
		    if (eq->eq_flags & EQ_CHREOS)
		  	esq_sqmods(IImodCEOS);
		    esq->flag |= ESQ_CURSOR;
		}
		break;
	case 810:
#line 5584 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
			    yyval.fet.intv1 = 0;
			}
		break;
	case 811:
#line 5663 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
			    yyval.i = 0;
			}
		break;
	case 812:
#line 5668 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
			    CVan(yypvt[-0].s, &yyval.i);
			    yyval.i = -yyval.i;
			}
		break;
	case 813:
#line 5673 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
			    CVan(yypvt[-0].s, &yyval.i);
			}
		break;
	case 814:
#line 5678 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    erec_use( IICSGET, FALSE, NULL );
		    yyval.i = 0;
		}
		break;
	case 815:
#line 5683 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 816:
#line 5687 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 817:
#line 5692 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IICSDAGET );
		    if (eq->eq_flags & EQ_FIPS) 
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2,
			    ERx("USING DESCRIPTOR"), ERx("FETCH") );
		}
		break;
	case 818:
#line 5753 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-1].s)	/* valid cursor name */
		    {
			i4		flags;

			flags = ecs_gcsrbits(yypvt[-1].s);

		      /* REPEATED cursor selects handle IIcsQuery themselves */
			if ((flags & ECS_REPEAT) == 0)
			    ecs_opquery( yypvt[-1].s );
			if (yypvt[-0].i)
			{
			    if ((flags & ECS_DYNAMIC) == 0)
				er_write( E_EQ0425_csUSINGDYN, EQ_ERROR, 1,
									yypvt[-1].s );
			}
		    }
		}
		break;
	case 819:
#line 5773 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4	value;

		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-2].s, yypvt[-1].s );
		    value = ecs_gcsrbits( yypvt[-1].s );
		    /*
		    ** If this was a bad cursor name then ecs_gcsrbits
		    ** printed an error message and returned ECS_NONEXIST.
		    ** Don't call ecs_open or it will print one too.
		    */
		    if ((value & ECS_NONEXIST) == 0)
		    {
		      /* repeat queries handle esq_init in the buffered text */
			if ((value & ECS_REPEAT) == 0)
			    esq_init();
			ecs_open( yypvt[-1].s, yypvt[-0].s,(SYM *)0, gr->gr_sym != (SYM *)0 );
			yyval.s = yypvt[-1].s;
		    } else
			yyval.s = (char *)0;	/* flag invalid cursor name */
		}
		break;
	case 820:
#line 5796 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 821:
#line 5801 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 822:
#line 5806 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = gr->gr_id;
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
		}
		break;
	case 823:
#line 5821 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = NULL;
		}
		break;
	case 824:
#line 5825 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = ERx(" for readonly ");
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("readonly")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
						ERx("OPEN"), ERx("READONLY") );
		    if (eq->eq_flags & EQ_FIPS) 
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2,
			    ERx("FOR READONLY"), ERx("OPEN cursor") );
		}
		break;
	case 825:
#line 5836 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 826:
#line 5840 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 827:
#line 5844 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IISQDAIN );
		    yyval.i = 1;
		}
		break;
	case 828:
#line 5852 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add(ARG_CHAR, (char *)0);	/* no statement name */
		    arg_int_add( 1 );			/* using vars */
		    gen_call( IISQEXSTMT );
		    if (eq->eq_flags & EQ_FIPS) 
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2,
			    ERx("USING"), ERx("OPEN cursor") );
		}
		break;
	case 829:
#line 5893 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s, NULL );
		    er_write( E_EQ0135_sqXACT, EQ_WARN, 1, yypvt[-0].s );
		    esq_init();
		    arg_int_add( IIxactABORT );		/* Close transaction */
		    gen_call( IIXACT );
		    gr->gr_flag |= GR_NOSYNC;	   /* No IIsync after this */
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("ABORT"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("ABORT"));
		}
		break;
	case 831:
#line 5910 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    er_write( E_EQ0135_sqXACT, EQ_WARN, 1, yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
				EQ_WARN, 1, ERx("ABORT"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("ABORT"));
		}
		break;
	case 832:
#line 5929 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-1].s, yypvt[-0].s );
		    esq_init();
		    arg_int_add( IIxactSCOMMIT );	/* Close transaction */
		    gen_call( IIXACT );
		    gr->gr_flag |= GR_NOSYNC;	   /* No IIsync after this */
		    if (yypvt[-0].s == NULL && eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0507_FIPS_NOTOP, EQ_WARN, 2,
			    ERx("WORK"), ERx("COMMIT") );
		}
		break;
	case 833:
#line 5941 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 834:
#line 5946 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("work")))
		    {
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					    ERx("COMMIT WORK"), ERx("WORK") );
			yyval.s = ERx("work");
		    }
		}
		break;
	case 835:
#line 5955 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = NULL;
		}
		break;
	case 836:
#line 5984 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1,
				ERx("DROP") );
		}
		break;
	case 837:
#line 5991 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
		    {
		        if (yypvt[-2].s == (char *)0)
			{
			    er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 1,
				ERx("DROP name_list"));
			}
			else if (yypvt[-1].i)
			{
			    if (ESQ_EQSTR(yypvt[-2].s, ERx("table")))
				er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 1,
				    ERx("DROP TABLE table_name_list"));
			    else if (ESQ_EQSTR(yypvt[-2].s, ERx("view")))
				er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 1,
				    ERx("DROP VIEW view_name_list"));
			    else
				er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 1,
				    ERx("DROP INDEX index_name_list"));
			}
		    }
		}
		break;
	case 855:
#line 6032 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 856:
#line 6037 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = (char *)0;
		}
		break;
	case 857:
#line 6041 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 858:
#line 6046 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 859:
#line 6051 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 860:
#line 6057 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("DROP LINK"));
		}
		break;
	case 861:
#line 6065 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( IIPROC_DROP );

		    /* use first name encountered. */
		    arg_str_add( ARG_CHAR, yypvt[-0].s);
		    gen_call( IIPROCINIT );
		}
		break;
	case 863:
#line 6075 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s);
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("DROP PROCEDURE"));
		}
		break;
	case 866:
#line 6088 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
					EQ_WARN, 1, ERx("DROP ROLE"));
		}
		break;
	case 867:
#line 6096 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP GROUP") );
		}
		break;
	case 868:
#line 6104 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
					EQ_WARN, 1, ERx("DROP DBEVENT"));
		}
		break;
	case 869:
#line 6112 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
					EQ_WARN, 1, ERx("DROP SEQUENCE"));
		}
		break;
	case 870:
#line 6120 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("DROP INTEGRITY"));
		}
		break;
	case 871:
#line 6127 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("DROP PERMIT"));
		}
		break;
	case 872:
#line 6135 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 874:
#line 6140 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 876:
#line 6145 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 889:
#line 6169 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP RULE") );
		}
		break;
	case 890:
#line 6177 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP DOMAIN") );
		}
		break;
	case 891:
#line 6185 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP LOCATION") );
		}
		break;
	case 892:
#line 6193 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP USER") );
		}
		break;
	case 893:
#line 6201 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP PROFILE") );
		}
		break;
	case 894:
#line 6209 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP SECURITY_ALARM") );
		}
		break;
	case 895:
#line 6217 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 899:
#line 6245 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
 		    if (eq->eq_flags & EQ_COMMON)
 			er_write( E_EQ0504_OPEN_WARN,
 				EQ_WARN, 1, ERx("REVOKE") );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
				EQ_WARN, 1, ERx("REVOKE") );
		    gr->gr_func = tREVOKE;  /* Prevent err msg in grant_whom */
		}
		break;
	case 900:
#line 6257 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-2].s );
		    if (!ESQ_EQSTR(yypvt[-1].s, ERx("option")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
					    ERx("REVOKE"), ERx("OPTION") );
		    db_key( ERx("option") );
		    db_key( yypvt[-0].s );
		}
		break;
	case 905:
#line 6273 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("privileges")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					    ERx("REVOKE"), ERx("PRIVILEGES") );
		    db_key( ERx("privileges") );
		}
		break;
	case 918:
#line 6295 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("installation")))
		    {
			er_write( E_EQ006A_grxBADWORD, EQ_ERROR, 2, yypvt[-1].s,
					  ERx("REVOKE"));
		    }
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 922:
#line 6310 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (ESQ_EQSTR(yypvt[-0].s, ERx("dbevent")))
		    {
			db_key( ERx("dbevent") );
		    }
		    else if (ESQ_EQSTR(yypvt[-0].s, ERx("sequence")))
		    {
			db_key( ERx("sequence") );
		    }
		    else
		    {
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					    ERx("REVOKE"), ERx("DATABASE") );
		        db_key( ERx("database") );
		    }
		}
		break;
	case 923:
#line 6327 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 924:
#line 6331 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 927:
#line 6339 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 928:
#line 6343 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 930:
#line 6355 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("SAVEPOINT"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
				EQ_WARN, 1, ERx("SAVEPOINT") );
		}
		break;
	case 931:
#line 6371 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s, NULL );
		    er_write( E_EQ0135_sqXACT, EQ_WARN, 1, yypvt[-0].s );
		    esq_init();
		    arg_int_add( IIxactBEGIN );		/* Begin transaction */
		    gen_call( IIXACT );
		    gr->gr_flag |= GR_NOSYNC;	   /* No IIsync after this */
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("BEGIN TRANSACTION"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
				EQ_WARN, 1, ERx("BEGIN TRANSACTION") );
		}
		break;
	case 933:
#line 6400 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
				EQ_WARN, 1, ERx("COPY TABLE") );
		}
		break;
	case 950:
#line 6429 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_op( yypvt[-2].s );
		    db_key( yypvt[-1].s );
		    db_op( yypvt[-0].s );
		}
		break;
	case 951:
#line 6435 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_op( yypvt[-4].s );
		    db_key( yypvt[-3].s );
		    db_op( yypvt[-2].s );
		    db_key( yypvt[-1].s );
		    db_op( yypvt[-0].s );
		}
		break;
	case 958:
#line 6452 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 959:
#line 6456 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 961:
#line 6462 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym)
		    {
			db_var( DB_STRING, gr->gr_sym, gr->gr_id,
				(SYM *)0, (char *)0, (char *)0 );
			if (gr->gr_type != T_CHAR && gr->gr_type != T_UNDEF)
			    er_write( E_EQ0067_grSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
		    }
		    else	/* Error caught in host_setvar */
			db_sconst( gr->gr_id );
		}
		break;
	case 962:
#line 6475 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("program")))
		    {
			er_write( E_EQ006A_grxBADWORD, EQ_ERROR, 2, yypvt[-0].s,
					    ERx("COPY") );
			db_sconst( yypvt[-0].s );
		    }
		    else
		    {
			db_key( yypvt[-0].s );
			gr->gr_flag |= GR_COPY_PROG;
		    }
		}
		break;
	case 963:
#line 6498 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s, NULL );
		    er_write( E_EQ0135_sqXACT, EQ_WARN, 1, yypvt[-0].s );
		    esq_init();
		    arg_int_add( IIxactCOMMIT );	/* Close transaction */
		    gen_call( IIXACT );
		    gr->gr_flag |= GR_NOSYNC;	   /* No IIsync after this */
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("END TRANSACTION"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
				EQ_WARN, 1, ERx("END TRANSACTION") );
		}
		break;
	case 965:
#line 6540 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (esq->flag & ESQ_CRE_SCHEMA)	/* in CREATE SCHEMA */
			db_key( yypvt[-0].s );
		    else
		    {
		        gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
 		        if (eq->eq_flags & EQ_COMMON)
 			    er_write( E_EQ0504_OPEN_WARN, 
 					EQ_WARN, 1, ERx("GRANT"));
		    }
		}
		break;
	case 966:
#line 6553 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS) 
			er_write( E_EQ0507_FIPS_NOTOP, EQ_WARN,
				2, ERx("PRIVILEGES"), ERx("GRANT") );
		}
		break;
	case 973:
#line 6567 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, ERx("INSERT"), ERx("GRANT") );
		    }
		}
		break;
	case 975:
#line 6576 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, ERx("EXECUTE"), ERx("GRANT") );
		    }
		}
		break;
	case 976:
#line 6584 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, ERx("REGISTER"), ERx("GRANT") );
		    }
		}
		break;
	case 979:
#line 6594 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, yypvt[-0].s, ERx("GRANT") );
		    }
		}
		break;
	case 980:
#line 6602 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, yypvt[-1].s, ERx("GRANT") );
		    }
		}
		break;
	case 983:
#line 6614 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("privileges")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					    ERx("GRANT"), ERx("PRIVILEGES") );
		    db_key( ERx("privileges") );
		}
		break;
	case 985:
#line 6624 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
                    db_key( yypvt[-0].s );
                }
		break;
	case 986:
#line 6629 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 987:
#line 6634 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 988:
#line 6639 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 989:
#line 6644 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 990:
#line 6649 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 991:
#line 6654 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS && yypvt[-0].i == 1)
		    {
			/* List of tables is non-FIPS */
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
			    2, ERx("ON <table_list>"), ERx("GRANT/CREATE SECURITY_ALARM") );
		    }
		}
		break;
	case 992:
#line 6663 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			/* FIPS allows ON to be followed only by <tablename> */
			STprintf( esq->sbuf, ERx("%s %s"), ERx("ON"), yypvt[-0].s );
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, esq->sbuf, ERx("GRANT/CREATE SECURITY_ALARM") );
		    }
		}
		break;
	case 993:
#line 6673 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		}
		break;
	case 994:
#line 6676 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("installation")))
		    {
			er_write( E_EQ006A_grxBADWORD, EQ_ERROR, 2, yypvt[-0].s,
				ERx("GRANT"));
		    }
		    /* ON CURRENT INSTALLATION is non-FIPS */
		    else if (eq->eq_flags & EQ_FIPS)
		    {
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2,
				ERx("ON CURRENT INSTALLATION"), ERx("GRANT/CREATE SECURITY_ALARM") );
		    }
		}
		break;
	case 996:
#line 6695 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 997:
#line 6700 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 998:
#line 6705 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 999:
#line 6711 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS && gr->gr_func != tREVOKE)
		    {
			/* TO ROLE is non-FIPS */
			STprintf( esq->sbuf, ERx("%s %s"), ERx("TO"),
				yypvt[-1].s );
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, esq->sbuf, ERx("GRANT") );
		    }
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
#line 6722 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS && gr->gr_func != tREVOKE)
		    {
			/* TO USER is non-FIPS */
			STprintf( esq->sbuf, ERx("%s %s"), ERx("TO"),
				yypvt[-1].s );
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, esq->sbuf, ERx("GRANT") );
		    }
		}
		break;
	case 1002:
#line 6734 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* TO GROUP is non-FIPS */
		    if (eq->eq_flags & EQ_FIPS && gr->gr_func != tREVOKE)
			er_write( E_EQ0506_FIPS_CLAUSE,
				EQ_WARN, 2, ERx("TO GROUP"), ERx("GRANT") );
		}
		break;
	case 1003:
#line 6741 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (   eq->eq_flags & EQ_FIPS
			&& ESQ_EQSTR(yypvt[-0].s, ERx(","))	/* A list */
			&& gr->gr_func != tREVOKE)
		    {
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, ERx("TO <user_list>"),  ERx("GRANT") );
		    }
		}
		break;
	case 1004:
#line 6758 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 1009:
#line 6768 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 1010:
#line 6773 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 1014:
#line 6782 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 1017:
#line 6799 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("MODIFY"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
				  EQ_WARN, 1, ERx("MODIFY") );
		}
		break;
	case 1025:
#line 6823 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1037:
#line 6855 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IISQTPC );
		    gr->gr_flag |= GR_NOSYNC;	   /* No IIsync after this */
		    if (!ESQ_EQSTR(yypvt[-6].s, ERx("highdxid")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-6].s,
				    ERx("PREPARE TO COMMIT"), ERx("HIGHDXID") );
		    if (!ESQ_EQSTR(yypvt[-2].s, ERx("lowdxid")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-2].s,
				    ERx("PREPARE TO COMMIT"), ERx("LOWDXID") );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 
				1, ERx("PREPARE TO COMMIT"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				1, ERx("PREPARE TO COMMIT"));
		}
		break;
	case 1038:
#line 6873 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-3].s, NULL );
		    esq_init();
		}
		break;
	case 1039:
#line 6879 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_INT, yypvt[-0].s );
		}
		break;
	case 1040:
#line 6883 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym)
		    {
			if (gr->gr_type == T_INT)
			{
			    arg_var_add( gr->gr_sym, gr->gr_id );
			}
			else
			{
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0061_grINTVAR, EQ_ERROR, 1,
				    gr->gr_id );
			    arg_int_add(0);
			}
		    }
		    else	/* Error in host_setvar */
		    {
			arg_int_add(0);
		    }
		}
		break;
	case 1041:
#line 6912 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1, yypvt[-1].s );
		}
		break;
	case 1042:
#line 6917 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 1043:
#line 6927 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("RAISE DBEVENT"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				1, ERx("RAISE DBEVENT") );
		}
		break;
	case 1044:
#line 6937 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 1047:
#line 6944 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym)
		    {
			db_var( DB_STRING, gr->gr_sym, gr->gr_id,
				(SYM *)0, (char *)0, (char *)0 );
			if (gr->gr_type != T_INT && gr->gr_type != T_CHAR &&
							gr->gr_type != T_UNDEF)
			    er_write( E_EQ0060_grINTSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
		    }
		    else	/* Error already from host_setvar */
			db_key( gr->gr_id );
		}
		break;
	case 1049:
#line 6990 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-0].s != (char *)0)
			gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		    else
			gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, NULL );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				1, ERx("REGISTER") );
		}
		break;
	case 1050:
#line 7001 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = (char *)0;
		}
		break;
	case 1051:
#line 7005 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1052:
#line 7009 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1053:
#line 7013 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1055:
#line 7019 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
		    {
			if (yypvt[-1].i & CRE_TBLKEY)
			    er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 1,
				ERx("REGISTER ... TABLE_KEY"));
			else if (yypvt[-1].i & CRE_OBJKEY)
			    er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 1,
				ERx("REGISTER ... OBJECT_KEY"));
			else if (yypvt[-1].i)
			    er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 1,
			    	ERx("REGISTER ... DEFAULT/SYSTEM_MAINTAINED"));
		    }	
		}
		break;
	case 1056:
#line 7035 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1057:
#line 7039 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-2].i | yypvt[-0].i;
		}
		break;
	case 1058:
#line 7044 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1059:
#line 7048 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-2].i;
		}
		break;
	case 1061:
#line 7054 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1062:
#line 7059 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (ESQ_EQSTR(yypvt[-0].s, ERx("import")))
		    {
			db_key( ERx("import") );
		    }
		    else
		    {
			db_key( ERx("link") );
		        if (!ESQ_EQSTR(yypvt[-0].s, ERx("link")))
		        {
			    er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					ERx("REGISTER"), ERx("LINK") );
			}
		    }
		}
		break;
	case 1070:
#line 7088 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				1, ERx("REGISTER INDEX") );
		}
		break;
	case 1071:
#line 7095 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    STprintf( esq->sbuf, ERx("%s %s"), yypvt[-2].s, yypvt[-1].s );
		    gr_mechanism( GR_EQSTMT, GR_sSQL, esq->sbuf, yypvt[-0].s );
		}
		break;
	case 1072:
#line 7100 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		}
		break;
	case 1078:
#line 7114 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("REGISTER DBEVENT"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				1, ERx("REGISTER DBEVENT") );
		}
		break;
	case 1080:
#line 7132 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("RELOCATE"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("RELOCATE"));
		}
		break;
	case 1081:
#line 7148 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("REMOVE"));
		}
		break;
	case 1082:
#line 7155 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-0].s != (char *)0)
			gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		    else
			gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, NULL );
		}
		break;
	case 1083:
#line 7162 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("REMOVE DBEVENT"));
		}
		break;
	case 1084:
#line 7170 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = (char *)0;
		}
		break;
	case 1085:
#line 7174 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1086:
#line 7178 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1087:
#line 7182 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1088:
#line 7186 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1089:
#line 7196 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s, NULL );
		    esq_init();
		    arg_int_add( IIxactABORT );		/* Close transaction */
		    gen_call( IIXACT );
		    gr->gr_flag |= GR_NOSYNC;	   /* No IIsync after this */
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0507_FIPS_NOTOP, EQ_WARN, 2,
			    		ERx("WORK"), ERx("ROLLBACK") );
		}
		break;
	case 1090:
#line 7207 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("work")))
		    {
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					    ERx("ROLLBACK WORK"), ERx("WORK") );
			yypvt[-0].s = ERx("work");
		    }
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-1].s, yypvt[-0].s );
		    esq_init();
		    arg_int_add( IIxactABORT );		/* Close transaction */
		    gen_call( IIXACT );
		    gr->gr_flag |= GR_NOSYNC;	   /* No IIsync after this */
		}
		break;
	case 1091:
#line 7221 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, NULL );
		    db_key( yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 
				1, ERx("ROLLBACK TO SAVEPOINT"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0507_FIPS_NOTOP, EQ_WARN, 2,
			    		ERx("WORK"), ERx("ROLLBACK") );
		}
		break;
	case 1093:
#line 7233 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-1].s, ERx("work")))
		    {
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
					ERx("ROLLBACK WORK TO"), ERx("WORK") );
			yypvt[-1].s = ERx("work");
		    }
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-2].s, yypvt[-1].s );
		    db_key( yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 1, 
				ERx("ROLLBACK WORK TO SAVEPOINT"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, ERx("TO SAVEPOINT"), ERx("ROLLBACK") );
		}
		break;
	case 1097:
#line 7260 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("SAVE"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				1, ERx("SAVE") );
		}
		break;
	case 1098:
#line 7271 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1100:
#line 7278 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1101:
#line 7321 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /*
		    ** Check for SET ROLE, we need to handle
		    ** the WITH PASSWORD clause carefully.
		    */
		    if (ESQ_EQSTR(yypvt[-0].s, ERx("role")))
		    	set_role=TRUE;
		    else
		    	set_role=FALSE;
		}
		break;
	case 1102:
#line 7332 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
		    {
			/*
			** We're flaggin non open/sql statements (-wopen)
			** Check for AUTOCOMMIT.  Flag all others.
			*/
		    	if (!ESQ_EQSTR(yypvt[-1].s, ERx("autocommit")))
			{
			   STprintf( esq->sbuf, ERx("%s %s"), ERx("SET"), yypvt[-1].s);
			   er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, esq->sbuf );
			}
		    }
		}
		break;
	case 1103:
#line 7348 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
                /* SET GROUP groupid */
                   if (eq->eq_flags & EQ_COMMON)
                   {
                       er_write( E_EQ0504_OPEN_WARN,
                                   EQ_WARN, 1, ERx("SET [NO]GROUP"));
                   }
                }
		break;
	case 1104:
#line 7357 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
                /* SET GROUP SESSION GROUP  */
                   if (eq->eq_flags & EQ_COMMON)
                   {
                       er_write( E_EQ0504_OPEN_WARN,
                                   EQ_WARN, 1, ERx("SET [NO]GROUP"));
                   }
                }
		break;
	case 1105:
#line 7366 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
                    {
                        er_write( E_EQ0504_OPEN_WARN,
                                        EQ_WARN, 1, ERx("SET SESSION"));
                    }
                }
		break;
	case 1106:
#line 7376 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    gr->gr_func = tSET;
                    if (eq->eq_flags & EQ_FIPS)
                        er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
                                        1, ERx("SET") );
		    set_role=FALSE;
		}
		break;
	case 1117:
#line 7423 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("privileges")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					    ERx("SET SESSION"), ERx("PRIVILEGES") );
		    db_key( ERx("privileges") );
		}
		break;
	case 1126:
#line 7445 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 1131:
#line 7454 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
			db_key (yypvt[-0].s);
		}
		break;
	case 1132:
#line 7458 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
			db_key (yypvt[-0].s);
		}
		break;
	case 1133:
#line 7462 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
			db_key (yypvt[-0].s);
		}
		break;
	case 1134:
#line 7466 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
			db_key (yypvt[-0].s);
		}
		break;
	case 1135:
#line 7470 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym)
		    {
			db_var( DB_STRING, gr->gr_sym, gr->gr_id,
				(SYM *)0, (char *)0, (char *)0 );
			if (gr->gr_type != T_INT && gr->gr_type != T_CHAR &&
							gr->gr_type != T_UNDEF)
			    er_write( E_EQ0064_grQRSTRVAR, EQ_ERROR, 1,
						    gr->gr_id );
		    }
		    else	/* Error already from host_setvar */
			db_key( gr->gr_id );
		}
		break;
	case 1137:
#line 7487 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1138:
#line 7491 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1146:
#line 7504 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Expecting SET WORK LOCATIONS command */		
		    if (!ESQ_EQSTR(yypvt[-4].s, ERx("locations")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-4].s,
					ERx("SET WORK"), ERx("LOCATIONS") );
		}
		break;
	case 1147:
#line 7511 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{ 
		    db_sconst( yypvt[-0].s );
		}
		break;
	case 1148:
#line 7515 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{ 
		    db_key( yypvt[-0].s );
		}
		break;
	case 1158:
#line 7531 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    erec_use( IISETDOM, gr->gr_flag & GR_REPEAT, ERx(",") );
		    erec_setup( 1 );
		}
		break;
	case 1168:
#line 7550 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1169:
#line 7554 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1174:
#line 7565 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
			db_key( yypvt[-0].s );
		}
		break;
	case 1176:
#line 7573 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
			db_key( yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ050F_FIPS_USE, EQ_WARN, 1,
					ERx(yypvt[-0].s) );
		}
		break;
	case 1177:
#line 7582 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
			db_key( yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ050F_FIPS_USE, EQ_WARN, 1,
					ERx(yypvt[-0].s) );
		}
		break;
	case 1178:
#line 7591 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
			db_key( yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ050F_FIPS_USE, EQ_WARN, 1,
					ERx(yypvt[-0].s) );
		}
		break;
	case 1179:
#line 7600 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
                    /* Don't print if in DECLARE TABLE statement */
                    if ((esq->flag & ESQ_NOPRT) == 0)
                        db_key( yypvt[-0].s );
		}
		break;
	case 1180:
#line 7607 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
                    if (eq->eq_flags & EQ_FIPS)
                        er_write( E_EQ050F_FIPS_USE, EQ_WARN, 1, yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0510_OPEN_FUNC, EQ_WARN, 1, yypvt[-0].s );
		}
		break;
	case 1182:
#line 7621 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    char   nbuf[50];

		    nbuf[0] = '$';
		    nbuf[1] = EOS;
		    yyval.s = str_add( STRNULL, STcat(nbuf, yypvt[-0].s) );

		    if ( (!ESQ_EQSTR(yypvt[-0].s, ERx("dba"))) &&
			 (!ESQ_EQSTR(yypvt[-0].s, ERx("ingres")))
		       )
                    {
                        er_write(  E_EQ0066_grSTR, EQ_ERROR, 1, yyval.s);
                    }
                    if (eq->eq_flags & EQ_FIPS)
                        er_write( E_EQ050F_FIPS_USE, EQ_WARN, 1, yyval.s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0510_OPEN_FUNC, EQ_WARN, 1, yyval.s );
		}
		break;
	case 1183:
#line 7641 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s ); 
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1190:
#line 7669 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key(yypvt[-1].s);
		    db_key(yypvt[-0].s);
		}
		break;
	case 1191:
#line 7674 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key(yypvt[-1].s);
		    db_key(yypvt[-0].s);
		}
		break;
	case 1192:
#line 7679 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key(yypvt[-1].s);
		    db_key(yypvt[-0].s);
		}
		break;
	case 1193:
#line 7684 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 1194:
#line 7690 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    gr->gr_func = tSET;
		}
		break;
	case 1195:
#line 7696 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 1196:
#line 7710 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1197:
#line 7715 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1198:
#line 7720 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1199:
#line 7725 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1201:
#line 7731 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1202:
#line 7736 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1203:
#line 7741 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1204:
#line 7746 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1205:
#line 7751 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1206:
#line 7756 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1207:
#line 7761 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1208:
#line 7766 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_op(ERx(", "));	/* Need a space for run-time */
		}
		break;
	case 1209:
#line 7771 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1210:
#line 7776 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1211:
#line 7781 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		if ((esq->flag & ESQ_NOPRT) == 0)
		    db_key( yypvt[-0].s );
		}
		break;
	case 1212:
#line 7787 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		if ((esq->flag & ESQ_NOPRT) == 0)
		    db_op( yypvt[-0].s );
		}
		break;
	case 1213:
#line 7793 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1214:
#line 7798 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1215:
#line 7803 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1216:
#line 7808 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1217:
#line 7813 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1218:
#line 7818 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1219:
#line 7823 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1220:
#line 7828 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1221:
#line 7833 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		if ((esq->flag & ESQ_NOPRT) == 0)
		    db_key( yypvt[-0].s );
		}
		break;
	case 1222:
#line 7839 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1223:
#line 7844 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1224:
#line 7849 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1225:
#line 7854 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1226:
#line 7859 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1227:
#line 7864 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1228:
#line 7869 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1229:
#line 7874 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1230:
#line 7879 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1231:
#line 7884 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1232:
#line 7889 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_op( ERx(" -") );
		}
		break;
	case 1233:
#line 7894 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1234:
#line 7899 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		if ((esq->flag & ESQ_NOPRT) == 0)
		    db_key( yypvt[-0].s );
		}
		break;
	case 1235:
#line 7905 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1236:
#line 7910 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1237:
#line 7915 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1238:
#line 7920 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1239:
#line 7925 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1240:
#line 7930 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1241:
#line 7935 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1242:
#line 7940 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1243:
#line 7945 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1244:
#line 7950 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1245:
#line 7955 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1246:
#line 7960 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1247:
#line 7965 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1248:
#line 7970 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1249:
#line 7975 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_op( ERx(" +") );
		}
		break;
	case 1250:
#line 7980 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1251:
#line 7985 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1252:
#line 7990 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1253:
#line 7995 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1254:
#line 8000 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1255:
#line 8005 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1256:
#line 8010 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1257:
#line 8015 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1258:
#line 8020 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1259:
#line 8025 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1260:
#line 8030 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1261:
#line 8035 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1262:
#line 8040 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1263:
#line 8045 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		    /*
		    ** Tables named in subselects in updatable cursors should
		    ** not contribute to the "> 1 table" error. (bug 5676)
		    */
		    if (ESQ_DCLCSR(esq))
			esq->flag &= ~ESQ_FROM_CHK;

		}
		break;
	case 1264:
#line 8057 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			if ((eqck_tgt( 1, 0 )) == FALSE)	/* reset */
			    esq->flag |= ESQ_UNION_SEL;
		    }
		    db_key( yypvt[-0].s );
		}
		break;
	case 1265:
#line 8066 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			if ((eqck_tgt( 1, 0 )) == FALSE)	/* reset */
			    esq->flag |= ESQ_UNION_SEL;
		    }
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 1266:
#line 8076 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			if ((eqck_tgt( 1, 0 )) == FALSE)	/* reset */
			    esq->flag |= ESQ_UNION_SEL;
		    }
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 1267:
#line 8087 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1268:
#line 8092 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1271:
#line 8121 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1272:
#line 8127 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 1273:
#line 8131 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 1274:
#line 8137 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if ((esq->flag & ESQ_NOPRT) == 0)
			db_key( yypvt[-0].s );
                    yyval.s = yypvt[-0].s;
                }
		break;
	case 1275:
#line 8143 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if ((esq->flag & ESQ_NOPRT) == 0)
			db_delimid( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1276:
#line 8150 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
                    yyval.s = yypvt[-0].s;
                    if (eq->eq_flags & EQ_FIPS)
                        eqck_regid(yypvt[-0].s); /* Check if id is FIPS compliant */
                }
		break;
	case 1277:
#line 8157 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    if (eq->eq_flags & EQ_FIPS)
			eqck_delimid(yypvt[-0].s); /* Check if id is FIPS complaint */
		}
		break;
	case 1279:
#line 8166 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		i4 dummy;

	  	/*
		** Put the quotes back for delimited ids and escape
		** them (\") so they will be generated correctly
		** later on. 
		*/
		/* Place the delimited ids (with quotes and escape 
		** characters) in buffers of the string table work space
		** to avoid being overwritten when there is another
		** call to gen_sqmakesconst within the same SQL 
		** statement.                 		b57771tt
		*/ 
		
		yyval.s = str_add( STRNULL,
                       gen_sqmakesconst(G_DELSTRING, yypvt[-0].s, &dummy) );

		}
		break;
	case 1282:
#line 8190 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
                    db_key( yypvt[-0].s );
                    yyval.s = yypvt[-0].s;
                    gr->gr_sym = (SYM *)0;
                }
		break;
	case 1283:
#line 8197 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
                    db_key( yypvt[-0].s );
                    yyval.s = yypvt[-0].s;
                    gr->gr_sym = (SYM *)0;
                }
		break;
	case 1284:
#line 8203 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
                    db_key( yypvt[-0].s );
                    yyval.s = yypvt[-0].s;
                    gr->gr_sym = (SYM *)0;
                }
		break;
	case 1285:
#line 8209 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
                    db_key( yypvt[-0].s );
                    yyval.s = yypvt[-0].s;
                    gr->gr_sym = (SYM *)0;
                }
		break;
	case 1286:
#line 8220 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_sconst( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1287:
#line 8229 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1288:
#line 8233 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1289:
#line 8241 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = ERx(" ");
		}
		break;
	case 1290:
#line 8245 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = ERx(",");
		}
		break;
	case 1291:
#line 8255 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1292:
#line 8267 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		    if (gr->gr_flag & GR_CPYHNDDEF) 	/* Turned off above */
		    {
			eqtl_init(IISETSQL);
		        eqtl_add(yypvt[-0].s, (PTR)0, (PTR)0, (PTR)0, T_INT,
				ERx("copyhandler"));
			gr->gr_flag |= GR_SEENCPYHND;
		    }
		}
		break;
	case 1293:
#line 8278 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		    if (gr->gr_flag & GR_CPYHNDDEF) 	/* Turned off above */
		    {
			eqtl_init(IISETSQL);
			eqtl_add(yypvt[-0].s, (PTR)0, (PTR)0, (PTR)0, T_NONE,
			        ERx("copyhandler"));
			gr->gr_flag |= GR_SEENCPYHND;
		    }
		}
		break;
	case 1294:
#line 8289 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_delimid( yypvt[-0].s );
		    if (gr->gr_flag & GR_CPYHNDDEF) 	/* Turned off above */
		    {
			eqtl_init(IISETSQL);
			eqtl_add(yypvt[-0].s, (PTR)0, (PTR)0, (PTR)0, T_NONE,
			        ERx("copyhandler"));
			gr->gr_flag |= GR_SEENCPYHND;
		    }
		}
		break;
	case 1296:
#line 8302 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4		db_type;

		    if (gr->gr_sym)
		    {
			if(set_role)
			{
			    /*
			    ** This should be SET ROLE WITH PASSWORD=:pwd
			    ** We send pwd as a quoted string, so handle
			    ** as special case here. This avoids changes which
			    ** might affect existing user code which passes
			    ** unexpected values down in the variable
			    */
			    db_var( DB_STRING, gr->gr_sym, gr->gr_id,
				(SYM *)0, (char *)0, (char *)0 );

			    if (gr->gr_type != T_INT && gr->gr_type != T_CHAR &&
							gr->gr_type != T_UNDEF)
			           er_write( E_EQ0064_grQRSTRVAR, EQ_ERROR, 1,
						    gr->gr_id );
			}
			else
			{
			    db_type = gr->gr_type == T_CHAR ? DB_ID : DB_STRING;
			    db_var( db_type, gr->gr_sym, gr->gr_id,
				(SYM *)0, (char *)0, (char *)0 );
			    if (gr->gr_type != T_INT && gr->gr_type != T_CHAR &&
							gr->gr_type != T_UNDEF)
			    	er_write( E_EQ0060_grINTSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
			 }
		    }
		    else	/* Error already from host_setvar */
			db_key( gr->gr_id );
		}
		break;
	case 1304:
#line 8348 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym)
		    {
			db_var( DB_STRING, gr->gr_sym, gr->gr_id,
				(SYM *)0, (char *)0, (char *)0 );
			if (gr->gr_type != T_INT && gr->gr_type != T_CHAR &&
							gr->gr_type != T_UNDEF)
			    er_write( E_EQ0060_grINTSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
		    }
		    else	/* Error already from host_setvar */
			db_key( gr->gr_id );
		}
		break;
	case 1305:
#line 8369 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym && gr->gr_type == T_CHAR)
			db_var( DB_ID, gr->gr_sym, gr->gr_id,
				(SYM *)0, (char *)0, (char *)0 );
		    else
		    {
			if (gr->gr_type != T_UNDEF)
			    er_write( E_EQ0064_grQRSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
			db_key( gr->gr_id );
		    }
		}
		break;
	case 1306:
#line 8389 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add(ARG_CHAR, yypvt[-0].s);
		}
		break;
	case 1307:
#line 8394 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    STprintf( esq->sbuf, ERx("%s%s%s"), yypvt[-2].s, yypvt[-1].s, yypvt[-0].s );
		    arg_str_add(ARG_CHAR, esq->sbuf);
		}
		break;
	case 1308:
#line 8399 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym && gr->gr_type == T_CHAR)
			arg_var_add(gr->gr_sym, gr->gr_id);
		    else
		    {
			if (gr->gr_type != T_UNDEF)
			    er_write(E_EQ0067_grSTRVAR, EQ_ERROR, 1, gr->gr_id);
			arg_str_add(ARG_CHAR, gr->gr_id);
		    }
		}
		break;
	case 1309:
#line 8414 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_nlsym = (SYM *)0;	/* Initialize state vars */
		    gr->gr_nlid = (char *)0;
		    gr->gr_nltype = T_NONE; 
		}
		break;
	case 1312:
#line 8423 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
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
	case 1313:
#line 8450 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* 
		    ** Save away variable information 
		    */
		    gr->gr_nlsym = gr->gr_sym;
		    gr->gr_nlid = gr->gr_id;
		    gr->gr_nltype = gr->gr_type;
		    /*
		    ** tell scanner it's a hostvar.  See slave grammers for
		    ** details (under HOST VARIABLE USAGE).
		    */
		    sc_hostvar = TRUE;
		}
		break;
	case 1315:
#line 8470 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1318:
#line 8480 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1321:
#line 8493 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("asc")) &&
					!ESQ_EQSTR(yypvt[-0].s, ERx("desc")))
			er_write( E_EQ0126_sqCOMMA, EQ_ERROR, 0);
		    db_key( yypvt[-0].s );
		}
		break;
	case 1322:
#line 8511 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 1323:
#line 8515 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 1327:
#line 8547 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_flag &= ~GR_CPYHNDDEF;
		}
		break;
	case 1333:
#line 8558 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (   gr->gr_flag & GR_COPY_PROG
		        && ESQ_EQSTR(yypvt[-0].s, ERx("copyhandler")))
			gr->gr_flag |= GR_CPYHNDDEF;
		    db_key( yypvt[-0].s );
		}
		break;
	case 1334:
#line 8565 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1335:
#line 8569 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1336:
#line 8573 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1337:
#line 8577 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1338:
#line 8581 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1339:
#line 8585 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1340:
#line 8589 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1341:
#line 8593 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1345:
#line 8601 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1346:
#line 8605 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1347:
#line 8609 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1348:
#line 8613 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1349:
#line 8617 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1354:
#line 8629 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1355:
#line 8633 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1394:
#line 8743 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( IIutEXE );
		    arg_str_add( ARG_CHAR, (char *)0 );
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_call( IIUTSYS );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
					1, ERx("CALL") );
		}
		break;
	case 1395:
#line 8753 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s, NULL );
		    esq_init();
		    arg_int_add( IIutPROG );
		}
		break;
	case 1396:
#line 8760 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_call( IIUTSYS );
		}
		break;
	case 1399:
#line 8770 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( IIutARG );
		}
		break;
	case 1402:
#line 8778 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IIUTSYS );
		}
		break;
	case 1404:
#line 8784 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 1405:
#line 8788 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 1406:
#line 8793 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( IIutARG );
		}
		break;
	case 1421:
#line 8845 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s, NULL );
		    esq_init();
		    arg_int_add( eq_genlang(eq->eq_lang) );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ050A_FIPS_EXTSN, EQ_WARN,
					1, ERx("CONNECT") );
		}
		break;
	case 1422:
#line 8855 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IISQSESS );
		}
		break;
	case 1423:
#line 8860 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_push();
# ifdef DGC_AOS
		    /*
		    ** Multiple connects are not legal in Open SQL, so if
		    ** user tries to do multiple connects on DG, then a
		    ** SYNTAX error is generated.  The multiple connect code
		    ** is still generated, but with the SYNTAX ERROR msg.,
		    ** the user's source code cannnot be compiled. (B 8174)
		    */
		    er_write( E_EQ006A_grxBADWORD, EQ_ERROR, 2, yypvt[-0].s,
			ERx("CONNECT") );
		    gen_host( G_H_OP|G_H_NEWLINE, ERx("...SYNTAX ERROR;") );
# endif /* DGC_AOS */
		}
		break;
	case 1424:
#line 8877 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IISQCNNM );
		}
		break;
	case 1425:
#line 8882 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_push();
		}
		break;
	case 1426:
#line 8886 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{ arg_push(); }
		break;
	case 1427:
#line 8887 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IISQUSER );
		}
		break;
	case 1429:
#line 8895 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{ 
		  arg_str_add(ARG_CHAR, ERx("-remote_system_user")); 
		}
		break;
	case 1431:
#line 8902 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{ 
		  arg_str_add(ARG_CHAR, ERx("-remote_system_password"));
		}
		break;
	case 1433:
#line 8908 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		  arg_str_add(ARG_CHAR, ERx("-dbms_password"));
	        }
		break;
	case 1437:
#line 8917 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-1].s, ERx("options")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
					    ERx("CONNECT"), ERx("OPTIONS") );
		}
		break;
	case 1441:
#line 8929 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /*
		    ** Can't mix transaction ids for 2-phase commit and
		    ** other with elements on connect
		    */
		    if (gr->gr_flag & GR_HIGHX && gr->gr_flag & GR_LOWX)
			er_write( E_EQ0079_grCONNECTID, EQ_ERROR, 0 );
		    if (ESQ_EQSTR(yypvt[-0].s, ERx("highdxid")))
			gr->gr_flag |= GR_HIGHX;
		    else if (ESQ_EQSTR(yypvt[-0].s, ERx("lowdxid")))
			gr->gr_flag |= GR_LOWX;
		}
		break;
	case 1442:
#line 8943 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 1443:
#line 8947 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 1444:
#line 8953 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add(IIsqpEND);
		    arg_str_add(ARG_CHAR, (char *)0);
		    arg_int_add(0);
		    arg_str_add(ARG_CHAR, (char *)0);
		    arg_int_add(0);
		    gen_call(IISQPARMS);
		}
		break;
	case 1445:
#line 8962 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IISQCONNECT );
		}
		break;
	case 1446:
#line 8967 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    esq_sqmods(IImodWITH);  	/* force in modifier */
		    gen_call( IISQCONNECT );	/* Close CONNECT */

		    arg_int_add(IIsqpINI);

		  /* Initialize for 2PC with-clause */
		    gr->gr_flag &= ~(GR_HIGHX|GR_LOWX);	
		}
		break;
	case 1447:
#line 8978 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
			/* All specified in string variable */
			arg_int_add(DB_CHR_TYPE);
			arg_str_add(ARG_CHAR, (char *)0);
			arg_int_add(0);
			gen_call(IISQPARMS);
		}
		break;
	case 1448:
#line 8986 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (   (gr->gr_flag & GR_HIGHX || gr->gr_flag & GR_LOWX)
			&& !(gr->gr_flag & GR_HIGHX && gr->gr_flag & GR_LOWX)
		       )
		    {
			er_write( E_EQ0079_grCONNECTID, EQ_ERROR, 0 );
		    }
		}
		break;
	case 1451:
#line 8999 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
			/* Start the next call */
			arg_int_add(IIsqpADD);
		}
		break;
	case 1452:
#line 9005 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* End the current call */
		    gen_call(IISQPARMS);
		}
		break;
	case 1453:
#line 9011 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add(DB_CHR_TYPE);
		    arg_str_add( ARG_CHAR, form_sconst(yypvt[-0].s) );
		    arg_int_add(0);
		    if (gr->gr_flag & GR_HIGHX || gr->gr_flag & GR_LOWX)
			er_write( E_EQ0079_grCONNECTID, EQ_ERROR, 0 );
		}
		break;
	case 1454:
#line 9019 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add(DB_CHR_TYPE);
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    arg_int_add(0);
		    if (gr->gr_flag & GR_HIGHX || gr->gr_flag & GR_LOWX)
			er_write( E_EQ0079_grCONNECTID, EQ_ERROR, 0 );
		}
		break;
	case 1455:
#line 9027 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add(DB_INT_TYPE);
		    arg_str_add( ARG_CHAR, (char *)0 );
		    arg_str_add( ARG_INT, yypvt[-0].s );
		}
		break;
	case 1456:
#line 9033 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym)
		    {
			if (gr->gr_type == T_CHAR)
			{
			    arg_int_add(DB_CHR_TYPE);
			    arg_var_add( gr->gr_sym, gr->gr_id );
			    arg_int_add(0);
			    if (gr->gr_flag & GR_HIGHX || gr->gr_flag & GR_LOWX)
				er_write( E_EQ0079_grCONNECTID, EQ_ERROR, 0 );
			}
			else if (gr->gr_type == T_INT)
			{
			    arg_int_add(DB_INT_TYPE);
			    arg_str_add( ARG_CHAR, (char *)0 );
			    arg_var_add( gr->gr_sym, gr->gr_id );
			}
			else
			{
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0060_grINTSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
			    arg_int_add(DB_CHR_TYPE);
			    arg_str_add( ARG_CHAR, gr->gr_id );
			    arg_int_add(0);
			}
		    }
		    else	/* Error in host_setvar */
		    {
			arg_int_add(DB_CHR_TYPE);
			arg_str_add( ARG_CHAR, gr->gr_id );
			arg_int_add(0);
		    }
		}
		break;
	case 1457:
#line 9077 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_CHAR, ERx("errortext") );
		    gen_call( IIEQINQ );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("COPY SQLERROR"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				1, ERx("COPY SQLERROR") );
		}
		break;
	case 1458:
#line 9089 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("sqlerror")))
		    {
			er_write( E_EQ0070_grMISSWORD, EQ_ERROR, 2,
						ERx("TABLE"), ERx("COPY") );
			YYERROR;
		    }
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-1].s, yypvt[-0].s );
		    arg_int_add( 0 );	/* No NULL indicator */
		}
		break;
	case 1459:
#line 9101 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym)
		    {
			if (gr->gr_type != T_CHAR)
			{
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0067_grSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
			}
		    } else	/* Error in host_retvar */
			;
		    arg_var_add( gr->gr_sym, gr->gr_id );
		}
		break;
	case 1464:
#line 9123 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    er_write( E_EQ0070_grMISSWORD, EQ_ERROR, 2, ERx("INTO"),
							ERx("COPY SQLERROR") );
		}
		break;
	case 1465:
#line 9136 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-1].i && eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, EQ_WARN, 1,
			    ERx("DECLARE TABLE ... DEFAULT/SYSTEM_MAINTAINED"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				1, ERx("DECLARE TABLE") );
		}
		break;
	case 1466:
#line 9146 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    esq->flag |= ESQ_NOPRT;	/* Suppress printing */
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-1].s, yypvt[-0].s );
		}
		break;
	case 1467:
#line 9151 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    esq->flag |= ESQ_NOPRT;	/* Suppress printing */
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-3].s, yypvt[-2].s );
		}
		break;
	case 1473:
#line 9165 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1474:
#line 9169 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-2].i || yypvt[-0].i;	/* DEFAULT keyword */
		}
		break;
	case 1475:
#line 9174 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    id_free();
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1476:
#line 9179 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    id_free();
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1477:
#line 9185 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    id_free();
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1483:
#line 9224 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1484:
#line 9229 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1485:
#line 9233 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1486:
#line 9238 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		    esq->flag &= ~ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 1487:
#line 9243 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		    esq->flag &= ~ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 1488:
#line 9248 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		    esq->flag &= ~ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 1489:
#line 9253 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		    esq->flag |= ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 1490:
#line 9258 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		    esq->flag &= ~ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 1491:
#line 9264 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Assure we're following a constraint definition. */
		    if (!(esq->flag & ESQ_WITHABLE_CONSTRAINT))
		     er_write(E_EQ006B_WITH_NOCONS, EQ_ERROR, 0);
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		    yyval.i = CRE_CONWTH;
		}
		break;
	case 1493:
#line 9278 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-1].s, ERx("no")))
			er_write(E_EQ006A_grxBADWORD, EQ_ERROR, 2,
			    yypvt[-1].s, ERx("with no index"));
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-1].s);
			db_key(yypvt[-0].s);
		    }
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1494:
#line 9291 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-2].s, ERx("base")))
			er_write(E_EQ006A_grxBADWORD, EQ_ERROR, 2,
			    yypvt[-2].s, ERx("with index = base table structure"));
		    else if (!ESQ_EQSTR(yypvt[-0].s, ERx("structure")))
			er_write(E_EQ006A_grxBADWORD, EQ_ERROR, 2,
			    yypvt[-0].s, ERx("with index = base table structure"));
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-4].s);
			db_op(yypvt[-3].s);
			db_key(yypvt[-2].s);
			db_key(yypvt[-1].s);
			db_key(yypvt[-0].s);
		    }
		    yyval.i = yypvt[-4].i;
		}
		break;
	case 1495:
#line 9310 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1496:
#line 9314 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-1].i;
		}
		break;
	case 1497:
#line 9319 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
		    	db_key( yypvt[-1].s );
		    	db_key( yypvt[-0].s );
		    }
		}
		break;
	case 1498:
#line 9329 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
		    	db_key( yypvt[-1].s );
		    	db_key( yypvt[-0].s );
		    }
		}
		break;
	case 1499:
#line 9339 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = CRE_WDEF;
		}
		break;
	default:
		    goto label_1500;
	}

label_1500:
	switch (yym) {
	case 1500:
#line 9343 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = CRE_WDEF;
		}
		break;
	case 1501:
#line 9347 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = CRE_OK;
		}
		break;
	case 1502:
#line 9351 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = CRE_NDEF;
		}
		break;
	case 1503:
#line 9356 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
		    	db_key( yypvt[-1].s );
		    	db_key( yypvt[-0].s );
		    }
		}
		break;
	case 1504:
#line 9366 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    	db_key( yypvt[-0].s );
		}
		break;
	case 1505:
#line 9373 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
		    	db_key( yypvt[-1].s );
		    	db_key( yypvt[-0].s );
		    }
		}
		break;
	case 1507:
#line 9384 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    	db_key( yypvt[-0].s );
		}
		break;
	case 1509:
#line 9391 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    	db_sconst( yypvt[-0].s );
		}
		break;
	case 1510:
#line 9397 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    	db_key( yypvt[-0].s );
		}
		break;
	case 1511:
#line 9403 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    	db_key( yypvt[-0].s );
		}
		break;
	case 1513:
#line 9411 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
		    	db_key( yypvt[-1].s );
		    	db_key( yypvt[-0].s );
		    }
		    yyval.i = CRE_WSYS;
		}
		break;
	case 1514:
#line 9421 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
		    	db_key( yypvt[-1].s );
		    	db_key( yypvt[-0].s );
		    }
		    yyval.i = CRE_NSYS;
		}
		break;
	case 1522:
#line 9450 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("statement")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					    ERx("DECLARE"), ERx("STATEMENT") );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				1, ERx("DECLARE STATEMENT") );
		}
		break;
	case 1529:
#line 9479 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IISQEXIT );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ050A_FIPS_EXTSN, EQ_WARN,
					1, ERx("DISCONNECT") );
		}
		break;
	case 1530:
#line 9487 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-0].s, NULL );
		    esq_init();
		}
		break;
	case 1531:
#line 9493 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IISQSESS );
		}
		break;
	case 1532:
#line 9497 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add ( IIsqdisALL );	
		    gen_call( IISQSESS );
		}
		break;
	case 1533:
#line 9502 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* connection name */
		    gen_call( IISQCNNM );
		}
		break;
	case 1536:
#line 9520 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    eqgen_tl(IIGETDATA);
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		}
		break;
	case 1537:
#line 9527 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		    gr->gr_func = IIGETDATA;
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 
				1, ERx("GET DATA") );
		    eqtl_init(IIGETDATA);
		}
		break;
	case 1539:
#line 9541 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (STbcompare(yypvt[-2].s, 0, ERx("maxlength"), 9, TRUE) != 0)
		    {
			er_write( E_EQ006A_grxBADWORD, EQ_ERROR, 2, yypvt[-2].s,
                                          ERx("GET DATA"));
		    }
		    eqtl_add(gr->gr_id, (PTR)gr->gr_sym, gr->gr_nlid, 
			(PTR)gr->gr_nlsym, gr->gr_type, yypvt[-2].s);
		}
		break;
	case 1540:
#line 9562 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
                    eqgen_tl(IIPUTDATA);
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
                }
		break;
	case 1541:
#line 9569 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
                    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
                    gr->gr_func = IIPUTDATA;
                    if (eq->eq_flags & EQ_FIPS)
                        er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
                                1, ERx("PUT DATA") );
                    eqtl_init(IIPUTDATA);
                }
		break;
	case 1542:
#line 9587 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IIEVTSTAT );
		    gr->gr_flag |= GR_GETEVT;	/* Special WHENEVER handling */
		}
		break;
	case 1543:
#line 9593 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("GET DBEVENT"));
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
					1, ERx("GET DBEVENT") );
		    esq_init();
		    arg_int_add( 0 );	/* Called from user code */
		}
		break;
	case 1544:
#line 9606 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( 0 );	/* "no wait" */
		}
		break;
	case 1547:
#line 9613 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-1].s, ERx("WAIT")))
		    {
			er_write(E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
					ERx("GET DBEVENT"), ERx("WAIT"));
		    }
		}
		break;
	case 1548:
#line 9622 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (ESQ_EQSTR(yypvt[-0].s, ERx("WAIT")))
		    {
			arg_int_add( -1 );	/* Wait forever */
		    }
		    else 
		    {
			arg_int_add( 0 );	/* Don't wait */
		        if (!ESQ_EQSTR(yypvt[-0].s, ERx("NOWAIT")))
			    er_write(E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					ERx("GET DBEVENT"), ERx("NOWAIT"));
		    }
		}
		break;
	case 1553:
#line 9651 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    eqtl_add(gr->gr_id, (PTR)gr->gr_sym, gr->gr_nlid, (PTR)gr->gr_nlsym,
				gr->gr_type, yypvt[-0].s);
		    if ( (gr->gr_type == T_STRUCT) && (eq->eq_flags & EQ_FIPS) )
			er_write( E_EQ050D_FIPS_VAR, EQ_WARN, 0 );
		}
		break;
	case 1554:
#line 9659 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1555:
#line 9663 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1556:
#line 9667 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1557:
#line 9672 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
                       gr->gr_id = yypvt[-0].s;
                       gr->gr_type = T_INT;
                       gr->gr_sym = (SYM *)0;
                       gr->gr_nlsym = (SYM *)0;
		}
		break;
	case 1558:
#line 9679 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
                    if (gr->gr_sym)
                    {
                        if (gr->gr_type != T_INT)
                        {
                            er_write(E_EQ0061_grINTVAR, EQ_ERROR,1, gr->gr_id);
                        }
                    }
		    else
		    {
		       gr->gr_id = yypvt[-0].s;
                       gr->gr_type = T_NONE;
                       gr->gr_sym = (SYM *)0;
                       gr->gr_nlsym = (SYM *)0;
		    }
                }
		break;
	case 1559:
#line 9704 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( QAPRINTF );
		}
		break;
	case 1560:
#line 9709 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-0].s, NULL );
		}
		break;
	case 1564:
#line 9719 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym)
			arg_var_add( gr->gr_sym, gr->gr_id );
		    else
			arg_str_add( ARG_CHAR, gr->gr_id );
		}
		break;
	case 1567:
#line 9736 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
			eqgen_tl(IIINQSQL);
			if (eq->eq_flags & EQ_CHRPAD)
			    esq_sqmods(IImodNOCPAD);
		}
		break;
	case 1568:
#line 9743 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		    gr->gr_func = IIINQSQL;
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
					1, ERx("INQUIRE_INGRES") );
		    eqtl_init(IIINQSQL);
		}
		break;
	case 1569:
#line 9763 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
			eqgen_tl(IISETSQL);
		}
		break;
	case 1570:
#line 9767 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    eqtl_add(gr->gr_id, (PTR)gr->gr_sym, gr->gr_nlid, 
			 (PTR)gr->gr_nlsym, gr->gr_type, ERx("connection_name"));
		    eqgen_tl(IISETSQL);
		}
		break;
	case 1571:
#line 9773 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /*
		    ** Only NONE (unfortunately not a keyword) 
		    ** is permitted as connection identifier.
		    */
		    if ( ESQ_EQSTR( yypvt[-0].s, ERx("none") ) )
		    {
			/*
			** SET CONNECTION NONE is handled 
			** as SET_SQL( SESSION = NONE ).
			*/
			char session[ 8 ];

			/* see notes in SET_SQL( SESSION ) */
			if ( eq->eq_flags & EQ_SQLSTATE )
			{
			    gen_diaginit(); 
			}

			CVna( IIsqsetNONE, session );
			eqtl_add( session, NULL, 
				  NULL, NULL, T_INT, ERx("session") );
			eqgen_tl( IISETSQL );
		    }
		    else
		    {
			er_write( E_EQ0244_yySYNWRD, EQ_ERROR, 1, yypvt[-0].s );
		    }
		}
		break;
	case 1572:
#line 9804 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
			gr->gr_id = yypvt[-0].s;
			gr->gr_type = T_CHAR;
			gr->gr_sym = (SYM *)0;
			gr->gr_nlsym = (SYM *)0;
		}
		break;
	case 1574:
#line 9813 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-0].s, NULL );
		    gr->gr_func = IISETSQL;
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
					1, ERx("SET_INGRES") );
		    eqtl_init(IISETSQL);
		}
		break;
	case 1575:
#line 9823 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-0].s, NULL );
		    /*  ANSI syntax is SET CONNECTION connection name */
		    if (eq->eq_flags & EQ_SQLSTATE)
		    {
		        gen_diaginit();
		    }
		    gr->gr_func = IISETSQL;
		    /* SET CONNECTION is only valid in FULL ANSI SQL */
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
					1, ERx("SET CONNECTION") );
		    eqtl_init(IISETSQL);
		}
		break;
	case 1579:
#line 9844 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		  /* 
		  ** We need to "gen_diaginit" to allow setting of the SQLSTATE 
		  ** for all SQL statements.  In all but one of these 
		  ** statements, we conveniently call esq_init.  So we 
		  ** gen_diaginit in there.  However,  we don't esq_init
		  ** for a SET_INGRES (SESSION = x), so we need to 
		  ** gen_diaginit here as well.  Note that SQLSTATE is valid
		  ** outside of the context of a connection; all statements
		  ** that affect a connect need to "gen_diaginit".
		  ** We also need to handle the special case of NONE
		  ** for the session ID.
		  */
		  if (!STbcompare( ERx("session"), 0, yypvt[-2].s, 0, TRUE))
		  {
		      if (eq->eq_flags & EQ_SQLSTATE)
		      {
		  	gen_diaginit();
		      }
		      if ( gr->gr_type == T_NONE  &&
		           ESQ_EQSTR( gr->gr_id, ERx("none") ) )
		      {
			  /*
			  ** Produce internal session ID for NONE.
			  */
			  static char session[ 8 ];
			  CVna( IIsqsetNONE, session );
			  gr->gr_id = session;
			  gr->gr_type = T_INT;
		      }
		  }
		  eqtl_add(gr->gr_id, (PTR)gr->gr_sym, gr->gr_nlid, 
			     (PTR)gr->gr_nlsym, gr->gr_type, yypvt[-2].s);
		}
		break;
	case 1580:
#line 9880 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
			gr->gr_id = yypvt[-0].s;
			gr->gr_type = T_INT;
			gr->gr_sym = (SYM *)0;
			gr->gr_nlsym = (SYM *)0;
		}
		break;
	case 1581:
#line 9887 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
			gr->gr_id = yypvt[-0].s;
			gr->gr_type = T_CHAR;
			gr->gr_sym = (SYM *)0;
			gr->gr_nlsym = (SYM *)0;
		}
		break;
	case 1583:
#line 9895 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
			gr->gr_id = yypvt[-0].s;
			gr->gr_type = T_NONE;
			gr->gr_sym = (SYM *)0;
			gr->gr_nlsym = (SYM *)0;
		}
		break;
	case 1584:
#line 9916 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-0].i && (esq->inc & sqcaSQL) == 0)
		    {
			er_write( E_EQ0128_sqNOSQLCA, EQ_ERROR, 0 );
			esq->inc |= sqcaSQL;
		    }
		}
		break;
	case 1585:
#line 9926 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    esq->flag |= ESQ_NOPRT;	/* Suppress printing */
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-0].s, NULL );
		}
		break;
	case 1586:
#line 9934 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (ESQ_EQSTR(yypvt[-0].s, ERx("sqlerror")))
		    {
			yyval.i = sqcaERROR;
		    }
		    else if (ESQ_EQSTR(yypvt[-0].s, ERx("sqlwarning")))
		    {
			yyval.i = sqcaWARNING;
			if (eq->eq_flags & EQ_FIPS)
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
					2, ERx("SQLWARNING"), ERx("WHENEVER") );
		    }
		    else if (ESQ_EQSTR(yypvt[-0].s, ERx("sqlmessage")))
		    {
			yyval.i = sqcaMESSAGE;
			if (eq->eq_flags & EQ_FIPS)
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
					2, ERx("SQLMESSAGE"), ERx("WHENEVER") );
		    }
		    else if (ESQ_EQSTR(yypvt[-0].s, ERx("dbevent")))
		    {
			yyval.i = sqcaEVENT;
			if (eq->eq_flags & EQ_FIPS)
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
					2, ERx("DBEVENT"), ERx("WHENEVER") );
		    }
		    else
		    {
			er_write( E_EQ0130_sqWCOND, EQ_ERROR, 1, yypvt[-0].s );
			yyval.i = sqcaDEFAULT;
		    }
		}
		break;
	case 1587:
#line 9967 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (ESQ_EQSTR(yypvt[-0].s, ERx("found")))
			yyval.i = sqcaNOTFOUND;
		    else
		    {
			STprintf( esq->sbuf, ERx("%s %s"), yypvt[-1].s, yypvt[-0].s );
			er_write( E_EQ0130_sqWCOND, EQ_ERROR, 1, esq->sbuf );
			yyval.i = sqcaDEFAULT;
		    }
		}
		break;
	case 1588:
#line 9983 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    esqlca( yypvt[-1].i, sqcaCONTINUE, (char *)0 );
		    yyval.i = 0;
		}
		break;
	case 1589:
#line 9988 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    esqlca( yypvt[-3].i, sqcaGOTO, yypvt[-0].s );
		    yyval.i = 1;
		}
		break;
	case 1590:
#line 9993 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    esqlca( yypvt[-2].i, sqcaGOTO, yypvt[-0].s );
		    yyval.i = 1;
		}
		break;
	case 1591:
#line 9998 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    esqlca( yypvt[-2].i, sqcaCALL, yypvt[-0].s );
		    yyval.i = 1;
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				    2, ERx("CALL"), ERx("WHENEVER") );
		}
		break;
	case 1592:
#line 10006 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-1].i == sqcaNOTFOUND)
		    {
			er_write( E_EQ0131_sqWSTOP, EQ_ERROR, 0 );
			esqlca( yypvt[-1].i, sqcaCONTINUE, (char *)0 );
			yyval.i = 0;
		    }
		    else
		    {
			esqlca( yypvt[-1].i, sqcaSTOP, (char *)0 );
			yyval.i = 1;
		    }
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				    2, ERx("STOP"), ERx("WHENEVER") );
		}
		break;
	case 1593:
#line 10037 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    arg_int_add( yypvt[-0].i );		/* using names? */
		    gen_call( IISQDESCRIBE );
		}
		break;
	case 1594:
#line 10044 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s, NULL );
		    esq_init();
		    gr->gr_flag |= GR_NOSYNC;
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("DESCRIBE") );
		}
		break;
	case 1598:
#line 10061 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = IIsqdNULUSE;
		}
		break;
	case 1599:
#line 10065 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (ESQ_EQSTR(yypvt[-0].s, ERx("names")))
			yyval.i = IIsqdNAMUSE;
		    else if (ESQ_EQSTR(yypvt[-0].s, ERx("adftypes")))
			yyval.i = IIsqdADFUSE;
		    else
		    {
			yyval.i = IIsqdNAMUSE;
			er_write(E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					ERx("PREPARE/DESCRIBE"), ERx("NAMES"));
		    }
		}
		break;
	case 1600:
#line 10090 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IISQDESCINPUT );
		}
		break;
	case 1601:
#line 10096 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s, NULL );
		    esq_init();
		    gr->gr_flag |= GR_NOSYNC;
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		}
		break;
	case 1602:
#line 10167 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("EXECUTE IMMEDIATE") );
		}
		break;
	case 1603:
#line 10173 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("EXECUTE") );
		}
		break;
	case 1604:
#line 10180 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s, NULL );
		    esq_init();
		    if (eq->eq_flags & EQ_ENDIF)
		    {
			gr->gr_flag |= GR_ENDIF;
			eq->eq_flags &= ~EQ_ENDIF;
		    }	
		}
		break;
	case 1605:
#line 10191 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IISQEXIMMED );
		}
		break;
	case 1606:
#line 10195 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_flag |= GR_NOSYNC;
		    esq->flag &= ~ESQ_DESC_SEL;
		}
		break;
	case 1607:
#line 10201 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_CHAR, form_sconst(yypvt[-0].s) );
		}
		break;
	case 1608:
#line 10205 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym)
		    {
			if (gr->gr_type == T_CHAR)
			    arg_var_add( gr->gr_sym, gr->gr_id );
			else
			{
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0067_grSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
			    arg_str_add( ARG_CHAR, gr->gr_id );
			}
		    } else	/* Error in host_fsetvar */
			arg_str_add( ARG_CHAR, gr->gr_id );
		}
		break;
	case 1609:
#line 10222 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    esq_sqmods(IImodDYNSEL);
		    esq->flag |= ESQ_REG_SELECT;
		    esl_query( ESL_START_QUERY );
		    gen_call( IISQEXIMMED );
		}
		break;
	case 1610:
#line 10230 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-0].i)
		    {
		      /* SELECT LOOP - gen WHILE */
			/* Close retrieve loop */
			gen_loop( G_CLOSE, L_RETBEG, L_RETFLUSH,
				    lbl_current(LBL_RET), IINEXTGET, C_0, 0 );
			gen_call( IIFLUSH );
			gen_label( G_NOLOOP, L_RETEND, lbl_current(LBL_RET) );
			gr->gr_flag &= ~GR_RETRIEVE;/* Done with SELECT loop */
		    } else
		    {
		      /* SINGLETON SELECT - gen IF */
			if (eq->eq_flags & EQ_CHREOS)
			    esq_eoscall(IImodNOCEOS);
			gen_if( G_OPEN, IINEXTGET, C_NOTEQ, 0 );
			if (eq->eq_flags & EQ_CHRPAD)
			    esq_sqmods(IImodCPAD);
			if (esq->flag & ESQ_DESC_SEL)
			    erec_desc( NULL );  /* Descriptor retrieval */
			else
			    ret_close();	    /* Variable retrieval */
			if (eq->eq_flags & EQ_CHRPAD)
			    esq_sqmods(IImodNOCPAD);
			gen_if( G_CLOSE, IINEXTGET, C_0, 0 );
			gen_call( IISQFLUSH );
		    }
		}
		break;
	case 1611:
#line 10260 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    erec_desc( gr->gr_id );	    /* Save arg for IIcsDaGet */
		    esq->flag |= ESQ_DESC_SEL;
		}
		break;
	case 1612:
#line 10265 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    erec_setup( 0 );			/* Done with list */
		}
		break;
	case 1613:
#line 10270 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( 0 );			/* not using vars */
		    gen_call( IISQEXSTMT );
		}
		break;
	case 1615:
#line 10276 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IISQDAIN );
		}
		break;
	case 1616:
#line 10283 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( 1 );			/* using vars */
		    gen_call( IISQEXSTMT );
		    gr->gr_func = tEXECUTE;
		}
		break;
	case 1617:
#line 10290 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( 0 );			/* not using vars */
		    gen_call( IISQEXSTMT );
		}
		break;
	case 1620:
#line 10299 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_var( DB_REG, gr->gr_sym, gr->gr_id,
			    gr->gr_nlsym, gr->gr_nlid, (char *)0 );
		}
		break;
	case 1621:
#line 10304 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_var( DB_HDLR, gr->gr_sym, gr->gr_id,
			    gr->gr_nlsym, gr->gr_nlid, yypvt[-1].s );
		}
		break;
	case 1622:
#line 10345 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-2].i)
		    {
			if (yypvt[-0].i == 0)
			{
			    i4	cur_ret;
			    /* No block - "singleton" execute. Insert
			    ** IInextget after "if" followed by 
			    ** getdomio's. */	
			    gen_if( G_OPEN, IINEXTGET, C_NOTEQ, 0 );
			    ret_close();
			    gen_if( G_CLOSE, IINEXTGET, C_0, 0 );
			    gen_call( IISQFLUSH );
			    _VOID_ lbl_next( LBL_RET );
			    cur_ret = lbl_current( LBL_RET );
			    gen_goto( G_TERM, L_RETEND, lbl_current(LBL_RET));
			}
			gen_loop( G_CLOSE,L_RETBEG, L_RETFLUSH, 
			    lbl_current(LBL_RET), IIPROCSTAT, C_NOTEQ, 0 );
			gen_call( IIFLUSH );
			gen_label( G_NOLOOP, L_RETEND, lbl_current(LBL_RET) );
			erec_setup( 0 );	/* Done with list */
		    }
		}
		break;
	case 1623:
#line 10370 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-3].i)
		    {
			if (yypvt[-0].i == 0)
			{
			    i4	cur_ret;
			    /* No block - "singleton" execute. Insert
			    ** IInextget after "if" followed by 
			    ** getdomio's. */	
			    gen_if( G_OPEN, IINEXTGET, C_NOTEQ, 0 );
			    ret_close();
			    gen_if( G_CLOSE, IINEXTGET, C_0, 0 );
			    gen_call( IISQFLUSH );
			    _VOID_ lbl_next( LBL_RET );
			    cur_ret = lbl_current( LBL_RET );
			    gen_goto( G_TERM, L_RETEND, lbl_current(LBL_RET));
			}
			gen_loop( G_CLOSE,L_RETBEG, L_RETFLUSH, 
			    lbl_current(LBL_RET), IIPROCSTAT, C_NOTEQ, 0 );
			gen_call( IIFLUSH );
			gen_label( G_NOLOOP, L_RETEND, lbl_current(LBL_RET) );
			erec_setup( 0 );	/* Done with list */
		    }
		    arg_int_add( 0 );

		    if (tempgr.gr_type != T_INT)
			er_write( E_EQ0061_grINTVAR, EQ_ERROR, 1, tempgr.gr_id );

		    arg_var_add(  tempgr.gr_sym, tempgr.gr_id );
		    arg_str_add( ARG_CHAR, ERx("iiret"));
		    gen_call( IIEQINQ );
		}
		break;
	case 1624:
#line 10405 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Copy (*gr) for "proc_exec" to build code. */
		    STRUCT_ASSIGN_MACRO(*gr, tempgr);
		}
		break;
	case 1627:
#line 10417 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_flag |= (GR_NOSYNC | GR_PROCEXEC );
		    STprintf( esq->sbuf, ERx("%s %s"), yypvt[-1].s, yypvt[-0].s );
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, esq->sbuf, NULL );
		    esq_init();
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("EXECUTE PROCEDURE") );
		    arg_int_add( IIPROC_EXEC );
		    gr->gr_rcnt = 0;
		}
		break;
	case 1628:
#line 10430 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IIPROCINIT );
		}
		break;
	case 1631:
#line 10438 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IIPROCGTTP );
		}
		break;
	case 1632:
#line 10442 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (esq->flag & ESQ_PROC_BYREF)
		    {
			/* procs w/BYREF params act like singleton select */
			esq_sqmods(IImodSINGLE);
		    }
		}
		break;
	case 1633:
#line 10450 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IISQDAIN );
		}
		break;
	case 1638:
#line 10464 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IIPROCVALIO );
		}
		break;
	case 1642:
#line 10474 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
			arg_int_add( TRUE );
		}
		break;
	case 1643:
#line 10479 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( FALSE );
		}
		break;
	case 1644:
#line 10484 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Null indicator argument */
		    if (gr->gr_nlsym)
			arg_var_add( gr->gr_nlsym, gr->gr_nlid );
		    else
			arg_int_add( 0 );
		    if (gr->gr_sym)
			arg_var_add( gr->gr_sym, gr->gr_id );
		    else        /* Error in host_fsetvar */
			arg_str_add( ARG_CHAR, gr->gr_id );
		    /* add var info to ret que */
		    ret_add( gr->gr_sym, gr->gr_id,
			     gr->gr_nlsym, gr->gr_nlid, NULL, 0);
		    /* there are BYREF params */
		    esq->flag |= ESQ_PROC_BYREF;
		}
		break;
	case 1647:
#line 10515 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (esq->flag & ESQ_PROC_RESULT)
		    {	/* only if there's a result clause */

			/* Add all the INTO variables and indicators */
			if ((esq->flag & ESQ_DESC_SEL) == 0)
			    erec_use( IIRETDOM, FALSE, NULL );

			if (esq->flag & ESQ_REG_SELECT)
			{
			    esl_query( ESL_SNGL_STOP_QUERY );   /* Dump the query */
			    esq->flag &= ~ESQ_REG_SELECT;
			}
			yyval.i = 0;
		    }
		}
		break;
	case 1648:
#line 10532 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    {
		      /* EXECUTE LOOP - close it off */
			gen_loop( G_CLOSE, L_RETBEG, L_RETFLUSH,
				    lbl_current(LBL_RET), IINEXTGET, C_0, 0 );
			gr->gr_flag &= ~GR_RETRIEVE;/* Done with SELECT loop */
		    } 
		    yyval.i = 1;
		}
		break;
	case 1649:
#line 10544 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4		cur_ret;

		    if (!(esq->flag & ESQ_PROC_RESULT))
			er_write( E_EQ005B_grEXLOOP, EQ_ERROR, 0);
				/* can't have block with no RESULT ROW */
		    if (eq->eq_flags & EQ_CHREOS)
			 esq_eoscall(IImodNOCEOS);

		    /* Add all the INTO variables and indicators */
		    if ((esq->flag & ESQ_DESC_SEL) == 0)
			erec_use( IIRETDOM, FALSE, NULL );

		    if (esq->flag & ESQ_REG_SELECT)
		    {
			esl_query( ESL_BULK_STOP_QUERY );   /* Dump the query */
			esq->flag &= ~ESQ_REG_SELECT;
		    }

		    /* Here's generic block-begin code */
		    _VOID_ lbl_next( LBL_RET );
		    lbl_enter_block();
		    lbl_set_mode( LBLmEXECLOOP );
		    lbl_adset( LBL_NOLEV, gr->gr_adjective );
		    gr->gr_mode = 0;
		    gr->gr_adjective = 0;
		    gr->gr_flag &= ~(GR_SQL|GR_ESQL);

		    /* Here's SELECT-specific block-begin code */
		    cur_ret = lbl_current( LBL_RET );
		    gen_if_goto( IIERRTEST, C_NOTEQ, 0, L_RETEND, cur_ret );
		    /* Open retrieve loop */
		    gen_label( G_LOOP, L_RETBEG, cur_ret );
		    gen_loop( G_OPEN, L_RETBEG, L_RETFLUSH, cur_ret,
			IINEXTGET, C_NOTEQ, 0 );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		    if (esq->flag & ESQ_DESC_SEL)
			/* Descriptor retrieval */
			erec_desc( NULL );
		    else
			ret_close();		/* Variable retrieval */
		    if (eq->eq_flags & EQ_CHRPAD)
			 esq_sqmods(IImodNOCPAD);
		    gen_if_goto( IIERRTEST, C_NOTEQ, 0, L_RETBEG, cur_ret );
		    gr->gr_flag &= ~GR_SQL;
		    gr->gr_flag |= GR_RETRIEVE; /* Prevent nesting queries */
		    if (eq->eq_flags & EQ_FIPS) 
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2, 
			        ERx("BEGIN"), ERx("EXECUTE") );
		}
		break;
	case 1650:
#line 10598 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    lbl_exit_block();
		}
		break;
	case 1651:
#line 10618 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( 0 );
		    gen_loop( G_OPEN, L_RETBEG, L_RETFLUSH, 
			    lbl_next(LBL_RET), IIPROCSTAT, C_NOTEQ, 0 );
		    gen_sqlca( sqcaSQPROC );
		    if (gr->gr_id && eq->eq_lang == EQ_COBOL )
			arg_int_add( 0 );
		    gen_loop( G_CLOSE,L_RETBEG, L_RETFLUSH, 
			    lbl_current(LBL_RET), IIPROCSTAT, C_NOTEQ, 0 );

		    /* If BYREF parms, emit that code, too. */
		    if (esq->flag & ESQ_PROC_BYREF)
		    {
			/* just like in singleton selects */
			gen_if( G_OPEN, IINEXTGET, C_NOTEQ, 0 );
			/* Generate byref variable loop */
			ret_close();
			gen_if( G_CLOSE, IINEXTGET, C_0, 0 );
			gen_call( IISQFLUSH );
		    }
		    yyval.i = 0;		/* indicate no result clause */
		}
		break;
	case 1652:
#line 10641 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( gr->gr_rcnt );
		    gen_loop( G_OPEN, L_RETBEG, L_RETFLUSH, 
			    lbl_next(LBL_RET), IIPROCSTAT, C_NOTEQ, 0 );
		    gen_sqlca( sqcaSQPROC );
		    yyval.i = 1;		/* indicate result clause */
		}
		break;
	case 1653:
#line 10650 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (esq->flag & ESQ_PROC_BYREF)
			er_write( E_EQ005A_grBYREF, EQ_ERROR, 0);
				/* can't have both BYREF parm & RESULT ROW */
		    erec_setup( 1 );	/* Initialize struct indicators stuff */
		    esq->flag |= ESQ_PROC_RESULT;  /* & set flag */
		}
		break;
	case 1656:
#line 10662 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    erec_setup( 0 );	/* Done with one element */
		}
		break;
	case 1657:
#line 10667 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_rcnt++;
		}
		break;
	case 1658:
#line 10681 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IISQPREPARE );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("PREPARE") );
		}
		break;
	case 1659:
#line 10690 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_id && eq->eq_lang == EQ_FORTRAN )
			arg_str_add( ARG_PSTRUCT, gr->gr_id );
		    else if (gr->gr_id)			/* sqlda */
			arg_str_add( ARG_RAW, gr->gr_id );
		    else
			arg_str_add( ARG_CHAR, (char *)0 );
		    arg_int_add( yypvt[-0].i );		/* using names? */
		}
		break;
	case 1660:
#line 10701 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s, NULL );
		    esq_init();
		    gr->gr_flag |= GR_NOSYNC;
		    arg_int_add( eq_genlang(eq->eq_lang) );  /* host language */
		}
		break;
	case 1661:
#line 10709 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_id = NULL;
		    yyval.i = 0;
		}
		break;
	case 1662:
#line 10714 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1664:
#line 10721 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym)
		    {
			if (gr->gr_type != T_CHAR)
			{
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0067_grSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
			}
		    }
		    arg_var_add( gr->gr_sym, gr->gr_id );
		}
		break;
	case 1665:
#line 10734 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_CHAR, form_sconst(yypvt[-0].s) );
		}
		break;
	case 1669:
#line 10763 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("DIRECT CONNECT") );
		}
		break;
	case 1670:
#line 10771 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("DIRECT DISCONNECT") );
		}
		break;
	case 1671:
#line 10779 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("DIRECT EXECUTE IMMEDIATE") );
		}
		break;
	case 1673:
#line 10788 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym)
		    {
			db_var( DB_STRING, gr->gr_sym, gr->gr_id,
				(SYM *)0, (char *)0, (char *)0 );
			if (gr->gr_type != T_CHAR && gr->gr_type != T_UNDEF)
			    er_write( E_EQ0067_grSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
		    }
		    else	/* Error caught in host_setvar */
			db_sconst( gr->gr_id );
		}
		break;
	case 1715:
#line 10879 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    lbl_exit_block();
		}
		break;
	case 1716:
#line 10884 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* This token is returned via scan_ahead - see esqgtoks.c */
		    lbl_enter_block();
		    lbl_set_mode( gr->gr_mode );
		    lbl_adset( LBL_NOLEV, gr->gr_adjective );
		    gr->gr_adjective = LBLaNONE;
		    gr->gr_mode = 0;
		    gr->gr_flag &= ~(GR_QUEL|GR_SQL|GR_EQUEL);
		}
		break;
	case 1718:
#line 10896 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 1720:
#line 10912 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IIADDFORM );
		    gr->gr_flag &= ~GR_NOINDIR;
		}
		break;
	case 1721:
#line 10918 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gr->gr_flag |= GR_NOINDIR;
		}
		break;
	case 1722:
#line 10924 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
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
			else if (gr->gr_type != T_INT && gr->gr_type != T_UNDEF)
			    er_write( E_EQ0061_grINTVAR, EQ_ERROR, 1,
								    gr->gr_id );
		    }	
		    else		/* Error already sent if not variable */
			arg_str_add( ARG_CHAR, gr->gr_id );
		}
		break;
	case 1723:
#line 10943 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
# ifndef EQ_ADD_FNAME
		    er_write( E_EQ0061_grINTVAR, EQ_ERROR, 1, yypvt[-0].s );
# endif /* EQ_ADD_FNAME */
		}
		break;
	case 1724:
#line 10959 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  form_no;

		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    if ((form_no = lbl_gen(LBLmDISPLAY, TRUE)) != LBL_NOLEV)
			gen_goto( G_IF, L_FDEND, 
				  lbl_value(LBLmDISPLAY, form_no) );
		    else
			er_write( E_EQ0058_grINLOOP, EQ_ERROR, 2, yypvt[-0].s,
							    ERx("DISPLAY") );
		}
		break;
	case 1725:
#line 10980 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-1].s, NULL );
		    gen_call( IICLRSCR );
		}
		break;
	case 1726:
#line 10985 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IICLRFLDS );
		}
		break;
	case 1728:
#line 10991 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-1].s, NULL );
		    gr->gr_func = IIFLDCLEAR;
		}
		break;
	case 1729:
#line 11023 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IIFRSQDESC );
		}
		break;
	case 1730:
#line 11029 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IIFRSQDESC );
		}
		break;
	case 1731:
#line 11035 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_int_add( 1 );			/* is_form = TRUE */
		}
		break;
	case 1732:
#line 11042 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_CHAR, (char* )0 );	/* No table name */
		}
		break;
	case 1733:
#line 11047 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    STprintf( esq->sbuf, ERx("%s %s"), yypvt[-1].s, yypvt[-0].s );
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, esq->sbuf, NULL );
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_int_add( 0 );			/* is_form = FALSE */
		}
		break;
	case 1734:
#line 11055 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_CHAR, ERx("u") );
		}
		break;
	case 1735:
#line 11059 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_CHAR, ERx("a") );
		}
		break;
	case 1736:
#line 11063 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (*(yypvt[-0].s) == EOS)		/* Empty string? */
		    {
			arg_str_add( ARG_CHAR, ERx("a") );
		    }
		    else if (STbcompare( ERx("update"), 0, yypvt[-0].s, 0, TRUE) == 0)
		    {
			arg_str_add( ARG_CHAR, ERx("u") );
		    }
		    else if (STbcompare( ERx("query"), 0, yypvt[-0].s, 0, TRUE) == 0)
		    {
			arg_str_add( ARG_CHAR, ERx("q") );
		    }
		    else if (STbcompare( ERx("all"), 0, yypvt[-0].s, 0, TRUE) == 0)
		    {
			arg_str_add( ARG_CHAR, ERx("a") );
		    }
		    else
		    {
			frs_error( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
				   ERx("FRS DESCRIBE"), ERx("ALL"));
			arg_str_add( ARG_CHAR, ERx("a") );
		    }
		}
		break;
	case 1737:
#line 11088 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym)
		    {
			if (gr->gr_type == T_CHAR)
			    arg_var_add( gr->gr_sym, gr->gr_id );
			else
			{
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0067_grSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
			    arg_str_add( ARG_CHAR, gr->gr_id );
			}
		    }
		    else
		    /* Error should already have been reported */
		    {
			arg_str_add( ARG_CHAR, ERx("a") );
		    }
		}
		break;
	case 1738:
#line 11108 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_CHAR, ERx("a") );
		}
		break;
	case 1739:
#line 11113 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1740:
#line 11117 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1741:
#line 11183 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  cur_form = lbl_current( LBL_FORM );

		    gen_label( G_NOLOOP, L_MUINIT, cur_form );
		    act_close( cur_form );
		    gen_goto( G_TERM, L_FDINIT, cur_form );
		    gen_label( G_NOLOOP, L_FDEND, cur_form );
		    if (yypvt[-3].i)				/* display submenu */
			gen_call( IIENDNEST );
		    else
			gen_call( IIENDFRM );
		    lbl_exit_block();
		}
		break;
	case 1742:
#line 11217 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  cur_form;

		    cur_form = lbl_next( LBL_FORM );
		    lbl_enter_block();
		  /* fake a forms BEGIN/END block */
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
	case 1743:
#line 11236 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{		/* display_key sets USEMODE *now* */
		    i4  cur_form;

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
	case 1744:
#line 11255 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gr->gr_func = IIDISPFRM;
		}
		break;
	case 1745:
#line 11261 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    frs_mode( ERx("DISPLAY/INITTABLE"), yypvt[-0].s );
		}
		break;
	case 1746:
#line 11265 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    frs_mode( ERx("DISPLAY/INITTABLE"), yypvt[-0].s );
		}
		break;
	case 1747:
#line 11269 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    frs_mode( ERx("DISPLAY/INITTABLE"), yypvt[-0].s );
		}
		break;
	case 1748:
#line 11273 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_CHAR, ERx("u") );
		}
		break;
	case 1749:
#line 11277 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym)
		    {
			if (gr->gr_type == T_CHAR)
			    arg_var_add( gr->gr_sym, gr->gr_id );
			else
			{
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0067_grSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
			    arg_str_add( ARG_CHAR, gr->gr_id );
			}
		    }
		    else	/* Error in host_setvar */
			frs_mode( ERx("DISPLAY/INTTABLE"), gr->gr_id );
		}
		break;
	case 1750:
#line 11294 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_CHAR, ERx("f") );
		}
		break;
	case 1751:
#line 11299 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    sc_eat( NULL, SC_NEST|SC_SEEN, ERx(")"), '(', ')' );
		    er_write( E_EQ0051_grDISPTL, EQ_ERROR, 0 );
		}
		break;
	case 1753:
#line 11327 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  func = IIRUNFRM;
		    i4  cur_form = lbl_current( LBL_FORM );

		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_label( G_LOOP, L_FDBEG, cur_form );
		    if (yypvt[-1].i)		/* disp_head was DISPLAY SUBMENU */
			func = IIRUNNEST;
		    gen_loop( G_OPEN, L_FDBEG, L_FDFINAL, cur_form, func, 
			      C_NOTEQ, 0 );
		}
		break;
	case 1756:
#line 11344 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (dml->dm_exec != (DML_EXEC|DML__FRS))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("FRS"),
									yypvt[-0].s );
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_if_goto( IIFSETIO, C_EQ, 0, L_FDEND,
			lbl_current(LBL_FORM) );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		    gr->gr_func = IISETFIELD;	/* In case of I/O */
		    gr->gr_mode = LBLmDISPLAY;
		}
		break;
	case 1759:
#line 11394 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  cur_form = lbl_current( LBL_FORM );

		    gen_else( G_OPEN, II0, C_0, 0, L_0, 0, L_0, 0 ); 
		    gen_goto( G_IF, L_FDEND, cur_form );
		    gen_else( G_CLOSE, IIRETVAL, C_0, 0, L_0, 0, 
			      L_ACTEND, cur_form );
		    gr->gr_flag |= GR_ACTIVATE;
		}
		break;
	case 1760:
#line 11404 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_flag &= ~GR_ACTIVATE;
		}
		break;
	case 1763:
#line 11412 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_else( G_ELSE, IIRETVAL, C_0, 0, L_ACTELSE, 
			lbl_current(LBL_INTR), L_ACTEND, lbl_current(LBL_FORM));
		    gr->gr_mode = LBLmDISPLAY;
		}
		break;
	case 1764:
#line 11419 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  cur_intr = lbl_next( LBL_INTR );

		    if (dml->dm_exec != (DML_EXEC|DML__FRS))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("FRS"),
									yypvt[-0].s );
		    gen_else( G_OPEN, IIRETVAL, C_EQ, cur_intr, L_ACTELSE,
			cur_intr, L_ACTEND, lbl_current(LBL_FORM) );
		    gr->gr_mode = LBLmDISPLAY;
		    /* Reset activate argument list */
		    act_args( FALSE, (char *)0, (char *)0, 0, (SYM *)0 );
		}
		break;
	case 1765:
#line 11433 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Reset activate argument list */
		    act_args( FALSE, (char *)0, (char *)0, 0, (SYM *)0 );
		}
		break;
	case 1768:
#line 11447 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    act_add( ACT_FIELD, lbl_current(LBL_INTR), yypvt[-2].i, yypvt[-0].ip );
		    gr->gr_adjective = LBLaFIELD;
		}
		break;
	case 1769:
#line 11452 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    act_add( ACT_MENU, lbl_current(LBL_INTR), 0, yypvt[-1].ip );
		    gr->gr_adjective = LBLaMITEM;
		}
		break;
	case 1770:
#line 11457 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    act_add( ACT_SCROLL, lbl_current(LBL_INTR), sclUP, yypvt[-0].ip );
		    gr->gr_adjective = LBLaSCROLL;
		}
		break;
	case 1771:
#line 11462 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    act_add( ACT_SCROLL,lbl_current(LBL_INTR), sclDOWN, yypvt[-0].ip );
		    gr->gr_adjective = LBLaSCROLL;
		}
		break;
	case 1772:
#line 11467 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  cur_intr = lbl_current( LBL_INTR );

		    act_add( ACT_COLUMN, cur_intr, yypvt[-3].i, yypvt[-1].ip );
		    act_add( ACT_COLUMN, cur_intr, yypvt[-3].i, yypvt[-0].ip );
		    gr->gr_adjective = LBLaCOLUMN;
		}
		break;
	case 1773:
#line 11475 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Old-style syntax - no longer supported */
                    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1,
                                            ERx("ACTIVATE COMMAND") );
		}
		break;
	case 1775:
#line 11483 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 1776:
#line 11487 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
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
	case 1777:
#line 11502 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.ip = act_string( form_sconst(yypvt[-0].s) );
		}
		break;
	case 1778:
#line 11506 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.ip = act_string( yypvt[-0].s );
		}
		break;
	case 1779:
#line 11510 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.ip = act_string( yypvt[-0].s );
		}
		break;
	case 1780:
#line 11514 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym)
		    {
			yyval.ip = act_var( gr->gr_sym, gr->gr_id );
			if (gr->gr_type != T_CHAR && gr->gr_type != T_UNDEF)
			    er_write( E_EQ0067_grSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
		    }
		    else	/* Error in host_setvar */
			yyval.ip = act_string( gr->gr_id );
		}
		break;
	case 1781:
#line 11527 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1782:
#line 11531 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1783:
#line 11545 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
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
    		    else if (STbcompare(s, 5, ERx("dbevent"), 5, TRUE) == 0)
    		    {
    			act_add(ACT_EVENT, lbl_current(LBL_INTR),0,(i4 *)0);
			/* Is this the only act_elm in this ACTIVATE? */
			if (gr->gr_adjective == LBLaNONE)
			    gr->gr_adjective = LBLaEVENT;
    			if (yypvt[-0].i == 1)
			{
    			    er_write( E_EQ0309_actTARGET, EQ_ERROR, 1,
    				    ERx("DBEVENT") );
    			}
			/* Activate dbevent is not allowed in a non-display
			** submenu block.
			*/
			if ((gr->gr_mode == LBLmSUBMENU))
			    er_write( E_EQ0058_grINLOOP, EQ_ERROR, 2,
				    ERx("ACTIVATE DBEVENT"), ERx("DISPLAY") );
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
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, s,
				     ERx("COMMAND"), ERx("ACTIVATE") );
		    }
		    else
		    {
			er_write( E_EQ0301_actBAD, EQ_ERROR, 1, s );
		    }
		}
		break;
	case 1784:
#line 11604 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
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
	case 1785:
#line 11623 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
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
	case 1786:
#line 11637 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
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
	case 1787:
#line 11653 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 1788:
#line 11657 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 1791:
#line 11665 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* It will check for errors */
		    act_args( TRUE, yypvt[-2].s, yypvt[-0].s, gr->gr_type, gr->gr_sym );
		}
		break;
	case 1792:
#line 11671 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1793:
#line 11675 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1794:
#line 11679 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1795:
#line 11683 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1796:
#line 11688 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = form_sconst(yypvt[-0].s);
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = NULL;
		}
		break;
	case 1797:
#line 11694 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_INT;
		    gr->gr_sym = NULL;
		}
		break;
	case 1798:
#line 11700 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = gr->gr_id;
		    if (gr->gr_sym)
		    {
			if (gr->gr_type != T_CHAR && gr->gr_type != T_INT &&
			    gr->gr_type != T_UNDEF)
			    er_write( E_EQ0060_grINTSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
		    }
		    else
			gr->gr_type = T_CHAR;
		}
		break;
	case 1799:
#line 11727 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_goto( G_TERM, L_FDEND, lbl_current(LBL_FORM) );
		}
		break;
	case 1800:
#line 11732 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  func = IIRUNFRM;
		    i4  cur_form = lbl_current( LBL_FORM );

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
	case 1803:
#line 11750 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (dml->dm_exec != (DML_EXEC|DML__FRS))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("FRS"),
									yypvt[-0].s );
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_if_goto( IIFSETIO, C_EQ, 0, L_FDEND,
			lbl_current(LBL_FORM) );
		    gr->gr_func = IIRETFIELD;
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		}
		break;
	case 1804:
#line 11775 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		  if (dml->dm_exec & DML__FRS)
                  {
		    i4  form_no;

		    gr_mechanism( GR_EQSTMT, GR_sFORMS, ERx("frs"), yypvt[-0].s );
		    gr->gr_flag &= ~GR_ESQL;
		    if ((form_no = lbl_gen(LBLmFORMDATA, TRUE)) != LBL_NOLEV)
			gen_goto( G_IF, L_FDEND, 
				  lbl_value(LBLmFORMDATA, form_no) );
		    else
			er_write( E_EQ0058_grINLOOP, EQ_ERROR, 2, yypvt[-0].s,
							    ERx("FORMDATA") );
		  }
		  else
		  {
                    gr->gr_flag |= GR_NOSYNC;
                    gr_mechanism( GR_EQSTMT, GR_sNODB, ERx("sql"), yypvt[-0].s );
                    gen_call( IIENDDATA );
                    if (eq->eq_flags & EQ_FIPS)
                            er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1,
                                ERx("ENDDATA") );
                    yyval.i = 1;
                    gr->gr_flag &= ~GR_NOSYNC; /* Bug 59881 */
		  }
		}
		break;
	case 1805:
#line 11811 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  form_no;

		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    if ((form_no = lbl_gen(LBLmDISPLAY, TRUE)) != LBL_NOLEV)
			gen_goto( G_IF, L_FDFINAL, 
				  lbl_value(LBLmDISPLAY, form_no) );
		    else
			er_write( E_EQ0058_grINLOOP, EQ_ERROR, 2, yypvt[-0].s,
							    ERx("DISPLAY") );
		}
		break;
	case 1806:
#line 11831 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gen_call( IIENDFORMS );
		}
		break;
	case 1807:
#line 11846 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  stop, stop_num, stop_mode;

		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    stop = lbl_gen(LBLmFORMDATA|LBLmTBLOOP, TRUE);
		    if (stop != LBL_NOLEV)
		    {
			stop_mode = lbl_get_mode( stop );
			stop_num = lbl_value( stop_mode, stop );

			switch ( stop_mode )
			{
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
							    ERx("DISPLAY") );
		}
		break;
	case 1808:
#line 11893 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  cur_form = lbl_current( LBL_FORM );

		    gen_loop( G_CLOSE, L_FDBEG, L_FDEND, cur_form, IIFNAMES,
			C_0, 0 );
		    gen_label( G_LOOP, L_FDEND, cur_form );
		    gen_call( IIENDFRM );
		}
		break;
	case 1809:
#line 11903 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  cur_form;

		    cur_form = lbl_next( LBL_FORM );
		    arg_str_add( ARG_CHAR, ERx("Names") );
		    gen_if_goto( IIDISPFRM, C_EQ, 0, L_FDEND, cur_form);
		    gen_loop( G_OPEN, L_FDBEG, L_FDEND, cur_form, IIFNAMES, 
			      C_NOTEQ, 0 );
		    gr->gr_func = IIFDATA;		/* In case of I/O */
		    gr->gr_mode = LBLmFORMDATA;		/* In case of block */
		}
		break;
	case 1810:
#line 11916 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1812:
#line 11930 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gr->gr_func = IIFORMINIT;
		}
		break;
	case 1813:
#line 11943 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( eq_genlang(eq->eq_lang) );
		    gen_call( IIFORMS );
		}
		break;
	case 1814:
#line 11967 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
                    if (eq->eq_flags & EQ_CHRPAD)
                        esq_sqmods(IImodNOCPAD);
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		}
		break;
	case 1815:
#line 11973 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
                    if (eq->eq_flags & EQ_CHRPAD)
                        esq_sqmods(IImodNOCPAD);
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		}
		break;
	case 1816:
#line 11980 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_if( G_OPEN, IIFSETIO, C_NOTEQ, 0 );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		    gr->gr_func = IIRETFIELD;
		}
		break;
	case 1817:
#line 11988 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1818:
#line 12003 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    er_write(E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1, ERx("GETOPER"));
		    YYERROR;
		}
		break;
	case 1820:
#line 12020 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( 0 );		/* Dummy type right now */
		}
		break;
	case 1821:
#line 12026 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
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
	case 1822:
#line 12037 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 1823:
#line 12049 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IIHELPFILE );
		}
		break;
	case 1824:
#line 12054 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1825:
#line 12066 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( QAMESSAGE );
		}
		break;
	case 1826:
#line 12071 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1827:
#line 12092 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		    frs_close();
		}
		break;
	case 1829:
#line 12101 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    frs_head();
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);

		}
		break;
	case 1830:
#line 12109 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    frs_inqset( FRSinq );
		}
		break;
	case 1831:
#line 12115 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 1832:
#line 12119 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 1833:
#line 12123 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 1834:
#line 12127 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 1835:
#line 12131 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 1836:
#line 12135 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    frs_object( "row" );
		}
		break;
	case 1839:
#line 12143 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    frs_parentname( yypvt[-0].s, T_CHAR, (PTR)0 );
		}
		break;
	case 1840:
#line 12147 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    frs_parentname( yypvt[-0].s, T_CHAR, (PTR)0 );
		}
		break;
	case 1841:
#line 12151 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    frs_parentname( yypvt[-0].s, T_INT, (PTR)0 );
		}
		break;
	case 1842:
#line 12155 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym)
		    {
			if (gr->gr_type != T_CHAR && gr->gr_type != T_INT)
			{
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0067_grSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
			    frs_parentname( gr->gr_id, T_CHAR, (PTR)0 );
			}
			else
			    frs_parentname( gr->gr_id, gr->gr_type, (PTR)(gr->gr_sym) );
		    }
		    else	/* Error in host_setvar */
			frs_parentname( gr->gr_id, T_CHAR, (PTR)0 );
		}
		break;
	case 1844:
#line 12178 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    er_write( E_EQ0077_grISUNSUPP, EQ_ERROR, 1,
							ERx("INQUIRE_FRS") );
		    YYERROR;
		}
		break;
	case 1848:
#line 12190 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    frs_iqvar( gr->gr_id, gr->gr_type, gr->gr_sym,
			gr->gr_nlid, gr->gr_nlsym );
		}
		break;
	case 1849:
#line 12196 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    frs_constant( yypvt[-0].s, (char *)0, T_NONE, (SYM *)0 );
		}
		break;
	case 1850:
#line 12200 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    er_write( E_EQ0054_grNOPARENS, EQ_ERROR, 0 );
		    frs_constant( yypvt[-2].s, (char *)0, T_NONE, (SYM *)0 );
		}
		break;
	case 1851:
#line 12205 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Should be an error */
		    frs_constant( yypvt[-3].s, yypvt[-1].s, gr->gr_type, gr->gr_sym );
		}
		break;
	case 1852:
#line 12212 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1853:
#line 12216 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1854:
#line 12220 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1855:
#line 12224 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1856:
#line 12228 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1857:
#line 12232 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1858:
#line 12236 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1859:
#line 12240 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1860:
#line 12244 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1861:
#line 12248 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1862:
#line 12253 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1863:
#line 12259 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1864:
#line 12265 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_INT;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1865:
#line 12271 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1866:
#line 12277 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1867:
#line 12283 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1868:
#line 12289 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1869:
#line 12295 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1870:
#line 12301 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1871:
#line 12307 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1872:
#line 12313 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = gr->gr_id;
		    if (gr->gr_sym)
		    {
			if (gr->gr_type != T_CHAR && gr->gr_type != T_INT)
			{
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0067_grSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
			    gr->gr_type = T_CHAR;
			    gr->gr_sym = (SYM *)0;
			}
		    }
		    else	/* Error in host_setvar */
			gr->gr_type = T_CHAR;
		}
		break;
	case 1873:
#line 12338 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( QAPROMPT );
		}
		break;
	case 1874:
#line 12343 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1875:
#line 12361 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IIMESSAGE ); 
    		}
		break;
	case 1876:
#line 12366 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
            	    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
    	    	    gr->gr_func = IIMESSAGE;
		}
		break;
	case 1879:
#line 12390 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    EQFW3glgen();	
		}
		break;
	case 1880:
#line 12395 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    EQFWopen(gr->gr_func);
		}
		break;
	case 1884:
#line 12411 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    EQFWwcoption(yypvt[-2].s, yypvt[-0].s);
		}
		break;
	case 1885:
#line 12416 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1886:
#line 12420 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1887:
#line 12425 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1888:
#line 12429 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1893:
#line 12454 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		EQFWsuboption(yypvt[-2].s, gr->gr_type, gr->gr_id, (PTR)gr->gr_sym);
	}
		break;
	case 1894:
#line 12459 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
	    	    yyval.s = yypvt[-0].s;
	        }
		break;
	case 1895:
#line 12463 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1896:
#line 12467 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1897:
#line 12473 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_INT;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 1898:
#line 12479 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_CHAR;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 1899:
#line 12485 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_FLOAT;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 1900:
#line 12491 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_PACKASCHAR;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 1901:
#line 12497 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (!gr->gr_sym)
		    {
			gr->gr_type = T_CHAR;
		    }	
		}
		break;
	case 1902:
#line 12504 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_CHAR;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 1903:
#line 12510 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_sym = (SYM *)0;
		    gr->gr_type = T_CHAR;
		    gr->gr_id = yypvt[-0].s;
		}
		break;
	case 1904:
#line 12516 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_sym = (SYM *)0;
		    gr->gr_type = T_CHAR;
		    gr->gr_id = yypvt[-0].s;
		}
		break;
	case 1905:
#line 12530 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IIPRNSCR );
		}
		break;
	case 1906:
#line 12535 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1907:
#line 12540 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Should check that $2.s is "file" */
		    if (STbcompare(yypvt[-3].s, 0, ERx("file"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-3].s,
					    ERx("PRINTSCREEN"), ERx("FILE") );
		}
		break;
	case 1908:
#line 12547 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		}
		break;
	case 1909:
#line 12552 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1910:
#line 12572 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		    gen_call( IINPROMPT );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);

		}
		break;
	case 1911:
#line 12582 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gr->gr_func = IINPROMPT;
		}
		break;
	case 1912:
#line 12588 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (STbcompare(yypvt[-0].s, 0, ERx("noecho"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
						ERx("PROMPT"), ERx("NOECHO") );
		    arg_int_add( 1 );
		}
		break;
	case 1913:
#line 12595 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( 0 );
		}
		break;
	case 1914:
#line 12600 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
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
	case 1915:
#line 12649 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		}
		break;
	case 1916:
#line 12655 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		}
		break;
	case 1917:
#line 12662 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_if( G_OPEN, IIFSETIO, C_NOTEQ, 0 );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		    gr->gr_func = IISETFIELD;
		}
		break;
	case 1918:
#line 12670 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1919:
#line 12682 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gen_call( IIREDISP );
		}
		break;
	case 1920:
#line 12703 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  form_no;

		    if ((form_no = lbl_gen(LBLmDISPLAY, TRUE)) != LBL_NOLEV)
		    {
    			if (yypvt[-0].s &&
		            lbl_adget(form_no) == LBLaEVENT)
			{
			    /* Only a "plain" RESUME is allowed in an
			    ** ACTIVATE DBEVENT block
			    */
			    er_write( E_EQ0310_actBADRES, EQ_ERROR, 2, yypvt[-0].s,
					ERx("DBEVENT") );
			}
			else
			{
			    gen_goto( G_IF, L_FDBEG, 
				  lbl_value(LBLmDISPLAY, form_no) );
			}
		    }	
		    else
			er_write( E_EQ0058_grINLOOP, EQ_ERROR, 2, yypvt[-1].s,
							    ERx("DISPLAY") );
		}
		break;
	case 1921:
#line 12729 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1922:
#line 12734 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = (char *)0;	
		}
		break;
	case 1923:
#line 12738 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4		form_no;

		    yyval.s = yypvt[-0].s;	
		    /* No RESUME NEXT unless we're in a DISPLAY loop */
		    if ((form_no = lbl_gen(LBLmDISPLAY, FALSE)) != LBL_NOLEV)
		    {
			switch (lbl_adget(form_no))
			{

			  /* RESUME should not be allowed on activate SCROLL,
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
	case 1924:
#line 12764 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-1].s;	
		    gen_call( IIRESFLD );
		}
		break;
	case 1925:
#line 12769 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-2].s;	
		    gen_call( IIRESCOL );
		}
		break;
	case 1926:
#line 12774 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4		stop;	

		    yyval.s = yypvt[-0].s;	
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
			    arg_int_add( IIFRresPREV );
			    gen_call( IIFRNXTPRVFLD );
			}
		    }
		    else
		    {
		        if (STbcompare(yypvt[-0].s, 0, ERx("menu"), 0, TRUE) != 0)
			    er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
						ERx("RESUME"), ERx("MENU") );
			/* error if nearest one is not a DISPLAY block */
		    	if ((stop != LBL_NOLEV)
			        && (lbl_get_mode(stop) == LBLmSUBMENU))
			    er_write( E_EQ0058_grINLOOP, EQ_ERROR, 2,
					ERx("RESUME MENU"), ERx("DISPLAY") );
		    	else
			    gen_call( IIRESMU );
		    }			
		}
		break;
	case 1927:
#line 12841 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1928:
#line 12856 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    frs_close();
		}
		break;
	case 1930:
#line 12865 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    frs_inqset( FRSset );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		}
		break;
	case 1935:
#line 12884 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    frs_setval( form_sconst(yypvt[-0].s), T_CHAR, (SYM *)0,
			(char *)0, (SYM *)0 );
		}
		break;
	case 1936:
#line 12889 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    frs_setval( yypvt[-0].s, T_INT, (SYM *)0, (char *)0, (SYM *)0 );
		}
		break;
	case 1937:
#line 12893 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    frs_setval(ERx("0"), T_INT, (SYM *)0, (char *)0, (SYM *)0);
		}
		break;
	case 1938:
#line 12898 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    frs_setval(ERx("0"), T_INT, (SYM *)0, (char *)0, (SYM *)0);
		}
		break;
	case 1939:
#line 12903 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    frs_setval( yypvt[-0].s, T_CHAR, (SYM *)0, (char *)0, (SYM *)0 );
		}
		break;
	case 1940:
#line 12907 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    frs_setval( yypvt[-0].s, T_CHAR, (SYM *)0, (char *)0, (SYM *)0 );
		}
		break;
	case 1941:
#line 12911 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (ESQ_EQSTR(yypvt[-0].s, ERx("off")))
		        frs_setval( ERx("0"), T_INT, (SYM *)0, (char *)0,
					(SYM *)0 );
		    else if (ESQ_EQSTR(yypvt[-0].s, ERx("bell")))
		        frs_setval( ERx("2"), T_INT, (SYM *)0, (char *)0,
					(SYM *)0 );
		    else
		        frs_setval( yypvt[-0].s, T_CHAR, (SYM *)0, (char *)0,
					(SYM *)0 );
		}
		break;
	case 1942:
#line 12923 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym)
		    {
			if (gr->gr_type != T_CHAR && gr->gr_type != T_INT)
			{
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0067_grSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
			    frs_setval( gr->gr_id, T_CHAR, (SYM *)0,
					(char *)0, (SYM *)0 );
			}
			else
			    frs_setval( gr->gr_id, gr->gr_type, gr->gr_sym,
					gr->gr_nlid, gr->gr_nlsym );

		    }
		    else	/* Error in host_fsetvar */
			frs_setval( gr->gr_id, T_CHAR, (SYM *)0, (char *)0,
                                    (SYM *)0 );
		}
		break;
	case 1943:
#line 12944 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    frs_setval( ERx("1"), T_INT, (SYM *)0, (char *)0,
				(SYM *)0 );
		}
		break;
	case 1944:
#line 12957 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IISLEEP );
		}
		break;
	case 1945:
#line 12962 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1946:
#line 12995 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  cur_form = lbl_current( LBL_FORM );

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
	case 1948:
#line 13013 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  cur_form = lbl_next( LBL_FORM );

		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gen_goto( G_TERM, L_MUINIT, cur_form );
		    gen_label( G_NOLOOP, L_FDBEG, cur_form );
		    arg_int_add( 0 );
		    gen_call( IIRUNMU );
		    lbl_a_reset();
		    act_open();
		}
		break;
	case 1951:
#line 13029 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_else( G_ELSE, IIRETVAL, C_0, 0, L_ACTELSE, 
			lbl_current(LBL_INTR), L_ACTEND, 
			lbl_current(LBL_FORM) );
		    gr->gr_mode = LBLmSUBMENU;
		}
		break;
	case 1952:
#line 13038 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_mode = LBLmSUBMENU;	/* Not LBLmDISPLAY */
		}
		break;
	case 1953:
#line 13043 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_mode = LBLmSUBMENU;	/* Not LBLmDISPLAY */
		}
		break;
	case 1954:
#line 13047 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_mode = LBLmSUBMENU;	/* Not LBLmDISPLAY */
		}
		break;
	case 1955:
#line 13052 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    act_add( ACT_MENU, lbl_current(LBL_INTR), 0, yypvt[-1].ip );
		    gr->gr_adjective = LBLaMITEM;
		}
		break;
	case 1957:
#line 13071 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  form_no;

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
	case 1958:
#line 13085 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1959:
#line 13090 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_func = IICHKFRM;
		}
		break;
	case 1960:
#line 13094 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_func = IIVALFLD;
		}
		break;
	case 1961:
#line 13116 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 1962:
#line 13122 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IITCLRCOL;
		}
		break;
	case 1963:
#line 13128 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmCLEARR );
		}
		break;
	case 1965:
#line 13135 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    er_write( E_EQ0054_grNOPARENS, EQ_ERROR, 0 );
		    gen_call( IITCLRROW );
		}
		break;
	case 1966:
#line 13140 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IITCLRROW );
		}
		break;
	case 1967:
#line 13161 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Close inner and deleterow if blocks */
		    gen_if( G_CLOSE, IITDELROW, C_0, 0 );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 1968:
#line 13172 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
                    if (eq->eq_flags & EQ_CHRPAD)
                        esq_sqmods(IImodCPAD);
                    if (eq->eq_flags & EQ_CHREOS)
                        esq_sqmods(IImodCEOS);
		    gr->gr_func = IITCOLRET;	/* May be an Out list */
		}
		break;
	case 1969:
#line 13182 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmDELETER );
		}
		break;
	case 1971:
#line 13189 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_VERSWARN)
			er_write( E_EQ0078_grSTYUNSUPP, EQ_WARN, 1,
							ERx("DELETEROW OUT") );
		}
		break;
	case 1972:
#line 13196 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( FALSE );
		    gen_if( G_OPEN, IITDELROW, C_NOTEQ, 0 );
		}
		break;
	case 1973:
#line 13201 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_VERSWARN)
			er_write( E_EQ0078_grSTYUNSUPP, EQ_WARN, 1,
							ERx("DELETEROW IN") );
		}
		break;
	case 1974:
#line 13208 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( TRUE );	/* In list */
		    gen_if( G_OPEN, IITDELROW, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 1975:
#line 13235 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
                    if (eq->eq_flags & EQ_CHRPAD)
                        esq_sqmods(IImodNOCPAD);
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 1976:
#line 13242 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
                    if (eq->eq_flags & EQ_CHRPAD)
                        esq_sqmods(IImodNOCPAD);
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 1977:
#line 13250 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
                    if (eq->eq_flags & EQ_CHRPAD)
                        esq_sqmods(IImodCPAD);
		    gr->gr_func = IITCOLRET;
		}
		break;
	case 1978:
#line 13258 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmGETR );
		}
		break;
	case 1979:
#line 13276 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IITFILL );
		    gen_if( G_CLOSE, IITBINIT, C_0, 0 );
		}
		break;
	case 1980:
#line 13282 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_if( G_OPEN, IITBINIT, C_NOTEQ, 0 );
		}
		break;
	case 1981:
#line 13287 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1985:
#line 13296 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_call( IITHIDECOL );
		}
		break;
	case 1989:
#line 13313 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{ gr->gr_sym = (SYM *)0; }
		break;
	case 1990:
#line 13315 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
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
	case 1992:
#line 13330 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    id_add( form_sconst(yypvt[-0].s) );
		}
		break;
	case 1993:
#line 13334 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym)
		    {
			if (gr->gr_type == T_CHAR)
			    arg_var_add( gr->gr_sym, gr->gr_id );
			else
			{
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0134_sqNAME, EQ_ERROR, 1,
								    gr->gr_id );
			    id_add( gr->gr_id );
			}
		    }
		    else
			id_add( gr->gr_id );
		}
		break;
	case 1994:
#line 13351 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    id_add( yypvt[-0].s );
		}
		break;
	case 1995:
#line 13356 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;		/* Paren not stuff present */
		}
		break;
	case 1996:
#line 13360 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    id_add( yypvt[-2].s );
		    id_add( yypvt[-1].s );
		    id_add( yypvt[-0].s );
		    yyval.i = 1;		/* Paren stuff present */
		}
		break;
	case 1997:
#line 13367 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    id_add( yypvt[-4].s );
		    id_add( yypvt[-3].s );
		    id_add( yypvt[-2].s );
		    id_add( yypvt[-1].s );
		    id_add( yypvt[-0].s );
		    yyval.i = 1;		/* Paren stuff present */
		}
		break;
	case 1998:
#line 13381 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;		/* No NULL clause */
		}
		break;
	case 1999:
#line 13385 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    id_add( ERx(" with null") );
		    yyval.i = 1;		/* With NULL clause */
		}
		break;
	default:
		    if (yy__case20(yym)) YYERROR;
		    break;
	}
	return 0; /* Success */
}

yy__case20(yym)
short yym;
{
#ifdef YYERROR
#undef YYERROR
#endif /* YYERROR */
#define YYERROR return 1

	switch (yym) {
	case 2000:
#line 13390 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-0].i == 0)
			id_add( ERx(" not null") );
		    else
			id_add( ERx(" not null with default") );
		    yyval.i = 1;		/* With NULL clause */
		}
		break;
	case 2002:
#line 13403 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 2003:
#line 13407 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 2004:
#line 13422 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
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
	case 2005:
#line 13434 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmPURGE );
		}
		break;
	case 2006:
#line 13473 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Close inner and insertrow if blocks */
		    gen_call( IITBCECOLEND );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IITINSERT, C_0, 0 );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 2007:
#line 13482 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Close inner and insertrow if blocks */
		    gen_call( IITBCECOLEND );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IITINSERT, C_0, 0 );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 2008:
#line 13492 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gen_if( G_OPEN, IITINSERT, C_NOTEQ, 0 );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 2009:
#line 13501 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmINSERTR );
		}
		break;
	case 2010:
#line 13536 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IITBCECOLEND );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IITBACT, C_0, 0 );
		}
		break;
	case 2011:
#line 13543 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IITBCECOLEND );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IITBACT, C_0, 0 );
		}
		break;
	case 2012:
#line 13551 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( TRUE );		/* Loading */
		    gen_if( G_OPEN, IITBACT, C_NOTEQ, 0 );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 2013:
#line 13560 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 2016:
#line 13568 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    frs_inqset(FRSload);
		    frs_object(ERx("row"));
		}
		break;
	case 2017:
#line 13594 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IITBCECOLEND );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 2018:
#line 13601 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IITBCECOLEND );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 2019:
#line 13609 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 2020:
#line 13617 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmPUTR );
		}
		break;
	case 2021:
#line 13643 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Close inner and scroll if blocks */
		    gen_if( G_CLOSE, IITSCROLL, C_0, 0 );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 2022:
#line 13650 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( rowNONE );
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		}
		break;
	case 2023:
#line 13656 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmSCROLL );
		}
		break;
	case 2024:
#line 13662 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_if( G_OPEN, IITSCROLL, C_NOTEQ, 0 );
		}
		break;
	case 2025:
#line 13666 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if ((yypvt[-1].i || yypvt[-0].i) && (eq->eq_flags & EQ_VERSWARN))
			er_write( E_EQ0078_grSTYUNSUPP, EQ_WARN, 1,
							ERx("SCROLL IN/OUT") );
		}
		break;
	case 2026:
#line 13673 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gen_call( IITBSMODE );
		    arg_int_add( FALSE );	/* Ignore In flag */
		}
		break;
	case 2028:
#line 13681 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( rowEND );
		}
		break;
	case 2029:
#line 13686 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gen_call( IITBSMODE );
		}
		break;
	case 2030:
#line 13691 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gen_call( IITBSMODE );
		}
		break;
	case 2033:
#line 13700 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_func = IITCOLRET;
		}
		break;
	case 2034:
#line 13705 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( FALSE );
		    arg_int_add( rowNONE );
		    gen_if( G_OPEN, IITSCROLL, C_NOTEQ, 0 );
		}
		break;
	case 2036:
#line 13713 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( TRUE );	/* In list */
		    arg_int_add( rowNONE );
		    gen_if( G_OPEN, IITSCROLL, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 2037:
#line 13735 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  cur_table = lbl_current( LBL_TBL );

		    /* Close tabledata loop */
		    gen_loop( G_CLOSE, L_TBBEG, L_TBEND, cur_table, IITDATA,
			      C_0, 0 );
		    if (lbl_is_used(LBL_TBL))
			gen_label( G_LOOP, L_TBEND, cur_table );
		    gen_call( IITDATEND );
		}
		break;
	case 2038:
#line 13746 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  cur_table = lbl_next( LBL_TBL );

		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    if (lbl_gen(LBLmFORMDATA, FALSE) == LBL_NOLEV)
			er_write( E_EQ0058_grINLOOP, EQ_ERROR, 2, yypvt[-0].s,
							    ERx("FORMDATA") );
		    gr->gr_mode = LBLmTBDATA;
		    gen_loop( G_OPEN, L_TBBEG, L_TBEND, cur_table, IITDATA, 
			      C_NOTEQ, 0 );
		}
		break;
	case 2039:
#line 13792 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  cur_table = lbl_current( LBL_TBL );

		    /* Close unloadtable loop */
		    gen_loop( G_CLOSE, L_TBBEG, L_TBEND, cur_table, IITUNLOAD,
			      C_0, 0 );
		    gen_label( G_LOOP, L_TBEND, cur_table );
		    gen_call( IITUNEND );
		}
		break;
	case 2040:
#line 13803 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		    gen_call( IITBCECOLEND );
		}
		break;
	case 2041:
#line 13809 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		    gen_call( IITBCECOLEND );	
		}
		break;
	case 2042:
#line 13816 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  cur_table = lbl_next( LBL_TBL );

		  /* Nested unloadtable? */
		    if (lbl_gen(LBLmTBUNLD,FALSE) != LBL_NOLEV)
			er_write( E_EQ0068_grUNLOADTB, EQ_ERROR, 0 );
		    gr->gr_mode = LBLmTBUNLD;
		    arg_int_add( FALSE );		/* Unload flag */
		    gen_if_goto( IITBACT, C_EQ, 0, L_TBEND, cur_table );
		    gen_loop( G_OPEN, L_TBBEG, L_TBEND, cur_table, IITUNLOAD, 
			      C_NOTEQ, 0 );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		    gr->gr_func = IITCOLRET;
		}
		break;
	case 2043:
#line 13833 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 2044:
#line 13854 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  stop_level;

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
	case 2045:
#line 13871 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{	
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLVAL;
		    yyval.s = yypvt[-3].s;
		}
		break;
	case 2046:
#line 13878 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmVALIDR );
		}
		break;
	case 2048:
#line 13885 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    er_write( E_EQ0054_grNOPARENS, EQ_ERROR, 0 );
		    gen_call( IITVALROW );
		}
		break;
	case 2049:
#line 13890 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IITVALROW );
		}
		break;
	case 2050:
#line 13901 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
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
	case 2051:
#line 13920 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 2052:
#line 13924 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 2054:
#line 13940 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    er_write( E_EQ0054_grNOPARENS, EQ_ERROR, 0 );
		}
		break;
	case 2059:
#line 13951 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 2060:
#line 13955 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
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
	case 2061:
#line 13982 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 2062:
#line 13995 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{	
		    arg_int_add( 0 );
		    arg_var_add( gr->gr_null, ERx("null") ); 
		}
		break;
	case 2064:
#line 14013 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    er_write( E_EQ0054_grNOPARENS, EQ_ERROR, 0 );
		}
		break;
	case 2069:
#line 14024 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 2070:
#line 14028 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    switch ( gr->gr_func )
		    {
		      case IIRETFIELD:
		      case IITCOLRET:
			arg_push();
			arg_int_add( TRUE );
			gen_call( IIGETOPER );
			if (yypvt[-5].i != T_INT && yypvt[-5].i != T_DBV && yypvt[-5].i != T_UNDEF)
			    er_write( E_EQ0053_grFMINTFUNC, EQ_ERROR, 1, yypvt[-4].s );
			break;

		      default:
			er_write( E_EQ0052_grFMFUNC, EQ_ERROR, 1, yypvt[-4].s );
			break;
		    }
		    gen_call( gr->gr_func );
		}
		break;
	case 2071:
#line 14048 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_nlsym)
			arg_var_add( gr->gr_nlsym, gr->gr_nlid );
		    else
			arg_int_add( 0 );
		    arg_var_add( gr->gr_sym, gr->gr_id );
		    yyval.i = gr->gr_type;
		}
		break;
	case 2073:
#line 14061 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( rowCURR );
		}
		break;
	case 2075:
#line 14069 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    if (lbl_gen(LBLmDISPLAY, FALSE) == LBL_NOLEV)
			er_write( E_EQ0056_grFORMNAME, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 2076:
#line 14078 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    er_write( E_EQ0071_grASSGNOP, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 2080:
#line 14099 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = form_sconst(yypvt[-0].s);
		}
		break;
	case 2081:
#line 14105 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 2082:
#line 14111 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( 0 );		/* null indicator */
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 2084:
#line 14124 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 2085:
#line 14128 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  dummy;
		    char *delim_arg;

		    /*
		    ** Put the quotes back for delimited ids and escape
		    ** them (\") so they will be generated correctly
		    ** later on.
		    */
		    /* Place the delimited ids (with quotes and escape 
	            ** characters) in buffers of the string table work space
		    ** to avoid being overwritten when there is another
		    ** call to gen_sqmakesconst within the same SQL 
		    ** statement.          		b57771tt       
		    */
		    delim_arg = str_add( STRNULL, 
                                gen_sqmakesconst(G_DELSTRING, yypvt[-0].s, &dummy) );

		    arg_str_add( ARG_CHAR, delim_arg );
		}
		break;
	case 2087:
#line 14151 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym)
		    {
			if (gr->gr_type == T_CHAR)
			    arg_var_add( gr->gr_sym, gr->gr_id );
			else
			{
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0067_grSTRVAR, EQ_ERROR, 1,
								    gr->gr_id );
			    arg_str_add( ARG_CHAR, gr->gr_id );
			}
		    }
		    else	/* Error already in host_setvar */
			arg_str_add( ARG_CHAR, gr->gr_id );
		}
		break;
	case 2088:
#line 14172 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2089:
#line 14176 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    char	ibuf[50];

		    ibuf[0] = '-';
		    ibuf[1] = '\0';
		    yyval.s = str_add( STRNULL, STcat(ibuf, yypvt[-0].s) );
		}
		break;
	case 2090:
#line 14184 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2091:
#line 14191 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_INT, yypvt[-0].s );
		}
		break;
	case 2092:
#line 14197 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( 0 );		/* null indicator */
		    arg_str_add( ARG_INT, yypvt[-0].s );
		}
		break;
	case 2093:
#line 14205 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2094:
#line 14209 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    char	fbuf[50];

		    fbuf[0] = '-';
		    fbuf[1] = '\0';
		    yyval.s = str_add( STRNULL, STcat(fbuf, yypvt[-0].s) );
		}
		break;
	case 2095:
#line 14217 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2096:
#line 14233 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( 0 );		/* null indicator */
		    arg_str_add( ARG_FLOAT, yypvt[-0].s );
		}
		break;
	case 2097:
#line 14241 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2098:
#line 14245 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    char	dbuf[50];

		    dbuf[0] = '-';
		    dbuf[1] = '\0';
		    yyval.s = str_add( STRNULL, STcat(dbuf, yypvt[-0].s) );
		}
		break;
	case 2099:
#line 14253 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2100:
#line 14259 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( 0 );		/* null indicator */
		    arg_str_add( ARG_PACK, yypvt[-0].s );
		}
		break;
	case 2102:
#line 14269 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    arg_int_add( 0 );
		}
		break;
	case 2103:
#line 14274 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    arg_int_add( 0 );
		}
		break;
	case 2104:
#line 14279 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym)
		    {
			if (gr->gr_type == T_INT)
			    arg_var_add( gr->gr_sym, gr->gr_id );
			else
			{
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0061_grINTVAR, EQ_ERROR, 1,
								    gr->gr_id );
			    arg_int_add( 0 );
			}
		    }
		    else	/* Error in host_setvar */
			arg_int_add( 0 );
		}
		break;
	case 2105:
#line 14304 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Null indicator argument */
		    if (gr->gr_nlsym)
			arg_var_add( gr->gr_nlsym, gr->gr_nlid );
		    else
			arg_int_add( 0 );

		    if (gr->gr_sym)
			arg_var_add( gr->gr_sym, gr->gr_id );
		    else	/* Error in host_fsetvar */
			arg_str_add( ARG_CHAR, gr->gr_id );
		}
		break;
	case 2106:
#line 14317 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_type = T_INT;
		}
		break;
	case 2107:
#line 14321 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_type = T_FLOAT;
		}
		break;
	case 2108:
#line 14325 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_type = T_PACKASCHAR;
		}
		break;
	case 2109:
#line 14329 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_type = T_CHAR;
		}
		break;
	case 2110:
#line 14336 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4	form_table, input_output;

		    switch (gr->gr_func) {
		      case IISETFIELD:
			form_table = 1;
			input_output= 1;
			break;
		      case IIRETFIELD:
			form_table = 1;
			input_output= 0;
			break;
		      case IITCOLSET:
			form_table = 0;
			input_output= 1;
			break;
		      case IITCOLRET:
			form_table = 0;
			input_output= 0;
			break;
		    }
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_int_add( form_table );		/* is form? */
		    arg_int_add( input_output );	/* is input? */
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IIFRSQEXEC );
		}
		break;
	case 2128:
#line 14416 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_call(IIG4CALLCOMP);
		    gen_if( G_CLOSE, IIG4INITCALL, C_EQ, 0);
		}
		break;
	case 2129:
#line 14427 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    char *statement_name = (yypvt[-1].i == G4CT_FRAME) ?
					       ERx("callframe") :
					       ERx("callproc");

		    gr_mechanism( GR_EQSTMT, GR_s4GL, statement_name, NULL );
		    arg_int_add(yypvt[-1].i);
		    gen_if( G_OPEN, IIG4INITCALL, C_EQ, 0);
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		}
		break;
	case 2130:
#line 14443 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = G4CT_PROC;
		}
		break;
	case 2131:
#line 14447 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = G4CT_FRAME;
		}
		break;
	case 2134:
#line 14458 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-0].i == 0)
		    {
			/* Not byref */
			args_toend(1, 2);
		        gen_call(IIG4VALPARAM);
		    }
		    else
		    {
			/* Byref */
			gen_call(IIG4BYREFPARAM);
		    }
		}
		break;
	case 2135:
#line 14472 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-0].i == 0)
		    {
			/* Not byref */
			args_toend(1, 2);
		        gen_call(IIG4VALPARAM);
		    }
		    else
		    {
			/* Byref */
			gen_call(IIG4BYREFPARAM);
		    }
		}
		break;
	case 2136:
#line 14488 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add(ARG_CHAR, yypvt[-2].s);
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 2137:
#line 14493 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add(ARG_CHAR, yypvt[-2].s);
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 2138:
#line 14498 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* 
		    ** The parameter name was pushed too early.
		    ** Now place it correctly.
		    */
		    args_toend(1, 1);
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 2139:
#line 14507 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add(ARG_CHAR, ERx(""));
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 2140:
#line 14514 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_type != T_NONE && 
			gr->gr_type != T_CHAR)
		    {
			er_write(E_EQ0067_grSTRVAR, EQ_ERROR, 
				 1, gr->gr_id);
		    }
		    arg_var_add(gr->gr_sym, gr->gr_id);
		}
		break;
	case 2141:
#line 14526 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 2142:
#line 14530 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 2144:
#line 14537 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call(IIG4RETVAL);
		}
		break;
	case 2145:
#line 14548 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IIG4CLRARR );
		}
		break;
	case 2146:
#line 14554 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2147:
#line 14565 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IIG4DESCRIBE );
		}
		break;
	case 2148:
#line 14573 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2149:
#line 14593 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
			if (eq->eq_flags & EQ_CHRPAD)
			    esq_sqmods(IImodNOCPAD);
			gen_if( G_CLOSE, IIG4CHKOBJ, C_EQ, 0);
		}
		break;
	case 2150:
#line 14601 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-0].s == NULL)
		    {
			/* index is 0; access is to whole object */
			arg_int_add(G4OA_OBJECT);
			arg_int_add(0);
		    }
		    else
		    {
			/* access is to row */
			arg_int_add(G4OA_ROW);
			arg_str_add(ARG_INT, yypvt[-0].s);
		    }

		    /* Caller is Get attribute */
		    arg_int_add(G4GATTR_ID);

		    gen_if( G_OPEN, IIG4CHKOBJ, C_EQ, 0);
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		    gr->gr_func = IIG4GETATTR;
		}
		break;
	case 2151:
#line 14626 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2152:
#line 14632 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call(IIG4USEDESCR);
		}
		break;
	case 2154:
#line 14639 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call(IIG4GETATTR);
		}
		break;
	case 2155:
#line 14643 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call(IIG4GETATTR);
		}
		break;
	case 2156:
#line 14654 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  gtype = 0;

		    if (STbcompare(yypvt[-3].s, 0, ERx("constant"), 0, TRUE) == 0)
		    {
			gtype = G4GT_CONST;
		    }
		    else if (STbcompare(yypvt[-3].s, 0, ERx("variable"), 0, TRUE) == 0)
		    {
			gtype = G4GT_VAR;
		    }
		    else
		    {
			er_write( E_EQ0081_TL_ATTR_UNKNOWN, EQ_ERROR, 1, yypvt[-3].s);
		    }
		    if(eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		    arg_int_add(gtype);
		    gen_call( IIG4GETGLOB );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);

		}
		break;
	case 2157:
#line 14680 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2158:
#line 14692 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_INT, yypvt[-2].s);
		    arg_str_add( ARG_INT, yypvt[-1].s);
		    gen_call( IIG4GETROW );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		}
		break;
	case 2159:
#line 14702 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		}
		break;
	case 2160:
#line 14710 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_type != T_UNDEF) /* Don't cascade errs */
		    {
			if (gr->gr_type != T_INT)
			    er_write( E_EQ0059_grINT, EQ_ERROR, 1, gr->gr_id );
		    }
		    yyval.s = gr->gr_id;
		}
		break;
	case 2162:
#line 14730 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		}
		break;
	case 2163:
#line 14736 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
                    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		    gr->gr_func = IIINQ4GL;
		}
		break;
	case 2166:
#line 14749 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call(IIINQ4GL);
		}
		break;
	case 2167:
#line 14760 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  which = 0;
		    INSROW4GL_PARMS *prmptr = (INSROW4GL_PARMS *)(yypvt[-1].ip);
		
		    /* 
		    ** Generate the parameters.  Unspecified optional 
		    ** parameters become 0. 
		    */
		    if (*prmptr->rownumber == EOS)
		    {
			    er_write( E_EQ051A_NOROWNUM, EQ_ERROR, 0);
		    }
		    arg_str_add( ARG_INT, prmptr->rownumber );
		    if (*prmptr->row == EOS)
		    {
			arg_int_add(0);
		    }
		    else
		    {
			arg_str_add( ARG_INT, prmptr->row);
			which |= G4IR_ROW;
		    }
		    if (*prmptr->row_state == EOS)
		    {
			arg_int_add(0);
		    }
		    else
		    {
			arg_str_add( ARG_INT, prmptr->row_state);
			which |= G4IR_STATE;
		    }
		    arg_int_add(which);
			
		    gen_call( IIG4INSROW );
		    MEfree(prmptr);
		}
		break;
	case 2168:
#line 14799 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2169:
#line 14805 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    INSROW4GL_PARMS *prmptr;

		    prmptr = (INSROW4GL_PARMS *)
				MEreqmem(0, sizeof(INSROW4GL_PARMS), 
					 FALSE, NULL);
		    prmptr->rownumber = prmptr->row_state = prmptr->row = 
			ERx("");
		    yyval.ip = (i4 *)prmptr;
		    set_insrow_param(yypvt[-0].s, gr->gr_id, gr->gr_type, 
				     gr->gr_sym, prmptr);
		}
		break;
	case 2170:
#line 14818 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    INSROW4GL_PARMS *prmptr = (INSROW4GL_PARMS *)(yypvt[-2].ip);

		    set_insrow_param(yypvt[-0].s, gr->gr_id, gr->gr_type, 
				     gr->gr_sym, prmptr);
		    yyval.ip = yypvt[-2].ip;
		}
		break;
	case 2171:
#line 14828 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 2172:
#line 14840 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IIG4REMROW );
		}
		break;
	case 2173:
#line 14846 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2174:
#line 14869 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IIG4CALLCOMP );
		    gen_if( G_CLOSE, IISENDUSEREVENT, C_EQ, 0);
		}
		break;
	case 2175:
#line 14876 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
                    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-1].s, NULL );
		    gen_if( G_OPEN, IISENDUSEREVENT, C_EQ, 0);
		}
		break;
	case 2179:
#line 14890 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add(ARG_CHAR, yypvt[-2].s);
		    args_toend(1, 2);
		    gen_call(IIG4VALPARAM);
		}
		break;
	case 2180:
#line 14904 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		}
		break;
	case 2181:
#line 14911 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
                    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		    gr->gr_func = IISET4GL;
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		}
		break;
	case 2184:
#line 14924 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call(IISET4GL);
		}
		break;
	case 2185:
#line 14944 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
			gen_if( G_CLOSE, IIG4CHKOBJ, C_EQ, 0);
		}
		break;
	case 2186:
#line 14952 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (yypvt[-0].s == NULL)
		    {
			/* index is 0; access is to whole object */
			arg_int_add(G4OA_OBJECT);
			arg_int_add(0);
		    }
		    else
		    {
			/* access is to row */
			arg_int_add(G4OA_ROW);
			arg_str_add(ARG_INT, yypvt[-0].s);
		    }

		    /* Caller is Set attribute */
		    arg_int_add(G4SATTR_ID);

		    gen_if( G_OPEN, IIG4CHKOBJ, C_EQ, 0);
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		    gr->gr_func = IIG4SETATTR;
		}
		break;
	case 2187:
#line 14978 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2188:
#line 14983 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call(IIG4USEDESCR);
		}
		break;
	case 2190:
#line 14990 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call(IIG4SETATTR);
		}
		break;
	case 2191:
#line 14994 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call(IIG4SETATTR);
		}
		break;
	case 2192:
#line 15005 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{

		    if (STbcompare(yypvt[-3].s, 0, ERx("variable"), 0, TRUE) != 0)
		    {
			er_write( E_EQ0081_TL_ATTR_UNKNOWN, EQ_ERROR, 1, yypvt[-3].s);
		    }
		    gen_call( IIG4SETGLOB );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		}
		break;
	case 2193:
#line 15018 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		}
		break;
	case 2194:
#line 15031 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IIG4SETROW );
		}
		break;
	case 2195:
#line 15037 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2196:
#line 15043 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (STbcompare(ERx("row"), 0, yypvt[-3].s, 0, TRUE) != 0)
		    {
			    er_write( E_EQ0081_TL_ATTR_UNKNOWN, EQ_ERROR, 1,
								yypvt[-3].s);
		    }
		}
		break;
	case 2197:
#line 15058 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_call( IIG4DELROW );
		}
		break;
	case 2198:
#line 15064 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2199:
#line 15080 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (STbcompare(ERx("rownumber"), 0, yypvt[-3].s, 0, TRUE) != 0)
		    {
			    er_write( E_EQ0081_TL_ATTR_UNKNOWN, EQ_ERROR, 1,
								yypvt[-3].s);
		    }
		}
		break;
	case 2200:
#line 15091 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_str_add( ARG_INT, yypvt[-0].s);
		}
		break;
	case 2201:
#line 15098 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2202:
#line 15102 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2203:
#line 15109 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add(0);	/* Null indicator */
		    arg_var_add( gr->gr_sym, gr->gr_id );
		}
		break;
	case 2204:
#line 15117 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Check that it's a valid object */
		    if (gr->gr_type != T_UNDEF) /* Don't cascade errs */
		    {
			if (gr->gr_type != T_INT
				|| sym_g_dsize(gr->gr_sym) != 4)
			    er_write( E_EQ0518_grOBJVAR, EQ_ERROR, 1,
								gr->gr_id );
		    }
		    yyval.s = gr->gr_id;
		}
		break;
	case 2205:
#line 15132 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 2206:
#line 15136 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    arg_int_add(0);
		    yyval.i = 0;
		}
		break;
	case 2208:
#line 15148 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = NULL;
		}
		break;
	case 2209:
#line 15152 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_type != T_UNDEF) /* Don't cascade errs */
		    {
			if (gr->gr_type != T_INT)
			    er_write( E_EQ0059_grINT, EQ_ERROR, 1, gr->gr_id );
		    }
		    yyval.s = gr->gr_id;
		}
		break;
	case 2210:
#line 15164 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		}
		break;
	case 2211:
#line 15167 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_type = T_INT;
		    gr->gr_id = yypvt[-0].s;
		    gr->gr_sym = NULL;
		}
		break;
	case 2212:
#line 15173 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_type = T_FLOAT;
		    gr->gr_id = yypvt[-0].s;
		    gr->gr_sym = NULL;
		}
		break;
	case 2213:
#line 15179 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_type = T_PACKASCHAR;
		    gr->gr_id = yypvt[-0].s;
		    gr->gr_sym = NULL;
		}
		break;
	case 2214:
#line 15185 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_type = T_CHAR;
		    gr->gr_id = yypvt[-0].s;
		    gr->gr_sym = NULL;
		}
		break;
	case 2215:
#line 15194 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  input_output;

		    if (gr->gr_func == IIG4GETATTR)
			input_output = G4SD_GET;
		    else
			input_output = G4SD_SET;

		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_int_add( input_output );	/* is input? */
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		}
		break;
	case 2219:
#line 15220 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    arg_int_add( 0 );
		}
		break;
	case 2220:
#line 15225 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    arg_int_add( 0 );
		}
		break;
	case 2221:
#line 15230 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym)
		    {
			if (gr->gr_type == T_INT)
			    arg_var_add( gr->gr_sym, gr->gr_id );
			else
			{
			    if (gr->gr_type != T_UNDEF)
				er_write( E_EQ0061_grINTVAR, EQ_ERROR, 1,
								    gr->gr_id );
			    arg_int_add( 0 );
			}
		    }
		    else	/* Error in host_setvar */
			arg_int_add( 0 );
		}
		break;
	case 2222:
#line 15250 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Null indicator argument */
		    if (gr->gr_nlsym)
			arg_var_add( gr->gr_nlsym, gr->gr_nlid );
		    else
			arg_int_add( 0 );

		    if (gr->gr_sym)
			arg_var_add( gr->gr_sym, gr->gr_id );
		    else	/* Error in host_fsetvar */
			arg_str_add( ARG_CHAR, gr->gr_id );
		}
		break;
	case 2223:
#line 15263 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_type = T_INT;
		}
		break;
	case 2224:
#line 15267 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_type = T_FLOAT;
		}
		break;
	case 2225:
#line 15271 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_type = T_PACKASCHAR;
		}
		break;
	case 2226:
#line 15275 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_type = T_CHAR;
		}
		break;
	case 2227:
#line 15282 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    i4  code;

		    code = g4_check(gr->gr_func, yypvt[-1].s, (bool)yypvt[-0].i);
		    arg_int_add(code);
		}
		break;
	case 2228:
#line 15291 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2229:
#line 15295 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2230:
#line 15330 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 2231:
#line 15334 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 2232:
#line 15338 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 2233:
#line 15342 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 2237:
#line 15355 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    id_add( yypvt[-0].s );
		}
		break;
	case 2238:
#line 15359 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    id_add( yypvt[-0].s );
		}
		break;
	case 2239:
#line 15368 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2240:
#line 15378 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 2241:
#line 15396 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    dml->dm_exec = DML_HOST;
		}
		break;
	case 2242:
#line 15413 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    dml->dm_exec = DML_HOST;
		}
		break;
	case 2243:
#line 15426 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2244:
#line 15430 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2245:
#line 15434 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2246:
#line 15438 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2247:
#line 15442 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2248:
#line 15454 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if ((gr->gr_flag & GR_HOSTCODE) == 0)
		    {
			gen_line( ERx("host_code") );
			gr->gr_flag |= GR_HOSTCODE;
		    }
		    gen_host( G_H_CODE, yypvt[-0].s );
		    dml->dm_exec = DML_HOST;
		}
		break;
	case 2250:
#line 15471 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (dml->dm_exec != (DML_EXEC|DML__SQL))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"),
							ERx("BEGIN DECLARE") );
		    dml->dm_exec = DML_DECL;
 		    eq->eq_flags |= EQ_INDECL;	/* Scanner in declare mode */
		}
		break;
	case 2251:
#line 15480 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (dml->dm_exec != (DML_EXEC|DML__SQL))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"),
							ERx("END DECLARE") );
		    dml->dm_exec = DML_HOST;
		    eq->eq_flags &= ~EQ_INDECL;	/* Scanner exits declare mode */
		}
		break;
	case 2253:
#line 15490 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->F_rec = 0;
		    gr->F_dims = 0;
		    gr->F_size = 0;
		    gr->gr_type = T_INT;
		    gr->F_struct = (SYM *)0;
		    gen_host( G_H_NEWLINE, (char *)0 );
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 2260:
#line 15593 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* 
		    ** Important to get scanner out of EXEC mode to
		    ** prevent it from continually pushing back fake
		    ** terminator.
		    */
		    dml->dm_exec = DML_DECL;
		}
		break;
	case 2262:
#line 15610 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    register SYM	*sy;

		  /* 
		  ** If we have a prev tag by same name then we'll get a 
		  ** symtab error here about redecl with incompatible types 
		  */
		    sy = yyval.v = symDcEuc( yypvt[-1].s, gr->F_rec, F_BLOCK, syFisTAG,
			    F_CLOSURE, SY_TAG );
		    gr->gr_type = T_STRUCT;
		    if (sy)
			sym_s_btype( sy, T_STRUCT );
		}
		break;
	case 2263:
#line 15626 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->F_rec++;
		    gen_host( G_H_INDENT | G_H_NEWLINE, (char *) NULL );
		}
		break;
	case 2264:
#line 15633 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->F_rec--;
		    gen_host( G_H_OUTDENT | G_H_NEWLINE, (char *) NULL );
		}
		break;
	case 2267:
#line 15645 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->F_dims = 0;
		}
		break;
	case 2272:
#line 15656 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_OP, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2274:
#line 15668 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* BUG 2259 - Explicitly set type of nested struct */
		    gr->gr_type = T_STRUCT;
		}
		break;
	case 2281:
#line 15691 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    CVan( yypvt[-0].s, &gr->F_size );
		    switch (gr->gr_type) {
		      case T_INT:
			if (gr->F_size != 1 && gr->F_size != 4 
			    && gr->F_size != 2)
			  er_write( E_E1000C_hfBADSIZE, EQ_ERROR, 0 );
			break;
		      case T_FLOAT:
			if (gr->F_size != 4 && gr->F_size != 8)
			  er_write( E_E1000C_hfBADSIZE, EQ_ERROR, 0);
		      break;
		    }
		}
		break;
	case 2282:
#line 15709 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->F_dims = 0;		/* not an array */
		}
		break;
	case 2284:
#line 15716 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_OP, ERx(" ") );	/* last token was OP */
		}
		break;
	case 2285:
#line 15722 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2295:
#line 15756 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    SYM		*sy;

    		    sym_hint_type( (SYM *)0, gr->gr_type, 0 );
    		    sy = symDcEuc( yypvt[-2].s, gr->F_rec, F_BLOCK, 
			syFisVAR, F_CLOSURE, SY_NORMAL );
		    if (sy)
		    {
			sym_s_btype( sy, gr->gr_type );
			sym_s_dims( sy, yypvt[-1].i );
		    }
		}
		break;
	case 2296:
#line 15780 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    SYM		*sy;

    		    sym_hint_type( (SYM *)0, gr->gr_type, 0 );
    		    sy = symDcEuc( yypvt[-2].s, gr->F_rec, F_BLOCK, 
			syFisVAR, F_CLOSURE, SY_NORMAL );
		    if (sy)
		    {
			sym_s_btype( sy, gr->gr_type );
			sym_s_dims( sy, yypvt[-0].i );
			sym_s_dsize( sy, gr->F_size );
		    }
		}
		break;
	case 2297:
#line 15796 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 2298:
#line 15800 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* $$.i = 1; */
		}
		break;
	case 2299:
#line 15806 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    sc_eat( gen_code, SC_SEEN|SC_NEST, ERx(")"), '(', ')' );
		    yyval.i = 1;
		}
		break;
	case 2303:
#line 15818 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    sc_eat( gen_code, SC_SEEN, ERx("/"), '/', '/' );
		}
		break;
	case 2304:
#line 15824 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_type = T_INT;
		}
		break;
	case 2314:
#line 15849 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    SYM		*sy;

		    sym_hint_type( (SYM *)0, gr->gr_type, 0 );
		    sy = symDcEuc( yypvt[-1].s, gr->F_rec, F_BLOCK, 
			syFisVAR, F_CLOSURE, SY_NORMAL );
		    if (sy)
		    {
			sym_s_btype( sy, gr->gr_type );
			sym_s_dims( sy, yypvt[-0].i );
		      /* assign type entry for structure variables */
			if (gr->gr_type == T_STRUCT && gr->F_struct)
			    sym_type_euc( sy, gr->F_struct );
		    }
		}
		break;
	case 2319:
#line 15885 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_INDENT | G_H_NEWLINE, (char *) NULL );
		}
		break;
	case 2320:
#line 15891 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_OUTDENT | G_H_NEWLINE, (char *) NULL );
		}
		break;
	case 2321:
#line 15897 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_NEWLINE, (char *) NULL );
		}
		break;
	case 2322:
#line 15907 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->F_size = 0;		/* for IIvmsret! */
		    gr->F_dims = 0;
		    gr->gr_type = T_CHAR;
		}
		break;
	case 2323:
#line 15916 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_OP, ERx(", ") );
		}
		break;
	case 2324:
#line 15922 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2325:
#line 15928 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s);
		}
		break;
	case 2326:
#line 15934 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2327:
#line 15938 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (*(yypvt[-1].s) == '-')
			gen_host( G_H_OP, ERx("-") );
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2330:
#line 15950 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2331:
#line 15956 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s);
		}
		break;
	case 2332:
#line 15962 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );

		    if (STbcompare(yypvt[-0].s, 3, ERx("SQL"), 3, TRUE) == 0)
		    {
			/* Test for SQLCOD */
			if (!(eq->eq_flags & EQ_NOSQLCODE)
			 && (STbcompare(yypvt[-0].s, 0, ERx("SQLCOD"), 0, TRUE) == 0))
			{
			    if (!(eq->eq_flags & EQ_SQLCODE))
			    {
				/* 
				** This is an error, because we probably 
				** included the wrong eqslqca.
				*/
				er_write( E_E1000D_hfNOSQLCODE, EQ_ERROR, 0, 
					  (char *)0 );
				eq->eq_flags |= EQ_SQLCODE;
			    }
			    if (gr->gr_type != T_INT)
			    {
				er_write( E_EQ0517_SQLSTATUS_DECL, EQ_ERROR, 2,
				   ERx("SQLCOD(E)"), ERx("integer"));
			    }
			    else
			    {
				eq->eq_flags |= EQ_SQLCODE;
			    }

			}

			/* Test for SQLSTA */
			if (STbcompare(yypvt[-0].s, 0, ERx("SQLSTA"), 0, TRUE) == 0)
			{
			    if (gr->gr_type != T_CHAR)
			    {
				er_write( E_EQ0517_SQLSTATUS_DECL, EQ_ERROR, 2,
				   ERx("SQLSTA(TE)"), ERx("CHARACTER*5"));
			    }
			    else
			    {
				eq->eq_flags |= EQ_SQLSTATE | EQ_SQLSTA;
			    }
			}

			/* Test for SQLSTATE */
			if (STbcompare(yypvt[-0].s, 0, ERx("SQLSTATE"), 0, TRUE) == 0)
			{
			    if (gr->gr_type != T_CHAR)
			    {
				er_write( E_EQ0517_SQLSTATUS_DECL, EQ_ERROR, 2,
				   ERx("SQLSTA(TE)"), ERx("CHARACTER*5"));
			    }
			    else
			    {
				eq->eq_flags |= EQ_SQLSTATE;
			    }
			}
		    }
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2333:
#line 16026 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2334:
#line 16032 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2335:
#line 16038 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    register SYM	*sy;

		    gen_host( G_H_KEY, yypvt[-0].s );
		    sy = sym_resolve( (SYM *)0, yypvt[-0].s, F_CLOSURE, syFisTAG );
		    if (sy == (SYM *)0)
			er_write( E_E1000A_hfNOSTRUCT, EQ_ERROR, 1, yypvt[-0].s );
		    else
		    {
			gr->F_struct = sy;
			gr->gr_type = T_STRUCT;
		    }
		}
		break;
	case 2336:
#line 16054 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->F_size = 1;
		    gr->gr_type = T_INT;
		}
		break;
	case 2337:
#line 16060 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->F_size = gr->F_intsize;
		    gr->gr_type = T_INT;
		}
		break;
	case 2338:
#line 16066 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->F_size = gr->F_intsize;
		    gr->gr_type = T_INT;
		}
		break;
	case 2339:
#line 16072 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->F_size = 4;
		    gr->gr_type = T_FLOAT;
		}
		break;
	case 2340:
#line 16078 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->F_size = 8;
		    gr->gr_type = T_FLOAT;
		}
		break;
	case 2341:
#line 16085 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    er_write( E_E10001_hfBADTYPE, EQ_ERROR, 1, ERx("COMPLEX") );
		    gr->F_size = 8;
		    gr->gr_type = T_FLOAT;
		}
		break;
	case 2342:
#line 16092 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->F_size = 8;
		    gr->gr_type = T_FLOAT;
		    er_write( E_E10001_hfBADTYPE, EQ_ERROR, 1,
							ERx("DOUBLE COMPLEX") );
		}
		break;
	case 2343:
#line 16103 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2346:
#line 16115 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gen_host( G_H_OP, ERx(" ") );
		}
		break;
	case 2349:
#line 16126 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gen_host( G_H_OP, ERx("=") );
		}
		break;
	case 2350:
#line 16133 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    register SYM	*sy;

		    if (gr->gr_type != T_NONE)
		    {
			if (sy=symDcEuc(yypvt[-2].s, 0, F_BLOCK, syFisCONST,
			    F_CLOSURE, SY_NORMAL))
			{
			    sym_s_btype( sy, gr->gr_type );
			    sym_s_dsize( sy, gr->F_size );
			}
		    } else
			er_write( E_E10002_hfDEFINE, EQ_ERROR, 1, yypvt[-2].s );
		}
		break;
	case 2351:
#line 16150 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_type = T_INT;
		    gr->F_size = gr->F_intsize;
		}
		break;
	case 2352:
#line 16155 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_type = T_FLOAT;
		    gr->F_size = sizeof(f4);	/* real constants are real*4 */
		}
		break;
	case 2353:
#line 16160 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_type = T_CHAR;
		    gen_host( G_H_SCONST, yypvt[-0].s );
		}
		break;
	case 2354:
#line 16165 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_type = T_NONE;
		}
		break;
	case 2355:
#line 16171 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2356:
#line 16175 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (*(yypvt[-1].s) == '-')
			gen_host( G_H_OP, ERx("-") );
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2357:
#line 16181 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2358:
#line 16185 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (*(yypvt[-1].s) == '-')
			gen_host( G_H_OP, ERx("-") );
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2359:
#line 16193 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
			gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 2360:
#line 16199 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
			gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 2361:
#line 16216 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /*
		    ** Inform scanner that we should read in a new line.
		    */
		    sc_reset();
		    if (inc_parse_name(yypvt[-0].s, FALSE) == SC_INC)
			inc_push_file();
		    dml->dm_exec = DML_DECL;
		}
		break;
	case 2362:
#line 16226 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    /* Better be "<EOF>" else Yacc will produce syntax error */
		    dml->dm_exec = DML_DECL;
		}
		break;
	case 2363:
#line 16232 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (dml->dm_exec != (DML_EXEC|DML__SQL))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"),
							    ERx("INCLUDE") );
		}
		break;
	case 2364:
#line 16239 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{	
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2365:
#line 16243 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2366:
#line 16257 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    dml->dm_exec = DML_DECL;
		}
		break;
	case 2367:
#line 16262 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    esq->flag |= ESQ_NOPRT;     /* Suppress printing */
		    if (dml->dm_exec != (DML_EXEC|DML__SQL))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"),
							ERx("DECLARE TABLE") );
		}
		break;
	case 2368:
#line 16269 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    esq->flag |= ESQ_NOPRT;     /* Suppress printing */
		    if (dml->dm_exec != (DML_EXEC|DML__SQL))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"),
							ERx("DECLARE TABLE") );
		}
		break;
	case 2373:
#line 16310 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    sc_hostvar = TRUE;	/* disable keyword lookups */
		}
		break;
	case 2375:
#line 16318 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    er_write( E_EQ0129_sqVARCOLON, EQ_ERROR, 0 );
		}
		break;
	case 2378:
#line 16343 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    if (gr->gr_sym != (SYM *)0)
		    {
			if (!syBitAnd(sym_g_useof(gr->gr_sym), syFisVAR))
			    er_write( E_EQ0057_grHOSTVAR, EQ_ERROR, 1,
								    gr->gr_id );
		    }
		}
		break;
	case 2381:
#line 16373 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    SYM		*sy;

		    gr->gr_type = T_UNDEF;		/* Default */
		    if ((sy = yypvt[-0].v) != (SYM *)0)
		    {
			if (sym_g_btype(sy) == T_STRUCT)
			    er_write( E_EQ0057_grHOSTVAR, EQ_ERROR, 1,
								id_getname() );
			else 
			    gr->gr_type = sym_g_btype(sy);
		    }
		    gr->gr_sym = sy;
		    gr->gr_id = str_add( STRNULL, id_getname() );
		    id_free();
		}
		break;
	case 2382:
#line 16396 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    sc_hostvar = FALSE;	/* re-enable keyword lookups */
		}
		break;
	case 2383:
#line 16400 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    sc_hostvar = FALSE;	/* re-enable keyword lookups */
		    sc_popscptr();	/* move SC_PTR back one word */
		}
		break;
	case 2384:
#line 16406 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    gr->gr_id = str_add( STRNULL, id_getname() );
		    id_free();
		    sym_f_init(); 	/* Discard sym stuff - ignored */
		}
		break;
	case 2385:
#line 16413 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    sc_hostvar = TRUE;	/* disable keyword lookups */
		}
		break;
	case 2387:
#line 16430 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    SYM		*sy, *mem;
		    char	*sid, *mid;

		    sid = str_add( STRNULL, id_getname() );
		    if ((sy = yypvt[-0].v) != (SYM *)0)
		    {
			if (sym_g_btype(sy) == T_STRUCT)
			{
			    for (mem=syStrInit(sy); mem; mem=syStrNext(mem))
			    {
				STprintf( esq->sbuf, ERx("%s.%s"), sid,
					sym_str_name(mem) );
				mid = str_add( STRNULL, esq->sbuf );
			      /* Members must be elementary */
				if (sym_g_btype(mem) == T_STRUCT
				    || sym_g_dims(mem))
				    er_write( E_E1000B_hfELEM, EQ_ERROR, 2, mid,
								id_getname() );
				erec_mem_add( mid, mem, sym_g_btype(mem) );
			    }
			}
			else
			{
			    if (!syBitAnd(sym_g_useof(sy), syFisVAR))
				er_write( E_EQ0057_grHOSTVAR, EQ_ERROR, 1,
								id_getname() );
			    erec_mem_add( sid, sy, sym_g_btype(sy) );
			}
		    }
		    id_free();
		}
		break;
	case 2388:
#line 16473 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    SYM		*sy, *mem;
		    char	*sid, *mid;

		    sid = str_add( STRNULL, id_getname() );
		    if ((sy = yypvt[-0].v) == (SYM *)0)
		    {
			/* Do nothing, as error already printed */
		    }
		    else
		    {
			if (sym_g_btype(sy) == T_STRUCT)
			{
			    for (mem=syStrInit(sy); mem; mem=syStrNext(mem))
			    {
				STprintf( esq->sbuf, ERx("%s.%s"), sid,
				   	sym_str_name(mem) );
				mid = str_add( STRNULL, esq->sbuf );
			      /* Members must be elementary */
				if (sym_g_btype(mem) == T_STRUCT
				    || sym_g_dims(mem))
				    er_write( E_E1000B_hfELEM, EQ_ERROR, 2, mid,
								id_getname() );
				erec_mem_add( mid, mem, sym_g_btype(mem) );
			    }
			}
			else		/* btype != T_STRUCT */
			{
			    erec_mem_add( sid, sy, sym_g_btype(sy) );
			}
		    }
		    id_free();
		}
		break;
	case 2389:
#line 16524 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    sc_hostvar = FALSE;	/* re-enable keyword lookups */
		}
		break;
	case 2390:
#line 16528 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    sc_hostvar = FALSE;	/* re-enable keyword lookups */
		    sc_popscptr();	/* move SC_PTR back a word */
		}
		break;
	case 2391:
#line 16534 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    SYM		*sy;
		    i2		stat;

		    sy = (SYM *)0;
		    stat = symRsEuc( &sy, F_CLOSURE, syFisVAR|syFisCONST );
		    if (stat != syL_OK)
		    {
			er_write( E_EQ0502_hNOVAR, EQ_ERROR, 2, ERx("FORTRAN"),
								id_getname() );
			sy = (SYM *)0;
			/* 
			** Undeclared?   Enter if no dims used and not a 
			** structure reference
			*/
			if (gr->F_dims == 0 && !yypvt[-0].i )
			{
			    /* Give hint for ESQL redecs */
			    sym_hint_type( (SYM *)0, T_UNDEF, 0 );
			    sy = symDcEuc( id_getname(), 0, F_BLOCK,
				syFisVAR, F_CLOSURE, SY_NORMAL );
			    if (sy)
				sym_s_btype( sy, T_UNDEF );
			}
		    }
		    else	/* state == syL_OK */
		    {
			/* 
			** Are we following use of a structure variable?
			** e.g., as in SELECT, FETCH or INSERT stmts).
			** If so, this is probably an indicator array.
			*/
			if (erec_vars() > 0)
			{
			    /* Definitely an indicator array if it uses one 
			    ** level of subscripting lower than that with 
			    ** which it was declared.
			    */
			    if (sym_g_dims(sy) == gr->F_dims+1)
			    {
				erec_ind_add( F_ARR_BASE, id_getname(),
					F_ARR_EXPR, sy );
			    }
			    else
			    {
				erec_ind_add( F_ARR_BASE, id_getname(),
					(char *)0, sy );
				if (gr->F_dims != sym_g_dims(sy))
			    	    er_write( E_E10003_hfINDIR, EQ_ERROR, 3,
					id_getname(), er_na(sym_g_dims(sy)),
					er_na(gr->F_dims) );
			    }
			}
			else	/* Not an indicator array */
			{
			    if (gr->F_dims != sym_g_dims(sy))
				er_write( E_E10003_hfINDIR, EQ_ERROR, 3,
				    id_getname(), er_na(sym_g_dims(sy)),
				    er_na(gr->F_dims) );
			}
		    }
		    yyval.v = sy;		/* Return pointer to sym */
		 }
		break;
	case 2392:
#line 16600 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    sym_f_init();
		    gr->F_dims = 0;
		}
		break;
	case 2393:
#line 16612 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = FALSE;
		}
		break;
	case 2394:
#line 16616 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    yyval.i = TRUE;
		    gr->gr_type = T_STRUCT;
		}
		break;
	case 2396:
#line 16635 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    id_add( yypvt[-0].s );	/* Save string for output */
		    sym_fpush( sym_find(yypvt[-0].s) );    /* Stack var ref */
		}
		break;
	case 2398:
#line 16650 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    SYM		*sy;
		    i2		stat;

		    stat = symRsEuc( &sy, F_CLOSURE, syFisVAR|syFisCONST );
		    if (stat == syL_OK 
			&& (sym_g_dims(sy) || sym_g_btype(sy) == T_STRUCT))
		    {
			sc_eat( id_add, SC_SEEN|SC_NEST, ERx(")"), '(', ')' );
			gr->F_dims = 1;
		    }
		    else
			SC_PTR--;
		}
		break;
	case 2399:
#line 16670 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/fortransq/forsq.yf"
{
		    id_add( yypvt[-0].s );
		    gr->F_dims = 0;
		}
		break;
		}
	return 0; /* Success */

	}
