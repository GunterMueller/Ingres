/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <sr.h>
#include    <me.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <ddb.h>
#include    <ulf.h>
#include    <ulm.h>
#include    <adf.h>
#include    <ade.h>
#include    <scf.h>
#include    <dmf.h>
#include    <dmhcb.h>
#include    <dmrcb.h>
#include    <dmtcb.h>
#include    <qefmain.h>
#include    <qsf.h>
#include    <qefrcb.h>
#include    <qefcb.h>
#include    <qefnode.h>
#include    <psfparse.h>
#include    <qefact.h>
#include    <qefqp.h>
#include    <qefdsh.h>
#include    <qefscb.h>
#include    <qefqeu.h>

#include    <dmmcb.h>
#include    <dmucb.h>
#include    <ex.h>
#include    <tm.h>

#include    <dudbms.h>
#include    <qeuqcb.h>
#include    <rqf.h>
#include    <tpf.h>
#include    <qefkon.h>
#include    <qefqry.h>
#include    <qefcat.h>
#include    <sxf.h>
#include    <qefprotos.h>

/**
**
**  Name: QEAHAGGF.C - return aggregate function row (using hash
**	aggregation)
**
**  Description:
**      This action returns the aggregate result tuple instead of
**  appending it to a file. It also does the aggregation using hash
**  techniques, rather than requiring the source data to be presorted 
** on the grouping expressions
**
**          qea_haggf - return aggregate function row (hash aggregation)
**
**
**  History:    $Log-for RCS$
**      26-jan-01 (inkdo01)
**          Written (for hash aggregation project).
**	24-sep-01 (devjo01)
**	    Modify space requirements calculation to allow for 64 bit
**	    pointers. (b105879).
**	01-oct-2001 (devjo01)
**	    Rename QEN_HASH_DUP to QEF_HASH_DUP.
**	05-oct-2001 (somsa01)
**	    Found one more spot where QEN_HASH_DUP had to be renamed
**	    to QEF_HASH_DUP.
**	11-oct-2001 (devjo01)
**	    Add "static" to forward declaration of local static funcs.
**	23-feb-04 (inkdo01)
**	    Changed qef_rcb->error to dsh->dsh_error for || thread safety
**	    (and others).
**	5-May-2005 (schka24)
**	    Use sizeof(PTR) for chunk link ptr size, not align-restrict
**	    (which could conceivably be something smaller than a pointer).
**	    Hash values must be unsigned (they can be -infinity).
**	    Kill a few more incorrect u_i2 casts on MEcopy sizes.
**	7-Jun-2005 (schka24)
**	    Break out memory allocator into a routine; apply recursion
**	    level entry/exit fixes from hash join.
**	    Include inkdo01 changes to delete temp files as they complete.
**	13-Oct-2005 (hweho01)
**	    Modified the chunkoff setting, so it will be rounded up to  
**          the sizeof(ALIGN_RESTRICT), avoid seg V on HP 32-bit build.
**          Since the pointer is 4-bytes, the float type number must be  
**          aligned at 8 byte boundary.
**	28-Dec-2005 (kschendel)
**	    Use DSH references whenever possible instead of QEF RCB
**	    references.  (This simplifies the code, eliminating many
**	    RCB references, and reflects the new primacy of the DSH
**	    thanks to the parallel query changes.)
[@history_template@]...
**/


/**
**
**  Name: HSHAG_STATE - local state structure to reduce parameter passing
**
**  Description:
**      This structure contains various variables local to the hash
**	aggregation functions. It is allocated in the locals of qea_haggf
**	passed to the other local functions. It reduces the parameter
**	passing overhead to these functions.
**
[@history_template@]...
**/

typedef struct _HSHAG_STATE {
    QEN_ADF	*hashadf;
    QEN_ADF	*curradf;
    QEN_ADF	*oflwadf;
    ADE_EXCB	*hashexcb;
    ADE_EXCB	*currexcb;
    ADE_EXCB	*oflwexcb;
    PTR		*hbufpp;
    PTR		*wbufpp;
    PTR		*obufpp;
    PTR		*owbufpp;
    QEF_HASH_DUP *hashbufp;
    PTR		curchunk;
    PTR		prevchunk;
    PTR		oflwbufp;
    i4		chunkoff;
    i4		chunk_count;
    i4		currgcount;
} HSHAG_STATE;

GLOBALREF QEF_S_CB *Qef_s_cb;

static DB_STATUS
qea_haggf_init(
QEE_DSH		*dsh,
QEN_HASH_AGGREGATE *haptr,
QEF_QEP		*qp);

static DB_STATUS
qea_haggf_load1(
QEF_RCB		*qef_rcb,
QEE_DSH		*dsh,
QEF_AHD		*action,
QEN_HASH_AGGREGATE *haptr,
HSHAG_STATE	*hstp,
i4		function,
bool		reset);

static DB_STATUS
qea_haggf_load2(
QEE_DSH		*dsh,
QEF_AHD		*action,
QEN_HASH_AGGREGATE *haptr,
HSHAG_STATE	*hstp);

static DB_STATUS
qea_haggf_insert(
QEE_DSH		*dsh,
QEN_HASH_AGGREGATE *haptr,
HSHAG_STATE	*hstp);

static DB_STATUS
qea_haggf_overflow(
QEE_DSH		*dsh,
QEN_HASH_AGGREGATE *haptr,
HSHAG_STATE	*hstp,
u_i4		hashno);

static DB_STATUS
qea_haggf_recurse(
QEE_DSH		*dsh,
QEN_HASH_AGGREGATE *haptr);

static DB_STATUS
qea_haggf_write(
QEE_DSH		*dsh,
QEN_HASH_AGGREGATE *haptr,
QEN_HASH_FILE	*hfptr,
PTR		buffer,
i4		*currbn);

static DB_STATUS
qea_haggf_read(
QEN_HASH_AGGREGATE *haptr,
QEN_HASH_FILE	*hfptr,
PTR		buffer,
i4		*currbn);

static DB_STATUS
qea_haggf_open(
QEE_DSH		*dsh,
QEN_HASH_AGGREGATE *haptr,
QEN_HASH_PART	*hpptr,
i4		flag);
/* qea_haggf_open flag settings */
#define OPEN_READ	1
#define OPEN_WRITE	2
 
static DB_STATUS
qea_haggf_close(
QEN_HASH_AGGREGATE *haptr,
QEN_HASH_FILE	**hfptr,
i4		flag);
/* qea_haggf_close flag settings */
#define CLOSE_RELEASE	1	
#define CLOSE_DESTROY	2


static DB_STATUS
qea_haggf_flush(
QEE_DSH		*dsh,
QEN_HASH_AGGREGATE *haptr);

static DB_STATUS qea_haggf_palloc(
	QEF_CB *qef_cb,
	QEN_HASH_AGGREGATE *haptr,
	SIZE_TYPE psize,
	void *pptr,
	DB_ERROR *dsh_error,
	char *what);
 

