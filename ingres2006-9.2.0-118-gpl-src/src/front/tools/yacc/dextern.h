/*
** Copyright (c) 2004 Ingres Corporation
*/
/*
 * dextern.h
 * 
 * History:
 *	10-apr-1990 (kimman/boba)
 *	    Increated MEMSIZE, ACTSIZE, NPROD for yacc-ing csq...
 *	03-aug-1992 (kathryn)
 *	24-aug-1992 (lan)
 *        Increased WSETSIZE and NNONTERM from 1000 to 1500
 *	    Incremented LSETSIZE from 1000 to 1500
 *	11/92 (Mike S)
 *	    Upped NSTATES and TEMPSIZE to 4000 
 *	23-nov-1992 (kathryn)
 *	    Upped CNAMESZ from 15000 to 20000.
 *	10-mar-93 (swm)
 *	    Increased MEMSIZE to 24000 for yacc-ing csq.
 *	12-sep-1993 (valerier)
 *	    Added correct file defines for WindowsNT.
 *	24-aug-93 (brett)
 *		Add WIN32 definitions.
 *	22-oct-1993 (mgw)
 *		Get rid of elif's as per Coding Standard.
 *	25-oct-93 (brett)
 *		Reformatted the chained ifdefs into a case like structure.
 *		Removed the WIN32 definitions, wgl_wnt is used instead.
 *	06-apr-94 (leighb)
 *		Make sure that if XHUGE is what is wanted, the HUGE macros
 *		don't get used because HUGE is also defined.
 *      15-jan-1996 (toumi01; from 1.1 axp_osf port)
 *		Increased MEMSIZE to 26000 for yacc-ing csq.
 *	25-Feb-1999 (hanch04)
 *	    Define int to be long so that pointers can fit in a long
 *	14-Jul-2000 (hanje04)
 *	    Made definition of 'int to be long' conditional on 64bit.
 *	27-Jul-2000 (hanje04)
 *	    Corrected #ifdef for change above
 *	11-oct-2000 (somsa01)
 *	    The previous change did not comment properly.
 *      11-oct-2000 (somsa01)
 *	    Now that files including this header file include "compat.h",
 *	    we do not need to include the system header files here anymore.
 *      Nov-17-2000 (hweho01)
 *          Added bzarch.h include file for the definition of platform 
 *          string.
 *	29-Apr-2004 (schka24)
 *	    Increase LSETSIZE, etc.
 *	30-Nov-2004 (lakvi01)
 *      Bumped MEMSIZE from 26000 to 36000 for yacc-ing of csq
 *		to work on AMD64. (Windows Server 2003)
 */

#include "bzarch.h"

#include "files.h"

	/* 
	** Change int to long for 64 bit port
	*/
#ifdef LP64
#define int long
#endif /* LP64 */

	/*  MANIFEST CONSTANT DEFINITIONS */

	/* base of nonterminal internal numbers */
#define	NTBASE	010000

	/* internal codes for error and accept actions */
#define	ERRCODE		8190
#define	ACCEPTCODE	8191

	/*
	 * relationships which must hold:
	 *
	 *	TBITSET ints must hold NTERMS+1 bits...
	 *	WSETSIZE >= NNONTERM
	 *	LSETSIZE >= NNONTERM
	 *	TEMPSIZE >= NTERMS + NNONTERMs + 1
	 *      NOMORE <= -NNONTERM  (automatic) -- NOMORE defined in y4.c
	 *	TEMPSIZE >= NSTATES
	 */

	/* sizes and limits */

/* It's this one, folks.  See files.h */
#ifdef XHUGE			/* added for yacc Equel grammer */
#    define	ACTSIZE		20000		/* was 12000 */
#    define	MEMSIZE		36000		/* was 24000, was 12000 */
#    define	NSTATES		4000		/* was 750 */
#    define	NTERMS		  600		/* was 127 */
#    define	NPROD		 2500		/* was 600 */
#    define	NNONTERM	 3000		/* was 300 -- was 1000 */
#    define	TEMPSIZE	 5000		/* was 1200 */
#    define	CNAMSZ		20000		/* was 15000 - was 5000 */
#    define	LSETSIZE	 3000		/* was 600 -- was 1000 */
#    define	WSETSIZE	 3000		/* was 350 -- was 1000 */
#endif

