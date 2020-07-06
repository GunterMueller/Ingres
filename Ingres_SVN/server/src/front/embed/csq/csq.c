
#line 6 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
/*
** Copyright (c) 2004, 2008 Ingres Corporation
*/

/* %L language - Which language are we using for symbol table? */
/* %L merge <esqlgram.my> -- get the master grammar */
/* %L fake begin */
/* %L fake end */
/* %L language begin 
** Define EQ_X_LANG where X is the EUC or PL1 for the symbol table
*/
# define	EQ_EUC_LANG
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
#define	tLBRKT		589
#define	tRBRKT		590
#define	tSEMICOL		591
#define	tARROW		592
#define	tLBRACE		593
#define	tRBRACE		594
#define	tPOUND		595
#define	tAMPERSAND		596
#define	tCTYPE		597
#define	tCTYPE_I		598
#define	tCTYPE_L		599
#define	tCTYPE_S		600
#define	tCTYPE_F		601
#define	tCTYPE_D		602
#define	tCTYPE_C		603
#define	tCTYPE_WC		604
#define	tCUNSIGNED		605
#define	tCBADTYPE		606
#define	tCTAG		607
#define	tCSTORAGE		608
#define	tCEXTERN		609
#define	tCTYPEDEF		610
#define	tCSTRUCT		611
#define	tCENUM		612
#define	tCENUMTAG		613
#define	tCLPAR_STAR		614
#define	tDEFINE		615
#define	tCFUNCVAR		616
#define	tCVARCHAR		617
#define	tCVARBYTE		618
#define	tCNVARCHAR		619
#define	tCCONST		620
#define	tCVOLATILE		621
#define	tOR		622
#define	tAND		623
#define	tLT		624
#define	tGT		625
#define	tNE		626
#define	tLE		627
#define	tGE		628
#define	tEQOP		629
#define	tPLUS		630
#define	tMINUS		631
#define	tSTAR		632
#define	tSLASH		633
#define	tEXP		634
#define	tUNARYOP		635

#line 1002 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
	

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


#line 1083 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
/* %L locals - Include L locals and global gr structure */
/* %L locals begin - Local variables and gr structure for C */
# include	<eqc.h>
# include	<ere0.h>
# include	<iisqlca.h>

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
	i4	gr_rcnt;	/* count of result row entries (for RPPs) */

	/* Add L (C) dependent members after this statement */
	i4	C_rec;		/* C record level */
	i4	C_storage;	/* extern, static, auto, register, typedef */
	i4	C_size;		/* Size of declaration */
	i4	C_indir;	/* Indirection of variables */
	i4	C_dec;		/* In declaration or usage */
	i4	C_varcharbyte;  /* Is a varchar, varbyte or nvarchar decl. */
				/*     0       = neither		   */
				/*     T_VCH   = varchar		   */
				/*     T_VBYTE = varbyte		   */
				/*     T_NVCH  = nvarchar		   */
	SYM	*C_struct;	/* Special pointer for references to structs */
	STR_TABLE *C_strvals;	/* String table for C values */
	C_VALS	*C_cvals;	/* C value structure */
};
GLOBALDEF struct gr_state _gr_local ZERO_FILL;
GLOBALDEF struct gr_state *gr = &_gr_local;
GLOBALDEF char *sqlca_var = NULL;
struct gr_state tempgr;

/* Where do C array indices start? */
# define C_NDX_BASE	0

/* How do you index a C array? */
# define C_NDX_EXPR	ERx("[%d]")

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

#line 17161 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"


/* %L mechanism */
/* %L mechanism begin - All the stuff to do with gr_mechanism() */

GLOBALDEF GR_TYPES   gr_typetab[] = {
	ERx("short"),	 	T_INT, 		sizeof(short),
	ERx("int"),		T_INT, 		sizeof(int),
	ERx("long"), 		T_INT, 		sizeof(long),
	ERx("void"), 		T_INT,		sizeof(int),
	ERx("float"), 		T_FLOAT,	sizeof(float),
	ERx("double"), 		T_FLOAT,	sizeof(double),
	ERx("char"), 		T_CHAR,		0,
	ERx("wchar_t"), 	T_WCHAR,	0,
	ERx("IISQLCA"),		T_SQLCA,	sizeof(IISQLCA),
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
	(char *)0,		0,		0,
	ERx("ERget"),		T_CHAR,		0,
	ERx("ERx"),		T_CHAR,		0,

	(char *)0,  		0, 		0
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
c_prtval( symval )
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
gr_mechanism( flag, arg1, arg2, arg3 )
i4	flag;
PTR	arg1;			/* Extra arguments for particular cases */
PTR	arg2;
PTR	arg3;
{
    register SYM	*sy;
    static   i4	einit = 0;
  /* DML routines */
    extern   i4	gen__sqltab[];
    i4			scStrIsCont(),
			esq_repeat(),
			yyesqlex(),
			sc_iscomment();

    switch (flag)
    {
      case GR_EQINIT:
	eq->eq_lang = EQ_C;
	eq->eq_def_in = ERx("sc");		/* s for SQL */
	eq->eq_def_out = ERx("c");
	if (!einit)
	{
	    eq->eq_in_ext = ERx("sc");
	    eq->eq_out_ext = ERx("c");
	    einit = 1;
	}
	eq->eq_sql_quote = '\'';
	eq->eq_quote_esc = '\\';
	eq->eq_host_quote = '"';

      /* tell the world we're Embedded SQL! */
	dml->dm_lang = DML_ESQL;
	dml->dm_gentab = gen__sqltab;
	dml->dm_lex = yyesqlex;
	dml->dm_strcontin = scStrIsCont;
	dml->dm_iscomment = sc_iscomment;

	dml->dm_repeat = esq_repeat; 	/* Provide hook for REPEATED queries */

	gr->gr_type = T_NONE;
	gr->gr_sym = (SYM *)0;
	gr->gr_id = ERx("");
	gr->gr_flag = GR_HOSTCODE;
	gr->C_dec = GR_C_DEC;
	gr->C_rec = 0;
	gr->C_storage = T_NONE;
	gr->C_size = 0;
	gr->C_indir = 0;
	gr->C_struct = (SYM *)0;
	gr->C_varcharbyte = 0;
	gr->C_cvals = (C_VALS *)0;
	if (gr->C_strvals)
	    str_free( gr->C_strvals, (char *)0 );
	else
	    gr->C_strvals = str_new( ID_MAXLEN );

	esq->inc = 0;
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
		    sy = symDcEuc(gt->gt_id, 0, C_BLOCK,
			    (syFisTYPE|syFisSYS), C_CLOSURE, SY_NORMAL);
		    sym_s_btype( sy, gt->gt_rep );
		    sym_s_dsize( sy, gt->gt_len );
		    sym_s_indir( sy, 0 );
		}
	    }

	    /* Declare "generic null" as a symbol */
	    sy = symDcEuc( ERx(".null"), 0, C_BLOCK, syFisVAR|syFisSYS,
			    C_CLOSURE, SY_NORMAL );
	    sym_s_btype( sy, T_NUL );
	    sym_s_dsize( sy, sizeof(i4) );
	    sym_s_indir( sy, 0 );
	    gr->gr_null = sy;

	    if (eq->eq_flags & EQ_COMPATLIB)
	    {
		static C_VALS	cv_local;

		cv_local.cv_flags = CvalFUNCVAR;
		cv_local.cv_dims[0] = '\0';
		/* Continue to declare internal variables */
		for (; gt->gt_id; gt++)
		{
		    sy = symDcEuc(gt->gt_id, 0, C_BLOCK,
			(syFisVAR|syFisSYS), C_CLOSURE, SY_NORMAL);
		    sym_s_btype( sy, gt->gt_rep );
		    sym_s_dsize( sy, gt->gt_len );
		    sym_s_vlue( sy, &cv_local);
		    sym_s_indir( sy, 0 );
		}
	    }
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

      case GR_LOOKUP:
	/* How should we look up this name? */
	*(i4 *)arg2 = tNAME;
	switch (gr->C_dec)
	{
	  case GR_C_DEC: 
	    if (sy = sym_resolve(NULL, (char *)arg1, C_CLOSURE, syFisTYPE))
		*(i4 *)arg2 = tCTYPE;
	    break;
	  case GR_C_TAG:
	    if (sy = sym_resolve(NULL, (char *)arg1, C_CLOSURE, syFisTAG))
	    {
		if (sym_g_btype(sy) == T_INT)
		    *(i4 *)arg2 = tCENUMTAG;
		else
		    *(i4 *)arg2 = tCTAG;
	    }
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
	  case GR_C_USE:
	    /*
	    ** See if this is a special Kanjii word (ERxxx).
	    ** We've already initialized *arg2 to tNAME.
	    */
	    sy = sym_resolve( (SYM *)0, (char *)arg1, C_CLOSURE, 
			      syFisVAR|syFisCONST );
	    if (sy)
	    {
		register C_VALS	*cv;

		if ((cv = (C_VALS *)sym_g_vlue(sy)) != NULL &&
			  cv->cv_flags == CvalFUNCVAR)
		    *(i4 *)arg2 = tCFUNCVAR;
	    }
	    break;
	}
	break;

      case GR_STMTFREE:
	str_reset();
	gr->gr_type = T_NONE;
	gr->gr_sym = (SYM *)0;
	esq->flag = 0;
	break;

      case GR_CLOSE:
	break;

      case GR_DUMP:
	{
	    register FILE	*f = eq->eq_dumpfile;

	    SIfprintf( f, ERx("GR_DUMP: gr_ \n") );
	    SIfprintf( f, ERx("  sym = 0x%p, id = '%s', type = %d, flag = 0%o\n"),
		       gr->gr_sym, gr->gr_id, gr->gr_type, gr->gr_flag );
	    SIfprintf( f, ERx("  func = %d, mode = 0%o\n"),
		       gr->gr_func, gr->gr_mode, gr->C_dec );
	    SIfprintf( f, ERx("  C_rec = %d, C_storage = %d, "),
		       gr->C_rec, gr->C_storage );
	    SIfprintf( f, ERx("C_size = %d, C_indir = %d\n"), 
		       gr->C_size, gr->C_indir );
	    SIfprintf( f, ERx("  C_dec = %d, C_struct = 0x%p\n"),
		       gr->C_dec, gr->C_struct );
	    SIfprintf( f, ERx("  C_varcharbyte = %d\n"), gr->C_varcharbyte );
	    SIfprintf( f, ERx("ESQ: esq_ \n") );
	    SIfprintf( f, ERx("  flag = 0%o,"), esq->flag );
	    SIfprintf( f, ERx(" inc = 0%o, dml_exec = 0%o\n"), 
		       esq->inc, dml->dm_exec );
	    SIflush( f );
	}
	break;

      case GR_BLOCK:		/* Enter/exit a block - inform symbol table */
	break;
       
      case GR_EQSTMT:
	{
	    i4		mode = (i4)arg1;
	    char	*kword = (char *)arg2;
	    char	*kw2 = (char *)arg3;
	    char	buf[50];

	    /* Turn off declarations flag and generate line directive */
	    gr->gr_flag &= ~GR_HOSTCODE;
	    gr->C_dec = GR_C_USE;
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
		db_key( kword );
		esq_init();
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
	591, 829,
	-2, 831,
	-1, 379,
	458, 811,
	-2, 810,
	-1, 503,
	504, 238,
	-2, 1306,
	-1, 669,
	502, 984,
	-2, 977,
	-1, 671,
	502, 984,
	-2, 982,
	-1, 729,
	485, 555,
	499, 555,
	597, 555,
	598, 555,
	599, 555,
	600, 555,
	601, 555,
	602, 555,
	603, 555,
	604, 555,
	617, 555,
	618, 555,
	619, 555,
	-2, 552,
	-1, 932,
	256, 1276,
	391, 1276,
	504, 1470,
	-2, 1468,
	-1, 933,
	256, 1277,
	391, 1277,
	504, 1471,
	-2, 1469,
	-1, 983,
	504, 238,
	-2, 236,
	-1, 1209,
	504, 238,
	-2, 414,
	-1, 1434,
	503, 2050,
	-2, 2087,
	-1, 1465,
	629, 2140,
	-2, 1309,
	-1, 1574,
	502, 2073,
	591, 2073,
	-2, 2430,
	-1, 1575,
	459, 2073,
	479, 2073,
	591, 2073,
	-2, 2430,
	-1, 1576,
	281, 2073,
	502, 2073,
	-2, 2430,
	-1, 1578,
	281, 2073,
	468, 2073,
	502, 2073,
	591, 2073,
	-2, 2430,
	-1, 1580,
	281, 2073,
	502, 2073,
	-2, 2430,
	-1, 1585,
	502, 2073,
	591, 2073,
	-2, 2430,
	-1, 1595,
	591, 2371,
	-2, 2383,
	-1, 1597,
	591, 2371,
	-2, 2383,
	-1, 1600,
	591, 2371,
	-2, 2383,
	-1, 1636,
	504, 2421,
	-2, 2419,
	-1, 1637,
	504, 2422,
	-2, 2420,
	-1, 1776,
	463, 1276,
	501, 1276,
	504, 1276,
	-2, 998,
	-1, 1875,
	314, 1317,
	318, 1317,
	322, 1317,
	323, 1317,
	329, 1317,
	-2, 523,
	-1, 1880,
	485, 555,
	499, 555,
	597, 555,
	598, 555,
	599, 555,
	600, 555,
	601, 555,
	602, 555,
	603, 555,
	604, 555,
	617, 555,
	618, 555,
	619, 555,
	-2, 548,
	-1, 1881,
	485, 555,
	499, 555,
	597, 555,
	598, 555,
	599, 555,
	600, 555,
	601, 555,
	602, 555,
	603, 555,
	604, 555,
	617, 555,
	618, 555,
	619, 555,
	-2, 550,
	-1, 1968,
	501, 1787,
	591, 1787,
	-2, 1776,
	-1, 2083,
	456, 1276,
	483, 1276,
	504, 1276,
	-2, 876,
	-1, 2097,
	485, 1200,
	499, 1200,
	597, 1200,
	598, 1200,
	599, 1200,
	600, 1200,
	601, 1200,
	602, 1200,
	603, 1200,
	604, 1200,
	617, 1200,
	618, 1200,
	619, 1200,
	-2, 497,
	-1, 2103,
	502, 782,
	-2, 1276,
	-1, 2124,
	504, 1183,
	-2, 1181,
	-1, 2150,
	502, 781,
	-2, 1227,
	-1, 2195,
	593, 2317,
	-2, 2319,
	-1, 2201,
	593, 2337,
	-2, 2336,
	-1, 2387,
	458, 1276,
	501, 1276,
	504, 1276,
	-2, 922,
	-1, 2413,
	502, 1276,
	-2, 1282,
	-1, 2491,
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
	503, 1305,
	569, 1305,
	591, 1305,
	-2, 1309,
	-1, 2651,
	458, 776,
	502, 782,
	-2, 1276,
	-1, 2652,
	458, 777,
	-2, 720,
	-1, 2724,
	504, 1276,
	-2, 1293,
	-1, 2725,
	504, 1277,
	-2, 1294,
	-1, 2900,
	314, 1317,
	318, 1317,
	322, 1317,
	323, 1317,
	329, 1317,
	-2, 524,
	-1, 2907,
	485, 555,
	499, 555,
	597, 555,
	598, 555,
	599, 555,
	600, 555,
	601, 555,
	602, 555,
	603, 555,
	604, 555,
	617, 555,
	618, 555,
	619, 555,
	-2, 528,
	-1, 3213,
	501, 442,
	503, 442,
	-2, 444,
	-1, 3214,
	501, 443,
	503, 443,
	-2, 445,
	-1, 3285,
	314, 528,
	318, 528,
	322, 528,
	323, 528,
	326, 528,
	329, 528,
	336, 596,
	503, 528,
	-2, 555,
	-1, 3409,
	339, 1362,
	-2, 1378,
	-1, 3590,
	339, 1363,
	-2, 1379,
};

#define	YYNPROD		2469
#define	YYLAST		11478

short yyact[] = {
	  2307,  1161,  2756,  3811,  3793,  3812,  3664,  3795,   669,  2623,
	  3591,  1824,  2710,  3091,  1561,  3492,  2319,  3483,  3248,  2768,
	  1946,  1294,  2620,  2948,  2295,   988,  2246,  3166,  3139,  3083,
	  3588,  3404,  2122,  2626,  1200,  3295,  2117,  3424,  1128,  3299,
	  3294,  2736,  3290,  3088,  2149,  3212,  2487,  3471,  1240,  2034,
	  2105,  1687,  3472,  3208,  2741,  2293,  2728,  1112,  2182,  2212,
	  2357,  2715,  2622,   555,   585,  2136,   730,   503,  3140,  3583,
	  3355,  3054,  2284,  3215,  2145,  3017,   844,   558,  2970,  1387,
	  2121,  1470,   593,  1474,  2875,  1788,   500,  1947,  2123,  3430,
	  2092,  3155,  1469,  2836,   589,  2698,  2304,  2774,  2697,  1596,
	  3287,  1473,   565,  2482,  1468,  2665,  1471,  2709,  2759,  3029,
	  2528,  3058,  2339,  2820,  2647,  3031,   886,  1800,  1290,  1600,
	  2566,   960,  2262,  2762,  2529,  2492,  2186,  3293,  2358,  1599,
	  1595,  2429,  2885,  2764,  1876,  2812,   595,  1534,  2004,  2142,
	  2422,  1744,  2375,  2335,  2752,  1560,  2417,  2334,  2807,  2281,
	  1598,  2332,  2108,  2353,  3169,  2336,  1562,  2779,  2133,  2792,
	  3124,  2758,  1352,   643,  2242,  2747,  2178,   559,   559,   559,
	  2646,   588,   594,   559,   559,  1085,  1853,  2098,   594,   559,
	   604,   588,   559,   613,  2095,   559,   588,  2181,   840,   559,
	   588,   588,  1514,   559,   559,   588,   638,  1587,  1985,  1113,
	  2638,   621,  2052,   657,   562,   657,   507,  1421,  2009,   670,
	  1961,  1017,   613,   878,  1951,   686,  1901,   594,   588,   594,
	   691,   692,   594,   661,  3209,  2405,   705,  1887,  1340,  1696,
	   727,  1922,   991,  1731,  2739,  1910,  1794,  1351,  2778,  1676,
	   890,  2435,   964,  1797,  2886,  1952,   578,  1298,  1300,  1921,
	   598,   598,  1541,  1661,  1467,  1508,  1511,  1509,  1507,  1480,
	  1634,  2669,   617,  1496,  2675,  2241,   631,  1460,  1502,  1400,
	   634,   635,   865,  1074,   888,  1419,  1402,  1435,   637,  1395,
	  1484,  1372,  2001,   687,   641,   642,  1384,  1377,   711,  1938,
	  1328,  1080,  1339,  1299,   992,   557,   577,  1464,   731,  1334,
	   507,   507,   753,  2829,  2421,   615,  2483,   603,   718,  1250,
	   606,  1278,   729,  3284,  2416,  1207,   664,   717,   507,  2763,
	   608,   666,  1265,  1597,   728,   665,  2793,  1535,  1670,  1451,
	   613,  1711,  1101,   507,  2240,  3125,  1373,   795,  1100,  2292,
	  1645,  1070,   741,   686,   667,   588,   559,   559,   588,   953,
	   559,   559,   559,   507,   588,   588,     7,  2185,   686,   668,
	   686,  1593,  2164,  1765,   712,  1403,  1565,   750,   930,  1019,
	  1367,   889,   889,  1357,   716,   889,  2165,   889,   889,   656,
	   679,   656,   851,   889,   889,  1432,  1865,  3219,  1594,  2406,
	   663,   612,  1191,   620,  2099,    63,   959,  1132,   644,   929,
	   731,  2838,  1680,  1180,  1036,    51,  2699,  1586,    48,  1572,
	  1205,  1375,   892,   849,  1499,  1225,  1212,   900,   901,  1437,
	   863,   628,   802,   909,   910,  1079,  1069,   954,   955,   956,
	   504,  1147,  1466,  1130,  2330,   507,   957,  1689,  3269,  1201,
	  3090,   983,  2621,  1099,   588,  1738,  1291,   987,   731,   970,
	  3268,  3261,  2794,  1738,  3356,  3526,  3413,  1466,  1702,  1703,
	  2627,  2628,  2629,  2214,  1659,   507,  1660,  3432,  3547,  3369,
	   713,   706,  1973,  1738,   950,   951,   507,  2790,  2789,  2770,
	  1162,  2700,  3327,   507,  2493,  1095,  1702,  1703,  2627,  2628,
	  2629,  3306,   843,  3305,  1099,  2635,  1738,  3233,  2934,  2524,
	  3546,  2472,  2700,  1665,  2470,   889,  2791,  1446,  2454,   845,
	  1478,   889,   756,   889,  1479,  2670,  2451,  2033,  2032,  2031,
	  1936,  1931,  1237,  3356,  2214,  2216,  2219,  2218,  1665,  2217,
	  2770,   513,   513,  1083,   845,  2668,  2479,   756,   564,  3266,
	   845,  2771,  2770,   756,  2680,  1023,  1639,   829,   671,  1043,
	  1044,  1024,  1228,  1025,  2205,  2700,   513,  2200,  1654,   867,
	   869,  2194,   513,   591,  2639,  2677,  1622,  1623,   581,   602,
	   845,  2674,  1702,  1703,  2627,  2628,  2629,   611,  3390,  2703,
	  1702,  1703,  2627,  2628,  2629,  1702,  1703,  2627,  2628,  2629,
	  2285,  1163,   513,   845,  1478,  3545,   756,  1167,  1479,  3374,
	  1702,  1703,  2627,  2628,  2629,  1689,   770,  1738,   688,  1133,
	   690,  3575,   613,   693,  1689,   513,   588,   755,  1702,  1703,
	  2627,  2628,  2629,  1702,  1703,  2627,  2628,  2629,  1659,  1197,
	  1660,  1666,  1666,   485,   813,  2666,  1090,   626,  2220,   824,
	  1974,  1072,   506,   845,  1478,  1461,  1462,  1116,  1479,  1659,
	  1659,  1660,  1660,   859,  1133,   839,  1477,  1476,  1383,  2667,
	  2794,   614,  1173,  1458,  1228,   513,  3267,   563,   845,  1478,
	  1461,  1462,  2215,  1479,  2124,  1659,  2640,  1660,  2636,  2201,
	   594,   847,   846,   614,  1666,   812,   513,   847,   846,  2195,
	   513,   845,   841,  3076,   647,  1251,   842,  1082,  2666,  1702,
	  1703,   494,   493,  1255,  1738,  1624,  2243,  1209,  2183,  1249,
	  1266,  2503,  1273,   513,  1630,  1631,  1632,   847,   846,  2663,
	  1233,  1702,  1703,  2627,  2628,  2629,  2771,  2770,  3366,  1293,
	  1295,  2229,  1176,  1627,  1628,  1629,   506,   506,  3084,  2221,
	  1477,  1476,   507,  1618,   797,  2202,  2204,  2002,  2203,  1702,
	  1703,  2627,  2628,  2629,   506,  1093,  1246,  2931,  2535,   616,
	  1702,  1703,  2627,  2628,  2629,  1756,  3198,   485,   961,   506,
	  1174,  1702,  1703,  2627,  2628,  2629,  1818,  1819,   508,  1466,
	  3833,   685,  1219,  3751,  3752,  3719,  3753,  3754,  2183,   506,
	  1477,  1476,  1789,  1648,  2244,   720,   721,   722,  1202,  1203,
	  1094,   485,  1147,  1073,   984,  1162,  1179,  1252,    21,  1216,
	  2771,  2770,  1703,  1245,   513,  1477,  1476,   513,   710,  1702,
	  1703,  2627,  2628,  2629,  2703,  2783,  2784,  2786,  2787,  2788,
	  2785,  1702,  1703,  2627,  2628,  2629,  1082,   485,   847,   846,
	   492,  2227,  1077,  2796,  2789,   590,   590,  1073,  1274,  1649,
	  1162,  1397,   590,  3315,  2627,   590,  2927,   590,  2926,  2925,
	   590,  2790,  2789,   786,   590,   590,  2754,  2228,   962,   590,
	  1892,   506,   508,   508,  2208,  2206,  2207,   962,   513,  2209,
	  2210,  1301,  1584,  2942,   513,  1072,   590,  1764,   510,  1289,
	  2791,   590,   590,   590,  1141,  1972,   590,   751,   756,   563,
	  1763,   506,   511,  2754,   590,   513,  1948,  1891,   513,   512,
	  1109,   752,   506,   594,  1523,  1441,  1108,   513,   513,   506,
	  1554,   845,  1517,  1228,  1166,   508,  1518,  1721,   845,   841,
	  1165,  1172,  1177,   842,  1183,  1211,  1529,   751,   756,  1141,
	  1078,  2460,   594,   513,  1720,   513,  1722,   678,  2760,  1724,
	   513,   752,  1204,  1738,  3786,  1454,  1098,   513,   559,   588,
	   588,   588,  1472,   513,  1721,  1102,   845,  1478,  1336,   756,
	  3767,  1479,  1539,  1103,   614,  1491,  1162,  1110,  3738,  1111,
	   756,  1720,  1493,  1722,   678,  2760,  1724,  2012,   513,   513,
	   845,  1478,  1067,   756,  1071,  1479,  1098,  1170,  1098,   513,
	  1492,  1649,  1171,  1318,   590,  1162,  2011,   934,  1524,   513,
	   928,  1253,   513,  1184,  1329,  1559,  1182,  3735,  1268,   590,
	  1254,  1282,   590,  1194,  1532,  1370,   510,  1267,   590,   590,
	  1281,  1184,  1206,  1193,  1258,  1543,  1192,   508,  1945,  1257,
	   511,  1292,   499,  1296,  2794,  1280,  1173,   512,   508,  3667,
	  2433,  1279,  1555,   731,  1256,  1569,  1374,  1220,  2753,  1215,
	  1224,  3177,   563,  1386,  1533,  3568,  1199,  1198,   847,   846,
	  1210,  1230,   510,  3556,  1217,   847,   846,   614,   674,  1082,
	  1232,   513,  1337,  1168,  1244,  1169,   511,  1229,   507,  1236,
	   510,  1450,   614,   512,  3555,  2753,  1489,   559,  1516,  3537,
	  1491,  1568,  1491,  1099,   511,  1738,   614,  1493,   513,  1493,
	   513,   512,   713,  1477,  1476,  1940,  1941,  3674,   590,  3673,
	  3334,  1644,  1650,  3333,   948,  1492,  2007,  1492,  1316,   614,
	  3182,  1913,  1914,  3337,  1276,  1939,  1374,  1477,  1476,   966,
	  1374,   967,   513,  3339,  3328,  1324,  1655,   713,   713,   932,
	   732,   513,  1331,  1133,  3084,  2006,  2005,  3621,  3531,  3620,
	  3530,  1503,  1706,   933,  1707,  3182,  1710,  1726,  1728,   513,
	   512,  1732,  1304,  1734,  1678,   513,  1235,   588,   506,   588,
	  1690,   507,  1741,  1481,  1530,  1745,   588,  2156,  2155,  1066,
	  1368,  2783,  2784,  2786,  2787,  2788,  2785,  1358,  3079,  1694,
	  1695,   594,  1567,  1566,   798,  1738,  2302,  1759,   513,  2783,
	  2784,  2786,  2787,  2788,  2785,  1702,  1703,  2627,  2628,  2629,
	   588,   588,   513,   513,   670,   686,  1754,  1738,  1429,  1760,
	  1369,  3024,  1485,  2222,  1787,  3000,  1497,   510,  1071,  1791,
	   785,   756,   720,   721,   722,  1799,  1801,  1802,   614,  1445,
	  3257,   511,  2223,   751,   756,   714,  2995,   513,   512,  2994,
	   513,  1374,  1417,  2984,   596,  1795,  2516,   752,  2982,  2978,
	  1102,  1430,  1335,   513,  1712,  1727,  1762,  1849,  1103,  1424,
	  2967,  2601,  1770,  1708,  1440,  2585,   590,  1442,  1443,  1717,
	   590,   727,  2016,  1879,  1880,  1785,  1881,  1882,  1883,  1715,
	   713,   658,  2359,  1283,  1521,  1453,  1997,  3326,  1486,  3325,
	  2754,   507,  3141,  1980,   508,  1359,  1120,   614,  3317,   629,
	  1904,  2463,  1206,  3075,  1758,  1520,  1121,  3256,  2966,   751,
	   756,  1674,   510,  1826,  2011,  1847,   666,  1845,  1526,  1783,
	  1677,   674,  1863,   752,  1363,  1362,   511,  1742,  1020,   513,
	  1895,  1896,   614,  2986,   590,  2985,   707,   596,   708,  1884,
	  1844,  1123,  2695,  1126,   513,  1862,  1120,  2657,  2591,  1804,
	  2590,  1721,   709,   729,  1355,  1843,  1121,  2641,   513,  1930,
	  1119,   831,  2523,  1823,  1122,   728,  1825,  1551,  1720,  2224,
	  1722,   678,  2760,  1724,  1834,  2495,  2463,  2494,  2462,   707,
	  1341,   708,   563,  1792,  1793,  1125,  1124,  1617,  2450,  1842,
	  2448,  1123,  2255,  3108,  2253,   709,  1668,  1573,  2064,   816,
	  2063,   513,  1397,  2158,  1975,   510,   563,  2059,  1099,  2058,
	  1119,  1129,   614,  1588,  1122,  1850,  1983,   586,  1141,   511,
	  1592,  1949,   995,  2068,   566,   569,   571,   570,   572,   573,
	   567,   568,   713,  2062,  2047,  1125,  1124,   507,  2035,  2056,
	  1905,  2055,  1771,  1777,   574,   575,   576,   815,   751,   756,
	  1099,   751,   756,  1698,  1704,  2162,   896,  2161,  2160,  2627,
	  2303,  1129,   752,   614,   801,   752,  2897,  1767,   513,  1664,
	  1082,   513,   896,   816,   510,   816,  1679,   513,   751,   756,
	  1683,   816,  2753,  1475,  1682,  2019,  1813,  1740,   511,  2000,
	  1995,   614,   752,   751,   756,   512,  1494,  1291,   513,   680,
	  2049,   616,  2048,  1099,   506,  2071,  2072,   752,  2042,  1691,
	  2041,  1118,  1875,   513,   588,  1667,  2938,   566,   569,   571,
	   570,   572,   573,   567,   568,  1651,  1784,   720,   721,   722,
	   588,   513,  1620,  1188,   751,   756,  1500,   574,   575,   576,
	  1099,  2096,  1513,   671,   751,   756,  1498,  1709,   752,  2030,
	  1735,  2029,  1425,  1356,   513,  2027,  1967,  1898,   752,  1899,
	  1747,  3107,  1487,  1748,   513,  1393,  1194,   590,   590,  1746,
	   866,  1356,  1753,  2014,  1897,  1354,  1193,   768,   614,  1192,
	  1993,   513,  1992,  1971,  3658,  1970,   566,   569,   571,   570,
	   572,   573,   567,   568,  1778,  1102,   590,   506,  1319,  1935,
	  1780,  1934,  2090,  1106,  1781,  1184,   574,   575,   576,  1933,
	  1929,  1932,  1928,   590,   590,   590,  1308,  1091,  1942,   771,
	  1020,  1494,   904,  1494,  1805,  1822,   903,   899,   885,  1976,
	  1809,  2754,   882,  1810,   862,  1944,   856,   507,  1102,  1868,
	   754,  1827,  1838,   823,  1852,   989,  1103,  1867,  1614,  1608,
	  1606,  1607,  1612,  1613,  1609,  1610,  1611,  1605,   826,   510,
	   732,   779,  1616,  1615,   559,  1877,   818,   751,   756,   782,
	  2073,  1918,  1889,   511,  2078,   773,  1907,   907,  1374,   513,
	  2015,   752,   764,  3060,  1894,   769,  1688,   513,  2279,  2280,
	  3194,  2389,  1721,   895,   831,   713,   866,  2289,  2666,  2176,
	  2526,  2184,   815,  2187,  1693,  2189,  2190,  2256,  1374,  1720,
	  2521,  1722,   678,  2760,  1724,   756,   613,  1959,  1956,   510,
	  3195,  1187,  1189,  2555,  2453,  2374,  2017,   506,  1116,  1368,
	   707,  1982,   708,   511,   513,   563,  2040,  2557,  1998,  2321,
	  2044,  1991,  1981,  1977,   505,  1926,   709,  2385,  2045,  1643,
	  2066,  2354,  1380,   588,  1761,  1356,  2388,  1924,  2688,  2543,
	  1099,   686,   686,  1072,  2317,  2286,  2371,  2468,  2373,  2559,
	  2556,  1925,  2689,  2658,  2028,   510,  2324,  2558,  2553,  2554,
	  2552,  2469,  2551,  2550,  2287,   588,   686,  1251,  2271,   511,
	   686,   686,  2259,  1099,  3803,  1255,   588,  2974,  2023,   510,
	  2548,   588,   513,   586,  1485,  2544,  2976,  2977,   686,  1343,
	  1266,  2348,  2349,   511,  2286,   588,  1717,  1826,  2271,  2302,
	  2412,  2239,  2264,  2753,  1988,  1989,  1715,  2076,  2069,  2070,
	   790,   590,  2247,   590,  2282,  3757,  1099,  1636,   505,   505,
	   590,  2077,  2387,  1839,  2329,  1322,   507,   507,  1228,   766,
	  3232,  1637,  3607,  1745,  2286,   590,   511,   916,   727,  1323,
	  2681,  2682,  3837,   512,   596,  2683,  2260,  1416,  2038,  3517,
	  2571,   733,  2039,   506,   590,   590,  3249,  2318,  2075,   713,
	  1845,  2570,   584,  2547,  1415,  2036,   787,  1845,  2344,  2037,
	  1846,   505,  1004,  2376,   586,  2088,  1826,  2271,  1348,  1252,
	   697,  1345,  2383,  1844,  2540,  2541,  2545,  2539,  1347,  2542,
	  1844,   713,   583,  1349,  1913,  1914,  3718,  3014,  2166,  2167,
	  2368,  2169,   696,  2170,  2168,   704,   513,  2355,  2171,  3606,
	  3605,  1860,  2972,  2973,  2177,  2188,  2568,  2569,  2191,  3703,
	   729,  2744,  2745,  2746,   703,   590,  1342,   756,   713,   586,
	   713,   563,   728,  3681,   566,   569,   571,   570,   572,   573,
	   567,   568,  3090,  2226,  2233,  2234,   582,  2235,  3593,  1005,
	  1504,  1505,  3056,   733,   574,   575,   576,  2245,   616,  3504,
	  1846,  2250,  2252,  2489,   586,  3455,  1779,  2488,  1420,  2490,
	  2510,  2511,  3304,  2476,  2430,  2360,   714,  1346,  3170,  1841,
	  1422,   713,  1840,   505,  2333,  2434,  3046,  2053,   917,   508,
	  2274,  2275,  2276,  1301,  2940,   511,  2939,  2443,  2291,  2880,
	  2871,   733,  2351,  2858,  2288,  1665,  1082,  2386,  3170,   596,
	  1002,  2684,  2670,  2602,  1378,  2314,  2322,  2394,  1003,   789,
	  2481,  2377,  2327,  1001,  1000,  2361,   834,  2350,  2415,   833,
	  2238,  1472,  2605,  2606,   835,  2257,  2533,  2258,  2091,   837,
	   638,  2086,  2263,  2054,  1927,  2272,   559,  2532,  2273,  1866,
	  3608,  1832,  2484,   506,  1808,  2610,  1685,  1681,  2618,  2531,
	  1656,  2534,  2506,  2507,  2508,  1652,  1633,  1472,  1472,  1472,
	  1472,  1392,  2290,  1350,  2296,  1332,  2391,  1320,  2309,  1259,
	  1222,  1084,  1336,  1253,   911,  2380,  2442,  3170,  2466,  2320,
	   908,  2323,  1254,   897,  1472,  2296,  1071,  1472,  1268,   889,
	   828,  1184,  1472,   684,  2400,   601,  1258,  1267,  2617,  2337,
	  2420,  1257,  1129,   626,  2340,  2337,  3822,  1228,  3769,  3740,
	  2419,  3715,  2607,  1543,  2681,  3662,  1256,  3626,  2424,  2633,
	  1329,  3554,  2411,  1282,   588,  3448,  2425,  3309,  2410,  2598,
	  1228,  1416,  1281,  2513,  2660,  2445,   713,  2447,  1415,  1449,
	  2440,  2655,  2656,  2441,  2437,  2438,  2382,  1280,  2504,  1040,
	   914,   858,   874,  1279,   854,  1768,  2505,   800,   590,  2396,
	  1752,   799,  1374,  2563,  3553,  2561,   875,   876,  1201,  2564,
	  2471,  1945,  1386,  1811,   590,  1558,  2496,  2480,  2474,  2497,
	  1963,  1966,  1297,  2473,  2664,  1263,  1337,  2661,  1964,  1965,
	  1102,  1846,  1968,  2502,  3671,   586,  1127,   586,  1103,  1390,
	  1228,  2283,  2089,  2514,  1129,  1920,  1439,  1326,   559,   559,
	  2426,  1311,  2427,  2721,   559,  2517,   777,  3192,  2519,  2330,
	  1271,  1846,  2436,  2527,  1978,  2537,  1374,  3804,  2562,  3768,
	  2733,  2734,   556,  1374,  2337,  3511,  2816,  1990,  1937,  2574,
	  1374,   686,  1994,   588,  2575,  2572,  1260,   971,  2577,  1999,
	  3141,  3625,   506,   506,   616,  1173,  2589,  2634,   740,  2798,
	   686,  2498,  2595,  2588,  1503,  2597,  1955,  1732,  2596,  2791,
	  2600,  2599,  2587,  2583,  2813,  2594,  2022,  1745,  2780,  2578,
	  2580,  2581,  2018,   742,  2232,  2712,  2712,   586,  2615,  2337,
	  1482,  2712,  2296,  1214,  1710,  2802,   915,  2801,  1457,  1242,
	   743,  2772,  2773,   586,   763,   559,   559,  2722,   559,   680,
	  1428,  3454,  2635,  2754,  2834,  1291,  2716,  3039,  2844,  1697,
	  1617,  2726,  2582,  1301,  2857,  1556,   683,  2862,   640,  1485,
	  1799,  2043,  1769,  2868,  2354,   588,  1497,  2459,  1563,  1684,
	  1846,  2635,  2486,  2050,   852,  2501,   810,  2877,  3733,  2057,
	  3338,  2841,  1092,  1795,  2060,   586,  3551,  2842,  2065,  2777,
	   590,   517,   997,   999,  2933,  2458,  2662,  2686,   814,  2174,
	  1638,   674,   554,  2856,  1721,  1082,  1335,   559,   559,  2398,
	  2408,  1027,   727,  2414,   791,  2418,  2418,   727,  2910,  2911,
	  1917,  1720,  2671,  1722,   678,  2760,  1724,   590,  2839,  2898,
	  3289,  1591,  1712,  2423,  2423,  1283,  3275,  2346,  2876,  3732,
	   736,  2837,  2678,  1869,  2679,  1361,   616,  1717,  2814,  1845,
	  2889,  2310,  2853,  2794,  2685,  2891,  2809,  1715,  2691,   590,
	  2308,  2692,  2899,  2847,  2848,  2830,  2849,  2530,  3739,  2687,
	   590,   678,  1844,  2854,  2708,   590,  2702,  2693,  2828,   629,
	  2833,  2704,  2637,  1677,  2712,  2712,  2701,  2719,  2341,   590,
	  1315,  2701,   743,   743,   729,  1026,   624,  2846,  1527,   729,
	  1870,  1871,  1213,  1434,  2330,   625,   728,  3121,  2945,  1481,
	  2956,   728,  2653,  2654,  2892,  2893,  3692,  2869,   622,  2859,
	   981,  2655,  2659,  2795,   623,  2636,  1465,  1286,  1288,  1287,
	  2803,  2365,   590,  1341,  2477,  2753,   677,   979,  2818,  1490,
	  2355,  1945,  2363,  2364,  3118,   980,  2270,  1721,  1536,  1537,
	  1519,   626,  2832,  3034,  2636,  2485,  2831,   974,  2313,  2312,
	  2981,   674,  2958,  3426,  2909,  1725,  1722,  2390,  2369,  1724,
	  2919,  2392,  2393,   925,   975,   735,  2983,   973,   507,  3695,
	  2074,   926,   977,  3123,  2330,  1625,  2952,  3002,  3003,  2401,
	  3122,  3427,  1475,   588,   588,   588,  1548,  2096,  2866,  1706,
	  3012,   969,   982,  2864,  2156,  2155,  2845,  2870,   939,   674,
	  2883,  1614,  1608,  1606,  1607,  1612,  1613,  1609,  1610,  1611,
	  3120,  2446,  1155,  2963,  2964,  1616,   733,  3119,  1475,  1475,
	  1475,  1475,  2711,  2711,  2433,  1570,  3135,  3282,  2711,   976,
	   610,  3048,   626,  3011,  2913,  3158,  2921,  2731,  2916,  1491,
	  3016,  2917,  3281,  2900,  3023,  1475,  1493,  3280,  1475,  3279,
	  2742,  3010,  2101,  1475,  1490,  3021,  1490,  1155,  2923,  3021,
	  2296,  2296,  3159,   830,  1492,  2936,  1945,  2950,  3052,  2937,
	  2797,  2944,   860,  3035,   864,   868,   870,  2946,  3412,  1553,
	  2957,  1967,  2960,  2947,  2962,  2965,  2961,  1721,  2951,  3095,
	  2968,  1525,  3096,  3097,  3098,  3081,   714,  1147,   918,   761,
	   920,  3026,  2337,   921,  1102,   945,  1722,   678,   946,  1724,
	  2296,   922,  1103,  1087,   894,   672,   947,   925,  3092,   624,
	   919,   356,   943,   887,   673,   926,  3132,  3132,   625,  3030,
	   944,  2822,  2852,   880,  1672,  1829,   924,  2980,  1953,  2352,
	  2823,   622,  2959,   677,  1642,  1376,  3071,   623,  2990,  1305,
	  1195,  1303,  1516,  2821,  3782,   940,  2999,  1816,  3104,  2824,
	  2998,  2915,  3148,  3151,  2997,   993,  3069,  1116,  3013,  1706,
	  2780,  3008,  2884,  1766,  1068,  3167,  3068,  3070,  3183,  3038,
	   746,   695,   923,  1226,   822,  3165,  3032,  2302,  3168,  2354,
	  1006,  2711,  2711,  1007,  3142,  3143,  1877,  3067,  3702,  3136,
	  3672,  2436,  3050,  3669,  2271,  1889,  3131,  3131,  3022,  2825,
	   935,  1018,  3121,  3011,  2826,  1826,  2956,  3507,   590,   936,
	  2843,   778,  3103,  3225,  1552,  2738,  1873,  3190,   738,   772,
	   781,  3080,   938,  2730,  3234,  3235,  3077,  1663,   937,  3179,
	   597,  1878,   681,  1662,   650,  3153,  2740,  2765,  3163,  2520,
	  2025,  1658,     2,  1081,  3193,   559,  2904,  3082,  2696,  3222,
	  2903,  1653,  1076,  1075,  2902,  2905,  2237,  1745,  1790,  3246,
	  3213,  2906,   727,  2225,  1626,  3214,  3265,  3228,  2805,  3117,
	  3210,   731,  3239,  1049,  3127,  3127,  1635,  3133,  3227,   507,
	  3300,  3134,  1048,  1621,  1046,  2213,  1057,  2211,  3243,  3244,
	  1619,  3152,  1045,  3226,  3149,  3109,  1657,   590,  3160,  3146,
	  3161,  3224,  3051,  2180,  2179,  3065,   507,  3292,  3064,  2592,
	  2593,  3063,  3059,  3172,  3181,  3370,  3057,  1238,  1127,  2676,
	  2199,  3308,  3185,  2943,  2198,  3078,  2197,   590,  3021,  2196,
	  1604,  1909,  3199,  3202,  3368,  3112,  3230,  3055,  3204,  3201,
	  3053,  2673,  3205,  2282,   729,  2376,  2193,  3314,  3207,  2672,
	   927,  2192,  3221,  3247,  1603,  2355,   728,  1602,  3021,  1601,
	  3066,  1050,  3021,  3341,  3342,  3312,  1042,  3240,  1039,  3236,
	  1038,  1037,  2175,  3250,  1035,   505,  1033,  1032,  3251,   495,
	    20,  1515,  3254,  2061,   307,  1361,  3255,  2067,  3274,  3018,
	  3329,  3259,  1472,  3018,   506,  3252,  3277,  2533,  2430,  3344,
	   306,  2097,  2296,   305,   599,  1510,  3344,  3310,  2532,   906,
	  3365,   590,   898,   436,  3346,  3347,  3348,   905,  3349,   590,
	  2531,   304,  2534,  1506,  3361,  3062,  3319,   303,  1501,  3021,
	  3021,  3021,  2766,  3021,   902,  3311,   302,   301,   889,   659,
	  3320,  2008,  2051,  2013,  3321,   300,  1495,   299,   559,  2586,
	  1341,  2799,   838,  3362,  2046,  1488,   298,  3398,  3399,  3401,
	  3394,   726,   780,  3323,   686,   783,   590,  3331,   792,   297,
	  3393,   590,  1483,   428,   893,  1737,   296,   295,  3340,   294,
	   820,   821,  1463,  1459,   425,  1456,   884,   293,   161,   160,
	  3110,   159,   158,   157,  3421,  3114,   156,   155,  3352,   686,
	   154,  3409,   153,  3030,  3351,   152,   151,   150,   149,   148,
	  3043,   147,  1371,  1394,   423,   881,   291,  3071,   474,  3367,
	  3360,   422,  3138,  2296,  2296,  2712,   290,  3428,  3414,  3371,
	   508,   289,  1116,  3154,  2021,  1448,  3162,  3069,  3392,  3391,
	  3442,  2020,  3447,  3379,  3384,  3389,  3373,  3068,  3070,  1447,
	  2813,   873,  1444,  3378,  3383,  3388,  3441,   872,   588,   420,
	   871,   288,   419,  3449,   287,  2096,  1438,  1710,  3067,   418,
	  1494,  2321,   286,   417,  3377,  3382,  3387,  3225,  3225,  3225,
	   285,  2141,   292,  3670,  2337,  3467,  3552,  3421,  3225,  2337,
	  2989,  3484,  3550,  2163,  2163,  1706,  3488,  3336,  1570,  3490,
	  2988,  2573,  1433,  2877,  3499,  3499,  1431,  3216,  3433,  3466,
	  3435,   416,  3437,  3222,  3222,  3222,   861,   284,   415,   283,
	  3500,  3443,  1427,   414,  3222,  2777,  1426,  2777,   857,  3475,
	  3502,  3476,   559,   282,   413,  2777,  3498,  3498,  3465,   727,
	  3493,  3493,  3227,  3227,  3227,  3512,  3271,   590,   590,   590,
	  3470,  3473,  3474,  3227,  3481,  3458,  3278,  3226,  3226,  3226,
	  3460,  3478,  3521,  3501,  2876,  3224,  3224,  3224,  3226,  3459,
	  3461,  3451,  3018,   855,  3457,  1712,  3224,  3480,  3344,  3485,
	  3101,  3514,  2730,  3523,  2837,  3452,   281,  3491,  3111,  3525,
	  1717,  3113,  3115,   853,  3446,   280,  1418,  3344,  3445,  3519,
	  1715,   850,  3018,   411,  2296,   848,  3018,  1127,  3415,  2712,
	  3418,   279,  3522,   278,  2538,   506,  3221,  3221,  3221,  1401,
	   409,   729,   277,   836,   832,   276,  3572,  3221,   407,  3344,
	  3344,  3344,  3344,   728,   274,  3533,  2979,   827,   273,  3506,
	  2141,  3066,   506,  1391,   825,   272,  1860,  3376,  3381,  3386,
	  3539,  2971,  2969,  2975,  1987,  3021,  3180,  2522,  1986,  3509,
	  3489,  2141,  3535,  1984,  3540,  3538,  1389,  1472,  3548,  1388,
	   559,  3543,  2533,  3018,  3018,  3018,   271,  3018,   507,  3577,
	  3557,   269,  3590,  2532,  3560,  2549,  1404,  3570,  3574,  3542,
	  1860,  1860,  3576,  3580,  3567,  2531,  2286,  2534,  3578,  3573,
	  3364,  3549,  1385,  1382,  2159,   403,  3062,  1381,  2530,   270,
	  2102,   268,  3375,  3380,  3385,  1379,  3613,   267,   817,  3581,
	   266,   398,   264,  2987,  3225,  3225,  3011,   262,  3617,   396,
	  3584,  3584,  3584,   261,  3597,  3609,  3585,  3586,  2500,  2418,
	  3467,  2423,  2996,  3467,  3467,  2499,  1958,  2712,  3467,  2488,
	  3332,  2567,  2565,  3603,  2010,  2509,  3402,  1962,  3001,  1960,
	  3222,  3222,  1366,  3643,  3466,  3627,  2512,  3466,  3466,  3629,
	   809,  3213,  3466,  3628,   808,  1096,  3214,  3630,  2157,   395,
	  1957,  3300,  1706,  1365,  3657,   807,   257,  1860,  3638,  3227,
	  3227,  3640,  3641,  3465,  3639,  1096,  3465,  3465,  3635,  3636,
	  1360,  3465,  1116,  1116,  3226,  3226,  3434,  2777,  3292,   256,
	   804,   255,  3224,  3224,   254,   796,   251,  2024,  3652,  3653,
	  3654,  1096,  3440,   879,  1096,  3646,  3011,  1175,   146,  3021,
	  3666,   145,   144,  1475,  3645,   731,   590,  1186,   143,   142,
	  2630,  2461,  2631,  2632,  3655,  3021,  3668,   141,   559,   140,
	   139,   138,   137,   559,   136,   135,   134,  2478,   133,   132,
	  3690,  3678,   131,  3221,  3221,  3694,   130,  3684,  2491,  3679,
	   507,  3216,  3216,  3216,  3687,  3623,  2644,  2649,  3689,   129,
	  1223,  3409,  3216,   128,  2008,  2008,  2008,   127,   126,  2013,
	   125,  3700,  3225,  3704,  3706,  3225,   124,   123,  3484,  3716,
	   122,  3701,  2296,  3499,   121,  3717,  3710,  2321,  3727,   120,
	  3729,  3595,   119,   118,  3598,  3722,  3712,  3600,  3724,  3730,
	  2546,  2560,  3725,   117,   116,  2712,  2711,  2315,  3222,   115,
	  2712,  3222,   114,  1877,  1284,  3498,   113,   112,   111,  3493,
	   110,  2325,   109,   108,  3734,   107,   106,   645,   201,  3344,
	   200,  1465,  2579,  2579,  2579,  2342,   199,  3227,  1234,  3747,
	  3227,  1550,   362,   682,  3416,  3720,   212,  2254,  3721,  1675,
	   516,  3746,  3226,  2706,  1671,  3226,  3318,  2362,  2579,  2955,
	  3224,  2579,  3092,  3224,  3316,  2954,  2579,  3422,  2953,  1950,
	  3467,  3467,  1353,   794,  2378,   793,   250,  1089,   515,  3018,
	  3748,   105,  3780,  3784,   502,   501,  2231,  3775,  2230,  1641,
	  3765,  3766,  3794,  1065,  3466,  3466,   498,   559,  3799,  3791,
	  2141,   497,  2141,  2141,   103,  3661,   198,   197,  1344,  3800,
	   788,  3221,   246,  3225,  3221,  3499,  3807,  2775,  3808,  2475,
	  3809,  3814,  1338,  3465,  3465,   784,  3797,  3797,   245,   244,
	  3806,   242,  1333,   241,  1327,  3815,  2141,  2141,   776,  1330,
	   775,   774,   240,   243,  3592,  2941,  1325,  3498,   239,  3222,
	  3536,  3493,   767,   238,  3827,  3467,  3828,  3601,  1321,  3826,
	  1701,  1701,  3794,  2530,  3830,   765,  3814,  2757,  3815,  3834,
	  2755,  3696,  2761,  3106,  2712,  3821,  2735,  3618,  3227,  3466,
	  3815,  3619,  2751,  2750,  2749,  2748,  3805,  2743,  2464,  1923,
	   931,   237,  1919,  3226,   506,  1317,  3797,   762,  3216,  3631,
	   236,  3224,  2924,  1102,  2452,  1908,  1906,  1310,  3465,  3836,
	  1912,  1103,   590,  1911,  3524,  3303,  3527,  3528,  3330,  3516,
	  2271,  3307,  2932,  2457,  2935,  3410,   760,  1314,  2930,  2456,
	  1916,  1860,  1860,  1860,  3642,   759,  1313,  3302,  2929,  2455,
	  1915,  1312,  1860,   758,  2296,  3647,   382,  3649,  1096,  1096,
	   676,  1309,  3221,  3018,   757,   235,  1902,  2132,  1127,  1127,
	  3423,   510,  1549,  2723,  1127,  2449,  1900,  1307,   748,  3018,
	  1306,   747,  3758,  3759,   234,   511,  3820,   675,  3792,  2267,
	  3407,   513,  2270,  3406,  3405,  3541,  3403,  3102,  1475,  2729,
	  2727,  3589,  3693,   590,  1117,  2261,  2141,  2141,   672,  1115,
	  1114,  3089,  2236,  2287,  1647,  2269,  1646,   673,  2718,   561,
	  3783,  3094,  3691,  1285,  3685,   707,   719,  2724,   563,   715,
	   674,   228,  3592,  1857,  3253,  2407,   677,  1837,  1836,  1835,
	   508,  2725,  3260,  2730,  2268,  3801,  1861,   513,   512,  3587,
	  2134,  1277,  1848,   658,  1275,   678,  2491,  1833,  1272,   227,
	  1831,  1830,  1270,   225,  1892,  1812,  2266,  1261,  1854,  1855,
	  1856,  1859,   978,   221,   220,   218,  3216,  2867,  2381,  3216,
	   219,  2860,  1798,   566,   569,   571,   570,   572,   573,   567,
	   568,  1796,   972,  1228,  1243,  2413,   506,   658,   217,   216,
	   214,  1891,   213,   574,   575,   576,  3482,  1127,  2855,   511,
	  3737,  2265,  1854,  1855,  1856,  1859,  1786,   968,  3479,  2887,
	  1686,  1860,  3229,  2370,  2648,  3744,  1231,   211,   456,   359,
	   358,   357,   209,  2356,  1772,   662,  3749,  1127,  1127,   566,
	   569,   571,   570,   572,   573,   567,   568,  2097,  3009,  1221,
	  1218,   660,   208,  3711,   626,  3651,  3178,   614,  2804,   574,
	   575,   576,  1730,  3174,  2800,  2928,  1729,   177,   226,  2384,
	  1807,  1248,  1803,  1247,  3665,  1228,  2129,   694,   563,  2127,
	  2130,  2128,  2144,   510,  3041,  3042,   222,  2609,  1860,  1860,
	  2132,  1540,  2614,  3592,  3798,  3798,  2613,   511,   512,  2612,
	  2172,  1589,   443,   354,  1531,   353,   505,   566,   569,   571,
	   570,   572,   573,   567,   568,  2008,  2008,  3216,  1538,  1860,
	  1860,   510,   563,   352,  1414,  3688,  1041,   574,   575,   576,
	   351,   350,  1051,   349,   348,   511,  2287,   347,  3697,   346,
	   345,   513,   512,   344,   343,  1412,   342,  2717,  2608,  2080,
	   942,   941,   341,   203,   510,   600,  1806,   176,   165,   745,
	  1557,  1413,   744,   233,  3798,   739,    -1,   986,   511,  1034,
	   985,   614,  1821,  2134,   513,   737,   658,   232,  1073,    -1,
	  1528,   734,   231,  2154,  2992,  2153,  1864,  3047,  2140,  2139,
	  2147,  1854,  1855,  1856,  1859,   566,   569,   571,   570,   572,
	   573,   567,   568,  1490,  1519,  2650,  2137,  2107,  2106,  3150,
	  2810,  3033,  3164,  2645,  3028,   574,   575,   576,  3027,  2643,
	  3025,  2642,  1702,  1703,  2135,  3680,  3171,  2141,  2141,  3569,
	  1408,  2141,  3363,   566,   569,   571,   570,   572,   573,   567,
	   568,  1407,  3045,  2125,  1860,  2141,  3044,  2131,  2120,  2141,
	  2119,  2850,  2851,   574,   575,   576,  1860,  2118,  2141,  1860,
	  2111,  2104,  1059,  2100,  2141,  2141,   566,   569,   571,   570,
	   572,   573,   567,   568,  3147,  2776,  2782,  3665,  3785,  2305,
	  2872,  2873,  3707,  1409,  3438,  1411,   574,   575,   576,  2881,
	  2754,  1410,  2781,  3157,  3156,  3431,  1405,  1406,  1228,  2129,
	   510,   563,  2127,  2130,  2128,  2144,  3750,  2301,  2300,  2299,
	  1860,  2298,  2297,  2769,   511,  2294,  3813,  3810,  1738,  3650,
	   513,   512,  2907,  2908,  3298,  3296,  2444,  1701,  1701,  1701,
	  3276,  2918,  1890,   676,  1888,  3513,  3286,  2912,  1860,  1860,
	  2439,  1886,  1885,  2338,  3781,  3731,  1127,  3515,  3288,  2914,
	  3301,  1721,  2949,  3313,  3726,  3510,  3728,  3829,  3648,  3283,
	   675,  3508,  2901,  1058,  1047,  2432,  1874,  2694,  1720,  1893,
	  1722,   678,  2760,  1724,  1860,  1860,  2248,  1086,  3663,  2141,
	  2141,   672,  2619,  2094,   392,  3532,  1943,   249,   104,  2141,
	   673,  2895,  2141,  2894,  1060,  2428,  1872,  1860,   725,   724,
	  2993,  2991,   723,   674,   229,  3223,  2141,  3220,  3464,   677,
	  3211,  3176,   566,   569,   571,   570,   572,   573,   567,   568,
	   513,  2367,  2366,  1860,  1782,   210,   204,   202,   678,  2347,
	  2840,  2465,   574,   575,   576,  2345,  1208,   639,  1549,  2079,
	   196,  1755,   195,   193,  1860,  1702,  1703,   194,  2085,   192,
	  1716,  1719,  3604,  3425,   733,  1723,  1718,  1860,  1714,  1713,
	  2737,  3353,  2835,  1757,  3354,  1062,  1178,   329,  3357,  3358,
	  3359,   191,   190,   958,  3197,  2819,  2331,  1751,  1061,  1053,
	  1052,  1750,  2753,  3196,  2817,  1063,  2328,  1054,  1055,  1056,
	  1043,  1044,  1749,   630,   188,  1196,  2887,  2173,  1190,   713,
	  2887,   627,  3270,   187,  1239,   215,  1817,  1828,   702,  2879,
	  2141,  3245,   566,   569,   571,   570,   572,   573,   567,   568,
	  2878,  3015,  1820,  2132,  1264,  2141,   701,  3496,  2141,  2874,
	  2141,  1815,   574,   575,   576,   700,   699,  3036,  1700,  1700,
	  2395,  1814,  1262,  2467,   569,   571,   570,   572,   573,   567,
	   568,   698,   224,  1743,  3061,  3624,   510,  3453,  2141,  3191,
	  2141,  2815,  2141,   574,   575,   576,  1185,  2714,   619,  2150,
	   511,  2151,   186,   185,   184,  2008,   183,  2152,  3429,  2811,
	   609,   607,   182,   180,  1733,  2630,  3436,   179,  2156,  2155,
	   181,  1146,  1145,  3439,  1144,  1143,  1142,  3086,  3335,  1140,
	  1139,  1138,  1137,  1136,  3093,  1135,  2134,  1134,  1692,   658,
	  1131,  1073,  2156,  2155,  3099,  3100,   189,   178,  1522,   913,
	  3105,   912,   317,   175,  1854,  1855,  1856,  1859,   173,  2141,
	  2141,  2141,  1097,  2141,  1617,  2097,   172,  2316,  2311,  1164,
	   171,  2141,   170,   580,  2141,   579,   169,   168,  2141,  2141,
	  2141,  2326,  1107,  1105,  3137,  1104,   167,   166,   102,   101,
	   100,  3144,  3145,  2713,    99,    98,  2343,    97,   566,   569,
	   571,   570,   572,   573,   567,   568,    96,    95,    94,  1549,
	    93,    92,    91,    90,    89,  1549,    88,  3173,   574,   575,
	   576,    87,   510,   563,    86,    85,  3186,    84,  1127,  3188,
	    83,    82,    81,    80,  2379,    79,   511,    78,    77,   614,
	    76,    75,   513,    74,    73,  2143,    72,    71,  3203,    70,
	    69,  1127,    68,    67,    66,    65,    64,    62,    61,    60,
	  2625,  1228,  2129,  2103,   563,  2127,  2130,  2128,  2144,    59,
	    58,    57,    56,    55,    54,    53,    52,   511,    50,    49,
	   614,  1738,    47,   513,   512,  3237,    46,   510,  2141,    45,
	    44,    43,  3241,    42,    41,  2141,  2141,    40,    39,    38,
	    37,   511,    36,  2141,    35,    34,    33,    32,    31,    30,
	  3272,  3273,  2431,    29,    28,    27,  3561,    26,    25,    24,
	    23,  3566,    22,   491,  1030,   490,    18,  3285,    16,    15,
	   626,    14,    13,    12,   566,   569,   571,   570,   572,   573,
	   567,   568,    11,    10,  3061,  2141,     9,     8,     6,     5,
	     4,     3,  1060,     1,   574,   575,   576,  1228,     0,   510,
	   563,  2714,  2126,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   511,  1790,   566,   569,   571,   570,   572,
	   573,   567,   568,  1127,  2141,  2141,     0,     0,  2141,     0,
	     0,     0,     0,     0,     0,   574,   575,   576,  3614,   566,
	   569,   571,   570,   572,   573,   567,   568,     0,  1702,  1703,
	     0,     0,  1127,     0,     0,     0,     0,     0,     0,   574,
	   575,   576,  3372,     0,  1617,  1614,  1608,  1606,  1607,  1612,
	  1613,  1609,  1610,  1611,  3345,     0,     0,     0,     0,  1616,
	  1615,  3350,   891,     0,  1227,  3072,  3073,  3074,     0,     0,
	     0,     0,     0,     0,     0,     0,   505,  2713,     0,     0,
	     0,     0,     0,     0,     0,     0,  2625,     0,  3544,     0,
	     0,   566,   569,   571,   570,   572,   573,   567,   568,     0,
	  1721,  3301,     0,  3656,     0,  2141,   510,   563,     0,     0,
	     0,   574,   575,   576,     0,     0,  2141,  1720,  1725,  1722,
	   511,  2141,  1724,     0,     0,     0,   513,     0,     0,  3395,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  3675,  3411,     0,  3676,     0,     0,     0,     0,   560,
	   560,   560,  2616,     0,  3419,   560,   560,     0,     0,     0,
	     0,   560,     0,     0,   560,     0,     0,   560,     0,     0,
	  1127,   560,     0,     0,     0,   560,   560,     0,     0,     0,
	     0,     0,     0,     0,   646,     0,     0,     0,     0,     0,
	     0,     0,  3611,     0,  3705,     0,     0,  3708,  2141,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  3444,     0,     0,     0,     0,     0,     0,  1242,   566,   569,
	   571,   570,   572,   573,   567,   568,     0,   653,     0,     0,
	     0,     0,  1060,     0,     0,   654,     0,  1774,   574,   575,
	   576,  1127,  1127,     0,     0,  1700,  1700,  1700,   510,  3462,
	     0,     0,     0,     0,   658,     0,     0,     0,     0,  3742,
	     0,     0,   511,     0,   648,  3477,     0,     0,   733,     0,
	  2141,     0,     0,     0,     0,     0,  2141,     0,     0,  2141,
	     0,  2141,     0,  2141,     0,     0,     0,  3761,  3503,     0,
	     0,     0,   655,  1773,     0,  2141,     0,     0,     0,   596,
	     0,     0,  1775,     0,   652,  1614,  1608,  1606,  1607,  1612,
	  1613,  1609,  1610,  1611,   648,     0,     0,  1096,     0,  1616,
	  1615,  2141,     0,     0,  2141,  3072,  3073,  3074,   560,   560,
	     0,     0,   560,   560,   560,     0,     0,     0,     0,     0,
	     0,   648,     0,  3802,     0,     0,     0,     0,  1127,   651,
	     0,  1542,     0,  1127,     0,     0,     0,     0,     0,     0,
	   566,   569,   571,   570,   572,   573,   567,   568,  3126,     0,
	     0,     0,     0,     0,  2141,  2081,     0,  2141,   510,     0,
	   574,   575,   576,     0,  3558,     0,  1564,  3559,  1776,     0,
	  2827,     0,   511,     0,     0,  3562,  3564,  3565,     0,     0,
	     0,     0,   511,  2141,     0,     0,     0,     0,     0,   512,
	     0,     0,     0,  2625,  2625,  2625,  1096,  3019,  1549,     0,
	     0,  3019,     0,     0,     0,     0,     0,  2625,     0,     0,
	     0,     0,  2625,  2156,  2155,     0,   510,     0,     0,  2141,
	  2082,     0,     0,     0,  1549,     0,     0,     0,     0,     0,
	   511,     0,  3594,   614,   658,  3596,     0,   512,     0,     0,
	     0,  3602,     0,     0,     0,     0,     0,  2141,     0,  1854,
	  1855,  1856,  1859,  3612,  2888,     0,  3615,  2888,     0,  2890,
	     0,     0,  2890,     0,     0,     0,  2896,     0,  3622,     0,
	   566,   569,   571,   570,   572,   573,   567,   568,     0,     0,
	   566,   569,   571,   570,   572,   573,   567,   568,  3632,     0,
	   574,   575,   576,     0,     0,     0,  3633,  1127,     0,     0,
	   574,   575,   576,  2141,     0,     0,  2083,     0,     0,     0,
	     0,  1699,  1699,  1705,     0,  2141,     0,     0,  1701,  1701,
	   511,     0,     0,     0,  3644,     0,     0,   512,   566,   569,
	   571,   570,   572,   573,   567,   568,     0,  1147,  2625,     0,
	     0,     0,   648,     0,   648,     0,     0,     0,   574,   575,
	   576,     0,     0,     0,     0,     0,  1228,  3130,   510,  3128,
	     0,  3129,     0,  1155,     0,     0,     0,     0,     0,     0,
	     0,     0,   511,     0,     0,     0,     0,     0,     0,   512,
	     0,     0,     0,     0,  3677,     0,     0,     0,     0,  3682,
	     0,     0,     0,   509,     0,     0,     0,     0,     0,  3686,
	  1153,  1154,  1150,  1151,  1152,  1158,  1159,  1156,  1157,  1160,
	  1149,  1148,     0,     0,     0,  3217,  2271,  3698,   566,   569,
	   571,   570,   572,   573,   567,   568,     0,     0,  2624,     0,
	     0,     0,     0,     0,     0,  3709,     0,     0,   574,   575,
	   576,     0,     0,     0,     0,     0,     0,  3004,     0,     0,
	     0,  1241,     0,     0,  3264,     0,     0,     0,     0,     0,
	  3723,     0,     0,     0,     0,  1269,     0,     0,     0,     0,
	   566,   569,   571,   570,   572,   573,   567,   568,     0,     0,
	  3019,     0,     0,     0,     0,     0,     0,  3037,     0,     0,
	   574,   575,   576,     0,  3741,     0,     0,  3743,     0,     0,
	  1549,     0,     0,  1702,  1703,     0,     0,     0,     0,     0,
	  3019,  3755,     0,     0,  3019,     0,     0,  3760,     0,     0,
	     0,     0,     0,     0,     0,     0,   648,     0,  3763,  3764,
	     0,     0,     0,     0,     0,     0,     0,     0,  3770,  3771,
	     0,     0,  3773,     0,  3776,     0,  3778,  3087,   510,   563,
	     0,     0,     0,     0,  3087,     0,     0,     0,     0,     0,
	  3787,     0,   511,  3789,  3087,  3790,     0,     0,     0,   512,
	     0,     0,  2625,     0,     0,     0,     0,     0,     0,  2625,
	     0,  3019,  3019,  3019,  1302,  3019,  2625,     0,   648,     0,
	     0,     0,     0,     0,     0,   649,   596,     0,     0,     0,
	     0,     0,     0,     0,  2625,  2625,  3817,  1547,     0,     0,
	  3818,   648,     0,     0,     0,     0,     0,     0,   648,     0,
	     0,     0,   648,  3823,  3824,     0,   510,   563,     0,     0,
	     0,     0,     0,     0,  2624,     0,     0,     0,  3831,     0,
	   511,     0,     0,     0,     0,   649,  3187,  3835,     0,  3189,
	   626,     0,  1228,     0,   510,   563,     0,  3200,     0,     0,
	   566,   569,   571,   570,   572,   573,   567,   568,   511,     0,
	     0,     0,   649,  3206,     0,     0,     0,     0,     0,   510,
	   574,   575,   576,     0,     0,  1096,  1096,  1096,   649,   649,
	     0,     0,   649,   511,   649,   649,     0,     0,     0,     0,
	   649,   649,   510,     0,     0,  3238,     0,     0,  1549,     0,
	     0,     0,  3242,  2625,     0,   648,   511,     0,     0,     0,
	   560,     0,     0,   512,     0,     0,     0,  2625,     0,     0,
	  3087,  3087,     0,   510,  2625,     0,     0,     0,   566,   569,
	   571,   570,   572,   573,   567,   568,     0,   511,     0,     0,
	     0,     0,     0,     0,   512,  2110,     0,     0,   574,   575,
	   576,     0,     0,     0,     0,     0,   566,   569,   571,   570,
	   572,   573,   567,   568,     0,     0,     0,     0,  3469,  3217,
	  3217,  3217,     0,     0,     0,     0,   574,   575,   576,     0,
	  3217,   566,   569,   571,   570,   572,   573,   567,   568,     0,
	     0,     0,     0,     0,     0,     0,  3495,  3495,     0,     0,
	     0,   574,   575,   576,   566,   569,   571,   570,   572,   573,
	   567,   568,   649,     0,     0,     0,     0,     0,   649,     0,
	   649,     0,     0,     0,   574,   575,   576,     0,     0,  2625,
	     0,     0,     0,     0,     0,   566,   569,   571,   570,   572,
	   573,   567,   568,     0,     0,     0,     0,     0,  2625,   560,
	     0,     0,     0,     0,  2625,   574,   575,   576,  1699,  1699,
	  1699,     0,     0,     0,  2277,  2278,     0,  2625,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   649,     0,   649,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  3019,     0,  2625,
	  2625,  2625,  2625,     0,     0,     0,  2625,  2625,     0,     0,
	  2625,  2625,  2625,     0,     0,     0,   587,   592,     0,  3396,
	     0,     0,     0,   592,  1302,     0,   605,     0,   592,   510,
	   563,   618,     0,     0,     0,   632,   633,     0,     0,     0,
	   636,     0,     0,   511,  3420,     0,     0,     0,     0,     0,
	     0,  2624,  2624,  2624,     0,  3020,     0,   592,     0,  3020,
	     0,     0,   592,   689,   592,  2624,     0,   592,     0,     0,
	  2624,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  2625,     0,     0,     0,     0,     0,     0,  2625,
	   510,     0,  2625,     0,     0,  2403,     0,     0,     0,     0,
	     0,     0,     0,  2409,   511,     0,     0,     0,     0,     0,
	     0,   512,  2372,     0,     0,     0,  3217,     0,     0,     0,
	     0,     0,     0,  3469,     0,     0,  3469,  3469,     0,  1851,
	     0,  3469,     0,     0,     0,     0,     0,     0,     0,  3463,
	     0,   566,   569,   571,   570,   572,   573,   567,   568,   596,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   574,   575,   576,     0,     0,     0,   649,     0,     0,
	     0,  3019,     0,     0,     0,   592,  1700,  1700,  1096,     0,
	     0,     0,     0,     0,     0,     0,     0,  3019,     0,     0,
	   949,     0,     0,   952,     0,     0,  2624,     0,     0,   963,
	   965,     0,   566,   569,   571,   570,   572,   573,   567,   568,
	     0,     0,     0,     0,  2625,     0,     0,     0,     0,  2625,
	     0,  3518,   574,   575,   576,   649,  3520,     0,     0,   649,
	     0,  1720,     0,     0,     0,     0,     0,     0,     0,  3518,
	     0,  3534,     0,     0,     0,     0,   649,   648,     0,     0,
	     0,     0,   649,     0,     0,     0,     0,   649,     0,   649,
	     0,     0,     0,   649,     0,     0,     0,  2625,     0,     0,
	     0,     0,  3713,  3218,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  3217,  3563,   510,  3217,     0,  1028,
	     0,     0,     0,     0,     0,  3495,     0,     0,     0,   649,
	   511,     0,     0,     0,     0,     0,   649,     0,     0,  2625,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  2625,
	     0,     0,     0,  3579,     0,     0,     0,     0,  2625,  2625,
	     0,  3582,     0,     0,   510,     0,     0,     0,  3020,     0,
	     0,     0,  1686,     0,  2611,     0,     0,     0,   511,  1542,
	     0,     0,     0,     0,     0,     0,   649,     0,  2625,     0,
	     0,  2625,   510,     0,     0,     0,     0,  3616,  3020,     0,
	     0,     0,  3020,  3469,  3469,     0,   511,     0,     0,   649,
	     0,     0,     0,   649,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  2625,     0,     0,   566,   569,
	   571,   570,   572,   573,   567,   568,  3634,     0,     0,     0,
	  3637,     0,     0,     0,  2625,   510,     0,     0,   574,   575,
	   576,     0,     0,     0,     0,  3217,     0,  3495,   990,   511,
	  2624,     0,     0,     0,  3087,     0,     0,  2624,     0,  3020,
	  3020,  3020,     0,  3020,  2624,     0,   566,   569,   571,   570,
	   572,   573,   567,   568,     0,  2625,     0,   592,  3469,     0,
	     0,  1181,  2624,  2624,     0,     0,   574,   575,   576,     0,
	     0,     0,     0,     0,   566,   569,   571,   570,   572,   573,
	   567,   568,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,   574,   575,   576,     0,     0,  3683,
	     0,     0,     0,     0,   649,     0,     0,   648,   648,   648,
	     0,   648,     0,   648,     0,     0,     0,     0,   648,  2754,
	     0,     0,     0,     0,     0,   592,     0,   566,   569,   571,
	   570,   572,   573,   567,   568,     0,   560,  2732,     0,     0,
	     0,     0,  2132,     0,     0,     0,     0,   574,   575,   576,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  1686,  1686,     0,     0,     0,     0,     0,
	     0,  2624,     0,     0,   649,   649,   648,     0,     0,     0,
	  1721,     0,     0,     0,  2808,  2624,     0,     0,  2287,     0,
	     0,  3736,  2624,     0,     0,     0,     0,  1720,     0,  1722,
	   678,  2760,  1724,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  3756,     0,   560,     0,  2134,     0,     0,   658,     0,
	     0,     0,     0,     0,     0,  3762,  3468,  3218,  3218,  3218,
	     0,     0,     0,  1854,  1855,  1856,  1859,     0,  3218,  3772,
	     0,     0,  3774,     0,  3777,     0,  3779,     0,     0,     0,
	     0,     0,     0,   749,  3494,  3494,     0,  2882,     0,     0,
	     0,     0,     0,     0,     0,  3396,     0,     0,     0,     0,
	     0,  2402,     0,   803,   805,   806,     0,     0,     0,     0,
	     0,   803,     0,     0,     0,     0,   819,  2624,  2132,     0,
	     0,     0,     0,     0,     0,     0,  2132,     0,     0,  2920,
	     0,  2753,     0,  3816,     0,     0,  2624,     0,     0,     0,
	  3819,   883,  2624,     0,     0,     0,     0,     0,   713,     0,
	     0,     0,     0,     0,     0,  2624,     0,     0,  3825,     0,
	     0,     0,     0,     0,  2287,   614,     0,     0,  3832,     0,
	  1228,  2129,  2287,   563,  2127,  2130,  2128,  2144,   592,     0,
	  1858,     0,     0,     0,     0,  3020,     0,  2624,  2624,  2624,
	  2624,     0,     0,   512,  2624,  2624,     0,     0,  2624,  2624,
	  2624,  3796,     0,     0,   658,     0,     0,   965,     0,  2134,
	     0,     0,   658,     0,     0,     0,     0,     0,   649,  1854,
	  1855,  1856,  1859,     0,  1544,  1545,  1546,  1854,  1855,  1856,
	  1859,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,   994,   996,  1302,   998,     0,  2744,
	  2745,  2746,     0,     0,     0,     0,   998,     0,     0,     0,
	     0,     0,  1009,  1010,  1011,  1012,  1013,  1014,  1015,  1016,
	  2624,     0,  1021,     0,  1022,     0,     0,  2624,     0,     0,
	  2624,     0,     0,     0,     0,     0,   648,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  3218,     0,     0,     0,     0,     0,
	     0,  3468,     0,     0,  3468,  3468,     0,     0,     0,  3468,
	     0,     0,     0,  1029,     0,     0,  1228,  2129,     0,   563,
	  2127,  2130,  2128,  2144,  1228,  2129,     0,   563,  2127,  2130,
	  2128,  2144,  2603,  2604,     0,   614,     0,   648,     0,   512,
	     0,     0,     0,   614,     0,     0,     0,   512,   560,  3020,
	     0,   649,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  3020,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   649,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  2624,     0,     0,     0,     0,  2624,     0,  2652,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  3116,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  1699,
	  1699,     0,  1736,     0,  1739,     0,     0,     0,     0,     0,
	     0,  1181,     0,     0,     0,  2624,     0,     0,     0,     0,
	  3714,     0,     0,     0,     0,     0,   592,     0,     0,     0,
	     0,     0,  3218,     0,     0,  3218,     0,     0,   649,   649,
	   649,     0,   649,  3494,   649,   965,   965,     0,     0,   649,
	     0,     0,     0,     0,     0,     0,     0,  2624,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  2624,     0,     0,
	     0,     0,     0,     0,     0,     0,  2624,  2624,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	   560,   560,     0,     0,     0,  2720,   560,     0,     0,     0,
	  3231,     0,     0,     0,     0,     0,  2624,   649,     0,  2624,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  3468,  3468,     0,  2132,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  3263,     0,     0,     0,     0,
	     0,     0,     0,  2624,     0,     0,  1302,     0,     0,     0,
	  2806,     0,     0,     0,     0,     0,     0,     0,  3291,     0,
	     0,     0,  2624,     0,     0,     0,     0,  1302,     0,     0,
	  2287,     0,     0,  3218,     0,  3494,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,   560,   560,     0,
	   560,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  2624,     0,     0,  3468,  2134,     0,  2863,
	   658,     0,  1073,  2865,     0,  1364,   560,     0,     0,     0,
	     0,     0,     0,     0,     0,  1854,  1855,  1856,  1859,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  1396,     0,     0,  1398,  1399,     0,     0,     0,  3343,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,   560,
	   560,     0,     0,  1423,     0,   803,     0,     0,     0,     0,
	     0,  1436,     0,     0,     0,  1396,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   803,  1455,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  1512,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  1228,  2129,   510,   563,  2127,  2130,  2128,  2144,
	     0,     0,     0,     0,     0,     0,  3408,     0,   511,     0,
	     0,     0,     0,     0,   513,   512,     0,   649,  3417,  2084,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  2087,     0,     0,     0,     0,
	     0,     0,     0,   648,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   649,     0,     0,     0,   649,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  1574,  1575,
	  1576,  1577,  1578,  1579,  1580,  1581,     0,     0,     0,     0,
	  1585,  3450,     0,     0,     0,   649,     0,     0,  1590,     0,
	     0,     0,     0,     0,     0,   649,   566,   569,   571,   570,
	   572,   573,   567,   568,     0,     0,     0,     0,   649,     0,
	     0,     0,     0,     0,  2003,     0,   574,   575,   576,     0,
	     0,     0,     0,   484,   486,   487,     0,     0,     0,  1702,
	  1703,   488,   489,     0,  3040,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,   649,
	     0,     0,     0,     0,     0,     0,   649,     0,  3505,     0,
	     0,   649,     0,   649,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,   649,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   592,     0,     0,     0,     0,     0,   496,     0,   514,
	   648,   518,   519,   520,   521,   522,     0,   523,   524,   525,
	     0,   526,   527,   528,   529,   530,   531,   532,   533,   534,
	   535,   536,   537,   538,   539,   540,   541,   542,   965,   543,
	   544,   545,   546,   547,   548,   549,   550,   551,  3571,     0,
	   552,   553,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	   965,     0,     0,  3175,     0,     0,     0,     0,     0,     0,
	  3184,  2397,     0,     0,     0,     0,  2399,     0,     0,     0,
	     0,   560,     0,     0,     0,     0,     0,     0,     0,     0,
	  2404,     0,     0,     0,  3599,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  2132,  2302,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,   649,     0,     0,
	     0,     0,     0,     0,     0,     0,  2116,     0,  2113,     0,
	     0,     0,  2146,  2138,  2109,     0,  1903,   560,   649,  2306,
	     0,     0,  3258,     0,  3262,     0,   811,  2150,     0,  2151,
	  2115,  2114,     0,     0,     0,  2152,  2148,     0,   674,  2112,
	     0,     0,     0,     0,     0,     0,  2156,  2155,     0,     0,
	     0,     0,     0,     0,   877,     0,     0,     0,     0,  3291,
	     0,     0,     0,     0,  2134,  1954,     0,   658,     0,  1073,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  1854,  1855,  1856,  1859,     0,     0,  1979,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  1996,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  3408,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  3699,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,   648,     0,
	  2132,     0,     0,  2143,     0,     0,  1008,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  1228,
	  2129,  2103,   563,  2127,  2130,  2128,  2144,     0,     0,     0,
	     0,     0,     0,     0,     0,   511,     0,     0,   614,     0,
	     0,   513,   512,     0,     0,     0,  2150,     0,  2151,     0,
	   560,     0,  1512,     0,  2152,     0,     0,  3745,     0,  3397,
	  3400,     0,     0,     0,   649,  2156,  2155,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  2134,     0,  1031,   658,     0,  1073,     0,
	     0,     0,     0,  1064,     0,     0,     0,     0,     0,     0,
	     0,  1854,  1855,  1856,  1859,     0,     0,     0,     0,     0,
	  1088,     0,     0,     0,     0,     0,     0,     0,  3788,     0,
	   649,     0,     0,     0,     0,     0,     0,     0,     0,   965,
	     0,     0,     0,   566,   569,   571,   570,   572,   573,   567,
	   568,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   574,   575,   576,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  1702,  1703,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  2143,     0,     0,     0,     0,  3486,  3487,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  1228,  2129,
	  2103,   563,  2127,  2130,  2128,  2144,     0,     0,     0,     0,
	     0,     0,     0,     0,   511,     0,     0,   614,     0,     0,
	   513,   512,     0,     0,   560,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  2767,     0,
	  2132,   649,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  2116,     0,  2113,     0,     0,
	     0,  2146,  2138,  2109,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  2150,     0,  2151,  2115,
	  2114,     0,     0,     0,  2152,  2148,     0,     0,  2112,     0,
	     0,     0,     0,     0,     0,  2156,  2155,     0,     0,     0,
	     0,     0,   566,   569,   571,   570,   572,   573,   567,   568,
	  2132,  2302,  2861,  2134,     0,     0,   658,     0,  1073,     0,
	   965,     0,   574,   575,   576,     0,     0,     0,     0,     0,
	     0,  1854,  1855,  1856,  1859,  2116,     0,  2113,     0,     0,
	   648,  2146,  2138,  2109,     0,     0,     0,     0,  2306,     0,
	     0,     0,   560,   648,     0,     0,  2150,     0,  2151,  2115,
	  2114,     0,     0,     0,  2152,  2148,     0,     0,  2112,     0,
	     0,     0,     0,     0,  3610,  2156,  2155,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  2134,     0,     0,   658,     0,  1073,  1241,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  1854,  1855,  1856,  1859,     0,     0,     0,     0,     0,
	     0,     0,  2143,     0,     0,     0,  1858,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  1228,  2129,
	  2103,   563,  2127,  2130,  2128,  2144,     0,     0,     0,     0,
	     0,     0,     0,     0,   511,  1452,     0,   614,     0,     0,
	   513,   512,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  2515,     0,     0,     0,     0,     0,  2518,     0,     0,
	     0,     0,  2143,     0,     0,     0,     0,     0,     0,  2525,
	     0,     0,  1396,     0,  2536,     0,     0,     0,  1228,  2129,
	  2103,   563,  2127,  2130,  2128,  2144,     0,     0,     0,   649,
	   560,     0,     0,     0,   511,   560,  1436,   614,     0,     0,
	   513,   512,     0,     0,     0,     0,     0,     0,  3005,  3006,
	  3007,     0,   566,   569,   571,   570,   572,   573,   567,   568,
	     0,     0,  2584,     0,     0,     0,     0,     0,     0,     0,
	  1571,     0,   574,   575,   576,   649,     0,     0,     0,     0,
	     0,     0,     0,  1512,     0,  1702,  1703,  2093,     0,     0,
	     0,     0,  1582,  1583,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,   566,   569,   571,   570,   572,   573,   567,   568,
	  1640,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,   574,   575,   576,     0,     0,     0,     0,     0,
	     0,     0,  1669,     0,  1673,  1702,  1703,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,   560,
	     0,  2132,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  2116,     0,  2113,     0,
	     0,     0,  2146,  2138,  2109,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  2150,     0,  2151,
	  2115,  2114,     0,     0,     0,  2152,  2148,     0,     0,  2112,
	     0,     0,     0,     0,     0,     0,  2156,  2155,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  2134,     0,     0,   658,     0,  1073,
	  1858,  1858,  1858,     0,     0,     0,     0,     0,     0,     0,
	     0,  1858,  1854,  1855,  1856,  1859,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   649,     0,  2143,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,   649,     0,     0,     0,     0,  1228,
	  2129,  2103,   563,  2127,  2130,  2128,  2144,     0,     0,     0,
	     0,     0,     0,     0,     0,   511,     0,     0,   614,     0,
	     0,   513,   512,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  1969,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  1903,     0,
	  2922,     0,     0,     0,     0,     0,     0,     0,    17,   164,
	   319,   454,   361,   364,   367,   369,   370,   374,   375,   376,
	     0,   318,   223,   372,     0,     0,     0,     0,     0,     0,
	     0,     0,   356,     0,     0,     0,     0,     0,     0,     0,
	     0,   248,     0,     0,   335,   363,     0,   334,   205,     0,
	     0,  2707,     0,   566,   569,   571,   570,   572,   573,   567,
	   568,   323,   333,   441,     0,     0,     0,  1858,  1858,     0,
	     0,   360,     0,   574,   575,   576,     0,     0,     0,     0,
	     0,     0,     0,     0,   458,     0,  1702,  1703,  3297,     0,
	   378,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,   381,   230,
	   457,     0,     0,     0,   332,   383,     0,   379,     0,     0,
	   380,   365,     0,     0,   391,   386,   387,   389,   390,     0,
	     0,   206,     0,  3456,   263,   397,   393,   460,   459,     0,
	     0,     0,   463,   461,   265,   275,   404,   410,   253,   400,
	   406,   465,   412,   408,   259,   258,   252,   247,   260,   464,
	   462,     0,   405,   399,   402,   401,   466,   467,   470,   468,
	   472,   471,   469,   475,   483,   473,   421,   424,     0,  3497,
	  3497,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  1858,     0,     0,  1858,     0,
	     0,     0,     0,     0,    21,   162,    19,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,   433,
	   438,   477,   476,   431,   435,   432,   427,   430,   309,   310,
	   308,   313,   314,   316,   312,   163,     0,  2251,   174,   440,
	   322,   325,   320,   321,   327,   373,   324,   326,   330,   394,
	   336,   340,   444,   442,   445,   446,   452,   448,   451,   449,
	   355,   207,   478,   429,   384,   385,   479,   437,   439,   426,
	   480,   481,   482,   455,   339,   337,   338,   447,   366,   371,
	   368,     0,   388,     0,   328,   453,   311,   434,   331,   315,
	   450,     0,     0,     0,     0,   377,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  1858,     0,     0,     0,
	     0,    17,   164,   319,   454,   361,   364,   367,   369,   370,
	   374,   375,   376,     0,   318,   223,   372,     0,     0,     0,
	     0,     0,     0,     0,     0,   356,     0,     0,     0,     0,
	     0,     0,     0,     0,   248,     0,     0,   335,   363,     0,
	   334,   205,     0,     0,  2249,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,   323,   333,   441,     0,     0,     0,
	     0,     0,     0,     0,   360,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,   458,     0,     0,
	     0,     0,     0,   378,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   381,   230,   457,     0,  3324,     0,   332,   383,     0,
	   379,     0,     0,   380,   365,     0,     0,   391,   386,   387,
	   389,   390,     0,     0,   206,     0,     0,   263,   397,   393,
	   460,   459,     0,     0,     0,   463,   461,   265,   275,   404,
	   410,   253,   400,   406,   465,   412,   408,   259,   258,   252,
	   247,   260,   464,   462,     0,   405,   399,   402,   401,   466,
	   467,   470,   468,   472,   471,   469,   475,   483,   473,   421,
	   424,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  3497,     0,
	     0,     0,     0,     0,     0,     0,     0,    21,   162,    19,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  2576,   433,   438,   477,   476,   431,   435,   432,   427,
	   430,   309,   310,   308,   313,   314,   316,   312,   163,     0,
	     0,   174,   440,   322,   325,   320,   321,   327,   373,   324,
	   326,   330,   394,   336,   340,   444,   442,   445,   446,   452,
	   448,   451,   449,   355,   207,   478,   429,   384,   385,   479,
	   437,   439,   426,   480,   481,   482,   455,   339,   337,   338,
	   447,   366,   371,   368,     0,   388,     0,   328,   453,   311,
	   434,   331,   315,   450,     0,     0,     0,     0,   377,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,    17,   164,   319,   454,   361,   364,   367,   369,   370,
	   374,   375,   376,     0,   318,   223,   372,     0,     0,     0,
	  3497,     0,     0,     0,     0,   356,     0,     0,     0,     0,
	     0,     0,     0,     0,   248,     0,     0,   335,   363,     0,
	   334,   205,     0,     0,  2026,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,   323,   333,   441,     0,     0,     0,
	     0,     0,     0,     0,   360,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,   458,     0,     0,
	     0,     0,     0,   378,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  2690,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   381,   230,   457,  2705,     0,  3529,   332,   383,     0,
	   379,     0,     0,   380,   365,     0,     0,   391,   386,   387,
	   389,   390,     0,     0,   206,     0,     0,   263,   397,   393,
	   460,   459,     0,     0,     0,   463,   461,   265,   275,   404,
	   410,   253,   400,   406,   465,   412,   408,   259,   258,   252,
	   247,   260,   464,   462,     0,   405,   399,   402,   401,   466,
	   467,   470,   468,   472,   471,   469,   475,   483,   473,   421,
	   424,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,    21,   162,    19,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,   433,   438,   477,   476,   431,   435,   432,   427,
	   430,   309,   310,   308,   313,   314,   316,   312,   163,     0,
	     0,   174,   440,   322,   325,   320,   321,   327,   373,   324,
	   326,   330,   394,   336,   340,   444,   442,   445,   446,   452,
	   448,   451,   449,   355,   207,   478,   429,   384,   385,   479,
	   437,   439,   426,   480,   481,   482,   455,   339,   337,   338,
	   447,   366,   371,   368,     0,   388,     0,   328,   453,   311,
	   434,   331,   315,   450,     0,     0,     0,     0,   377,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  3659,  3660,     0,     0,
	     0,     0,     0,    17,   164,   319,   454,   361,   364,   367,
	   369,   370,   374,   375,   376,     0,   318,   223,   372,     0,
	     0,     0,     0,     0,     0,     0,     0,   356,     0,     0,
	     0,     0,     0,     0,     0,     0,   248,     0,     0,   335,
	   363,     0,   334,   205,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,   323,   333,   441,     0,
	     0,     0,     0,     0,     0,     0,   360,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,   458,
	     0,     0,     0,     0,     0,   378,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   381,   230,   457,     0,     0,     0,   332,
	   383,     0,   379,     0,     0,   380,   365,     0,     0,   391,
	   386,   387,   389,   390,     0,     0,   206,     0,     0,   263,
	   397,   393,   460,   459,     0,     0,     0,   463,   461,   265,
	   275,   404,   410,   253,   400,   406,   465,   412,   408,   259,
	   258,   252,   247,   260,   464,   462,  3049,   405,   399,   402,
	   401,   466,   467,   470,   468,   472,   471,   469,   475,   483,
	   473,   421,   424,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  3085,     0,     0,     0,     0,     0,     0,    21,
	   162,    19,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,   433,   438,   477,   476,   431,   435,
	   432,   427,   430,   309,   310,   308,   313,   314,   316,   312,
	   163,     0,     0,   174,   440,   322,   325,   320,   321,   327,
	   373,   324,   326,   330,   394,   336,   340,   444,   442,   445,
	   446,   452,   448,   451,   449,   355,   207,   478,   429,   384,
	   385,   479,   437,   439,   426,   480,   481,   482,   455,   339,
	   337,   338,   447,   366,   371,   368,  2132,   388,     0,   328,
	   453,   311,   434,   331,   315,   450,     0,     0,     0,     0,
	   377,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  2116,     0,  2113,     0,     0,     0,  2146,  2138,  2109,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  2150,     0,  2151,  2115,  2114,     0,     0,     0,
	  2152,  2148,     0,   674,  2112,     0,  2132,     0,     0,     0,
	     0,  2156,  2155,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  2134,
	     0,  2116,   658,  2113,  1073,     0,     0,  2146,  2138,  2109,
	     0,     0,     0,     0,     0,     0,     0,  1854,  1855,  1856,
	  1859,     0,  2150,     0,  2151,  2115,  2114,     0,     0,     0,
	  2152,  2148,     0,     0,  2112,     0,     0,     0,     0,     0,
	     0,  2156,  2155,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  2134,
	     0,     0,   658,     0,  1073,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  1854,  1855,  1856,
	  1859,     0,     0,     0,     0,  3322,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  2143,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  1228,  2129,  2103,   563,  2127,  2130,
	  2128,  2144,     0,     0,     0,     0,     0,     0,     0,     0,
	   511,     0,     0,   614,     0,     0,   513,   512,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,   626,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  2143,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  1228,  2129,  2103,   563,  2127,  2130,
	  2128,  2144,     0,     0,     0,     0,     0,     0,     0,     0,
	   511,     0,     0,   614,     0,     0,   513,   512,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,   566,   569,
	   571,   570,   572,   573,   567,   568,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,   574,   575,
	   576,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  1702,  1703,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  2132,     0,     0,     0,     0,     0,     0,     0,   566,   569,
	   571,   570,   572,   573,   567,   568,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  2116,     0,  2113,   574,   575,
	   576,  2146,  2138,  2109,     0,     0,     0,     0,     0,     0,
	     0,  1702,  1703,     0,     0,     0,  2150,     0,  2151,  2115,
	  2114,     0,     0,     0,  2152,  2148,     0,     0,  2112,     0,
	  2132,     0,     0,     0,     0,  2156,  2155,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  2134,     0,  2116,   658,  2113,  1073,     0,
	     0,  2146,  2138,  2109,     0,     0,     0,     0,     0,     0,
	     0,  1854,  1855,  1856,  1859,     0,  2150,     0,  2151,  2115,
	  2114,     0,     0,     0,  2152,  2148,     0,     0,  2112,     0,
	     0,     0,     0,     0,     0,  2156,  2155,     0,  2330,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  2134,     0,     0,   658,     0,  1073,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  1854,  1855,  1856,  1859,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  2143,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  1228,  2129,
	  2651,   563,  2127,  2130,  2128,  2144,     0,     0,     0,     0,
	     0,     0,     0,     0,   511,     0,     0,   614,  1738,     0,
	   513,   512,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  2143,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  1228,  2129,
	  2103,   563,  2127,  2130,  2128,  2144,     0,     0,     0,     0,
	     0,     0,     0,     0,   511,     0,     0,   614,     0,     0,
	   513,   512,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,   566,   569,   571,   570,   572,   573,   567,   568,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,   574,   575,   576,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  1702,  1703,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  2132,     0,     0,     0,     0,     0,
	     0,     0,   566,   569,   571,   570,   572,   573,   567,   568,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  2116,
	     0,  2113,   574,   575,   576,  2146,  2138,  2109,     0,     0,
	     0,     0,     0,     0,     0,  1702,  1703,  2132,     0,     0,
	  2150,     0,  2151,  2115,  2114,     0,     0,     0,  2152,  2148,
	     0,     0,  2112,     0,     0,     0,     0,     0,     0,  2156,
	  2155,  2132,     0,     0,     0,     0,     0,     0,  2146,  2138,
	     0,     0,     0,     0,     0,     0,     0,  2134,     0,     0,
	   658,     0,  1073,  2150,     0,  2151,     0,     0,     0,     0,
	     0,  2152,  2148,     0,     0,  1854,  1855,  1856,  1859,     0,
	     0,     0,  2156,  2155,     0,     0,     0,  2150,     0,  2151,
	     0,     0,     0,     0,     0,  2152,     0,     0,     0,     0,
	  2134,     0,     0,   658,     0,  1073,  2156,  2155,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  1854,  1855,
	  1856,  1859,     0,     0,  2134,     0,     0,   658,     0,  1073,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  1854,  1855,  1856,  1859,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  2143,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  1228,  2129,  2103,   563,  2127,  2130,  2128,  2144,
	     0,     0,     0,     0,     0,     0,     0,     0,   511,     0,
	     0,   614,     0,     0,   513,   512,     0,     0,     0,  2143,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  1228,  2129,  2103,   563,  2127,
	  2130,  2128,  2144,  2143,     0,     0,     0,     0,     0,     0,
	     0,   511,     0,     0,   614,     0,     0,   513,   512,  1228,
	  2129,  2103,   563,  2127,  2130,  2128,  2144,     0,     0,     0,
	     0,     0,     0,     0,     0,   511,     0,     0,   614,     0,
	     0,   513,   512,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,   566,   569,   571,   570,
	   572,   573,   567,   568,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,   574,   575,   576,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  1702,
	  1703,     0,     0,     0,     0,     0,     0,     0,     0,   566,
	   569,   571,   570,   572,   573,   567,   568,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,   574,
	   575,   576,     0,   566,   569,   571,   570,   572,   573,   567,
	   568,     0,  1702,  1703,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   574,   575,   576,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  1702,  1703
};

short yypact[] = {
	 -1000,  9737, -1000, -1000, -1000, -1000, -1000, -1000,    42,    42,
	    42, -1000, -1000, -1000, -1000, -1000,    42,    42,   216, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,    42,   752,    42,  2001,    42,    42,    42,    42,
	    42, -1000,    42,    42,    42, -1000,    42,    42,    42,    42,
	    42,    42,    42,    42,    42,    42,    42,    42,    42,    42,
	    42,    42,    42, -1000,    42,    42,    42,    42,    42,    42,
	    42,    42,    42, -1000, -1000,    42,    42, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,  2022, -1000,  1819,  5574,  5574,  5574,  1501,
	  5635,  5368,  5574,  5574, -1000, -1000,  1660,  5368,  5574,  5960,
	  5635,  5574,  4851,  1842,  5574,  5635,  2206,  2147,  5574,  5635,
	  5635, -1000,  5574,  5574,  5635,  5831,  1920,   605,   605,  1779,
	 -1000,   413,  4793, -1000,  4793, -1000, -1000, -1000,  3648, -1000,
	  1899,  5368,  1918,  1658,  5960,  1334,  5368,  5635,  5368,  5960,
	  5960,  5368,  2531,  1457,  1479,  5960,  1267,  1701,   215,  5368,
	  1194,  2204,  2052,  2529,  1079,  2428,  1894,  1368,  1249,  1193,
	  1798,  1179,  1193,  1187,  1187,   755,  1574, -1000,  2035,  1890,
	   605,   718, -1000,  1735,  1028,  1079,  1079,  1979, -1000, -1000,
	 -1000,    42,  1079, -1000,  1220, -1000,  1184,  1079,  1179,  1179,
	  1161,   494,  1176,  1655,  1212, -1000,  1594,  1161,   445,  1975,
	  1732,  1154,  1722,  1220,  1152,  1214,  1212,  1212,  1739,    42,
	  2465,  1150,  1079,  1146,    87,    87,  1211,  1648,    87,  1145,
	    87,    87,  1144,  1140,  1195,  1645,    87,    87, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,  1639,  1866, -1000,  1543,
	 -1000, -1000, -1000,  2413, -1000, -1000, -1000, -1000, -1000,  5368,
	 -1000, -1000,   664, -1000,  2530,  2294, -1000, -1000, -1000,  2495,
	 -1000,  2418,  5960, -1000,  5635,  5574,  5574,  5635,  5960,  5574,
	  5574,  5574,   306,  5635,  5635, -1000, -1000,  5960, -1000,  5960,
	 -1000,  2287,   605,  1834, -1000, -1000, -1000,  2260, -1000, -1000,
	  2223, -1000, -1000,   541, -1000, -1000, -1000, -1000, -1000, -1000,
	  1194, -1000,  1202, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,  2513, -1000, -1000,  1013,  1079, -1000,  1079, -1000,
	 -1000, -1000, -1000,  1578, -1000, -1000, -1000,  1079, -1000,  1578,
	 -1000,    42, -1000,  1079,  1079,  1079,  1079,  1079,  1079,  1079,
	  1079, -1000,  1138,  1079, -1000,  1079, -1000, -1000,    87, -1000,
	 -1000, -1000, -1000, -1000,    87, -1000,    87, -1000, -1000, -1000,
	 -1000,  2116, -1000,  5635, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,  1079, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	    42, -1000, -1000, -1000, -1000,  3920, -1000, -1000,    42,   703,
	  2523,   484,   337, -1000,   436, -1000, -1000, -1000, -1000,   -83,
	 -1000, -1000,  1636, -1000,  2445,   176,  1135,  1991, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,   164,  1267, -1000,  1355, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1312,  1779, -1000,
	  1779, -1000, -1000, -1000, -1000,  1046, -1000,  5156,   346,   346,
	 -1000,  2431, -1000, -1000,   346,   346, -1000,  1937, -1000,  1937,
	 -1000, -1000,   604,   969,  1844, -1000,   969, -1000, -1000,   927,
	  1842,  5368, -1000,   301, -1000,  5635, -1000,  1779,   604,  1279,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,  2471,  5960, -1000,
	   306, -1000,  5156,  1749,  1779,  1779,   604,   604, -1000,   587,
	 -1000,  2062,  2062, -1000, -1000,  1779, -1000, -1000,   -83, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	   297,  1635,   927, -1000, -1000,   604, -1000, -1000, -1000,  2534,
	  1674,  2534, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  5368,
	 -1000,  1869, -1000,   603,  -107,   927, -1000, -1000,   413,   604,
	 -1000,  1842,  1819, -1000,  3648,  1634,  1833, -1000,  1766, -1000,
	 -1000,  2435,  1269, -1000, -1000,  1805, -1000, -1000, -1000, -1000,
	 -1000,  5831,   967, -1000, -1000, -1000,   927, -1000, -1000,  2165,
	 -1000, -1000, -1000, -1000, -1000, -1000,  1907,   501,   346,  2426,
	 -1000, -1000, -1000, -1000, -1000,  1763, -1000, -1000, -1000, -1000,
	 -1000,   484,  2487, -1000, -1000,   484,  2485,  1134, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1793,  2162, -1000,
	   494, -1000,   312, -1000,  1116,  1632,  1380, -1000,   445, -1000,
	 -1000, -1000,  1789,   312, -1000,   445, -1000,  1630, -1000,    57,
	 -1000, -1000,  1481, -1000, -1000, -1000, -1000, -1000,  1532, -1000,
	  1628, -1000,  1907, -1000,  1093, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,  1274, -1000,   859, -1000,  1079,  1975, -1000,  1088,
	 -1000,  2465,  1274, -1000, -1000,   312,  2481, -1000,  1569,  1271,
	 -1000, -1000,    27, -1000,  1781, -1000,  1626,  1083, -1000, -1000,
	 -1000,   983, -1000, -1000,  1079,  1079, -1000, -1000,  3861, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,  1705,  1698,  1975, -1000,
	  1535, -1000, -1000, -1000,  1079, -1000,  1069,  1901,   965, -1000,
	 -1000, -1000,   768,  1788,  1788, -1000,   412,  1788,  1788, -1000,
	 -1000, -1000,   208,  1710, -1000, -1000, -1000,  2465, -1000, -1000,
	    42, -1000,   452,  1079,  1888,   160, -1000, -1000, -1000, -1000,
	 -1000,   -83,  1880, -1000, -1000,   312,  2481,  1080,    26,   312,
	  1064,  1054, -1000,  1505,  1505, -1000, -1000,  1079,  1060,   438,
	  1054, -1000,  5368,  5635, -1000, -1000, -1000, -1000, -1000,  2404,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   604,  2171, -1000,
	 -1000,   432, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,  5368,  1842, -1000, -1000, -1000, -1000, -1000,   927, -1000,
	   927,   927, -1000, -1000, -1000, -1000,  2224,  4394,  5635,  5635,
	  5337,  2282, -1000, -1000,   927, -1000,   927,   927, -1000, -1000,
	  2062,  2585, -1000,  2392, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,   416,  1194,  1917,  1756, -1000,  1079,
	 -1000,  2115,  1674, -1000,   717,    42, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1079,
	  1079,  1079,  1079,  1079,  1079,  1079,  1079,    42,    42, -1000,
	   379,  1079, -1000,    26, -1000,    26, -1000, -1000, -1000,  1079,
	 -1000, -1000, -1000, -1000,  2059,   -71,  1071, -1000, -1000, -1000,
	 -1000,   152, -1000, -1000, -1000,  1050,    81, -1000,  2271, -1000,
	   116, -1000, -1000,    97, -1000, -1000, -1000,  1621, -1000,  1372,
	  2020, -1000,   -69, -1000, -1000,    42, -1000, -1000,  2480,  1268,
	 -1000,   496,   496,  1043, -1000,  1620,   -38, -1000,  1334, -1000,
	 -1000,  1615,    18,  1033, -1000, -1000, -1000,    42,  2466,    42,
	   312,   312, -1000, -1000, -1000, -1000,  5574,  1779, -1000, -1000,
	  1612,  1612, -1000, -1000, -1000,   604,  1933,  1611, -1000, -1000,
	 -1000, -1000,   927, -1000, -1000,   -15, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  -192, -1000,
	 -1000,  5156, -1000,  1230,  1779,  1779,  1912,    69,    69,  1674,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,  5960, -1000,  5960, -1000,  4663,  5907,  5960, -1000, -1000,
	  5960, -1000,  5960, -1000, -1000,   604,  5635,   450,  5635, -1000,
	  1844,  5960, -1000,  1779,  5960,  5635,  2439, -1000, -1000, -1000,
	  2471, -1000, -1000, -1000, -1000,  2259, -1000, -1000,  1740, -1000,
	  5368, -1000, -1000, -1000,  2426,   196,  5960,  2426,  1310, -1000,
	   396,   383,  2522,   337,  1911, -1000, -1000, -1000,  1819,  4803,
	  5635, -1000, -1000,  4121,  5960,   604, -1000, -1000, -1000,   604,
	   981,  1819, -1000,  5960, -1000, -1000, -1000,   309,  5960,  1779,
	  1779, -1000, -1000,  2426,  5960,  5960,  5960,   297,   927,  1609,
	 -1000, -1000, -1000, -1000, -1000,   604, -1000, -1000,   604,  1754,
	 -1000,  1267,  2498,   192,  1059, -1000,   604, -1000, -1000,  2470,
	  1606, -1000,  1536, -1000,  2411, -1000,  3686,  1777,   927,  1604,
	  1604, -1000, -1000, -1000,   215, -1000,  2079,  2140, -1000,  2589,
	  4851, -1000,  5960,  5960, -1000,  5960,  5960,  3719,  1203, -1000,
	   496,   496,   -83,   337,  1268,   337, -1000,   986, -1000, -1000,
	   646, -1000,  2043, -1000,   445, -1000, -1000,  1787, -1000,  1292,
	 -1000,  1264, -1000, -1000, -1000, -1000,  1599,  1129, -1000,   496,
	 -1000, -1000,  -108,  1128, -1000, -1000, -1000, -1000,  1118, -1000,
	  -109, -1000, -1000, -1000, -1000, -1000,  1825,   630,   630, -1000,
	 -1000,  2225,  1807, -1000,   403,  2474,  1079,  1856, -1000, -1000,
	 -1000, -1000, -1000, -1000,   859, -1000,  1975, -1000,  1757,    42,
	 -1000,  1102, -1000,    11,   496,   337, -1000,  1262,    11,  1079,
	 -1000,   810,  1261, -1000, -1000,  3861,   496, -1000, -1000,  1359,
	 -1000,    11, -1000,   494,  1099, -1000,    11,  1008, -1000,  1079,
	   803,  1257, -1000,    11,  1007, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   246, -1000,
	   670, -1000,   504, -1000,  1090, -1000, -1000,   879, -1000, -1000,
	 -1000,   789, -1000,  1245, -1000, -1000,    11, -1000,  1003, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1867,   965, -1000,
	 -1000,  9375, -1000,  1072, -1000, -1000, -1000,   312, -1000,  1068,
	 -1000,  -110,  -111,  -112, -1000,   496,   956, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,  1421,  1404, -1000, -1000,
	 -1000, -1000,   337,  1027, -1000,    11,   337,   312,   952, -1000,
	 -1000, -1000, -1000, -1000, -1000,  1019, -1000,    11,  1542, -1000,
	  1598,   958, -1000,    11, -1000, -1000,   926, -1000,    11,   951,
	   917, -1000,    11,  1079,   941, -1000, -1000, -1000, -1000, -1000,
	 -1000,  1844,  1844,   346,   346, -1000,  1779,  2266, -1000,  1362,
	   969,   927, -1000,  4941, -1000, -1000, -1000, -1000, -1000, -1000,
	   927, -1000, -1000, -1000, -1000, -1000, -1000,  1596, -1000,  5635,
	 -1000,  2522,  1784, -1000,  1334, -1000, -1000, -1000,  1593, -1000,
	  7895, -1000, -1000, -1000, -1000,   921, -1000, -1000, -1000, -1000,
	 -1000, -1000,   992,   992,   445,   445,   445,   717,   445, -1000,
	   445, -1000, -1000, -1000, -1000,   445, -1000, -1000, -1000,   927,
	 -1000,  2019,  1071,   117, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,    76,    72, -1000,   147,   -44, -1000, -1000,
	 -1000,   276, -1000, -1000, -1000, -1000, -1000, -1000, -1000,    41,
	 -1000,   148,   748, -1000, -1000,   887,   316, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,   363, -1000, -1000,   140, -1000,
	 -1000,  2053, -1000,   484, -1000, -1000,   344, -1000, -1000, -1000,
	 -1000,  1585, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,   202,   202, -1000, -1000, -1000, -1000, -1000,  9035, -1000,
	 -1000, -1000,    42,  2466, -1000,   911, -1000,   496, -1000, -1000,
	   604, -1000,   604,  5574, -1000, -1000,  1046,  3605, -1000, -1000,
	   604, -1000, -1000,   604,    69,    69,    69, -1000, -1000, -1000,
	  1674,  1674, -1000, -1000, -1000, -1000,   346,   346, -1000,  1783,
	  1483, -1000,  2230, -1000, -1000, -1000,  5960,   604,  2134,  7975,
	 -1000, -1000,  2120,   604, -1000,  2111,  2234,  2224, -1000,   602,
	 -1000, -1000,  -192,  1779,   930,  5368,   604, -1000, -1000, -1000,
	  7975, -1000, -1000,   602, -1000, -1000,   604, -1000, -1000,  2181,
	 -1000,  1539, -1000, -1000,   746,  1779,  2159,   450,   927,  1483,
	   746,  2067, -1000,  1334,  1334, -1000,  1582, -1000,   337,  2475,
	   940,   927,  5635,  1580, -1000, -1000, -1000,   927, -1000,   602,
	  5960,  5960, -1000,  2212,  2249,  5879,   346, -1000,  1244, -1000,
	 -1000, -1000, -1000, -1000,  1907,  1576,   602, -1000,  1844,  1483,
	 -1000,   604, -1000,  1907,  1377,  5960,  4121, -1000, -1000,  5960,
	  5960, -1000,  1267, -1000,   604,  5635,  2030, -1000, -1000, -1000,
	  5635,  2435, -1000, -1000, -1000, -1000, -1000,  5960,  1269, -1000,
	 -1000,  1674, -1000, -1000,  5635,  1777, -1000, -1000, -1000,  3560,
	 -1000, -1000,  1446, -1000,  1230,  1230, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   -83, -1000,
	 -1000, -1000,  1230,  1230,   662,   604, -1000,   604, -1000, -1000,
	 -1000, -1000,  5960, -1000,   927,  2328, -1000,  1009, -1000, -1000,
	  2426,  2426, -1000, -1000, -1000, -1000,  2360, -1000,  1907,   746,
	  2513, -1000,  2309,   484, -1000, -1000, -1000, -1000, -1000, -1000,
	   907, -1000,  -113, -1000, -1000, -1000, -1000, -1000,  1243, -1000,
	 -1000,  -121, -1000, -1000, -1000,  2008, -1000,  1930, -1000, -1000,
	   458,   895, -1000,  3996, -1000, -1000,  1302,  -125, -1000,   312,
	  -128, -1000, -1000,    57, -1000,  1481,    51,   630, -1000,  1575,
	 -1000, -1000, -1000,  2315,   746,  2119,  2411,  7975, -1000,  -145,
	   894, -1000,   337, -1000, -1000,   337,  1851, -1000,  1977, -1000,
	   210, -1000,  1726,   670,   670,   670,  1525, -1000,   504,  2465,
	 -1000,   312,   844, -1000, -1000, -1000, -1000,  1569,  1079, -1000,
	 -1000,   312, -1000,    11,  1229, -1000,   880,  -130, -1000, -1000,
	  1079,  1219, -1000,   983,   483,  1079, -1000, -1000,  3861,  1441,
	  1317,  2465,  1535, -1000,   832, -1000, -1000, -1000, -1000, -1000,
	   832,  1471, -1000, -1000, -1000, -1000, -1000,  1079, -1000,  3861,
	 -1000,   879, -1000, -1000, -1000, -1000,    42, -1000, -1000, -1000,
	   185,   507,   507,   507, -1000,   312, -1000, -1000, -1000, -1000,
	 -1000, -1000,   312,  1079, -1000,   782, -1000,   312, -1000,   312,
	  1505,   867, -1000,    11,    11, -1000,  1505,    26, -1000,  1505,
	    26, -1000,    87, -1000,  1079,    26,   778, -1000,  1568,  1269,
	  1269,  5960,  5960, -1000, -1000, -1000, -1000, -1000, -1000,  5831,
	  1697, -1000, -1000, -1000, -1000,  5289, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,   927, -1000,   -24,   141, -1000, 10779,
	 -1000, 10779, 10779, -1000, -1000, -1000,   222,   604,  2118,    62,
	 -1000, -1000, -1000, -1000, -1000, -1000,   875, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, 10465, 10405,   604,   604, -1000,
	 -1000,   496,   496,   865, -1000,  1307, -1000,   604, -1000,  1749,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1781, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,  5635,   128,   117,  1217, -1000, -1000,    30,
	  1567,  -168, -1000, -1000,  1217, -1000,  1217,  1217, -1000,  1217,
	  1217, -1000,   -22, -1000, -1000, -1000,   -28,   -28, -1000,   -28,
	 -1000, -1000, -1000, -1000, -1000, -1000,   -55, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,  1397, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,  1566,  1292, -1000, -1000,  2017,  1293, -1000,
	 -1000,    42,  2474,   484, -1000, -1000, -1000,   -83,   860,    17,
	   202, -1000,  1560, -1000, -1000,   202,   -10,    43, -1000,    42,
	  8662, -1000, -1000, -1000,   312, -1000, -1000,  4257,  4257,   602,
	 -1000, -1000,  3492,  4257, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,   815,  1674, -1000, -1000, -1000, -1000, -1000,  5960,
	  5960, -1000, -1000, -1000,  2587,  6273,  1230, -1000, -1000, -1000,
	  5960, -1000,  5635,   -81, -1000,  7975,  7456, -1000, -1000, -1000,
	 -1000, -1000, -1000,   585,   567,   604, -1000,   346, -1000,  5960,
	 -1000, -1000, -1000, -1000, -1000,  1887,  5960,  1485, -1000, -1000,
	  1674,  2559,   450,  5960,   -81, -1000,  5960,  1823,  1912,  2451,
	 -1000, -1000,   927, -1000,  1885, -1000, -1000,   746,  1907,  2513,
	 -1000,  2150,  2426,  4663,  2360,  7975,  2576,  1807, -1000, -1000,
	 -1000, -1000,   337,  1779,  5574,  5574, -1000,  5574,   927, -1000,
	   927, -1000, -1000,   602,   602, -1000, -1000,   604,  2315, -1000,
	  2229,  -192, -1000,  5960,  1558,  1779,  5183, -1000, -1000,  5960,
	  1269,  2426,  5960,   940,  5635,  1555,   927, -1000, -1000, -1000,
	   927, -1000,   602,   602, -1000, -1000,  3436, -1000, -1000,  1554,
	 -1000,   602, -1000,  1674,  1777, -1000, -1000,   604,   873, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,   927, -1000,  -192,   927,
	 -1000,   927, -1000,  -192,   927, -1000,  4257,  4257,  1022, -1000,
	  -192,  4851,  2572, -1000,   702,   724,  1009,  5960,  5960,  2507,
	   575,  1912, -1000,   702,  2115,  1674, -1000, -1000, -1000,   986,
	 -1000,  1079,  1449, -1000,   373,   178, -1000,  2006,  -131, -1000,
	 -1000, -1000, -1000,  1292,  1034,  1551, -1000,  1549, -1000, -1000,
	   400, -1000,  1481, -1000, -1000, -1000, -1000, -1000,   496, -1000,
	 -1000, -1000,  2507,  1912,  2225, -1000, -1000,  2381,  1912,   -81,
	 -1000,   496,  2258, -1000, -1000,   605, -1000, -1000,   337, -1000,
	   846, -1000,  2465,  1757,   670,   670,   832, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,   826,   777, -1000, -1000,
	 -1000,  1359, -1000,  1467,  1341,   766,   312, -1000, -1000, -1000,
	   496, -1000, -1000, -1000, -1000, -1000,   765, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,   496, -1000, -1000,   760,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,   852, -1000,    11, -1000, -1000,
	 -1000, -1000, -1000,  3965,   756, -1000, -1000, -1000, -1000,   496,
	 -1000, -1000,   753, -1000, -1000, -1000,    11, -1000, -1000, -1000,
	 -1000,  1542,    26,   438, -1000, -1000, -1000, -1000,   732, -1000,
	 -1000, -1000,    11, -1000, -1000,   346,   346, -1000, -1000,   927,
	 -1000, -1000,  5635,  5635,  5635, -1000, 10779, 10779,   847,  5960,
	 10779, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	   -50, -1000, -1000,   450, 10822, -1000, -1000,  2118, 10822, -1000,
	   728, -1000,  2432, -1000,    91,  2261, -1000,  7975, -1000,    -7,
	  1909,  1269, -1000, 10779, 10779, -1000, -1000,   312,  1541, -1000,
	  5960,    42,   927, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,  1507, -1000, -1000,  4608, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,   820,   102, -1000, -1000, -1000,
	  2445,   337,  1203, -1000,   695, -1000,   202, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,   651, -1000, -1000,    42, -1000,   602,
	  1497,  6297, -1000, -1000, -1000, -1000,   602, -1000,  5960, -1000,
	   346,   346,   346, -1000, -1000, -1000,   602,   450, -1000,  1230,
	  1842,  1230,   450, -1000, -1000,  1096, -1000,  1230, -1000, -1000,
	   627,  1230,  1674,  2087, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,  2289,  2252,  4993,  4993, -1000, -1000,  2380,
	 -1000, -1000, -1000, -1000, -1000,  1819,   602,   850,  7975,  7975,
	 -1000, -1000, -1000,   104,   450,   201,  1907,  2115, 10091,  2361,
	  2014, 10779, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	  2538, -1000, -1000,  1563, -1000, 10779,  2538,  2297,  4322,   602,
	   576, -1000, -1000, -1000,  1779,   663, -1000,  5261,   602, -1000,
	 -1000,   602, -1000,  1497, -1000,  1801,  1254,   200,   940,   927,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1539, -1000,   746,
	   702, -1000,  2115, -1000,   746,   927, -1000, -1000,  1783,  2507,
	 -1000,   -81, -1000,  1194, -1000, -1000, -1000,   927,   927,   927,
	 -1000, -1000, 10846,  2507,  1842, -1000,  1385, -1000,  -132, -1000,
	 -1000, -1000,   346,   346, -1000, -1000,  1907,   602,  1497,  2224,
	   927, -1000, -1000, -1000,   602, -1000,  -192,  -192,  1926,  1411,
	 -1000, -1000, -1000, -1000,  5574,   873, -1000, -1000,  1230,   916,
	  1230,   181,   602,   602, -1000, -1000,  5960,  2066, -1000, 10779,
	  2328,  4851, -1000,  2357,  2355,  2350,  2335,  2426, -1000,   702,
	 -1000, -1000,  2055, -1000,   440, -1000, -1000, 10822, -1000,  8366,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1527,
	 -1000,  -136, -1000,  -138, -1000,  1079, -1000, -1000,  1694, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,  2055, 10822, -1000,  7975,
	 -1000, 10822,   349, -1000,   816,   483, -1000, -1000,  -145, -1000,
	    42, -1000, -1000, -1000,   670, -1000,  1079, -1000, -1000,   806,
	 -1000,  -147, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   641,
	   496, -1000,    11, -1000, -1000, -1000,  1471,   637,   631, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,  1989, -1000, -1000,   640,
	 -1000,    87,  5960,  5960,  1674, -1000, -1000, -1000, -1000,  -172,
	 -1000, -1000, -1000, -1000, -1000, -1000,   -50, -1000, 10822, 10822,
	 10822, -1000, 10822,   -50, -1000, -1000, -1000, -1000,  2261, -1000,
	 10779, -1000, -1000, 10779, -1000,   188, -1000, 10779, 10779, 10779,
	 -1000,   -12,   130,   496, -1000,   604, -1000, -1000,   346, -1000,
	 -1000, -1000,   -10,   134, -1000,  -160, -1000,  4348, -1000, -1000,
	 -1000,     8, -1000,  2034,  2034,  2034, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,   -13, -1000, -1000, -1000, -1000,
	    70,   -10,    -4, -1000, -1000, -1000, -1000,  4521, -1000, -1000,
	 -1000,   602, -1000, -1000, -1000, -1000,  5574,  5574,  5960, -1000,
	 -1000,   604,  1767,  5960,   450, -1000, -1000,  2391,  -173, -1000,
	  1046, -1000, -1000,  4064,  1674,  2087,   602, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,   346, -1000, -1000,  2567,  1230, -1000,  5960, -1000,
	  2273, -1000, -1000,   -93, -1000, -1000,  1807, 10779, -1000, -1000,
	   189,   604,  2089, -1000, 10031, 10779, -1000, -1000, -1000, -1000,
	  2361, -1000, 10031, -1000,  -172, -1000, -1000,   604,  1642, -1000,
	 -1000,   189,   450, -1000,  1779, -1000, -1000, -1000, -1000,  2559,
	   628,  5960,  1692, -1000, -1000, -1000,  2541,  1674,  2229,  5960,
	 -1000,  1903,  1520, -1000, -1000, -1000, -1000,  5635, -1000, -1000,
	  2451, -1000, -1000, -1000,  7895,  2360,  4663, -1000, -1000,  2055,
	 -1000,   602, -1000, -1000, -1000,  -172, 10846, 10846, 10846,   496,
	 -1000,   496, -1000, -1000, -1000,   346, -1000,  4298, -1000,  1777,
	  5960, -1000, -1000,   309,  5574,  5574,  1779, -1000,  5960, -1000,
	 -1000, -1000,  3436,  6899,  6899, -1000, -1000,  7975,  2426, -1000,
	   602, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,  1514, -1000, -1000,  1674, -1000, -1000, -1000, -1000, -1000,
	 -1000,  4257, -1000, -1000, -1000,  2573,  -172,  2004,  4851, -1000,
	 -1000, -1000, -1000, -1000,  5960,  2426,  1749, -1000,  1414, -1000,
	 -1000, -1000, -1000,   927, -1000,   -45, -1000, -1000,   927, -1000,
	   -24,   130, -1000,  1079,  -174, -1000, -1000,  1079, -1000,   657,
	  1779,   927,   -81,   927,  -172,   605, -1000,   312, -1000, -1000,
	 -1000,  2574, -1000, -1000,   596, -1000,  1467,   110,  1781, -1000,
	    26, -1000, -1000, -1000, -1000, -1000,  1938,  1688,   591, -1000,
	   570, -1000, -1000, -1000, 10822, -1000,   -50, -1000, -1000,   -50,
	 -1000, -1000, -1000,   119,  -172, 10779, -1000,   -58,   -50,   -50,
	 10779,  1483,   562,  1819,  1674,  5960, -1000,  1507,   -20, -1000,
	 -1000, -1000, -1000,   197, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,   -10,  1497,   927,  6297, -1000, -1000, -1000,
	 -1000, -1000,  1230,   927, -1000,  1779,  1779,  1779,  1230,  4257,
	   927, -1000, -1000,  1503,   602, -1000,  2087,   450, -1000,  1375,
	  1674,  2284,   590,   602,  1838,  1605, -1000, -1000,  2411,  -172,
	 -1000,   413, -1000, -1000,  2297,   450,  -144, 10779,   450,   -58,
	  1533, -1000,   604, -1000, -1000, -1000,   604, -1000,   656, -1000,
	   450,  1779, -1000,  1839,  1684, -1000,   413, -1000, -1000,  2507,
	 -1000, -1000, -1000, 10846,  7645, -1000, -1000, -1000, -1000, -1000,
	   -50, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   -58, -1000,
	  1046,  1046,   927, -1000,  1497, -1000, -1000, -1000, -1000, -1000,
	  1497, -1000, -1000, -1000,  6297,  6297, -1000, -1000, -1000,   472,
	 -1000,   -81,  5960, -1000, -1000, -1000,   602,  1194, -1000,  7975,
	   604, -1000,   604, -1000,  1230,   440, -1000, -1000, 10822, -1000,
	 10779,   847, 10779,  5960,  1103, -1000, -1000,  1079,  1079, -1000,
	  2087,  1682, -1000,  1230, 10822,   546,   496, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,  2558,  1776,  2555,   616, -1000, -1000, -1000, -1000, -1000,
	 10779,  -172, -1000, 10779, -1000, -1000,   -30,  2587, -1000, -1000,
	  1488,   602, -1000, -1000, -1000,  1681, -1000,   235, -1000,   604,
	 -1000,   450,  1767, -1000,  1230, -1000, -1000,  4257, -1000,  2197,
	 -1000, -1000,  4521,  2265, -1000, -1000,  2087,  1230, -1000,   450,
	 -1000,  1674,  1838,  2273, -1000, -1000, -1000,  2553,  1474,  2381,
	 -1000, -1000, -1000, 10779,  -144, -1000, 10779,   450,  1533,  3642,
	 -1000,  1678,  2541, -1000,  1807,  1451,   280, -1000, -1000, -1000,
	 -1000, 10846, -1000, -1000, 10846,   927,   927,  5960, -1000, -1000,
	 -1000, -1000,  3865,  1411, -1000, -1000,   -81,  5960, -1000,  5960,
	  2218, -1000,  2048, -1000, -1000, -1000,  -172, -1000,  1079, -1000,
	 -1000, -1000,   514,   927, -1000,   604,  -172, -1000,   475, -1000,
	 -1000,  2130, -1000, -1000,  1676,  -172,   -50, -1000, 10779,   450,
	   604, -1000, -1000,  1674, -1000,  6297, -1000, -1000,  -192, -1000,
	 -1000,   159, -1000,   602, -1000,  1370, -1000,  2087,  2087,   450,
	 -1000, -1000, -1000, -1000, -1000,  -172, 10779,   927,  -172, -1000,
	   450,   450, -1000,  6297,  6297,   467, -1000, -1000,  1816,  1675,
	   -50,   -58, -1000, -1000,   602,  2426,   602, -1000,   602, -1000,
	  2571,  2493, -1000, -1000, -1000,  2087,  1230,  1230, -1000, -1000,
	   451, -1000,   -50, -1000,  1674,   450,   -10,   602,   604,  6453,
	  6461, -1000, -1000, -1000, -1000, -1000,  4521, -1000, -1000, -1000,
	  2087,  -172, 10779, -1000, -1000, -1000, -1000, -1000,  1329,  1814,
	 -1000, -1000, 10846, -1000,  6899,  5960, -1000,  5960, -1000,  5960,
	  5314, -1000, -1000, -1000, -1000,   927, -1000, -1000,   450, -1000,
	 -1000, -1000,   602, -1000, -1000, -1000, -1000, -1000,  6297, -1000,
	 -1000, -1000,  -172, -1000,  1673,   -50,   450, -1000, -1000, -1000,
	   927, -1000, -1000,  5314,   346, -1000,  1230, -1000, -1000,  6453,
	   602, -1000,   275, -1000, -1000,  5314, -1000, -1000, -1000,   450,
	 -1000, -1000,  6297,  1399, -1000, -1000, -1000, -1000
};

short yypgo[] = {
	     0,  4873,  2880,  4871,  4870,  4869,  4868,   356,  4867,  7514,
	  4866,  4863,  4862,  4853,  4852,  4851,  4849,  4848,  4846,  4845,
	  4844,  4843,  4842,  4840,  4839,  4838,  4837,  4835,  4834,  4833,
	  4829,  4828,  4827,  4826,  4825,  4824,  4822,  4820,  4819,  4818,
	  4817,  4814,  4813,  4811,  4810,  4809,  4806,  4802,   408,  4799,
	  4798,   405,  4796,  4795,  4794,  4793,  4792,  4791,  4790,  4789,
	  4779,  4778,  4777,   395,  4776,  4775,  4774,  4773,  4772,  4770,
	  4769,  4767,  4766,  4764,  4763,  4761,  4760,  4758,  4757,  4755,
	  4753,  4752,  4751,  4750,  4747,  4745,  4744,  4741,  4736,  4734,
	  4733,  4732,  4731,  4730,  4728,  4727,  4726,  4717,  4715,  4714,
	  4710,  4709,  4708,  4707,   128,   485,  4706,  4705,  4703,   394,
	  1937,  4702,   338,   332,  4697,    77,   398,  4696,  4695,  4693,
	  4692,  5875,   433,  4690,   391,  4689,   117,   401,   149,     0,
	  4688,   108,   327,  4686,  2870,  4682,  4678,   402,   107,  4673,
	  4672,  4671,   326,  4882,  4669,  4668,     1,    94,  4667,  4666,
	  4660,   397,    21,  4658,  4657,    64,   335,  4655,  4653,   229,
	  4652,  4651,  4650,  4293,  4649,   306,  4646,  4645,  4644,  4642,
	  4641,   590,  4640,   163,  4637,  4634,    16,  2957,    24,   154,
	  4633,  4632,  4631,   320,  4630,  2660,   121,  4629,   122,   135,
	    43,  4626,   403,    55,  4624,  4623,   315,  4622,  4618,  4616,
	  4611,  4609,  4607,  4605,    20,   393,    14,  4603,   141,    67,
	  4602,  4601,  4592,  4591,  4590,   430,   674,  4586,  4585,  4581,
	  4579,    84,    61,    51,    15,    36,  3450,  2682,  4577,    32,
	  4576,  4574,  4572,  4570,  4561,   114,   322,     8,   781,   325,
	   316,   380,   118,    11,  4559,    18,  4558,  4557,  4556,  4555,
	  4554,  4553,  4551,  4548,   421,  4545,   392,  4544,  4543,   436,
	  4542,  4536,  4534,  4533,  4531,  4527,  4526,   151,  4525,   113,
	   153,  4524,    48,  4523,   396,  4522,    34,  4521,  4517,  4516,
	   410,  4513,   147,   103,    53,  4512,    93,    72,    41,   234,
	    54,    74,    63,  4510,   331,  4509,  4508,   133,   123,  4506,
	   319,   339,    28,  4505,    68,    37,  4503,  4502,  4501,  4500,
	   144,  4499,  4497,  4493,  4492,    82,  4491,   136,  4490,  4487,
	  4486,  4485,  4480,  4479,    66,  4477,  2874,  4476,   176,  4475,
	  4474,  4472,  4471,  4460,   155,    45,    47,    52,    73,  4458,
	    62,    33,     9,   387,    49,  4457,   139,  4455,    88,    80,
	    65,  4454,  4452,  4449,  4448,  2871,   162,  4446,  4445,  4443,
	  4441,   131,  1480,  4438,   175,  4437,  2858,   237,  4436,   224,
	   100,  4435,  4434,   232,   145,    90,   294,   156,  4433,   184,
	  4432,  4428,     6,  4427,   329,  4426,   342,   247,   293,  4419,
	   248,  4417,    46,    23,  4416,   241,   134,  4415,  4412,  4411,
	  4409,   313,  4408,  4406,  4405,  4404,    87,    60,  4403,    35,
	   242,  4402,   303,   127,    40,  4399,    42,  4398,  4397,  4395,
	  4394,   143,  4393,   112,  4392,  4391,  4390,  4387,  4386,  4385,
	   227,  4384,  4382,  4376,  4375,  4374,    39,  4369,  4367,     3,
	     5,  4366,  4365,    19,  4363,  4362,  4361,  4359,  4358,  4357,
	   238,    89,    96,   152,    97,  4356,  4345,  1660,   157,    91,
	  4344,  4343,  4342,  4334,   159,  4332,  4329,  4326,    27,  4325,
	  4324,    22,   177,  4313,  4311,    50,    75,  4310,  4307,  4300,
	  4298,  4297,   158,  4296,  4292,  4282,  4279,  4275,  4274,  4271,
	  4270,  4269,  4268,  4264,   115,   109,    70,  4263,   170,  4261,
	  4258,   200,  4257,  4256,  4255,  4240,  4239,  4238,    44,  4237,
	  4235,  4233,   492,  4232,  4231,  4230,  4229,  4227,  4225,  4220,
	  4217,  4216,  4215,   273,  4213,  4212,  4210,  4209,   426,  4208,
	   244,  4207,  4205,  4203,  4202,  4201,  4200,  4199,  4198,  4196,
	  4194,  4193,  4190,  4189,  4187,  4184,  4183,  4181,  4180,   137,
	  4173,  4168,  4155,  4154,  4153,  4152,  4151,  4150,  4149,  4146,
	  4142,  4141,   252,  4137,  4136,  4127,  4123,  4122,  4121,  4120,
	   309,   359,   344,  4119,  4118,  4117,  4116,  4114,  4113,  4112,
	   233,  4108,  4106,   148,  4103,  4102,  4101,  4100,  4099,  4085,
	   390,   415,  4084,  4083,  4082,  4081,  4080,  4079,  4078,  4077,
	  4076,  4073,  4072,  4068,  4067,    38,  4066,  4058,  4056,    17,
	    57,  4052,    85,  4050,  4049,  4048,  4044,   236,   142,  4042,
	  4041,   243,  4032,  4031,  4030,  4028,  4027,  4025,  4024,  4023,
	  4022,  4017,  4015,  4013,  4012,  4011,  4010,  4009,  4008,  4007,
	  4004,  4002,  4001,  3996,   311,   386,   317,   308,   304,   140,
	  3992,   225,  3989,  3988,  3987,   389,  3985,   132,   314,   146,
	  3984,  3983,  3981,  3979,   374,  3976,  3973,  3972,  3971,  3969,
	   538,   102,  3968,    86,   340,  3966,  3964,  3962,  3961,   199,
	  3960,  3959,  3955,  3954,    12,    13,    10,  3952,  3951,  3950,
	    56,  3949,  3947,  3946,    31,  3944,    69,  3943,  3940,    30,
	  3938,     7,     4,     2,  3936,  3934,  3931,  3930,  3928,  6458,
	  3927,  3926,   216,  3925,  3916,  3915,  3914,  3911,  3906,    25,
	  3903,  3901,  3900,  3899,  3898,  3897,  3896,  3895,  3890,  3889,
	  3888,  3887,   376,  3886,   367,  3884,  3883,  3882,  3881,  3877,
	  3876,  3875,  3874,  3873,  3870,  3867,  3866,   385,  3865,   235,
	  3864,  3862,  3860,  3857,  3855,  3852,   240,  3851,   249,   368,
	  3850,   231,  3849,  3848,  3847,   165,  3845,  3844,  3843,  3842,
	  3836,  3833,  3832,  3830,   160,  3827,   161,  3825,  3818,  3813,
	  3812,  3808,  2859,  3806,  3805,  3803,  2860,  3802,  3801,  3800,
	  3799,  3798,  3794,   290,   228,   617,  3793,  2851,  3792,   299,
	   188,   302,  3791,  3789,  3788,  3785,  3782,   292,  3779,    76,
	  3772,  3770,  3768,   289,  3767,   416,   363,  3766,  3764,  3761,
	  3756,  3753,  3749,  3748,  3746,   245,  3745,  3744,   341,  3741,
	  3738,   328,  3737,  3736,  3735,  3733,  3732,   125,  3729,   214,
	  3728,  3725,  3724,  3719,  3716,   110,   124,  3714,  3713,  3710,
	  3709,   239,  3707,  3706,  3703,  3702,  3701,  3698,  3696,  3690,
	  3688,  3687,  3686,  3685,  3683,  3682,  3680,  3678,  3677,  3676,
	  3672,  3669,  3664,  3663,  3653,  3652,  3649,  3644,  3640,  3637,
	  3636,  3630,  3628,  3627,  3623,  3619,  3606,  3602,  3599,  3598,
	  3596,  3595,  3594,  3592,  3591,  3590,  3589,  3587,  3579,  3578,
	  3572,  3571,  3568,   213,  3563,  3557,  3556,  3555,  3554,   422,
	  3551,  3550,   373,  3549,  3540,  3526,  3525,  3523,  3520,  3519,
	   366,  3518,    79,  3514,  3510,   420,  3506,  3502,   370,   382,
	  3499,   282,   210,  3497,   138,   208,  3495,   207,  3494,  3492,
	   120,  3491,   895,  3490,  3486,  3485,  3478,   211,  3473,  3469,
	  3467,  3462,   369,  2418,  3461,  2412,  3460,  3458,   287,  3457,
	  3455,  3451,  3449,  2814,  3447,  3445,  2413,   409,  3444,  3443,
	   286,  3442,   365,  3426,  3425,  3421,  3416,  3409,  3406,  3403,
	   198,  3398,  3397,  3394,  3393,  3392,    78,  3391,  3390,   101,
	    83,  3385,  3384,  3383,  3378,  3377,  3376,  3374,   272,  3368,
	  3365,  3364,  3363,  3362,   269,  3360,  3359,   276,  3354,  3353,
	  3351,  3345,  3343,   413,  3341,  3336,   275,  3335,  3333,  3326,
	  3313,  3274,   362,  3273,  3268,  3266,  3263,  3262,  3259,  3258,
	  3257,  3256,  3251,  3246,  3242,   277,  3241,  3240,  3237,  3232,
	  3230,  3226,  3223,  3222,  3220,   419,  3213,  3212,  3209,  3206,
	  3204,  3202,  3201,  3200,  3199,  3197,  3192,  3191,  3189,  3181,
	  3175,  3174,  3171,  3166,  3161,  3158,  3156,  3155,  3154,  3153,
	   279,  3152,   281,   336,   106,   104,    92,    81,   411,  3151,
	  3149,  3148,  3147,  3146,  3145,  3142,  3140,  3137,  3136,  3133,
	  3132,  3131,  3129,  3128,  3127,  3126,  3125,  3124,  3123,   267,
	   297,  3122,   254,  3119,   116,  3117,  3116,  3114,  3113,   407,
	  2744,  3112,   280,  3109,  3096,  2753,  3095,  3094,   197,  3089,
	  3087,  3086,   263,   255,  3085,  3082,   202,  3077,   414,  3076,
	  3074,  3068,   268,   257,  3067,  3063,   258,  3061,  3057,  3053,
	  3045,   256,  3043,  3040,   192,  3027,  3024,   274,  3023,  3021,
	   204,  3020,  3019,  3017,  3016,  3014,   404,  3012,    58,   361,
	  3011,  3010,  3008,  1703,  3006,  3001,   126,   388,   130,   357,
	    99,   323,   150,   129,   119,  2999,  2997,  2994,  2991,  2989,
	  2986,  2981,  2980,    71,   105,  2977,  2974,    59,  2970,  2969,
	   264,  2966,  2964,  2960,  2959,  2956,  2955,   111,  2952,  2951,
	  2948,  2945,   166,  2944,   261,  2943,  2942,   187,  2936,    26,
	  2932,  2930,  2927,  2926,  2925,  2924,  2923,  2922,   260,  2916,
	  2913,  2904,  2903,  5513,  4962,  2896,   259,   291,   425,  2893,
	  2892,  2891,  2888,    98,   164,    95,   406,    29,  2883,  2881,
	   253,   334,  2873,  2867,   265
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
	   129,   763,   763,   474,  1037,  1150,  1150,  1150,  1150,  1150,
	  1150,  1150,  1150,  1150,  1150,  1150,     9,    20,     7,   813,
	   813,   813,     6,  1151,  1153,  1152,  1152,  1154,  1154,  1154,
	  1154,  1154,  1154,  1154,  1156,  1156,  1156,  1155,  1164,  1164,
	  1164,  1164,  1164,  1164,  1164,  1164,  1164,  1164,  1164,  1164,
	  1164,  1165,  1157,  1157,  1159,  1159,  1159,  1159,  1159,  1159,
	  1159,  1159,  1167,  1176,  1176,  1176,  1176,  1176,  1176,  1176,
	  1176,  1176,  1176,  1176,  1176,  1176,  1176,  1176,  1176,  1176,
	  1176,  1176,  1175,  1168,  1170,  1177,  1178,  1178,  1178,  1180,
	  1179,  1181,  1182,  1182,  1183,  1183,  1185,  1186,  1171,  1172,
	  1173,  1174,  1188,  1188,  1189,  1191,  1192,  1193,  1190,  1194,
	  1196,  1195,  1195,  1197,  1197,  1197,  1198,  1198,  1198,  1198,
	  1198,  1198,  1198,  1198,  1198,  1198,  1198,  1198,  1198,  1198,
	  1198,  1198,  1198,  1198,  1198,  1198,  1198,  1198,  1199,  1200,
	  1201,  1169,  1169,  1184,  1166,  1166,  1202,  1202,  1202,  1202,
	  1203,  1204,  1205,  1207,  1207,  1208,  1208,  1206,  1209,  1209,
	  1160,  1210,  1213,  1213,  1211,  1211,  1212,  1212,  1187,  1187,
	  1214,  1214,  1214,  1214,  1214,  1214,  1214,  1158,  1161,  1161,
	  1215,  1216,  1216,  1216,  1216,  1216,  1162,  1217,  1217,  1218,
	  1218,  1219,  1219,  1163,  1220,  1221,  1221,  1222,  1223,  1224,
	  1224,   677,   756,  1226,   512,   457,  1225,  1228,   523,   523,
	  1229,  1230,  1230,  1231,  1231,  1232,  1232,  1233,  1233,  1235,
	   390,   343,  1227,  1227,  1238,  1238,  1239,  1239,  1239,  1240,
	  1243,  1242,  1236,  1237,  1241,  1241,  1244,  1234,  1234
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
	     1,     1,     2,     3,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
	     1,     1,     3,     3,     4,     0,     2,     4,     3,     1,
	     1,     1,     1,     2,     0,     1,     1,     1,     1,     1,
	     2,     2,     2,     1,     1,     2,     2,     2,     1,     1,
	     1,     1,     1,     1,     2,     2,     2,     2,     2,     2,
	     2,     1,     1,     1,     1,     1,     1,     1,     1,     2,
	     2,     2,     2,     2,     2,     2,     1,     1,     2,     2,
	     3,     1,     1,     3,     2,     1,     1,     1,     0,     1,
	     3,     1,     1,     3,     3,     1,     1,     1,     3,     3,
	     2,     3,     1,     1,     1,     0,     1,     1,     3,     1,
	     1,     1,     2,     3,     1,     2,     1,     2,     2,     2,
	     1,     2,     2,     2,     1,     2,     2,     2,     1,     2,
	     2,     2,     1,     2,     2,     2,     1,     1,     1,     1,
	     1,     0,     1,     1,     1,     3,     2,     2,     2,     1,
	     3,     1,     1,     0,     2,     1,     1,     1,     0,     2,
	     3,     2,     2,     1,     1,     0,     1,     1,     1,     2,
	     1,     2,     1,     2,     1,     1,     1,     1,     3,     1,
	     1,     1,     1,     2,     3,     1,     6,     2,     4,     1,
	     1,     1,     1,     3,     3,     0,     2,     3,     1,     1,
	     0,     2,     2,     1,     2,     2,     0,     1,     1,     2,
	     4,     1,     0,     1,     0,     1,     3,     2,     6,     1,
	     2,     2,     1,     2,     2,     2,     1,     2,     2,     2,
	     1,     5,     1,     1,     1,     2,     2,     1,     1
};

short yychk[] = {
	 -4000,    -1,    -2,    -3,    -4,    -5,    -6,    -7,    -8,   -10,
	   -11,   -12,   -13,   -14,   -15,   -16,   -17,   256,   -18,   494,
	 -1151,   492,   -22,   -23,   -24,   -25,   -26,   -27,   -28,   -29,
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
	   558,   559,   560,   452,    -9,   591,    -9,    -9,    -9,    -9,
	   -19,   -21,   624,   486,   485, -1152,    -9,  -819,  -820,   290,
	  -673,  -826,  -827,  -209,  -215,  -457,  -670,  -671,  -216, -1223,
	   485,   499,   506,   505,    -9,  -830,  -849,   410,    -9,    -9,
	    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,
	    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,
	    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,
	    -9,    -9,    -9,    -9,   400,  -292,   463,  -104,  -115,  -129,
	  -143,  -669, -1150,   486,  -670,  -671,   597,   603,   604,   598,
	   600,   599,   601,   602,   617,   618,   619,  -104,  -115,  -118,
	  -119,  -116,   485,   431,   401,  -155,   468,  -121,  -129,  -147,
	  -216,  -124,  -121,  -315,  -129,  -317,   348,  -134,  -115,  -134,
	  -532,   485,  -124,  -104,  -129,  -121,  -104,  -182,  -183,  -184,
	  -185,  -109,  -124,  -129,   502,  -186,   462,  -115,  -121,  -198,
	  -205,  -206,   332,   338,   310,   319,   456,  -252,  -254,   352,
	  -258,  -115,  -121,  -121,  -115,  -115,  -121,  -315,  -129,  -319,
	   458,  -673,  -673,  -173,  -116,  -861,  -143,  -512, -1224, -1223,
	  -326,   456,   411,   334,   342,   399,  -328,  -129,   361,  -326,
	  -586,  -206,  -589,  -590,  -240,  -239,  -423,  -572,  -571,  -237,
	  -129,  -301,   310,   319,   332,   289,   262,   338,   357,  -241,
	   460,  -185,  -854,   458,   485,  -238,  -129,  -209,  -124,  -121,
	  -124,  -129,  -129,  -124,  -565,   280,   485,   463,  -211,  -217,
	  -218,  -230,  -246,   485,   466,  -129,  -530,   483,   485,   499,
	  -171,  -407,  -317,   485,   315,  -663,  -664,  -646,  -647,  -665,
	   580,   581,   582,  -352,  -353,  -354,  -355,  -129,  -147,  -315,
	  -324,  -209,   486,  -457,  -514,   391,   256,  -518,  -366,  -522,
	   256,  -386,   281,   460,  -525,  -527,   281,  -706,  -708,  -709,
	  -734,   485,   499,  -801,  -457,  -795,   486,  -716,  -720,  -727,
	  -733,   301,  -753,   460,   334,  -777,   501,  -780,   348,   456,
	  -734,   390,  -782,   502,  -788,  -789,  -791,   468,  -797,   502,
	  -782,  -786,   502,  -786,  -805,   485,  -795,  -457,  -811,   485,
	   276,   399,  -366,  -834,  -835,  -673,  -907,  -512,   486,   476,
	   472,   456,  -909,  -709,  -911,  -709,  -709,  -916,  -923,  -924,
	   417,    -9,  -909,  -952,  -953,   502,   281,  -957,   502,  -709,
	  -797,  -797,  -963,   502,  -734,  -992,   502,  -995,   485,  -998,
	  -953,   502, -1001,   475,   472,   480, -1002,   485,  -963,  -732,
	  -800,   484,   488,  -512,  -809,   483,   631,   630, -1011, -1013,
	 -1014,  -929,   419, -1018,   472, -1020,   502, -1024,   479,  -952,
	  -953, -1031,   502,  -925,  -953,  -998,   502,  -998,  -953,  -998,
	  -953, -1053, -1055, -1057,   463,   477,   478,    -9,  -903,  -904,
	   298, -1067,   502,  -709, -1095,   502, -1104, -1115, -1147,  -809,
	  -756, -1224, -1104, -1107, -1110,   502,   281,   485, -1115,   502,
	 -1104, -1104, -1130,   502,   502, -1138, -1110,   502,   485, -1104,
	 -1104,   485,  -141,  -144,   334,   480,   334,   485,   315,   337,
	   317,   320,   328,   399,   353,   334,   342,  -185,   346,  -324,
	  -759,  -760,   485,   499,  -216,   310,   319,   338,   332,   334,
	   290,  -535,  -536,   334,   342,   317,   320,   328,  -238,  -121,
	  -104,  -104,  -121,  -129,  -115,  -115,  -115,  -259,  -273,  -274,
	  -186,   462,   571,  -121,  -410,  -121,  -238,  -238,  -604,   334,
	  -673,   463,  -619,   337,   317,   334,   399,   342,  -630,   334,
	   342,   317,   399,  -209,  -215,  -519,  -520,  -324,  -719,   463,
	  -709,  -373,  -376,   282,  -709,   429,  -709,  -955,  -709,  -966,
	   486,   485,   472,   480,   334,   411,  -955,  -966,    -9,  -709,
	  -709,  -709,  -709,  -709,  -709,  -709,  -709,  -947,  -953,  -952,
	   502,  -709,  -709, -1104, -1104, -1104,   399,   315,  -121,  -709,
	   -20,    -9, -1153, -1154,   299, -1155, -1156, -1160, -1161, -1162,
	 -1163,   256, -1164,   620,   621, -1210, -1215,   494, -1217, -1220,
	 -1165,   262,   610,   609,   617,   618,   619, -1213,   493,   392,
	   524,   608,   595,   615,    -9,  -821,   486,  -512,   281,  -528,
	  -828,  -512,  -346,   363,  -523, -1229, -1230,   505,   504, -1228,
	 -1227, -1238, -1207,   616,   485,  -364,  -383,   298,    -9,  -832,
	   460,   502,   411,   591,  -530,  -105,  -177,  -135,  -116,   501,
	  -112,  -113,   303,   311,  -107,  -108,   311,  -111,  -112,  -113,
	  -116,  -116,  -610,  -679,  -680,  -681,  -222,  -683,   485,   334,
	   270,   280,   338,   315,   360,   359,   317,  -457,  -605,   385,
	  -122,  -150,  -151,  -152,  -154,  -157,  -158,  -160,  -161,  -162,
	  -164,  -165,  -166,  -167,  -168,  -169,  -170,   301,   375,   374,
	   366,   367,   368,   364,   365,   327,   371,   372,   369,   370,
	   373,  -146,   504,  -146,  -125,  -112,  -113,  -146,  -105,  -105,
	  -109,  -116,  -142,   461,  -173,  -177,  -186,  -183,  -279,   505,
	  -192,  -121,  -116,  -196,  -109,  -199,  -177,   462,   274,   463,
	  -253,  -256,   -48,   -51,   -63,   309,  -255,  -129,  -259,  -122,
	  -276,   469,  -173,  -173,  -196,  -280,  -109,  -196,  -320,  -209,
	  -215,   348,  -815,   460,   281,  -815,  -173, -1228,  -587,  -186,
	  -274,  -588,   485,  -177,  -109,  -591,   279,  -163,   483,  -591,
	  -124,  -600,  -116,  -605,  -857,  -512,   486,   629,  -177,  -250,
	  -272,  -143,  -457,  -616,  -109,  -186,  -292,  -566,  -568,  -206,
	  -570,  -129,  -423,  -240,  -239,  -237,  -571,  -572,  -301,   485,
	   463,  -631,  -212,   469,  -231,  -236,  -237,  -239,  -240,  -143,
	  -634,   465,  -638,  -129,  -317,  -640,  -254,  -642,  -644,  -112,
	  -113,  -646,  -647,  -171,  -177,  -666,   382,   384,   383,  -356,
	  -242,   458,  -401,  -146,  -152,  -146,  -401,   469,  -387,  -388,
	  -390,  -346, -1224,   294,  -528,   294,  -707,  -710,   502,  -717,
	  -745,   468,  -721,  -726,  -731,   348,  -734,  -754,  -756,   502,
	   485,  -778,   485,   499,  -732,  -783,   468,  -792,  -793,  -756,
	  -790,  -732,   485,  -798,  -799,  -512,  -800,  -801,  -806,  -807,
	  -794,  -795,   485,   348,  -812,   389,   485,   396,   386,   401,
	   485,  -367,  -356,  -836,   502,   281,   501,  -912,   338,   456,
	  -914,  -457,   486,   485,  -709,  -917,  -927,  -928,  -929,  -925,
	  -903, -1071, -1072, -1073,  -756, -1078,   294,  -958,   485,  -960,
	   501,  -964,  -969,   631,  -970,  -971,  -756,  -922,  -977,  -978,
	   468,  -993,   485,   502, -1069, -1070,  -709,   439,  -709,  -709,
	 -1004, -1006, -1007,  -972,  -973,   485,   486,   430,   419,   472,
	   480,   474,   334,   350,   313,   483,   483, -1013, -1015, -1016,
	   473,  -937,   485,  -709,  -909,   503, -1025, -1027,   459,  -952,
	   503, -1033,  -747, -1034,  -457, -1035,  -709, -1045, -1049,   468,
	 -1045,   503, -1045, -1045, -1056,  -732,   299, -1058, -1060,   479,
	  -903,  -384,    -9,  -909,   503,  -709, -1096,   460,   503, -1098,
	 -1099,   485,   486, -1101, -1100,  -457,   272, -1102, -1075, -1076,
	 -1077, -1074,  -809,  -989,  -990,  -795,   631,   630,   484,   488,
	 -1226, -1227,   460, -1111, -1112, -1073, -1078,   502, -1116, -1118,
	  -457,  -809,  -989,  -990,  -795, -1121, -1122, -1073,   502, -1128,
	   502, -1131, -1132, -1133,   485,   486, -1135, -1136, -1123, -1133,
	 -1140, -1141,  -709,   502, -1144, -1149,  -800,   484,   488,  -457,
	 -1128,  -124,  -145,  -129,  -147,   317,  -280,   347,  -515,   504,
	  -410,  -553,  -315,  -186,  -549,  -132,   344,   345,  -551,  -206,
	  -561,  -562,  -163,  -115,  -121,  -121,  -121,   390,   334,  -177,
	  -856,  -815,   269,   317,   504,  -324,   458,  -526,   469,  -719,
	  -374,  -206,  -377,   273,  -163,  -920,   486,   485,   384,   338,
	  -457,    -9,  -967,  -967,  -709,  -709,  -709,  -709,  -709,  -709,
	  -709,  -709,    -9,    -9,   503,  -709, -1109, -1118, -1109,  -556,
	  -709,   392, -1156, -1159, -1167, -1168, -1170, -1171, -1172, -1173,
	 -1174, -1175, -1176, -1177, -1188,   606,   599,   600,   598,   603,
	   604,   605,   601,   602,   597,   612,   611,   336,   591, -1211,
	   502, -1216,   485,   486,   624,   334, -1221,   617,   618,   619,
	   617,   618,   619,   485, -1218, -1219,   485,   499,   400,   615,
	    -9,  -822,   294,   501,  -344,  -674,  -675,  -676,   297,   505,
	  -344,   502,   485, -1231,   596,  -209,   485, -1208, -1239,   632,
	   634, -1240, -1242, -1243, -1236,   485,   614,   502,  -384,    -9,
	  -831,  -847,   298,    -9,  -756,  -850,  -851,  -390,  -115,  -116,
	  -137,   485,  -137,  -109,   456,   485,  -177,  -223,  -171,   629,
	  -223,  -151,  -153,  -171,  -155,  -155,  -159,   457,  -156,  -163,
	  -226,  -227,   630,   631,  -156,  -163,  -129,  -129,  -126,  -127,
	  -129,  -294,  -131,  -295,  -296,  -297,  -309,  -298,  -299,  -308,
	   354,   337,   356,  -303,   359,   355,  -129,  -131,  -129,  -576,
	  -579,  -580,  -129,  -175,  -129,  -109,  -121,  -110,   503,  -121,
	  -142,  -129,  -173,  -207,  -208,  -129,  -192,  -205,  -256,  -260,
	  -264,  -265,   470,  -124,  -152,  -316,   569,  -281,  -126,  -129,
	  -152,   464,  -209,   504,   504,  -816,   281,  -523,   294,   471,
	  -292,  -410,  -592,   390,   334,   399,   485,  -410,  -590,  -238,
	  -109,  -109,  -330,   358,  -196,  -292,  -606,  -129,  -612,   483,
	  -457,  -129,  -173,  -173,  -617,  -152,  -620,  -621,  -622,  -129,
	  -126,  -129,  -129,  -567,  -186,  -274,  -177,  -569,   485,  -109,
	  -109,   469,  -632,  -530,  -213,  -219,   289,  -248,   584,   585,
	  -232,  -177,  -241,  -242,  -243,  -186,   274,  -109,  -247,   295,
	  -635,  -636,   485,  -639,  -186,  -652,  -653,  -654,  -655,  -171,
	   486,   483,  -317,  -671,  -406,  -155,   464,  -407,  -641,  -129,
	  -482,  -143,  -343,  -328,   376,   377,   378,  -661, -1223,   379,
	  -216,  -643,  -406,  -155,  -177,  -645,   485,  -645,  -664,   384,
	   380,   381,  -357,   267,  -394,  -395,  -396,  -109,  -355,  -129,
	  -129,  -129,  -129,  -129,  -222,  -424,  -425,  -430,  -431,  -109,
	  -432,   332,   295,  -389,   501,  -344,  -344, -1227,  -523,  -523,
	  -711,  -712,  -714,  -709,   334,   474,  -746,  -747,  -748,  -457,
	  -749,  -743,  -744,   485,   486,  -722,  -728,   397,  -732,  -755,
	   468,  -758,  -761,  -762,   485,   499,   501,   485,   503,   501,
	  -344,   629,   503,   501,   503,   501,   629,   463,  -813,   505,
	   485,   486,  -813,  -368,  -412,   336,  -204,  -406,   503,  -673,
	  -838,  -839,  -825,   294,  -709,   460,  -912,  -918,  -944,  -928,
	  -930,  -932,  -933,   473,   481,   482,   474,  -937,   485,    -9,
	   503,   501,  -942,   461,   629,  -344,  -523,   501,  -942,  -709,
	   503,   501,  -972,  -344,  -979,  -980,  -981,  -983,   485,   486,
	  -942,  -734,   503,   501,  -942,   502,  -709,   503,   501,  -942,
	   502,  -931,   501,    -9,  -934,   486,   485,   456,  -457,  -935,
	  -938,   502,   483,  -457,   503,  -998,   503,   501,  -942,   502,
	 -1059, -1061,   459,  -952,  -905,    -2,   299,   503, -1073,   503,
	   501,   629,   629,   629,  -344,   502,   484,   488,   484,   488,
	  -523,   503,   501,  -942,  -523, -1112, -1117,   502,   503,   501,
	  -942, -1125, -1126,   485,   485,   503,   501,  -942,   503,   501,
	  -942, -1148,   502,   503,   501,  -942, -1141, -1145,   502,  -142,
	  -142,  -146,  -146,  -173,   334,  -759,   485,   499,  -173,  -177,
	  -537,   334,   399,   485,  -121,  -177,   485,  -121,  -816,   468,
	  -209,   485,  -375,   632,  -378,  -379,  -129,  -362,  -472,  -109,
	  -473,  -227,  -226,   485,  -474,  -475,  -500,  -502,  -453,   308,
	  -121,  -477,   333,   302,   325,   324,   300,  -225,  -478,  -479,
	  -480,  -349,  -229,  -348,  -216,  -163,  -143,   487,   489,   484,
	   488,  -481,   275,  -482,   358,  -488,  -350,  -503,   307,  -506,
	  -507,  -457,  -346,   467,   490,  -291,   306,  -505,   330,  -508,
	   321,   323,   329,  -510,  -511,   341,   340,  -921,   502,  -968,
	   486,   485,   483,  -457, -1022,  -732, -1022, -1022,  -920, -1022,
	 -1022, -1022,  -557,  -177,   400, -1157, -1166, -1159, -1202, -1203,
	 -1205, -1207, -1158,   591, -1166, -1169, -1166, -1166, -1169, -1166,
	 -1166, -1169, -1178, -1180,   485,   613, -1189, -1191, -1192, -1193,
	   485,   607,   598,   601,   599,   598,   599,   600,   598,   603,
	   604, -1212, -1187, -1214,   483,   631,   484,   488,   486,   485,
	   597,   591,   485,   504,   502, -1222,    -7,   525,   504,   591,
	  -823,  -824,   281,  -386,  -828,  -674,  -677, -1225,   485, -1207,
	 -1241, -1244, -1234,   504,   592, -1241, -1209, -1207,  -385,   299,
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
	 -1227,  -648,  -649,  -171,  -648,  -644,  -109,  -109,  -358,  -361,
	  -208,  -177,  -397,   326,  -282,  -395,  -109,  -401,  -401,  -426,
	  -412,  -165,  -367,  -282,  -433,  -376,   332,  -388,   503,  -713,
	   501,   629,  -750,   501,   629,  -723,  -729,  -736,   407,   457,
	   483,  -457,   503,   501,  -763,   485, -1150,   597,   485,   499,
	   629,  -793,   629,  -799,  -807,  -808,  -809,  -795,  -457,   485,
	  -813,   485,  -283,  -165,  -282,   456,   273,  -392,  -407,  -193,
	  -222,  -457,  -837,   629,   503,   501,  -523,  -523,   460,  -945,
	  -946,   418,  -931,   501,   472,   480,  -934,  -934,  -934,  -936,
	   485,   486,  -926,  -903, -1072,  -709,   422,  -958,  -709,  -970,
	  -942,   501,  -982,   502,   629,  -709,   501, -1070,  -845,  -846,
	  -512, -1075, -1076, -1077, -1074,   275,  -709, -1007, -1008,   486,
	   483,   484,   488,   338,   384,   485,  -457,   462,   503,  -974,
	   486,   485,   483,   481,   482,   426,   473,   440,   480,   472,
	  -457,  -903, -1016,  -935,  -935,  -939,  -940,  -941,   485,   486,
	   430,   419, -1035, -1036, -1004,  -998,    -9, -1099, -1100,  -457,
	 -1100, -1100, -1073, -1112,  -709,   503, -1119, -1147, -1122, -1123,
	   503,   501,  -942,  -942, -1132, -1102, -1136, -1102, -1104, -1141,
	 -1102,   503,   485,  -143,  -143,  -129,  -129,  -315,  -538,  -563,
	  -206,  -163,  -558,  -559,  -560,  -562,  -177,  -223,  -146,  -380,
	  -471,   301,  -340,  -342,  -226,  -227,  -341,   632,   633,   634,
	  -362,  -362,  -362,  -340,  -501,   273,   456,  -109,  -501,   502,
	   614,   502,  -489,  -491,  -362,  -497,  -498,  -235,  -110,  -362,
	  -504,   485,  -143,  -109,  -109,  -344,  -344,   502,   486,  -109,
	  -276,  -922,  -410,   591, -1158, -1184,   501,   629,   505, -1204,
	   485, -1204, -1179, -1181,   593, -1190, -1194,   593, -1190, -1190,
	   599,   483,   484,   488,   485,  -758,   400, -1218,   485,   499,
	    -9,  -825,  -387, -1226,  -391,   502, -1232, -1233, -1235, -1236,
	   485, -1244, -1240,   589, -1240,    -9,  -848,   299,  -851,  -138,
	  -684,  -109,  -115,   456,   360,  -222,  -138,  -110,  -672,  -530,
	  -143,  -129,  -147,  -457,   485,   499,  -138,  -689,  -690,  -691,
	  -171,  -109,  -163,  -129,  -129,  -770,  -288,  -293,   268,  -289,
	  -171,  -290,  -109,  -764,   586,   587,   588,  -765,  -766,  -767,
	  -768,  -769,  -310,   468,   276,  -773,  -703,  -775,  -776,  -131,
	   358,  -772,  -298,  -300,  -297,  -171,  -238,  -121,  -443,  -444,
	   623,   622,  -193,  -193,  -454,  -362,  -469,  -423,  -450,  -458,
	  -178,  -462,  -467,   624,   625,   629,   626,   627,   628,   277,
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
	  -730,   579,  -737,   408,   629,  -735,  -761,  -289,   502,   485,
	   485,  -784,   483,  -512,  -794,  -344,  -369,  -159,  -393,  -411,
	   316,  -159,   348,  -840,  -841,  -843,   272,  -839,  -673,  -523,
	  -947,  -903,  -932,  -934,  -934,  -935,   502,   503,  -980,  -985,
	  -986,  -987,   485,   486,   350,  -984,   485,   486,   503,  -996,
	  -756,  -344,   503,  -344,   503,   503,   501,  -942, -1037, -1040,
	 -1150,   486,  -457,   485,   503,   503,  -942, -1126, -1118, -1144,
	   503,  -942,  -146,  -146,  -177,  -121,  -121,  -121,  -379,  -362,
	  -340,  -508,  -129,  -472,  -110,  -110,  -409,  -476,  -109,  -227,
	  -226,  -475,  -501,  -409,   503,  -490,   299,  -492,  -493,  -495,
	  -235,  -494,  -498,  -499,   312,  -193,  -110,  -177,  -464,   458,
	  -143,  -362,  -362,  -512,  -483,  -484,   485,  -509,  -129,    -9,
	 -1202, -1206, -1209, -1182, -1183, -1185,   485, -1195, -1197, -1198,
	 -1163,   256, -1167, -1199, -1200, -1201, -1171, -1172, -1173, -1174,
	 -1168, -1170,   617,   618,   619,   503,   591,  -364,  -523,   503,
	 -1234, -1209, -1207, -1237,   503,    -9,  -110,  -177,  -190,  -678,
	   485,  -685,  -225,  -110,  -668,  -129,  -146,  -146,  -146,  -110,
	  -110,  -171,  -692,  -186,  -690,  -110,  -771,   485,   317,  -679,
	  -109,  -171,  -289,  -171,  -109,  -171,  -163,  -765,   275,   358,
	   351,   275,   358,   351,  -774,  -156,   275,  -482,   486,   488,
	   484,  -349,  -129,  -774,  -776,   276,  -292,  -110,  -109,  -302,
	  -304,   462,  -193,  -193,  -110,  -110,  -356,  -470,  -206,  -377,
	  -362,  -206,  -453,  -452,  -109,  -459,  -460,  -461,   304,   331,
	  -458,  -464,  -109,  -452,  -362,   277,  -468,  -243,  -178,  -179,
	   485,  -362,  -334,  -110,  -578,  -143,  -457,   485,  -582,  -155,
	  -171,  -583,   502,  -129,  -143,  -179,  -110,  -177,  -110,  -177,
	  -190,  -201,   466,  -209,   456,   486,  -263,  -271,   566,  -270,
	  -177,  -267,  -421,  -110,  -374,  -282,  -177,  -128,  -284,  -369,
	  -324,  -333,  -335,  -336,  -337,  -338,  -109,  -227,  -226,  -343,
	  -345,  -346,  -225,  -347,  -348,  -129,  -349,  -350,  -284,  -602,
	  -186,  -163,   485,   629,  -146,  -146,  -618,  -110,  -177,  -190,
	  -549,  -110,  -177,  -223,  -223,  -234,  -204,  -235,  -245,   485,
	  -104,  -657,  -659,  -660,  -657,  -317,   411,   334,  -143,  -649,
	  -650,   270,  -143,  -163,  -227,  -206,   358,   485,   269,   257,
	  -457,  -109,  -110,  -110,  -361,   390,  -362,  -396,  -109,   322,
	   322,   322,   322,  -400,  -401,  -110,  -428,  -370,  -417,   395,
	  -416,  -163,  -229,  -413,  -414,  -409,  -434,   632,  -435,  -436,
	  -129,  -362,  -725,  -741,   485,   629,   629,  -738,  -719,   483,
	  -370,  -413,  -193,  -408,  -409,   504,  -842,   502,  -844,  -845,
	  -846,  -837,    -9,  -934,  -709,   503,   501,   629,   503,  -344,
	  -942,  -940,  -943,   486,   483,  -457, -1038,   502,   411,   503,
	 -1104,  -129,  -129,  -163,  -471,  -110,  -409,  -409,  -409,  -409,
	  -110,  -494,  -495,  -362,  -362,  -496,   335,  -362,  -362,  -362,
	  -242,  -152,  -344,  -485,  -109,  -146,   594, -1184, -1186,   629,
	 -1196, -1197,   594, -1166,   591, -1167, -1171, -1172, -1173, -1174,
	 -1167, -1171, -1172, -1173, -1174, -1167, -1171, -1172, -1173, -1174,
	   591, -1233, -1235, -1209,  -684,  -110,  -177,  -143,  -129,  -129,
	  -143,  -129,  -109,  -693,  -694,  -695,  -697,  -698,  -163,  -605,
	  -238,  -110,   317,   629,  -610,  -289,  -171,  -163,  -289,  -110,
	  -177,  -146,  -171,  -238,  -305,  -306,   310,   338,  -204,  -362,
	  -451,  -456,   278,  -452,  -109,  -454,  -362,  -459,  -463,  -362,
	  -109,  -468,  -243,  -451,  -110,  -173,  -179,  -129,   483,  -176,
	  -163,  -188,  -189,  -202,   458,   485,  -121,  -269,  -375,  -283,
	  -286,  -370,  -110,  -177,  -339,  -340,  -341,  -342,  -226,  -227,
	  -338,  -336,  -337,  -338,  -338,  -344,  -344,  -110,  -338,  -603,
	  -406,  -155,  -608,  -609,  -129,  -612,  -143,  -143,  -129,  -173,
	  -129,  -221,  -224,  -225,  -226,  -227,  -228,  -121,  -229,  -129,
	  -224,  -193,  -152,  -110,   485,  -163,  -138,   274,  -399,  -186,
	  -404,   281,  -129,  -429,  -276,  -418,  -171,   475,  -177,  -190,
	  -177,  -146,  -223,  -152,  -742,  -719,   629,  -740,  -739,  -709,
	   503,   501,  -371,  -155,  -177,  -673,  -512,   503,  -986,  -988,
	  -809,  -795,  -989,  -990,  -457,   485,   390,   358,  -922,  -845,
	 -1039,   468, -1041,   276,   483,   503,   503,  -476,  -110,  -110,
	  -496,  -362,  -110,  -177,  -110,  -110,  -362,  -287,   503,  -486,
	  -292,  -163,  -129, -1183, -1187,   631, -1158, -1209,  -190,  -177,
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
	  -487,   485,  -110,  -177, -1237,  -109,  -110,  -694,  -171,  -699,
	  -684,  -667,   339,  -687,  -684,   334,  -289,  -171,  -110,  -163,
	  -302,  -305,   275,   485,  -393,  -362,  -443,  -465,  -362,  -110,
	  -468,  -584,  -225,  -227,  -226,   483,  -176,  -204,   485,   505,
	  -338,  -338,  -609,  -110,  -224,  -245,  -405,  -129,  -403,  -129,
	  -237,  -419,   411,   350,  -719,   503,  -177,  -109,   503,   358,
	   483,  -110,  -362,  -110,  -109,  -163, -1209,  -685,  -223,  -109,
	  -455,   624,   625,   627,   628,  -110,  -177,   485,  -289,  -289,
	  -110,  -362,  -177,  -110,  -110,  -225,  -225,   503,   463,   483,
	  -110,  -110,  -177,  -110,  -177,  -152,  -110,  -177,  -110,  -177,
	  -243,  -420,   291,  -289,  -382,  -171,   503,  -110,  -163,  -110,
	  -110,  -686,  -700,  -702,  -703,  -701,   358,  -225,  -109,  -701,
	  -684,  -289,  -362,   485,   463,  -338,  -224,  -129,  -129,  -129,
	  -438,  -439,  -440,  -441,  -129,  -206,  -177,  -110,  -110,  -177,
	  -704,  -225,   483,  -110,  -110,  -177,  -440,  -129,  -146,  -171,
	  -702,  -110,  -177,   505,  -439,  -110,  -225,   483
};

short yydef[] = {
	     2,    -2,     1,     3,     4,     5,     6,     7,     0,     0,
	     0,    11,    12,    13,    14,    15,     0,     0,     0,    19,
	  2255,  2248,    25,    26,    27,    28,    29,    30,    31,    32,
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
	  1667,  2430,     0,   836,     0,   430,   431,   963,     0,  1004,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,   901,  1089,     0,     0,     0,     0,     0,     0,
	     0,     0,   510,   825,     0,  1413,  1464,  1524,  1535,     0,
	  1544,     0,     0,     0,     0,     0,     0,  1804,  1604,   510,
	     0,  2430,  1724,     0,     0,     0,     0,  1755,  1805,  1806,
	  1807,     0,     0,  1813,     0,  1818,     0,     0,     0,     0,
	     0,     0,  1908,  1913,     0,  1919,  1922,     0,  2430,     0,
	  1959,  1966,  1970,     0,  1984,  2055,     0,     0,     0,     0,
	     0,  2049,     0,     0,  2430,  2430,     0,     0,  2430,     0,
	  2430,  2430,     0,     0,     0,     0,  2430,  2430,   107,   109,
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
	     0,  2038,  2065,     0,  2005,     0,  2146,  2148,  2430,  2157,
	  2159,  2163,  2168,  2173,  2430,  2181,  2430,  2193,  2195,  2198,
	   152,   855,   860,     0,   868,   866,   889,   890,   891,   892,
	   893,   894,   869,   895,  1009,  1014,  1010,  1421,   483,  1744,
	  1810,  1817,  1830,  1918,  1930,  1948,  1963,  1969,  1978,  1981,
	  2009,  2013,  2020,  2023,     0,  2046,  2130,  2131,  2151,  2187,
	  1011,  1012,  1013,  2043,     8,  2246,     9,    10,    16,    17,
	     0,    21,    22,    23,    24,  2264,   105,  1602,     0,  2430,
	  1613,  2430,  2442,    -2,     0,  1308,  1278,  1279,   239,  2383,
	  1276,  1277,     0,  2428,   506,     0,     0,     0,  1674,  1675,
	  1676,  1677,  1678,  1680,  1681,  1682,  1684,  1685,  1686,  1687,
	  1688,  1689,  1690,  1691,  1692,  1693,  1694,  1695,  1696,  1697,
	  1698,  1699,  1700,  1702,  1703,  1704,  1705,  1706,  1707,  1708,
	  1709,  1710,  1713,  1714,     0,     0,  1259,     0,  1289,  1287,
	  1288,  1271,  2230,  1286,  1274,  1275,  2235,  2236,  2237,  2238,
	  2239,  2240,  2241,  2242,  2243,  2244,  2245,     0,     0,   117,
	   119,   121,   122,   123,   124,     0,  1267,   163,   569,     0,
	  1183,     0,   934,   935,   401,     0,   402,     0,   139,     0,
	   832,   834,     0,     0,     0,   191,  1323,   193,   196,   197,
	     0,     0,   336,   401,  1230,     0,  1244,     0,   223,     0,
	   212,   214,   215,   216,   217,   218,  1197,   289,     0,   293,
	     0,   301,   163,     0,  1323,  1323,   223,   223,   401,     0,
	   413,     0,     0,  1666,  1322,  1323,  1672,  1673,  2383,  2429,
	   419,   423,   424,   425,   426,   427,   428,   429,  1174,   421,
	   995,   966,   968,   969,   971,   972,   974,   975,   976,    -2,
	   979,    -2,   986,   987,   988,   989,   990,   633,   388,     0,
	  1228,  1020,  1658,  2430,     0,  1041,  1272,   285,  1045,  1054,
	  1077,     0,     0,  1081,     0,     0,  1090,  1091,   281,   240,
	   241,     0,     0,   260,   279,  1096,   929,  1292,  1293,  1294,
	  1101,     0,  1111,  1282,  1223,  1184,  1185,  1187,  1188,     0,
	  1137,  1138,  1195,   468,   471,   472,   522,   401,     0,    -2,
	   797,   820,   821,   822,   798,     0,   806,   807,   814,   815,
	   816,  2430,     0,   512,   818,  2430,   828,     0,  1396,  1397,
	  2083,  2084,  2085,  2086,  2087,  2081,  2080,  1445,  1411,  1414,
	     0,  1425,  2430,  1463,     0,     0,     0,  1529,  2430,  1532,
	  1533,  1534,  1538,  2430,  1542,  2430,  1546,     0,  1559,  2430,
	  1567,  1540,     0,  1569,  1570,  1571,  1572,  1573,     0,  1586,
	     0,  1627,   522,  1625,  1629,  1628,  1720,  1722,  1723,  1725,
	  1728,  1726,  1727,  2051,  1738,  1732,     0,  1760,  1753,  2055,
	  1756,     0,  1811,  1814,  1815,  2430,  2111,  1819,     0,  2079,
	  1825,  1873,  2430,  1829,  1878,  1905,     0,     0,  1912,  1915,
	  1916,     0,  1920,  1923,     0,     0,  1926,  1927,     0,  1944,
	  2101,  2102,  2103,  2104,  2091,  2088,     0,     0,  1946,  1949,
	     0,  1952,  1764,  1957,     0,  1961,     0,  1972,     0,  1975,
	  1976,  1979,     0,  2014,  2014,  2053,     0,  2014,  2014,  2017,
	  2018,  2021,  2430,  2031,  2026,  2029,  2030,     0,  2039,  1717,
	     0,  2044,     0,     0,  2143,     0,  2145,  2200,  2201,  2202,
	  2204,  2383,     0,  2149,  2152,  2430,  2111,     0,     0,  2430,
	     0,     0,  2174,     0,     0,  2185,  2188,     0,     0,     0,
	     0,   145,     0,     0,   153,   154,   933,  1458,   183,     0,
	   195,   205,   211,   287,   291,   335,   397,   223,     0,   800,
	  1466,     0,    -2,    -2,  1472,   411,   434,   470,   484,  1733,
	  1671,     0,     0,   857,   858,   859,   870,   871,   839,   844,
	   843,   845,   846,   847,   848,   849,   926,     0,     0,     0,
	     0,  1244,   330,   852,   854,   565,  1007,  1008,  1017,  1018,
	  1661,     0,  1049,     0,  1072,  1051,  1052,  1053,  1082,  1085,
	  1086,  1087,  1088,    -2,     0,     0,     0,   823,  1408,     0,
	  1440,   488,     0,   487,  1750,     0,  1809,  1816,  2074,  1837,
	  1831,  1832,  1833,  1834,  1835,  1836,  1917,  1837,  1947,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  2063,
	  2430,     0,  2129,  2208,  2175,  2208,   863,   867,   861,     0,
	    18,  2247,  2252,  2256,     0,  2264,     0,  2259,  2260,  2261,
	  2262,     0,  2267,  2265,  2266,  2395,     0,  2409,     0,  2425,
	  2268,  2269,  2273,  2274,  2278,  2279,  2280,     0,  2410,     0,
	     0,  2281,     0,  2393,  1603,  1609,  1607,  1608,  1616,  1614,
	  1618,  1309,  1309,     0,  1615,  2438,  2444,  2441,     0,  2435,
	  2437,  2452,     0,     0,  1182,   480,  1717,     0,  1647,     0,
	  2430,  2430,  1653,  2253,   830,   106,     0,   140,   142,  1208,
	     0,     0,  1196,  1213,   108,     0,  1213,     0,   113,   114,
	   115,   120,  1324,  1325,  1327,  1329,  1331,  1332,  1333,  1334,
	  1335,  1336,  1337,  1338,  1339,  1340,  1341,  1305,     0,  1248,
	   125,   162,   164,     0,     0,     0,     0,     0,     0,     0,
	   173,   174,   175,   176,   177,   178,   179,  1199,  1256,  1251,
	  1226,  1231,  1233,  1206,  1210,  1247,  1237,  1238,  1235,  1236,
	  1239,     0,  1212,     0,   127,     0,     0,     0,   136,   143,
	   937,   158,     0,  1229,   182,     0,     0,     0,     0,  1209,
	     0,   556,   206,  1323,     0,     0,     0,   219,   220,   221,
	   290,   295,   297,   298,   299,     0,   292,   294,   305,   160,
	     0,  1221,   392,   395,     0,     0,     0,     0,   404,    -2,
	     0,     0,  1598,  2442,     0,  1600,  1668,  2434,     0,     0,
	     0,   967,   983,     0,     0,     0,   985,   980,  1291,     0,
	   223,     0,  1016,     0,  1663,  1664,  1665,     0,     0,  1323,
	  1323,  1046,  1047,     0,     0,     0,     0,   921,   902,   903,
	   906,   908,   909,   910,   911,   912,   914,   915,     0,     0,
	  1093,     0,     0,     0,     0,   263,   265,   267,   268,     0,
	     0,  1098,  1139,  1103,     0,  1105,     0,  1120,  1109,     0,
	     0,  1114,  1115,  1116,     0,  1189,     0,     0,  1193,     0,
	     0,  1222,     0,     0,   554,     0,     0,     0,   511,   513,
	  1309,  1309,  2383,  2442,   826,  2442,  1394,     0,  1399,  1407,
	     0,  1446,  1415,  1412,  2430,  1423,  1424,  1462,  1459,     0,
	  1522,  1523,  1525,  1527,  1531,  1536,     0,     0,  1551,  1309,
	  1545,  1549,  1548,     0,  1562,  1564,  1565,  1566,     0,  1577,
	     0,  1554,  1555,  1556,  1584,  1588,     0,     0,     0,  1592,
	  1587,   575,   560,  1626,     0,  1634,     0,     0,  1734,  1735,
	  1736,  1737,  1739,  1740,  1738,  1800,  1759,  1761,  1775,     0,
	  1808,     0,  2067,     0,  1309,  2442,  2112,     0,     0,     0,
	  2078,     0,  1843,  1844,  1845,     0,  1309,  1875,  1877,     0,
	  1880,     0,  1909,     0,     0,  2057,     0,     0,  1924,     0,
	     0,  1931,  1932,     0,  1849,  1852,  1853,  1854,  1855,  1856,
	  1857,  1858,  1859,  1860,  1861,  2089,  2090,  1950,     0,  1953,
	     0,  1956,  1787,  1960,     0,  1965,  1967,     0,  1974,  1971,
	  1982,     0,  1985,  1986,    -2,  1987,     0,  2006,     0,  2016,
	  2007,  2054,  2010,  2011,  2024,  2027,  2028,  2034,     0,  2033,
	  2037,     0,  1716,     0,  2048,  2004,  2128,  2430,  2132,     0,
	  2134,     0,  2080,     0,  2139,    -2,     0,  2142,  2223,  2224,
	  2225,  2226,  2092,  2096,  2100,  2082,     0,     0,  2093,  2097,
	  2432,  2433,  2442,     0,  2154,     0,  2442,  2430,     0,  2160,
	  2210,  2211,  2212,  2213,  2214,     0,  2164,     0,     0,  2172,
	     0,     0,  2177,     0,  2228,  2229,     0,  2182,     0,  2206,
	     0,  2190,     0,     0,     0,  2207,  2218,  2219,  2220,  2221,
	  2197,     0,     0,     0,     0,   194,  1323,     0,   799,     0,
	  1323,   853,   896,     0,   850,   925,   927,   928,   851,   879,
	   880,   881,   883,   884,   326,   327,   328,     0,   331,     0,
	  1659,  1598,     0,  1071,     0,   808,   809,   819,     0,  1409,
	     0,   489,   490,   491,   486,  1752,  1745,  1746,  1747,  1748,
	  1749,  1743,  1828,  1929,    -2,    -2,    -2,  1750,    -2,  2012,
	    -2,  2022,  2040,  2041,  2064,    -2,  2150,  2209,  2186,   864,
	  2042,     0,  2383,     0,  2383,    -2,  2383,    -2,  2383,  2383,
	    -2,  2291,  2292,  2318,  2335,  2312,  2293,  2294,  2295,  2296,
	  2297,  2298,  2306,  2307,  2311,  2315,  2332,  2333,  2263,     0,
	  2394,     0,  2411,  2412,  2415,     0,     0,  2270,  2271,  2272,
	  2275,  2276,  2277,  2391,  2417,     0,    -2,    -2,     0,  2392,
	  1605,     0,  1617,  2430,  1620,  1310,     0,  2436,  1314,  1313,
	  1621,     0,  2439,  2383,  2443,  1307,  2453,  2384,  2454,  2385,
	  2386,  2456,     0,  2388,  2383,  2460,  2462,  2455,     0,   508,
	  1622,  1717,     0,  1647,  1624,     0,  1654,  1309,  1290,   141,
	     0,   148,     0,     0,   111,   112,     0,  1316,  1330,  1216,
	     0,   165,   166,   180,     0,     0,     0,  1205,   170,   791,
	     0,     0,  1249,  1232,   171,   172,   568,   571,   129,   390,
	     0,   362,     0,   364,   365,   366,     0,     0,     0,     0,
	   387,   384,     0,     0,   389,     0,   133,     0,   400,     0,
	   936,   938,     0,  1323,   189,     0,     0,   198,  1252,   337,
	     0,   557,   208,     0,   224,   572,   223,   213,   296,   312,
	   304,     0,   307,   332,     0,     0,     0,     0,   342,   572,
	     0,   405,   407,     0,     0,  1593,     0,  1595,  2442,     0,
	     0,   991,     0,     0,   996,   997,    -2,   992,   970,     0,
	     0,     0,   432,     0,     0,     0,  1021,  1022,     0,  1039,
	  1040,  1273,   286,  1043,  1063,     0,     0,  1056,  1058,  1060,
	  1061,  1073,  1079,     0,     0,     0,     0,   904,   905,     0,
	     0,   900,     0,  1092,   244,     0,     0,   282,   283,   284,
	     0,     0,   269,   270,   271,   272,  1242,     0,     0,   280,
	  1095,     0,  1100,  1102,  1140,  1159,  1143,  1144,  1145,  1280,
	  1147,  1148,  1159,  1281,     0,     0,  1263,  1104,  1107,  1155,
	  1156,  1157,  1158,  1175,  1176,  1177,  1178,  1179,  2383,  1180,
	  1181,  1108,     0,     0,     0,     0,  1117,     0,  1186,  1190,
	  1191,  1192,     0,  1254,   521,    -2,   525,     0,   534,   547,
	    -2,    -2,   553,   803,   804,   805,   617,   600,   522,     0,
	   607,   605,     0,  2430,   515,   516,   517,  2450,   817,   827,
	     0,  1400,     0,  1403,  1404,  1405,  1444,  1447,  1448,  2050,
	  1449,     0,  1441,  1442,  1443,  1417,  1416,     0,  1422,  1457,
	     0,     0,  1473,     0,  1478,  1479,     0,     0,  1550,  2430,
	     0,  1547,  1561,  2430,  1576,     0,     0,     0,  1590,     0,
	  2250,  2251,  1591,   577,     0,  1264,   562,     0,  1630,     0,
	     0,  1636,  2442,  1635,  2052,  2442,     0,  1741,  1802,  1762,
	     0,  1766,     0,     0,     0,     0,     0,  1774,    -2,  1758,
	  2066,  2430,     0,  2076,  2077,  2071,  2110,     0,     0,  1823,
	  1827,  2430,  1847,     0,  1879,  1881,  1890,     0,  1885,  1886,
	     0,     0,  2056,     0,  2430,     0,  1925,  1928,     0,     0,
	     0,     0,     0,  1765,  1787,  1777,  1778,  1779,  1780,  1783,
	  1787,     0,  1785,  1786,  1964,  1973,  1983,     0,  1989,     0,
	  2025,     0,  2036,  2032,  1715,  1718,     0,  2047,  2144,  2133,
	     0,     0,     0,     0,  2222,  2430,  2094,  2098,  2095,  2099,
	  2147,  2153,  2430,     0,  2215,     0,  2158,  2430,  2162,  2430,
	     0,     0,  2169,     0,     0,  2176,     0,     0,  2180,     0,
	     0,  2227,  2430,  2189,     0,     0,     0,  2194,     0,     0,
	     0,     0,     0,   338,   399,  1467,  1468,  1469,   837,     0,
	     0,   872,   874,    -2,   878,     0,   329,   566,  1662,  1038,
	   237,   824,   482,   492,   493,   494,   569,    -2,   692,     0,
	   699,     0,     0,    -2,   702,   703,   765,     0,   765,     0,
	   710,   711,   759,   760,   761,   762,     0,   712,   713,   714,
	   715,   716,   717,   718,    -2,   719,   720,   721,   722,   723,
	   724,   725,   726,   727,   728,     0,     0,     0,     0,   783,
	   784,  1309,  1309,     0,   729,     0,  1207,     0,  1257,     0,
	    -2,   779,   780,   789,   790,  1234,  1211,  1878,  1751,  1838,
	  1839,  1840,  1841,  1842,  1962,  2072,  1968,  1977,  1980,  2008,
	  2019,  2045,   862,     0,     0,     0,  2282,  2283,  2374,  2379,
	     0,  2382,  2258,  2407,  2284,  2285,  2372,  2286,  2287,  2288,
	  2289,  2290,     0,  2314,  2316,    -2,     0,     0,  2330,     0,
	  2334,    -2,  2304,  2308,  2309,  2305,  2299,  2300,  2301,  2302,
	  2303,  2390,  2396,  2397,  2398,     0,  2400,  2402,  2404,  2405,
	  2406,  2408,  2413,     0,     0,  2423,  2426,     0,     0,  2424,
	  1606,     0,  1634,  2430,  1619,  1311,  1312,  2383,     0,     0,
	  2457,  2464,     0,  2467,  2468,  2458,  2459,     0,   507,     0,
	     0,  1649,  1623,  1652,  2430,  1656,  1657,     0,     0,     0,
	  1326,  1328,     0,     0,  1344,  1345,  1346,  1347,  1348,  1349,
	  1315,  1241,     0,     0,   167,   168,   169,   792,   793,     0,
	     0,   130,   391,  1491,   360,  1482,     0,  1227,   363,  1517,
	     0,   368,     0,   383,   634,     0,     0,   639,   640,   641,
	   642,   643,  1240,     0,     0,     0,  1218,   569,   385,     0,
	   386,   131,   134,   135,   132,     0,     0,     0,   184,   186,
	     0,     0,     0,     0,   204,   222,     0,     0,   322,     0,
	  1262,   306,   308,   310,  1323,   593,   595,     0,   522,   485,
	   398,     0,   339,     0,   577,     0,     0,   560,   415,   416,
	  1599,  1596,  2442,  1323,  1287,     0,  1001,     0,  1003,  1250,
	   993,   994,   973,     0,     0,   435,   436,     0,   577,   439,
	  1316,  1026,  1025,     0,     0,  1323,     0,  1062,  1055,     0,
	     0,     0,     0,     0,     0,     0,   920,    -2,   923,   924,
	   919,   907,     0,     0,  1094,   235,     0,   242,   243,   273,
	   264,     0,   278,     0,  1159,  1142,  1160,     0,  1151,  1154,
	  1171,  1172,  1173,    -2,  1149,  1150,  1161,  1163,     0,  1162,
	  2451,  1118,  1121,  1124,  1119,  1110,     0,     0,   560,   477,
	     0,     0,     0,  1318,     0,  1317,     0,     0,     0,   583,
	     0,     0,   602,     0,   488,     0,   606,   514,  1398,     0,
	  1406,     0,     0,  1451,     0,  1419,  1418,     0,     0,  1426,
	  1460,  1461,  1465,     0,  1482,  1480,  2231,  2235,  1526,  1528,
	  2430,  1552,     0,  1563,  1578,  1579,  1580,  1581,  1309,  1583,
	  1589,  2249,   583,     0,   576,  1265,  1266,   574,     0,   558,
	   559,    -2,  1643,  1639,  1632,     0,  1633,  1729,  2442,  1799,
	  2065,  1803,     0,  1775,     0,     0,  1787,  1770,  1771,  1773,
	  1781,  1782,  1754,  1757,  2068,  2069,     0,     0,  1822,  1846,
	  1848,     0,  1883,     0,     0,     0,  2430,  2058,  2059,  2061,
	  1309,  2106,  2107,  2108,  2109,  2062,     0,  1933,  1934,  1935,
	  1936,  1937,  1938,  1939,  1940,  1941,  1309,  1943,  1850,     0,
	  1862,  1863,  1864,  1865,  1866,  1867,  1868,  1869,  1870,  1871,
	  1872,  1951,  1954,  1955,  1784,     0,  1789,     0,  1792,  1793,
	  1794,  1795,  1988,     0,     0,  2035,  1719,  2135,  2136,  1309,
	  2137,  2138,     0,  2155,  2216,  2156,     0,  2203,  2165,  2166,
	  2167,     0,     0,     0,  2178,  2179,  2183,  2184,     0,  2191,
	  2217,  2192,     0,   150,   151,   155,     0,   897,   838,   885,
	   886,   887,     0,     0,     0,   882,     0,     0,     0,     0,
	     0,  1201,   693,   694,   695,   696,   697,  1255,  1253,  1219,
	     0,   700,   701,     0,     0,   763,   764,   765,     0,   757,
	     0,   758,     0,   741,     0,   751,   748,     0,   767,     0,
	     0,    -2,    -2,     0,     0,   794,   795,  2430,     0,   778,
	     0,     0,   865,  2254,  2257,  2383,  2373,  2376,  2377,  2378,
	  2381,  2388,  2313,     0,  2321,  2328,     0,  2339,  2329,  2331,
	  2310,  2399,  2401,  2403,  2414,     0,     0,  2418,  2419,  2420,
	   506,  2442,  1612,  2431,     0,   519,  2440,  2445,  2388,  2383,
	  2449,  2465,  2466,  2389,     0,   509,  1648,     0,  1655,     0,
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
	  2232,  1539,  1557,  1558,  1553,  1582,   585,     0,   520,     0,
	  1224,     0,     0,  1638,     0,  2430,  1642,  1637,     0,  1730,
	     0,  1763,  1767,  1768,     0,  1769,     0,  1821,  1882,     0,
	  1891,     0,  1894,  1895,  1896,  1884,  1887,  1888,  1907,     0,
	  1309,  2105,     0,  1942,  1851,  1788,     0,     0,  1995,  1991,
	  2234,  1992,  1993,  1994,  2015,  2141,     0,  2170,  2171,     0,
	  2205,  2430,     0,     0,     0,   873,   875,   877,   495,   496,
	   499,   788,   498,   691,   698,   753,     0,   705,     0,     0,
	     0,   709,     0,     0,  2233,   740,  1215,   742,   751,   744,
	     0,   747,   749,     0,  1214,     0,   768,     0,     0,     0,
	   775,     0,     0,  1309,   730,   733,   732,   785,   787,  1742,
	  2375,  2380,  2387,     0,  2322,  2325,  2326,     0,  2341,  2383,
	  2344,     0,  2346,     0,     0,     0,  2350,  2354,  2358,  2362,
	  2366,  2367,  2368,  2369,  2370,     0,  2427,  1610,  1611,   518,
	     0,  2447,     0,  2388,  2463,  1650,   146,     0,  1350,  1320,
	  1321,     0,  1357,   147,  1342,  1270,     0,     0,     0,  1343,
	  1364,     0,     0,     0,     0,   181,  1492,  1333,  1341,  1495,
	     0,   361,   350,  1482,     0,  1482,     0,  1485,  1497,  1503,
	  1513,  1498,  1505,  1514,  1500,  1506,  1507,  1508,  1509,  1510,
	  1511,  1512,     0,  1501,  1516,     0,     0,   367,     0,   370,
	     0,   375,   635,   636,   638,   685,   560,     0,   689,   690,
	   663,     0,   765,   647,     0,     0,   667,   668,  1203,  1204,
	   666,   672,     0,   670,   671,   656,   679,     0,     0,   683,
	   684,   663,     0,   371,  1323,   960,   961,   962,   940,     0,
	   947,   948,     0,   944,   945,   946,   189,     0,  1316,     0,
	   203,   232,     0,   226,   227,   228,   300,     0,   325,   321,
	     0,   309,   594,   596,     0,   577,     0,   347,   340,   585,
	   409,     0,   440,    -2,    -2,     0,     0,     0,     0,  1309,
	   457,  1309,   459,   460,   461,   462,   463,     0,   438,  1036,
	     0,  1027,  1028,     0,     0,     0,  1323,  1074,     0,  1075,
	   898,   245,     0,     0,     0,   259,   261,     0,     0,   277,
	     0,  1153,  1164,  1165,  1166,  1167,  1168,  1169,  1170,  1122,
	  1123,     0,  1127,  1128,     0,  1130,  1131,  1132,  1133,  1134,
	  1135,     0,  1112,  1113,   478,     0,   479,   543,     0,   537,
	   538,   539,   540,   527,     0,    -2,   621,   620,     0,  1220,
	   584,   591,   592,   618,   579,  1319,   604,   609,   610,   611,
	   569,   614,  1410,     0,     0,  1433,  1431,     0,  1427,     0,
	   500,   578,   573,   561,   563,     0,  1640,  2430,  1641,  1645,
	  1646,  1643,  1801,  1772,     0,  1889,     0,     0,  1878,  1914,
	  2430,  1790,  1791,  1796,  1797,  1798,  1998,     0,     0,  2199,
	     0,   156,   157,   888,     0,   754,     0,   707,   708,     0,
	   756,   743,   745,     0,   752,     0,  1258,     0,     0,     0,
	     0,     0,     0,   736,     0,     0,  2320,     0,     0,  2327,
	  2338,  2342,  2340,     0,  2345,  2347,  2351,  2355,  2359,  2363,
	  2348,  2352,  2356,  2360,  2364,  2349,  2353,  2357,  2361,  2365,
	  2416,  2446,  2388,  2461,  1319,     0,     0,  1302,  1303,  1299,
	  1301,  1300,     0,  1369,  1372,  1392,  1392,  1392,     0,    -2,
	  1371,  1367,  1493,     0,     0,   351,  1482,     0,   353,  1482,
	     0,     0,  1482,     0,   373,     0,   377,   378,   562,   687,
	   644,     0,  1217,   645,     0,     0,     0,     0,     0,     0,
	     0,   681,     0,   648,   677,   932,   954,   949,     0,   187,
	     0,  1323,   202,   234,     0,   229,  1045,   314,   598,   583,
	   345,   582,   437,     0,     0,   451,   452,   453,   454,   455,
	     0,   444,   445,   449,   450,   456,   458,   464,     0,  1015,
	     0,     0,  1029,  1031,  1319,  1037,  1066,  1068,  1069,  1070,
	  1319,   247,   248,   250,     0,     0,   253,   254,   255,   569,
	   249,   262,     0,  1146,  1126,  1129,     0,     0,   526,     0,
	     0,  1260,   530,   599,   631,     0,   587,   588,     0,   581,
	     0,     0,     0,     0,  1435,  1438,  1437,     0,     0,  1430,
	  1482,     0,   481,     0,     0,     0,  1309,  2070,  1892,  1893,
	  1897,  1898,  1899,  1900,  1901,  1902,  1903,  1904,  1910,  2060,
	  1990,     0,  2003,     0,     0,  2161,  2196,   704,   706,   755,
	     0,   750,   769,     0,   773,   774,     0,   360,   796,   731,
	     0,     0,   786,  2323,  2324,     0,  2343,     0,  1351,     0,
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
	   738,   739,   734,     0,  2388,     0,  1368,  1373,     0,  1377,
	  1379,     0,  1261,     0,  1360,     0,   357,  1482,  1482,     0,
	   369,   374,   381,   382,   686,   664,     0,   674,   675,   680,
	     0,     0,   955,     0,     0,     0,   188,   210,     0,     0,
	     0,     0,  1032,   256,     0,     0,     0,   544,     0,   532,
	   623,     0,   589,   590,  1439,  1482,     0,     0,  1644,  2002,
	     0,   770,     0,   766,     0,     0,  2448,     0,     0,     0,
	     0,   657,   658,   659,   660,  1359,     0,  1494,   354,   359,
	  1482,   665,     0,   682,   953,   956,   957,   951,     0,     0,
	   447,   466,     0,   257,     0,     0,   542,     0,   531,     0,
	     0,   586,  1245,  1477,   503,     0,  1997,   772,     0,   735,
	  1352,  1393,     0,  1384,  1386,  1387,   728,  1388,     0,  1383,
	  1361,   356,   676,   233,     0,     0,     0,   275,   545,   533,
	   622,   624,   626,     0,   629,   630,     0,   737,  1382,     0,
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
#line 1145 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* 
		    ** Note that FRS blocks fall through to this rule too, so
		    ** if you add actions here, do it at block_stmt too.
		    */
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 3:
#line 1156 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_func = 0;	/* No current function */
		}
		break;
	case 8:
#line 1169 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1211 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1227 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1256 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1265 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1282 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1296 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1319 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1330 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1340 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1389 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1402 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1412 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
                    /* Accept either EXEC SQL | 4GL INCLUDE */

                    if ((dml->dm_exec != (DML_EXEC|DML__SQL)) &&
                        (dml->dm_exec != (DML_EXEC|DML__4GL)))
                        er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"),
                                                            ERx("INCLUDE") );
		}
		break;
	case 21:
#line 1422 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1449 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Eat the name till right angle bracket */
		    sc_eat(id_add, SC_STRIP, ERx(">"), '<', '>');
		    yyval.s = str_add(STRNULL, id_getname());
		    id_free();
		    gr->gr_adjective = TRUE;
		}
		break;
	case 23:
#line 1458 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 24:
#line 1462 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 92:
#line 1558 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 93:
#line 1562 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 94:
#line 1566 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 95:
#line 1570 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 96:
#line 1574 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 97:
#line 1578 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 98:
#line 1582 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 99:
#line 1586 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 100:
#line 1590 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 101:
#line 1594 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 102:
#line 1598 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 103:
#line 1602 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 104:
#line 1606 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 106:
#line 1624 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1634 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 108:
#line 1646 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1656 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 111:
#line 1662 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 112:
#line 1668 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1686 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1696 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 117:
#line 1708 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1718 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 122:
#line 1727 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{ db_key(yypvt[-0].s);}
		break;
	case 123:
#line 1729 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{ db_key(yypvt[-0].s);}
		break;
	case 124:
#line 1731 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{ db_key(yypvt[-0].s);}
		break;
	case 125:
#line 1738 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
				    EQ_WARN, 1, ERx("ALTER SEQUENCE"));
		}
		break;
	case 126:
#line 1745 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 127:
#line 1758 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1768 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 134:
#line 1779 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( ERx("cascade") );
		}
		break;
	case 135:
#line 1783 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( ERx("restrict") );
		}
		break;
	case 136:
#line 1798 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1808 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 143:
#line 1829 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1839 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
	    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
	}
		break;
	case 145:
#line 1844 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
	    if (!ESQ_EQSTR(yypvt[-0].s, ERx("profile")))
		er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
				    ERx("ALTER DEFAULT"), ERx("PROFILE") );
	    gr_mechanism( GR_EQSTMT, GR_sSQL, "alter default profile ", NULL );
	}
		break;
	case 148:
#line 1857 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("privileges")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					    ERx("ALTER USER/PROFILE"), ERx("PRIVILEGES") );
		    db_key( ERx("privileges") );
		}
		break;
	case 149:
#line 1871 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1884 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
			gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 153:
#line 1889 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 154:
#line 1894 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 158:
#line 1909 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1919 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 160:
#line 1929 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
				    EQ_WARN, 1, ERx("CREATE SEQUENCE"));
		}
		break;
	case 161:
#line 1936 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 182:
#line 1970 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 1980 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		}
		break;
	case 185:
#line 1993 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2019 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2049 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("CREATE INDEX"));
		}
		break;
	case 194:
#line 2056 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    STprintf( esq->sbuf, ERx("%s %s"), yypvt[-2].s, yypvt[-1].s );
		    gr_mechanism( GR_EQSTMT, GR_sSQL, esq->sbuf, yypvt[-0].s );
		}
		break;
	case 195:
#line 2061 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		}
		break;
	case 205:
#line 2088 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2108 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2127 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2149 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2160 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 213:
#line 2164 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 219:
#line 2175 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 220:
#line 2179 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 221:
#line 2183 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 226:
#line 2196 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 227:
#line 2201 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 228:
#line 2206 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-1].s );
		    db_sconst( yypvt[-0].s );
		}
		break;
	case 229:
#line 2212 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 231:
#line 2220 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2233 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-3].s );
		    db_key( yypvt[-2].s );
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 236:
#line 2257 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2270 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2292 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 260:
#line 2321 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2336 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = CSR_STATIC;
		}
		break;
	case 276:
#line 2359 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2369 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2387 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 280:
#line 2392 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 282:
#line 2398 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 285:
#line 2423 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2436 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2446 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2465 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
 		    if (eq->eq_flags & EQ_COMMON)
 			er_write( E_EQ0504_OPEN_WARN,
 				EQ_WARN, 1, ERx("CREATE SCHEMA") );
		    esq->flag |= ESQ_CRE_SCHEMA;
		}
		break;
	case 293:
#line 2476 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		    if (gr->gr_func == tSET)
		    	erec_setup( 1 );
		}
		break;
	case 303:
#line 2508 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2525 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 310:
#line 2533 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2554 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 316:
#line 2558 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 317:
#line 2562 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 318:
#line 2566 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 319:
#line 2570 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 320:
#line 2574 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 325:
#line 2585 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{db_key( yypvt[-0].s);}
		break;
	case 326:
#line 2589 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2599 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2609 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2618 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2635 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{ 
			db_key(yypvt[-0].s);
		}
		break;
	case 331:
#line 2640 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
			db_key(yypvt[-1].s);
			db_key(yypvt[-0].s);
		}
		break;
	case 333:
#line 2652 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
				EQ_WARN, 1, ERx("CREATE SYNONYM"));
		}
		break;
	case 335:
#line 2666 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2684 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				    2, ERx("location"), ERx("CREATE TABLE") );
		}
		break;
	case 338:
#line 2700 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* With clause is non-FIPS compliant */
		    if (yypvt[-0].i == 1 && eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
					2, ERx("WITH"), ERx("CREATE TABLE") );
		}
		break;
	case 339:
#line 2708 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2767 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2, 
			        ERx("AS SELECT"), ERx("CREATE TABLE") );
		}
		break;
	case 341:
#line 2774 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2, 
			        ERx("AS SELECT"), ERx("CREATE TABLE") );
		}
		break;
	case 342:
#line 2805 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;		/* Info on dec_nullfmt */
		}
		break;
	case 343:
#line 2809 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-2].i | yypvt[-0].i;		/* Info on dec_nullfmt */
		}
		break;
	case 344:
#line 2814 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 345:
#line 2818 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-2].i | yypvt[-0].i;
		}
		break;
	case 346:
#line 2823 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 347:
#line 2827 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-1].i;
		}
		break;
	case 348:
#line 2832 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-1].i;
}
		break;
	case 349:
#line 2837 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2858 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 351:
#line 2862 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 352:
#line 2866 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 353:
#line 2871 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 354:
#line 2875 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 355:
#line 2879 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 356:
#line 2883 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 357:
#line 2888 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 358:
#line 2892 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2907 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 362:
#line 2915 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    esq->flag |= ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 363:
#line 2919 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    esq->flag |= ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 367:
#line 2928 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 375:
#line 2945 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 377:
#line 2956 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 378:
#line 2964 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 379:
#line 2973 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 380:
#line 2981 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 381:
#line 2989 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 2998 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 3016 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 385:
#line 3024 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 3034 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 3044 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 388:
#line 3053 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 389:
#line 3062 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 393:
#line 3083 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 3099 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* UNION views are non-FIPS */
		    if (eq->eq_flags & EQ_FIPS && yypvt[-1].i == 1)
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2, 
			        ERx("UNION"), ERx("CREATE VIEW") );
		}
		break;
	case 396:
#line 3115 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 3126 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 3152 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 3166 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    STpolycat(3, yypvt[-2].s, yypvt[-1].s, yypvt[-0].s, session_temp);
		    yyval.s = session_temp;
		}
		break;
	case 401:
#line 3171 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 402:
#line 3176 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 403:
#line 3181 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-2].s );
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 404:
#line 3200 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 3211 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 3225 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 3239 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 3251 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_flag |= GR_NOSYNC;	/* no error output yet */
		    yyval.i = 0;
		}
		break;
	case 411:
#line 3256 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_flag |= GR_NOSYNC;	/* no error output yet */
		    if (eq->eq_flags & EQ_FIPS) 
			    er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1, 
			        ERx("REPEATED DELETE") );
		    yyval.i = 1;
		}
		break;
	case 412:
#line 3266 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    er_write( E_EQ0070_grMISSWORD, EQ_ERROR, 2, ERx("FROM"),
								ERx("DELETE") );
		    yyval.s = ERx("from");
		}
		break;
	case 414:
#line 3274 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 415:
#line 3280 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    STprintf( esq->sbuf, ERx("%s.%s"), yypvt[-2].s, yypvt[-0].s );
		    yyval.s = esq->sbuf;
		}
		break;
	case 416:
#line 3286 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    STprintf( esq->sbuf, ERx("%s.%s"), yypvt[-2].s, yypvt[-0].s );
		    yyval.s = esq->sbuf;
		}
		break;
	case 418:
#line 3293 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if ( gr->gr_flag & GR_REPEAT )
		    {
			db_op( ERx("...SYNTAX ERROR") );
			er_write( E_EQ0378_repWHERE, EQ_ERROR, 0 );
		    }
		}
		break;
	case 419:
#line 3307 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 3317 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 421:
#line 3326 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 3336 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 423:
#line 3341 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 424:
#line 3345 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 425:
#line 3349 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 426:
#line 3353 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 427:
#line 3357 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 430:
#line 3376 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 3406 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 3434 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 434:
#line 3438 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-1].s, yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS) 
			    er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1, 
			        ERx("REPEATED INSERT") );
		}
		break;
	case 435:
#line 3446 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 439:
#line 3456 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		    erec_setup( 1 );
		}
		break;
	case 442:
#line 3473 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (yypvt[-0].i > 1)
			er_write( E_EQ0132_sqILLSTRUCTEXPR, EQ_ERROR, 0 );
		}
		break;
	case 446:
#line 3485 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		  /* return the max num of struct elems on one side */
		    yyval.i = (yypvt[-2].i > yypvt[-0].i) ? yypvt[-2].i : yypvt[-0].i;
		}
		break;
	case 447:
#line 3490 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
 		  /* return the max num of struct elems on one side */
 		    yyval.i = (yypvt[-4].i > yypvt[-1].i) ? yypvt[-4].i : yypvt[-1].i;
 		}
		break;
	case 448:
#line 3495 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-1].i;	/* Might be a structure */
		}
		break;
	case 449:
#line 3499 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;	/* Might be a structure */
		}
		break;
	case 450:
#line 3503 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;	/* Might be a structure */
		}
		break;
	case 456:
#line 3515 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = erec_vars();
		    erec_use( IISETDOM, gr->gr_flag & GR_REPEAT, ERx(",") );
		    erec_setup( 1 );		/* end of variable */
		    if ( (gr->gr_type == T_STRUCT) && (eq->eq_flags & EQ_FIPS) )
			er_write( E_EQ050D_FIPS_VAR, EQ_WARN, 0 );
		}
		break;
	case 457:
#line 3523 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;		/* Not a structure */
		}
		break;
	case 458:
#line 3527 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    erec_use( IISETDOM, gr->gr_flag & GR_REPEAT, ERx(",") );
		    erec_setup( 1 );
		}
		break;
	case 460:
#line 3535 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (   !ESQ_EQSTR(yypvt[-0].s, ERx("dbmsinfo")) 
		        && eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0510_OPEN_FUNC, EQ_WARN, 1, yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS) 
			er_write( E_EQ0509_FIPS_FUNC, EQ_WARN, 1, yypvt[-0].s );
		}
		break;
	case 462:
#line 3544 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    er_write( E_EQ0129_sqVARCOLON, EQ_ERROR, 0 );
		    YYERROR;
		}
		break;
	case 464:
#line 3551 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-1].s;
		}
		break;
	case 465:
#line 3555 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 466:
#line 3559 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-4].s;
		}
		break;
	case 467:
#line 3564 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-6].s;
		}
		break;
	case 469:
#line 3582 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    gr->gr_func = tUPDATE;
		}
		break;
	case 470:
#line 3587 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-1].s, yypvt[-0].s );
		    gr->gr_func = tUPDATE;
		    if (eq->eq_flags & EQ_FIPS) 
			    er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1, 
			        ERx("REPEATED UPDATE") );
		}
		break;
	case 473:
#line 3601 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 3638 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 3653 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    esq->flag |= ESQ_CURSOR;
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 480:
#line 3721 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 3765 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* don't set until after call to gr_mechanism */ 
		}
		break;
	case 483:
#line 3770 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 3782 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-1].s, yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS) 
			    er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1, 
			        ERx("REPEATED SELECT") );
		}
		break;
	case 487:
#line 3793 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 490:
#line 3800 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* DISTINCT makes a cursor Read-Only */
		    if (ESQ_DCLCSR(esq))
			ecs_csrset( NULL, ECS_UNIQUE );
		}
		break;
	case 491:
#line 3807 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 492:
#line 3812 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s ); /* db_key 'cause we want spaces around it */
		}
		break;
	case 496:
#line 3823 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2,
			    ERx("<= result_column_name>"), ERx("SELECT") );
		}
		break;
	case 497:
#line 3829 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 3862 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2,
			    ERx("<result_column_name AS>"), ERx("SELECT") );
		}
		break;
	case 499:
#line 3868 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 3900 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 3923 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 508:
#line 3929 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 3987 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    lbl_exit_block();
		}
		break;
	case 510:
#line 4007 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* error must have INTO clause before FROM clause */
		    er_write( E_EQ0070_grMISSWORD, EQ_ERROR, 2, ERx("INTO"),
							ERx("SELECT/FETCH") );
		    YYERROR;
		}
		break;
	case 511:
#line 4014 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    erec_setup( 0 );	/* Done with list */
		}
		break;
	case 512:
#line 4019 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    erec_setup( 1 );	/* Initialize struct indicators stuff */
		}
		break;
	case 515:
#line 4027 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    erec_setup( 0 );	/* Done with one element */
		}
		break;
	case 516:
#line 4032 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if ( (gr->gr_type == T_STRUCT) && (eq->eq_flags & EQ_FIPS) )
			er_write( E_EQ050D_FIPS_VAR, EQ_WARN, 0 );
		}
		break;
	case 518:
#line 4039 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 4063 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    sc_eat(id_add,SC_STRIP|SC_NEST|SC_SEEN,ERx(")"), '(', ')');
		}
		break;
	case 520:
#line 4078 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 4118 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 4131 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = CSR_NO_FROM;
		}
		break;
	case 523:
#line 4136 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 524:
#line 4140 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-2].i | yypvt[-0].i;
		}
		break;
	case 525:
#line 4145 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 526:
#line 4149 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = FRM_JOIN;
		}
		break;
	case 527:
#line 4153 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-2].i;
		}
		break;
	case 529:
#line 4159 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-1].s;
		}
		break;
	case 534:
#line 4170 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = FRM_OK;
		}
		break;
	case 535:
#line 4174 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-1].i;
		}
		break;
	case 536:
#line 4179 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 537:
#line 4183 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 538:
#line 4188 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 539:
#line 4193 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 540:
#line 4198 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 546:
#line 4211 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_addtab( NULL, yypvt[-0].s );
		    yyval.s = (char *)0;
		}
		break;
	case 547:
#line 4217 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_addtab( NULL, yypvt[-2].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 548:
#line 4223 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    char tbuf[100];     /* buffer to hold owner.tablename */

		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_addtab( NULL, STpolycat(3,yypvt[-2].s,yypvt[-1].s,yypvt[-0].s,tbuf) );
		    yyval.s = (char *)0;
		}
		break;
	case 549:
#line 4231 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    char tbuf[100];     /* buffer to hold owner.tablename */
		
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
		        ecs_addtab( NULL, STpolycat(3,yypvt[-4].s,yypvt[-3].s,yypvt[-2].s,tbuf) );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 550:
#line 4240 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    char tbuf[100];     /* buffer to hold owner.tablename */

		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_addtab( NULL, STpolycat(3,yypvt[-2].s,yypvt[-1].s,yypvt[-0].s,tbuf) );
		    yyval.s = (char *)0;
		}
		break;
	case 551:
#line 4249 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    char tbuf[100];     /* buffer to hold owner.tablename */
		
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
		        ecs_addtab( NULL, STpolycat(3,yypvt[-4].s,yypvt[-3].s,yypvt[-2].s,tbuf) );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 552:
#line 4257 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_addtab( NULL, yypvt[-0].s );
		    yyval.s = (char *)0;
		}
		break;
	case 553:
#line 4263 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
		        ecs_addtab( NULL, yypvt[-2].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 558:
#line 4283 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = CSR_STATIC;
		}
		break;
	case 559:
#line 4287 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 4296 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = CSR_NO_WHERE;
		}
		break;
	case 561:
#line 4302 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* GROUP BY makes a cursor Read-Only */
		    if (ESQ_DCLCSR(esq))
			ecs_csrset( NULL, ECS_GROUP );
		    yyval.i = 1;
		}
		break;
	case 562:
#line 4309 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 563:
#line 4314 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 564:
#line 4318 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 565:
#line 4323 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 566:
#line 4327 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 567:
#line 4333 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 568:
#line 4338 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 569:
#line 4342 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 570:
#line 4347 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 571:
#line 4352 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 572:
#line 4357 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 4368 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* HAVING makes a cursor Read-Only */
		    if (ESQ_DCLCSR(esq))
			ecs_csrset( NULL, ECS_HAVING );
		    yyval.i = 1;
		}
		break;
	case 574:
#line 4375 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 587:
#line 4401 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
	if (!ESQ_EQSTR(yypvt[-0].s, ERx("first")))
	    er_write( E_EQ0244_yySYNWRD, EQ_ERROR, 1, yypvt[-0].s );
    }
		break;
	case 588:
#line 4406 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
	db_key( yypvt[-0].s );
    }
		break;
	case 589:
#line 4411 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
	db_key( yypvt[-0].s );
    }
		break;
	case 590:
#line 4415 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
	db_key( yypvt[-0].s );
    }
		break;
	case 593:
#line 4428 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;	/* UNION info for CREATE VIEW rule */
		}
		break;
	case 594:
#line 4432 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 1;		/* UNION info for CREATE VIEW rule */
		}
		break;
	case 595:
#line 4437 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;		/* UNION info used by CREATE VIEW */
		}
		break;
	case 596:
#line 4441 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-2].i;	/* UNION info used by CREATE VIEW */
		}
		break;
	case 599:
#line 4468 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 4493 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 601:
#line 4497 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (yypvt[-2].i == CSR_NO_FROM || yypvt[-0].i == CSR_NO_FROM)
			yyval.i = CSR_NO_FROM;
		    else
			yyval.i = yypvt[-2].i | yypvt[-0].i; /* MRW: probably just return $3.i */
		    ecs_csrset( NULL, ECS_UNION );
		}
		break;
	case 602:
#line 4513 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 603:
#line 4517 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-1].i;	/* MRW: probably can just return 0 */
		}
		break;
	case 605:
#line 4524 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 606:
#line 4528 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 4543 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s ); /* db_key 'cause we want spaces around it */
		    _VOID_ ecs_colupd( NULL, ERx(""), ECS_ADD|ECS_ISWILD );
		}
		break;
	case 613:
#line 4561 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 4576 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 4618 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 4633 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 4648 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    ecs_csrset( NULL, ECS_SORTED );
		}
		break;
	case 621:
#line 4657 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = CSR_STATIC;
		}
		break;
	case 622:
#line 4661 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 4679 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 4713 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    ecs_addtab( NULL, yypvt[-1].s );
		}
		break;
	case 629:
#line 4718 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 4757 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    er_write( E_EQ0411_csUPDALL, EQ_ERROR, 1, ecs_namecsr() );
		}
		break;
	case 631:
#line 4762 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = ECS_UPDATE;
		}
		break;
	case 632:
#line 4766 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 4780 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 647:
#line 4808 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 4819 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 4831 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 650:
#line 4835 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 651:
#line 4839 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 652:
#line 4843 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 4854 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 654:
#line 4858 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 655:
#line 4862 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 656:
#line 4866 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 657:
#line 4873 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 658:
#line 4877 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 659:
#line 4881 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 660:
#line 4885 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 662:
#line 4904 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 4960 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 685:
#line 4966 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-1].i;		/* bug #58171 */
		}
		break;
	case 686:
#line 4972 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 5007 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 692:
#line 5011 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 698:
#line 5023 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 699:
#line 5027 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 700:
#line 5031 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 701:
#line 5035 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 702:
#line 5041 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = ERx("");
		    /* functions make a cursor Read-Only */
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_csrset( NULL, ECS_FUNCTION );
		}
		break;
	case 703:
#line 5048 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 710:
#line 5066 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 711:
#line 5070 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 712:
#line 5076 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = NULL;
		}
		break;
	case 713:
#line 5080 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 714:
#line 5084 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 5093 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 716:
#line 5097 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 717:
#line 5101 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 718:
#line 5105 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 721:
#line 5112 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_hexconst( yypvt[-0].s );
		}
		break;
	case 722:
#line 5116 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_uconst( yypvt[-0].s );
		}
		break;
	case 723:
#line 5120 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 724:
#line 5124 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 726:
#line 5129 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 728:
#line 5134 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 729:
#line 5139 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_dtconst( yypvt[-0].s );
		}
		break;
	case 732:
#line 5147 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 5168 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 5204 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0509_FIPS_FUNC, EQ_WARN, 1, ERx("ANY"));
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0510_OPEN_FUNC, EQ_WARN, 1, ERx("ANY"));
		}
		break;
	case 757:
#line 5212 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-1].s );
		    db_op( yypvt[-0].s );
		}
		break;
	case 758:
#line 5218 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-1].s );
		    db_op( yypvt[-0].s );
		}
		break;
	case 759:
#line 5224 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 760:
#line 5228 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 761:
#line 5232 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 762:
#line 5236 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 763:
#line 5242 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 764:
#line 5246 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 767:
#line 5256 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-1].s;
		}
		break;
	case 768:
#line 5260 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 769:
#line 5264 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-4].s;
		}
		break;
	case 770:
#line 5269 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-6].s;
		}
		break;
	case 771:
#line 5273 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-5].s;
		}
		break;
	case 772:
#line 5277 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-7].s;
		}
		break;
	case 773:
#line 5281 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-4].s, ERx("position")))
			er_write( E_EQ006A_grxBADWORD, EQ_ERROR, 2, yypvt[-4].s,
				ERx("query"));
		    yyval.s = yypvt[-4].s;
		}
		break;
	case 774:
#line 5288 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-4].s, ERx("trim")))
			er_write( E_EQ006A_grxBADWORD, EQ_ERROR, 2, yypvt[-4].s,
				ERx("query"));
		    yyval.s = yypvt[-4].s;
		}
		break;
	case 778:
#line 5300 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-1].s;
		}
		break;
	case 779:
#line 5305 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 780:
#line 5310 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 781:
#line 5315 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 782:
#line 5320 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 786:
#line 5331 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 787:
#line 5335 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 794:
#line 5362 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 5374 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 5393 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 5431 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-1].s, yypvt[-0].s );
		    esq_init();
		    ecs_close( yypvt[-0].s, gr->gr_sym != (SYM *)0 );
		}
		break;
	case 798:
#line 5448 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 5463 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 5476 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 801:
#line 5490 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 802:
#line 5494 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = ECS_KEYSET;
                }
		break;
	case 803:
#line 5499 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    ecs_csrset( NULL, ECS_DYNAMIC );
		}
		break;
	case 804:
#line 5503 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    ecs_csrset( NULL, ECS_DYNAMIC );
		}
		break;
	case 805:
#line 5507 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 5543 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    ecs_csrset( NULL, ECS_ERR );
		}
		break;
	case 807:
#line 5582 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 5592 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 5606 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
			    yyval.fet.intv1 = 0;
			}
		break;
	case 811:
#line 5685 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
			    yyval.i = 0;
			}
		break;
	case 812:
#line 5690 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
			    CVan(yypvt[-0].s, &yyval.i);
			    yyval.i = -yyval.i;
			}
		break;
	case 813:
#line 5695 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
			    CVan(yypvt[-0].s, &yyval.i);
			}
		break;
	case 814:
#line 5700 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    erec_use( IICSGET, FALSE, NULL );
		    yyval.i = 0;
		}
		break;
	case 815:
#line 5705 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 816:
#line 5709 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 817:
#line 5714 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 5775 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 5795 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 5818 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 821:
#line 5823 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 822:
#line 5828 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 5843 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = NULL;
		}
		break;
	case 824:
#line 5847 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 5858 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 826:
#line 5862 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 827:
#line 5866 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IISQDAIN );
		    yyval.i = 1;
		}
		break;
	case 828:
#line 5874 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 5915 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 5932 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 5951 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 5963 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 834:
#line 5968 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 5977 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = NULL;
		}
		break;
	case 836:
#line 6006 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1,
				ERx("DROP") );
		}
		break;
	case 837:
#line 6013 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 6054 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 856:
#line 6059 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = (char *)0;
		}
		break;
	case 857:
#line 6063 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 858:
#line 6068 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 859:
#line 6073 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 860:
#line 6079 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("DROP LINK"));
		}
		break;
	case 861:
#line 6087 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( IIPROC_DROP );

		    /* use first name encountered. */
		    arg_str_add( ARG_CHAR, yypvt[-0].s);
		    gen_call( IIPROCINIT );
		}
		break;
	case 863:
#line 6097 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s);
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("DROP PROCEDURE"));
		}
		break;
	case 866:
#line 6110 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
					EQ_WARN, 1, ERx("DROP ROLE"));
		}
		break;
	case 867:
#line 6118 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP GROUP") );
		}
		break;
	case 868:
#line 6126 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
					EQ_WARN, 1, ERx("DROP DBEVENT"));
		}
		break;
	case 869:
#line 6134 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
					EQ_WARN, 1, ERx("DROP SEQUENCE"));
		}
		break;
	case 870:
#line 6142 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("DROP INTEGRITY"));
		}
		break;
	case 871:
#line 6149 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("DROP PERMIT"));
		}
		break;
	case 872:
#line 6157 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 874:
#line 6162 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 876:
#line 6167 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 889:
#line 6191 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP RULE") );
		}
		break;
	case 890:
#line 6199 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP DOMAIN") );
		}
		break;
	case 891:
#line 6207 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP LOCATION") );
		}
		break;
	case 892:
#line 6215 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP USER") );
		}
		break;
	case 893:
#line 6223 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP PROFILE") );
		}
		break;
	case 894:
#line 6231 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP SECURITY_ALARM") );
		}
		break;
	case 895:
#line 6239 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 899:
#line 6267 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 6279 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 6295 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("privileges")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					    ERx("REVOKE"), ERx("PRIVILEGES") );
		    db_key( ERx("privileges") );
		}
		break;
	case 918:
#line 6317 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 6332 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 6349 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 924:
#line 6353 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 927:
#line 6361 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 928:
#line 6365 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 930:
#line 6377 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 6393 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 6422 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
				EQ_WARN, 1, ERx("COPY TABLE") );
		}
		break;
	case 950:
#line 6451 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_op( yypvt[-2].s );
		    db_key( yypvt[-1].s );
		    db_op( yypvt[-0].s );
		}
		break;
	case 951:
#line 6457 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_op( yypvt[-4].s );
		    db_key( yypvt[-3].s );
		    db_op( yypvt[-2].s );
		    db_key( yypvt[-1].s );
		    db_op( yypvt[-0].s );
		}
		break;
	case 958:
#line 6474 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 959:
#line 6478 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 961:
#line 6484 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 6497 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 6520 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 6562 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 6575 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_FIPS) 
			er_write( E_EQ0507_FIPS_NOTOP, EQ_WARN,
				2, ERx("PRIVILEGES"), ERx("GRANT") );
		}
		break;
	case 973:
#line 6589 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, ERx("INSERT"), ERx("GRANT") );
		    }
		}
		break;
	case 975:
#line 6598 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, ERx("EXECUTE"), ERx("GRANT") );
		    }
		}
		break;
	case 976:
#line 6606 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, ERx("REGISTER"), ERx("GRANT") );
		    }
		}
		break;
	case 979:
#line 6616 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, yypvt[-0].s, ERx("GRANT") );
		    }
		}
		break;
	case 980:
#line 6624 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, yypvt[-1].s, ERx("GRANT") );
		    }
		}
		break;
	case 983:
#line 6636 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("privileges")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					    ERx("GRANT"), ERx("PRIVILEGES") );
		    db_key( ERx("privileges") );
		}
		break;
	case 985:
#line 6646 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
                    db_key( yypvt[-0].s );
                }
		break;
	case 986:
#line 6651 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 987:
#line 6656 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 988:
#line 6661 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 989:
#line 6666 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 990:
#line 6671 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 991:
#line 6676 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 6685 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 6695 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		}
		break;
	case 994:
#line 6698 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 6717 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 997:
#line 6722 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 998:
#line 6727 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 999:
#line 6733 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 6744 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 6756 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* TO GROUP is non-FIPS */
		    if (eq->eq_flags & EQ_FIPS && gr->gr_func != tREVOKE)
			er_write( E_EQ0506_FIPS_CLAUSE,
				EQ_WARN, 2, ERx("TO GROUP"), ERx("GRANT") );
		}
		break;
	case 1003:
#line 6763 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 6780 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 1009:
#line 6790 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 1010:
#line 6795 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 1014:
#line 6804 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 1017:
#line 6821 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 6845 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1037:
#line 6877 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 6895 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-3].s, NULL );
		    esq_init();
		}
		break;
	case 1039:
#line 6901 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_INT, yypvt[-0].s );
		}
		break;
	case 1040:
#line 6905 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 6934 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1, yypvt[-1].s );
		}
		break;
	case 1042:
#line 6939 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 1043:
#line 6949 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 6959 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 1047:
#line 6966 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 7012 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 7023 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = (char *)0;
		}
		break;
	case 1051:
#line 7027 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1052:
#line 7031 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1053:
#line 7035 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1055:
#line 7041 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 7057 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1057:
#line 7061 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-2].i | yypvt[-0].i;
		}
		break;
	case 1058:
#line 7066 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1059:
#line 7070 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-2].i;
		}
		break;
	case 1061:
#line 7076 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1062:
#line 7081 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 7110 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				1, ERx("REGISTER INDEX") );
		}
		break;
	case 1071:
#line 7117 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    STprintf( esq->sbuf, ERx("%s %s"), yypvt[-2].s, yypvt[-1].s );
		    gr_mechanism( GR_EQSTMT, GR_sSQL, esq->sbuf, yypvt[-0].s );
		}
		break;
	case 1072:
#line 7122 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		}
		break;
	case 1078:
#line 7136 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 7154 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 7170 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("REMOVE"));
		}
		break;
	case 1082:
#line 7177 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (yypvt[-0].s != (char *)0)
			gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		    else
			gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, NULL );
		}
		break;
	case 1083:
#line 7184 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("REMOVE DBEVENT"));
		}
		break;
	case 1084:
#line 7192 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = (char *)0;
		}
		break;
	case 1085:
#line 7196 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1086:
#line 7200 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1087:
#line 7204 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1088:
#line 7208 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1089:
#line 7218 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 7229 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 7243 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 7255 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 7282 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 7293 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1100:
#line 7300 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1101:
#line 7343 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 7354 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 7370 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 7379 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 7388 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
                    {
                        er_write( E_EQ0504_OPEN_WARN,
                                        EQ_WARN, 1, ERx("SET SESSION"));
                    }
                }
		break;
	case 1106:
#line 7398 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 7445 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("privileges")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					    ERx("SET SESSION"), ERx("PRIVILEGES") );
		    db_key( ERx("privileges") );
		}
		break;
	case 1126:
#line 7467 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 1131:
#line 7476 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
			db_key (yypvt[-0].s);
		}
		break;
	case 1132:
#line 7480 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
			db_key (yypvt[-0].s);
		}
		break;
	case 1133:
#line 7484 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
			db_key (yypvt[-0].s);
		}
		break;
	case 1134:
#line 7488 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
			db_key (yypvt[-0].s);
		}
		break;
	case 1135:
#line 7492 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 7509 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1138:
#line 7513 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1146:
#line 7526 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Expecting SET WORK LOCATIONS command */		
		    if (!ESQ_EQSTR(yypvt[-4].s, ERx("locations")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-4].s,
					ERx("SET WORK"), ERx("LOCATIONS") );
		}
		break;
	case 1147:
#line 7533 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{ 
		    db_sconst( yypvt[-0].s );
		}
		break;
	case 1148:
#line 7537 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{ 
		    db_key( yypvt[-0].s );
		}
		break;
	case 1158:
#line 7553 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    erec_use( IISETDOM, gr->gr_flag & GR_REPEAT, ERx(",") );
		    erec_setup( 1 );
		}
		break;
	case 1168:
#line 7572 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1169:
#line 7576 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1174:
#line 7587 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
			db_key( yypvt[-0].s );
		}
		break;
	case 1176:
#line 7595 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 7604 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 7613 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 7622 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
                    /* Don't print if in DECLARE TABLE statement */
                    if ((esq->flag & ESQ_NOPRT) == 0)
                        db_key( yypvt[-0].s );
		}
		break;
	case 1180:
#line 7629 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
                    if (eq->eq_flags & EQ_FIPS)
                        er_write( E_EQ050F_FIPS_USE, EQ_WARN, 1, yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0510_OPEN_FUNC, EQ_WARN, 1, yypvt[-0].s );
		}
		break;
	case 1182:
#line 7643 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 7663 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s ); 
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1190:
#line 7691 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key(yypvt[-1].s);
		    db_key(yypvt[-0].s);
		}
		break;
	case 1191:
#line 7696 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key(yypvt[-1].s);
		    db_key(yypvt[-0].s);
		}
		break;
	case 1192:
#line 7701 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key(yypvt[-1].s);
		    db_key(yypvt[-0].s);
		}
		break;
	case 1193:
#line 7706 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 1194:
#line 7712 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    gr->gr_func = tSET;
		}
		break;
	case 1195:
#line 7718 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 1196:
#line 7732 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1197:
#line 7737 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1198:
#line 7742 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1199:
#line 7747 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1201:
#line 7753 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1202:
#line 7758 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1203:
#line 7763 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1204:
#line 7768 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1205:
#line 7773 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1206:
#line 7778 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1207:
#line 7783 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1208:
#line 7788 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_op(ERx(", "));	/* Need a space for run-time */
		}
		break;
	case 1209:
#line 7793 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1210:
#line 7798 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1211:
#line 7803 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		if ((esq->flag & ESQ_NOPRT) == 0)
		    db_key( yypvt[-0].s );
		}
		break;
	case 1212:
#line 7809 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		if ((esq->flag & ESQ_NOPRT) == 0)
		    db_op( yypvt[-0].s );
		}
		break;
	case 1213:
#line 7815 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1214:
#line 7820 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1215:
#line 7825 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1216:
#line 7830 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1217:
#line 7835 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1218:
#line 7840 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1219:
#line 7845 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1220:
#line 7850 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1221:
#line 7855 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		if ((esq->flag & ESQ_NOPRT) == 0)
		    db_key( yypvt[-0].s );
		}
		break;
	case 1222:
#line 7861 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1223:
#line 7866 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1224:
#line 7871 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1225:
#line 7876 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1226:
#line 7881 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1227:
#line 7886 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1228:
#line 7891 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1229:
#line 7896 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1230:
#line 7901 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1231:
#line 7906 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1232:
#line 7911 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_op( ERx(" -") );
		}
		break;
	case 1233:
#line 7916 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1234:
#line 7921 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		if ((esq->flag & ESQ_NOPRT) == 0)
		    db_key( yypvt[-0].s );
		}
		break;
	case 1235:
#line 7927 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1236:
#line 7932 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1237:
#line 7937 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1238:
#line 7942 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1239:
#line 7947 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1240:
#line 7952 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1241:
#line 7957 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1242:
#line 7962 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1243:
#line 7967 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1244:
#line 7972 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1245:
#line 7977 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1246:
#line 7982 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1247:
#line 7987 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1248:
#line 7992 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1249:
#line 7997 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_op( ERx(" +") );
		}
		break;
	case 1250:
#line 8002 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1251:
#line 8007 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1252:
#line 8012 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1253:
#line 8017 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1254:
#line 8022 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1255:
#line 8027 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1256:
#line 8032 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1257:
#line 8037 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1258:
#line 8042 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1259:
#line 8047 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1260:
#line 8052 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1261:
#line 8057 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1262:
#line 8062 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1263:
#line 8067 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 8079 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 8088 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 8098 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 8109 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1268:
#line 8114 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1271:
#line 8143 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1272:
#line 8149 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 1273:
#line 8153 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 1274:
#line 8159 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if ((esq->flag & ESQ_NOPRT) == 0)
			db_key( yypvt[-0].s );
                    yyval.s = yypvt[-0].s;
                }
		break;
	case 1275:
#line 8165 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if ((esq->flag & ESQ_NOPRT) == 0)
			db_delimid( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1276:
#line 8172 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
                    yyval.s = yypvt[-0].s;
                    if (eq->eq_flags & EQ_FIPS)
                        eqck_regid(yypvt[-0].s); /* Check if id is FIPS compliant */
                }
		break;
	case 1277:
#line 8179 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		    if (eq->eq_flags & EQ_FIPS)
			eqck_delimid(yypvt[-0].s); /* Check if id is FIPS complaint */
		}
		break;
	case 1279:
#line 8188 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 8212 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
                    db_key( yypvt[-0].s );
                    yyval.s = yypvt[-0].s;
                    gr->gr_sym = (SYM *)0;
                }
		break;
	case 1283:
#line 8219 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
                    db_key( yypvt[-0].s );
                    yyval.s = yypvt[-0].s;
                    gr->gr_sym = (SYM *)0;
                }
		break;
	case 1284:
#line 8225 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
                    db_key( yypvt[-0].s );
                    yyval.s = yypvt[-0].s;
                    gr->gr_sym = (SYM *)0;
                }
		break;
	case 1285:
#line 8231 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
                    db_key( yypvt[-0].s );
                    yyval.s = yypvt[-0].s;
                    gr->gr_sym = (SYM *)0;
                }
		break;
	case 1286:
#line 8242 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_sconst( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1287:
#line 8251 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1288:
#line 8255 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1289:
#line 8263 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = ERx(" ");
		}
		break;
	case 1290:
#line 8267 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = ERx(",");
		}
		break;
	case 1291:
#line 8277 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1292:
#line 8289 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 8300 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 8311 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 8324 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 8370 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 8391 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 8411 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add(ARG_CHAR, yypvt[-0].s);
		}
		break;
	case 1307:
#line 8416 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    STprintf( esq->sbuf, ERx("%s%s%s"), yypvt[-2].s, yypvt[-1].s, yypvt[-0].s );
		    arg_str_add(ARG_CHAR, esq->sbuf);
		}
		break;
	case 1308:
#line 8421 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 8436 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_nlsym = (SYM *)0;	/* Initialize state vars */
		    gr->gr_nlid = (char *)0;
		    gr->gr_nltype = T_NONE; 
		}
		break;
	case 1312:
#line 8445 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 8472 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 8492 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1318:
#line 8502 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1321:
#line 8515 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("asc")) &&
					!ESQ_EQSTR(yypvt[-0].s, ERx("desc")))
			er_write( E_EQ0126_sqCOMMA, EQ_ERROR, 0);
		    db_key( yypvt[-0].s );
		}
		break;
	case 1322:
#line 8533 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 1323:
#line 8537 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 1327:
#line 8569 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_flag &= ~GR_CPYHNDDEF;
		}
		break;
	case 1333:
#line 8580 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (   gr->gr_flag & GR_COPY_PROG
		        && ESQ_EQSTR(yypvt[-0].s, ERx("copyhandler")))
			gr->gr_flag |= GR_CPYHNDDEF;
		    db_key( yypvt[-0].s );
		}
		break;
	case 1334:
#line 8587 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1335:
#line 8591 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1336:
#line 8595 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1337:
#line 8599 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1338:
#line 8603 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1339:
#line 8607 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1340:
#line 8611 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1341:
#line 8615 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1345:
#line 8623 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1346:
#line 8627 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1347:
#line 8631 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1348:
#line 8635 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1349:
#line 8639 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1354:
#line 8651 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1355:
#line 8655 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1394:
#line 8765 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 8775 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s, NULL );
		    esq_init();
		    arg_int_add( IIutPROG );
		}
		break;
	case 1396:
#line 8782 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_call( IIUTSYS );
		}
		break;
	case 1399:
#line 8792 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( IIutARG );
		}
		break;
	case 1402:
#line 8800 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IIUTSYS );
		}
		break;
	case 1404:
#line 8806 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 1405:
#line 8810 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 1406:
#line 8815 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( IIutARG );
		}
		break;
	case 1421:
#line 8867 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 8877 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IISQSESS );
		}
		break;
	case 1423:
#line 8882 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 8899 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IISQCNNM );
		}
		break;
	case 1425:
#line 8904 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_push();
		}
		break;
	case 1426:
#line 8908 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{ arg_push(); }
		break;
	case 1427:
#line 8909 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IISQUSER );
		}
		break;
	case 1429:
#line 8917 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{ 
		  arg_str_add(ARG_CHAR, ERx("-remote_system_user")); 
		}
		break;
	case 1431:
#line 8924 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{ 
		  arg_str_add(ARG_CHAR, ERx("-remote_system_password"));
		}
		break;
	case 1433:
#line 8930 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		  arg_str_add(ARG_CHAR, ERx("-dbms_password"));
	        }
		break;
	case 1437:
#line 8939 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-1].s, ERx("options")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
					    ERx("CONNECT"), ERx("OPTIONS") );
		}
		break;
	case 1441:
#line 8951 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 8965 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 1443:
#line 8969 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 1444:
#line 8975 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 8984 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IISQCONNECT );
		}
		break;
	case 1446:
#line 8989 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    esq_sqmods(IImodWITH);  	/* force in modifier */
		    gen_call( IISQCONNECT );	/* Close CONNECT */

		    arg_int_add(IIsqpINI);

		  /* Initialize for 2PC with-clause */
		    gr->gr_flag &= ~(GR_HIGHX|GR_LOWX);	
		}
		break;
	case 1447:
#line 9000 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
			/* All specified in string variable */
			arg_int_add(DB_CHR_TYPE);
			arg_str_add(ARG_CHAR, (char *)0);
			arg_int_add(0);
			gen_call(IISQPARMS);
		}
		break;
	case 1448:
#line 9008 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9021 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
			/* Start the next call */
			arg_int_add(IIsqpADD);
		}
		break;
	case 1452:
#line 9027 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* End the current call */
		    gen_call(IISQPARMS);
		}
		break;
	case 1453:
#line 9033 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add(DB_CHR_TYPE);
		    arg_str_add( ARG_CHAR, form_sconst(yypvt[-0].s) );
		    arg_int_add(0);
		    if (gr->gr_flag & GR_HIGHX || gr->gr_flag & GR_LOWX)
			er_write( E_EQ0079_grCONNECTID, EQ_ERROR, 0 );
		}
		break;
	case 1454:
#line 9041 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add(DB_CHR_TYPE);
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    arg_int_add(0);
		    if (gr->gr_flag & GR_HIGHX || gr->gr_flag & GR_LOWX)
			er_write( E_EQ0079_grCONNECTID, EQ_ERROR, 0 );
		}
		break;
	case 1455:
#line 9049 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add(DB_INT_TYPE);
		    arg_str_add( ARG_CHAR, (char *)0 );
		    arg_str_add( ARG_INT, yypvt[-0].s );
		}
		break;
	case 1456:
#line 9055 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9099 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9111 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9123 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9145 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    er_write( E_EQ0070_grMISSWORD, EQ_ERROR, 2, ERx("INTO"),
							ERx("COPY SQLERROR") );
		}
		break;
	case 1465:
#line 9158 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9168 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    esq->flag |= ESQ_NOPRT;	/* Suppress printing */
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-1].s, yypvt[-0].s );
		}
		break;
	case 1467:
#line 9173 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    esq->flag |= ESQ_NOPRT;	/* Suppress printing */
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-3].s, yypvt[-2].s );
		}
		break;
	case 1473:
#line 9187 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1474:
#line 9191 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-2].i || yypvt[-0].i;	/* DEFAULT keyword */
		}
		break;
	case 1475:
#line 9196 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    id_free();
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1476:
#line 9201 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    id_free();
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1477:
#line 9207 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    id_free();
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1483:
#line 9246 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1484:
#line 9251 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1485:
#line 9255 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1486:
#line 9260 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		    esq->flag &= ~ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 1487:
#line 9265 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		    esq->flag &= ~ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 1488:
#line 9270 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		    esq->flag &= ~ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 1489:
#line 9275 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		    esq->flag |= ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 1490:
#line 9280 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		    esq->flag &= ~ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 1491:
#line 9286 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9300 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9313 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9332 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1496:
#line 9336 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-1].i;
		}
		break;
	case 1497:
#line 9341 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9351 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9361 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9365 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = CRE_WDEF;
		}
		break;
	case 1501:
#line 9369 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = CRE_OK;
		}
		break;
	case 1502:
#line 9373 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = CRE_NDEF;
		}
		break;
	case 1503:
#line 9378 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9388 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    	db_key( yypvt[-0].s );
		}
		break;
	case 1505:
#line 9395 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9406 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    	db_key( yypvt[-0].s );
		}
		break;
	case 1509:
#line 9413 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    	db_sconst( yypvt[-0].s );
		}
		break;
	case 1510:
#line 9419 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    	db_key( yypvt[-0].s );
		}
		break;
	case 1511:
#line 9425 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    	db_key( yypvt[-0].s );
		}
		break;
	case 1513:
#line 9433 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9443 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9472 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9501 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IISQEXIT );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ050A_FIPS_EXTSN, EQ_WARN,
					1, ERx("DISCONNECT") );
		}
		break;
	case 1530:
#line 9509 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-0].s, NULL );
		    esq_init();
		}
		break;
	case 1531:
#line 9515 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IISQSESS );
		}
		break;
	case 1532:
#line 9519 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add ( IIsqdisALL );	
		    gen_call( IISQSESS );
		}
		break;
	case 1533:
#line 9524 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* connection name */
		    gen_call( IISQCNNM );
		}
		break;
	case 1536:
#line 9542 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    eqgen_tl(IIGETDATA);
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		}
		break;
	case 1537:
#line 9549 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9563 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9584 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
                    eqgen_tl(IIPUTDATA);
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
                }
		break;
	case 1541:
#line 9591 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9609 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IIEVTSTAT );
		    gr->gr_flag |= GR_GETEVT;	/* Special WHENEVER handling */
		}
		break;
	case 1543:
#line 9615 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9628 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( 0 );	/* "no wait" */
		}
		break;
	case 1547:
#line 9635 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-1].s, ERx("WAIT")))
		    {
			er_write(E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
					ERx("GET DBEVENT"), ERx("WAIT"));
		    }
		}
		break;
	case 1548:
#line 9644 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9673 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    eqtl_add(gr->gr_id, (PTR)gr->gr_sym, gr->gr_nlid, (PTR)gr->gr_nlsym,
				gr->gr_type, yypvt[-0].s);
		    if ( (gr->gr_type == T_STRUCT) && (eq->eq_flags & EQ_FIPS) )
			er_write( E_EQ050D_FIPS_VAR, EQ_WARN, 0 );
		}
		break;
	case 1554:
#line 9681 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1555:
#line 9685 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1556:
#line 9689 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1557:
#line 9694 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
                       gr->gr_id = yypvt[-0].s;
                       gr->gr_type = T_INT;
                       gr->gr_sym = (SYM *)0;
                       gr->gr_nlsym = (SYM *)0;
		}
		break;
	case 1558:
#line 9701 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9726 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( QAPRINTF );
		}
		break;
	case 1560:
#line 9731 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-0].s, NULL );
		}
		break;
	case 1564:
#line 9741 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (gr->gr_sym)
			arg_var_add( gr->gr_sym, gr->gr_id );
		    else
			arg_str_add( ARG_CHAR, gr->gr_id );
		}
		break;
	case 1567:
#line 9758 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
			eqgen_tl(IIINQSQL);
			if (eq->eq_flags & EQ_CHRPAD)
			    esq_sqmods(IImodNOCPAD);
		}
		break;
	case 1568:
#line 9765 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9785 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
			eqgen_tl(IISETSQL);
		}
		break;
	case 1570:
#line 9789 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    eqtl_add(gr->gr_id, (PTR)gr->gr_sym, gr->gr_nlid, 
			 (PTR)gr->gr_nlsym, gr->gr_type, ERx("connection_name"));
		    eqgen_tl(IISETSQL);
		}
		break;
	case 1571:
#line 9795 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9826 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
			gr->gr_id = yypvt[-0].s;
			gr->gr_type = T_CHAR;
			gr->gr_sym = (SYM *)0;
			gr->gr_nlsym = (SYM *)0;
		}
		break;
	case 1574:
#line 9835 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9845 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9866 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9902 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
			gr->gr_id = yypvt[-0].s;
			gr->gr_type = T_INT;
			gr->gr_sym = (SYM *)0;
			gr->gr_nlsym = (SYM *)0;
		}
		break;
	case 1581:
#line 9909 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
			gr->gr_id = yypvt[-0].s;
			gr->gr_type = T_CHAR;
			gr->gr_sym = (SYM *)0;
			gr->gr_nlsym = (SYM *)0;
		}
		break;
	case 1583:
#line 9917 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
			gr->gr_id = yypvt[-0].s;
			gr->gr_type = T_NONE;
			gr->gr_sym = (SYM *)0;
			gr->gr_nlsym = (SYM *)0;
		}
		break;
	case 1584:
#line 9938 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (yypvt[-0].i && (esq->inc & sqcaSQL) == 0)
		    {
			er_write( E_EQ0128_sqNOSQLCA, EQ_ERROR, 0 );
			esq->inc |= sqcaSQL;
		    }
		}
		break;
	case 1585:
#line 9948 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    esq->flag |= ESQ_NOPRT;	/* Suppress printing */
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-0].s, NULL );
		}
		break;
	case 1586:
#line 9956 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 9989 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 10005 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    esqlca( yypvt[-1].i, sqcaCONTINUE, (char *)0 );
		    yyval.i = 0;
		}
		break;
	case 1589:
#line 10010 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    esqlca( yypvt[-3].i, sqcaGOTO, yypvt[-0].s );
		    yyval.i = 1;
		}
		break;
	case 1590:
#line 10015 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    esqlca( yypvt[-2].i, sqcaGOTO, yypvt[-0].s );
		    yyval.i = 1;
		}
		break;
	case 1591:
#line 10020 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    esqlca( yypvt[-2].i, sqcaCALL, yypvt[-0].s );
		    yyval.i = 1;
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				    2, ERx("CALL"), ERx("WHENEVER") );
		}
		break;
	case 1592:
#line 10028 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 10059 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    arg_int_add( yypvt[-0].i );		/* using names? */
		    gen_call( IISQDESCRIBE );
		}
		break;
	case 1594:
#line 10066 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 10083 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = IIsqdNULUSE;
		}
		break;
	case 1599:
#line 10087 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 10112 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IISQDESCINPUT );
		}
		break;
	case 1601:
#line 10118 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s, NULL );
		    esq_init();
		    gr->gr_flag |= GR_NOSYNC;
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		}
		break;
	case 1602:
#line 10189 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("EXECUTE IMMEDIATE") );
		}
		break;
	case 1603:
#line 10195 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("EXECUTE") );
		}
		break;
	case 1604:
#line 10202 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 10213 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IISQEXIMMED );
		}
		break;
	case 1606:
#line 10217 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_flag |= GR_NOSYNC;
		    esq->flag &= ~ESQ_DESC_SEL;
		}
		break;
	case 1607:
#line 10223 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_CHAR, form_sconst(yypvt[-0].s) );
		}
		break;
	case 1608:
#line 10227 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 10244 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    esq_sqmods(IImodDYNSEL);
		    esq->flag |= ESQ_REG_SELECT;
		    esl_query( ESL_START_QUERY );
		    gen_call( IISQEXIMMED );
		}
		break;
	case 1610:
#line 10252 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 10282 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    erec_desc( gr->gr_id );	    /* Save arg for IIcsDaGet */
		    esq->flag |= ESQ_DESC_SEL;
		}
		break;
	case 1612:
#line 10287 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    erec_setup( 0 );			/* Done with list */
		}
		break;
	case 1613:
#line 10292 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( 0 );			/* not using vars */
		    gen_call( IISQEXSTMT );
		}
		break;
	case 1615:
#line 10298 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IISQDAIN );
		}
		break;
	case 1616:
#line 10305 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( 1 );			/* using vars */
		    gen_call( IISQEXSTMT );
		    gr->gr_func = tEXECUTE;
		}
		break;
	case 1617:
#line 10312 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( 0 );			/* not using vars */
		    gen_call( IISQEXSTMT );
		}
		break;
	case 1620:
#line 10321 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_var( DB_REG, gr->gr_sym, gr->gr_id,
			    gr->gr_nlsym, gr->gr_nlid, (char *)0 );
		}
		break;
	case 1621:
#line 10326 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_var( DB_HDLR, gr->gr_sym, gr->gr_id,
			    gr->gr_nlsym, gr->gr_nlid, yypvt[-1].s );
		}
		break;
	case 1622:
#line 10367 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 10392 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 10427 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Copy (*gr) for "proc_exec" to build code. */
		    STRUCT_ASSIGN_MACRO(*gr, tempgr);
		}
		break;
	case 1627:
#line 10439 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 10452 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IIPROCINIT );
		}
		break;
	case 1631:
#line 10460 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IIPROCGTTP );
		}
		break;
	case 1632:
#line 10464 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (esq->flag & ESQ_PROC_BYREF)
		    {
			/* procs w/BYREF params act like singleton select */
			esq_sqmods(IImodSINGLE);
		    }
		}
		break;
	case 1633:
#line 10472 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IISQDAIN );
		}
		break;
	case 1638:
#line 10486 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IIPROCVALIO );
		}
		break;
	case 1642:
#line 10496 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
			arg_int_add( TRUE );
		}
		break;
	case 1643:
#line 10501 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( FALSE );
		}
		break;
	case 1644:
#line 10506 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 10537 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 10554 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 10566 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 10620 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    lbl_exit_block();
		}
		break;
	case 1651:
#line 10640 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 10663 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( gr->gr_rcnt );
		    gen_loop( G_OPEN, L_RETBEG, L_RETFLUSH, 
			    lbl_next(LBL_RET), IIPROCSTAT, C_NOTEQ, 0 );
		    gen_sqlca( sqcaSQPROC );
		    yyval.i = 1;		/* indicate result clause */
		}
		break;
	case 1653:
#line 10672 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (esq->flag & ESQ_PROC_BYREF)
			er_write( E_EQ005A_grBYREF, EQ_ERROR, 0);
				/* can't have both BYREF parm & RESULT ROW */
		    erec_setup( 1 );	/* Initialize struct indicators stuff */
		    esq->flag |= ESQ_PROC_RESULT;  /* & set flag */
		}
		break;
	case 1656:
#line 10684 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    erec_setup( 0 );	/* Done with one element */
		}
		break;
	case 1657:
#line 10689 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_rcnt++;
		}
		break;
	case 1658:
#line 10703 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IISQPREPARE );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("PREPARE") );
		}
		break;
	case 1659:
#line 10712 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 10723 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s, NULL );
		    esq_init();
		    gr->gr_flag |= GR_NOSYNC;
		    arg_int_add( eq_genlang(eq->eq_lang) );  /* host language */
		}
		break;
	case 1661:
#line 10731 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_id = NULL;
		    yyval.i = 0;
		}
		break;
	case 1662:
#line 10736 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1664:
#line 10743 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 10756 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_CHAR, form_sconst(yypvt[-0].s) );
		}
		break;
	case 1669:
#line 10785 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("DIRECT CONNECT") );
		}
		break;
	case 1670:
#line 10793 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("DIRECT DISCONNECT") );
		}
		break;
	case 1671:
#line 10801 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("DIRECT EXECUTE IMMEDIATE") );
		}
		break;
	case 1673:
#line 10810 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 10901 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    lbl_exit_block();
		}
		break;
	case 1716:
#line 10906 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 10918 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 1720:
#line 10934 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IIADDFORM );
		    gr->gr_flag &= ~GR_NOINDIR;
		}
		break;
	case 1721:
#line 10940 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gr->gr_flag |= GR_NOINDIR;
		}
		break;
	case 1722:
#line 10946 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 10965 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
# ifndef EQ_ADD_FNAME
		    er_write( E_EQ0061_grINTVAR, EQ_ERROR, 1, yypvt[-0].s );
# endif /* EQ_ADD_FNAME */
		}
		break;
	case 1724:
#line 10981 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11002 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-1].s, NULL );
		    gen_call( IICLRSCR );
		}
		break;
	case 1726:
#line 11007 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IICLRFLDS );
		}
		break;
	case 1728:
#line 11013 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-1].s, NULL );
		    gr->gr_func = IIFLDCLEAR;
		}
		break;
	case 1729:
#line 11045 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IIFRSQDESC );
		}
		break;
	case 1730:
#line 11051 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IIFRSQDESC );
		}
		break;
	case 1731:
#line 11057 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_int_add( 1 );			/* is_form = TRUE */
		}
		break;
	case 1732:
#line 11064 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_CHAR, (char* )0 );	/* No table name */
		}
		break;
	case 1733:
#line 11069 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    STprintf( esq->sbuf, ERx("%s %s"), yypvt[-1].s, yypvt[-0].s );
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, esq->sbuf, NULL );
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_int_add( 0 );			/* is_form = FALSE */
		}
		break;
	case 1734:
#line 11077 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_CHAR, ERx("u") );
		}
		break;
	case 1735:
#line 11081 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_CHAR, ERx("a") );
		}
		break;
	case 1736:
#line 11085 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11110 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11130 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_CHAR, ERx("a") );
		}
		break;
	case 1739:
#line 11135 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1740:
#line 11139 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1741:
#line 11205 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11239 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11258 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11277 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gr->gr_func = IIDISPFRM;
		}
		break;
	case 1745:
#line 11283 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    frs_mode( ERx("DISPLAY/INITTABLE"), yypvt[-0].s );
		}
		break;
	case 1746:
#line 11287 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    frs_mode( ERx("DISPLAY/INITTABLE"), yypvt[-0].s );
		}
		break;
	case 1747:
#line 11291 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    frs_mode( ERx("DISPLAY/INITTABLE"), yypvt[-0].s );
		}
		break;
	case 1748:
#line 11295 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_CHAR, ERx("u") );
		}
		break;
	case 1749:
#line 11299 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11316 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_CHAR, ERx("f") );
		}
		break;
	case 1751:
#line 11321 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    sc_eat( NULL, SC_NEST|SC_SEEN, ERx(")"), '(', ')' );
		    er_write( E_EQ0051_grDISPTL, EQ_ERROR, 0 );
		}
		break;
	case 1753:
#line 11349 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11366 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11416 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11426 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_flag &= ~GR_ACTIVATE;
		}
		break;
	case 1763:
#line 11434 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_else( G_ELSE, IIRETVAL, C_0, 0, L_ACTELSE, 
			lbl_current(LBL_INTR), L_ACTEND, lbl_current(LBL_FORM));
		    gr->gr_mode = LBLmDISPLAY;
		}
		break;
	case 1764:
#line 11441 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11455 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Reset activate argument list */
		    act_args( FALSE, (char *)0, (char *)0, 0, (SYM *)0 );
		}
		break;
	case 1768:
#line 11469 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    act_add( ACT_FIELD, lbl_current(LBL_INTR), yypvt[-2].i, yypvt[-0].ip );
		    gr->gr_adjective = LBLaFIELD;
		}
		break;
	case 1769:
#line 11474 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    act_add( ACT_MENU, lbl_current(LBL_INTR), 0, yypvt[-1].ip );
		    gr->gr_adjective = LBLaMITEM;
		}
		break;
	case 1770:
#line 11479 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    act_add( ACT_SCROLL, lbl_current(LBL_INTR), sclUP, yypvt[-0].ip );
		    gr->gr_adjective = LBLaSCROLL;
		}
		break;
	case 1771:
#line 11484 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    act_add( ACT_SCROLL,lbl_current(LBL_INTR), sclDOWN, yypvt[-0].ip );
		    gr->gr_adjective = LBLaSCROLL;
		}
		break;
	case 1772:
#line 11489 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    i4  cur_intr = lbl_current( LBL_INTR );

		    act_add( ACT_COLUMN, cur_intr, yypvt[-3].i, yypvt[-1].ip );
		    act_add( ACT_COLUMN, cur_intr, yypvt[-3].i, yypvt[-0].ip );
		    gr->gr_adjective = LBLaCOLUMN;
		}
		break;
	case 1773:
#line 11497 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Old-style syntax - no longer supported */
                    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1,
                                            ERx("ACTIVATE COMMAND") );
		}
		break;
	case 1775:
#line 11505 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 1776:
#line 11509 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11524 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.ip = act_string( form_sconst(yypvt[-0].s) );
		}
		break;
	case 1778:
#line 11528 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.ip = act_string( yypvt[-0].s );
		}
		break;
	case 1779:
#line 11532 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.ip = act_string( yypvt[-0].s );
		}
		break;
	case 1780:
#line 11536 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11549 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1782:
#line 11553 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1783:
#line 11567 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11626 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11645 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11659 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11675 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 1788:
#line 11679 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 1791:
#line 11687 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* It will check for errors */
		    act_args( TRUE, yypvt[-2].s, yypvt[-0].s, gr->gr_type, gr->gr_sym );
		}
		break;
	case 1792:
#line 11693 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1793:
#line 11697 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1794:
#line 11701 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1795:
#line 11705 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1796:
#line 11710 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = form_sconst(yypvt[-0].s);
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = NULL;
		}
		break;
	case 1797:
#line 11716 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_INT;
		    gr->gr_sym = NULL;
		}
		break;
	case 1798:
#line 11722 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11749 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_goto( G_TERM, L_FDEND, lbl_current(LBL_FORM) );
		}
		break;
	case 1800:
#line 11754 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11772 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11797 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11833 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11853 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gen_call( IIENDFORMS );
		}
		break;
	case 1807:
#line 11868 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11915 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    i4  cur_form = lbl_current( LBL_FORM );

		    gen_loop( G_CLOSE, L_FDBEG, L_FDEND, cur_form, IIFNAMES,
			C_0, 0 );
		    gen_label( G_LOOP, L_FDEND, cur_form );
		    gen_call( IIENDFRM );
		}
		break;
	case 1809:
#line 11925 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 11938 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1812:
#line 11952 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gr->gr_func = IIFORMINIT;
		}
		break;
	case 1813:
#line 11965 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( eq_genlang(eq->eq_lang) );
		    gen_call( IIFORMS );
		}
		break;
	case 1814:
#line 11989 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
                    if (eq->eq_flags & EQ_CHRPAD)
                        esq_sqmods(IImodNOCPAD);
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		}
		break;
	case 1815:
#line 11995 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
                    if (eq->eq_flags & EQ_CHRPAD)
                        esq_sqmods(IImodNOCPAD);
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		}
		break;
	case 1816:
#line 12002 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_if( G_OPEN, IIFSETIO, C_NOTEQ, 0 );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		    gr->gr_func = IIRETFIELD;
		}
		break;
	case 1817:
#line 12010 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1818:
#line 12025 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    er_write(E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1, ERx("GETOPER"));
		    YYERROR;
		}
		break;
	case 1820:
#line 12042 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( 0 );		/* Dummy type right now */
		}
		break;
	case 1821:
#line 12048 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 12059 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 1823:
#line 12071 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IIHELPFILE );
		}
		break;
	case 1824:
#line 12076 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1825:
#line 12088 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( QAMESSAGE );
		}
		break;
	case 1826:
#line 12093 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1827:
#line 12114 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		    frs_close();
		}
		break;
	case 1829:
#line 12123 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    frs_head();
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);

		}
		break;
	case 1830:
#line 12131 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    frs_inqset( FRSinq );
		}
		break;
	case 1831:
#line 12137 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 1832:
#line 12141 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 1833:
#line 12145 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 1834:
#line 12149 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 1835:
#line 12153 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 1836:
#line 12157 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    frs_object( "row" );
		}
		break;
	case 1839:
#line 12165 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    frs_parentname( yypvt[-0].s, T_CHAR, (PTR)0 );
		}
		break;
	case 1840:
#line 12169 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    frs_parentname( yypvt[-0].s, T_CHAR, (PTR)0 );
		}
		break;
	case 1841:
#line 12173 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    frs_parentname( yypvt[-0].s, T_INT, (PTR)0 );
		}
		break;
	case 1842:
#line 12177 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 12200 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    er_write( E_EQ0077_grISUNSUPP, EQ_ERROR, 1,
							ERx("INQUIRE_FRS") );
		    YYERROR;
		}
		break;
	case 1848:
#line 12212 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    frs_iqvar( gr->gr_id, gr->gr_type, gr->gr_sym,
			gr->gr_nlid, gr->gr_nlsym );
		}
		break;
	case 1849:
#line 12218 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    frs_constant( yypvt[-0].s, (char *)0, T_NONE, (SYM *)0 );
		}
		break;
	case 1850:
#line 12222 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    er_write( E_EQ0054_grNOPARENS, EQ_ERROR, 0 );
		    frs_constant( yypvt[-2].s, (char *)0, T_NONE, (SYM *)0 );
		}
		break;
	case 1851:
#line 12227 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Should be an error */
		    frs_constant( yypvt[-3].s, yypvt[-1].s, gr->gr_type, gr->gr_sym );
		}
		break;
	case 1852:
#line 12234 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1853:
#line 12238 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1854:
#line 12242 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1855:
#line 12246 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1856:
#line 12250 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1857:
#line 12254 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1858:
#line 12258 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1859:
#line 12262 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1860:
#line 12266 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1861:
#line 12270 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1862:
#line 12275 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1863:
#line 12281 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1864:
#line 12287 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_INT;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1865:
#line 12293 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1866:
#line 12299 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1867:
#line 12305 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1868:
#line 12311 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1869:
#line 12317 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1870:
#line 12323 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1871:
#line 12329 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1872:
#line 12335 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 12360 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( QAPROMPT );
		}
		break;
	case 1874:
#line 12365 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1875:
#line 12383 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IIMESSAGE ); 
    		}
		break;
	case 1876:
#line 12388 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
            	    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
    	    	    gr->gr_func = IIMESSAGE;
		}
		break;
	case 1879:
#line 12412 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    EQFW3glgen();	
		}
		break;
	case 1880:
#line 12417 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    EQFWopen(gr->gr_func);
		}
		break;
	case 1884:
#line 12433 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    EQFWwcoption(yypvt[-2].s, yypvt[-0].s);
		}
		break;
	case 1885:
#line 12438 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1886:
#line 12442 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1887:
#line 12447 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1888:
#line 12451 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1893:
#line 12476 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		EQFWsuboption(yypvt[-2].s, gr->gr_type, gr->gr_id, (PTR)gr->gr_sym);
	}
		break;
	case 1894:
#line 12481 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
	    	    yyval.s = yypvt[-0].s;
	        }
		break;
	case 1895:
#line 12485 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1896:
#line 12489 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1897:
#line 12495 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_INT;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 1898:
#line 12501 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_CHAR;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 1899:
#line 12507 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_FLOAT;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 1900:
#line 12513 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_PACKASCHAR;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 1901:
#line 12519 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (!gr->gr_sym)
		    {
			gr->gr_type = T_CHAR;
		    }	
		}
		break;
	case 1902:
#line 12526 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_CHAR;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 1903:
#line 12532 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_sym = (SYM *)0;
		    gr->gr_type = T_CHAR;
		    gr->gr_id = yypvt[-0].s;
		}
		break;
	case 1904:
#line 12538 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_sym = (SYM *)0;
		    gr->gr_type = T_CHAR;
		    gr->gr_id = yypvt[-0].s;
		}
		break;
	case 1905:
#line 12552 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IIPRNSCR );
		}
		break;
	case 1906:
#line 12557 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1907:
#line 12562 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Should check that $2.s is "file" */
		    if (STbcompare(yypvt[-3].s, 0, ERx("file"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-3].s,
					    ERx("PRINTSCREEN"), ERx("FILE") );
		}
		break;
	case 1908:
#line 12569 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		}
		break;
	case 1909:
#line 12574 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1910:
#line 12594 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		    gen_call( IINPROMPT );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);

		}
		break;
	case 1911:
#line 12604 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gr->gr_func = IINPROMPT;
		}
		break;
	case 1912:
#line 12610 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (STbcompare(yypvt[-0].s, 0, ERx("noecho"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
						ERx("PROMPT"), ERx("NOECHO") );
		    arg_int_add( 1 );
		}
		break;
	case 1913:
#line 12617 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( 0 );
		}
		break;
	case 1914:
#line 12622 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 12671 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		}
		break;
	case 1916:
#line 12677 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		}
		break;
	case 1917:
#line 12684 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_if( G_OPEN, IIFSETIO, C_NOTEQ, 0 );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		    gr->gr_func = IISETFIELD;
		}
		break;
	case 1918:
#line 12692 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1919:
#line 12704 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gen_call( IIREDISP );
		}
		break;
	case 1920:
#line 12725 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 12751 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1922:
#line 12756 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = (char *)0;	
		}
		break;
	case 1923:
#line 12760 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 12786 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-1].s;	
		    gen_call( IIRESFLD );
		}
		break;
	case 1925:
#line 12791 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-2].s;	
		    gen_call( IIRESCOL );
		}
		break;
	case 1926:
#line 12796 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 12863 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1928:
#line 12878 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    frs_close();
		}
		break;
	case 1930:
#line 12887 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    frs_inqset( FRSset );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		}
		break;
	case 1935:
#line 12906 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    frs_setval( form_sconst(yypvt[-0].s), T_CHAR, (SYM *)0,
			(char *)0, (SYM *)0 );
		}
		break;
	case 1936:
#line 12911 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    frs_setval( yypvt[-0].s, T_INT, (SYM *)0, (char *)0, (SYM *)0 );
		}
		break;
	case 1937:
#line 12915 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    frs_setval(ERx("0"), T_INT, (SYM *)0, (char *)0, (SYM *)0);
		}
		break;
	case 1938:
#line 12920 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    frs_setval(ERx("0"), T_INT, (SYM *)0, (char *)0, (SYM *)0);
		}
		break;
	case 1939:
#line 12925 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    frs_setval( yypvt[-0].s, T_CHAR, (SYM *)0, (char *)0, (SYM *)0 );
		}
		break;
	case 1940:
#line 12929 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    frs_setval( yypvt[-0].s, T_CHAR, (SYM *)0, (char *)0, (SYM *)0 );
		}
		break;
	case 1941:
#line 12933 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 12945 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 12966 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    frs_setval( ERx("1"), T_INT, (SYM *)0, (char *)0,
				(SYM *)0 );
		}
		break;
	case 1944:
#line 12979 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IISLEEP );
		}
		break;
	case 1945:
#line 12984 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1946:
#line 13017 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 13035 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 13051 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_else( G_ELSE, IIRETVAL, C_0, 0, L_ACTELSE, 
			lbl_current(LBL_INTR), L_ACTEND, 
			lbl_current(LBL_FORM) );
		    gr->gr_mode = LBLmSUBMENU;
		}
		break;
	case 1952:
#line 13060 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_mode = LBLmSUBMENU;	/* Not LBLmDISPLAY */
		}
		break;
	case 1953:
#line 13065 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_mode = LBLmSUBMENU;	/* Not LBLmDISPLAY */
		}
		break;
	case 1954:
#line 13069 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_mode = LBLmSUBMENU;	/* Not LBLmDISPLAY */
		}
		break;
	case 1955:
#line 13074 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    act_add( ACT_MENU, lbl_current(LBL_INTR), 0, yypvt[-1].ip );
		    gr->gr_adjective = LBLaMITEM;
		}
		break;
	case 1957:
#line 13093 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 13107 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1959:
#line 13112 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_func = IICHKFRM;
		}
		break;
	case 1960:
#line 13116 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_func = IIVALFLD;
		}
		break;
	case 1961:
#line 13138 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 1962:
#line 13144 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IITCLRCOL;
		}
		break;
	case 1963:
#line 13150 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmCLEARR );
		}
		break;
	case 1965:
#line 13157 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    er_write( E_EQ0054_grNOPARENS, EQ_ERROR, 0 );
		    gen_call( IITCLRROW );
		}
		break;
	case 1966:
#line 13162 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IITCLRROW );
		}
		break;
	case 1967:
#line 13183 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 13194 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 13204 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmDELETER );
		}
		break;
	case 1971:
#line 13211 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_VERSWARN)
			er_write( E_EQ0078_grSTYUNSUPP, EQ_WARN, 1,
							ERx("DELETEROW OUT") );
		}
		break;
	case 1972:
#line 13218 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( FALSE );
		    gen_if( G_OPEN, IITDELROW, C_NOTEQ, 0 );
		}
		break;
	case 1973:
#line 13223 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_VERSWARN)
			er_write( E_EQ0078_grSTYUNSUPP, EQ_WARN, 1,
							ERx("DELETEROW IN") );
		}
		break;
	case 1974:
#line 13230 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( TRUE );	/* In list */
		    gen_if( G_OPEN, IITDELROW, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 1975:
#line 13257 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
                    if (eq->eq_flags & EQ_CHRPAD)
                        esq_sqmods(IImodNOCPAD);
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 1976:
#line 13264 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
                    if (eq->eq_flags & EQ_CHRPAD)
                        esq_sqmods(IImodNOCPAD);
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 1977:
#line 13272 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
                    if (eq->eq_flags & EQ_CHRPAD)
                        esq_sqmods(IImodCPAD);
		    gr->gr_func = IITCOLRET;
		}
		break;
	case 1978:
#line 13280 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmGETR );
		}
		break;
	case 1979:
#line 13298 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IITFILL );
		    gen_if( G_CLOSE, IITBINIT, C_0, 0 );
		}
		break;
	case 1980:
#line 13304 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_if( G_OPEN, IITBINIT, C_NOTEQ, 0 );
		}
		break;
	case 1981:
#line 13309 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1985:
#line 13318 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_call( IITHIDECOL );
		}
		break;
	case 1989:
#line 13335 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{ gr->gr_sym = (SYM *)0; }
		break;
	case 1990:
#line 13337 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 13352 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    id_add( form_sconst(yypvt[-0].s) );
		}
		break;
	case 1993:
#line 13356 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 13373 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    id_add( yypvt[-0].s );
		}
		break;
	case 1995:
#line 13378 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;		/* Paren not stuff present */
		}
		break;
	case 1996:
#line 13382 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    id_add( yypvt[-2].s );
		    id_add( yypvt[-1].s );
		    id_add( yypvt[-0].s );
		    yyval.i = 1;		/* Paren stuff present */
		}
		break;
	case 1997:
#line 13389 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 13403 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;		/* No NULL clause */
		}
		break;
	case 1999:
#line 13407 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 13412 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (yypvt[-0].i == 0)
			id_add( ERx(" not null") );
		    else
			id_add( ERx(" not null with default") );
		    yyval.i = 1;		/* With NULL clause */
		}
		break;
	case 2002:
#line 13425 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 2003:
#line 13429 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 2004:
#line 13444 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 13456 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmPURGE );
		}
		break;
	case 2006:
#line 13495 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 13504 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 13514 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gen_if( G_OPEN, IITINSERT, C_NOTEQ, 0 );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 2009:
#line 13523 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmINSERTR );
		}
		break;
	case 2010:
#line 13558 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IITBCECOLEND );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IITBACT, C_0, 0 );
		}
		break;
	case 2011:
#line 13565 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IITBCECOLEND );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IITBACT, C_0, 0 );
		}
		break;
	case 2012:
#line 13573 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( TRUE );		/* Loading */
		    gen_if( G_OPEN, IITBACT, C_NOTEQ, 0 );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 2013:
#line 13582 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 2016:
#line 13590 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    frs_inqset(FRSload);
		    frs_object(ERx("row"));
		}
		break;
	case 2017:
#line 13616 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IITBCECOLEND );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 2018:
#line 13623 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IITBCECOLEND );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 2019:
#line 13631 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 2020:
#line 13639 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmPUTR );
		}
		break;
	case 2021:
#line 13665 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Close inner and scroll if blocks */
		    gen_if( G_CLOSE, IITSCROLL, C_0, 0 );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 2022:
#line 13672 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( rowNONE );
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		}
		break;
	case 2023:
#line 13678 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmSCROLL );
		}
		break;
	case 2024:
#line 13684 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_if( G_OPEN, IITSCROLL, C_NOTEQ, 0 );
		}
		break;
	case 2025:
#line 13688 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if ((yypvt[-1].i || yypvt[-0].i) && (eq->eq_flags & EQ_VERSWARN))
			er_write( E_EQ0078_grSTYUNSUPP, EQ_WARN, 1,
							ERx("SCROLL IN/OUT") );
		}
		break;
	case 2026:
#line 13695 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gen_call( IITBSMODE );
		    arg_int_add( FALSE );	/* Ignore In flag */
		}
		break;
	case 2028:
#line 13703 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( rowEND );
		}
		break;
	case 2029:
#line 13708 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gen_call( IITBSMODE );
		}
		break;
	case 2030:
#line 13713 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gen_call( IITBSMODE );
		}
		break;
	case 2033:
#line 13722 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_func = IITCOLRET;
		}
		break;
	case 2034:
#line 13727 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( FALSE );
		    arg_int_add( rowNONE );
		    gen_if( G_OPEN, IITSCROLL, C_NOTEQ, 0 );
		}
		break;
	case 2036:
#line 13735 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( TRUE );	/* In list */
		    arg_int_add( rowNONE );
		    gen_if( G_OPEN, IITSCROLL, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 2037:
#line 13757 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 13768 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 13814 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 13825 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		    gen_call( IITBCECOLEND );
		}
		break;
	case 2041:
#line 13831 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		    gen_call( IITBCECOLEND );	
		}
		break;
	case 2042:
#line 13838 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 13855 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 2044:
#line 13876 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 13893 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{	
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLVAL;
		    yyval.s = yypvt[-3].s;
		}
		break;
	case 2046:
#line 13900 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmVALIDR );
		}
		break;
	case 2048:
#line 13907 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    er_write( E_EQ0054_grNOPARENS, EQ_ERROR, 0 );
		    gen_call( IITVALROW );
		}
		break;
	case 2049:
#line 13912 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IITVALROW );
		}
		break;
	case 2050:
#line 13923 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 13942 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 2052:
#line 13946 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 2054:
#line 13962 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    er_write( E_EQ0054_grNOPARENS, EQ_ERROR, 0 );
		}
		break;
	case 2059:
#line 13973 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 2060:
#line 13977 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 14004 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 2062:
#line 14017 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{	
		    arg_int_add( 0 );
		    arg_var_add( gr->gr_null, ERx("null") ); 
		}
		break;
	case 2064:
#line 14035 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    er_write( E_EQ0054_grNOPARENS, EQ_ERROR, 0 );
		}
		break;
	case 2069:
#line 14046 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 2070:
#line 14050 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 14070 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 14083 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( rowCURR );
		}
		break;
	case 2075:
#line 14091 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    if (lbl_gen(LBLmDISPLAY, FALSE) == LBL_NOLEV)
			er_write( E_EQ0056_grFORMNAME, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 2076:
#line 14100 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    er_write( E_EQ0071_grASSGNOP, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 2080:
#line 14121 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = form_sconst(yypvt[-0].s);
		}
		break;
	case 2081:
#line 14127 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 2082:
#line 14133 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( 0 );		/* null indicator */
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 2084:
#line 14146 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 2085:
#line 14150 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 14173 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 14194 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2089:
#line 14198 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    char	ibuf[50];

		    ibuf[0] = '-';
		    ibuf[1] = '\0';
		    yyval.s = str_add( STRNULL, STcat(ibuf, yypvt[-0].s) );
		}
		break;
	case 2090:
#line 14206 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2091:
#line 14213 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_INT, yypvt[-0].s );
		}
		break;
	case 2092:
#line 14219 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( 0 );		/* null indicator */
		    arg_str_add( ARG_INT, yypvt[-0].s );
		}
		break;
	case 2093:
#line 14227 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2094:
#line 14231 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    char	fbuf[50];

		    fbuf[0] = '-';
		    fbuf[1] = '\0';
		    yyval.s = str_add( STRNULL, STcat(fbuf, yypvt[-0].s) );
		}
		break;
	case 2095:
#line 14239 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2096:
#line 14255 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( 0 );		/* null indicator */
		    arg_str_add( ARG_FLOAT, yypvt[-0].s );
		}
		break;
	case 2097:
#line 14263 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2098:
#line 14267 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    char	dbuf[50];

		    dbuf[0] = '-';
		    dbuf[1] = '\0';
		    yyval.s = str_add( STRNULL, STcat(dbuf, yypvt[-0].s) );
		}
		break;
	case 2099:
#line 14275 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2100:
#line 14281 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( 0 );		/* null indicator */
		    arg_str_add( ARG_PACK, yypvt[-0].s );
		}
		break;
	case 2102:
#line 14291 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    arg_int_add( 0 );
		}
		break;
	case 2103:
#line 14296 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    arg_int_add( 0 );
		}
		break;
	case 2104:
#line 14301 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 14326 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 14339 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_type = T_INT;
		}
		break;
	case 2107:
#line 14343 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_type = T_FLOAT;
		}
		break;
	case 2108:
#line 14347 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_type = T_PACKASCHAR;
		}
		break;
	case 2109:
#line 14351 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_type = T_CHAR;
		}
		break;
	case 2110:
#line 14358 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 14438 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 14449 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 14465 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = G4CT_PROC;
		}
		break;
	case 2131:
#line 14469 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = G4CT_FRAME;
		}
		break;
	case 2134:
#line 14480 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 14494 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 14510 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add(ARG_CHAR, yypvt[-2].s);
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 2137:
#line 14515 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add(ARG_CHAR, yypvt[-2].s);
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 2138:
#line 14520 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 14529 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add(ARG_CHAR, ERx(""));
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 2140:
#line 14536 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 14548 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 2142:
#line 14552 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 2144:
#line 14559 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call(IIG4RETVAL);
		}
		break;
	case 2145:
#line 14570 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IIG4CLRARR );
		}
		break;
	case 2146:
#line 14576 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2147:
#line 14587 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IIG4DESCRIBE );
		}
		break;
	case 2148:
#line 14595 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2149:
#line 14615 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
			if (eq->eq_flags & EQ_CHRPAD)
			    esq_sqmods(IImodNOCPAD);
			gen_if( G_CLOSE, IIG4CHKOBJ, C_EQ, 0);
		}
		break;
	case 2150:
#line 14623 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 14648 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2152:
#line 14654 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call(IIG4USEDESCR);
		}
		break;
	case 2154:
#line 14661 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call(IIG4GETATTR);
		}
		break;
	case 2155:
#line 14665 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call(IIG4GETATTR);
		}
		break;
	case 2156:
#line 14676 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 14702 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2158:
#line 14714 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_INT, yypvt[-2].s);
		    arg_str_add( ARG_INT, yypvt[-1].s);
		    gen_call( IIG4GETROW );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		}
		break;
	case 2159:
#line 14724 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		}
		break;
	case 2160:
#line 14732 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 14752 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		}
		break;
	case 2163:
#line 14758 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
                    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		    gr->gr_func = IIINQ4GL;
		}
		break;
	case 2166:
#line 14771 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call(IIINQ4GL);
		}
		break;
	case 2167:
#line 14782 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 14821 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2169:
#line 14827 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 14840 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    INSROW4GL_PARMS *prmptr = (INSROW4GL_PARMS *)(yypvt[-2].ip);

		    set_insrow_param(yypvt[-0].s, gr->gr_id, gr->gr_type, 
				     gr->gr_sym, prmptr);
		    yyval.ip = yypvt[-2].ip;
		}
		break;
	case 2171:
#line 14850 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 2172:
#line 14862 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IIG4REMROW );
		}
		break;
	case 2173:
#line 14868 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2174:
#line 14891 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IIG4CALLCOMP );
		    gen_if( G_CLOSE, IISENDUSEREVENT, C_EQ, 0);
		}
		break;
	case 2175:
#line 14898 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
                    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-1].s, NULL );
		    gen_if( G_OPEN, IISENDUSEREVENT, C_EQ, 0);
		}
		break;
	case 2179:
#line 14912 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add(ARG_CHAR, yypvt[-2].s);
		    args_toend(1, 2);
		    gen_call(IIG4VALPARAM);
		}
		break;
	case 2180:
#line 14926 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		}
		break;
	case 2181:
#line 14933 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
                    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		    gr->gr_func = IISET4GL;
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		}
		break;
	case 2184:
#line 14946 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call(IISET4GL);
		}
		break;
	case 2185:
#line 14966 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
			gen_if( G_CLOSE, IIG4CHKOBJ, C_EQ, 0);
		}
		break;
	case 2186:
#line 14974 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 15000 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2188:
#line 15005 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call(IIG4USEDESCR);
		}
		break;
	case 2190:
#line 15012 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call(IIG4SETATTR);
		}
		break;
	case 2191:
#line 15016 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call(IIG4SETATTR);
		}
		break;
	case 2192:
#line 15027 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 15040 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		}
		break;
	case 2194:
#line 15053 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IIG4SETROW );
		}
		break;
	case 2195:
#line 15059 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2196:
#line 15065 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (STbcompare(ERx("row"), 0, yypvt[-3].s, 0, TRUE) != 0)
		    {
			    er_write( E_EQ0081_TL_ATTR_UNKNOWN, EQ_ERROR, 1,
								yypvt[-3].s);
		    }
		}
		break;
	case 2197:
#line 15080 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_call( IIG4DELROW );
		}
		break;
	case 2198:
#line 15086 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2199:
#line 15102 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (STbcompare(ERx("rownumber"), 0, yypvt[-3].s, 0, TRUE) != 0)
		    {
			    er_write( E_EQ0081_TL_ATTR_UNKNOWN, EQ_ERROR, 1,
								yypvt[-3].s);
		    }
		}
		break;
	case 2200:
#line 15113 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_str_add( ARG_INT, yypvt[-0].s);
		}
		break;
	case 2201:
#line 15120 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2202:
#line 15124 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2203:
#line 15131 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add(0);	/* Null indicator */
		    arg_var_add( gr->gr_sym, gr->gr_id );
		}
		break;
	case 2204:
#line 15139 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 15154 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 2206:
#line 15158 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    arg_int_add(0);
		    yyval.i = 0;
		}
		break;
	case 2208:
#line 15170 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = NULL;
		}
		break;
	case 2209:
#line 15174 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 15186 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		}
		break;
	case 2211:
#line 15189 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_type = T_INT;
		    gr->gr_id = yypvt[-0].s;
		    gr->gr_sym = NULL;
		}
		break;
	case 2212:
#line 15195 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_type = T_FLOAT;
		    gr->gr_id = yypvt[-0].s;
		    gr->gr_sym = NULL;
		}
		break;
	case 2213:
#line 15201 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_type = T_PACKASCHAR;
		    gr->gr_id = yypvt[-0].s;
		    gr->gr_sym = NULL;
		}
		break;
	case 2214:
#line 15207 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_type = T_CHAR;
		    gr->gr_id = yypvt[-0].s;
		    gr->gr_sym = NULL;
		}
		break;
	case 2215:
#line 15216 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 15242 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    arg_int_add( 0 );
		}
		break;
	case 2220:
#line 15247 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    arg_int_add( 0 );
		}
		break;
	case 2221:
#line 15252 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 15272 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
#line 15285 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_type = T_INT;
		}
		break;
	case 2224:
#line 15289 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_type = T_FLOAT;
		}
		break;
	case 2225:
#line 15293 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_type = T_PACKASCHAR;
		}
		break;
	case 2226:
#line 15297 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_type = T_CHAR;
		}
		break;
	case 2227:
#line 15304 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    i4  code;

		    code = g4_check(gr->gr_func, yypvt[-1].s, (bool)yypvt[-0].i);
		    arg_int_add(code);
		}
		break;
	case 2228:
#line 15313 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2229:
#line 15317 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2230:
#line 15358 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 2233:
#line 15366 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    db_key(yypvt[-2].s);
		    db_op(yypvt[-1].s);
		    db_op(yypvt[-0].s);
		}
		break;
	case 2234:
#line 15374 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    id_add( yypvt[-0].s );
		}
		break;
	case 2246:
#line 15401 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    sc_moresql();
		    dml->dm_exec = DML_HOST;
		}
		break;
	case 2248:
#line 15417 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if ((gr->gr_flag & GR_HOSTCODE) == 0)
		    {
			gen_line( ERx("host code") );
			gr->gr_flag |= GR_HOSTCODE;
		    }
		    gen_host( G_H_CODE, yypvt[-0].s );
		    dml->dm_exec = DML_HOST;
		}
		break;
	case 2249:
#line 15434 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2250:
#line 15438 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2251:
#line 15442 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2253:
#line 15456 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Accept either EXEC SQL | 4GL BEGIN DECLARE */

		    if ((dml->dm_exec != (DML_EXEC|DML__SQL)) &&
		        (dml->dm_exec != (DML_EXEC|DML__4GL)))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"), 
				ERx("BEGIN DECLARE") );
		    dml->dm_exec = DML_DECL;
		    gr->C_rec = 0;
		    gr->C_size = 0;
		    gr->C_indir = 0;
		    gr->C_dec = GR_C_DEC;	/* Maybe dec's to follow */
		    gr->C_varcharbyte = 0;
		    gr->gr_type = T_INT;
		    gr->C_storage = T_NONE;
		    gr->C_struct = (SYM *)0;
		    eq->eq_flags |= EQ_INDECL;	/* Scanner in declare mode */
		}
		break;
	case 2254:
#line 15476 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
                    /* Accept either EXEC SQL | 4GL END DECLARE */

                    if ((dml->dm_exec != (DML_EXEC|DML__SQL)) &&
                        (dml->dm_exec != (DML_EXEC|DML__4GL)))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"), 
				ERx("END DECLARE") );
		    dml->dm_exec = DML_HOST;
		    /* Some interface file may need to be included */
		    gen_declare();
		    gr->C_dec = GR_C_USE;
		    gr->gr_type = T_UNDEF;
		    eq->eq_flags &= ~EQ_INDECL;	/* Scanner exits declare mode */
		}
		break;
	case 2256:
#line 15493 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->C_rec = 0;
		    gr->C_size = 0;
		    gr->C_indir = 0;
		    gr->C_dec = GR_C_DEC;	/* Maybe dec's to follow */
		    gr->gr_type = T_INT;
		    gr->C_storage = T_NONE;
		    gr->C_struct = (SYM *)0;
		    gr->C_varcharbyte = 0;
		}
		break;
	case 2263:
#line 15511 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    dml->dm_exec = DML_DECL;
		    gen_host( G_H_OP|G_H_NEWLINE, ERx("...DECLARE ERROR;") );
		}
		break;
	case 2265:
#line 15518 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host(G_H_KEY, yypvt[-0].s);
		}
		break;
	case 2266:
#line 15522 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host(G_H_KEY, yypvt[-0].s);
		}
		break;
	case 2267:
#line 15535 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->C_dec = GR_C_DEC;	/* Dec's to follow */
		    gr->gr_type = T_INT;
		    gr->C_size = sizeof(i4);
		}
		break;
	case 2269:
#line 15543 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );  
		}
		break;
	case 2270:
#line 15547 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->C_varcharbyte = T_VCH;
		}
		break;
	case 2271:
#line 15551 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->C_varcharbyte = T_VBYTE;
		}
		break;
	case 2272:
#line 15555 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->C_varcharbyte = T_NVCH;
		}
		break;
	case 2273:
#line 15559 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->C_storage = T_TYPEDEF;
		}
		break;
	case 2274:
#line 15564 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );  
		    gr->C_storage = T_EXTERN;
		}
		break;
	case 2275:
#line 15569 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );  
		    gr->C_storage = T_EXTERN;
		    gr->C_varcharbyte = T_VCH;
		}
		break;
	case 2276:
#line 15575 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );  
		    gr->C_storage = T_EXTERN;
		    gr->C_varcharbyte = T_VBYTE;
		}
		break;
	case 2277:
#line 15581 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );  
		    gr->C_storage = T_EXTERN;
		    gr->C_varcharbyte = T_NVCH;
		}
		break;
	case 2278:
#line 15587 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->C_varcharbyte = T_VCH;
		}
		break;
	case 2279:
#line 15591 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->C_varcharbyte = T_VBYTE;
		}
		break;
	case 2280:
#line 15595 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->C_varcharbyte = T_NVCH;
		}
		break;
	case 2281:
#line 15600 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /*
		    ** Under the -compatlib flag special case the words:
		    **	GLOBALREF, GLOBALDEF
		    ** for compat.h
		    */
		    if (eq->eq_flags & EQ_COMPATLIB)
		    {
			if (STbcompare(yypvt[-0].s, 0, ERx("globalref"), 0, TRUE) == 0)
			    gen_host(G_H_KEY, ERx("GLOBALREF"));
			else
			 if (STbcompare(yypvt[-0].s, 0, ERx("globaldef"), 0, TRUE)== 0)
			    gen_host(G_H_KEY, ERx("GLOBALDEF"));
			else
			 if (STbcompare(yypvt[-0].s, 0, ERx("globalconstref"), 0, TRUE)== 0)
			    gen_host(G_H_KEY, ERx("GLOBALCONSTREF"));
			else
			 if (STbcompare(yypvt[-0].s, 0, ERx("globalconstdef"), 0, TRUE)== 0)
			    gen_host(G_H_KEY, ERx("GLOBALCONSTDEF"));
			else
			    gen_host(G_H_KEY, yypvt[-0].s);  
		    }
		    else
			gen_host(G_H_KEY, yypvt[-0].s);  
		}
		break;
	case 2292:
#line 15644 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
			**	typedef char	Buf[10];
			**	Buf	b;	-- should have dimension 10
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
	case 2293:
#line 15674 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2294:
#line 15679 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2295:
#line 15684 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2296:
#line 15689 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2297:
#line 15694 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2298:
#line 15699 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = ERx("int");
		}
		break;
	case 2299:
#line 15704 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2300:
#line 15710 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2301:
#line 15716 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2302:
#line 15722 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2303:
#line 15728 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2304:
#line 15734 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-1].s;
		}
		break;
	case 2305:
#line 15740 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-1].s;
		}
		break;
	case 2306:
#line 15746 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2307:
#line 15751 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2308:
#line 15756 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = ERx("double");
		}
		break;
	case 2309:
#line 15762 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = ERx("i8");
		}
		break;
	case 2310:
#line 15768 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-2].s );
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = ERx("u_i8");
		}
		break;
	case 2311:
#line 15775 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2312:
#line 15783 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    er_write( E_EQ0501_hBADTYPE, EQ_ERROR, 2, ERx("C"), yypvt[-0].s );
		    sc_eat( gen_code, SC_NEST|SC_BACK, ERx(";"), '{', '}' );
		    gr->C_dec = GR_C_DEC;	/* Dec's to follow */
		}
		break;
	case 2313:
#line 15821 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->C_size = sizeof(i4);
		    gr->C_indir = 0;
		    gr->gr_type = T_INT;
		}
		break;
	case 2314:
#line 15828 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->C_size = sizeof(i4);
		    gr->C_indir = 0;
		    gr->gr_type = T_INT;
		}
		break;
	case 2315:
#line 15835 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->C_dec = GR_C_TAG;	/* Get ready for enum tag */
		    gen_host( G_H_KEY, yypvt[-0].s );
		    if (gr->C_varcharbyte != 0)
		    {
			er_write( E_E00008_hcVCHIGNRE, EQ_ERROR, 0 );
			gr->C_varcharbyte = 0;
		    }
		}
		break;
	case 2316:
#line 15846 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    SYM		*sy;

		    gen_host( G_H_KEY, yypvt[-0].s );
		    /* Declare the tag and mark as integer */
		    sy = symDcEuc( yypvt[-0].s, gr->C_rec, C_BLOCK, syFisTAG,
			             C_CLOSURE, SY_TAG );
		    if (sy)
			sym_s_btype( sy, T_INT );
		    gr->C_dec = GR_C_DEC;
		}
		break;
	case 2317:
#line 15858 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /*
		    ** Probably will be redeclaration to compiler but
		    ** we will ignore it.
		    */
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->C_dec = GR_C_DEC;
		}
		break;
	case 2318:
#line 15867 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->C_dec = GR_C_DEC;
		}
		break;
	case 2319:
#line 15872 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->C_dec = GR_C_DEC;
		}
		break;
	case 2320:
#line 15878 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_NEWLINE|G_H_OUTDENT, (char *)0 );
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 2321:
#line 15884 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_OP|G_H_NEWLINE|G_H_INDENT, yypvt[-0].s );
		}
		break;
	case 2324:
#line 15892 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host(G_H_KEY, yypvt[-0].s);
		}
		break;
	case 2326:
#line 15898 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    SYM		*sy;

		    gen_host(G_H_KEY, yypvt[-0].s);
		    /* Not recorded as child of anything -- orphans */
		    if (sy = symDcEuc(yypvt[-0].s, 0, C_BLOCK,
				syFisORPHAN|syFisCONST, C_CLOSURE, SY_NORMAL))
		    {
			sym_s_btype( sy, T_INT );
			sym_s_dsize(sy, sizeof(i4));
		    }
		}
		break;
	case 2327:
#line 15912 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 2328:
#line 15941 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_type = T_STRUCT;
		}
		break;
	case 2329:
#line 15946 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_type = T_STRUCT;
		}
		break;
	case 2330:
#line 15951 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_type = T_STRUCT;
		}
		break;
	case 2331:
#line 15956 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Repeated structure tag - legal in C */
		    gr->gr_type = T_STRUCT;
		}
		break;
	case 2332:
#line 15962 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->C_dec = GR_C_TAG;		/* Get ready for tag */
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2333:
#line 15967 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->C_dec = GR_C_TAG;		/* Get ready for tag */
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2334:
#line 15973 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->C_struct = symDcEuc( yypvt[-0].s, gr->C_rec, C_BLOCK, syFisTAG,
			             C_CLOSURE, SY_TAG );
		    gr->C_dec = GR_C_DEC;
		}
		break;
	case 2335:
#line 15980 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->C_struct = symDcEuc( ERx(""), gr->C_rec, C_BLOCK, 
					syFisTAG, C_CLOSURE, SY_TAG );
		    gr->C_dec = GR_C_DEC;
		}
		break;
	case 2336:
#line 15987 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->C_struct =
			 sym_resolve((SYM *)0, yypvt[-0].s, C_CLOSURE, syFisTAG) ;
		    gr->C_dec = GR_C_DEC;
		}
		break;
	case 2337:
#line 15995 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    gr->C_struct = symDcEuc( yypvt[-0].s, gr->C_rec, C_BLOCK, syFisTAG, 
				     C_CLOSURE, SY_TAG );
		    gr->C_dec = GR_C_DEC;
		}
		break;
	case 2339:
#line 16006 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_OP|G_H_NEWLINE|G_H_INDENT, yypvt[-0].s );
		    gr->C_rec++;
		    /* 
		    ** Push the struct entry in case there are nested structs,
		    ** so that variables of this struct type can be set.
		    ** The struct pointer is popped on the right brace.
		    */
		    sym_fpush( gr->C_struct );
		    /*
		    ** Save info about outer varchar (or varbyte).
		    ** Note that C_struct may be null if redecl.
		    */
		    if (gr->C_struct && gr->C_varcharbyte != 0)
			sym_s_btype( gr->C_struct, gr->C_varcharbyte ); 
		    gr->C_varcharbyte = 0;
		}
		break;
	case 2340:
#line 16025 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->C_rec--;
		    gr->C_struct = sym_fpop();
		    gen_host( G_H_KEY|G_H_OUTDENT, yypvt[-0].s );
		    if (gr->C_struct)	/* Could be null if redecl */
		    {
			if (sym_g_btype(gr->C_struct) == T_VCH)
			{
			    gr->C_varcharbyte = T_VCH;
			    sym_s_btype( gr->C_struct, T_STRUCT ); 
			}
			else if (sym_g_btype(gr->C_struct) == T_VBYTE)
			{
			    gr->C_varcharbyte = T_VBYTE;
			    sym_s_btype( gr->C_struct, T_STRUCT ); 
			}
			else if (sym_g_btype(gr->C_struct) == T_NVCH)
			{
			    gr->C_varcharbyte = T_NVCH;
			    sym_s_btype(gr->C_struct, T_STRUCT); 
			}
			else
			{
			    gr->C_varcharbyte = 0; /* Reset nested varthings */
			}
		    }
		}
		break;
	case 2343:
#line 16065 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_type = T_NONE;
		    gr->C_struct = (SYM *)0;
		    gr->C_size = 0;
		    gr->C_indir = 0;
		    gr->C_varcharbyte = 0;
		}
		break;
	case 2345:
#line 16074 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    dml->dm_exec = DML_DECL;
		    gen_host( G_H_OP|G_H_NEWLINE, ERx("...FIELD ERROR;") );
		    gr->gr_type = T_NONE;
		    gr->C_struct = (SYM *)0;
		    gr->C_size = 0;
		    gr->C_indir = 0;
		    gr->C_varcharbyte = 0;
		}
		break;
	case 2368:
#line 16108 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->C_varcharbyte = T_VCH;
		}
		break;
	case 2369:
#line 16112 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->C_varcharbyte = T_VBYTE;
		}
		break;
	case 2370:
#line 16116 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->C_varcharbyte = T_NVCH;
		}
		break;
	case 2371:
#line 16121 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /*
		    ** Varchar storage class ONLY allowed with variable decls.
		    ** Disallow with structure tags (and no vars) but give
		    ** error.
		    */
		    if (gr->C_varcharbyte != 0)
		    {
			er_write( E_E00008_hcVCHIGNRE, EQ_ERROR, 0 );
			gr->C_varcharbyte = 0;
		    }
		}
		break;
	case 2373:
#line 16136 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2376:
#line 16144 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    /* Now strip off any C initialization clause */
		    sc_eat( gen_code, SC_NEST|SC_BACK, ERx(";,"), '{', '}' );
		}
		break;
	case 2377:
#line 16150 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    /* Now strip off any C bitfield clause */
		    sc_eat( gen_code, SC_NEST|SC_BACK, ERx(";,"), '(', ')' );
		}
		break;
	case 2378:
#line 16156 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if ((eq->eq_flags & EQ_COMPATLIB) &&
			    STbcompare(yypvt[-0].s, 0, ERx("ZERO_FILL"), 0, 0)==0)
			;
		    else
			er_write( E_E00009_hcMISSEQUAL, EQ_ERROR, 0 );
		}
		break;
	case 2380:
#line 16166 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
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
		    if ((cv = gr->C_cvals) == NULL &&
			(gr_type == T_CHAR || gr_type == T_WCHAR) &&
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
		    if ((gr_type == T_CHAR || gr_type == T_WCHAR) &&
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

		    /*
		    ** Save SQLCA variables when declared in form:
		    **
		    ** IISQLCA *var;
		    */
		    if ( type_bit == syFisVAR  &&  gr_type == T_SQLCA  &&
			 yypvt[-2].i == 1  &&  ! yypvt[-0].i  &&  ! gr->C_rec )
		    {
			if ( sqlca_var )  MEfree( (PTR)sqlca_var );
			sqlca_var = STalloc( yypvt[-1].s );
		    }

		  /* hint for ESQL redeclaration errors */
		    sym_hint_type( gr->C_struct, gr_type, cindir );
		    if (sy = symDcEuc(yypvt[-1].s, gr->C_rec, C_BLOCK, type_bit, 
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
			    if (gr->C_varcharbyte != 0) 
			    { 
				SYM	*sib1, *sib2; 

				if (sib1 = syStrInit(gr->C_struct))
				{
				    if (!(sym_g_btype(sib1) == T_INT) ||
					!(sym_g_dsize(sib1) == 2))
				    {
					er_write( E_E00008_hcVCHIGNRE, 
						EQ_ERROR, 0 );
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
					    if ((sym_g_btype(sib2) == T_CHAR ||
					        sym_g_btype(sib2) == T_WCHAR) &&
						sym_g_indir(sib2) == 0 &&
						sym_g_vlue(sib2))
					    {
						sym_s_btype( sy, 
						    gr->C_varcharbyte );
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
			else if (gr->C_varcharbyte != 0)/* Vchr must be strct */
			    er_write( E_E00008_hcVCHIGNRE, EQ_ERROR, 0 );
		    }
		}
		break;
	case 2381:
#line 16330 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    if (!(eq->eq_flags & EQ_NOSQLCODE)
		      && (STbcompare(yypvt[-0].s, 0, ERx("SQLCODE"), 0, FALSE) == 0))
		    {
			if ((gr->gr_type != T_INT) || (gr->C_size != 4))
			    er_write( E_EQ0517_SQLSTATUS_DECL, EQ_ERROR, 2, 
#if defined(LP64) 
			      ERx("SQLCODE"), ERx("int SQLCODE;"));
#else
			      ERx("SQLCODE"), ERx("long SQLCODE;"));
#endif
			else
			    eq->eq_flags |= EQ_SQLCODE;
		    }
		    if (STbcompare(yypvt[-0].s, 0, ERx("SQLSTATE"), 0, FALSE) == 0)
		    {
			if (gr->gr_type != T_CHAR)
			    er_write( E_EQ0517_SQLSTATUS_DECL, EQ_ERROR, 2, 
				ERx("SQLSTATE"), ERx("char SQLSTATE[6];"));
			else
			    eq->eq_flags |= EQ_SQLSTATE;

		    }
		    yyval.s = yypvt[-0].s;
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2382:
#line 16359 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (yypvt[-0].i)
		    {
			gen_host( G_H_OP, id_getname() );
			id_free();
		    }
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 2383:
#line 16369 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 2384:
#line 16373 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-1].i + yypvt[-0].i;
		}
		break;
	case 2385:
#line 16378 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 1;
		    id_add( yypvt[-0].s );
		}
		break;
	case 2386:
#line 16383 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 2;
		    id_add( yypvt[-0].s );
		}
		break;
	case 2387:
#line 16389 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (yypvt[-0].i)
		    {
			gen_host( G_H_OP, id_getname() );
			/*
			** If the array list eater changes then change the
			** cv_dims stuff in the Cdec_var rule.
			** Don't id_free here.
			*/
		    }
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 2388:
#line 16403 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 2389:
#line 16407 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		  /* Eat up the array dimension and save it - nest them */
		    sc_eat( id_add, SC_SEEN|SC_NEST, ERx("]"), '[', ']' );
		    yyval.i = yypvt[-1].i +1;
		}
		break;
	case 2390:
#line 16414 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    SYM		*sy;

		    if (gr->gr_type == T_CHAR)
			gen_host( G_H_SCONST|G_H_NEWLINE, yypvt[-0].s );
		    else
			gen_host( G_H_KEY|G_H_NEWLINE, yypvt[-0].s );

		    if (gr->gr_type != T_NONE)
		    {
		      /* hint for ESQL redeclaration errors */
			sym_hint_type( (SYM *)0, gr->gr_type, 0 );
			if (sy = symDcEuc(yypvt[-2].s, 0, C_BLOCK, syFisCONST, 
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
	case 2391:
#line 16438 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->C_dec = GR_C_DEC;	/* Dec's to follow */
		    gen_host( G_H_OP, ERx("#") );
		    gen_host( G_H_KEY, ERx("define") );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2393:
#line 16448 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    er_write( E_EQ0070_grMISSWORD, EQ_ERROR, 2, ERx("#"), yypvt[-0].s );
		}
		break;
	case 2394:
#line 16453 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    sc_eat( NULL, SC_SEEN|SC_NEST, ERx(")"), '(', ')' );
		    er_write( E_E00002_hcIGNCAST, EQ_ERROR, 0 );
		}
		break;
	case 2396:
#line 16460 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2397:
#line 16464 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2398:
#line 16469 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_type = T_INT;
		    gr->C_size = sizeof(i4);
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2399:
#line 16475 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    char	ibuf[50];

		    ibuf[0] = '-';
		    ibuf[1] = '\0';
		    yyval.s = str_add( STRNULL, STcat(ibuf, yypvt[-0].s) );
		    gr->gr_type = T_INT;
		    gr->C_size = sizeof(i4);
		}
		break;
	case 2400:
#line 16486 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_type = T_FLOAT;
		    gr->C_size = sizeof(f8);
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2401:
#line 16492 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    char	fbuf[50];

		    fbuf[0] = '-';
		    fbuf[1] = '\0';
		    yyval.s = str_add( STRNULL, STcat(fbuf, yypvt[-0].s) );
		    gr->gr_type = T_FLOAT;
		    gr->C_size = sizeof(f8);
		}
		break;
	case 2402:
#line 16502 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_type = T_FLOAT;
		    gr->C_size = sizeof(f8);
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2403:
#line 16508 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    char	dbuf[50];

		    dbuf[0] = '-';
		    dbuf[1] = '\0';
		    yyval.s = str_add( STRNULL, STcat(dbuf, yypvt[-0].s) );
		    gr->gr_type = T_FLOAT;
		    gr->C_size = sizeof(f8);
		}
		break;
	case 2404:
#line 16518 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_type = T_CHAR;
		    gr->C_size = STlength( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2405:
#line 16524 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_type = T_NONE;
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2406:
#line 16529 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_type = T_NONE;
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2407:
#line 16535 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		    gen_host( G_H_NEWLINE, (char *)0 );
		}
		break;
	case 2408:
#line 16549 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* 
		    ** Strip to end of line, but ensure scanner is synchronized
		    ** via 'lex_need_input' - see yylex routines.  This is
		    ** because the newline is the EXEC trigger.
		    */
		    sc_reset();
		    if (inc_parse_name(yypvt[-1].s, gr->gr_adjective) == SC_INC)
			inc_push_file();
		    gr->gr_adjective = 0;
		}
		break;
	case 2409:
#line 16561 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Better be "<EOF>" else Yacc will produce syntax error */
		    dml->dm_exec = DML_DECL;
		}
		break;
	case 2410:
#line 16567 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (dml->dm_exec != (DML_EXEC|DML__SQL))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"), 
				ERx("INCLUDE") );
		    dml->dm_exec = DML_DECL;
		}
		break;
	case 2411:
#line 16575 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_adjective = FALSE;
		}
		break;
	case 2412:
#line 16580 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_adjective = FALSE;
		}
		break;
	case 2413:
#line 16585 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    STprintf( esq->sbuf, ERx("%s.%s"), yypvt[-1].s, yypvt[-0].s );
		    yyval.s = esq->sbuf;
		    gr->gr_adjective = FALSE;
		}
		break;
	case 2414:
#line 16591 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    STprintf( esq->sbuf, ERx("%s.%s"), yypvt[-2].s, yypvt[-0].s );
		    yyval.s = esq->sbuf;
		    gr->gr_adjective = FALSE;
		}
		break;
	case 2415:
#line 16598 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* Eat the name till right angle bracket */
		    sc_eat(id_add, SC_STRIP, ERx(">"), '<', '>');
		    yyval.s = str_add(STRNULL, id_getname());
		    id_free();
		    gr->gr_adjective = TRUE;
		}
		break;
	case 2416:
#line 16613 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    dml->dm_exec = DML_DECL;
		}
		break;
	case 2417:
#line 16618 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    esq->flag |= ESQ_NOPRT;     /* Suppress printing */
		    if (dml->dm_exec != (DML_EXEC|DML__SQL))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"), 
				ERx("DECLARE TABLE") );
		}
		break;
	case 2418:
#line 16625 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    esq->flag |= ESQ_NOPRT;     /* Suppress printing */
		    if (dml->dm_exec != (DML_EXEC|DML__SQL))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"), 
				ERx("DECLARE TABLE") );
		}
		break;
	case 2424:
#line 16663 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    if (dml->dm_exec != (DML_EXEC|DML__SQL))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"), 
				ERx("EXCLUDE SECTION") );
		    dml->dm_exec = DML_HOST;
		}
		break;
	case 2427:
#line 16676 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    dml->dm_exec = DML_DECL;
		}
		break;
	case 2428:
#line 16706 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->C_dec = GR_C_USE;
		    sym_f_init();
		    sc_hostvar = TRUE;  /* disable keyword lookups */
		}
		break;
	case 2430:
#line 16715 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->C_dec = GR_C_USE;
		    sym_f_init();
		    er_write( E_EQ0129_sqVARCOLON, EQ_ERROR, 0 );
		}
		break;
	case 2433:
#line 16738 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    SYM 	*sy;

		    gr->gr_type = T_UNDEF;
		    if ((sy = yypvt[-0].v) == (SYM *)0)
		    {
			/* Do nothing - error already printed */
		    } else 
		    {
			if ((sym_g_useof(sy) & syFisVAR) == 0 || 
			     sym_g_btype(sy) == T_STRUCT)
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
	case 2436:
#line 16772 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->C_dec = GR_C_USE;
		    sym_f_init();
		}
		break;
	case 2437:
#line 16779 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    SYM 	*sy;

		    gr->gr_type = T_UNDEF;
		    if ((sy = yypvt[-0].v) == (SYM *)0)
		    {
			/* Do nothing - error already printed */
		    } else 
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
	case 2438:
#line 16825 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    sc_hostvar = FALSE;    /* re-enable keyword lookups */
		}
		break;
	case 2439:
#line 16829 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    sc_hostvar = FALSE;  /* re-enable keyword lookups */
		    sc_popscptr();	 /* move SC_PTR back one word */
		}
		break;
	case 2440:
#line 16835 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    gr->gr_id = str_add( STRNULL, id_getname() );
		    id_free();
		    if (yypvt[-2].i && yypvt[-1].i)
			gr->gr_id += 2;		/* skip over the "&*" */
		}
		break;
	case 2441:
#line 16843 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    sc_hostvar = TRUE;  /* disable keyword lookups */
		}
		break;
	case 2443:
#line 16850 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    id_add( yypvt[-0].s );
		    yyval.i = 1;
		}
		break;
	case 2444:
#line 16855 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 2449:
#line 16874 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    id_add( yypvt[-0].s );
		}
		break;
	case 2450:
#line 16890 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    SYM 	*sy, *mem;
		    char	*sid, *mid;

		    sid = str_add( STRNULL, id_getname() );
		    if ((sy = yypvt[-0].v) == (SYM *)0)
		    {
			/* Do nothing, as error already printed */
		    }
		    else if (sym_g_btype(sy) == T_STRUCT)
		    {
			for (mem=syStrInit(sy); mem; mem=syStrNext(mem))
			{
			    /* Parens on struct name in case it's a pointer */
			    STprintf(esq->sbuf, ERx("(%s).%s"), sid, 
				     sym_str_name(mem));
			    mid = str_add( STRNULL, esq->sbuf );
			    if ((sym_g_useof(mem) & syFisVAR) == 0 || 
				 sym_g_btype(mem) == T_STRUCT)
				er_write( E_EQ0057_grHOSTVAR, EQ_ERROR, 1, mid );
			    else if (sym_g_indir(mem))
				er_write(E_E00003_hcINDIR, EQ_ERROR, 3, mid, 
				    ERx("0"), er_na(sym_g_indir(mem)));
			    erec_mem_add( mid, mem, sym_g_btype(mem) );
			}
		    } else
		    {
			if ((sym_g_useof(sy) & syFisVAR) == 0)
			    er_write( E_EQ0057_grHOSTVAR, EQ_ERROR, 1, 
				    id_getname() );
			erec_mem_add( sid, sy, sym_g_btype(sy) );
		    }
		    id_free();
		}
		break;
	case 2451:
#line 16935 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    SYM 	*sy, *mem;
		    char	*sid, *mid;

		    sid = str_add( STRNULL, id_getname() );
		    if ((sy = yypvt[-0].v) == (SYM *)0)
		    {
			/* Do nothing, as error already printed */
		    } else if (sym_g_btype(sy) == T_STRUCT)
		    {
			for (mem=syStrInit(sy); mem; mem=syStrNext(mem))
			{
			    /* Parens on struct name in case it's a pointer */
			    STprintf(esq->sbuf, ERx("(%s).%s"), sid, 
				     sym_str_name(mem));
			    mid = str_add( STRNULL, esq->sbuf );
			    if (sym_g_btype(mem) == T_STRUCT)
				er_write(E_EQ0057_grHOSTVAR, EQ_ERROR, 1, mid);
			    else if (sym_g_indir(mem))
				er_write(E_E00003_hcINDIR, EQ_ERROR, 3, mid, 
				    ERx("0"), er_na(sym_g_indir(mem)));
			    erec_mem_add( mid, mem, sym_g_btype(mem) );
			    
			}
		    } else
		    {
			erec_mem_add( sid, sy, sym_g_btype(sy) );
		    }
		    id_free();
		}
		break;
	case 2452:
#line 16988 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    sc_hostvar = FALSE;  /* re-enable keyword lookups */
		}
		break;
	case 2453:
#line 16992 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    /* the tNAME might have been a real keyword */
		    sc_hostvar = FALSE;  /* re-enable keyword lookups */
		    sc_popscptr();       /* move SC_PTR back a word */
		}
		break;
	case 2454:
#line 16999 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    i4		cindir;
		    SYM		*sy;
		    i4		stat;

		    cindir = yypvt[-1].i + yypvt[-0].i;	/* **'s + []'s */
		    stat = symRsEuc( &sy,  C_CLOSURE, syFisVAR|syFisCONST );
		    if (stat != syL_OK || sy == (SYM *)0)
		    {
			/* 
			** This is not a var.
			** Only enter as undefined, if no entry, no indirection
			** and not a struct usage.
			*/
			if (gr->gr_type == T_STRUCT)
			    er_write( E_E00004_hcSTRUCTREF, EQ_ERROR, 1, 
				    id_getname() );
			else
			{
			    er_write( E_EQ0502_hNOVAR, EQ_ERROR, 2, ERx("C"), 
					id_getname() );
			    if (sy == (SYM *)0 && cindir == 0)
			    {
				/* hint for ESQL redeclaration errors */
				sym_hint_type( (SYM *)0, T_UNDEF, 0 );
				sy = symDcEuc(id_getname(), 0, C_BLOCK,
					      syFisVAR, C_CLOSURE, SY_NORMAL);
				if (sy)
				    sym_s_btype( sy, T_UNDEF );
			    }
			}
		    } else if (erec_vars() > 0)
		    {
			/*
			** If we are an indicator variable for a var which
			** could be a struct (currently SELECT, FETCH, or
			** INSERT) then allow an indirection mismatch of at
			** most 1, and add the indicator.
			** If the indirection matched, then we probably
			** shouldn't try indexing into the indicator var.
			*/
			switch (sym_g_indir(sy) - cindir)
			{
			  case 1:
			    erec_ind_add( C_NDX_BASE, id_getname(), C_NDX_EXPR,
				    sy );
			    break;
			  case 0:
			    erec_ind_add( C_NDX_BASE, id_getname(), NULL, sy );
			    break;
			  default:
			    /* Illegal indirection */
			    if ((gr->gr_flag & GR_NOINDIR) == 0)
			    {
				er_write( E_E00003_hcINDIR, EQ_ERROR, 3, 
				    id_getname(), er_na(cindir), 
				    er_na(sym_g_indir(sy)) );
			    }
			    break;
			}
		    } else if (sym_g_indir(sy) != cindir && 
			     (gr->gr_flag & GR_NOINDIR) == 0) 
		    {
			    /* Illegal indirection */
			    er_write( E_E00003_hcINDIR, EQ_ERROR, 3, 
				    id_getname(), er_na(cindir), 
				    er_na(sym_g_indir(sy)) );
		    }
		    yyval.v = sy;
		}
		break;
	case 2455:
#line 17070 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
 		    /*
 		    ** Kanji support - allow usages such as:
 		    ** 	EXEC SQL MESSAGE ERget(QBF_FETCHING, 1234)
 		    ** or:
 		    ** 	EXEC SQL ACTIVATE menuitem ERx(QBF_SAVE)
 		    ** Behave like system function "variables".
 		    */
 		    /* Eat up func args - allow nesting */
 		    id_add( yypvt[-1].s );
 		    sc_eat( id_add, SC_SEEN|SC_NEST, ERx(")"), '(', ')' );
 	    	    yyval.v = sym_resolve( (SYM *)0, yypvt[-1].s, C_CLOSURE, syFisVAR );
 		}
		break;
	case 2456:
#line 17090 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		    gr->gr_type = T_UNDEF;
		}
		break;
	case 2457:
#line 17095 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		    gr->gr_type = T_STRUCT;
		}
		break;
	case 2458:
#line 17100 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		    gr->gr_type = T_STRUCT;
		}
		break;
	case 2459:
#line 17106 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 2460:
#line 17111 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    id_add( yypvt[-0].s );
		    sym_fpush( sym_find(yypvt[-0].s) );
		}
		break;
	case 2462:
#line 17127 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    id_add( yypvt[-0].s );
		}
		break;
	case 2463:
#line 17132 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    id_add( yypvt[-0].s );
		}
		break;
	case 2464:
#line 17137 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 2465:
#line 17141 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 2466:
#line 17146 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 2467:
#line 17151 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    id_add( yypvt[-0].s );
		}
		break;
	case 2468:
#line 17155 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/csq/csq.yf"
{
		    id_add( yypvt[-0].s );
		}
		break;
		}
	return 0; /* Success */

	}
