/*
** Copyright (c) 2004, 2006 Ingres Corporation. All rights reserved.
*/
# include <compat.h>
# include <si.h>
# include <st.h>
# include <lo.h>
# include <ex.h>
# include <er.h>
# include <gc.h>
# include <cm.h>
# include <nm.h>
# include <gl.h>
# include <sl.h>
# include <iicommon.h>
# include <fe.h>
# include <ug.h>
# include <ui.h>
# include <te.h>
# include <gv.h>
# include <iplist.h>
# include <erst.h>
# include <uigdata.h>
# include <stdprmpt.h>
# include <sys/stat.h>
# include <ip.h>
# include <ipcl.h>

/*
**  Name: genrpm.c
**  Purpose: Generates specfiles for Ingres RPM packages
** 
**  History:
**	Sept 2003 (fanch02)
**	    Created.
**	05-Jan-2004 (hanje04)
**	    Added comments and copyright etc.
**	    Fixed up gen_rpm_create_path so it doesn't fail if subdirectories 
**	    are not writable.
**	31-Jan-2004 (hanje04)
**	    Change name of confg file to rpmconfig and move it to 
**	    $II_MANIFEST_DIR.
**	    Also move the spec templates to $ING_BUILD/files/rpmtemplates
**	11-Mar-2004 (hanje04)
**	    For reasons unknown the emb-c part ends up in the package list
**	    for Embeded SQL 3 times. It seems to be included esqlc, esqlcc 
**	    and esqlada. So as not to break the manifest for other platforms, 
**	    add check in gen_rpm_file_list to make sure the contents
**	    is only written once to the spec file. Otherwise we get duplicate
**	    files errors under RPM 3.0.6
**	26-Mar-2004 (hanje04)
**	    Don't attempt to generate filelist for EULA SPEC file. It only
**	    includes the files in the template.
**	01-Apr-2004 (hanje04)
**	    Don't generate filelist for documentation package either
**	20-May-2004 (hanje04)
**	    Increase size of buf in gen_rpm_template() to 1024 + 1 to reduce
**	    the risk of breaking up long lines in the spec file templates.
**	26-Apr-2004 (bonro01)
**	    Install as a user other than 'ingres'.
**	28-Jun-2004 (hanje04)
**	    Add rpm_emb_prefix (and subsEmbPrefix) which is defined 
**	    (in rpmconfig) as $II_RPM_EMB_PREFIX and use it instead of 
** 	    rpm_prefix for the embedded (EI) base package.
**	15-Jul-2004 (hanje04)
**	    Make sure we also use rpm_emb_prefix when inserting the spec 
**	    file in the file list for the embedded package
**	25-Oct-2004 (hanje04)
**	    Add support for creating ca-ingres-mdb package which has it's own
**	    Prefix, Version and Release strings.
**	20-Jan-2004 (hanje04)
**	    BUG 113780
**	    Add %verify tag to files to control which file attributes are 
**	    verified by rpm -V. Most files need size, md5sum, modify time
**	    and permissions checking. We only check the permissions on
**	    iimerge as it is relinked at install time.
**	05-Apr-2005 (hanje04)
**	    Don't need CATOSL spec file.
**	09-Jan-2006 (hanje04)
**	    SIR 115662
**	    Hardcode the name of the spec file templates. No matter what
**	    the name of the product, template names will always start
**	    with 'ingres'.
**	30-Jan-2006 (hanje04)
**	    SIR 115688
**	    To allow saveset to be built with 3 different types of 
**	    licensing, allow %rpm_license% to be over-ridden in buildrel
**	    by speciying -L flag. 
**	    If licence_type is non-NULL use it instead of the value 
**	    defined in rpmconfig when replacing the %rpm_license% token.
**	01-Feb-2006 (hanje04)
**	    SIR 115688
**	    Replace -L with -l to avoid problems on VMS.
**	03-Feb-2006 (hanje04)
**	    SIR 115688
**	    Don't put any files in the LICENSE SPEC file.
**	04-Feb-2006 (hanje04)
**	    BUG 115698
**	    Make buildrel generate the correct file list for
**	    the doc package.
*/

/**** external references ****/
GLOBALREF char *rpmBuildroot;
GLOBALREF LIST distPkgList;
GLOBALREF char *license_type;