/*{
** Name: QEA_HAGGF	- compute aggregate function
**
** Description:
**      The following algorithm is used to produce the aggregate result
**  tuple. It does so on unordered input and produces no-project aggregates.
**
**	1) Allocate memory for hash table and structures.
**	2) Read each tuple in turn from the source QEP.
**	3) Hash the by expressions and search the hash table for a matching
**	    row.
**	4) If a match is found, accumulate the aggregate result into it, else
**	    init new aggregate row and add to hash table.
**	4.5) If rows with distinct group expression values exceed storage
**	    available, remaining non-matching rows are partitioned into
**	    buffers that may eventually be written to disk.
**	5) When input rows are done, read rows out of hash table, performing
**	    final aggregation on each and returning to caller.
**	5.5) If some rows were written to disk, hash table is reinitialized
**	    and overflow rows are read from disk and loaded into hash table. 
**	    Steps 3 thru 5 are repeated for overflow rows, with the 
**	    possibility that enough overflow rows may even cause recursive
**	    partitioning.
**
** Inputs:
**      action                          the QEA_HAGGF action
**	qef_rcb
**	reset				The reset flag if result has to be 
**					re-initialized
**
** Outputs:
**      dsh
**	    .error.err_code		one of the following
**				E_QE0000_OK
**				E_QE000D_NO_MEMORY_LEFT
**				E_QE0017_BAD_CB
**				E_QE0018_BAD_PARAM_IN_CB
**				E_QE0019_NON_INTERNAL_FAIL
**				E_QE0002_INTERNAL_ERROR
**				E_QE0015_NO_MORE_ROWS
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      26-jan-01 (inkdo01)
**          Written (for hash aggregation project).
**	23-feb-01 (inkdo01)
**	    Add overflow support.
**	2-oct-01 (inkdo01)
**	    Changes to assure chunk link leaves chunk 8-byte aligned.
**	29-dec-03 (inkdo01)
**	    DSH is now parameter, "function" replaces "reset".
**	27-feb-04 (inkdo01)
**	    Changed dsh_hashagg from array of QEN_HASH_AGGREGATE to array
**	    of ptrs.
**	31-aug-04 (inkdo01)
**	    Changes to support global base arrays.
**	20-Sep-2004 (jenjo02)
**	    Reset hshag_dmhcb; with child threads, dsh_cbs
**	    aren't what they used to be when qee initialized them.
[@history_template@]...
*/
DB_STATUS
qea_haggf(
QEF_AHD		    *action,
QEF_RCB		    *qef_rcb,
QEE_DSH		    *dsh,
i4		    function,
bool		    first_time )
{
    QEN_NODE	*node = action->qhd_obj.qhd_qep.ahd_qep;
    DB_STATUS	status = E_DB_OK;
    ADF_CB	*adfcb = dsh->dsh_adf_cb;
    PTR		*cbs = dsh->dsh_cbs;
    HSHAG_STATE hastate;
    QEF_HASH_DUP *workbufp;
    QEN_HASH_AGGREGATE	*haptr;
    i4		i;
    bool	reset = ((function & FUNC_RESET) != 0);


    /* Init a bunch of stuff. */
    dsh->dsh_error.err_code = E_QE0000_OK;
    dsh->dsh_qef_rowcount = 0;
    hastate.curradf = action->qhd_obj.qhd_qep.ahd_current;
    hastate.hashadf = action->qhd_obj.qhd_qep.ahd_by_comp;
    hastate.oflwadf = action->qhd_obj.qhd_qep.u1.s2.ahd_agoflow;
    hastate.currexcb = (ADE_EXCB *)cbs[hastate.curradf->qen_pos];
    hastate.wbufpp = (dsh->dsh_qp_ptr->qp_status & QEQP_GLOBAL_BASEARRAY) ?
	&(dsh->dsh_row[action->qhd_obj.qhd_qep.u1.s2.ahd_agwbuf]) :
	&(hastate.currexcb->excb_bases[action->qhd_obj.qhd_qep.u1.s2.ahd_agwbuf]);
    haptr = dsh->dsh_hashagg[action->qhd_obj.qhd_qep.u1.s2.ahd_agcbix];
    haptr->hshag_dmhcb = cbs[action->qhd_obj.qhd_qep.u1.s2.ahd_agdmhcb];

    if (first_time || reset)
    {
	/* Init. hash table, etc. */
	status = qea_haggf_init(dsh, haptr, &action->qhd_obj.qhd_qep);
	if (status != E_DB_OK)
	{
	    /* the error.err_code field in dsh should already be
	    ** filled in.
	    */
	    dsh->dsh_qef_rowcount = 0;
	    return (status);
	}
    }

    /* Loop over input rows and aggregate into hash table. */
    if (haptr->hshag_flags & HASHAG_LOAD1)
    {
	hastate.curchunk = haptr->hshag_1stchunk;
	hastate.prevchunk = NULL;
	hastate.chunk_count = 0;
	hastate.chunkoff = (sizeof(PTR) + DB_ALIGN_SZ - 1) &
	                   (~(DB_ALIGN_SZ - 1));
	status = qea_haggf_load1(qef_rcb, dsh, action,
				haptr, &hastate, function, reset);
	if (status != E_DB_OK)
	    return(status);
    }

    /* Big for-loop to force continued overflow processing when hash table
    ** empties from current phase. Performs RETURN as long as there are 
    ** completed rows remaining, then calls load2 to build next set of 
    ** result rows from overflow (if any). */
    for ( ; ; )
    {
	if (haptr->hshag_flags & HASHAG_RETURN)
	{
	    /* Locate next work row to produce result from. */
	    workbufp = NULL;
	    if (haptr->hshag_lastrow == NULL ||
		(workbufp = haptr->hshag_lastrow->hash_next) == NULL)
	     for (i = ++(haptr->hshag_curr_bucket); i < haptr->hshag_htsize;
		    i = ++(haptr->hshag_curr_bucket))
	      if ((workbufp = ((QEF_HASH_DUP *)haptr->hshag_htable[i])) != NULL) 
								break;

	    if (workbufp == NULL)
	    {
		if (!(haptr->hshag_flags & HASHAG_OVFLW) ||
		    haptr->hshag_flags & HASHAG_DONE)
		{
		    /* We're all done! First close/destroy any open files,
		    ** then return "end of scan" indication. */
		    status = qea_haggf_closeall(haptr);
		    if (status != E_DB_OK)
			return(status);
		    dsh->dsh_error.err_code = E_QE0015_NO_MORE_ROWS;
		    return(E_DB_WARN);
		}

		/* (More) overflow processing yet to be done. Reset flag 
		** and hash table search vars and loop. */
		haptr->hshag_flags &= ~HASHAG_RETURN;
		haptr->hshag_curr_bucket = -1;
		haptr->hshag_lastrow = (QEF_HASH_DUP *) NULL;
		continue;
	    }
	    else haptr->hshag_lastrow = workbufp;

	    /* Initialize the aggregate result for next output row. */
	    if (hastate.curradf->qen_uoutput > -1)
	     if (dsh->dsh_qp_ptr->qp_status & QEQP_GLOBAL_BASEARRAY)
		dsh->dsh_row[hastate.curradf->qen_uoutput] =
				dsh->dsh_qef_output->dt_data;
	     else hastate.currexcb->excb_bases[ADE_ZBASE + 
		hastate.curradf->qen_uoutput] = dsh->dsh_qef_output->dt_data;
	    *(hastate.wbufpp) = (PTR)workbufp;

	    /* Locate next work row, compute final result and return. */
	    hastate.currexcb->excb_seg = ADE_SFINIT;
	    status = ade_execute_cx(adfcb, hastate.currexcb);
	    if (status != E_DB_OK)
	    {
		if ((status = qef_adf_error(&adfcb->adf_errcb, 
		    status, dsh->dsh_qefcb, &dsh->dsh_error)) != E_DB_OK)
		    return (status);
	    }
	    dsh->dsh_qef_remnull |= hastate.currexcb->excb_nullskip;
	    dsh->dsh_qef_rowcount = 1;
	    return (E_DB_OK);
	}

	if (haptr->hshag_flags & (HASHAG_LOAD2P1 | HASHAG_LOAD2P2))
	{
	    /* Loop over overflow rows and aggregate into hash table. */
	    hastate.curchunk = haptr->hshag_1stchunk;
	    hastate.prevchunk = NULL;
	    hastate.chunk_count = 0;
	    hastate.chunkoff = (sizeof(PTR) + DB_ALIGN_SZ - 1) &
			       (~(DB_ALIGN_SZ - 1));
	    status = qea_haggf_load2(dsh, action, haptr, &hastate);
	    if (status != E_DB_OK)
		return(status);

	    if (haptr->hshag_flags & HASHAG_DONE)
	    {
		/* We're all done! First close/destroy any open files,
		** then return "end of scan" indication. */
		status = qea_haggf_closeall(haptr);
		if (status != E_DB_OK)
		    return(status);
		dsh->dsh_error.err_code = E_QE0015_NO_MORE_ROWS;
		return(E_DB_WARN);
	    }
	}
    }	/* end of "for( ; ; )" */
}


/*{
** Name: QEA_HAGGF_INIT	- allocate/format hash memory 
**
** Description:
**      This function allocates (if necessary) memory for the hash table
**	and first chunk for aggregate accumulation. It then inits all hash
**	table entries to NULL in preparation for the aggregation operation.
**
** Inputs:
**	dsh
**	haptr			Ptr to dynamic hash aggregation structure
**	hbufsz			Size of hash work buffer
**	obufsz			Size of overflow work buffer
**
** Outputs:
**      dsh
**	    .error.err_code		one of the following
**				E_QE0000_OK
**				E_QE000D_NO_MEMORY_LEFT
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      26-jan-01 (inkdo01)
**          Written (for hash aggregation project).
**	24-sep-01 (devjo01)
**	    Modify space requirements calculation to allow for 64 bit
**	    pointers. (b105879).
**	2-oct-01 (inkdo01)
**	    Changes to assure chunk link leaves chunk 8-byte aligned.
**	4-oct-01 (inkdo01)
**	    Rearranged memory to avoid ugly alignment code from above.
**	22-july-03 (inkdo01)
**	    Set dmh_tran_id. Used to be set in qee.c, but that was too early
**	    and the tran ID could change before query exec (bug 110603).
**	30-Jul-2004 (jenjo02)
**	    Use dsh_dmt_id rather than qef_dmt_id transaction context.
[@history_template@]...
*/
static DB_STATUS
qea_haggf_init(
QEE_DSH		*dsh,
QEN_HASH_AGGREGATE *haptr,
QEF_QEP		*qp)

