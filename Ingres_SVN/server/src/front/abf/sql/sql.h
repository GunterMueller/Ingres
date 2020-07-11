#define	ALL		257
#define	ARRAY_OF		258
#define	BEGIN		259
#define	BYREF		260
#define	CALL		261
#define	CALLF		262
#define	CALLP		263
#define	DECLARE		264
#define	DEREFERENCE		265
#define	DISPLAY_MENU		266
#define	DO		267
#define	ELSE		268
#define	ELSEIF		269
#define	ENDIF		270
#define	ENDLOOP		271
#define	END		272
#define	ENDWHILE		273
#define	EXE_PROC		274
#define	EXIT		275
#define	FIELD		276
#define	FIELD_ENTRY		277
#define	FIELD_EXIT		278
#define	IF		279
#define	INITIALIZE		280
#define	KEY		281
#define	MODE		282
#define	NEXT		283
#define	NULLK		284
#define	ON_DBEVENT		285
#define	PROCEDURE		286
#define	PROCEDURE_RETURNING		287
#define	CLASS_OF		288
#define	REPEAT		289
#define	RETURN		290
#define	RUN_MENU		291
#define	QUALIFICATION		292
#define	SESSION		293
#define	SYSTEM		294
#define	THEN		295
#define	USER		296
#define	WHILE		297
#define	WITH		298
#define	ABORT		299
#define	ALTER_DEFAULT		300
#define	ALTER_PROFILE		301
#define	ALTER_TABLE		302
#define	ALTER_USER		303
#define	ALTER_LOCATION		304
#define	ALTER_SECAUDIT		305
#define	BEGINTRANSACTION		306
#define	COMMIT		307
#define	COPY		308
#define	CREATE		309
#define	CRE_DBEVENT		310
#define	CRE_PROFILE		311
#define	CRE_USER		312
#define	CRE_SECALARM		313
#define	CRE_LOCATION		314
#define	CURRENT_INST		315
#define	DELETE		316
#define	DBMS_PASSWORD		317
#define	DISABLE_SECAUDIT		318
#define	DROP		319
#define	DRP_DBEVENT		320
#define	DRP_PROFILE		321
#define	DRP_VIEW		322
#define	DRP_PROC		323
#define	DRP_LINK		324
#define	DRP_LOCATION		325
#define	DRP_SECALARM		326
#define	DRP_USER		327
#define	ENABLE_SECAUDIT		328
#define	ENDTRANSACTION		329
#define	EXECUTE_IMMED		330
#define	GET_DBEVENT		331
#define	GRANT		332
#define	INDEX		333
#define	CRE_INTEGRITY		334
#define	INSERT		335
#define	MODIFY		336
#define	CRE_PERMIT		337
#define	RAISE_DBEVENT		338
#define	REFERENCING		339
#define	REGISTER		340
#define	REGISTER_DBEVENT		341
#define	RELOCATE		342
#define	REMOVE_DBEVENT		343
#define	DRP_INTEGRITY		344
#define	DRP_PERMIT		345
#define	ROLLBACK		346
#define	SAVE		347
#define	SAVEPOINT		348
#define	SELECT		349
#define	SET		350
#define	TABLE		351
#define	UNION		352
#define	UPDATE		353
#define	CRE_VIEW		354
#define	GLOB_TEMP		355
#define	CRE_SYNONYM		356
#define	DRP_SYNONYM		357
#define	FROM_GROUP		358
#define	FROM_ROLE		359
#define	FROM_USER		360
#define	TO_GROUP		361
#define	TO_ROLE		362
#define	TO_USER		363
#define	CONNECT		364
#define	DISCONNECT		365
#define	IDENTIFIED_BY		366
#define	ANY		367
#define	ASC		368
#define	AT		369
#define	BETWEEN		370
#define	BY		371
#define	DEFAULT		372
#define	DESC		373
#define	DISTINCT		374
#define	ESCAPE		375
#define	EXISTS		376
#define	FOR		377
#define	FROM		378
#define	GROUP		379
#define	HAVING		380
#define	IN		381
#define	INTO		382
#define	IS		383
#define	JOIN		384
#define	OF		385
#define	ON		386
#define	ORDER		387
#define	SOME		388
#define	TO		389
#define	UNIQUE		390
#define	UNTIL		391
#define	VALUES		392
#define	WHERE		393
#define	WORK		394
#define	INNER_JOIN		395
#define	FULL_JOIN		396
#define	LEFT_JOIN		397
#define	RIGHT_JOIN		398
#define	ON_COMMIT		399
#define	ON_DATABASE		400
#define	ON_LOCATION		401
#define	BY_USER		402
#define	BY_GROUP		403
#define	BY_ROLE		404
#define	CHECK		405
#define	CONSTRAINT		406
#define	FOREIGN_KEY		407
#define	PRIMARY_KEY		408
#define	REFERENCES		409
#define	COMMENT_ON		410
#define	ADD		411
#define	ALTER_GROUP		412
#define	ALTER_ROLE		413
#define	CRE_GROUP		414
#define	CRE_ROLE		415
#define	CRE_RULE		416
#define	DRP_GROUP		417
#define	DRP_ROLE		418
#define	DRP_RULE		419
#define	NEW_AS		420
#define	OLD_AS		421
#define	REVOKE		422