/**** defines ****/
# define SETUID_MASK    04000

/**** type definitions ****/
typedef struct _FEATURES
{
    struct _FEATURES    *next;          /* next feature */
    char                *feature;       /* feature name */
} FEATURES;

typedef struct _SPECS {
    struct _SPECS       *next;          /* next spec file definition */
    char                *name;          /* spec package name */
    FEATURES            *featureFirst;  /* list of features the spec file includes */
    FEATURES            *featureLast;   /* last item in the list */
} SPECS;

typedef struct _SUBS {
    struct _SUBS        *next;          /* next substitution */
    char                *sub;           /* string to substitute */
    char                *value;         /* value to substitute with */
    int                 diff;           /* strlen(value) - strlen(sub) */
    int                 valueLength;    /* strlen(value) */
    int                 subLength;      /* strlen(sub); */
} SUBS;


/**** signatures ****/
STATUS gen_rpm_spec();
static SPECS* gen_rpm_specs_add(char* name);
static FEATURES* gen_rpm_specs_features_add(SPECS* specs, char* feature);
static SUBS* gen_rpm_subs_add(char* sub, char* value);
static SUBS* gen_rpm_subs_set(char* sub, char* value);
static SUBS* gen_rpm_subs_get(char* sub);
static STATUS gen_rpm_strsubs(char* str, int maxCount);
static char* gen_rpm_cmclean (char* str);
static char* gen_rpm_spec_name(char *buff, char *directory, SPECS* specs);
static SPECS* gen_rpm_config_read_spec( char* line );
static STATUS gen_rpm_file_list( SPECS *specs, FILE *fileSpecs );
static STATUS gen_rpm_config_read( char* filenameConfig );
static STATUS gen_rpm_config_readline (FILE* file, char* line, int maxCount);
static STATUS gen_rpm_config_read_sub( char* line );
static STATUS gen_rpm_config_read_esub( char* line );
static STATUS gen_rpm_template( SPECS* specs );
static STATUS gen_rpm_create_path(char *path, char *errmsg);


/**** Data definitions ****/
/**** spec and substitution list maintenance ****/
static SPECS *specsFirst = 0;
static SPECS *specsLast = 0;
static SUBS *subsFirst = 0;
static SUBS *subsLast = 0;

/**** required config file substitutions ****/
static SUBS *subsBaseName;
static SUBS *subsVersion;
static SUBS *subsRelease;
static SUBS *subsPrefix;
static SUBS *subsEmbPrefix;
static SUBS *subsMDBPrefix;
static SUBS *subsMDBVersion;
static SUBS *subsMDBRelease;
static SUBS *subsDocPrefix;


/*
** Add a spec file definition.
**
** char *name(in) - unique part of the spec file name
**
** Returns:
**  SPECS* of the newly created definition
**  NULL if creation failed
**
*/
static SPECS*
gen_rpm_specs_add(char *name)
{
    int nameLength = STlength(name);
    SPECS *specs = (SPECS *) MEreqmem(0, sizeof(SPECS) + nameLength + 1, TRUE, NULL);
    if (specs == NULL) {
        SIfprintf(stderr, ERx("spec file segment allocation failed\n"));
        return(NULL);
    }

    specs->name = &((char *)specs)[sizeof(SPECS)];
    STmove(name, EOS, nameLength + 1, specs->name);

    if (specsLast) {
        specsLast->next = specs;
    } else {
        specsFirst = specs;
    }
    specsLast = specs;

    return specs;
}

/*
** Add a feature to a spec file definition.
**
** SPECS *specs(in) - spec definition to add feature to
** char *feature(in) - feature to add
**
** Returns:
**  FEATURES* of the newly added feature definition
**  NULL if creation failed
**
*/
static FEATURES*
gen_rpm_specs_features_add(SPECS *specs, char *feature)
{
    int featureLength = STlength(feature);
    FEATURES *nfeature = (FEATURES *) MEreqmem(0, sizeof(FEATURES) + featureLength + 1, TRUE, NULL);
    if (nfeature == NULL) {
        SIfprintf(stderr, ERx("spec file nfeature segment allocation failed\n"));
        return(NULL);
    }

    nfeature->feature = &((char *)nfeature)[sizeof(FEATURES)];
    STmove(feature, EOS, featureLength + 1, nfeature->feature);
    
    if (specs->featureLast) {
        specs->featureLast->next = nfeature;
    } else {
        specs->featureFirst = nfeature;
    }
    specs->featureLast = nfeature;

    return nfeature;
}

