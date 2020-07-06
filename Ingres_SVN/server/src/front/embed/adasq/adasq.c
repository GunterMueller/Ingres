
#line 6 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#define	tAACCESS		589
#define	tAARRAY		590
#define	tAARROW		591
#define	tAASSIGN		592
#define	tACASE		593
#define	tACONST		594
#define	tADELTA		595
#define	tADIGITS		596
#define	tADOTDOT		597
#define	tAFUNCTION		598
#define	tALIMITED		599
#define	tANEW		600
#define	tANULL		601
#define	tAOTHERS		602
#define	tAPACKAGE		603
#define	tAPIPE		604
#define	tAPRIVATE		605
#define	tARAISE		606
#define	tARANGE		607
#define	tARECORD		608
#define	tARENAMES		609
#define	tARETURN		610
#define	tASQL_STANDARD		611
#define	tASUBTYPE		612
#define	tATYPEDEF		613
#define	tSEMICOL		614
#define	tOR		615
#define	tAND		616
#define	tLT		617
#define	tGT		618
#define	tNE		619
#define	tLE		620
#define	tGE		621
#define	tEQOP		622
#define	tPLUS		623
#define	tMINUS		624
#define	tSTAR		625
#define	tSLASH		626
#define	tEXP		627
#define	tUNARYOP		628

#line 997 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
	

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


#line 1078 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
/* %L locals - Include L locals and global gr structure */
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
	i4      gr_rcnt;        /* count of result row entries (for RPPs) */

	/* Add L (ADA) dependent members after this statement */
	i4	A_type;		/* ADA base type (in declarations) */
	i4	A_rec;		/* ADA record level */
	i4	A_size;		/* Size of declaration */
	i4	A_dims;		/* number of array dimensions */
	i4	A_val;		/* Flags in st_value (really a i4  *) */
	i4	A_simpvar;	/* Is a simple variable */
	i4	A_err;		/* Var usage has an error */
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
struct gr_state tempgr;

/* Generate an error if no error is yet issued */
# define   ADA_ERR( x )				\
			if (!gr->A_err) {	\
			    gr->A_err = 1;	\
			    er_write x;		\
			} else			\

/* Declare variables that have been pushed on stack */
# define   ADA_DECLARE( count, typbits, typesy )		  \
		       ada_declare( count, gr->A_rec, A_BLOCK,  \
				    typbits, gr->A_dims, gr->A_val, typesy )
SYM	*ada_declare();

/* Where do Ada array indices start? */
# define A_ARR_BASE	1

/* How do you index an Ada array? */
# define A_ARR_EXPR	ERx("(%d)")

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

#line 17176 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"


/* %L mechanism */
/* %L mechanism begin - All the stuff to do with gr_mechanism() */

