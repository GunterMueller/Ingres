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
# include "genpkg.h"
# include "genpkgdata.h"

/*
**  Name: genrpm.c
**  Purpose: Generates specfiles for Ingres RPM packages
** 
**  History:
**	Sept 2003 (fanch02)
**	    Created.
**	05-Jan-2004 (hanje04)
**	    Added comments and copyright etc.
**	    Fixed up gen_pkg_create_path so it doesn't fail if subdirectories 
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
**	02-Sep-2007 (hanje04)
**	    SIR 118420
**	    To add support for building DEB package control files, move
**	    GLOBALDEFs/REFs, strutures and function prototypes into separate
**	    header files. Move the following funtions to genpkg.c as they
**	    are used for creation of RPM & DEB packages:
**
**		gen_rpm_specs_add()
**		gen_rpm_specs_features_add()
**		gen_rpm_subs_add()
**		gen_pkg_subs_get()
**		gen_pkg_subs_set()
**		gen_rpm_cmclean()
**		gen_pkg_strsubs()
**		gen_pkg_config_readline()
**		gen_pkg_config_read_sub()
**		gen_pkg_config_read_esub()
**		gen_pkg_config_read_spec()
**		gen_pkg_config_read()
**		gen_pkg_create_path().
**
**	    Finally, recode gen_rpm_template to generate the SPEC files 
**	    from the new layout of the "template files". Each SPEC file is
**	    now generated from the a package unique header, generic and
**	    specific script files and ingbuild package info.
*/

static STATUS
gen_rpm_template( SPECS *specs );
static char*
gen_rpm_spec_name(char *buff, char *directory, SPECS *specs);
static STATUS 
gen_rpm_file_list( SPECS *specs, FILE *fileSpecs );