/*
** Add a substitution variable to the substitution list.
**
** char *sub(in) - substitution name/key
** char *value(in) - value to substitute in place of the name/key
**
** Returns:
**  SUBS* of the newly added substitution definition
**  NULL if creation failed
**
*/
static SUBS*
gen_rpm_subs_add(char *sub, char *value)
{
    i4 subLength = STlength(sub);
    i4 valueLength = -1;
    SUBS *subs = (SUBS *) MEreqmem(0, sizeof(SUBS) + subLength + 1, TRUE, NULL);

    if (subs == NULL) {
        SIfprintf(stderr, ERx("substitution spec file segment allocation failed\n"));
        return(NULL);
    }

    /* copy values into new subs structure */
    subs->sub = &((char *)subs)[sizeof(SUBS)];
    STmove(sub, EOS, subLength + 1, subs->sub);

    /*
    ** Check for Licensing case where value in rpmconfig may have been
    ** over ridden by -l flag
    */
    if ( license_type && STcompare(sub, "%rpm_license%") == 0)
	valueLength = STlength(license_type);
    else
        valueLength = STlength(value);

    /* allocate value separately since we may modify it */
    if (!(subs->value = MEreqmem(0, valueLength + 1, TRUE, NULL))) {
        SIfprintf(stderr, ERx("substitution spec value allocation failed\n"));
        MEfree((PTR) subs);
        return(NULL);
    }

    /* Again check for special case */
    if ( license_type && STcompare(sub, "%rpm_license%") == 0)
	STmove(license_type, EOS, valueLength + 1, subs->value);
    else
	STmove(value, EOS, valueLength + 1, subs->value);

    subs->valueLength = valueLength;
    subs->subLength = subLength;
    subs->diff = valueLength - subLength;

    if (subsLast) {
        subsLast->next = subs;
    } else {
        subsFirst = subs;
    }
    subsLast = subs;

    return subs;
}

/*
** Retrieve a substite structure given a substitute name.
**
** char *sub(in) - substitute name
**
** Returns:
**  SUBS* of the requested definition
**  NULL if not found
**
*/
static SUBS*
gen_rpm_subs_get(char *sub)
{
    SUBS *at;
    for (at = subsFirst; at; at = at->next)
        if (STequal(at->sub, sub))
            return at;
    return(NULL);
}

/*
** Set the value of a substitution given a substitute name
** and value.  Adds a substitution to the list if it does
** not exist.
**
** char *sub(in) - substitute name
** char *value(in) - value of the substitution
**
** Returns:
**  SUBS* of the specified definition
**  NULL if creation failed
**
*/
static SUBS*
gen_rpm_subs_set(char *sub, char *value)
{
    int valueLength = STlength(value);
    SUBS *subs = gen_rpm_subs_get(sub);
    char *new_value;
    if (!subs)
        return gen_rpm_subs_add(sub, value);
    
    /* allocate space for new value */
    new_value = MEreqmem(0, valueLength + 1, TRUE, NULL);
    if (!new_value) {
        SIfprintf(stderr, ERx("substitution new spec value allocation failed\n"));
        return 0;
    }

    STmove(value, EOS, valueLength + 1, new_value);
    MEfree((PTR) subs->value);
    subs->value = new_value;
    subs->valueLength = valueLength;
    subs->diff = subs->valueLength - STlength(subs->sub);
    
    return(subs);
}

