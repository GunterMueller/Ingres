
#line 160 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
# include <compat.h>
# include <st.h>
# include <si.h>
# include <lo.h>
# include <nm.h>
# include <er.h>
# include <cm.h>
# include <cv.h>
# include <me.h>
# include <gv.h>
# include <iplist.h>
# include <erst.h>
# include <ip.h>
# include <ipcl.h>
# include <ipcibits.h>
# include <iphash.h>
# include <gl.h>

# ifdef VMS
# define DEFAULT_FILE_PERMISSION	ERx( "s:rwd,o:rwd,g:r,w:r" )
# define DEFAULT_SCRIPT_PERMISSION	ERx( "s:rwed,o:rwed,g:re,w:re" )
# define DEFAULT_EXECUTABLE_PERMISSION	ERx( "s:rwed,o:rwed,g:re,w:re" )
# else
# define DEFAULT_FILE_PERMISSION	ERx( "s:r,o:rwd,g:r,w:r" )
# define DEFAULT_SCRIPT_PERMISSION	ERx( "s:re,o:rwed,g:re,w:re" )
# define DEFAULT_EXECUTABLE_PERMISSION	ERx( "s:re,o:rwed,g:re,w:re" )
# endif
# define COMMENT_CHAR	ERx( "#" )

PKGBLK *currPkg;
PRTBLK *currPrt;
FILBLK *currFil;
REFBLK *currRef;

GLOBALREF char installDir[];
GLOBALREF char releaseManifest[];
GLOBALREF char Version[];
GLOBALREF bool batchMode;
GLOBALREF bool pmode;

GLOBALDEF FILE *batchOutf = 0;
GLOBALDEF FILE *rSTREAM = 0;

static LIST *pkgListP;
static char **releaseIdP;
static char **prodNameP;
static char manifestDir[ MAX_LOC + 1 ];

static void ci2bit();
static void init_reference( LIST *, char *, uchar );
static void newfile( char *, PERMISSION, bool );
static i4  compute_apparent_size( LIST*, PKGBLK *, i4  );

/*
** yacc on usl_us5 is about to spit out a #include for <stdlib.h>
** which has a prototyped forward reference for abs(int).
** Because we define abs as a macro in compat.h, this forward
** ref gets macroexpanded into "extern int ((int) > 0? (int) : -(int))"
** which is clearly garbage, and won't compile, so stomp on the macro
*/
# ifdef sqs_ptx
# undef abs
# endif

# ifdef su4_us5
# undef abs
# endif

# ifdef ts2_us5
# undef abs
# endif

/*
** Forward declaration of yyerror for rs4_us5
*/
void yyerror(char *);

/* structure is used to maintain the file attribute stack */

typedef struct f_atts
{
	char		*build_dir;	/* build directory */
	char		*build_file;	/* build file name */
	char		*directory;	/* release directory */
	char		*generic_dir;	/* generic release directory */ 
	char		*volume;	/* release media volume */
	char		*executable;	/* last executable referenced */
	PERMISSION	permission;	/* permission string */
	struct f_atts	*next;		/* next record in stack */

} FILE_ATTRIBUTES;

static FILE_ATTRIBUTES *f_atts;	/* top of file attribute stack pointer */

static char where_line[ MAX_MANIFEST_LINE + 1 ];

/*
** The file stack is an array of files which are open at any point in
** time.  Whenever we need to go to a new file -- in other words, when
** we find that a package or a part is defined in an external file -- we
** just push a new file on top of the stack.  The lexical scanner reads
** from the file on top of the stack, and we pop the stack when we hit
** either an end-of-file condition or an "End" statement.  When the
** stack goes empty, that's a real EOF condition.
*/

# define FILE_STACK_SIZE 10	/* maximum nested file references */	

static struct _file 
{
	FILE *fp;
	char name[ 100 ];
	i4 line;

} file[ FILE_STACK_SIZE ];

/*
** Pointer to the topmost element on the file stack.
*/

static i4  filex = -1;

/*
** ip_lex_where() -- set up a text field describing the current location
** in the input file.  (Used for error reporting.)
*/

static char *
ip_lex_where()
{
	STprintf( where_line, "line %d of %s", file[ filex ].line,
		file[ filex ].name );
	return( where_line );
}

static bool parse_error;

/*
** ip_error() -- report a parse error.
*/

static void
ip_error( char *s )
{
	SIfprintf( stderr, "Error detected at %s:\n\n\t%s\n\n",
		ip_lex_where(), s );
	parse_error = TRUE;
}


#line 315 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
typedef union 
{
	char *cptr;
	char datatxt[ MAX_MANIFEST_LINE + 1 ];
	uchar comparison;
} YYSTYPE;
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

#line 362 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
/*
** map token text to identifiers...
*/

GLOBALDEF MAP 
ip_tokenid[] =
{
        "AGGREGATE",    tAGGREGATE,
        "BUILD_DIR",	tBUILD_DIR,
        "BUILD_FILE",	tBUILD_FILE,
        "CAPABILITY",	tCAPABILITY,
        "CHECKSUM",	tCHECKSUM,
        "CUSTOM",	tCUSTOM,
        "DEFINITION",	tDEFINITION,
        "DELETE",	tDELETE,
        "DIRECTORY",	tDIRECTORY,
        "DYNAMIC",	tDYNAMIC,
        "EXECUTABLE",	tEXECUTABLE,
        "FILE",		tFILE,
        "INCLUDE",	tINCLUDE,
        "INVISIBLE",	tINVISIBLE,
        "LINK",		tLINK,
        "PREFER",	tPREFER,
        "NEED",		tNEED,
        "PACKAGE",	tPACKAGE,
        "PART",		tPART,
        "PERMISSION",	tPERMISSION,
        "PRELOAD",	tPRELOAD,
        "RELEASE",	tRELEASE,
        "SCRIPT",	tSCRIPT,
        "SETUP",	tSETUP,
        "SIZE",		tSIZE,
        "STATE",	tSTATE,
        "VERSION",	tVERSION,
        "VISIBLE",	tVISIBLE,
        "VISIBLEPKG",	tVISIBLEPKG,
        "VOLUME",	tVOLUME,
        "{",		tLEFT_BRACE,
        "}",		tRIGHT_BRACE,
	"",		-1
};

