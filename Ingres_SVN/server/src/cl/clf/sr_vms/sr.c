/*
**    Copyright (c) 1985, 2003 Ingres Corporation
*/

# include   <compat.h>
# include   <gl.h>
# include   <sr.h>
# include   <me.h>
# include   <cs.h>
# include   <iodef.h>
# include   <iosbdef.h>
# include   <fibdef.h>
# include   <ssdef.h>
# include   <eventflag.h>
#ifdef xDEBUG
# include   <dvidef.h>
# include   <dcdef.h>
# include   <vmstypes.h>
#endif
# include   <starlet.h>

/**
**
**  Name: SR.C - This file contains all the SR routines.
**
**  Description:
**      The SR.C file contains all the routines needed to perform
**      direct I/O to files.  These are used only for sort files.
**      The I/O is done in blocks(sectors).   
**
**        SRclose - Closes aan may delete a file.
**        SRopen - Opens and may create and allocate space for a file.
**        SRread - Reads a page from a file.
**        SRwrite - Writes a page to a file and extends it if necessary.
**
**  History:
**      30-sep-85 (jennifer)    
**          Created new for 5.0.
**      02-apr-86 (jennifer)
**          Modified the system error returned from a i4 to
**          the type CL_ERR_DESC per CL request.
**	09-mar-1987 (Derek)
**	    Changed allocate to minimize number of I/O's to extend file.
**	27-jul-1987 (rogerk)
**	    Check for disk full or quota exceeded and return SR_EXCEED_LIMIT.
**	26-mar-91 (jrb)
**	    Got rid of compiler warnings.  (Added FIB pointer.)
**	26-apr-1993 (bryanp)
**	    Prototyping code for 6.5.
**      16-jul-93 (ed)
**	    added gl.h
**	19-jul-2000 (kinte01)
**	   Correct prototype definitions by adding missing includes
**	01-dec-2000	(kinte01)
**	    Bug 103393 - removed nat, longnat, u_nat, & u_longnat
**	    from VMS CL as the use is no longer allowed
**	02-sep-2003 (abbjo03)
**	    Changes to use VMS headers provided by HP.
**/

/*
**  Forward and/or External typedef/struct references.
*/

typedef struct _SR_STR_DESC SR_STR_DESC;

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

/*}
** Name: SR_STR_DESC - Area to contain directory or file names.
**
** Description:
**      This structure is used to contain an operating system 
**      descriptor for file or path names. 
**
** History:
**     30-sep-85 (jennifer)
**          Created new for 5.0.
**      02-apr-86 (jennifer)
**          Modified the system error returned from a i4 to
**          the type CL_ERR_DESC per CL request.
*/
typedef struct _SR_STR_DESC
{
    i4              str_len;            /* lenght of string */
    char            *str_str;           /* the string */
};

/*
**  Definition of static variables and forward static functions.
*/