/*
** Strip leading and trailing whitespace from a string.
** Remove repeated whitespace and replace with a single space.
**
** char *str(in/out) - string to clean
**
** Returns:
**  char* of the cleaned string
**
*/
static char *gen_rpm_cmclean (char* str)
{
    int state = 0;
    char* to = str;
    char* at = str;

    while (CMwhite(at))
        CMnext(at);

    while (*at != EOS) {
        switch (state) {
            case 0:
                /* normal text */
                if (CMwhite(at)) {
                    /* switch to whitespace mode */
                    state = 1;
                    break;
                }
                CMcpychar(at, to);
                CMnext(to);
                break;
            case 1:
                /* whitespace mode */
                if (!CMwhite(at)) {
                    state = 0;
                    *to = ' ';
                    CMnext(to);
                    CMcpychar(at, to);
                    CMnext(to);
                }
                break;
            default:
                /* Huh? Unreachable */
                *to++ = ' ';
                *to = EOS;
                return(str);
        }
        CMnext(at);
    }
    *to = EOS;
    return(str);
}

/*
** Process spec file template line substitutions
**
** char *str(in/out) - buffer to replace substitutions in
** int maxCount(in) - maximum size of buffer
**
** Returns:
**  OK if substitutions were completed
**  FAIL if an error was encountered, str will still be
**     properly terminated
**
*/
static STATUS
gen_rpm_strsubs( char* str, int maxCount )
{
    SUBS *subs;
    char *match, *loc, *at;
    int currentLength = STlength(str);
    int lp;

    for(subs = subsFirst; subs; subs = subs->next) {
        match = STindex(at = str, subs->sub, 0);
        while (match != NULL) {
			if (!STxcompare(match, subs->subLength, subs->sub, subs->subLength, FALSE, FALSE)) {
				/* matched first character and sub, replace if possible */
				if (currentLength + subs->diff > maxCount) {
					SIfprintf(stderr, ERx("substitution would exceed maximum line length:\n   '%s' <- '%s'\n   %s\n"),
										 subs->sub,
										 subs->value, str);
					return(FAIL);
				}
				if (subs->diff > 0) {
					loc = match + subs->diff;
					/* FIXME wrong overlap for ST/MEcopy */
					memmove(loc, match, 1 + (currentLength - (match - str)));
				} else if (subs->diff < 0) {
					loc = match - subs->diff;
					MEcopy(loc, 1 + (currentLength - (loc - str)), match);
				}
				MEcopy(subs->value, subs->valueLength, match);
				currentLength += subs->diff;
				match = STindex(at, subs->sub, 0);
			}
			else {
				/* matched first character but not the sub, advance */
				match = STindex(++at, subs->sub, 0);
			}
        }
    }

    return (OK);
}


/*
** Read a config file line and toss comments and blanks.
** Only "real lines" should be returned.
** Strip leading, trailing blanks, repeated spaces/tabs.
**
** file(in) - filename to read from (current position)
** line(out) - buffer to store line in
** maxCount(in) - maximum characters to read into buffer
**
** Returns:
**  OK if a line was successfully read
**  FAIL on error
**
** Note: SIgetrec() does not provide the capability of
** detecting long lines.
**
*/
static STATUS
gen_rpm_config_readline (FILE* file, char* line, int maxCount)
{
    line[maxCount-1] = EOS;
	while (OK == SIgetrec(line, maxCount, file)) {
        if (line[maxCount-1] != EOS) {
            line[maxCount-1] = EOS;
            SIfprintf(stderr, ERx("warning: maximum line length encountered\n"));
        }
        gen_rpm_cmclean(line);
		if (line[0] != EOS && line[0] != '#')
			return(OK);
	}
	return(FAIL);
}

/*
** Process a substitution line and add the value to the
** substitution list.  If the value is not present an
** empty string is provided.
**
** The line is in the format:
**  <sub name>[ <sub value>[ <ignored text]]
**
** char *line(in/out) - text line
**
** Returns:
**  OK on success
**  other values on failure
**
*/
static STATUS
gen_rpm_config_read_sub( char *line )
{
    char *nextWord = line;
    while (*nextWord != EOS && !CMwhite(nextWord))
        CMnext(nextWord);
    if (*nextWord) {
        *nextWord = EOS;
        CMnext(nextWord);
    }
    return !gen_rpm_subs_set(line, nextWord);
}

