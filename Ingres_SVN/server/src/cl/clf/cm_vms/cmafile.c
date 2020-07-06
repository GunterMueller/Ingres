/*
** Copyright (c) 1989, 2004 Ingres Corporation
*/

# include <compat.h>
# include <gl.h>
# include <me.h>
# include <cm.h>
# include <nm.h>
# include <st.h>
# include <lo.h>
# include <si.h>

# include <locale.h>
# ifdef xCL_006_FCNTL_H_EXISTS
# include       <fcntl.h>
# endif /* xCL_006_FCNTL_H_EXISTS */

#include <fab.h>
#include <rab.h>
#include <rmsdef.h>
#include <starlet.h>

/*
** SETCLERR is a UNIX mechanism for reporting internal CL errors and
** system errors.  The VMS variant of the CL_ERRDESC only has a single
** error which is assumed to map to a system message.  To avoid having
** to use #ifdef's in more places, we will define the SETCLERR macro
** to simply set the error number to 0 - all the instances in here are
** intended as internal error reporting on UNIX, and have no analog
** on VMS.  For VMS specific instances we will directly set the system
** error number.
*/
#define SETCLERR(X,M,N)	X->error = 0


/**
**
**  Name: CMAFILE.C - Routines to read and write attribute files
**
**  Description:
**	The file contains routines to read and write character attribute
**	description file.  This does not use SI because that is not legal
**	for the DB server.
**
**		CMget_charset_name - Get installation charset name
**		CMset_attr - set attribute table for CM macros.
**		CMset_charset - set attribute table for installation charset
**		CMset_locale - set locale for the current process
**		CMwrite_attr - write attribute file
**
**  History:
**	9/89 (bobm)    Created.
**      25-sep-90 (bonobo)
**	    Added #include <systypes.h>, <clsecret.h> and #ifdef 
**	    xCL_006_FCNTL_H_EXISTS for portability
**      04-feb-1991 (wolf)
**	    Change VMS definition of *CM_AttrTab and *CM_CaseTab to make the
**	    DBMS group's CL image more static.
**      1-mar-91 (stevet)
**	    Fixed RMS problem with CMset_attr.
**	4/91 (Mike S)
**	   Moved global declarations to cmglobs.c .
**	7/91 (bobm)
**	   change search path in attrfile().
**      16-jul-93 (ed)
**	    added gl.h
**      11-aug-93 (ed)
**          added missing includes
**	19-jul-2000 (kinte01)
**	    Correct prototype definitions by adding missing includes and
**	    forward function references
**	01-dec-2000	(kinte01)
**		Bug 103393 - removed nat, longnat, u_nat, & u_longnat
**		from VMS CL as the use is no longer allowed
**	14-may-2002 (somsa01)
**	    Added CMset_locale().
**	23-jan-20004 (stephenb)
**	    work for generic double-byte build,
**	    extend ENTRY to include double-byte attributes
**	23-Jan-2004 (gupsh01)
**	    Added CM_getcharset to obtain the characterset for the
**	    platform.
**	14-Jun-2004 (schka24)
**	    Changes for safer string handling.
**	14-jan-2005 (abbjo03)
**	    Add CMgetDefCS.
**	11-apr-2007 (gupsh01)
**	    Added CM_isUTF8.
**/

/*
** the default, compiled in translation.
*/
GLOBALREF u_i2 CM_DefAttrTab[];
GLOBALREF char CM_DefCaseTab[];
GLOBALREF char CM_BytesForUTF8[];
GLOBALREF i4 CMdefcs;

static char *attrfile();
static char Double_byte = 0;
static STATUS init_frab();

static CMATTR Readattr;

/* Pointers to the current tables */
GLOBALREF u_i2 *CM_AttrTab;
GLOBALREF char *CM_CaseTab;
GLOBALREF char CM_isUTF8;
GLOBALREF char *CM_UTF8Bytes;

/*
** Local definitions
*/
typedef struct tagENTRY
{
    char value[128];
    char key[128];
    char isdouble; /* is character set double-byte */
}ENTRY;