/*
** map the text version of the comparison operators onto IDs...
*/

GLOBALDEF MAP
ip_comparison[] =
{
	"==",	COMP_EQ,
	"=",	COMP_EQ,
	"<",	COMP_LT,
	"<=",	COMP_LE,
	">",	COMP_GT,
	">=",	COMP_GE,
	"",	0
};

static LOCATION
*build_location( char *generic_path )
{
	LOCATION *loc = (LOCATION *) ip_alloc_blk( sizeof( LOCATION ) );

	IPCL_LOfroms( PATH, IPCLbuildPath( generic_path, ip_error ), loc );
	return( loc );
}

#define	yyclearin	yychar = -1
#define	yyerrok		yyerrflag = 0
extern int		yychar;
extern short		yyerrflag;
#ifndef YYMAXDEPTH
#define	YYMAXDEPTH	150
#endif
YYSTYPE			yylval, yyval;
#define	YYERRCODE	256

#line 1034 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"


static char input_line[ MAX_MANIFEST_LINE + 1 ];
static char *dataptr = NULL;

/*
** push_file() -- start parsing a different file.  
**
** Open the file, and create an entry on top of the file stack for it.
**
** Returns OK if the file opened OK, and something else if it didn't.
*/

static STATUS
push_file( char *dir, char *fil )
{
	LOCATION loc;
	char locBuf[ MAX_LOC + 1 ];
	FILE *rfile;
	i4 rtn = 0;

	if( filex >= FILE_STACK_SIZE )
		rtn = 999; /* file stack overflow */
	else
	{
		STcopy( dir, locBuf );
		IPCL_LOfroms( PATH, locBuf, &loc );
		LOfstfile( fil, &loc );
		if( OK == (rtn = SIfopen( &loc, ERx("r"), SI_TXT,
			MAX_MANIFEST_LINE, &rfile) ) )
		{
			/* Create a new entry on the stack... */
			file[ ++filex ].fp = rfile;
			STlcopy( fil, file[ filex ].name,
				sizeof( file[ filex ].name ) );
				file[ filex ].line = 0;
			return( OK );
		}
		rtn = FAIL;
	}
	return( rtn );
}

/* ip_lex_pop_file() -- close a file and remove it from the stack. */

static bool
ip_lex_pop_file()
{
    SIclose(file[filex--].fp);
    if( filex < 0 )
	return( FALSE );
    return( TRUE );
}

/* yyerror -- handle parse error discovered inside the yacc grammar. */

void
yyerror(s)
char *s;
{
    SIfprintf( stderr, ERget( E_ST0110_parse_failed ), ip_lex_where(), s );
    parse_error = TRUE;
}


/*
** ip_lex_read_line() -- read a line from the current input file.
**
** This function will keep reading until it finds a "good" line, i.e.
** one which isn't all whitespace and has something on it besides a comment.
*/

# define END_OF_LINE(x) (*x == EOS || !CMcmpcase(x,COMMENT_CHAR))

static char*
ip_lex_read_line()
{
    char *cp;
    do
    {
	if( OK != SIgetrec( input_line, MAX_MANIFEST_LINE, file[filex].fp ) )
	    return NULL;
	file[filex].line++;
	for (cp=input_line; CMwhite(cp); CMnext(cp))
	    ;
    } while( END_OF_LINE( cp ) );

    return cp;
}

/*
** ip_lex_next_line() -- read and parse an input file line.
**
** Reads in a line from the input file, and parses it.  Sets the two
** argument pointers to, respectively, the keyword and the data fields
** on the line, and EOS-terminates both fields.
**
** Returns OK unless it encounters an EOF condition, i.e. end of file
** and no further files on the stack.
*/

STATUS 
ip_lex_next_line(kword, data)
char **kword, **data;
{
    char *cp;
    STATUS rtn;

    while( NULL == (cp = ip_lex_read_line() ) )
    {
	if( ! ip_lex_pop_file() )
	    return tEOF;  /* final EOF reached */
    }

    *data = NULL;  /* Initially assume no data field. */

    /* 
    ** The keyword is the first non-blank thing on the line; that's what
    ** ip_lex_read_line returned, so we know that already.  Now we have
    ** to move ahead to the data field, which follows the whitespace
    ** that follows the keyword. 
    */

    for ( *kword = cp ; !CMwhite(cp) && !END_OF_LINE(cp) ; CMnext(cp) )
	;

    /* If we found some whitespace after the keyword... */

    if (*cp != EOS && CMcmpcase(cp,COMMENT_CHAR))
    {
	*cp = EOS;   /* Null-terminate the keyword */

	/* Find whatever follows the whitespace... */
	for (CMnext(cp) ; CMwhite(cp) ; CMnext(cp))
	    ;

	if (!END_OF_LINE(cp))  /* If there's something there... */
	{
	    /* Find the end-of-line position... */
	    for (*data = cp ; !END_OF_LINE(cp) ; CMnext(cp))
		;

	    /* ...and then back up to the end of the data field. */
	    for (CMprev(cp,cp-2); CMwhite(cp); CMprev(cp,cp-2))
		;

	    /* Null-terminate the data field. */
	    CMnext(cp);
	    *cp = EOS;
	}
    }
    return OK;
}

/*
** comparison() -- parse a comparison operator.
**
** Checks to see if the data field (global "dataptr") begins with a
** comparison operator.  If not, returns 0; if so, returns the "id"
** field of the MAP structure that corresponds to the operator.  If
** a comparison operator is found, advances the "dataptr" pointer to
** point at whatever follows the operator.
**
** Example: this deals with constructs like this:
**
**    Version    >= 6.5/00
*/

static uchar
comparison()
{
    uchar len;
    uchar mlen = 0;  /* Length of longest operator matched so far. */
    uchar comp = 0;
    MAP *compid;

    for (compid = ip_comparison; *compid->token; compid++)
    {
	len = STlength(compid->token);
	if ( len > mlen &&   /* If this potentially matches a longer operator */
	     !STbcompare(compid->token, len, dataptr, 99, FALSE) )
	{
	    mlen = len;
	    comp = compid->id;
	}
	     
    }

    for (dataptr += mlen ; CMwhite(dataptr) ; CMnext(dataptr))
	;

    return comp;
}