/*
** Process an environment variable substitution line.
** Used to set a substitution to an environment variable value.
** If the variable does not exist, it is treated as a normal
** substitution.
**
** The line is in the format:
**  <sub name>[ <env variable name>[ <ignored text]]
**
** char *line(in/out) - text line
**
** Returns:
**  OK on success
**  other values on failure
**
*/
static STATUS
gen_rpm_config_read_esub( char *line )
{
    char *nextWord = line, *esub = NULL;
    while (*nextWord != EOS && !CMwhite(nextWord))
        CMnext(nextWord);
    if (*nextWord != EOS) {
		*nextWord = EOS;
        CMnext(nextWord);
    }
	
	/* get the environment variable */
	NMgtAt(nextWord, &esub);
	if (esub == NULL) {
		STprintf(ERx("warning: '%s' <- '%s' environment substitution failed\n"), line, nextWord);
	}

    return !gen_rpm_subs_set(line, esub == NULL ? nextWord : esub);
}

/*
** Process a config file spec entry line.  If the unique part
** of the spec name is not specified the spec entry will not
** contain a unique part.  Use only up to one of these per
** spec file definition.
**
** The line is in the format:
**  [<spec name>]
**
** char *line(in/out) - text line
**
** Returns:
**  OK on success
**  other values on failure
**
*/
static SPECS*
gen_rpm_config_read_spec( char *line )
{
    while (CMwhite(line))
        CMnext(line);
    return gen_rpm_specs_add(line);
}

/*
** Process RPM generation config file.  This file contains
** all of the spec file definitions and substitutions.
**
** char *filenameConfig(in) - file name of the config file
**
** Returns:
**  OK on success
**  other values on failure
**
*/
static STATUS
gen_rpm_config_read( char *filenameConfig )
{
	FILE *file;
    char line[MAX_LOC + 1];
    int rval;
    LOCATION loc;
    FEATURES *ncontain = 0;
    SPECS *cspecs = 0;
    SUBS *nsubs = 0;
	int state = 0; /* initial */

    STcopy(filenameConfig, line);
    LOfroms(PATH & FILENAME, line, &loc);

    if (SIfopen(&loc, ERx("r"), SI_TXT, 0, &file)) {
        SIfprintf(stderr, ERx("failed to open config file '%s' for reading\n"), filenameConfig);
        return(FAIL);
    }

    rval = !gen_rpm_config_readline (file, line, sizeof(line));
	while (rval)
	{
		switch (state) {
            case 0: /* init */
                if (!STbcompare(ERx("$sub "), 5, line, 5, 0)) {
                    /* add substitution */
                    if (gen_rpm_config_read_sub(&line[5])) {
                        SIclose(file);
                        return(FAIL);
                    }
                } else if (!STbcompare(ERx("$spec"), 5, line, 5, 0)) {
                    /* process spec file section */
                    state = 1; /* switch to spec:features */
                    if (!(cspecs = gen_rpm_config_read_spec(&line[5]))) {
                        SIclose(file);
                        return(FAIL);
                    }
                } else if (!STbcompare(ERx("$esub "), 6, line, 6, 0)) {
					/* add environment substitution */
					if (gen_rpm_config_read_esub(&line[6])) {
						SIclose(file);
						return(FAIL);
					}
				}
                break;
            case 1: /* in spec:features */
                if (STbcompare(ERx("$feature "), 9, line, 9, 0)) {
                    state = 0;
                    cspecs = 0;
                    continue;
                }
                gen_rpm_specs_features_add(cspecs, &line[9]);
                break;
            default: /* Huh? Unreachable */
                SIclose(file);
                return(FAIL);
		}
        rval = !gen_rpm_config_readline (file, line, sizeof(line));
	}

    SIclose(file);
    return(0);
}