{
    QEF_CB	*qef_cb = dsh->dsh_qefcb;
    DB_STATUS	status = E_DB_OK;
    DMH_CB	*dmhcb = (DMH_CB *)haptr->hshag_dmhcb;
    i4		i;
    i4		hbufsz = haptr->hshag_hbufsz;
    i4		obufsz = haptr->hshag_obufsz;
    bool	initchunk = FALSE;

#ifdef xDEBUG
    TRdisplay("%@ hashagg init thread %d htsize %d chunksize %d chmax %d\n",
	dsh->dsh_threadno,haptr->hshag_htsize, haptr->hshag_chsize, haptr->hshag_chmax);
    TRdisplay("t%d hashagg init part buf size %d pcount %d, aggest %d\n",
	dsh->dsh_threadno,haptr->hshag_pbsize, haptr->hshag_pcount, qp->u1.s2.ahd_aggest);
#endif
    /* See if hash table needs allocation - if so, stream also needs
    ** to be opened. */
    if (haptr->hshag_htable == NULL)
    {
	/* now allocate hash table - single ULM call
	** also allocates initial chunk of aggregation memory, plus a 
	** buffer for running the hash computation and a buffer to hold
	** an overflow row (if it spans 2 blocks) and space to assure the
	** chunk linkage leaves chunks aligned on 8 byte boundaries. */
	status = qea_haggf_palloc(qef_cb, haptr,
			sizeof(PTR) * haptr->hshag_htsize
			+ haptr->hshag_chsize
			+ obufsz + obufsz
			+ (sizeof(PTR) + DB_ALIGN_SZ -1) & (~(DB_ALIGN_SZ -1)),
			&haptr->hshag_1stchunk,
			&dsh->dsh_error, "ht_ptrs+1stchunk");
	if (status != E_DB_OK)
	    return (status);
	haptr->hshag_htable = (PTR *)(haptr->hshag_1stchunk +
				(obufsz + obufsz + haptr->hshag_chsize));
					/* hash table follows 1stchunk and 
					** work buffers */
	initchunk = TRUE;
    }

    /* Init. hash buckets to NULL. */
    for (i = 0; i < haptr->hshag_htsize; i++)
			haptr->hshag_htable[i] = NULL;

    if (initchunk)
	((PTR **)haptr->hshag_1stchunk)[0] = NULL;
    haptr->hshag_flags |= HASHAG_LOAD1;	/* indicate load phase */

    dmhcb->dmh_tran_id = dsh->dsh_dmt_id;

    return(E_DB_OK);
}


/*{
** Name: QEA_HAGGF_LOAD1 - phase I (from action header source node) load
**	of aggregate rows
**
** Description:
**      This function reads rows from the QEA_HAGGF source node and attempts
**	to insert/aggregate them into the hash table.
**
** Inputs:
**	qef_rcb
**	action			Ptr to hash aggregate action header
**	haptr			Ptr to dynamic hash aggregation structure
**	hstp			Ptr to hash aggregation state structure (to
**				reduce paramaters passed)
**	reset			Flag from main call
**
** Outputs:
**      qef_rcb
**	    .error.err_code		one of the following
**				E_QE0000_OK
**				E_QE000D_NO_MEMORY_LEFT
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      26-mar-01 (inkdo01)
**          Split from qea_haggf as part of overflow processing.
**	21-nov-03 (inkdo01)
**	    Add open_flag parm to qea_func calls for parallel query support.
**	8-jan-04 (inkdo01)
**	    Fix parms to qea_func to conform to 29-dec changes.
**	31-aug-04 (inkdo01)
**	    Changes to support global base arrays.
[@history_template@]...
*/
static DB_STATUS
qea_haggf_load1(
QEF_RCB		*qef_rcb,
QEE_DSH		*dsh,
QEF_AHD		*action,
QEN_HASH_AGGREGATE *haptr,
HSHAG_STATE	*hstp,
i4		function,
bool		reset)

{
    QEN_NODE	*node = action->qhd_obj.qhd_qep.ahd_qep;
    DB_STATUS	status = E_DB_OK;
    ADF_CB	*adfcb = dsh->dsh_adf_cb;
    PTR		*cbs = dsh->dsh_cbs;
    bool	load_reset = reset;
    i4		open_flag = (load_reset) ? (TOP_OPEN | function) : function;
    
    /* Init. various variables and enter read loop. */

    hstp->hashexcb = (ADE_EXCB *)cbs[hstp->hashadf->qen_pos];
    hstp->hashbufp = (QEF_HASH_DUP *)(hstp->curchunk + haptr->hshag_chsize);
    hstp->hbufpp = (dsh->dsh_qp_ptr->qp_status & QEQP_GLOBAL_BASEARRAY) ?
	&(dsh->dsh_row[action->qhd_obj.qhd_qep.u1.s2.ahd_aghbuf]) :
	&(hstp->hashexcb->excb_bases[action->qhd_obj.qhd_qep.u1.s2.ahd_aghbuf]);
    *(hstp->hbufpp) = (PTR)hstp->hashbufp;
    hstp->oflwexcb = (ADE_EXCB *)cbs[hstp->oflwadf->qen_pos];
    hstp->obufpp = (dsh->dsh_qp_ptr->qp_status & QEQP_GLOBAL_BASEARRAY) ?
	&(dsh->dsh_row[action->qhd_obj.qhd_qep.u1.s2.ahd_agobuf]) :
	&(hstp->oflwexcb->excb_bases[action->qhd_obj.qhd_qep.u1.s2.ahd_agobuf]);
    hstp->currgcount = 0;

    for (;;)
    {
	/* fetch rows */
	status = (*node->qen_func)(node, qef_rcb, dsh, open_flag);
	load_reset = FALSE;
	open_flag = function &= ~(TOP_OPEN | FUNC_RESET);
	if (status != E_DB_OK)
	{
	    /* the error.err_code field in dsh should already be
	    ** filled in.
	    */
	    if (dsh->dsh_error.err_code == E_QE0015_NO_MORE_ROWS &&
			haptr->hshag_rcount)
	    {
		haptr->hshag_flags &= ~HASHAG_LOAD1;
		haptr->hshag_flags |= HASHAG_RETURN;
		haptr->hshag_curr_bucket = -1;
		haptr->hshag_lastrow = NULL;
		dsh->dsh_error.err_code = E_QE0000_OK;
		break;
	    }
	    return (status);
	}
	haptr->hshag_rcount++;

	/* Search for matching row in hash table. Project hash number,
	/* then look for it in hash table. */
	MEfill(haptr->hshag_hbufsz, 0, (PTR)hstp->hashbufp);
					/* init hash stuff buffer */
	hstp->hashexcb->excb_seg = ADE_SMAIN;
	status = ade_execute_cx(adfcb, hstp->hashexcb);
	if (status != E_DB_OK)
	{
	    if ((status = qef_adf_error(&adfcb->adf_errcb, 
		    status, dsh->dsh_qefcb, &dsh->dsh_error)) != E_DB_OK)
	      return (status);
	}
	/* Insert (or aggregate) into hash table. */
	status = qea_haggf_insert(dsh, haptr, hstp);
	if (status != E_DB_OK)
	    return(status);
    }
#ifdef xDEBUG
    {
	i4 i;
	QEN_HASH_PART *hpptr;
	TRdisplay("%@ hashagg thread %d LOAD1 loaded %d rows, %d chunks\n",
		dsh->dsh_threadno, haptr->hshag_rcount, hstp->chunk_count);
	for (i=0; i<haptr->hshag_pcount; i++)
	{
	    hpptr = &haptr->hshag_currlink->hsl_partition[i];
	    TRdisplay("t%d Partition[%d]: %d rows, offset %d, %d blocks, flags %v\n",
		dsh->dsh_threadno, i, hpptr->hsp_brcount, hpptr->hsp_offset, hpptr->hsp_currbn,
		"BSPILL,PSPILL,BINHASH,PINHASH,BFLUSHED,PFLUSHED,BALLSAME,PALLSAME,DONE",hpptr->hsp_flags);
	}
    }
#endif
    

    return(E_DB_OK);
}


/*{
** Name: QEA_HAGGF_LOAD2 - phase II (from overflow structures) load of aggregate i
**	rows
**
** Description:
**      This function reads rows from the hash aggregate overflow structures and
**	attempts to insert/aggregate them into the hash table (note that large
**	numbers of distinct grouping column sets may lead to recursive overflow 
**	handling).
**
** Inputs:
**	dsh
**	action			Ptr to hash aggregate action header
**	haptr			Ptr to dynamic hash aggregation structure
**	hstp			Ptr to hash aggregation state structure (to
**				reduce paramaters passed)
**
** Outputs:
**      dsh
**	    .error.err_code		one of the following
**				E_QE0000_OK
**				E_QE000D_NO_MEMORY_LEFT
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      26-mar-01 (inkdo01)
**          Written for hash aggregate overflow processing.
**	31-aug-04 (inkdo01)
**	    Changes to support global base arrays.
**	17-may-05 (inkdo01)
**	    Get rid of files and free space, as soon as we're done
**	    with them.
[@history_template@]...
*/
static DB_STATUS
qea_haggf_load2(
QEE_DSH		*dsh,
QEF_AHD		*action,
QEN_HASH_AGGREGATE *haptr,
HSHAG_STATE	*hstp)