GLOBALDEF GR_ADTYPES   gr_typetab[] = {
  /* first 3 are fake; just for internal use */
 {ERx(".byte"),		T_INT,	sizeof(char),	EQ_ADALSYS|EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX},
 {ERx(".short"),	T_INT,	sizeof(short),	EQ_ADALSYS|EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX},
 {ERx(".undef"),	T_UNDEF,sizeof(int), 	EQ_ADALSYS|EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX},
  /* now the real ones */
 {ERx("character"),	T_CHAR,	0,   		EQ_ADALSYS|EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX},
 {ERx("string"),	T_CHAR,	0,   		EQ_ADALSYS|EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX},
  /* size of booleans is 1 byte on VMS */
 {ERx("boolean"),	T_INT,	sizeof(char),	EQ_ADALSYS|EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX},
#if !defined(axp_osf)
 {ERx("tiny_integer"),	T_INT,	sizeof(char),	EQ_ADVADS|EQ_ADGNAT	     },
#endif /* axp_osf */
 {ERx("short_short_integer"),T_INT,	sizeof(char),	EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX},
 {ERx("short_integer"),	T_INT, 	sizeof(char),	EQ_ADALSYS		     },
 {ERx("short_integer"),	T_INT, 	sizeof(short),	EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX },
 {ERx("integer"), 	T_INT, 	sizeof(short),	EQ_ADALSYS		     },
 {ERx("integer"), 	T_INT, 	sizeof(int),	EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX },
#if defined(axp_osf)
 {ERx("long_integer"),  T_INT,  sizeof(long),   EQ_ADVADS|EQ_ADGNAT          },
#endif /* axp_osf */
 {ERx("long integer"), 	T_INT, 	sizeof(int),	EQ_ADALSYS		     },
 {ERx("natural"), 	T_INT, 	sizeof(int),   	EQ_ADALSYS|EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX},
 {ERx("positive"), 	T_INT, 	sizeof(int),   	EQ_ADALSYS|EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX},
 {ERx("short_float"), 	T_FLOAT,sizeof(float),	EQ_ADVADS|EQ_ADGNAT  	     },
 {ERx("float"), 	T_FLOAT,sizeof(float),	EQ_ADALSYS|EQ_ADVAX|EQ_ADGNAT},
#if defined(axp_osf)
 {ERx("float"),         T_FLOAT,sizeof(float),  EQ_ADVADS|EQ_ADGNAT          },
 {ERx("long_float"),    T_FLOAT,sizeof(double), EQ_ADALSYS|EQ_ADVAX|EQ_ADVADS|EQ_ADGNAT},
 {ERx("long_long_float"),T_FLOAT,sizeof(double),EQ_ADVADS|EQ_ADGNAT          },
#else /* axp_osf */
 {ERx("float"),	 	T_FLOAT,sizeof(double),	EQ_ADVADS		     },
 {ERx("long_float"), 	T_FLOAT,sizeof(double),	EQ_ADALSYS|EQ_ADVAX	     },
#endif /* axp_osf */
 {ERx("f_float"), 	T_FLOAT,sizeof(float), 	EQ_ADVAX		     },
 {ERx("d_float"), 	T_FLOAT,sizeof(double),	EQ_ADVAX		     },
 {ERx("char"),		T_CHAR,	0,   		EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX },
 {ERx("smallint"),	T_INT, 	sizeof(short),	EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX },
 {ERx("int"), 		T_INT, 	sizeof(int),	EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX },
 {ERx("real"), 		T_FLOAT,sizeof(float),	EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX },
 {ERx("double_precision"),T_FLOAT,sizeof(double),EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX},
 {ERx("sqlcode_type"), 	T_INT,	sizeof(int),	EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX },
 {ERx("sqlstate_type"), T_CHAR, 0,		EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX },
 {ERx("indicator_type"),T_INT, 	sizeof(short),	EQ_ADVADS|EQ_ADGNAT|EQ_ADVAX },

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
 {(char *)0,		0,	0,		0			     }
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
i4	arg1;			/* Extra arguments for particular cases */
#if defined(axp_osf)
PTR	arg2;
PTR	arg3;
#else /* axp_osf */
i4	arg2;
i4	arg3;
#endif /* axp_osf */
{
    register SYM	*sy;
    static   i4	einit = 0;
    static   char	*patch_extension = "--"; /* Patched after SYMINIT */
  /* DML routines */
    extern   i4	gen__sqltab[];
    i4			scStrIsCont(),
			esq_repeat(),
			yyesqlex(),
			sc_iscomment();

    switch (flag)
    {
      case GR_EQINIT:
	eq->eq_lang = EQ_ADA;
	eq->eq_def_in = ERx("sa");
	eq->eq_def_out = patch_extension;
	if (!einit)
	{
	    eq->eq_in_ext = ERx("sa");
	    eq->eq_out_ext = patch_extension;
	    einit = 1;
	}
      /* tell the world we're Embedded SQL! */
	dml->dm_lang = DML_ESQL;
	dml->dm_exec = DML_HOST;
	dml->dm_gentab = gen__sqltab;
	dml->dm_lex = yyesqlex;
	dml->dm_strcontin = scStrIsCont;
	dml->dm_iscomment = sc_iscomment;
	dml->dm_repeat = esq_repeat; 	/* Provide hook for REPEATED queries */

	gr->gr_type = T_NONE;
	gr->gr_sym = (SYM *)0;
	gr->gr_id = ERx("");
	gr->gr_flag = GR_HOSTCODE;

      /* ADA specific flags */
	gr->A_rec = 0;
	gr->A_size = 0;
	gr->A_dims = 0;
	ada_init();

      /* For the SQL scanner ... */
	eq->eq_sql_quote = '\'';
	eq->eq_quote_esc = '"';
	eq->eq_host_quote = '"';

	esq->inc = 0;
	ecs_clear();
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

		sy = symDcEuc(gt->gt_id, 0, A_BLOCK,
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

		sy = symDcEuc(gt->gt_id, 0, A_BLOCK,
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

		sy = symDcEuc(gt->gt_id, 0, A_BLOCK,
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
	sy = symDcEuc( ERx(".null"), 0, A_BLOCK, syFisVAR|syFisSYS, 
		      A_CLOSURE, SY_NORMAL );
	sym_s_btype( sy, T_NUL );
	sym_s_dsize( sy, sizeof(i4) );
	gr->gr_null = sy;
	break;

      case GR_LOOKUP:
	/* How should we look up this name? */
	*(i4 *)arg2 = tNAME;
	break;

      case GR_NUMBER:		/* are we in a label declaration? */
	break;

      case GR_STMTFREE:
	ada_reset();		/* clear the "name" stack */
	str_reset();

	gr->gr_type = T_NONE;
	gr->gr_sym = (SYM *)0;

	gr->A_size = gr->A_rec = 0;
	gr->A_dims = 0;
	gr->A_val = 0;
	gr->A_err = 0;

	esq->flag = 0;
	break;

      case GR_ERROR:
	break;

      case GR_CLOSE:
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

	    SIfprintf( f, ERx("  A_type = %s, A_rec = %d\n"),
			trBaseType(gr->A_type), gr->A_rec );
	    SIfprintf( f, ERx("  A_size = %d, A_dims = %d, A_val = 0x%x\n"),
			gr->A_size, gr->A_dims, gr->A_val );
	    SIfprintf( f, ERx("  A_simpvar = %d, A_err = %d\n"),
			gr->A_simpvar, gr->A_err  );

	    SIfprintf( f, ERx("ESQ: esq_ \n") );
	    SIfprintf( f, ERx("  flag = 0%o, inc = 0%o, dml_exec = 0%o\n"), 
		       esq->flag, esq->inc, dml->dm_exec );
	    SIflush( f );
	}
	break;

      case GR_BLOCK:	/* Enter/exit an EQUEL block - no symbol table stuff */
	break;

      case GR_EQSTMT:
	{
	    i4		mode = arg1;
	    char	*kword = (char *)arg2;
	    char	*kw2 = (char *)arg3;
	    char	buf[50];
	    static i4	in_include_err = FALSE;	/* Already complained */

	  /* Turn off declarations flag and generate line directive */
	    gr->gr_flag &= ~GR_HOSTCODE;

	    if (inc_is_nested() != SC_NOINC && !in_include_err)
	    {
		er_write( E_E60006_haINCPKG, EQ_ERROR, 1, kword );
		in_include_err = TRUE;
	    }

	    if ((mode == GR_sSQL || mode==GR_sREPEAT
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
	614, 829,
	-2, 831,
	-1, 379,
	458, 811,
	-2, 810,
	-1, 546,
	504, 238,
	-2, 1306,
	-1, 700,
	502, 984,
	-2, 977,
	-1, 702,
	502, 984,
	-2, 982,
	-1, 762,
	485, 555,
	499, 555,
	-2, 552,
	-1, 965,
	256, 1276,
	391, 1276,
	504, 1470,
	-2, 1468,
	-1, 966,
	256, 1277,
	391, 1277,
	504, 1471,
	-2, 1469,
	-1, 1016,
	504, 238,
	-2, 236,
	-1, 1093,
	504, 2409,
	-2, 2406,
	-1, 1094,
	504, 2410,
	-2, 2407,
	-1, 1095,
	504, 2411,
	-2, 2408,
	-1, 1240,
	504, 238,
	-2, 414,
	-1, 1469,
	503, 2050,
	-2, 2087,
	-1, 1500,
	622, 2140,
	-2, 1309,
	-1, 1608,
	485, 2453,
	-2, 2073,
	-1, 1609,
	485, 2453,
	-2, 2073,
	-1, 1610,
	485, 2453,
	-2, 2073,
	-1, 1612,
	485, 2453,
	-2, 2073,
	-1, 1614,
	485, 2453,
	-2, 2073,
	-1, 1619,
	485, 2453,
	-2, 2073,
	-1, 1780,
	485, 998,
	499, 998,
	506, 998,
	-2, 1276,
	-1, 1879,
	314, 1317,
	318, 1317,
	322, 1317,
	323, 1317,
	329, 1317,
	-2, 523,
	-1, 1884,
	485, 555,
	499, 555,
	-2, 548,
	-1, 1885,
	485, 555,
	499, 555,
	-2, 550,
	-1, 1973,
	501, 1787,
	614, 1787,
	-2, 1776,
	-1, 2088,
	485, 876,
	499, 876,
	506, 876,
	-2, 1276,
	-1, 2102,
	485, 1200,
	499, 1200,
	-2, 497,
	-1, 2108,
	502, 782,
	-2, 1276,
	-1, 2129,
	504, 1183,
	-2, 1181,
	-1, 2155,
	502, 781,
	-2, 1227,
	-1, 2366,
	485, 922,
	499, 922,
	506, 922,
	-2, 1276,
	-1, 2392,
	502, 1276,
	-2, 1282,
	-1, 2468,
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
	614, 1305,
	-2, 1309,
	-1, 2627,
	458, 776,
	502, 782,
	-2, 1276,
	-1, 2628,
	458, 777,
	-2, 720,
	-1, 2718,
	504, 1276,
	-2, 1293,
	-1, 2719,
	504, 1277,
	-2, 1294,
	-1, 2894,
	314, 1317,
	318, 1317,
	322, 1317,
	323, 1317,
	329, 1317,
	-2, 524,
	-1, 2901,
	485, 555,
	499, 555,
	-2, 528,
	-1, 3223,
	501, 442,
	503, 442,
	-2, 444,
	-1, 3224,
	501, 443,
	503, 443,
	-2, 445,
	-1, 3295,
	336, 596,
	485, 555,
	499, 555,
	-2, 528,
	-1, 3417,
	339, 1362,
	-2, 1378,
	-1, 3603,
	339, 1363,
	-2, 1379,
};

#define	YYNPROD		2479
#define	YYLAST		10516

short yyact[] = {
	  2286,  2750,  3698,  3814,  3840,   514,  3074,  3817,  3822,  3381,
	  1083,  2600,  3500,  2599,  3081,  2704,  3604,  3824,  1021,  1595,
	  3677,  3101,  1691,   700,  3258,  3601,  3764,  3388,  3841,  2298,
	  2762,  1951,  2127,  3491,  2274,  3432,  2941,  2147,  1159,  3149,
	  1192,  2141,  3176,  2642,  2730,  3378,  2201,  3309,  2039,  2336,
	   536,  1828,  2733,  3412,  3304,  3222,  3225,  1231,  3300,  3479,
	  3218,  3305,   763,  3480,  2464,  3098,  2186,  2154,  1271,  3150,
	  1327,  2722,  3071,  1143,   500,  2735,  3048,  2709,  3365,  2318,
	  2122,  2150,  2263,  1505,  1637,  2963,  2603,  1509,  2703,  3596,
	  3009,  1508,  1792,   526,  2869,  3050,   598,   624,   546,  3297,
	  2753,  2097,  1421,   620,  2110,  2505,  1504,   993,  1503,  2814,
	   876,  2756,  1072,  1952,  2830,  2806,  1626,  2126,  2768,  1506,
	  3438,  2758,   543,  1804,  1073,  3165,  3080,  3076,  2241,  1977,
	  3020,  2647,  3022,  2459,  2659,  2469,  3303,  1323,   674,  2283,
	  2128,  3088,  2623,  2543,  2009,  2506,  1880,  2408,  2401,  3179,
	  2879,  2354,  1748,  2313,  2314,  1568,   626,  2746,  2396,   919,
	  2332,  2260,  2311,  2315,  1594,  2801,  1857,   602,   602,   602,
	  2786,   619,   625,   602,   602,  2773,  2113,  2752,   625,   602,
	   635,   619,   602,   644,  3134,   602,   619,  1596,  2741,   602,
	   619,   619,  1144,   602,   602,   619,   669,  1386,  1116,  2104,
	  3049,  2196,  2138,   688,  2622,   688,   652,  2103,  1621,   701,
	  2615,  1548,   644,  1990,  2100,   719,  2057,   625,   619,   625,
	   724,   725,   625,   873,  2014,   911,   738,  1956,   692,  1456,
	   760,  1700,  1373,  1966,  1801,  3219,   923,  1050,  1926,   601,
	  1905,  1024,   718,  2772,  1914,  1891,  1798,  2414,  2384,   997,
	  2272,  1735,  1385,   607,  1333,  1680,  2225,  1957,  1642,  2880,
	  2189,  2651,  1331,  2190,  2648,  2183,  1575,  2337,  1650,  1542,
	  1502,   921,  2200,  1541,  1545,  1495,  1499,   744,  1543,   499,
	  1518,  1470,  2597,  1434,  1536,  1454,  1436,  1429,   697,  1406,
	  1411,   646,  1530,   668,  1418,  2006,  1372,  1942,  3294,  1361,
	  1332,  2460,  1025,   786,  1311,   751,  1367,   750,  2823,  1132,
	  2271,  2129,   678,  1407,   720,  2395,  1131,  1112,  1298,  2400,
	   672,   673,  1281,   639,   698,  1715,   699,   762,   695,   764,
	   644,  1238,   696,   761,  1106,  1569,  2757,  3135,  1674,  1657,
	  1102,  1486,   898,   719,  2787,   619,   602,   602,   619,   986,
	   602,   602,   602,   774,   619,   619,  1091,   550,   719,  1111,
	   719,  1925,   830,  1069,  2643,  1599,   963,   675,  1769,   687,
	  1052,   687,  1437,   828,  1467,  1401,   783,   884,   749,  2169,
	  1391,  1869,   642,  2170,   745,  2385,   712,  3229,   694,    63,
	  1222,   651,  2832,  1211,  1163,   962,   992,  2649,    51,  1684,
	  1665,  1640,  1606,  1243,  1620,    48,   643,  1236,   609,  1409,
	  1533,   882,   629,   629,  1472,   551,   896,   835,   659,  1256,
	   547,  1101,  1161,  2612,   648,   990,   556,  3555,   662,  1193,
	  1693,   764,   665,   666,   600,   608,  2784,  2783,  1178,  1259,
	  1126,  3366,  1978,  1020,   619,  1742,   634,  3534,  3421,   637,
	  3279,   550,   550,  1232,  3052,   925,  3337,  2470,   878,  3554,
	   933,   934,  3278,  3271,   746,  2785,   942,   943,  3316,   550,
	  3315,  3243,  1016,  1706,  1707,  2604,  2605,  2606,  2928,   764,
	   556,  2501,   621,   621,   550,  1003,   739,  1742,  2449,   621,
	  3440,  2447,   621,  2433,   621,  3052,  3054,   621,   878,  1551,
	  3055,   621,   621,  1552,   550,  1070,   621,  2764,  1501,   551,
	   551,  2790,  2783,  2764,  1706,  1707,  2604,  2605,  2606,   702,
	   556,  2430,  2038,   621,  2037,  2036,  1940,  1935,   621,   621,
	   621,  2784,  2783,   621,  1268,  2512,  1121,   612,  1089,  2788,
	  2598,   621,  1084,  1324,  2480,  1417,  1130,  1693,  1742,   494,
	   493,  3276,   878,  1513,  3553,   789,  2204,  1514,  2765,  2764,
	  2785,  2309,   551,  3398,  1638,  1706,  1707,  2604,  2605,  2606,
	  1077,  1078,  1080,  1628,   556,  1742,   485,  1501,   622,  1706,
	  1707,  1130,  1104,  1742,   633,   981,   550,  2007,  1056,   987,
	   988,   989,  3066,  3587,  1057,  3058,  1058,   606,   880,   879,
	   999,  1693,  1000,  1979,  2639,  1644,  2613,  1706,  1707,  2604,
	  2605,  2606,  1076,   983,   984,   803,   550,   862,  1124,  2788,
	  1071,  1204,  1090,   721,  2653,   723,  3068,   550,   726,   900,
	   902,  3366,  3100,   503,   550,   846,  3053,  3006,   880,   879,
	  3063,   621,   538,   644,   967,  1481,  3382,   619,   857,   657,
	  3084,   616,  2197,  1099,   892,  1103,   621,   485,  2925,   621,
	  1228,  1194,   872,  1742,  3078,   621,   621,  1198,  1706,  1707,
	  2604,  2605,  2606,  3850,   551,  3208,  1259,  1760,  3277,   605,
	   845,   492,  1634,  1501,  1629,   551,  3851,  2765,  2764,  1164,
	   485,   647,  1512,  1511,  1147,   645,  1822,  1823,   556,  1635,
	   485,   549,   994,  1706,  1707,  2604,  2605,  2606,  1706,  1707,
	  2604,  2605,  2606,   625,  2788,  3872,  1204,  1084,  1660,   878,
	  1513,  1496,  1497,   605,  1514,  3735,  2192,  2654,  1282,  1706,
	  1707,  2604,  2605,  2606,  1164,   753,   754,   755,  2653,  1493,
	  2650,   556,   556,   878,  1513,  1306,   789,  1280,  1514,  1207,
	  3084,  1286,  1618,  1264,   556,   621,   503,   556,  1299,  1178,
	  1706,  1707,  2604,  2605,  2606,   556,  1109,  1105,  1105,  1240,
	  1706,  1707,  2604,  2605,  2606,  2604,  1205,  2777,  2778,  2780,
	  2781,  2782,  2779,  1706,  1707,  2604,  2605,  2606,   878,  1513,
	  1496,  1497,  1793,  1514,  1017,   549,   549,  1193,  1210,  1250,
	   995,  1326,  1328,   878,  1233,  1234,   789,  1283,  2161,  2160,
	   556,   995,  1334,   549,   556,  1247,  1104,  1707,  3052,  3054,
	   538,  3056,  1277,  3055,  1661,   556,   553,  1266,   549,   878,
	   874,  2194,  1276,  1201,   875,  1706,  1707,  2604,  2605,  2606,
	   554,  1706,  1707,  2604,  2605,  2606,   556,   555,   549,  1215,
	  2654,   556,  2777,  2778,  2780,  2781,  2782,  2779,  1125,  1512,
	  1511,  2653,  1706,  1707,  2604,  2605,  2606,  1215,  1237,   627,
	  2935,   645,  2777,  2778,  2780,  2781,  2782,  2779,  1706,  1707,
	  2604,  2605,  2606,  1512,  1511,   878,  1513,  1259,   789,  1103,
	  1514,  1950,   556,   538,   878,  1513,  1255,   789,   645,  1514,
	  1193,  1307,  1581,  3052,  3054,   538,  3056,   556,  3055,   556,
	   556,  2765,  2764,   553,   605,  1742,   556,  1075,  1140,  2017,
	  1172,  1074,  1275,  1368,   550,  1139,  1661,   554,  1512,  1511,
	   549,   553,  1197,   878,   555,  2456,   789,  3818,  2016,  1196,
	   542,   556,   553,   880,   879,   554,   625,  1557,  1193,  1953,
	  2708,   556,   555,   553,   621,   556,   554,  1322,   621,  3053,
	   549,   645,  1193,   555,  3582,  1172,  1208,   554,  1129,   880,
	   879,   549,   801,  3344,  3325,   625,  3343,  1141,   549,  1142,
	  1203,  1214,   551,   878,   874,  2439,  3772,  3773,   875,  3774,
	  3775,   602,   619,   619,   619,   556,  1129,   553,  1129,  1235,
	  2224,  3084,  1202,  2198,  3807,   556,   553,   556,   531,  3082,
	  1573,   554,  1768,   689,   804,   503,  1213,   627,   555,  1527,
	   554,   789,  3079,  1526,   621,  1512,  1511,   555,  1858,  1859,
	  1860,  1863,  1351,  1767,  1512,  1511,  1369,   502,  1289,  1248,
	   556,  1593,  2412,  1362,  3053,  1651,  2707,  1267,  1225,  2282,
	  1558,  1315,  1288,  1314,  1287,  1313,  1284,  1224,  1742,  1325,
	  1285,  1329,  1312,  1301,  1223,  1098,   556,  1300,  1588,  1199,
	  1404,  1200,  1566,   880,   879,   553,   605,  1246,   533,   831,
	   802,  1589,  1263,  1567,   556,  1408,  1230,  1229,  1251,   554,
	  1563,  1241,  1420,  2604,   538,   556,  1603,  1133,   556,  1130,
	  1110,  1742,  2686,  2194,  2921,  1134,  2920,  2919,  2690,  2654,
	   789,  3788,  2681,  2687,  1645,  1381,  1370,   764,  1378,  1261,
	  2653,  2689,  1260,   880,   879,  1380,   556,  3583,   602,   556,
	  1382,  3754,  3687,  1646,  3686,   553,  1485,   553,   605,  1133,
	  1523,   961,  1602,  3634,  1527,  3633,  1527,  1134,  1526,   554,
	  1526,   554,  1656,  1662,   553,   556,   555,   556,   555,  3539,
	  1237,  3538,  1639,  3751,  1309,  1408,  1550,  1636,   554,  1408,
	  1349,   645,  2368,   535,  1896,   555,   784,   789,   709,  1075,
	   531,  1628,  1694,  1630,  2167,  1357,  2166,  2165,   660,  3336,
	   785,  3335,  1364,  1710,  1489,  1711,   556,  1714,  1730,  1732,
	  1337,  2979,  1736,  2978,  1738,   708,   556,   532,   619,  1669,
	   619,  1895,   530,  1745,  1379,  1537,  1749,   619,  1402,  1742,
	   553,   529,   537,  3680,  1564,   707,   705,  2442,  2364,  2663,
	  1577,  3576,   625,  1164,   554,   706,  3564,  2367,  1763,  2137,
	   556,   627,  1519,  1601,  1600,   740,  1531,   741,   707,  3563,
	   533,   619,   619,  1830,   710,   701,   719,   621,   621,  1403,
	  3545,   742,  1464,   556,  3349,  1791,   538,   556,   549,  2568,
	  1795,  2567,   550,   711,   534,   524,  1803,  1805,  1806,   746,
	   965,   765,  1392,   503,  1515,  2266,   621,  3338,  3151,  1480,
	  1408,  3187,   605,  1452,   966,  3094,   746,  1716,  1731,  2992,
	   556,   555,  2987,   621,   621,   621,  1758,  1459,  1721,  1764,
	  1853,   556,  1475,  3192,  3192,  1477,  1478,  2472,  1719,  2471,
	  1712,   746,  2139,  2366,   760,   689,  1883,  1884,   645,  1885,
	  1886,  1887,   707,  1488,   697,  1383,  1687,   554,  1766,  1520,
	  1858,  1859,  1860,  1863,   555,  1799,  1774,  1641,  1641,  2986,
	  2985,  2983,  1555,  1746,  1554,  2977,  2975,  1851,  1678,  1789,
	  2442,  1762,  2441,  2429,   550,  2427,  1448,  1682,  1560,  1778,
	   556,  2234,   657,  2232,   753,   754,   755,  1681,   553,   532,
	  2971,   645,  1899,  1900,   530,  2960,  1808,  1446,  2069,  1242,
	  2068,  2578,   554,   529,   537,   645,  2064,  2562,  2063,   555,
	  1393,   553,  2493,  1447,  2021,  1829,  1739,  1585,  1888,  1796,
	  1797,  1934,  2061,  1838,  2060,   554,   753,   754,   755,  1848,
	  1133,   762,   784,   789,  1866,  1777,  3267,   761,  1134,  1397,
	  1396,  2054,  2281,  2053,  1779,  1827,   785,  1324,  2012,   713,
	   627,   647,   556,  1607,  2047,  2002,  2046,  1259,  2134,   556,
	   605,  2132,  2135,  2133,  2149,  1985,  1784,  1980,  1896,  1672,
	  1785,  1215,  1846,  1622,  1172,   784,   789,  2011,  2010,  1988,
	   555,  1431,  1442,  3347,  1944,  1945,  3327,  1389,  2520,   785,
	  1130,  1643,  2035,  1441,  2034,   556,  1813,   556,  1649,  1814,
	  1917,  1918,   746,   550,  1943,  1895,  1683,  2959,  1783,  1831,
	  1775,  1781,   645,  3266,  3136,  1290,  1702,  1708,  1787,   645,
	   556,  1954,  1854,   849,  1390,  1671,  2032,   784,   789,   621,
	  1780,   621,  1291,  1881,  2521,  1443,   553,  1445,   621,   929,
	  1893,   785,  1686,  1444,   554,  1476,   747,   556,  1439,  1440,
	   554,   555,  1983,   621,  2708,  2016,  1390,   555,  2019,  1998,
	   740,  1997,  2718,   605,  1817,  1995,  1744,  1695,  1053,  1847,
	  1999,   929,   621,   621,  2226,   702,  2719,  2004,   619,  2161,
	  2160,  1879,   556,   555,  1976,   740,  1975,   741,   605,  1771,
	  2695,  2086,   689,  2338,   619,   617,   503,  1706,  1707,  1713,
	   689,   742,  1939,  1788,  1938,  2101,  1908,   556,  2076,  2077,
	   849,  2023,  2524,   849,  2633,  1858,  1859,  1860,  1863,  1751,
	  1750,  1225,  1752,  2617,   818,   789,   549,  1937,  1130,  1936,
	  1224,  1864,  2616,  2517,  2518,  2522,  2516,  1223,  2519,  1933,
	   617,  1932,  1972,  1070,   556,   621,   784,   789,  1757,   864,
	  2707,  1219,   550,  1782,  2748,   556,  2087,  2500,   553,  2048,
	   785,  2202,   645,  1901,  1465,  2891,   556,   849,   645,  2208,
	  2163,  2055,   554,  1130,   746,   645,  1449,  2062,   556,   553,
	   605,  1902,  2065,  1903,  2073,  1809,  2070,  1133,  2067,  1946,
	  2052,  1947,  2748,   554,  1826,  1137,   645,  2095,   602,   556,
	  2040,  1842,  1130,  1104,  1949,  1871,  1872,  1856,  1388,  2078,
	   551,   848,  2024,  2083,  2199,  1725,   553,   605,   549,  2005,
	  2000,  1663,  1259,  3140,   553,  3138,  1647,  3139,  1911,  1534,
	   554,  2268,  1724,  1431,  1726,   711,  2754,  1728,   554,  1408,
	  2235,  1922,  2088,  2207,  1053,   555,  1909,  1133,  1547,  1259,
	   644,   553,   605,  1725,  1981,  1134,   554,   784,   789,   498,
	   940,  2258,  2259,   555,  1532,   554,  1521,  2137,  1408,  2296,
	  1724,   785,  1726,   711,  2754,  1728,  1103,   556,  1147,   784,
	   789,  1427,   899,  2300,  1352,  2333,  1964,   619,  1402,  1961,
	   784,   789,   928,   785,  1341,   719,   719,   553,   765,   556,
	  2350,  1122,  1987,   937,   785,   936,  2265,  1130,  1460,   932,
	   556,   554,  2050,  2266,  1996,  2020,  2033,   556,   918,   619,
	   719,  1282,   915,   895,   719,   719,  2107,  1698,  1699,   889,
	   619,   864,  2071,   856,   899,   619,   859,   812,  1721,  1218,
	  1220,  2352,   719,   851,  1286,  1519,  2747,   549,  1719,   619,
	  3825,  2243,   550,   689,  2391,  2265,  1741,   815,   806,  1299,
	  1028,  2045,  1706,  1707,  2028,  2049,   797,  3671,  1858,  1859,
	  1860,  1863,  2250,  1898,  2503,  2498,  2327,  2328,  1130,  3204,
	   645,  2261,  2432,   627,  2747,  1130,  2297,  1749,   848,   621,
	  1022,   657,   760,  2239,  2236,  2265,  2237,  2161,  2160,  2250,
	  1283,  2242,  2353,  2022,  2251,   621,  2308,  2252,   553,  3205,
	  2074,  2075,   784,   789,  2188,  2003,   784,   789,  2932,   740,
	   553,   741,   554,  1986,   553,   605,   785,  1259,  2323,  2392,
	   785,  2269,   556,  2275,   554,   742,   556,  2288,   554,  1982,
	  2080,  1930,  1655,   554,  1414,  1334,  2355,  1390,  2299,   605,
	  2302,  2334,  2210,  2212,  2275,  2362,  2634,   553,  2246,  1724,
	  1215,  2249,  2347,  1848,  2093,  2238,  2211,  1849,  2316,  1928,
	  1848,   554,  1867,  3832,  2316,  1259,  2134,  2548,   605,  2132,
	  2135,  2133,  2149,  1929,  2248,  3383,  3384,  2173,  2547,   762,
	  3385,   949,  1451,  2043,   645,   761,   549,  2044,   555,  2171,
	  2172,  2180,  2174,  2351,  2175,  2303,  2217,  2195,  2218,  2176,
	  2445,  2465,  2266,  2247,  2250,  2361,  3778,  2216,  2209,  2206,
	   553,   834,  3620,  1259,  2446,  3242,  2231,  2229,  2375,  2220,
	  2264,   550,   550,   799,   554,  2245,  2339,  2342,  2343,  2409,
	  2467,  2081,   553,  2545,  2546,  2413,  2253,  2254,  2255,  1355,
	   784,   789,  2267,   823,  1765,  2082,   554,  2422,  1368,   538,
	  3056,   621,  2369,  1356,   785,  2967,  2371,  2372,  2365,  2270,
	   556,  1093,  1095,  2301,  3259,   553,  3734,  2293,   737,  2405,
	  2244,  2406,   553,   605,  2380,  1094,  2373,  2582,  2583,   554,
	   553,  2415,  2306,  2510,  1376,   669,   554,   736,   621,  3619,
	  3618,   602,  3525,  2316,   554,  2394,  1450,  2041,  1830,  2250,
	  2281,  2042,   746,  2461,   730,  2587,  2509,  2330,  2508,  3719,
	  2507,  2969,  2970,  2483,  2484,  2485,   645,  1037,  2497,  2511,
	   621,  1289,  1993,  1994,  2594,  2610,   729,  2319,  1830,  2250,
	  2698,   621,   950,  2370,  3380,  1288,   621,  1287,   867,  1284,
	  3694,   866,  2595,  1285,  2379,  2421,   868,  2359,   747,  2316,
	   621,   870,  2275,  2390,  1301,  1968,  1971,  3100,  1300,  2697,
	  2389,  1369,  3606,  1969,  1970,  2398,   746,  1973,  1917,  1918,
	  1362,   746,   605,  2404,  1315,   657,  1314,  2192,  1313,   619,
	  2399,   627,  2584,  2416,  2417,  1312,   549,  2403,  2569,  2570,
	  2965,  2966,  2420,   621,  1038,  2631,  2632,  2424,  2426,  2419,
	  2490,   538,  1259,  2466,   553,   605,  2656,  1538,  1539,  3512,
	  2657,  3463,  2636,  1408,  3180,   553,  2487,  2488,   554,  1086,
	  1087,  1375,   789,  1420,  1850,   538,  1082,  2575,   617,   554,
	  3621,  2644,  2538,  2448,  2540,  1455,  2451,   602,   602,  2457,
	  2541,  1370,  2715,   602,  2450,   746,  1067,  1457,   743,  3314,
	  2312,  3037,   822,  2058,  1334,  1035,  2933,  2874,   627,  2727,
	  2728,  2479,  2865,  1036,  2852,  2637,  2491,  2579,  1034,  1033,
	   719,  1412,   619,  2494,  2458,  2444,  2660,  1408,  2356,  2340,
	  2329,  2496,  2221,  2205,  1408,  2096,  2504,  2091,  2059,   719,
	  2514,  1408,  2473,  2539,  1931,  2474,  1736,  1870,  1836,  1812,
	  1689,  1685,  1668,  2807,  2549,  1670,  1749,  1664,  2551,  3180,
	  1426,  2554,  2614,  2555,  2557,  2558,  1384,  2774,   746,  1365,
	  1353,  1292,  2611,  1714,  2564,  2566,  2710,  2792,  2560,  1253,
	  1577,  2720,  1115,  2572,   602,   602,  2574,   602,  2573,  3180,
	  1537,  2577,  2629,  2630,  2576,  2716,  2571,  2565,   944,   941,
	   930,   861,  2635,  2851,  2559,  2771,  2856,  2592,  2838,  1803,
	   717,  1519,  2862,  2333,   619,   549,   549,   632,  1531,  2552,
	   657,  2294,   647,  2850,  1850,  3877,  2871,  3857,   617,  1259,
	  3790,  3756,  3731,  3675,  3639,  2304,  1725,  1160,  3562,  3456,
	  3383,  3319,  2828,  1845,  1451,   746,  1844,  1259,  1450,  2321,
	  2185,  1484,  2836,  1724,  1729,  1726,   602,   602,  1728,   554,
	   891,   760,  1772,  2184,   887,   947,   760,  2904,  2905,  2481,
	  2883,  2341,   684,  1716,  3561,  2885,   907,  2482,  2638,  1756,
	   685,  1799,  2893,  2185,  1721,  1232,  2705,  2705,  2357,  2892,
	   908,   909,  2705,   833,  1719,  1950,  2831,   832,  2641,   689,
	  2803,  2725,  2822,  2870,  2640,  1133,  1850,  2833,  2808,  1815,
	   617,  3684,  2655,  1134,  2736,  1592,  2682,  2662,  2669,  2683,
	  2824,  2840,  2676,  2692,  2275,  2275,  2706,  2706,  2848,  2693,
	  2696,  2847,  2706,  1330,  2791,  1259,  1296,   686,  1681,  2702,
	   621,  1849,   615,  2853,  2886,  2887,  2931,  1848,  1849,   683,
	   617,  2713,  1160,  1424,  2938,  2262,  2094,  1924,   762,  1474,
	  1151,  1359,  2760,   762,   761,  1344,  2316,  2631,   810,   761,
	  1152,  3202,   614,  2863,  2275,  2766,  2767,  2789,  2309,  2334,
	  1304,  2793,  1850,  3833,   553,  3789,   599,  3519,  2810,  2812,
	  1941,  1293,  1004,  1638,   682,  3151,  2846,  2797,   554,  3638,
	  2796,   647,  2795,   621,   773,  1154,  2974,  3118,  2936,   617,
	  2826,   948,  1204,  2785,  2215,  2475,  1960,  1516,  1492,  2903,
	  2825,   776,  2976,   553,  1150,  2835,   613,   617,  1153,   775,
	  1245,   796,   713,   621,  2027,   617,  2878,   554,  2913,  1773,
	   619,   619,   619,  2858,  2101,  2951,  1710,  3004,  1463,  1156,
	  1155,  2841,  2842,  3462,  2843,  2705,  2705,  2860,  1324,  3002,
	  1881,  3030,  2612,  2864,  1590,  2415,  3559,   716,   671,  1893,
	   617,  1701,  2438,  2994,  2995,  1160,  2956,  2957,  1688,   885,
	  2478,  1597,  2877,   843,   655,  2612,  2463,  3039,  3749,  3348,
	  1123,   560,  2927,   656,  2102,  2706,  2706,  2179,  1032,  1850,
	  2437,  1060,  2911,  3043,  2907,  3044,   653,  1527,  2894,  3047,
	   597,  1526,   654,  3003,  2377,  2910,  2839,   621,   824,  2915,
	  1921,   707,  3065,  3008,  2980,   621,  2917,  3015,  1030,  3299,
	  3285,  2930,  2937,   847,  3083,  1625,   769,  2325,  1319,  1321,
	  1320,  2748,  2940,  2988,  1014,  2939,  1873,  2944,  2289,  3748,
	  2950,  2287,  3051,  1874,  1875,  2954,  3755,  3131,  2958,  2993,
	  1972,  1012,  2961,  3105,  2955,  2953,  3013,  2788,   647,  1013,
	  3013,   711,   621,  3128,  2827,  3117,   550,   621,   660,  3057,
	  2320,  1348,  1561,  2949,  3064,  1059,  1570,  1571,  2292,  2291,
	  2973,  3766,   645,   776,  3034,   556,  3085,  3086,  2309,  3434,
	  3142,  3142,  1725,  2816,   707,  3106,  3107,  3108,   776,  1244,
	  3087,  2348,  2817,  3021,  3708,  3090,  1316,  2344,  3072,  1724,
	  3073,  1726,   711,  2754,  1728,  2815,  1015,  3435,  2990,  2624,
	   657,  2818,  2991,  3133,   551,  2989,  3102,  3122,   710,  3145,
	  3132,  3158,  3161,  1710,  1550,  2613,  3029,  3114,  2532,  3130,
	  2161,  2160,  3193,  3025,  2774,  3005,  3129,  1147,  3000,  2945,
	  2952,  3010,  2534,  2333,   657,  3010,  3089,  2249,  2613,  2462,
	  3389,   768,  3178,  2275,   978,  3014,  3023,   979,  1950,   958,
	  3711,  2819,  3113,  2079,  2309,   980,  2820,   959,   787,  3177,
	  1582,   976,  3042,  1088,  2536,  2533,  1002,  3235,   972,   977,
	  1725,   707,  2535,  2530,  2531,  2529,  3146,  2528,  2527,  2425,
	  3003,  1007,  1186,  1186,  3766,  3168,  2412,  3141,  3141,  1726,
	   711,  1950,  1728,  3200,  3026,  2525,  2711,   556,  1008,   602,
	  3292,  1006,  3291,  2747,  3231,  3290,  1010,  3289,  3237,  3091,
	  3092,  1749,  3169,  3253,  3254,  3420,   760,  3244,  3245,  1725,
	  3220,   621,   621,   621,  3256,  3275,  3223,  1587,  3238,  3203,
	  3224,  1559,  3163,   641,  3310,  3173,   655,  1729,  1726,  2943,
	   747,  1728,  3119,  1178,  1133,   656,  3127,  3232,  3249,  3120,
	   927,  3144,  1134,   794,  3124,  3143,   764,   356,   653,  2804,
	  3394,  3302,   548,  1009,   654,  3171,  3389,  3017,  3318,  3162,
	  3170,  3195,  3137,  3137,  1066,  3182,  3240,   920,   863,  3159,
	  1118,  3148,  2275,  2275,  3236,  3191,  1226,   893,  3156,   897,
	   901,   903,  3164,  3209,   913,  3172,  1676,  1833,  3212,  2334,
	  2844,  2845,  3215,   968,  3211,  1704,  1704,  3234,  1958,  2331,
	  2261,  3214,   969,   762,  3217,  3351,  3352,  1654,  2355,   761,
	  1410,  2738,  2739,  2740,  1820,   971,  3324,  1338,  1336,  2866,
	  2867,   970,  3246,  3152,  3153,  3257,  3013,  1864,  2875,  3250,
	   705,  3803,  3339,  2316,   973,  2909,  1026,  3093,  2316,   706,
	  3261,  1770,  2510,  3269,  3264,  1849,   548,   548,  3284,  3320,
	  3265,  3262,  3287,  2409,  1100,  3013,  3226,   779,   710,  3013,
	   728,  2901,  2902,  1257,  3329,  2509,  3376,  2508,  1040,  2507,
	  2912,  1864,  1864,  3175,   550,   855,  2281,  3718,  2511,   766,
	   549,  3685,  3356,  3357,  3358,  3682,  3359,  3321,  3391,  2250,
	  3375,  3083,  3131,  3372,   820,  3281,  1039,  3331,  3397,   548,
	  1830,   550,  3515,  3060,  3330,  3288,  2837,  3401,   602,  2949,
	  1586,  2732,  3333,   811,  3371,  3340,  1051,  3406,  3407,  3409,
	  1877,  3010,   771,  3402,   719,  3013,  3013,  3013,   814,  3013,
	  2898,  2223,   805,  3341,  2897,   714,  3387,  1882,  2896,  2899,
	   628,  2030,  3399,     2,  1667,  2900,  1269,  2660,  2222,  1666,
	  3010,  1113,  2275,  1114,  3010,   681,  3260,  1108,  1107,   719,
	  3362,  3417,  3361,  3350,  2607,  1092,  2608,  2609,  1864,   525,
	  1085,   523,  3021,   522,  3379,  3581,  3377,  1692,  3046,  3045,
	  3370,   766,  2646,  2182,  3041,  2181,  3423,  1627,  3426,  1068,
	  1081,  2661,  2203,  3429,  1079,  1697,   521,  3392,  3436,  3089,
	  2620,  2625,  3455,  3322,  3422,  3393,  3400,  3390,  1147,  3386,
	  2807,   548,  3396,  3395,   520,  3062,  3061,   621,   619,  2658,
	  3010,  3010,  3010,   519,  3010,  2101,   518,  1714,  2673,   766,
	  3067,  3449,  2665,  2680,  2679,  3816,  3457,  3235,  3235,  3235,
	  3450,  2300,  3815,  3849,  3848,   939,  3374,  3475,  3235,  3473,
	  3813,  3492,  2771,   960,  2771,  1710,  3496,  3765,  3007,  3498,
	  3763,  3077,  2771,  2871,  3507,  3507,   931,  3075,  2685,  2678,
	  2688,  2684,  2677,  3027,  3231,  3231,  3231,  3508,  3237,  3237,
	  3237,  2675,  3070,  3069,  2674,  3231,  3429,  2672,  3483,  3237,
	  3484,  3443,   602,  3478,  3481,  3482,  3506,  3506,  2671,   760,
	  2670,  3354,  2668,  2667,  3486,  3520,  3445,  2666,  3354,  2664,
	  2106,  3441,  3451,  1648,   630,   528,   527,  3232,  3232,  3232,
	   517,  3410,  3474,   516,   515,  3460,  3466,  1716,  3232,  3469,
	  3465,  3059,  2193,  3453,  2191,  2769,  1843,  3459,  1721,  3510,
	  2870,  3468,  3533,  3530,  3501,  3501,  3493,  2706,  1719,  3454,
	  2831,  3096,  2652,   871,  3236,  3236,  3236,  3499,  3103,  3467,
	   690,  3529,  2645,  3488,  3522,  3236,  3418,   759,  3109,  3110,
	  1633,  3442,   825,   816,  3115,   813,  1632,  3234,  3234,  3234,
	  3514,  3527,   853,   854,  2187,  1631,  3580,  3448,  3234,   513,
	   512,   511,  3531,   510,   509,  3497,   762,   508,   507,  3584,
	   506,  3431,   761,   505,   504,  3517,   501,   497,  3147,   496,
	  1065,   495,    20,  1549,  2066,  3154,  3155,   307,   549,  2072,
	   306,   305,  1544,   436,   938,   304,  1540,   303,   602,  1535,
	   935,   302,  3546,   301,  2510,  3551,  3226,  3226,  3226,  3550,
	  2056,  3183,  3051,  3603,   300,   549,  1529,  3226,  3544,  3585,
	  3196,  3556,  3568,  3198,   299,  3565,  3557,  2509,  3543,  2508,
	  3189,  2507,  2563,  2265,  3575,  1158,  2051,  2275,  1522,  3013,
	  2511,   298,  3213,   297,  1517,  3064,  3586,  3589,  3591,   428,
	  3578,   926,  3235,  3235,   296,  3626,   295,  3608,  3590,   294,
	  3611,  2705,  3594,  3613,  3588,  3593,  2465,  3072,  1881,  3073,
	  3475,  3630,  3473,  3475,  3475,  3473,  3473,  3003,  3475,  3247,
	  3473,  3622,  3616,  3610,  3598,  3599,  3251,  1498,  3509,  3231,
	  3231,  3656,  1494,  3237,  3237,  1704,  1704,  1704,   425,   621,
	  1491,  2706,  2771,   917,  3282,  3283,   293,  3642,  3641,  3310,
	  1710,  3223,  3670,  3640,   161,  3224,  3643,   160,  1864,  1864,
	  1864,  3295,   159,  3668,   158,   157,   156,   155,   154,  1864,
	   153,   152,  3232,  3232,  3010,   151,  3302,   150,  3651,   149,
	  1273,   148,  3648,  3649,  3652,  3474,  1147,  1147,  3474,  3474,
	   147,  1405,  1428,  3474,   423,   914,  3667,   291,  3658,   550,
	   474,  3666,  3665,  3653,  3654,   422,   290,   289,  3354,  3236,
	  3236,  3674,  2026,  3681,  1483,  2025,  1482,  3003,  3636,   906,
	   621,   602,  1479,   905,  3679,   420,   602,  3354,   904,   288,
	   419,   287,  3234,  3234,   764,  1473,  3706,  3605,   418,   286,
	  3692,  3710,   417,   285,   292,  3683,  3705,  3697,  3560,  2982,
	  3614,  3013,  3691,  3700,  3417,  3699,  3558,   551,  3346,  3354,
	  3354,  3354,  3354,  2102,  3001,  3235,  3355,  3013,  3235,  3703,
	  3631,  3492,  3717,  3360,  3632,  3716,  3507,  2706,  3722,  3720,
	  2981,  3743,  3712,  3745,  2550,  3732,  1468,  1466,  3740,  3733,
	  2300,  3226,  3644,   416,  3726,   894,  1395,   284,   415,  3032,
	  3033,  3741,  3231,   283,  3738,  3231,  3237,  3746,  3506,  3237,
	  3750,  1462,   414,  1461,   890,   282,   413,   888,   281,   886,
	   280,  3736,  1453,   883,  3737,   411,   881,  3655,   279,  3762,
	   278,  3767,  2515,  3728,  1435,   409,  1864,  2275,  3660,   277,
	  3662,   869,   865,  3768,   276,  3232,  3010,  3769,  3232,   407,
	   274,  2972,   860,   273,  1469,  1425,  3501,  1127,   858,  3403,
	   272,  3547,  3010,  2964,  2962,  1127,  2968,  1992,  3475,  3475,
	  3473,  3473,  3419,  2499,  1991,  1989,  1423,  1500,  1422,   271,
	   269,  2526,  3236,  1438,  3427,  3236,  3779,  3780,  3659,   550,
	  1524,  1127,  3823,  3805,  1127,  1419,  1416,  1206,   602,  2164,
	   403,  1553,  3102,  1864,  1864,  3234,  3820,  1217,  3234,  3828,
	  3812,  3811,  3701,  3829,  3235,  1415,  3507,  3836,  3801,  3837,
	  3605,  3838,  3843,   270,  3804,   268,  1413,   267,  3835,   850,
	  3786,  3787,  3796,   266,  1864,  1864,  3051,   398,   264,   262,
	  3452,  3844,  3160,  3474,  3474,  3174,  3847,  3852,  3506,   396,
	  1254,  3231,   261,  2477,  3830,  3237,  2476,  1963,  3342,  3181,
	  2706,  2544,  2542,  3862,  3226,  2706,  3475,  3226,  3473,  2015,
	  3834,  3826,  3826,  3867,  3083,  3866,  3823,   766,  3865,  3470,
	  3868,  3843,  3844,  3869,  2387,  3873,  1604,  2393,  3875,  2397,
	  2397,  3861,  2486,  1967,  3232,  3485,  3501,   951,  3753,   953,
	  3844,   788,   954,  1965,  3863,  1400,  1317,  2402,  2402,  1316,
	   955,  3489,  2489,  3760,   842,  1524,   958,  1524,  3511,   952,
	  3051,   841,  2162,  3051,   959,   395,  1962,  3770,  3856,  1399,
	   840,  3236,   257,  1394,   256,   957,   837,   255,   254,  2601,
	   829,  3474,   251,   549,  2029,   912,   146,   145,   144,   143,
	   142,   141,   140,  2137,  3234,  3826,   139,   138,   137,   136,
	   135,   134,   133,  3286,  1864,   132,   131,   130,  2748,   129,
	   128,   127,  3876,   126,   125,   124,  1864,   123,  2121,  1864,
	  2118,   956,  3354,  3311,  2151,  2143,  2116,   122,   502,  3605,
	  3827,  3827,  3541,   121,   120,   119,   118,   117,   116,  2155,
	   115,  2156,  2120,  2119,   114,   113,   112,  2157,  2153,   111,
	   110,  2117,   109,  3226,  3566,   108,   107,  3567,  2161,  2160,
	   106,   676,   201,   200,   199,  3570,  3572,  3573,  1151,  1725,
	  1265,  1584,   362,  1864,   715,   212,  2139,  2706,  1152,   689,
	  2233,  1105,  1679,   559,  2700,  1675,  1724,  3328,  1726,   711,
	  2754,  1728,  2948,  3326,  1858,  1859,  1860,  1863,  2947,  2946,
	  1955,  1864,  1864,  1387,   827,   826,   250,  1120,   558,   105,
	   545,   544,  2214,  1154,  3827,  1157,  2213,  1653,  1097,   541,
	  3607,   540,   103,  3609,   198,  3597,  3597,  3597,   197,  3615,
	  1377,  3363,  1150,   821,  3364,   246,  1153,  2452,  3367,  3368,
	  3369,  3625,  1864,  1864,  3628,  1371,   817,   245,   244,   242,
	  2137,  1366,   241,  1360,   809,  1363,  3635,  1156,  1155,  2601,
	   808,   807,   240,   243,   535,  1864,  2934,  1794,  1358,   239,
	   800,   531,   238,   549,  1354,   798,  3645,  2751,  2749,  2755,
	  1127,  1127,  3116,  1160,  3646,  2148,  2729,   819,  2745,  2744,
	  2743,  2742,  2737,  2443,  1583,  1927,  2266,   964,   237,  1864,
	  2747,  1259,  2134,  2108,   605,  2132,  2135,  2133,  2149,  1923,
	  1350,   795,  3657,   236,  2918,  2431,  1912,   554,  1910,  1343,
	   645,  1916,  1915,   556,   555,  3532,  1864,  3313,  3535,  1158,
	  3536,   709,   709,  2139,  3317,  2748,   689,  2926,  1105,  2436,
	  2929,   533,  1913,  1864,   793,  1347,  2924,  2435,  1920,   792,
	  1346,  1858,  1859,  1860,  1863,  3312,  2923,   538,   708,   708,
	  2434,  1919,  1345,   791,   382,   534,   524,  3437,  1342,   790,
	   235,  1906,  3690,  2428,  2607,  3444,  1904,  3695,  1340,   705,
	   705,   781,  3447,  1149,  1339,   780,   234,   548,   706,   706,
	  3855,  3821,  3702,  3415,  3414,  3413,  1725,  1395,  3411,  3112,
	  2723,   707,   707,   556,  2721,  3602,  3709,   710,   710,  1148,
	  3714,  2240,  1146,  1724,  1145,  1726,   711,  2754,  1728,  3099,
	  2219,  1659,  2748,  1658,  2102,  2712,   711,   711,  3725,   604,
	  3104,  3707,  2724,  1318,   752,   748,   924,   228,  1861,  3263,
	  1690,  1706,  1707,  2098,   709,  2734,  2759,  2386,  1841,  1840,
	  1839,  3270,  1865,  3739,  2013,  1310,  2018,  1852,  1259,  2134,
	   553,   605,  2132,  2135,  2133,  2149,  1308,  1837,  2748,  1305,
	   532,   708,   227,  1835,   554,   530,  1834,  2799,  1742,  1303,
	   556,   555,   225,  1725,   529,   537,  1816,  3757,  1294,  1011,
	  3759,   221,   705,   220,   218,  2861,  2360,   219,  2854,  1802,
	  1724,   706,  1726,   711,  2754,  1728,  1800,  3776,   877,  1005,
	  1274,   217,   216,  3781,   707,   214,  2130,  2131,   213,  3490,
	   710,  2849,  1790,  1001,  3784,  3785,   657,  2747,  3487,  1725,
	  3239,  2349,  1262,   211,  3791,  3792,   456,   359,  3794,   711,
	  3797,   358,  3799,   357,   746,   209,  1724,  2335,  1726,   711,
	  2754,  1728,  1776,   693,   553,   553,  3808,  1252,  1249,  3810,
	   691,   645,   208,  3727,  3188,  2798,  3819,  1734,   554,   554,
	  3184,  2794,  1733,   177,   226,  3569,  1810,  2363,  1811,  1279,
	  3574,  1807,  1278,   727,  2601,  2601,  2601,   222,  3012,  2586,
	  1574,  2591,  3012,  2146,  1825,  2590,  2589,  2601,  1706,  1707,
	  2177,  1623,  2601,   443,   354,  2168,  2168,  3846,  1868,  1565,
	  1604,   353,  1572,   352,  2747,   351,   350,   349,   348,  3853,
	   347,   346,   345,   344,   343,   342,  2585,  2085,   975,  1705,
	  1705,   746,  3858,  3859,   974,   341,   203,   631,   679,   176,
	   165,   778,  1591,   777,   233,  2738,  2739,  2740,   645,   772,
	    -1,  1019,  1018,  3870,   770,  3627,   232,    -1,  1290,   703,
	  2747,  1562,  3874,   767,   231,  2159,  2137,   553,  2158,  3038,
	  2145,  2144,  2152,  2626,  2142,  1291,   704,   746,  2112,  2111,
	  3024,   554,  2621,  3019,   603,   603,   603,  3018,   679,  1158,
	   603,   603,  2619,  3016,  2618,  2140,   603,  2151,  2143,   603,
	  3693,  3577,   603,  3373,  3036,  3035,   603,  2136,  2125,  2124,
	   603,   603,  2155,  2123,  2156,   679,  2137,  2115,  2109,   677,
	  2157,  2153,  2146,  2105,  3157,  2770,  1704,  1704,  2776,  2284,
	  3723,  2161,  2160,  3446,  2775,  3167,  3166,  3439,  3311,  3771,
	  3669,  2280,  2279,  2146,  2137,  2278,  2601,  2277,  2276,  2139,
	  2763,  2273,   689,  3842,  1105,  3839,  3663,  3308,  3306,  2423,
	  1894,  1892,  2266,  3521,  3296,  2906,  2418,  1858,  1859,  1860,
	  1863,   703,  1890,  1889,  2317,  3802,  3747,  3523,  3688,  3298,
	  2908,  3689,  2942,  3323,  3742,  3518,  3744,  3661,   704,  3293,
	  2266,  3516,  2895,  2411,  1878,  2694,  1897,  2227,  1117,  2139,
	  3676,  2596,   689,   922,   922,  2099,   392,   922,  3540,   922,
	   922,  1948,   249,   104,  2889,   922,   922,  1858,  1859,  1860,
	  1863,  2888,  2407,  3228,  1876,   758,   757,  2139,   756,   229,
	   689,  3233,  3230,  3472,  3221,  2346,  3721,  2345,  1786,  3724,
	   210,   204,   202,  2326,  2834,  1858,  1859,  1860,  1863,  2324,
	  1239,   670,   196,  1759,   195,   193,   194,  1374,  2148,   192,
	  1720,  1583,  2084,   603,   603,  1723,  3617,   603,   603,   603,
	  3433,  2090,  1727,  1722,  1259,  2134,  2108,   605,  2132,  2135,
	  2133,  2149,  1718,  1717,  2731,  2829,  1761,  1209,  3012,   329,
	   554,   191,   190,   645,   991,  3207,   556,   555,  2813,  2310,
	  1755,  3758,  1754,  3206,  3111,  2811,  2724,  2307,  1753,   661,
	   188,  1227,  3121,  1221,   658,  3123,  3125,  3012,   187,  2137,
	  2178,  3012,  1270,  2440,  1259,  2134,   215,   605,  2132,  2135,
	  2133,  2149,  3782,  1821,  1832,   735,  2873,   922,  3255,  2455,
	  2872,  1824,  1297,   922,   734,   922,  3504,   555,  2868,  1819,
	   733,  2468,  1259,  2134,   732,   605,  2132,  2135,  2133,  2149,
	  1510,  2374,  1818,  1295,   731,  2266,   224,  2013,  2013,  2013,
	  1747,   645,  2018,  1528,  3637,   555,  3461,  3201,  2601,   679,
	  3190,   679,  1178,  2809,  1216,  2601,   650,  3012,  3012,  3012,
	   186,  3012,  2601,  3831,   185,   184,   183,  2805,   640,   638,
	   182,   180,  2139,  2523,  2537,   689,  1737,  1105,  1186,  2601,
	  2601,   179,   181,  1177,  1706,  1707,  1176,  1175,  1174,  1173,
	  1858,  1859,  1860,  1863,  1171,  1170,  1169,  1168,  1167,  1166,
	  2295,  1165,  1696,  1162,  1500,  2556,  2556,  2556,   189,   178,
	  1556,   946,   945,   317,  2305,  1184,  1185,  1181,  1182,  1183,
	  1189,  1190,  1187,  1188,  1191,  1180,  1179,   175,   173,  2322,
	  1128,  2556,   172,  2397,  2556,  2402,  2290,  1195,   171,  2556,
	   170,   611,  1583,   610,   169,   168,  1138,  1136,  1583,  1135,
	   167,   166,   102,   101,   100,    99,    98,    97,    96,    95,
	    94,    93,    92,    91,    90,    89,    88,  2358,  1528,    87,
	  1528,    86,    85,  2146,    84,  2146,  2146,    83,    82,    81,
	    80,    79,    78,    77,    76,    75,    74,    73,    72,    71,
	    70,    69,    68,    67,    66,    65,    64,  1259,  2134,   553,
	   605,  2132,  2135,  2133,  2149,    62,    61,  2601,    60,  2146,
	  2146,    59,    58,   554,    57,    56,    55,    54,    53,   556,
	   555,  2601,    52,   679,    50,    49,    47,    46,  2601,  1705,
	  1705,  1705,    45,    44,    43,    42,    41,    40,    39,    38,
	    37,    36,    35,    34,    33,  2410,    32,    31,    30,    29,
	    28,    27,    26,    25,    24,    23,    22,   491,  1063,   490,
	    18,    16,    15,    14,    13,    12,    11,    10,     9,     8,
	     7,     6,  3476,  3228,  3228,  3228,     5,     4,     3,     1,
	     0,  1335,     0,     0,  3228,   679,     0,     0,  1258,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  3502,  3502,     0,     0,     0,  1158,  1158,     0,   679,  1272,
	  2717,  1158,     0,     0,     0,   679,     0,     0,     0,   679,
	     0,     0,     0,  1302,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  2601,     0,     0,     0,  1706,  1707,     0,
	     0,     0,     0,  2146,  2146,     0,     0,     0,     0,     0,
	     0,     0,  2601,     0,     0,     0,     0,     0,  2601,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  2601,     0,     0,  3424,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  2137,     0,     0,     0,     0,     0,
	     0,     0,     0,  2468,     0,     0,     0,  3430,     0,     0,
	     0,  3012,     0,  2601,  2601,  2601,  2601,     0,     0,     0,
	  2601,  2601,   679,     0,  2601,  2601,  2601,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  2155,     0,  2156,     0,     0,     0,     0,     0,  2157,     0,
	     0,     0,     0,     0,  1158,     0,     0,     0,     0,  2161,
	  2160,     0,     0,     0,     0,     0,  2881,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  2593,  2139,     0,     0,
	   689,     0,  1105,     0,  1158,  1158,     0,     0,     0,     0,
	     0,     0,     0,     0,  2601,  1858,  1859,  1860,  1863,     0,
	     0,  2601,     0,     0,  2601,     0,     0,  1507,     0,     0,
	     0,     0,  2922,     0,     0,     0,     0,     0,     0,     0,
	  1525,     0,     0,     0,     0,     0,     0,     0,  3228,     0,
	     0,     0,     0,     0,     0,  3476,     0,     0,  3476,  3476,
	     0,     0,     0,  3476,     0,     0,     0,     0,     0,     0,
	     0,   548,     0,     0,     0,     0,     0,     0,     0,  3524,
	  2013,  2013,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  3012,     0,     0,  2148,  1576,   603,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  3012,
	     0,     0,  1259,  2134,  2108,   605,  2132,  2135,  2133,  2149,
	     0,     0,     0,     0,     0,  1127,     0,     0,   554,     0,
	     0,   645,  1598,     0,   556,   555,  2601,     0,     0,  2984,
	     0,  2601,     0,     0,     0,     0,     0,     0,     0,  1335,
	     0,     0,     0,  2602,     0,  1525,     0,  1525,  1524,  1553,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  2724,  2146,  2146,     0,     0,  2146,  3600,     0,     0,
	     0,   552,     0,     0,  2601,     0,     0,     0,  2821,  3730,
	  2146,     0,     0,     0,  2146,     0,     0,     0,     0,     0,
	     0,  3228,  2146,     0,  3228,     0,     0,     0,  2146,  2146,
	     0,     0,  3502,     0,  1127,     0,  1583,     0,  2114,     0,
	     0,     0,     0,     0,     0,   603,  2601,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  2601,     0,     0,     0,
	     0,     0,  1583,     0,     0,  2601,  2601,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  1703,  1703,  1709,     0,     0,
	     0,     0,  2882,     0,     0,  2882,     0,  2884,  2601,     0,
	  2884,  2601,     0,  3664,  2890,     0,     0,     0,     0,     0,
	     0,     0,     0,  3476,  3476,     0,     0,     0,     0,     0,
	     0,     0,  3678,     0,     0,     0,     0,     0,  1158,     0,
	     0,     0,     0,     0,     0,  2601,     0,     0,     0,     0,
	     0,     0,     0,  2602,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  2601,
	     0,  2146,  2146,     0,     0,     0,     0,     0,     0,     0,
	  3228,  2146,  3502,     0,  2146,     0,     0,     0,  3704,     0,
	     0,     0,     0,     0,   679,     0,     0,     0,  2146,     0,
	     0,  3713,     0,  3186,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   680,     0,     0,     0,     0,  2601,   618,
	   623,  3476,     0,     0,     0,     0,   623,     0,     0,   636,
	     0,   623,     0,     0,   649,     0,     0,  1855,   663,   664,
	     0,     0,     0,   667,     0,     0,   766,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	   623,     0,     0,   680,     0,   623,   722,   623,     0,     0,
	   623,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  2881,     0,
	   680,     0,  2881,  2996,  3280,     0,     0,     0,     0,     0,
	     0,     0,  2146,     0,     0,     0,   680,   680,     0,     0,
	   680,     0,   680,   680,     0,     0,     0,  2146,   680,   680,
	  2146,     0,  2146,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  3028,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  3678,  3806,  1583,     0,     0,     0,  2146,
	     0,  2146,     0,  2146,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  2013,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,   623,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  3345,
	     0,  1374,  2454,   982,     0,     0,   985,     0,     0,     0,
	     0,     0,   996,   998,     0,     0,     0,     0,     0,     0,
	  3097,     0,     0,     0,     0,     0,     0,  3097,     0,  2146,
	  2146,  2146,     0,  2146,     0,     0,     0,  3097,     0,     0,
	  2146,     0,     0,  2146,     0,     0,  3864,  2146,  2146,  2146,
	   680,     0,     0,     0,     0,     0,   680,     0,   680,     0,
	     0,  1510,     0,     0,     0,   679,   679,   679,     0,   679,
	     0,   679,     0,     0,     0,     0,   679,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  2602,  2602,
	  2602,     0,  3011,     0,     0,     0,  3011,  1510,  1510,  1510,
	  1510,  2602,  1061,     0,     0,     0,  2602,     0,     0,     0,
	     0,     0,   679,     0,     0,     0,  1158,     0,     0,  3197,
	     0,     0,  3199,     0,  1510,     0,     0,  1510,     0,     0,
	  3210,     0,  1510,     0,     0,     0,     0,     0,     0,  1158,
	     0,     0,     0,     0,     0,     0,  3216,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  1127,  1127,
	  1127,     0,     0,     0,   680,     0,   680,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  2146,     0,  3248,     0,
	     0,  1583,     0,  2146,  2146,  3252,     0,     0,     0,     0,
	     0,  2146,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  3097,  3097,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,   603,     0,     0,     0,     0,
	  1705,  1705,     0,  2146,     0,  1703,  1703,  1703,     0,     0,
	     0,  2256,  2257,     0,     0,     0,     0,     0,     0,     0,
	  2602,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  1794,     0,     0,     0,     0,     0,     0,     0,
	     0,  1158,  2146,  2146,     0,     0,  2146,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  1158,   623,     0,     0,     0,  1212,     0,     0,     0,     0,
	     0,     0,   603,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  3227,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,   680,     0,
	     0,     0,     0,     0,   548,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  1335,     0,  3552,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  3274,     0,     0,     0,
	  2381,   623,  2382,  2146,     0,     0,     0,     0,     0,     0,
	  2388,     0,     0,     0,  2146,     0,     0,     0,     0,  2146,
	     0,     0,  3011,     0,   679,     0,   680,     0,     0,     0,
	   680,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,   680,  3404,     0,
	     0,  3011,     0,   680,     0,  3011,     0,     0,   680,     0,
	   680,     0,     0,     0,   680,     0,  1158,     0,     0,     0,
	     0,     0,     0,  3428,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,   679,     0,  3624,     0,
	     0,     0,     0,     0,  2146,     0,     0,     0,     0,  2453,
	   680,     0,     0,     0,     0,     0,     0,   680,     0,     0,
	     0,     0,  2602,  1273,     0,     0,     0,     0,     0,  2602,
	     0,  3011,  3011,  3011,     0,  3011,  2602,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  1158,  1158,     0,
	     0,  1374,     0,  2602,  2602,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,   680,  1507,     0,
	     0,     0,     0,     0,   766,     0,  2146,     0,  3471,     0,
	     0,     0,  2146,     0,     0,  2146,     0,  2146,     0,  2146,
	   680,     0,     0,     0,   680,     0,     0,     0,     0,     0,
	     0,  2146,     0,     0,  1507,  1507,  1507,  1507,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  1127,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  2146,     0,     0,
	  2146,  1507,     0,     0,  1507,     0,   922,     0,     0,  1507,
	     0,     0,     0,     0,   623,     0,     0,     0,     0,     0,
	     0,     0,  2580,  2581,     0,     0,     0,     0,     0,  1158,
	  3526,     0,  2588,     0,  1158,  3528,     0,  1576,   603,     0,
	     0,  1528,     0,   998,     0,     0,     0,     0,  3526,     0,
	  3542,  2602,     0,     0,     0,  2146,     0,     0,  2146,     0,
	  1578,  1579,  1580,     0,     0,  2602,     0,     0,     0,     0,
	     0,     0,  2602,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  1335,  2146,   680,     0,     0,     0,  2628,
	     0,     0,     0,     0,  3571,     0,     0,     0,     0,     0,
	  1335,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  3477,  3227,  3227,  3227,
	  2146,     0,     0,     0,     0,     0,     0,     0,  3227,     0,
	  3592,     0,     0,     0,     0,     0,     0,     0,  3595,     0,
	     0,     0,     0,     0,  3503,  3503,     0,     0,     0,  1690,
	     0,  2146,     0,   680,   680,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  3629,     0,     0,  2602,     0,     0,
	     0,     0,     0,     0,   603,   603,     0,     0,     0,  2714,
	   603,     0,     0,     0,     0,     0,  2602,     0,     0,  2726,
	     0,     0,  2602,     0,     0,     0,  1158,     0,     0,     0,
	     0,     0,  2146,  3647,     0,  2602,     0,  3650,     0,     0,
	     0,     0,     0,     0,  2146,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  3097,     0,     0,  2800,  3011,  2802,  2602,  2602,  2602,
	  2602,     0,     0,     0,  2602,  2602,     0,     0,  2602,  2602,
	  2602,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  1740,     0,  1743,     0,
	     0,   603,   603,     0,   603,  1212,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	   623,     0,     0,  2857,     0,     0,  3696,  2859,     0,     0,
	   603,     0,     0,     0,   679,     0,     0,     0,     0,   998,
	   998,     0,     0,     0,     0,     0,     0,     0,  2602,  2876,
	     0,     0,     0,     0,     0,  2602,     0,     0,  2602,     0,
	     0,  1862,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   603,   603,     0,     0,     0,     0,     0,
	     0,     0,  3227,     0,     0,     0,     0,     0,     0,  3477,
	     0,  2914,  3477,  3477,     0,  1690,  1690,  3477,     0,   680,
	  2137,  2281,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  3752,     0,  2121,     0,  2118,  2137,     0,
	     0,  2151,  2143,  2116,     0,     0,     0,  3011,  2285,     0,
	  1510,     0,     0,     0,     0,     0,  2155,     0,  2156,  2120,
	  2119,     0,     0,  3011,  2157,  2153,  3777,   707,  2117,     0,
	     0,     0,     0,  1023,     0,  2161,  2160,     0,     0,     0,
	  3783,     0,     0,     0,  2155,     0,  2156,     0,     0,     0,
	  2602,     0,  2157,  2139,  3793,  2602,   689,  3795,  1105,  3798,
	     0,  3800,     0,  2161,  2160,     0,     0,     0,     0,     0,
	     0,  1858,  1859,  1860,  1863,     0,     0,     0,     0,     0,
	     0,  2139,     0,     0,   689,  3404,  1105,     0,     0,     0,
	   679,     0,     0,     0,     0,     0,     0,     0,  1525,  1858,
	  1859,  1860,  1863,     0,     0,     0,     0,     0,  2602,     0,
	     0,     0,     0,  3729,   680,     0,     0,     0,     0,     0,
	     0,     0,     0,  3845,     0,  3227,     0,     0,  3227,     0,
	     0,     0,     0,     0,     0,     0,  3503,     0,  3854,     0,
	     0,     0,     0,   680,     0,     0,     0,     0,     0,     0,
	  2602,     0,     0,     0,     0,     0,  3860,     0,     0,     0,
	  2602,     0,  2148,     0,     0,  3031,     0,     0,     0,  2602,
	  2602,     0,  3871,     0,     0,     0,     0,     0,  1259,  2134,
	  2108,   605,  2132,  2135,  2133,  2149,     0,     0,     0,     0,
	  2148,     0,     0,     0,   554,     0,     0,   645,     0,     0,
	   556,   555,  2602,     0,     0,  2602,  1259,  2134,  2108,   605,
	  2132,  2135,  2133,  2149,     0,     0,     0,  3477,  3477,     0,
	     0,     0,   554,     0,     0,   645,  2089,     0,   556,   555,
	   680,   680,   680,     0,   680,     0,   680,     0,     0,  2602,
	     0,   680,  2092,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  2602,     0,     0,     0,     0,   782,     0,
	     0,     0,     0,  3126,  3227,     0,  3503,   680,     0,  2137,
	     0,     0,     0,     0,     0,     0,  1703,  1703,   836,   838,
	   839,     0,     0,     0,     0,     0,   836,     0,     0,     0,
	     0,   852,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  2602,     0,     0,  3477,     0,     0,  1706,  1707,
	     0,     0,     0,     0,     0,  2155,   916,  2156,     0,     0,
	     0,     0,  3185,  2157,     0,     0,     0,     0,     0,  3194,
	     0,     0,     0,     0,  2161,  2160,  1706,  1707,     0,     0,
	   603,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  2139,     0,     0,   689,     0,  1105,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  1858,  1859,  1860,  1863,     0,     0,     0,  3241,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,   623,  3549,
	     0,     0,  1510,     0,     0,   679,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,   603,     0,     0,     0,
	     0,  3268,  3273,  3272,     0,     0,     0,     0,     0,  1027,
	  1029,     0,  1031,     0,     0,   998,     0,     0,     0,     0,
	     0,  1031,     0,     0,     0,  3301,     0,  1042,  1043,  1044,
	  1045,  1046,  1047,  1048,  1049,     0,     0,  1054,     0,  1055,
	     0,     0,     0,     0,     0,     0,     0,   998,     0,     0,
	     0,  2148,     0,     0,     0,     0,     0,  1507,  2376,     0,
	     0,     0,     0,  2378,     0,     0,     0,  1259,  2134,  2108,
	   605,  2132,  2135,  2133,  2149,     0,     0,  2383,     0,     0,
	     0,     0,     0,   554,     0,     0,   645,  1742,  1062,   556,
	   555,     0,     0,     0,     0,     0,     0,     0,     0,   680,
	     0,     0,   922,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  3353,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,   680,     0,     0,     0,   680,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  2008,     0,     0,
	     0,     0,     0,     0,     0,     0,   484,   486,   487,     0,
	     0,     0,     0,     0,   488,   489,     0,     0,   680,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,   680,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   680,     0,     0,     0,     0,     0,  1706,  1707,     0,
	     0,     0,     0,     0,     0,   603,     0,     0,     0,     0,
	     0,     0,     0,     0,  3405,  3408,     0,     0,     0,  3416,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  3425,   680,     0,     0,     0,     0,     0,     0,   680,
	     0,     0,     0,     0,   680,     0,   680,     0,     0,     0,
	   539,     0,   557,     0,   561,   562,   563,   564,   565,   680,
	   566,   567,   568,     0,   569,   570,   571,   572,   573,   574,
	   575,   576,   577,   578,   579,   580,   581,   582,   583,   584,
	   585,     0,   586,   587,   588,   589,   590,   591,   592,   593,
	   594,     0,     0,   595,   596,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  3458,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  3494,  3495,   679,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,   679,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  3513,     0,     0,     0,     0,     0,     0,     0,   603,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,   998,   680,   844,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,   680,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  3548,   910,     0,  1507,
	     0,     0,     0,  1398,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  2137,  2281,     0,     0,     0,  1430,     0,
	     0,  1432,  1433,     0,     0,     0,     0,     0,     0,     0,
	     0,  3579,     0,     0,     0,     0,     0,     0,  2121,     0,
	  2118,  1458,     0,   836,  2151,  2143,  2116,     0,     0,  1471,
	  2761,  2285,     0,  1430,     0,     0,     0,     0,     0,  2155,
	     0,  2156,  2120,  2119,     0,     0,     0,  2157,  2153,   836,
	  1490,  2117,     0,     0,     0,   603,     0,     0,  2161,  2160,
	     0,     0,     0,     0,     0,  3612,     0,     0,     0,     0,
	     0,     0,     0,     0,  1546,     0,  2139,  3623,     0,   689,
	     0,  1105,     0,     0,     0,     0,     0,     0,     0,  1041,
	     0,     0,     0,     0,  1858,  1859,  1860,  1863,     0,     0,
	     0,     0,  1272,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  2855,     0,     0,     0,     0,     0,
	     0,     0,   998,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  3301,     0,     0,     0,     0,     0,     0,     0,  1064,   680,
	     0,     0,     0,     0,     0,     0,  1608,  1609,  1610,  1611,
	  1612,  1613,  1614,  1615,     0,  2148,     0,     0,  1619,     0,
	     0,     0,     0,     0,     0,     0,  1624,     0,     0,     0,
	     0,  1259,  2134,  2108,   605,  2132,  2135,  2133,  2149,     0,
	     0,     0,     0,     0,  2137,     0,     0,   554,     0,  1096,
	   645,     0,     0,   556,   555,   680,     0,     0,     0,     0,
	     0,     0,  3416,     0,     0,     0,  1119,     0,   603,  2121,
	     0,  2118,     0,   603,     0,  2151,  2143,  2116,     0,     0,
	     0,  3715,     0,     0,     0,     0,     0,     0,     0,     0,
	  2155,     0,  2156,  2120,  2119,     0,     0,     0,  2157,  2153,
	     0,     0,  2117,     0,  2137,     0,     0,     0,     0,  2161,
	  2160,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  2139,     0,  2121,
	   689,  2118,  1105,     0,     0,  2151,  2143,  2116,     0,     0,
	     0,     0,     0,     0,     0,  1858,  1859,  1860,  1863,     0,
	  2155,     0,  2156,  2120,  2119,     0,     0,     0,  2157,  2153,
	     0,   707,  2117,  3761,     0,     0,     0,     0,     0,  2161,
	  2160,  1706,  1707,     0,     0,     0,     0,     0,  2997,  2998,
	  2999,     0,     0,     0,     0,   680,     0,  2139,     0,     0,
	   689,     0,  1105,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  1858,  1859,  1860,  1863,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  3809,     0,     0,
	     0,     0,     0,     0,     0,     0,  2148,     0,     0,     0,
	     0,     0,     0,     0,     0,   603,     0,     0,     0,     0,
	     0,     0,  1259,  2134,  2108,   605,  2132,  2135,  2133,  2149,
	     0,     0,     0,     0,     0,     0,     0,     0,   554,     0,
	     0,   645,     0,     0,   556,   555,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  2148,     0,     0,     0,
	     0,     0,     0,     0,  1907,     0,     0,     0,     0,     0,
	     0,     0,  1259,  2134,  2108,   605,  2132,  2135,  2133,  2149,
	     0,     0,     0,     0,     0,     0,     0,     0,   554,     0,
	     0,   645,     0,     0,   556,   555,     0,     0,     0,     0,
	     0,  2137,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  1959,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  2121,     0,  2118,     0,
	     0,     0,  2151,  2143,  2116,     0,     0,  1984,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  2155,     0,  2156,
	  2120,  2119,  1706,  1707,  3307,  2157,  2153,  2001,  1862,  2117,
	     0,     0,     0,     0,     0,     0,  2161,  2160,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  1487,     0,     0,  2139,     0,     0,   689,     0,  1105,
	     0,     0,     0,     0,     0,     0,     0,  2137,     0,     0,
	     0,     0,  1858,  1859,  1860,  1863,     0,     0,     0,     0,
	     0,     0,  1706,  1707,     0,     0,     0,     0,     0,     0,
	     0,     0,  2121,     0,  2118,     0,     0,     0,  2151,  2143,
	  2116,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  2155,     0,  2156,  2120,  2119,     0,     0,
	     0,  2157,  2153,     0,     0,  2117,     0,     0,     0,     0,
	   680,     0,  2161,  2160,     0,     0,     0,     0,     0,     0,
	     0,  1546,     0,     0,     0,     0,     0,     0,     0,     0,
	  2139,     0,   657,   689,     0,  1105,     0,     0,     0,     0,
	     0,     0,     0,  2148,     0,     0,  1605,     0,  1858,  1859,
	  1860,  1863,     0,     0,     0,   680,     0,     0,     0,  1259,
	  2134,  2108,   605,  2132,  2135,  2133,  2149,     0,  1616,  1617,
	     0,     0,     0,     0,     0,   554,     0,     0,   645,     0,
	     0,   556,   555,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  1652,     0,     0,  2137,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  1673,     0,  1677,  2148,
	     0,     0,     0,  2121,     0,  2118,     0,     0,     0,  2151,
	  2143,  2116,     0,     0,     0,  1259,  2134,  2627,   605,  2132,
	  2135,  2133,  2149,     0,  2155,     0,  2156,  2120,  2119,     0,
	     0,   554,  2157,  2153,   645,  1742,  2117,   556,   555,     0,
	     0,     0,  2137,  2161,  2160,     0,  2309,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  1706,
	  1707,  2139,     0,     0,   689,     0,  1105,  2121,     0,  2118,
	     0,     0,     0,  2151,  2143,  2116,     0,     0,     0,  1858,
	  1859,  1860,  1863,     0,     0,     0,     0,     0,  2155,     0,
	  2156,  2120,  2119,     0,     0,     0,  2157,  2153,     0,     0,
	  2117,     0,     0,     0,     0,     0,     0,  2161,  2160,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  2139,     0,     0,   689,     0,
	  1105,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  1858,  1859,  1860,  1863,     0,  1862,  1862,
	  1862,     0,     0,     0,     0,  1706,  1707,     0,     0,  1862,
	     0,     0,     0,     0,     0,     0,  3464,     0,     0,     0,
	  2148,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  1259,  2134,  2108,   605,
	  2132,  2135,  2133,  2149,     0,     0,     0,     0,     0,     0,
	     0,     0,   554,     0,     0,   645,     0,     0,   556,   555,
	     0,     0,  3505,  3505,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  2148,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,   680,
	  1259,  2134,  2108,   605,  2132,  2135,  2133,  2149,     0,     0,
	     0,     0,   680,     0,     0,     0,   554,     0,     0,   645,
	     0,     0,   556,   555,     0,     0,     0,     0,     0,     0,
	     0,  1974,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  2492,     0,     0,     0,     0,     0,  2495,     0,     0,
	     0,     0,     0,     0,     0,     0,  1706,  1707,     0,  2502,
	     0,     0,  1430,     0,  2513,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  1471,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  2561,     0,     0,     0,     0,     0,     0,     0,
	  1706,  1707,     0,  1862,  1862,     0,     0,     0,     0,     0,
	     0,     0,     0,  1546,     0,     0,     0,     0,     0,     0,
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
	     0,     0,     0,     0,  2230,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  1862,     0,     0,  1862,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  3505,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	    17,   164,   319,   454,   361,   364,   367,   369,   370,   374,
	   375,   376,     0,   318,   223,   372,     0,     0,     0,     0,
	     0,     0,     0,     0,   356,     0,     0,     0,     0,     0,
	     0,     0,     0,   248,     0,     0,   335,   363,     0,   334,
	   205,     0,     0,  2701,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   323,   333,   441,     0,     0,     0,     0,
	     0,     0,     0,   360,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  1862,   458,     0,     0,     0,
	     0,     0,   378,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  3505,     0,     0,     0,     0,     0,
	     0,     0,  1907,     0,  2916,     0,     0,     0,     0,     0,
	   381,   230,   457,     0,     0,     0,   332,   383,     0,   379,
	     0,     0,   380,   365,     0,     0,   391,   386,   387,   389,
	   390,     0,     0,   206,     0,     0,   263,   397,   393,   460,
	   459,     0,     0,     0,   463,   461,   265,   275,   404,   410,
	   253,   400,   406,   465,   412,   408,   259,   258,   252,   247,
	   260,   464,   462,     0,   405,   399,   402,   401,   466,   467,
	   470,   468,   472,   471,   469,   475,   483,   473,   421,   424,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,    21,   162,    19,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   433,   438,   477,   476,   431,   435,   432,   427,   430,
	   309,   310,   308,   313,   314,   316,   312,   163,     0,  2553,
	   174,   440,   322,   325,   320,   321,   327,   373,   324,   326,
	   330,   394,   336,   340,   444,   442,   445,   446,   452,   448,
	   451,   449,   355,   207,   478,   429,   384,   385,   479,   437,
	   439,   426,   480,   481,   482,   455,   339,   337,   338,   447,
	   366,   371,   368,     0,   388,     0,   328,   453,   311,   434,
	   331,   315,   450,     0,     0,     0,     0,   377,    17,   164,
	   319,   454,   361,   364,   367,   369,   370,   374,   375,   376,
	     0,   318,   223,   372,     0,     0,     0,     0,     0,     0,
	     0,     0,   356,     0,     0,     0,     0,     0,     0,     0,
	     0,   248,     0,     0,   335,   363,     0,   334,   205,     0,
	     0,  2228,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   323,   333,   441,     0,     0,     0,     0,     0,     0,
	     0,   360,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,   458,     0,     0,     0,     0,     0,
	   378,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  2691,     0,     0,     0,     0,     0,   381,   230,
	   457,     0,     0,     0,   332,   383,  2699,   379,     0,     0,
	   380,   365,     0,     0,   391,   386,   387,   389,   390,     0,
	     0,   206,     0,     0,   263,   397,   393,   460,   459,     0,
	     0,     0,   463,   461,   265,   275,   404,   410,   253,   400,
	   406,   465,   412,   408,   259,   258,   252,   247,   260,   464,
	   462,     0,   405,   399,   402,   401,   466,   467,   470,   468,
	   472,   471,   469,   475,   483,   473,   421,   424,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,    21,   162,    19,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,   433,
	   438,   477,   476,   431,   435,   432,   427,   430,   309,   310,
	   308,   313,   314,   316,   312,   163,     0,     0,   174,   440,
	   322,   325,   320,   321,   327,   373,   324,   326,   330,   394,
	   336,   340,   444,   442,   445,   446,   452,   448,   451,   449,
	   355,   207,   478,   429,   384,   385,   479,   437,   439,   426,
	   480,   481,   482,   455,   339,   337,   338,   447,   366,   371,
	   368,     0,   388,     0,   328,   453,   311,   434,   331,   315,
	   450,     0,     0,  3334,     0,   377,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	    17,   164,   319,   454,   361,   364,   367,   369,   370,   374,
	   375,   376,     0,   318,   223,   372,     0,     0,     0,     0,
	     0,     0,     0,     0,   356,     0,     0,     0,     0,     0,
	     0,     0,     0,   248,     0,     0,   335,   363,     0,   334,
	   205,     0,     0,  2031,     0,     0,     0,     0,     0,     0,
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
	   459,     0,     0,     0,   463,   461,   265,   275,   404,   410,
	   253,   400,   406,   465,   412,   408,   259,   258,   252,   247,
	   260,   464,   462,     0,   405,   399,   402,   401,   466,   467,
	   470,   468,   472,   471,   469,   475,   483,   473,   421,   424,
	     0,     0,     0,     0,     0,  3040,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,    21,   162,    19,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   433,   438,   477,   476,   431,   435,   432,   427,   430,
	   309,   310,   308,   313,   314,   316,   312,   163,     0,  3095,
	   174,   440,   322,   325,   320,   321,   327,   373,   324,   326,
	   330,   394,   336,   340,   444,   442,   445,   446,   452,   448,
	   451,   449,   355,   207,   478,   429,   384,   385,   479,   437,
	   439,   426,   480,   481,   482,   455,   339,   337,   338,   447,
	   366,   371,   368,     0,   388,     0,   328,   453,   311,   434,
	   331,   315,   450,     0,     0,     0,     0,   377,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  3537,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,    17,   164,
	   319,   454,   361,   364,   367,   369,   370,   374,   375,   376,
	     0,   318,   223,   372,     0,     0,     0,     0,     0,     0,
	     0,     0,   356,     0,     0,     0,     0,     0,     0,     0,
	     0,   248,     0,     0,   335,   363,     0,   334,   205,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   323,   333,   441,     0,     0,     0,     0,     0,     0,
	     0,   360,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,   458,     0,     0,     0,     0,     0,
	   378,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,   381,   230,
	   457,     0,     0,     0,   332,   383,     0,   379,     0,     0,
	   380,   365,     0,     0,   391,   386,   387,   389,   390,     0,
	     0,   206,     0,     0,   263,   397,   393,   460,   459,     0,
	     0,  3332,   463,   461,   265,   275,   404,   410,   253,   400,
	   406,   465,   412,   408,   259,   258,   252,   247,   260,   464,
	   462,     0,   405,   399,   402,   401,   466,   467,   470,   468,
	   472,   471,   469,   475,   483,   473,   421,   424,     0,  3672,
	  3673,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,    21,   162,    19,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,   433,
	   438,   477,   476,   431,   435,   432,   427,   430,   309,   310,
	   308,   313,   314,   316,   312,   163,     0,     0,   174,   440,
	   322,   325,   320,   321,   327,   373,   324,   326,   330,   394,
	   336,   340,   444,   442,   445,   446,   452,   448,   451,   449,
	   355,   207,   478,   429,   384,   385,   479,   437,   439,   426,
	   480,   481,   482,   455,   339,   337,   338,   447,   366,   371,
	   368,     0,   388,     0,   328,   453,   311,   434,   331,   315,
	   450,     0,     0,     0,     0,   377
};

short yypact[] = {
	 -1000,  9932, -1000, -1000, -1000, -1000, -1000, -1000,   -38,   -38,
	   -38, -1000, -1000, -1000, -1000, -1000,   -38,   -38,    64, -1000,
	   781, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,   -38,   650,   -38,  2231,   -38,   -38,   -38,   -38,
	   -38, -1000,   -38,   -38,   -38, -1000,   -38,   -38,   -38,   -38,
	   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,   -38,
	   -38,   -38,   -38, -1000,   -38,   -38,   -38,   -38,   -38,   -38,
	   -38,   -38,   -38, -1000, -1000,   -38,   -38, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,  2260, -1000,  2073,  1429,  1429,  1429,  2091,
	   457,   521,  1429,  1429, -1000, -1000,  1882,   521,  1429,  1462,
	   457,  1429,   669,  2089,  1429,   457,  2324,  2376,  1429,   457,
	   457, -1000,  1429,  1429,   457,  1525,  2160,   341,   341,  2032,
	 -1000,   237,  2088, -1000,  2088, -1000, -1000, -1000,  3910, -1000,
	  2122,   521,  2159,  1875,  1462,  1462,   521,   457,   521,  1462,
	  1462,   521,  2770,  1641,  1602,  1462,  1426,  1833,   155,   521,
	  1302,  2430,  2298,  2766,   937,  2632,  2121,  1522,   624,  1346,
	  2050,  1325,  1346,  1345,  1345,  1129,  1767, -1000,  2269,  2111,
	   341,   593, -1000,  1971,  1555,   937,   937,  2216, -1000, -1000,
	 -1000,   -38,   937, -1000,  1376, -1000,  1331,   937,  1325,  1325,
	  1321,   535,  1324,  1866,  1319, -1000,  1666,  1321,   500,  2210,
	  1942,  1317,  1931,  1376,  1311,  1322,  1319,  1319,  1963,   -38,
	  2676,  1310,   937,  1306,   -25,   -25,  1280,  1865,   -25,  1297,
	   -25,   -25,  1293,  1291,  1248,  1864,   -25,   -25, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,  1863,  2081, -1000,  1647,
	 -1000, -1000, -1000,  3562, -1000, -1000, -1000, -1000, -1000,   521,
	 -1000, -1000,   795, -1000,  2673,  2514, -1000, -1000, -1000,  2734,
	 -1000,  2507,  1462, -1000,   457,  1429,  1429,   457,  1462,  1429,
	  1429,  1429,   240,   457,   457, -1000, -1000,  1462, -1000,  1462,
	 -1000,  2512,   341,  2079, -1000, -1000, -1000,  2544, -1000, -1000,
	  2377, -1000, -1000,   457, -1000, -1000, -1000, -1000, -1000, -1000,
	  1302, -1000,  1417, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,  2744, -1000, -1000,  1421,   937, -1000,   937, -1000,
	 -1000, -1000, -1000,  1783, -1000, -1000, -1000,   937, -1000,  1783,
	 -1000,   -38, -1000,   937,   937,   937,   937,   937,   937,   937,
	   937, -1000,  1232,   937, -1000,   937, -1000, -1000,   -25, -1000,
	 -1000, -1000, -1000, -1000,   -25, -1000,   -25, -1000, -1000, -1000,
	 -1000,  2336, -1000,   457, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,   937, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	   -38, -1000, -1000, -1000, -1000,  2655,  3712, -1000, -1000,  1716,
	 -1000, -1000,     6, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,   416, -1000, -1000, -1000,  1716,  1716,
	  1716,  1740,   -72,  1734, -1000,  2509, -1000,   -72,  1716, -1000,
	 -1000, -1000, -1000, -1000, -1000,  1576, -1000, -1000, -1000, -1000,
	 -1000,   -38,   579,  2763,   405,   261, -1000,   596, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,  1847, -1000,  2662,    76,  1289,
	  2229, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,     4,  1426, -1000,
	  1117, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1374,  2032,
	 -1000,  2032, -1000, -1000, -1000, -1000,  3738, -1000,  4521,   444,
	   444, -1000,  2621, -1000, -1000,   444,   444, -1000,  2152, -1000,
	  2152, -1000, -1000,   459,  1162,  2101, -1000,  1162, -1000, -1000,
	  1296,  2089,   521, -1000,   293, -1000,   457, -1000,  2032,   459,
	  1367, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  2657,  1462,
	 -1000,   240, -1000,  4521,  1966,  2032,  2032,   459,   459, -1000,
	  1041, -1000,  2299,  2299, -1000, -1000,  2032, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,   229,  1844,  1296, -1000, -1000,   459, -1000, -1000, -1000,
	  2774,  1896,  2774, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,   521, -1000,  2117, -1000,   561,   -88,  1296, -1000,
	 -1000,   237,   459, -1000,  2089,  2073, -1000,   916,  1836,  2078,
	 -1000,  2017, -1000, -1000,  2710,  1453, -1000, -1000,  2065, -1000,
	 -1000, -1000, -1000, -1000,  1525,   836, -1000, -1000, -1000,  1296,
	 -1000, -1000,  2306, -1000, -1000, -1000, -1000, -1000, -1000,  2150,
	   458,   444,  2622, -1000, -1000, -1000, -1000, -1000,  2014, -1000,
	 -1000, -1000, -1000, -1000,   405,  2714, -1000, -1000,   405,  2713,
	  1282, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	  2047,  2383, -1000,   535, -1000,   252, -1000,  1272,  1835,  1554,
	 -1000,   500, -1000, -1000, -1000,  2043,   252, -1000,   500, -1000,
	  1834, -1000,   320, -1000, -1000,  1736, -1000, -1000, -1000, -1000,
	 -1000,   729, -1000,  1831, -1000,  2150, -1000,  1196, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,  1436, -1000,   944, -1000,   937,
	  2210, -1000,  1270, -1000,  2676,  1436, -1000, -1000,   252,  2706,
	 -1000,  1786,  1433, -1000, -1000,   -79, -1000,  2035, -1000,  1825,
	  1269, -1000, -1000, -1000,  1284, -1000, -1000,   937,   937, -1000,
	 -1000,  1053, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1915,
	  1911,  2210, -1000,  1762, -1000, -1000, -1000,   937, -1000,  1295,
	  2139,  1199, -1000, -1000, -1000,  1151,  2041,  2041, -1000,  1032,
	  2041,  2041, -1000, -1000, -1000,   346,  1922, -1000, -1000, -1000,
	  2676, -1000, -1000,   -38, -1000,   691,   937,  2108,   236, -1000,
	 -1000, -1000, -1000, -1000, -1000,  2107, -1000, -1000,   252,  2706,
	  1254,   402,   252,  1252,  1217, -1000,  1722,  1722, -1000, -1000,
	   937,  1236,    15,  1217, -1000,   521,   457, -1000, -1000, -1000,
	 -1000, -1000,  2594, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	   459,  2385, -1000, -1000,   586, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,   521,  2089, -1000, -1000, -1000, -1000,
	 -1000,  1296, -1000,  1296,  1296, -1000, -1000, -1000, -1000,  2392,
	  1719,   457,   457,   512,  2506, -1000, -1000,  1296, -1000,  1296,
	  1296, -1000, -1000,  2299,  2831, -1000,  2590, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   564,  1302,  2156,
	  1996, -1000,   937, -1000,  2358,  1896, -1000,   758,   -38, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,   937,   937,   937,   937,   937,   937,   937,   937,
	   -38,   -38, -1000,   249,   937, -1000,   402, -1000,   402, -1000,
	 -1000, -1000,   937, -1000, -1000, -1000,  2293, -1000,    70, -1000,
	   416, -1000,    88,  1716, -1000, -1000,  2082,  2082,  1084,  1084,
	 -1000, -1000, -1000, -1000, -1000,    -9,   629, -1000,  1214, -1000,
	  1084, -1000,   541, -1000, -1000, -1000, -1000,   -38, -1000, -1000,
	  2703,  1431, -1000,   421,   421,  1209, -1000,  1822,  1817, -1000,
	  1462, -1000, -1000,  1820,  1817, -1000, -1000, -1000,   -38,  2678,
	   -38,   252,   252, -1000, -1000, -1000, -1000,  1429,  2032, -1000,
	 -1000,  1816,  1816, -1000, -1000, -1000,   459,  2172,  1815, -1000,
	 -1000, -1000, -1000,  1296, -1000, -1000,   -21, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  -192,
	 -1000, -1000,  4521, -1000,  1179,  2032,  2032,  2164,   -44,   -44,
	  1896, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,  1462, -1000,  1462, -1000,  2049,  1595,  1462, -1000,
	 -1000,  1462, -1000,  1462, -1000, -1000,   459,   457,   412,   457,
	 -1000,  2101,  1462, -1000,  2032,  1462,   457,  2606, -1000, -1000,
	 -1000,  2657, -1000, -1000, -1000, -1000,  2495, -1000, -1000,  1959,
	 -1000,   521, -1000, -1000, -1000,  2622,   108,  1462,  2622,  1580,
	 -1000,   529,   508,  2750,   261,  2118, -1000, -1000, -1000,  2073,
	  1035,   457, -1000, -1000,  4022,  1462,   459, -1000, -1000, -1000,
	   459,  1150,  2073, -1000,  1462, -1000, -1000, -1000,   309,  1462,
	  2032,  2032, -1000, -1000,  2622,  1462,  1462,  1462,   229,  1296,
	  1814, -1000, -1000, -1000, -1000, -1000,   459, -1000, -1000,   459,
	 -1000, -1000,  1990, -1000,  1426,  2715,   112,   979, -1000,   459,
	 -1000, -1000,  2682,  1813, -1000,  1910, -1000,  2605, -1000,   652,
	  1992,  1296,  1812,  1812, -1000, -1000, -1000,   155, -1000,  2312,
	  2323, -1000,  2843,   669, -1000,  1462,  1462, -1000,  1462,  1462,
	  1163,  1362, -1000,   421,   421, -1000,   261,  1431,   261, -1000,
	  1262, -1000, -1000,  1005, -1000,  2273, -1000,   500, -1000, -1000,
	  2039, -1000,  1474, -1000,  1430, -1000, -1000, -1000, -1000,  1809,
	  1128, -1000,   421, -1000, -1000,   -95,  1116, -1000, -1000, -1000,
	 -1000,  1091, -1000,   -96, -1000, -1000, -1000, -1000, -1000,  2077,
	   989,   989, -1000,   989, -1000,  2492,  2068, -1000,   446,  2694,
	   937,  2106, -1000, -1000, -1000, -1000, -1000, -1000,   944, -1000,
	  2210, -1000,  1682,   -38, -1000,  1073, -1000,   -19,   421,   261,
	 -1000,  1428,   -19,   937, -1000,   952,  1412, -1000, -1000,  1053,
	   421, -1000, -1000,  1637, -1000,   -19, -1000,   535,  1048, -1000,
	   -19,  1208, -1000,   937,   942,  1404, -1000,   -19,  1207, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,    86, -1000,   982, -1000,   436, -1000,  1045,
	 -1000, -1000,  1137, -1000, -1000, -1000,   901, -1000,  1392, -1000,
	 -1000,   -19, -1000,  1200, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,  2125,  1199, -1000, -1000,  9574, -1000,  1013, -1000,
	 -1000, -1000,   252, -1000,   981, -1000,   -97,   -98,  -100, -1000,
	   421,  1188, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,  1613,  1499, -1000, -1000, -1000,   261,   943, -1000,   -19,
	   261,   252,  1178, -1000, -1000, -1000, -1000, -1000, -1000,   930,
	 -1000,   -19,  1768, -1000,  1803,   911, -1000,   -19, -1000, -1000,
	   895, -1000,   -19,  1176,   887, -1000,   -19,   937,  1172, -1000,
	 -1000, -1000, -1000, -1000, -1000,  2101,  2101,   444,   444, -1000,
	  2032,  2499, -1000,  1546,  1162,  1296, -1000,  1247, -1000, -1000,
	 -1000, -1000, -1000, -1000,  1296, -1000, -1000, -1000, -1000, -1000,
	 -1000,  1802, -1000,   457, -1000,  2750,  2038, -1000,  1462, -1000,
	 -1000, -1000,  1800, -1000,  3658, -1000, -1000, -1000, -1000,  1158,
	 -1000, -1000, -1000, -1000, -1000, -1000,   701,   701,   500,   500,
	   500,   758,   500, -1000,   500, -1000, -1000, -1000, -1000,   500,
	 -1000, -1000, -1000,  1296, -1000,  2247, -1000,  1716, -1000, -1000,
	  1954,   241,    60, -1000, -1000,   499, -1000,  1692, -1000, -1000,
	   -72, -1000, -1000,   -54, -1000, -1000,  1798,  1474,   103, -1000,
	 -1000,  1457, -1000,  2283, -1000,   405, -1000, -1000,   319, -1000,
	 -1000, -1000, -1000,  1797, -1000, -1000,   496,  1062, -1000, -1000,
	 -1000, -1000,  9232, -1000, -1000, -1000,   -38,  2678, -1000,   870,
	 -1000,   421, -1000, -1000,   459, -1000,   459,  1429, -1000, -1000,
	  3738,  1614, -1000, -1000,   459, -1000, -1000,   459,   -44,   -44,
	   -44, -1000, -1000, -1000,  1896,  1896, -1000, -1000, -1000, -1000,
	   444,   444, -1000,  2037,  1681, -1000,  2562, -1000, -1000, -1000,
	  1462,   459,  2364,  7438, -1000, -1000,  2341,   459, -1000,  2338,
	  2394,  2392, -1000,   598, -1000, -1000,  -192,  2032,  1156,   521,
	   459, -1000, -1000, -1000,  7438, -1000, -1000,   598, -1000, -1000,
	   459, -1000, -1000,  2405, -1000,  1765, -1000, -1000,  1000,  2032,
	  2381,   412,  1296,  1681,  1000,  2297, -1000,  1462,  1462, -1000,
	  1795, -1000,   261,  2695,  1221,  1296,   457,  1794, -1000, -1000,
	 -1000,  1296, -1000,   598,  1462,  1462, -1000,  2428,  2422,  1730,
	   444, -1000,  1391, -1000, -1000, -1000, -1000, -1000,  2150,  1793,
	   598, -1000,  2101,  1681, -1000,   459, -1000,  2150,   838,  1462,
	  3909, -1000, -1000,  1462,  1462, -1000,  1426, -1000,   459,   457,
	  2265, -1000, -1000, -1000,   457,  2710, -1000, -1000, -1000, -1000,
	 -1000,  1462,  1453, -1000, -1000,  1896, -1000, -1000,   457,  1992,
	 -1000, -1000, -1000,  1434, -1000, -1000,  1760, -1000,  1179,  1179,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,  1179,  1179,   794,   459,
	 -1000,   459, -1000, -1000, -1000, -1000,  1462, -1000,  1296,  2540,
	 -1000,   893, -1000, -1000,  2622,  2622, -1000, -1000, -1000, -1000,
	  2535, -1000,  2150,  1000,  2744, -1000,  2527,   405, -1000, -1000,
	 -1000, -1000, -1000, -1000,   862, -1000,  -101, -1000, -1000, -1000,
	 -1000, -1000,  1371, -1000, -1000,  -129, -1000, -1000, -1000,  2243,
	 -1000,  2165, -1000, -1000,   502,   859, -1000,  1790, -1000, -1000,
	  1515,  -131, -1000,   252,  -134, -1000, -1000,   320, -1000,  1736,
	   450,   989, -1000,  1789, -1000, -1000, -1000, -1000,  2536,  1000,
	  2363,  2605,  7438, -1000,  -165,   816, -1000,   261, -1000, -1000,
	   261,  2105, -1000,  2212, -1000,    43, -1000,  1947,   982,   982,
	   982,  1731, -1000,   436,  2676, -1000,   252,   980, -1000, -1000,
	 -1000, -1000,  1786,   937, -1000, -1000,   252, -1000,   -19,  1364,
	 -1000,  1145,  -141, -1000, -1000,   937,  1363, -1000,  1284,   260,
	   937, -1000, -1000,  1053,  1140,  2372,  2676,  1762, -1000,  1043,
	 -1000, -1000, -1000, -1000, -1000,  1043,  1548, -1000, -1000, -1000,
	 -1000, -1000,   937, -1000,  1053, -1000,  1137, -1000, -1000, -1000,
	 -1000,   -38, -1000, -1000, -1000,   305,   411,   411,   411, -1000,
	   252, -1000, -1000, -1000, -1000, -1000, -1000,   252,   937, -1000,
	   894, -1000,   252, -1000,   252,  1722,   768, -1000,   -19,   -19,
	 -1000,  1722,   402, -1000,  1722,   402, -1000,   -25, -1000,   937,
	   402,   888, -1000,  1782,  1453,  1453,  1462,  1462, -1000, -1000,
	 -1000, -1000, -1000, -1000,  1525,  1914, -1000, -1000, -1000, -1000,
	  1256, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1296,
	 -1000,   -75,   239, -1000,  8287, -1000,  8287,  8287, -1000, -1000,
	 -1000,   150,   459,  2362, -1000, -1000,  1120, -1000, -1000, -1000,
	 -1000,  1111, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	  8223,  8052,   459,   459, -1000, -1000,   421,   421,  1102, -1000,
	  1460, -1000,   459, -1000,  1966, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,  2035, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   457,   -10,
	 -1000,  1692,  1921, -1000, -1000, -1000,    60,    60,   131, -1000,
	 -1000, -1000, -1000,  1084, -1000,   -72, -1000, -1000, -1000,   -72,
	   254,   609, -1000,  1692, -1000, -1000,   726,   513, -1000, -1000,
	 -1000, -1000, -1000, -1000,   -38,  2694,   405, -1000, -1000, -1000,
	 -1000,  1078, -1000,  1062,  1674, -1000, -1000, -1000,   -38,  8904,
	 -1000, -1000, -1000,   252, -1000, -1000,  1184,  1184,   598, -1000,
	 -1000,  1067,  1184, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,  1007,  1896, -1000, -1000, -1000, -1000, -1000,  1462,  1462,
	 -1000, -1000, -1000,  2833,  3899,  1179, -1000, -1000, -1000,  1462,
	 -1000,   457,   -57, -1000,  7438,  6515, -1000, -1000, -1000, -1000,
	 -1000, -1000,   255,   235,   459, -1000,   444, -1000,  1462, -1000,
	 -1000, -1000, -1000, -1000,  2092,  1462,  1686, -1000, -1000,  1896,
	  2804,   412,  1462,   -57, -1000,  1462,  2075,  2164,  2443, -1000,
	 -1000,  1296, -1000,  2109, -1000, -1000,  1000,  2150,  2744, -1000,
	  2374,  2622,  2049,  2535,  7438,  2822,  2068, -1000, -1000, -1000,
	 -1000,   261,  2032,  1429,  1429, -1000,  1429,  1296, -1000,  1296,
	 -1000, -1000,   598,   598, -1000, -1000,   459,  2536, -1000,  2480,
	  -192, -1000,  1462,  1779,  2032,   428, -1000, -1000,  1462,  1453,
	  2622,  1462,  1221,   457,  1777,  1296, -1000, -1000, -1000,  1296,
	 -1000,   598,   598, -1000, -1000,   735, -1000, -1000,  1772, -1000,
	   598, -1000,  1896,  1992, -1000, -1000,   459,   762, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,  1296, -1000,  -192,  1296, -1000,
	  1296, -1000,  -192,  1296, -1000,  1184,  1184,  1191, -1000,  -192,
	   669,  2806, -1000,   555,   716,   893,  1462,  1462,  2741,   879,
	  2164, -1000,   555,  2358,  1896, -1000, -1000, -1000,  1262, -1000,
	   937,  1683, -1000,   621,    79, -1000,  2234,  -144, -1000, -1000,
	 -1000, -1000,  1474,  1406,  1771, -1000, -1000,   387, -1000,  1736,
	 -1000, -1000, -1000, -1000, -1000,   421, -1000, -1000, -1000,  2741,
	  2164,  2492, -1000, -1000,  2603,  2164,   -57, -1000,   421,  2461,
	 -1000, -1000,   341, -1000, -1000,   261, -1000,  1056, -1000,  2676,
	  1682,   982,   982,  1043, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,   995,   882, -1000, -1000, -1000,  1637, -1000,
	  1705,  1626,   877,   252, -1000, -1000, -1000,   421, -1000, -1000,
	 -1000, -1000, -1000,   853, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,   421, -1000, -1000,   852, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,   700, -1000,   -19, -1000, -1000, -1000, -1000, -1000,
	   865,   846, -1000, -1000, -1000, -1000,   421, -1000, -1000,   799,
	 -1000, -1000, -1000,   -19, -1000, -1000, -1000, -1000,  1768,   402,
	    15, -1000, -1000, -1000, -1000,   796, -1000, -1000, -1000,   -19,
	 -1000, -1000,   444,   444, -1000, -1000,  1296, -1000, -1000,   457,
	   457,   457, -1000,  8287,  8287,   468,  1462,  8287, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   -58, -1000, -1000,
	   412,  4231, -1000, -1000,  2362,  4231, -1000, -1000,  2648, -1000,
	   218,  2491, -1000,  7438, -1000,    80,  2153,  1453, -1000,  8287,
	  8287, -1000, -1000,   252,  1766, -1000,  1462,   -38,  1296, -1000,
	    60, -1000,   -72, -1000,   -72, -1000, -1000, -1000, -1000, -1000,
	 -1000,   420,   -29, -1000, -1000,  2819, -1000, -1000,    35,   609,
	 -1000,   -72, -1000,   -22, -1000,    21, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,  1564, -1000, -1000, -1000,   408,  1692,
	  1692, -1000, -1000,   -29,    17, -1000, -1000, -1000,    12, -1000,
	 -1000,  2662,   261,  1362,   792, -1000, -1000, -1000, -1000, -1000,
	 -1000,   -38, -1000,   598,  1672,  4281, -1000, -1000, -1000, -1000,
	   598, -1000,  1462, -1000,   444,   444,   444, -1000, -1000, -1000,
	   598,   412, -1000,  1179,  2089,  1179,   412, -1000, -1000,  2240,
	 -1000,  1179, -1000, -1000,  3986,  1179,  1896,  3672, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,  2448,  2432,  1229,
	  1229, -1000, -1000,  2513, -1000, -1000, -1000, -1000, -1000,  2073,
	   598,   826,  7438,  7438, -1000, -1000, -1000,    72,   412,   160,
	  2150,  2358,  7956,  2561,  2258,  8287, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,  2786, -1000, -1000,  1824, -1000,  8287,
	  2786,  2519,  1547,   598,   806, -1000, -1000, -1000,  2032,   811,
	 -1000,  1587,   598, -1000, -1000,   598, -1000,  1672, -1000,  2055,
	  1413,   109,  1221,  1296, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,  1765, -1000,  1000,   555, -1000,  2358, -1000,  1000,  1296,
	 -1000, -1000,  2037,  2741, -1000,   -57, -1000,  1302, -1000, -1000,
	 -1000,  1296,  1296,  1296, -1000, -1000,  6543,  2741,  2089, -1000,
	  1530, -1000,  -151, -1000, -1000, -1000,   444,   444, -1000, -1000,
	  2150,   598,  1672,  2392,  1296, -1000, -1000, -1000,   598, -1000,
	  -192,  -192,  2185,  1579, -1000, -1000, -1000, -1000,  1429,   762,
	 -1000, -1000,  1179,  1092,  1179,   193,   598,   598, -1000, -1000,
	  1462,  2290, -1000,  8287,  2540,   669, -1000,  2565,  2563,  2560,
	  2558,  2622, -1000,   555, -1000, -1000,  2284, -1000,   404, -1000,
	 -1000,  4231, -1000,  7659, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,  1764, -1000,  -152, -1000,  -154, -1000,   937,
	 -1000, -1000,  1908, -1000, -1000, -1000, -1000, -1000, -1000,  2284,
	  4231, -1000,  7438, -1000,  4231,   470, -1000,   974,   260, -1000,
	 -1000,  -165, -1000,   -38, -1000, -1000, -1000,   982, -1000,   937,
	 -1000, -1000,   688, -1000,  -166, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,   784,   421, -1000,   -19, -1000, -1000, -1000,  1548,
	   490,   971, -1000, -1000, -1000, -1000, -1000, -1000,  2228, -1000,
	 -1000,   761, -1000,   -25,  1462,  1462,  1896, -1000, -1000, -1000,
	 -1000,  -150, -1000, -1000, -1000, -1000, -1000, -1000,   -58, -1000,
	  4231,  4231,  4231, -1000,  4231,   -58, -1000, -1000, -1000,  2491,
	 -1000,  8287, -1000, -1000,  8287, -1000,   296, -1000,  8287,  8287,
	  8287, -1000,    85,   137,   421, -1000,   459, -1000, -1000,   444,
	 -1000, -1000, -1000, -1000, -1000,   -72,  1649,    49, -1000, -1000,
	 -1000, -1000, -1000,  1492, -1000, -1000, -1000,    17,  1907,  1692,
	 -1000,  2647,   609, -1000, -1000, -1000, -1000,   -72, -1000, -1000,
	   678, -1000, -1000, -1000,  2641,   157, -1000,   -72,   -51, -1000,
	 -1000,  1716, -1000,   416, -1000, -1000, -1000,    49, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   590, -1000, -1000,
	 -1000,   598, -1000, -1000, -1000, -1000,  1429,  1429,  1462, -1000,
	 -1000,   459,  2002,  1462,   412, -1000, -1000,  2578,  -174, -1000,
	  3738, -1000, -1000,  4032,  1896,  3672,   598, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,   444, -1000, -1000,  2807,  1179, -1000,  1462, -1000,
	  2439, -1000, -1000,  -103, -1000, -1000,  2068,  8287, -1000, -1000,
	   212,   459,  2339, -1000,  7719,  8287, -1000, -1000, -1000, -1000,
	  2561, -1000,  7719, -1000,  -150, -1000, -1000,   459,  1854, -1000,
	 -1000,   212,   412, -1000,  2032, -1000, -1000, -1000, -1000,  2804,
	   812,  1462,  1906, -1000, -1000, -1000,  2790,  1896,  2480,  1462,
	 -1000,  2145,  1726, -1000, -1000, -1000, -1000,   457, -1000, -1000,
	  2443, -1000, -1000, -1000,  3658,  2535,  2049, -1000, -1000,  2284,
	 -1000,   598, -1000, -1000, -1000,  -150,  6543,  6543,  6543,   421,
	 -1000,   421, -1000, -1000, -1000,   444, -1000,  6824, -1000,  1992,
	  1462, -1000, -1000,   309,  1429,  1429,  2032, -1000,  1462, -1000,
	 -1000, -1000,   735,  4484,  4484, -1000, -1000,  7438,  2622, -1000,
	   598, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,  1724, -1000, -1000,  1896, -1000, -1000, -1000, -1000, -1000,
	 -1000,  1184, -1000, -1000, -1000,  2818,  -150,  2256,   669, -1000,
	 -1000, -1000, -1000, -1000,  1462,  2622,  1966, -1000,  1617, -1000,
	 -1000, -1000, -1000,  1296, -1000,   147, -1000, -1000,  1296, -1000,
	   -75,   137, -1000,   937,  -175, -1000, -1000,   937, -1000,   658,
	  2032,  1296,   -57,  1296,  -150,   341, -1000,   252, -1000, -1000,
	 -1000,  2827, -1000, -1000,   757, -1000,  1705,    69,  2035, -1000,
	   402, -1000, -1000, -1000, -1000, -1000,  2148,  1905,   746, -1000,
	   733, -1000, -1000, -1000,  4231, -1000,   -58, -1000, -1000,   -58,
	 -1000, -1000, -1000,   106,  -150,  8287, -1000,    45,   -58,   -58,
	  8287,  1681,   728,  2073,  1896,  1462, -1000,   460, -1000,   625,
	 -1000,   420, -1000, -1000, -1000, -1000, -1000, -1000,  1716, -1000,
	   609, -1000, -1000,  1564,   -15, -1000, -1000, -1000, -1000,  2082,
	  1692,   -29,  1672,  1296,  4281, -1000, -1000, -1000, -1000, -1000,
	  1179,  1296, -1000,  2032,  2032,  2032,  1179,  1184,  1296, -1000,
	 -1000,  1677,   598, -1000,  3672,   412, -1000,  2415,  1896,  2460,
	  1368,   598,  2083,  1745, -1000, -1000,  2605,  -150, -1000,   237,
	 -1000, -1000,  2519,   412,  -109,  8287,   412,    45,  1729, -1000,
	   459, -1000, -1000, -1000,   459, -1000,   642, -1000,   412,  2032,
	 -1000,  2087,  1901, -1000,   237, -1000, -1000,  2741, -1000, -1000,
	 -1000,  6543,  4879, -1000, -1000, -1000, -1000, -1000,   -58, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,    45, -1000,  3738,  3738,
	  1296, -1000,  1672, -1000, -1000, -1000, -1000, -1000,  1672, -1000,
	 -1000, -1000,  4281,  4281, -1000, -1000, -1000,   396, -1000,   -57,
	  1462, -1000, -1000, -1000,   598,  1302, -1000,  7438,   459, -1000,
	   459, -1000,  1179,   404, -1000, -1000,  4231, -1000,  8287,   468,
	  8287,  1462,  1356, -1000, -1000,   937,   937, -1000,  3672,  1900,
	 -1000,  1179,  4231,   720,   421, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  2800,
	  1993,  2796,   631, -1000, -1000, -1000, -1000, -1000,  8287,  -150,
	 -1000,  8287, -1000, -1000,   -16,  2833, -1000, -1000,  1655,   598,
	 -1000,  1649, -1000, -1000, -1000, -1000, -1000, -1000, -1000,    60,
	 -1000, -1000,   459, -1000,   412,  2002, -1000,  1179, -1000, -1000,
	  1184, -1000,  2425, -1000, -1000,   590,  2496, -1000, -1000,  3672,
	  1179, -1000,   412, -1000,  1896,  2083,  2439, -1000, -1000, -1000,
	  2792,  1624,  2603, -1000, -1000, -1000,  8287,  -109, -1000,  8287,
	   412,  1729,   964, -1000,  1899,  2790, -1000,  2068,  1581,   220,
	 -1000, -1000, -1000, -1000,  6543, -1000, -1000,  6543,  1296,  1296,
	  1462, -1000, -1000, -1000, -1000,  3815,  1579, -1000, -1000,   -57,
	  1462, -1000,  1462,  2450, -1000,  2288, -1000, -1000, -1000,  -150,
	 -1000,   937, -1000, -1000, -1000,   660,  1296, -1000,   459,  -150,
	 -1000,   628, -1000, -1000,  2348, -1000, -1000,  1898,  -150,   -58,
	 -1000,  8287,   412,   459, -1000, -1000,  1896, -1000,   -72,  2398,
	   -72,  4281, -1000, -1000,  -192, -1000, -1000,   369, -1000,   598,
	 -1000,  1521, -1000,  3672,  3672,   412, -1000, -1000, -1000, -1000,
	 -1000,  -150,  8287,  1296,  -150, -1000,   412,   412, -1000,  4281,
	  4281,   608, -1000, -1000,  2072,  1897,   -58,    45, -1000, -1000,
	   598,  2622,   598, -1000,   598, -1000,  2816,  2730, -1000, -1000,
	 -1000,  3672,  1179,  1179, -1000, -1000,   501, -1000,   -58, -1000,
	  1896,   412, -1000,  2521, -1000,   335, -1000, -1000,   598,   459,
	  1482,  4309, -1000, -1000, -1000, -1000, -1000,   590, -1000, -1000,
	 -1000,  3672,  -150,  8287, -1000, -1000, -1000, -1000, -1000,  1478,
	  2070, -1000, -1000,  6543, -1000,  4484,  1462, -1000,  1462, -1000,
	  1462,  1425, -1000, -1000, -1000, -1000,  1296, -1000, -1000,   412,
	 -1000,    57, -1000,    82, -1000, -1000, -1000,    49, -1000, -1000,
	 -1000,   598, -1000, -1000, -1000, -1000, -1000,  4281, -1000, -1000,
	 -1000,  -150, -1000,  1894,   -58,   412, -1000, -1000, -1000,  1296,
	 -1000, -1000,  1425,   444, -1000,  1179, -1000,   -72,   408,   335,
	 -1000, -1000,   420, -1000,  1482,   598, -1000,   210, -1000, -1000,
	  1425, -1000, -1000, -1000,   412, -1000, -1000, -1000, -1000, -1000,
	 -1000,  4281,  1892, -1000, -1000, -1000, -1000, -1000
};

short yypgo[] = {
	     0,  5049,  3131,  5048,  5047,  5046,  5041,  5040,  5039,  7397,
	  5038,  5037,  5036,  5035,  5034,  5033,  5032,  5031,  5030,  5029,
	  5028,  5027,  5026,  5025,  5024,  5023,  5022,  5021,  5020,  5019,
	  5018,  5017,  5016,  5014,  5013,  5012,  5011,  5010,  5009,  5008,
	  5007,  5006,  5005,  5004,  5003,  5002,  4997,  4996,   405,  4995,
	  4994,   398,  4992,  4988,  4987,  4986,  4985,  4984,  4982,  4981,
	  4978,  4976,  4975,   389,  4966,  4965,  4964,  4963,  4962,  4961,
	  4960,  4959,  4958,  4957,  4956,  4955,  4954,  4953,  4952,  4951,
	  4950,  4949,  4948,  4947,  4944,  4942,  4941,  4939,  4936,  4935,
	  4934,  4933,  4932,  4931,  4930,  4929,  4928,  4927,  4926,  4925,
	  4924,  4923,  4922,  4921,   267,   440,  4920,  4919,  4917,   199,
	   637,  4916,   316,   309,  4915,   239,   367,  4914,  4913,  4911,
	  4910,  5478,   422,  4908,   406,  4907,   123,   392,   161,     0,
	  4906,   100,   335,  4902,  3130,  4900,  4898,   399,    88,  4897,
	  4883,  4882,   344,  4357,  4881,  4880,    40,   103,  4879,  4878,
	  4873,   394,    70,  4872,  4871,   651,   337,  4869,  4868,   231,
	  4867,  4866,  4865,  4356,  4864,   301,  4859,  4858,  4857,  4856,
	  4853,  2020,  4852,   138,  4851,  4846,    29,  3136,    34,   149,
	  4841,  4840,  4839,   323,  4838,  2913,   107,  4837,   128,   115,
	    65,  4836,   393,   250,  4835,  4834,   331,  4830,  4826,  4824,
	  4823,  4817,  4816,  4814,    31,   391,    19,  4810,   152,    98,
	  4806,  4804,  4803,  4802,  4801,   420,   311,  4794,  4790,  4789,
	  4788,    94,    77,    22,    12,    80,  1816,  3300,  4786,    32,
	  4784,  4782,  4781,  4780,  4778,   142,   318,    23,   242,   332,
	   328,   386,   137,    51,  4776,    24,  4775,  4774,  4773,  4766,
	  4762,  4758,  4754,  4753,   418,  4751,   390,  4750,  4749,   425,
	  4748,  4747,  4745,  4743,  4742,  4740,  4739,   162,  4738,   109,
	   160,  4735,    68,  4734,   396,  4732,    57,  4731,  4729,  4727,
	   407,  4726,   153,   133,    60,  4725,   114,    82,    44,    52,
	    75,    81,    96,  4724,   325,  4723,  4722,   121,   111,  4713,
	   336,   310,    39,  4712,    69,    35,  4710,  4706,  4705,  4700,
	   157,  4699,  4696,  4695,  4694,    97,  4693,   156,  4692,  4691,
	  4690,  4689,  4684,  4683,    62,  4682,  3145,  4681,   166,  4680,
	  4678,  4677,  4675,  4674,   163,    55,    59,    63,    56,  4673,
	    13,    86,    11,   387,    48,  4672,    37,  4671,   140,   117,
	    41,  4669,  4668,  4666,  4665,  3127,   197,  4664,  4662,  4661,
	  4654,   147,  1049,  4653,   198,  4652,  3112,   252,  4651,   235,
	    99,  4648,  4646,   241,   164,   101,   302,   187,  4645,   214,
	  4641,  4640,    20,  4638,   341,  4637,   353,   262,   300,  4636,
	   254,  4635,    64,    36,  4634,   247,   146,  4633,  4632,  4631,
	  4629,   298,  4627,  4626,  4625,  4624,   113,    49,  4623,    61,
	   249,  4622,   308,   136,    54,  4620,    58,  4619,  4617,  4616,
	  4615,   154,  4614,    79,  4613,  4612,  4606,  4605,  4604,  4603,
	   245,  4601,  4600,  4599,  4598,  4597,    47,  4596,  4595,     4,
	    28,  4593,  4591,    30,  4590,  4588,  4587,  4585,  4582,  4581,
	   243,   120,   139,   176,   118,  4579,  4577,  2838,   175,   125,
	  4576,  4575,  4574,  4573,   170,  4570,  4569,  4568,    42,  4565,
	  4564,   282,   207,  4563,  4558,   104,    90,  4557,  4553,  4549,
	  4548,  4547,   202,  4545,  4544,  4543,  4541,  4540,  4535,  4534,
	  4533,  4532,  4527,  4523,   132,   130,    78,  4522,   204,  4520,
	  4519,   210,  4518,  4514,  4513,  4512,  4511,  4510,    67,  4509,
	  4508,  4505,   110,  4504,  4503,  4501,  4497,  4496,  4494,  4492,
	  4491,  4490,  4489,   334,  4484,  4483,  4482,  4481,   421,  4480,
	   259,  4479,  4477,  4476,  4475,  4474,  4468,  4467,  4466,  4465,
	  4464,  4463,  4462,  4461,  4460,  4458,  4457,  4456,  4455,   155,
	  4453,  4452,  4451,  4449,  4444,  4443,  4441,  4440,  4436,  4435,
	  4431,  4430,   266,  4429,  4427,  4423,  4422,  4421,  4419,  4418,
	   322,   326,   324,  4417,  4414,  4413,  4412,  4411,  4410,  4407,
	   251,  4405,  4404,   165,  4403,  4402,  4400,  4398,  4397,  4393,
	   388,   419,  4392,  4387,  4385,  4383,  4381,  4377,  4376,  4373,
	  4372,  4371,  4370,  4368,  4363,    38,  4362,  4361,  4359,    33,
	    73,  4358,    92,  4355,  4352,  4351,  4350,   246,   151,  4349,
	  4346,   234,  4339,  4338,  4337,  4336,  4335,  4334,  4333,  4331,
	  4329,  4328,  4326,  4322,  4319,  4316,  4313,  4312,  4309,  4307,
	  4306,  4297,  4295,  4292,   304,   381,   307,   305,   319,   148,
	  4291,   248,  4290,  4289,  4288,   385,  4287,   150,   315,   158,
	  4279,  4278,  4277,  4275,   378,  4274,  4273,  4271,  4270,  4269,
	   597,   253,  4265,   122,   339,  4263,  4261,  4260,  4259,   192,
	  4254,  4252,  4251,  4249,    15,    21,    16,  4246,  4245,  4244,
	    71,  4240,  4239,  4238,    53,  4235,    89,  4234,  4233,    25,
	  4231,    17,     8,     1,  4230,  4226,  4225,  4224,  4221,  6853,
	  4218,  4216,   240,  4213,  4211,  4210,  4209,  4208,  4204,    18,
	  4203,  4202,  4201,  4200,  4196,  4195,  4190,  4189,  4188,  4187,
	  4186,  4185,   383,  4184,   376,  4180,  4179,  4177,  4174,  4170,
	  4168,  4167,  4165,  4162,  4161,  4159,  4158,   374,  4156,   244,
	  4155,  4154,  4153,  4151,  4150,  4149,   236,  4138,   361,   366,
	  4137,   238,  4135,  4133,  4132,   188,  4131,  4130,  4129,  4128,
	  4126,  4122,  4119,  4118,   184,  4117,   177,  4115,  4114,  4112,
	  4110,  4109,  3122,  4108,  4106,  4103,  3118,  4102,  4101,  4100,
	  4095,  4094,  4093,   299,   232,  3881,  4092,  3103,  4091,   306,
	   223,   303,  4089,  4088,  4087,  4086,  4085,   296,  4077,  4348,
	  4075,  4073,  4070,   297,  4068,   403,   368,  4064,  4062,  4061,
	  4059,  4058,  4057,  4056,  4052,   257,  4051,  4050,   340,  4049,
	  4048,   338,  4047,  4046,  4045,  4044,  4043,   135,  4040,   227,
	  4039,  4038,  4033,  4032,  4027,   105,   145,  4025,  4024,  4023,
	  4022,   255,  4020,  4015,  4014,  4012,  4011,  4010,  4004,  4003,
	  4002,  4001,  4000,  3996,  3995,  3992,  3990,  3989,  3986,  3985,
	  3984,  3980,  3978,  3977,  3976,  3975,  3974,  3973,  3967,  3957,
	  3955,  3954,  3953,  3951,  3950,  3949,  3947,  3946,  3945,  3942,
	  3941,  3940,  3939,  3938,  3937,  3936,  3932,  3931,  3930,  3929,
	  3928,  3927,  3926,   225,  3925,  3924,  3922,  3920,  3918,   417,
	  3917,  3916,   380,  3914,  3913,  3912,  3910,  3909,  3906,  3905,
	   365,  3902,   102,  3901,  3894,   416,  3892,  3885,   375,   377,
	  3883,   295,   233,  3873,   144,   224,  3872,   229,  3849,  3842,
	   143,  3841,   129,  3838,  3837,  3836,  3833,   237,  3832,  3829,
	  3819,  3818,   370,  2683,  3817,  2678,  3813,  3809,   290,  3807,
	  3806,  3805,  3803,  3065,  3795,  3780,  2648,   402,  3779,  3776,
	   294,  3775,   372,  3763,  3761,  3760,  3759,  3758,  3756,  3755,
	   213,  3754,  3753,  3747,  3746,  3744,    85,  3743,  3741,    91,
	    87,  3740,  3738,  3735,  3733,  3732,  3731,  3730,   342,  3729,
	  3724,  3722,  3721,  3719,   283,  3715,  3714,   286,  3712,  3710,
	  3708,  3706,  3705,   411,  3703,  3702,   285,  3700,  3699,  3698,
	  3697,  3696,   379,  3695,  3694,  3693,  3692,  3691,  3683,  3678,
	  3677,  3675,  3673,  3667,  3666,   281,  3664,  3660,  3638,  3636,
	  3629,  3628,  3625,  3624,  3623,   414,  3622,  3619,  3618,  3615,
	  3611,  3610,  3609,  3608,  3605,  3603,  3602,  3599,  3596,  3595,
	  3594,  3592,  3587,  3586,  3585,  3580,  3577,  3575,  3574,  3572,
	   287,  3571,   289,   313,   119,   108,   106,    83,   409,  3570,
	  3561,  3559,  3557,  3555,  3551,  3550,  3548,  3547,  3546,  3545,
	  3544,  3542,  3537,  3534,  3526,  3523,  3520,  3518,  3512,   275,
	   276,  3507,   270,  3479,   159,  3476,  3474,  3471,  3469,   404,
	  2930,  3464,   280,  3463,  3461,  2957,  3458,  3456,   208,  3452,
	  3444,  3436,   292,   269,  3434,  3430,   216,  3423,   410,  3421,
	  3420,  3419,   284,   278,  3417,  3416,   273,  3415,  3414,  3413,
	  3412,   274,  3411,  3410,   211,  3409,  3407,   271,  3404,  3403,
	  3402,  3401,  3400,  3399,  3397,  1749,    74,  3396,  3394,  3393,
	  3390,  3388,  3387,  3384,  3383,  3381,  3380,  3379,     5,   112,
	  3375,    66,    43,    10,  3374,    93,   124,    50,  3366,  3360,
	   272,  3352,   263,   131,   264,   397,   261,     7,     9,  3342,
	    76,   141,   268,   258,   260,  3324,  3322,  3321,   364,   201,
	  3314,  3313,  3310,  3306,  3305,  3303,    84,  3299,  3297,  3293,
	  3292,  3290,  3288,  3277,  3274,  3273,  3272,   116,    72,    95,
	  3271,  3262,  3261,  3260,   200,  3259,     6,  3258,  3257,   127,
	  3251,   126,    46,     2,    14,  3250,    27,    26,  3247,  3240,
	  3234,     3,  3233,  3232,  3225,  3224,  3223,  3222,  3220,  3218,
	  3216,  3213,  3209,  3206,  3205,   134,  3204,   401,  3186,  3184,
	  3182,  3181,  3180,  3179,   363,  3177,  3175,  3174,  3173,   265,
	  3172,  3169,  3168,  3166,    45,  3165,  3164,  3163,  3161,  3160,
	  3159,   356,  3155,  5441,  4276,   359,   317,  3148,  3147,   400,
	  3143,  3141,  3139,  3138,  3134,   256,  3121
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
	     9,    20,     7,   813,   813,   813,   973,   812,   570,   570,
	   590,   590,     6,  1150,  1152,  1151,  1151,  1151,  1153,  1153,
	  1155,  1155,  1155,  1156,  1156,  1156,  1156,  1156,  1156,  1156,
	  1156,  1157,  1157,  1158,  1158,  1168,  1168,  1175,  1170,  1170,
	  1170,  1171,  1180,  1182,  1181,  1181,  1181,  1181,  1183,  1184,
	  1191,  1191,  1185,  1192,  1174,  1194,  1195,  1172,  1172,  1198,
	  1159,  1160,  1160,  1160,  1200,  1203,  1205,  1205,  1207,  1207,
	  1207,  1207,  1207,  1207,  1207,  1208,  1214,  1215,  1216,  1216,
	  1218,  1218,  1209,  1220,  1210,  1210,  1221,  1222,  1222,  1211,
	  1225,  1226,  1226,  1226,  1226,  1226,  1226,  1228,  1228,  1231,
	  1232,  1233,  1229,  1235,  1235,  1237,  1239,  1239,  1241,  1241,
	  1243,  1243,  1212,  1213,  1201,  1202,  1247,  1247,  1161,  1162,
	  1253,  1253,  1252,  1252,  1255,  1255,  1163,  1163,  1257,  1257,
	  1259,  1259,  1261,  1154,  1263,  1263,  1265,  1264,  1266,  1266,
	  1266,  1266,  1267,  1267,  1164,  1271,  1272,  1273,  1273,  1274,
	  1274,  1275,  1276,  1165,  1277,  1187,  1187,  1187,  1187,  1219,
	  1190,  1190,  1224,  1224,  1224,  1224,  1166,  1166,  1278,  1279,
	  1279,  1279,  1279,  1167,  1280,  1280,  1281,  1281,  1281,  1282,
	  1282,  1282,  1245,  1196,  1240,  1199,  1234,  1169,  1176,  1178,
	  1223,  1189,  1188,  1236,  1258,  1268,  1206,  1249,  1193,  1177,
	  1246,  1230,  1197,  1244,  1269,  1251,  1242,  1248,  1254,  1256,
	  1186,  1227,  1270,  1260,  1217,  1173,  1262,  1179,  1250,  1204,
	  1238,  1283,  1284,  1284,   677,   756,   512,   457,  1285,   523,
	   523,  1287,  1288,  1288,   390,   343,  1290,  1286,  1286,  1291,
	  1289,  1289,  1292,  1292,  1294,  1296,  1293,  1295,  1295
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
	     1,     1,     1,     2,     1,     1,     1,     2,     1,     1,
	     1,     1,     3,     3,     4,     0,     1,     1,     1,     2,
	     1,     1,     2,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     6,     6,     1,     3,     1,     0,     1,
	     1,     2,     1,     1,     0,     1,     1,     1,     4,     3,
	     0,     1,     1,     1,     1,     1,     4,     0,     1,     1,
	     5,     1,     1,     1,     2,     5,     0,     1,     1,     1,
	     1,     1,     1,     1,     1,     2,     1,     2,     1,     3,
	     1,     1,     1,     4,     1,     1,     2,     2,     2,     4,
	     1,     1,     2,     1,     2,     2,     1,     1,     2,     5,
	     0,     0,     9,     1,     2,     4,     1,     3,     1,     1,
	     1,     3,     2,     2,     4,     7,     0,     1,     5,    10,
	     0,     2,     0,     1,     1,     2,     4,     6,     0,     1,
	     1,     1,     1,     3,     1,     3,     1,     5,     0,     1,
	     2,     1,     0,     1,     7,     2,     0,     1,     3,     1,
	     2,     1,     1,     2,     1,     1,     1,     1,     1,     1,
	     1,     2,     1,     2,     1,     2,     3,     1,     1,     1,
	     1,     2,     3,     6,     2,     4,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     1,     1,     2,     1,     1,
	     1,     1,     1,     0,     1,     2,     2,     2,     1,     1,
	     2,     2,     1,     0,     2,     2,     0,     1,     2,     2,
	     2,     1,     2,     3,     1,     2,     2,     1,     0
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
	   558,   559,   560,   452,    -9,   614,    -9,    -9,    -9,    -9,
	   -19,   -21,   617,   486,   485, -1151, -1153, -1154, -1155, -1193,
	 -1156, -1157,   256,   502, -1158, -1159, -1160, -1161, -1162, -1163,
	 -1164, -1165, -1166, -1167, -1168, -1200, -1201, -1202, -1250, -1251,
	 -1256, -1258, -1277, -1278,   494, -1280, -1175, -1203, -1204,   612,
	   603,   399,   598,   469,   493,   392, -1177,   613,   485,    -9,
	  -819,  -820,   290,  -673,  -826,  -827,  -209,  -215,  -457,  -670,
	  -671,  -216, -1283,   485,   499,   506,   505,    -9,  -830,  -849,
	   410,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,
	    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,
	    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,
	    -9,    -9,    -9,    -9,    -9,    -9,    -9,   400,  -292,   463,
	  -104,  -115,  -129,  -143,  -669,   486,  -670,  -671,  -104,  -115,
	  -118,  -119,  -116,   485,   431,   401,  -155,   468,  -121,  -129,
	  -147,  -216,  -124,  -121,  -315,  -129,  -317,   348,  -134,  -115,
	  -134,  -532,   485,  -124,  -104,  -129,  -121,  -104,  -182,  -183,
	  -184,  -185,  -109,  -124,  -129,   502,  -186,   462,  -115,  -121,
	  -198,  -205,  -206,   332,   338,   310,   319,   456,  -252,  -254,
	   352,  -258,  -115,  -121,  -121,  -115,  -115,  -121,  -315,  -129,
	  -319,   458,  -673,  -673,  -173,  -116,  -861,  -143,  -512, -1284,
	 -1283,  -326,   456,   411,   334,   342,   399,  -328,  -129,   361,
	  -326,  -586,  -206,  -589,  -590,  -240,  -239,  -423,  -572,  -571,
	  -237,  -129,  -301,   589,   606,   310,   319,   332,   289,   262,
	   338,   357,  -241,   460,  -185,  -854,   458,   485,  -238,  -129,
	  -209,  -124,  -121,  -124,  -129,  -129,  -124,  -565,   280,   485,
	   463,  -211,  -217,  -218,  -230,  -246,   485,   466,  -129,  -530,
	   483,   485,   499,  -171,  -407,  -317,   485,   315,  -663,  -664,
	  -646,  -647,  -665,   580,   581,   582,  -352,  -353,  -354,  -355,
	  -129,  -147,  -315,  -324,  -209,   486,  -457,  -514,   391,   256,
	  -518,  -366,  -522,   256,  -386,   281,   460,  -525,  -527,   281,
	  -706,  -708,  -709,  -734,   485,   499,  -801,  -457,  -795,   486,
	  -716,  -720,  -727,  -733,   301,  -753,   460,   334,  -777,   501,
	  -780,   348,   456,  -734,   390,  -782,   502,  -788,  -789,  -791,
	   468,  -797,   502,  -782,  -786,   502,  -786,  -805,   485,  -795,
	  -457,  -811,   485,   276,   399,  -366,  -834,  -835,  -673,  -907,
	  -512,   486,   476,   472,   456,  -909,  -709,  -911,  -709,  -709,
	  -916,  -923,  -924,   417,    -9,  -909,  -952,  -953,   502,   281,
	  -957,   502,  -709,  -797,  -797,  -963,   502,  -734,  -992,   502,
	  -995,   485,  -998,  -953,   502, -1001,   475,   472,   480, -1002,
	   485,  -963,  -732,  -800,   484,   488,  -512,  -809,   483,   624,
	   623, -1011, -1013, -1014,  -929,   419, -1018,   472, -1020,   502,
	 -1024,   479,  -952,  -953, -1031,   502,  -925,  -953,  -998,   502,
	  -998,  -953,  -998,  -953, -1053, -1055, -1057,   463,   477,   478,
	    -9,  -903,  -904,   298, -1067,   502,  -709, -1095,   502, -1104,
	 -1115, -1147,  -809,  -756, -1284, -1104, -1107, -1110,   502,   281,
	   485, -1115,   502, -1104, -1104, -1130,   502,   502, -1138, -1110,
	   502,   485, -1104, -1104,   485,  -141,  -144,   334,   480,   334,
	   485,   315,   337,   317,   320,   328,   399,   353,   334,   342,
	  -185,   346,  -324,  -759,  -760,   485,   499,  -216,   310,   319,
	   338,   332,   334,   290,  -535,  -536,   334,   342,   317,   320,
	   328,  -238,  -121,  -104,  -104,  -121,  -129,  -115,  -115,  -115,
	  -259,  -273,  -274,  -186,   462,   571,  -121,  -410,  -121,  -238,
	  -238,  -604,   334,  -673,   463,  -619,   337,   317,   334,   399,
	   342,  -630,   334,   342,   317,   399,  -209,  -215,  -519,  -520,
	  -324,  -719,   463,  -709,  -373,  -376,   282,  -709,   429,  -709,
	  -955,  -709,  -966,   486,   485,   472,   480,   334,   411,  -955,
	  -966,    -9,  -709,  -709,  -709,  -709,  -709,  -709,  -709,  -709,
	  -947,  -953,  -952,   502,  -709,  -709, -1104, -1104, -1104,   399,
	   315,  -121,  -709,   -20,    -9, -1152,   299, -1155, -1263, -1264,
	 -1168,   614, -1169, -1176,   505,   501, -1175, -1177, -1177, -1259,
	 -1177, -1262,   486, -1173,   614, -1279,   485,   486,   334, -1173,
	 -1175, -1281, -1282,   485,   499,   486,    -9,  -821,   486,  -512,
	   281,  -528,  -828,  -512,  -346,   363,  -523, -1287, -1288,   505,
	   504, -1285, -1286, -1291, -1290,   485,  -364,  -383,   298,    -9,
	  -832,   460,   502,   411,   614,  -530,  -105,  -177,  -135,  -116,
	   501,  -112,  -113,   303,   311,  -107,  -108,   311,  -111,  -112,
	  -113,  -116,  -116,  -610,  -679,  -680,  -681,  -222,  -683,   485,
	   334,   270,   280,   338,   315,   360,   359,   317,  -457,  -605,
	   385,  -122,  -150,  -151,  -152,  -154,  -157,  -158,  -160,  -161,
	  -162,  -164,  -165,  -166,  -167,  -168,  -169,  -170,   301,   375,
	   374,   366,   367,   368,   364,   365,   327,   371,   372,   369,
	   370,   373,  -146,   504,  -146,  -125,  -112,  -113,  -146,  -105,
	  -105,  -109,  -116,  -142,   461,  -173,  -177,  -186,  -183,  -279,
	   505,  -192,  -121,  -116,  -196,  -109,  -199,  -177,   462,   274,
	   463,  -253,  -256,   -48,   -51,   -63,   309,  -255,  -129,  -259,
	  -122,  -276,   469,  -173,  -173,  -196,  -280,  -109,  -196,  -320,
	  -209,  -215,   348,  -815,   460,   281,  -815,  -173, -1285,  -587,
	  -186,  -274,  -588,   485,  -177,  -109,  -591,   279,  -163,   483,
	  -591,  -124,  -600,  -116,  -605,  -857,  -512,   486,   622,  -177,
	  -250,  -272,  -143,  -457,  -616,  -109,  -186,  -292,  -566,  -568,
	  -206,  -570,  -129,  -423,  -240,  -239,  -237,  -571,  -572,  -301,
	   589,   606,   485,   463,  -631,  -212,   469,  -231,  -236,  -237,
	  -239,  -240,  -143,  -634,   465,  -638,  -129,  -317,  -640,  -254,
	  -642,  -644,  -112,  -113,  -646,  -647,  -171,  -177,  -666,   382,
	   384,   383,  -356,  -242,   458,  -401,  -146,  -152,  -146,  -401,
	   469,  -387,  -388,  -390,  -346, -1284,   294,  -528,   294,  -707,
	  -710,   502,  -717,  -745,   468,  -721,  -726,  -731,   348,  -734,
	  -754,  -756,   502,   485,  -778,   485,   499,  -732,  -783,   468,
	  -792,  -793,  -756,  -790,  -732,   485,  -798,  -799,  -512,  -800,
	  -801,  -806,  -807,  -794,  -795,   485,   348,  -812,   389,   485,
	   396,   386,   401,   606,   485,  -367,  -356,  -836,   502,   281,
	   501,  -912,   338,   456,  -914,  -457,   486,   485,  -709,  -917,
	  -927,  -928,  -929,  -925,  -903, -1071, -1072, -1073,  -756, -1078,
	   294,  -958,   485,  -960,   501,  -964,  -969,   624,  -970,  -971,
	  -756,  -922,  -977,  -978,   468,  -993,   485,   502, -1069, -1070,
	  -709,   439,  -709,  -709, -1004, -1006, -1007,  -972,  -973,   485,
	   486,   430,   419,   472,   480,   474,   334,   350,   313,   613,
	   483,   483, -1013, -1015, -1016,   473,  -937,   485,  -709,  -909,
	   503, -1025, -1027,   459,  -952,   503, -1033,  -747, -1034,  -457,
	 -1035,  -709, -1045, -1049,   468, -1045,   503, -1045, -1045, -1056,
	  -732,   299, -1058, -1060,   479,  -903,  -384,    -9,  -909,   503,
	  -709, -1096,   460,   503, -1098, -1099,   485,   486, -1101, -1100,
	  -457,   272, -1102, -1075, -1076, -1077, -1074,  -809,  -989,  -990,
	  -795,   624,   623,   484,   488, -1285,   460, -1111, -1112, -1073,
	 -1078,   502, -1116, -1118,  -457,  -809,  -989,  -990,  -795, -1121,
	 -1122, -1073,   502, -1128,   502, -1131, -1132, -1133,   485,   486,
	 -1135, -1136, -1123, -1133, -1140, -1141,  -709,   502, -1144, -1149,
	  -800,   484,   488,  -457, -1128,  -124,  -145,  -129,  -147,   317,
	  -280,   347,  -515,   504,  -410,  -553,  -315,  -186,  -549,  -132,
	   344,   345,  -551,  -206,  -561,  -562,  -163,  -115,  -121,  -121,
	  -121,   390,   334,  -177,  -856,  -815,   269,   317,   504,  -324,
	   458,  -526,   469,  -719,  -374,  -206,  -377,   273,  -163,  -920,
	   486,   485,   384,   338,  -457,    -9,  -967,  -967,  -709,  -709,
	  -709,  -709,  -709,  -709,  -709,  -709,    -9,    -9,   503,  -709,
	 -1109, -1118, -1109,  -556,  -709,   392, -1217, -1265,   503,   614,
	 -1169, -1170, -1178, -1179,   594,   611, -1175, -1206,   461, -1206,
	 -1257, -1192, -1193, -1257,   614,   485,   504,   502, -1205, -1185,
	 -1192,   504,    -9,  -822,   294,   501,  -344,  -674,  -675,  -676,
	   297,   505,  -344,   502,   485, -1289, -1292, -1294,   485,  -209,
	   485, -1289,  -384,    -9,  -831,  -847,   298,    -9,  -756,  -850,
	  -851,  -390,  -115,  -116,  -137,   485,  -137,  -109,   456,   485,
	  -177,  -223,  -171,   622,  -223,  -151,  -153,  -171,  -155,  -155,
	  -159,   457,  -156,  -163,  -226,  -227,   623,   624,  -156,  -163,
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
	   378,  -661, -1283,   379,  -216,  -643,  -406,  -155,  -177,  -645,
	   485,  -645,  -664,   384,   380,   381,  -357,   267,  -394,  -395,
	  -396,  -109,  -355,  -129,  -129,  -129,  -129,  -129,  -222,  -424,
	  -425,  -430,  -431,  -109,  -432,   332,   295,  -389,   501,  -344,
	  -344, -1286,  -523,  -523,  -711,  -712,  -714,  -709,   334,   474,
	  -746,  -747,  -748,  -457,  -749,  -743,  -744,   485,   486,  -722,
	  -728,   397,  -732,  -755,   468,  -758,  -761,  -762,   485,   499,
	   501,   485,   503,   501,  -344,   622,   503,   501,   503,   501,
	   622,   463,  -813,   505,   485,   486,  -813,  -813,  -368,  -412,
	   336,  -204,  -406,   503,  -673,  -838,  -839,  -825,   294,  -709,
	   460,  -912,  -918,  -944,  -928,  -930,  -932,  -933,   473,   481,
	   482,   474,  -937,   485,    -9,   503,   501,  -942,   461,   622,
	  -344,  -523,   501,  -942,  -709,   503,   501,  -972,  -344,  -979,
	  -980,  -981,  -983,   485,   486,  -942,  -734,   503,   501,  -942,
	   502,  -709,   503,   501,  -942,   502,  -931,   501,    -9,  -934,
	   486,   485,   456,  -457,  -935,  -938,   502,   483,  -457,   503,
	  -998,   503,   501,  -942,   502, -1059, -1061,   459,  -952,  -905,
	    -2,   299,   503, -1073,   503,   501,   622,   622,   622,  -344,
	   502,   484,   488,   484,   488,  -523,   503,   501,  -942,  -523,
	 -1112, -1117,   502,   503,   501,  -942, -1125, -1126,   485,   485,
	   503,   501,  -942,   503,   501,  -942, -1148,   502,   503,   501,
	  -942, -1141, -1145,   502,  -142,  -142,  -146,  -146,  -173,   334,
	  -759,   485,   499,  -173,  -177,  -537,   334,   399,   485,  -121,
	  -177,   485,  -121,  -816,   468,  -209,   485,  -375,   625,  -378,
	  -379,  -129,  -362,  -472,  -109,  -473,  -227,  -226,   485,  -474,
	  -475,  -500,  -502,  -453,  -121,  -477,   308,   333,   302,   325,
	   324,   300,  -225,  -478,  -479,  -480,  -349,  -229,  -348,  -216,
	  -163,  -143,   487,   489,   484,   488,  -481,   275,  -482,   358,
	  -488,  -350,  -503,   307,  -506,  -507,  -457,  -346,   467,   490,
	  -291,   306,  -505,   330,  -508,   321,   323,   329,  -510,  -511,
	   341,   340,  -921,   502,  -968,   486,   485,   483,  -457, -1022,
	  -732, -1022, -1022,  -920, -1022, -1022, -1022,  -557,  -177,   400,
	 -1264, -1266, -1268, -1269,   459,   479, -1171, -1174, -1180, -1194,
	 -1182, -1195,   485, -1196,   590, -1198, -1199,   592,   504, -1171,
	 -1180, -1232, -1173, -1260,   610,   485,  -758, -1206, -1173, -1281,
	   485,   499,   486,  -823,  -824,   281,  -386,  -828,  -674,  -677,
	 -1285,   485, -1293, -1296,   504, -1295,   502,  -385,   299,  -384,
	    -9,  -831,   503,  -852,   501,  -344,  -109,  -109,  -104,  -679,
	  -682,  -188,  -109,  -179,   456,   411,   334,   389,   360,   337,
	   275,  -109,  -109,  -156,  -156,  -156,  -163,  -163,  -146,  -146,
	  -128,  -310,   468,  -287,  -171,  -291,   321,  -294,  -129,  -109,
	  -300,  -301,  -193,  -442,  -178,  -109,  -445,  -446,  -447,  -448,
	  -449,   276,  -362,  -452,  -466,   313,  -129,   360,  -109,   360,
	  -130,   345,   344,  -132,  -110,  -177,  -223,  -173,  -176,  -109,
	  -178,  -183,  -109,  -193,  -110,  -177,  -196,  -261,  -235,   343,
	  -266,  -267,   485,  -282,  -421,  -334,  -109,  -422,  -423,  -116,
	   349,  -110,  -177,  -282,  -321,   390,  -323,  -209,  -209,   485,
	  -523,   294,  -270,  -129,  -328,  -593,  -407,  -104,   362,  -410,
	   485,  -110,  -238,  -238,   339,  -331,  -332,  -334,   339,  -601,
	  -129,   263,  -146,   501,  -618,  -242,   485,  -110,  -177,  -142,
	  -625,  -109,  -242,  -573,   390,  -410,   485,   399,   334,  -238,
	  -570,  -238,  -238,  -530,  -214,  -109,  -121,   399,  -121,  -236,
	  -238,  -143,  -163,  -121,  -651,  -655,  -656,  -171,  -163,  -112,
	  -113,  -129,   485,  -171,  -651,  -658,  -659,  -171,  -658, -1286,
	  -648,  -649,  -171,  -648,  -644,  -109,  -109,  -358,  -361,  -208,
	  -177,  -397,   326,  -282,  -395,  -109,  -401,  -401,  -426,  -412,
	  -165,  -367,  -282,  -433,  -376,   332,  -388,   503,  -713,   501,
	   622,  -750,   501,   622,  -723,  -729,  -736,   407,   457,   483,
	  -457,   503,   501,  -763,   485,   485,   499,   622,  -793,   622,
	  -799,  -807,  -808,  -809,  -795,  -457,   485,  -813,   485,  -283,
	  -165,  -282,   456,   273,  -392,  -407,  -193,  -222,  -457,  -837,
	   622,   503,   501,  -523,  -523,   460,  -945,  -946,   418,  -931,
	   501,   472,   480,  -934,  -934,  -934,  -936,   485,   486,  -926,
	  -903, -1072,  -709,   422,  -958,  -709,  -970,  -942,   501,  -982,
	   502,   622,  -709,   501, -1070,  -845,  -846,  -512, -1075, -1076,
	 -1077, -1074,   275,  -709, -1007, -1008,   486,   483,   484,   488,
	   338,   384,   485,  -457,   462,   503,  -974,   486,   485,   483,
	   481,   482,   426,   473,   440,   480,   472,  -457,  -903, -1016,
	  -935,  -935,  -939,  -940,  -941,   485,   486,   430,   419, -1035,
	 -1036, -1004,  -998,    -9, -1099, -1100,  -457, -1100, -1100, -1073,
	 -1112,  -709,   503, -1119, -1147, -1122, -1123,   503,   501,  -942,
	  -942, -1132, -1102, -1136, -1102, -1104, -1141, -1102,   503,   485,
	  -143,  -143,  -129,  -129,  -315,  -538,  -563,  -206,  -163,  -558,
	  -559,  -560,  -562,  -177,  -223,  -146,  -380,  -471,   301,  -340,
	  -342,  -226,  -227,  -341,   625,   626,   627,  -362,  -362,  -362,
	  -340,  -501,   273,   456,  -109,  -501,   502,   502,  -489,  -491,
	  -362,  -497,  -498,  -235,  -110,  -362,  -504,   485,  -143,  -109,
	  -109,  -344,  -344,   502,   486,  -109,  -276,  -922,  -410,   614,
	 -1180, -1269, -1172, -1198, -1172, -1181, -1270, -1183, -1184, -1185,
	   609, -1186, -1189,   607,   596, -1192, -1173, -1173, -1252, -1255,
	 -1156, -1261, -1182,   503, -1207, -1247, -1208, -1209, -1210, -1194,
	 -1211, -1212, -1213, -1249, -1214, -1220, -1184, -1221, -1225, -1245,
	 -1246,   599, -1193, -1186, -1222, -1227,   589,   600, -1223,   608,
	   595,    -9,  -825,  -387,  -391,   502, -1295,   485,   456,    -9,
	  -848,   299,  -851,  -138,  -684,  -109,  -115,   456,   360,  -222,
	  -138,  -110,  -672,  -530,  -143,  -129,  -147,  -457,   485,   499,
	  -138,  -689,  -690,  -691,  -171,  -109,  -163,  -129,  -129,  -770,
	  -288,  -293,   268,  -289,  -171,  -290,  -109,  -764,   586,   587,
	   588,  -765,  -766,  -767,  -768,  -769,  -310,   468,   276,  -773,
	  -703,  -775,  -776,  -131,   358,  -772,  -298,  -300,  -297,  -171,
	  -238,  -121,  -443,  -444,   616,   615,  -193,  -193,  -454,  -362,
	  -469,  -423,  -450,  -458,  -178,  -462,  -467,   617,   618,   622,
	   619,   620,   621,   277,   276,   305,  -464,  -142,   459,  -450,
	   276,  -109,  -146,  -238,  -577,   460,   458,  -580,  -581,  -171,
	  -143,  -583,  -163,  -179,  -110,  -187,  -189,  -129,  -208,  -200,
	   463,  -262,  -159,  -268,  -269,   332,   310,   319,   338,   388,
	   393,  -177,  -173,  -412,  -282,  -367,  -373,   350,  -152,  -285,
	  -286,  -126,  -127,  -283,  -322,  -193,  -222,   274,  -204,  -523,
	  -173,  -104,  -104,  -104,  -110,  -110,  -109,  -283,  -188,  -607,
	  -223,  -129,   485,  -173,  -623,  -121,  -129,  -143,  -621,  -143,
	  -617,  -626,  -129,  -270,  -410,   485,  -110,  -110,  -220,  -221,
	  -222,  -129,  -233,  -244,   485,  -110,  -163,  -651,  -109,  -657,
	  -530,  -457,  -177,  -223,  -177,  -223,  -138,  -138,  -359,  -360,
	  -177,   464,  -204,  -223,  -395,  -398,   322,   318,   314,   323,
	   329,  -110,  -110,  -282,  -129,  -129,  -427,  -369,  -415,   284,
	  -430,  -159,  -110,  -374,  -163,  -712,  -709,  -749,  -751,   486,
	   485,   483,  -457,  -724,  -730,   579,  -737,   408,   622,  -735,
	  -761,  -289,   502,   485,  -784,   483,  -512,  -794,  -344,  -369,
	  -159,  -393,  -411,   316,  -159,   348,  -840,  -841,  -843,   272,
	  -839,  -673,  -523,  -947,  -903,  -932,  -934,  -934,  -935,   502,
	   503,  -980,  -985,  -986,  -987,   485,   486,   350,  -984,   485,
	   486,   503,  -996,  -756,  -344,   503,  -344,   503,   503,   501,
	  -942, -1037, -1040,   486,  -457,   485,   503,   503,  -942, -1126,
	 -1118, -1144,   503,  -942,  -146,  -146,  -177,  -121,  -121,  -121,
	  -379,  -362,  -340,  -508,  -129,  -472,  -110,  -110,  -409,  -476,
	  -109,  -227,  -226,  -475,  -501,  -409,  -490,   299,  -492,  -493,
	  -495,  -235,  -494,  -498,  -499,   312,  -193,  -110,  -177,  -464,
	   458,  -143,  -362,  -362,  -512,  -483,  -484,   485,  -509,  -129,
	    -9, -1267, -1199, -1173, -1173, -1271, -1272, -1187, -1190, -1224,
	 -1219, -1177,   483,   624,   484,   488,   486, -1190,   624, -1197,
	   274, -1253, -1254,   605, -1156, -1173,   614, -1248,   605, -1215,
	 -1216, -1218, -1175, -1219, -1226, -1228, -1229, -1230,   256,   614,
	 -1231, -1234,   601, -1168,   593, -1171, -1171, -1190, -1191, -1183,
	 -1190, -1224,  -364,  -523,   503,    -9,  -110,  -177,  -190,  -678,
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
	  -186,  -163,   485,   622,  -146,  -146,  -618,  -110,  -177,  -190,
	  -549,  -110,  -177,  -223,  -223,  -234,  -204,  -235,  -245,   485,
	  -104,  -657,  -659,  -660,  -657,  -317,   411,   334,  -143,  -649,
	  -650,   270,  -143,  -163,  -227,  -206,   358,   485,   269,   257,
	  -457,  -109,  -110,  -110,  -361,   390,  -362,  -396,  -109,   322,
	   322,   322,   322,  -400,  -401,  -110,  -428,  -370,  -417,   395,
	  -416,  -163,  -229,  -413,  -414,  -409,  -434,   625,  -435,  -436,
	  -129,  -362,  -725,  -741,   485,   622,   622,  -738,  -719,   483,
	  -370,  -413,  -193,  -408,  -409,   504,  -842,   502,  -844,  -845,
	  -846,  -837,    -9,  -934,  -709,   503,   501,   622,   503,  -344,
	  -942,  -940,  -943,   486,   483,  -457, -1038,   502,   411,   503,
	 -1104,  -129,  -129,  -163,  -471,  -110,  -409,  -409,  -409,  -409,
	  -110,  -494,  -495,  -362,  -362,  -496,   335,  -362,  -362,  -362,
	  -242,  -152,  -344,  -485,  -109,  -146, -1173, -1273, -1274, -1276,
	   485, -1188,   597,   483,   484,   488, -1191, -1171, -1236,   299,
	 -1255, -1173, -1217, -1176,   299, -1229, -1231, -1173,   614, -1177,
	 -1169, -1188,  -684,  -110,  -177,  -143,  -129,  -129,  -143,  -129,
	  -109,  -693,  -694,  -695,  -697,  -698,  -163,  -605,  -238,  -110,
	   317,   622,  -610,  -289,  -171,  -163,  -289,  -110,  -177,  -146,
	  -171,  -238,  -305,  -306,   310,   338,  -204,  -362,  -451,  -456,
	   278,  -452,  -109,  -454,  -362,  -459,  -463,  -362,  -109,  -468,
	  -243,  -451,  -110,  -173,  -179,  -129,   483,  -176,  -163,  -188,
	  -189,  -202,   458,   485,  -121,  -269,  -375,  -283,  -286,  -370,
	  -110,  -177,  -339,  -340,  -341,  -342,  -226,  -227,  -338,  -336,
	  -337,  -338,  -338,  -344,  -344,  -110,  -338,  -603,  -406,  -155,
	  -608,  -609,  -129,  -612,  -143,  -143,  -129,  -173,  -129,  -221,
	  -224,  -225,  -226,  -227,  -228,  -121,  -229,  -129,  -224,  -193,
	  -152,  -110,   485,  -163,  -138,   274,  -399,  -186,  -404,   281,
	  -129,  -429,  -276,  -418,  -171,   475,  -177,  -190,  -177,  -146,
	  -223,  -152,  -742,  -719,   622,  -740,  -739,  -709,   503,   501,
	  -371,  -155,  -177,  -673,  -512,   503,  -986,  -988,  -809,  -795,
	  -989,  -990,  -457,   485,   390,   358,  -922,  -845, -1039,   468,
	 -1041,   276,   483,   503,   503,  -476,  -110,  -110,  -496,  -362,
	  -110,  -177,  -110,  -110,  -362,  -287,   503,  -486,  -292,  -163,
	  -129, -1275,   504,   502, -1187, -1177, -1218,   608, -1206, -1171,
	 -1190,  -190,  -177,  -225,  -690,  -177,  -696,  -155,  -696,  -696,
	  -171,  -699,  -688,  -684,  -686,  -109,   485,  -110,  -289,  -110,
	  -290,  -289,  -163,  -289,  -109,  -110,  -304,  -307,   345,   344,
	   267,   485,  -392,  -143,  -457,  -110,  -443,  -362,  -110,  -177,
	  -468,  -109,  -109,   503,   501,  -110,  -173,  -203,   462,   483,
	  -272,  -284,  -335,  -337,  -109,  -110,  -110,  -177,  -610,  -610,
	  -177,  -190,  -190,  -225,  -225,  -109,  -129,  -110,  -324,  -193,
	  -109,  -402,  -109,  -437,  -171,  -416,  -414,  -436,  -340,  -362,
	  -129,   501,  -709,  -709,  -289,   483,  -381,  -382,  -171,  -409,
	   503,  -344,   275, -1042,   468,   275,   503,   501,  -362,  -362,
	  -110,  -276,  -288,  -487,   485,  -110,  -177, -1274, -1233, -1232,
	 -1172,  -109,  -110,  -694,  -171,  -699,  -684,  -667,   339,  -687,
	  -684,   334,  -289,  -171,  -110,  -163,  -302,  -305,   275,   485,
	  -393,  -362,  -443,  -465,  -362,  -110,  -468,  -584,  -225,  -227,
	  -226,   483,  -176,  -204,   485,   505,  -338,  -338,  -609,  -110,
	  -224,  -245,  -405,  -129,  -403,  -129,  -237,  -419,   411,   350,
	  -719,   503,  -177,  -109,   503,   358,   483,  -110,  -362,  -110,
	  -109,  -163, -1173, -1235, -1237, -1238,   343, -1173,  -685,  -223,
	  -109,  -455,   617,   618,   620,   621,  -110,  -177,   485,  -289,
	  -289,  -110,  -362,  -177,  -110,  -110,  -225,  -225,   503,   463,
	   483,  -110,  -110,  -177,  -110,  -177,  -152,  -110,  -177,  -110,
	  -177,  -243,  -420,   291,  -289,  -382,  -171,   503,  -110,  -163,
	  -110, -1236, -1237, -1239, -1241, -1243, -1244, -1187,   602,  -110,
	  -686,  -700,  -702,  -703,  -701,   358,  -225,  -109,  -701,  -684,
	  -289,  -362,   485,   463,  -338,  -224,  -129,  -129,  -129,  -438,
	  -439,  -440,  -441,  -129,  -206,  -177,  -110, -1234, -1240, -1242,
	   591,   604, -1188,  -110,  -177,  -704,  -225,   483,  -110,  -110,
	  -177,  -440,  -129,  -146,  -171, -1173, -1226, -1241, -1187,  -702,
	  -110,  -177,   505,  -439,  -110, -1233,  -225,   483
};

short yydef[] = {
	     2,    -2,     1,     3,     4,     5,     6,     7,     0,     0,
	     0,    11,    12,    13,    14,    15,     0,     0,     0,    19,
	  2245,  2232,    25,    26,    27,    28,    29,    30,    31,    32,
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
	  1667,  2453,     0,   836,     0,   430,   431,   963,     0,  1004,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,   901,  1089,     0,     0,     0,     0,     0,     0,
	     0,     0,   510,   825,     0,  1413,  1464,  1524,  1535,     0,
	  1544,     0,     0,     0,     0,     0,     0,  1804,  1604,   510,
	     0,  2453,  1724,     0,     0,     0,     0,  1755,  1805,  1806,
	  1807,     0,     0,  1813,     0,  1818,     0,     0,     0,     0,
	     0,     0,  1908,  1913,     0,  1919,  1922,     0,  2453,     0,
	  1959,  1966,  1970,     0,  1984,  2055,     0,     0,     0,     0,
	     0,  2049,     0,     0,  2453,  2453,     0,     0,  2453,     0,
	  2453,  2453,     0,     0,     0,     0,  2453,  2453,   107,   109,
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
	     0,  2038,  2065,     0,  2005,     0,  2146,  2148,  2453,  2157,
	  2159,  2163,  2168,  2173,  2453,  2181,  2453,  2193,  2195,  2198,
	   152,   855,   860,     0,   868,   866,   889,   890,   891,   892,
	   893,   894,   869,   895,  1009,  1014,  1010,  1421,   483,  1744,
	  1810,  1817,  1830,  1918,  1930,  1948,  1963,  1969,  1978,  1981,
	  2009,  2013,  2020,  2023,     0,  2046,  2130,  2131,  2151,  2187,
	  1011,  1012,  1013,  2043,     8,  2230,     9,    10,    16,    17,
	     0,    21,    22,    23,    24,     0,  2246,  2247,  2248,     0,
	  2250,  2251,     0,  2428,  2253,  2254,  2255,  2256,  2257,  2258,
	  2259,  2260,  2261,  2262,     0,  2291,  2292,  2293,     0,     0,
	     0,     0,     0,     0,  2397,     0,  2265,     0,     0,  2448,
	  2435,  2439,  2424,  2384,  2398,     0,  2267,  2449,  2429,   105,
	  1602,     0,  2453,  1613,  2453,  2463,    -2,     0,  1308,  1278,
	  1279,   239,  2466,  1276,  1277,     0,  2451,   506,     0,     0,
	     0,  1674,  1675,  1676,  1677,  1678,  1680,  1681,  1682,  1684,
	  1685,  1686,  1687,  1688,  1689,  1690,  1691,  1692,  1693,  1694,
	  1695,  1696,  1697,  1698,  1699,  1700,  1702,  1703,  1704,  1705,
	  1706,  1707,  1708,  1709,  1710,  1713,  1714,     0,     0,  1259,
	     0,  1289,  1287,  1288,  1271,  1286,  1274,  1275,     0,     0,
	   117,   119,   121,   122,   123,   124,     0,  1267,   163,   569,
	     0,  1183,     0,   934,   935,   401,     0,   402,     0,   139,
	     0,   832,   834,     0,     0,     0,   191,  1323,   193,   196,
	   197,     0,     0,   336,   401,  1230,     0,  1244,     0,   223,
	     0,   212,   214,   215,   216,   217,   218,  1197,   289,     0,
	   293,     0,   301,   163,     0,  1323,  1323,   223,   223,   401,
	     0,   413,     0,     0,  1666,  1322,  1323,  1672,  1673,  2466,
	  2452,   419,   423,   424,   425,   426,   427,   428,   429,  1174,
	   421,   995,   966,   968,   969,   971,   972,   974,   975,   976,
	    -2,   979,    -2,  2240,  2241,   986,   987,   988,   989,   990,
	   633,   388,     0,  1228,  1020,  1658,  2453,     0,  1041,  1272,
	   285,  1045,  1054,  1077,     0,     0,  1081,     0,     0,  1090,
	  1091,   281,   240,   241,     0,     0,   260,   279,  1096,   929,
	  1292,  1293,  1294,  1101,     0,  1111,  1282,  1223,  1184,  1185,
	  1187,  1188,     0,  1137,  1138,  1195,   468,   471,   472,   522,
	   401,     0,    -2,   797,   820,   821,   822,   798,     0,   806,
	   807,   814,   815,   816,  2453,     0,   512,   818,  2453,   828,
	     0,  1396,  1397,  2083,  2084,  2085,  2086,  2087,  2081,  2080,
	  1445,  1411,  1414,     0,  1425,  2453,  1463,     0,     0,     0,
	  1529,  2453,  1532,  1533,  1534,  1538,  2453,  1542,  2453,  1546,
	     0,  1559,  2453,  1567,  1540,     0,  1569,  1570,  1571,  1572,
	  1573,     0,  1586,     0,  1627,   522,  1625,  1629,  1628,  1720,
	  1722,  1723,  1725,  1728,  1726,  1727,  2051,  1738,  1732,     0,
	  1760,  1753,  2055,  1756,     0,  1811,  1814,  1815,  2453,  2111,
	  1819,     0,  2079,  1825,  1873,  2453,  1829,  1878,  1905,     0,
	     0,  1912,  1915,  1916,     0,  1920,  1923,     0,     0,  1926,
	  1927,     0,  1944,  2101,  2102,  2103,  2104,  2091,  2088,     0,
	     0,  1946,  1949,     0,  1952,  1764,  1957,     0,  1961,     0,
	  1972,     0,  1975,  1976,  1979,     0,  2014,  2014,  2053,     0,
	  2014,  2014,  2017,  2018,  2021,  2453,  2031,  2026,  2029,  2030,
	     0,  2039,  1717,     0,  2044,     0,     0,  2143,     0,  2145,
	  2200,  2201,  2202,  2204,  2466,     0,  2149,  2152,  2453,  2111,
	     0,     0,  2453,     0,     0,  2174,     0,     0,  2185,  2188,
	     0,     0,     0,     0,   145,     0,     0,   153,   154,   933,
	  1458,   183,     0,   195,   205,   211,   287,   291,   335,   397,
	   223,     0,   800,  1466,     0,    -2,    -2,  1472,   411,   434,
	   470,   484,  1733,  1671,     0,     0,   857,   858,   859,   870,
	   871,   839,   844,   843,   845,   846,   847,   848,   849,   926,
	     0,     0,     0,     0,  1244,   330,   852,   854,   565,  1007,
	  1008,  1017,  1018,  1661,     0,  1049,     0,  1072,  1051,  1052,
	  1053,  1082,  1085,  1086,  1087,  1088,    -2,     0,     0,     0,
	   823,  1408,     0,  1440,   488,     0,   487,  1750,     0,  1809,
	  1816,  2074,  1837,  1831,  1832,  1833,  1834,  1835,  1836,  1917,
	  1837,  1947,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  2063,  2453,     0,  2129,  2208,  2175,  2208,   863,
	   867,   861,     0,    18,  2231,  2242,     0,  2249,     0,  2364,
	     0,  2252,  2268,     0,  2417,  2418,     0,     0,  2358,  2358,
	  2360,  2361,  2446,  2383,  2445,     0,  2399,  2400,     0,  2294,
	  2296,  2404,     0,    -2,    -2,    -2,  1603,  1609,  1607,  1608,
	  1616,  1614,  1618,  1309,  1309,     0,  1615,  2459,     0,  2462,
	     0,  2457,  2458,  2467,     0,  1182,   480,  1717,     0,  1647,
	     0,  2453,  2453,  1653,  2243,   830,   106,     0,   140,   142,
	  1208,     0,     0,  1196,  1213,   108,     0,  1213,     0,   113,
	   114,   115,   120,  1324,  1325,  1327,  1329,  1331,  1332,  1333,
	  1334,  1335,  1336,  1337,  1338,  1339,  1340,  1341,  1305,     0,
	  1248,   125,   162,   164,     0,     0,     0,     0,     0,     0,
	     0,   173,   174,   175,   176,   177,   178,   179,  1199,  1256,
	  1251,  1226,  1231,  1233,  1206,  1210,  1247,  1237,  1238,  1235,
	  1236,  1239,     0,  1212,     0,   127,     0,     0,     0,   136,
	   143,   937,   158,     0,  1229,   182,     0,     0,     0,     0,
	  1209,     0,   556,   206,  1323,     0,     0,     0,   219,   220,
	   221,   290,   295,   297,   298,   299,     0,   292,   294,   305,
	   160,     0,  1221,   392,   395,     0,     0,     0,     0,   404,
	    -2,     0,     0,  1598,  2463,     0,  1600,  1668,  2456,     0,
	     0,     0,   967,   983,     0,     0,     0,   985,   980,  1291,
	     0,   223,     0,  1016,     0,  1663,  1664,  1665,     0,     0,
	  1323,  1323,  1046,  1047,     0,     0,     0,     0,   921,   902,
	   903,   906,   908,   909,   910,   911,   912,   914,   915,     0,
	  2238,  2239,     0,  1093,     0,     0,     0,     0,   263,   265,
	   267,   268,     0,     0,  1098,  1139,  1103,     0,  1105,     0,
	  1120,  1109,     0,     0,  1114,  1115,  1116,     0,  1189,     0,
	     0,  1193,     0,     0,  1222,     0,     0,   554,     0,     0,
	     0,   511,   513,  1309,  1309,  2466,  2463,   826,  2463,  1394,
	     0,  1399,  1407,     0,  1446,  1415,  1412,  2453,  1423,  1424,
	  1462,  1459,     0,  1522,  1523,  1525,  1527,  1531,  1536,     0,
	     0,  1551,  1309,  1545,  1549,  1548,     0,  1562,  1564,  1565,
	  1566,     0,  1577,     0,  1554,  1555,  1556,  1584,  1588,     0,
	     0,     0,  1592,     0,  1587,   575,   560,  1626,     0,  1634,
	     0,     0,  1734,  1735,  1736,  1737,  1739,  1740,  1738,  1800,
	  1759,  1761,  1775,     0,  1808,     0,  2067,     0,  1309,  2463,
	  2112,     0,     0,     0,  2078,     0,  1843,  1844,  1845,     0,
	  1309,  1875,  1877,     0,  1880,     0,  1909,     0,     0,  2057,
	     0,     0,  1924,     0,     0,  1931,  1932,     0,  1849,  1852,
	  1853,  1854,  1855,  1856,  1857,  1858,  1859,  1860,  1861,  2236,
	  2089,  2090,  1950,     0,  1953,     0,  1956,  1787,  1960,     0,
	  1965,  1967,     0,  1974,  1971,  1982,     0,  1985,  1986,    -2,
	  1987,     0,  2006,     0,  2016,  2007,  2054,  2010,  2011,  2024,
	  2027,  2028,  2034,     0,  2033,  2037,     0,  1716,     0,  2048,
	  2004,  2128,  2453,  2132,     0,  2134,     0,  2080,     0,  2139,
	    -2,     0,  2142,  2223,  2224,  2225,  2226,  2092,  2096,  2100,
	  2082,     0,     0,  2093,  2097,  2455,  2463,     0,  2154,     0,
	  2463,  2453,     0,  2160,  2210,  2211,  2212,  2213,  2214,     0,
	  2164,     0,     0,  2172,     0,     0,  2177,     0,  2228,  2229,
	     0,  2182,     0,  2206,     0,  2190,     0,     0,     0,  2207,
	  2218,  2219,  2220,  2221,  2197,     0,     0,     0,     0,   194,
	  1323,     0,   799,     0,  1323,   853,   896,     0,   850,   925,
	   927,   928,   851,   879,   880,   881,   883,   884,   326,   327,
	   328,     0,   331,     0,  1659,  1598,     0,  1071,     0,   808,
	   809,   819,     0,  1409,     0,   489,   490,   491,   486,  1752,
	  1745,  1746,  1747,  1748,  1749,  1743,  1828,  1929,    -2,    -2,
	    -2,  1750,    -2,  2012,    -2,  2022,  2040,  2041,  2064,    -2,
	  2150,  2209,  2186,   864,  2042,     0,  2363,     0,  2444,  2366,
	  2368,     0,  2269,  2270,  2419,     0,  2266,     0,  2426,  2330,
	     0,  2359,  2283,     0,  2396,  2401,     0,     0,     0,  2297,
	  2282,     0,  1605,     0,  1617,  2453,  1620,  1310,     0,  2466,
	  1314,  1313,  1621,     0,  2460,  2461,  2471,  2478,  2474,  1307,
	  2468,  2469,     0,   508,  1622,  1717,     0,  1647,  1624,     0,
	  1654,  1309,  1290,   141,     0,   148,     0,     0,   111,   112,
	     0,  1316,  1330,  1216,     0,   165,   166,   180,     0,     0,
	     0,  1205,   170,   791,     0,     0,  1249,  1232,   171,   172,
	   568,   571,   129,   390,     0,   362,     0,   364,   365,   366,
	     0,     0,     0,     0,   387,   384,     0,     0,   389,     0,
	   133,     0,   400,     0,   936,   938,     0,  1323,   189,     0,
	     0,   198,  1252,   337,     0,   557,   208,     0,   224,   572,
	   223,   213,   296,   312,   304,     0,   307,   332,     0,     0,
	     0,     0,   342,   572,     0,   405,   407,     0,     0,  1593,
	     0,  1595,  2463,     0,     0,   991,     0,     0,   996,   997,
	    -2,   992,   970,     0,     0,     0,   432,     0,     0,     0,
	  1021,  1022,     0,  1039,  1040,  1273,   286,  1043,  1063,     0,
	     0,  1056,  1058,  1060,  1061,  1073,  1079,     0,     0,     0,
	     0,   904,   905,     0,     0,   900,     0,  1092,   244,     0,
	     0,   282,   283,   284,     0,     0,   269,   270,   271,   272,
	  1242,     0,     0,   280,  1095,     0,  1100,  1102,  1140,  1159,
	  1143,  1144,  1145,  1280,  1147,  1148,  1159,  1281,     0,     0,
	  1263,  1104,  1107,  1155,  1156,  1157,  1158,  1175,  1176,  1177,
	  1178,  1179,  2466,  1180,  1181,  1108,     0,     0,     0,     0,
	  1117,     0,  1186,  1190,  1191,  1192,     0,  1254,   521,    -2,
	   525,     0,   534,   547,    -2,    -2,   553,   803,   804,   805,
	   617,   600,   522,     0,   607,   605,     0,  2453,   515,   516,
	   517,  2464,   817,   827,     0,  1400,     0,  1403,  1404,  1405,
	  1444,  1447,  1448,  2050,  1449,     0,  1441,  1442,  1443,  1417,
	  1416,     0,  1422,  1457,     0,     0,  1473,     0,  1478,  1479,
	     0,     0,  1550,  2453,     0,  1547,  1561,  2453,  1576,     0,
	     0,     0,  1590,     0,  2234,  2235,  1591,  2237,   577,     0,
	  1264,   562,     0,  1630,     0,     0,  1636,  2463,  1635,  2052,
	  2463,     0,  1741,  1802,  1762,     0,  1766,     0,     0,     0,
	     0,     0,  1774,    -2,  1758,  2066,  2453,     0,  2076,  2077,
	  2071,  2110,     0,     0,  1823,  1827,  2453,  1847,     0,  1879,
	  1881,  1890,     0,  1885,  1886,     0,     0,  2056,     0,  2453,
	     0,  1925,  1928,     0,     0,     0,     0,     0,  1765,  1787,
	  1777,  1778,  1779,  1780,  1783,  1787,     0,  1785,  1786,  1964,
	  1973,  1983,     0,  1989,     0,  2025,     0,  2036,  2032,  1715,
	  1718,     0,  2047,  2144,  2133,     0,     0,     0,     0,  2222,
	  2453,  2094,  2098,  2095,  2099,  2147,  2153,  2453,     0,  2215,
	     0,  2158,  2453,  2162,  2453,     0,     0,  2169,     0,     0,
	  2176,     0,     0,  2180,     0,     0,  2227,  2453,  2189,     0,
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
	  2365,     0,  2369,  2371,  2425,  2434,  2287,  2287,  2274,  2284,
	  2272,  2285,  2273,     0,  2413,     0,  2289,  2415,  2447,     0,
	  2274,  2352,  2356,     0,  2443,  2402,     0,  2346,  2344,  2405,
	  2406,  2407,  2408,  1606,     0,  1634,  2453,  1619,  1311,  1312,
	  2454,     0,  2470,  2478,     0,  2472,  2477,   507,     0,     0,
	  1649,  1623,  1652,  2453,  1656,  1657,     0,     0,     0,  1326,
	  1328,     0,     0,  1344,  1345,  1346,  1347,  1348,  1349,  1315,
	  1241,     0,     0,   167,   168,   169,   792,   793,     0,     0,
	   130,   391,  1491,   360,  1482,     0,  1227,   363,  1517,     0,
	   368,     0,   383,   634,     0,     0,   639,   640,   641,   642,
	   643,  1240,     0,     0,     0,  1218,   569,   385,     0,   386,
	   131,   134,   135,   132,     0,     0,     0,   184,   186,     0,
	     0,     0,     0,   204,   222,     0,     0,   322,     0,  1262,
	   306,   308,   310,  1323,   593,   595,     0,   522,   485,   398,
	     0,   339,     0,   577,     0,     0,   560,   415,   416,  1599,
	  1596,  2463,  1323,  1287,     0,  1001,     0,  1003,  1250,   993,
	   994,   973,     0,     0,   435,   436,     0,   577,   439,  1316,
	  1026,  1025,     0,     0,  1323,     0,  1062,  1055,     0,     0,
	     0,     0,     0,     0,     0,   920,    -2,   923,   924,   919,
	   907,     0,     0,  1094,   235,     0,   242,   243,   273,   264,
	     0,   278,     0,  1159,  1142,  1160,     0,  1151,  1154,  1171,
	  1172,  1173,    -2,  1149,  1150,  1161,  1163,     0,  1162,  2465,
	  1118,  1121,  1124,  1119,  1110,     0,     0,   560,   477,     0,
	     0,     0,  1318,     0,  1317,     0,     0,     0,   583,     0,
	     0,   602,     0,   488,     0,   606,   514,  1398,     0,  1406,
	     0,     0,  1451,     0,  1419,  1418,     0,     0,  1426,  1460,
	  1461,  1465,     0,  1482,  1480,  1526,  1528,  2453,  1552,     0,
	  1563,  1578,  1579,  1580,  1581,  1309,  1583,  1589,  2233,   583,
	     0,   576,  1265,  1266,   574,     0,   558,   559,    -2,  1643,
	  1639,  1632,     0,  1633,  1729,  2463,  1799,  2065,  1803,     0,
	  1775,     0,     0,  1787,  1770,  1771,  1773,  1781,  1782,  1754,
	  1757,  2068,  2069,     0,     0,  1822,  1846,  1848,     0,  1883,
	     0,     0,     0,  2453,  2058,  2059,  2061,  1309,  2106,  2107,
	  2108,  2109,  2062,     0,  1933,  1934,  1935,  1936,  1937,  1938,
	  1939,  1940,  1941,  1309,  1943,  1850,     0,  1862,  1863,  1864,
	  1865,  1866,  1867,  1868,  1869,  1870,  1871,  1872,  1951,  1954,
	  1955,  1784,     0,  1789,     0,  1792,  1793,  1794,  1795,  1988,
	     0,     0,  2035,  1719,  2135,  2136,  1309,  2137,  2138,     0,
	  2155,  2216,  2156,     0,  2203,  2165,  2166,  2167,     0,     0,
	     0,  2178,  2179,  2183,  2184,     0,  2191,  2217,  2192,     0,
	   150,   151,   155,     0,   897,   838,   885,   886,   887,     0,
	     0,     0,   882,     0,     0,     0,     0,     0,  1201,   693,
	   694,   695,   696,   697,  1255,  1253,  1219,     0,   700,   701,
	     0,     0,   763,   764,   765,     0,   757,   758,     0,   741,
	     0,   751,   748,     0,   767,     0,     0,    -2,    -2,     0,
	     0,   794,   795,  2453,     0,   778,     0,     0,   865,  2244,
	  2372,  2370,     0,  2288,     0,  2271,  2376,  2275,  2276,  2277,
	  2442,     0,     0,  2440,  2421,     0,  2290,  2348,  2350,  2353,
	  2354,     0,  2362,     0,  2295,     0,  2298,  2299,  2300,  2301,
	  2302,  2303,  2304,  2347,     0,  2312,  2314,  2315,     0,     0,
	     0,  2427,  2306,     0,  2280,  2320,  2412,  2430,     0,  2441,
	  2420,   506,  2463,  1612,     0,   519,  2473,  2475,  2476,   509,
	  1648,     0,  1655,     0,  1319,     0,  1353,  1354,  1355,  1356,
	     0,   110,  1269,  1297,  1298,     0,     0,  1304,    -2,    -2,
	     0,     0,  1365,  1366,  1370,     0,     0,   567,   570,     0,
	   348,     0,  1268,   349,  1482,     0,     0,  1483,  1283,  1284,
	  1285,  1484,  1486,  1487,  1488,  1489,  1490,  1491,     0,  1499,
	     0,  1502,  1515,     0,  1504,  1518,  1519,  1520,  1521,     0,
	     0,   372,     0,     0,  1198,  1246,   637,     0,     0,     0,
	   522,   688,     0,   666,     0,     0,   678,   649,   650,   651,
	   652,   653,   654,   655,  1240,  1202,   673,     0,  1225,     0,
	     0,     0,     0,     0,     0,   958,   959,   939,   952,   941,
	   942,   943,     0,   190,   199,     0,   201,  1319,   225,   230,
	     0,   324,     0,   311,   313,   315,   316,   317,   318,   319,
	   320,     0,   394,     0,     0,   597,   488,   403,     0,   343,
	   344,   346,   390,   583,   406,   417,   418,     0,   408,  1597,
	   964,   999,  1000,  1002,   978,   981,     0,   583,  1030,  1024,
	     0,  1023,     0,  1048,  1064,  1065,   569,  1067,  1057,  1059,
	  1063,     0,  1319,   926,   917,   918,   913,   916,     0,   246,
	     0,     0,   560,     0,   276,   266,  1099,  1141,     0,  1152,
	  1295,  1296,     0,     0,     0,  1125,     0,     0,   473,   474,
	     0,  1263,   475,     0,    -2,     0,   536,     0,     0,     0,
	     0,    -2,   535,     0,   549,   551,   585,   619,     0,  1243,
	   601,     0,   603,     0,   608,  1401,  1402,  1450,  1452,  1453,
	  1454,  1455,  1456,  1436,  1420,     0,  1428,     0,  1429,     0,
	  1474,  1475,     0,  1481,  1539,  1557,  1558,  1553,  1582,   585,
	     0,   520,     0,  1224,     0,     0,  1638,     0,  2453,  1642,
	  1637,     0,  1730,     0,  1763,  1767,  1768,     0,  1769,     0,
	  1821,  1882,     0,  1891,     0,  1894,  1895,  1896,  1884,  1887,
	  1888,  1907,     0,  1309,  2105,     0,  1942,  1851,  1788,     0,
	     0,  1995,  1991,  1992,  1993,  1994,  2015,  2141,     0,  2170,
	  2171,     0,  2205,  2453,     0,     0,     0,   873,   875,   877,
	   495,   496,   499,   788,   498,   691,   698,   753,     0,   705,
	     0,     0,     0,   709,     0,     0,   740,  1215,   742,   751,
	   744,     0,   747,   749,     0,  1214,     0,   768,     0,     0,
	     0,   775,     0,     0,  1309,   730,   733,   732,   785,   787,
	  1742,  2367,  2373,  2263,  2264,     0,     0,     0,  2385,  2386,
	  2387,  2388,  2390,     0,  2392,  2394,  2389,  2280,     0,     0,
	  2432,     0,     0,  2438,  2355,  2357,  2403,     0,  2437,  2305,
	     0,  2308,  2310,  2311,     0,  2321,  2323,     0,     0,  2326,
	  2327,     0,  2431,     0,  2416,  2342,  2343,     0,  2316,  2281,
	  2317,  2318,  1610,  1611,   518,  1650,   146,     0,  1350,  1320,
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
	   500,   578,   573,   561,   563,     0,  1640,  2453,  1641,  1645,
	  1646,  1643,  1801,  1772,     0,  1889,     0,     0,  1878,  1914,
	  2453,  1790,  1791,  1796,  1797,  1798,  1998,     0,     0,  2199,
	     0,   156,   157,   888,     0,   754,     0,   707,   708,     0,
	   756,   743,   745,     0,   752,     0,  1258,     0,     0,     0,
	     0,     0,     0,   736,     0,     0,  2374,  2375,  2377,  2379,
	  2382,     0,  2422,  2391,  2393,  2395,  2279,  2286,     0,  2423,
	  2351,  2345,  2307,     0,     0,  2322,  2328,  2324,  2325,     0,
	     0,     0,  1319,     0,     0,  1302,  1303,  1299,  1301,  1300,
	     0,  1369,  1372,  1392,  1392,  1392,     0,    -2,  1371,  1367,
	  1493,     0,     0,   351,  1482,     0,   353,  1482,     0,     0,
	  1482,     0,   373,     0,   377,   378,   562,   687,   644,     0,
	  1217,   645,     0,     0,     0,     0,     0,     0,     0,   681,
	     0,   648,   677,   932,   954,   949,     0,   187,     0,  1323,
	   202,   234,     0,   229,  1045,   314,   598,   583,   345,   582,
	   437,     0,     0,   451,   452,   453,   454,   455,     0,   444,
	   445,   449,   450,   456,   458,   464,     0,  1015,     0,     0,
	  1029,  1031,  1319,  1037,  1066,  1068,  1069,  1070,  1319,   247,
	   248,   250,     0,     0,   253,   254,   255,   569,   249,   262,
	     0,  1146,  1126,  1129,     0,     0,   526,     0,     0,  1260,
	   530,   599,   631,     0,   587,   588,     0,   581,     0,     0,
	     0,     0,  1435,  1438,  1437,     0,     0,  1430,  1482,     0,
	   481,     0,     0,     0,  1309,  2070,  1892,  1893,  1897,  1898,
	  1899,  1900,  1901,  1902,  1903,  1904,  1910,  2060,  1990,     0,
	  2003,     0,     0,  2161,  2196,   704,   706,   755,     0,   750,
	   769,     0,   773,   774,     0,   360,   796,   731,     0,     0,
	   786,     0,  2381,  2380,  2278,  2331,  2309,  2319,  2330,  2287,
	  2313,  1351,     0,  1358,     0,     0,  1374,     0,  1375,  1376,
	  1378,  1381,     0,    -2,  1380,     0,     0,  1496,   352,  1482,
	     0,   358,     0,   355,     0,   372,     0,   376,   379,   380,
	     0,     0,   574,   661,   662,   646,     0,     0,   669,     0,
	     0,     0,     0,   950,     0,   189,   200,   560,     0,     0,
	   323,   341,   441,   446,     0,   448,   465,     0,  1034,  1035,
	     0,  1033,  1076,   251,   252,     0,   274,  1136,   476,   541,
	     0,   529,     0,     0,   632,     0,   580,   612,   616,   613,
	   615,     0,  1434,  1432,  1476,     0,   501,   502,   504,   564,
	  1631,     0,  1999,  2000,     0,  2001,  1996,     0,   746,     0,
	   771,     0,     0,   738,   739,   734,     0,  2378,     0,     0,
	     0,     0,  1368,  1373,     0,  1377,  1379,     0,  1261,     0,
	  1360,     0,   357,  1482,  1482,     0,   369,   374,   381,   382,
	   686,   664,     0,   674,   675,   680,     0,     0,   955,     0,
	     0,     0,   188,   210,     0,     0,     0,     0,  1032,   256,
	     0,     0,     0,   544,     0,   532,   623,     0,   589,   590,
	  1439,  1482,     0,     0,  1644,  2002,     0,   770,     0,   766,
	     0,     0,  2349,     0,  2333,     0,  2450,  2329,     0,     0,
	     0,     0,   657,   658,   659,   660,  1359,     0,  1494,   354,
	   359,  1482,   665,     0,   682,   953,   956,   957,   951,     0,
	     0,   447,   466,     0,   257,     0,     0,   542,     0,   531,
	     0,     0,   586,  1245,  1477,   503,     0,  1997,   772,     0,
	   735,     0,  2334,     0,  2336,  2338,  2339,  2340,  2433,  1352,
	  1393,     0,  1384,  1386,  1387,   728,  1388,     0,  1383,  1361,
	   356,   676,   233,     0,     0,     0,   275,   545,   533,   622,
	   624,   626,     0,   629,   630,     0,   737,     0,     0,     0,
	  2414,  2436,     0,  1382,     0,     0,  1390,     0,   467,   258,
	     0,   627,   629,   628,     0,  2331,  2335,  2337,  2341,  1385,
	  1389,     0,     0,   625,   505,  2332,  1391,   231
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
#line 1153 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* 
		    ** Note that FRS blocks fall through to this rule too, so
		    ** if you add actions here, do it at block_stmt too.
		    */
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 3:
#line 1164 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_func = 0;	/* No current function */
		}
		break;
	case 8:
#line 1177 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1219 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1235 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1264 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1273 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1290 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1304 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1327 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1338 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1348 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1397 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1410 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1420 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
                    /* Accept either EXEC SQL | 4GL INCLUDE */

                    if ((dml->dm_exec != (DML_EXEC|DML__SQL)) &&
                        (dml->dm_exec != (DML_EXEC|DML__4GL)))
                        er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"),
                                                            ERx("INCLUDE") );
		}
		break;
	case 21:
#line 1430 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1457 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Eat the name till right angle bracket */
		    sc_eat(id_add, SC_STRIP, ERx(">"), '<', '>');
		    yyval.s = str_add(STRNULL, id_getname());
		    id_free();
		    gr->gr_adjective = TRUE;
		}
		break;
	case 23:
#line 1466 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 24:
#line 1470 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 92:
#line 1566 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 93:
#line 1570 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 94:
#line 1574 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 95:
#line 1578 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 96:
#line 1582 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 97:
#line 1586 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 98:
#line 1590 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 99:
#line 1594 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 100:
#line 1598 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 101:
#line 1602 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 102:
#line 1606 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 103:
#line 1610 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 104:
#line 1614 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 106:
#line 1632 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1642 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 108:
#line 1654 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1664 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 111:
#line 1670 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 112:
#line 1676 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1694 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1704 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 117:
#line 1716 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1726 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 122:
#line 1735 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{ db_key(yypvt[-0].s);}
		break;
	case 123:
#line 1737 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{ db_key(yypvt[-0].s);}
		break;
	case 124:
#line 1739 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{ db_key(yypvt[-0].s);}
		break;
	case 125:
#line 1746 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
				    EQ_WARN, 1, ERx("ALTER SEQUENCE"));
		}
		break;
	case 126:
#line 1753 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 127:
#line 1766 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1776 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 134:
#line 1787 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( ERx("cascade") );
		}
		break;
	case 135:
#line 1791 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( ERx("restrict") );
		}
		break;
	case 136:
#line 1806 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1816 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 143:
#line 1837 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1847 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
	    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
	}
		break;
	case 145:
#line 1852 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
	    if (!ESQ_EQSTR(yypvt[-0].s, ERx("profile")))
		er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
				    ERx("ALTER DEFAULT"), ERx("PROFILE") );
	    gr_mechanism( GR_EQSTMT, GR_sSQL, "alter default profile ", NULL );
	}
		break;
	case 148:
#line 1865 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("privileges")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					    ERx("ALTER USER/PROFILE"), ERx("PRIVILEGES") );
		    db_key( ERx("privileges") );
		}
		break;
	case 149:
#line 1879 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1892 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
			gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 153:
#line 1897 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 154:
#line 1902 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 158:
#line 1917 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1927 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 160:
#line 1937 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
				    EQ_WARN, 1, ERx("CREATE SEQUENCE"));
		}
		break;
	case 161:
#line 1944 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 182:
#line 1978 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 1988 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		}
		break;
	case 185:
#line 2001 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2027 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2057 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("CREATE INDEX"));
		}
		break;
	case 194:
#line 2064 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    STprintf( esq->sbuf, ERx("%s %s"), yypvt[-2].s, yypvt[-1].s );
		    gr_mechanism( GR_EQSTMT, GR_sSQL, esq->sbuf, yypvt[-0].s );
		}
		break;
	case 195:
#line 2069 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		}
		break;
	case 205:
#line 2096 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2116 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2135 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2157 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2168 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 213:
#line 2172 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 219:
#line 2183 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 220:
#line 2187 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 221:
#line 2191 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 226:
#line 2204 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 227:
#line 2209 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 228:
#line 2214 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-1].s );
		    db_sconst( yypvt[-0].s );
		}
		break;
	case 229:
#line 2220 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 231:
#line 2228 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2241 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-3].s );
		    db_key( yypvt[-2].s );
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 236:
#line 2265 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2278 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2300 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 260:
#line 2329 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2344 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = CSR_STATIC;
		}
		break;
	case 276:
#line 2367 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2377 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2395 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 280:
#line 2400 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 282:
#line 2406 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 285:
#line 2431 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2444 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2454 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2473 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
 		    if (eq->eq_flags & EQ_COMMON)
 			er_write( E_EQ0504_OPEN_WARN,
 				EQ_WARN, 1, ERx("CREATE SCHEMA") );
		    esq->flag |= ESQ_CRE_SCHEMA;
		}
		break;
	case 293:
#line 2484 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		    if (gr->gr_func == tSET)
		    	erec_setup( 1 );
		}
		break;
	case 303:
#line 2516 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2533 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 310:
#line 2541 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2562 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 316:
#line 2566 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 317:
#line 2570 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 318:
#line 2574 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 319:
#line 2578 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 320:
#line 2582 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 325:
#line 2593 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{db_key( yypvt[-0].s);}
		break;
	case 326:
#line 2597 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2607 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2617 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2626 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2643 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{ 
			db_key(yypvt[-0].s);
		}
		break;
	case 331:
#line 2648 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
			db_key(yypvt[-1].s);
			db_key(yypvt[-0].s);
		}
		break;
	case 333:
#line 2660 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
				EQ_WARN, 1, ERx("CREATE SYNONYM"));
		}
		break;
	case 335:
#line 2674 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2692 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				    2, ERx("location"), ERx("CREATE TABLE") );
		}
		break;
	case 338:
#line 2708 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* With clause is non-FIPS compliant */
		    if (yypvt[-0].i == 1 && eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
					2, ERx("WITH"), ERx("CREATE TABLE") );
		}
		break;
	case 339:
#line 2716 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2775 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2, 
			        ERx("AS SELECT"), ERx("CREATE TABLE") );
		}
		break;
	case 341:
#line 2782 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2, 
			        ERx("AS SELECT"), ERx("CREATE TABLE") );
		}
		break;
	case 342:
#line 2813 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;		/* Info on dec_nullfmt */
		}
		break;
	case 343:
#line 2817 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-2].i | yypvt[-0].i;		/* Info on dec_nullfmt */
		}
		break;
	case 344:
#line 2822 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 345:
#line 2826 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-2].i | yypvt[-0].i;
		}
		break;
	case 346:
#line 2831 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 347:
#line 2835 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-1].i;
		}
		break;
	case 348:
#line 2840 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-1].i;
}
		break;
	case 349:
#line 2845 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2866 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 351:
#line 2870 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 352:
#line 2874 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 353:
#line 2879 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 354:
#line 2883 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 355:
#line 2887 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 356:
#line 2891 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 357:
#line 2896 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 358:
#line 2900 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 2915 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 362:
#line 2923 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    esq->flag |= ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 363:
#line 2927 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    esq->flag |= ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 367:
#line 2936 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 375:
#line 2953 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 377:
#line 2964 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 378:
#line 2972 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 379:
#line 2981 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 380:
#line 2989 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 381:
#line 2997 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3006 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3024 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 385:
#line 3032 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3042 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3052 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 388:
#line 3061 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 389:
#line 3070 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 393:
#line 3091 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3107 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* UNION views are non-FIPS */
		    if (eq->eq_flags & EQ_FIPS && yypvt[-1].i == 1)
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2, 
			        ERx("UNION"), ERx("CREATE VIEW") );
		}
		break;
	case 396:
#line 3123 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3134 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3160 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3174 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    STpolycat(3, yypvt[-2].s, yypvt[-1].s, yypvt[-0].s, session_temp);
		    yyval.s = session_temp;
		}
		break;
	case 401:
#line 3179 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 402:
#line 3184 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 403:
#line 3189 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-2].s );
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 404:
#line 3208 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3219 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3233 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3247 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3259 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_flag |= GR_NOSYNC;	/* no error output yet */
		    yyval.i = 0;
		}
		break;
	case 411:
#line 3264 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_flag |= GR_NOSYNC;	/* no error output yet */
		    if (eq->eq_flags & EQ_FIPS) 
			    er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1, 
			        ERx("REPEATED DELETE") );
		    yyval.i = 1;
		}
		break;
	case 412:
#line 3274 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    er_write( E_EQ0070_grMISSWORD, EQ_ERROR, 2, ERx("FROM"),
								ERx("DELETE") );
		    yyval.s = ERx("from");
		}
		break;
	case 414:
#line 3282 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 415:
#line 3288 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    STprintf( esq->sbuf, ERx("%s.%s"), yypvt[-2].s, yypvt[-0].s );
		    yyval.s = esq->sbuf;
		}
		break;
	case 416:
#line 3294 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    STprintf( esq->sbuf, ERx("%s.%s"), yypvt[-2].s, yypvt[-0].s );
		    yyval.s = esq->sbuf;
		}
		break;
	case 418:
#line 3301 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if ( gr->gr_flag & GR_REPEAT )
		    {
			db_op( ERx("...SYNTAX ERROR") );
			er_write( E_EQ0378_repWHERE, EQ_ERROR, 0 );
		    }
		}
		break;
	case 419:
#line 3315 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3325 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 421:
#line 3334 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3344 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 423:
#line 3349 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 424:
#line 3353 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 425:
#line 3357 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 426:
#line 3361 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 427:
#line 3365 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 430:
#line 3384 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3414 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3442 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 434:
#line 3446 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-1].s, yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS) 
			    er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1, 
			        ERx("REPEATED INSERT") );
		}
		break;
	case 435:
#line 3454 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 439:
#line 3464 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		    erec_setup( 1 );
		}
		break;
	case 442:
#line 3481 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (yypvt[-0].i > 1)
			er_write( E_EQ0132_sqILLSTRUCTEXPR, EQ_ERROR, 0 );
		}
		break;
	case 446:
#line 3493 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		  /* return the max num of struct elems on one side */
		    yyval.i = (yypvt[-2].i > yypvt[-0].i) ? yypvt[-2].i : yypvt[-0].i;
		}
		break;
	case 447:
#line 3498 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
 		  /* return the max num of struct elems on one side */
 		    yyval.i = (yypvt[-4].i > yypvt[-1].i) ? yypvt[-4].i : yypvt[-1].i;
 		}
		break;
	case 448:
#line 3503 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-1].i;	/* Might be a structure */
		}
		break;
	case 449:
#line 3507 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;	/* Might be a structure */
		}
		break;
	case 450:
#line 3511 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;	/* Might be a structure */
		}
		break;
	case 456:
#line 3523 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = erec_vars();
		    erec_use( IISETDOM, gr->gr_flag & GR_REPEAT, ERx(",") );
		    erec_setup( 1 );		/* end of variable */
		    if ( (gr->gr_type == T_STRUCT) && (eq->eq_flags & EQ_FIPS) )
			er_write( E_EQ050D_FIPS_VAR, EQ_WARN, 0 );
		}
		break;
	case 457:
#line 3531 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;		/* Not a structure */
		}
		break;
	case 458:
#line 3535 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    erec_use( IISETDOM, gr->gr_flag & GR_REPEAT, ERx(",") );
		    erec_setup( 1 );
		}
		break;
	case 460:
#line 3543 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (   !ESQ_EQSTR(yypvt[-0].s, ERx("dbmsinfo")) 
		        && eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0510_OPEN_FUNC, EQ_WARN, 1, yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS) 
			er_write( E_EQ0509_FIPS_FUNC, EQ_WARN, 1, yypvt[-0].s );
		}
		break;
	case 462:
#line 3552 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    er_write( E_EQ0129_sqVARCOLON, EQ_ERROR, 0 );
		    YYERROR;
		}
		break;
	case 464:
#line 3559 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-1].s;
		}
		break;
	case 465:
#line 3563 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 466:
#line 3567 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-4].s;
		}
		break;
	case 467:
#line 3572 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-6].s;
		}
		break;
	case 469:
#line 3590 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    gr->gr_func = tUPDATE;
		}
		break;
	case 470:
#line 3595 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-1].s, yypvt[-0].s );
		    gr->gr_func = tUPDATE;
		    if (eq->eq_flags & EQ_FIPS) 
			    er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1, 
			        ERx("REPEATED UPDATE") );
		}
		break;
	case 473:
#line 3609 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3646 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3661 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    esq->flag |= ESQ_CURSOR;
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 480:
#line 3729 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3773 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* don't set until after call to gr_mechanism */ 
		}
		break;
	case 483:
#line 3778 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3790 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-1].s, yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS) 
			    er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1, 
			        ERx("REPEATED SELECT") );
		}
		break;
	case 487:
#line 3801 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 490:
#line 3808 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* DISTINCT makes a cursor Read-Only */
		    if (ESQ_DCLCSR(esq))
			ecs_csrset( NULL, ECS_UNIQUE );
		}
		break;
	case 491:
#line 3815 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 492:
#line 3820 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s ); /* db_key 'cause we want spaces around it */
		}
		break;
	case 496:
#line 3831 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2,
			    ERx("<= result_column_name>"), ERx("SELECT") );
		}
		break;
	case 497:
#line 3837 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3870 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2,
			    ERx("<result_column_name AS>"), ERx("SELECT") );
		}
		break;
	case 499:
#line 3876 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3908 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3931 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 508:
#line 3937 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 3995 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    lbl_exit_block();
		}
		break;
	case 510:
#line 4015 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* error must have INTO clause before FROM clause */
		    er_write( E_EQ0070_grMISSWORD, EQ_ERROR, 2, ERx("INTO"),
							ERx("SELECT/FETCH") );
		    YYERROR;
		}
		break;
	case 511:
#line 4022 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    erec_setup( 0 );	/* Done with list */
		}
		break;
	case 512:
#line 4027 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    erec_setup( 1 );	/* Initialize struct indicators stuff */
		}
		break;
	case 515:
#line 4035 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    erec_setup( 0 );	/* Done with one element */
		}
		break;
	case 516:
#line 4040 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if ( (gr->gr_type == T_STRUCT) && (eq->eq_flags & EQ_FIPS) )
			er_write( E_EQ050D_FIPS_VAR, EQ_WARN, 0 );
		}
		break;
	case 518:
#line 4047 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 4071 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    sc_eat(id_add,SC_STRIP|SC_NEST|SC_SEEN,ERx(")"), '(', ')');
		}
		break;
	case 520:
#line 4086 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 4126 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 4139 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = CSR_NO_FROM;
		}
		break;
	case 523:
#line 4144 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 524:
#line 4148 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-2].i | yypvt[-0].i;
		}
		break;
	case 525:
#line 4153 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 526:
#line 4157 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = FRM_JOIN;
		}
		break;
	case 527:
#line 4161 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-2].i;
		}
		break;
	case 529:
#line 4167 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-1].s;
		}
		break;
	case 534:
#line 4178 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = FRM_OK;
		}
		break;
	case 535:
#line 4182 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-1].i;
		}
		break;
	case 536:
#line 4187 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 537:
#line 4191 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 538:
#line 4196 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 539:
#line 4201 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 540:
#line 4206 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 546:
#line 4219 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_addtab( NULL, yypvt[-0].s );
		    yyval.s = (char *)0;
		}
		break;
	case 547:
#line 4225 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_addtab( NULL, yypvt[-2].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 548:
#line 4231 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    char tbuf[100];     /* buffer to hold owner.tablename */

		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_addtab( NULL, STpolycat(3,yypvt[-2].s,yypvt[-1].s,yypvt[-0].s,tbuf) );
		    yyval.s = (char *)0;
		}
		break;
	case 549:
#line 4239 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    char tbuf[100];     /* buffer to hold owner.tablename */
		
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
		        ecs_addtab( NULL, STpolycat(3,yypvt[-4].s,yypvt[-3].s,yypvt[-2].s,tbuf) );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 550:
#line 4248 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    char tbuf[100];     /* buffer to hold owner.tablename */

		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_addtab( NULL, STpolycat(3,yypvt[-2].s,yypvt[-1].s,yypvt[-0].s,tbuf) );
		    yyval.s = (char *)0;
		}
		break;
	case 551:
#line 4257 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    char tbuf[100];     /* buffer to hold owner.tablename */
		
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
		        ecs_addtab( NULL, STpolycat(3,yypvt[-4].s,yypvt[-3].s,yypvt[-2].s,tbuf) );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 552:
#line 4265 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_addtab( NULL, yypvt[-0].s );
		    yyval.s = (char *)0;
		}
		break;
	case 553:
#line 4271 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
		        ecs_addtab( NULL, yypvt[-2].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 558:
#line 4291 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = CSR_STATIC;
		}
		break;
	case 559:
#line 4295 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 4304 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = CSR_NO_WHERE;
		}
		break;
	case 561:
#line 4310 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* GROUP BY makes a cursor Read-Only */
		    if (ESQ_DCLCSR(esq))
			ecs_csrset( NULL, ECS_GROUP );
		    yyval.i = 1;
		}
		break;
	case 562:
#line 4317 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 563:
#line 4322 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 564:
#line 4326 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 565:
#line 4331 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 566:
#line 4335 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 567:
#line 4341 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 568:
#line 4346 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 569:
#line 4350 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 570:
#line 4355 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 571:
#line 4360 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 572:
#line 4365 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 4376 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* HAVING makes a cursor Read-Only */
		    if (ESQ_DCLCSR(esq))
			ecs_csrset( NULL, ECS_HAVING );
		    yyval.i = 1;
		}
		break;
	case 574:
#line 4383 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 587:
#line 4409 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
	if (!ESQ_EQSTR(yypvt[-0].s, ERx("first")))
	    er_write( E_EQ0244_yySYNWRD, EQ_ERROR, 1, yypvt[-0].s );
    }
		break;
	case 588:
#line 4414 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
	db_key( yypvt[-0].s );
    }
		break;
	case 589:
#line 4419 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
	db_key( yypvt[-0].s );
    }
		break;
	case 590:
#line 4423 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
	db_key( yypvt[-0].s );
    }
		break;
	case 593:
#line 4436 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;	/* UNION info for CREATE VIEW rule */
		}
		break;
	case 594:
#line 4440 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;		/* UNION info for CREATE VIEW rule */
		}
		break;
	case 595:
#line 4445 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;		/* UNION info used by CREATE VIEW */
		}
		break;
	case 596:
#line 4449 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-2].i;	/* UNION info used by CREATE VIEW */
		}
		break;
	case 599:
#line 4476 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 4501 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 601:
#line 4505 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (yypvt[-2].i == CSR_NO_FROM || yypvt[-0].i == CSR_NO_FROM)
			yyval.i = CSR_NO_FROM;
		    else
			yyval.i = yypvt[-2].i | yypvt[-0].i; /* MRW: probably just return $3.i */
		    ecs_csrset( NULL, ECS_UNION );
		}
		break;
	case 602:
#line 4521 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 603:
#line 4525 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-1].i;	/* MRW: probably can just return 0 */
		}
		break;
	case 605:
#line 4532 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 606:
#line 4536 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 4551 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s ); /* db_key 'cause we want spaces around it */
		    _VOID_ ecs_colupd( NULL, ERx(""), ECS_ADD|ECS_ISWILD );
		}
		break;
	case 613:
#line 4569 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 4584 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 4626 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 4641 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 4656 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    ecs_csrset( NULL, ECS_SORTED );
		}
		break;
	case 621:
#line 4665 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = CSR_STATIC;
		}
		break;
	case 622:
#line 4669 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 4687 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 4721 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    ecs_addtab( NULL, yypvt[-1].s );
		}
		break;
	case 629:
#line 4726 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 4765 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    er_write( E_EQ0411_csUPDALL, EQ_ERROR, 1, ecs_namecsr() );
		}
		break;
	case 631:
#line 4770 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = ECS_UPDATE;
		}
		break;
	case 632:
#line 4774 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 4788 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 647:
#line 4816 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 4827 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 4839 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 650:
#line 4843 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 651:
#line 4847 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 652:
#line 4851 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 4862 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 654:
#line 4866 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 655:
#line 4870 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 656:
#line 4874 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 657:
#line 4881 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 658:
#line 4885 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 659:
#line 4889 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 660:
#line 4893 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 662:
#line 4912 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 4968 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 685:
#line 4974 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-1].i;		/* bug #58171 */
		}
		break;
	case 686:
#line 4980 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 5015 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 692:
#line 5019 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 698:
#line 5031 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 699:
#line 5035 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 700:
#line 5039 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 701:
#line 5043 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 702:
#line 5049 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = ERx("");
		    /* functions make a cursor Read-Only */
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_csrset( NULL, ECS_FUNCTION );
		}
		break;
	case 703:
#line 5056 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 710:
#line 5074 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 711:
#line 5078 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 712:
#line 5084 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = NULL;
		}
		break;
	case 713:
#line 5088 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 714:
#line 5092 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 5101 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 716:
#line 5105 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 717:
#line 5109 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 718:
#line 5113 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 721:
#line 5120 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_hexconst( yypvt[-0].s );
		}
		break;
	case 722:
#line 5124 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_uconst( yypvt[-0].s );
		}
		break;
	case 723:
#line 5128 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 724:
#line 5132 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 726:
#line 5137 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 728:
#line 5142 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 729:
#line 5147 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_dtconst( yypvt[-0].s );
		}
		break;
	case 732:
#line 5155 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 5176 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 5212 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0509_FIPS_FUNC, EQ_WARN, 1, ERx("ANY"));
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0510_OPEN_FUNC, EQ_WARN, 1, ERx("ANY"));
		}
		break;
	case 757:
#line 5220 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-1].s );
		    db_op( yypvt[-0].s );
		}
		break;
	case 758:
#line 5226 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-1].s );
		    db_op( yypvt[-0].s );
		}
		break;
	case 759:
#line 5232 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 760:
#line 5236 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 761:
#line 5240 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 762:
#line 5244 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 763:
#line 5250 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 764:
#line 5254 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 767:
#line 5264 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-1].s;
		}
		break;
	case 768:
#line 5268 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 769:
#line 5272 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-4].s;
		}
		break;
	case 770:
#line 5277 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-6].s;
		}
		break;
	case 771:
#line 5281 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-5].s;
		}
		break;
	case 772:
#line 5285 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-7].s;
		}
		break;
	case 773:
#line 5289 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-4].s, ERx("position")))
			er_write( E_EQ006A_grxBADWORD, EQ_ERROR, 2, yypvt[-4].s,
				ERx("query"));
		    yyval.s = yypvt[-4].s;
		}
		break;
	case 774:
#line 5296 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-4].s, ERx("trim")))
			er_write( E_EQ006A_grxBADWORD, EQ_ERROR, 2, yypvt[-4].s,
				ERx("query"));
		    yyval.s = yypvt[-4].s;
		}
		break;
	case 778:
#line 5308 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-1].s;
		}
		break;
	case 779:
#line 5313 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 780:
#line 5318 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 781:
#line 5323 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 782:
#line 5328 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 786:
#line 5339 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 787:
#line 5343 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 794:
#line 5370 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 5382 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 5401 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 5439 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-1].s, yypvt[-0].s );
		    esq_init();
		    ecs_close( yypvt[-0].s, gr->gr_sym != (SYM *)0 );
		}
		break;
	case 798:
#line 5456 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 5471 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 5484 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 801:
#line 5498 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 802:
#line 5502 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = ECS_KEYSET;
                }
		break;
	case 803:
#line 5507 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    ecs_csrset( NULL, ECS_DYNAMIC );
		}
		break;
	case 804:
#line 5511 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    ecs_csrset( NULL, ECS_DYNAMIC );
		}
		break;
	case 805:
#line 5515 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 5551 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    ecs_csrset( NULL, ECS_ERR );
		}
		break;
	case 807:
#line 5590 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 5600 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 5614 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
			    yyval.fet.intv1 = 0;
			}
		break;
	case 811:
#line 5693 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
			    yyval.i = 0;
			}
		break;
	case 812:
#line 5698 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
			    CVan(yypvt[-0].s, &yyval.i);
			    yyval.i = -yyval.i;
			}
		break;
	case 813:
#line 5703 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
			    CVan(yypvt[-0].s, &yyval.i);
			}
		break;
	case 814:
#line 5708 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    erec_use( IICSGET, FALSE, NULL );
		    yyval.i = 0;
		}
		break;
	case 815:
#line 5713 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 816:
#line 5717 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 817:
#line 5722 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 5783 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 5803 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 5826 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 821:
#line 5831 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 822:
#line 5836 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 5851 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = NULL;
		}
		break;
	case 824:
#line 5855 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 5866 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 826:
#line 5870 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 827:
#line 5874 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IISQDAIN );
		    yyval.i = 1;
		}
		break;
	case 828:
#line 5882 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 5923 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 5940 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 5959 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 5971 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 834:
#line 5976 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 5985 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = NULL;
		}
		break;
	case 836:
#line 6014 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1,
				ERx("DROP") );
		}
		break;
	case 837:
#line 6021 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 6062 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 856:
#line 6067 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = (char *)0;
		}
		break;
	case 857:
#line 6071 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 858:
#line 6076 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 859:
#line 6081 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 860:
#line 6087 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("DROP LINK"));
		}
		break;
	case 861:
#line 6095 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( IIPROC_DROP );

		    /* use first name encountered. */
		    arg_str_add( ARG_CHAR, yypvt[-0].s);
		    gen_call( IIPROCINIT );
		}
		break;
	case 863:
#line 6105 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s);
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("DROP PROCEDURE"));
		}
		break;
	case 866:
#line 6118 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
					EQ_WARN, 1, ERx("DROP ROLE"));
		}
		break;
	case 867:
#line 6126 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP GROUP") );
		}
		break;
	case 868:
#line 6134 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
					EQ_WARN, 1, ERx("DROP DBEVENT"));
		}
		break;
	case 869:
#line 6142 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
					EQ_WARN, 1, ERx("DROP SEQUENCE"));
		}
		break;
	case 870:
#line 6150 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("DROP INTEGRITY"));
		}
		break;
	case 871:
#line 6157 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("DROP PERMIT"));
		}
		break;
	case 872:
#line 6165 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 874:
#line 6170 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 876:
#line 6175 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 889:
#line 6199 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP RULE") );
		}
		break;
	case 890:
#line 6207 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP DOMAIN") );
		}
		break;
	case 891:
#line 6215 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP LOCATION") );
		}
		break;
	case 892:
#line 6223 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP USER") );
		}
		break;
	case 893:
#line 6231 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP PROFILE") );
		}
		break;
	case 894:
#line 6239 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP SECURITY_ALARM") );
		}
		break;
	case 895:
#line 6247 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 899:
#line 6275 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 6287 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 6303 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("privileges")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					    ERx("REVOKE"), ERx("PRIVILEGES") );
		    db_key( ERx("privileges") );
		}
		break;
	case 918:
#line 6325 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 6340 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 6357 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 924:
#line 6361 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 927:
#line 6369 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 928:
#line 6373 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 930:
#line 6385 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 6401 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 6430 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
				EQ_WARN, 1, ERx("COPY TABLE") );
		}
		break;
	case 950:
#line 6459 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_op( yypvt[-2].s );
		    db_key( yypvt[-1].s );
		    db_op( yypvt[-0].s );
		}
		break;
	case 951:
#line 6465 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_op( yypvt[-4].s );
		    db_key( yypvt[-3].s );
		    db_op( yypvt[-2].s );
		    db_key( yypvt[-1].s );
		    db_op( yypvt[-0].s );
		}
		break;
	case 958:
#line 6482 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 959:
#line 6486 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 961:
#line 6492 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 6505 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 6528 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 6570 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 6583 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_FIPS) 
			er_write( E_EQ0507_FIPS_NOTOP, EQ_WARN,
				2, ERx("PRIVILEGES"), ERx("GRANT") );
		}
		break;
	case 973:
#line 6597 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, ERx("INSERT"), ERx("GRANT") );
		    }
		}
		break;
	case 975:
#line 6606 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, ERx("EXECUTE"), ERx("GRANT") );
		    }
		}
		break;
	case 976:
#line 6614 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, ERx("REGISTER"), ERx("GRANT") );
		    }
		}
		break;
	case 979:
#line 6624 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, yypvt[-0].s, ERx("GRANT") );
		    }
		}
		break;
	case 980:
#line 6632 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, yypvt[-1].s, ERx("GRANT") );
		    }
		}
		break;
	case 983:
#line 6644 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("privileges")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					    ERx("GRANT"), ERx("PRIVILEGES") );
		    db_key( ERx("privileges") );
		}
		break;
	case 985:
#line 6654 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
                    db_key( yypvt[-0].s );
                }
		break;
	case 986:
#line 6659 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 987:
#line 6664 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 988:
#line 6669 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 989:
#line 6674 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 990:
#line 6679 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 991:
#line 6684 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 6693 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 6703 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		}
		break;
	case 994:
#line 6706 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 6725 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 997:
#line 6730 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 998:
#line 6735 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 999:
#line 6741 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 6752 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 6764 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* TO GROUP is non-FIPS */
		    if (eq->eq_flags & EQ_FIPS && gr->gr_func != tREVOKE)
			er_write( E_EQ0506_FIPS_CLAUSE,
				EQ_WARN, 2, ERx("TO GROUP"), ERx("GRANT") );
		}
		break;
	case 1003:
#line 6771 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 6788 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 1009:
#line 6798 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 1010:
#line 6803 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 1014:
#line 6812 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 1017:
#line 6829 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 6853 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1037:
#line 6885 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 6903 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-3].s, NULL );
		    esq_init();
		}
		break;
	case 1039:
#line 6909 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_INT, yypvt[-0].s );
		}
		break;
	case 1040:
#line 6913 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 6942 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1, yypvt[-1].s );
		}
		break;
	case 1042:
#line 6947 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 1043:
#line 6957 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 6967 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 1047:
#line 6974 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 7020 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 7031 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = (char *)0;
		}
		break;
	case 1051:
#line 7035 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1052:
#line 7039 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1053:
#line 7043 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1055:
#line 7049 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 7065 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1057:
#line 7069 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-2].i | yypvt[-0].i;
		}
		break;
	case 1058:
#line 7074 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1059:
#line 7078 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-2].i;
		}
		break;
	case 1061:
#line 7084 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1062:
#line 7089 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 7118 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				1, ERx("REGISTER INDEX") );
		}
		break;
	case 1071:
#line 7125 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    STprintf( esq->sbuf, ERx("%s %s"), yypvt[-2].s, yypvt[-1].s );
		    gr_mechanism( GR_EQSTMT, GR_sSQL, esq->sbuf, yypvt[-0].s );
		}
		break;
	case 1072:
#line 7130 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		}
		break;
	case 1078:
#line 7144 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 7162 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 7178 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("REMOVE"));
		}
		break;
	case 1082:
#line 7185 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (yypvt[-0].s != (char *)0)
			gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		    else
			gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, NULL );
		}
		break;
	case 1083:
#line 7192 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("REMOVE DBEVENT"));
		}
		break;
	case 1084:
#line 7200 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = (char *)0;
		}
		break;
	case 1085:
#line 7204 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1086:
#line 7208 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1087:
#line 7212 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1088:
#line 7216 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1089:
#line 7226 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 7237 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 7251 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 7263 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 7290 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 7301 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1100:
#line 7308 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1101:
#line 7351 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 7362 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 7378 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 7387 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 7396 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
                    {
                        er_write( E_EQ0504_OPEN_WARN,
                                        EQ_WARN, 1, ERx("SET SESSION"));
                    }
                }
		break;
	case 1106:
#line 7406 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 7453 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("privileges")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					    ERx("SET SESSION"), ERx("PRIVILEGES") );
		    db_key( ERx("privileges") );
		}
		break;
	case 1126:
#line 7475 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 1131:
#line 7484 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
			db_key (yypvt[-0].s);
		}
		break;
	case 1132:
#line 7488 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
			db_key (yypvt[-0].s);
		}
		break;
	case 1133:
#line 7492 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
			db_key (yypvt[-0].s);
		}
		break;
	case 1134:
#line 7496 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
			db_key (yypvt[-0].s);
		}
		break;
	case 1135:
#line 7500 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 7517 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1138:
#line 7521 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1146:
#line 7534 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Expecting SET WORK LOCATIONS command */		
		    if (!ESQ_EQSTR(yypvt[-4].s, ERx("locations")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-4].s,
					ERx("SET WORK"), ERx("LOCATIONS") );
		}
		break;
	case 1147:
#line 7541 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{ 
		    db_sconst( yypvt[-0].s );
		}
		break;
	case 1148:
#line 7545 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{ 
		    db_key( yypvt[-0].s );
		}
		break;
	case 1158:
#line 7561 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    erec_use( IISETDOM, gr->gr_flag & GR_REPEAT, ERx(",") );
		    erec_setup( 1 );
		}
		break;
	case 1168:
#line 7580 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1169:
#line 7584 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1174:
#line 7595 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
			db_key( yypvt[-0].s );
		}
		break;
	case 1176:
#line 7603 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 7612 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 7621 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 7630 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
                    /* Don't print if in DECLARE TABLE statement */
                    if ((esq->flag & ESQ_NOPRT) == 0)
                        db_key( yypvt[-0].s );
		}
		break;
	case 1180:
#line 7637 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
                    if (eq->eq_flags & EQ_FIPS)
                        er_write( E_EQ050F_FIPS_USE, EQ_WARN, 1, yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0510_OPEN_FUNC, EQ_WARN, 1, yypvt[-0].s );
		}
		break;
	case 1182:
#line 7651 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 7671 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s ); 
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1190:
#line 7699 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key(yypvt[-1].s);
		    db_key(yypvt[-0].s);
		}
		break;
	case 1191:
#line 7704 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key(yypvt[-1].s);
		    db_key(yypvt[-0].s);
		}
		break;
	case 1192:
#line 7709 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key(yypvt[-1].s);
		    db_key(yypvt[-0].s);
		}
		break;
	case 1193:
#line 7714 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 1194:
#line 7720 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    gr->gr_func = tSET;
		}
		break;
	case 1195:
#line 7726 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 1196:
#line 7740 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1197:
#line 7745 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1198:
#line 7750 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1199:
#line 7755 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1201:
#line 7761 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1202:
#line 7766 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1203:
#line 7771 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1204:
#line 7776 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1205:
#line 7781 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1206:
#line 7786 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1207:
#line 7791 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1208:
#line 7796 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_op(ERx(", "));	/* Need a space for run-time */
		}
		break;
	case 1209:
#line 7801 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1210:
#line 7806 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1211:
#line 7811 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		if ((esq->flag & ESQ_NOPRT) == 0)
		    db_key( yypvt[-0].s );
		}
		break;
	case 1212:
#line 7817 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		if ((esq->flag & ESQ_NOPRT) == 0)
		    db_op( yypvt[-0].s );
		}
		break;
	case 1213:
#line 7823 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1214:
#line 7828 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1215:
#line 7833 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1216:
#line 7838 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1217:
#line 7843 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1218:
#line 7848 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1219:
#line 7853 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1220:
#line 7858 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1221:
#line 7863 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		if ((esq->flag & ESQ_NOPRT) == 0)
		    db_key( yypvt[-0].s );
		}
		break;
	case 1222:
#line 7869 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1223:
#line 7874 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1224:
#line 7879 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1225:
#line 7884 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1226:
#line 7889 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1227:
#line 7894 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1228:
#line 7899 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1229:
#line 7904 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1230:
#line 7909 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1231:
#line 7914 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1232:
#line 7919 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_op( ERx(" -") );
		}
		break;
	case 1233:
#line 7924 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1234:
#line 7929 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		if ((esq->flag & ESQ_NOPRT) == 0)
		    db_key( yypvt[-0].s );
		}
		break;
	case 1235:
#line 7935 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1236:
#line 7940 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1237:
#line 7945 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1238:
#line 7950 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1239:
#line 7955 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1240:
#line 7960 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1241:
#line 7965 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1242:
#line 7970 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1243:
#line 7975 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1244:
#line 7980 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1245:
#line 7985 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1246:
#line 7990 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1247:
#line 7995 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1248:
#line 8000 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1249:
#line 8005 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_op( ERx(" +") );
		}
		break;
	case 1250:
#line 8010 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1251:
#line 8015 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1252:
#line 8020 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1253:
#line 8025 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1254:
#line 8030 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1255:
#line 8035 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1256:
#line 8040 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1257:
#line 8045 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1258:
#line 8050 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1259:
#line 8055 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1260:
#line 8060 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1261:
#line 8065 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1262:
#line 8070 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1263:
#line 8075 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 8087 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 8096 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 8106 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 8117 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1268:
#line 8122 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1271:
#line 8151 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1272:
#line 8157 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 1273:
#line 8161 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 1274:
#line 8167 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if ((esq->flag & ESQ_NOPRT) == 0)
			db_key( yypvt[-0].s );
                    yyval.s = yypvt[-0].s;
                }
		break;
	case 1275:
#line 8173 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if ((esq->flag & ESQ_NOPRT) == 0)
			db_delimid( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1276:
#line 8180 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
                    yyval.s = yypvt[-0].s;
                    if (eq->eq_flags & EQ_FIPS)
                        eqck_regid(yypvt[-0].s); /* Check if id is FIPS compliant */
                }
		break;
	case 1277:
#line 8187 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		    if (eq->eq_flags & EQ_FIPS)
			eqck_delimid(yypvt[-0].s); /* Check if id is FIPS complaint */
		}
		break;
	case 1279:
#line 8196 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 8220 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
                    db_key( yypvt[-0].s );
                    yyval.s = yypvt[-0].s;
                    gr->gr_sym = (SYM *)0;
                }
		break;
	case 1283:
#line 8227 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
                    db_key( yypvt[-0].s );
                    yyval.s = yypvt[-0].s;
                    gr->gr_sym = (SYM *)0;
                }
		break;
	case 1284:
#line 8233 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
                    db_key( yypvt[-0].s );
                    yyval.s = yypvt[-0].s;
                    gr->gr_sym = (SYM *)0;
                }
		break;
	case 1285:
#line 8239 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
                    db_key( yypvt[-0].s );
                    yyval.s = yypvt[-0].s;
                    gr->gr_sym = (SYM *)0;
                }
		break;
	case 1286:
#line 8250 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_sconst( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1287:
#line 8259 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1288:
#line 8263 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1289:
#line 8271 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = ERx(" ");
		}
		break;
	case 1290:
#line 8275 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = ERx(",");
		}
		break;
	case 1291:
#line 8285 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1292:
#line 8297 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 8308 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 8319 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 8332 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 8378 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 8399 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 8419 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add(ARG_CHAR, yypvt[-0].s);
		}
		break;
	case 1307:
#line 8424 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    STprintf( esq->sbuf, ERx("%s%s%s"), yypvt[-2].s, yypvt[-1].s, yypvt[-0].s );
		    arg_str_add(ARG_CHAR, esq->sbuf);
		}
		break;
	case 1308:
#line 8429 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 8444 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_nlsym = (SYM *)0;	/* Initialize state vars */
		    gr->gr_nlid = (char *)0;
		    gr->gr_nltype = T_NONE; 
		}
		break;
	case 1312:
#line 8453 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 8480 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 8500 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1318:
#line 8510 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1321:
#line 8523 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("asc")) &&
					!ESQ_EQSTR(yypvt[-0].s, ERx("desc")))
			er_write( E_EQ0126_sqCOMMA, EQ_ERROR, 0);
		    db_key( yypvt[-0].s );
		}
		break;
	case 1322:
#line 8541 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 1323:
#line 8545 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 1327:
#line 8577 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_flag &= ~GR_CPYHNDDEF;
		}
		break;
	case 1333:
#line 8588 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (   gr->gr_flag & GR_COPY_PROG
		        && ESQ_EQSTR(yypvt[-0].s, ERx("copyhandler")))
			gr->gr_flag |= GR_CPYHNDDEF;
		    db_key( yypvt[-0].s );
		}
		break;
	case 1334:
#line 8595 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1335:
#line 8599 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1336:
#line 8603 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1337:
#line 8607 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1338:
#line 8611 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1339:
#line 8615 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1340:
#line 8619 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1341:
#line 8623 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1345:
#line 8631 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1346:
#line 8635 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1347:
#line 8639 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1348:
#line 8643 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1349:
#line 8647 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1354:
#line 8659 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1355:
#line 8663 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1394:
#line 8773 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 8783 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s, NULL );
		    esq_init();
		    arg_int_add( IIutPROG );
		}
		break;
	case 1396:
#line 8790 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_call( IIUTSYS );
		}
		break;
	case 1399:
#line 8800 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( IIutARG );
		}
		break;
	case 1402:
#line 8808 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IIUTSYS );
		}
		break;
	case 1404:
#line 8814 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 1405:
#line 8818 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 1406:
#line 8823 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( IIutARG );
		}
		break;
	case 1421:
#line 8875 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 8885 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IISQSESS );
		}
		break;
	case 1423:
#line 8890 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 8907 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IISQCNNM );
		}
		break;
	case 1425:
#line 8912 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_push();
		}
		break;
	case 1426:
#line 8916 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{ arg_push(); }
		break;
	case 1427:
#line 8917 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IISQUSER );
		}
		break;
	case 1429:
#line 8925 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{ 
		  arg_str_add(ARG_CHAR, ERx("-remote_system_user")); 
		}
		break;
	case 1431:
#line 8932 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{ 
		  arg_str_add(ARG_CHAR, ERx("-remote_system_password"));
		}
		break;
	case 1433:
#line 8938 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		  arg_str_add(ARG_CHAR, ERx("-dbms_password"));
	        }
		break;
	case 1437:
#line 8947 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-1].s, ERx("options")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
					    ERx("CONNECT"), ERx("OPTIONS") );
		}
		break;
	case 1441:
#line 8959 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 8973 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 1443:
#line 8977 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 1444:
#line 8983 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 8992 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IISQCONNECT );
		}
		break;
	case 1446:
#line 8997 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    esq_sqmods(IImodWITH);  	/* force in modifier */
		    gen_call( IISQCONNECT );	/* Close CONNECT */

		    arg_int_add(IIsqpINI);

		  /* Initialize for 2PC with-clause */
		    gr->gr_flag &= ~(GR_HIGHX|GR_LOWX);	
		}
		break;
	case 1447:
#line 9008 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
			/* All specified in string variable */
			arg_int_add(DB_CHR_TYPE);
			arg_str_add(ARG_CHAR, (char *)0);
			arg_int_add(0);
			gen_call(IISQPARMS);
		}
		break;
	case 1448:
#line 9016 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9029 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
			/* Start the next call */
			arg_int_add(IIsqpADD);
		}
		break;
	case 1452:
#line 9035 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* End the current call */
		    gen_call(IISQPARMS);
		}
		break;
	case 1453:
#line 9041 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add(DB_CHR_TYPE);
		    arg_str_add( ARG_CHAR, form_sconst(yypvt[-0].s) );
		    arg_int_add(0);
		    if (gr->gr_flag & GR_HIGHX || gr->gr_flag & GR_LOWX)
			er_write( E_EQ0079_grCONNECTID, EQ_ERROR, 0 );
		}
		break;
	case 1454:
#line 9049 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add(DB_CHR_TYPE);
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    arg_int_add(0);
		    if (gr->gr_flag & GR_HIGHX || gr->gr_flag & GR_LOWX)
			er_write( E_EQ0079_grCONNECTID, EQ_ERROR, 0 );
		}
		break;
	case 1455:
#line 9057 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add(DB_INT_TYPE);
		    arg_str_add( ARG_CHAR, (char *)0 );
		    arg_str_add( ARG_INT, yypvt[-0].s );
		}
		break;
	case 1456:
#line 9063 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9107 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9119 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9131 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9153 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    er_write( E_EQ0070_grMISSWORD, EQ_ERROR, 2, ERx("INTO"),
							ERx("COPY SQLERROR") );
		}
		break;
	case 1465:
#line 9166 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9176 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    esq->flag |= ESQ_NOPRT;	/* Suppress printing */
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-1].s, yypvt[-0].s );
		}
		break;
	case 1467:
#line 9181 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    esq->flag |= ESQ_NOPRT;	/* Suppress printing */
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-3].s, yypvt[-2].s );
		}
		break;
	case 1473:
#line 9195 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1474:
#line 9199 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-2].i || yypvt[-0].i;	/* DEFAULT keyword */
		}
		break;
	case 1475:
#line 9204 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    id_free();
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1476:
#line 9209 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    id_free();
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1477:
#line 9215 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    id_free();
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1483:
#line 9254 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1484:
#line 9259 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1485:
#line 9263 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1486:
#line 9268 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		    esq->flag &= ~ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 1487:
#line 9273 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		    esq->flag &= ~ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 1488:
#line 9278 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		    esq->flag &= ~ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 1489:
#line 9283 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		    esq->flag |= ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 1490:
#line 9288 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		    esq->flag &= ~ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 1491:
#line 9294 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9308 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9321 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9340 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1496:
#line 9344 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-1].i;
		}
		break;
	case 1497:
#line 9349 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9359 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9369 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9373 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = CRE_WDEF;
		}
		break;
	case 1501:
#line 9377 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = CRE_OK;
		}
		break;
	case 1502:
#line 9381 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = CRE_NDEF;
		}
		break;
	case 1503:
#line 9386 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9396 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    	db_key( yypvt[-0].s );
		}
		break;
	case 1505:
#line 9403 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9414 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    	db_key( yypvt[-0].s );
		}
		break;
	case 1509:
#line 9421 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    	db_sconst( yypvt[-0].s );
		}
		break;
	case 1510:
#line 9427 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    	db_key( yypvt[-0].s );
		}
		break;
	case 1511:
#line 9433 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    	db_key( yypvt[-0].s );
		}
		break;
	case 1513:
#line 9441 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9451 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9480 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9509 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IISQEXIT );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ050A_FIPS_EXTSN, EQ_WARN,
					1, ERx("DISCONNECT") );
		}
		break;
	case 1530:
#line 9517 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-0].s, NULL );
		    esq_init();
		}
		break;
	case 1531:
#line 9523 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IISQSESS );
		}
		break;
	case 1532:
#line 9527 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add ( IIsqdisALL );	
		    gen_call( IISQSESS );
		}
		break;
	case 1533:
#line 9532 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* connection name */
		    gen_call( IISQCNNM );
		}
		break;
	case 1536:
#line 9550 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    eqgen_tl(IIGETDATA);
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		}
		break;
	case 1537:
#line 9557 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9571 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9592 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
                    eqgen_tl(IIPUTDATA);
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
                }
		break;
	case 1541:
#line 9599 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9617 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IIEVTSTAT );
		    gr->gr_flag |= GR_GETEVT;	/* Special WHENEVER handling */
		}
		break;
	case 1543:
#line 9623 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9636 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( 0 );	/* "no wait" */
		}
		break;
	case 1547:
#line 9643 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-1].s, ERx("WAIT")))
		    {
			er_write(E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
					ERx("GET DBEVENT"), ERx("WAIT"));
		    }
		}
		break;
	case 1548:
#line 9652 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9681 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    eqtl_add(gr->gr_id, (PTR)gr->gr_sym, gr->gr_nlid, (PTR)gr->gr_nlsym,
				gr->gr_type, yypvt[-0].s);
		    if ( (gr->gr_type == T_STRUCT) && (eq->eq_flags & EQ_FIPS) )
			er_write( E_EQ050D_FIPS_VAR, EQ_WARN, 0 );
		}
		break;
	case 1554:
#line 9689 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1555:
#line 9693 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1556:
#line 9697 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1557:
#line 9702 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
                       gr->gr_id = yypvt[-0].s;
                       gr->gr_type = T_INT;
                       gr->gr_sym = (SYM *)0;
                       gr->gr_nlsym = (SYM *)0;
		}
		break;
	case 1558:
#line 9709 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9734 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( QAPRINTF );
		}
		break;
	case 1560:
#line 9739 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-0].s, NULL );
		}
		break;
	case 1564:
#line 9749 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (gr->gr_sym)
			arg_var_add( gr->gr_sym, gr->gr_id );
		    else
			arg_str_add( ARG_CHAR, gr->gr_id );
		}
		break;
	case 1567:
#line 9766 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
			eqgen_tl(IIINQSQL);
			if (eq->eq_flags & EQ_CHRPAD)
			    esq_sqmods(IImodNOCPAD);
		}
		break;
	case 1568:
#line 9773 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9793 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
			eqgen_tl(IISETSQL);
		}
		break;
	case 1570:
#line 9797 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    eqtl_add(gr->gr_id, (PTR)gr->gr_sym, gr->gr_nlid, 
			 (PTR)gr->gr_nlsym, gr->gr_type, ERx("connection_name"));
		    eqgen_tl(IISETSQL);
		}
		break;
	case 1571:
#line 9803 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9834 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
			gr->gr_id = yypvt[-0].s;
			gr->gr_type = T_CHAR;
			gr->gr_sym = (SYM *)0;
			gr->gr_nlsym = (SYM *)0;
		}
		break;
	case 1574:
#line 9843 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9853 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9874 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9910 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
			gr->gr_id = yypvt[-0].s;
			gr->gr_type = T_INT;
			gr->gr_sym = (SYM *)0;
			gr->gr_nlsym = (SYM *)0;
		}
		break;
	case 1581:
#line 9917 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
			gr->gr_id = yypvt[-0].s;
			gr->gr_type = T_CHAR;
			gr->gr_sym = (SYM *)0;
			gr->gr_nlsym = (SYM *)0;
		}
		break;
	case 1583:
#line 9925 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
			gr->gr_id = yypvt[-0].s;
			gr->gr_type = T_NONE;
			gr->gr_sym = (SYM *)0;
			gr->gr_nlsym = (SYM *)0;
		}
		break;
	case 1584:
#line 9946 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (yypvt[-0].i && (esq->inc & sqcaSQL) == 0)
		    {
			er_write( E_EQ0128_sqNOSQLCA, EQ_ERROR, 0 );
			esq->inc |= sqcaSQL;
		    }
		}
		break;
	case 1585:
#line 9956 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    esq->flag |= ESQ_NOPRT;	/* Suppress printing */
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-0].s, NULL );
		}
		break;
	case 1586:
#line 9964 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 9997 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 10013 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    esqlca( yypvt[-1].i, sqcaCONTINUE, (char *)0 );
		    yyval.i = 0;
		}
		break;
	case 1589:
#line 10018 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    esqlca( yypvt[-3].i, sqcaGOTO, yypvt[-0].s );
		    yyval.i = 1;
		}
		break;
	case 1590:
#line 10023 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    esqlca( yypvt[-2].i, sqcaGOTO, yypvt[-0].s );
		    yyval.i = 1;
		}
		break;
	case 1591:
#line 10028 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    esqlca( yypvt[-2].i, sqcaCALL, yypvt[-0].s );
		    yyval.i = 1;
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				    2, ERx("CALL"), ERx("WHENEVER") );
		}
		break;
	case 1592:
#line 10036 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 10067 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    arg_int_add( yypvt[-0].i );		/* using names? */
		    gen_call( IISQDESCRIBE );
		}
		break;
	case 1594:
#line 10074 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 10091 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = IIsqdNULUSE;
		}
		break;
	case 1599:
#line 10095 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 10120 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IISQDESCINPUT );
		}
		break;
	case 1601:
#line 10126 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s, NULL );
		    esq_init();
		    gr->gr_flag |= GR_NOSYNC;
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		}
		break;
	case 1602:
#line 10197 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("EXECUTE IMMEDIATE") );
		}
		break;
	case 1603:
#line 10203 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("EXECUTE") );
		}
		break;
	case 1604:
#line 10210 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 10221 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IISQEXIMMED );
		}
		break;
	case 1606:
#line 10225 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_flag |= GR_NOSYNC;
		    esq->flag &= ~ESQ_DESC_SEL;
		}
		break;
	case 1607:
#line 10231 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_CHAR, form_sconst(yypvt[-0].s) );
		}
		break;
	case 1608:
#line 10235 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 10252 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    esq_sqmods(IImodDYNSEL);
		    esq->flag |= ESQ_REG_SELECT;
		    esl_query( ESL_START_QUERY );
		    gen_call( IISQEXIMMED );
		}
		break;
	case 1610:
#line 10260 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 10290 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    erec_desc( gr->gr_id );	    /* Save arg for IIcsDaGet */
		    esq->flag |= ESQ_DESC_SEL;
		}
		break;
	case 1612:
#line 10295 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    erec_setup( 0 );			/* Done with list */
		}
		break;
	case 1613:
#line 10300 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( 0 );			/* not using vars */
		    gen_call( IISQEXSTMT );
		}
		break;
	case 1615:
#line 10306 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IISQDAIN );
		}
		break;
	case 1616:
#line 10313 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( 1 );			/* using vars */
		    gen_call( IISQEXSTMT );
		    gr->gr_func = tEXECUTE;
		}
		break;
	case 1617:
#line 10320 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( 0 );			/* not using vars */
		    gen_call( IISQEXSTMT );
		}
		break;
	case 1620:
#line 10329 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_var( DB_REG, gr->gr_sym, gr->gr_id,
			    gr->gr_nlsym, gr->gr_nlid, (char *)0 );
		}
		break;
	case 1621:
#line 10334 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_var( DB_HDLR, gr->gr_sym, gr->gr_id,
			    gr->gr_nlsym, gr->gr_nlid, yypvt[-1].s );
		}
		break;
	case 1622:
#line 10375 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 10400 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 10435 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Copy (*gr) for "proc_exec" to build code. */
		    STRUCT_ASSIGN_MACRO(*gr, tempgr);
		}
		break;
	case 1627:
#line 10447 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 10460 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IIPROCINIT );
		}
		break;
	case 1631:
#line 10468 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IIPROCGTTP );
		}
		break;
	case 1632:
#line 10472 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (esq->flag & ESQ_PROC_BYREF)
		    {
			/* procs w/BYREF params act like singleton select */
			esq_sqmods(IImodSINGLE);
		    }
		}
		break;
	case 1633:
#line 10480 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IISQDAIN );
		}
		break;
	case 1638:
#line 10494 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IIPROCVALIO );
		}
		break;
	case 1642:
#line 10504 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
			arg_int_add( TRUE );
		}
		break;
	case 1643:
#line 10509 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( FALSE );
		}
		break;
	case 1644:
#line 10514 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 10545 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 10562 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 10574 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 10628 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    lbl_exit_block();
		}
		break;
	case 1651:
#line 10648 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 10671 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( gr->gr_rcnt );
		    gen_loop( G_OPEN, L_RETBEG, L_RETFLUSH, 
			    lbl_next(LBL_RET), IIPROCSTAT, C_NOTEQ, 0 );
		    gen_sqlca( sqcaSQPROC );
		    yyval.i = 1;		/* indicate result clause */
		}
		break;
	case 1653:
#line 10680 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (esq->flag & ESQ_PROC_BYREF)
			er_write( E_EQ005A_grBYREF, EQ_ERROR, 0);
				/* can't have both BYREF parm & RESULT ROW */
		    erec_setup( 1 );	/* Initialize struct indicators stuff */
		    esq->flag |= ESQ_PROC_RESULT;  /* & set flag */
		}
		break;
	case 1656:
#line 10692 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    erec_setup( 0 );	/* Done with one element */
		}
		break;
	case 1657:
#line 10697 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_rcnt++;
		}
		break;
	case 1658:
#line 10711 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IISQPREPARE );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("PREPARE") );
		}
		break;
	case 1659:
#line 10720 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 10731 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s, NULL );
		    esq_init();
		    gr->gr_flag |= GR_NOSYNC;
		    arg_int_add( eq_genlang(eq->eq_lang) );  /* host language */
		}
		break;
	case 1661:
#line 10739 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_id = NULL;
		    yyval.i = 0;
		}
		break;
	case 1662:
#line 10744 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1664:
#line 10751 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 10764 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_CHAR, form_sconst(yypvt[-0].s) );
		}
		break;
	case 1669:
#line 10793 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("DIRECT CONNECT") );
		}
		break;
	case 1670:
#line 10801 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("DIRECT DISCONNECT") );
		}
		break;
	case 1671:
#line 10809 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("DIRECT EXECUTE IMMEDIATE") );
		}
		break;
	case 1673:
#line 10818 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 10909 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    lbl_exit_block();
		}
		break;
	case 1716:
#line 10914 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 10926 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 1720:
#line 10942 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IIADDFORM );
		    gr->gr_flag &= ~GR_NOINDIR;
		}
		break;
	case 1721:
#line 10948 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gr->gr_flag |= GR_NOINDIR;
		}
		break;
	case 1722:
#line 10954 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 10973 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
# ifndef EQ_ADD_FNAME
		    er_write( E_EQ0061_grINTVAR, EQ_ERROR, 1, yypvt[-0].s );
# endif /* EQ_ADD_FNAME */
		}
		break;
	case 1724:
#line 10989 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11010 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-1].s, NULL );
		    gen_call( IICLRSCR );
		}
		break;
	case 1726:
#line 11015 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IICLRFLDS );
		}
		break;
	case 1728:
#line 11021 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-1].s, NULL );
		    gr->gr_func = IIFLDCLEAR;
		}
		break;
	case 1729:
#line 11053 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IIFRSQDESC );
		}
		break;
	case 1730:
#line 11059 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IIFRSQDESC );
		}
		break;
	case 1731:
#line 11065 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_int_add( 1 );			/* is_form = TRUE */
		}
		break;
	case 1732:
#line 11072 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_CHAR, (char* )0 );	/* No table name */
		}
		break;
	case 1733:
#line 11077 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    STprintf( esq->sbuf, ERx("%s %s"), yypvt[-1].s, yypvt[-0].s );
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, esq->sbuf, NULL );
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_int_add( 0 );			/* is_form = FALSE */
		}
		break;
	case 1734:
#line 11085 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_CHAR, ERx("u") );
		}
		break;
	case 1735:
#line 11089 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_CHAR, ERx("a") );
		}
		break;
	case 1736:
#line 11093 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11118 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11138 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_CHAR, ERx("a") );
		}
		break;
	case 1739:
#line 11143 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1740:
#line 11147 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1741:
#line 11213 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11247 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11266 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11285 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gr->gr_func = IIDISPFRM;
		}
		break;
	case 1745:
#line 11291 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    frs_mode( ERx("DISPLAY/INITTABLE"), yypvt[-0].s );
		}
		break;
	case 1746:
#line 11295 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    frs_mode( ERx("DISPLAY/INITTABLE"), yypvt[-0].s );
		}
		break;
	case 1747:
#line 11299 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    frs_mode( ERx("DISPLAY/INITTABLE"), yypvt[-0].s );
		}
		break;
	case 1748:
#line 11303 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_CHAR, ERx("u") );
		}
		break;
	case 1749:
#line 11307 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11324 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_CHAR, ERx("f") );
		}
		break;
	case 1751:
#line 11329 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    sc_eat( NULL, SC_NEST|SC_SEEN, ERx(")"), '(', ')' );
		    er_write( E_EQ0051_grDISPTL, EQ_ERROR, 0 );
		}
		break;
	case 1753:
#line 11357 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11374 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11424 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11434 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_flag &= ~GR_ACTIVATE;
		}
		break;
	case 1763:
#line 11442 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_else( G_ELSE, IIRETVAL, C_0, 0, L_ACTELSE, 
			lbl_current(LBL_INTR), L_ACTEND, lbl_current(LBL_FORM));
		    gr->gr_mode = LBLmDISPLAY;
		}
		break;
	case 1764:
#line 11449 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11463 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Reset activate argument list */
		    act_args( FALSE, (char *)0, (char *)0, 0, (SYM *)0 );
		}
		break;
	case 1768:
#line 11477 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    act_add( ACT_FIELD, lbl_current(LBL_INTR), yypvt[-2].i, yypvt[-0].ip );
		    gr->gr_adjective = LBLaFIELD;
		}
		break;
	case 1769:
#line 11482 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    act_add( ACT_MENU, lbl_current(LBL_INTR), 0, yypvt[-1].ip );
		    gr->gr_adjective = LBLaMITEM;
		}
		break;
	case 1770:
#line 11487 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    act_add( ACT_SCROLL, lbl_current(LBL_INTR), sclUP, yypvt[-0].ip );
		    gr->gr_adjective = LBLaSCROLL;
		}
		break;
	case 1771:
#line 11492 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    act_add( ACT_SCROLL,lbl_current(LBL_INTR), sclDOWN, yypvt[-0].ip );
		    gr->gr_adjective = LBLaSCROLL;
		}
		break;
	case 1772:
#line 11497 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    i4  cur_intr = lbl_current( LBL_INTR );

		    act_add( ACT_COLUMN, cur_intr, yypvt[-3].i, yypvt[-1].ip );
		    act_add( ACT_COLUMN, cur_intr, yypvt[-3].i, yypvt[-0].ip );
		    gr->gr_adjective = LBLaCOLUMN;
		}
		break;
	case 1773:
#line 11505 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Old-style syntax - no longer supported */
                    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1,
                                            ERx("ACTIVATE COMMAND") );
		}
		break;
	case 1775:
#line 11513 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 1776:
#line 11517 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11532 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.ip = act_string( form_sconst(yypvt[-0].s) );
		}
		break;
	case 1778:
#line 11536 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.ip = act_string( yypvt[-0].s );
		}
		break;
	case 1779:
#line 11540 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.ip = act_string( yypvt[-0].s );
		}
		break;
	case 1780:
#line 11544 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11557 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1782:
#line 11561 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1783:
#line 11575 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11634 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11653 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11667 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11683 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 1788:
#line 11687 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 1791:
#line 11695 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* It will check for errors */
		    act_args( TRUE, yypvt[-2].s, yypvt[-0].s, gr->gr_type, gr->gr_sym );
		}
		break;
	case 1792:
#line 11701 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1793:
#line 11705 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1794:
#line 11709 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1795:
#line 11713 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1796:
#line 11718 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = form_sconst(yypvt[-0].s);
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = NULL;
		}
		break;
	case 1797:
#line 11724 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_INT;
		    gr->gr_sym = NULL;
		}
		break;
	case 1798:
#line 11730 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11757 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_goto( G_TERM, L_FDEND, lbl_current(LBL_FORM) );
		}
		break;
	case 1800:
#line 11762 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11780 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11805 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11841 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11861 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gen_call( IIENDFORMS );
		}
		break;
	case 1807:
#line 11876 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11923 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    i4  cur_form = lbl_current( LBL_FORM );

		    gen_loop( G_CLOSE, L_FDBEG, L_FDEND, cur_form, IIFNAMES,
			C_0, 0 );
		    gen_label( G_LOOP, L_FDEND, cur_form );
		    gen_call( IIENDFRM );
		}
		break;
	case 1809:
#line 11933 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 11946 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1812:
#line 11960 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gr->gr_func = IIFORMINIT;
		}
		break;
	case 1813:
#line 11973 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( eq_genlang(eq->eq_lang) );
		    gen_call( IIFORMS );
		}
		break;
	case 1814:
#line 11997 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
                    if (eq->eq_flags & EQ_CHRPAD)
                        esq_sqmods(IImodNOCPAD);
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		}
		break;
	case 1815:
#line 12003 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
                    if (eq->eq_flags & EQ_CHRPAD)
                        esq_sqmods(IImodNOCPAD);
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		}
		break;
	case 1816:
#line 12010 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_if( G_OPEN, IIFSETIO, C_NOTEQ, 0 );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		    gr->gr_func = IIRETFIELD;
		}
		break;
	case 1817:
#line 12018 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1818:
#line 12033 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    er_write(E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1, ERx("GETOPER"));
		    YYERROR;
		}
		break;
	case 1820:
#line 12050 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( 0 );		/* Dummy type right now */
		}
		break;
	case 1821:
#line 12056 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 12067 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 1823:
#line 12079 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IIHELPFILE );
		}
		break;
	case 1824:
#line 12084 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1825:
#line 12096 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( QAMESSAGE );
		}
		break;
	case 1826:
#line 12101 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1827:
#line 12122 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		    frs_close();
		}
		break;
	case 1829:
#line 12131 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    frs_head();
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);

		}
		break;
	case 1830:
#line 12139 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    frs_inqset( FRSinq );
		}
		break;
	case 1831:
#line 12145 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 1832:
#line 12149 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 1833:
#line 12153 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 1834:
#line 12157 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 1835:
#line 12161 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 1836:
#line 12165 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    frs_object( "row" );
		}
		break;
	case 1839:
#line 12173 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    frs_parentname( yypvt[-0].s, T_CHAR, (PTR)0 );
		}
		break;
	case 1840:
#line 12177 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    frs_parentname( yypvt[-0].s, T_CHAR, (PTR)0 );
		}
		break;
	case 1841:
#line 12181 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    frs_parentname( yypvt[-0].s, T_INT, (PTR)0 );
		}
		break;
	case 1842:
#line 12185 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 12208 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    er_write( E_EQ0077_grISUNSUPP, EQ_ERROR, 1,
							ERx("INQUIRE_FRS") );
		    YYERROR;
		}
		break;
	case 1848:
#line 12220 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    frs_iqvar( gr->gr_id, gr->gr_type, gr->gr_sym,
			gr->gr_nlid, gr->gr_nlsym );
		}
		break;
	case 1849:
#line 12226 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    frs_constant( yypvt[-0].s, (char *)0, T_NONE, (SYM *)0 );
		}
		break;
	case 1850:
#line 12230 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    er_write( E_EQ0054_grNOPARENS, EQ_ERROR, 0 );
		    frs_constant( yypvt[-2].s, (char *)0, T_NONE, (SYM *)0 );
		}
		break;
	case 1851:
#line 12235 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Should be an error */
		    frs_constant( yypvt[-3].s, yypvt[-1].s, gr->gr_type, gr->gr_sym );
		}
		break;
	case 1852:
#line 12242 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1853:
#line 12246 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1854:
#line 12250 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1855:
#line 12254 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1856:
#line 12258 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1857:
#line 12262 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1858:
#line 12266 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1859:
#line 12270 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1860:
#line 12274 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1861:
#line 12278 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1862:
#line 12283 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1863:
#line 12289 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1864:
#line 12295 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_INT;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1865:
#line 12301 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1866:
#line 12307 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1867:
#line 12313 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1868:
#line 12319 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1869:
#line 12325 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1870:
#line 12331 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1871:
#line 12337 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1872:
#line 12343 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 12368 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( QAPROMPT );
		}
		break;
	case 1874:
#line 12373 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1875:
#line 12391 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IIMESSAGE ); 
    		}
		break;
	case 1876:
#line 12396 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
            	    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
    	    	    gr->gr_func = IIMESSAGE;
		}
		break;
	case 1879:
#line 12420 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    EQFW3glgen();	
		}
		break;
	case 1880:
#line 12425 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    EQFWopen(gr->gr_func);
		}
		break;
	case 1884:
#line 12441 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    EQFWwcoption(yypvt[-2].s, yypvt[-0].s);
		}
		break;
	case 1885:
#line 12446 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1886:
#line 12450 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1887:
#line 12455 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1888:
#line 12459 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1893:
#line 12484 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		EQFWsuboption(yypvt[-2].s, gr->gr_type, gr->gr_id, (PTR)gr->gr_sym);
	}
		break;
	case 1894:
#line 12489 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
	    	    yyval.s = yypvt[-0].s;
	        }
		break;
	case 1895:
#line 12493 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1896:
#line 12497 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1897:
#line 12503 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_INT;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 1898:
#line 12509 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_CHAR;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 1899:
#line 12515 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_FLOAT;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 1900:
#line 12521 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_PACKASCHAR;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 1901:
#line 12527 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (!gr->gr_sym)
		    {
			gr->gr_type = T_CHAR;
		    }	
		}
		break;
	case 1902:
#line 12534 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_CHAR;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 1903:
#line 12540 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_sym = (SYM *)0;
		    gr->gr_type = T_CHAR;
		    gr->gr_id = yypvt[-0].s;
		}
		break;
	case 1904:
#line 12546 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_sym = (SYM *)0;
		    gr->gr_type = T_CHAR;
		    gr->gr_id = yypvt[-0].s;
		}
		break;
	case 1905:
#line 12560 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IIPRNSCR );
		}
		break;
	case 1906:
#line 12565 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1907:
#line 12570 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Should check that $2.s is "file" */
		    if (STbcompare(yypvt[-3].s, 0, ERx("file"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-3].s,
					    ERx("PRINTSCREEN"), ERx("FILE") );
		}
		break;
	case 1908:
#line 12577 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		}
		break;
	case 1909:
#line 12582 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1910:
#line 12602 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		    gen_call( IINPROMPT );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);

		}
		break;
	case 1911:
#line 12612 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gr->gr_func = IINPROMPT;
		}
		break;
	case 1912:
#line 12618 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (STbcompare(yypvt[-0].s, 0, ERx("noecho"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
						ERx("PROMPT"), ERx("NOECHO") );
		    arg_int_add( 1 );
		}
		break;
	case 1913:
#line 12625 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( 0 );
		}
		break;
	case 1914:
#line 12630 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 12679 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		}
		break;
	case 1916:
#line 12685 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		}
		break;
	case 1917:
#line 12692 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_if( G_OPEN, IIFSETIO, C_NOTEQ, 0 );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		    gr->gr_func = IISETFIELD;
		}
		break;
	case 1918:
#line 12700 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1919:
#line 12712 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gen_call( IIREDISP );
		}
		break;
	case 1920:
#line 12733 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 12759 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1922:
#line 12764 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = (char *)0;	
		}
		break;
	case 1923:
#line 12768 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 12794 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-1].s;	
		    gen_call( IIRESFLD );
		}
		break;
	case 1925:
#line 12799 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-2].s;	
		    gen_call( IIRESCOL );
		}
		break;
	case 1926:
#line 12804 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 12871 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1928:
#line 12886 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    frs_close();
		}
		break;
	case 1930:
#line 12895 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    frs_inqset( FRSset );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		}
		break;
	case 1935:
#line 12914 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    frs_setval( form_sconst(yypvt[-0].s), T_CHAR, (SYM *)0,
			(char *)0, (SYM *)0 );
		}
		break;
	case 1936:
#line 12919 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    frs_setval( yypvt[-0].s, T_INT, (SYM *)0, (char *)0, (SYM *)0 );
		}
		break;
	case 1937:
#line 12923 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    frs_setval(ERx("0"), T_INT, (SYM *)0, (char *)0, (SYM *)0);
		}
		break;
	case 1938:
#line 12928 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    frs_setval(ERx("0"), T_INT, (SYM *)0, (char *)0, (SYM *)0);
		}
		break;
	case 1939:
#line 12933 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    frs_setval( yypvt[-0].s, T_CHAR, (SYM *)0, (char *)0, (SYM *)0 );
		}
		break;
	case 1940:
#line 12937 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    frs_setval( yypvt[-0].s, T_CHAR, (SYM *)0, (char *)0, (SYM *)0 );
		}
		break;
	case 1941:
#line 12941 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 12953 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 12974 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    frs_setval( ERx("1"), T_INT, (SYM *)0, (char *)0,
				(SYM *)0 );
		}
		break;
	case 1944:
#line 12987 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IISLEEP );
		}
		break;
	case 1945:
#line 12992 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1946:
#line 13025 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 13043 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 13059 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_else( G_ELSE, IIRETVAL, C_0, 0, L_ACTELSE, 
			lbl_current(LBL_INTR), L_ACTEND, 
			lbl_current(LBL_FORM) );
		    gr->gr_mode = LBLmSUBMENU;
		}
		break;
	case 1952:
#line 13068 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_mode = LBLmSUBMENU;	/* Not LBLmDISPLAY */
		}
		break;
	case 1953:
#line 13073 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_mode = LBLmSUBMENU;	/* Not LBLmDISPLAY */
		}
		break;
	case 1954:
#line 13077 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_mode = LBLmSUBMENU;	/* Not LBLmDISPLAY */
		}
		break;
	case 1955:
#line 13082 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    act_add( ACT_MENU, lbl_current(LBL_INTR), 0, yypvt[-1].ip );
		    gr->gr_adjective = LBLaMITEM;
		}
		break;
	case 1957:
#line 13101 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 13115 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1959:
#line 13120 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_func = IICHKFRM;
		}
		break;
	case 1960:
#line 13124 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_func = IIVALFLD;
		}
		break;
	case 1961:
#line 13146 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 1962:
#line 13152 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IITCLRCOL;
		}
		break;
	case 1963:
#line 13158 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmCLEARR );
		}
		break;
	case 1965:
#line 13165 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    er_write( E_EQ0054_grNOPARENS, EQ_ERROR, 0 );
		    gen_call( IITCLRROW );
		}
		break;
	case 1966:
#line 13170 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IITCLRROW );
		}
		break;
	case 1967:
#line 13191 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 13202 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 13212 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmDELETER );
		}
		break;
	case 1971:
#line 13219 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_VERSWARN)
			er_write( E_EQ0078_grSTYUNSUPP, EQ_WARN, 1,
							ERx("DELETEROW OUT") );
		}
		break;
	case 1972:
#line 13226 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( FALSE );
		    gen_if( G_OPEN, IITDELROW, C_NOTEQ, 0 );
		}
		break;
	case 1973:
#line 13231 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_VERSWARN)
			er_write( E_EQ0078_grSTYUNSUPP, EQ_WARN, 1,
							ERx("DELETEROW IN") );
		}
		break;
	case 1974:
#line 13238 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( TRUE );	/* In list */
		    gen_if( G_OPEN, IITDELROW, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 1975:
#line 13265 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
                    if (eq->eq_flags & EQ_CHRPAD)
                        esq_sqmods(IImodNOCPAD);
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 1976:
#line 13272 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
                    if (eq->eq_flags & EQ_CHRPAD)
                        esq_sqmods(IImodNOCPAD);
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 1977:
#line 13280 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
                    if (eq->eq_flags & EQ_CHRPAD)
                        esq_sqmods(IImodCPAD);
		    gr->gr_func = IITCOLRET;
		}
		break;
	case 1978:
#line 13288 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmGETR );
		}
		break;
	case 1979:
#line 13306 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IITFILL );
		    gen_if( G_CLOSE, IITBINIT, C_0, 0 );
		}
		break;
	case 1980:
#line 13312 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_if( G_OPEN, IITBINIT, C_NOTEQ, 0 );
		}
		break;
	case 1981:
#line 13317 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1985:
#line 13326 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_call( IITHIDECOL );
		}
		break;
	case 1989:
#line 13343 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{ gr->gr_sym = (SYM *)0; }
		break;
	case 1990:
#line 13345 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 13360 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    id_add( form_sconst(yypvt[-0].s) );
		}
		break;
	case 1993:
#line 13364 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 13381 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    id_add( yypvt[-0].s );
		}
		break;
	case 1995:
#line 13386 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;		/* Paren not stuff present */
		}
		break;
	case 1996:
#line 13390 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    id_add( yypvt[-2].s );
		    id_add( yypvt[-1].s );
		    id_add( yypvt[-0].s );
		    yyval.i = 1;		/* Paren stuff present */
		}
		break;
	case 1997:
#line 13397 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 13411 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;		/* No NULL clause */
		}
		break;
	case 1999:
#line 13415 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 13420 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (yypvt[-0].i == 0)
			id_add( ERx(" not null") );
		    else
			id_add( ERx(" not null with default") );
		    yyval.i = 1;		/* With NULL clause */
		}
		break;
	case 2002:
#line 13433 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 2003:
#line 13437 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 2004:
#line 13452 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 13464 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmPURGE );
		}
		break;
	case 2006:
#line 13503 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 13512 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 13522 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gen_if( G_OPEN, IITINSERT, C_NOTEQ, 0 );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 2009:
#line 13531 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmINSERTR );
		}
		break;
	case 2010:
#line 13566 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IITBCECOLEND );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IITBACT, C_0, 0 );
		}
		break;
	case 2011:
#line 13573 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IITBCECOLEND );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IITBACT, C_0, 0 );
		}
		break;
	case 2012:
#line 13581 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( TRUE );		/* Loading */
		    gen_if( G_OPEN, IITBACT, C_NOTEQ, 0 );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 2013:
#line 13590 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 2016:
#line 13598 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    frs_inqset(FRSload);
		    frs_object(ERx("row"));
		}
		break;
	case 2017:
#line 13624 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IITBCECOLEND );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 2018:
#line 13631 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IITBCECOLEND );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 2019:
#line 13639 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 2020:
#line 13647 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmPUTR );
		}
		break;
	case 2021:
#line 13673 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Close inner and scroll if blocks */
		    gen_if( G_CLOSE, IITSCROLL, C_0, 0 );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 2022:
#line 13680 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( rowNONE );
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		}
		break;
	case 2023:
#line 13686 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmSCROLL );
		}
		break;
	case 2024:
#line 13692 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_if( G_OPEN, IITSCROLL, C_NOTEQ, 0 );
		}
		break;
	case 2025:
#line 13696 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if ((yypvt[-1].i || yypvt[-0].i) && (eq->eq_flags & EQ_VERSWARN))
			er_write( E_EQ0078_grSTYUNSUPP, EQ_WARN, 1,
							ERx("SCROLL IN/OUT") );
		}
		break;
	case 2026:
#line 13703 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gen_call( IITBSMODE );
		    arg_int_add( FALSE );	/* Ignore In flag */
		}
		break;
	case 2028:
#line 13711 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( rowEND );
		}
		break;
	case 2029:
#line 13716 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gen_call( IITBSMODE );
		}
		break;
	case 2030:
#line 13721 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gen_call( IITBSMODE );
		}
		break;
	case 2033:
#line 13730 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_func = IITCOLRET;
		}
		break;
	case 2034:
#line 13735 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( FALSE );
		    arg_int_add( rowNONE );
		    gen_if( G_OPEN, IITSCROLL, C_NOTEQ, 0 );
		}
		break;
	case 2036:
#line 13743 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( TRUE );	/* In list */
		    arg_int_add( rowNONE );
		    gen_if( G_OPEN, IITSCROLL, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 2037:
#line 13765 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 13776 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 13822 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 13833 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		    gen_call( IITBCECOLEND );
		}
		break;
	case 2041:
#line 13839 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		    gen_call( IITBCECOLEND );	
		}
		break;
	case 2042:
#line 13846 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 13863 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 2044:
#line 13884 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 13901 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{	
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLVAL;
		    yyval.s = yypvt[-3].s;
		}
		break;
	case 2046:
#line 13908 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmVALIDR );
		}
		break;
	case 2048:
#line 13915 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    er_write( E_EQ0054_grNOPARENS, EQ_ERROR, 0 );
		    gen_call( IITVALROW );
		}
		break;
	case 2049:
#line 13920 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IITVALROW );
		}
		break;
	case 2050:
#line 13931 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 13950 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 2052:
#line 13954 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 2054:
#line 13970 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    er_write( E_EQ0054_grNOPARENS, EQ_ERROR, 0 );
		}
		break;
	case 2059:
#line 13981 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 2060:
#line 13985 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 14012 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 2062:
#line 14025 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{	
		    arg_int_add( 0 );
		    arg_var_add( gr->gr_null, ERx("null") ); 
		}
		break;
	case 2064:
#line 14043 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    er_write( E_EQ0054_grNOPARENS, EQ_ERROR, 0 );
		}
		break;
	case 2069:
#line 14054 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 2070:
#line 14058 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 14078 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 14091 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( rowCURR );
		}
		break;
	case 2075:
#line 14099 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    if (lbl_gen(LBLmDISPLAY, FALSE) == LBL_NOLEV)
			er_write( E_EQ0056_grFORMNAME, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 2076:
#line 14108 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    er_write( E_EQ0071_grASSGNOP, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 2080:
#line 14129 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = form_sconst(yypvt[-0].s);
		}
		break;
	case 2081:
#line 14135 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 2082:
#line 14141 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( 0 );		/* null indicator */
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 2084:
#line 14154 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 2085:
#line 14158 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 14181 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 14202 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2089:
#line 14206 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    char	ibuf[50];

		    ibuf[0] = '-';
		    ibuf[1] = '\0';
		    yyval.s = str_add( STRNULL, STcat(ibuf, yypvt[-0].s) );
		}
		break;
	case 2090:
#line 14214 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2091:
#line 14221 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_INT, yypvt[-0].s );
		}
		break;
	case 2092:
#line 14227 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( 0 );		/* null indicator */
		    arg_str_add( ARG_INT, yypvt[-0].s );
		}
		break;
	case 2093:
#line 14235 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2094:
#line 14239 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    char	fbuf[50];

		    fbuf[0] = '-';
		    fbuf[1] = '\0';
		    yyval.s = str_add( STRNULL, STcat(fbuf, yypvt[-0].s) );
		}
		break;
	case 2095:
#line 14247 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2096:
#line 14263 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( 0 );		/* null indicator */
		    arg_str_add( ARG_FLOAT, yypvt[-0].s );
		}
		break;
	case 2097:
#line 14271 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2098:
#line 14275 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    char	dbuf[50];

		    dbuf[0] = '-';
		    dbuf[1] = '\0';
		    yyval.s = str_add( STRNULL, STcat(dbuf, yypvt[-0].s) );
		}
		break;
	case 2099:
#line 14283 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2100:
#line 14289 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( 0 );		/* null indicator */
		    arg_str_add( ARG_PACK, yypvt[-0].s );
		}
		break;
	case 2102:
#line 14299 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    arg_int_add( 0 );
		}
		break;
	case 2103:
#line 14304 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    arg_int_add( 0 );
		}
		break;
	case 2104:
#line 14309 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 14334 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 14347 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_type = T_INT;
		}
		break;
	case 2107:
#line 14351 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_type = T_FLOAT;
		}
		break;
	case 2108:
#line 14355 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_type = T_PACKASCHAR;
		}
		break;
	case 2109:
#line 14359 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_type = T_CHAR;
		}
		break;
	case 2110:
#line 14366 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 14446 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 14457 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 14473 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = G4CT_PROC;
		}
		break;
	case 2131:
#line 14477 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = G4CT_FRAME;
		}
		break;
	case 2134:
#line 14488 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 14502 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 14518 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add(ARG_CHAR, yypvt[-2].s);
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 2137:
#line 14523 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add(ARG_CHAR, yypvt[-2].s);
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 2138:
#line 14528 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 14537 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add(ARG_CHAR, ERx(""));
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 2140:
#line 14544 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 14556 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 2142:
#line 14560 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 2144:
#line 14567 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call(IIG4RETVAL);
		}
		break;
	case 2145:
#line 14578 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IIG4CLRARR );
		}
		break;
	case 2146:
#line 14584 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2147:
#line 14595 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IIG4DESCRIBE );
		}
		break;
	case 2148:
#line 14603 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2149:
#line 14623 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
			if (eq->eq_flags & EQ_CHRPAD)
			    esq_sqmods(IImodNOCPAD);
			gen_if( G_CLOSE, IIG4CHKOBJ, C_EQ, 0);
		}
		break;
	case 2150:
#line 14631 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 14656 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2152:
#line 14662 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call(IIG4USEDESCR);
		}
		break;
	case 2154:
#line 14669 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call(IIG4GETATTR);
		}
		break;
	case 2155:
#line 14673 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call(IIG4GETATTR);
		}
		break;
	case 2156:
#line 14684 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 14710 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2158:
#line 14722 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_INT, yypvt[-2].s);
		    arg_str_add( ARG_INT, yypvt[-1].s);
		    gen_call( IIG4GETROW );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		}
		break;
	case 2159:
#line 14732 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		}
		break;
	case 2160:
#line 14740 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 14760 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		}
		break;
	case 2163:
#line 14766 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
                    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		    gr->gr_func = IIINQ4GL;
		}
		break;
	case 2166:
#line 14779 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call(IIINQ4GL);
		}
		break;
	case 2167:
#line 14790 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 14829 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2169:
#line 14835 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 14848 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    INSROW4GL_PARMS *prmptr = (INSROW4GL_PARMS *)(yypvt[-2].ip);

		    set_insrow_param(yypvt[-0].s, gr->gr_id, gr->gr_type, 
				     gr->gr_sym, prmptr);
		    yyval.ip = yypvt[-2].ip;
		}
		break;
	case 2171:
#line 14858 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 2172:
#line 14870 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IIG4REMROW );
		}
		break;
	case 2173:
#line 14876 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2174:
#line 14899 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IIG4CALLCOMP );
		    gen_if( G_CLOSE, IISENDUSEREVENT, C_EQ, 0);
		}
		break;
	case 2175:
#line 14906 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
                    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-1].s, NULL );
		    gen_if( G_OPEN, IISENDUSEREVENT, C_EQ, 0);
		}
		break;
	case 2179:
#line 14920 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add(ARG_CHAR, yypvt[-2].s);
		    args_toend(1, 2);
		    gen_call(IIG4VALPARAM);
		}
		break;
	case 2180:
#line 14934 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		}
		break;
	case 2181:
#line 14941 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
                    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		    gr->gr_func = IISET4GL;
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		}
		break;
	case 2184:
#line 14954 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call(IISET4GL);
		}
		break;
	case 2185:
#line 14974 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
			gen_if( G_CLOSE, IIG4CHKOBJ, C_EQ, 0);
		}
		break;
	case 2186:
#line 14982 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 15008 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2188:
#line 15013 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call(IIG4USEDESCR);
		}
		break;
	case 2190:
#line 15020 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call(IIG4SETATTR);
		}
		break;
	case 2191:
#line 15024 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call(IIG4SETATTR);
		}
		break;
	case 2192:
#line 15035 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 15048 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		}
		break;
	case 2194:
#line 15061 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IIG4SETROW );
		}
		break;
	case 2195:
#line 15067 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2196:
#line 15073 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (STbcompare(ERx("row"), 0, yypvt[-3].s, 0, TRUE) != 0)
		    {
			    er_write( E_EQ0081_TL_ATTR_UNKNOWN, EQ_ERROR, 1,
								yypvt[-3].s);
		    }
		}
		break;
	case 2197:
#line 15088 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_call( IIG4DELROW );
		}
		break;
	case 2198:
#line 15094 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2199:
#line 15110 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (STbcompare(ERx("rownumber"), 0, yypvt[-3].s, 0, TRUE) != 0)
		    {
			    er_write( E_EQ0081_TL_ATTR_UNKNOWN, EQ_ERROR, 1,
								yypvt[-3].s);
		    }
		}
		break;
	case 2200:
#line 15121 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_str_add( ARG_INT, yypvt[-0].s);
		}
		break;
	case 2201:
#line 15128 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2202:
#line 15132 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2203:
#line 15139 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add(0);	/* Null indicator */
		    arg_var_add( gr->gr_sym, gr->gr_id );
		}
		break;
	case 2204:
#line 15147 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 15162 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 2206:
#line 15166 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    arg_int_add(0);
		    yyval.i = 0;
		}
		break;
	case 2208:
#line 15178 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = NULL;
		}
		break;
	case 2209:
#line 15182 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 15194 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		}
		break;
	case 2211:
#line 15197 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_type = T_INT;
		    gr->gr_id = yypvt[-0].s;
		    gr->gr_sym = NULL;
		}
		break;
	case 2212:
#line 15203 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_type = T_FLOAT;
		    gr->gr_id = yypvt[-0].s;
		    gr->gr_sym = NULL;
		}
		break;
	case 2213:
#line 15209 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_type = T_PACKASCHAR;
		    gr->gr_id = yypvt[-0].s;
		    gr->gr_sym = NULL;
		}
		break;
	case 2214:
#line 15215 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_type = T_CHAR;
		    gr->gr_id = yypvt[-0].s;
		    gr->gr_sym = NULL;
		}
		break;
	case 2215:
#line 15224 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 15250 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    arg_int_add( 0 );
		}
		break;
	case 2220:
#line 15255 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    arg_int_add( 0 );
		}
		break;
	case 2221:
#line 15260 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 15280 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
#line 15293 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_type = T_INT;
		}
		break;
	case 2224:
#line 15297 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_type = T_FLOAT;
		}
		break;
	case 2225:
#line 15301 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_type = T_PACKASCHAR;
		}
		break;
	case 2226:
#line 15305 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->gr_type = T_CHAR;
		}
		break;
	case 2227:
#line 15312 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    i4  code;

		    code = g4_check(gr->gr_func, yypvt[-1].s, (bool)yypvt[-0].i);
		    arg_int_add(code);
		}
		break;
	case 2228:
#line 15321 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2229:
#line 15325 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2230:
#line 15366 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    sc_moresql();	
		    dml->dm_exec = DML_HOST;
		}
		break;
	case 2232:
#line 15380 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if ((gr->gr_flag & GR_HOSTCODE) == 0)
		    {
			gen_line( ERx("host code") );
			gr->gr_flag |= GR_HOSTCODE;
		    }
		    gen_host( G_H_CODE, yypvt[-0].s );
		
#if 0
/*
** Based on client experience with multi-tasked embedded Ada, include
** the TLS-based sqlca triggered by "EXEC SQL BEGIN DECLARE SECTION;"
** rather than on task body definition. This may be revisited, so leave
** (dormant) code in place to include sqlca based on "TASK BODY".
*/
		    if (eq->eq_flags & EQ_ATASKBODY)
		    {
			eq->eq_flags &= ~EQ_ATASKBODY;
			gen_host(G_H_OP|G_H_NEWLINE,
			    ERx("sqlca : IISQLCA; for sqlca use at II_sqlca;"));
		    }
#endif
		    dml->dm_exec = DML_HOST;
		}
		break;
	case 2233:
#line 15412 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2234:
#line 15416 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2235:
#line 15420 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2236:
#line 15434 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2237:
#line 15444 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    STprintf(esq->sbuf, ERx("!%s"), yypvt[-0].s);
		    esqlca(yypvt[-2].i, sqcaGOTO, esq->sbuf);
		    yyval.i = 1;
		}
		break;
	case 2238:
#line 15455 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 2239:
#line 15459 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 2240:
#line 15464 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			STprintf( esq->sbuf, ERx("%s %s"), yypvt[-0].s,
			    ERx("PRIVILEGE") );
		        er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
			    2, esq->sbuf, ERx("GRANT") );
		    }
		    db_key(yypvt[-0].s);
		}
		break;
	case 2241:
#line 15475 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 2243:
#line 15488 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (dml->dm_exec != (DML_EXEC|DML__SQL))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"),
							ERx("BEGIN DECLARE") );
		    dml->dm_exec = DML_DECL;
		    eq->eq_flags |= EQ_INDECL;	/* Scanner in declare mode */
		    if (eq->eq_flags & EQ_MULTI)
			gen_host(G_H_OP|G_H_NEWLINE,
			    ERx("sqlca : IISQLCA; for sqlca use at II_sqlca;"));
		}
		break;
	case 2244:
#line 15500 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (dml->dm_exec != (DML_EXEC|DML__SQL))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"),
							ERx("END DECLARE") );
		    dml->dm_exec = DML_HOST;
		    eq->eq_flags &= ~EQ_INDECL;	/* Scanner exits declare mode */
		}
		break;
	case 2248:
#line 15514 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 2249:
#line 15518 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 2252:
#line 15525 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    dml->dm_exec = DML_DECL;
		    gen_host(G_H_OP|G_H_NEWLINE, ERx("...DECLARATION ERROR;"));
		}
		break;
	case 2263:
#line 15570 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
	case 2264:
#line 15611 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    SYM	*sy;

		    sy = ADA_DECLARE(yypvt[-5].i, (yypvt[-3].i ? syFisCONST:syFisVAR), yypvt[-2].v);
		    if (sy)
		    {
			if ((i4)sym_g_vlue(sy) & AvalCHAR)
			    sym_s_dims( sy, 0 );	/* not an array */
			if (yypvt[-3].i && !yypvt[-1].i)
			    er_write( E_E60009_haNOTINITED, EQ_ERROR, 1,
							    sym_str_name(sy) );
		    }
		}
		break;
	case 2265:
#line 15627 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 2266:
#line 15631 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-2].i + 1;
		}
		break;
	case 2267:
#line 15637 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    ada_push( yypvt[-0].s );
		}
		break;
	case 2268:
#line 15643 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 2269:
#line 15647 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 2270:
#line 15651 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 2272:
#line 15664 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
	case 2273:
#line 15685 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2279:
#line 15700 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (yypvt[-1].i > 0 && yypvt[-1].i <= 6)
			yyval.v = gr->A_fltsy;
		    else if (yypvt[-1].i >= 7 && yypvt[-1].i <= 16)
			yyval.v = gr->A_dblsy;
		    else
		    {
			er_write( E_E60005_haDIGITS, EQ_ERROR, 1, er_na(yypvt[-1].i) );
			yyval.v = gr->A_fltsy;
		    }
		}
		break;
	case 2283:
#line 15724 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    sc_eat( gen_code, SC_SEEN|SC_NEST|SC_STRIP, ERx(")"),
								    '(', ')' );
		    gen_host( G_H_OP, ERx(")") );
		}
		break;
	case 2285:
#line 15737 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->A_dims++;
		}
		break;
	case 2286:
#line 15743 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.v = yypvt[-0].v;
		}
		break;
	case 2287:
#line 15749 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 2288:
#line 15753 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 2289:
#line 15759 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    sc_eat( gen_code, SC_BACK, ERx(";"), '(', ')' );
		}
		break;
	case 2290:
#line 15774 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    ADA_DECLARE( yypvt[-4].i, syFisCONST, gr->A_intsy );
		}
		break;
	case 2295:
#line 15797 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
	case 2298:
#line 15825 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.v = (SYM *)0;	/* flag as enum -- already declared */
		}
		break;
	case 2306:
#line 15844 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
	case 2307:
#line 15857 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		  /* Uses $0.v from Aenum_begin */
		    gr->A_type = T_INT;
		    /*
		    ** Each enumeration is a name space unto itself.
		    */
		    gr->A_rec++;
		    if (yypvt[-1].i > 0)
			ADA_DECLARE( yypvt[-1].i, syFisCONST, yypvt[-2].v );
		    gr->A_rec--;
		}
		break;
	case 2309:
#line 15873 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = yypvt[-2].i + yypvt[-0].i;
		}
		break;
	case 2310:
#line 15879 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 2311:
#line 15883 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 2312:
#line 15893 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
	case 2313:
#line 15913 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    i4		m, n;

		    m = ada_sizeof( yypvt[-2].i );
		    n = ada_sizeof( yypvt[-0].i );
		    yyval.i = max( m, n );
		}
		break;
	case 2314:
#line 15928 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.v = yypvt[-0].v;
		}
		break;
	case 2315:
#line 15932 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.v = gr->A_fltsy;
		    er_write( E_E60013_haTYPUNSUPP, EQ_ERROR, 1, ERx("FIXED") );
		}
		break;
	case 2319:
#line 15959 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->A_rec--;
		    gr->A_type = T_STRUCT;
		    gen_host( G_H_KEY|G_H_OUTDENT, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		  /* { $$.v = $1.v; } */
		}
		break;
	case 2320:
#line 15969 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    register SYM	*sy;

		    sy = yyval.v = symDcEuc( ERx(""), gr->A_rec, A_BLOCK, syFisTAG,
					A_CLOSURE, SY_TAG );
		    if (sy)
			sym_s_btype( sy, T_STRUCT );
		    gr->A_rec++;
		    gr->A_val = 0;
		}
		break;
	case 2329:
#line 16003 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    ADA_DECLARE( yypvt[-4].i, syFisVAR, yypvt[-2].v );
		}
		break;
	case 2330:
#line 16010 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_NEWLINE|G_H_INDENT, NULL );
		}
		break;
	case 2331:
#line 16017 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_OUTDENT, NULL );
		}
		break;
	case 2342:
#line 16052 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.v = yypvt[-0].v;
		    gr->A_val |= AvalACCESS;
		}
		break;
	case 2343:
#line 16064 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.v = yypvt[-0].v;
		}
		break;
	case 2344:
#line 16074 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    register SYM	*sy;

		    sy = ADA_DECLARE( 1, syFisTYPE|syFisFORWARD, (SYM *)0 );
		    sym_s_btype( sy, T_FORWARD );
		}
		break;
	case 2345:
#line 16085 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    register SYM	*sy;

		    sy = ADA_DECLARE( 1, syFisTYPE|syFisFORWARD, (SYM *)0 );
		    sym_s_btype( sy, T_FORWARD );
		}
		break;
	case 2348:
#line 16102 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    ADA_DECLARE( 1, syFisTYPE, yypvt[-1].v );
		}
		break;
	case 2354:
#line 16126 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 2355:
#line 16130 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 2362:
#line 16156 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2366:
#line 16177 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->A_val = 0;		/* not really needed */
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 2367:
#line 16184 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    ADA_DECLARE( yypvt[-4].i, (yypvt[-2].i ? syFisCONST:syFisVAR), yypvt[-1].v );
		}
		break;
	case 2368:
#line 16190 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;		/* default is IN */
		}
		break;
	case 2369:
#line 16194 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 2370:
#line 16198 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 2371:
#line 16202 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 2373:
#line 16208 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    sc_eat( gen_code, SC_BACK, ERx(");"), '(', ')' );
		}
		break;
	case 2374:
#line 16232 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
				er_write( E_E6000F_haRNMCOMPAT, EQ_ERROR, 0 );
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
		}
		break;
	case 2375:
#line 16275 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    SYM		*sy;
		    i4		stat;
		    char	*name;

		    stat = symRsEuc( &sy, A_CLOSURE, syFisVAR|syFisCONST );
		    name = str_add( STRNULL, id_getname() );
		    id_free();
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
	case 2376:
#line 16304 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    sym_f_init();
		    gr->A_dims = 0;
		}
		break;
	case 2380:
#line 16316 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->A_dims = 1;
		    sc_eat( id_add, SC_SEEN|SC_NEST, ERx(")"), '(', ')' );
		}
		break;
	case 2381:
#line 16323 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    id_add( yypvt[-0].s );
		    gr->A_dims = 0;
		}
		break;
	case 2382:
#line 16330 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    id_add( yypvt[-0].s );
		    sym_fpush( sym_find(yypvt[-0].s) );
		}
		break;
	case 2384:
#line 16345 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    sc_eat( gen_code, SC_BACK, ERx(";"), '(', ')' );
		    er_write( E_E60016_haUNSUPP, EQ_ERROR, 1, ERx("FOR") );
		}
		break;
	case 2385:
#line 16355 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->A_type = T_INT;
		    yyval.v = gr->A_intsy;
		}
		break;
	case 2386:
#line 16360 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->A_type = T_FLOAT;
		    yyval.v = gr->A_fltsy;
		}
		break;
	case 2387:
#line 16365 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->A_type = T_CHAR;
		    yyval.v = gr->A_chrsy;
		}
		break;
	case 2388:
#line 16370 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->A_type = T_INT;
		    yyval.v = gr->A_bytesy;
		}
		break;
	case 2389:
#line 16377 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    char	buf[4];

		    buf[0] = '\'';
		    buf[1] = *(yypvt[-0].s);
		    buf[2] = '\'';
		    buf[3] = '\0';
		    gen_host( G_H_OP, buf );
		}
		break;
	case 2390:
#line 16389 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    i4		i = 0;

		    gen_host( G_H_KEY, yypvt[-0].s );
		    CVan( yypvt[-0].s, &i );
		    yyval.i = i;
		}
		break;
	case 2391:
#line 16397 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    i4		i = 0;

		    gen_host( G_H_OP, ERx("-") );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    CVan( yypvt[-0].s, &i );
		    yyval.i = -i;
		}
		break;
	case 2392:
#line 16408 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2393:
#line 16412 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_OP, ERx("-") );
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2394:
#line 16417 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2395:
#line 16421 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_OP, ERx("-") );
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2396:
#line 16432 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* 
		    ** Strip to end of line, but ensure scanner is synchronized
		    ** via 'lex_need_input' - see the yylex routines.  This is
		    ** because the newline is the EXEC trigger.
		    */
		    sc_reset();
		    if (inc_parse_name(yypvt[-1].s, FALSE) == SC_INC)
		    {
			/* Within declarations INCLUDE files are INLINE */
			inc_inline();
			inc_push_file();
		    }
		}
		break;
	case 2397:
#line 16447 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Better be "<EOF>" else Yacc will produce syntax error */
		    dml->dm_exec = DML_DECL;
		}
		break;
	case 2398:
#line 16453 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (dml->dm_exec != (DML_EXEC|DML__SQL))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"),
							    ERx("INCLUDE") );
		    dml->dm_exec = DML_DECL;
		}
		break;
	case 2399:
#line 16461 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2400:
#line 16465 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2401:
#line 16469 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    STprintf( esq->sbuf, ERx("%s.%s"), yypvt[-1].s, yypvt[-0].s );
		    yyval.s = esq->sbuf;
		}
		break;
	case 2402:
#line 16474 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    STprintf( esq->sbuf, ERx("%s.%s"), yypvt[-2].s, yypvt[-0].s );
		    yyval.s = esq->sbuf;
		}
		break;
	case 2403:
#line 16486 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    dml->dm_exec = DML_DECL;
		}
		break;
	case 2404:
#line 16491 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    esq->flag |= ESQ_NOPRT;     /* Suppress printing */
		    if (dml->dm_exec != (DML_EXEC|DML__SQL))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"),
							ERx("DECLARE TABLE") );
		}
		break;
	case 2405:
#line 16498 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    esq->flag |= ESQ_NOPRT;     /* Suppress printing */
		    if (dml->dm_exec != (DML_EXEC|DML__SQL))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"),
							ERx("DECLARE TABLE") );
		}
		break;
	case 2412:
#line 16520 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2413:
#line 16525 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2414:
#line 16530 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 2415:
#line 16535 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 2416:
#line 16540 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2417:
#line 16545 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 2418:
#line 16550 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 2419:
#line 16555 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2420:
#line 16560 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2421:
#line 16565 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2422:
#line 16570 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 2423:
#line 16575 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY|G_H_OUTDENT|G_H_INDENT, yypvt[-0].s );
		}
		break;
	case 2424:
#line 16580 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2425:
#line 16585 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2426:
#line 16590 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2427:
#line 16595 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2428:
#line 16600 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 2429:
#line 16605 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2430:
#line 16610 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2431:
#line 16615 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2432:
#line 16620 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2433:
#line 16625 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2434:
#line 16630 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2435:
#line 16635 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2436:
#line 16640 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 2437:
#line 16645 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2438:
#line 16650 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host(G_H_KEY|G_H_INDENT|G_H_OUTDENT|G_H_NEWLINE, yypvt[-0].s);
		}
		break;
	case 2439:
#line 16655 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2440:
#line 16660 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2441:
#line 16665 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY|G_H_INDENT|G_H_NEWLINE, yypvt[-0].s );
		}
		break;
	case 2442:
#line 16670 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2443:
#line 16675 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2444:
#line 16680 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 2445:
#line 16685 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->A_val = 0;
		    gen_host( G_H_OP|G_H_NEWLINE, yypvt[-0].s );
		}
		break;
	case 2446:
#line 16691 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_SCONST, yypvt[-0].s );
		}
		break;
	case 2447:
#line 16696 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2448:
#line 16702 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2449:
#line 16707 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2450:
#line 16712 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2451:
#line 16742 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    sc_hostvar = TRUE;	/* disable keyword lookups */
		}
		break;
	case 2453:
#line 16749 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    er_write( E_EQ0129_sqVARCOLON, EQ_ERROR, 0 );
		}
		break;
	case 2454:
#line 16760 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Must be a real variable - not constant or enum */
		    if (yypvt[-0].i)
			ADA_ERR( (E_E6000A_haNOTVAR, EQ_ERROR, 1, gr->gr_id) );
		}
		break;
	case 2455:
#line 16774 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Must be a real variable - not constant or enum */
		    if (yypvt[-0].i)
			ADA_ERR( (E_E6000A_haNOTVAR, EQ_ERROR, 1, gr->gr_id) );
		}
		break;
	case 2458:
#line 16812 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    SYM		*sy;

		    yyval.i = 0;
		    gr->gr_type = T_UNDEF;		/* Default */
		    gr->gr_id = str_add( STRNULL, id_getname() );
		    id_free();
		    if ((sy = yypvt[-0].v) == (SYM *)0)
		    {
			/* Do nothing - Error already printed */
		    } else	/* Known to EQUEL -- check usage */
		    {
			/* May be variable, constant or enum - send upstairs */
			yyval.i = (sym_g_useof(sy) & syFisVAR) == 0;
			if (sym_g_btype(sy) == T_STRUCT)
			{
			    /* Cannot be a structure */
			    ADA_ERR( (E_E60014_haVARRECORD, EQ_ERROR, 1,
								gr->gr_id) );
			} else
			    gr->gr_type = sym_g_btype(sy);
		    }
		    gr->gr_sym = sy;
		}
		break;
	case 2459:
#line 16842 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    sc_hostvar = FALSE;	/* re-enable keyword lookups */
		}
		break;
	case 2460:
#line 16846 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    sc_hostvar = FALSE;	/* re-enable keyword lookups */
		    sc_popscptr();	/* move SC_PTR back one word */
		}
		break;
	case 2461:
#line 16852 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    STprintf(esq->sbuf, ADA_SQLDA_ARG, id_getname());
		    gr->gr_id = str_add(STRNULL, esq->sbuf);
		    id_free();
		    sym_f_init(); 	/* Discard sym stuff - ignored */
		}
		break;
	case 2462:
#line 16860 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->A_err = 1;	/* No errors on SQLDA */
		    sc_hostvar = TRUE;	/* disable keyword lookups */
		}
		break;
	case 2463:
#line 16865 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->A_err = 1;	/* No errors on SQLDA */
		}
		break;
	case 2464:
#line 16881 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
			    STprintf(esq->sbuf, ERx("%s.%s"), sid, 
				     sym_str_name(mem));
			    mid = str_add( STRNULL, esq->sbuf );
			    if ((sym_g_useof(mem) & syFisVAR) == 0)
				ADA_ERR((E_E6000A_haNOTVAR, EQ_ERROR, 1, mid));
			    else if (sym_g_btype(mem) == T_STRUCT)
				ADA_ERR( (E_E60014_haVARRECORD, EQ_ERROR, 1,
									mid) );
			    else if (sym_g_dims(mem))
				ADA_ERR( (E_E60001_haMUSTINDIR, EQ_ERROR, 1,
									mid) );
			    erec_mem_add( mid, mem, sym_g_btype(mem) );
			}
		    }
		    else
		    {
			if ((sym_g_useof(sy) & syFisVAR) == 0)
			    ADA_ERR( (E_E6000A_haNOTVAR, EQ_ERROR, 1, sid) );
			erec_mem_add( sid, sy, sym_g_btype(sy) );
		    }
		    id_free();
		}
		break;
	case 2465:
#line 16927 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
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
			    STprintf(esq->sbuf, ERx("%s.%s"), sid, 
				     sym_str_name(mem));
			    mid = str_add( STRNULL, esq->sbuf );
			    if (sym_g_btype(mem) == T_STRUCT)
				ADA_ERR( (E_E60014_haVARRECORD, EQ_ERROR, 1,
									mid) );
			    else if (sym_g_indir(mem))
				ADA_ERR( (E_E60001_haMUSTINDIR, EQ_ERROR, 1,
									mid) );
			    erec_mem_add( mid, mem, sym_g_btype(mem) );
			}
		    }
		    else
			erec_mem_add( sid, sy, sym_g_btype(sy) );
		    id_free();
		}
		break;
	case 2466:
#line 16960 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    gr->A_simpvar = TRUE;
		    gr->A_err = 0;
		    gr->A_dims = 0;
		    sym_f_init();
		}
		break;
	case 2467:
#line 16979 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    sc_hostvar = FALSE;	/* re-enable keyword lookups */
		}
		break;
	case 2468:
#line 16983 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    sc_hostvar = FALSE;	/* re-enable keyword lookups */
		    sc_popscptr();	/* move SC_PTR back a word */
		}
		break;
	case 2469:
#line 16989 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    SYM		*sy = yypvt[-0].v;

		    if (sy == (SYM *)0)
		    {
			/* Not a var - only enter as undefined if simpvar */
			ADA_ERR((E_E60012_haUNDEC, EQ_ERROR, 1, id_getname()));
			if (gr->A_simpvar)
			{
			    /* hint for ESQL redeclaration errors */
			    sym_hint_type( (SYM *)0, T_UNDEF, 0 );
			    sy = symDcEuc( id_getname(), 0, A_BLOCK,
					syFisVAR, A_CLOSURE, SY_NORMAL );
			    if (sy)
				sym_s_btype( sy, T_UNDEF );
			}
		    }
		    else
		    {
			_VOID_ symExtType(sy);	/* It might be a forward ref */
			if (erec_vars() > 0)	/* Indicator array ? */
			{
			    /*
			    ** If we are an indicator variable for a var which
			    ** could be a struct (currently SELECT, FETCH, or
			    ** INSERT) then list the indicators.
			    */
			    if (sym_g_dims(sy) && gr->A_dims == 0)
			    {
				erec_ind_add(A_ARR_BASE, id_getname(),
					A_ARR_EXPR, sy );
			    }
			    else 
			    {
			        /* No var dims or already subscripted by user */
				erec_ind_add(A_ARR_BASE, id_getname(),
					(char *)0, sy );
			    }
			}
		    }
		    yyval.v = sy;
		}
		break;
	case 2470:
#line 17038 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.v = yypvt[-0].v;
		}
		break;
	case 2471:
#line 17042 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.v = yypvt[-0].v;
		}
		break;
	case 2472:
#line 17047 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.v = yypvt[-1].v;
		}
		break;
	case 2473:
#line 17051 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    yyval.v = yypvt[-1].v;
		}
		break;
	case 2474:
#line 17064 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    SYM		*sy;

		    id_add( yypvt[-0].s );
		    sym_fpush( sym_find(yypvt[-0].s) );
		    if (symRsEuc(&sy, A_CLOSURE, syFisVAR|syFisCONST) != syL_OK)
			sy = (SYM *) 0;
		    if (sy)	/* It might have been a forward reference */
			_VOID_ symExtType( sy );
		    yyval.v = sy;
		}
		break;
	case 2475:
#line 17078 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    SYM		*sy;

		    id_add( yypvt[-1].s );
		    id_add( yypvt[-0].s );
		    sym_fpush( sym_find(yypvt[-0].s) );
		    if (symRsEuc(&sy, A_CLOSURE, syFisVAR|syFisCONST) != syL_OK)
		    {
			sy = (SYM *) 0;
			ADA_ERR( (E_E6000D_haRECORDREF, EQ_ERROR, 1,
								id_getname()) );
		    }
		    if (sy)	/* It might have been a forward reference */
			_VOID_ symExtType( sy );
		    gr->A_simpvar = FALSE;
		    yyval.v = sy;
		}
		break;
	case 2476:
#line 17098 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    SYM		*sy;
		    i4		stat;

		    gr->A_simpvar = FALSE;
		    stat = symRsEuc( &sy, A_CLOSURE, syFisVAR|syFisCONST );

		    /*
		    ** Inspect symbol, and its children if it is a record,
		    ** or its type if it is an access type to a scalar type.
		    */
		    if (stat != syL_OK)
		    {
			if (sy == (SYM *)0)
			    ADA_ERR( (E_E6000B_haBADRECALL, EQ_ERROR, 1,
								id_getname()) );
			else
			    ADA_ERR( (E_E6000D_haRECORDREF, EQ_ERROR, 1,
								id_getname()) );
			sy = (SYM *)0;
		    } else if (sym_g_btype(sy) == T_STRUCT)
		    {
		       ADA_ERR( (E_E6000C_haSCALRECALL, EQ_ERROR, 1,
								id_getname()) );
		    } else
		    {
			/*
			** Must be a scalar access type:
			** An access type was used, without a record as its
			** final type. Eg: type ip is access INTEGER;
			**		   I: ip;
			**		   sleep I.ALL
			*/
		    }
		    id_add( yypvt[-1].s );
		    id_add( yypvt[-0].s );
		    yyval.v = sy;
		}
		break;
	case 2477:
#line 17144 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    if (yypvt[-1].v)    /* Variable found - are these our parens */
		    {
			/*
			** If array set eat parens. Otherwise if a record eat
			** parens as rec cannot be last in reference.
			*/
			if (sym_g_dims(yypvt[-1].v))	
			{
			    gr->A_simpvar = FALSE;
			    gr->A_dims = 1;
			    sc_eat(id_add, SC_SEEN|SC_NEST, ERx(")"), '(', ')');
			} else if (sym_g_btype(yypvt[-1].v) == T_STRUCT)
			{
			    gr->A_simpvar = FALSE;
			    sc_eat(id_add, SC_SEEN|SC_NEST, ERx(")"), '(', ')');
			    ADA_ERR( (E_E60002_haCANTINDIR, EQ_ERROR, 1,
								id_getname()) );
			} else
			    SC_PTR--;
		    } else
			SC_PTR--;
		}
		break;
	case 2478:
#line 17168 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/adasq/adasq.yf"
{
		    /* Check to see dimensions needed on last element */
		    if (yypvt[-0].v  && sym_g_dims(yypvt[-0].v) && erec_vars() == 0)
			ADA_ERR( (E_E60001_haMUSTINDIR, EQ_ERROR, 1,
								id_getname()) );
		}
		break;
		}
	return 0; /* Success */

	}
