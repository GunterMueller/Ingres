/*
**Copyright (c) 2004 Ingres Corporation
*/

/**
**
** Name:  DMHCB.H   Typedefs for DMF access to hash partition files.
**
** Description:
**
**      This file contains the control block used to pass hash partition file
**	operations from QEF to DMF where they are executed. It also contains
**	prototype definitions for the DMF functions which perform the 
**	operations.
**    
** History:
** 
**      9-apr-99 (inkdo01)
**          Written.
**      08-Oct-2002 (hanch04)
**          Replace the i4 for length with a SIZE_TYPE (size_t) to allow
**          64-bit memory allocation.
**      05-Aug-2005 (hanje04)
**          Back out change 478041 as it should never have been X-integrated
**          into main in the first place.
**/


/*}
** Name: DMH_CB - DMF hash partition file operation control block.
**
** Description:
**      This typedef defines the control block required for
**      the following DMF hash partition file operations:
**
**      DMH_OPEN - Open/create a file.
**      DMH_CLOSE - Close/destroy a file.
**	DMH_WRITE - Write a block to a file.
**	DMH_READ - Read a block from a file.
**
** History:
**      9-apr-99 (inkdo01)
**          Written.
**	6-Jun-2005 (schka24)
**	    Embed the filename within the DMHCB so that parallel query
**	    child threads don't have to allocate their own filename areas.
**	6-Jul-2006 (kschendel)
**	    Comment update.
*/
typedef struct _DMH_CB
{
    PTR             q_next;
    PTR             q_prev;
    SIZE_TYPE       length;                 /* Length of control block. */
    i2		    type;                   /* Control block type. */
#define                 DMH_PFILE_CB        14
    i2              s_reserved;
    PTR             l_reserved;
    PTR             owner;
    i4	            ascii_id;             
#define                 DMH_ASCII_ID        CV_C_CONST_MACRO('#', 'D', 'M', 'H')
    DB_ERROR	    error;                  /* Common DMF error block. */
    PTR             dmh_file_context;	    /* ptr to file context struct */
    PTR 	    dmh_buffer;		    /* ptr to read/write buffer */
    PTR		    dmh_db_id;		    /* DMF database ID */
					/* Really an ODCB pointer, so it's the
					** same for all threads of a session.
					*/
    PTR		    dmh_tran_id;	    /* ptr to transaction ID */
    char	    dmh_filename[12];	    /* filename.ext, always 8.3 */
    i4		    *dmh_locidxp;	    /* ptr to location array index
					    ** for target file */
    i4		    dmh_numlocs;	    /* # of avail work locations */
    i4		    dmh_blksize;	    /* size (in bytes) of block to
					    ** read/write */
    i4		    dmh_rbn;		    /* RBN of block being processed */
    i4		    dmh_func;		    /* requested operation */
#define DMH_OPEN	1
#define DMH_CLOSE	2
#define DMH_READ	3
#define DMH_WRITE	4
    i4		    dmh_flags_mask;	    /* modifier to operation */
#define DMH_CREATE	0x01
#define	DMH_DESTROY	0x02
}	DMH_CB;


/*}
**
** External function prototypes for hash partition file operation functions.
**
*/

FUNC_EXTERN DB_STATUS
dmh_open(
	DMH_CB	*dmhcb);

FUNC_EXTERN DB_STATUS
dmh_close(
	DMH_CB	*dmhcb);

FUNC_EXTERN DB_STATUS
dmh_read(
	DMH_CB	*dmhcb);

FUNC_EXTERN DB_STATUS
dmh_write(
	DMH_CB	*dmhcb);

