
typedef union 
{
	f8	real;
	char	*string;
	i4	integer;
	CR_NODE	*node;
} YYSTYPE;

extern YYSTYPE		yylval;

#define	STR_CON		257
#define	EXEC_TEXT		258
#define	EXEC_TEXT_STR		259
#define	SYMBOL		260
#define	PATTERN		261
#define	COMPARE_OP		262
#define	LOGIC_OP		263
#define	NUM_CON		264
#define	BOOL_CON		265
#define	REQUIRES		266
#define	VALID		267
#define	MIN		268
#define	MAX		269
#define	PRIME		270
#define	POWER2		271
#define	IF		272
#define	ELSE		273
#define	ELSEIF		274
#define	ENDIF		275
#define	UNDEFINED		276
#define	DIRECTORY		277
#define	FILESPEC		278
#define	SIGNED_INT		279
#define	DECIMAL		280
#define	SIZETYPE		281
#define	SUM		282
