/*
**Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
NO_OPTIM = i64_aix
*/

#include	<compat.h>
#include	<gl.h>
#include	<st.h>
#include	<clconfig.h>
#include	"lolocal.h"
#include	<lo.h>

/**
** Name:	louniq.c -	CL Location Module Make a Unique Location.
**
** History:
**	Revision 6.2  89/02/23  daveb, seng
**	Rename LO.h to lolocal.h
**	89/07  wong  Modified to make location using input path location
**			for uniqueness.
**
**	Revision 2.0  83/03/09  mikem
**	Initial revision.
**	15-jul-93 (ed)
**	    adding <gl.h> after <compat.h>
**	16-aug-93 (ed)
**	    add <st.h>
**	29-may-1997 (canor01)
**	    Clean up compiler warning.
**      28-apr-1999 (hanch04)
**          Replaced STlcopy with STncpy
**	01-aug-2001 (toumi01 or stephenb)
**	    NO_OPTIM for i64_aix - can't recall why.
*/

/*{
** Name:	LOuniq() -	Make a Unique Location.
**
** Description:
**	Make unique location.  It appends process ID using 'mktemp()' so the
**	location is guaranteed to be unique with respect to other processes on
**	the system, if not within the current process.  At present, it does not
**	guarantee a unique location if a suffix is specified.  Note also that
**	uniqueness cannot be guaranteed on an NFS file system.
**
**	To work correctly, the input path location must contain the path for
**	the file when 'LOuniq()' is called (any file name will be ignored.)
**	Then the input pattern string is prepended to an internal alphanumeric
**	pattern guaranteed (with respect to other calls of 'LOuniq()' for the
**	same directory) to be unique.  A NULL or empty path will use the
**	current process path to determine uniqueness.
**
**	The resulting file name including the input path is placed in
**	the input path location.  The file is not created.
**
** History:
**	07/89 (jhw) -- Modified to make name using input path.  This corrects
**		behavior for QG module when queries are nested and multiple
**		temporary files are opened.  Bug #14810 and JupBug #7185.
**	03/09/83 -- (mmm) written
**    15-aug-89 (kimman)
**	    Integrate 62devcl changes to louniq.c as per change #10057/#10065
**	23-may-90 (blaise)
**	    Integrated changes from ingresug:
**		Add "include <clconfig.h>".
*/

# define	EXTENSION_LENGTH	6
# define	SUFFIX_LENGTH		4

STATUS
LOuniq(
	char			*pat,
	char			*suffix,
	register LOCATION	*pathloc)
{
	STATUS		rval;
	register char	*nmp;			/* file name pointer */
	char		buf[MAX_LOC + 1];	/* path buffer (+ file name) */
	LOCATION	loc;

	char		*mktemp();

	buf[0] = EOS;

	/* Get path from input path location */
	if ( pathloc->path == NULL || pathloc->path == pathloc->fname )
		nmp = buf;
	else
	{ /* get path from input */
		if ( pathloc->fname == NULL ) 
		{
			STncpy(buf, pathloc->path, MAX_LOC);
			buf[ MAX_LOC ] = EOS;
			nmp = buf + STlen(buf);
		}
		else
		{
			/* assert:  STlength(pathloc->path) < sizeof(buf) */
			STncpy(buf, pathloc->path, MAX_LOC );
			buf[ MAX_LOC ] = EOS;
			nmp = buf + ( pathloc->fname - pathloc->path );
			if ( *(nmp-1) != '/' )
			{
				*nmp++ = '/';
				*nmp = EOS;
			}
		}
	}

	/* Copy input pattern to file name (which is appended to path name) */
	if ( pat != NULL && *pat != EOS )
		STcopy(pat, nmp);

	/* Check allowed file name size */
	if ( suffix == NULL || *suffix == EOS )
	{ /* no extension so create a MAXFILENAMELEN long file name */

		/* don't over run file name space */

		if (*nmp != EOS && STlength(nmp) > LO_NM_LEN - EXTENSION_LENGTH)
		{
			nmp[LO_NM_LEN - EXTENSION_LENGTH] = EOS;
		}
	}
	else
	{ /* Suffix to be added so allow for 4 characters appended at the end */
		if (STlength(suffix) >= SUFFIX_LENGTH)
			suffix[SUFFIX_LENGTH - 1] = EOS;
			/* SUFFIX_LENGTH includes '.' */

		if ( *nmp != EOS && STlength(nmp) >
				LO_NM_LEN - EXTENSION_LENGTH - SUFFIX_LENGTH )
		{ /* pattern too long -- chop it */
			nmp[LO_NM_LEN - EXTENSION_LENGTH - SUFFIX_LENGTH] = EOS;
		}
	}

	/* 'mktemp()' replaces "XXXXXX" with the current process ID,
	** and if necessary, a unique letter.
	*/

	STcat(nmp, "XXXXXX");
	mktemp(buf);

	/* add suffix if there is one */

	if ( suffix != NULL && *suffix != EOS )
		STpolycat(2, ".", suffix, nmp + STlength(nmp));

	/* Add generated file name to input path location */

	if ( (rval = LOfroms(FILENAME, nmp, &loc)) == OK )
		return LOstfile(&loc, pathloc);
	return rval;
}