/*
** yylex() -- lexical scanner for the yacc parser.
**
** Returns the ID of the next token, and sets up the fields of the "yylval"
** union as required by yacc.  (The yylval fields are defined by the
** "%union" declaration; see a yacc manual for details on how this stuff
** works.)
*/

yylex()
{
	static char *kwordptr = NULL;
	MAP *tkid;
	uchar comp;
		STATUS status;

	/*
	** If kwordptr is NULL, get another line of input
	** until a non-blank line is read or until EOF.
	*/
	while( kwordptr == NULL && (status = ip_lex_next_line(
		&kwordptr, &dataptr )) != tEOF );

	if( status == tEOF )
		return( 0 );

	/* check whether kwordptr points to a keyword */ 
	for( tkid = ip_tokenid; *tkid->token; tkid++ )
	{
		if( !STbcompare( kwordptr, 0, tkid->token, 0, TRUE ) )
   		{
			char *p;

			/*
			** Keyword found.  Before returning, modify
			** kwordptr to point to next possible keyword,
			** if one exists.  Also, modify dataptr to
			** point to remainder of buffer. 
			**
			** Code mostly stolen from ip_lex_next_line().
			*/
   			yylval.cptr = tkid->token;

			if( (kwordptr = dataptr) != NULL )
			{
				/*
				** scan until EOS, whitespace or comment
				** character.
				*/
   				for( p = kwordptr; !CMwhite( p ) &&
					!END_OF_LINE( p ) ; CMnext( p ) );

				if( *p != EOS && CMcmpcase( p, COMMENT_CHAR ) )
    				{
					/* tDATA may follow */
					char *pp;

					*p = EOS; /* terminate keyword */

					/* scan to non-whitespace */
					for( pp = p, CMnext( pp );
						CMwhite( pp ); CMnext( pp ) );

					/* check whether tDATA follows */
					if( !END_OF_LINE( pp ) )
						dataptr = CMnext( p );
					else
						dataptr = NULL;
    				}
				else
				{
					/* tDATA does not follow */
					dataptr = NULL;
				}
			}
	   		return( tkid->id );
		}
	}

	if( *tkid->token == 0 )
	{
		/*
		** kwordptr doesn't point to a keyword,
		** so rejoin kwordptr and dataptr (if not
		** NULL) and break out to dataptr section. 
		*/
		if( dataptr != NULL )
		{
			while( *dataptr != EOS ) CMprev( dataptr, kwordptr ); 
				*dataptr = ' ';
		}
		dataptr = kwordptr;
		kwordptr = NULL;
	}	

	/* assume dataptr is not NULL */
	comp = comparison();
	if( comp )  /* Is it a comparison operator? */
	{
		yylval.comparison = comp;
		dataptr = NULL;
		return( tCOMPARISON );
	}
	else
	{
		/* copy in the data text */
		STcopy( dataptr, yylval.datatxt );
		dataptr = NULL;
		return( tDATA );
	}
}

/*
** Return TRUE if the first package is in the second's reference list.
*/

static bool
pkg_depends_on_pkg( PKGBLK *p1, PKGBLK *p2 )
{
	REFBLK *ref;

	SCAN_LIST( p1->refList, ref )
	{
		if( ref->type == INCLUDE )
			continue;
		if( STbcompare( ref->name, 0, p2->feature, 0, TRUE ) == 0 )
			return( TRUE );
	}
	SCAN_LIST( p2->refList, ref )
	{
		if( ref->type == INCLUDE )
		{
			PKGBLK *ipkg;

			if( (ipkg = ip_find_package( ref->name,
				DISTRIBUTION )) != NULL &&
				pkg_depends_on_pkg( p1, ipkg ) )
			{
				return( TRUE );
			}
		}
	}
	return( FALSE );
}

/*
** ip_parse() -- manifest parser interface
**
** Reads a manifest-format file, and creates an internal list that corresponds
** to the definitions on the file.
**
** Inputs:
**    
**    which		INSTALLATION or DISTRIBUTION
**    pkgList		Pointer to the list we're creating
**    relidptr		Pointer to where to store the pointer to the
**			release ID
**    prodnamep		Pointer to where to store the pointer to the
**			product name
**
** Returns OK if the parse succeeded, FAIL if there were errors.
*/