{
    DB_STATUS	status = E_DB_OK;
    QEN_HASH_LINK *hlink = haptr->hshag_currlink;
    QEN_HASH_PART *hpptr;
    QEN_HASH_PART *prev_hpptr;
    PTR		obptr;
    i4		i, j;

    /* The "load2" phase loads overflowed partitions into the hash table
    ** (performing aggregation as they are loaded) in stages. After each
    ** stage, the aggregated results are returned to the caller. The
    ** first stage (or set of stages) loads partitions that didn't spill
    ** to disk. The remaining stage(s) process partitions that did overflow
    ** to disk. Any such partition that still has too many distinct 
    ** values of the grouping columns will recursively partition until
    ** the rows can be successfully aggregated.
    **
    ** A "rule" in processing the partitions is that if the hash table 
    ** already contains data from another partition processed in the same
    ** stage, no partition will be processed that contains more rows than
    ** will fit the hash table under the assumption that ALL those rows
    ** have distinct grouping column values. This is a very pessimistic 
    ** assumption, but little is gained by reducing the "qea_haggf_load2"
    ** calls, and the assumption will not result in any more partition file
    ** I/O. At a later date, techniques such as bit maps may be used to
    ** estimate the number of distinct groups in a partition file, thus
    ** enabling this rule to be loosened somewhat. */

    /* Init. local and global variables, set hash table to "empty". */
    hstp->oflwexcb = (ADE_EXCB *)dsh->dsh_cbs[hstp->oflwadf->qen_pos];
    hstp->obufpp = (dsh->dsh_qp_ptr->qp_status & QEQP_GLOBAL_BASEARRAY) ?
	&(dsh->dsh_row[action->qhd_obj.qhd_qep.u1.s2.ahd_agobuf]) :
	&(hstp->oflwexcb->excb_bases[action->qhd_obj.qhd_qep.u1.s2.ahd_agobuf]);
    hstp->owbufpp = (dsh->dsh_qp_ptr->qp_status & QEQP_GLOBAL_BASEARRAY) ?
	&(dsh->dsh_row[action->qhd_obj.qhd_qep.u1.s2.ahd_agowbuf]) :
	&(hstp->oflwexcb->excb_bases[action->qhd_obj.qhd_qep.u1.s2.ahd_agowbuf]);

    for (i = 0; i < haptr->hshag_htsize; i++)
	haptr->hshag_htable[i] = NULL;
    hstp->currgcount = 0;

    /* First step is to loop over partitions, looking for non-empty,
    ** non-spilled ones to load first. */
    if (haptr->hshag_flags & HASHAG_LOAD2P1)
    {
	for (i = 0; i < haptr->hshag_pcount; i++)
	{
	    hpptr = &hlink->hsl_partition[i];
	    if (hpptr->hsp_bptr == NULL || 
		    hpptr->hsp_flags & (HSP_DONE | HSP_BSPILL))
		continue;		/* parition must have rows and be in mem. */
	    if (hstp->currgcount > 0 && 
		hstp->currgcount + hpptr->hsp_brcount > haptr->hshag_htmax)
		continue;		/* "rule" from above */

#ifdef xDEBUG
	    TRdisplay("%@ hashagg thread %d LOAD2 nonspilled pno %d, %d rows, %d total so far\n",
		dsh->dsh_threadno, i, hpptr->hsp_brcount, hstp->currgcount);
#endif
	    /* This partition will fit into hash table. Loop over buffer
	    ** and insert rows. */
	    for (j = 0, obptr = hpptr->hsp_bptr; j < hpptr->hsp_brcount; j++)
	    {
		hstp->hashbufp = (QEF_HASH_DUP *)obptr;
		hstp->oflwbufp = *(hstp->obufpp) = obptr;
		status = qea_haggf_insert(dsh, haptr, hstp);
		if (status != E_DB_OK)
		    return(status);
		obptr = (PTR)(obptr + haptr->hshag_obufsz);
					/* next row in buffer */
	    }
	    hpptr->hsp_flags = HSP_DONE;	/* finished with partition */
	    status = qea_haggf_close(haptr, &hpptr->hsp_bfile,
		CLOSE_DESTROY);		/* discard the file */
	}

	/* If any data was processed from in-memory buffers, return 
	** corresponding aggregate rows now. This avoids confusing the 
	** in-memory logic with the spilled partitions logic. */
	if (hstp->currgcount > 0)
	{
	    haptr->hshag_flags |= HASHAG_RETURN;
	    return(E_DB_OK);
	}

	/* Nothing left in memory buffers - flush buffers for any open
	** files and continue with phase 2 of overflowed data. */
	haptr->hshag_flags &= ~HASHAG_LOAD2P1;
	status = qea_haggf_flush(dsh, haptr);
	if (status != E_DB_OK)
	    return(status);
	haptr->hshag_flags |= HASHAG_LOAD2P2;
    }

    /* Phase 2 of overflow handling - loop in search of offloaded partitions
    ** and process them now. */
    {
	i4	leftover, offset, j;

	for (i = 0; i < haptr->hshag_pcount; i++)
	{
	    hpptr = &hlink->hsl_partition[i];
	    /* Process an overflowed partition, but only if we haven't picked
	    ** one yet; or if we've started loading partitions, only if
	    ** this one will fit.
	    */
	    if (!(hpptr->hsp_flags & HSP_BSPILL) ||
		(hstp->currgcount > 0 && 
		 hstp->currgcount + hpptr->hsp_brcount > haptr->hshag_htmax))
		continue;		/* process overflowed partitions, but
					** only if there's room */
#ifdef xDEBUG
	    TRdisplay("%@ hashagg P2 loading pno %d at level %d, %d rows, thread %d\n",
			i, hlink->hsl_level, hpptr->hsp_brcount, dsh->dsh_threadno);
#endif
	    status = qea_haggf_open(dsh, haptr, hpptr, OPEN_READ);
					/* open partition file */
	    if (status != E_DB_OK)
		return(status);

	    offset = haptr->hshag_pbsize;  /* init to trigger 1st block read */

	    for (j = 0; j < hpptr->hsp_brcount; j++)
	    {
		if (offset + haptr->hshag_obufsz > haptr->hshag_pbsize)
		{
		    /* Copy last bit of prev. block, then read and process
		    ** next block. */
		    leftover = haptr->hshag_pbsize - offset;
		    if (leftover > 0)
			MEcopy((PTR)(haptr->hshag_rbptr + offset), leftover,
					haptr->hshag_workrow);
		    status = qea_haggf_read(haptr, hpptr->hsp_bfile,
				haptr->hshag_rbptr, &hpptr->hsp_currbn);
		    if (status != E_DB_OK)
			return(status);
		    if (leftover > 0)
		    {
			/* Copy rest of row to work buffer */
			offset = haptr->hshag_obufsz - leftover;
			MEcopy(haptr->hshag_rbptr, offset, 
				(PTR)(haptr->hshag_workrow + leftover));
			hstp->oflwbufp = haptr->hshag_workrow;
		    }
		    else
		    {
			/* Row starts at beginning of read buffer. */
			hstp->oflwbufp = haptr->hshag_rbptr;
			offset = haptr->hshag_obufsz;
		    }
		}
		else
		{
		    /* Address next row right in read buffer. */
		    hstp->oflwbufp = (PTR)(haptr->hshag_rbptr + offset);
		    offset += haptr->hshag_obufsz;
		}

		/* Finally have a row - now aggregate/insert into hash table. */
		*(hstp->obufpp) = hstp->oflwbufp;
		hstp->hashbufp = (QEF_HASH_DUP *)hstp->oflwbufp;
						/* set up hash buff ptr, too */
		status = qea_haggf_insert(dsh, haptr, hstp);
		if (status != E_DB_OK)
		    return(status);
	    }	/* end of "rows from partition i" loop */
	
	    hpptr->hsp_flags = HSP_DONE;	/* even if we recursed, 
						** this partition is done */
	    status = qea_haggf_close(haptr, &hpptr->hsp_bfile,
		CLOSE_DESTROY);			/* discard the file */

	    if (haptr->hshag_flags & HASHAG_RECURSING)
	    {
		/* This partition caused recursion. Set flags to return agg
		** rows already built, but then do phase 1 processing of 
		** recursed partition. */
		haptr->hshag_flags &= ~(HASHAG_RECURSING | HASHAG_LOAD2P2);
		haptr->hshag_flags |= (HASHAG_LOAD2P1 | HASHAG_RETURN);
		return(E_DB_OK);
	    }
	}	/* end of partition loop */
    }

    if (hstp->currgcount > 0)
	haptr->hshag_flags |= HASHAG_RETURN;	/* show agg rows to return */
    else if (hlink->hsl_level > 0)
    {
	/* No more rows from this recursion level, but back up to
	** previous level for possibly more overflow. LOAD2P2 
	** causes us to pick up where we left off in previous 
	** level. */
#ifdef xDEBUG
	TRdisplay("%@ Exiting hashagg recursion level %d, thread %d\n",
			hlink->hsl_level, dsh->dsh_threadno);
#endif
	/* Copy back partition buffers from lower level, it might have
	** allocated new ones if upper level partitioning was uneven
	*/
	prev_hpptr = &hlink->hsl_prev->hsl_partition[haptr->hshag_pcount-1];
	for (hpptr = &hlink->hsl_partition[haptr->hshag_pcount-1];
	     hpptr >= &hlink->hsl_partition[0];
	    --hpptr, --prev_hpptr)
	{
	    /* Ignore the partition file, hash agg destroys them as it
	    ** goes, unlike hash join.
	    */
	    prev_hpptr->hsp_bptr = hpptr->hsp_bptr;
	}
	haptr->hshag_currlink = hlink->hsl_prev;
	haptr->hshag_flags |= HASHAG_LOAD2P2;
    }
    else haptr->hshag_flags = HASHAG_DONE;	/* we're truly done! */

    return(E_DB_OK);
}