/*
** Process spec file template given a spec file definition.
** Create an output spec file and process all substitutions.
**
** SPECS *specs(in) - spec definition to create
**
** Returns:
**  OK on success
**  other values on failure
**
*/
static STATUS
gen_rpm_template( SPECS *specs )
{
    char buff[1024 + 1];
	char *cp = NULL;
    FILE *fileTemplate, *fileSpecs;
    LOCATION loc;
    STATUS status;

    /* set the spec instance name */
    STprintf(buff, ERx("%s%s"), *specs->name ? ERx("-") : ERx(""), *specs->name ? specs->name : ERx(""));
    if (!gen_rpm_subs_set(ERx("%rpm_specname%"), buff))
        return(FAIL);

    /* open spec template file */
	/* ING_BUILD must be defined, buildrel.c depends on this */
	NMgtAt(ERx("ING_BUILD"), &cp);
    STprintf(buff, ERx("%s/%s/%s%s%s%s"),
			cp,
            ERx("files/rpmtemplates"),
            "ingres",
            *specs->name != EOS ? ERx("-") : ERx(""),
            specs->name,
            ERx(".template"));
    LOfroms(PATH & FILENAME, buff, &loc);

    if (SIfopen(&loc, ERx("r"), SI_TXT, 0, &fileTemplate)) {
        SIfprintf(stderr, ERx("template '%s' not found, correct config file or missing template\n"), buff);
        return(FAIL);
    }

    /* open spec output file */
    gen_rpm_spec_name(buff, rpmBuildroot, specs);
    LOfroms(PATH & FILENAME, buff, &loc);

    if (SIfopen(&loc, ERx("w"), SI_TXT, 0, &fileSpecs)) {
        SIfprintf(stderr, ERx("could not write '%s', correct config file or II_RPM_BUILDROOT\n"), buff);
        SIclose(fileTemplate);
        return(FAIL);
    }

	while (1) {
        buff[sizeof(buff)-1] = EOS;
        if (OK != SIgetrec(buff, sizeof(buff), fileTemplate))
            break;
        
        /* check for error condition */
        /* can't do this with SIgetrec
        if (buff[sizeof(buff)-1] != EOS) {
            buff[sizeof(buff)-1] = EOS;
            SIfprintf(stderr, ERx("warning: maximum line length encountered reading from template\n"));
        }*/
        STtrmwhite(buff);

        /* perform substitutions */
        status = gen_rpm_strsubs(buff, sizeof(buff));
        if (status) {
            SIclose(fileTemplate);
            SIclose(fileSpecs);
            return(status);
        }
        
        /* write to spec file */
        SIfprintf(fileSpecs, ERx("%s\n"), buff);
        /* error checking not supported - grr
        if (SIfprintf(fileSpecs, ERx("%s\n"), buff) == EOF) {
            SIfprintf(stderr, ERx("could not write to spec file\n");
            SIclose(fileTemplate);
            SIclose(fileSpecs);
            return(FAIL);
        }*/
	}

/* Don't add any more files to EULA spec */
    if ( STcompare( specs->name, "LICENSE" ) )
	gen_rpm_file_list ( specs, fileSpecs );

    SIclose(fileTemplate);
    SIclose(fileSpecs);

    return(0);
}


/*
** Generate spec completely specified spec file name or and/or
** name rooted at specified directory.
**
** char *buff(out) - buffer to place complete path and file name
** char *directory(in) - directory
** SPECS *specs(in) - spec definition to use to generate file name
**   NULL if only directory is desired
**
** Returns:
**  char* of buffer containing complete path and file name
**
*/
static char*
gen_rpm_spec_name(char *buff, char *directory, SPECS *specs)
{
    /* MDB package is a bit different so it's separate */
    if ( specs && ( ! STcompare( specs->name, "mdb" ) ) )
	 STprintf(buff, ERx("%s%s/ingres/files/rpmspec%s%s%s%s%s%s%s%s%s"),
		directory,
		subsMDBPrefix->value,
		"/",
		subsBaseName->value,
		"-",
		subsMDBVersion->value,
		*specs->name ? "-" : "",
		specs->name,
		"-",
		subsMDBRelease->value,
		".spec");
    else
    /* Everything else */
        STprintf(buff, ERx("%s%s/ingres/files/rpmspec%s%s%s%s%s%s%s%s%s"),
		directory,
		specs && ( ! STcompare( specs->name, "EI" ) ) ?
                           	subsEmbPrefix->value :
                           	subsPrefix->value,
		specs ? ERx("/") : ERx(""),
		specs ? subsBaseName->value : ERx(""),
		specs ? ERx("-") : ERx(""),
		specs ? subsVersion->value : ERx(""),
		specs && *specs->name ? ERx("-") : ERx(""),
		/*
		** If package is LICENSE use the license_type instead
		** if spec->name, if that's not set default to "gpl"
		** specs ?  ( ! STcompare( specs->name, "LICENSE" )) ? 
		**	license_type ? license_type : "gpl" : specs->name
		**	 : ERx(""),
		**
		** Don't actually need the above code at the moment but
		** may do in the future.
		*/
		specs ? specs->name : ERx(""),
		specs && subsRelease && *subsRelease->value ? ERx("-") : ERx(""),
		specs && subsRelease && *subsRelease->value ? subsRelease->value : ERx(""),
		specs ? ERx(".spec") : ERx(""));
    return buff;
}

