/*
**Copyright (c) 2004 Ingres Corporation
*/

/**
** Name: QEFCOPY.H - copy control blocks
**
** Description:
**	Description of control block used to sequence the COPY operation.
**	This block is filled in by PSF and then used by the Sequencer to
**	control the copy operation and sent to QEF to retrieve or append
**	rows. Described here is the mail QEU_COPY block as well as the
**	QEU_CPFILED which describes a copy file, and the QEU_CPDOMD
**	struct, which describes a copy file domain.
**
** History: $Log-for RCS$
**	05-dec-86 (rogerk)
**	    Moved from QEFQEU.H
**	06-apr-87 (rogerk)
**	    Added fields for new error handling features.
**	29-apr-87 (puree)
**	    Add fields for COPY WITH NULL in QEU_CPDOMD.
**	20-sep-89 (jrb)
**	    Add fields for precision during COPY for DECIMAL project.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      22-oct-2002 (horda03) Bug 77895
**          Added QEU_MISSING_COL typedef to store information on
**          column default values for columns not specified in a
**          COPY FROM command. The list of missing column default
**          values has been added to the QEU_COPY structure.
**      18-jun-2004 (stial01)
**          Added mc_dv, mc_dtbits to QEU_MISSING_COL (b112521)
**	13-Jun-2006 (kschendel)
**	    Added sequence poop to QEU_MISSING_COL.
**/

/*
**  Forward and/or External typedef/struct references.
*/

typedef struct _QEU_CPFILED	QEU_CPFILED;
typedef struct _QEU_CPDOMD	QEU_CPDOMD;
typedef struct _QEU_CPATTINFO	QEU_CPATTINFO;
typedef struct _QEU_MISSING_COL QEU_MISSING_COL;

/*
**  Defines of other constants.
*/

#define	QEU_SYNC_BLOCKS		20	/* default sync_frequency during copy */


/*}
** Name: QEU_CPDOMD - copy file domain descriptor
**
** Description:
**
** History:
**	27-may-86 (daved)
**          written
**	29-apr-87 (puree)
**	    Add fields for COPY WITH NULL.
**	20-sep-89 (jrb)
**	    Add fields for precision for DECIMAL project.
**	30-jan-1992 (bonobo)
**	    Removed the redundant typedefs to quiesce the ANSI C 
**	    compiler warnings.
*/
struct _QEU_CPDOMD
{
    QEU_CPDOMD	*cp_next;		/* next domain descriptor */
    char	cp_domname[DB_MAXNAME]; /* domain name */
    DB_DT_ID	cp_type;		/* domain type */
    i4	cp_length;		/* domain length */
    i4	cp_prec;		/* domain precision */
    bool	cp_user_delim;		/* TRUE if there is a delimiter */
    char	cp_delimiter[CPY_MAX_DELIM];	/* delimiter char */
    i4	cp_tupmap;		/* tuple attribute corresponding to
					** this file row domain
					*/
    ADI_FI_ID	cp_cvid;		/* conversion id to map row to tuple */
    i4	cp_cvlen;		/* conversion result length */
    i4	cp_cvprec;		/* conversion result precision */
    bool	cp_withnull;		/* TRUE if "with null" specified */
    i4	cp_nullen;		/* length of the value for null */
    i4	cp_nulprec;		/* precision of the value for null */
    PTR		cp_nuldata;		/* ptr to the value for null */
    /* The data in cp_nulldbv below is only used by the backend grammar and 
       is never sent to the front end */
    DB_DATA_VALUE cp_nulldbv;		/* raw value for null */
};

/*}
** Name: QEU_CPATTINFO - useful attribute information
**
** Description: 
**	This structure contains information about attributes that need
**	special QEF processing in COPY, typically because their 
**	formatting has to be handled in the DBMS, e.g export conversion or
**	defaults.
**
** History:
**	14-jul-93 (robf)
**	    Created
*/
struct _QEU_CPATTINFO 
{
    QEU_CPATTINFO *cp_next;		/* next attribute descriptor */
    DB_DT_ID	cp_type;		/* attribute type */
    i4	cp_length;		/* attribute length */
    i4	cp_prec;		/* attribute precision */
    i4	cp_tup_offset;		/* attribute offset in DMF tuple */
    i4	cp_ext_offset;		/* attribute offset in external tuple */
    i4	cp_attseq;		/* attribute sequence */
    i4	cp_flags;		/* special flags */
#define QEU_CPATT_EXP_INP      0x01       /* export/inport processing needed */
} ;
	

/*}
** Name: QEU_CPFILED - copy file descriptor
**
** Description:
**
** History:
**     27-may-86 (daved)
**          written
*/
struct _QEU_CPFILED
{
    i4	cp_filedoms;		/* num of attributes in file row */
    char	*cp_filename;		/* file name */
    QEU_CPDOMD	*cp_fdom_desc;		/* points to list of domain descs */
    QEU_CPDOMD	*cp_cur_fdd;		/* currently used domain desc */
};