STATUS
ip_parse( i4  which, LIST *pkgList, char **relidptr, char **prodnamep,
	bool complain )
{
	STATUS st;
	PKGBLK *pkg;
	REFBLK *ref;
	bool flag;
	char *cp;
	LISTELE	*lp1;

	ip_list_init( pkgListP = pkgList );

	*(releaseIdP = relidptr) = ip_stash_string( Version );

	if( NULL != (prodNameP = prodnamep) )
		*prodNameP = NULL;

	/* set manifestDir variable */
	NMgtAt( ERx( "II_MANIFEST_DIR" ), &cp );
	if( cp != NULL && *cp != EOS )
		STlcopy( cp, manifestDir, sizeof(manifestDir)-1 );
	else
	{
		LOCATION loc;
		char locBuf[ MAX_LOC + 1 ];

		NMgtAt( SystemLocationVariable, &cp );
		STlcopy( cp, locBuf, sizeof(locBuf)-20-1 );
		IPCL_LOfroms( PATH, locBuf, &loc );
		LOfaddpath( &loc, SystemLocationSubdirectory, &loc );
		LOfaddpath( &loc, ERx( "manifest" ), &loc );

		if( LOexist( &loc ) == OK )
		    STlcopy( locBuf, manifestDir, sizeof(manifestDir)-1 );
		else
		    STlcopy( ip_install_loc(), manifestDir, sizeof(manifestDir)-1 );
	}

	switch( which )
	{
		case INSTALLATION:
		{
			LOCATION loc;
			char locBuf[ MAX_LOC + 1 ];

			NMgtAt( SystemLocationVariable, &cp );
			STlcopy( cp, locBuf, sizeof(locBuf)-20-1 );
			IPCL_LOfroms( PATH, locBuf, &loc );
			LOfaddpath( &loc, SystemLocationSubdirectory, &loc );
			LOfaddpath( &loc, ERx( "files" ), &loc );
			
			if( OK != (st = push_file( locBuf,
				INSTALLATION_MANIFEST )) )
			{
				if( complain ) 
				{
					IIUGerr( E_ST0140_no_manifest,
						0, 1, INSTALLATION_MANIFEST );
					return( st );
				}
				return( OK );
			}
			break;
		}
	
		case DISTRIBUTION:

			if( OK != (st = push_file( manifestDir,
				releaseManifest )) )
			{
				if( complain )
				{
					IIUGerr( E_ST0140_no_manifest,
						0, 1, releaseManifest );
					return( st );
				}
				return( OK );
			}
			break;
	}

	/* initialize file attributes */
	f_atts = (FILE_ATTRIBUTES *) ip_alloc_blk( sizeof( FILE_ATTRIBUTES ) );
	f_atts->build_dir = NULL;
	f_atts->build_file = NULL;
	f_atts->directory = NULL;
	f_atts->generic_dir = NULL;
	f_atts->volume = NULL;
	f_atts->executable = NULL;
	f_atts->next = NULL;
	f_atts->permission = NULL_PERMISSION;

	parse_error = FALSE;

	yyparse();

	if( parse_error )
		return( PARSE_FAIL );

	/*
	** Report all undefined but referenced packages (then fail).
	*/
	for( lp1 = pkgList->head; lp1 != NULL; lp1 = lp1->cdr )
	{
		LISTELE *lp2;

		/* Allow INVISIBLE packages to reference undefined packages */
		if( !ip_is_visible( (PKGBLK *) lp1->car ) )
			continue;

		/* Scan referenced packages to verify existence */
		for( lp2 = ((PKGBLK *) lp1->car)->refList.head; lp2 != NULL;
			lp2 = lp2->cdr )
		{
			bool found;
			LISTELE *lp3;

			ref = (REFBLK *) lp2->car;

			if( ref->type == PREFER )
				continue;

			found = FALSE;
			for( lp3 = pkgList->head; lp3 != NULL; lp3 = lp3->cdr )
			{
				if( STequal( ref->name,
					((PKGBLK *) lp3->car)->feature ) )
				{
					found = TRUE;
				}
			}
			if( !found )
			{
				SIfprintf( stderr, ERx( "Package \"%s\" is referenced but not defined.\n" ), ref->name );
				if( complain )
					parse_error = TRUE;
			}
		}
	}

	if( parse_error )
		return( PARSE_FAIL );

	/*
	** Perform insertion sort on list of packages so that setup
	** programs are run in the correct order.
	*/
	for( lp1 = pkgList->head; lp1 != NULL; lp1 = lp1->cdr )
	{
		LISTELE *lp2;

		for( lp2 = lp1->cdr; lp2 != NULL; lp2 = lp2->cdr )
		{
			if( pkg_depends_on_pkg( (PKGBLK *) lp1->car,
				(PKGBLK *) lp2->car ) )
			{
				PTR tmp;

				tmp = lp1->car;
				lp1->car = lp2->car;
				lp2->car = tmp;
			}
		}
	}

	/* Compute the apparent sizes of all package. */
	for( lp1 = pkgList->head; lp1 != NULL; lp1 = lp1->cdr )
	{
		pkg = (PKGBLK *) lp1->car;
		pkg->apparent_size =
			compute_apparent_size( pkgList, pkg, 0 );
	}

	return( OK );
}

/*
** compute_apparent_size() -- compute the size of a package.  The size of
** a package is the sum of the sizes of all files contained in the package
** and all INCLUDEd packages.
**
** This function finds all packages INCLUDEd by the one it's called for
** and calls itself recursively to compute the total size.
**
** This number is displayed by the installation utility as a worst-case
** estimate of the size of a package.  When installing multiple packages,
** the actual disk space required will be less if packages share files.
*/

# ifdef DEBUG_COMPUTE_SIZE
# define INDENT(N) { i4  i; for( i = 0; i <= N; i++ ) SIprintf( "\t" ); }
# endif /* DEBUG_COMPUTE_SIZE */

static i4
compute_apparent_size( LIST *pkgList, PKGBLK *pkg, i4  nest )
{
	LISTELE *lp1;
	REFBLK *ref;
	i4 apparent_size = 0;

	if( nest == 0 )
	{
# ifdef DEBUG_COMPUTE_SIZE
INDENT(nest);
SIprintf( "marking all visited.\n" );
# endif /* DEBUG_COMPUTE_SIZE */ 
		/* mark all packages "not visited" */
		for( lp1 = pkgList->head; lp1 != NULL; lp1 = lp1->cdr )
		{
			PKGBLK *spkg = (PKGBLK *) lp1->car;
			spkg->visited = FALSE;
		}
	}
# ifdef DEBUG_COMPUTE_SIZE
else
{
INDENT(nest);
SIprintf( "visiting %s.\n", pkg->feature );
}
# endif /* DEBUG_COMPUTE_SIZE */ 

	/* return 0 if package's actual size already counted */
	if( pkg->visited )
# ifdef DEBUG_COMPUTE_SIZE
{
INDENT(nest);
SIprintf( "\t%s already visited. return( 0 );\n", pkg->feature );
# endif /* DEBUG_COMPUTE_SIZE */ 
		return( 0 );
# ifdef DEBUG_COMPUTE_SIZE
}
# endif /* DEBUG_COMPUTE_SIZE */ 

	pkg->visited = TRUE;

	/* scan all packages in this package's refList */
	for( lp1 = pkg->refList.head; lp1 != NULL; lp1 = lp1->cdr )
	{
		ref = (REFBLK *) lp1->car;
		if( ref->type == INCLUDE )
		{
			LISTELE *lp2;

			for( lp2 = pkgList->head; lp2 != NULL; lp2 = lp2->cdr )
			{
				PKGBLK *spkg = (PKGBLK *) lp2->car;

				if( !STbcompare( ref->name, 0, spkg->feature,
					0, TRUE ) )
				{
					apparent_size +=
						compute_apparent_size( pkgList,
							spkg, nest + 1 );
				}
			}
		}
	}
	apparent_size += pkg->actual_size;

# ifdef DEBUG_COMPUTE_SIZE
INDENT(nest);
SIprintf( "return( %d );\n", apparent_size );
# endif /* DEBUG_COMPUTE_SIZE */ 
	return( apparent_size );
}