/*
** Create a complete path.  Optionally displays an error
** message if the creation fails.
**
** char *path(in) - path to create
** char *errmsg(in) - displays this message on failure
**
** Returns:
**  OK if successful
**  other values on failure
**
*/
static STATUS
gen_rpm_create_path(char *path, char *errmsg)
{
	char *pptr = path;
	char delim;
	STATUS status;

	/* advance over any leading slashes */
	if (*pptr != EOS && *pptr == '/')
		CMnext(pptr);

	while (*pptr != EOS) {
		while (*pptr != EOS && *pptr != '/')
			CMnext(pptr);
		delim = *pptr;
		*pptr = '\0';
		status = OK;
		if( IPCLcreateDir( path ) != OK )
			status = FAIL;	 /* if we fail to create the 
				 		directory, log it */
			
		*pptr = delim;
		CMnext(pptr);
	}

	if (errmsg != NULL && status) /* If we completely fail then report it */
	{
	    SIfprintf(stderr, errmsg);
	    SIfprintf(stderr, ERx("\t%s\n\n"), path);
	}

	return(status);
}


/*
** Generate list of files and attributes from MANIFEST packages and
** features.
**
** SPECS *specs(in) - spec definition to use to generate file name
** FILE *fileSpecs(in) - handle of spec file to write file list to
**
** Returns:
**  OK on success
**  other values on failure
**
*/
static STATUS 
gen_rpm_file_list( SPECS *specs, FILE *fileSpecs )
{
    FEATURES *features;
    PKGBLK *package;
    PRTBLK *part;
    FILBLK *file;
    LOCATION loc;
    int packageFeatureLength;
    char buff[ MAX_LOC + 1 ];
    char *filename;
    bool embcdone = FALSE;

    SIfprintf(fileSpecs, ERx("%%attr (%o,root,root) %%verify (%s) %s\n"),
			  0644,
			  ERx("mode md5 size mtime"),
			  gen_rpm_spec_name(buff, ERx(""), specs));

    SCAN_LIST(distPkgList, package) {
        packageFeatureLength = STlength(package->feature);

        for (features = specs->featureFirst; features; features = features->next) {
            if (!STxcompare(package->feature, packageFeatureLength, features->feature, packageFeatureLength, TRUE, FALSE)) {

                SCAN_LIST(package->prtList, part) {
		    if ( ! STcompare( part->name, "emb-c" ) )
		    {
			if ( embcdone )
			   continue;
			else
			   embcdone=TRUE;
		    }

                    SCAN_LIST(part->filList, file) {
                        /* compose file name */
                        STcopy( file->build_dir, buff );
                        LOfroms( PATH, buff, &loc );
                        LOfstfile( file->build_file != NULL ? file->build_file : file->name, &loc );
                        LOtos( &loc, &filename );
			/* location info is different for doc package */
			if ( ! STcompare( part->name, "documentation" ) )
			{
			 /*
			 ** as we're not using the first part of the dir name
			 ** in the doc location, make sure we preserve
			 ** everything after ingres/files/english
			 */
			 char namebuf[50];  /* filename and any needed dirs */
			 char *lastdir = STrindex( file->directory, "/", 0 );
			 
			 /* do we need to append a dir to the filename */
			 if ( STcompare( ++lastdir, "english" ) )
			     STprintf( namebuf, "%s/%s", lastdir, file->name );
			 else
			     STcopy( file->name, namebuf );

                         SIfprintf(fileSpecs, 
			  ERx("%%attr (%o,root,root) %%verify (%s) %s/%s-%s/%s\n"),
			      file->permission_sb,
			      "mode md5 size mtime", 
			      subsDocPrefix->value,
			  subsBaseName->value,
			  subsVersion->value,
			  namebuf);
			}
			else
			{
                         SIfprintf(fileSpecs, 
			  ERx("%%attr (%o,root,root) %%verify (%s) %s/%s/%s\n"),
			      file->permission_sb,
			     ( file->permission_sb & SETUID_MASK ) ?
				ERx("mode" ) :
				ERx("mode md5 size mtime"), 
			  STcompare( specs->name, "documentation" ) ? 
				subsPrefix->value : 
				subsDocPrefix->value,
			  file->directory,
			  file->name);
			}
                    }
                }
            }
        }
    }

    return(0);
}

