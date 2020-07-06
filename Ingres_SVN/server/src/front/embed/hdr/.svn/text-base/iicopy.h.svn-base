/*
** Copyright (c) 1986, 2004 Ingres Corporation
*/

/*
**  Name: iicopy.h - defines structures used by libq to execute COPY command.
**
**  Description:
**	This file describes the structures that hold information needed
**	to execute the COPY command. 
**
**  History:
**	01-dec-1986	- Written (rogerk)
**	26-oct-87 (puree)
**	    change CPY_VCHLEN from 4 to 5 to support longer varchar.
**	29-mar-1991 (barbara)
**	    Added II_CP_USR struct to keep track of variable length rows
**	    copied from program.
**	06-aug-1991 (teresal)
**	    Modify II_CP_MAP to save the type of the 'with null' constant.
**	18-Dec-97 (gordy)
**	    Added support for multi-threaded applications.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	26-jan-2004 (somsa01)
**	    Updated prototypes of BLOB functions to account for
**	    "COPY INTO/FROM... PROGRAM" syntax.
**      24-jan-2005 (stial01)
**          Added cp_dbgname, cp_dbg_ptr for COPY tracing. (b113792)
*/

/* varchar length specifier as 5-byte ascii value */

# define		CPY_VCHLEN	5

/*}
+*  Name: II_CP_MAP - 
**
**  Description:
-*
**  History:
**	01-dec-1986	- Written (rogerk)
**	24-feb-88 (puree)
**	    rearrange for word alignment.  Added 2 bytes to cp_name
**	    for the null terminator.
**	06-aug-1991 (teresal)
**	    Add a data type for the 'with null' constant value.
**	    A new COPY map being sent from the server now
**	    sends the null value in a DBV form.  For now,
**	    cp_nultype is the same as cp_rowtype.
**	23-apr-1996 (chech02)
**	    Add function prototypes for windows 3.1 port.
**	16-aug-1996 (thoda04)
**	    Corrected function prototypes for windows 3.1 port.
**	24-Jun-2005 (schka24)
**	    Added unicode normalization work buffer.
*/

typedef struct {
    PTR		cp_normbuf;		/* Unicode normalization work buffer */
    PTR		cp_srcbuf;		/* Unicode copy column data */
    PTR		cp_nuldata;		/* ptr to constant value for null */
    i4	cp_rowlen;		/* domain length as specified in 
					** the COPY command. ADE_LEN_UNKNOWN
					** if variable length domain */
    i4	cp_fieldlen;		/* actual field length in the copy
					** file. */
    i4	cp_tuplen;		/* corresponding attribute length */
    i4	cp_attrmap;		/* corresponding attribute number */
    i4	cp_attroffset;		/* offset in tuple to attribute */
    DB_DT_ID	cp_rowtype;		/* domain type */
    i2		cp_rowprec;		/* domain decimal precision */
    DB_DT_ID	cp_tuptype;		/* corresponding attribute type */
    i2		cp_tupprec;		/* corresponding attribute precision */
    ADI_FI_ID	cp_cvid;		/* adf function id for conversion */
    i4		cp_cvlen;		/* length of result of function id */
    i2		cp_cvprec;		/* precision of result of function id */
    bool	cp_withnull;		/* TRUE if "with null" specified */
    DB_DT_ID	cp_nultype;		/* type of constant value for null */
    i4		cp_nullen;		/* length of constant value for null */
    i2		cp_nulprec;		/* precis'n of constant val for null */
    i4		cp_valchk;		/* do we need value checking */
    i4		cp_isdelim;		/* is there a delimiter */
    i4		cp_l_delim;		/* length of delim character */
    ADF_FN_BLK	cp_adf_fnblk;		/* ADF function block set up for
					** row/tuple conversion calls */
    char	cp_name[GCA_MAXNAME+2];	/* domain name */
    char	cp_delim[CPY_MAX_DELIM];/* domain delimiter */
} II_CP_MAP;

/*}
+*  Name: II_CP_REC - Record buffer for COPY FROM
**
**  Description:
**	This structure manages a row at a time from a user function in
**	COPY FROM when the row format is variable length.  A row from a
**	user program is read into cp_recbuf and then can be handed out in
**	chunks as by requested.  This could be used to to handle a record
**	at a time from a file.
**	
**  History:
**	29-mar-1991 (barbara)
**	    Created for COPY FROM PROGRAM 
*/
typedef struct {
    char	*u_rowp;
    i4		u_rowlen;
    char	u_rowbuf[DB_GW3_MAXTUP];
} II_CP_USR;
    
/*}
+*  Name: II_CP_STRUCT
**
**  Description:
-*
**  History:
**	01-dec-1986	- Written (rogerk)
**	24-feb-88 (puree)
**	    rearrange for better word alignment.
**	29-mar-1991 (barbara)
**	    Added II_CP_USR struct to keep track of variable length rows
**	    copied from program.
*/