static ENTRY charsets[]=
{
    {"874", "THAI", 1},
    {"874", "WTHAI", 1},
    {"932", "SHIFTJIS", 1},
    {"932", "KANJIEUC", 1},
    {"936", "CHINESES", 1},
    {"936", "CHINESET", 1},
    {"936", "CSGBK", 1},
    {"936", "CSGB2312", 1},
    {"949", "KOREAN", 1},
    {"950", "CHTBIG5", 1},
    {"950", "CHTEUC", 1},
    {"950", "CHTHP", 1},
    {"852",  "SLAV852", 0},
    {"1200", "UCS2", 0},
    {"1200", "UCS4", 0},
    {"1200", "UTF7", 0},
    {"1200", "UTF8", 0},
    {"1250", "IBMPC850", 0},
    {"1250", "WIN1250", 0},
    {"1251", "IBMPC866", 0},
    {"1251", "ALT", 0},
    {"1251", "CW", 0},
    {"1252", "WIN1252", 0},
    {"1253", "GREEK", 0},
    {"1253", "ELOT437", 0},
    {"1254", "PC857", 0},
    {"1255", "HEBREW", 0},
    {"1255", "WHEBREW", 0},
    {"1255", "PCHEBREW", 0},
    {"1256", "ARABIC", 0},
    {"1256", "WARABIC", 0},
    {"1256", "DOSASMO", 0},
    {"1257", "WIN1252", 0},
    {"28591", "ISO88591", 0},
    {"437", "IBMPC437", 0},
    {"", "HPROMAN8", 0},
    {"28605", "IS885915", 0},
    {"28592", "ISO88592", 0},
    {"28595", "ISO88595", 0},
    {"28599", "ISO88599", 0},
    {"", "DECMULTI", 0},
    {"855", "KOI8", 0},
    {"1252", "INST1252", 0},
    {0,0,0}
};

/*{
**  Name: CMset_attr - set character attributes.
**
**  Description:
**	Sets character attribute and case translation to correspond
**	to installed character set.  If an alternate character set has
**	never been installed, assure that the default character set is
**	represented.  If an error status is returned, the default character
**	set will be reflected, also.
**
**  Inputs:
**	name - name of character set.
**
**  Outputs:
**	Returns:
**		CM_NOCHARSET - character set does not exist.
**		STATUS
**
**  Side Effects:
**	may perform file i/o on a "hidden" file name.
**
**  History:
**	9/89 (bobm)    Created.
**	23-jan-2004 (stephenb)
**	    Set double-byte attribute
**	11-apr-2007 (gupsh01)
**	    Added CM_isUTF8.
*/
STATUS
CMset_attr(
char		*name,
CL_ERR_DESC	*err)
{
#ifdef VMS
	struct FAB fab;
	struct RAB rab;
	long status;
#else
	i4 fd;
#endif
	char *fn;
	bool exists;
	LOCATION loc;
	char lbuf[MAX_LOC+1];
	i4	i;

	SETCLERR(err, 0, 0);

	/*
	** set defaults in case we fail.
	*/
	CM_AttrTab = CM_DefAttrTab;
	CM_CaseTab = CM_DefCaseTab;
	CM_UTF8Bytes = CM_BytesForUTF8;

	if (name == NULL || *name == EOS)
		return OK;

	if ((fn = attrfile(name,&loc,lbuf,err,&exists)) == NULL)
		return FAIL;

	if (! exists)
	{
		SETCLERR(err, 0, ER_open);
		return CM_NOCHARSET;
	}

	if (STbcompare(name, 0, "utf8", 0, TRUE))
	    CM_isUTF8 = 1;

	for (i = 0; STbcompare(name, 0, charsets[i].key, 0, TRUE); i++)
	{
	    if (charsets[i].key[0] == 0)
	    {
		SETCLERR(err, 0, ER_open);
		return CM_NOCHARSET;
	    }
	}
	Double_byte = charsets[i].isdouble;

#ifdef VMS

	if (init_frab(&fab,&rab,fn,FALSE,&status) != OK)
	{
		err->error = status;
		return FAIL;
	}

	rab.rab$l_ubf = &Readattr;

	status = sys$get(&rab);
	if ((status & 1) == 0 || rab.rab$w_rsz != sizeof(CMATTR))
	{
		err->error = RMS$_USZ;
		sys$close(&fab);
		return FAIL;
	}

	/*
	** if we can't close the file, don't call it an error -
	** We did manage to retrieve the attributes, so we will
	** return OK.
	*/
	sys$close(&fab);

#else

	if ((fd = open(fn,O_RDONLY)) < 0)
	{
		SETCLERR(err, 0, ER_open);
		return FAIL;
	}

	if (read(fd, (char *) &Readattr, sizeof(CMATTR)) != sizeof(CMATTR))
	{
		close(fd);
		SETCLERR(err, 0, ER_read);
		return FAIL;
	}

	close(fd);

#endif

	CM_AttrTab = Readattr.attr;
	CM_CaseTab = Readattr.xcase;

	return OK;
}

/*{
** Name: CMget_attr - get character-set attributes
**
** Description:
**      Gets character set attributes. 
**
**	Currentlya only returns double-byte status
**
** Inputs:
**	None.
**
** Outputs:
**	isdouble - is current charset double-byte
**
** History:
**	23-jan-2004 (stephenb)
**	    Created.
*/
STATUS
CMget_attr(
char	*isdouble,
char	*isUTF8)
{
    *isdouble = Double_byte;
    *isUTF8 = CM_isUTF8;
    return(OK);
}

