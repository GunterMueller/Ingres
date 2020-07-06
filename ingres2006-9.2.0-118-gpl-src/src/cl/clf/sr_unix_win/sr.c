/*
**Copyright (c) 2004 Ingres Corporation
*/
# include   <compat.h>
# include   <gl.h>
# include   <di.h>
# include   <sr.h>

# include   <errno.h>

/**
**
**  Name: SR.C - This file contains all the SR routines.
**
**  Description:
**      The SR.C file contains all the routines needed to perform
**      direct I/O to files.  These are used only for sort files.
**      The I/O is done in blocks(sectors).   
**
**        SRclose - Closes and may delete a file.
**        SRopen - Opens and may create and allocate space for a file.
**        SRread - Reads a page from a file.
**        SRwrite - Writes a page to a file and extends it if necessary.
**
**  History:
**	07-apr-87 (mmm)
**          Created new for 6.0.
**	21-jan-89 (mmm)
**	    SR files should not need syncing to disk, so don't set
**	    DI_SYNC_MASK in DIopen() call.
**	26-apr-1993 (bryanp)
**	    Prototyping SR for 6.5.
**	15-jul-93 (ed)
**	    adding <gl.h> after <compat.h>
**	19-apr-95 (canor01)
**	    added <errno.h>
**	10-jul-95 (canor01)
**	    NT_GENERIC patch to match NT DI structures
**	08-dec-1997 (canor01)
**	    Return SR_EXCEED_LIMIT if we run out of resources.
**	01-oct-1998 (somsa01)
**	    In SRwrite(), return SR_EXCEED_LIMIT if we hit DI_NODISKSPACE.
**	11-Nov-1999 (jenjo02)
**	    Use CL_CLEAR_ERR instead of SETCLERR to clear CL_ERR_DESC.
**	    The latter causes a wasted ___errno function call.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/

/*
**  Forward and/or External typedef/struct references.
*/


/*
**  Local Definitions.
*/

#define                 SR_MIN_INCR     128
                                        /* maximum number of blocks
                                        ** to expand file. */
#define                 SR_PATH_MAX     128
                                        /* maximum path length in
                                        ** bytes. */
#define                 SR_FILENAME_MAX 32
                                        /* maximum file name part
                                        ** of a full file name. */

/*
**  Definition of static variables and forward static functions.
*/


/*{
** Name: SRclose - Closes a file.
**
** Description:
**      The SRclose routine is used to close a direct access file.  
**      It will check to insure the file is open before trying to close.
**      It will also delete it if the delete flag is set to SR_IO_DELETE.
**   
** Inputs:
**      f                    Pointer to the SR file
**                           context needed to do I/O.
**      delete_flag          Variable used to indicate if 
**                           file is to be deleted.  This must 
**                           always be set to SR_IO_DELETE.
** Outputs:
**      err_code             Pointer to a variable used
**                           to return operating system 
**                           errors.
**    Returns:
**          OK
**          SR_BADCLOSE		Error closing file or bad parameters.
**          SR_BADFILE		Bad file context.
**    Exceptions:
**        none
**
** Side Effects:
**        none
**
** History:
**    07-apr-87 (mmm)
**          Created new for 6.0.
*/
STATUS
SRclose(
SR_IO		*f,
i4		delete_flag,
CL_ERR_DESC	*err_code)
{
    STATUS	ret_val;

    CL_CLEAR_ERR( err_code );

    /*  Check for good parmeters */

    if ((delete_flag) && (delete_flag != SR_IO_DELETE))
    {
	return(SR_BADPARAM);
    }

    /*	Check for valid control block. */

    if (f->io_type != SR_IO_ASCII_ID) 
        return (SR_BADFILE);

    if (!(ret_val = DIclose(f, err_code)))
    {
	if (delete_flag)
	{
	    ret_val = DIdelete(f, f->io_pathname, f->io_l_pathname, 
			       f->io_filename, f->io_l_filename, err_code);
	}
    }

    if (ret_val)
    {
	ret_val = SR_BADCLOSE;
    }

    return(ret_val);
}