/*
** init_reference() -- create and populate a reference block.
**
** Creates a reference block of the type specified by the "type"
** argument, defining a reference to the object named by the "name"
** parameter, and then appends it onto the reference list specified
** by the "list" parameter.
*/

static void
init_reference( LIST *list, char *name, uchar type)
{
	currRef = (REFBLK *) ip_alloc_blk( sizeof( REFBLK ) );
	currRef->type = type;
	currRef->version = NULL;
	currRef->comparison = 0;
	currRef->name = ip_stash_string( name );
	ip_list_append( list, (PTR) currRef );
}

/*
** ci2bit() -- translate a capability name to a bit number.
**
** The capability name is specified by the "ciname" parameter; the
** corresponding bit number is left where "cival" points.  An unrecognized
** capability name causes a syntax error.
**
** If the capability name is preceded by an exclamation point, the
** bit number is biased by the constant NOT_BIAS.  This will have the
** effect of inverting any tests made on the value of the bit.
*/

static void
ci2bit( char *ciname, i4  *cival )
{
    CIBIT *ci;
    i4  bias = 0;
    
    if (!CMcmpcase(ciname, ERx("!")))
    {
	bias = NOT_BIAS;
	CMnext(ciname);
    }

    for (ci=cibits; ci->name; ci++)
    {
        if (!STbcompare(ciname, 0, ci->name, 0, TRUE))
	{
	    *cival = ci->bitno + bias;
	    return;
	}
    }

    ip_error( "Invalid capability name." );
}


/*
** ip_bit2ci() -- convert a bit number, possibly biased, to a capability name.
**
** This routine is the inverse of ci2bit.  It's not really used in parsing;
** it's just located in this file because the conversion table is defined
** here.  The parameter is a capability bit number, possibly biased by the
** constant NOT_BIAS.  The return code is a pointer to a static character
** string containing the corresponding capability name, preceded by an
** exclamation point if the input value was biased.
*/

char *
ip_bit2ci( i4  cibitno )
{
    CIBIT *ci;
    static char name[ MAX_MANIFEST_LINE ];

    if (cibitno >= NOT_BIAS)
    {
	STcopy(ERx("!"), name);
	cibitno -= NOT_BIAS;
    }
    else
    {
	*name = EOS;
    }

    for (ci=cibits; ci->name; ci++)
    {
	if (ci->bitno == cibitno)
	{
	    STcat(name, ci->name);
	    break;
	}
    }

    return name;
}

/*
** newfile() -- create and populate a file block.
**
** Creates a FILBLK structure corresponding to the file named by the
** "file" parameter.  This is done as a separate function, rather
** than directly in the code generated in the yacc pattern, because
** there are two different declarations that create a file block: "file"
** and "link".  The "linkflag" parameter tells us which one we're processing;
** the value of "linkflag" becomes the value of the "link" boolean field
** in the FILBLK that's created.
**
** The created block is linked onto the end of the file list for the
** part that's currently being created.
**
** If in 'patch mode', add dir+name of file and FILBLK address to
** hash index.
*/

static void
newfile( char *file, PERMISSION permission, bool link )
{
	currPkg->nfiles++;
	currFil = (FILBLK *) ip_alloc_blk( sizeof( FILBLK ) );
	ip_list_assign_new( &currFil->name, file );
	currFil->directory = f_atts->directory;
	currFil->generic_dir = f_atts->generic_dir;
	currFil->build_dir = f_atts->build_dir;
	if( link )
	{
		if( f_atts->executable == NULL ) 
		{
			/* no EXECUTABLE is defined to link to */
			ip_error( "LINK requires prior EXECUTABLE specification." );
		}
		currFil->build_file = f_atts->executable;	
	}
	else
	{
		currFil->build_file = f_atts->build_file;
		f_atts->executable = file;	
	}
	f_atts->build_file = NULL;
	currFil->volume = f_atts->volume;
	currFil->exists = TRUE;
	currFil->setup = FALSE;
	currFil->checksum = -1;
	currFil->size = 0;
	currFil->link = link;
	if( f_atts->permission != NULL_PERMISSION )
		currFil->permission_sb = f_atts->permission;
	else
		currFil->permission_sb = permission;
	currFil->custom = FALSE;
	currFil->dynamic = FALSE;
	ip_list_append( &currPrt->filList, (PTR) currFil );
	if ( (link == FALSE) && (pmode == TRUE))
		ip_hash_file_put(currFil->directory, currFil->name, (PTR)currFil);
}
short yyexca[] = {
	-1, 1,
	0, -1,
	-2, 0,
};

#define	YYNPROD		65
#define	YYLAST		200

short yyact[] = {
	    12,    13,     6,     2,    57,    91,    95,    14,    89,    74,
	    76,    61,   103,    71,    87,    77,    84,    85,    56,    16,
	   102,    86,   104,    75,    12,    13,   101,   100,    99,    98,
	    15,    14,    92,    74,    76,    90,    37,    71,    83,    77,
	    27,    88,    61,    16,    93,    62,    67,    75,    29,    30,
	    66,    32,    64,    33,    15,    63,    53,    65,    50,    49,
	    12,    13,    34,    28,    35,    36,    31,    14,    12,    13,
	    48,    47,    46,    45,    44,    14,    43,    23,    41,    16,
	    22,    21,    20,    19,    18,    17,     8,    16,     3,    39,
	    15,    59,    55,    81,    38,    25,   106,    10,    15,    94,
	    79,    82,    72,    80,    70,    60,    58,    54,    40,    26,
	    24,    11,     9,     7,     5,     4,     1,     0,     0,     0,
	     0,     0,    42,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,    51,    52,     0,     0,    96,     0,
	     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,    73,    68,    69,    78,     0,
	     0,     0,     0,     0,     0,   108,     0,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,    97,     0,     0,     0,
	     0,     0,     0,     0,     0,     0,    73,     0,     0,     0,
	     0,     0,     0,     0,     0,   105,     0,     0,    73,   107
};