/* Name: CM_ischarsetUTF8 - checks if the II_CHARSETxx is set to
**       UTF8.
**
**  Description:
**      Returns TRUE if II_CHARSETxx is set to UTF8 else returns FALSE.
**
**  History:
**      26-Apr-2007 (gupsh01)
**          Created.
*/
bool
CM_ischarsetUTF8(void)
{
    if (CM_isUTF8)
      return(TRUE);
    else
      return(FALSE);
}

/*{
**  Name: CMwrite_attr - write character attribute file.
**
**  Description:
**	Use a passed-in CMATTR to write a named character attributes and case
**	translation file.  Once this call is successful, CMset_attr calls 
**	using that character set will reflect the given CMATTR.
**
**  Inputs:
**
**	name - name of character set.
**	attr - character attributes and case translation.
**
**  Outputs:
**	Returns:
**		STATUS
**
**  Side Effects:
**	Perform file i/o on a "hidden" file name.
**
**  History:
**	9/89 (bobm)    Created.
**
*/

STATUS
CMwrite_attr(name,attr,err)
char *name;
CMATTR *attr;
CL_ERR_DESC *err;
{
#ifdef VMS
	struct FAB fab;
	struct RAB rab;
	long status;
#else
	int fd;
#endif
	char *fn;
	bool trash;
	LOCATION loc;
	char lbuf[MAX_LOC+1];

	SETCLERR(err, 0, 0);

	if ((fn = attrfile(name,&loc,lbuf,err,&trash)) == NULL)
		return FAIL;

#ifdef VMS

	if (init_frab(&fab,&rab,fn,TRUE,&status) != OK)
	{
		err->error = status;
		return FAIL;
	}

	rab.rab$l_rbf = attr;

	status = sys$put(&rab);
	if ((status & 1) == 0)
	{
		sys$close(&fab);
		err->error = status;
		return FAIL;
	}

	status = sys$close(&fab);
	if ((status & 1) == 0)
	{
		err->error = status;
		return FAIL;
	}
#else

	if ((fd = open(fn,O_WRONLY|O_CREAT|O_TRUNC,0666)) < 0)
	{
		SETCLERR(err, 0, ER_open);
		return FAIL;
	}

	if (write(fd, (char *) attr, sizeof(CMATTR)) != sizeof(CMATTR))
	{
		close(fd);
		SETCLERR(err, 0, ER_write);
		return FAIL;
	}

	close(fd);

#endif

	return OK;
}

/*
** routine to concoct the file name using NM / LO, etc.  Also returns
** whether it exists or not.
*/

static char *
attrfile(name,loc,lbuf,err,exists)
char *name;
LOCATION *loc;
char *lbuf;
CL_ERR_DESC *err;
bool *exists;
{
	LOCATION	tloc;
	char		*str;
	STATUS		st;
	char		nm[CM_MAXATTRNAME+1];

	if (STlength(name) > CM_MAXATTRNAME)
	{
		SETCLERR(err, 0, ER_open);
		return NULL;
	}

	STcopy(name,nm);
	name = nm;

	/*
	** Check name via 'a' - 'z', 'A' - 'Z', '1' - '9'
	** ruse - we're inside the CL so we can get away with this.
	** Done this way so that we don't worry about our current
	** CM translation.  Must be done some different way on a system
	** where these characters are not consecutive.
	*/
	for ( str = name; *str != EOS; ++str)
	{
		if ( *str >= 'a' && *str <= 'z' )
			continue;
		if ( *str >= 'A' && *str <= 'Z' )
		{
			*str += 'a' - 'A';
			continue;
		}
		if ( *str < '0' || *str > '9' )
		{
			SETCLERR(err, 0, ER_open);
			return NULL;
		}
	}

	st =NMloc(FILES, PATH, (char *)NULL, &tloc);
	if (st != OK)
	{
		SETCLERR(err, 0, ER_open);
		return NULL;
	}

	LOcopy(&tloc,lbuf,loc);

	LOfaddpath(loc,"charsets",loc);
	LOfaddpath(loc,name,loc);
	LOfstfile("desc.chx",loc);

	LOtos(loc, &str);


	if (LOexist(loc) != OK)
		*exists = FALSE;
	else
		*exists = TRUE;

	return str;
}

/*{
**  Name: CMset_attr - set process' locale based upon II_CHARSETxx
**
**  Description:
**	This procedure grabs the value of II_CHARSETxx, using it to
**	determine the machine's proper locale mapping. It then uses
**	setlocale() to set the particular code page for the current
**	process.
**
**  Inputs:
**	none
**
**  Outputs:
**	Returns:
**		OK
**		FAIL
**
**  Side Effects:
**	none
**
**  History:
**	14-may-2002 (somsa01)
**	    Created.
*/
STATUS
CMset_locale()
{
    return OK;
}