typedef struct {
    i4	cp_status;		/* status bits - defined in COPY.H */
    i4	cp_direction;		/* CPY_FROM or CPY_INTO */
    i4	cp_row_length;		/* width of copy row - 0 if variable */
    i4	cp_tup_length;		/* width of tuple in copy table */
    i4	cp_num_domains;		/* number of domains in copy file */
    i4	cp_rowcount;		/* count of rows processed */
    i4	cp_truncated;		/* count of truncated domains */
    i4	cp_blanked;		/* domains with control chars blanked */
    i4	cp_warnings;		/* count of warnings given */
    i4	cp_badrows;		/* rows that couldn't be processed */
    i4	cp_logged;		/* count of rows written to log file */
    i4	cp_maxerrs;		/* errors allowed till terminate copy */
    i4	cp_rbuf_size;		/* size of cp_rowbuf below */
    char	*cp_filename;		/* copy file name */
    FILE	*cp_file_ptr;		/* copy file descriptor */
    char	*cp_logname;		/* error row log name */
    FILE	*cp_log_ptr;		/* error row log descriptor */
    char	*cp_dbgname;		/* debug log name */
    FILE	*cp_dbg_ptr;		/* debug file descriptor */
    II_CP_MAP	*cp_copy_map;		/* file domain descriptors */
    PTR		cp_rowbuf;		/* buffer for copy rows */
    PTR		cp_tupbuf;		/* buffer for tuples */
    PTR		cp_zerotup;		/* initialized tuple with no data */
    PTR		cp_buffer;		/* temporary buffer */
    i4		cp_sync_frequency;	/* how often send sync blocks */
    i4		cp_convert;		/* is row/tuple conversion needed */
    i4		cp_format;		/* is format routine needed */
    i4		cp_tupcopy;		/* is separate copy of tuple needed */
    i4		cp_varrows;		/* does file have var length rows */
    i4		cp_has_blobs;		/* are BLOB columns presetn */
    i4		cp_not_flushed;		/* is output buffer flushed */
    i4		cp_error;		/* is there a fatal error */
    i4		cp_dbmserr;		/* is there an error on the dbms side */
    i4		cp_readerr;		/* was there error reading from dbms */
    i4		cp_continue;		/* should we continue on error */
    i4		cp_rowlog;		/* is there a log for bad rows */
    i4		cp_program;		/* is this an in-memory copy */
    i4		cp_filetype;		/* user-specified filetype */
    II_CP_USR	cp_usrrow;		/* record buffer for COPY FROM */
} II_CP_STRUCT;


/*}
+*  Name: II_CPY_BUFH
**
**  Description:    This structure is used for keeping track of GCA buffer
**		    used for sending tuples to the DBMS.
-*
**  History:
**	29-sep-87 (puree)
**	    writtetn for GCA architecture
*/

typedef struct {
    i4		cpy_bytesused;
    DB_DATA_VALUE	cpy_dbv;
} II_CPY_BUFH;			/* tuple buffer header */

FUNC_EXTERN  i4	IIcpinit( II_LBQ_CB *, 
				  II_CP_STRUCT *, II_CPY_BUFH *, i4  );
FUNC_EXTERN  i4	IIcprowformat( II_LBQ_CB *, II_CP_STRUCT *, PTR, PTR );
FUNC_EXTERN  i4	IIcpfrom_init( II_LBQ_CB *, II_CP_STRUCT *, 
				       II_CPY_BUFH *, PTR *, PTR * );
FUNC_EXTERN  i4	IIcptupformat( II_THR_CB *, II_CP_STRUCT *, PTR, PTR );
FUNC_EXTERN  i4	IIcpdbwrite(II_LBQ_CB *, II_CP_STRUCT *, II_CPY_BUFH *);
FUNC_EXTERN  i4	IIcpdbread( II_LBQ_CB *, 
				    II_CP_STRUCT *, II_CPY_BUFH *, PTR * );
FUNC_EXTERN  VOID	IIcpendcopy(II_LBQ_CB *, II_CP_STRUCT *, II_CPY_BUFH *);
FUNC_EXTERN  i4	IIcpdbflush(II_LBQ_CB *, II_CP_STRUCT *, II_CPY_BUFH *);
FUNC_EXTERN  VOID	IIcpabort( II_LBQ_CB *, II_CP_STRUCT * );
FUNC_EXTERN  VOID	IIcpdbgdump( II_LBQ_CB *, II_CP_STRUCT *, char * );
FUNC_EXTERN  i4	IICPbrBlobRead( II_LBQ_CB *, 
				II_CP_STRUCT *, II_CPY_BUFH *, i4 (*)() );