/*{
** Name: QEA_HAGGF_INSERT - locates/inserts rows in hash table
**
** Description:
**      This function processes rows from either load phase, attempting to locate 
**	matching row in hasyh table. If match is found, row is accumulated into
**	aggregate. If not, attempt is made to insert row into hash table. If no
**	space is available for insertion, row is added to overflow structures.
**
** Inputs:
**	dsh
**	haptr			Ptr to dynamic hash aggregation structure
**	hstp			Ptr to hash aggregation state structure (to
**				reduce paramaters passed)
**
** Outputs:
**      dsh
**	    .error.err_code		one of the following
**				E_QE0000_OK
**				E_QE000D_NO_MEMORY_LEFT
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      26-mar-01 (inkdo01)
**          Split from qea_haggf as part of overflow processing.
**	2-oct-01 (inkdo01)
**	    Changes to assure chunk link leaves chunk 8-byte aligned.
[@history_template@]...
*/
static DB_STATUS
qea_haggf_insert(
QEE_DSH		*dsh,
QEN_HASH_AGGREGATE *haptr,
HSHAG_STATE	*hstp)

{
    DB_STATUS	status = E_DB_OK;
    QEF_CB	*qef_cb = dsh->dsh_qefcb;
    ADF_CB	*adfcb = dsh->dsh_adf_cb;
    QEF_HASH_DUP *workbufp, *prevbufp;
    ADE_EXCB	*aggexcb;
    QEN_HASH_LINK *hlink = haptr->hshag_currlink;
    i4		hbucket, cmpres;
    u_i4	hashno;
    bool	match;

    /* Set up locals, then process row. */

    hashno = hstp->hashbufp->hash_number;
    hbucket = hashno % haptr->hshag_htsize;

    /* Search is 2 loops - outer looks for hashno on chain and
    ** (with luck) usually finds it. If 2 consecutive entries have 
    ** same hashno, inner loop uses hash keylist to compare 
    ** actual by-list entries. If neither finds a match, new 
    ** entry is added to chain. */
    adfcb->adf_errcb.ad_errcode = 0;
    for (workbufp = (QEF_HASH_DUP *)haptr->hshag_htable[hbucket], 
		    prevbufp = NULL, match = FALSE; 
	    workbufp != NULL && !match;
	    prevbufp = workbufp, workbufp = workbufp->hash_next)
    {
	if (hashno > workbufp->hash_number) continue;
	else if (hashno < workbufp->hash_number)
				break;
	else for (; workbufp != NULL && hashno == workbufp->hash_number;
		prevbufp = workbufp, workbufp = workbufp->hash_next)
	{
	    /* Inner search loop - does adt_sortcmp's on by exprs. */
	    cmpres = adt_sortcmp(adfcb, haptr->hshag_keylist,
		haptr->hshag_keycount, &(hstp->hashbufp->hash_key[0]),
		&(workbufp->hash_key[0]), 0);
	    if (adfcb->adf_errcb.ad_errcode != 0)
	    {
		/* Return comparison error. */
		dsh->dsh_error.err_code = adfcb->adf_errcb.ad_errcode;
		return(E_DB_ERROR);
	    }
	    if (cmpres <= 0)	/* found the spot? */
	    {
		if (cmpres == 0) 
		    match = TRUE;
		break;
	    }
	}
	break;			/* if we get here, must exit
				** outer loop */
    }

    /* If no match was found, must execute init code to 
    ** prepare new aggregate work row. */
    if (!match)
    {
	if (hstp->chunkoff+haptr->hshag_wbufsz <= haptr->hshag_chsize)
	{
	    workbufp = (QEF_HASH_DUP *)(hstp->curchunk + hstp->chunkoff);
	    hstp->chunkoff += haptr->hshag_wbufsz;
	}
	else
	{
	    /* Allocate next chunk and continue. */
	    if (*((PTR *)hstp->curchunk) != NULL)
	    {
		/* Chunkset already allocated - get next. */
		hstp->prevchunk = hstp->curchunk;
		hstp->curchunk = *((PTR *)hstp->curchunk);
		hstp->chunk_count++;
	    }
	    else if (hstp->chunk_count++ < haptr->hshag_chmax)
	    {
		/* Allocate another chunk. */
		hstp->prevchunk = hstp->curchunk;
		status = qea_haggf_palloc(qef_cb, haptr,
				haptr->hshag_chsize,
				&hstp->curchunk,
				&dsh->dsh_error, "chunk");
		if (status != E_DB_OK)
		    return(E_DB_ERROR);

		*((PTR *)hstp->prevchunk) = hstp->curchunk;
		*((PTR *)hstp->curchunk) = NULL;
	    }
	    else
	    {
		/* Overflow - if we're in LOAD2P2, we're in recursion
		** and must see if a new recursion must be started. 
		** In any event, row is sent to qea_haggf_overflow for
		** overflow processing. */
		if (haptr->hshag_flags & HASHAG_LOAD2P2 &&
		    !(haptr->hshag_flags & HASHAG_RECURSING))
		{
		    status = qea_haggf_recurse(dsh, haptr);
		    if (status != E_DB_OK)
			return(status);
		}

		status = qea_haggf_overflow(dsh, haptr, 
				hstp, hashno);
		return(status);
	    }

	    hstp->chunkoff = (sizeof(PTR) + DB_ALIGN_SZ - 1) &
			     (~(DB_ALIGN_SZ - 1));
	    workbufp = (QEF_HASH_DUP *) (hstp->curchunk + hstp->chunkoff);
	    hstp->chunkoff += haptr->hshag_wbufsz;
	}
	/* Copy hash stuff to front end, then exec init code for
	** new row. */
	MEcopy((PTR)hstp->hashbufp, haptr->hshag_hbufsz, (PTR)workbufp);
	*(hstp->wbufpp) = (PTR) workbufp;
	hstp->currexcb->excb_seg = ADE_SINIT;
	status = ade_execute_cx(adfcb, hstp->currexcb);
	if (status != E_DB_OK)
	{
	    if ((status = qef_adf_error(&adfcb->adf_errcb, 
    		status, qef_cb, &dsh->dsh_error)) != E_DB_OK)
    	    return (status);
	}
	workbufp->hash_number = hashno;
	if (prevbufp == NULL)
	{
	    if (haptr->hshag_htable[hbucket] == NULL)
		workbufp->hash_next = (QEF_HASH_DUP *) NULL;
	    else workbufp->hash_next = 
		(QEF_HASH_DUP *)haptr->hshag_htable[hbucket];
	    haptr->hshag_htable[hbucket] = (PTR)workbufp;
	}
	else
	{
	    workbufp->hash_next = prevbufp->hash_next;
	    prevbufp->hash_next = workbufp;
	}
	haptr->hshag_gcount++;
	hstp->currgcount++;
    }

    /* Found matching row, accumulate the aggregate. */
    if (haptr->hshag_flags & HASHAG_LOAD1)
    {
	/* If processing original input rows, setup ahd_current
	** & buffer. */
	aggexcb = hstp->currexcb;
	*(hstp->wbufpp) = (PTR)workbufp;
    }
    else 
    {
	/* Processing overflow rows, setup ahd_agoflow &
	** overflow and work buffers. */
	aggexcb = hstp->oflwexcb;
	*(hstp->owbufpp) = (PTR)workbufp;
    }
    aggexcb->excb_seg = ADE_SMAIN;
    status = ade_execute_cx(adfcb, aggexcb);
    if (status != E_DB_OK)
    {
	if ((status = qef_adf_error(&adfcb->adf_errcb, 
	    status, qef_cb, &dsh->dsh_error)) != E_DB_OK)
	    return (status);
    }
    dsh->dsh_qef_remnull |= aggexcb->excb_nullskip;

    return(E_DB_OK);
}