static char	*rpmTemplateFiles[] = {
				# define CORETMPLT 0
				"ingres",
				# define DBMSTMPLT 1
				"ingres-dbms",
				# define NETTMPLT 2
				"ingres-net",
				# define GENTMPLT 3
				"ingres-generic",
				# define DOCTMPLT 4
				"ingres-documentation",
				# define LICTMPLT 5
				"ingres-LICENSE",
				};
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
    char buff[READ_BUF + 1];
    char *cp = NULL;
    char subuf[50];
    i4 template = GENTMPLT ;
    i4 sulen;
    FILE *fileTemplate, *fileSpecs;
    LOCATION loc;
    STATUS status;

    /* set the spec instance name */
    STprintf(buff, ERx("%s%s"), *specs->name ?
		ERx("-") : ERx(""), *specs->name ?
		    specs->name : ERx(""));
    if (!gen_pkg_subs_set(ERx("%rpm_specname%"), buff))
        return(FAIL);

    /* define the package name */
    if ( *specs->name != EOS )
        if (!gen_pkg_subs_set("%pkg_name%", specs->name ))
            return(FAIL);

    /* ...and the setup scripts */
    subuf[0] = EOS;
    sulen = gen_pkg_su_files( specs, subuf, sizeof( subuf ) );
    if ( sulen >= 0 )
        gen_pkg_subs_set( "%pkg_sulist%", subuf );
    else
    {
        SIfprintf( stderr, "Failed to set setup script list for %s package\n",
                    *specs->name ? specs->name : "core" );
        return( FAIL );
    }

    /* ING_BUILD must be defined, buildrel.c depends on this */
    NMgtAt(ERx("ING_BUILD"), &cp);

    /* open spec output file */
    gen_rpm_spec_name(buff, pkgBuildroot, specs);
    LOfroms(PATH & FILENAME, buff, &loc);

    if (SIfopen(&loc, ERx("w"), SI_TXT, 0, &fileSpecs))
    {
        SIfprintf(stderr,
	 "could not write '%s', correct config file or II_RPM_BUILDROOT\n",
	 buff);
        return(FAIL);
    }

    /*
    ** Need to do a multi phase pass. First process the header and the
    ** package decriptions. Then add main body of the spec file,
    ** finally the add the file list
    */
    /* open spec header file */
    STprintf(buff, ERx("%s/%s/%s%s%s%s"),
			cp,
            ERx("files/rpmhdrs"),
            "ingres",
            *specs->name != EOS ? ERx("-") : ERx(""),
            specs->name,
            ERx(".pkghdr"));
    LOfroms(PATH & FILENAME, buff, &loc);

    if (SIfopen(&loc, ERx("r"), SI_TXT, 0, &fileTemplate))
    {
        SIfprintf(stderr,
	 "template '%s' not found, correct config file or missing template\n",
	 buff);
        SIclose(fileSpecs);
        return(FAIL);
    }

    while (1)
    {
        buff[READ_BUF] = EOS;
        if (OK != SIgetrec(buff, sizeof(buff), fileTemplate))
            break;
        
        STtrmwhite(buff);

        /* perform substitutions */
        status = gen_pkg_strsubs(buff, sizeof(buff));
	if ( status == IP_SKIP_LINE )
	    continue;
        else if (status) {
            SIclose(fileTemplate);
            SIclose(fileSpecs);
            return(status);
        }
        
        /* write to spec file */
        SIfprintf(fileSpecs, ERx("%s\n"), buff);
    }

    /* next add package descriptions from the package hlp files */
    SIflush( fileSpecs );
    if ( gen_pkg_hlp_file( specs, fileSpecs, RPM_HLP_FMT ) != OK )
    {
        SIfprintf( stderr,
                "Failed to write help info to Spec file\n" );
        SIclose( fileSpecs );
        return( FAIL );
    }

    SIclose(fileTemplate);
   
    /* now do the main body of the spec file */
    /* get the appropriate template file */
    if ( *specs->name == EOS )
	template = CORETMPLT;
    else if ( ! STcompare( specs->name, "dbms" ) )
	template = DBMSTMPLT;
    else if ( ! STcompare( specs->name, "net" ) )
	template = NETTMPLT;
    else if ( ! STcompare( specs->name, "documentation" ) )
	template = DOCTMPLT;
    else if ( ! STcompare( specs->name, "LICENSE" ) )
	template = LICTMPLT;
    else
	template = GENTMPLT;

    /* open spec file template */
    STprintf(buff, "%s/%s/%s.template",
			cp,
            "files/rpmtemplates",
            rpmTemplateFiles[template] );
    LOfroms(PATH & FILENAME, buff, &loc);

    if (SIfopen(&loc, ERx("r"), SI_TXT, 0, &fileTemplate))
    {
        SIfprintf(stderr,
	 "template '%s' not found, correct config file or missing template\n",
	  buff);
        SIclose(fileSpecs);
        return(FAIL);
    }

    while (1)
    {
        buff[READ_BUF] = EOS;
        if (OK != SIgetrec(buff, sizeof(buff), fileTemplate))
            break;
        
        STtrmwhite(buff);

        /* perform substitutions */
        status = gen_pkg_strsubs(buff, sizeof(buff));
	if ( status == IP_SKIP_LINE )
	    continue;
        else if (status) {
            SIclose(fileTemplate);
            SIclose(fileSpecs);
            return(status);
        }
        
        /* write to spec file */
        SIfprintf(fileSpecs, ERx("%s\n"), buff);
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
    SIZE_TYPE packageFeatureLength;
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
    STATUS status;
	char buff[MAX_LOC + 1];
	char *cp = NULL;
    SPECS *specs;

	/* II_MANIFEST_DIR must be defined, buildrel depends on it */
	NMgtAt( ERx("II_MANIFEST_DIR"), &cp );
	STprintf(buff, ERx("%s/rpmconfig"), cp);

    /* defaults */
    if (!gen_pkg_subs_set(ERx("%rpm_basename%"), ERx("Ingres"))
		|| !gen_pkg_subs_set(ERx("%rpm_version%"), ERx("1.0.0000"))
		|| !gen_pkg_subs_set(ERx("%rpm_buildroot%"), pkgBuildroot))
        return(FAIL);

    /* read the configuration file */
    status = gen_pkg_config_read(buff);
    if (status != OK)
        return(status);

    /* release is optional */
    subsRelease = gen_pkg_subs_get(ERx("%rpm_release%"));

    /* check for basename */
    if (NULL == (subsBaseName = gen_pkg_subs_get(ERx("%rpm_basename%")))) {
        SIfprintf(stderr, ERx("rpm_basename not declared in spec generation config file\n"));
        return(FAIL);
    }

    /* check for version */
    if (NULL == (subsVersion = gen_pkg_subs_get(ERx("%rpm_version%")))) {
        SIfprintf(stderr, ERx("rpm_version not declared in spec configuration config file\n"));
        return(FAIL);
    }

    /* check for prefix */
    if (NULL == (subsPrefix = gen_pkg_subs_get(ERx("%rpm_prefix%")))) {
        SIfprintf(stderr, ERx("rpm_prefix not declared in spec configuration config file\n"));
        return(FAIL);
    }

    /* check for Embedded prefix */
    if (NULL == (subsEmbPrefix = gen_pkg_subs_get(ERx("%rpm_emb_prefix%")))) {
        SIfprintf(stderr, ERx("rpm_emb_prefix not declared in spec configuration config file\n"));
        return(FAIL);
    }


    /* check MDB info */
    if (NULL == (subsMDBPrefix = gen_pkg_subs_get(ERx("%mdb_prefix%")))) {
        SIfprintf(stderr, ERx("mdb_prefix not declared in spec configuration config file\n"));
        return(FAIL);
    }

    if (NULL == (subsMDBVersion = gen_pkg_subs_get(ERx("%mdb_version%")))) {
        SIfprintf(stderr, ERx("mdb_version not declared in spec configuration config file\n"));
        return(FAIL);
    }
    if (NULL == (subsMDBRelease = gen_pkg_subs_get(ERx("%mdb_release%")))) {
        SIfprintf(stderr, ERx("mdb_release not declared in spec configuration config file\n"));
        return(FAIL);
    }

    /* check for Doc prefix */
    if (NULL == (subsDocPrefix = gen_pkg_subs_get(ERx("%rpm_doc_prefix%")))) {
        SIfprintf(stderr, ERx("rpm_doc_prefix not declared in spec configuration config file\n"));
        return(FAIL);
    }

	/* create output directory */
	gen_rpm_spec_name(buff, pkgBuildroot, NULL);
	if (gen_pkg_create_path(buff, ERx("Failed to create spec file output directory:\n")) != OK)
        return(FAIL);

	/* generate spec files from templates */
    for(specs = specsFirst; specs; specs = specs->next) {
        status = gen_rpm_template(specs);
        if (status)
            return(status);
    }

    return(OK);
}