/*
** Public entry point to genrpm.c
**
** Generates spec files from spec file templates.
**
** Required parameters are global and ensured by buildrel.c
**
** Returns:
**  OK on successful completion
**  FAIL or other values on failure
*/
STATUS
gen_rpm_spec()
{
	/* generate RPM spec files according to the config file */
    int status;
	char buff[MAX_LOC + 1];
	char *cp = NULL;
    SPECS *specs;

	/* II_MANIFEST_DIR must be defined, buildrel depends on it */
	NMgtAt( ERx("II_MANIFEST_DIR"), &cp );
	STprintf(buff, ERx("%s/rpmconfig"), cp);

    /* defaults */
    if (!gen_rpm_subs_set(ERx("%rpm_basename%"), ERx("Ingres"))
		|| !gen_rpm_subs_set(ERx("%rpm_version%"), ERx("1.0.0000"))
		|| !gen_rpm_subs_set(ERx("%rpm_buildroot%"), rpmBuildroot))
        return(FAIL);

    /* read the configuration file */
    status = gen_rpm_config_read(buff);
    if (status != OK)
        return(status);

    /* release is optional */
    subsRelease = gen_rpm_subs_get(ERx("%rpm_release%"));

    /* check for basename */
    if (NULL == (subsBaseName = gen_rpm_subs_get(ERx("%rpm_basename%")))) {
        SIfprintf(stderr, ERx("rpm_basename not declared in spec generation config file\n"));
        return(FAIL);
    }

    /* check for version */
    if (NULL == (subsVersion = gen_rpm_subs_get(ERx("%rpm_version%")))) {
        SIfprintf(stderr, ERx("rpm_version not declared in spec configuration config file\n"));
        return(FAIL);
    }

    /* check for prefix */
    if (NULL == (subsPrefix = gen_rpm_subs_get(ERx("%rpm_prefix%")))) {
        SIfprintf(stderr, ERx("rpm_prefix not declared in spec configuration config file\n"));
        return(FAIL);
    }

    /* check for Embedded prefix */
    if (NULL == (subsEmbPrefix = gen_rpm_subs_get(ERx("%rpm_emb_prefix%")))) {
        SIfprintf(stderr, ERx("rpm_emb_prefix not declared in spec configuration config file\n"));
        return(FAIL);
    }


    /* check MDB info */
    if (NULL == (subsMDBPrefix = gen_rpm_subs_get(ERx("%mdb_prefix%")))) {
        SIfprintf(stderr, ERx("mdb_prefix not declared in spec configuration config file\n"));
        return(FAIL);
    }

    if (NULL == (subsMDBVersion = gen_rpm_subs_get(ERx("%mdb_version%")))) {
        SIfprintf(stderr, ERx("mdb_version not declared in spec configuration config file\n"));
        return(FAIL);
    }
    if (NULL == (subsMDBRelease = gen_rpm_subs_get(ERx("%mdb_release%")))) {
        SIfprintf(stderr, ERx("mdb_release not declared in spec configuration config file\n"));
        return(FAIL);
    }

    /* check for Doc prefix */
    if (NULL == (subsDocPrefix = gen_rpm_subs_get(ERx("%rpm_doc_prefix%")))) {
        SIfprintf(stderr, ERx("rpm_doc_prefix not declared in spec configuration config file\n"));
        return(FAIL);
    }

	/* create output directory */
	gen_rpm_spec_name(buff, rpmBuildroot, NULL);
	if (gen_rpm_create_path(buff, ERx("Failed to create spec file output directory:\n")) != OK)
        return(FAIL);

	/* generate spec files from templates */
    for(specs = specsFirst; specs; specs = specs->next) {
        status = gen_rpm_template(specs);
        if (status)
            return(status);
    }

    return(OK);
}