/*{
** Name: QEA_HAGGF_OVERFLOW - adds overflow row to hash aggregate overflow 
**	structure
**
** Description:
**      This function processes overflow rows. Hash number determines partition
**	to which row is assigned. Row is added to buffer, possibly requiring 
**	write of previous buffer contents to disk file.
**
** Inputs:
**	dsh
**	haptr			Ptr to dynamic hash aggregation structure
**	hstp			Ptr to hash aggregation state structure (to
**				reduce paramaters passed)
**	hashno			Hash number of current row
**
** Outputs:
**      dsh
**	    .error.err_code		one of the following
**				E_QE0000_OK
**				E_QE000D_NO_MEMORY_LEFT
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      26-mar-01 (inkdo01)
**          Written for hash aggregate overflow processing.
**	20-apr-01 (inkdo01)
**	    Fix to handle recursion - overflow row is already materialized
**	    in a recursion.
**	7-Jun-2005 (schka24)
**	    Typo fix (- instead of = getting open status).
[@history_template@]...
*/
static DB_STATUS
qea_haggf_overflow(
QEE_DSH		*dsh,
QEN_HASH_AGGREGATE *haptr,
HSHAG_STATE	*hstp,
u_i4		hashno)

{
    DB_STATUS	status = E_DB_OK;
    QEF_CB	*qef_cb = dsh->dsh_qefcb;
    ADF_CB	*adfcb = dsh->dsh_adf_cb;
    QEN_HASH_LINK *hlink = haptr->hshag_currlink;
    QEN_HASH_PART *hpptr;
    PTR		obptr;
    i4		pno, chunk;
    bool	spill = FALSE;
    
    /* Determine partition number, allocate (if necessary) partition
    ** buffer, add row to buffer and (if necessary) write block to 
    ** file. */

    haptr->hshag_flags |= (HASHAG_OVFLW | HASHAG_LOAD2P1);
						/* indicate overflow */
    pno = hashno % haptr->hshag_pcount;		/* partition no. */
    hpptr = &hlink->hsl_partition[pno];

    if (hpptr->hsp_bptr == NULL)
    {
	/* Allocate buffer for partition. */
	status = qea_haggf_palloc(qef_cb, haptr,
			haptr->hshag_pbsize,
			&hpptr->hsp_bptr,
			&dsh->dsh_error, "part_buf");
	if (status != E_DB_OK)
	    return (status);

	haptr->hshag_toplink->hsl_partition[pno].hsp_bptr =
			hpptr->hsp_bptr;	/* set in top level, too */
	hpptr->hsp_offset = 0;
    }

    /* Determine addr of buffer into which overflow row is built. */
    if ((hpptr->hsp_offset + haptr->hshag_obufsz) > haptr->hshag_pbsize)
    {
	spill = TRUE;		/* must write a block */
	obptr = haptr->hshag_workrow;
	chunk = haptr->hshag_pbsize - hpptr->hsp_offset;
    }
    else obptr = (PTR)(hpptr->hsp_bptr + hpptr->hsp_offset);

    if (hlink->hsl_level > 0)
    {
	/* We're in recursion. No need to materialize overflow row,
	** it was just read from partition file and can be copied to 
	** obptr. */
	MEcopy(hstp->oflwbufp, haptr->hshag_obufsz, obptr);
    }
    else
    {
	/* Move hash number, keys to head of buffer. */
	MEcopy((PTR)hstp->hashbufp, haptr->hshag_hbufsz, obptr);
	*(hstp->obufpp) = obptr;

	/* Materialize rest of overflow row into buffer. */
	hstp->oflwexcb->excb_seg = ADE_SINIT;
	status = ade_execute_cx(adfcb, hstp->oflwexcb);
	if (status != E_DB_OK)
	{
	    if ((status = qef_adf_error(&adfcb->adf_errcb, 
		status, qef_cb, &dsh->dsh_error)) != E_DB_OK)
		return (status);
	}
    }

    /* Update offset, row counts, etc. and leave (unless we need
    ** to write a block. */
    hpptr->hsp_brcount++;
    hpptr->hsp_offset += haptr->hshag_obufsz;
    if (!spill) 
	return(E_DB_OK);

    /* This row overflowed current buffer - copy bit that fits into
    ** buffer (if any), write the block (after possibly opening a
    ** new file) and copy rest of row to refreshed buffer. */
    if (chunk > 0)
	MEcopy(obptr, chunk, (PTR)(hpptr->hsp_bptr + 
			hpptr->hsp_offset - haptr->hshag_obufsz));
    if (!(hpptr->hsp_flags & HSP_BSPILL))
    {
	/* Open (and possibly allocate) a file before write. */
	status = qea_haggf_open(dsh, haptr, hpptr, OPEN_WRITE);
	if (status != E_DB_OK)
	    return(status);
    }
    status = qea_haggf_write(dsh, haptr, hpptr->hsp_bfile,
			hpptr->hsp_bptr, &hpptr->hsp_currbn);
    if (status != E_DB_OK)
	return(status);
    MEcopy((PTR)(haptr->hshag_workrow + chunk), 
	(haptr->hshag_obufsz - chunk), hpptr->hsp_bptr);
    hpptr->hsp_offset = haptr->hshag_obufsz - chunk;
    hpptr->hsp_flags |= HSP_BSPILL;
    return(E_DB_OK);
}


/*{
** Name: QEA_HAGGF_RECURSE - prepares next overflow recursion level
**
** Description:
**      This function sets up QEN_HASH_LINK structure for next level of
**	overflow partitioning in the event that a partition from the current
**	level has too many distinct groups to fit the hash table. A new
**	link structure is allocated and formatted unless one has already
**	been prepared at this level (more than one partition required 
**	recursion).
**
** Inputs:
**	dsh
**	haptr			Ptr to dynamic hash aggregation structure
**
** Outputs:
**      dsh
**	    .error.err_code		one of the following
**				E_QE0000_OK
**				E_QE000D_NO_MEMORY_LEFT
**	Returns:
**	    E_DB_{OK, WARN, ERROR, FATAL}
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      11-apr-01 (inkdo01)
**          Written for hash aggregate overflow processing.
[@history_template@]...
*/
static DB_STATUS
qea_haggf_recurse(
QEE_DSH		*dsh,
QEN_HASH_AGGREGATE *haptr)

{
    DB_STATUS	status = E_DB_OK;
    QEF_CB	*qef_cb = dsh->dsh_qefcb;
    QEN_HASH_LINK *hlink = haptr->hshag_currlink;
    QEN_HASH_LINK *prevlink;
    QEN_HASH_PART *hpptr;
    i4		i;


    /* First check if there is already a link structure at this recursion
    ** level. If so, tidy it up and prepare it for the new recursed
    ** partition. If not, allocate a new link structure and corresponding
    ** partition structures and copy partition buffers from top level (buffers
    ** are only used for load of one recursion level at a time - and top level
    ** partition structures have union of all allocated buffers). */

#ifdef xDEBUG
    TRdisplay("%@ hashagg recursing to level %d, thread %d\n",
		hlink->hsl_level+1, dsh->dsh_threadno);
#endif
    if (hlink->hsl_next == (QEN_HASH_LINK *) NULL)
    {
	/* First visit to this recursion level. Allocate/format link
	** structure and corresponding partition descriptor array. */
	status = qea_haggf_palloc(qef_cb, haptr,
			sizeof(QEN_HASH_LINK)
			+ haptr->hshag_pcount * sizeof(QEN_HASH_PART),
			&hlink->hsl_next,
			&dsh->dsh_error, "link+part_descs");
	if (status != E_DB_OK)
	    return (status);

	prevlink = hlink;
	hlink = hlink->hsl_next;	/* hook 'em together */
	hlink->hsl_prev = prevlink;
	hlink->hsl_next = (QEN_HASH_LINK *) NULL;
	hlink->hsl_level = prevlink->hsl_level + 1;
	hlink->hsl_partition = (QEN_HASH_PART *)&((char *)hlink)
						[sizeof(QEN_HASH_LINK)];
	for (i = 0; i < haptr->hshag_pcount; i++)
	{
	    QEN_HASH_PART   *hpart = &hlink->hsl_partition[i];

	    hpart->hsp_bptr = prevlink->hsl_partition[i].hsp_bptr;
	    hpart->hsp_bfile = (QEN_HASH_FILE *) NULL;
	    hpart->hsp_offset = 0;
	    hpart->hsp_brcount = 0;
	    hpart->hsp_currbn = 0;
	    hpart->hsp_reclevel = 0;
	    hpart->hsp_flags  = 0;
	}
    }
    else
    {
	prevlink = hlink;
	hlink = hlink->hsl_next;	/* new link level */
	for (i = 0; i < haptr->hshag_pcount; i++)
	{
	    QEN_HASH_PART   *hpart = &hlink->hsl_partition[i];

	    /* Reset stuff in existing partition array. */
	    hpart->hsp_bptr = prevlink->hsl_partition[i].hsp_bptr;
	    hpart->hsp_bfile = (QEN_HASH_FILE *) NULL;
	    hpart->hsp_offset = 0;
	    hpart->hsp_brcount = 0;
	    hpart->hsp_currbn = 0;
	    hpart->hsp_reclevel = 0;
	    hpart->hsp_flags  = 0;
	}
    }

    hlink->hsl_brcount = 0;

    haptr->hshag_currlink = hlink;
    haptr->hshag_flags |= HASHAG_RECURSING;

    return(E_DB_OK);
}