short yypact[] = {
	  -278, -1000,  -169, -1000,  -285, -1000,  -171,  -192, -1000, -1000,
	 -1000, -1000,  -173,  -174,  -175,  -176,  -177,  -180, -1000, -1000,
	 -1000, -1000, -1000, -1000,  -223,  -200,  -223,  -181,  -183,  -184,
	 -1000, -1000,  -185,  -186,  -187,  -198,  -199, -1000, -1000,  -200,
	  -200,  -201, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000,
	 -1000, -1000, -1000, -1000,  -262,  -220,  -202,  -205, -1000, -1000,
	 -1000,  -207,  -211,  -262,  -262,  -236,  -251, -1000, -1000, -1000,
	 -1000, -1000,  -247, -1000,  -243,  -249,  -252,  -213, -1000,  -272,
	 -1000, -1000,  -247,  -228,  -229, -1000, -1000, -1000,  -230, -1000,
	  -231, -1000,  -237, -1000, -1000,  -245,  -260, -1000, -1000, -1000,
	 -1000, -1000, -1000,  -251, -1000, -1000,  -262, -1000,  -236
};

short yypgo[] = {
	     0,   116,   115,   114,   113,   112,    89,   111,   110,    95,
	    94,   109,   108,   107,    92,   106,    91,    57,   105,   104,
	   103,   102,    93,   101,   100,    99,    96
};

short yyr1[] = {
	     0,     2,     1,     3,     3,     4,     4,     5,     5,     8,
	     7,     9,     9,    11,    11,    11,    11,    11,    11,    11,
	    11,    11,    11,    11,    10,    10,    10,    13,    12,    15,
	    15,    14,    14,    14,    16,    16,    17,    17,    20,    19,
	    19,    19,    21,    21,    21,    21,    21,    21,    21,    22,
	    22,    23,    23,    23,    23,     6,     6,     6,     6,     6,
	    24,    18,    25,    26,    25
};

short yyr2[] = {
	     0,     0,     5,     0,     2,     0,     2,     1,     1,     0,
	     5,     0,     2,     2,     2,     2,     1,     1,     2,     2,
	     2,     2,     2,     1,     0,     2,     2,     0,     5,     2,
	     1,     0,     3,     3,     0,     3,     0,     2,     0,     4,
	     2,     1,     2,     3,     2,     3,     2,     3,     2,     0,
	     2,     2,     2,     1,     1,     2,     2,     2,     2,     2,
	     0,     4,     0,     0,     6
};

short yychk[] = {
	 -4000,    -1,   281,   257,    -2,    -3,   287,    -4,   257,    -5,
	    -6,    -7,   260,   261,   267,   290,   279,   277,   257,   257,
	   257,   257,   257,   257,    -8,    -9,   -11,   263,   286,   271,
	   272,   289,   274,   276,   285,   287,   288,   259,   -10,    -6,
	   -12,   278,    -9,   257,   257,   257,   257,   257,   257,   257,
	   257,   -10,   -10,   257,   -13,   -14,   280,   266,   -15,   -16,
	   -18,   262,   265,   257,   257,   -17,   257,   257,   -14,   -14,
	   -19,   273,   -21,    -6,   269,   283,   270,   275,   -16,   -24,
	   -20,   -22,   -23,   285,   263,   264,   268,   257,   284,   257,
	   284,   257,   284,   257,   -25,   278,   -17,   -22,   257,   257,
	   257,   257,   257,   257,   282,   -16,   -26,   -14,   -17
};

short yydef[] = {
	     0,    -2,     0,     1,     3,     5,     0,     2,     4,     6,
	     7,     8,     0,     0,     0,     0,     0,     0,    55,    56,
	    57,    58,    59,     9,    11,    24,    11,     0,     0,     0,
	    16,    17,     0,     0,     0,     0,     0,    23,    10,    24,
	    24,     0,    12,    13,    14,    15,    18,    19,    20,    21,
	    22,    25,    26,    27,    31,    34,     0,     0,    28,    36,
	    30,     0,     0,    31,    31,    29,    34,    60,    32,    33,
	    37,    38,    49,    41,     0,     0,     0,     0,    35,    62,
	    36,    40,    49,     0,     0,    53,    54,    42,     0,    44,
	     0,    46,     0,    48,    61,     0,     0,    50,    51,    52,
	    43,    45,    47,    34,    39,    63,    31,    36,    64
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
#line 437 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		if( prodNameP != NULL )
			ip_list_assign_string( prodNameP, yypvt[-0].datatxt );
	}
		break;
	case 4:
#line 450 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		ip_list_assign_string( releaseIdP, yypvt[-0].datatxt );
	}
		break;
	case 9:
#line 472 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		char locBuf[ MAX_LOC + 1 ];
		char buf[ 256 ];
		LOCATION loc;
		FILE *fp;
		bool space;

		currPkg = (PKGBLK *) ip_alloc_blk( sizeof( PKGBLK ) );
		currPkg->file = NULL;
		currPkg->feature = ip_stash_string( yypvt[-0].datatxt );
		currPkg->version = ip_stash_string( *releaseIdP );
		currPkg->visible = VISIBLE;
		ip_list_init( &currPkg->prtList );
		ip_list_init( &currPkg->refList );
		currPkg->selected = FALSE;
		currPkg->actual_size = currPkg->apparent_size = 0;
		currPkg->nfiles = 0;
		currPkg->image_cksum = currPkg->image_size = 0;
		currPkg->state = NOT_INSTALLED;
                currPkg->aggregate = FALSE;
# ifdef OMIT
		currPkg->delete = FALSE;