#ifndef XHUGE
#ifdef HUGE
#    define	ACTSIZE		12000
#    define	MEMSIZE		12000
#    define	NSTATES		750
#    define	NTERMS		127
#    define	NPROD		600
#    define	NNONTERM	300
#    define	TEMPSIZE	1200
#    define	CNAMSZ		5000
#    define	LSETSIZE	600
#    define	WSETSIZE	350
#endif

#ifndef HUGE
#ifdef MEDIUM
#    define	ACTSIZE		4000
#    define	MEMSIZE		5200
#    define	NSTATES		600
#    define	NTERMS		127
#    define	NPROD		400
#    define	NNONTERM	200
#    define TEMPSIZE	800
#    define	CNAMSZ		4000
#    define	LSETSIZE	450
#    define	WSETSIZE	250
#endif	/* MEDIUM */
#endif	/* HUGE */
#endif	/* XHUGE */

#define	NAMESIZE	50
#define	NTYPES		63

#ifdef WORD32
#    define	TBITSET		((32+NTERMS)/32)
	/* bit packing macros (may be machine dependent) */
#    define	BIT(a,i)	((a)[(i)>>5] & (1<<((i)&037)))
#    define	SETBIT(a,i)	((a)[(i)>>5] |= (1<<((i)&037)))
	/* number of words needed to hold n+1 bits */
#    define	NWORDS(n)	(((n)+32)/32)
#else	/* WORD32 */
#    define	TBITSET		((16+NTERMS)/16)
	/* bit packing macros (may be machine dependent) */
#    define	BIT(a,i)	((a)[(i)>>4] & (1<<((i)&017)))
#    define	SETBIT(a,i)	((a)[(i)>>4] |= (1<<((i)&017)))
	/* number of words needed to hold n+1 bits */
#    define	NWORDS(n)	(((n)+16)/16)
#endif	/* WORD32 */

	/* associativities */
#define	NOASC		0			/* no assoc. */
#define	LASC		1			/* left assoc. */
#define	RASC		2			/* right assoc. */
#define	BASC		3			/* binary assoc. */

	/* flags for state generation */
#define	DONE		0
#define	MUSTDO		1
#define	MUSTLOOKAHEAD	2

	/* flags for a rule having an action, and being reduced */
#define	ACTFLAG		04
#define	REDFLAG		010

	/* output parser flags */
#define	YYFLAG1		(-1000)

	/* macros for getting associativity and precedence levels */
#define	ASSOC(i)	((i)&03)
#define	PLEVEL(i)	(((i)>>4)&077)
#define TYPE(i) 	((i>>10)&077)

	/* macros for setting associativity and precedence levels */
#define	SETASC(i,j)	i |= j
#define	SETPLEV(i,j)	i |= (j<<4)
#define	SETTYPE(i,j)	i |= (j<<10)

	/* looping macros */
#define	TLOOP(i)	for(i=1;i<=ntokens;++i)
#define	NTLOOP(i)	for(i=0;i<=nnonter;++i)
#define	PLOOP(s,i)	for(i=s;i<nprod;++i)
#define	SLOOP(i)	for(i=0;i<nstate;++i)
#define	WSBUMP(x)	++x
#define	WSLOOP(s,j)	for(j=s;j<cwp;++j)
#define	ITMLOOP(i,p,q)	q=pstate[i+1];for(p=pstate[i];p<q;++p)
#define	SETLOOP(i)	for(i=0;i<tbitset;++i)

	/* I/O descriptors */
extern FILE * finput;			/* input file */
extern FILE * faction;			/* file for saving actions */
extern FILE *fdefine;			/* file for # defines */
extern FILE * ftable;			/* y.tab.c file */
extern FILE * ftemp;			/* tempfile to pass 2 */
extern FILE * foutput;			/* y.output file */

	/* structure declarations */
struct looksets {
	int lset[TBITSET];
};

struct item {
	int *pitem;
	struct looksets *look;
};

struct toksymb {
	char *name;
	int value;
};

struct ntsymb {
	char *name;
	int tvalue;
};