static	STATUS	iiSRallocate(
		    SR_IO		*f,
		    i4		n,
		    i4			sid,
		    CL_ERR_DESC		*err_code);

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
**    11-sep-85 (jennifer)
**          Created new for 5.0.
**      02-apr-86 (jennifer)
**          Modified the system error returned from a i4 to
**          the type CL_ERR_DESC per CL request.
**     8-dec-88 (rogerk)
**	    Check that we still own the file we are about to deaccess.  Look
**	    for problem where we close somebody else's file channel.
*/
STATUS
SRclose(
SR_IO		*f,
i4		delete_flag,
CL_ERR_DESC	*err_code)
{
    IOSB		local_iosb;         /*  Operation return status. */
    int			s;                  /*  Request return status. */
    i4			sid;
#ifdef xDEBUG
    long		device_class;
    long		dvi_length;
    II_VMS_ITEM_LIST_3	itmlst = {4, DVI$_DEVCLASS, &device_class, &dvi_length};
#endif

    /*	Check for valid control block. */

    if (f->io_type != SR_IO_ASCII_ID) 
        return (SR_BADFILE);

    /*	Check that the file is open. */

    if (f->io_channel == 0)
        return (OK);
    
    CSget_sid(&sid);
    
#ifdef xDEBUG
    /*
    ** Check that the channel we are deallocating is still connected
    ** to a disk device.  This will trap the errors where we are
    ** conflicting with GCF on IO channels.
    */
    s = sys$getdvi(0, f->io_channel, 0, &itmlst, &local_iosb,
	CSresume, sid, 0);
    if (s & 1)
    {
	CSsuspend(CS_DIO_MASK, 0, 0);
	s = local_iosb.iosb$w_status;
    }
    if (((s & 1) == 0) || (device_class != DC$_DISK))
    {
	/*
	** This io channel is no longer any good, or is not what we
	** think it is.  Return BADCLOSE and either the system error
	** (if one was returned), or SS$_NOTFILEDEV.
	*/
	err_code->error = s;
	if (s & 1)
	    err_code->error = SS$_NOTFILEDEV;
	return (SR_BADCLOSE);
    }
#endif

    /*	Queue request to close and delete the file. */

    s = sys$qio(CACHE_IO_EF, f->io_channel, 
		IO$_DEACCESS, &local_iosb, CSresume, sid,
		0, 0, 0, 0, 0, 0);

    if (s & 1)
    {
	/*  Successfully queued, wait for completion. */

	CSsuspend(CS_DIO_MASK, 0, 0);

	if (local_iosb.iosb$w_status & 1)
	{
	    /*	Deassign the channel. */

	    s = sys$dassgn(f->io_channel);
	    if (s & 1)
	    {
		/*  Return successful. */

		f->io_channel = 0;
		f->io_type = 0;
		return (OK);
	    }   
	}
	else
	    s = local_iosb.iosb$w_status;
    }

    err_code->error = s;
    return (SR_BADCLOSE);
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
**	    SR_EXCEED_LIMIT	Too many open files, exceeding disk quota
**				or exceeding available disk space.
**    Exceptions:
**        none
**
** Side Effects:
**        none
**
** History:
**    30-sep-85 (jennifer)
**          Created new for 5.0.
**      02-apr-86 (jennifer)
**          Modified the system error returned from a i4 to
**          the type CL_ERR_DESC per CL request.
**	27-jul-87 (rogerk)
**	    Added return code SR_EXCEED_LIMIT.
**	17-aug-87 (rogerk)
**	    Save local return status after CSsuspend call before acting on
**	    its value.  We were losing the return status from the allocate
**	    call by seting the local return status from CSsuspend later on.
**	11-may-98 (kinte01)
**	    Added page size for 65536
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
    IOSB		local_iosb;         /* Operation return status. */
    i4 	        s;		    /* Request return status. */
    FIBDEF		*fib;
    struct
    {
	i4	    count;
	char	    *pointer;
    }			descriptor;
    i4			sid;

    /* Initialize file control block. */

    MEfill (sizeof(SR_IO), NULL, (PTR)f);
    f->io_type = SR_IO_ASCII_ID;
    CSget_sid(&sid);

    /* Assign a channel for I/O. */

    descriptor.count = pathlength;
    descriptor.pointer = path;
    s = sys$assign(&descriptor, &f->io_channel, 0, 0);
    if ((s & 1) == 0)
    {
        err_code->error = s;
	if ((s == SS$_EXQUOTA) || (s == SS$_EXDISKQUOTA) || (s == SS$_NOIOCHAN))
	    return (SR_EXCEED_LIMIT);
        return (SR_BADOPEN); 
    }

    /* Access the file with no locking.*/

    fib = (FIBDEF *)f->io_fib;
    fib->fib$l_acctl = FIB$M_NOLOCK | FIB$M_WRITE;
    fib->fib$w_nmctl = 0;
    fib->fib$w_exctl = 0;
    f->io_fib_desc.str_len = sizeof(f->io_fib);
    f->io_fib_desc.str_str = f->io_fib;

    /*	Queue the Create call. */

    s = sys$qio(CACHE_IO_EF, f->io_channel, 
		IO$_CREATE | IO$M_CREATE | IO$M_ACCESS | IO$M_DELETE,
		&local_iosb, CSresume, sid,
		&f->io_fib_desc, 0, 0, 0, 0, 0);

    if (s & 1)
    {
	/*  Successfully queued, wait for completion. */

        CSsuspend(CS_DIO_MASK, 0, 0);
	s = local_iosb.iosb$w_status;
	if (local_iosb.iosb$w_status & 1)
	{
	    /*	Successful completion, setup the control block. */

	    fib->fib$l_acctl = 0;
	    f->io_block_size = pagesize;
	    f->io_blks = pagesize >> 9;
	    f->io_log2_blk = 3;
	    if (pagesize != 4096)
	    {
		f->io_log2_blk = 4;
		if (pagesize != 8192)
		{
		    f->io_log2_blk = 5;
		    if (pagesize != 16384)
		    {
			f->io_log2_blk = 6;
			if (pagesize != 32768)
                           f->io_log2_blk = 7;	
		    }		
		}
	    }
	    if (n != 0)
	    {
		/*  Preallocate space for the file. */

		s = iiSRallocate(f, n, sid, err_code);
		if (s == OK)
		    return (OK);

		/*
		** If allocate returns an error, stuff the os errcode into
		** 's' for processing below.
		*/
		s = err_code->error;
	    }
	    else
	    {
		return (OK);
	    }
	}
    }

    /*	Return error. */

    err_code->error = s;
    sys$dassgn(f->io_channel);
    f->io_type = 0;
    if ((s == SS$_DEVICEFULL) || (s == SS$_EXQUOTA) || (s == SS$_EXDISKQUOTA))
	return (SR_EXCEED_LIMIT);
    return (SR_BADCREATE);
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
**    30-sep-85 (jennifer)
**          Created new for 5.0.
**      02-apr-86 (jennifer)
**          Modified the system error returned from a i4 to
**          the type CL_ERR_DESC per CL request.
*/
STATUS
SRread(
SR_IO	        *f,
i4         page,
char            *buf,
CL_ERR_DESC	*err_code)
{
    int           s;                    /* Request return status. */
    IOSB	 local_iosb;           /* Operation return status. */ 
    i4		 sid;

    /* Check file control block pointer, return if bad.*/

    if (f->io_type == SR_IO_ASCII_ID) 
    {
	/*  Queue the read request. */

	CSget_sid(&sid);
	s = sys$qio(CACHE_IO_EF, f->io_channel, 
                 IO$_READVBLK, &local_iosb, CSresume, sid,
                 buf, f->io_block_size, (page << f->io_log2_blk) + 1, 0, 0, 0
                );

	if (s & 1)
	{
	    /*	Successfully queued, wait for completion. */

	    CSsuspend(CS_DIO_MASK, 0, 0);

	    /*	Check completion status. */

	    if (local_iosb.iosb$w_status & 1)
		return (OK);
	    s = local_iosb.iosb$w_status;
	}

	/*  Return error. */

	err_code->error = s;
	return (SR_BADREAD);
    }

    err_code->error = OK;    
    return (SR_BADFILE);
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
**	    SR_EXCEED_LIMIT	Exceeding disk quota or exceeding available
**				disk space.
**    Exceptions:
**        none
**
** Side Effects:
**        none
**
** History:
**    11-sep-85 (jennifer)
**          Created new for 5.0.
**      02-apr-86 (jennifer)
**          Modified the system error returned from a i4 to
**          the type CL_ERR_DESC per CL request.
**	27-jul-87 (rogerk)
**	    Added return code SR_EXCEED_LIMIT.
*/
STATUS
SRwrite(
SR_IO	        *f,
i4         page,
char            *buf,
CL_ERR_DESC	*err_code)
{
    int           s;                    /* Request return status. */
    IOSB	 local_iosb;           /* Operation return status. */ 
    i4		 sid;

    /* Check file control block pointer, return if bad.*/

    if (f->io_type == SR_IO_ASCII_ID) 
    {
	CSget_sid(&sid);

	if (f->io_allocated < ((page << f->io_log2_blk) + f->io_blks))
	{
	    s = iiSRallocate(f, 1, sid, err_code);
	    if (s != OK)
		return (s);
	}

	s = sys$qio(CACHE_IO_EF, f->io_channel, 
                     IO$_WRITEVBLK, &local_iosb, CSresume, sid,
                     buf, f->io_block_size, (page << f->io_log2_blk) + 1, 0, 0, 0
                    );

	if (s & 1)
	{
	    /*	Successfully queued, wait for completion. */

	    CSsuspend(CS_DIO_MASK, 0, 0);

	    /*	Check completion status. */

	    if (local_iosb.iosb$w_status & 1)
		return (OK);
	    s = local_iosb.iosb$w_status;
	}

	/*  Return error. */

	err_code->error = s;
	return (SR_BADWRITE);
    }

    err_code->error = OK;    
    return (SR_BADFILE);
}

/*
** Name: iiSRallocate - allocates a page to a direct access file.
**
** Description:
**      This routine is used to add pages to a direct
**      access file.  This routines can add more than one page
**      at a time by accepting a count of the number of pages to add.
**      The number of pages is converted to the number of blocks
**      (sectors) to add. If the amount requested cannot 
**      be obtained in large contiguous areas, this routine tries to find
**      as many smaller fragments needed to meet the request.
**   
** Inputs:
**      f                Pointer to the SR file
**                       context needed to do I/O.
**      n                The number of pages to allocate.
**
** Outputs:
**      err_code         Pointer to a variable used
**                       to return operating system 
**                       errors.
**    Returns:
**        OK
**        SR_BADEXTEND
**        SR_BADFILE
**	  SR_EXCEED_LIMIT
**    Exceptions:
**        none
**
** Side Effects:
**        none
**
** History:
**    30-sep-85 (jennifer)
**          Created new for 5.0.
**      02-apr-86 (jennifer)
**          Modified the system error returned from a i4 to
**          the type CL_ERR_DESC per CL request.
**	09-mar-1987 (Derek)
**	    Changed extend options to include ALCON so that we always allocate
**	    the largest extent less than or equal to the extend size.
**	27-jul-87 (rogerk)
**	    Added return code SR_EXCEED_LIMIT.
*/
static	STATUS
iiSRallocate(
SR_IO		*f,
i4		n,
i4		sid,
CL_ERR_DESC	*err_code)
{
    int            blocks;
    i4	          s;                    /* Request return status. */
    IOSB	  local_iosb;           /* Operation return status. */
    i4            allocated;    
    i4            amount;               /* Amount allocated each
                                        ** request. */
    i4		  extend_count;
    FIBDEF	  *fib;

    /* Convert allocation size from pages to blocks. */

    blocks = n * f->io_blks;             
    allocated = f->io_allocated;

    /* 
    ** Set file I/O control block extention parameters 
    ** for the allocation I/O request.
    */

    fib = (FIBDEF *)f->io_fib;
    fib->fib$l_acctl = 0;
    fib->fib$w_exctl = FIB$M_EXTEND | FIB$M_ALCONB | FIB$M_ALDEF | FIB$M_ALCON;
    extend_count = 0;

    /*
    ** Repeat request for extensions until the number of blocks
    ** requested by the caller is allocated or until an error
    ** is returned indicating no more space.
    */

    while (blocks > 0)
    {
	fib->fib$l_exsz = SR_MIN_INCR;
	if (blocks > SR_MIN_INCR)
	    fib->fib$l_exsz = blocks;
	fib->fib$l_exvbn = 0;
        s = sys$qio(CACHE_IO_EF, f->io_channel, IO$_MODIFY, 
                     &local_iosb, CSresume, sid,
                     &f->io_fib_desc, 0, 0, 0, 0, 0
                    );

	if (s & 1)
	{
	    CSsuspend(CS_DIO_MASK, 0, 0);
	    if (local_iosb.iosb$w_status & 1)
	    {
		++extend_count;
		amount = local_iosb.iosb$l_dev_depend;
		allocated += amount;
		blocks -= amount;
		if (blocks < (1 << extend_count))
		    fib->fib$w_exctl &= ~FIB$M_ALCON;
		continue;
	    }

	    /*	Request failed. */

	    s = local_iosb.iosb$w_status;
	}

	err_code->error = s;
	if ((s == SS$_EXDISKQUOTA) || (s == SS$_DEVICEFULL) ||
	    (s == SS$_EXQUOTA))
	    return (SR_EXCEED_LIMIT);
	return (SR_BADEXTEND);
    }

    /* Reset file I/O control block extension request values. */

    f->io_allocated = allocated;
    return (OK);
}