FUNC_EXTERN  i4	IICPbbrBulkBlobRead( II_LBQ_CB *, 
				     II_CP_STRUCT *, II_CPY_BUFH *, i4 (*)() );
FUNC_EXTERN  i4	IICPfbrFmtBlobRead( II_LBQ_CB *, 
				    II_CP_STRUCT *, II_CPY_BUFH *, i4 (*)() );
FUNC_EXTERN  i4	IICPbwBlobWrite( II_THR_CB *, 
				 II_CP_STRUCT *, II_CPY_BUFH *, i4 (*)() );
FUNC_EXTERN  i4	IICPbbwBulkBlobWrite( II_THR_CB *, 
				      II_CP_STRUCT *, II_CPY_BUFH *, i4 (*)() );
FUNC_EXTERN  i4	IICPfbwFmtBlobWrite( II_THR_CB *, 
				     II_CP_STRUCT *, II_CPY_BUFH *, i4 (*)() );
FUNC_EXTERN  i4	IICPcfgCheckForGaps( II_LBQ_CB *, II_CP_STRUCT *, 
					     II_CP_MAP *, char **, char **, 
					     II_CPY_BUFH *, i4 *, bool );
FUNC_EXTERN  STATUS	IICPvcValCheck( II_LBQ_CB *, char *, 
					II_CP_MAP *, II_CP_STRUCT * );
FUNC_EXTERN  i4	IICPbdrBlobDbRead( II_LBQ_CB *, II_CP_STRUCT *, 
					   II_CPY_BUFH *, PTR *, 
					   II_CP_MAP *, i4 * );

#ifdef WIN16
/* function prototypes */
FUNC_EXTERN i4     IIcopy(i4 msg_type, i4  (*user_routine)());
FUNC_EXTERN i4     IIcpinto_init(II_CP_STRUCT *copy_blk, II_CPY_BUFH *cpy_bufh,
                                 char **row, char *tuple);
FUNC_EXTERN i4     IIcp_isnull(II_CP_MAP *cmap, char *row_ptr);
FUNC_EXTERN i4     IIcpfopen(II_CP_STRUCT *copy_blk);
FUNC_EXTERN i4     IIcpgetrow(II_CP_STRUCT *copy_blk, PTR *row, i4  (*get_row)());
FUNC_EXTERN STATUS IICPgcGetCol(II_CP_STRUCT *copy_blk, char **row_ptr, 
                                char **end_ptr, i4  i, PTR *row,
				i4 (*get_row)());
FUNC_EXTERN i4     IIcpputrow(II_CP_STRUCT *copy_blk, PTR row, i4 (*put_row)());
FUNC_EXTERN i4     IIcpfclose(II_CP_STRUCT *copy_blk); 
FUNC_EXTERN STATUS IIcpgetch(II_CP_STRUCT *copy_blk, char *ch_ptr, 
                             bool *is_double, i4  i, bool start_domain,
			     i4 (*get_row)());
FUNC_EXTERN STATUS IIcpgetbytes(II_CP_STRUCT *copy_blk, char *bytes_ptr,
                                i4  num_req, i4  i, bool start_domain,
				i4 (*get_row)());
FUNC_EXTERN void   IIcpcleanup(II_CP_STRUCT *copy_blk);
FUNC_EXTERN i4     IIcplgopen(II_CP_STRUCT *copy_blk);
FUNC_EXTERN void   IIcplgrow(II_CP_STRUCT *copy_blk, char *row);
FUNC_EXTERN i4     IIcplgclose(II_CP_STRUCT	*copy_blk);
FUNC_EXTERN STATUS IIcp_handler(/* EX_ARGS	*exargs */);
FUNC_EXTERN void   IICPbbiBulkBlobInto(/* LOBULKINTO_HDL *bbi */);
FUNC_EXTERN void   IICPfbiFmtBlobInto(/* LOBULKINTO_HDL *fbi */);
FUNC_EXTERN void   IICPwemWriteErrorMsg(i4 ernum, i4 rownum, 
    	                                char *col_name, char *msg);
FUNC_EXTERN STATUS IICPbcfBulkColFlush(char **col_begin, char **col_end, 
                                       bool eot, IICGC_DESC  *cgc, 
                                       II_CP_STRUCT *copy_blk, II_CPY_BUFH *cpy_bufh);
FUNC_EXTERN i4 IICPfslFmtSegLen(FILE *fptr, i4 rowcount, char *colname,
    	                             i2 *seglen, i4  i);
FUNC_EXTERN void   IICPbbfBulkBlobFrom(/* LOBULKFROM_HDL *bbf */);
FUNC_EXTERN void   IICPfbfFmtBlobFrom(/* LOBULKFROM_HDL *fbf */);
FUNC_EXTERN void   IICPfbFlushBlob(void);
FUNC_EXTERN void   IICPsebSendEmptyBlob(void);
#endif /* WIN16 */
