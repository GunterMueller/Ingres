
#define YYSTDEF
typedef union 
{
    i4              psl_tytype;         /* Operator types, etc. */
    PST_QNODE       *psl_trtype;        /* Query tree node pointer */
    PSS_RNGTAB	    *psl_rngtype;	/* Range table entry */
    i4	    psl_inttype;	/* General-purpose number */
    i2		    *psl_i2type;
    i4		    *psl_i4type;
    u_char	    *psl_dectype;
    f4		    *psl_f4type;
    f8		    *psl_f8type;
    char	    *psl_strtype;
    DB_TEXT_STRING  *psl_textype;	/* A text string, with 2-byte count */
    PST_QTREE	    *psl_qttype;	/* Query tree type */
    DB_CURSOR_ID    *psl_cursid;	/* A cursor id */
    PSC_CURBLK	    *psl_crblk;		/* A cursor control block */
    DB_DATA_VALUE   *psl_dbval;		/* A db data value */
    PST_RESKEY	    *psl_reskey;	/* key struct ptr */
} YYSTYPE;
# define ABORT 257
# define AGOP 258
# define ALL 259
# define APPEND 260
# define AS 261
# define AT 262
# define BAOP 263
# define BAOPH 264
# define BDOP 265
# define BGNXACT 266
# define BY 267
# define CLSCURSOR 268
# define COLNO 269
# define COLON 270
# define COMMA 271
# define COMMITTED 272
# define COPY 273
# define CREATE 274
# define DECCONST 275
# define DEFCURSOR 276
# define DEFERUPD 277
# define DEFINTEG 278
# define DEFLOC 279
# define DEFPERM 280
# define DEFQRY 281
# define DEFREF 282
# define DEFVIEW 283
# define DELIM_IDENT 284
# define DELETE 285
# define DESTROY 286
# define DESTINTEG 287
# define DESTPERM 288
# define DESTREF 289
# define DIRECTUPD 290
# define DISTRIBUTE 291
# define DOLLAR 292
# define ENDXACT 293
# define EOP 294
# define EXECUTE 295
# define EXCLUDING 296
# define F4CONST 297
# define F8CONST 298
# define FOR 299
# define FORREADONLY 300
# define FORUPDATE 301
# define FROM 302
# define HELP 303
# define HEXCONST 304
# define I2CONST 305
# define I4CONST 306
# define IN 307
# define INDEX 308
# define INTO 309
# define INTEGRITY 310
# define IS 311
# define ISOLATION 312
# define ISOP 313
# define ISNULL 314
# define JOURNALING 315
# define LBOP 316
# define LEVEL 317
# define LOGGING 318
# define LPAREN 319
# define MODIFY 320
# define MOVE 321
# define NAME 322
# define NOT 323
# define NOTNULL 324
# define OF 325
# define ON 326
# define ONLY 327
# define ONLYWHERE 328
# define OPENCURSOR 329
# define ORDERBY 330
# define PERIOD 331
# define PERMIT 332
# define PRINT 333
# define QDATA 334
# define QUERYID 335
# define RANGE 336
# define READ 337
# define RELOCATE 338
# define REPEATABLE 339
# define REPLACE 340
# define REPCURSOR 341
# define REPLICATE 342
# define RETCURSOR 343
# define RETRIEVE 344
# define RPAREN 345
# define SAVE 346
# define SAVEPOINT 347
# define SCONST 348
# define SERIALIZABLE 349
# define SET 350
# define SETAUTOCOMMIT 351
# define SETAGGR 352
# define SETCPUFACT 353
# define SETDATEFMT 354
# define SETDECIMAL 355
# define SETFLATTEN 356
# define SETHASH 357
# define SETIOTRACE 358
# define SETJOINOP 359
# define SETJOURNAL 360
# define SETLOCKMODE 361
# define SETLTRACE 362
# define SETLOGGING 363
# define SETLOGTRACE 364
# define SETMNYFMT 365
# define SETMXIO 366
# define SETMXROW 367
# define SETMXCPU 368
# define SETMXPAGE 369
# define SETMXCOST 370
# define SETMNYPREC 371
# define SETOJFLATTEN 372
# define SETOPTIMIZEONLY 373
# define SETPARALLEL 374
# define SETPRINTQRY 375
# define SETQEP 376
# define SETRANDOMSEED 377
# define SETRETINTO 378
# define SETRSTRUCT 379
# define SETSESSION 380
# define SETSQL 381
# define SETSTATS 382
# define SETTRACE 383
# define SETTRANSACTION 384
# define SETUPDROWCNT 385
# define SORT 386
# define TABID 387
# define TO 388
# define UAOP 389
# define UNCOMMITTED 390
# define UNIQUE 391
# define UNTIL 392
# define UPDATE 393
# define VAR 394
# define VIEW 395
# define WHERE 396
# define WITH 397
# define WITHNULL 398
# define WRITE 399
# define unaryop 400