typedef union  {
	char		*st_name;
#ifdef YACC
	OSSYM		*st_sym;
	OSNODE		*st_node;
	OSTLIST		*st_tlist;
	OSSRTNODE	*st_srtlist;
	OSQRY		*st_qry;
#endif
	i4		st_nat;
} YYSTYPE;

extern YYSTYPE		yylval;

#define	AVG		423
#define	COUNT		424
#define	MAX		425
#define	MIN		426
#define	SUM		427
#define	UNION_OP		428
#define	AS		429
#define	BELL_TOK		430
#define	OFF		431
#define	OWNER_INGRES		432
#define	USER_AUTH		433
#define	SYSTEM_USER		434
#define	SESSION_USER		435
#define	SESSION_GROUP		436
#define	SESSION_ROLE		437
#define	ADD_PRIVILEGES		438
#define	DROP_PRIVILEGES		439
#define	DIR_CONNECT		440
#define	DIR_DISCONNECT		441
#define	DIR_EXECUTE		442
#define	IMMEDIATE		443
#define	REG_TABLE		444
#define	REM_TABLE		445
#define	REG_VIEW		446
#define	REM_VIEW		447
#define	REMOVE		448
#define	CLEAR		449
#define	HELPFILE		450
#define	HELP_FORMS		451
#define	MESSAGE		452
#define	NOECHO		453
#define	PRINTSCREEN		454
#define	PROMPT		455
#define	REDISPLAY		456
#define	RESUME		457
#define	RESENTRY		458
#define	RESNEXT		459
#define	RESMENU		460
#define	RESNFLD		461
#define	RESPFLD		462
#define	SCREEN		463
#define	SCROLL		464
#define	SLEEP		465
#define	VALIDATE		466
#define	INQ_FORMS		467
#define	INQ_INGRES		468
#define	SET_FORMS		469
#define	SET_INGRES		470
#define	INQ_4GL		471
#define	SET_4GL		472
#define	SET_CONNECTION		473
#define	SET_RANDOM_SEED		474
#define	CLEARROW		475
#define	DELETEROW		476
#define	INITTABLE		477
#define	INSERTROW		478
#define	VALIDROW		479
#define	LOADTABLE		480
#define	UNLOADTABLE		481
#define	PURGETBL		482
#define	COLID		483
#define	ID		484
#define	FCONST		485
#define	ICONST		486
#define	SCONST		487
#define	XCONST		488
#define	DCONST		489
#define	AND		490
#define	EXP		491
#define	GTE		492
#define	LIKE		493
#define	LTE		494
#define	NOT		495
#define	NOTLIKE		496
#define	NOTEQ		497
#define	OR		498
#define	LSQBRK		499
#define	RSQBRK		500
#define	COLEQ		501
#define	LBOP		502
#define	ROP		503
#define	BOP		504
#define	BOPH		505
#define	UOP		506
#define	UOP_POSTFIX		507