/*{
** Name: QEA_HAGGF_WRITE	- write a block to a partition file
**
** Description:
**	This function writes blocks to partition files. 
**
** Inputs:
**	dsh		- Thread data segment header
**	haptr		- ptr to base hash structure
**	hfptr		- ptr to file descriptor
**	buffer		- ptr to buffer being written
**	currbn		- ptr to current RBN to read 
**
** Outputs:
**
** Side Effects:
**
**
** History:
**	28-mar-01 (inkdo01)
**	    Cloned from qenhashutl.c
**
*/
 
static DB_STATUS
qea_haggf_write(
QEE_DSH		*dsh,
QEN_HASH_AGGREGATE *haptr,
QEN_HASH_FILE	*hfptr,
PTR		buffer,
i4		*currbn)

{

    DMH_CB	*dmhcb;
    DB_STATUS	status = E_DB_OK;
    QEF_CB	*qcb = dsh->dsh_qefcb;

    /* If we're writing a block, we'll need to read it back sooner
    ** or later. So allocate read buffer here. */
    if (haptr->hshag_rbptr == NULL)
    {

	/* Partition read buffer has not yet been allocated. Do
	** it now. */
	status = qea_haggf_palloc(qcb, haptr,
				haptr->hshag_pbsize,
				&haptr->hshag_rbptr,
				&dsh->dsh_error, "io_buf");
	if (status != E_DB_OK)
	    return (status);
    }

    /* Load up DMH_CB and issue the call. */
    dmhcb = (DMH_CB *)haptr->hshag_dmhcb;
    dmhcb->dmh_locidxp = &hfptr->hsf_locidx;
    dmhcb->dmh_file_context = hfptr->hsf_file_context;
    dmhcb->dmh_rbn = (*currbn)++;
    dmhcb->dmh_buffer = buffer;
    dmhcb->dmh_func = DMH_WRITE;
    dmhcb->dmh_flags_mask = 0;

    status = dmh_write(dmhcb);
    return(status);

}


/*{
** Name: QEA_HAGGF_OPEN 	- determines if open file is available
**				or creates (and opens) a new one
**
** Description:
**	This file returns immediately for read processing (since file
**	is already there and open), after picking the next available
**	from the free chain for write processing or calls DMF to create
**	and open a new file for write processing when no files are on 
**	the free chain.
**
** Inputs:
**	dsh		- Thread data segment header
**	haptr		- ptr to base hash structure
**	hpptr		- ptr to partition descriptor
**	flag		- flag indicating open for read or write
**
** Outputs:
**
** Side Effects:
**
**
** History:
**	28-mar-01 (inkdo01)
**	    Cloned from qenhashutl.c
**
*/
 
static DB_STATUS
qea_haggf_open(
QEE_DSH		*dsh,
QEN_HASH_AGGREGATE *haptr,
QEN_HASH_PART	*hpptr,
i4		flag)

{

    DMH_CB	*dmhcb;
    QEN_HASH_FILE  **fptr;
    DB_STATUS	status = E_DB_OK;


    /* It seems that SR functions only require a "real" open when
    ** the file is being created. Subsequently, reads or writes 
    ** can be issued (assuming their sequence makes sense) on the
    ** file without closing/reopening. The logic of this function
    ** checks for read/write flag and returns immediately for read
    ** assuming the file already exists and is open). For write,
    ** it checks the free chain and again returns, if an available 
    ** file is found. If the free chain is empty, a file context 
    ** is allocated and we really issue the OPEN call. */

    hpptr->hsp_currbn = 0;		/* this is a necessity */
    if (flag & OPEN_READ) return(E_DB_OK);

    /* Load up DMH_CB and issue the call. */
    dmhcb = (DMH_CB *)haptr->hshag_dmhcb;
    fptr = &hpptr->hsp_bfile;

    /* Check free list and reuse existing file, if possible. 
    ** Otherwise, allocate a QEN_HASH_FILE + file context block
    ** and create the file, too. */
    if (haptr->hshag_freefile != NULL)
    {
	*fptr = haptr->hshag_freefile;
	haptr->hshag_freefile = (*fptr)->hsf_nextf;
	return(E_DB_OK);
    }
    else
    {
	QEF_CB		*qcb = dsh->dsh_qefcb;

	/* Allocate memory for the file context (and containing
	** QEN_HASH_FILE block). */
	status = qea_haggf_palloc(qcb, haptr,
				sizeof(QEN_HASH_FILE) + sizeof(SR_IO),
				fptr,
				&dsh->dsh_error, "hash_file");
	if (status != E_DB_OK)
	    return (status);

	(*fptr)->hsf_nextf = NULL;
	(*fptr)->hsf_nexto = haptr->hshag_openfiles;
	haptr->hshag_openfiles = (*fptr);		/* link to open list */
	(*fptr)->hsf_locidx = haptr->hshag_lastloc;
	(*fptr)->hsf_file_context = (PTR)&((char *)(*fptr))
					[sizeof(QEN_HASH_FILE)];
    }
    dmhcb->dmh_file_context = (*fptr)->hsf_file_context;
    dmhcb->dmh_locidxp = &(*fptr)->hsf_locidx;
    dmhcb->dmh_blksize = haptr->hshag_pbsize;
    dmhcb->dmh_func = DMH_OPEN;
    dmhcb->dmh_flags_mask = DMH_CREATE;

    status = dmh_open(dmhcb);
    if (status == E_DB_OK) haptr->hshag_lastloc = (*fptr)->hsf_locidx;
			/* save last used location index */
    return(status);

}


/*{
** Name: QEA_HAGGF_READ 	- read a block from a partition file
**
** Description:
**	This function reads blocks from partition files. 
**
** Inputs:
**	haptr		- ptr to base hash structure
**	hfptr		- ptr to file descriptor
**	buffer		- ptr to buffer being written
**	currbn		- ptr to current RBN to read 
**
** Outputs:
**
** Side Effects:
**
**
** History:
**	28-mar-01 (inkdo01)
**	    Cloned from qenhashutl.c
**
*/
 
static DB_STATUS
qea_haggf_read(
QEN_HASH_AGGREGATE *haptr,
QEN_HASH_FILE	*hfptr,
PTR		buffer,
i4		*currbn)

{

    DMH_CB	*dmhcb;
    DB_STATUS	status = E_DB_OK;

    /* Load up DMH_CB and issue the call. */
    dmhcb = (DMH_CB *)haptr->hshag_dmhcb;
    dmhcb->dmh_locidxp = &hfptr->hsf_locidx;
    dmhcb->dmh_file_context = hfptr->hsf_file_context;
    dmhcb->dmh_rbn = (*currbn)++;
    dmhcb->dmh_buffer = buffer;
    dmhcb->dmh_func = DMH_READ;
    dmhcb->dmh_flags_mask = 0;

    status = dmh_read(dmhcb);
    if (status != E_DB_OK) 
{
status = status;
}
    return(status);

}


/*{
** Name: QEA_HAGGF_FLUSH	- write out contents of partition buffers
**				and close files
**
** Description:
**	This function writes remaining buffer of all disk resident 
**	partition files, then closes them.
**
** Inputs:
**	dsh		- Thread data segment header
**	haptr		- ptr to base hash structure
**
** Outputs:
**
** Side Effects:
**
**
** History:
**	28-mar-01 (inkdo01)
**	    Cloned from qenhashutl.c
**
*/
 
static DB_STATUS
qea_haggf_flush(
QEE_DSH		*dsh,
QEN_HASH_AGGREGATE *haptr)

{

    QEN_HASH_LINK  *hlink = haptr->hshag_currlink;
    QEN_HASH_PART  *hpptr;
    DMH_CB	*dmhcb;
    QEN_HASH_FILE  *fptr;
    i4		i;
    DB_STATUS	status = E_DB_OK;
    

    /* Search partition array for "spilled" partitions. Write last
    ** block for each, then close the file. */

    for (i = 0; i < haptr->hshag_pcount; i++)
    {
	hpptr = &hlink->hsl_partition[i];
	if (!(hpptr->hsp_flags & HSP_BSPILL) ||
		(hpptr->hsp_flags & HSP_BFLUSHED)) continue;

	/* There is always data in the buffer of a spilled partition.
	** The flag isn't set (and a write isn't done) until there's
	** enough data to partially fill the next block. */
	fptr = hpptr->hsp_bfile;
	status = qea_haggf_write(dsh, haptr, fptr, hpptr->hsp_bptr, 
						&hpptr->hsp_currbn);
	if (status != E_DB_OK) return(status);

	status = qea_haggf_close(haptr, &fptr, 0);
	hpptr->hsp_flags |= HSP_BFLUSHED;
	hpptr->hsp_offset = 0;
	hpptr->hsp_currbn = 0;
	if (status != E_DB_OK) return(status);
    }

    return(E_DB_OK);

}