struct wset {
	int *pitem;
	int flag;
	struct looksets ws;
};

	/* token information */
extern int ntokens;	/* number of tokens */
extern struct toksymb tokset[];
extern int toklev[];	/* vector with the precedence of the terminals */

	/* nonterminal information */
extern int nnonter;	/* the number of nonterminals */
extern struct ntsymb nontrst[];

	/* grammar rule information */
extern int nprod;	/* number of productions */
extern int *prdptr[];	/* pointers to descriptions of productions */
extern int levprd[];	/* contains production levels to break conflicts */

	/* state information */
extern int nstate;		/* number of states */
extern struct item *pstate[];	/* pointers to the descriptions of the states */
extern int tystate[];	/* contains type information about the states */
extern int defact[];	/* the default action of the state */
extern int tstates[];	/* the states deriving each token */
extern int ntstates[];	/* the states deriving each nonterminal */
extern int mstates[];	/* the continuation of the chains begun in tstates */
			/*	and ntstates */

	/* lookahead set information */
extern struct looksets lkst[];
extern int nolook;	/* flag to turn off lookahead computations */

	/* working set information */
extern struct wset wsets[];
extern struct wset *cwp;

	/* storage for productions */
extern int mem0[];
extern int *mem;

	/* storage for action table */
extern int amem[];	/* action table storage */
extern int *memp;	/* next free action table position */
extern int indgo[];	/* index to the stored goto table */

	/* temporary vector, indexable by states, terms, or ntokens */
extern int temp1[];
extern int lineno;	/* current line number */

	/* statistics collection variables */
extern int zzgoent;
extern int zzgobest;
extern int zzacent;
extern int zzexcp;
extern int zzclose;
extern int zzrrconf;
extern int zzsrconf;

extern char *prog;		/* our program name */

	/* define functions with strange types... */
extern char *cstash();
extern struct looksets *flset();
extern char *symnam();
extern char *writem();

	/* default settings for a number of macros */

	/* name of yacc tempfiles */
#ifndef TEMPNAME
# ifdef vms
#    define	TEMPNAME	"pg.tmp"
# else
#    define	TEMPNAME	"yacc.tmp"
# endif /* vms */
#endif

#ifndef ACTNAME
# ifdef vms
#	define	ACTNAME "pg.act"
# endif /* vms */
# if OS2
#	define	ACTNAME "yacc.act"
# endif /* OS2 */
# if wgl_wnt
#	define	ACTNAME "yacc.act"
# endif /* wgl_wnt */

# ifndef ACTNAME
#	define	ACTNAME "yacc.acts"
# endif /* !ACTNAME */
#endif /* !ACTNAME */

	/* output file name */
#ifndef OFILE
# ifdef vms
#	define	OFILE "pgtab.c"
# endif /* vms */
# if OS2
#	define	OFILE "ytab.c"
# endif /* OS2 */
# if wgl_wnt
#	define	OFILE "y_tab.c"
# endif /* wgl_wnt */

# ifndef OFILE
#	define	OFILE "y.tab.c"
# endif /* !OFILE */
#endif /* !OFILE */

	/* user output file name */
#ifndef FILEU
# ifdef vms
#	define	FILEU "pgtab.out"
# endif /* vms */
# if OS2
#	define	FILEU "y.out"
# endif /* OS2 */
# if wgl_wnt
#	define	FILEU "yacc.out"
# endif /* wgl_wnt */

# ifndef FILEU
#	define	FILEU "y.output"
# endif /* !FILEU */
#endif /* !FILEU */

	/* output file for # defines */
#ifndef FILED
# ifdef vms
#	define	FILED "pgtab.h"
# endif /* vms */
# if OS2
#	define	FILED "ytab.h"
# endif /* OS2 */
# if wgl_wnt
#	define	FILED "y_tab.h"
# endif /* wgl_wnt */

# ifndef FILED
#	define	FILED "y.tab.h"
# endif /* !FILED */
#endif /* !FILED */

	/* command to clobber tempfiles after use */
#ifndef ZAPFILE
# ifdef vms
#    define ZAPFILE(x)	delete(x)
# else
#    define ZAPFILE(x)	unlink(x)
# endif /* vms */
#endif