# endif /* OMIT */
		ip_list_append( pkgListP, (PTR) currPkg );
		/*
		** Try to open package description file in release.dat 
		** directory.  Look for file named "(feature).hlp",
		** where "(feature)" is value of "feature" structure
		** member.
		*/
		STcopy( manifestDir, locBuf );
		IPCL_LOfroms( PATH, locBuf, &loc );
		STprintf( buf, ERx( "%s.hlp" ), currPkg->feature ); 
		LOfstfile( buf, &loc );
		if( SIfopen( &loc, ERx( "r" ), SI_TXT, MAX_MANIFEST_LINE,
			&fp ) == OK && SIgetrec( buf, MAX_MANIFEST_LINE,
			fp ) == OK )
		{
			char msg[ 5000 ]; /* should be plenty */
			/* bool described = FALSE, init = FALSE; */ /*b67112*/
			bool described = FALSE;                     /*b67112*/
			i4 len;

			/* STcopy( buf, msg ); */                   /*b67112*/

			(void) STtrmwhite( buf );

			STcopy( buf, msg );                         /*b67112*/

			currPkg->name = ip_stash_string( buf ); 

			/* Use text on remaining lines as description. */
			while( SIgetrec( buf, MAX_MANIFEST_LINE, fp ) == OK )
			{
				described = TRUE;

				if( !batchMode )
				{
					/* 
					** If not in batch mode, remove
					** newlines from text (except for
					** paragraph separators) so frs
					** "message" statement can perform
					** formatting.
					*/

					len = STlength( msg );
					if( ( len + STlength( buf ) ) >=
						sizeof( msg ) )
					{
						break;
					}
/*   b67112  */    /*  un-comment next line */
					(void) STtrmwhite( buf );  /*b67112*/
/*   b67112  */
					if( *buf == EOS )
					{
						/* if( init ) */   /*b67112*/
						/* {          */   /*b67112*/
						/*   init = FALSE;  *b67112*/
						/*   continue;      *b67112*/
						/* }          */   /*b67112*/
						STcopy( ERx( "\n\n" ),
							&msg[ len ] );
						space = FALSE;
					}
					else
					{
						if( space )
							STcat( msg,
							ERx( " " ) );
						space = TRUE;
					}
					/* init = FALSE; */        /*b67112*/
				}
				STcat( msg, buf );
			}
			if( described )
				currPkg->description = ip_stash_string( msg );
			else
				currPkg->description = NULL;
			(void) SIclose( fp );
		}
		else
		{
			/* not description file found */
			currPkg->name = currPkg->feature;
			currPkg->description = NULL;
		}
	}
		break;
	case 13:
#line 595 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		(void) CVan( yypvt[-0].datatxt, &currPkg->image_cksum );
	}
		break;
	case 14:
#line 600 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		(void) CVan(yypvt[-0].datatxt, &currPkg->state);
	}
		break;
	case 15:
#line 605 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		init_reference( &currPkg->refList, yypvt[-0].datatxt, INCLUDE );
	}
		break;
	case 16:
#line 610 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		if( currPkg->visible != VISIBLE &&
			currPkg->visible != VISIBLEPKG &&
			    currPkg->visible != INVISIBLE )
		{
			ip_error( "Package already declared visible." );
		}
		else
			currPkg->visible = INVISIBLE;
	}
		break;
	case 17:
#line 622 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		if( currPkg->visible == INVISIBLE )
			ip_error( "Package already declared INVISIBLE." );
		else
                        currPkg->visible = VISIBLEPKG;

	}
		break;
	case 18:
#line 631 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		init_reference( &currPkg->refList, yypvt[-0].datatxt, PREFER );
	}
		break;
	case 19:
#line 636 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		init_reference( &currPkg->refList, yypvt[-0].datatxt, NEED );
	}
		break;
	case 20:
#line 641 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		(void) CVan( yypvt[-0].datatxt, &currPkg->image_size );
	}
		break;
	case 21:
#line 646 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		currPkg->version = ip_stash_string( yypvt[-0].datatxt );
	}
		break;
	case 22:
#line 651 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		if( currPkg->visible == INVISIBLE )
			ip_error( "Package already declared INVISIBLE." );
		else
			ci2bit( yypvt[-0].datatxt, &currPkg->visible );
	}
		break;
	case 23:
#line 659 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
                if( currPkg->aggregate == TRUE )
                        ip_error( "Package already declared AGGREGATE." );
                else
                        currPkg->aggregate = TRUE;

        }
		break;
	case 27:
#line 681 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		FILE_ATTRIBUTES *tmp;

		currPrt = (PRTBLK *) ip_alloc_blk( sizeof( PRTBLK ) );
		currPrt->file = NULL;
		currPrt->name = ip_stash_string( yypvt[-0].datatxt );
		currPrt->version = ip_stash_string( currPkg->version );
		currPrt->preload = NULL;
		currPrt->delete = NULL;
		currPrt->size = 0;
		ip_list_init( &currPrt->refList );
		ip_list_init( &currPrt->capList );
		ip_list_init( &currPrt->filList );
		ip_list_append( &currPkg->prtList, (PTR) currPrt );
	}
		break;
	case 32:
#line 712 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
	    if (currPrt->preload == NULL)
		currPrt->preload = ip_stash_string( yypvt[-1].datatxt );
	    else
		ip_error( "Pre-load program redeclared." );
	}
		break;
	case 33:
#line 720 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
	    if (currPrt->delete == NULL)
		currPrt->delete = ip_stash_string( yypvt[-1].datatxt );
	    else
		ip_error( "Deletion program redeclared." );
	}
		break;
	case 35:
#line 733 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		i4 *bitp;
		i4 bit;

		ci2bit( yypvt[-1].datatxt, &bit );
		*( bitp = (i4 *) ip_alloc_blk( sizeof( i4  ) ) ) = bit;
		ip_list_append( &currPrt->capList, (PTR) bitp );
	}
		break;
	case 38:
#line 753 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		FILE_ATTRIBUTES *tmp;

		/* create new file attribute context */
		tmp = (FILE_ATTRIBUTES *) ip_alloc_blk(
			sizeof( FILE_ATTRIBUTES ) );
		MEcopy( (PTR) f_atts, sizeof( FILE_ATTRIBUTES ), (PTR) tmp );
		tmp->next = f_atts;
		f_atts = tmp;
	}
		break;
	case 39:
#line 764 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		FILE_ATTRIBUTES *tmp = f_atts;

		/* free (out of scope) attribute context */
		f_atts = f_atts->next;	
		MEfree( (PTR) tmp );
	}
		break;
	case 42:
#line 780 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		i4 len = STlength( yypvt[-0].datatxt ) + 1;
		char *file;
# ifdef VMS
		len += 4;
# endif
		file = (char *) ip_alloc_blk( len ); 
		STcopy( yypvt[-0].datatxt, file );
# ifdef VMS
		STcat( file, ERx( ".EXE" ) );
# endif
		newfile( file,
			IPCLbuildPermission( DEFAULT_EXECUTABLE_PERMISSION,
			ip_error ), FALSE );
	}
		break;
	case 43:
#line 797 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		i4 len = STlength( yypvt[-0].datatxt ) + 1;
		char *file;
# ifdef VMS
		len += 4;
# endif
		file = (char *) ip_alloc_blk( len ); 
		STcopy( yypvt[-0].datatxt, file );
# ifdef VMS
		STcat( file, ERx( ".EXE" ) );
# endif
		newfile( file,
			IPCLbuildPermission( DEFAULT_EXECUTABLE_PERMISSION,
			ip_error ), FALSE );

		currFil->setup = TRUE; 

		init_reference( &currPrt->refList, currFil->name, SETUP );
	}
		break;
	case 44:
#line 818 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		i4 len = STlength( yypvt[-0].datatxt ) + 1;
		char *file;
# ifdef VMS
		len += 4;
# endif
		file = (char *) ip_alloc_blk( len ); 
		STcopy( yypvt[-0].datatxt, file );
# ifdef VMS
		STcat( file, ERx( ".COM" ) );
# endif
		newfile( file,
			IPCLbuildPermission( DEFAULT_SCRIPT_PERMISSION,
			ip_error ), FALSE );
	}
		break;
	case 45:
#line 835 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		i4 len = STlength( yypvt[-0].datatxt ) + 1;
		char *file;
# ifdef VMS
		len += 4;
# endif
		file = (char *) ip_alloc_blk( len ); 
		STcopy( yypvt[-0].datatxt, file );
# ifdef VMS
		STcat( file, ERx( ".COM" ) );
# endif
		newfile( file,
			IPCLbuildPermission( DEFAULT_SCRIPT_PERMISSION,
			ip_error ), FALSE );

		currFil->setup = TRUE; 

		init_reference( &currPrt->refList, currFil->name, SETUP );
	}
		break;
	case 46:
#line 856 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		char *file;
# ifdef VMS
		char *p;
		bool dot_found = FALSE; 

		/* scan file name to determine whether it contains '.' */
		for( p = yypvt[-0].datatxt; *p != EOS; CMnext( p ) )
		{
			if( *p == '.' )
			{
				dot_found = TRUE;
				break;
			}
		}
	
		/* append '.' if necessary */ 
		if( !dot_found )
		{
			file = (char *) ip_alloc_blk( STlength( yypvt[-0].datatxt ) + 2 ); 
			STcopy( yypvt[-0].datatxt, file );
			STcat( file, ERx( "." ) );
		}
		else
			file = yypvt[-0].datatxt;
# else /* VMS */
		file = yypvt[-0].datatxt;
# endif /* VMS */
		newfile( file,
			IPCLbuildPermission( DEFAULT_FILE_PERMISSION,
			ip_error ), FALSE );
	}
		break;
	case 47:
#line 890 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		char *file;
# ifdef VMS
		char *p;
		bool dot_found = FALSE; 

		/* scan file name to determine whether it contains '.' */
		for( p = yypvt[-0].datatxt; *p != EOS; CMnext( p ) )
		{
			if( *p == '.' )
			{
				dot_found = TRUE;
				break;
			}
		}
	
		/* append '.' if necessary */ 
		if( !dot_found )
		{
			file = (char *) ip_alloc_blk( STlength( yypvt[-0].datatxt ) + 2 ); 
			STcopy( yypvt[-0].datatxt, file );
			STcat( file, ERx( "." ) );
		}
		else
			file = yypvt[-0].datatxt;
# else /* VMS */
		file = yypvt[-0].datatxt;
# endif /* VMS */

		newfile( file,
			IPCLbuildPermission( DEFAULT_FILE_PERMISSION,
			ip_error ), FALSE );

		currFil->setup = TRUE; 

		init_reference( &currPrt->refList, currFil->name, SETUP );
	}
		break;
	case 48:
#line 929 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		newfile( yypvt[-0].datatxt,
			IPCLbuildPermission( DEFAULT_EXECUTABLE_PERMISSION,
			ip_error ), TRUE );
	}
		break;
	case 51:
#line 946 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		if( !currFil->link )
		{
			(void) CVan( yypvt[-0].datatxt, &currFil->size );
			currPkg->actual_size += currFil->size;
			currPrt->size += currFil->size;
		}
	}
		break;
	case 52:
#line 956 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		if( !currFil->link )
			(void) CVan( yypvt[-0].datatxt, &currFil->checksum );
	}
		break;
	case 53:
#line 962 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		currFil->custom = TRUE; 
	}
		break;
	case 54:
#line 967 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		currFil->dynamic = TRUE; 
	}
		break;
	case 55:
#line 975 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		f_atts->build_dir = IPCLbuildPath( yypvt[-0].datatxt, ip_error );
	}
		break;
	case 56:
#line 980 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		f_atts->build_file = ip_stash_string( yypvt[-0].datatxt );
	}
		break;
	case 57:
#line 985 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		f_atts->directory = IPCLbuildPath( yypvt[-0].datatxt, ip_error );
		f_atts->generic_dir = ip_stash_string( yypvt[-0].datatxt );
	}
		break;
	case 58:
#line 991 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		f_atts->volume = ip_stash_string( yypvt[-0].datatxt );
	}
		break;
	case 59:
#line 996 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		f_atts->permission = IPCLbuildPermission( yypvt[-0].datatxt, ip_error );
	}
		break;
	case 60:
#line 1004 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{ 
		currPrt->file = ip_stash_string( yypvt[-0].datatxt );
		if( OK != push_file( manifestDir, yypvt[-0].datatxt ) )
		{
			ip_error( "Cannot open part descriptor file:" );
			SIfprintf( stderr, "\t%s\n", yypvt[-0].datatxt );
		}
	}
		break;
	case 62:
#line 1018 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		currPrt->file = NULL;
	}
		break;
	case 63:
#line 1023 "/home/ingres/SANDBOX/Ingres_SVN/server/src/front/st/install/ipparse.yf"
{
		if( STbcompare( yypvt[-1].datatxt, 0, currPrt->name, 0, TRUE ) )
		{
			ip_error( "Part names do not match:" );
			SIfprintf( stderr, "\t[%s]\n\t[%s]\n",
				currPrt->name, yypvt[-1].datatxt );
		}
	}
		break;
		}
		goto yystack;  /* stack new state and value */

	}
