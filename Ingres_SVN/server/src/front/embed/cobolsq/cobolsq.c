
#line 6 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
/*
** Copyright (c) 2004, 2008 Ingres Corporation
*/

/* %L language - Which language are we using for symbol table? */
/* %L merge <esqlgram.my> -- get the master grammar */
/* %L fake begin */
/* %L fake end */
/* %L language begin - Must define EQ_PL1_LANG or EQ_EUC_LANG for eqsym.h */
# define	EQ_PL1_LANG
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
#define	tSEMICOL		589
#define	tCBTYPE		590
#define	tCBDIREC		591
#define	tCBBLANK		592
#define	tCBCHARACTER		593
#define	tCBDEPENDING		594
#define	tCBFILLER		595
#define	tCBINDEXED		596
#define	tCBJUSTIFIED		597
#define	tCBOCCURS		598
#define	tCBPICTURE		599
#define	tCBREDEFINES		600
#define	tCBSEPARATE		601
#define	tCBSIGN		602
#define	tCBSYNC		603
#define	tCBTIMES		604
#define	tCBLEAD_TRAIL		605
#define	tCBUSAGE		606
#define	tCBVALUE		607
#define	tCBZERO		608
#define	tCBBADTYPE		609
#define	tCBEXTERNAL		610
#define	tCBREFERENCE		611
#define	tCBEJECT		612
#define	tCBSKIP1		613
#define	tCBSKIP2		614
#define	tCBSKIP3		615
#define	tOR		616
#define	tAND		617
#define	tLT		618
#define	tGT		619
#define	tNE		620
#define	tLE		621
#define	tGE		622
#define	tEQOP		623
#define	tPLUS		624
#define	tMINUS		625
#define	tSTAR		626
#define	tSLASH		627
#define	tEXP		628
#define	tUNARYOP		629

#line 986 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
	

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


#line 1067 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
/* %L locals - Include L locals and global gr structure */
/* %L locals begin - Local variables and gr structure for COBOL */

# include	<eqcob.h>
# include	<ere4.h>

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

	/* Add L (COBOL) dependent members after this statement */
	i4	Cb_rec;		/* COBOL record level (increments of 1) */
	i4	Cb_print;	/* Do we need to print a COBOL terminator */
	i4     Cb_dims;	/* Was variable used as an array? */
	i4	Cb_err;		/* Have we given err msg re use of this var? */
	COB_SYM	Cb_sym;		/* Declaration information */
};
GLOBALDEF struct gr_state _gr_local ZERO_FILL;
GLOBALDEF struct gr_state *gr = &_gr_local;
struct gr_state tempgr;

SYM		*gr_resolve_cob();

/* Where do COBOL array indices start? */
# define C_ARR_BASE	1

/* How do you index a COBOL array? */
# define C_ARR_EXPR	ERx("(%d)")

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

#line 16642 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"


/* %L mechanism */
/* %L mechanism begin - All the stuff to do with gr_mechanism() */

GLOBALDEF GR_TYPES   gr_typetab[] = {
		    /* Name *	Type		Length */
		    (char *)0,  0, 		0
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
PTR	arg1, arg2, arg3;	/* Extra arguments for particular cases */
{
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
	eq->eq_lang = EQ_COBOL;

	if (!einit)
	{
	    eq->eq_in_ext = ERx("scb");		/* Include files */
# ifdef NT_GENERIC
          eq->eq_out_ext = ERx("cbl");        /* added JJS */
# endif
# ifdef VMS
	    eq->eq_out_ext = ERx("lib");
# endif 
# ifdef UNIX
	    eq->eq_out_ext = ERx("cbl");
# endif 
# ifdef CMS
	    eq->eq_out_ext = ERx("cobol");
# endif 
# ifdef DGC_AOS
	    eq->eq_out_ext = ERx("cob");
# endif 
# ifdef hp9_mpe
	    eq->eq_out_ext = ERx("cob");
# endif
	    einit = 1;
	}

	eq->eq_def_in = ERx("scb");		/* Main source files */
# ifdef NT_GENERIC
        eq->eq_def_out = ERx("cbl"); 
# endif
# ifdef VMS
	eq->eq_def_out = ERx("cob");
# endif 
# ifdef UNIX
	eq->eq_def_out = ERx("cbl");
# endif 
# ifdef CMS
	eq->eq_def_out = ERx("cobol");
# endif 
# ifdef DGC_AOS
	eq->eq_def_out = ERx("cob");
# endif 
# ifdef hp9_mpe
	eq->eq_def_out = ERx("cob");
# endif

      /* tell the world we're Embedded SQL! */
	dml->dm_lang = DML_ESQL;
	dml->dm_exec = DML_HOST;
	dml->dm_gentab = gen__sqltab;
	dml->dm_lex = yyesqlex;
	dml->dm_strcontin = scStrIsCont;
	dml->dm_iscomment = sc_iscomment;
	dml->dm_repeat = esq_repeat; 	/* Provide hook for REPEATED queries */

	eq->eq_sql_quote = '\'';
	eq->eq_quote_esc = '"';
	eq->eq_host_quote = '"';

	gr->gr_type = T_NONE;
	gr->gr_sym = (SYM *)0;
	gr->gr_id = ERx("");
	gr->gr_flag = GR_HOSTCODE;

	gr->Cb_rec = 0;
	gr->Cb_print = 1;
	gr->Cb_dims = 0;
	gr->Cb_err = 0;
	gr->Cb_sym.cs_lev = (i1)1;
	gr->Cb_sym.cs_type = (i1)COB_NOTYPE;
	gr->Cb_sym.cs_nlen = (i1)0;
	gr->Cb_sym.cs_nscale = (i1)0;

	ecs_clear();
	esq->inc = 0;
	break;

      case GR_SYMINIT:
	{
	    /* Enter declarations into EQUEL symbol table */
	    extern	i4	(*sym_delval)();
	    extern	i4	(*sym_prtval)();
	    SYM			*sy;

	    sym_init( TRUE );
	    sym_delval = cob_clrsym;		/* Clr routine for COB_SYM */
	    sym_prtval = cob_prtsym;		/* Prt routine for COB_SYM */

	    /* Declare "generic null" as a symbol */
	    sy = symDcPL1(ERx(".null"), 0, 1, syFisVAR|syFisSYS, COBCLOSURE, 0);
	    sym_s_btype( sy, T_NUL );
	    sym_s_dsize( sy, sizeof(i4) );
	    gr->gr_null = sy;
	}
	break;

      case GR_LOOKUP:
	/* How should we look up this name? Unused mechanism, return tNAME */
	*(i4 *)arg2 = tNAME;
	break;

      case GR_NUMBER:
	/* No need to do anything for ESQL, return tINTCONST */
	*(i4 *)arg1 = tINTCONST;
	break;

      case GR_STMTFREE:
	str_reset();
	gr->gr_type = T_NONE;
	gr->gr_sym = (SYM *)0;
	esq->flag = 0;
	break;

      case GR_DUMP:
	{
	    register FILE	*f = eq->eq_dumpfile;
	    register COB_SYM	*cs = &gr->Cb_sym;
	    register i4	typ;
	    static char		*cb_type_names[] = {
		ERx("COMP"),			/* COB_COMP */
		ERx("COMP-1"),		/* COB_1 */
		ERx("COMP-2"),		/* COB_2 */
		ERx("COMP-3"),		/* COB_3 */
		ERx("COMP-4"),		/* COB_4 */
		ERx("COMP-5"),		/* COB_5 */
		ERx("COMP-6"),		/* COB_6 */
		ERx("NO TYPE"),		/* COB_NOTYPE */
		ERx("DISPLAY"),		/* COB_DISPLAY */
		ERx("RECORD"),		/* COB_RECORD */
		ERx("INDEX"),		/* COB_INDEX */
		ERx("NUMERIC DISPLAY"),	/* COB_NUMDISP */
		ERx("NUMERIC EDITED"),	/* COB_EDIT */
		ERx("BAD TYPE")		/* COB_BADTYPE */
	    };

	    SIfprintf( f, ERx("GR_DUMP: gr_ \n") );
	    SIfprintf( f,
		       ERx("  sym = 0x%p, id = '%s', type = %d, flag = 0%o\n"),
		       gr->gr_sym, gr->gr_id, gr->gr_type, gr->gr_flag );
	    SIfprintf( f, ERx("  func = %d, mode = 0%o\n"),
		       gr->gr_func, gr->gr_mode );
	    SIfprintf( f, ERx("  Cb_rec = %d, Cb_print = %d, Cb_dims = %d,\n"),
		       gr->Cb_rec, gr->Cb_print, gr->Cb_dims );
	    SIfprintf( f, ERx("  Cb_err = %d,\n"), gr->Cb_err );
	    SIfprintf( f, ERx("  Cb: cs_lev = %d, "),
		       cs->cs_lev );
	    typ = cs->cs_type & ~COB_NOSIGN;
	    if (typ >= COB_COMP && typ <= COB_BADTYPE)
	    {
		if (cs->cs_type & COB_NOSIGN)
		    SIfprintf( f, ERx("cs_type = '%s' (unsigned), "),
			cb_type_names[typ] );
		else
		    SIfprintf( f, ERx("cs_type = '%s', "), cb_type_names[typ] );
	    } else
	    {
		SIfprintf( f, ERx("cs_type = %d, "), cs->cs_type );
	    }
    	    if (gr->Cb_sym.cs_type == COB_DISPLAY)
		SIfprintf( f, ERx("cs_slen = %d\n"), cs->cs_slen );
	    else
		SIfprintf( f, ERx("cs_nlen = %d, cs_nscale = %d\n"), 
		           cs->cs_nlen, cs->cs_nscale );
	    SIfprintf( f, ERx("ESQ: esq_ \n") );
	    SIfprintf( f, ERx(" flag = 0%o, inc = 0%o, dml_exec = 0%o\n"), 
		       esq->flag, esq->inc, dml->dm_exec );
	    SIflush( f );
	}
	break;

      case GR_EQSTMT:
	{
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
	    {
		gen_eqstmt( G_OPEN, kword );
		if (eq_ansifmt)
		    (void) Cput_seqno();	/* Emit sequence no. */
		gr->Cb_print = 1;
	    }
	    else
		gr->Cb_print = 0;

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

	    /*
	    ** If Cb_rec is greater than 1, then we have just been 
	    ** generating data declarations of level 02 and above at an 
	    ** indented level.  If so, outdent.
	    */
	    while (gr->Cb_rec > 0)
	    {
		gen_host( G_H_OUTDENT, (char *)0 );
		gr->Cb_rec--;
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

/*
+* Procedure:	gr_resolve_cob
** Purpose:	Resolve a COBOL usage and return symbol pointer.
** Parameters:
**	is_pl1	- bool	- The PL/I usage was used (DOT intead of IN).
**	simp_var - bool - The symbol used refers to a simple variable
** Return Values:
-*	SYM *	- Pointer to found entry.
** Notes:
**  1. COBOL variables can be referenced in 3 ways:
**		name 
**		name3 in name 2 in name1
**		name1.name2.name3		- PL/I method
**     If the usage is illeagal catch it.  
**     If the variable is undeclared (AND simple) redeclare it.
**     We don't redeclare records or arrays because we can't
**       reconstruct those declarations from their references.
**     If PL/I is used then change the internal reference to COBOL use,  
**        ie: Change a.b to b IN a.
**
** Imports modified:
**	What id_ bufferer thinks it has if PL/I is used.
*/

SYM	*
gr_resolve_cob( is_pl1, simp_var )
bool	is_pl1;
bool	simp_var;
{
    COB_SYM	*cs;
    SYM		*sy = (SYM *)0;
    i4		stat;
    char	pl1_name[ ID_MAXLEN +1 ];
    register char	*id, *cp;

    id = str_add( STRNULL, id_getname() );
    id_free();
    stat = symRsPL1( &sy, 1, is_pl1 ? syFisVAR|syFisREVERSE : syFisVAR );
    if (stat == syL_AMBIG)
    {
	er_write( E_E40001_hcbAMBIG, EQ_ERROR, 1, id );
	gr->Cb_err = 1;
    }
    else if (stat != syL_OK )
    {
	er_write( E_E4000A_hcbNOVAR, EQ_ERROR, 1, id );
	gr->Cb_err = 1;
    } else if (sy)
    {
	/*
	** Convert identifier DOT notation into IN notation.
	** This search for dots is wasteful: a name with n dots
	** has n+1 components, but searches a total of
	**	sum({i=1 to n+1} i) == (n+1)(n+2)/2
	** components, and is thus O(n squared) rather then O(n).
	** On the other hand, most identifiers have few dots,
	** and the alternatives are equally dismal (we have to worry
	** about double-byte characters, and so can't go backwards).
	*/
	STcopy( id, pl1_name );
	while (cp = STrchr(pl1_name,'.'))
	{
	    id_add( cp+1 );
	    id_key( ERx("IN") );
	    *cp = '\0';
	}
	id_add( pl1_name );

	if (((COB_SYM *)sym_g_vlue(sy))->cs_type == COB_NOTYPE)
	{
	    er_write( E_EQ0057_grHOSTVAR, EQ_ERROR, 1, id );
	    gr->Cb_err = 1;
	} else if (erec_vars() > 0)	/* Indicator array? */
	{
	    /*
	    ** If we are an indicator variable for a var which
	    ** could be a struct (currently SELECT, FETCH, or
	    ** INSERT) then list the indicators.
	    */
	    if (sym_g_dims(sy) == gr->Cb_dims+1)
	    {
		erec_ind_add(C_ARR_BASE, id_getname(), C_ARR_EXPR, sy );
	    } else
	    {
		erec_ind_add(C_ARR_BASE, id_getname(), (char *)0, sy );
		if (sym_g_dims(sy) != gr->Cb_dims)
		{
		  /* Check to see if subscription was needed */
		    if (sym_g_dims(sy) > 0 && gr->Cb_dims == 0)
		    {
			er_write( E_E40015_hcbMUSTINDIR, EQ_ERROR, 1, id );
			gr->Cb_err = 1;
		    } else if (sym_g_dims(sy) == 0 && gr->Cb_dims > 0)
		    {
			er_write( E_E40016_hcbCANTINDIR, EQ_ERROR, 1, id );
			gr->Cb_err = 1;
		    }
		}
	    }
	} else
	{
	    if (sym_g_dims(sy) > 0 && gr->Cb_dims == 0)
	    {
		er_write( E_E40015_hcbMUSTINDIR, EQ_ERROR, 1, id );
		gr->Cb_err = 1;
	    } else if (sym_g_dims(sy) == 0 && gr->Cb_dims > 0)
	    {
		er_write( E_E40016_hcbCANTINDIR, EQ_ERROR, 1, id );
		gr->Cb_err = 1;
	    }
	}
    }
	
    if (sy == (SYM *)0)
    {
	if (simp_var)		/* Enter undefined var if it's simple */
	{
	    /* Give hint for ESQL redecs */
	    sym_hint_type( (SYM *)0, T_UNDEF, 0 );
	    sy = symDcPL1( id, 0, 1, syFisVAR, COBCLOSURE, 0 );
	    if (sy)
	    {
		/* Enter default attributes - punt on a string type */
		cs = cob_newsym();
		cs->cs_lev = 1;
		cs->cs_type = COB_DISPLAY;
		cs->cs_slen = 10;
		sym_s_vlue( sy, cs );
		sym_s_btype( sy, T_UNDEF );
	    }
	}
    }
    return sy;
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
	-1, 165,
	463, 831,
	-2, 829,
	-1, 380,
	458, 811,
	-2, 810,
	-1, 505,
	504, 238,
	-2, 1306,
	-1, 660,
	502, 984,
	-2, 977,
	-1, 662,
	502, 984,
	-2, 982,
	-1, 720,
	485, 555,
	499, 555,
	593, 555,
	-2, 552,
	-1, 923,
	256, 1276,
	391, 1276,
	504, 1470,
	-2, 1468,
	-1, 924,
	256, 1277,
	391, 1277,
	504, 1471,
	-2, 1469,
	-1, 974,
	504, 238,
	-2, 236,
	-1, 1189,
	504, 238,
	-2, 414,
	-1, 1414,
	503, 2050,
	-2, 2087,
	-1, 1445,
	623, 2140,
	-2, 1309,
	-1, 1553,
	485, 2369,
	-2, 2073,
	-1, 1554,
	485, 2369,
	-2, 2073,
	-1, 1555,
	485, 2369,
	-2, 2073,
	-1, 1557,
	485, 2369,
	-2, 2073,
	-1, 1559,
	485, 2369,
	-2, 2073,
	-1, 1564,
	485, 2369,
	-2, 2073,
	-1, 1572,
	346, 2291,
	504, 2260,
	610, 2291,
	-2, 2289,
	-1, 1582,
	504, 2365,
	-2, 2363,
	-1, 1583,
	504, 2366,
	-2, 2364,
	-1, 1713,
	463, 1276,
	501, 1276,
	504, 1276,
	-2, 998,
	-1, 1812,
	314, 1317,
	318, 1317,
	322, 1317,
	323, 1317,
	329, 1317,
	-2, 523,
	-1, 1817,
	485, 555,
	499, 555,
	593, 555,
	-2, 548,
	-1, 1818,
	485, 555,
	499, 555,
	593, 555,
	-2, 550,
	-1, 1907,
	472, 1776,
	480, 1776,
	-2, 1787,
	-1, 2022,
	456, 1276,
	483, 1276,
	504, 1276,
	-2, 876,
	-1, 2036,
	485, 1200,
	499, 1200,
	593, 1200,
	-2, 497,
	-1, 2042,
	502, 782,
	-2, 1276,
	-1, 2063,
	504, 1183,
	-2, 1181,
	-1, 2089,
	502, 781,
	-2, 1227,
	-1, 2115,
	346, 2291,
	504, 2259,
	610, 2291,
	-2, 2289,
	-1, 2308,
	458, 1276,
	501, 1276,
	504, 1276,
	-2, 922,
	-1, 2334,
	502, 1276,
	-2, 1282,
	-1, 2413,
	280, 1305,
	284, 1305,
	289, 1305,
	309, 1305,
	315, 1305,
	316, 1305,
	327, 1305,
	336, 1305,
	394, 1305,
	395, 1305,
	468, 1305,
	469, 1305,
	503, 1305,
	569, 1305,
	589, 1305,
	-2, 1309,
	-1, 2572,
	458, 776,
	502, 782,
	-2, 1276,
	-1, 2573,
	458, 777,
	-2, 720,
	-1, 2642,
	504, 1276,
	-2, 1293,
	-1, 2643,
	504, 1277,
	-2, 1294,
	-1, 2818,
	314, 1317,
	318, 1317,
	322, 1317,
	323, 1317,
	329, 1317,
	-2, 524,
	-1, 2825,
	485, 555,
	499, 555,
	593, 555,
	-2, 528,
	-1, 3121,
	501, 442,
	503, 442,
	-2, 444,
	-1, 3122,
	501, 443,
	503, 443,
	-2, 445,
	-1, 3193,
	336, 596,
	485, 555,
	499, 555,
	593, 555,
	-2, 528,
	-1, 3306,
	339, 1362,
	-2, 1378,
	-1, 3487,
	339, 1363,
	-2, 1379,
};

#define	YYNPROD		2408
#define	YYLAST		10575

short yyact[] = {
	  2228,  3720,  2674,  3721,  2544,  3702,  2545,  3704,  2999,  2657,
	  3389,  3488,  3561,  2542,  3156,  1141,  2061,  1624,  3074,  1761,
	  3321,  1885,  2206,   660,  3380,  2628,  2548,  3485,   979,  2866,
	  3047,   505,  3581,  3301,  2081,  2654,  2686,  2240,  1108,  2088,
	  3202,  3207,  1540,  1274,  3198,  3369,  3048,  2044,  3368,   721,
	  2216,  3582,  1180,  2995,  1091,  3120,  2659,  1220,  3116,  3203,
	  2409,  2278,  3480,  2984,  2127,  2205,  1454,  2935,  3263,  3123,
	   557,  2646,  1450,  2031,  2627,  1449,  2084,  2680,  2450,  1448,
	  1453,  1725,  1451,  1886,  2060,  3195,  2682,  2730,  2738,  2075,
	  2888,  2793,  2183,  2754,  2225,  3327,  1367,  2633,  2677,  2603,
	   576,  2692,  3063,  1071,  2488,  2946,   560,  1270,  2062,  2451,
	  2414,  2260,   877,  3201,  2568,   502,  2948,  1813,  2350,   634,
	  2338,  1513,  2803,  2296,  3077,  2343,  1681,   951,  2670,  2202,
	  1943,  2404,  2256,  1539,  1737,   586,  2710,  2274,  2725,  2047,
	  2257,   567,  2253,  2697,  2676,  2665,  3032,  1541,  1063,  1332,
	  2072,  2567,  2034,  2255,   580,  2279,  1092,  1493,  2037,  1401,
	  1900,  1566,  2560,  1991,  1008,  2605,  1924,  1948,   561,   561,
	   561,  2214,   579,   585,   561,   561,   831,  1320,  1790,   585,
	   561,   595,   579,   561,   604,  3117,   561,   579,  1890,  1859,
	   561,   579,   579,  1838,   561,   561,   579,   629,  1847,   869,
	   566,  1824,  2356,  2326,   648,  1731,   648,   982,  1734,  1668,
	   661,  2056,  1053,   604,  1331,  2696,   677,   955,   585,   579,
	   585,   682,   683,   585,  1280,  1613,  2804,   696,  1633,  1278,
	   612,   718,  1580,  1891,  2116,  1858,  1447,  1598,  2166,  2595,
	   879,  1520,  1490,  1486,  1488,  1487,   509,  1481,   678,  2159,
	  1463,   701,   652,  1444,  1380,  1399,  1382,   856,  1475,  1375,
	  1364,  1357,  1415,   722,  1352,  1940,  1877,  1440,  1308,  1875,
	  1279,   983,  1319,   584,  2747,   744,   569,  1314,  2405,  1080,
	   589,   589,   709,   708,  1059,   655,  2213,  1258,  2337,  2342,
	   702,  1079,   608,   599,   658,  1245,   622,  1230,   659,   881,
	   625,   626,  3192,  1648,  1187,   508,  1514,  2681,   656,  3033,
	  1607,  1431,   606,  1058,   632,   633,  1589,  2158,  1049,  1544,
	   921,   657,   732,   559,   568,  2711,  1702,   741,  1010,  1383,
	   842,   604,  1347,  1412,   707,   594,  3127,  2327,   597,  1337,
	   509,   509,  1802,   670,   677,   950,   579,   561,   561,   579,
	   944,   561,   561,   561,  2103,   579,   579,   654,   509,   677,
	  2104,   677,  1171,   611,   703,   722,    64,   786,  2756,  1112,
	    52,  2038,  1160,   509,    49,  1565,  1617,  1478,  1551,  1353,
	  1192,  1417,   647,   920,   647,   719,  1185,   793,   854,   619,
	   840,  1048,  2712,   509,  1205,  1426,  1355,   704,   603,   508,
	   508,  1639,  1640,  2549,  2550,  2551,   974,   506,  1110,   883,
	   515,  2095,  2094,   722,   891,   892,  2557,   508,  2688,  1912,
	   900,   901,  2402,  1626,  3034,  1639,  1640,  2549,  2550,  2551,
	  1142,   978,   508,  3423,  1078,  3310,  1675,  3235,  2415,  3214,
	  3213,  3141,  2852,  2708,  2707,   579,  2446,  2392,  2390,  2375,
	  3177,  2372,   508,  1181,   697,  1972,  1971,   945,   946,   947,
	  3264,  1970,  3176,  3169,  1078,  1873,  1675,  1868,  1217,   948,
	   628,  2688,  2709,   836,  1458,   509,   747,  2588,  1459,   961,
	  2606,  2063,  1208,  3264,  2689,  2688,  2987,  1675,  2968,  2095,
	  2094,  1675,  2604,  3329,  2986,   515,   662,  1127,   496,   495,
	  3275,  2144,   941,   942,   720,   509,  3286,  3587,  2192,  1574,
	   649,  1973,  3470,  2983,  1074,   836,   509,  2399,   747,   746,
	  1675,  1271,   512,   509,  2849,  1791,  1792,  1793,  1796,  1693,
	  1363,  3106,  2997,   820,   508,  1626,   513,   515,  1051,  2386,
	  2592,   836,  1014,   836,   747,   858,   860,  1446,  1015,  1626,
	  1016,  3174,   486,   565,   834,   602,   512,  1639,  1640,  2549,
	  2550,  2551,  2403,   515,   508,   515,   607,   761,  2543,  3742,
	   513,   582,   515,   836,  1496,   508,  1142,   593,  1497,   836,
	   832,  1913,   508,  1563,   833,   515,   510,  1639,  1640,  2549,
	  2550,  2551,  2714,  2707,   804,   515,  1143,  3444,   952,  2558,
	   815,   515,  1147,   604,  2689,  2688,  3622,   579,  1639,  1640,
	  2549,  2550,  2551,   850,  1457,  1456,   679,  2606,   681,  1575,
	  1177,   684,  1113,  1639,  1640,  1592,  2712,  2986,  1153,  3443,
	   564,   494,  1208,  3038,   512,  3036,  1726,  3037,  2998,  2591,
	   830,  1639,  1640,  2549,  2550,  2551,   515,  2387,   513,   617,
	   747,   803,  1056,   581,   581,   514,   838,   837,   515,   676,
	   581,  1593,  1189,   581,   564,   581,  1052,  1113,   581,   515,
	  1675,   585,   581,   581,  1095,   953,  1208,   581,  3175,   565,
	   510,   510,   838,   837,   838,   837,  1231,  1639,  1640,  2549,
	  2550,  2551,  2251,  2048,   581,   605,  3223,  2549,   515,   581,
	   581,   581,  2168,  1253,   581,  1755,  1756,   953,  2586,  1235,
	  2997,  1213,   581,  2162,   838,   837,  1246,  1154,   512,   565,
	   838,   837,   836,  1458,  3442,   747,  1263,  1459,  1229,  1156,
	  1142,  1159,   513,   510,  1273,  1275,  1216,   635,   605,  2148,
	  1142,  2589,   564,  2354,   515,  1182,  1183,   487,  2859,  1639,
	  1640,  2549,  2550,  2551,  1226,   515,  1196,   638,   836,  1458,
	  1441,  1442,   605,  1459,  2689,  2688,   777,  1281,  2293,  2549,
	   515,  1051,   509,  1639,  1640,  2146,   836,   832,  1438,  1199,
	   515,   833,   975,  1701,  1073,  2701,  2702,  2704,  2705,  2706,
	  2703,  1639,  1640,  2549,  2550,  2551,  1700,  1232,   515,  1571,
	  2971,  2974,  2977,  2976,  1127,  2975,  1533,   788,   515,  1078,
	  1225,  1675,   581,  2590,   512,   925,  2998,  1508,  1640,  1057,
	  1639,  1640,  2549,  2550,  2551,   617,   564,   581,   513,  1675,
	   581,   508,  2593,  1593,   515,   486,   581,   581,  1254,  1639,
	  1640,  2549,  2550,  2551,  3693,   510,  1052,  1081,  1088,  1639,
	  1640,  2549,  2550,  2551,   512,  1082,   510,  1121,  3674,  1446,
	  1087,  2381,  1146,  1457,  1456,   578,   583,  1269,   513,   711,
	   712,   713,   583,  1884,  1145,   596,  3641,   583,  3658,  3659,
	   609,  3660,  3661,   515,   623,   624,  2632,  1338,  3571,   627,
	  3570,  1639,  1640,  2549,  2550,  2551,  1157,  1030,  2161,  1457,
	  1456,  1068,  1121,  3638,   585,  1502,   583,   698,   572,  2642,
	   565,   583,   680,   583,  1163,  3564,   583,   838,   837,  3465,
	  1675,  1152,   564,  2643,  3453,  2606,   581,  2978,  2979,   515,
	   514,  3452,  1184,   585,  2701,  2702,  2704,  2705,  2706,  2703,
	  1025,  2972,  2973,  2168,   605,  2095,  2094,  1316,  1045,   561,
	   579,   579,   579,  1197,   512,   565,  1472,   587,  3434,  2911,
	  2909,  3247,   564,  2457,  3236,  1150,  1208,   515,   513,  2991,
	  1471,  1233,  1238,  1639,  1640,  2549,  2550,  2551,  1248,   515,
	  1237,  1164,  2631,  1260,  1236,  1174,  1262,  1261,   515,  1173,
	   512,  1518,  2918,  1172,  1234,  1259,  3518,  1200,  3517,  1164,
	  1186,  1247,  1350,   939,   513,  1339,  3090,  1142,   722,  1538,
	  3049,   512,   565,  2224,  1195,   486,  2913,   564,   957,   587,
	   958,  1272,  2912,  1276,   583,   513,  1534,  1208,  1204,  2334,
	   487,   515,  1179,  1038,  1343,  1342,  1946,  2902,  1190,   940,
	  1675,  3242,   943,   513,  3241,  1526,  1317,  1335,   954,   956,
	   605,  1468,  1224,  1298,   515,  1522,  1046,  1209,  1050,  1503,
	  2900,  1512,   564,   515,  1309,  1945,  1944,  2908,  1430,  1210,
	   836,  1458,  1441,  1442,  2896,  1459,   561,  1495,  2885,  1296,
	   617,  1472,   704,  1472,   581,   515,  2523,  2507,   581,  1601,
	   512,  1178,   515,  1256,   512,  1471,  1955,  1471,   564,  3090,
	   512,   565,   665,  1148,   513,  1149,  1354,  1208,   513,   512,
	   565,   605,   510,  1366,   513,   514,  1936,  1625,   509,   564,
	  1304,   514,  2425,   513,  1036,  2223,  1627,  1311,  1284,  1348,
	   789,  3428,   486,  3427,  1037,  1033,  1630,   564,  1019,  1482,
	   512,  3234,  1643,  3233,  1644,  1919,  1647,  1663,  1665,   515,
	  1509,  1669,   581,  1671,   513,  1113,   512,   579,  2384,   579,
	  2988,   514,  1678,  3085,   565,  1682,   579,  3245,   704,  1951,
	   513,   836,  1458,  1657,   747,  1720,  1459,   508,  1409,  3225,
	  1078,   585,  1615,   515,  2020,   605,  1354,  1696,  1950,  1349,
	  1354,   515,   587,   515,   807,  2904,  1460,  2903,   564,   509,
	   579,   579,   564,  2884,   661,   677,  1511,  2513,   564,  2512,
	   487,  1457,  1456,   705,  1724,  1631,  1632,   564,  3583,  1728,
	  1699,  1950,   512,  1654,  1425,  1736,  1738,  1739,  1691,  1215,
	  1397,  1697,  1652,  1446,  3651,  1404,   513,  1420,   667,  1941,
	  1422,  1423,   515,   514,  1649,  1664,   665,  1011,   564,  2021,
	  2417,  3555,  2416,  1039,  1040,  1041,  1042,  1786,   508,   649,
	  2280,  1433,   587,  2618,   564,   666,  1464,  1732,  1334,  1707,
	  1476,   718,   605,  1816,  1817,  1776,  1818,  1819,  1820,  1499,
	  1645,  2578,  1722,  1679,  1465,  1208,   663,   512,   565,   512,
	  1186,  1050,  2562,  1321,  1614,   664,   583,  1077,  2561,  1604,
	  1161,   513,  1500,   513,   512,  1505,  1831,  1089,   665,  1090,
	   514,  1354,  1457,  1456,   668,   657,  1784,  2136,   513,   605,
	   822,  1695,  2384,  2815,  2383,  1315,  1077,   487,  1077,   512,
	   509,  2445,  1151,   669,  2163,  2022,  1781,  1191,  2147,  1729,
	  1730,  1799,  1530,   513,  1658,  2845,  1162,  2844,  2843,   513,
	   514,   605,  1760,  1782,  2097,  1741,   514,  2007,  1800,  2001,
	  1078,  1657,  1662,  1659,   583,   577,  1661,   515,  1611,  2371,
	  2448,  2369,  1762,  1986,  1974,  1821,  1605,  1552,   806,   512,
	  1771,   887,  1958,   512,   565,   581,   581,   649,  1779,   508,
	  1121,  1939,  1567,   513,  1780,   564,  1455,   513,  1078,   512,
	   514,  1829,  1791,  1792,  1793,  1796,  1704,  1787,  1934,  1473,
	  1212,   919,   564,   513,   581,  1011,   605,  1708,  1714,  2071,
	   514,   742,   747,  1595,  2310,   719,  1479,  1635,  1641,  1711,
	  1829,   581,   581,   581,  1081,   743,  1492,   564,  1828,  1434,
	  1477,   515,  1082,   564,   836,  1458,   887,   747,  2141,  1459,
	  1888,  1850,  1851,  2140,  2143,  2128,  1620,  1619,  2138,  2139,
	  1466,   512,  2137,  2142,  1373,  2208,   515,  1828,  1750,   857,
	  2176,   515,  2174,  1812,   512,   513,   509,   501,  2443,   512,
	  2306,  1628,  2003,   620,  2002,  1710,  1677,   564,   513,  2309,
	   662,   564,   512,   513,  1712,   514,  1835,  1887,  1836,   515,
	   514,   644,  2073,  1299,   807,   649,   513,   564,  1906,   645,
	  1288,   512,   565,   579,  1646,  1721,  1069,   895,  2010,  2011,
	  1791,  1792,  1793,  1796,  1998,   513,  1997,  1672,   649,   579,
	  1684,   515,   514,  1685,  1473,   508,  1473,  1174,  1683,   894,
	  2035,  1173,   890,   876,   720,  1172,  1995,   755,  1994,  1548,
	   923,   723,  1880,   807,  1879,  1878,   776,   747,  1988,  2374,
	  1987,  1715,  1588,  1594,   924,  2029,   646,  1834,  1915,   564,
	   515,   514,   873,  1742,  1876,  2308,   515,  1717,   643,  1690,
	  1713,  1718,  1164,   853,   847,  1457,  1456,   807,  1759,   513,
	  1775,   512,  2295,  1789,   513,  1547,   514,   583,  1881,  1805,
	   564,   514,   898,  1804,  1081,   513,  1883,  1746,   605,  1168,
	  1747,   515,  1082,  1981,  1956,  1980,   704,   814,  1764,   564,
	   817,   561,  1051,   642,  1844,  2012,   956,  1208,  2068,  2017,
	   565,  2066,  2069,  2067,  2083,   770,   809,   605,   581,  1969,
	   581,  1968,  1814,  1523,  1524,  1525,  1336,   581,  1966,  1826,
	   514,  1336,   512,  1953,  2210,  1855,   742,   747,   773,  2200,
	  2201,  1932,   581,  1931,   764,  1954,   513,   886,   565,   698,
	   743,   699,   512,   604,  1979,   509,   515,  1348,  1983,  1898,
	  1937,   581,   581,   564,  1895,   700,   513,  2238,   564,   742,
	   747,   515,   515,   514,  1081,  1921,  1546,  1545,  1910,   564,
	  1909,  1930,  1085,   743,  1377,  2579,  1920,  1410,  2275,   515,
	   579,   711,   712,   713,   757,  1916,   515,  1984,   677,   677,
	   759,  1095,  2242,  2292,  2207,   822,  1872,  1654,  1871,  1870,
	  1863,  1869,  2269,  2270,   508,  2005,  1652,  1354,  1797,  2294,
	  1587,  1360,   579,   677,  1231,  1336,   907,   677,   677,  2185,
	   742,   747,   581,   579,  1866,  2192,  1865,  1962,   579,  1208,
	   564,  3140,   762,  1081,   743,   677,  1354,  1235,  1639,  1640,
	   515,  1082,   579,  2207,   857,  2203,  2180,  2333,  3414,   698,
	  2181,   699,  1246,   704,   667,  1698,   704,  2465,   704,  1861,
	  2239,  2208,  1832,  1833,  1880,   700,  1879,  1167,  1169,  2329,
	  1400,  2250,  2335,  1862,  2339,  2339,   512,  3712,   806,  3583,
	  1682,   666,  1402,  2207,  1616,   718,   510,  1967,  3504,  1323,
	   513,  1867,  2344,  2344,  1263,  1377,  2008,  2009,   760,  2014,
	  1841,  2612,   663,  2466,  2894,  2895,  1078,  1763,  2192,  2297,
	  2438,   664,   509,   509,  3664,  2613,  1464,  2388,  2304,  2245,
	  1673,  2265,  1676,   512,   665,  1232,  1781,  2027,   747,  1161,
	   668,  2389,  2289,  1781,  1716,  1281,  1914,   513,  1927,  1928,
	  3102,   742,   747,  1782,   583,  3157,  2107,   515,  1922,   669,
	  1782,   711,   712,   713,  2015,   743,  2276,   745,  3621,  1421,
	  3606,   515,  1078,   956,   956,  3503,  3502,   908,  2016,   512,
	  3103,   508,   508,   742,   747,  2156,  2153,  2154,  2152,  2105,
	  2106,  2469,  2108,   513,  2109,  1582,   980,   743,  2272,  2110,
	  2171,  2173,  2164,   515,  1850,  1851,  3165,  2281,   577,  1583,
	   742,   747,  2462,  2463,  2467,  2461,  2351,  2464,   742,   747,
	   587,  2195,  2196,  2197,   743,  3746,  1302,  2410,  1405,  3578,
	   515,  3490,   743,  2209,   515,   704,  1322,   747,   515,  2307,
	  1303,  1078,  3401,  2212,   825,  3078,  2243,   824,  2355,   719,
	  1842,  2235,   826,   512,   723,   792,  2315,   828,   617,   695,
	  2364,   742,   747,  2336,  2412,  3279,  3280,   513,  2248,  2178,
	  3281,  2179,   507,   515,   581,   743,  2184,  2892,   694,  2193,
	   688,   515,  2194,  3164,   742,   747,  2455,   512,  3352,  2454,
	   581,  2527,  2528,  2453,   704,   565,  2456,  3283,   743,   629,
	  3282,   513,   687,  1763,   515,   561,  2211,  3212,  2217,  1233,
	  1238,  2254,  2230,  2428,  2429,  2430,  3505,  2406,  1237,  2963,
	  2363,  2312,  1236,  2241,  1248,  2244,  1992,  1316,  3078,  2217,
	  2555,  2540,  1234,  2539,  2321,  1164,  2332,  2101,  2411,  2100,
	  2099,  2301,  2532,  2258,   781,  3276,  3277,  1247,  2331,  2258,
	  3278,  2340,  1763,  2192,  2223,   698,  2863,   699,   565,   515,
	  2341,  1260,  1483,  1484,  1262,  1261,   507,   507,   720,  2346,
	  2345,   700,  2493,  1259,   986,  1396,  1977,   515,  2361,  2366,
	  1978,  2368,  2362,  2492,  2418,  2432,  2433,  2419,  2435,  2857,
	  2303,  2485,  2798,   579,  2520,   564,  2789,  2486,  2776,   724,
	  2358,  2359,  1600,  2317,  1395,  1975,  2177,  1577,  1578,  1976,
	   705,  1522,  2890,  2891,   778,  2391,  2609,  2932,  2608,   507,
	  2483,  2581,  1050,  2401,  2400,  2394,  1317,  2524,  2393,  1358,
	   742,   747,  2298,  2282,   581,  2271,  2157,  2030,  2490,  2491,
	  1902,  1905,  2025,   587,   743,  2424,  1309,  1783,  1903,  1904,
	   515,   577,  1907,   835,  2347,  2436,  2348,  1993,  2439,   561,
	   561,  2441,  1864,  1803,  2639,   561,  2357,  1281,   704,  1769,
	  1745,   581,  2449,  2582,  2459,  2594,  1622,  2484,  2258,  2601,
	  1618,  2651,  2652,  1602,  1596,  1372,  2023,  1271,  2556,   671,
	  1354,   607,   677,  2496,   579,  1330,  2648,  2584,  2497,  2494,
	  1366,   724,  2026,   581,  2500,  2502,  2503,  2509,  1312,  2658,
	  2683,   677,  2505,  2517,   581,  2511,  2519,  2499,  1669,   581,
	  1482,  2522,  2518,  2516,  2716,  2731,  2521,  2510,  1682,  1300,
	  1078,   507,  1239,   581,  2634,  2258,  1202,  1328,  2217,  2644,
	  1325,  2723,  1062,   902,   899,  1647,  2537,  1327,  2477,   724,
	   888,   819,  1329,   780,  1354,  2698,   561,   561,   675,   561,
	   592,  1354,  2479,  3078,   617,  2630,  2630,  3731,  1354,  1208,
	  2762,  2630,  2529,  1109,  2632,  2775,   581,  3676,  2780,  3643,
	   704,  1736,  3618,   575,  2786,  2275,   579,  2752,  3559,  3523,
	  2774,  1208,  3451,  3345,  2481,  2478,  3217,  1396,  2795,  1395,
	   905,   865,  2480,  2475,  2476,  2474,  2426,  2473,  2472,  2695,
	  2583,  1429,   849,   574,  2427,   866,   867,   791,  2640,   845,
	  1689,   790,  1654,  1705,  1181,  2470,  1732,   515,   561,   561,
	  2587,  1652,  1884,   718,  2504,  3450,  1326,  2807,   718,  2828,
	  2829,  1464,  2809,  1649,  2760,  1081,   583,  2727,  1476,  2817,
	   577,  2816,  2598,  1082,  2732,  2746,  1748,  2284,  2285,  2672,
	   587,  2611,  2616,  2610,  2772,  2615,  1537,   573,  2690,  2691,
	  2631,  1208,  1321,  2397,  2764,  2855,  1277,  2757,  2620,  2755,
	  1614,  2626,  2311,   956,  2621,  2622,  2313,  2314,  2619,  1781,
	  2637,  1243,  2748,  1783,  2619,  2794,  2777,   577,  2559,   512,
	   565,  2771,  2810,  2811,  2322,  1315,  1782,  1109,  3568,   577,
	  2261,  2765,  2766,   513,  2767,   956,   605,  1370,  2759,   515,
	  1658,  2713,  2787,  2204,  2028,  1857,  2318,  2721,  2574,  2575,
	  1419,  2320,  1306,  1455,  2630,  2630,  1394,  1657,  2580,  1659,
	   669,  2678,  1661,  2192,  1107,  2325,   906,  1291,  2750,   880,
	   880,   768,  3100,   880,  2749,   880,   880,  1392,  2736,  2251,
	  1251,   880,   880,  2276,   577,  1783,  2763,  3713,  2452,  1455,
	  1455,  1455,  1455,  1393,   607,  3653,  1783,  3675,   558,  2837,
	   577,  2734,  1874,  3408,  1240,  2136,   962,   719,  2784,  2782,
	   577,  2827,   719,  3049,  3522,  1778,  1455,   704,  1777,  1455,
	  1706,   607,  2188,  2788,  1455,  2191,  2720,   564,  2719,  2801,
	   577,   513,  2607,  2876,  1153,   579,   579,   579,  2162,  2035,
	  2709,  1643,  2930,  2920,  2921,  2928,  2831,  3448,  2190,  2151,
	  2629,  2629,  2420,  2036,  1894,  2818,  2629,  2881,  2882,   509,
	  1461,  1437,  1388,  2834,  2839,  2649,  1194,  1222,   734,   754,
	  2861,  2671,  2841,  1387,  2854,   671,   731,  2189,  2660,  1961,
	  2929,  1472,  2965,  1408,  3351,  1906,  2880,  2878,  2217,  2217,
	  2864,  2835,  2576,  2577,   581,  1471,  2883,  1271,  2715,  2187,
	  1783,   733,  2956,   880,  2939,   605,  2875,  2557,  2939,   880,
	  2886,   880,  1535,   674,   631,  1389,  2934,  1391,   508,  3472,
	  2941,  1542,   995,  1390,  2879,  2557,   720,  1634,  1385,  1386,
	  2258,   720,  2380,  2877,  2865,  1621,   615,  3003,  2217,  2869,
	   843,  2423,   801,  2408,  2186,   616,   519,  3636,  3246,  1070,
	   990,  2851,  2379,  2113,  3004,  3005,  3006,   556,   613,  1018,
	  2770,   988,  2319,  1854,   614,   581,   665,  3197,  3020,   782,
	  3009,   972,  2648,  2917,  3040,  3040,  2916,  2915,  3019,  1570,
	  2947,  3021,  3023,  1341,   607,  1072,  2145,  1022,   970,  3183,
	   605,  2926,  1495,  2267,  2712,   581,   971,   727,  1806,   996,
	  2802,  2931,  3288,  1266,  1268,  1267,  2231,  2955,  3635,  2229,
	   965,  1807,  1808,  3642,   669,   620,  2751,  1643,  2949,  2629,
	  2629,  3012,  2940,  2161,  1814,  2874,  3091,   966,   734,  2357,
	   964,  3075,  2262,  1826,   805,   968,  1295,  2275,  3056,  3059,
	  2952,  1414,  1506,  1017,  2698,  1193,  3088,  2600,  2898,  1515,
	  1516,  1095,  2251,   973,  3044,  3595,  2929,  2286,  3039,  3039,
	   993,  3323,  3076,  2989,  1445,  2969,  3101,  2980,   994,  2951,
	  3029,  3133,   668,   992,   991,  2191,  3011,  1469,  3026,   581,
	   734,  1911,   617,  2234,  2233,  3098,  3043,   581,  1498,  3324,
	  2558,  3061,   967,   722,  3071,   665,  3142,  3143,  2095,  2094,
	  2251,  2870,  2290,   561,   617,  3129,   956,  3025,  2558,   916,
	  3017,  3118,  3151,  3152,  1884,  1682,  3122,   917,  3154,  3121,
	   718,  3041,  3042,  3087,  3035,  3035,  2407,  3598,  2990,  2339,
	  3136,  2344,   726,  2013,   581,  3069,  3060,   665,  3208,   581,
	  3147,  3000,  3068,  3057,  3054,  1579,  3031,  1658,  1135,  3200,
	  3093,  1527,  3173,  3030,  3028,  3134,  3080,  1884,  3050,  3051,
	  3135,  3027,  3089,   663,   724,   960,  1659,   669,   930,  1661,
	  1135,  2684,   664,  1549,  3107,  1658,   509,  2367,  2354,  3132,
	  3110,  3190,  3216,  2939,  3112,  2203,  3115,   601,  3109,  2040,
	  2717,   668,  2297,  1662,  1659,  2596,   615,  1661,  3189,   510,
	  3138,  2597,  1469,   509,  1469,   616,  3113,  2685,  3144,  3148,
	  2862,  3155,  1321,  2939,   926,  2276,  3188,  2939,   613,  3187,
	  3066,  3249,  3250,   927,   614,  2576,  3159,  3160,  2936,  3222,
	  3162,  3309,  2936,  3182,  3167,   508,   929,  3185,  1532,  1504,
	  2217,  2351,   928,  3163,  2868,  2860,  2455,  3067,  3252,  2454,
	  3218,   705,  3227,  2453,  1127,  3252,  2456,  2822,  3158,  1081,
	   357,  2821,   508,   752,  2899,  2820,  2823,  1082,  2943,   885,
	  1065,   871,  2824,   878,   719,  1609,  1766,  1892,  2273,  3219,
	  2901,  3273,  3130,  3228,  2939,  2939,  2939,  3229,  2939,  1175,
	  1586,  2779,  1356,  1285,  1283,   561,  3254,  3255,  3256,   956,
	  3257,  3689,   931,  3269,  1753,  3295,  3296,  3298,  2833,  3239,
	   821,   984,   677,  3231,  1703,  1047,   581,   581,   581,   851,
	  3291,   855,   859,   861,   737,  3018,   686,  1206,  3073,  2223,
	  3022,  3312,  3248,  3315,  1473,  3605,   813,  3569,  3566,  3220,
	  2192,  3029,  1763,  3404,  3313,  2761,  2874,   677,  1531,  3306,
	  1452,  3260,  2552,  2656,  2553,  2554,  3318,  3046,  2217,  2217,
	  2947,   998,  3259,  1470,  1810,  1815,  3268,  3319,  3062,   936,
	   997,  3070,   937,  3311,  1035,  1218,  3325,   641,   772,  1599,
	   938,   588,  1964,  2167,     2,  3289,   934,   763,  2565,  2570,
	  3344,  2160,  3290,   720,   935,  3338,  3339,   729,  2731,  2165,
	   672,  2630,  1603,  1061,  1060,  1727,   579,  1597,  1055,  1054,
	  1581,  1797,  1034,  2035,   769,  1647,  1095,  1576,  1032,  2258,
	  3471,  3285,  3650,  3287,  2258,  3133,  3133,  3133,  3362,  2982,
	  3364,  3652,  3346,  2960,  3586,  3585,  3133,  2985,  3284,  3381,
	  2981,  2602,  3124,  1643,  3385,  2242,  2135,  3387,  2970,  3318,
	  3363,  2795,  3396,  3396,  3330,  1797,  1797,  2134,  1009,  3129,
	  3129,  3129,  3332,  3397,  2599,  1107,  2133,  2132,  3395,  3395,
	  3129,  3334,  2695,  2131,  2695,  3340,  3274,  2967,  1846,  2130,
	   561,  3179,  2695,  2129,  1573,  3349,  3355,   718,  1470,  3348,
	  1470,  3186,  1654,  3409,  3367,  3370,  3371,  3354,  2126,  2672,
	  3399,  1652,  3342,  3358,  2125,  3375,  2124,  2936,  3357,  3134,
	  3134,  3134,  3343,  1649,  3135,  3135,  3135,  3252,   918,  3413,
	  3134,  3377,   507,  3382,  3418,  3135,  3419,  2123,  2923,  2924,
	  2925,  2693,  1341,  3132,  3132,  3132,  3252,  2936,  3378,  2217,
	  3422,  2936,  3388,  2122,  3132,  3356,  2121,  3411,  2794,  2755,
	  2120,  2119,  1797,  3420,  3403,  2118,   590,  3416,  2117,  2115,
	  1658,  2585,  2114,  1572,  3386,  1031,  1029,  1028,  3252,  3252,
	  3252,  3252,  1027,   889,  3469,   897,  2740,  1657,  1026,  1659,
	   669,  2678,  1661,   650,  1024,  2741,  2630,  1023,  1947,   497,
	  1952,    21,  1494,  2000,   308,  1674,   717,  2006,  2739,   307,
	  2939,   581,  3440,   306,  2742,  1489,   437,   561,  2936,  2936,
	  2936,  2455,  2936,  3406,  2454,   829,  3439,  3446,  2453,  3430,
	  3454,  2456,  2648,  3492,   774,  3435,  3495,  3398,  3484,  3497,
	  3457,   771,  3487,  3445,  3272,  3464,  3130,  3130,  3130,  3432,
	   896,   719,  3467,   305,  1485,  3475,  2207,  3130,   783,  2064,
	   304,  1480,  3473,  3474,  2743,   893,  3514,   303,  2929,  2744,
	   302,  3133,  3133,  3390,  3390,   509,  3482,  3483,  3500,  1990,
	  3510,  3478,  3362,  3494,  3364,  3362,  3362,  3364,  3364,   301,
	  3362,  3299,  3364,  1474,   811,   812,  3506,  2410,   300,  2508,
	  1985,  2671,  1467,  1455,  3363,  3129,  3129,  3363,  3363,   299,
	  3540,   298,  3363,  3481,  3481,  3481,  3122,  3527,   704,  3121,
	  3237,  3524,  1462,  2630,   429,  3525,  3526,   884,  3208,  1643,
	   297,  3554,   296,  3552,   508,   605,   295,  2080,  2452,  3200,
	  1443,  3331,  3532,  3533,  3548,  3535,   722,  3558,  1439,  2102,
	  2102,  3536,   426,  2695,  1549,  3134,  3134,  3337,  1436,   875,
	  3135,  3135,   294,  3562,  3542,   162,  3550,  3549,  2929,  3551,
	   720,   161,   160,  2939,  2708,  2707,   159,   158,  3520,  3132,
	  3132,   157,  3270,   156,   155,  1095,  1095,   154,   153,  2939,
	   152,   151,   150,   149,   148,   561,  1351,  1374,   424,   872,
	   561,  3563,   292,  2709,   475,   423,  3124,  3124,  3124,   291,
	  3576,   290,  1960,  3599,  3591,  3477,  3584,  3124,  1428,  1959,
	  3593,  1107,  3592,  3590,  1427,  3597,  3575,  3600,  3306,   864,
	  1424,  3604,   863,   421,   862,   289,   420,  2217,   288,  3133,
	  3603,  1418,  3133,   419,  3613,  3381,  3607,   287,   418,   286,
	  3396,   293,  3567,  3620,  2080,  3630,   509,  3632,  3609,  3449,
	  3627,  2629,  2036,  2927,  2907,  3628,  3395,  3619,  1814,  3625,
	  3447,  3244,  2906,  3129,  2495,  2080,  3129,  1413,  2041,  1411,
	  2242,  3633,  3130,  3130,   417,   852,   285,  3252,  3543,   416,
	   284,  1407,   415,  1406,  3637,   848,   283,   581,  2958,  2959,
	   414,  2630,   846,   282,   844,   281,  2630,  3654,  3623,  1398,
	   841,  3624,   412,  3537,  3538,   508,  1797,  1797,  1797,  3655,
	  3665,  3666,   839,  3134,   280,   279,  3134,  1797,  3135,  2460,
	  1381,  3135,   410,   278,  2936,   827,   823,   277,  3362,  3362,
	  3364,  3364,   408,  3649,   275,  1075,  3649,  3132,  2897,  3372,
	  3132,  3373,   818,   274,  1075,  1371,   816,  2712,  3690,  1153,
	  3363,  3363,  3691,  3687,   273,  3436,  2889,  2887,  2893,  3703,
	  1926,  2444,  3562,  3692,   561,  3708,  1925,  3700,   581,  1923,
	  1075,  3307,  3682,  1075,  1369,  1368,  1155,  3710,  3489,   272,
	  3133,   270,  3396,  3716,  2471,  3717,  1166,  3718,  3723,  3709,
	  1384,  3498,  3715,  1365,  1362,  2098,   404,  1361,  3395,   271,
	   269,  1359,  3697,   268,  3698,   510,  3320,   808,   267,   399,
	  3058,  3515,   265,  3072,  3129,  3516,   263,   397,   262,  3362,
	  2422,  3364,  2421,  3736,   882,  1897,  3735,  3079,  3615,  1203,
	  3724,  3703,  3124,  3528,  3739,  3723,  3743,  3240,  2489,  3737,
	  3130,  3363,  2487,  3130,  1949,  2382,  2431,  1901,  3738,  3714,
	  1899,  3390,  1346,  2434,   800,  3438,   799,  2096,  1455,   396,
	  1896,  2398,  1345,   798,  3134,  3724,   258,  1340,  3539,  3135,
	  2630,   257,   795,   256,  2413,  1797,   255,  3724,  2217,  3544,
	  3433,  3546,   787,  1264,   252,  1963,   870,  2936,  3132,   147,
	  1947,  1947,  1947,  2452,   146,  1952,   145,   144,   143,  3353,
	  3000,   142,   141,  2936,  2236,   140,  2701,  2702,  2704,  2705,
	  2706,  2703,  1639,  1640,  2549,  2550,  2551,   139,  2246,   138,
	   137,   136,   135,   134,   133,   132,  2468,  2482,  3672,  3673,
	   131,  3184,  2263,  2672,   130,   129,   128,   127,   126,   125,
	  2672,   124,  1797,  1797,   123,  3394,  3394,   122,  3588,   121,
	   120,  3209,   119,   118,  2283,   117,  3489,  1445,  2501,  2501,
	  2501,   116,   115,   114,   113,   112,   111,   110,  3706,  3706,
	   109,  2299,   108,  1797,  1797,   107,   636,   202,   201,   200,
	  1214,  1529,   363,   673,  2501,   213,  1099,  2501,  2175,  1612,
	   518,  3130,  2501,  3390,  1658,  2624,  1100,  1608,  3226,  2873,
	  3124,  1658,  3224,  3124,  2672,  2872,  2871,  1889,  1333,   785,
	   784,  1657,   251,  1659,   669,  2678,  1661,  1067,  1657,  3730,
	  1659,   669,  2678,  1661,   517,   106,  2080,   639,  2080,  2080,
	   504,  1102,   503,  3016,  3640,  2150,  2149,  1585,  1044,   500,
	  3706,  2672,   499,   104,   199,  3565,   198,  1324,  1099,  3647,
	  1098,   779,   247,  3745,  1101,  2395,  1318,   775,  1100,   246,
	   245,  3261,  2080,  2080,  3262,  1658,  3656,   243,  3265,  3266,
	  3267,  1313,  2672,   242,  1307,  1104,  1103,   639,   767,  1310,
	   766,   765,  1657,   241,  1659,   669,  2678,  1661,   244,  2858,
	  1305,   240,   758,  1102,   239,  1105,  1301,   756,  1797,  2675,
	  2673,  1109,  1658,  2679,   639,  3014,  2653,  1638,  1638,  2669,
	  1797,  1207,  1098,  1797,  2668,  2667,  1101,  1075,  1075,  1657,
	  2666,  1659,   669,  2678,  1661,  2671,  2661,  3489,  3707,  3707,
	  2385,  1528,  2671,  1658,  1860,   922,   238,  1104,  1103,  1856,
	  1297,   753,   237,  2842,  2373,  1845,  1843,  2396,  1290,  1849,
	  1657,  3124,  1659,   669,  2678,  1661,  1848,  3421,  3211,  2856,
	  3424,  3425,  3215,  1109,  2850,  2378,  1107,  1107,  2853,  3326,
	  1797,  2641,  1107,   751,  1294,  2848,  2552,  3333,  2377,  1853,
	   750,  1293,  3210,  2847,  3336,  2376,  1852,  1292,   749,   383,
	   909,  1289,   911,   748,   236,   912,  2671,  1839,  1797,  1797,
	  3707,  3015,  2370,   913,  2080,  2080,  1837,  1452,  1287,   916,
	   739,  1286,   910,   704,   738,   235,  3729,   917,   605,  3701,
	  3304,   515,  3303,  3302,  3300,  3010,  2036,  2647,   915,  2645,
	   605,  3486,  3596,  2671,  1096,  2182,  1094,  1093,  1797,  1797,
	  1917,  2071,  2996,  1452,  1452,  1452,  1452,  2155,  1591,  1590,
	  2662,  2663,  2664,  1929,  2413,  2636,   563,  3002,  1933,  3594,
	  1265,  1797,   710,  1097,  2671,  1938,   706,  1623,   229,  1794,
	  1452,  3161,  2328,  1452,   914,   880,  1774,  1773,  1452,  1772,
	  3168,   704,  1798,   515,  1257,  1785,  1255,  2208,  1770,  1797,
	  1252,   228,  1768,  1767,  1250,   226,  1749,  1241,  1957,   969,
	   222,   221,   219,  2785,  2302,  1107,  2071,   220,  2778,  1735,
	  1797,  1733,   963,  2569,  2662,  2663,  2664,  2805,  1223,   218,
	   217,   215,  1106,  1797,  3705,   214,   639,   649,   639,  3379,
	  2773,  1723,   959,  3394,  3376,  1107,  1107,  3137,  2291,  1211,
	   212,   457,  1791,  1792,  1793,  1796,  1982,   360,   359,   358,
	   210,  2277,  2208,  1709,   653,  1201,  1198,   651,  1989,   209,
	  3614,  3086,  2722,  2846,  1996,  1667,  3082,  2718,  1666,  1999,
	   178,   227,  2305,  2004,  1744,  1228,  1740,  3458,  1227,   685,
	   223,  2531,  3463,  1519,  1106,  2536,  2535,  2534,  2111,  2073,
	  1568,   444,   649,   355,  1510,   354,  1517,   353,  1521,   352,
	   351,   350,   349,   348,  1743,   507,   347,  1791,  1792,  1793,
	  1796,   346,   345,   344,  1947,  1947,   343,  2530,  2635,  2019,
	  1758,  1127,   933,   932,   342,   204,   591,   177,   166,   736,
	  1536,   735,   234,  1543,  1801,   730,    -1,   977,   976,   728,
	   233,    -1,  1507,   725,   232,  2093,  2092,  1135,  3511,  1208,
	  2068,  2964,   565,  2066,  2069,  2067,  2083,  2079,  2078,  2086,
	  2571,  2076,  2046,  2045,  2950,  2566,  2945,  2944,   605,  2564,
	  2942,  2563,   514,  2074,  3577,  3394,  3466,  3271,  2962,  2961,
	  2070,  2728,  2059,  2910,  1133,  1134,  1130,  1131,  1132,  1138,
	  1139,  1136,  1137,  1140,  1129,  1128,  2058,  2057,  2049,   639,
	  2043,  2039,  1469,  1498,  3055,  2694,  2700,  2226,  3610,  3335,
	  2699,  3065,  3064,  3328,  1208,  2068,  3657,   565,  2066,  2069,
	  2067,  2083,  2768,  2769,  2222,  2221,  2080,  2080,  2220,  2219,
	  2080,  3209,  2218,  3553,  2687,  2215,  3722,   514,  3719,  3547,
	  3206,  3204,  2071,  2365,  2080,  1827,  1825,  3410,  2080,  3194,
	  2071,  2790,  2791,  2830,  2360,  1823,  2080,  1282,  1822,  2259,
	  2799,   639,  2080,  2080,  3688,  3634,  3412,  1636,  1636,  1642,
	  3196,  3572,  2832,  2867,  3573,  2055,  3221,  2052,  3629,  3407,
	  3631,  2085,  2077,  2050,   639,  3545,  3191,  3405,  2208,  2819,
	  2353,   639,  1811,  2825,  2826,   639,  2089,  2617,  2090,  2054,
	  2053,  1830,  2836,  2169,  2091,  2087,  1064,  3560,  2051,  2541,
	  2033,   393,  3429,  1882,   250,  2095,  2094,   105,  2813,  2812,
	  2349,  1638,  1638,  1638,  3608,  2073,  1809,  3611,   649,   716,
	  1052,   715,  2071,  2073,   714,   230,   649,  3131,  1052,  3128,
	  3361,  1107,  3119,  1791,  1792,  1793,  1796,  2288,  2287,  1719,
	   211,  1791,  1792,  1793,  1796,   205,   203,  2268,  2758,  2266,
	  1188,   630,   197,  1692,   196,   194,   195,   193,  1653,  1656,
	  3501,  3322,  1660,  1655,  2080,  2080,  1651,  1650,  2208,  2655,
	  2753,  1694,  1158,   330,  2080,  1528,  2018,  2080,   639,  3645,
	   192,   191,   949,  3105,  2737,  2024,  2252,  1688,  1687,  3104,
	  2735,  2080,  2249,  1686,   621,   189,  3084,  1176,  1170,   618,
	   188,  1219,   216,  1754,  1765,  2073,   693,  2797,   649,  3153,
	  2796,  1757,  1244,  3668,   692,  3393,  2792,  1752,   691,   690,
	  2316,  1751,  1242,  1791,  1792,  1793,  1796,   689,   225,  1680,
	  3521,  3350,  2082,  3099,  2112,  2733,  1165,   610,  2071,   724,
	  1208,  2068,   512,   565,  2066,  2069,  2067,  2083,  1208,  2068,
	  2042,   565,  2066,  2069,  2067,  2083,   513,   187,   186,   185,
	  1675,   184,   515,   514,   513,  2729,   600,   605,   598,   183,
	   515,   514,   181,  3711,  1670,   180,  1637,  1637,  1470,   182,
	  1126,  2805,  1125,  2933,  2089,  2805,  2090,  3178,  1124,  1123,
	  1122,  1120,  2091,  1119,  2442,  2080,  1118,  1117,  2953,  1116,
	  1115,  1114,  1629,  2095,  2094,  1111,   190,   179,  1501,   904,
	  2080,   903,   318,  2080,   176,  2080,   174,  1076,   173,  2232,
	  1144,  2073,   172,   171,   649,   571,  1052,   570,   170,   169,
	  1208,  2068,  2237,   565,  2066,  2069,  2067,  2083,  1086,  1791,
	  1792,  1793,  1796,  2080,  1084,  2080,  2247,  2080,  1083,   605,
	   564,   168,   167,   514,   103,  2993,   102,   101,   564,   100,
	  1947,  2264,  3001,    99,  2514,  2515,    98,    97,    96,    95,
	    94,    93,  3007,  3008,  1528,    92,    91,    90,  3013,    89,
	  1528,  1639,  1640,  3243,  1282,    88,    87,    86,    85,  1639,
	  1640,  2032,    84,    83,    82,    81,    80,    79,    78,  2300,
	    77,    76,    75,    74,    73,    72,    71,    70,    69,    68,
	    67,    66,  3045,    65,  2080,  2080,  2080,    63,  2080,  3052,
	  3053,    62,    61,    60,    59,  2080,    58,    57,  2080,    56,
	  2082,    55,  2080,  2080,  2080,    54,    53,    51,    50,    48,
	    47,    46,    45,    44,    43,  3081,  1208,  2068,  2042,   565,
	  2066,  2069,  2067,  2083,  3094,    42,    41,  3096,    40,    39,
	    38,    37,   513,    36,  2071,   605,  1675,    35,   515,   514,
	    34,    33,  1107,    32,    31,    30,  3111,  2352,    29,    28,
	    27,    26,    25,    24,    23,   493,  1021,   492,    19,    17,
	    16,    15,    14,    13,    12,    11,  1107,    10,     9,     8,
	     7,     6,     5,     3,     1,     0,     0,     0,     0,     0,
	  2089,     0,  2090,  3145,     0,     0,  2547,     0,  2091,     0,
	  3149,     0,     0,     0,     0,     0,     0,     0,     0,  2095,
	  2094,     0,     0,  2080,     0,     0,     0,     0,  3180,  3181,
	  2080,  2080,     0,     0,     0,     0,     0,  2073,  2080,     0,
	   649,     0,  1052,     0,     0,  3193,   564,  2071,     0,     0,
	     0,     0,     0,     0,     0,  1791,  1792,  1793,  1796,     0,
	     0,  1636,  1636,  1636,     0,     0,     0,  2198,  2199,     0,
	     0,     0,     0,     0,     0,     0,     0,  1639,  1640,     0,
	  2080,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  2208,     0,     0,     0,     0,     0,   639,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  1727,
	     0,     0,     0,     0,     0,     0,     0,     0,  1107,  2080,
	  2080,     0,     0,  2080,     0,     0,     0,  1452,     0,     0,
	  2073,     0,     0,   649,     0,  1052,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  2082,  1107,  1791,  1792,
	  1793,  1796,  3253,     0,     0,     0,     0,     0,     0,  3258,
	     0,     0,  1208,  2068,  2042,   565,  2066,  2069,  2067,  2083,
	     0,     0,     0,   880,     0,     0,     0,     0,   513,     0,
	     0,   605,     0,     0,   515,   514,     0,     0,     0,  2538,
	     0,   507,     0,     0,  2547,     0,     0,     0,  2324,     0,
	     0,     0,     0,  3441,     0,     0,  2330,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  3292,     0,     0,
	  2080,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  3308,  2080,     0,     0,     0,     0,  2080,     0,     0,     0,
	     0,     0,  3316,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  1208,  2068,   512,   565,  2066,
	  2069,  2067,  2083,     0,     0,     0,     0,     0,     0,     0,
	     0,   513,   564,     0,  1107,     0,     0,   515,   514,     0,
	  1637,  1637,  1637,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  3508,     0,  3341,     0,
	     0,     0,  2080,  1639,  1640,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  1222,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  1075,  3359,     0,     0,
	     0,     0,     0,     0,     0,  1107,  1107,     0,     0,     0,
	     0,  2905,     0,  3374,     0,     0,     0,     0,   639,   639,
	   639,     0,   639,     0,   639,   564,     0,     0,     0,   639,
	  2914,     0,   724,     0,  2080,     0,  3400,  2065,     0,     0,
	  2080,     0,     0,  2080,     0,  2080,  2919,  2080,     0,     0,
	     0,     0,   639,     0,     0,     0,  1639,  1640,     0,  2080,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  2745,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  2080,     0,     0,  2080,     0,
	     0,     0,     0,     0,     0,  1075,     0,  1528,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  2533,
	     0,     0,  1107,     0,  1521,     0,     0,  1107,     0,     0,
	     0,     0,     0,  1528,     0,     0,     0,     0,     0,     0,
	     0,     0,  3455,     0,     0,  3456,     0,     0,  2080,     0,
	     0,  2080,     0,  3459,  3461,  3462,     0,     0,     0,  3437,
	     0,     0,  1452,  2806,     0,     0,  2806,     0,  2808,     0,
	     0,  2808,     0,     0,     0,  2814,     0,  2080,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  2547,  2547,  2547,     0,  2937,     0,     0,  3491,
	  2937,     0,  3493,     0,     0,  2547,     0,     0,  3499,     0,
	  2547,     0,     0,  2080,     0,   562,   562,   562,     0,     0,
	  3509,   562,   562,  3512,     0,     0,     0,   562,     0,     0,
	   562,     0,     0,   562,     0,  3519,     0,   562,     0,     0,
	     0,   562,   562,     0,     0,     0,     0,  2080,     0,     0,
	   637,     0,     0,     0,     0,  3529,     0,     0,     0,     0,
	     0,     0,     0,  3530,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  3541,     0,     0,  2650,     0,     0,     0,     0,     0,
	     0,  1107,     0,     0,     0,  1282,     0,  2080,     0,     0,
	     0,     0,     0,  1638,  1638,     0,     0,     0,     0,  2080,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  2547,     0,     0,     0,     0,     0,     0,
	     0,  2726,     0,     0,     0,   639,     0,     0,     0,     0,
	     0,  3574,     0,     0,     0,  2546,  3579,  2922,     0,     0,
	     0,     0,     0,     0,     0,     0,  3589,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  3601,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,   562,   562,  2954,     0,   562,   562,
	   562,     0,  3612,     0,     0,     0,     0,     0,   639,  1528,
	  3125,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  2800,     0,     0,  3626,     0,     0,
	     0,     0,  3238,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  3172,
	     0,     0,     0,  2994,     0,     0,     0,     0,     0,     0,
	  2994,  3644,     0,     0,  3646,     0,  2838,     0,     0,     0,
	  2994,     0,     0,     0,     0,  2937,     0,     0,     0,     0,
	     0,     0,     0,     0,  3662,     0,     0,     0,     0,     0,
	  3667,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  3670,  3671,     0,     0,  2937,     0,     0,     0,  2937,
	     0,  3677,  3678,     0,     0,  3680,     0,  3683,     0,  3685,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,   511,  3694,     0,     0,  3696,     0,     0,     0,
	     0,     0,  3699,  2546,     0,     0,     0,     0,     0,     0,
	     0,     0,  3095,     0,     0,  3097,     0,     0,     0,     0,
	     0,     0,     0,  3108,     0,     0,     0,  2547,     0,     0,
	     0,     0,     0,     0,  2547,     0,  2937,  2937,  2937,  3114,
	  2937,  2547,     0,  3726,     0,     0,  2071,     0,     0,  3727,
	     0,  1075,  1075,  1075,     0,     0,     0,     0,  2547,  2547,
	     0,     0,  3732,  3733,     0,     0,     0,     0,     0,     0,
	     0,  3146,     0,     0,  1528,     0,     0,  3740,  3150,     0,
	     0,     0,     0,     0,     0,     0,  3744,  1282,     0,     0,
	     0,     0,  2089,     0,  2090,     0,  2994,  2994,     0,     0,
	  2091,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  1282,  2095,  2094,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  2073,
	     0,     0,   649,     0,  1052,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  1791,  1792,  1793,
	  1796,     0,     0,     0,     0,     0,     0,     0,  2547,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  2547,     0,     0,     0,     0,     0,     0,  2547,
	     0,     0,     0,     0,     0,     0,     0,  1221,     0,     0,
	     0,     0,     0,     0,     0,   640,     0,     0,     0,     0,
	     0,  1249,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  3024,     0,     0,  3366,  3125,  3125,  3125,     0,     0,     0,
	     0,     0,     0,  1636,  1636,  3125,     0,     0,  2082,     0,
	     0,     0,     0,     0,     0,   640,     0,     0,     0,     0,
	     0,  3392,  3392,     0,  1208,  2068,  2042,   565,  2066,  2069,
	  2067,  2083,     0,     0,     0,     0,     0,     0,     0,     0,
	   513,     0,   640,   605,     0,     0,   515,   514,     0,     0,
	     0,     0,     0,     0,  2547,  3293,     0,     0,   640,   640,
	     0,     0,   640,     0,   640,   640,     0,     0,     0,     0,
	   640,   640,     0,  2547,     0,     0,     0,     0,     0,  2547,
	  3317,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  2547,     0,     0,   639,     0,     0,     0,     0,
	     0,  2546,  2546,  2546,  3139,  2938,     0,     0,     0,  2938,
	     0,     0,     0,     0,  2546,     0,     0,     0,     0,  2546,
	     0,     0,  2937,     0,  2547,  2547,  2547,  2547,     0,     0,
	     0,  2547,  2547,     0,   564,  2547,  2547,  2547,     0,  3171,
	     0,     0,     0,     0,     0,     0,   981,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  3199,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  3360,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,   640,     0,     0,     0,  2547,     0,   640,     0,
	   640,     0,     0,  2547,     0,     0,  2547,     0,     0,     0,
	     0,     0,     0,     0,  1075,     0,     0,     0,     0,     0,
	     0,     0,  1637,  1637,     0,     0,   562,     0,     0,     0,
	  3125,     0,     0,     0,     0,     0,     0,  3366,     0,     0,
	  3366,  3366,  2546,     0,     0,  3366,     0,     0,     0,     0,
	     0,     0,  3251,     0,     0,     0,     0,  3415,     0,     0,
	     0,     0,  3417,     0,   640,     0,   640,     0,     0,     0,
	     0,     0,     0,     0,     0,  3415,     0,  3431,     0,     0,
	     0,     0,     0,   639,     0,  2937,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  2937,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  3126,
	     0,  3460,     0,     0,     0,     0,     0,     0,  2547,     0,
	     0,     0,     0,  2547,     0,     0,     0,     0,     0,     0,
	  3305,     0,     0,     0,     0,     0,     0,     0,  3476,     0,
	     0,     0,  3314,   562,     0,     0,  3479,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  1623,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  2547,   740,     0,  2938,     0,  3616,     0,     0,     0,
	     0,     0,  3513,     0,     0,     0,     0,     0,  3125,     0,
	     0,  3125,   794,   796,   797,     0,     0,     0,     0,  3392,
	   794,     0,     0,     0,  2938,   810,     0,     0,  2938,     0,
	     0,     0,     0,  2547,     0,  3347,     0,     0,     0,     0,
	     0,  3531,     0,  2547,     0,  3534,     0,   640,     0,     0,
	   874,     0,  2547,  2547,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  2994,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  2546,     0,  2547,     0,
	     0,  2547,     0,  2546,     0,  2938,  2938,  2938,     0,  2938,
	  2546,     0,     0,  3366,  3366,   640,     0,     0,     0,   640,
	     0,     0,  3402,     0,     0,     0,     0,  2546,  2546,     0,
	     0,     0,     0,     0,     0,  2547,   640,     0,     0,     0,
	     0,     0,   640,     0,  3580,     0,     0,   640,     0,   640,
	     0,     0,     0,   640,  1788,     0,     0,     0,  2547,     0,
	     0,     0,     0,   985,   987,     0,   989,     0,     0,  3125,
	     0,  3392,     0,     0,     0,   989,     0,     0,     0,     0,
	     0,  1000,  1001,  1002,  1003,  1004,  1005,  1006,  1007,   640,
	     0,  1012,     0,  1013,     0,     0,   640,     0,   639,     0,
	     0,  2547,     0,     0,  3366,     0,     0,     0,  1623,  1623,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  3468,     0,     0,     0,     0,  2546,     0,     0,
	     0,     0,     0,     0,     0,     0,  3639,     0,     0,     0,
	     0,  2546,  1020,     0,     0,     0,   640,     0,  2546,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  3496,     0,   640,
	     0,     0,  3663,   640,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  3669,     0,     0,     0,
	     0,     0,  3365,  3126,  3126,  3126,     0,     0,     0,     0,
	  3679,     0,     0,  3681,  3126,  3684,     0,  3686,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  3391,  3391,     0,     0,     0,     0,     0,     0,     0,     0,
	  3293,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  2546,     0,     0,     0,     0,     0,     0,
	     0,     0,  3199,     0,     0,     0,     0,     0,  3725,     0,
	     0,     0,  2546,     0,     0,     0,     0,  3728,  2546,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  2546,     0,     0,   640,  3734,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  3741,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  2938,     0,  2546,  2546,  2546,  2546,     0,     0,  3305,
	  2546,  2546,     0,     0,  2546,  2546,  2546,     0,     0,     0,
	     0,     0,     0,     0,     0,  2071,     0,     0,  3602,     0,
	     0,   640,   640,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  2085,  2077,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  2089,     0,  2090,     0,  2546,     0,     0,     0,  2091,
	  2087,     0,  2546,     0,     0,  2546,     0,     0,     0,     0,
	  2095,  2094,     0,     0,     0,     0,     0,     0,   562,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  2073,  3126,
	  3648,   649,     0,  1052,     0,     0,  3365,     0,     0,  3365,
	  3365,     0,     0,     0,  3365,     0,  1791,  1792,  1793,  1796,
	   639,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   639,  1344,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  2938,     0,     0,     0,     0,  1376,
	     0,     0,  1378,  1379,     0,     0,     0,  3695,     0,     0,
	  2938,     0,     0,     0,     0,   562,     0,     0,     0,     0,
	     0,     0,  1403,     0,   794,     0,     0,     0,     0,     0,
	  1416,     0,     0,     0,  1376,     0,     0,  2546,     0,     0,
	     0,     0,  2546,     0,     0,     0,     0,  2082,     0,  1795,
	   794,  1435,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  1208,  2068,  2042,   565,  2066,  2069,  2067,
	  2083,     0,     0,  2323,     0,  1491,     0,     0,     0,   513,
	     0,     0,   605,     0,     0,   515,   514,   640,     0,     0,
	  2546,     0,     0,     0,     0,  3617,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  3126,     0,     0,
	  3126,     0,     0,     0,     0,     0,     0,     0,  3391,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  2546,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  2546,     0,     0,     0,     0,     0,     0,     0,
	     0,  2546,  2546,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   564,     0,     0,     0,  1553,  1554,  1555,
	  1556,  1557,  1558,  1559,  1560,     0,     0,  2546,     0,  1564,
	  2546,     0,     0,     0,     0,     0,     0,  1569,     0,     0,
	     0,     0,  3365,  3365,  1639,  1640,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  2546,     0,     0,     0,     0,     0,
	   640,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  2546,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  3126,   640,
	  3391,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  1942,     0,     0,     0,     0,     0,
	  2546,     0,     0,  3365,   485,   488,   489,     0,     0,     0,
	     0,     0,   490,   491,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  2525,  2526,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,   562,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,   640,   640,   640,     0,
	   640,     0,   640,     0,     0,     0,     0,   640,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	   640,     0,     0,  2573,     0,     0,     0,     0,   498,     0,
	   516,     0,   520,   521,   522,   523,   524,     0,   525,   526,
	   527,     0,   528,   529,   530,   531,   532,   533,   534,   535,
	   536,   537,   538,   539,   540,   541,   542,   543,   544,     0,
	   545,   546,   547,   548,   549,   550,   551,   552,   553,     0,
	     0,   554,   555,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  1840,     0,     0,     0,     0,  2071,
	  2223,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,   562,   562,     0,     0,
	     0,  2638,   562,     0,  2055,     0,  2052,     0,     0,     0,
	  2085,  2077,  2050,     0,     0,     0,     0,  2227,     0,     0,
	     0,     0,     0,  1893,     0,  2089,     0,  2090,  2054,  2053,
	     0,     0,     0,  2091,  2087,     0,   665,  2051,     0,     0,
	     0,     0,     0,     0,  2095,  2094,  1918,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  2724,   802,     0,     0,
	     0,     0,  2073,     0,     0,   649,  1935,  1052,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  1791,  1792,  1793,  1796,     0,   868,     0,     0,     0,     0,
	     0,     0,     0,   562,   562,     0,   562,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  2781,     0,     0,     0,  2783,
	     0,     0,   562,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   640,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,   562,   562,     0,     0,   640,
	     0,  2082,     0,   640,     0,     0,     0,     0,     0,  1491,
	     0,     0,     0,     0,     0,     0,     0,  1208,  2068,  2042,
	   565,  2066,  2069,  2067,  2083,     0,     0,  2071,  2223,     0,
	     0,     0,     0,   513,     0,     0,   605,   999,     0,   515,
	   514,     0,     0,   640,     0,     0,     0,     0,     0,     0,
	     0,     0,  2055,   640,  2052,     0,     0,     0,  2085,  2077,
	  2050,     0,     0,     0,     0,  2227,   640,     0,     0,     0,
	     0,     0,     0,  2089,     0,  2090,  2054,  2053,     0,     0,
	     0,  2091,  2087,     0,     0,  2051,     0,     0,     0,     0,
	     0,  2071,  2095,  2094,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,   640,     0,     0,
	  2073,     0,     0,   649,   640,  1052,  2055,     0,  2052,   640,
	     0,   640,  2085,  2077,  2050,  1043,     0,   564,  1791,  1792,
	  1793,  1796,     0,     0,   640,     0,     0,  2089,     0,  2090,
	  2054,  2053,  1066,     0,     0,  2091,  2087,     0,     0,  2051,
	     0,     0,     0,     0,     0,     0,  2095,  2094,  1639,  1640,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  2073,     0,     0,   649,     0,  1052,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  1791,  1792,  1793,  1796,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  2082,
	  2957,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  1208,  2068,  2042,   565,  2066,
	  2069,  2067,  2083,     0,     0,     0,     0,     0,     0,     0,
	     0,   513,     0,     0,   605,     0,     0,   515,   514,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  2082,     0,   640,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  1208,
	  2068,  2042,   565,  2066,  2069,  2067,  2083,     0,   640,     0,
	     0,     0,     0,     0,     0,   513,     0,     0,   605,     0,
	     0,   515,   514,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,   564,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  3083,     0,     0,     0,
	     0,     0,     0,  3092,     0,     0,  1639,  1640,     0,     0,
	     0,     0,     0,     0,   562,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,   564,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  2437,     0,
	     0,     0,     0,     0,  2440,     0,  2071,     0,     0,     0,
	  1639,  1640,  3205,     0,     0,     0,  2447,     0,     0,  1376,
	   562,  2458,     0,     0,     0,  3166,     0,  3170,     0,     0,
	     0,  2055,     0,  2052,     0,     0,     0,  2085,  2077,  2050,
	     0,     0,     0,  1416,     0,     0,     0,     0,     0,     0,
	     0,     0,  2089,     0,  2090,  2054,  2053,     0,     0,     0,
	  2091,  2087,     0,   665,  2051,     0,  1432,     0,     0,  2506,
	     0,  2095,  2094,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  2071,     0,  2073,
	  1491,     0,   649,   640,  1052,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  1791,  1792,  1793,
	  1796,     0,  2055,     0,  2052,     0,     0,     0,  2085,  2077,
	  2050,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  2089,     0,  2090,  2054,  2053,     0,     0,
	     0,  2091,  2087,     0,     0,  2051,     0,     0,     0,     0,
	     0,   640,  2095,  2094,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  2073,     0,     0,   649,     0,  1052,     0,     0,     0,     0,
	     0,  1550,     0,     0,     0,     0,     0,     0,  1791,  1792,
	  1793,  1796,     0,     0,     0,     0,     0,     0,  2082,     0,
	     0,     0,     0,  1561,  1562,     0,     0,     0,     0,     0,
	     0,     0,   562,     0,  1208,  2068,  2042,   565,  2066,  2069,
	  2067,  2083,  3294,  3297,     0,     0,     0,     0,     0,     0,
	   513,     0,     0,   605,     0,     0,   515,   514,     0,  1584,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  1606,     0,  1610,     0,     0,     0,     0,     0,   617,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  2082,
	     0,   640,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  1208,  2068,  2042,   565,  2066,
	  2069,  2067,  2083,     0,     0,     0,     0,     0,     0,     0,
	     0,   513,     0,     0,   605,     0,     0,   515,   514,     0,
	     0,     0,     0,     0,   564,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  1639,  1640,     0,     0,     0,
	  3383,  3384,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,   562,     0,     0,
	     0,     0,     0,     0,     0,   564,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  1639,  1640,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  1840,     0,  2840,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  1795,     0,     0,     0,     0,    18,   165,
	   320,   455,   362,   365,   368,   370,   371,   375,   376,   377,
	     0,   319,   224,   373,     0,     0,     0,     0,     0,     0,
	     0,     0,   357,     0,     0,     0,     0,     0,     0,     0,
	     0,   249,     0,     0,   336,   364,     0,   335,   206,     0,
	     0,  2625,     0,     0,   562,     0,     0,     0,     0,     0,
	     0,   324,   334,   442,     0,     0,     0,     0,     0,     0,
	     0,   361,     0,     0,  1908,     0,  3507,     0,     0,     0,
	     0,     0,     0,     0,   459,     0,     0,     0,     0,     0,
	   379,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  1221,     0,     0,     0,     0,   640,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,   382,   231,
	   458,     0,     0,     0,   333,   384,     0,   380,     0,     0,
	   381,   366,   640,     0,   392,   387,   388,   390,   391,     0,
	     0,   207,     0,     0,   264,   398,   394,   461,   460,     0,
	     0,     0,   464,   462,   266,   276,   405,   411,   254,   401,
	   407,   466,   413,   409,   260,   259,   253,   248,   261,   465,
	   463,     0,   406,   400,   403,   402,   467,   468,   471,   469,
	   473,   472,   470,   476,   484,   474,   422,   425,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,   562,     0,     0,     0,     0,   562,     0,     0,
	     0,     0,     0,     0,    22,   163,    20,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     4,     0,     0,   434,
	   439,   478,   477,   432,   436,   433,   428,   431,   310,   311,
	   309,   314,   315,   317,   313,   164,     0,     0,   175,   441,
	   323,   326,   321,   322,   328,   374,   325,   327,   331,   395,
	   337,   341,   445,   443,   446,   447,   453,   449,   452,   450,
	   356,   208,   479,   430,   385,   386,   480,   438,   440,   427,
	   481,   482,   483,   456,   340,   338,   339,   448,   367,   372,
	   369,     0,   389,     0,   329,   454,   312,   435,   332,   316,
	   451,     0,     0,     0,  2172,   378,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  1795,  1795,  1795,
	     0,     0,     0,     0,     0,     0,     0,     0,  1795,     0,
	     0,     0,     0,     0,     0,     0,     0,    18,   165,   320,
	   455,   362,   365,   368,   370,   371,   375,   376,   377,     0,
	   319,   224,   373,     0,     0,     0,     0,     0,     0,     0,
	     0,   357,     0,     0,     0,     0,     0,     0,     0,     0,
	   249,   562,     0,   336,   364,     0,   335,   206,     0,     0,
	  2170,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	   324,   334,   442,     0,     0,     0,     0,     0,     0,     0,
	   361,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   459,     0,     0,     0,     0,   640,   379,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   640,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  3232,     0,     0,     0,     0,     0,   382,   231,   458,
	     0,     0,     0,   333,   384,     0,   380,     0,     0,   381,
	   366,     0,     0,   392,   387,   388,   390,   391,     0,     0,
	   207,     0,     0,   264,   398,   394,   461,   460,     0,     0,
	     0,   464,   462,   266,   276,   405,   411,   254,   401,   407,
	   466,   413,   409,   260,   259,   253,   248,   261,   465,   463,
	     0,   406,   400,   403,   402,   467,   468,   471,   469,   473,
	   472,   470,   476,   484,   474,   422,   425,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  1795,  1795,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,    22,   163,    20,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     4,     0,     0,   434,   439,
	   478,   477,   432,   436,   433,   428,   431,   310,   311,   309,
	   314,   315,   317,   313,   164,     0,     0,   175,   441,   323,
	   326,   321,   322,   328,   374,   325,   327,   331,   395,   337,
	   341,   445,   443,   446,   447,   453,   449,   452,   450,   356,
	   208,   479,   430,   385,   386,   480,   438,   440,   427,   481,
	   482,   483,   456,   340,   338,   339,   448,   367,   372,   369,
	  2498,   389,     0,   329,   454,   312,   435,   332,   316,   451,
	     0,     0,     0,     0,   378,     0,     0,     0,     0,     0,
	     0,     0,    18,   165,   320,   455,   362,   365,   368,   370,
	   371,   375,   376,   377,     0,   319,   224,   373,     0,     0,
	     0,     0,     0,     0,     0,     0,   357,     0,     0,     0,
	     0,     0,     0,     0,     0,   249,     0,     0,   336,   364,
	     0,   335,   206,     0,     0,  1965,     0,     0,     0,     0,
	     0,  1795,     0,     0,  1795,   324,   334,   442,     0,     0,
	     0,     0,     0,     0,     0,   361,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,   459,     0,
	     0,     0,     0,     0,   379,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  3426,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,   382,   231,   458,     0,     0,     0,   333,   384,
	     0,   380,     0,     0,   381,   366,     0,     0,   392,   387,
	   388,   390,   391,     0,     0,   207,     0,     0,   264,   398,
	   394,   461,   460,     0,     0,  2614,   464,   462,   266,   276,
	   405,   411,   254,   401,   407,   466,   413,   409,   260,   259,
	   253,   248,   261,   465,   463,  2623,   406,   400,   403,   402,
	   467,   468,   471,   469,   473,   472,   470,   476,   484,   474,
	   422,   425,  1795,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,    22,   163,
	    20,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     4,     0,     0,   434,   439,   478,   477,   432,   436,   433,
	   428,   431,   310,   311,   309,   314,   315,   317,   313,   164,
	     0,     0,   175,   441,   323,   326,   321,   322,   328,   374,
	   325,   327,   331,   395,   337,   341,   445,   443,   446,   447,
	   453,   449,   452,   450,   356,   208,   479,   430,   385,   386,
	   480,   438,   440,   427,   481,   482,   483,   456,   340,   338,
	   339,   448,   367,   372,   369,     0,   389,     0,   329,   454,
	   312,   435,   332,   316,   451,     0,     0,     0,     0,   378,
	     0,  3556,  3557,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,    18,   165,   320,   455,   362,   365,   368,   370,   371,
	   375,   376,   377,     0,   319,   224,   373,     0,     0,     0,
	     0,     0,     0,     0,     0,   357,     0,     0,     0,     0,
	     0,     0,     0,     0,   249,     0,     0,   336,   364,     0,
	   335,   206,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,   324,   334,   442,     0,     0,     0,
	     0,     0,     0,     0,   361,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,   459,     0,     0,
	     0,     0,     0,   379,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,   382,   231,   458,     0,     0,     0,   333,   384,     0,
	   380,     0,     0,   381,   366,     0,     0,   392,   387,   388,
	   390,   391,     0,     0,   207,     0,     0,   264,   398,   394,
	   461,   460,     0,     0,     0,   464,   462,   266,   276,   405,
	   411,   254,   401,   407,   466,   413,   409,   260,   259,   253,
	   248,   261,   465,   463,     0,   406,   400,   403,   402,   467,
	   468,   471,   469,   473,   472,   470,   476,   484,   474,   422,
	   425,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  2966,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,    22,   163,    20,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     4,
	     0,     0,   434,   439,   478,   477,   432,   436,   433,   428,
	   431,   310,   311,   309,   314,   315,   317,   313,   164,     0,
	  2992,   175,   441,   323,   326,   321,   322,   328,   374,   325,
	   327,   331,   395,   337,   341,   445,   443,   446,   447,   453,
	   449,   452,   450,   356,   208,   479,   430,   385,   386,   480,
	   438,   440,   427,   481,   482,   483,   456,   340,   338,   339,
	   448,   367,   372,   369,     0,   389,  2071,   329,   454,   312,
	   435,   332,   316,   451,     0,     0,     0,     0,   378,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,  2055,     0,  2052,     0,     0,     0,  2085,  2077,  2050,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  2089,     0,  2090,  2054,  2053,     0,     0,     0,
	  2091,  2087,     0,     0,  2051,     0,  2071,     0,     0,     0,
	     0,  2095,  2094,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  2073,
	     0,  2055,   649,  2052,  1052,     0,     0,  2085,  2077,  2050,
	     0,     0,     0,     0,     0,     0,     0,  1791,  1792,  1793,
	  1796,     0,  2089,     0,  2090,  2054,  2053,     0,     0,     0,
	  2091,  2087,     0,     0,  2051,     0,     0,     0,     0,     0,
	     0,  2095,  2094,     0,  2251,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  2073,
	     0,     0,   649,     0,  1052,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  1791,  1792,  1793,
	  1796,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  2082,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  3230,  1208,  2068,  2572,   565,  2066,  2069,
	  2067,  2083,     0,     0,     0,     0,     0,     0,     0,     0,
	   513,     0,     0,   605,  1675,     0,   515,   514,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,  2082,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  1208,  2068,  2042,   565,  2066,  2069,
	  2067,  2083,     0,     0,  2071,     0,     0,     0,     0,     0,
	   513,     0,     0,   605,     0,     0,   515,   514,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,  2055,
	     0,  2052,     0,     0,     0,  2085,  2077,  2050,     0,     0,
	     0,     0,     0,     0,   564,     0,     0,     0,     0,     0,
	  2089,     0,  2090,  2054,  2053,     0,     0,     0,  2091,  2087,
	     0,     0,  2051,     0,     0,     0,     0,     0,     0,  2095,
	  2094,     0,     0,     0,     0,  1639,  1640,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,  2073,     0,     0,
	   649,     0,  1052,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,   564,  1791,  1792,  1793,  1796,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,  1639,  1640,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,  2082,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,  1208,  2068,  2042,   565,  2066,  2069,  2067,  2083,
	     0,     0,     0,     0,     0,     0,     0,     0,   513,     0,
	     0,   605,     0,     0,   515,   514,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,   564,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,  1639,  1640
};

short yypact[] = {
	 -1000,  9375, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   158,
	   158,   158, -1000, -1000, -1000, -1000, -1000,   158,   158,    13,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,   158,  1187,   158,  2236,   158,   158,   158,
	   158,   158, -1000,   158,   158,   158, -1000,   158,   158,   158,
	   158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
	   158,   158,   158,   158, -1000,   158,   158,   158,   158,   158,
	   158,   158,   158,   158, -1000, -1000,   158,   158, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,  2257, -1000,  2035,   469,   469,   469,
	  1902,   894,   844,   469,   469, -1000, -1000,  1795,   844,   469,
	    37,   894,   469,   609,  2059,   469,   894,  2326,  2363,   469,
	   894,   894, -1000,   469,   469,   894,   671,  2156,   737,   737,
	  1961, -1000,    67,  1167, -1000,  1167, -1000, -1000, -1000,  1522,
	 -1000,  2115,   844,  2155,  1793,    37,  1368,   844,   894,   844,
	    37,    37,   844,  2746,  1537,  1513,    37,  1296,  1815,   289,
	   844,  1488,  2441,  2320,  2743,  1171,  2662,  2109,  1213,  1372,
	  1162,  2003,  1133,  1162,  1156,  1156,  1071,  1788, -1000,  2270,
	  2108,   737,   644, -1000,  1865,  1519,  1171,  1171,  2225, -1000,
	 -1000, -1000,   158,  1171, -1000,  1306, -1000,  1134,  1171,  1133,
	  1133,  1115,   164,  1118,  1786,  1223, -1000,  1492,  1115,   293,
	  2221,  1867,  1082,  1853,  1306,  1081,  1272,  1223,  1223,  1858,
	   158,  2673,  1070,  1171,  1041,    60,    60,  1165,  1785,    60,
	  1040,    60,    60,  1037,  1015,  1100,  1779,    60,    60, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1778,  1986, -1000,
	  1412, -1000, -1000, -1000,  3775, -1000, -1000, -1000, -1000, -1000,
	   844, -1000, -1000,  1065, -1000,  2604,  2534, -1000, -1000, -1000,
	  2712, -1000,  2752,    37, -1000,   894,   469,   469,   894,    37,
	   469,   469,   469,   136,   894,   894, -1000, -1000,    37, -1000,
	    37, -1000,  2531,   737,  2043, -1000, -1000, -1000,  2393, -1000,
	 -1000,  2354, -1000, -1000,   804, -1000, -1000, -1000, -1000, -1000,
	 -1000,  1488, -1000,  1453, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,  2729, -1000, -1000,  1665,  1171, -1000,  1171,
	 -1000, -1000, -1000, -1000,  2288, -1000, -1000, -1000,  1171, -1000,
	  2288, -1000,   158, -1000,  1171,  1171,  1171,  1171,  1171,  1171,
	  1171,  1171, -1000,   913,  1171, -1000,  1171, -1000, -1000,    60,
	 -1000, -1000, -1000, -1000, -1000,    60, -1000,    60, -1000, -1000,
	 -1000, -1000,  2344, -1000,   894, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,  1171, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,  2293, -1000, -1000, -1000, -1000,   641, -1000, -1000,
	   158,   462,  2734,   303,   147, -1000,   315, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,  1777, -1000,  2672,   441,  1014,  2238,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,  2291,  1296, -1000,  1460,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1391,  1961,
	 -1000,  1961, -1000, -1000, -1000, -1000,  3678, -1000,  4020,    72,
	    72, -1000,  2656, -1000, -1000,    72,    72, -1000,  2062, -1000,
	  2062, -1000, -1000,   442,   897,  2073, -1000,   897, -1000, -1000,
	   679,  2059,   844, -1000,   226, -1000,   894, -1000,  1961,   442,
	  1335, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  2680,    37,
	 -1000,   136, -1000,  4020,  1875,  1961,  1961,   442,   442, -1000,
	   989, -1000,  2285,  2285, -1000, -1000,  1961, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,   104,  1771,   679, -1000, -1000,   442, -1000, -1000, -1000,
	  2748,  1806,  2748, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	   844, -1000,  2042, -1000,   250,  -155,   679, -1000, -1000,    67,
	   442, -1000,  2059,  2035, -1000,  1522,  1767,  2041, -1000,  1942,
	 -1000, -1000,  2553,  1182, -1000, -1000,  2015, -1000, -1000, -1000,
	 -1000, -1000,   671,  1131, -1000, -1000, -1000,   679, -1000, -1000,
	  2321, -1000, -1000, -1000, -1000, -1000, -1000,  2139,   503,    72,
	  2653, -1000, -1000, -1000, -1000, -1000,  1927, -1000, -1000, -1000,
	 -1000, -1000,   303,  2700, -1000, -1000,   303,  2699,  1008, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1999,  2388,
	 -1000,   164, -1000,   141, -1000,  1001,  1764,  1461, -1000,   293,
	 -1000, -1000, -1000,  1984,   141, -1000,   293, -1000,  1743, -1000,
	    58, -1000, -1000,  1471, -1000, -1000, -1000, -1000, -1000,  1871,
	 -1000,  1730, -1000,  2139, -1000,   766, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,  1244, -1000,   549, -1000,  1171,  2221, -1000,
	   967, -1000,  2673,  1244, -1000, -1000,   141,  2698, -1000,  1664,
	  1240, -1000, -1000,   -95, -1000,  1969, -1000,  1720,   962, -1000,
	 -1000, -1000,  1265, -1000, -1000,  1171,  1171, -1000, -1000,  2143,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1836,  1834,  2221,
	 -1000,  1327, -1000, -1000, -1000,  1171, -1000,  1445,  2124,   876,
	 -1000, -1000, -1000,  1204,  1982,  1982, -1000,  1386,  1982,  1982,
	 -1000, -1000, -1000,    96,  1852, -1000, -1000, -1000,  2673, -1000,
	 -1000,   158, -1000,   936,  1171,  2101,   275, -1000, -1000, -1000,
	 -1000, -1000, -1000,  2100, -1000, -1000,   141,  2698,   958,   -10,
	   141,   938,   924, -1000,  1597,  1597, -1000, -1000,  1171,   934,
	    90,   924, -1000,   844,   894, -1000, -1000, -1000, -1000, -1000,
	  2622, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   442,  2395,
	 -1000, -1000,   313, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,   844,  2059, -1000, -1000, -1000, -1000, -1000,   679,
	 -1000,   679,   679, -1000, -1000, -1000, -1000,  2405,   624,   894,
	   894,   655,  2517, -1000, -1000,   679, -1000,   679,   679, -1000,
	 -1000,  2285,  2779, -1000,  2621, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,   302,  1488,  2154,  1917, -1000,
	  1171, -1000,  2348,  1806, -1000,  1211,   158, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	  1171,  1171,  1171,  1171,  1171,  1171,  1171,  1171,   158,   158,
	 -1000,    80,  1171, -1000,   -10, -1000,   -10, -1000, -1000, -1000,
	  1171, -1000, -1000, -1000, -1000,  2287, -1000, -1000, -1000, -1000,
	   295,    24,  1642, -1000,  2511, -1000, -1000, -1000,  1430, -1000,
	 -1000, -1000, -1000, -1000,   158, -1000, -1000,  2696,  1239, -1000,
	   328,   328,   921, -1000,  1719,  1637, -1000,  1368, -1000, -1000,
	  1718,  1637, -1000, -1000, -1000,   158,  2677,   158,   141,   141,
	 -1000, -1000, -1000, -1000, -1000,   469,  1961, -1000, -1000,  1715,
	  1715, -1000, -1000, -1000,   442,  2179,  1711, -1000, -1000, -1000,
	 -1000,   679, -1000, -1000,   -88, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  -200, -1000,
	 -1000,  4020, -1000,  1298,  1961,  1961,  2170,    -1,    -1,  1806,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,    37, -1000,    37, -1000,  1007,   819,    37, -1000, -1000,
	    37, -1000,    37, -1000, -1000,   442,   894,   326,   894, -1000,
	  2073,    37, -1000,  1961,    37,   894,  2586, -1000, -1000, -1000,
	  2680, -1000, -1000, -1000, -1000,  2475, -1000, -1000,  1870, -1000,
	   844, -1000, -1000, -1000,  2653,   -40,    37,  2653,  1321, -1000,
	   292,   279,  2733,   147,  2049, -1000, -1000, -1000,  2035,  1095,
	   894, -1000, -1000,   976,    37,   442, -1000, -1000, -1000,   442,
	   817,  2035, -1000,    37, -1000, -1000, -1000,   153,    37,  1961,
	  1961, -1000, -1000,  2653,    37,    37,    37,   104,   679,  1705,
	 -1000, -1000, -1000, -1000, -1000,   442, -1000, -1000,   442,  1907,
	 -1000,  1296,  2715,   121,  1749, -1000,   442, -1000, -1000,  2681,
	  1704, -1000,  2032, -1000,  2636, -1000,  1026,  1949,   679,  1698,
	  1698, -1000, -1000, -1000,   289, -1000,  2314,  2331, -1000,  2797,
	   609, -1000,    37,    37, -1000,    37,    37,  1106,   805, -1000,
	   328,   328, -1000,   147,  1239,   147, -1000,  1496, -1000, -1000,
	   966, -1000,  2266, -1000,   293, -1000, -1000,  1977, -1000,  1304,
	 -1000,  1229, -1000, -1000, -1000, -1000,  1697,  1253, -1000,   328,
	 -1000, -1000,  -156,  1228, -1000, -1000, -1000, -1000,  1225, -1000,
	  -158, -1000, -1000, -1000, -1000, -1000,  2039,  1069,  1069, -1000,
	 -1000,  2478,  2021, -1000,   994,  2683,  1171,  2094, -1000, -1000,
	 -1000, -1000, -1000, -1000,   549, -1000,  2221, -1000,  1687,   158,
	 -1000,  1197, -1000,   -42,   328,   147, -1000,  1214,   -42,  1171,
	 -1000,   642,  1205, -1000, -1000,  2143,   328, -1000, -1000,  1383,
	 -1000,   -42, -1000,   164,  1160, -1000,   -42,   906, -1000,  1171,
	   613,  1179, -1000,   -42,   889, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   738, -1000,
	   580, -1000,   686, -1000,  1150, -1000, -1000,   818, -1000, -1000,
	 -1000,   593, -1000,  1113, -1000, -1000,   -42, -1000,   880, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,  2120,   876, -1000,
	 -1000,  9006, -1000,  1145, -1000, -1000, -1000,   141, -1000,  1138,
	 -1000,  -162,  -167,  -168, -1000,   328,   872, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,  1641,  1612, -1000, -1000,
	 -1000,   147,  1112, -1000,   -42,   147,   141,   871, -1000, -1000,
	 -1000, -1000, -1000, -1000,  1057, -1000,   -42,  1561, -1000,  1692,
	  1045, -1000,   -42, -1000, -1000,  1023, -1000,   -42,   857,   981,
	 -1000,   -42,  1171,   855, -1000, -1000, -1000, -1000, -1000, -1000,
	  2073,  2073,    72,    72, -1000,  1961,  2499, -1000,  1399,   897,
	   679, -1000,   850, -1000, -1000, -1000, -1000, -1000, -1000,   679,
	 -1000, -1000, -1000, -1000, -1000, -1000,  1677, -1000,   894, -1000,
	  2733,  1976, -1000,  1368, -1000, -1000, -1000,  1672, -1000,  4175,
	 -1000, -1000, -1000, -1000,   852, -1000, -1000, -1000, -1000, -1000,
	 -1000,  1574,  1574,   293,   293,   293,  1211,   293, -1000,   293,
	 -1000, -1000, -1000, -1000,   293, -1000, -1000, -1000,   679, -1000,
	  2253, -1000,   856,   -99, -1000, -1000,  2292,   271, -1000,   836,
	 -1000,   235, -1000, -1000, -1000,  2268, -1000,   303, -1000, -1000,
	   156, -1000, -1000, -1000, -1000,  1671, -1000, -1000,  2264,   832,
	 -1000, -1000, -1000, -1000,   439,  8671, -1000, -1000, -1000,   158,
	  2677, -1000,   969, -1000,   328, -1000, -1000,   442, -1000,   442,
	   469, -1000, -1000,  3678,  2188, -1000, -1000,   442, -1000, -1000,
	   442,    -1,    -1,    -1, -1000, -1000, -1000,  1806,  1806, -1000,
	 -1000, -1000, -1000,    72,    72, -1000,  1975,  1470, -1000,  2538,
	 -1000, -1000, -1000,    37,   442,  2357,  7402, -1000, -1000,  2349,
	   442, -1000,  2346,  2439,  2405, -1000,   308, -1000, -1000,  -200,
	  1961,   849,   844,   442, -1000, -1000, -1000,  7402, -1000, -1000,
	   308, -1000, -1000,   442, -1000, -1000,  2409, -1000,  1546, -1000,
	 -1000,   770,  1961,  2383,   326,   679,  1470,   770,  2303, -1000,
	  1368,  1368, -1000,  1670, -1000,   147,  2684,   898,   679,   894,
	  1668, -1000, -1000, -1000,   679, -1000,   308,    37,    37, -1000,
	  2418,  2463,   505,    72, -1000,  1091, -1000, -1000, -1000, -1000,
	 -1000,  2139,  1667,   308, -1000,  2073,  1470, -1000,   442, -1000,
	  2139,  1090,    37,   976, -1000, -1000,    37,    37, -1000,  1296,
	 -1000,   442,   894,  2263, -1000, -1000, -1000,   894,  2553, -1000,
	 -1000, -1000, -1000, -1000,    37,  1182, -1000, -1000,  1806, -1000,
	 -1000,   894,  1949, -1000, -1000, -1000,   544, -1000, -1000,  1703,
	 -1000,  1298,  1298, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1298,
	  1298,  1301,   442, -1000,   442, -1000, -1000, -1000, -1000,    37,
	 -1000,   679,  2552, -1000,   914, -1000, -1000,  2653,  2653, -1000,
	 -1000, -1000, -1000,  2521, -1000,  2139,   770,  2729, -1000,  2545,
	   303, -1000, -1000, -1000, -1000, -1000, -1000,   868, -1000,  -172,
	 -1000, -1000, -1000, -1000, -1000,  1058, -1000, -1000,  -174, -1000,
	 -1000, -1000,  2245, -1000,  2175, -1000, -1000,   378,   821, -1000,
	    54, -1000, -1000,  1362,  -175, -1000,   141,  -176, -1000, -1000,
	    58, -1000,  1471,    32,  1069, -1000,  1311, -1000, -1000, -1000,
	   -63, -1000,  2543,   770,  2370,  2636,  7402, -1000,  -185,   749,
	 -1000,   147, -1000, -1000,   147,  2092, -1000,  2223, -1000,   621,
	 -1000,  1854,   580,   580,   580,  1620, -1000,   686,  2673, -1000,
	   141,  1418, -1000, -1000, -1000, -1000,  1664,  1171, -1000, -1000,
	   141, -1000,   -42,   977, -1000,   829,  -177, -1000, -1000,  1171,
	   869, -1000,  1265,   688,  1171, -1000, -1000,  2143,  1449,  1842,
	  2673,  1327, -1000,   719, -1000, -1000, -1000, -1000, -1000,   719,
	  1673, -1000, -1000, -1000, -1000, -1000,  1171, -1000,  2143, -1000,
	   818, -1000, -1000, -1000, -1000,   158, -1000, -1000, -1000,   587,
	   961,   961,   961, -1000,   141, -1000, -1000, -1000, -1000, -1000,
	 -1000,   141,  1171, -1000,   584, -1000,   141, -1000,   141,  1597,
	   706, -1000,   -42,   -42, -1000,  1597,   -10, -1000,  1597,   -10,
	 -1000,    60, -1000,  1171,   -10,   583, -1000,  1662,  1182,  1182,
	    37,    37, -1000, -1000, -1000, -1000, -1000, -1000,   671,  1828,
	 -1000, -1000, -1000, -1000,   802, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,   679, -1000,   -74,   267, -1000,  9949, -1000,
	  9949,  9949, -1000, -1000, -1000,   143,   442,  2352, -1000, -1000,
	   796, -1000, -1000, -1000, -1000,   790, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,  9731,  9671,   442,   442, -1000, -1000,
	   328,   328,   779, -1000,  1219, -1000,   442, -1000,  1875, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000,  1969, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,   894,  2291,   204,   856, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   131, -1000,   223,
	  2044,  2572,  2572,  2404,  2044,     9, -1000,  2071, -1000, -1000,
	 -1000, -1000, -1000, -1000,  1653, -1000,  1651,  1304,  1346, -1000,
	   158,  2683,   303, -1000, -1000, -1000, -1000,   761,  2264, -1000,
	  1637, -1000, -1000, -1000,  2264,   198, -1000,  1637, -1000, -1000,
	   158,  8302, -1000, -1000, -1000,   141, -1000, -1000,  1934,  1934,
	   308, -1000, -1000,   424,  1934, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000,   683,  1806, -1000, -1000, -1000, -1000, -1000,
	    37,    37, -1000, -1000, -1000,  2785,  3628,  1298, -1000, -1000,
	 -1000,    37, -1000,   894,  -132, -1000,  7402,  7184, -1000, -1000,
	 -1000, -1000, -1000, -1000,  3188,   316,   442, -1000,    72, -1000,
	    37, -1000, -1000, -1000, -1000, -1000,  2068,    37,  1529, -1000,
	 -1000,  1806,  2765,   326,    37,  -132, -1000,    37,  2038,  2170,
	  2966, -1000, -1000,   679, -1000,  2016, -1000, -1000,   770,  2139,
	  2729, -1000,  2366,  2653,  1007,  2521,  7402,  2771,  2021, -1000,
	 -1000, -1000, -1000,   147,  1961,   469,   469, -1000,   469,   679,
	 -1000,   679, -1000, -1000,   308,   308, -1000, -1000,   442,  2543,
	 -1000,  2438,  -200, -1000,    37,  1633,  1961,   615, -1000, -1000,
	    37,  1182,  2653,    37,   898,   894,  1631,   679, -1000, -1000,
	 -1000,   679, -1000,   308,   308, -1000, -1000,   329, -1000, -1000,
	  1627, -1000,   308, -1000,  1806,  1949, -1000, -1000,   442,  1186,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   679, -1000,  -200,
	   679, -1000,   679, -1000,  -200,   679, -1000,  1934,  1934,   859,
	 -1000,  -200,   609,  2643, -1000,   537,   417,   914,    37,    37,
	  2724,  1135,  2170, -1000,   537,  2348,  1806, -1000, -1000, -1000,
	  1496, -1000,  1171,  1439, -1000,   862,   -55, -1000,  2243,  -181,
	 -1000, -1000, -1000, -1000,  1304,  3557,  1624, -1000, -1000, -1000,
	   265, -1000,  1471, -1000, -1000, -1000, -1000, -1000,   328, -1000,
	 -1000, -1000, -1000,  1591,  2724,  2170,  2478, -1000, -1000,  2628,
	  2170,  -132, -1000,   328,  2453, -1000, -1000,   737, -1000, -1000,
	   147, -1000,   745, -1000,  2673,  1687,   580,   580,   719, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   701,   575,
	 -1000, -1000, -1000,  1383, -1000,  1647,  1349,   571,   141, -1000,
	 -1000, -1000,   328, -1000, -1000, -1000, -1000, -1000,   557, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,   328, -1000,
	 -1000,   534, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   694, -1000,   -42,
	 -1000, -1000, -1000, -1000, -1000,   474,   519, -1000, -1000, -1000,
	 -1000,   328, -1000, -1000,   513, -1000, -1000, -1000,   -42, -1000,
	 -1000, -1000, -1000,  1561,   -10,    90, -1000, -1000, -1000, -1000,
	   489, -1000, -1000, -1000,   -42, -1000, -1000,    72,    72, -1000,
	 -1000,   679, -1000, -1000,   894,   894,   894, -1000,  9949,  9949,
	    71,    37,  9949, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000,    17, -1000, -1000,   326,  6570, -1000, -1000,  2352,
	  6570, -1000, -1000,  2669, -1000,   349,  2457, -1000,  7402, -1000,
	   -67,  2144,  1182, -1000,  9949,  9949, -1000, -1000,   141,  1554,
	 -1000,    37,   158,   679, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,  -117, -1000, -1000, -1000, -1000,  -128,
	 -1000,   317,   -78,    23, -1000, -1000, -1000, -1000, -1000, -1000,
	   657, -1000, -1000, -1000,  2672,   147,   805,   466, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,   158, -1000,   308,    47,  3931,
	 -1000, -1000, -1000, -1000,   308, -1000,    37, -1000,    72,    72,
	    72, -1000, -1000, -1000,   308,   326, -1000,  1298,  2059,  1298,
	   326, -1000, -1000,  3616, -1000,  1298, -1000, -1000,  2923,  1298,
	  1806,  3665, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000,  2503,  2495,   149,   149, -1000, -1000,  2510, -1000, -1000,
	 -1000, -1000, -1000,  2035,   308,   548,  7402,  7402, -1000, -1000,
	 -1000,   -12,   326,   167,  2139,  2348,  7892,  2616,  2235,  9949,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  2751, -1000,
	 -1000,  1798, -1000,  9949,  2751,  2505,   605,   308,   678, -1000,
	 -1000, -1000,  1961,   597, -1000,   233,   308, -1000, -1000,   308,
	 -1000,    47, -1000,  2006,  1414,   -35,   898,   679, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,  1546, -1000,   770,   537, -1000,
	  2348, -1000,   770,   679, -1000, -1000,  1975,  2724, -1000,  -132,
	 -1000,  1488, -1000, -1000, -1000,   679,   679,   679, -1000, -1000,
	  4599,  2724,  2059, -1000,  1276, -1000,  -182, -1000, -1000, -1000,
	    72,    72, -1000, -1000,  2139,   308,    47,  2405,   679, -1000,
	 -1000, -1000,   308, -1000,  -200,  -200,  2136,  1390, -1000, -1000,
	 -1000, -1000,   469,  1186, -1000, -1000,  1298,  1592,  1298,   193,
	   308,   308, -1000, -1000,    37,  2299, -1000,  9949,  2552,   609,
	 -1000,  2597,  2594,  2576,  2559,  2653, -1000,   537, -1000, -1000,
	  2272, -1000,   483, -1000, -1000,  6570, -1000,  7466, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1542, -1000,  -183,
	 -1000,  -184, -1000,  1171, -1000, -1000,  1833, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,  2272,  6570, -1000,  7402, -1000,  6570,
	   192, -1000,   677,   688, -1000, -1000,  -185, -1000,   158, -1000,
	 -1000, -1000,   580, -1000,  1171, -1000, -1000,   640, -1000,  -186,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000,   461,   328, -1000,
	   -42, -1000, -1000, -1000,  1673,   558,   665, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,  2237, -1000, -1000,   458, -1000,    60,
	    37,    37,  1806, -1000, -1000, -1000, -1000,  -223, -1000, -1000,
	 -1000, -1000, -1000, -1000,    17, -1000,  6570,  6570,  6570, -1000,
	  6570,    17, -1000, -1000, -1000,  2457, -1000,  9949, -1000, -1000,
	  9949, -1000,   148, -1000,  9949,  9949,  9949, -1000,    63,   196,
	   328, -1000,   442, -1000, -1000,    72, -1000,  -101, -1000, -1000,
	 -1000, -1000,  1582,  1502, -1000, -1000, -1000, -1000,  1535,  1532,
	 -1000,   -90,  2342, -1000, -1000,     9, -1000, -1000,  2291, -1000,
	 -1000, -1000, -1000, -1000,   526, -1000, -1000, -1000, -1000,   308,
	 -1000, -1000, -1000, -1000,   469,   469,    37, -1000, -1000,   442,
	  1908,    37,   326, -1000, -1000,  2614,  -188, -1000,  3678, -1000,
	 -1000,  3696,  1806,  3665,   308, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	    72, -1000, -1000,  2766,  1298, -1000,    37, -1000,  2451, -1000,
	 -1000,  -146, -1000, -1000,  2021,  9949, -1000, -1000,   215,   442,
	  2334, -1000,  7811,  9949, -1000, -1000, -1000, -1000,  2616, -1000,
	  7811, -1000,  -223, -1000, -1000,   442,  1563, -1000, -1000,   215,
	   326, -1000,  1961, -1000, -1000, -1000, -1000,  2765,   504,    37,
	  1830, -1000, -1000, -1000,  2753,  1806,  2438,    37, -1000,  2126,
	  1523, -1000, -1000, -1000, -1000,   894, -1000, -1000,  2966, -1000,
	 -1000, -1000,  4175,  2521,  1007, -1000, -1000,  2272, -1000,   308,
	 -1000, -1000, -1000,  -223,  4599,  4599,  4599,   328, -1000,   328,
	 -1000, -1000, -1000,    72, -1000,  4373, -1000,  1949,    37, -1000,
	 -1000,   153,   469,   469,  1961, -1000,    37, -1000, -1000, -1000,
	   329,  4692,  4692, -1000, -1000,  7402,  2653, -1000,   308, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  1477,
	 -1000, -1000,  1806, -1000, -1000, -1000, -1000, -1000, -1000,  1934,
	 -1000, -1000, -1000,  2769,  -223,  2222,   609, -1000, -1000, -1000,
	 -1000, -1000,    37,  2653,  1875, -1000,  1303, -1000, -1000, -1000,
	 -1000,   679, -1000,   225, -1000, -1000,   679, -1000,   -74,   196,
	 -1000,  1171,  -190, -1000, -1000,  1171, -1000,   630,  1961,   679,
	  -132,   679,  -223,   737, -1000,   141, -1000, -1000, -1000,  2774,
	 -1000, -1000,   455, -1000,  1647,   239,  1969, -1000,   -10, -1000,
	 -1000, -1000, -1000, -1000,  2079,  1829,   428, -1000,   421, -1000,
	 -1000, -1000,  6570, -1000,    17, -1000, -1000,    17, -1000, -1000,
	 -1000,   125,  -223,  9949, -1000,   -37,    17,    17,  9949,  1470,
	   416,  2035,  1806,    37, -1000,   -81, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,  2162, -1000,  2044, -1000,  -110,
	 -1000,    47,   679,  3931, -1000, -1000, -1000, -1000, -1000,  1298,
	   679, -1000,  1961,  1961,  1961,  1298,  1934,   679, -1000, -1000,
	  1466,   308, -1000,  3665,   326, -1000,  3564,  1806,  2458,  2103,
	   308,  2051,  1551, -1000, -1000,  2636,  -223, -1000,    67, -1000,
	 -1000,  2505,   326,  -199,  9949,   326,   -37,  1480, -1000,   442,
	 -1000, -1000, -1000,   442, -1000,   495, -1000,   326,  1961, -1000,
	  2052,  1826, -1000,    67, -1000, -1000,  2724, -1000, -1000, -1000,
	  4599,  5561, -1000, -1000, -1000, -1000, -1000,    17, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000,   -37, -1000,  3678,  3678,   679,
	 -1000,    47, -1000, -1000, -1000, -1000, -1000,    47, -1000, -1000,
	 -1000,  3931,  3931, -1000, -1000, -1000,   236, -1000,  -132,    37,
	 -1000, -1000, -1000,   308,  1488, -1000,  7402,   442, -1000,   442,
	 -1000,  1298,   483, -1000, -1000,  6570, -1000,  9949,    71,  9949,
	    37,   750, -1000, -1000,  1171,  1171, -1000,  3665,  1825, -1000,
	  1298,  6570,   412,   328, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,  2763,  1960,
	  2762,   387, -1000, -1000, -1000, -1000, -1000,  9949,  -223, -1000,
	  9949, -1000, -1000,   -16,  2785, -1000, -1000,  1464,   308, -1000,
	 -1000,  1324, -1000,  1324,   -87, -1000,   442, -1000,   326,  1908,
	 -1000,  1298, -1000, -1000,  1934, -1000,  2416, -1000, -1000,   526,
	  2493, -1000, -1000,  3665,  1298, -1000,   326, -1000,  1806,  2051,
	  2451, -1000, -1000, -1000,  2760,  1405,  2628, -1000, -1000, -1000,
	  9949,  -199, -1000,  9949,   326,  1480,  1144, -1000,  1819,  2753,
	 -1000,  2021,  1403,   101, -1000, -1000, -1000, -1000,  4599, -1000,
	 -1000,  4599,   679,   679,    37, -1000, -1000, -1000, -1000,  4167,
	  1390, -1000, -1000,  -132,    37, -1000,    37,  2434, -1000,  2297,
	 -1000, -1000, -1000,  -223, -1000,  1171, -1000, -1000, -1000,   400,
	   679, -1000,   442,  -223, -1000,   373, -1000, -1000,  2355, -1000,
	 -1000,  1816,  -223,    17, -1000,  9949,   326,   442, -1000, -1000,
	  1806,   733, -1000, -1000,   733, -1000,  2033, -1000,  3931, -1000,
	 -1000,  -200, -1000, -1000,   260, -1000,   308, -1000,  1359, -1000,
	  3665,  3665,   326, -1000, -1000, -1000, -1000, -1000,  -223,  9949,
	   679,  -223, -1000,   326,   326, -1000,  3931,  3931,   355, -1000,
	 -1000,  2034,  1814,    17,   -37, -1000, -1000,   308,  2653,   308,
	 -1000,   308, -1000,  2768,  2710, -1000, -1000, -1000,  3665,  1298,
	  1298, -1000, -1000,   341, -1000,    17, -1000,  1806,   326, -1000,
	  1324, -1000,  1324, -1000,   308,   442,  3866,  4257, -1000, -1000,
	 -1000, -1000, -1000,   526, -1000, -1000, -1000,  3665,  -223,  9949,
	 -1000, -1000, -1000, -1000, -1000,  1322,  2024, -1000, -1000,  4599,
	 -1000,  4692,    37, -1000,    37, -1000,    37,   369, -1000, -1000,
	 -1000, -1000,   679, -1000, -1000,   326, -1000, -1000, -1000, -1000,
	 -1000,   308, -1000, -1000, -1000, -1000, -1000,  3931, -1000, -1000,
	 -1000,  -223, -1000,  1804,    17,   326, -1000, -1000, -1000,   679,
	 -1000, -1000,   369,    72, -1000,  1298, -1000, -1000,  3866,   308,
	 -1000,    64, -1000, -1000,   369, -1000, -1000, -1000,   326, -1000,
	 -1000,  3931,  1462, -1000, -1000, -1000, -1000
};

short yypgo[] = {
	     0,  4914,  3082,  4913,  4912,  4911,  4910,  4909,  4908,  7274,
	  4907,  4905,  4904,  4903,  4902,  4901,  4900,  4899,  4898,  4897,
	  4896,  4895,  4894,  4893,  4892,  4891,  4890,  4889,  4888,  4885,
	  4884,  4883,  4881,  4880,  4877,  4873,  4871,  4870,  4869,  4868,
	  4866,  4865,  4854,  4853,  4852,  4851,  4850,  4849,   374,  4848,
	  4847,   370,  4846,  4845,  4841,  4839,  4837,  4836,  4834,  4833,
	  4832,  4831,  4827,   366,  4823,  4821,  4820,  4819,  4818,  4817,
	  4816,  4815,  4814,  4813,  4812,  4811,  4810,  4808,  4807,  4806,
	  4805,  4804,  4803,  4802,  4798,  4797,  4796,  4795,  4789,  4787,
	  4786,  4785,  4781,  4780,  4779,  4778,  4777,  4776,  4773,  4769,
	  4767,  4766,  4764,  4762,   155,   514,  4761,  4758,  4754,   371,
	  2137,  4748,   291,   279,  4739,   106,   737,  4738,  4737,  4735,
	  4733,   693,   408,  4732,   398,  4730,   134,   368,   129,     0,
	  4729,    98,   306,  4728,  3081,  4727,  4726,   376,    74,  4724,
	  4722,  4721,   325,  5297,  4719,  4718,    15,   154,  4717,  4716,
	  4715,   369,    43,  4712,  4711,   100,   309,  4710,  4709,   228,
	  4707,  4706,  4703,  3349,  4701,   278,  4700,  4699,  4698,  4692,
	  4690,    22,  4689,   119,  4685,  4684,    37,  3075,    50,   124,
	  4682,  4679,  4678,   293,  4676,  2887,   127,  4675,    92,    87,
	    53,  4671,   372,   171,  4669,  4668,   304,  4667,  4647,  4646,
	  4645,  4643,  4641,  4640,    21,   363,    42,  4639,   126,    31,
	  4638,  4637,  4632,  4631,  4630,   407,   481,  4629,  4628,  4627,
	  4626,    91,    97,    17,    10,   211,  3568,  2889,  4625,    16,
	  4624,  4622,  4621,  4620,  4619,   114,   295,    23,   659,   308,
	   285,   343,   107,    19,  4617,    14,  4616,  4614,  4613,  4612,
	  4611,  4610,  4609,  4608,   389,  4607,   362,  4605,  4604,   469,
	  4603,  4602,  4600,  4599,  4598,  4597,  4596,   142,  4594,    88,
	   137,  4593,    57,  4592,   345,  4591,    52,  4590,  4583,  4582,
	   386,  4581,   153,   131,    58,  4580,    93,    65,    35,     9,
	    56,    76,    70,  4579,   303,  4577,  4576,    86,    77,  4573,
	   307,   286,    30,  4572,    46,    20,  4571,  4570,  4569,  4568,
	   128,  4567,  4566,  4565,  4564,   273,  4563,   135,  4562,  4561,
	  4560,  4559,  4558,  4557,    49,  4556,  3077,  4555,   178,  4550,
	  4549,  4548,  4547,  4542,   140,    55,    48,    45,    69,  4540,
	     4,    26,     6,   336,   511,  4539,    34,  4537,   108,    84,
	    89,  4535,  4534,  4531,  4529,  3065,   149,  4526,  4520,  4519,
	  4518,   118,  1013,  4517,   148,  4514,  3097,   214,  4513,   185,
	    85,  4512,  4511,   207,   133,    73,   271,   147,  4510,   152,
	  4509,  4507,    12,  4506,   311,  4503,   322,   229,   270,  4501,
	   224,  4497,    60,    29,  4492,   202,   117,  4490,  4489,  4487,
	  4486,   302,  4485,  4480,  4479,  4478,    83,    61,  4476,    59,
	   217,  4473,   274,   113,    40,  4472,    44,  4470,  4466,  4465,
	  4464,   132,  4459,   111,  4458,  4455,  4454,  4453,  4449,  4447,
	   201,  4446,  4445,  4443,  4441,  4440,    41,  4439,  4438,     1,
	     3,  4436,  4435,    36,  4434,  4432,  4429,  4428,  4425,  4424,
	   215,    95,    94,   139,   101,  4416,  4413,  1887,   143,   102,
	  4412,  4411,  4410,  4409,   136,  4408,  4407,  4406,    18,  4405,
	  4404,    13,   158,  4401,  4400,    47,    67,  4398,  4397,  4396,
	  4382,  4380,   150,  4379,  4378,  4377,  4376,  4374,  4373,  4371,
	  4370,  4369,  4367,  4366,   116,   105,    68,  4365,   151,  4364,
	  4363,   162,  4362,  4361,  4360,  4359,  4358,  4357,    39,  4351,
	  4346,  4345,   554,  4344,  4343,  4342,  4341,  4340,  4339,  4338,
	  4337,  4336,  4335,   212,  4332,  4331,  4330,  4329,   391,  4328,
	   226,  4327,  4326,  4325,  4324,  4323,  4322,  4319,  4317,  4316,
	  4313,  4312,  4311,  4306,  4303,  4302,  4301,  4300,  4299,   121,
	  4297,  4296,  4295,  4294,  4293,  4291,  4290,  4288,  4287,  4286,
	  4285,  4283,   241,  4281,  4280,  4279,  4278,  4276,  4275,  4274,
	   297,   298,   294,  4272,  4271,  4270,  4268,  4267,  4266,  4265,
	   209,  4262,  4261,   138,  4260,  4259,  4257,  4256,  4255,  4254,
	   357,   394,  4253,  4251,  4250,  4249,  4248,  4247,  4241,  4240,
	  4239,  4238,  4237,  4234,  4232,    38,  4231,  4230,  4229,    24,
	    54,  4225,    81,  4221,  4220,  4219,  4218,   205,   123,  4212,
	  4211,   208,  4209,  4208,  4207,  4204,  4203,  4202,  4201,  4200,
	  4199,  4197,  4196,  4195,  4194,  4193,  4192,  4191,  4190,  4188,
	  4186,  4185,  4184,  4182,   287,   342,   283,   282,   289,   125,
	  4180,   203,  4179,  4177,  4176,   337,  4172,   122,   288,   120,
	  4171,  4169,  4168,  4166,   334,  4162,  4160,  4159,  4157,  4156,
	   200,   141,  4155,   115,   316,  4149,  4148,  4147,  4142,   156,
	  4137,  4136,  4135,  4134,    25,     8,    11,  4132,  4131,  4129,
	    71,  4127,  4125,  4124,    33,  4123,    62,  4122,  4120,    27,
	  4119,     7,     5,     2,  4116,  4115,  4114,  4111,  4110,  6166,
	  4108,  4106,   193,  4102,  4097,  4094,  4093,  4091,  4089,    28,
	  4088,  4087,  4086,  4085,  4083,  4082,  4081,  4080,  4079,  4078,
	  4075,  4074,   360,  4073,   327,  4068,  4065,  4064,  4062,  4061,
	  4060,  4058,  4057,  4056,  4049,  4048,  4046,   333,  4045,   198,
	  4044,  4043,  4042,  4041,  4040,  4039,   299,  4036,   235,   320,
	  4035,   189,  4034,  4030,  4026,   145,  4020,  4015,  4014,  4009,
	  4006,  4005,  4003,  4000,   146,  3999,   144,  3997,  3996,  3994,
	  3992,  3991,  3087,  3990,  3989,  3988,  3078,  3983,  3981,  3980,
	  3979,  3978,  3974,   268,   177,   519,  3973,  3114,  3971,   277,
	   176,   275,  3967,  3960,  3959,  3957,  3956,   272,  3955,  2173,
	  3952,  3951,  3947,   269,  3946,   380,   326,  3944,  3943,  3942,
	  3939,  3938,  3937,  3936,  3935,   233,  3932,  3930,   318,  3925,
	  3924,   310,  3917,  3912,  3910,  3909,  3908,   110,  3907,   188,
	  3906,  3905,  3902,  3899,  3898,    78,   109,  3897,  3895,  3890,
	  3889,   225,  3888,  3885,  3883,  3882,  3881,  3880,  3879,  3878,
	  3877,  3876,  3875,  3872,  3870,  3867,  3866,  3865,  3864,  3863,
	  3862,  3861,  3855,  3853,  3852,  3850,  3849,  3847,  3844,  3841,
	  3839,  3838,  3837,  3836,  3835,  3834,  3830,  3825,  3824,  3823,
	  3822,  3821,  3820,  3819,  3817,  3805,  3802,  3801,  3798,  3797,
	  3796,  3794,  3789,   199,  3786,  3785,  3784,  3782,  3776,   387,
	  3773,  3772,   339,  3771,  3767,  3766,  3763,  3762,  3760,  3759,
	   319,  3757,    96,  3756,  3754,   388,  3753,  3752,   332,   330,
	  3750,   265,   160,  3747,   130,   167,  3746,   159,  3744,  3742,
	   104,  3738,  2781,  3737,  3725,  3722,  3720,   164,  3718,  3717,
	  3716,  3712,   328,  2734,  3709,  2661,  3708,  3707,   261,  3703,
	  3701,  3700,  3699,  3036,  3697,  3696,  2650,   378,  3695,  3694,
	   260,  3693,   329,  3690,  3684,  3681,  3679,  3675,  3674,  3669,
	   166,  3666,  3661,  3660,  3658,  3657,    90,  3656,  3655,    80,
	    66,  3654,  3646,  3645,  3643,  3642,  3638,  3634,   257,  3632,
	  3627,  3626,  3625,  3623,   254,  3622,  3620,   256,  3619,  3615,
	  3614,  3612,  3602,   390,  3600,  3599,   255,  3595,  3594,  3593,
	  3592,  3590,   354,  3586,  3585,  3583,  3582,  3581,  3580,  3579,
	  3576,  3575,  3574,  3569,  3567,   262,  3564,  3562,  3561,  3560,
	  3554,  3549,  3542,  3541,  3539,   381,  3538,  3537,  3533,  3531,
	  3528,  3526,  3525,  3524,  3523,  3522,  3520,  3519,  3514,  3509,
	  3508,  3502,  3501,  3499,  3495,  3494,  3492,  3489,  3488,  3487,
	   259,  3486,   264,   379,    82,    79,    75,    72,   396,  3484,
	  3483,  3482,  3481,  3480,  3478,  3477,  3474,  3473,  3471,  3467,
	  3466,  3462,  3461,  3455,  3452,  3449,  3448,  3442,  3438,   267,
	   253,  3430,   236,  3426,   112,  3422,  3420,  3417,  3414,   375,
	  2969,  3412,   250,  3401,  3399,  2973,  3392,  3390,   161,  3389,
	  3388,  3383,   258,   245,  3379,  3369,   163,  3360,   377,  3357,
	  3355,  3351,   247,   244,  3350,  3344,   243,  3343,  3340,  3306,
	  3305,   242,  3303,  3299,   157,  3297,  3294,   240,  3293,  3292,
	   103,   266,  3291,  3289,  3287,  3284,  3278,  3272,  3267,  3266,
	  3265,  3263,  3262,  3261,  3259,   234,  3258,  3255,  3251,  3250,
	  3246,  3243,  3227,  3206,  3204,  3198,  3184,    64,  3183,  3179,
	  3177,  3176,  3173,   239,  3167,  3166,  3164,   165,  3157,  3148,
	  3146,  3141,  3140,  3138,    99,    63,  3137,  3135,  3134,  3131,
	    51,  3129,  3123,    32,  3122,  3121,  3120,  3118,  3117,  3112,
	   232,  3110,  5782,  3724,   313,   284,  3109,  3108,  3107,   237,
	   317,  3104,  3103,  3102,  3099,   249,  3091,   238,  3083,  3079,
	  3074
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
	   678,   129,   763,  1037,   681,     9,     9,     2,    20,  1150,
	     7,   813,   813,   813,  1151,  1151,  1151,  1151,     6,  1152,
	  1154,  1153,  1153,  1155,  1155,  1155,  1155,  1155,  1156,  1162,
	  1162,  1164,  1164,  1165,  1165,  1165,  1165,  1165,  1165,  1165,
	  1165,  1165,  1165,  1163,  1160,  1161,  1161,  1176,  1176,  1176,
	  1166,  1167,  1168,  1169,  1169,  1169,  1169,  1178,  1178,  1178,
	  1177,  1177,  1170,  1179,  1180,  1181,  1181,  1181,  1171,  1182,
	  1183,  1183,  1183,  1172,  1184,  1173,  1185,  1186,  1186,  1187,
	  1174,  1188,  1189,  1189,  1189,  1189,  1189,  1189,  1189,  1189,
	  1189,  1189,  1189,  1189,  1189,  1189,  1175,  1190,  1191,  1191,
	  1194,  1194,  1195,  1195,  1196,  1197,  1198,  1199,  1199,  1192,
	  1192,  1201,  1202,  1202,  1203,  1203,  1203,  1200,  1204,  1193,
	  1193,  1205,  1206,  1206,  1157,  1157,  1207,  1208,  1208,  1208,
	  1158,  1209,  1209,  1210,  1210,  1211,  1211,  1212,  1213,  1213,
	   756,   677,   512,   457,  1214,   523,   523,  1216,  1217,  1217,
	  1218,  1218,   390,   343,  1215,  1215,  1221,  1222,  1223,  1223,
	  1223,  1220,  1220,  1225,  1226,  1226,  1224,  1224,  1227,  1228,
	  1219,  1219,  1229,  1159,  1230,  1230,  1230,  1230
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
	     1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
	     1,     2,     1,     1,     1,     1,     2,     3,     3,     3,
	     4,     0,     2,     1,     1,     1,     1,     2,     4,     1,
	     0,     1,     2,     1,     1,     1,     1,     1,     1,     1,
	     1,     1,     1,     1,     1,     3,     1,     1,     1,     0,
	     2,     2,     1,     2,     2,     2,     2,     2,     1,     0,
	     1,     0,     4,     1,     1,     2,     1,     0,     2,     1,
	     1,     1,     0,     2,     1,     3,     1,     1,     0,     1,
	     3,     1,     1,     2,     2,     1,     2,     2,     1,     2,
	     2,     1,     1,     2,     2,     1,     4,     1,     2,     5,
	     1,     1,     1,     0,     1,     3,     1,     1,     0,     4,
	     0,     1,     1,     0,     1,     2,     3,     1,     1,     3,
	     0,     1,     1,     0,     3,     1,     1,     1,     1,     3,
	     6,     2,     4,     1,     1,     1,     1,     1,     1,     0,
	     2,     1,     2,     2,     1,     1,     2,     2,     1,     0,
	     1,     2,     2,     2,     1,     2,     2,     0,     1,     2,
	     2,     1,     2,     2,     1,     1,     1,     2,     2,     1,
	     1,     2,     1,     1,     1,     1,     1,     1
};

short yychk[] = {
	 -4000,    -1,    -2,    -3,   504,    -4,    -5,    -6,    -7,    -8,
	   -10,   -11,   -12,   -13,   -14,   -15,   -16,   -17,   256,   -18,
	   494, -1152,   492,   -22,   -23,   -24,   -25,   -26,   -27,   -28,
	   -29,   -30,   -31,   -32,   -33,   -34,   -35,   -36,   -37,   -38,
	   -39,   -40,   -41,   -42,   -43,   -44,   -45,   -46,   -47,   -48,
	   -49,   -50,   -51,   -52,   -53,   -54,   -55,   -56,   -57,   -58,
	   -59,   -60,   -61,   -62,   -63,   -64,   -65,   -66,   -67,   -68,
	   -69,   -70,   -71,   -72,   -73,   -74,   -75,   -76,   -77,   -78,
	   -79,   -80,   -81,   -82,   -83,   -84,   -85,   -86,   -87,   -88,
	   -89,   -90,   -91,   -92,   -93,   -94,   -95,   -96,   -97,   -98,
	   -99,  -100,  -101,  -102,  -818,  -363,  -829,  -862,  -863,  -864,
	  -865,  -866,  -867,  -868,  -869,  -870,  -871,  -872,  -873,  -874,
	  -875,  -876,  -877,  -878,  -879,  -880,  -881,  -882,  -883,  -884,
	  -885,  -886,  -887,  -888,  -889,  -890,  -891,  -892,  -893,  -894,
	  -895,  -896,  -897,  -898,  -899,  -900,  -901,  -902, -1079, -1080,
	 -1081, -1082, -1083, -1084, -1085, -1086, -1087, -1088, -1089, -1090,
	 -1091, -1092, -1093,   493,   523,   257,  -529,  -103,  -106,  -114,
	  -117,  -120,  -123,  -133,  -136,   526,  -139,  -531,  -575,  -148,
	  -174,  -180,  -172,  -181,  -191,  -194,  -195,  -197,  -251,  -257,
	  -149,  -275,  -277,  -311,  -313,  -312,  -314,  -318,  -814,  -817,
	  -858,  -859,  -860,  -325,  -533,  -327,   296,   409,   549,  -585,
	  -594,  -329,  -599,  -853,  -611,  -613,  -249,  -614,  -615,  -627,
	  -624,  -628,  -629,  -564,   270,  -210,  -633,  -574,  -637,  -662,
	  -351,   387,  -513,  -517,  -524,  -705,  -715,  -752,  -757,  -779,
	  -781,  -787,  -796,  -802,  -785,  -803,  -804,  -810,   435,   289,
	  -365,  -833,  -906,   434,   426,  -908,  -910,  -913,  -915,   433,
	   432,   436,  -948,  -950,   412,  -951,   422,  -956,  -959,  -961,
	  -975,  -962,  -976,  -991,  -994,  -997,   423, -1000, -1003, -1009,
	 -1010, -1017, -1019, -1023, -1028, -1030, -1044, -1047, -1050, -1052,
	 -1062, -1063, -1066, -1043, -1094, -1103, -1105, -1106, -1113, -1114,
	 -1120, -1124, -1127, -1129, -1134, -1137, -1142, -1143, -1146,   518,
	   516,   517,   574,   522,   519,   520,   577,   521,  -140,   269,
	   258,   530,   531,   528,   309,   534,   529,   535,   532,   572,
	  -278,   536,   576,   392,   310,   295,   292,   538,   563,   564,
	   562,   539,  -534,  -539,  -540,  -541,  -542,  -543,  -544,  -545,
	  -546,  -547,  -548,  -550,  -552,  -554,   548,   280,  -595,  -596,
	  -597,   319,   260,  -855,   293,   261,   399,   566,   262,   568,
	   263,   264,   567,   271,   533,   265,   266,   267,   583,   338,
	   395,   398,   386,  -718,   393,   552,   553,   403,   404,   570,
	   405,   406,   402,  -372,   414,   537,  -919,  -949,   413,  -954,
	   441,   427,   443,   442,  -965,   424,   440,   428,  -999,   431,
	 -1005,   425, -1012,   430, -1021, -1026, -1029, -1032, -1046, -1048,
	 -1051, -1054,   454, -1064, -1068,   455, -1097,   557,   514, -1108,
	   551,   515,   511,   513,   507,   575,   512, -1139,   555,   508,
	   556,   527,   311,   541,  -555,   540,   542,   543,   565,   545,
	   547,   578,   546,   544,   573,   259,   561,  -598,   388,   332,
	   416,   415,   421,   438,   420,   437,   429,   444,   445,   447,
	   450,   446,   449,   448,   453, -1065,   451,   510,   509,   550,
	   554,   558,   559,   560,   452,    -9,   394,   589,    -9,    -9,
	    -9,    -9,   -19,   -21,   618,   486,   485, -1153,    -9,  -819,
	  -820,   290,  -673,  -826,  -827,  -209,  -215,  -457,  -670,  -671,
	  -216, -1212,   485,   499,   506,   505,    -9,  -830,  -849,   410,
	    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,
	    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,
	    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,    -9,
	    -9,    -9,    -9,    -9,    -9,    -9,   400,  -292,   463,  -104,
	  -115,  -129,  -143,  -669,   593,   486,  -670,  -671,  -104,  -115,
	  -118,  -119,  -116,   485,   431,   401,  -155,   468,  -121,  -129,
	  -147,  -216,  -124,  -121,  -315,  -129,  -317,   348,  -134,  -115,
	  -134,  -532,   485,  -124,  -104,  -129,  -121,  -104,  -182,  -183,
	  -184,  -185,  -109,  -124,  -129,   502,  -186,   462,  -115,  -121,
	  -198,  -205,  -206,   332,   338,   310,   319,   456,  -252,  -254,
	   352,  -258,  -115,  -121,  -121,  -115,  -115,  -121,  -315,  -129,
	  -319,   458,  -673,  -673,  -173,  -116,  -861,  -143,  -512, -1213,
	 -1212,  -326,   456,   411,   334,   342,   399,  -328,  -129,   361,
	  -326,  -586,  -206,  -589,  -590,  -240,  -239,  -423,  -572,  -571,
	  -237,  -129,  -301,   310,   319,   332,   289,   262,   338,   357,
	  -241,   460,  -185,  -854,   458,   485,  -238,  -129,  -209,  -124,
	  -121,  -124,  -129,  -129,  -124,  -565,   280,   485,   463,  -211,
	  -217,  -218,  -230,  -246,   485,   466,  -129,  -530,   483,   485,
	   499,  -171,  -407,  -317,   485,   315,  -663,  -664,  -646,  -647,
	  -665,   580,   581,   582,  -352,  -353,  -354,  -355,  -129,  -147,
	  -315,  -324,  -209,   486,  -457,  -514,   391,   256,  -518,  -366,
	  -522,   256,  -386,   281,   460,  -525,  -527,   281,  -706,  -708,
	  -709,  -734,   485,   499,  -801,  -457,  -795,   486,  -716,  -720,
	  -727,  -733,   301,  -753,   460,   334,  -777,   501,  -780,   348,
	   456,  -734,   390,  -782,   502,  -788,  -789,  -791,   468,  -797,
	   502,  -782,  -786,   502,  -786,  -805,   485,  -795,  -457,  -811,
	   485,   276,   399,  -366,  -834,  -835,  -673,  -907,  -512,   486,
	   476,   472,   456,  -909,  -709,  -911,  -709,  -709,  -916,  -923,
	  -924,   417,    -9,  -909,  -952,  -953,   502,   281,  -957,   502,
	  -709,  -797,  -797,  -963,   502,  -734,  -992,   502,  -995,   485,
	  -998,  -953,   502, -1001,   475,   472,   480, -1002,   485,  -963,
	  -732,  -800,   484,   488,  -512,  -809,   483,   625,   624, -1011,
	 -1013, -1014,  -929,   419, -1018,   472, -1020,   502, -1024,   479,
	  -952,  -953, -1031,   502,  -925,  -953,  -998,   502,  -998,  -953,
	  -998,  -953, -1053, -1055, -1057,   463,   477,   478,    -9,  -903,
	  -904,   298, -1067,   502,  -709, -1095,   502, -1104, -1115, -1147,
	  -809,  -756, -1213, -1104, -1107, -1110,   502,   281,   485, -1115,
	   502, -1104, -1104, -1130,   502,   502, -1138, -1110,   502,   485,
	 -1104, -1104,   485,  -141,  -144,   334,   480,   334,   485,   315,
	   337,   317,   320,   328,   399,   353,   334,   342,  -185,   346,
	  -324,  -759,  -760,   485,   499,  -216,   310,   319,   338,   332,
	   334,   290,  -535,  -536,   334,   342,   317,   320,   328,  -238,
	  -121,  -104,  -104,  -121,  -129,  -115,  -115,  -115,  -259,  -273,
	  -274,  -186,   462,   571,  -121,  -410,  -121,  -238,  -238,  -604,
	   334,  -673,   463,  -619,   337,   317,   334,   399,   342,  -630,
	   334,   342,   317,   399,  -209,  -215,  -519,  -520,  -324,  -719,
	   463,  -709,  -373,  -376,   282,  -709,   429,  -709,  -955,  -709,
	  -966,   486,   485,   472,   480,   334,   411,  -955,  -966,    -9,
	  -709,  -709,  -709,  -709,  -709,  -709,  -709,  -709,  -947,  -953,
	  -952,   502,  -709,  -709, -1104, -1104, -1104,   399,   315,  -121,
	  -709,   -20,   394, -1154, -1155,   299, -1156, -1157, -1158, -1159,
	   256, -1160, -1207,   494, -1209, -1230,   483,   493,   392,   612,
	   613,   614,   615,    -9,  -821,   486,  -512,   281,  -528,  -828,
	  -512,  -346,   363,  -523, -1216, -1217,   505,   504, -1214, -1215,
	 -1221, -1222,   485,  -364,  -383,   298,    -9,  -832,   460,   502,
	   411, -1150,   394,  -530,  -105,  -177,  -135,  -116,   501,  -112,
	  -113,   303,   311,  -107,  -108,   311,  -111,  -112,  -113,  -116,
	  -116,  -610,  -679,  -680,  -681,  -222,  -683,   485,   334,   270,
	   280,   338,   315,   360,   359,   317,   606,  -457,  -605,   385,
	  -122,  -150,  -151,  -152,  -154,  -157,  -158,  -160,  -161,  -162,
	  -164,  -165,  -166,  -167,  -168,  -169,  -170,   301,   375,   374,
	   366,   367,   368,   364,   365,   327,   371,   372,   369,   370,
	   373,  -146,   504,  -146,  -125,  -112,  -113,  -146,  -105,  -105,
	  -109,  -116,  -142,   461,  -173,  -177,  -186,  -183,  -279,   505,
	  -192,  -121,  -116,  -196,  -109,  -199,  -177,   462,   274,   463,
	  -253,  -256,   -48,   -51,   -63,   309,  -255,  -129,  -259,  -122,
	  -276,   469,  -173,  -173,  -196,  -280,  -109,  -196,  -320,  -209,
	  -215,   348,  -815,   460,   281,  -815,  -173, -1214,  -587,  -186,
	  -274,  -588,   485,  -177,  -109,  -591,   279,  -163,   483,  -591,
	  -124,  -600,  -116,  -605,  -857,  -512,   486,   623,  -177,  -250,
	  -272,  -143,  -457,  -616,  -109,  -186,  -292,  -566,  -568,  -206,
	  -570,  -129,  -423,  -240,  -239,  -237,  -571,  -572,  -301,   485,
	   463,  -631,  -212,   469,  -231,  -236,  -237,  -239,  -240,  -143,
	  -634,   465,  -638,  -129,  -317,  -640,  -254,  -642,  -644,  -112,
	  -113,  -646,  -647,  -171,  -177,  -666,   382,   384,   383,  -356,
	  -242,   458,  -401,  -146,  -152,  -146,  -401,   469,  -387,  -388,
	  -390,  -346, -1213,   294,  -528,   294,  -707,  -710,   502,  -717,
	  -745,   468,  -721,  -726,  -731,   348,  -734,  -754,  -756,   502,
	   485,  -778,   485,   499,  -732,  -783,   468,  -792,  -793,  -756,
	  -790,  -732,   485,  -798,  -799,  -512,  -800,  -801,  -806,  -807,
	  -794,  -795,   485,   348,  -812,   389,   485,   396,   386,   401,
	   485,  -367,  -356,  -836,   502,   281,   501,  -912,   338,   456,
	  -914,  -457,   486,   485,  -709,  -917,  -927,  -928,  -929,  -925,
	  -903, -1071, -1072, -1073,  -756, -1078,   294,  -958,   485,  -960,
	   501,  -964,  -969,   625,  -970,  -971,  -756,  -922,  -977,  -978,
	   468,  -993,   485,   502, -1069, -1070,  -709,   439,  -709,  -709,
	 -1004, -1006, -1007,  -972,  -973,   485,   486,   430,   419,   472,
	   480,   474,   334,   350,   313,   483,   483, -1013, -1015, -1016,
	   473,  -937,   485,  -709,  -909,   503, -1025, -1027,   459,  -952,
	   503, -1033,  -747, -1034,  -457, -1035,  -709, -1045, -1049,   468,
	 -1045,   503, -1045, -1045, -1056,  -732,   299, -1058, -1060,   479,
	  -903,  -384,    -9,  -909,   503,  -709, -1096,   460,   503, -1098,
	 -1099,   485,   486, -1101, -1100,  -457,   272, -1102, -1075, -1076,
	 -1077, -1074,  -809,  -989,  -990,  -795,   625,   624,   484,   488,
	 -1214,   460, -1111, -1112, -1073, -1078,   502, -1116, -1118,  -457,
	  -809,  -989,  -990,  -795, -1121, -1122, -1073,   502, -1128,   502,
	 -1131, -1132, -1133,   485,   486, -1135, -1136, -1123, -1133, -1140,
	 -1141,  -709,   502, -1144, -1149,  -800,   484,   488,  -457, -1128,
	  -124,  -145,  -129,  -147,   317,  -280,   347,  -515,   504,  -410,
	  -553,  -315,  -186,  -549,  -132,   344,   345,  -551,  -206,  -561,
	  -562,  -163,  -115,  -121,  -121,  -121,   390,   334,  -177,  -856,
	  -815,   269,   317,   504,  -324,   458,  -526,   469,  -719,  -374,
	  -206,  -377,   273,  -163,  -920,   486,   485,   384,   338,  -457,
	    -9,  -967,  -967,  -709,  -709,  -709,  -709,  -709,  -709,  -709,
	  -709,    -9,    -9,   503,  -709, -1109, -1118, -1109,  -556,  -709,
	   392,   504, -1161, -1176,   485,   595, -1208,   485,   486,   334,
	 -1210, -1211,   485,   499,    -9,  -822,   294,   501,  -344,  -674,
	  -675,  -676,   297,   505,  -344,   502,   485, -1218, -1219, -1229,
	   485,  -209,   485, -1223, -1219,  -384,    -9,  -831,  -847,   298,
	    -9,  -756,  -850,  -851,  -390,  -115,  -116,  -137,   485,  -137,
	  -109,   456,   485,  -177,  -223,  -171,   623,  -223,  -151,  -153,
	  -171,  -155,  -155,  -159,   457,  -156,  -163,  -226,  -227,   624,
	   625,  -156,  -163,  -129,  -129,  -126,  -127,  -129,  -294,  -131,
	  -295,  -296,  -297,  -309,  -298,  -299,  -308,   354,   337,   356,
	  -303,   359,   355,  -129,  -131,  -129,  -576,  -579,  -580,  -129,
	  -175,  -129,  -109,  -121,  -110,   503,  -121,  -142,  -129,  -173,
	  -207,  -208,  -129,  -192,  -205,  -256,  -260,  -264,  -265,   470,
	  -124,  -152,  -316,   569,  -281,  -126,  -129,  -152,   464,  -209,
	   504,   504,  -816,   281,  -523,   294,   471,  -292,  -410,  -592,
	   390,   334,   399,   485,  -410,  -590,  -238,  -109,  -109,  -330,
	   358,  -196,  -292,  -606,  -129,  -612,   483,  -457,  -129,  -173,
	  -173,  -617,  -152,  -620,  -621,  -622,  -129,  -126,  -129,  -129,
	  -567,  -186,  -274,  -177,  -569,   485,  -109,  -109,   469,  -632,
	  -530,  -213,  -219,   289,  -248,   584,   585,  -232,  -177,  -241,
	  -242,  -243,  -186,   274,  -109,  -247,   295,  -635,  -636,   485,
	  -639,  -186,  -652,  -653,  -654,  -655,  -171,   486,   483,  -317,
	  -671,  -406,  -155,   464,  -407,  -641,  -129,  -482,  -143,  -343,
	  -328,   376,   377,   378,  -661, -1212,   379,  -216,  -643,  -406,
	  -155,  -177,  -645,   485,  -645,  -664,   384,   380,   381,  -357,
	   267,  -394,  -395,  -396,  -109,  -355,  -129,  -129,  -129,  -129,
	  -129,  -222,  -424,  -425,  -430,  -431,  -109,  -432,   332,   295,
	  -389,   501,  -344,  -344, -1215,  -523,  -523,  -711,  -712,  -714,
	  -709,   334,   474,  -746,  -747,  -748,  -457,  -749,  -743,  -744,
	   485,   486,  -722,  -728,   397,  -732,  -755,   468,  -758,  -761,
	  -762,   485,   499,   501,   485,   503,   501,  -344,   623,   503,
	   501,   503,   501,   623,   463,  -813,   505, -1151,   486,   485,
	   483,  -813,  -368,  -412,   336,  -204,  -406,   503,  -673,  -838,
	  -839,  -825,   294,  -709,   460,  -912,  -918,  -944,  -928,  -930,
	  -932,  -933,   473,   481,   482,   474,  -937,   485,    -9,   503,
	   501,  -942,   461,   623,  -344,  -523,   501,  -942,  -709,   503,
	   501,  -972,  -344,  -979,  -980,  -981,  -983,   485,   486,  -942,
	  -734,   503,   501,  -942,   502,  -709,   503,   501,  -942,   502,
	  -931,   501,    -9,  -934,   486,   485,   456,  -457,  -935,  -938,
	   502,   483,  -457,   503,  -998,   503,   501,  -942,   502, -1059,
	 -1061,   459,  -952,  -905,    -2,   299,   503, -1073,   503,   501,
	   623,   623,   623,  -344,   502,   484,   488,   484,   488,  -523,
	   503,   501,  -942,  -523, -1112, -1117,   502,   503,   501,  -942,
	 -1125, -1126,   485,   485,   503,   501,  -942,   503,   501,  -942,
	 -1148,   502,   503,   501,  -942, -1141, -1145,   502,  -142,  -142,
	  -146,  -146,  -173,   334,  -759,   485,   499,  -173,  -177,  -537,
	   334,   399,   485,  -121,  -177,   485,  -121,  -816,   468,  -209,
	   485,  -375,   626,  -378,  -379,  -129,  -362,  -472,  -109,  -473,
	  -227,  -226,   485,  -474,  -475,  -500,  -502,  -453,  -121,  -477,
	   308,   333,   302,   325,   324,   300,  -225,  -478,  -479,  -480,
	  -349,  -229,  -348,  -216,  -163,  -143,   487,   489,   484,   488,
	  -481,   275,  -482,   358,  -488,  -350,  -503,   307,  -506,  -507,
	  -457,  -346,   467,   490,  -291,   306,  -505,   330,  -508,   321,
	   323,   329,  -510,  -511,   341,   340,  -921,   502,  -968,   486,
	   485,   483,  -457, -1022,  -732, -1022, -1022,  -920, -1022, -1022,
	 -1022,  -557,  -177,   400, -1162, -1164, -1165, -1166, -1167, -1168,
	 -1169, -1170, -1171, -1172, -1173, -1174, -1175, -1177,   599, -1178,
	 -1179, -1182, -1184, -1185, -1188, -1190,   461,   606,   602,   603,
	   597,   592,   607,   598,   600,   394,   504,   502,   504,  -823,
	  -824,   281,  -386,  -828,  -674,  -677, -1214,   485, -1220, -1225,
	 -1226,   459,   274,   502, -1220, -1224, -1227, -1228,   504,  -385,
	   299,  -384,    -9,  -831,   503,  -852,   501,  -344,  -109,  -109,
	  -104,  -679,  -682,  -188,  -109,  -179,   456,   411,   334,   389,
	   360,   337,   275,  -109,  -109,  -156,  -156,  -156,  -163,  -163,
	  -146,  -146,  -128,  -310,   468,  -287,  -171,  -291,   321,  -294,
	  -129,  -109,  -300,  -301,  -193,  -442,  -178,  -109,  -445,  -446,
	  -447,  -448,  -449,   276,  -362,  -452,  -466,   313,  -129,   360,
	  -109,   360,  -130,   345,   344,  -132,  -110,  -177,  -223,  -173,
	  -176,  -109,  -178,  -183,  -109,  -193,  -110,  -177,  -196,  -261,
	  -235,   343,  -266,  -267,   485,  -282,  -421,  -334,  -109,  -422,
	  -423,  -116,   349,  -110,  -177,  -282,  -321,   390,  -323,  -209,
	  -209,   485,  -523,   294,  -270,  -129,  -328,  -593,  -407,  -104,
	   362,  -410,   485,  -110,  -238,  -238,   339,  -331,  -332,  -334,
	   339,  -601,  -129,   263,  -146,   501,  -618,  -242,   485,  -110,
	  -177,  -142,  -625,  -109,  -242,  -573,   390,  -410,   485,   399,
	   334,  -238,  -570,  -238,  -238,  -530,  -214,  -109,  -121,   399,
	  -121,  -236,  -238,  -143,  -163,  -121,  -651,  -655,  -656,  -171,
	  -163,  -112,  -113,  -129,   485,  -171,  -651,  -658,  -659,  -171,
	  -658, -1215,  -648,  -649,  -171,  -648,  -644,  -109,  -109,  -358,
	  -361,  -208,  -177,  -397,   326,  -282,  -395,  -109,  -401,  -401,
	  -426,  -412,  -165,  -367,  -282,  -433,  -376,   332,  -388,   503,
	  -713,   501,   623,  -750,   501,   623,  -723,  -729,  -736,   407,
	   457,   483,  -457,   503,   501,  -763,   485,   593,   485,   499,
	   623,  -793,   623,  -799,  -807,  -808,  -809,  -795,  -457,   485,
	  -813, -1151,   485,   625,  -283,  -165,  -282,   456,   273,  -392,
	  -407,  -193,  -222,  -457,  -837,   623,   503,   501,  -523,  -523,
	   460,  -945,  -946,   418,  -931,   501,   472,   480,  -934,  -934,
	  -934,  -936,   485,   486,  -926,  -903, -1072,  -709,   422,  -958,
	  -709,  -970,  -942,   501,  -982,   502,   623,  -709,   501, -1070,
	  -845,  -846,  -512, -1075, -1076, -1077, -1074,   275,  -709, -1007,
	 -1008,   486,   483,   484,   488,   338,   384,   485,  -457,   462,
	   503,  -974,   486,   485,   483,   481,   482,   426,   473,   440,
	   480,   472,  -457,  -903, -1016,  -935,  -935,  -939,  -940,  -941,
	   485,   486,   430,   419, -1035, -1036, -1004,  -998,    -9, -1099,
	 -1100,  -457, -1100, -1100, -1073, -1112,  -709,   503, -1119, -1147,
	 -1122, -1123,   503,   501,  -942,  -942, -1132, -1102, -1136, -1102,
	 -1104, -1141, -1102,   503,   485,  -143,  -143,  -129,  -129,  -315,
	  -538,  -563,  -206,  -163,  -558,  -559,  -560,  -562,  -177,  -223,
	  -146,  -380,  -471,   301,  -340,  -342,  -226,  -227,  -341,   626,
	   627,   628,  -362,  -362,  -362,  -340,  -501,   273,   456,  -109,
	  -501,   502,   502,  -489,  -491,  -362,  -497,  -498,  -235,  -110,
	  -362,  -504,   485,  -143,  -109,  -109,  -344,  -344,   502,   486,
	  -109,  -276,  -922,  -410, -1150, -1163,   504, -1165,   346,   610,
	   590,   416,   317,   609, -1177, -1183,   323,   329, -1183, -1186,
	   343, -1177, -1191, -1194,   483, -1187,   608,   461,   485,   485,
	  -758, -1210,   485,   499,    -9,  -825,  -387,  -391,   502, -1225,
	 -1219, -1227, -1219,    -9,  -848,   299,  -851,  -138,  -684,  -109,
	  -115,   456,   360,  -222,  -138,  -110,  -672,  -530,  -143,  -129,
	  -147,  -457,   485,   499,  -138,  -689,  -690,  -691,  -171,  -109,
	  -163,  -129,  -129,  -770,  -288,  -293,   268,  -289,  -171,  -290,
	  -109,  -764,   586,   587,   588,  -765,  -766,  -767,  -768,  -769,
	  -310,   468,   276,  -773,  -703,  -775,  -776,  -131,   358,  -772,
	  -298,  -300,  -297,  -171,  -238,  -121,  -443,  -444,   617,   616,
	  -193,  -193,  -454,  -362,  -469,  -423,  -450,  -458,  -178,  -462,
	  -467,   618,   619,   623,   620,   621,   622,   277,   276,   305,
	  -464,  -142,   459,  -450,   276,  -109,  -146,  -238,  -577,   460,
	   458,  -580,  -581,  -171,  -143,  -583,  -163,  -179,  -110,  -187,
	  -189,  -129,  -208,  -200,   463,  -262,  -159,  -268,  -269,   332,
	   310,   319,   338,   388,   393,  -177,  -173,  -412,  -282,  -367,
	  -373,   350,  -152,  -285,  -286,  -126,  -127,  -283,  -322,  -193,
	  -222,   274,  -204,  -523,  -173,  -104,  -104,  -104,  -110,  -110,
	  -109,  -283,  -188,  -607,  -223,  -129,   485,  -173,  -623,  -121,
	  -129,  -143,  -621,  -143,  -617,  -626,  -129,  -270,  -410,   485,
	  -110,  -110,  -220,  -221,  -222,  -129,  -233,  -244,   485,  -110,
	  -163,  -651,  -109,  -657,  -530,  -457,  -177,  -223,  -177,  -223,
	  -138,  -138,  -359,  -360,  -177,   464,  -204,  -223,  -395,  -398,
	   322,   318,   314,   323,   329,  -110,  -110,  -282,  -129,  -129,
	  -427,  -369,  -415,   284,  -430,  -159,  -110,  -374,  -163,  -712,
	  -709,  -749,  -751,   486,   485,   483,  -457,  -724,  -730,   579,
	  -737,   408,   623,  -735,  -761,  -289,   502,   485,  -784,   483,
	  -512,  -794,  -344,   485,  -369,  -159,  -393,  -411,   316,  -159,
	   348,  -840,  -841,  -843,   272,  -839,  -673,  -523,  -947,  -903,
	  -932,  -934,  -934,  -935,   502,   503,  -980,  -985,  -986,  -987,
	   485,   486,   350,  -984,   485,   486,   503,  -996,  -756,  -344,
	   503,  -344,   503,   503,   501,  -942, -1037, -1040,   593,   486,
	  -457,   485,   503,   503,  -942, -1126, -1118, -1144,   503,  -942,
	  -146,  -146,  -177,  -121,  -121,  -121,  -379,  -362,  -340,  -508,
	  -129,  -472,  -110,  -110,  -409,  -476,  -109,  -227,  -226,  -475,
	  -501,  -409,  -490,   299,  -492,  -493,  -495,  -235,  -494,  -498,
	  -499,   312,  -193,  -110,  -177,  -464,   458,  -143,  -362,  -362,
	  -512,  -483,  -484,   485,  -509,  -129,    -9, -1180,   605, -1187,
	 -1189,   483,   624,   625,   484,   488,   486,   485,   610,   611,
	 -1187, -1192, -1201,   591, -1195, -1196,   604,   463,   503,  -364,
	  -523,   503,    -9,  -110,  -177,  -190,  -678,   485,   591,  -685,
	  -225,  -110,  -668,  -129,  -146,  -146,  -146,  -110,  -110,  -171,
	  -692,  -186,  -690,  -110,  -771,   485,   317,  -679,  -109,  -171,
	  -289,  -171,  -109,  -171,  -163,  -765,   275,   358,   351,   275,
	   358,   351,  -774,  -156,   275,  -482,   486,   488,   484,  -349,
	  -129,  -774,  -776,   276,  -292,  -110,  -109,  -302,  -304,   462,
	  -193,  -193,  -110,  -110,  -356,  -470,  -206,  -377,  -362,  -206,
	  -453,  -452,  -109,  -459,  -460,  -461,   304,   331,  -458,  -464,
	  -109,  -452,  -362,   277,  -468,  -243,  -178,  -179,   485,  -362,
	  -334,  -110,  -578,  -143,  -457,   485,  -582,  -155,  -171,  -583,
	   502,  -129,  -143,  -179,  -110,  -177,  -110,  -177,  -190,  -201,
	   466,  -209,   456,   486,  -263,  -271,   566,  -270,  -177,  -267,
	  -421,  -110,  -374,  -282,  -177,  -128,  -284,  -369,  -324,  -333,
	  -335,  -336,  -337,  -338,  -109,  -227,  -226,  -343,  -345,  -346,
	  -225,  -347,  -348,  -129,  -349,  -350,  -284,  -602,  -186,  -163,
	   485,   623,  -146,  -146,  -618,  -110,  -177,  -190,  -549,  -110,
	  -177,  -223,  -223,  -234,  -204,  -235,  -245,   485,  -104,  -657,
	  -659,  -660,  -657,  -317,   411,   334,  -143,  -649,  -650,   270,
	  -143,  -163,  -227,  -206,   358,   485,   269,   257,  -457,  -109,
	  -110,  -110,  -361,   390,  -362,  -396,  -109,   322,   322,   322,
	   322,  -400,  -401,  -110,  -428,  -370,  -417,   395,  -416,  -163,
	  -229,  -413,  -414,  -409,  -434,   626,  -435,  -436,  -129,  -362,
	  -725,  -741,   485,   623,   623,  -738,  -719,   483,  -370,  -413,
	  -193,  -408,  -409,   504,  -842,   502,  -844,  -845,  -846,  -837,
	    -9,  -934,  -709,   503,   501,   623,   503,  -344,  -942,  -940,
	  -943,   486,   483,  -457, -1038,   502,   411,   503, -1104,  -129,
	  -129,  -163,  -471,  -110,  -409,  -409,  -409,  -409,  -110,  -494,
	  -495,  -362,  -362,  -496,   335,  -362,  -362,  -362,  -242,  -152,
	  -344,  -485,  -109,  -146, -1181,   601,   483,   484,   488,   483,
	   484,   488,   485,   485, -1193, -1205,   596, -1202,   360, -1194,
	 -1150,  -684,  -110,  -177,  -143,  -129,  -129,  -143,  -129,  -109,
	  -693,  -694,  -695,  -697,  -698,  -163,  -605,  -238,  -110,   317,
	   623,  -610,  -289,  -171,  -163,  -289,  -110,  -177,  -146,  -171,
	  -238,  -305,  -306,   310,   338,  -204,  -362,  -451,  -456,   278,
	  -452,  -109,  -454,  -362,  -459,  -463,  -362,  -109,  -468,  -243,
	  -451,  -110,  -173,  -179,  -129,   483,  -176,  -163,  -188,  -189,
	  -202,   458,   485,  -121,  -269,  -375,  -283,  -286,  -370,  -110,
	  -177,  -339,  -340,  -341,  -342,  -226,  -227,  -338,  -336,  -337,
	  -338,  -338,  -344,  -344,  -110,  -338,  -603,  -406,  -155,  -608,
	  -609,  -129,  -612,  -143,  -143,  -129,  -173,  -129,  -221,  -224,
	  -225,  -226,  -227,  -228,  -121,  -229,  -129,  -224,  -193,  -152,
	  -110,   485,  -163,  -138,   274,  -399,  -186,  -404,   281,  -129,
	  -429,  -276,  -418,  -171,   475,  -177,  -190,  -177,  -146,  -223,
	  -152,  -742,  -719,   623,  -740,  -739,  -709,   503,   501,  -371,
	  -155,  -177,  -673,  -512,   503,  -986,  -988,  -809,  -795,  -989,
	  -990,  -457,   485,   390,   358,  -922,  -845, -1039,   468, -1041,
	   276,   483,   503,   503,  -476,  -110,  -110,  -496,  -362,  -110,
	  -177,  -110,  -110,  -362,  -287,   503,  -486,  -292,  -163,  -129,
	   593, -1206,   457, -1177, -1195,  -190,  -177,  -225,  -690,  -177,
	  -696,  -155,  -696,  -696,  -171,  -699,  -688,  -684,  -686,  -109,
	   485,  -110,  -289,  -110,  -290,  -289,  -163,  -289,  -109,  -110,
	  -304,  -307,   345,   344,   267,   485,  -392,  -143,  -457,  -110,
	  -443,  -362,  -110,  -177,  -468,  -109,  -109,   503,   501,  -110,
	  -173,  -203,   462,   483,  -272,  -284,  -335,  -337,  -109,  -110,
	  -110,  -177,  -610,  -610,  -177,  -190,  -190,  -225,  -225,  -109,
	  -129,  -110,  -324,  -193,  -109,  -402,  -109,  -437,  -171,  -416,
	  -414,  -436,  -340,  -362,  -129,   501,  -709,  -709,  -289,   483,
	  -381,  -382,  -171,  -409,   503,  -344,   275, -1042,   468,   275,
	   503,   501,  -362,  -362,  -110,  -276,  -288,  -487,   485,  -110,
	  -177, -1203, -1200,   485, -1203, -1197, -1198,   594,  -109,  -110,
	  -694,  -171,  -699,  -684,  -667,   339,  -687,  -684,   334,  -289,
	  -171,  -110,  -163,  -302,  -305,   275,   485,  -393,  -362,  -443,
	  -465,  -362,  -110,  -468,  -584,  -225,  -227,  -226,   483,  -176,
	  -204,   485,   505,  -338,  -338,  -609,  -110,  -224,  -245,  -405,
	  -129,  -403,  -129,  -237,  -419,   411,   350,  -719,   503,  -177,
	  -109,   503,   358,   483,  -110,  -362,  -110,  -109,  -163, -1200,
	 -1204,   501, -1199,   462,  -685,  -223,  -109,  -455,   618,   619,
	   621,   622,  -110,  -177,   485,  -289,  -289,  -110,  -362,  -177,
	  -110,  -110,  -225,  -225,   503,   463,   483,  -110,  -110,  -177,
	  -110,  -177,  -152,  -110,  -177,  -110,  -177,  -243,  -420,   291,
	  -289,  -382,  -171,   503,  -110,  -163,  -110, -1200, -1200,  -110,
	  -686,  -700,  -702,  -703,  -701,   358,  -225,  -109,  -701,  -684,
	  -289,  -362,   485,   463,  -338,  -224,  -129,  -129,  -129,  -438,
	  -439,  -440,  -441,  -129,  -206,  -177,  -110,  -110,  -177,  -704,
	  -225,   483,  -110,  -110,  -177,  -440,  -129,  -146,  -171,  -702,
	  -110,  -177,   505,  -439,  -110,  -225,   483
};

short yydef[] = {
	     2,    -2,     1,     3,  2237,     4,     5,     6,     7,     0,
	     0,     0,    11,    12,    13,    14,    15,     0,     0,     0,
	    19,  2251,  2240,    25,    26,    27,    28,    29,    30,    31,
	    32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
	    42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
	    52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
	    62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
	    72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
	    82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
	    92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
	   102,   103,   104,     0,     0,     0,  1651,     0,     0,     0,
	     0,     0,  1679,     0,     0,     0,  1683,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  1701,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,  1711,  1712,     0,     0,  2113,  2114,
	  2115,  2116,  2117,  2118,  2119,  2120,  2121,  2122,  2123,  2124,
	  2125,  2126,  2127,    20,     0,    -2,     0,     0,     0,     0,
	     0,     0,     0,   138,   138,   931,   149,   835,     0,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,   302,
	     0,     0,   334,     0,     0,     0,     0,   412,     0,     0,
	  1323,  1667,  2369,     0,   836,     0,   430,   431,   963,     0,
	  1004,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,   901,  1089,     0,     0,     0,     0,     0,
	     0,     0,     0,   510,   825,     0,  1413,  1464,  1524,  1535,
	     0,  1544,     0,     0,     0,     0,     0,     0,  1804,  1604,
	   510,     0,  2369,  1724,     0,     0,     0,     0,  1755,  1805,
	  1806,  1807,     0,     0,  1813,     0,  1818,     0,     0,     0,
	     0,     0,     0,  1908,  1913,     0,  1919,  1922,     0,  2369,
	     0,  1959,  1966,  1970,     0,  1984,  2055,     0,     0,     0,
	     0,     0,  2049,     0,     0,  2369,  2369,     0,     0,  2369,
	     0,  2369,  2369,     0,     0,     0,     0,  2369,  2369,   107,
	   109,   116,   118,   126,   128,   137,   144,     0,     0,   833,
	     0,   159,   185,   192,     0,   207,   209,   303,   161,   333,
	     0,   393,   396,     0,   410,     0,  1594,  1601,  1669,  1670,
	     0,   420,   856,   840,   841,   842,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,   422,   965,  1005,  1006,
	     0,   433,  1019,     0,  1660,  1042,   288,  1044,  1050,  1078,
	  1080,  1084,  1083,   899,     0,  1097,   930,  1106,  1194,   469,
	    -2,     0,  1395,     0,  1530,  1537,  1543,  1560,  1568,  1541,
	  1574,  1575,  1585,   485,  1721,  1731,     0,     0,  1812,  2075,
	  1820,  1824,  1826,  1874,     0,  1876,  1906,  1911,  2075,  1921,
	     0,  1945,     0,  1958,     0,     0,     0,     0,     0,     0,
	     0,     0,  2038,  2065,     0,  2005,     0,  2146,  2148,  2369,
	  2157,  2159,  2163,  2168,  2173,  2369,  2181,  2369,  2193,  2195,
	  2198,   152,   855,   860,     0,   868,   866,   889,   890,   891,
	   892,   893,   894,   869,   895,  1009,  1014,  1010,  1421,   483,
	  1744,  1810,  1817,  1830,  1918,  1930,  1948,  1963,  1969,  1978,
	  1981,  2009,  2013,  2020,  2023,     0,  2046,  2130,  2131,  2151,
	  2187,  1011,  1012,  1013,  2043,     8,  2235,  2236,     9,    10,
	    16,    17,     0,    21,    22,    23,    24,     0,   105,  1602,
	     0,  2369,  1613,  2369,  2379,    -2,     0,  1308,  1278,  1279,
	   239,  2387,  1276,  1277,     0,  2367,   506,     0,     0,     0,
	  1674,  1675,  1676,  1677,  1678,  1680,  1681,  1682,  1684,  1685,
	  1686,  1687,  1688,  1689,  1690,  1691,  1692,  1693,  1694,  1695,
	  1696,  1697,  1698,  1699,  1700,  1702,  1703,  1704,  1705,  1706,
	  1707,  1708,  1709,  1710,  1713,  1714,     0,     0,  1259,     0,
	  1289,  1287,  1288,  1271,  2231,  1286,  1274,  1275,     0,     0,
	   117,   119,   121,   122,   123,   124,     0,  1267,   163,   569,
	     0,  1183,     0,   934,   935,   401,     0,   402,     0,   139,
	     0,   832,   834,     0,     0,     0,   191,  1323,   193,   196,
	   197,     0,     0,   336,   401,  1230,     0,  1244,     0,   223,
	     0,   212,   214,   215,   216,   217,   218,  1197,   289,     0,
	   293,     0,   301,   163,     0,  1323,  1323,   223,   223,   401,
	     0,   413,     0,     0,  1666,  1322,  1323,  1672,  1673,  2387,
	  2368,   419,   423,   424,   425,   426,   427,   428,   429,  1174,
	   421,   995,   966,   968,   969,   971,   972,   974,   975,   976,
	    -2,   979,    -2,   986,   987,   988,   989,   990,   633,   388,
	     0,  1228,  1020,  1658,  2369,     0,  1041,  1272,   285,  1045,
	  1054,  1077,     0,     0,  1081,     0,     0,  1090,  1091,   281,
	   240,   241,     0,     0,   260,   279,  1096,   929,  1292,  1293,
	  1294,  1101,     0,  1111,  1282,  1223,  1184,  1185,  1187,  1188,
	     0,  1137,  1138,  1195,   468,   471,   472,   522,   401,     0,
	    -2,   797,   820,   821,   822,   798,     0,   806,   807,   814,
	   815,   816,  2369,     0,   512,   818,  2369,   828,     0,  1396,
	  1397,  2083,  2084,  2085,  2086,  2087,  2081,  2080,  1445,  1411,
	  1414,     0,  1425,  2369,  1463,     0,     0,     0,  1529,  2369,
	  1532,  1533,  1534,  1538,  2369,  1542,  2369,  1546,     0,  1559,
	  2369,  1567,  1540,     0,  1569,  1570,  1571,  1572,  1573,     0,
	  1586,     0,  1627,   522,  1625,  1629,  1628,  1720,  1722,  1723,
	  1725,  1728,  1726,  1727,  2051,  1738,  1732,     0,  1760,  1753,
	  2055,  1756,     0,  1811,  1814,  1815,  2369,  2111,  1819,     0,
	  2079,  1825,  1873,  2369,  1829,  1878,  1905,     0,     0,  1912,
	  1915,  1916,     0,  1920,  1923,     0,     0,  1926,  1927,     0,
	  1944,  2101,  2102,  2103,  2104,  2091,  2088,     0,     0,  1946,
	  1949,     0,  1952,  1764,  1957,     0,  1961,     0,  1972,     0,
	  1975,  1976,  1979,     0,  2014,  2014,  2053,     0,  2014,  2014,
	  2017,  2018,  2021,  2369,  2031,  2026,  2029,  2030,     0,  2039,
	  1717,     0,  2044,     0,     0,  2143,     0,  2145,  2200,  2201,
	  2202,  2204,  2387,     0,  2149,  2152,  2369,  2111,     0,     0,
	  2369,     0,     0,  2174,     0,     0,  2185,  2188,     0,     0,
	     0,     0,   145,     0,     0,   153,   154,   933,  1458,   183,
	     0,   195,   205,   211,   287,   291,   335,   397,   223,     0,
	   800,  1466,     0,    -2,    -2,  1472,   411,   434,   470,   484,
	  1733,  1671,     0,     0,   857,   858,   859,   870,   871,   839,
	   844,   843,   845,   846,   847,   848,   849,   926,     0,     0,
	     0,     0,  1244,   330,   852,   854,   565,  1007,  1008,  1017,
	  1018,  1661,     0,  1049,     0,  1072,  1051,  1052,  1053,  1082,
	  1085,  1086,  1087,  1088,    -2,     0,     0,     0,   823,  1408,
	     0,  1440,   488,     0,   487,  1750,     0,  1809,  1816,  2074,
	  1837,  1831,  1832,  1833,  1834,  1835,  1836,  1917,  1837,  1947,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	  2063,  2369,     0,  2129,  2208,  2175,  2208,   863,   867,   861,
	     0,    18,  2238,  2248,  2252,     0,  2253,  2254,  2255,  2256,
	     0,  2279,     0,  2355,     0,  2403,  2274,  2356,     0,  2404,
	  2405,  2406,  2407,  1603,  1609,  1607,  1608,  1616,  1614,  1618,
	  1309,  1309,     0,  1615,  2375,     0,  2378,     0,  2373,  2374,
	  2384,     0,  1182,   480,  1717,     0,  1647,     0,  2369,  2369,
	  1653,  2249,  2239,   830,   106,     0,   140,   142,  1208,     0,
	     0,  1196,  1213,   108,     0,  1213,     0,   113,   114,   115,
	   120,  1324,  1325,  1327,  1329,  1331,  1332,  1333,  1334,  1335,
	  1336,  1337,  1338,  1339,  1340,  1341,  2234,  1305,     0,  1248,
	   125,   162,   164,     0,     0,     0,     0,     0,     0,     0,
	   173,   174,   175,   176,   177,   178,   179,  1199,  1256,  1251,
	  1226,  1231,  1233,  1206,  1210,  1247,  1237,  1238,  1235,  1236,
	  1239,     0,  1212,     0,   127,     0,     0,     0,   136,   143,
	   937,   158,     0,  1229,   182,     0,     0,     0,     0,  1209,
	     0,   556,   206,  1323,     0,     0,     0,   219,   220,   221,
	   290,   295,   297,   298,   299,     0,   292,   294,   305,   160,
	     0,  1221,   392,   395,     0,     0,     0,     0,   404,    -2,
	     0,     0,  1598,  2379,     0,  1600,  1668,  2372,     0,     0,
	     0,   967,   983,     0,     0,     0,   985,   980,  1291,     0,
	   223,     0,  1016,     0,  1663,  1664,  1665,     0,     0,  1323,
	  1323,  1046,  1047,     0,     0,     0,     0,   921,   902,   903,
	   906,   908,   909,   910,   911,   912,   914,   915,     0,     0,
	  1093,     0,     0,     0,     0,   263,   265,   267,   268,     0,
	     0,  1098,  1139,  1103,     0,  1105,     0,  1120,  1109,     0,
	     0,  1114,  1115,  1116,     0,  1189,     0,     0,  1193,     0,
	     0,  1222,     0,     0,   554,     0,     0,     0,   511,   513,
	  1309,  1309,  2387,  2379,   826,  2379,  1394,     0,  1399,  1407,
	     0,  1446,  1415,  1412,  2369,  1423,  1424,  1462,  1459,     0,
	  1522,  1523,  1525,  1527,  1531,  1536,     0,     0,  1551,  1309,
	  1545,  1549,  1548,     0,  1562,  1564,  1565,  1566,     0,  1577,
	     0,  1554,  1555,  1556,  1584,  1588,     0,     0,     0,  1592,
	  1587,   575,   560,  1626,     0,  1634,     0,     0,  1734,  1735,
	  1736,  1737,  1739,  1740,  1738,  1800,  1759,  1761,  1775,     0,
	  1808,     0,  2067,     0,  1309,  2379,  2112,     0,     0,     0,
	  2078,     0,  1843,  1844,  1845,     0,  1309,  1875,  1877,     0,
	  1880,     0,  1909,     0,     0,  2057,     0,     0,  1924,     0,
	     0,  1931,  1932,     0,  1849,  1852,  1853,  1854,  1855,  1856,
	  1857,  1858,  1859,  1860,  1861,  2089,  2090,  1950,     0,  1953,
	     0,  1956,  1787,  1960,     0,  1965,  1967,     0,  1974,  1971,
	  1982,     0,  1985,  1986,    -2,  1987,     0,  2006,     0,  2016,
	  2007,  2054,  2010,  2011,  2024,  2027,  2028,  2034,     0,  2033,
	  2037,     0,  1716,     0,  2048,  2004,  2128,  2369,  2132,     0,
	  2134,     0,  2080,     0,  2139,    -2,     0,  2142,  2223,  2224,
	  2225,  2226,  2092,  2096,  2100,  2082,     0,     0,  2093,  2097,
	  2370,  2379,     0,  2154,     0,  2379,  2369,     0,  2160,  2210,
	  2211,  2212,  2213,  2214,     0,  2164,     0,     0,  2172,     0,
	     0,  2177,     0,  2228,  2229,     0,  2182,     0,  2206,     0,
	  2190,     0,     0,     0,  2207,  2218,  2219,  2220,  2221,  2197,
	     0,     0,     0,     0,   194,  1323,     0,   799,     0,  1323,
	   853,   896,     0,   850,   925,   927,   928,   851,   879,   880,
	   881,   883,   884,   326,   327,   328,     0,   331,     0,  1659,
	  1598,     0,  1071,     0,   808,   809,   819,     0,  1409,     0,
	   489,   490,   491,   486,  1752,  1745,  1746,  1747,  1748,  1749,
	  1743,  1828,  1929,    -2,    -2,    -2,  1750,    -2,  2012,    -2,
	  2022,  2040,  2041,  2064,    -2,  2150,  2209,  2186,   864,  2042,
	     0,  2257,    -2,  2276,  2277,  2278,     0,  2357,  2358,     0,
	  2361,     0,    -2,    -2,  1605,     0,  1617,  2369,  1620,  1310,
	     0,  2387,  1314,  1313,  1621,     0,  2376,  2377,  2380,  2400,
	  2402,  1307,  2385,  2386,  2388,     0,   508,  1622,  1717,     0,
	  1647,  1624,     0,  1654,  1309,  1290,   141,     0,   148,     0,
	     0,   111,   112,     0,  1316,  1330,  1216,     0,   165,   166,
	   180,     0,     0,     0,  1205,   170,   791,     0,     0,  1249,
	  1232,   171,   172,   568,   571,   129,   390,     0,   362,     0,
	   364,   365,   366,     0,     0,     0,     0,   387,   384,     0,
	     0,   389,     0,   133,     0,   400,     0,   936,   938,     0,
	  1323,   189,     0,     0,   198,  1252,   337,     0,   557,   208,
	     0,   224,   572,   223,   213,   296,   312,   304,     0,   307,
	   332,     0,     0,     0,     0,   342,   572,     0,   405,   407,
	     0,     0,  1593,     0,  1595,  2379,     0,     0,   991,     0,
	     0,   996,   997,    -2,   992,   970,     0,     0,     0,   432,
	     0,     0,     0,  1021,  1022,     0,  1039,  1040,  1273,   286,
	  1043,  1063,     0,     0,  1056,  1058,  1060,  1061,  1073,  1079,
	     0,     0,     0,     0,   904,   905,     0,     0,   900,     0,
	  1092,   244,     0,     0,   282,   283,   284,     0,     0,   269,
	   270,   271,   272,  1242,     0,     0,   280,  1095,     0,  1100,
	  1102,  1140,  1159,  1143,  1144,  1145,  1280,  1147,  1148,  1159,
	  1281,     0,     0,  1263,  1104,  1107,  1155,  1156,  1157,  1158,
	  1175,  1176,  1177,  1178,  1179,  2387,  1180,  1181,  1108,     0,
	     0,     0,     0,  1117,     0,  1186,  1190,  1191,  1192,     0,
	  1254,   521,    -2,   525,     0,   534,   547,    -2,    -2,   553,
	   803,   804,   805,   617,   600,   522,     0,   607,   605,     0,
	  2369,   515,   516,   517,  2382,   817,   827,     0,  1400,     0,
	  1403,  1404,  1405,  1444,  1447,  1448,  2050,  1449,     0,  1441,
	  1442,  1443,  1417,  1416,     0,  1422,  1457,     0,     0,  1473,
	     0,  1478,  1479,     0,     0,  1550,  2369,     0,  1547,  1561,
	  2369,  1576,     0,     0,     0,  1590,     0,  2242,  2243,  2244,
	  2245,  1591,   577,     0,  1264,   562,     0,  1630,     0,     0,
	  1636,  2379,  1635,  2052,  2379,     0,  1741,  1802,  1762,     0,
	  1766,     0,     0,     0,     0,     0,  1774,    -2,  1758,  2066,
	  2369,     0,  2076,  2077,  2071,  2110,     0,     0,  1823,  1827,
	  2369,  1847,     0,  1879,  1881,  1890,     0,  1885,  1886,     0,
	     0,  2056,     0,  2369,     0,  1925,  1928,     0,     0,     0,
	     0,     0,  1765,  1787,  1777,  1778,  1779,  1780,  1783,  1787,
	     0,  1785,  1786,  1964,  1973,  1983,     0,  1989,     0,  2025,
	     0,  2036,  2032,  1715,  1718,     0,  2047,  2144,  2133,     0,
	     0,     0,     0,  2222,  2369,  2094,  2098,  2095,  2099,  2147,
	  2153,  2369,     0,  2215,     0,  2158,  2369,  2162,  2369,     0,
	     0,  2169,     0,     0,  2176,     0,     0,  2180,     0,     0,
	  2227,  2369,  2189,     0,     0,     0,  2194,     0,     0,     0,
	     0,     0,   338,   399,  1467,  1468,  1469,   837,     0,     0,
	   872,   874,    -2,   878,     0,   329,   566,  1662,  1038,   237,
	   824,   482,   492,   493,   494,   569,    -2,   692,     0,   699,
	     0,     0,    -2,   702,   703,   765,     0,   765,   710,   711,
	     0,   759,   760,   761,   762,     0,   712,   713,   714,   715,
	   716,   717,   718,    -2,   719,   720,   721,   722,   723,   724,
	   725,   726,   727,   728,     0,     0,     0,     0,   783,   784,
	  1309,  1309,     0,   729,     0,  1207,     0,  1257,     0,    -2,
	   779,   780,   789,   790,  1234,  1211,  1878,  1751,  1838,  1839,
	  1840,  1841,  1842,  1962,  2072,  1968,  1977,  1980,  2008,  2019,
	  2045,   862,     0,     0,     0,    -2,  2261,  2263,  2264,  2265,
	  2266,  2267,  2268,  2269,  2270,  2271,  2272,     0,  2282,     0,
	  2291,  2302,  2302,  2308,  2291,     0,  2290,  2288,  2293,  2299,
	  2304,  2306,  2311,  2327,     0,  2354,     0,     0,     0,  1606,
	     0,  1634,  2369,  1619,  1311,  1312,  2371,     0,  2381,  2391,
	     0,  2394,  2395,  2401,  2389,  2390,  2396,     0,  2399,   507,
	     0,     0,  1649,  1623,  1652,  2369,  1656,  1657,     0,     0,
	     0,  1326,  1328,     0,     0,  1344,  1345,  1346,  1347,  1348,
	  1349,  1315,  1241,     0,     0,   167,   168,   169,   792,   793,
	     0,     0,   130,   391,  1491,   360,  1482,     0,  1227,   363,
	  1517,     0,   368,     0,   383,   634,     0,     0,   639,   640,
	   641,   642,   643,  1240,     0,     0,     0,  1218,   569,   385,
	     0,   386,   131,   134,   135,   132,     0,     0,     0,   184,
	   186,     0,     0,     0,     0,   204,   222,     0,     0,   322,
	     0,  1262,   306,   308,   310,  1323,   593,   595,     0,   522,
	   485,   398,     0,   339,     0,   577,     0,     0,   560,   415,
	   416,  1599,  1596,  2379,  1323,  1287,     0,  1001,     0,  1003,
	  1250,   993,   994,   973,     0,     0,   435,   436,     0,   577,
	   439,  1316,  1026,  1025,     0,     0,  1323,     0,  1062,  1055,
	     0,     0,     0,     0,     0,     0,     0,   920,    -2,   923,
	   924,   919,   907,     0,     0,  1094,   235,     0,   242,   243,
	   273,   264,     0,   278,     0,  1159,  1142,  1160,     0,  1151,
	  1154,  1171,  1172,  1173,    -2,  1149,  1150,  1161,  1163,     0,
	  1162,  2383,  1118,  1121,  1124,  1119,  1110,     0,     0,   560,
	   477,     0,     0,     0,  1318,     0,  1317,     0,     0,     0,
	   583,     0,     0,   602,     0,   488,     0,   606,   514,  1398,
	     0,  1406,     0,     0,  1451,     0,  1419,  1418,     0,     0,
	  1426,  1460,  1461,  1465,     0,  1482,  1480,  2232,  1526,  1528,
	  2369,  1552,     0,  1563,  1578,  1579,  1580,  1581,  1309,  1583,
	  1589,  2241,  2246,     0,   583,     0,   576,  1265,  1266,   574,
	     0,   558,   559,    -2,  1643,  1639,  1632,     0,  1633,  1729,
	  2379,  1799,  2065,  1803,     0,  1775,     0,     0,  1787,  1770,
	  1771,  1773,  1781,  1782,  1754,  1757,  2068,  2069,     0,     0,
	  1822,  1846,  1848,     0,  1883,     0,     0,     0,  2369,  2058,
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
	     0,     0,    -2,    -2,     0,     0,   794,   795,  2369,     0,
	   778,     0,     0,   865,  2250,  2258,  2273,  2262,  2280,  2281,
	  2283,  2284,  2285,  2286,     0,  2298,  2300,  2301,  2303,     0,
	  2307,     0,  2340,  2333,  2330,  2331,  2309,  2287,  2275,  2359,
	     0,  2362,  2363,  2364,   506,  2379,  1612,     0,   519,  2392,
	  2393,  2397,  2398,   509,  1648,     0,  1655,     0,  1319,     0,
	  1353,  1354,  1355,  1356,     0,   110,  1269,  1297,  1298,     0,
	     0,  1304,    -2,    -2,     0,     0,  1365,  1366,  1370,     0,
	     0,   567,   570,     0,   348,     0,  1268,   349,  1482,     0,
	     0,  1483,  1283,  1284,  1285,  1484,  1486,  1487,  1488,  1489,
	  1490,  1491,     0,  1499,     0,  1502,  1515,     0,  1504,  1518,
	  1519,  1520,  1521,     0,     0,   372,     0,     0,  1198,  1246,
	   637,     0,     0,     0,   522,   688,     0,   666,     0,     0,
	   678,   649,   650,   651,   652,   653,   654,   655,  1240,  1202,
	   673,     0,  1225,     0,     0,     0,     0,     0,     0,   958,
	   959,   939,   952,   941,   942,   943,     0,   190,   199,     0,
	   201,  1319,   225,   230,     0,   324,     0,   311,   313,   315,
	   316,   317,   318,   319,   320,     0,   394,     0,     0,   597,
	   488,   403,     0,   343,   344,   346,   390,   583,   406,   417,
	   418,     0,   408,  1597,   964,   999,  1000,  1002,   978,   981,
	     0,   583,  1030,  1024,     0,  1023,     0,  1048,  1064,  1065,
	   569,  1067,  1057,  1059,  1063,     0,  1319,   926,   917,   918,
	   913,   916,     0,   246,     0,     0,   560,     0,   276,   266,
	  1099,  1141,     0,  1152,  1295,  1296,     0,     0,     0,  1125,
	     0,     0,   473,   474,     0,  1263,   475,     0,    -2,     0,
	   536,     0,     0,     0,     0,    -2,   535,     0,   549,   551,
	   585,   619,     0,  1243,   601,     0,   603,     0,   608,  1401,
	  1402,  1450,  1452,  1453,  1454,  1455,  1456,  1436,  1420,     0,
	  1428,     0,  1429,     0,  1474,  1475,     0,  1481,  1539,  1557,
	  1558,  1553,  1582,  2247,   585,     0,   520,     0,  1224,     0,
	     0,  1638,     0,  2369,  1642,  1637,     0,  1730,     0,  1763,
	  1767,  1768,     0,  1769,     0,  1821,  1882,     0,  1891,     0,
	  1894,  1895,  1896,  1884,  1887,  1888,  1907,     0,  1309,  2105,
	     0,  1942,  1851,  1788,     0,     0,  1995,  1991,  2233,  1992,
	  1993,  1994,  2015,  2141,     0,  2170,  2171,     0,  2205,  2369,
	     0,     0,     0,   873,   875,   877,   495,   496,   499,   788,
	   498,   691,   698,   753,     0,   705,     0,     0,     0,   709,
	     0,     0,   740,  1215,   742,   751,   744,     0,   747,   749,
	     0,  1214,     0,   768,     0,     0,     0,   775,     0,     0,
	  1309,   730,   733,   732,   785,   787,  1742,  2297,  2294,  2305,
	  2310,  2312,     0,     0,  2315,  2318,  2321,  2322,     0,     0,
	  2325,  2350,  2343,  2341,  2328,     0,  2332,  2334,     0,  1610,
	  1611,   518,  1650,   146,     0,  1350,  1320,  1321,  2230,     0,
	  1357,   147,  1342,  1270,     0,     0,     0,  1343,  1364,     0,
	     0,     0,     0,   181,  1492,  1333,  1341,  1495,     0,   361,
	   350,  1482,     0,  1482,     0,  1485,  1497,  1503,  1513,  1498,
	  1505,  1514,  1500,  1506,  1507,  1508,  1509,  1510,  1511,  1512,
	     0,  1501,  1516,     0,     0,   367,     0,   370,     0,   375,
	   635,   636,   638,   685,   560,     0,   689,   690,   663,     0,
	   765,   647,     0,     0,   667,   668,  1203,  1204,   666,   672,
	     0,   670,   671,   656,   679,     0,     0,   683,   684,   663,
	     0,   371,  1323,   960,   961,   962,   940,     0,   947,   948,
	     0,   944,   945,   946,   189,     0,  1316,     0,   203,   232,
	     0,   226,   227,   228,   300,     0,   325,   321,     0,   309,
	   594,   596,     0,   577,     0,   347,   340,   585,   409,     0,
	   440,    -2,    -2,     0,     0,     0,     0,  1309,   457,  1309,
	   459,   460,   461,   462,   463,     0,   438,  1036,     0,  1027,
	  1028,     0,     0,     0,  1323,  1074,     0,  1075,   898,   245,
	     0,     0,     0,   259,   261,     0,     0,   277,     0,  1153,
	  1164,  1165,  1166,  1167,  1168,  1169,  1170,  1122,  1123,     0,
	  1127,  1128,     0,  1130,  1131,  1132,  1133,  1134,  1135,     0,
	  1112,  1113,   478,     0,   479,   543,     0,   537,   538,   539,
	   540,   527,     0,    -2,   621,   620,     0,  1220,   584,   591,
	   592,   618,   579,  1319,   604,   609,   610,   611,   569,   614,
	  1410,     0,     0,  1433,  1431,     0,  1427,     0,   500,   578,
	   573,   561,   563,     0,  1640,  2369,  1641,  1645,  1646,  1643,
	  1801,  1772,     0,  1889,     0,     0,  1878,  1914,  2369,  1790,
	  1791,  1796,  1797,  1798,  1998,     0,     0,  2199,     0,   156,
	   157,   888,     0,   754,     0,   707,   708,     0,   756,   743,
	   745,     0,   752,     0,  1258,     0,     0,     0,     0,     0,
	     0,   736,     0,     0,  2292,  2296,  2313,  2316,  2319,  2314,
	  2317,  2320,  2323,  2324,  2326,  2353,  2351,  2291,  2342,  2333,
	  2360,  1319,     0,     0,  1302,  1303,  1299,  1301,  1300,     0,
	  1369,  1372,  1392,  1392,  1392,     0,    -2,  1371,  1367,  1493,
	     0,     0,   351,  1482,     0,   353,  1482,     0,     0,  1482,
	     0,   373,     0,   377,   378,   562,   687,   644,     0,  1217,
	   645,     0,     0,     0,     0,     0,     0,     0,   681,     0,
	   648,   677,   932,   954,   949,     0,   187,     0,  1323,   202,
	   234,     0,   229,  1045,   314,   598,   583,   345,   582,   437,
	     0,     0,   451,   452,   453,   454,   455,     0,   444,   445,
	   449,   450,   456,   458,   464,     0,  1015,     0,     0,  1029,
	  1031,  1319,  1037,  1066,  1068,  1069,  1070,  1319,   247,   248,
	   250,     0,     0,   253,   254,   255,   569,   249,   262,     0,
	  1146,  1126,  1129,     0,     0,   526,     0,     0,  1260,   530,
	   599,   631,     0,   587,   588,     0,   581,     0,     0,     0,
	     0,  1435,  1438,  1437,     0,     0,  1430,  1482,     0,   481,
	     0,     0,     0,  1309,  2070,  1892,  1893,  1897,  1898,  1899,
	  1900,  1901,  1902,  1903,  1904,  1910,  2060,  1990,     0,  2003,
	     0,     0,  2161,  2196,   704,   706,   755,     0,   750,   769,
	     0,   773,   774,     0,   360,   796,   731,     0,     0,   786,
	  2295,     0,  2352,     0,     0,  1351,     0,  1358,     0,     0,
	  1374,     0,  1375,  1376,  1378,  1381,     0,    -2,  1380,     0,
	     0,  1496,   352,  1482,     0,   358,     0,   355,     0,   372,
	     0,   376,   379,   380,     0,     0,   574,   661,   662,   646,
	     0,     0,   669,     0,     0,     0,     0,   950,     0,   189,
	   200,   560,     0,     0,   323,   341,   441,   446,     0,   448,
	   465,     0,  1034,  1035,     0,  1033,  1076,   251,   252,     0,
	   274,  1136,   476,   541,     0,   529,     0,     0,   632,     0,
	   580,   612,   616,   613,   615,     0,  1434,  1432,  1476,     0,
	   501,   502,   504,   564,  1631,     0,  1999,  2000,     0,  2001,
	  1996,     0,   746,     0,   771,     0,     0,   738,   739,   734,
	     0,  2349,  2344,  2347,  2339,  2329,  2338,  2336,     0,  1368,
	  1373,     0,  1377,  1379,     0,  1261,     0,  1360,     0,   357,
	  1482,  1482,     0,   369,   374,   381,   382,   686,   664,     0,
	   674,   675,   680,     0,     0,   955,     0,     0,     0,   188,
	   210,     0,     0,     0,     0,  1032,   256,     0,     0,     0,
	   544,     0,   532,   623,     0,   589,   590,  1439,  1482,     0,
	     0,  1644,  2002,     0,   770,     0,   766,     0,     0,  2345,
	     0,  2348,     0,  2337,     0,     0,     0,     0,   657,   658,
	   659,   660,  1359,     0,  1494,   354,   359,  1482,   665,     0,
	   682,   953,   956,   957,   951,     0,     0,   447,   466,     0,
	   257,     0,     0,   542,     0,   531,     0,     0,   586,  1245,
	  1477,   503,     0,  1997,   772,     0,   735,  2346,  2335,  1352,
	  1393,     0,  1384,  1386,  1387,   728,  1388,     0,  1383,  1361,
	   356,   676,   233,     0,     0,     0,   275,   545,   533,   622,
	   624,   626,     0,   629,   630,     0,   737,  1382,     0,     0,
	  1390,     0,   467,   258,     0,   627,   629,   628,     0,  1385,
	  1389,     0,     0,   625,   505,  1391,   231
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
#line 1122 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* 
		    ** Note that FRS blocks fall through to this rule too, so
		    ** if you add actions here, do it at block_stmt too.
		    */
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 3:
#line 1133 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_func = 0;	/* No current function */
		}
		break;
	case 8:
#line 1146 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1188 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1204 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1233 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1242 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1259 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1273 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1296 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1307 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1317 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1366 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1379 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1389 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
                    /* Accept either EXEC SQL | 4GL INCLUDE */

                    if ((dml->dm_exec != (DML_EXEC|DML__SQL)) &&
                        (dml->dm_exec != (DML_EXEC|DML__4GL)))
                        er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"),
                                                            ERx("INCLUDE") );
		}
		break;
	case 21:
#line 1399 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1426 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Eat the name till right angle bracket */
		    sc_eat(id_add, SC_STRIP, ERx(">"), '<', '>');
		    yyval.s = str_add(STRNULL, id_getname());
		    id_free();
		    gr->gr_adjective = TRUE;
		}
		break;
	case 23:
#line 1435 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 24:
#line 1439 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 92:
#line 1535 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 93:
#line 1539 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 94:
#line 1543 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 95:
#line 1547 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 96:
#line 1551 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 97:
#line 1555 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 98:
#line 1559 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 99:
#line 1563 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 100:
#line 1567 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 101:
#line 1571 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 102:
#line 1575 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 103:
#line 1579 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 104:
#line 1583 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 106:
#line 1601 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1611 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 108:
#line 1623 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1633 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 111:
#line 1639 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 112:
#line 1645 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1663 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1673 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 117:
#line 1685 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1695 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 122:
#line 1704 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{ db_key(yypvt[-0].s);}
		break;
	case 123:
#line 1706 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{ db_key(yypvt[-0].s);}
		break;
	case 124:
#line 1708 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{ db_key(yypvt[-0].s);}
		break;
	case 125:
#line 1715 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
				    EQ_WARN, 1, ERx("ALTER SEQUENCE"));
		}
		break;
	case 126:
#line 1722 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 127:
#line 1735 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1745 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 134:
#line 1756 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( ERx("cascade") );
		}
		break;
	case 135:
#line 1760 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( ERx("restrict") );
		}
		break;
	case 136:
#line 1775 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1785 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 143:
#line 1806 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1816 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
	    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
	}
		break;
	case 145:
#line 1821 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
	    if (!ESQ_EQSTR(yypvt[-0].s, ERx("profile")))
		er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
				    ERx("ALTER DEFAULT"), ERx("PROFILE") );
	    gr_mechanism( GR_EQSTMT, GR_sSQL, "alter default profile ", NULL );
	}
		break;
	case 148:
#line 1834 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("privileges")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					    ERx("ALTER USER/PROFILE"), ERx("PRIVILEGES") );
		    db_key( ERx("privileges") );
		}
		break;
	case 149:
#line 1848 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1861 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
			gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 153:
#line 1866 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 154:
#line 1871 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 158:
#line 1886 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1896 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 160:
#line 1906 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
				    EQ_WARN, 1, ERx("CREATE SEQUENCE"));
		}
		break;
	case 161:
#line 1913 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 182:
#line 1947 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1957 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		}
		break;
	case 185:
#line 1970 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 1996 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2026 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("CREATE INDEX"));
		}
		break;
	case 194:
#line 2033 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    STprintf( esq->sbuf, ERx("%s %s"), yypvt[-2].s, yypvt[-1].s );
		    gr_mechanism( GR_EQSTMT, GR_sSQL, esq->sbuf, yypvt[-0].s );
		}
		break;
	case 195:
#line 2038 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		}
		break;
	case 205:
#line 2065 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2085 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2104 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2126 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2137 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 213:
#line 2141 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 219:
#line 2152 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 220:
#line 2156 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 221:
#line 2160 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 226:
#line 2173 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 227:
#line 2178 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 228:
#line 2183 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_sconst( yypvt[-0].s );
		}
		break;
	case 229:
#line 2189 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 231:
#line 2197 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2210 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-3].s );
		    db_key( yypvt[-2].s );
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 236:
#line 2234 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2247 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2269 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 260:
#line 2298 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2313 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = CSR_STATIC;
		}
		break;
	case 276:
#line 2336 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2346 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2364 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 280:
#line 2369 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 282:
#line 2375 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 285:
#line 2400 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2413 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2423 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2442 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
 		    if (eq->eq_flags & EQ_COMMON)
 			er_write( E_EQ0504_OPEN_WARN,
 				EQ_WARN, 1, ERx("CREATE SCHEMA") );
		    esq->flag |= ESQ_CRE_SCHEMA;
		}
		break;
	case 293:
#line 2453 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		    if (gr->gr_func == tSET)
		    	erec_setup( 1 );
		}
		break;
	case 303:
#line 2485 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2502 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 310:
#line 2510 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2531 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 316:
#line 2535 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 317:
#line 2539 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 318:
#line 2543 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 319:
#line 2547 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 320:
#line 2551 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 325:
#line 2562 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{db_key( yypvt[-0].s);}
		break;
	case 326:
#line 2566 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2576 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2586 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2595 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2612 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{ 
			db_key(yypvt[-0].s);
		}
		break;
	case 331:
#line 2617 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
			db_key(yypvt[-1].s);
			db_key(yypvt[-0].s);
		}
		break;
	case 333:
#line 2629 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
				EQ_WARN, 1, ERx("CREATE SYNONYM"));
		}
		break;
	case 335:
#line 2643 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2661 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				    2, ERx("location"), ERx("CREATE TABLE") );
		}
		break;
	case 338:
#line 2677 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* With clause is non-FIPS compliant */
		    if (yypvt[-0].i == 1 && eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
					2, ERx("WITH"), ERx("CREATE TABLE") );
		}
		break;
	case 339:
#line 2685 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2744 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2, 
			        ERx("AS SELECT"), ERx("CREATE TABLE") );
		}
		break;
	case 341:
#line 2751 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2, 
			        ERx("AS SELECT"), ERx("CREATE TABLE") );
		}
		break;
	case 342:
#line 2782 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;		/* Info on dec_nullfmt */
		}
		break;
	case 343:
#line 2786 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-2].i | yypvt[-0].i;		/* Info on dec_nullfmt */
		}
		break;
	case 344:
#line 2791 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 345:
#line 2795 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-2].i | yypvt[-0].i;
		}
		break;
	case 346:
#line 2800 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 347:
#line 2804 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-1].i;
		}
		break;
	case 348:
#line 2809 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-1].i;
}
		break;
	case 349:
#line 2814 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2835 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 351:
#line 2839 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 352:
#line 2843 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 353:
#line 2848 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 354:
#line 2852 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 355:
#line 2856 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 356:
#line 2860 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 357:
#line 2865 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 358:
#line 2869 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2884 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 362:
#line 2892 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    esq->flag |= ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 363:
#line 2896 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    esq->flag |= ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 367:
#line 2905 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 375:
#line 2922 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 377:
#line 2933 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 378:
#line 2941 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 379:
#line 2950 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 380:
#line 2958 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 381:
#line 2966 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2975 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 2993 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 385:
#line 3001 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 3011 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 3021 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 388:
#line 3030 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 389:
#line 3039 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    {
			db_key(yypvt[-0].s);
		    }
		}
		break;
	case 393:
#line 3060 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 3076 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* UNION views are non-FIPS */
		    if (eq->eq_flags & EQ_FIPS && yypvt[-1].i == 1)
			    er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2, 
			        ERx("UNION"), ERx("CREATE VIEW") );
		}
		break;
	case 396:
#line 3092 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 3103 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 3129 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 3143 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    STpolycat(3, yypvt[-2].s, yypvt[-1].s, yypvt[-0].s, session_temp);
		    yyval.s = session_temp;
		}
		break;
	case 401:
#line 3148 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 402:
#line 3153 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 403:
#line 3158 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-2].s );
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 404:
#line 3177 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 3188 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 3202 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 3216 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 3228 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_flag |= GR_NOSYNC;	/* no error output yet */
		    yyval.i = 0;
		}
		break;
	case 411:
#line 3233 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_flag |= GR_NOSYNC;	/* no error output yet */
		    if (eq->eq_flags & EQ_FIPS) 
			    er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1, 
			        ERx("REPEATED DELETE") );
		    yyval.i = 1;
		}
		break;
	case 412:
#line 3243 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    er_write( E_EQ0070_grMISSWORD, EQ_ERROR, 2, ERx("FROM"),
								ERx("DELETE") );
		    yyval.s = ERx("from");
		}
		break;
	case 414:
#line 3251 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 415:
#line 3257 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    STprintf( esq->sbuf, ERx("%s.%s"), yypvt[-2].s, yypvt[-0].s );
		    yyval.s = esq->sbuf;
		}
		break;
	case 416:
#line 3263 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    STprintf( esq->sbuf, ERx("%s.%s"), yypvt[-2].s, yypvt[-0].s );
		    yyval.s = esq->sbuf;
		}
		break;
	case 418:
#line 3270 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if ( gr->gr_flag & GR_REPEAT )
		    {
			db_op( ERx("...SYNTAX ERROR") );
			er_write( E_EQ0378_repWHERE, EQ_ERROR, 0 );
		    }
		}
		break;
	case 419:
#line 3284 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 3294 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 421:
#line 3303 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 3313 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 423:
#line 3318 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 424:
#line 3322 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 425:
#line 3326 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 426:
#line 3330 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 427:
#line 3334 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 430:
#line 3353 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 3383 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 3411 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 434:
#line 3415 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-1].s, yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS) 
			    er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1, 
			        ERx("REPEATED INSERT") );
		}
		break;
	case 435:
#line 3423 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 439:
#line 3433 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		    erec_setup( 1 );
		}
		break;
	case 442:
#line 3450 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (yypvt[-0].i > 1)
			er_write( E_EQ0132_sqILLSTRUCTEXPR, EQ_ERROR, 0 );
		}
		break;
	case 446:
#line 3462 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		  /* return the max num of struct elems on one side */
		    yyval.i = (yypvt[-2].i > yypvt[-0].i) ? yypvt[-2].i : yypvt[-0].i;
		}
		break;
	case 447:
#line 3467 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
 		  /* return the max num of struct elems on one side */
 		    yyval.i = (yypvt[-4].i > yypvt[-1].i) ? yypvt[-4].i : yypvt[-1].i;
 		}
		break;
	case 448:
#line 3472 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-1].i;	/* Might be a structure */
		}
		break;
	case 449:
#line 3476 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;	/* Might be a structure */
		}
		break;
	case 450:
#line 3480 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;	/* Might be a structure */
		}
		break;
	case 456:
#line 3492 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = erec_vars();
		    erec_use( IISETDOM, gr->gr_flag & GR_REPEAT, ERx(",") );
		    erec_setup( 1 );		/* end of variable */
		    if ( (gr->gr_type == T_STRUCT) && (eq->eq_flags & EQ_FIPS) )
			er_write( E_EQ050D_FIPS_VAR, EQ_WARN, 0 );
		}
		break;
	case 457:
#line 3500 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;		/* Not a structure */
		}
		break;
	case 458:
#line 3504 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    erec_use( IISETDOM, gr->gr_flag & GR_REPEAT, ERx(",") );
		    erec_setup( 1 );
		}
		break;
	case 460:
#line 3512 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (   !ESQ_EQSTR(yypvt[-0].s, ERx("dbmsinfo")) 
		        && eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0510_OPEN_FUNC, EQ_WARN, 1, yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS) 
			er_write( E_EQ0509_FIPS_FUNC, EQ_WARN, 1, yypvt[-0].s );
		}
		break;
	case 462:
#line 3521 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    er_write( E_EQ0129_sqVARCOLON, EQ_ERROR, 0 );
		    YYERROR;
		}
		break;
	case 464:
#line 3528 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-1].s;
		}
		break;
	case 465:
#line 3532 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 466:
#line 3536 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-4].s;
		}
		break;
	case 467:
#line 3541 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-6].s;
		}
		break;
	case 469:
#line 3559 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    gr->gr_func = tUPDATE;
		}
		break;
	case 470:
#line 3564 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-1].s, yypvt[-0].s );
		    gr->gr_func = tUPDATE;
		    if (eq->eq_flags & EQ_FIPS) 
			    er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1, 
			        ERx("REPEATED UPDATE") );
		}
		break;
	case 473:
#line 3578 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 3615 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 3630 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    esq->flag |= ESQ_CURSOR;
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 480:
#line 3698 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 3742 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* don't set until after call to gr_mechanism */ 
		}
		break;
	case 483:
#line 3747 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 3759 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sREPEAT, yypvt[-1].s, yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS) 
			    er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1, 
			        ERx("REPEATED SELECT") );
		}
		break;
	case 487:
#line 3770 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 490:
#line 3777 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* DISTINCT makes a cursor Read-Only */
		    if (ESQ_DCLCSR(esq))
			ecs_csrset( NULL, ECS_UNIQUE );
		}
		break;
	case 491:
#line 3784 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 492:
#line 3789 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s ); /* db_key 'cause we want spaces around it */
		}
		break;
	case 496:
#line 3800 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2,
			    ERx("<= result_column_name>"), ERx("SELECT") );
		}
		break;
	case 497:
#line 3806 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 3839 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN, 2,
			    ERx("<result_column_name AS>"), ERx("SELECT") );
		}
		break;
	case 499:
#line 3845 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 3877 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 3900 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 508:
#line 3906 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 3964 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    lbl_exit_block();
		}
		break;
	case 510:
#line 3984 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* error must have INTO clause before FROM clause */
		    er_write( E_EQ0070_grMISSWORD, EQ_ERROR, 2, ERx("INTO"),
							ERx("SELECT/FETCH") );
		    YYERROR;
		}
		break;
	case 511:
#line 3991 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    erec_setup( 0 );	/* Done with list */
		}
		break;
	case 512:
#line 3996 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    erec_setup( 1 );	/* Initialize struct indicators stuff */
		}
		break;
	case 515:
#line 4004 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    erec_setup( 0 );	/* Done with one element */
		}
		break;
	case 516:
#line 4009 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if ( (gr->gr_type == T_STRUCT) && (eq->eq_flags & EQ_FIPS) )
			er_write( E_EQ050D_FIPS_VAR, EQ_WARN, 0 );
		}
		break;
	case 518:
#line 4016 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 4040 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    sc_eat(id_add,SC_STRIP|SC_NEST|SC_SEEN,ERx(")"), '(', ')');
		}
		break;
	case 520:
#line 4055 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 4095 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 4108 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = CSR_NO_FROM;
		}
		break;
	case 523:
#line 4113 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 524:
#line 4117 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-2].i | yypvt[-0].i;
		}
		break;
	case 525:
#line 4122 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 526:
#line 4126 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = FRM_JOIN;
		}
		break;
	case 527:
#line 4130 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-2].i;
		}
		break;
	case 529:
#line 4136 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-1].s;
		}
		break;
	case 534:
#line 4147 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = FRM_OK;
		}
		break;
	case 535:
#line 4151 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-1].i;
		}
		break;
	case 536:
#line 4156 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 537:
#line 4160 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 538:
#line 4165 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 539:
#line 4170 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 540:
#line 4175 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 546:
#line 4188 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_addtab( NULL, yypvt[-0].s );
		    yyval.s = (char *)0;
		}
		break;
	case 547:
#line 4194 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_addtab( NULL, yypvt[-2].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 548:
#line 4200 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    char tbuf[100];     /* buffer to hold owner.tablename */

		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_addtab( NULL, STpolycat(3,yypvt[-2].s,yypvt[-1].s,yypvt[-0].s,tbuf) );
		    yyval.s = (char *)0;
		}
		break;
	case 549:
#line 4208 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    char tbuf[100];     /* buffer to hold owner.tablename */
		
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
		        ecs_addtab( NULL, STpolycat(3,yypvt[-4].s,yypvt[-3].s,yypvt[-2].s,tbuf) );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 550:
#line 4217 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    char tbuf[100];     /* buffer to hold owner.tablename */

		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_addtab( NULL, STpolycat(3,yypvt[-2].s,yypvt[-1].s,yypvt[-0].s,tbuf) );
		    yyval.s = (char *)0;
		}
		break;
	case 551:
#line 4226 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    char tbuf[100];     /* buffer to hold owner.tablename */
		
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
		        ecs_addtab( NULL, STpolycat(3,yypvt[-4].s,yypvt[-3].s,yypvt[-2].s,tbuf) );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 552:
#line 4234 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_addtab( NULL, yypvt[-0].s );
		    yyval.s = (char *)0;
		}
		break;
	case 553:
#line 4240 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
		        ecs_addtab( NULL, yypvt[-2].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 558:
#line 4260 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = CSR_STATIC;
		}
		break;
	case 559:
#line 4264 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 4273 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = CSR_NO_WHERE;
		}
		break;
	case 561:
#line 4279 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* GROUP BY makes a cursor Read-Only */
		    if (ESQ_DCLCSR(esq))
			ecs_csrset( NULL, ECS_GROUP );
		    yyval.i = 1;
		}
		break;
	case 562:
#line 4286 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 563:
#line 4291 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 564:
#line 4295 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 565:
#line 4300 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 566:
#line 4304 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 567:
#line 4310 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 568:
#line 4315 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 569:
#line 4319 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 570:
#line 4324 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 571:
#line 4329 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 572:
#line 4334 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 4345 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* HAVING makes a cursor Read-Only */
		    if (ESQ_DCLCSR(esq))
			ecs_csrset( NULL, ECS_HAVING );
		    yyval.i = 1;
		}
		break;
	case 574:
#line 4352 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 587:
#line 4378 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
	if (!ESQ_EQSTR(yypvt[-0].s, ERx("first")))
	    er_write( E_EQ0244_yySYNWRD, EQ_ERROR, 1, yypvt[-0].s );
    }
		break;
	case 588:
#line 4383 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
	db_key( yypvt[-0].s );
    }
		break;
	case 589:
#line 4388 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
	db_key( yypvt[-0].s );
    }
		break;
	case 590:
#line 4392 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
	db_key( yypvt[-0].s );
    }
		break;
	case 593:
#line 4405 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;	/* UNION info for CREATE VIEW rule */
		}
		break;
	case 594:
#line 4409 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 1;		/* UNION info for CREATE VIEW rule */
		}
		break;
	case 595:
#line 4414 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;		/* UNION info used by CREATE VIEW */
		}
		break;
	case 596:
#line 4418 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-2].i;	/* UNION info used by CREATE VIEW */
		}
		break;
	case 599:
#line 4445 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 4470 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 601:
#line 4474 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (yypvt[-2].i == CSR_NO_FROM || yypvt[-0].i == CSR_NO_FROM)
			yyval.i = CSR_NO_FROM;
		    else
			yyval.i = yypvt[-2].i | yypvt[-0].i; /* MRW: probably just return $3.i */
		    ecs_csrset( NULL, ECS_UNION );
		}
		break;
	case 602:
#line 4490 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 603:
#line 4494 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-1].i;	/* MRW: probably can just return 0 */
		}
		break;
	case 605:
#line 4501 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 606:
#line 4505 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 4520 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s ); /* db_key 'cause we want spaces around it */
		    _VOID_ ecs_colupd( NULL, ERx(""), ECS_ADD|ECS_ISWILD );
		}
		break;
	case 613:
#line 4538 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 4553 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 4595 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 4610 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 4625 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    ecs_csrset( NULL, ECS_SORTED );
		}
		break;
	case 621:
#line 4634 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = CSR_STATIC;
		}
		break;
	case 622:
#line 4638 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 4656 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 4690 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    ecs_addtab( NULL, yypvt[-1].s );
		}
		break;
	case 629:
#line 4695 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 4734 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    er_write( E_EQ0411_csUPDALL, EQ_ERROR, 1, ecs_namecsr() );
		}
		break;
	case 631:
#line 4739 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = ECS_UPDATE;
		}
		break;
	case 632:
#line 4743 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 4757 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 647:
#line 4785 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 4796 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 4808 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 650:
#line 4812 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 651:
#line 4816 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 652:
#line 4820 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 4831 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 654:
#line 4835 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 655:
#line 4839 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 656:
#line 4843 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 657:
#line 4850 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 658:
#line 4854 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 659:
#line 4858 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 660:
#line 4862 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 662:
#line 4881 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 4937 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 685:
#line 4943 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-1].i;		/* bug #58171 */
		}
		break;
	case 686:
#line 4949 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 4984 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 692:
#line 4988 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 698:
#line 5000 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 699:
#line 5004 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 700:
#line 5008 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 701:
#line 5012 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 702:
#line 5018 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = ERx("");
		    /* functions make a cursor Read-Only */
		    if (ESQ_DCLCSR(esq) && (esq->flag & ESQ_FROM_CHK))
			ecs_csrset( NULL, ECS_FUNCTION );
		}
		break;
	case 703:
#line 5025 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 710:
#line 5043 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 711:
#line 5047 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 712:
#line 5053 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = NULL;
		}
		break;
	case 713:
#line 5057 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 714:
#line 5061 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 5070 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 716:
#line 5074 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 717:
#line 5078 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 718:
#line 5082 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = ERx("");
		}
		break;
	case 721:
#line 5089 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_hexconst( yypvt[-0].s );
		}
		break;
	case 722:
#line 5093 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_uconst( yypvt[-0].s );
		}
		break;
	case 723:
#line 5097 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 724:
#line 5101 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 726:
#line 5106 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 728:
#line 5111 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 729:
#line 5116 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_dtconst( yypvt[-0].s );
		}
		break;
	case 732:
#line 5124 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 5145 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 5181 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0509_FIPS_FUNC, EQ_WARN, 1, ERx("ANY"));
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0510_OPEN_FUNC, EQ_WARN, 1, ERx("ANY"));
		}
		break;
	case 757:
#line 5189 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_op( yypvt[-0].s );
		}
		break;
	case 758:
#line 5195 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_op( yypvt[-0].s );
		}
		break;
	case 759:
#line 5201 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 760:
#line 5205 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 761:
#line 5209 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 762:
#line 5213 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 763:
#line 5219 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 764:
#line 5223 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 767:
#line 5233 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-1].s;
		}
		break;
	case 768:
#line 5237 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 769:
#line 5241 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-4].s;
		}
		break;
	case 770:
#line 5246 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-6].s;
		}
		break;
	case 771:
#line 5250 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-5].s;
		}
		break;
	case 772:
#line 5254 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-7].s;
		}
		break;
	case 773:
#line 5258 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-4].s, ERx("position")))
			er_write( E_EQ006A_grxBADWORD, EQ_ERROR, 2, yypvt[-4].s,
				ERx("query"));
		    yyval.s = yypvt[-4].s;
		}
		break;
	case 774:
#line 5265 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-4].s, ERx("trim")))
			er_write( E_EQ006A_grxBADWORD, EQ_ERROR, 2, yypvt[-4].s,
				ERx("query"));
		    yyval.s = yypvt[-4].s;
		}
		break;
	case 778:
#line 5277 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-1].s;
		}
		break;
	case 779:
#line 5282 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 780:
#line 5287 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 781:
#line 5292 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 782:
#line 5297 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 786:
#line 5308 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 787:
#line 5312 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 794:
#line 5339 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 5351 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 5370 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 5408 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-1].s, yypvt[-0].s );
		    esq_init();
		    ecs_close( yypvt[-0].s, gr->gr_sym != (SYM *)0 );
		}
		break;
	case 798:
#line 5425 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 5440 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 5453 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 801:
#line 5467 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 802:
#line 5471 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = ECS_KEYSET;
                }
		break;
	case 803:
#line 5476 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    ecs_csrset( NULL, ECS_DYNAMIC );
		}
		break;
	case 804:
#line 5480 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    ecs_csrset( NULL, ECS_DYNAMIC );
		}
		break;
	case 805:
#line 5484 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 5520 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    ecs_csrset( NULL, ECS_ERR );
		}
		break;
	case 807:
#line 5559 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 5569 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 5583 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
			    yyval.fet.intv1 = 0;
			}
		break;
	case 811:
#line 5662 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
			    yyval.i = 0;
			}
		break;
	case 812:
#line 5667 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
			    CVan(yypvt[-0].s, &yyval.i);
			    yyval.i = -yyval.i;
			}
		break;
	case 813:
#line 5672 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
			    CVan(yypvt[-0].s, &yyval.i);
			}
		break;
	case 814:
#line 5677 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    erec_use( IICSGET, FALSE, NULL );
		    yyval.i = 0;
		}
		break;
	case 815:
#line 5682 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 816:
#line 5686 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 817:
#line 5691 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 5752 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 5772 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 5795 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 821:
#line 5800 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 822:
#line 5805 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 5820 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = NULL;
		}
		break;
	case 824:
#line 5824 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 5835 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 826:
#line 5839 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 827:
#line 5843 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IISQDAIN );
		    yyval.i = 1;
		}
		break;
	case 828:
#line 5851 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 5892 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 5909 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 5928 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 5940 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 834:
#line 5945 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 5954 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = NULL;
		}
		break;
	case 836:
#line 5983 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN, 1,
				ERx("DROP") );
		}
		break;
	case 837:
#line 5990 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 6031 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 856:
#line 6036 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = (char *)0;
		}
		break;
	case 857:
#line 6040 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 858:
#line 6045 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 859:
#line 6050 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 860:
#line 6056 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("DROP LINK"));
		}
		break;
	case 861:
#line 6064 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( IIPROC_DROP );

		    /* use first name encountered. */
		    arg_str_add( ARG_CHAR, yypvt[-0].s);
		    gen_call( IIPROCINIT );
		}
		break;
	case 863:
#line 6074 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s);
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("DROP PROCEDURE"));
		}
		break;
	case 866:
#line 6087 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
					EQ_WARN, 1, ERx("DROP ROLE"));
		}
		break;
	case 867:
#line 6095 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP GROUP") );
		}
		break;
	case 868:
#line 6103 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
					EQ_WARN, 1, ERx("DROP DBEVENT"));
		}
		break;
	case 869:
#line 6111 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
					EQ_WARN, 1, ERx("DROP SEQUENCE"));
		}
		break;
	case 870:
#line 6119 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("DROP INTEGRITY"));
		}
		break;
	case 871:
#line 6126 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("DROP PERMIT"));
		}
		break;
	case 872:
#line 6134 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 874:
#line 6139 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 876:
#line 6144 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 889:
#line 6168 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP RULE") );
		}
		break;
	case 890:
#line 6176 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP DOMAIN") );
		}
		break;
	case 891:
#line 6184 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP LOCATION") );
		}
		break;
	case 892:
#line 6192 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP USER") );
		}
		break;
	case 893:
#line 6200 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP PROFILE") );
		}
		break;
	case 894:
#line 6208 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN,
				EQ_WARN, 1, ERx("DROP SECURITY_ALARM") );
		}
		break;
	case 895:
#line 6216 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 899:
#line 6244 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 6256 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 6272 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("privileges")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					    ERx("REVOKE"), ERx("PRIVILEGES") );
		    db_key( ERx("privileges") );
		}
		break;
	case 918:
#line 6294 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 6309 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 6326 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 924:
#line 6330 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 927:
#line 6338 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 928:
#line 6342 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 930:
#line 6354 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 6370 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 6399 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
				EQ_WARN, 1, ERx("COPY TABLE") );
		}
		break;
	case 950:
#line 6428 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_op( yypvt[-2].s );
		    db_key( yypvt[-1].s );
		    db_op( yypvt[-0].s );
		}
		break;
	case 951:
#line 6434 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_op( yypvt[-4].s );
		    db_key( yypvt[-3].s );
		    db_op( yypvt[-2].s );
		    db_key( yypvt[-1].s );
		    db_op( yypvt[-0].s );
		}
		break;
	case 958:
#line 6451 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 959:
#line 6455 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 961:
#line 6461 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 6474 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 6497 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 6539 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 6552 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS) 
			er_write( E_EQ0507_FIPS_NOTOP, EQ_WARN,
				2, ERx("PRIVILEGES"), ERx("GRANT") );
		}
		break;
	case 973:
#line 6566 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, ERx("INSERT"), ERx("GRANT") );
		    }
		}
		break;
	case 975:
#line 6575 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, ERx("EXECUTE"), ERx("GRANT") );
		    }
		}
		break;
	case 976:
#line 6583 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, ERx("REGISTER"), ERx("GRANT") );
		    }
		}
		break;
	case 979:
#line 6593 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, yypvt[-0].s, ERx("GRANT") );
		    }
		}
		break;
	case 980:
#line 6601 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
		    {
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				2, yypvt[-1].s, ERx("GRANT") );
		    }
		}
		break;
	case 983:
#line 6613 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("privileges")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					    ERx("GRANT"), ERx("PRIVILEGES") );
		    db_key( ERx("privileges") );
		}
		break;
	case 985:
#line 6623 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
                    db_key( yypvt[-0].s );
                }
		break;
	case 986:
#line 6628 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 987:
#line 6633 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 988:
#line 6638 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 989:
#line 6643 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 990:
#line 6648 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 991:
#line 6653 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 6662 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 6672 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		}
		break;
	case 994:
#line 6675 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 6694 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 997:
#line 6699 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 998:
#line 6704 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 999:
#line 6710 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 6721 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 6733 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* TO GROUP is non-FIPS */
		    if (eq->eq_flags & EQ_FIPS && gr->gr_func != tREVOKE)
			er_write( E_EQ0506_FIPS_CLAUSE,
				EQ_WARN, 2, ERx("TO GROUP"), ERx("GRANT") );
		}
		break;
	case 1003:
#line 6740 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 6757 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 1009:
#line 6767 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 1010:
#line 6772 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 1014:
#line 6781 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 1017:
#line 6798 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 6822 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1037:
#line 6854 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 6872 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-3].s, NULL );
		    esq_init();
		}
		break;
	case 1039:
#line 6878 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_INT, yypvt[-0].s );
		}
		break;
	case 1040:
#line 6882 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 6911 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1, yypvt[-1].s );
		}
		break;
	case 1042:
#line 6916 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 1043:
#line 6926 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 6936 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		}
		break;
	case 1047:
#line 6943 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 6989 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 7000 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = (char *)0;
		}
		break;
	case 1051:
#line 7004 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1052:
#line 7008 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1053:
#line 7012 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1055:
#line 7018 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 7034 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1057:
#line 7038 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-2].i | yypvt[-0].i;
		}
		break;
	case 1058:
#line 7043 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1059:
#line 7047 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-2].i;
		}
		break;
	case 1061:
#line 7053 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1062:
#line 7058 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 7087 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				1, ERx("REGISTER INDEX") );
		}
		break;
	case 1071:
#line 7094 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    STprintf( esq->sbuf, ERx("%s %s"), yypvt[-2].s, yypvt[-1].s );
		    gr_mechanism( GR_EQSTMT, GR_sSQL, esq->sbuf, yypvt[-0].s );
		}
		break;
	case 1072:
#line 7099 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		}
		break;
	case 1078:
#line 7113 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 7131 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 7147 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT,
					EQ_WARN, 1, ERx("REMOVE"));
		}
		break;
	case 1082:
#line 7154 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (yypvt[-0].s != (char *)0)
			gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		    else
			gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, NULL );
		}
		break;
	case 1083:
#line 7161 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0504_OPEN_WARN, 
					EQ_WARN, 1, ERx("REMOVE DBEVENT"));
		}
		break;
	case 1084:
#line 7169 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = (char *)0;
		}
		break;
	case 1085:
#line 7173 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1086:
#line 7177 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1087:
#line 7181 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1088:
#line 7185 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1089:
#line 7195 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 7206 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 7220 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 7232 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 7259 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 7270 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1100:
#line 7277 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1101:
#line 7320 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 7331 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 7347 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 7356 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 7365 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_COMMON)
                    {
                        er_write( E_EQ0504_OPEN_WARN,
                                        EQ_WARN, 1, ERx("SET SESSION"));
                    }
                }
		break;
	case 1106:
#line 7375 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 7422 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("privileges")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
					    ERx("SET SESSION"), ERx("PRIVILEGES") );
		    db_key( ERx("privileges") );
		}
		break;
	case 1126:
#line 7444 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-1].s );
		    db_key( yypvt[-0].s );
		}
		break;
	case 1131:
#line 7453 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
			db_key (yypvt[-0].s);
		}
		break;
	case 1132:
#line 7457 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
			db_key (yypvt[-0].s);
		}
		break;
	case 1133:
#line 7461 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
			db_key (yypvt[-0].s);
		}
		break;
	case 1134:
#line 7465 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
			db_key (yypvt[-0].s);
		}
		break;
	case 1135:
#line 7469 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 7486 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1138:
#line 7490 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1146:
#line 7503 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Expecting SET WORK LOCATIONS command */		
		    if (!ESQ_EQSTR(yypvt[-4].s, ERx("locations")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-4].s,
					ERx("SET WORK"), ERx("LOCATIONS") );
		}
		break;
	case 1147:
#line 7510 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{ 
		    db_sconst( yypvt[-0].s );
		}
		break;
	case 1148:
#line 7514 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{ 
		    db_key( yypvt[-0].s );
		}
		break;
	case 1158:
#line 7530 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    erec_use( IISETDOM, gr->gr_flag & GR_REPEAT, ERx(",") );
		    erec_setup( 1 );
		}
		break;
	case 1168:
#line 7549 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1169:
#line 7553 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1174:
#line 7564 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
			db_key( yypvt[-0].s );
		}
		break;
	case 1176:
#line 7572 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 7581 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 7590 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 7599 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
                    /* Don't print if in DECLARE TABLE statement */
                    if ((esq->flag & ESQ_NOPRT) == 0)
                        db_key( yypvt[-0].s );
		}
		break;
	case 1180:
#line 7606 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
                    if (eq->eq_flags & EQ_FIPS)
                        er_write( E_EQ050F_FIPS_USE, EQ_WARN, 1, yypvt[-0].s );
		    if (eq->eq_flags & EQ_COMMON)
			er_write( E_EQ0510_OPEN_FUNC, EQ_WARN, 1, yypvt[-0].s );
		}
		break;
	case 1182:
#line 7620 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 7640 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s ); 
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1190:
#line 7668 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key(yypvt[-1].s);
		    db_key(yypvt[-0].s);
		}
		break;
	case 1191:
#line 7673 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key(yypvt[-1].s);
		    db_key(yypvt[-0].s);
		}
		break;
	case 1192:
#line 7678 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key(yypvt[-1].s);
		    db_key(yypvt[-0].s);
		}
		break;
	case 1193:
#line 7683 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 1194:
#line 7689 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    gr->gr_func = tSET;
		}
		break;
	case 1195:
#line 7695 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key(yypvt[-0].s);
		}
		break;
	case 1196:
#line 7709 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1197:
#line 7714 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1198:
#line 7719 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1199:
#line 7724 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1201:
#line 7730 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1202:
#line 7735 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1203:
#line 7740 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1204:
#line 7745 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1205:
#line 7750 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1206:
#line 7755 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1207:
#line 7760 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1208:
#line 7765 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_op(ERx(", "));	/* Need a space for run-time */
		}
		break;
	case 1209:
#line 7770 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1210:
#line 7775 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1211:
#line 7780 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		if ((esq->flag & ESQ_NOPRT) == 0)
		    db_key( yypvt[-0].s );
		}
		break;
	case 1212:
#line 7786 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		if ((esq->flag & ESQ_NOPRT) == 0)
		    db_op( yypvt[-0].s );
		}
		break;
	case 1213:
#line 7792 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1214:
#line 7797 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1215:
#line 7802 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1216:
#line 7807 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1217:
#line 7812 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1218:
#line 7817 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1219:
#line 7822 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1220:
#line 7827 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1221:
#line 7832 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		if ((esq->flag & ESQ_NOPRT) == 0)
		    db_key( yypvt[-0].s );
		}
		break;
	case 1222:
#line 7838 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1223:
#line 7843 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1224:
#line 7848 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1225:
#line 7853 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1226:
#line 7858 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1227:
#line 7863 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1228:
#line 7868 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1229:
#line 7873 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1230:
#line 7878 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1231:
#line 7883 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1232:
#line 7888 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_op( ERx(" -") );
		}
		break;
	case 1233:
#line 7893 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1234:
#line 7898 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		if ((esq->flag & ESQ_NOPRT) == 0)
		    db_key( yypvt[-0].s );
		}
		break;
	case 1235:
#line 7904 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1236:
#line 7909 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1237:
#line 7914 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1238:
#line 7919 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1239:
#line 7924 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1240:
#line 7929 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1241:
#line 7934 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1242:
#line 7939 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1243:
#line 7944 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1244:
#line 7949 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1245:
#line 7954 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1246:
#line 7959 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1247:
#line 7964 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1248:
#line 7969 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1249:
#line 7974 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_op( ERx(" +") );
		}
		break;
	case 1250:
#line 7979 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1251:
#line 7984 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1252:
#line 7989 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1253:
#line 7994 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1254:
#line 7999 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1255:
#line 8004 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_op( yypvt[-0].s );
		}
		break;
	case 1256:
#line 8009 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1257:
#line 8014 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1258:
#line 8019 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1259:
#line 8024 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1260:
#line 8029 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1261:
#line 8034 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1262:
#line 8039 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1263:
#line 8044 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 8056 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 8065 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 8075 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 8086 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1268:
#line 8091 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1271:
#line 8120 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1272:
#line 8126 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 1273:
#line 8130 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 1274:
#line 8136 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if ((esq->flag & ESQ_NOPRT) == 0)
			db_key( yypvt[-0].s );
                    yyval.s = yypvt[-0].s;
                }
		break;
	case 1275:
#line 8142 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if ((esq->flag & ESQ_NOPRT) == 0)
			db_delimid( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1276:
#line 8149 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
                    yyval.s = yypvt[-0].s;
                    if (eq->eq_flags & EQ_FIPS)
                        eqck_regid(yypvt[-0].s); /* Check if id is FIPS compliant */
                }
		break;
	case 1277:
#line 8156 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    if (eq->eq_flags & EQ_FIPS)
			eqck_delimid(yypvt[-0].s); /* Check if id is FIPS complaint */
		}
		break;
	case 1279:
#line 8165 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 8189 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
                    db_key( yypvt[-0].s );
                    yyval.s = yypvt[-0].s;
                    gr->gr_sym = (SYM *)0;
                }
		break;
	case 1283:
#line 8196 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
                    db_key( yypvt[-0].s );
                    yyval.s = yypvt[-0].s;
                    gr->gr_sym = (SYM *)0;
                }
		break;
	case 1284:
#line 8202 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
                    db_key( yypvt[-0].s );
                    yyval.s = yypvt[-0].s;
                    gr->gr_sym = (SYM *)0;
                }
		break;
	case 1285:
#line 8208 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
                    db_key( yypvt[-0].s );
                    yyval.s = yypvt[-0].s;
                    gr->gr_sym = (SYM *)0;
                }
		break;
	case 1286:
#line 8219 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_sconst( yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1287:
#line 8228 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1288:
#line 8232 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1289:
#line 8240 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = ERx(" ");
		}
		break;
	case 1290:
#line 8244 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = ERx(",");
		}
		break;
	case 1291:
#line 8254 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1292:
#line 8266 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 8277 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 8288 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 8301 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 8347 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 8368 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 8388 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add(ARG_CHAR, yypvt[-0].s);
		}
		break;
	case 1307:
#line 8393 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    STprintf( esq->sbuf, ERx("%s%s%s"), yypvt[-2].s, yypvt[-1].s, yypvt[-0].s );
		    arg_str_add(ARG_CHAR, esq->sbuf);
		}
		break;
	case 1308:
#line 8398 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 8413 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_nlsym = (SYM *)0;	/* Initialize state vars */
		    gr->gr_nlid = (char *)0;
		    gr->gr_nltype = T_NONE; 
		}
		break;
	case 1312:
#line 8422 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 8449 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 8469 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1318:
#line 8479 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1321:
#line 8492 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-0].s, ERx("asc")) &&
					!ESQ_EQSTR(yypvt[-0].s, ERx("desc")))
			er_write( E_EQ0126_sqCOMMA, EQ_ERROR, 0);
		    db_key( yypvt[-0].s );
		}
		break;
	case 1322:
#line 8510 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 1323:
#line 8514 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 1327:
#line 8546 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_flag &= ~GR_CPYHNDDEF;
		}
		break;
	case 1333:
#line 8557 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (   gr->gr_flag & GR_COPY_PROG
		        && ESQ_EQSTR(yypvt[-0].s, ERx("copyhandler")))
			gr->gr_flag |= GR_CPYHNDDEF;
		    db_key( yypvt[-0].s );
		}
		break;
	case 1334:
#line 8564 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1335:
#line 8568 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1336:
#line 8572 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1337:
#line 8576 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1338:
#line 8580 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1339:
#line 8584 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1340:
#line 8588 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1341:
#line 8592 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1345:
#line 8600 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1346:
#line 8604 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1347:
#line 8608 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1348:
#line 8612 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1349:
#line 8616 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1354:
#line 8628 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1355:
#line 8632 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 1394:
#line 8742 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 8752 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s, NULL );
		    esq_init();
		    arg_int_add( IIutPROG );
		}
		break;
	case 1396:
#line 8759 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_call( IIUTSYS );
		}
		break;
	case 1399:
#line 8769 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( IIutARG );
		}
		break;
	case 1402:
#line 8777 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IIUTSYS );
		}
		break;
	case 1404:
#line 8783 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 1405:
#line 8787 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 1406:
#line 8792 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( IIutARG );
		}
		break;
	case 1421:
#line 8844 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 8854 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IISQSESS );
		}
		break;
	case 1423:
#line 8859 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 8876 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IISQCNNM );
		}
		break;
	case 1425:
#line 8881 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_push();
		}
		break;
	case 1426:
#line 8885 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{ arg_push(); }
		break;
	case 1427:
#line 8886 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IISQUSER );
		}
		break;
	case 1429:
#line 8894 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{ 
		  arg_str_add(ARG_CHAR, ERx("-remote_system_user")); 
		}
		break;
	case 1431:
#line 8901 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{ 
		  arg_str_add(ARG_CHAR, ERx("-remote_system_password"));
		}
		break;
	case 1433:
#line 8907 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		  arg_str_add(ARG_CHAR, ERx("-dbms_password"));
	        }
		break;
	case 1437:
#line 8916 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-1].s, ERx("options")))
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
					    ERx("CONNECT"), ERx("OPTIONS") );
		}
		break;
	case 1441:
#line 8928 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 8942 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 1443:
#line 8946 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 1444:
#line 8952 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 8961 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IISQCONNECT );
		}
		break;
	case 1446:
#line 8966 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    esq_sqmods(IImodWITH);  	/* force in modifier */
		    gen_call( IISQCONNECT );	/* Close CONNECT */

		    arg_int_add(IIsqpINI);

		  /* Initialize for 2PC with-clause */
		    gr->gr_flag &= ~(GR_HIGHX|GR_LOWX);	
		}
		break;
	case 1447:
#line 8977 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
			/* All specified in string variable */
			arg_int_add(DB_CHR_TYPE);
			arg_str_add(ARG_CHAR, (char *)0);
			arg_int_add(0);
			gen_call(IISQPARMS);
		}
		break;
	case 1448:
#line 8985 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 8998 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
			/* Start the next call */
			arg_int_add(IIsqpADD);
		}
		break;
	case 1452:
#line 9004 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* End the current call */
		    gen_call(IISQPARMS);
		}
		break;
	case 1453:
#line 9010 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add(DB_CHR_TYPE);
		    arg_str_add( ARG_CHAR, form_sconst(yypvt[-0].s) );
		    arg_int_add(0);
		    if (gr->gr_flag & GR_HIGHX || gr->gr_flag & GR_LOWX)
			er_write( E_EQ0079_grCONNECTID, EQ_ERROR, 0 );
		}
		break;
	case 1454:
#line 9018 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add(DB_CHR_TYPE);
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    arg_int_add(0);
		    if (gr->gr_flag & GR_HIGHX || gr->gr_flag & GR_LOWX)
			er_write( E_EQ0079_grCONNECTID, EQ_ERROR, 0 );
		}
		break;
	case 1455:
#line 9026 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add(DB_INT_TYPE);
		    arg_str_add( ARG_CHAR, (char *)0 );
		    arg_str_add( ARG_INT, yypvt[-0].s );
		}
		break;
	case 1456:
#line 9032 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9076 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9088 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9100 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9122 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    er_write( E_EQ0070_grMISSWORD, EQ_ERROR, 2, ERx("INTO"),
							ERx("COPY SQLERROR") );
		}
		break;
	case 1465:
#line 9135 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9145 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    esq->flag |= ESQ_NOPRT;	/* Suppress printing */
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-1].s, yypvt[-0].s );
		}
		break;
	case 1467:
#line 9150 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    esq->flag |= ESQ_NOPRT;	/* Suppress printing */
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-3].s, yypvt[-2].s );
		}
		break;
	case 1473:
#line 9164 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1474:
#line 9168 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-2].i || yypvt[-0].i;	/* DEFAULT keyword */
		}
		break;
	case 1475:
#line 9173 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    id_free();
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1476:
#line 9178 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    id_free();
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1477:
#line 9184 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    id_free();
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1483:
#line 9223 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1484:
#line 9228 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1485:
#line 9232 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1486:
#line 9237 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		    esq->flag &= ~ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 1487:
#line 9242 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		    esq->flag &= ~ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 1488:
#line 9247 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		    esq->flag &= ~ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 1489:
#line 9252 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		    esq->flag |= ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 1490:
#line 9257 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		    esq->flag &= ~ESQ_WITHABLE_CONSTRAINT;
		}
		break;
	case 1491:
#line 9263 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9277 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9290 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9309 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1496:
#line 9313 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-1].i;
		}
		break;
	case 1497:
#line 9318 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9328 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9338 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9342 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = CRE_WDEF;
		}
		break;
	case 1501:
#line 9346 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = CRE_OK;
		}
		break;
	case 1502:
#line 9350 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = CRE_NDEF;
		}
		break;
	case 1503:
#line 9355 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9365 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    	db_key( yypvt[-0].s );
		}
		break;
	case 1505:
#line 9372 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9383 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    	db_key( yypvt[-0].s );
		}
		break;
	case 1509:
#line 9390 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    	db_sconst( yypvt[-0].s );
		}
		break;
	case 1510:
#line 9396 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    	db_key( yypvt[-0].s );
		}
		break;
	case 1511:
#line 9402 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Don't print if in DECLARE TABLE statement */
		    if ((esq->flag & ESQ_NOPRT) == 0)
		    	db_key( yypvt[-0].s );
		}
		break;
	case 1513:
#line 9410 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9420 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9449 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9478 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IISQEXIT );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ050A_FIPS_EXTSN, EQ_WARN,
					1, ERx("DISCONNECT") );
		}
		break;
	case 1530:
#line 9486 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-0].s, NULL );
		    esq_init();
		}
		break;
	case 1531:
#line 9492 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IISQSESS );
		}
		break;
	case 1532:
#line 9496 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add ( IIsqdisALL );	
		    gen_call( IISQSESS );
		}
		break;
	case 1533:
#line 9501 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* connection name */
		    gen_call( IISQCNNM );
		}
		break;
	case 1536:
#line 9519 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    eqgen_tl(IIGETDATA);
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		}
		break;
	case 1537:
#line 9526 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9540 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9561 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
                    eqgen_tl(IIPUTDATA);
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
                }
		break;
	case 1541:
#line 9568 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9586 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IIEVTSTAT );
		    gr->gr_flag |= GR_GETEVT;	/* Special WHENEVER handling */
		}
		break;
	case 1543:
#line 9592 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9605 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( 0 );	/* "no wait" */
		}
		break;
	case 1547:
#line 9612 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (!ESQ_EQSTR(yypvt[-1].s, ERx("WAIT")))
		    {
			er_write(E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-1].s,
					ERx("GET DBEVENT"), ERx("WAIT"));
		    }
		}
		break;
	case 1548:
#line 9621 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9650 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    eqtl_add(gr->gr_id, (PTR)gr->gr_sym, gr->gr_nlid, (PTR)gr->gr_nlsym,
				gr->gr_type, yypvt[-0].s);
		    if ( (gr->gr_type == T_STRUCT) && (eq->eq_flags & EQ_FIPS) )
			er_write( E_EQ050D_FIPS_VAR, EQ_WARN, 0 );
		}
		break;
	case 1554:
#line 9658 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1555:
#line 9662 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1556:
#line 9666 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1557:
#line 9671 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
                       gr->gr_id = yypvt[-0].s;
                       gr->gr_type = T_INT;
                       gr->gr_sym = (SYM *)0;
                       gr->gr_nlsym = (SYM *)0;
		}
		break;
	case 1558:
#line 9678 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9703 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( QAPRINTF );
		}
		break;
	case 1560:
#line 9708 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-0].s, NULL );
		}
		break;
	case 1564:
#line 9718 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (gr->gr_sym)
			arg_var_add( gr->gr_sym, gr->gr_id );
		    else
			arg_str_add( ARG_CHAR, gr->gr_id );
		}
		break;
	case 1567:
#line 9735 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
			eqgen_tl(IIINQSQL);
			if (eq->eq_flags & EQ_CHRPAD)
			    esq_sqmods(IImodNOCPAD);
		}
		break;
	case 1568:
#line 9742 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9762 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
			eqgen_tl(IISETSQL);
		}
		break;
	case 1570:
#line 9766 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    eqtl_add(gr->gr_id, (PTR)gr->gr_sym, gr->gr_nlid, 
			 (PTR)gr->gr_nlsym, gr->gr_type, ERx("connection_name"));
		    eqgen_tl(IISETSQL);
		}
		break;
	case 1571:
#line 9772 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9803 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
			gr->gr_id = yypvt[-0].s;
			gr->gr_type = T_CHAR;
			gr->gr_sym = (SYM *)0;
			gr->gr_nlsym = (SYM *)0;
		}
		break;
	case 1574:
#line 9812 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9822 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9843 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9879 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
			gr->gr_id = yypvt[-0].s;
			gr->gr_type = T_INT;
			gr->gr_sym = (SYM *)0;
			gr->gr_nlsym = (SYM *)0;
		}
		break;
	case 1581:
#line 9886 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
			gr->gr_id = yypvt[-0].s;
			gr->gr_type = T_CHAR;
			gr->gr_sym = (SYM *)0;
			gr->gr_nlsym = (SYM *)0;
		}
		break;
	case 1583:
#line 9894 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
			gr->gr_id = yypvt[-0].s;
			gr->gr_type = T_NONE;
			gr->gr_sym = (SYM *)0;
			gr->gr_nlsym = (SYM *)0;
		}
		break;
	case 1584:
#line 9915 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (yypvt[-0].i && (esq->inc & sqcaSQL) == 0)
		    {
			er_write( E_EQ0128_sqNOSQLCA, EQ_ERROR, 0 );
			esq->inc |= sqcaSQL;
		    }
		}
		break;
	case 1585:
#line 9925 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    esq->flag |= ESQ_NOPRT;	/* Suppress printing */
		    gr_mechanism( GR_EQSTMT, GR_sNODB, yypvt[-0].s, NULL );
		}
		break;
	case 1586:
#line 9933 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9966 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 9982 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    esqlca( yypvt[-1].i, sqcaCONTINUE, (char *)0 );
		    yyval.i = 0;
		}
		break;
	case 1589:
#line 9987 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    esqlca( yypvt[-3].i, sqcaGOTO, yypvt[-0].s );
		    yyval.i = 1;
		}
		break;
	case 1590:
#line 9992 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    esqlca( yypvt[-2].i, sqcaGOTO, yypvt[-0].s );
		    yyval.i = 1;
		}
		break;
	case 1591:
#line 9997 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    esqlca( yypvt[-2].i, sqcaCALL, yypvt[-0].s );
		    yyval.i = 1;
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0506_FIPS_CLAUSE, EQ_WARN,
				    2, ERx("CALL"), ERx("WHENEVER") );
		}
		break;
	case 1592:
#line 10005 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 10036 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    arg_int_add( yypvt[-0].i );		/* using names? */
		    gen_call( IISQDESCRIBE );
		}
		break;
	case 1594:
#line 10043 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 10060 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = IIsqdNULUSE;
		}
		break;
	case 1599:
#line 10064 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 10089 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IISQDESCINPUT );
		}
		break;
	case 1601:
#line 10095 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s, NULL );
		    esq_init();
		    gr->gr_flag |= GR_NOSYNC;
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		}
		break;
	case 1602:
#line 10166 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("EXECUTE IMMEDIATE") );
		}
		break;
	case 1603:
#line 10172 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("EXECUTE") );
		}
		break;
	case 1604:
#line 10179 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 10190 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IISQEXIMMED );
		}
		break;
	case 1606:
#line 10194 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_flag |= GR_NOSYNC;
		    esq->flag &= ~ESQ_DESC_SEL;
		}
		break;
	case 1607:
#line 10200 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_CHAR, form_sconst(yypvt[-0].s) );
		}
		break;
	case 1608:
#line 10204 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 10221 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    esq_sqmods(IImodDYNSEL);
		    esq->flag |= ESQ_REG_SELECT;
		    esl_query( ESL_START_QUERY );
		    gen_call( IISQEXIMMED );
		}
		break;
	case 1610:
#line 10229 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 10259 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    erec_desc( gr->gr_id );	    /* Save arg for IIcsDaGet */
		    esq->flag |= ESQ_DESC_SEL;
		}
		break;
	case 1612:
#line 10264 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    erec_setup( 0 );			/* Done with list */
		}
		break;
	case 1613:
#line 10269 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( 0 );			/* not using vars */
		    gen_call( IISQEXSTMT );
		}
		break;
	case 1615:
#line 10275 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IISQDAIN );
		}
		break;
	case 1616:
#line 10282 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( 1 );			/* using vars */
		    gen_call( IISQEXSTMT );
		    gr->gr_func = tEXECUTE;
		}
		break;
	case 1617:
#line 10289 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( 0 );			/* not using vars */
		    gen_call( IISQEXSTMT );
		}
		break;
	case 1620:
#line 10298 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_var( DB_REG, gr->gr_sym, gr->gr_id,
			    gr->gr_nlsym, gr->gr_nlid, (char *)0 );
		}
		break;
	case 1621:
#line 10303 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_var( DB_HDLR, gr->gr_sym, gr->gr_id,
			    gr->gr_nlsym, gr->gr_nlid, yypvt[-1].s );
		}
		break;
	case 1622:
#line 10344 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 10369 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 10404 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Copy (*gr) for "proc_exec" to build code. */
		    STRUCT_ASSIGN_MACRO(*gr, tempgr);
		}
		break;
	case 1627:
#line 10416 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 10429 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IIPROCINIT );
		}
		break;
	case 1631:
#line 10437 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IIPROCGTTP );
		}
		break;
	case 1632:
#line 10441 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (esq->flag & ESQ_PROC_BYREF)
		    {
			/* procs w/BYREF params act like singleton select */
			esq_sqmods(IImodSINGLE);
		    }
		}
		break;
	case 1633:
#line 10449 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IISQDAIN );
		}
		break;
	case 1638:
#line 10463 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IIPROCVALIO );
		}
		break;
	case 1642:
#line 10473 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
			arg_int_add( TRUE );
		}
		break;
	case 1643:
#line 10478 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( FALSE );
		}
		break;
	case 1644:
#line 10483 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 10514 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 10531 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 10543 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 10597 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    lbl_exit_block();
		}
		break;
	case 1651:
#line 10617 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 10640 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( gr->gr_rcnt );
		    gen_loop( G_OPEN, L_RETBEG, L_RETFLUSH, 
			    lbl_next(LBL_RET), IIPROCSTAT, C_NOTEQ, 0 );
		    gen_sqlca( sqcaSQPROC );
		    yyval.i = 1;		/* indicate result clause */
		}
		break;
	case 1653:
#line 10649 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (esq->flag & ESQ_PROC_BYREF)
			er_write( E_EQ005A_grBYREF, EQ_ERROR, 0);
				/* can't have both BYREF parm & RESULT ROW */
		    erec_setup( 1 );	/* Initialize struct indicators stuff */
		    esq->flag |= ESQ_PROC_RESULT;  /* & set flag */
		}
		break;
	case 1656:
#line 10661 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    erec_setup( 0 );	/* Done with one element */
		}
		break;
	case 1657:
#line 10666 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_rcnt++;
		}
		break;
	case 1658:
#line 10680 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IISQPREPARE );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("PREPARE") );
		}
		break;
	case 1659:
#line 10689 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 10700 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sLIBQ, yypvt[-0].s, NULL );
		    esq_init();
		    gr->gr_flag |= GR_NOSYNC;
		    arg_int_add( eq_genlang(eq->eq_lang) );  /* host language */
		}
		break;
	case 1661:
#line 10708 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_id = NULL;
		    yyval.i = 0;
		}
		break;
	case 1662:
#line 10713 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 1664:
#line 10720 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 10733 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_CHAR, form_sconst(yypvt[-0].s) );
		}
		break;
	case 1669:
#line 10762 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("DIRECT CONNECT") );
		}
		break;
	case 1670:
#line 10770 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("DIRECT DISCONNECT") );
		}
		break;
	case 1671:
#line 10778 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sSQL, yypvt[-1].s, yypvt[-0].s );
		    if (eq->eq_flags & EQ_FIPS)
			er_write( E_EQ0505_FIPS_STATMT, EQ_WARN,
				    1, ERx("DIRECT EXECUTE IMMEDIATE") );
		}
		break;
	case 1673:
#line 10787 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 10878 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    lbl_exit_block();
		}
		break;
	case 1716:
#line 10883 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 10895 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_STMTFREE );
		}
		break;
	case 1720:
#line 10911 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IIADDFORM );
		    gr->gr_flag &= ~GR_NOINDIR;
		}
		break;
	case 1721:
#line 10917 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gr->gr_flag |= GR_NOINDIR;
		}
		break;
	case 1722:
#line 10923 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 10942 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
# ifndef EQ_ADD_FNAME
		    er_write( E_EQ0061_grINTVAR, EQ_ERROR, 1, yypvt[-0].s );
# endif /* EQ_ADD_FNAME */
		}
		break;
	case 1724:
#line 10958 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 10979 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-1].s, NULL );
		    gen_call( IICLRSCR );
		}
		break;
	case 1726:
#line 10984 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IICLRFLDS );
		}
		break;
	case 1728:
#line 10990 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-1].s, NULL );
		    gr->gr_func = IIFLDCLEAR;
		}
		break;
	case 1729:
#line 11022 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IIFRSQDESC );
		}
		break;
	case 1730:
#line 11028 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IIFRSQDESC );
		}
		break;
	case 1731:
#line 11034 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_int_add( 1 );			/* is_form = TRUE */
		}
		break;
	case 1732:
#line 11041 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_CHAR, (char* )0 );	/* No table name */
		}
		break;
	case 1733:
#line 11046 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    STprintf( esq->sbuf, ERx("%s %s"), yypvt[-1].s, yypvt[-0].s );
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, esq->sbuf, NULL );
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_int_add( 0 );			/* is_form = FALSE */
		}
		break;
	case 1734:
#line 11054 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_CHAR, ERx("u") );
		}
		break;
	case 1735:
#line 11058 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_CHAR, ERx("a") );
		}
		break;
	case 1736:
#line 11062 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 11087 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 11107 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_CHAR, ERx("a") );
		}
		break;
	case 1739:
#line 11112 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1740:
#line 11116 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1741:
#line 11182 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 11216 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 11235 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 11254 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gr->gr_func = IIDISPFRM;
		}
		break;
	case 1745:
#line 11260 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    frs_mode( ERx("DISPLAY/INITTABLE"), yypvt[-0].s );
		}
		break;
	case 1746:
#line 11264 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    frs_mode( ERx("DISPLAY/INITTABLE"), yypvt[-0].s );
		}
		break;
	case 1747:
#line 11268 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    frs_mode( ERx("DISPLAY/INITTABLE"), yypvt[-0].s );
		}
		break;
	case 1748:
#line 11272 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_CHAR, ERx("u") );
		}
		break;
	case 1749:
#line 11276 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 11293 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_CHAR, ERx("f") );
		}
		break;
	case 1751:
#line 11298 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    sc_eat( NULL, SC_NEST|SC_SEEN, ERx(")"), '(', ')' );
		    er_write( E_EQ0051_grDISPTL, EQ_ERROR, 0 );
		}
		break;
	case 1753:
#line 11326 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 11343 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 11393 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 11403 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_flag &= ~GR_ACTIVATE;
		}
		break;
	case 1763:
#line 11411 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_else( G_ELSE, IIRETVAL, C_0, 0, L_ACTELSE, 
			lbl_current(LBL_INTR), L_ACTEND, lbl_current(LBL_FORM));
		    gr->gr_mode = LBLmDISPLAY;
		}
		break;
	case 1764:
#line 11418 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 11432 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Reset activate argument list */
		    act_args( FALSE, (char *)0, (char *)0, 0, (SYM *)0 );
		}
		break;
	case 1768:
#line 11446 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    act_add( ACT_FIELD, lbl_current(LBL_INTR), yypvt[-2].i, yypvt[-0].ip );
		    gr->gr_adjective = LBLaFIELD;
		}
		break;
	case 1769:
#line 11451 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    act_add( ACT_MENU, lbl_current(LBL_INTR), 0, yypvt[-1].ip );
		    gr->gr_adjective = LBLaMITEM;
		}
		break;
	case 1770:
#line 11456 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    act_add( ACT_SCROLL, lbl_current(LBL_INTR), sclUP, yypvt[-0].ip );
		    gr->gr_adjective = LBLaSCROLL;
		}
		break;
	case 1771:
#line 11461 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    act_add( ACT_SCROLL,lbl_current(LBL_INTR), sclDOWN, yypvt[-0].ip );
		    gr->gr_adjective = LBLaSCROLL;
		}
		break;
	case 1772:
#line 11466 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    i4  cur_intr = lbl_current( LBL_INTR );

		    act_add( ACT_COLUMN, cur_intr, yypvt[-3].i, yypvt[-1].ip );
		    act_add( ACT_COLUMN, cur_intr, yypvt[-3].i, yypvt[-0].ip );
		    gr->gr_adjective = LBLaCOLUMN;
		}
		break;
	case 1773:
#line 11474 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Old-style syntax - no longer supported */
                    er_write( E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1,
                                            ERx("ACTIVATE COMMAND") );
		}
		break;
	case 1775:
#line 11482 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 1776:
#line 11486 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 11501 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.ip = act_string( form_sconst(yypvt[-0].s) );
		}
		break;
	case 1778:
#line 11505 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.ip = act_string( yypvt[-0].s );
		}
		break;
	case 1779:
#line 11509 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.ip = act_string( yypvt[-0].s );
		}
		break;
	case 1780:
#line 11513 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 11526 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1782:
#line 11530 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1783:
#line 11544 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 11603 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 11622 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 11636 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 11652 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 1788:
#line 11656 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 1791:
#line 11664 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* It will check for errors */
		    act_args( TRUE, yypvt[-2].s, yypvt[-0].s, gr->gr_type, gr->gr_sym );
		}
		break;
	case 1792:
#line 11670 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1793:
#line 11674 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1794:
#line 11678 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1795:
#line 11682 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1796:
#line 11687 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = form_sconst(yypvt[-0].s);
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = NULL;
		}
		break;
	case 1797:
#line 11693 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_INT;
		    gr->gr_sym = NULL;
		}
		break;
	case 1798:
#line 11699 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 11726 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_goto( G_TERM, L_FDEND, lbl_current(LBL_FORM) );
		}
		break;
	case 1800:
#line 11731 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 11749 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 11774 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 11810 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 11830 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gen_call( IIENDFORMS );
		}
		break;
	case 1807:
#line 11845 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 11892 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    i4  cur_form = lbl_current( LBL_FORM );

		    gen_loop( G_CLOSE, L_FDBEG, L_FDEND, cur_form, IIFNAMES,
			C_0, 0 );
		    gen_label( G_LOOP, L_FDEND, cur_form );
		    gen_call( IIENDFRM );
		}
		break;
	case 1809:
#line 11902 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 11915 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1812:
#line 11929 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gr->gr_func = IIFORMINIT;
		}
		break;
	case 1813:
#line 11942 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( eq_genlang(eq->eq_lang) );
		    gen_call( IIFORMS );
		}
		break;
	case 1814:
#line 11966 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
                    if (eq->eq_flags & EQ_CHRPAD)
                        esq_sqmods(IImodNOCPAD);
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		}
		break;
	case 1815:
#line 11972 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
                    if (eq->eq_flags & EQ_CHRPAD)
                        esq_sqmods(IImodNOCPAD);
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		}
		break;
	case 1816:
#line 11979 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_if( G_OPEN, IIFSETIO, C_NOTEQ, 0 );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		    gr->gr_func = IIRETFIELD;
		}
		break;
	case 1817:
#line 11987 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1818:
#line 12002 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    er_write(E_EQ0076_grNOWUNSUPP, EQ_ERROR, 1, ERx("GETOPER"));
		    YYERROR;
		}
		break;
	case 1820:
#line 12019 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( 0 );		/* Dummy type right now */
		}
		break;
	case 1821:
#line 12025 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 12036 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 1823:
#line 12048 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IIHELPFILE );
		}
		break;
	case 1824:
#line 12053 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1825:
#line 12065 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( QAMESSAGE );
		}
		break;
	case 1826:
#line 12070 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1827:
#line 12091 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		    frs_close();
		}
		break;
	case 1829:
#line 12100 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    frs_head();
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);

		}
		break;
	case 1830:
#line 12108 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    frs_inqset( FRSinq );
		}
		break;
	case 1831:
#line 12114 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 1832:
#line 12118 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 1833:
#line 12122 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 1834:
#line 12126 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 1835:
#line 12130 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    frs_object( yypvt[-0].s );
		}
		break;
	case 1836:
#line 12134 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    frs_object( "row" );
		}
		break;
	case 1839:
#line 12142 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    frs_parentname( yypvt[-0].s, T_CHAR, (PTR)0 );
		}
		break;
	case 1840:
#line 12146 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    frs_parentname( yypvt[-0].s, T_CHAR, (PTR)0 );
		}
		break;
	case 1841:
#line 12150 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    frs_parentname( yypvt[-0].s, T_INT, (PTR)0 );
		}
		break;
	case 1842:
#line 12154 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 12177 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    er_write( E_EQ0077_grISUNSUPP, EQ_ERROR, 1,
							ERx("INQUIRE_FRS") );
		    YYERROR;
		}
		break;
	case 1848:
#line 12189 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    frs_iqvar( gr->gr_id, gr->gr_type, gr->gr_sym,
			gr->gr_nlid, gr->gr_nlsym );
		}
		break;
	case 1849:
#line 12195 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    frs_constant( yypvt[-0].s, (char *)0, T_NONE, (SYM *)0 );
		}
		break;
	case 1850:
#line 12199 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    er_write( E_EQ0054_grNOPARENS, EQ_ERROR, 0 );
		    frs_constant( yypvt[-2].s, (char *)0, T_NONE, (SYM *)0 );
		}
		break;
	case 1851:
#line 12204 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Should be an error */
		    frs_constant( yypvt[-3].s, yypvt[-1].s, gr->gr_type, gr->gr_sym );
		}
		break;
	case 1852:
#line 12211 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1853:
#line 12215 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1854:
#line 12219 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1855:
#line 12223 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1856:
#line 12227 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1857:
#line 12231 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1858:
#line 12235 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1859:
#line 12239 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1860:
#line 12243 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1861:
#line 12247 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1862:
#line 12252 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1863:
#line 12258 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1864:
#line 12264 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_INT;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1865:
#line 12270 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1866:
#line 12276 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1867:
#line 12282 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1868:
#line 12288 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1869:
#line 12294 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1870:
#line 12300 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1871:
#line 12306 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		    gr->gr_type = T_CHAR;
		    gr->gr_sym = (SYM *)0;
		}
		break;
	case 1872:
#line 12312 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 12337 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( QAPROMPT );
		}
		break;
	case 1874:
#line 12342 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{ 
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1875:
#line 12360 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IIMESSAGE ); 
    		}
		break;
	case 1876:
#line 12365 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
            	    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
    	    	    gr->gr_func = IIMESSAGE;
		}
		break;
	case 1879:
#line 12389 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    EQFW3glgen();	
		}
		break;
	case 1880:
#line 12394 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    EQFWopen(gr->gr_func);
		}
		break;
	case 1884:
#line 12410 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    EQFWwcoption(yypvt[-2].s, yypvt[-0].s);
		}
		break;
	case 1885:
#line 12415 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1886:
#line 12419 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1887:
#line 12424 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1888:
#line 12428 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1893:
#line 12453 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		EQFWsuboption(yypvt[-2].s, gr->gr_type, gr->gr_id, (PTR)gr->gr_sym);
	}
		break;
	case 1894:
#line 12458 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
	    	    yyval.s = yypvt[-0].s;
	        }
		break;
	case 1895:
#line 12462 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1896:
#line 12466 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1897:
#line 12472 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_INT;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 1898:
#line 12478 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_CHAR;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 1899:
#line 12484 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_FLOAT;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 1900:
#line 12490 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_PACKASCHAR;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 1901:
#line 12496 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (!gr->gr_sym)
		    {
			gr->gr_type = T_CHAR;
		    }	
		}
		break;
	case 1902:
#line 12503 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
	    	    gr->gr_sym = (SYM *)0;
	    	    gr->gr_type = T_CHAR;
	    	    gr->gr_id = yypvt[-0].s;
	    	}
		break;
	case 1903:
#line 12509 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_sym = (SYM *)0;
		    gr->gr_type = T_CHAR;
		    gr->gr_id = yypvt[-0].s;
		}
		break;
	case 1904:
#line 12515 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_sym = (SYM *)0;
		    gr->gr_type = T_CHAR;
		    gr->gr_id = yypvt[-0].s;
		}
		break;
	case 1905:
#line 12529 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IIPRNSCR );
		}
		break;
	case 1906:
#line 12534 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1907:
#line 12539 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Should check that $2.s is "file" */
		    if (STbcompare(yypvt[-3].s, 0, ERx("file"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-3].s,
					    ERx("PRINTSCREEN"), ERx("FILE") );
		}
		break;
	case 1908:
#line 12546 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		}
		break;
	case 1909:
#line 12551 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1910:
#line 12571 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		    gen_call( IINPROMPT );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);

		}
		break;
	case 1911:
#line 12581 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gr->gr_func = IINPROMPT;
		}
		break;
	case 1912:
#line 12587 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (STbcompare(yypvt[-0].s, 0, ERx("noecho"), 0, TRUE) != 0)
			er_write( E_EQ0069_grBADWORD, EQ_ERROR, 3, yypvt[-0].s,
						ERx("PROMPT"), ERx("NOECHO") );
		    arg_int_add( 1 );
		}
		break;
	case 1913:
#line 12594 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( 0 );
		}
		break;
	case 1914:
#line 12599 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 12648 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		}
		break;
	case 1916:
#line 12654 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IIFSETIO, C_0, 0 );
		}
		break;
	case 1917:
#line 12661 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_if( G_OPEN, IIFSETIO, C_NOTEQ, 0 );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		    gr->gr_func = IISETFIELD;
		}
		break;
	case 1918:
#line 12669 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1919:
#line 12681 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    gen_call( IIREDISP );
		}
		break;
	case 1920:
#line 12702 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 12728 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1922:
#line 12733 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = (char *)0;	
		}
		break;
	case 1923:
#line 12737 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 12763 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-1].s;	
		    gen_call( IIRESFLD );
		}
		break;
	case 1925:
#line 12768 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-2].s;	
		    gen_call( IIRESCOL );
		}
		break;
	case 1926:
#line 12773 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 12840 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 1928:
#line 12855 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    frs_close();
		}
		break;
	case 1930:
#line 12864 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    frs_inqset( FRSset );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		}
		break;
	case 1935:
#line 12883 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    frs_setval( form_sconst(yypvt[-0].s), T_CHAR, (SYM *)0,
			(char *)0, (SYM *)0 );
		}
		break;
	case 1936:
#line 12888 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    frs_setval( yypvt[-0].s, T_INT, (SYM *)0, (char *)0, (SYM *)0 );
		}
		break;
	case 1937:
#line 12892 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    frs_setval(ERx("0"), T_INT, (SYM *)0, (char *)0, (SYM *)0);
		}
		break;
	case 1938:
#line 12897 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    frs_setval(ERx("0"), T_INT, (SYM *)0, (char *)0, (SYM *)0);
		}
		break;
	case 1939:
#line 12902 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    frs_setval( yypvt[-0].s, T_CHAR, (SYM *)0, (char *)0, (SYM *)0 );
		}
		break;
	case 1940:
#line 12906 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    frs_setval( yypvt[-0].s, T_CHAR, (SYM *)0, (char *)0, (SYM *)0 );
		}
		break;
	case 1941:
#line 12910 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 12922 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 12943 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    frs_setval( ERx("1"), T_INT, (SYM *)0, (char *)0,
				(SYM *)0 );
		}
		break;
	case 1944:
#line 12956 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IISLEEP );
		}
		break;
	case 1945:
#line 12961 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1946:
#line 12994 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 13012 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 13028 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_else( G_ELSE, IIRETVAL, C_0, 0, L_ACTELSE, 
			lbl_current(LBL_INTR), L_ACTEND, 
			lbl_current(LBL_FORM) );
		    gr->gr_mode = LBLmSUBMENU;
		}
		break;
	case 1952:
#line 13037 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_mode = LBLmSUBMENU;	/* Not LBLmDISPLAY */
		}
		break;
	case 1953:
#line 13042 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_mode = LBLmSUBMENU;	/* Not LBLmDISPLAY */
		}
		break;
	case 1954:
#line 13046 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_mode = LBLmSUBMENU;	/* Not LBLmDISPLAY */
		}
		break;
	case 1955:
#line 13051 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    act_add( ACT_MENU, lbl_current(LBL_INTR), 0, yypvt[-1].ip );
		    gr->gr_adjective = LBLaMITEM;
		}
		break;
	case 1957:
#line 13070 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 13084 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1959:
#line 13089 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_func = IICHKFRM;
		}
		break;
	case 1960:
#line 13093 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_func = IIVALFLD;
		}
		break;
	case 1961:
#line 13115 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 1962:
#line 13121 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IITCLRCOL;
		}
		break;
	case 1963:
#line 13127 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmCLEARR );
		}
		break;
	case 1965:
#line 13134 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    er_write( E_EQ0054_grNOPARENS, EQ_ERROR, 0 );
		    gen_call( IITCLRROW );
		}
		break;
	case 1966:
#line 13139 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IITCLRROW );
		}
		break;
	case 1967:
#line 13160 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 13171 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 13181 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmDELETER );
		}
		break;
	case 1971:
#line 13188 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_VERSWARN)
			er_write( E_EQ0078_grSTYUNSUPP, EQ_WARN, 1,
							ERx("DELETEROW OUT") );
		}
		break;
	case 1972:
#line 13195 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( FALSE );
		    gen_if( G_OPEN, IITDELROW, C_NOTEQ, 0 );
		}
		break;
	case 1973:
#line 13200 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_VERSWARN)
			er_write( E_EQ0078_grSTYUNSUPP, EQ_WARN, 1,
							ERx("DELETEROW IN") );
		}
		break;
	case 1974:
#line 13207 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( TRUE );	/* In list */
		    gen_if( G_OPEN, IITDELROW, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 1975:
#line 13234 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
                    if (eq->eq_flags & EQ_CHRPAD)
                        esq_sqmods(IImodNOCPAD);
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 1976:
#line 13241 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
                    if (eq->eq_flags & EQ_CHRPAD)
                        esq_sqmods(IImodNOCPAD);
		    gen_call( IITBCECOLEND );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 1977:
#line 13249 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
                    if (eq->eq_flags & EQ_CHRPAD)
                        esq_sqmods(IImodCPAD);
		    gr->gr_func = IITCOLRET;
		}
		break;
	case 1978:
#line 13257 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmGETR );
		}
		break;
	case 1979:
#line 13275 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IITFILL );
		    gen_if( G_CLOSE, IITBINIT, C_0, 0 );
		}
		break;
	case 1980:
#line 13281 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_if( G_OPEN, IITBINIT, C_NOTEQ, 0 );
		}
		break;
	case 1981:
#line 13286 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 1985:
#line 13295 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    gen_call( IITHIDECOL );
		}
		break;
	case 1989:
#line 13312 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{ gr->gr_sym = (SYM *)0; }
		break;
	case 1990:
#line 13314 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 13329 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    id_add( form_sconst(yypvt[-0].s) );
		}
		break;
	case 1993:
#line 13333 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 13350 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    id_add( yypvt[-0].s );
		}
		break;
	case 1995:
#line 13355 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;		/* Paren not stuff present */
		}
		break;
	case 1996:
#line 13359 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    id_add( yypvt[-2].s );
		    id_add( yypvt[-1].s );
		    id_add( yypvt[-0].s );
		    yyval.i = 1;		/* Paren stuff present */
		}
		break;
	case 1997:
#line 13366 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 13380 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;		/* No NULL clause */
		}
		break;
	case 1999:
#line 13384 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 13389 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (yypvt[-0].i == 0)
			id_add( ERx(" not null") );
		    else
			id_add( ERx(" not null with default") );
		    yyval.i = 1;		/* With NULL clause */
		}
		break;
	case 2002:
#line 13402 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 2003:
#line 13406 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 2004:
#line 13421 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 13433 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmPURGE );
		}
		break;
	case 2006:
#line 13472 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 13481 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 13491 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gen_if( G_OPEN, IITINSERT, C_NOTEQ, 0 );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 2009:
#line 13500 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmINSERTR );
		}
		break;
	case 2010:
#line 13535 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IITBCECOLEND );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IITBACT, C_0, 0 );
		}
		break;
	case 2011:
#line 13542 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IITBCECOLEND );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IITBACT, C_0, 0 );
		}
		break;
	case 2012:
#line 13550 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( TRUE );		/* Loading */
		    gen_if( G_OPEN, IITBACT, C_NOTEQ, 0 );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 2013:
#line 13559 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 2016:
#line 13567 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    frs_inqset(FRSload);
		    frs_object(ERx("row"));
		}
		break;
	case 2017:
#line 13593 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IITBCECOLEND );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 2018:
#line 13600 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IITBCECOLEND );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 2019:
#line 13608 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 2020:
#line 13616 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmPUTR );
		}
		break;
	case 2021:
#line 13642 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Close inner and scroll if blocks */
		    gen_if( G_CLOSE, IITSCROLL, C_0, 0 );
		    gen_if( G_CLOSE, IITBSETIO, C_0, 0 );
		}
		break;
	case 2022:
#line 13649 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( rowNONE );
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		}
		break;
	case 2023:
#line 13655 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmSCROLL );
		}
		break;
	case 2024:
#line 13661 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_if( G_OPEN, IITSCROLL, C_NOTEQ, 0 );
		}
		break;
	case 2025:
#line 13665 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if ((yypvt[-1].i || yypvt[-0].i) && (eq->eq_flags & EQ_VERSWARN))
			er_write( E_EQ0078_grSTYUNSUPP, EQ_WARN, 1,
							ERx("SCROLL IN/OUT") );
		}
		break;
	case 2026:
#line 13672 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gen_call( IITBSMODE );
		    arg_int_add( FALSE );	/* Ignore In flag */
		}
		break;
	case 2028:
#line 13680 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( rowEND );
		}
		break;
	case 2029:
#line 13685 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gen_call( IITBSMODE );
		}
		break;
	case 2030:
#line 13690 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		    gen_call( IITBSMODE );
		}
		break;
	case 2033:
#line 13699 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_func = IITCOLRET;
		}
		break;
	case 2034:
#line 13704 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( FALSE );
		    arg_int_add( rowNONE );
		    gen_if( G_OPEN, IITSCROLL, C_NOTEQ, 0 );
		}
		break;
	case 2036:
#line 13712 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( TRUE );	/* In list */
		    arg_int_add( rowNONE );
		    gen_if( G_OPEN, IITSCROLL, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLSET;
		}
		break;
	case 2037:
#line 13734 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 13745 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 13791 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 13802 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		    gen_call( IITBCECOLEND );
		}
		break;
	case 2041:
#line 13808 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		    gen_call( IITBCECOLEND );	
		}
		break;
	case 2042:
#line 13815 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 13832 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		}
		break;
	case 2044:
#line 13853 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 13870 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{	
		    gen_if( G_OPEN, IITBSETIO, C_NOTEQ, 0 );
		    gr->gr_func = IITCOLVAL;
		    yyval.s = yypvt[-3].s;
		}
		break;
	case 2046:
#line 13877 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_sFORMS, yypvt[-0].s, NULL );
		    arg_int_add( cmVALIDR );
		}
		break;
	case 2048:
#line 13884 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    er_write( E_EQ0054_grNOPARENS, EQ_ERROR, 0 );
		    gen_call( IITVALROW );
		}
		break;
	case 2049:
#line 13889 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IITVALROW );
		}
		break;
	case 2050:
#line 13900 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 13919 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 2052:
#line 13923 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 2054:
#line 13939 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    er_write( E_EQ0054_grNOPARENS, EQ_ERROR, 0 );
		}
		break;
	case 2059:
#line 13950 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 2060:
#line 13954 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 13981 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 2062:
#line 13994 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{	
		    arg_int_add( 0 );
		    arg_var_add( gr->gr_null, ERx("null") ); 
		}
		break;
	case 2064:
#line 14012 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    er_write( E_EQ0054_grNOPARENS, EQ_ERROR, 0 );
		}
		break;
	case 2069:
#line 14023 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( gr->gr_func );
		}
		break;
	case 2070:
#line 14027 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 14047 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 14060 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( rowCURR );
		}
		break;
	case 2075:
#line 14068 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_CHAR, (char *)0 );
		    if (lbl_gen(LBLmDISPLAY, FALSE) == LBL_NOLEV)
			er_write( E_EQ0056_grFORMNAME, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 2076:
#line 14077 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    er_write( E_EQ0071_grASSGNOP, EQ_ERROR, 1, yypvt[-0].s );
		}
		break;
	case 2080:
#line 14098 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = form_sconst(yypvt[-0].s);
		}
		break;
	case 2081:
#line 14104 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 2082:
#line 14110 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( 0 );		/* null indicator */
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 2084:
#line 14123 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_CHAR, yypvt[-0].s );
		}
		break;
	case 2085:
#line 14127 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 14150 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 14171 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2089:
#line 14175 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    char	ibuf[50];

		    ibuf[0] = '-';
		    ibuf[1] = '\0';
		    yyval.s = str_add( STRNULL, STcat(ibuf, yypvt[-0].s) );
		}
		break;
	case 2090:
#line 14183 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2091:
#line 14190 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_INT, yypvt[-0].s );
		}
		break;
	case 2092:
#line 14196 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( 0 );		/* null indicator */
		    arg_str_add( ARG_INT, yypvt[-0].s );
		}
		break;
	case 2093:
#line 14204 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2094:
#line 14208 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    char	fbuf[50];

		    fbuf[0] = '-';
		    fbuf[1] = '\0';
		    yyval.s = str_add( STRNULL, STcat(fbuf, yypvt[-0].s) );
		}
		break;
	case 2095:
#line 14216 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2096:
#line 14232 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( 0 );		/* null indicator */
		    arg_str_add( ARG_FLOAT, yypvt[-0].s );
		}
		break;
	case 2097:
#line 14240 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2098:
#line 14244 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    char	dbuf[50];

		    dbuf[0] = '-';
		    dbuf[1] = '\0';
		    yyval.s = str_add( STRNULL, STcat(dbuf, yypvt[-0].s) );
		}
		break;
	case 2099:
#line 14252 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2100:
#line 14258 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( 0 );		/* null indicator */
		    arg_str_add( ARG_PACK, yypvt[-0].s );
		}
		break;
	case 2102:
#line 14268 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    arg_int_add( 0 );
		}
		break;
	case 2103:
#line 14273 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    arg_int_add( 0 );
		}
		break;
	case 2104:
#line 14278 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 14303 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 14316 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_type = T_INT;
		}
		break;
	case 2107:
#line 14320 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_type = T_FLOAT;
		}
		break;
	case 2108:
#line 14324 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_type = T_PACKASCHAR;
		}
		break;
	case 2109:
#line 14328 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_type = T_CHAR;
		}
		break;
	case 2110:
#line 14335 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 14415 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 14426 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 14442 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = G4CT_PROC;
		}
		break;
	case 2131:
#line 14446 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = G4CT_FRAME;
		}
		break;
	case 2134:
#line 14457 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 14471 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 14487 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add(ARG_CHAR, yypvt[-2].s);
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 2137:
#line 14492 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add(ARG_CHAR, yypvt[-2].s);
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 2138:
#line 14497 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 14506 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add(ARG_CHAR, ERx(""));
		    yyval.i = yypvt[-0].i;
		}
		break;
	case 2140:
#line 14513 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 14525 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 2142:
#line 14529 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 0;
		}
		break;
	case 2144:
#line 14536 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call(IIG4RETVAL);
		}
		break;
	case 2145:
#line 14547 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IIG4CLRARR );
		}
		break;
	case 2146:
#line 14553 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2147:
#line 14564 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add( eq_genlang(eq->eq_lang) );/* host language */
		    arg_str_add( ARG_RAW, gr->gr_id );	/* sqlda */
		    gen_call( IIG4DESCRIBE );
		}
		break;
	case 2148:
#line 14572 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2149:
#line 14592 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
			if (eq->eq_flags & EQ_CHRPAD)
			    esq_sqmods(IImodNOCPAD);
			gen_if( G_CLOSE, IIG4CHKOBJ, C_EQ, 0);
		}
		break;
	case 2150:
#line 14600 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 14625 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2152:
#line 14631 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call(IIG4USEDESCR);
		}
		break;
	case 2154:
#line 14638 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call(IIG4GETATTR);
		}
		break;
	case 2155:
#line 14642 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call(IIG4GETATTR);
		}
		break;
	case 2156:
#line 14653 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 14679 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2158:
#line 14691 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_INT, yypvt[-2].s);
		    arg_str_add( ARG_INT, yypvt[-1].s);
		    gen_call( IIG4GETROW );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		}
		break;
	case 2159:
#line 14701 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		}
		break;
	case 2160:
#line 14709 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 14729 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodNOCPAD);
		}
		break;
	case 2163:
#line 14735 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
                    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_CHRPAD)
			esq_sqmods(IImodCPAD);
		    gr->gr_func = IIINQ4GL;
		}
		break;
	case 2166:
#line 14748 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call(IIINQ4GL);
		}
		break;
	case 2167:
#line 14759 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 14798 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2169:
#line 14804 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 14817 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    INSROW4GL_PARMS *prmptr = (INSROW4GL_PARMS *)(yypvt[-2].ip);

		    set_insrow_param(yypvt[-0].s, gr->gr_id, gr->gr_type, 
				     gr->gr_sym, prmptr);
		    yyval.ip = yypvt[-2].ip;
		}
		break;
	case 2171:
#line 14827 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 2172:
#line 14839 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IIG4REMROW );
		}
		break;
	case 2173:
#line 14845 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2174:
#line 14868 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IIG4CALLCOMP );
		    gen_if( G_CLOSE, IISENDUSEREVENT, C_EQ, 0);
		}
		break;
	case 2175:
#line 14875 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
                    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-1].s, NULL );
		    gen_if( G_OPEN, IISENDUSEREVENT, C_EQ, 0);
		}
		break;
	case 2179:
#line 14889 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add(ARG_CHAR, yypvt[-2].s);
		    args_toend(1, 2);
		    gen_call(IIG4VALPARAM);
		}
		break;
	case 2180:
#line 14903 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
		}
		break;
	case 2181:
#line 14910 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
                    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		    gr->gr_func = IISET4GL;
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		}
		break;
	case 2184:
#line 14923 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call(IISET4GL);
		}
		break;
	case 2185:
#line 14943 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodNOCEOS);
			gen_if( G_CLOSE, IIG4CHKOBJ, C_EQ, 0);
		}
		break;
	case 2186:
#line 14951 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 14977 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2188:
#line 14982 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call(IIG4USEDESCR);
		}
		break;
	case 2190:
#line 14989 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call(IIG4SETATTR);
		}
		break;
	case 2191:
#line 14993 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call(IIG4SETATTR);
		}
		break;
	case 2192:
#line 15004 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 15017 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		    if (eq->eq_flags & EQ_CHREOS)
			esq_sqmods(IImodCEOS);
		}
		break;
	case 2194:
#line 15030 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IIG4SETROW );
		}
		break;
	case 2195:
#line 15036 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2196:
#line 15042 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (STbcompare(ERx("row"), 0, yypvt[-3].s, 0, TRUE) != 0)
		    {
			    er_write( E_EQ0081_TL_ATTR_UNKNOWN, EQ_ERROR, 1,
								yypvt[-3].s);
		    }
		}
		break;
	case 2197:
#line 15057 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_call( IIG4DELROW );
		}
		break;
	case 2198:
#line 15063 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_EQSTMT, GR_s4GL, yypvt[-0].s, NULL );
		}
		break;
	case 2199:
#line 15079 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (STbcompare(ERx("rownumber"), 0, yypvt[-3].s, 0, TRUE) != 0)
		    {
			    er_write( E_EQ0081_TL_ATTR_UNKNOWN, EQ_ERROR, 1,
								yypvt[-3].s);
		    }
		}
		break;
	case 2200:
#line 15090 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_str_add( ARG_INT, yypvt[-0].s);
		}
		break;
	case 2201:
#line 15097 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2202:
#line 15101 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2203:
#line 15108 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add(0);	/* Null indicator */
		    arg_var_add( gr->gr_sym, gr->gr_id );
		}
		break;
	case 2204:
#line 15116 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 15131 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 1;
		}
		break;
	case 2206:
#line 15135 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    arg_int_add(0);
		    yyval.i = 0;
		}
		break;
	case 2208:
#line 15147 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = NULL;
		}
		break;
	case 2209:
#line 15151 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 15163 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		}
		break;
	case 2211:
#line 15166 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_type = T_INT;
		    gr->gr_id = yypvt[-0].s;
		    gr->gr_sym = NULL;
		}
		break;
	case 2212:
#line 15172 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_type = T_FLOAT;
		    gr->gr_id = yypvt[-0].s;
		    gr->gr_sym = NULL;
		}
		break;
	case 2213:
#line 15178 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_type = T_PACKASCHAR;
		    gr->gr_id = yypvt[-0].s;
		    gr->gr_sym = NULL;
		}
		break;
	case 2214:
#line 15184 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_type = T_CHAR;
		    gr->gr_id = yypvt[-0].s;
		    gr->gr_sym = NULL;
		}
		break;
	case 2215:
#line 15193 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 15219 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    arg_int_add( 0 );
		}
		break;
	case 2220:
#line 15224 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    er_write( E_EQ0059_grINT, EQ_ERROR, 1, yypvt[-0].s );
		    arg_int_add( 0 );
		}
		break;
	case 2221:
#line 15229 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 15249 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
#line 15262 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_type = T_INT;
		}
		break;
	case 2224:
#line 15266 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_type = T_FLOAT;
		}
		break;
	case 2225:
#line 15270 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_type = T_PACKASCHAR;
		}
		break;
	case 2226:
#line 15274 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_type = T_CHAR;
		}
		break;
	case 2227:
#line 15281 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    i4  code;

		    code = g4_check(gr->gr_func, yypvt[-1].s, (bool)yypvt[-0].i);
		    arg_int_add(code);
		}
		break;
	case 2228:
#line 15290 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2229:
#line 15294 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2230:
#line 15336 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yyval.s );
		}
		break;
	case 2231:
#line 15342 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 2233:
#line 15349 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    id_add( yypvt[-0].s );
		}
		break;
	case 2234:
#line 15363 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    db_key( yypvt[-0].s );
		}
		break;
	case 2235:
#line 15392 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    dml->dm_exec = DML_HOST;
		}
		break;
	case 2236:
#line 15396 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    dml->dm_exec = DML_HOST;
		}
		break;
	case 2237:
#line 15401 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (gr->Cb_print)
			gen_term( TRUE );
		}
		break;
	case 2238:
#line 15407 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    dml->dm_exec = DML_HOST;
		    if (*SC_PTR == '.')
			SC_PTR++;
		}
		break;
	case 2239:
#line 15414 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (*SC_PTR == '.')
			SC_PTR++;
		}
		break;
	case 2240:
#line 15426 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
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
	case 2241:
#line 15452 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2242:
#line 15456 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2243:
#line 15460 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2244:
#line 15465 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2245:
#line 15469 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2246:
#line 15473 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    id_add(yypvt[-1].s);
		    id_add(yypvt[-0].s);
		    yyval.s = str_add(STRNULL, id_getname());
		    id_free();
		}
		break;
	case 2247:
#line 15480 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    id_add(yypvt[-2].s);
		    id_add(yypvt[-1].s);
		    id_add(yypvt[-0].s);
		    yyval.s = str_add(STRNULL, id_getname());
		    id_free();
		}
		break;
	case 2249:
#line 15498 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (dml->dm_exec != (DML_EXEC|DML__SQL))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"),
							ERx("BEGIN DECLARE") );
		    dml->dm_exec = DML_DECL;
		    eq->eq_flags |= EQ_INDECL;	/* Scanner in declare mode */
		}
		break;
	case 2250:
#line 15507 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (dml->dm_exec != (DML_EXEC|DML__SQL))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"),
							ERx("END DECLARE") );
		    dml->dm_exec = DML_HOST;
		    eq->eq_flags &= ~EQ_INDECL;	/* Scanner exits declare mode */
		}
		break;
	case 2252:
#line 15517 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr_mechanism( GR_STMTFREE );	/* Clear str table */
		}
		break;
	case 2257:
#line 15533 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    dml->dm_exec = DML_DECL;
		    gen_host( G_H_OP|G_H_NEWLINE, ERx("...DECLARE ERROR.") );
		}
		break;
	case 2258:
#line 15566 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    register SYM	*sy, *psy;
		    i4			eq_type, eq_len;
		    register COB_SYM	*cs;

		    /* if data-item is just a 88 condition-name definition
		    ** just ignore it.  Condition-names can't be used as
		    ** host variable but they are supportive of the real
		    ** host variable that is superior to it.
		    */

		    if (gr->Cb_sym.cs_lev == 88)  
			break;

		    /*
		    ** Update global COB_SYM pointer with type info.
		    ** Rules are :
		    **  Picture and type - evaluate;
		    **  Picture and no type - usage is display;
	            **  No picture and type - better be allowed for that type;
		    **  No picture and no type - record entry.
	            */
		    cob_use_type(cob_rtpic(), cob_rtusage(), &gr->Cb_sym);

		    /* Set EQUEL type and length - will differ from COBOL */
		    cob_to_eqtype( &gr->Cb_sym, &eq_type, &eq_len );
		    sym_hint_type( (SYM *)0, eq_type, 0 );

		    /* Enter name into symbol table at the correct level */
		    sy = symDcPL1( yypvt[-2].s, gr->Cb_sym.cs_lev -1, 1, syFisVAR, 
				   COBCLOSURE, 0 );
		    if (sy == (SYM *)0)	/* Probably redeclaration */
			break;
		    /* 
		    ** COBOL level, type, num-of-digits and scale are already 
		    ** set, so copy into the SYM value field.
		    */
		    cs = cob_newsym();   /* Allocate a new COB_SYM element */
		    MEcopy( (char *)&gr->Cb_sym,(u_i2)sizeof(*cs),(char *)cs );
		    sym_s_vlue( sy, cs );
		    sym_s_btype( sy, eq_type );
		    sym_s_dsize( sy, eq_len );
		    /* 
		    ** If there was an Occurs clause or the parent was an
		    ** array then set the array bit. 
		    */
		    if (cob_rtoccurs() && cs->cs_lev == 1) /* Occurs clause on level 01 */
			er_write( E_E4000C_hcbOCCURS, EQ_WARN, 0 );
		    psy = sym_g_parent( sy );
		    if (cob_rtoccurs() || (psy && sym_g_dims(psy)))
			sym_s_dims( sy, 1 );

		    /* check for SQLCODE and/or SQLSTATE */
		    if (!(eq->eq_flags & EQ_NOSQLCODE)
                     && (STbcompare(yypvt[-2].s, 0, ERx("SQLCODE"), 0, TRUE) == 0))
                    {
			if (!(eq->eq_flags & EQ_SQLCODE))
			{
			    /*
			    ** This is an error, because we probably
			    ** included the wrong eqslqca.
			    */
			    er_write( E_E40018_hcbNOSQLCODE, EQ_ERROR, 0,
				(char *)0 );
			    eq->eq_flags |= EQ_SQLCODE;
			}
			if (eq_type != T_INT)
			{
			    er_write( E_EQ0517_SQLSTATUS_DECL, EQ_ERROR, 2,
			    ERx("SQLCODE"), ERx("PICTURE S9(9) USAGE COMP."));
			}
			else
			{
                            eq->eq_flags |= EQ_SQLCODE;
			}
                    }
                    if (STbcompare(yypvt[-2].s, 0, ERx("SQLSTATE"), 0, TRUE) == 0)
                    {
			if (eq_type != T_CHAR)
			{
			    er_write( E_EQ0517_SQLSTATUS_DECL, EQ_ERROR, 2,
			      ERx("SQLSTATE"), ERx("PICTURE X(5)."));
			}
			else
			{
                            eq->eq_flags |= EQ_SQLSTATE;
			}
                    }

		}
		break;
	case 2273:
#line 15678 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_OP|G_H_NEWLINE, ERx(".") );
		}
		break;
	case 2274:
#line 15694 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    i4		last_lev;	/* Last level number */
		    i4		cur_lev;	/* Current level */

		    /* init data decl save stuff: 
		    **    usage=NOTYPE, occurs=FALSE, picture=NULL
		    */
		    cob_svinit();

		    CVan( yypvt[-0].s, &cur_lev );
		    last_lev = gr->Cb_sym.cs_lev;
		    if (cur_lev == 77)
			cur_lev = 1;
		    else if (cur_lev == 88)
		    {  /* if 88 level condition-name, ignore */
		    }
		    else if (cur_lev > 49 )
		    {
			er_write( E_E40006_hcbLEVEL, EQ_ERROR, 1, yypvt[-0].s );
			if (cur_lev == 66)
			    cur_lev = 1;
		    }
		    else if (cur_lev <= 0)
		    {
			er_write( E_E40006_hcbLEVEL, EQ_ERROR, 1, yypvt[-0].s );
			cur_lev = 1;
		    }
		    else if (cur_lev > last_lev 
			&& gr->Cb_sym.cs_type != COB_RECORD)
		    {
			/* Parent node (if there is one) must be record type */
			er_write( E_E40008_hcbNOPAR, EQ_ERROR, 1, yypvt[-0].s );
			cur_lev = 1;
		    }
		    if (cur_lev == 1 && cur_lev != last_lev)	/* Pop to 01 */
		    {
			/* Outdent declarations, and update Record level */
			while (gr->Cb_rec)
			{
			    if (eq_ansifmt)
			        (void) Cput_seqno();	/* Emit sequence no. */
			    gen_host( G_H_OUTDENT, (char *)0 );
			    gr->Cb_rec--;
			}
		    }
		    else if (cur_lev > last_lev)
		    {
			gr->Cb_rec++;
			if (eq_ansifmt)
			    (void) Cput_seqno();	/* Emit sequence no. */
			gen_host( G_H_INDENT, (char *)0 );
		    }
		    else if (cur_lev < last_lev)
		    {
			gr->Cb_rec--;
			if (eq_ansifmt)
			    (void) Cput_seqno();	/* Emit sequence no. */
			gen_host( G_H_OUTDENT, (char *)0 );
		    }
		    gr->Cb_sym.cs_lev = (i1)cur_lev;
		    if (cur_lev == 1)
		    {
			if (eq_ansifmt)
			    (void) Cput_seqno();	/* Emit sequence no. */
			gen_cob01( yypvt[-0].s );	/* special AreaA output rtn */
		    }
		    else
		    {
			if (eq_ansifmt)		
			    (void) Cput_seqno();	/* Emit sequence no. */
			gen_host( G_H_KEY, yypvt[-0].s );
		    }		
		}
		break;
	case 2275:
#line 15777 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-2].s;
		}
		break;
	case 2276:
#line 15783 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2277:
#line 15789 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2278:
#line 15794 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    /* If FILLER is used do not reenter the word */
		    yyval.s = ERx("");
		}
		break;
	case 2279:
#line 15800 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = ERx("");	/* Do not reenter the word */
		}
		break;
	case 2280:
#line 15806 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2281:
#line 15811 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2282:
#line 15823 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    char	*pic;

		    gen_host( G_H_KEY, yypvt[-0].s );
		    /* Strip off [Is] Picture string */
		    if (pic = cob_picture())
			gen_host( G_H_KEY, pic );
		    cob_svpic(pic);	/* Save picture */
		}
		break;
	case 2283:
#line 15842 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    cob_svusage(cob_type(yypvt[-0].s)); /* Determine Cobol Comp type */
		}
		break;
	case 2284:
#line 15847 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    cob_svusage( COB_DISPLAY );
		}
		break;
	case 2285:
#line 15852 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    cob_svusage( COB_INDEX );
		}
		break;
	case 2286:
#line 15857 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		    er_write( E_E40011_hcbUSAGE, EQ_ERROR, 1, yypvt[-0].s );
		    cob_svusage( COB_BADTYPE );	/* So we can ignore picture */
		}
		break;
	case 2287:
#line 15864 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2288:
#line 15869 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2290:
#line 15875 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2293:
#line 15887 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2294:
#line 15892 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2295:
#line 15897 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2296:
#line 15902 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2299:
#line 15914 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2300:
#line 15919 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2301:
#line 15923 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2304:
#line 15937 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2306:
#line 15944 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2307:
#line 15949 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2309:
#line 15955 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2311:
#line 15972 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2312:
#line 15977 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2313:
#line 15981 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_OP, ERx(" ") );
		    gen_host( G_H_OP, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2314:
#line 15987 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_OP, ERx(" ") );
		    gen_host( G_H_OP, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2315:
#line 15993 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2316:
#line 15997 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_OP, ERx(" ") );
		    gen_host( G_H_OP, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2317:
#line 16003 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_OP, ERx(" ") );
		    gen_host( G_H_OP, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2318:
#line 16009 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2319:
#line 16013 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_OP, ERx(" ") );
		    gen_host( G_H_OP, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2320:
#line 16019 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_OP, ERx(" ") );
		    gen_host( G_H_OP, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2321:
#line 16025 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_SCONST, yypvt[-0].s );
		}
		break;
	case 2322:
#line 16029 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2323:
#line 16033 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2324:
#line 16038 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-1].s );
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2326:
#line 16059 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    cob_svoccurs( TRUE );
		}
		break;
	case 2327:
#line 16064 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2330:
#line 16072 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2332:
#line 16078 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2334:
#line 16084 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2336:
#line 16091 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2337:
#line 16096 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2341:
#line 16109 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2342:
#line 16114 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2347:
#line 16124 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2348:
#line 16129 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_OP, yypvt[-0].s );
		}
		break;
	case 2351:
#line 16141 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2352:
#line 16146 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gen_host( G_H_KEY, yypvt[-0].s );
		}
		break;
	case 2354:
#line 16157 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* 
		    ** Strip to end of line, but ensure scanner is synchronized
		    ** via 'lex_need_input' - see yylex routines.  This is
		    ** because the newline is the EXEC trigger.
		    */
		    sc_reset();
		    if (inc_parse_name(yypvt[-1].s, FALSE) == SC_INC)
			inc_push_file();
		}
		break;
	case 2355:
#line 16168 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    /* Better be "<EOF>" else Yacc will produce syntax error */
		    dml->dm_exec = DML_DECL;
		}
		break;
	case 2356:
#line 16174 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (dml->dm_exec != (DML_EXEC|DML__SQL))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"),
							    ERx("INCLUDE") );
		    dml->dm_exec = DML_DECL;
		}
		break;
	case 2357:
#line 16182 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2358:
#line 16186 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.s = yypvt[-0].s;
		}
		break;
	case 2359:
#line 16190 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    STprintf( esq->sbuf, ERx("%s.%s"), yypvt[-2].s, yypvt[-0].s );
		    yyval.s = esq->sbuf;
		}
		break;
	case 2360:
#line 16202 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    dml->dm_exec = DML_DECL;
		}
		break;
	case 2361:
#line 16207 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    esq->flag |= ESQ_NOPRT;     /* Suppress printing */
		    if (dml->dm_exec != (DML_EXEC|DML__SQL))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"),
							ERx("DECLARE TABLE") );
		}
		break;
	case 2362:
#line 16214 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    esq->flag |= ESQ_NOPRT;     /* Suppress printing */
		    if (dml->dm_exec != (DML_EXEC|DML__SQL))
			er_write( E_EQ0127_sqEXECMD, EQ_ERROR, 2, ERx("SQL"),
							ERx("DECLARE TABLE") );
		}
		break;
	case 2367:
#line 16256 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    sc_hostvar = TRUE;	/* disable keyword lookups */
		}
		break;
	case 2369:
#line 16262 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    er_write( E_EQ0129_sqVARCOLON, EQ_ERROR, 0 );
		}
		break;
	case 2374:
#line 16299 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    COB_SYM	*cs;
		    SYM		*sy;

		    gr->gr_type = T_UNDEF;		/* Error default */
		    if ((sy = yypvt[-0].v) == (SYM *)0)
		    {
			/* Do nothing - error already printed */
		    }
		    else
		    {
			cs = (COB_SYM *)sym_g_vlue( sy );
			if (cs->cs_type == COB_RECORD)
			{
			    if (!gr->Cb_err)
			        er_write( E_E40005_hcbELEM, EQ_ERROR, 1,
								id_getname() );
			}
			else
			    gr->gr_type = sym_g_btype( sy );
		    }
		    gr->gr_sym = sy;
		    gr->gr_id = str_add( STRNULL, id_getname() );
		    id_free();
		}
		break;
	case 2375:
#line 16330 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    sc_hostvar = FALSE;	/* re-enable keyword lookups */
		}
		break;
	case 2376:
#line 16334 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    sc_hostvar = FALSE;	/* re-enable keyword lookups */
		    sc_popscptr();	/* move SC_PTR back one word */
		}
		break;
	case 2377:
#line 16340 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    gr->gr_id = str_add( STRNULL, id_getname() );
		    id_free();
		    sym_f_init(); 	/* Discard sym stuff - ignored */
		}
		break;
	case 2378:
#line 16347 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    sc_hostvar = TRUE;	/* disable keyword lookups */
		}
		break;
	case 2382:
#line 16371 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    SYM 	*sy, *mem;
		    char	*sid, *mid;
		    COB_SYM	*cs;

		    sid = str_add( STRNULL, id_getname() );
		    if ((sy = yypvt[-0].v) == (SYM *)0)
		    {
			/* Do nothing, as error already printed */
		    }
		    else
		    {
			cs = (COB_SYM *)sym_g_vlue( sy );
			if (cs->cs_type == COB_RECORD)
			{
			    for (mem=syStrInit(sy); mem; mem=syStrNext(mem))
			    {
				STprintf( esq->sbuf, ERx("%s IN %s"), 
					sym_str_name(mem), sid );
				mid = str_add( STRNULL, esq->sbuf );
				cs = (COB_SYM *)sym_g_vlue( mem );
				if (cs->cs_type == COB_NOTYPE)
				{
				    if (!gr->Cb_err)
				        er_write( E_EQ0057_grHOSTVAR, EQ_ERROR,
								    1, mid );
				}
				else if (cs->cs_type == COB_RECORD ||
				    (sym_g_dims(mem) && !gr->Cb_dims))
				{
				     if (!gr->Cb_err)
					er_write( E_E40005_hcbELEM, EQ_ERROR,
								    1, mid );
				}
				erec_mem_add( mid, mem, sym_g_btype(mem) );
			    }
			}
			else
			{
			    erec_mem_add( sid, sy, sym_g_btype(sy) );
			}
		    }
		    id_free();
		}
		break;
	case 2383:
#line 16429 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    SYM 	*sy, *mem;
		    char	*sid, *mid;
		    COB_SYM 	*cs;

		    sid = str_add( STRNULL, id_getname() );
		    if ((sy = yypvt[-0].v) == (SYM *)0)
		    {
			/* Do nothing, as error already printed */
			yyval.ip = (i4 *)0;
		    }
		    else
		    {
			cs = (COB_SYM *)sym_g_vlue( sy );
			if (cs->cs_type == COB_RECORD)
			{
			    for (mem=syStrInit(sy); mem; mem=syStrNext(mem))
			    {
				STprintf( esq->sbuf, ERx("%s IN %s"), 
					sym_str_name(mem), sid );
				mid = str_add( STRNULL, esq->sbuf );
				cs = (COB_SYM *)sym_g_vlue( mem );
				if (cs->cs_type == COB_NOTYPE)
				{
				    if (!gr->Cb_err)
				        er_write( E_EQ0057_grHOSTVAR, EQ_ERROR,
								    1, mid );
				}
				else if (cs->cs_type == COB_RECORD ||
				    (sym_g_dims(mem) && !gr->Cb_dims))
				{
				     if (!gr->Cb_err)
					er_write( E_E40005_hcbELEM, EQ_ERROR,
								    1, mid );
				}
				erec_mem_add( mid, mem, sym_g_btype(mem) );
			    }
			}
			else
			    erec_mem_add( sid, sy, sym_g_btype(sy) );
		    }
		    id_free();
		}
		break;
	case 2384:
#line 16488 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    sc_hostvar = FALSE;	/* re-enable keyword lookups */
		}
		break;
	case 2385:
#line 16492 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    sc_hostvar = FALSE;	/* re-enable keyword lookups */
		    sc_popscptr();	/* move SC_PTR back a word */
		}
		break;
	case 2386:
#line 16498 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.v = yypvt[-0].v;
		}
		break;
	case 2387:
#line 16503 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    sym_f_init();
		    gr->Cb_dims = 0;
		    gr->Cb_err = 0;
		    yyval.i = 0;	/* Default way to look up entry in C_usename */
		}
		break;
	case 2388:
#line 16511 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.v = gr_resolve_cob( FALSE, yypvt[-0].i );
		}
		break;
	case 2389:
#line 16515 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.v = gr_resolve_cob( FALSE, FALSE );
		    gr->gr_type = T_STRUCT;
		}
		break;
	case 2390:
#line 16520 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.v = gr_resolve_cob( TRUE, FALSE );
		    gr->gr_type = T_STRUCT;
		}
		break;
	case 2394:
#line 16543 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    id_key( yypvt[-0].s );
		    yyval.i = 0;
		}
		break;
	case 2395:
#line 16548 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    id_key( yypvt[-0].s );
		    yyval.i = 0;
		}
		break;
	case 2399:
#line 16564 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    id_add( yypvt[-0].s );
		    yyval.i = 1;
		}
		break;
	case 2400:
#line 16571 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    yyval.i = 1;		/* Indicates simple variable */
		}
		break;
	case 2401:
#line 16575 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    SYM		*sy = (SYM *)0;
		    i4		stat;	/* Ignored */

		    /*
		    ** Lookup the entry here, even though we have not parsed
		    ** the fully qualified path of names yet.  If at this point
		    ** we have an ambiguous reference, that's okay 'cause most
		    ** COBOL compilers will complain too! Reference must be 
		    ** unambiguous by the time the left paren is seen.
		    **
		    ** 01 a pic s9(4) usage comp.
		    ** 01 arr.
		    **	 02 objs occurs 3 times.
		    **     03 a pic s9(4) usage comp.
		    **
		    ** move 10 to a(2) in arr.  	! Ambiguous reference
		    **
		    */
		    yyval.i = 1;  			/* Assume simple var */
		    if (yypvt[-2].i)	/* PL/I */
			stat = symRsPL1( &sy, 1, syFisVAR|syFisREVERSE );
		    else
			stat = symRsPL1( &sy, 1, syFisVAR );
		    if (sy != (SYM *)0)
		    {
			/* 
			** Is this an EQUEL array reference?  Base type 
			** (EQUEL type is defined) and has array bit set.
			*/
			if ((sym_g_btype(sy) != T_NONE && sym_g_dims(sy)) || 
			    ((COB_SYM *)sym_g_vlue(sy))->cs_type == COB_RECORD )
			{
			    sc_eat( id_add, SC_SEEN|SC_NEST, ERx(")"),
								    '(', ')' );
			    gr->Cb_dims = 1;
			    yyval.i = 0; 		/* Not a simple variable */
			}
			else
			    SC_PTR--;
		    }
		    else
			SC_PTR--;
		}
		break;
	case 2402:
#line 16621 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    sym_fpush( sym_find(yypvt[-0].s) );
		    id_add( yypvt[-0].s );
		}
		break;
	case 2403:
#line 16628 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/embed/cobolsq/cobolsq.yf"
{
		    if (eq_ansifmt)             /* if ansi format       */
			(void) Cput_seqno();    /*    emit sequence no. */
		    gen_host( G_H_KEY | G_H_NEWLINE, yypvt[-0].s );
		}
		break;
		}
	return 0; /* Success */

	}