/*{
** Name: SRopen - Opens a file.
**
** Description:
**      The SRopen routine is used to open a direct access file.  
**      It will open the file for write access and will
**      place no locks on the file.  If the create_flag is set 
**      it will create it and allocate the amount specified.
**      Additional space may be required to accomplish the sort.
**      An unlimited number of extensions must be allowed.
**      You can specify that nothing should be allocated at
**      create time.
**   
** Inputs:
**      f                    Pointer to the SR file
**                           context needed to do I/O.
**      path                 Pointer to the directory name
**                           for the file.
**      pathlength           Length of path name.
**      filename             Pointer to file name.
**      filelength           Length of file name.
**      pagesize             Value indicating size of page 
**                           in bytes.  Must be one of:
**				4096, 8192, 16384 , 32768.
**      create_flag          Value indicating if creation needed.
**                           Must be SR_IO_CREATE.
**      n                    Value indicating number of pages to
**                           pre-allocate.
**      
** Outputs:
**      f                    Updates the file control block.
**      err_code             Pointer to a variable used
**                           to return operating system 
**                           errors.
**    Returns:
**          OK
**          SR_BADDIR		Error in path specification.
**          SR_BADOPEN		Error opening file.
**          SR_BADFILE		Bad file context.
**          SR_PADPARAM		Parameter(s) in error.
**    Exceptions:
**        none
**
** Side Effects:
**        none
**
** History:
**    07-apr-87 (mmm)
**          Created new for 6.0.
**    21-jan-89 (mmm)
**	    SR files should not need syncing to disk, so don't set
**	    DI_SYNC_MASK in DIopen() call.
**    24-feb-97 (cohmi01)
**	    Pass back SR_EXCEED_LIMIT if so indicated by DIopen(). Bug 80242.
*/
STATUS
SRopen(
SR_IO           *f,
char            *path,
u_i4		pathlength,
char            *filename,
u_i4	        filelength,
i4		pagesize,
u_i4		create_flag,
i4         n,
CL_ERR_DESC	*err_code)
{
    STATUS	ret_val;

    /* defaults */
    ret_val = OK;

    if (create_flag)
    {
	ret_val = DIcreate(f, path, pathlength, filename, filelength, 
			   pagesize, err_code);
    }

    if (!ret_val)
    {
	/* we are assuming that SR files do not need to
	** be guaranteed to disk.
	*/
	ret_val = DIopen(f, path, pathlength, filename, filelength, 
			 pagesize, (i4) DI_IO_WRITE, 
			 (u_i4) 0, err_code);
    }
	    
    if (ret_val)
    {
	switch (ret_val)
	{ 
	    case DI_BADDIR:
		ret_val = SR_BADDIR;
		break;
	    case DI_BADFILE:
		ret_val = SR_BADFILE;
		break;
	    case DI_EXCEED_LIMIT:
		ret_val = SR_EXCEED_LIMIT;
		break;
	    default:
		ret_val = SR_BADOPEN;
		break;
	}
    }

    /* Since SR files can be indefinitely extended by writing to them set
    ** allocated end of file to largest possible page.
    */
# ifdef NT_GENERIC
    f->io_system_eof = MAXI4;
# else
    f->io_alloc_eof = MAXI4;
# endif /* NT_GENERIC */

    return(ret_val);
}

/*{
** Name: SRread - Read a page of a file.
**
** Description:
**      The SRread routine is used to read pages of a direct access 
**      sort file. 
**   
** Inputs:
**      f                    Pointer to the SR file
**                           context needed to do I/O.
**      page                 Value indicating page to read.
**      buf                  Pointer to the area to hold
**                           page being read.
**      
** Outputs:
**      f                    Updates the file control block.
**      err_code             Pointer to a variable used
**                           to return operating system 
**                           errors.
**    Returns:
**          OK
**          SR_BADFILE		Bad file context.
**          SR_BADREAD		Error reading file.
**          SR_ENDFILE		End of file reached.
**    Exceptions:
**        none
**
** Side Effects:
**        none
**
** History:
**    07-apr-87 (mmm)
**          Created new for 6.0.
*/
STATUS
SRread(
SR_IO	        *f,
i4         page,
char            *buf,
CL_ERR_DESC	*err_code)
{
    STATUS	ret_val;
    i4		num_of_pages;

    num_of_pages = 1;

    if (ret_val = DIread(f, &num_of_pages, page, buf, err_code))
    {
	if (ret_val ==  DI_BADFILE)
	    ret_val = SR_BADFILE;
	else if (ret_val ==  DI_BADPARAM)
	    ret_val = SR_BADPARAM;
	else if (ret_val ==  DI_ENDFILE)
	    ret_val = SR_ENDFILE;
	else if (ret_val == DI_EXCEED_LIMIT)
	    ret_val = SR_EXCEED_LIMIT;
	else 
	    ret_val = SR_BADREAD;
    }

    return(ret_val);
}

/*{
** Name: SRwrite -  Writes a page of a file.
**
** Description:
**      The SRwrite routine is used to write pages of a direct access 
**      file.  To insure that a full page can always be read or 
**      written, this routine must always allocate pages using the
**      maximum page size of 32K.  Then if the file is created 
**      with a size of 8K, but openned with a size of 32K, there 
**      will be enough pages allocated to satisfy the last read
**      request even if only 8K of the last page is valid.
**   
** Inputs:
**      f                    Pointer to the SR file
**                           context needed to do I/O.
**      page                 Value indicating page to write.
**      buf                  Pointer to page to write.
**      
** Outputs:
**      f                    Updates the file control block.
**      err_code             Pointer to a variable used
**                           to return operating system 
**                           errors.
**    Returns:
**          OK
**          SR_BADFILE		Bad file context.
**          SR_BADWRITE		Error writing file.
**          SR_BADPARAM		Parameter(s) in error.
**    Exceptions:
**        none
**
** Side Effects:
**        none
**
** History:
**    07-apr-87 (mmm)
**          Created new for 5.0.
**	01-oct-1998 (somsa01)
**	    For now, DI_NODISKSPACE meanse SR_EXCEED_LIMIT.
*/
STATUS
SRwrite(
SR_IO	        *f,
i4         page,
char            *buf,
CL_ERR_DESC	*err_code)
{
    STATUS	ret_val;
    i4		num_of_pages;

    num_of_pages = 1;

    if (ret_val = DIwrite(f, &num_of_pages, page, buf, err_code))
    {
	if (ret_val ==  DI_BADFILE)
	    ret_val = SR_BADFILE;
	else if (ret_val ==  DI_BADPARAM)
	    ret_val = SR_BADPARAM;
	else if (ret_val ==  DI_ENDFILE)
	    ret_val = SR_ENDFILE;
	else if ((ret_val == DI_EXCEED_LIMIT) || (ret_val == DI_NODISKSPACE))
	    ret_val = SR_EXCEED_LIMIT;
	else 
	    ret_val = SR_BADWRITE;
    }

    return(ret_val);
}
