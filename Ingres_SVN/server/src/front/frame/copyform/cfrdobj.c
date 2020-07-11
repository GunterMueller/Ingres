# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<st.h>
# include	<si.h>
# include	<lo.h>
# include	<nm.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include <ooclass.h>
# include	<uf.h>
# include	<ug.h>
# include	<cf.h>
# include	<me.h>
#include	"ercf.h"
/*{
** Name:	CF_RDOBJ - read in a file containing form-related definitions
**
** Description:
**	Reads an intermediate file containing definitions for one or more forms,
**	qbfnames and/or joindefs, and writes them to the open database.
**
** Input params:
**	filename	name of file containing specs.
**	username	username of owner of db.
**	replace		TRUE if forms to be replaced without prompt.
**
** Output params:
**	none
**
** Returns:
**	OK		completed successfully.
**	FAIL		did not complete successfully.
**
** Exceptions:
**	none
**
** Side Effects:
**	none
**
** Pseudocode:
**	open intermediate file.
**	read first record of intermediate file.
**	IF version incompatible
**		close intermediate file.
**		create new format intermediate file (cf_convto60).
**		open new intermediate file.
**		read first record of intermediate file.
**	ENDIF
**
**	IF there is an object name clash
**		resolve user overwrite intentions.
**		IF overwrite not desired
**			return.
**		ENDIF
**	ENDIF
**
**	WHILE not end of file
**		read record.
**		determine record type.
**		write record to appropriate catalog.
**
**	END PROCEDURE
**
** History:
**	8/2/83 (ps)	written.
**	10/5/84 (agh)	Eliminated reference to fldummy field in fdfields
**			catalog.
**	06/12/85 (jrc)	Modified to look for versions after the names of the
**			catalogs.  Currently only knows about version 3.0 or
**			no version present.  Only the QBFMAP is changed.
**	06/12/85 (scl)	Restructured some stuff to make it easier for CMS to
**			massage the filenames. (Also changed the temp
**			filenames so they will not exceed 8 characters.)
**	10/27/85 (prs)	Change cf_readln to SIgetrec.
**	11/04/85 (prs)	Fix bugs with reading in files.
**	17-Apr-87 (rld) Modified to use new FE catalogs (ii_..); tuple must
**			be placed in ii_objects for each form, qbfname and
**			joindef to be copied.  Fixed to correctly resolve
**			namespace for associated qbfnames and joindefs.
**			(Previously qbfnames and joindefs were copied only
**			if they has the same owner as the form).
**	19-nov-1987 (rdesmond) Fixed strange memory stomping bug by placing
**			definition of inlist at beginning of decl list.
**	22-dec-1987 (rdesmond)
**			moved begin transaction statement after file
**			conversion so temp file from conversion may be 
**			deleted (no deletes within multi-statement transaction).
**	03-mar-1988 (rdesmond)
**			changed timing of 'begin transaction' and 
**			'end transaction'.  Transaction now begins for each
**			object in cf_addobj() and ends in this procedure.
**	07/28/88 (dkh) - Put in special hook so that formindex can
**			 find out the name of the form that was copied in.
**			 Declaration of Cf_silent and Cfversion moved here
**			 from copyform.c
**	08/02/88 (dkh) - Moved Cfversion and Cf_silent back to copyform.c.
**	09/03/88 (dkh) - Reduced size of temporary filename for portability.
**	01/25/90 (dkh) - Changes for new LO on VMS.
**	07/06/90 (dkh) - Added error message if input file is empty.
**	07/11/90 (dkh) - Put in check for bad input so we know when
**			 an input with bad data was specified. 
**	08/11/91 (dkh) - Fixed bug 39055.
**	08/18/93 (dkh) - Changed code to eliminate compile problems on NT.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      29-Nov-1999 (hanch04)
**          First stage of updating ME calls to use OS memory management.
**          Make sure me.h is included.  Use SIZE_TYPE for lengths.
[@history_template@]...
*/
GLOBALREF char	Cfversion[];
GLOBALREF bool	Cf_silent;
STATUS
cf_rdobj(filename, username, replace, formname)
char	*filename;
char	*username;
bool	replace;
char	*formname;
{
	/* internal declarations */
	FORMINFO	in_array[MAX_OBJNAMES];
	FORMINFO	*inlist;
	i4		incount;
	i4		max_objs = MAX_OBJNAMES;
	i4		validinput;
	char		inbuf[750];		/* character buffer */
	char		yorn[6];		/* Buffer for prompt */
	char		objname[FE_MAXNAME+1];
	char		*short_remark;
	char		*long_remark;
	OOID		class;
	OOID		curr_id;
	i4		rectype;
	bool		goodcopy;
	bool		conversion;
	i4		msgptr;
	LOCATION	tloc;
	LOCATION	loc_forms;
	FILE		*fd_forms;
	char		loc_buf[MAX_LOC + 1];
	char		file_locbuf[MAX_LOC + 1];
	char		*new_filename;
	char		*orig_fname;
	if (formname)
	{
	    *formname = EOS;
	}
	inlist = &(in_array[0]);
	orig_fname = filename;
	/* start of routine */
	IIUIendXaction();
	/* Open intermediate file */
	STcopy(filename, file_locbuf);
	LOfroms(PATH & FILENAME, file_locbuf, &loc_forms);
	if (SIopen(&loc_forms, ERx("r"), &fd_forms) != OK)
	{
	    IIUGerr(E_CF0000_CANNOT_OPEN_FILE, 0, 1, filename);
	    return (FAIL);
	}
	/* get first record */
	if (SIgetrec(inbuf, 750, fd_forms) == ENDFILE)
	{
		IIUGerr(E_CF0024_EMPTY, 0, 1, filename);
		return (FAIL);
	}
	/* Check if file is appropriate version */
	if (STbcompare(inbuf, 11, ERx("COPYFORM:\t6"), 11, FALSE) == 0)
	{
		conversion = FALSE;
	}
	else
	{
		/* Incompatible version, must convert first */
		SIclose(fd_forms);
		/* generate new file name and get location */
		NMloc(TEMP, PATH, NULL, &tloc);
		LOuniq(ERx("cfr"), ERx("cnv"), &tloc);
		LOcopy(&tloc, loc_buf, &loc_forms);
		LOtos(&loc_forms, &new_filename);
		/* convert file to new format */
		if ((cf_convto60(filename, new_filename) != OK)
		/* Open converted intermediate file */
		|| (SIopen(&loc_forms, ERx("r"), &fd_forms) != OK)
		/* get first record */
		|| (SIgetrec(inbuf, 750, fd_forms) == ENDFILE)
		/* Check if file is appropriate version */
		|| (STbcompare(inbuf, 11, ERx("COPYFORM:\t6"), 11, FALSE) != 0))
		{
		    IIUGerr(E_CF0008_CANNOT_CONVERT_FILE, 0, 1, new_filename);
		    return (FAIL);
		}
		/* set flag so new file can be deleted */
		conversion = TRUE;
		filename = new_filename;
	}
	/* scan away first token */
	cf_gettok(inbuf);
	if (!replace)
		/* check if any of the objects already exist in database */
		if (cf_conflict(filename, username))
		{
		    FEprompt(ERget(F_CF0000_OVERWRITE_PROMPT), FALSE, 5, yorn);
		    if(!IIUGyn(yorn, (STATUS *)NULL))
			return (FAIL);
		}
	class = 0;
	incount = 0;
	validinput = FALSE;
	for(;;)
	{
	    /* read until end of file reached */
	    rectype = (SIgetrec(inbuf, 750, fd_forms) == ENDFILE) ?
	      RT_EOF : cf_rectype(inbuf);
	    /* new object encountered (or EOF), write appropriate message */
	    if ((rectype == RT_QBFHDR || rectype == RT_JDEFHDR ||
	      rectype == RT_FORMHDR || rectype == RT_EOF) && (class != 0))
	    {
		/*
		** End the transaction for this object, which was begun in
		** cf_addobj().
		*/
		IIUIendXaction();
		if (goodcopy)
		{
		    /* previous object was copied */
		    switch(class)
		    {
			case OC_QBFNAME:
			    msgptr = S_CF0009_COPIED_QBFNAME;
			    break;
			case OC_JOINDEF:
			    msgptr = S_CF0010_COPIED_JOINDEF;
			    break;
			case OC_FORM:
			    msgptr = S_CF0011_COPIED_FORM;
			    break;
		    }
		    /* print status message */
		    if (!Cf_silent)
			FEmsg(ERget(msgptr), (bool)FALSE, objname);
		}
		else
		{
		    /* previous object was not copied */
		    switch(class)
		    {
			case OC_QBFNAME:
			    msgptr = E_CF0012_CANNOT_COPY_QBFNAME;
			    break;
			case OC_JOINDEF:
			    msgptr = E_CF0013_CANNOT_COPY_JOINDEF;
			    break;
			case OC_FORM:
			    msgptr = E_CF0014_CANNOT_COPY_FORM;
			    break;
		    }
		    /* print error message */
		    IIUGerr(msgptr, 0, 1, objname);
		}
	    }
	    if (rectype == RT_EOF)
	    /* end of file */
		break;
	    /* add tuple to appropriate relation */
	    switch(rectype)
	    {
		case RT_QBFHDR:
		case RT_JDEFHDR:
		case RT_FORMHDR:
		    validinput = TRUE;
		    cf_gettok(inbuf);
		    STcopy(cf_gettok(NULL), objname);
		    /* determine ooclass for ii_object access */
		    switch(rectype)
		    {
			case RT_QBFHDR:
			    class = OC_QBFNAME;
			    break;
			case RT_JDEFHDR:
			    class = OC_JOINDEF;
			    break;
			case RT_FORMHDR:
			    class = OC_FORM;
			    /* save formname to compile later */
			    /*
			    **  Check to see if we need to allocate a bigger
			    **  array if there are more objects than the
			    **  static array.
			    */
			    if (incount >= max_objs)
			    {
				PTR		pointer;
				PTR		oldarray;
				PTR		newarray;
				u_i4	size;
				u_i4	oldsize;
				u_i2		cpsize;
				oldsize = sizeof(FORMINFO) * max_objs;
				size = oldsize + oldsize;
				if ((pointer = (PTR) MEreqmem((u_i4) 0, size,
					(bool) TRUE, NULL)) == NULL)
				{
					/*
					**  Can't allocate more memory.
					**  Give error and return FAIL.
					*/
					IIUGbmaBadMemoryAllocation(ERx("cfrdobj()"));
					if (inlist != &(in_array[0]))
					{
						MEfree(inlist);
					}
					return(FAIL);
				}
				newarray = pointer;
				oldarray = (PTR) inlist;
				/*
				**  Copy contents of old array to new array.
				*/
				while(oldsize > 0)
				{
					cpsize = (oldsize <= MAXI2) ? oldsize : MAXI2;
					MEcopy(oldarray, cpsize, newarray);
					oldsize -= cpsize;
					oldarray = (PTR) ((char *) oldarray + 
						cpsize);
					newarray = (PTR) ((char *) newarray +
						cpsize);
				}
				/*
				**  Free up old array if it was dynamically
				**  allocated.
				*/
				if (inlist != &(in_array[0]))
				{
					MEfree(inlist);
				}
				/*
				**  Set new count for maximum objects that
				**  array can hold and reassign pointers.
				*/
				max_objs += max_objs;
				inlist = (FORMINFO *) pointer;
			    }
			    inlist[incount].name = STalloc(objname);
			    break;
		    }
		    short_remark = cf_gettok(NULL);
		    long_remark = cf_gettok(NULL);
		    goodcopy = TRUE;
		    if (cf_addobj(class, objname, username,
		      short_remark, long_remark, &curr_id) != OK)
			goodcopy = FALSE;
		    if (class == OC_FORM)
			inlist[incount++].id = curr_id;
		    break;
		case RT_QBFDET:
		case RT_JDEFDET:
		case RT_FORMDET:
		case RT_FIELDDET:
		case RT_TRIMDET:
		    validinput = TRUE;
		    if (cf_adddet(rectype, inbuf, curr_id) != OK)
			goodcopy = FALSE;
		    break;
	    }
	}
	/* close intermediate file */
	SIclose(fd_forms);
	if (conversion)
	    /* delete converted intermediate file */
	    LOdelete(&loc_forms);
	IIUIendXaction();
	/*
	**  If we didn't find any good input, then the input
	**  file was either be corrupted or was not in copyform format.
	**  We just put out an error message and exit with FAIL.
	**  Note that the checking for validinput is (very) simple
	**  and assumes people are not malicious about trashing things.
	**  The checking can be fooled with liitle effort.
	*/
	if (!validinput)
	{
		IIUGerr(E_CF0025_BADFILE, 0, 1, orig_fname);
		/*
		**  Free up any dynamically allocated array before
		**  returning.
		*/
		if (inlist != &(in_array[0]))
		{
			MEfree(inlist);
		}
		return(FAIL);
	}
	/* compile forms */
	if (incount != 0)
	{
	    cfcompile(inlist, incount);
	    /*
	    **  If from formindex, copy the name of form
	    **  into "formname" so formindex can use it.
	    **  We are of course assuming that formindex
	    **  is only called with copyform files that
	    **  contain only one form.
	    */
	    if (formname)
	    {
		STcopy(inlist[0].name, formname);
	    }
	}
	/*
	**  Free up any dynamically allocated array before
	**  returning.
	*/
	if (inlist != &(in_array[0]))
	{
		MEfree(inlist);
	}
	return (OK);
}