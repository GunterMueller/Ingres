/*
**Copyright (c) 2004 Ingres Corporation
*/
 
/*	@(#)getopt.c	1.4	*/
/*	3.0 SID #	1.1	*/
/*   Changes:
**   Nov-27-2000 (hweho01)
**      1) Added <generic.h> so platform specific code could be defined.
**      2) To resolve the compiler error on axp_osf platform, changed
**         the prototype of getopt().
**   Dec-21-2000 (toumi01)
**	Remove defines of strchr and strrchr now included by generic.h.
**	Otherwise, some platforms (e.g. int_lnx) will get compile errors.
*/

#include	<stdio.h>

#include        <generic.h>

main(argc, argv)
int argc;
char **argv;
{
	extern	int optind;
	extern	char *optarg;
	register int	c;
	int	errflg = 0;
	char	tmpstr[4];
	char	outstr[5120];
	char	*goarg;

	if(argc < 2) {
		fputs("usage: getopt legal-args $*\n", stderr);
		exit(2);
	}

	goarg = argv[1];
	argv[1] = argv[0];
	argv++;
	argc--;

	outstr[0] = '\0';
	while((c=getopt(argc, argv, goarg)) != EOF) {
		if(c=='?') {
			errflg++;
			continue;
		}

		tmpstr[0] = '-';
		tmpstr[1] = c;
		tmpstr[2] = ' ';
		tmpstr[3] = '\0';

		strcat(outstr, tmpstr);

		if(*(strchr(goarg, c)+1) == ':') {
			strcat(outstr, optarg);
			strcat(outstr, " ");
		}
	}

	if(errflg) {
		exit(2);
	}

	strcat(outstr, "-- ");
	while(optind < argc) {
		strcat(outstr, argv[optind++]);
		strcat(outstr, " ");
	}

	printf("%s\n", outstr);
	exit(0);
}

# ifndef SYS5

/*
 * this is a public domain version of getopt(3).
 * bugs, fixes to:
 *		Keith Bostic
 *			ARPA: keith@seismo
 *			UUCP: seismo!keith
 */

/*
 * get option letter from argument vector
 */
int	opterr = 1,		/* useless, never set or used */
	optind = 1,		/* index into parent argv vector */
	optopt;			/* character checked for validity */
char	*optarg;		/* argument associated with option */

#define BADCH	(int)'?'
#define EMSG	""
#define tell(s)	fputs(*nargv,stderr);fputs(s,stderr); \
		fputc(optopt,stderr);fputc('\n',stderr);return(BADCH);

#if defined(axp_osf)
getopt(nargc,nargv,ostr)
int     nargc;
char* const*   nargv;
const char*    ostr;
#else
getopt(nargc,nargv,ostr)
int	nargc;
char	**nargv,
	*ostr;
#endif  /* axp_osf */
{
	static char	*place = EMSG;	/* option letter processing */
	register char	*oli;		/* option letter list strchr */
	char	*strchr();

	if(!*place) {			/* update scanning pointer */
		if(optind >= nargc || *(place = nargv[optind]) != '-' || !*++place) return(EOF);
		if (*place == '-') {	/* found "--" */
			++optind;
			return(EOF);
		}
	}				/* option letter okay? */
	if ((optopt = (int)*place++) == (int)':' || !(oli = strchr(ostr,optopt))) {
		if(!*place) ++optind;
		tell(": illegal option -- ");
	}
	if (*++oli != ':') {		/* don't need argument */
		optarg = NULL;
		if (!*place) ++optind;
	}
	else {				/* need an argument */
		if (*place) optarg = place;	/* no white space */
		else if (nargc <= ++optind) {	/* no arg */
			place = EMSG;
			tell(": option requires an argument -- ");
		}
	 	else optarg = nargv[optind];	/* white space */
		place = EMSG;
		++optind;
	}
	return(optopt);			/* dump back option letter */
}

# endif