/*
** Name: QEU_MISSING_COL - Missing Column information.
**
** Description :
**
**      Information about default values for columns
**      not specified in a COPY FROM command.
**
**	mc_dv's db_data pointer will point at the actual data item
**	to be supplied as a default.  Space for the data item is
**	allocated from the same stream as the QEU_MISSING_COL structure;
**	we can get away with that because COPY doesn't really generate
**	query plans, there is no chance of sharing these things.
**	The data item is bit-copied into the row in the proper place.
**
**	If the default is a constant, the parser can set up the value
**	at parse time.  For non-constants, such as sequences, the
**	runtime has to set up the value.  For sequence defaults
**	specifically, the new sequence values are fetched at the
**	start of each row, and coerced into the missing-value data
**	item for the column.  The mc_seqdv member contains the sequence
**	datatype and pointer to the native sequence value for coercion.
**
** History :
**      21-Oct-2002 (horda03)
**         Created.
**	13-Jul-2006 (kschendel)
**	    Add sequence default support; remove redundant mc_data.
*/
struct _QEU_MISSING_COL
{
    QEU_MISSING_COL  *mc_next;
    i4               mc_tup_offset;
    i4               mc_attseq;
    DB_DATA_VALUE    mc_dv;		/* Column datatype, default value */
    i4		     mc_dtbits;
    DB_DATA_VALUE    mc_seqdv;		/* Only if sequence default */
};

/*}
** Name: QEU_COPY - copy control block
**
** Description:
**      This control block contains the information necessary to perform
** the copy operation.
**
** History:
**     27-may-86 (daved)
**          written
**     06-apr-87 (rogerk)
**	    added qeu_logname and qeu_maxerrs for new error handling features.
**     14-sep-87 (rogerk)
**          added qeu_load field which signifies to use load rather than puts.
**     14-jul-93 (robf)
**	    added qeu_ext_length to indicate how big the external tuple size is
**	25-Apr-2004 (schka24)
**	    Replace QEU_CB with pointer to copy control info.
*/
struct _QEU_COPY
{
    i4		qeu_stat;           /* input specification status */
    i4		qeu_direction;      /* copy into or from */
    DB_TAB_ID		qeu_tab_id;         /* table identification */
    DB_TAB_TIMESTAMP	qeu_timestamp;	    /* timestamp of relation */

    /* qeu_count is running total of whole tuples successfully copied.
    ** maintained by SCF for copy-into and QEF for copy-from.
    */
    i4		qeu_count;          /* number of tuples */
    i4		qeu_duptup;         /* number of duplicate tuples */
    i4		qeu_tup_length;	    /* width of tuples being copied */
    i4		qeu_ext_length;	    /* external width of tuples */
    i4		qeu_maxerrs;	    /* num errs till have to end copy */
    i4		qeu_uputerr[5];     /* array of errors to send to FE */
					    /*  temporary tilGCF */
    i4			qeu_load;	    /* Use load to append tuples */
    i4			qeu_sync_frequency; /* how often FE and DBMS sync up */
    i4			qeu_next_sync;      /* how far till next sync up */
    i4			qeu_rowspace;       /* max tuples per comm block */
    i4			qeu_cur_tups;	    /* number of tuples in this block */
    i4			qeu_moving_data;    /* middle of sending io block */
    i4			qeu_partial_tup;    /* tuple split between com blocks */
    i4			qeu_error;	    /* error encountered in COPY */
    PTR			*qeu_tdesc;         /* description of copy table */
    QEU_CPFILED		*qeu_fdesc;         /* description of copy file */
    char		*qeu_logname;	    /* error log file - NULL if none */
    QEF_DATA		*qeu_input;         /* input tuples */
    QEF_DATA		*qeu_output;        /* output tuples */
    PTR			qeu_sbuf;	    /* storage for unfinished tuple */
    i4		qeu_ssize;	    /* size of storage */
    i4		qeu_sused;	    /* amount of storage used */
    char		*qeu_sptr;	    /* pointer into storage */
    QEU_CPATTINFO	*qeu_att_info;	    /* pointer to useful att info*/
    PTR			qeu_tmptuple;	    /* temporary workspace */
    QEU_MISSING_COL     *qeu_missing;       /* pointer to missing column defaults */
    struct _DMR_CB	*qeu_dmrcb;	    /* Pointer to parser created DMR
					    ** CB to point to optional DMU-
					    ** CHAR_ARRAY entries. Probably
					    ** should just be a DM_DATA.
					    */

    struct _DMS_CB	*qeu_dmscb;	    /* Pointer to parser created DMS
					    ** CB for sequence defaults.  DMSCB
					    ** is filled out with sequence
					    ** entries and ptrs to data slots.
					    ** NULL if no sequence defaults.
					    */

    /* For COPY FROM into a partitioned table, we need the partition
    ** definition.  The parse makes a copy and sticks the address here.
    */
    DB_PART_DEF		*qeu_part_def;	    /* Address of partition def */

    /* This copy control info pointed to here is defined locally
    ** in qefcopy.c;  nobody outside cares what's in here.
    */
    struct _QEU_COPY_CTL *qeu_copy_ctl;	    /* Pointer to COPY controller */
};
