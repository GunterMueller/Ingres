
typedef union 
{
	char *cptr;
	char datatxt[ MAX_MANIFEST_LINE + 1 ];
	uchar comparison;
} YYSTYPE;

extern YYSTYPE		yylval;

#define	tDATA		257
#define	tCOMPARISON		258
#define	tAGGREGATE		259
#define	tBUILD_DIR		260
#define	tBUILD_FILE		261
#define	tCAPABILITY		262
#define	tCHECKSUM		263
#define	tCUSTOM		264
#define	tDEFINITION		265
#define	tDELETE		266
#define	tDIRECTORY		267
#define	tDYNAMIC		268
#define	tEXECUTABLE		269
#define	tFILE		270
#define	tINCLUDE		271
#define	tINVISIBLE		272
#define	tLEFT_BRACE		273
#define	tPREFER		274
#define	tLINK		275
#define	tNEED		276
#define	tPACKAGE		277
#define	tPART		278
#define	tPERMISSION		279
#define	tPRELOAD		280
#define	tRELEASE		281
#define	tRIGHT_BRACE		282
#define	tSCRIPT		283
#define	tSETUP		284
#define	tSIZE		285
#define	tSTATE		286
#define	tVERSION		287
#define	tVISIBLE		288
#define	tVISIBLEPKG		289
#define	tVOLUME		290
#define	tEOF		291