/* Name: CM_getcharset- Get Platforms default character set
**
** Input:
**   pcs - Pointer to hold the value of platform characterset. The
**	   calling funtion allocates the memory for this variable.
** Output:
**   pcs - The platform character set.
** Results:
**   returns OK if successful, FAIL if error.
** History:
**
**	23-Jan-2004 (gupsh01)
**	    Created.
*/
STATUS
CM_getcharset(
char	*pcs)
{
    char *charset = NULL;

    if (!pcs)
	return FAIL;

    if (charset = setlocale (LC_CTYPE,0))
    {
	STlcopy (charset, pcs, CM_MAXLOCALE);
	return OK;
    }
    else
	return FAIL;
}

/* Name: CMget_charset_name - Get II_CHARSET name
**
** Description:
**	The Ingres character set is in an environment variable
**	named either II_CHARSET or II_CHARSETxx where xx is the
**	installation ID.  Fetching the character set name is
**	sufficiently common throughout Ingres that it's worth doing
**	a little routine to fetch it -- safely, with no buffer
**	overrun holes!
**
** Inputs:
**	charset		An output, actually
**
** Outputs:
**	charset		Caller supplied char[CM_MAXATTRNAME+1] area.
**			Name of character set is returned here.
**			If no II_CHARSET env vars at all, this is set
**			to a null string.
**
** History:
**	14-Jun-2004 (schka24)
**	    Write for buffer overrun safety.
**	22-Jun-2004 (schka24)
**	    Oops.  Avoid barfo if no II_CHARSET set, as advertised.
*/

void
CMget_charset_name(char *charset)
{

    char csevname[30+1];		/* II_CHARSETxx */
    char *evp;				/* Pointer to env var value */

    NMgtAt("II_INSTALLATION", &evp);	/* Get installation ID */
    if (evp == NULL || *evp == '\0')
    {
	/* No II_INSTALLATION, use II_CHARSET */
	NMgtAt("II_CHARSET", &evp);
    }
    else
    {
	/* Construct II_CHARSETxx */
	STlpolycat(2, sizeof(csevname)-1, "II_CHARSET", evp, &csevname[0]);
	NMgtAt(csevname, &evp);
    }
    charset[0] = '\0';
    if (evp != NULL)
	STlcopy(evp, &charset[0], CM_MAXATTRNAME);

} /* CMget_charset_name */

/* Name: CMset_charset - Set up CM character set attributes
**
** Description:
**	Quite a few Ingres components need to call CMset_attr with the
**	installation character set name, sometime during initialization.
**	This convenience routine simply combines calls to CMget_charset_name
**	(to get the installation character set name) and CMset_attr.
**
** Inputs:
**	cl_err			An output
**
** Outputs:
**	Returns OK or not-OK status (see CMset_attr)
**	cl_err			Place to return a CL_ERR_DESC describing
**				errors, if any. 
**
** History:
**	14-Jun-2004 (schka24)
**	    Write for buffer overrun fixups.
*/

STATUS
CMset_charset(CL_ERR_DESC *cl_err)
{

    char chset[CM_MAXATTRNAME+1];	/* Installation charset name */

    CMget_charset_name(&chset[0]);

    return (CMset_attr(chset, cl_err));

} /* CMset_charset */

i4 CMgetDefCS()
{
    return CMdefcs;
}

#ifdef VMS

static STATUS
init_frab(fab,rab,fn,creat,rstat)
struct FAB *fab;
struct RAB *rab;
char *fn;
bool creat;
i4 *rstat;
{
	i4 status;

	MEfill(sizeof(struct FAB), 0, (PTR)fab);
	MEfill(sizeof(struct RAB), 0, (PTR)rab);
	fab->fab$b_bid = FAB$C_BID;
	fab->fab$b_bln = FAB$C_BLN;
	rab->rab$b_bid = RAB$C_BID;
	rab->rab$b_bln = RAB$C_BLN;
	rab->rab$l_fab = fab;
	fab->fab$b_rfm = FAB$C_FIX;
	fab->fab$b_org = FAB$C_SEQ;
	rab->rab$w_usz = rab->rab$w_rsz = fab->fab$w_mrs = sizeof(CMATTR);
	fab->fab$l_fna = fn;
	fab->fab$b_fns = STlength(fn);
	rab->rab$b_rac = RAB$C_SEQ;

	if (creat)
		status = sys$create(fab);
	else
		status = sys$open(fab);
	if ((status & 1) == 0)
	{
		*rstat = status;
		return FAIL;
	}

	status = sys$connect(rab);
	if ((status & 1) == 0)
	{
		*rstat = status;
		return FAIL;
	}

	return OK;
}

#endif