/*{
** Name: QEA_HAGGF_CLOSE	- closes an existing partition file and
**				places it on free chain
**
** Description:
**	This function closes an existing partition file and, if file
**	isn't immediately being reread (to load it into the hash table
**	or to repartition it), places its context block on the free
**	chain for possible reuse. 
**
** Inputs:
**	haptr		- ptr to base hash structure
**	hfptr		- ptr to partition descriptor ptr
**	flag		- flag indicating whether to "release" file
**
** Outputs:
**
** Side Effects:
**
**
** History:
**	28-mar-01 (inkdo01)
**	    Cloned from qenhashutl.c
**	17-may-05 (inkdo01)
**	    Add code to drop destroyed files from open chain.
*/
 
static DB_STATUS
qea_haggf_close(
QEN_HASH_AGGREGATE *haptr,
QEN_HASH_FILE	**hfptr,
i4		flag)

{

    DMH_CB	*dmhcb;
    QEN_HASH_FILE  *fptr = *hfptr, **wfptr;


    /* SR close is not required to flip from output to input. This 
    ** function does nothing, if no flags are input. It moves the
    ** file context for the still-open file to the free chain if
    ** the CLOSE_RELEASE flag is on. And it calls DMF to destroy it 
    ** if the CLOSE_DESTROY flag is on. */

    if (!(flag & (CLOSE_RELEASE | CLOSE_DESTROY)) || fptr == NULL)
	return(E_DB_OK);

    (*hfptr) = (QEN_HASH_FILE *) NULL;
    if (flag & CLOSE_RELEASE)
    {
	/* Add file context to free list. */
	fptr->hsf_nextf = haptr->hshag_freefile;
	haptr->hshag_freefile = fptr;
	return(E_DB_OK);
    }

    /* Must be CLOSE_DESTROY case. */
    /* Load up DMH_CB and issue the call. */
    dmhcb = (DMH_CB *)haptr->hshag_dmhcb;
    dmhcb->dmh_locidxp = &fptr->hsf_locidx;
    dmhcb->dmh_file_context = fptr->hsf_file_context;
    dmhcb->dmh_func = DMH_CLOSE;
    /* Search for file on open chain and remove it. */
    for (wfptr = &haptr->hshag_openfiles; (*wfptr) && (*wfptr) != fptr;
	    wfptr = &(*wfptr)->hsf_nexto);
    if ((*wfptr))
	(*wfptr) = fptr->hsf_nexto;

    dmhcb->dmh_flags_mask = DMH_DESTROY;

    return(dmh_close(dmhcb));


}

/*{
** Name: QEA_HAGGF_CLOSEALL	- closes and destroys all files on hash
**				control block's file chain
**
** Description:
**	This function loops over the hshag_openfiles chain, issuing 
**	qea_haggf_close calls with the DESTROY option to free them all.
**	It is called at the end of hash join processing.
**
** Inputs:
**	haptr		- ptr to base hash structure
**
** Outputs:
**
** Side Effects:
**
**
** History:
**	28-mar-01 (inkdo01)
**	    Cloned from qenhashutl.c
**
*/
 
DB_STATUS
qea_haggf_closeall(
QEN_HASH_AGGREGATE *haptr)

{
    QEN_HASH_FILE	*hfptr, *savefptr;
    DB_STATUS		status;


    /* Simply loop over file list, qea_haggf_close'ing each entry. */
    for (hfptr = savefptr = haptr->hshag_openfiles; hfptr; 
				hfptr = savefptr = savefptr->hsf_nexto)
    {
	status = qea_haggf_close(haptr, &hfptr, CLOSE_DESTROY);
	if (status != E_DB_OK) return(status);
    }
    return(E_DB_OK);		/* to get here, there must have been no
				** files to close */
}


/*
** Name: qea_haggf_palloc
**
** Description:
**	Allocate memory from the hash stream, making sure that we don't
**	exceed the session limit.  If the hash stream isn't open, we'll
**	open it.
**
** Inputs:
**	qefcb			QEF session control block
**	haptr			Hash aggregation control base
**	psize			Size of request in bytes
**	pptr			Put allocated block address here
**	dsh_error		Where to put error code if error
**	what			Short name for request for tracing
**
** Outputs:
**	Returns E_DB_OK or error status
**	If error, dsh_error->err_code filled in.
**
** History:
**	7-Jun-2005 (schka24)
**	    Copy from qen_hash_palloc.
**	24-Oct-2005 (schka24)
**	    Delay if no memory resources are available (from Datallegro).
*/

static DB_STATUS
qea_haggf_palloc(QEF_CB *qefcb, QEN_HASH_AGGREGATE *haptr,
	SIZE_TYPE psize, void *pptr,
	DB_ERROR *dsh_error, char *what)

{
    DB_STATUS status;		/* Usual status thing */
    i4 naptime;			/* Milliseconds delayed so far */
    ULM_RCB ulm;		/* ULM request block */

    if (qefcb->qef_h_used + psize > qefcb->qef_h_max)
    {
	TRdisplay("%@ QEF sorthash pool: hash %p alloc of %s (%u) fails because used is %u\n",
		haptr, what, psize, qefcb->qef_h_used);
	dsh_error->err_code = E_QE000D_NO_MEMORY_LEFT;
	return (E_DB_ERROR);
    }

    /* Use the sorthash pool */
    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_s_ulmcb, ulm);

    /* If stream not allocated, do that now */
    if (haptr->hshag_streamid == NULL)
    {
	ulm.ulm_streamid_p = &haptr->hshag_streamid;
	/* **** temp until I get qec straightened out */
	if (haptr->hshag_pbsize > 16 * 1024)
	    ulm.ulm_blocksize = 1024 * 1024;
	naptime = 0;
	for (;;)
	{
	    status = ulm_openstream(&ulm);
	    if (status == E_DB_OK || naptime > Qef_s_cb->qef_max_mem_nap)
		break;
	    CSms_thread_nap(500);		/* Half a second */
	    naptime += 500;
	}
	if (status != E_DB_OK)
	{
	    TRdisplay("%@ QEF sorthash pool: hashagg %p open of %s (%u) failed, pool full, used is %u\n",
			haptr, what, psize, qefcb->qef_h_used);
	    if (ulm.ulm_error.err_code == E_UL0005_NOMEM)
		dsh_error->err_code = E_QE000D_NO_MEMORY_LEFT;
	    else
		dsh_error->err_code = ulm.ulm_error.err_code;
	    return (E_DB_ERROR);
	}
	if (naptime > 0)
	{
	    TRdisplay("%@ QEF sorthash pool: hashagg %p open of %s (%u) delayed %d ms, pool full, used is %d\n",
		haptr, what, psize, naptime, qefcb->qef_h_used);
	}
    }

    ulm.ulm_streamid = haptr->hshag_streamid;
    ulm.ulm_psize = psize;
    naptime = 0;
    for (;;)
    {
	status = ulm_palloc(&ulm);
	if (status == E_DB_OK || naptime > Qef_s_cb->qef_max_mem_nap)
	    break;
	CSms_thread_nap(500);		/* Half a second */
	naptime += 500;
    }
    if (status != E_DB_OK)
    {
	TRdisplay("%@ QEF sorthash pool: hashagg %p alloc of %s (%u) failed, pool full, used is %u\n",
		haptr, what, psize, qefcb->qef_h_used);
	if (ulm.ulm_error.err_code == E_UL0005_NOMEM)
	    dsh_error->err_code = E_QE000D_NO_MEMORY_LEFT;
	else
	    dsh_error->err_code = ulm.ulm_error.err_code;
	return (E_DB_ERROR);
    }
    if (naptime > 0)
    {
	TRdisplay("%@ QEF sorthash pool: hashagg %p alloc of %s (%u) delayed %d ms, pool full, used is %d\n",
		haptr, what, psize, naptime, qefcb->qef_h_used);
    }
    haptr->hshag_halloc += psize;
    CSadjust_counter(&qefcb->qef_h_used, psize);	/* thread-safe */
    *(void **) pptr = ulm.ulm_pptr;
#ifdef xDEBUG
    TRdisplay("%@ hashagg %p alloc %u %s at %p, new halloc %u used %u\n",
		haptr, psize, what, ulm.ulm_pptr, haptr->hshag_halloc, qefcb->qef_h_used);
#endif

    return (E_DB_OK);

} /* qea_haggf_palloc */
