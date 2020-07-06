/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <me.h>
#include    <bt.h>
#include    <st.h>
#include    <sr.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <ddb.h>
#include    <adf.h>
#include    <ade.h>
#include    <cs.h>
#include    <scf.h>
#include    <ulf.h>
#include    <ulm.h>
#include    <dmf.h>
#include    <dmhcb.h>
#include    <dmtcb.h>
#include    <dmrcb.h>
#include    <qefmain.h>
#include    <qsf.h>
#include    <qefrcb.h>
#include    <qefcb.h>
#include    <qefnode.h>
#include    <psfparse.h>
#include    <qefact.h>
#include    <qefqp.h>
#include    <qefscb.h>
#include    <qefdsh.h>
#include    <ex.h>
#include    <dmmcb.h>
#include    <dmucb.h>
#include    <tm.h>

#include    <dudbms.h>
#include    <qefqeu.h>
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
**  Name: QENHASHUTL.C	- hash join utility functions.
**
**  Description:
**          qen_hash_partition - partition set of rows (build 
**		or probe)
**	    qen_hash_build  - partitioning done for one set
**		of rows (build or probe). Build a hash table
**          qen_hash_probe1 - probe source phase 1 processing
**	    qen_hash_probe2 - phase 2 processing
**
**
**  History:
**      4-mar-99 (inkdo01)
**          Written.
**	23-aug-01 (devjo01)
**	    Corrected offset calculations for non-32 bit platforms. b105598.
**	01-oct-2001 (devjo01)
**	    Rename QEN_HASH_DUP to QEF_HASH_DUP.
**	19-apr-02 (inkdo01)
**	    Added parm to qen_hash_flush call to resolve 107539.
**	23-feb-04 (inkdo01)
**	    Changed qef_rcb->error to dsh->dsh_error for || thread safety.
**	6-May-2005 (schka24)
**	    Hash values are unsigned (they can be -infinity).
**	    Take now-incorrect u_i2 casts off MEcopy sizes.
**	17-may-05 (inkdo01)
**	    Close and destroy work files after they've been used.
**	7-Jun-2005 (schka24)
**	    A variety of fixes found by 100 Gb TPC-H, mostly in CO joining.
**	22-Jun-2005 (schka24)
**	    Bit filters are allocated per-partition, but they weren't being
**	    used that way.  Fix.
**	10-Nov-2005 (schka24)
**	    Clarify row-filter stuff when repartitioning, rename filter flag.
**	    Refine OJ indicator code so that we don't hand the pointers
**	    around, which turned out to just be a nuisance.  Fix OJ bug
**	    involving role reversal.
**	7-Dec-2005 (kschendel)
**	    Join key compare-list is now built into node, minor adjustment
**	    in several places here.
**	14-Dec-2005 (kschendel)
**	    Eliminate rcb parameter from routines, dsh suffices now
**	    that we have an ADF CB pointer in the DSH.
**/


GLOBALREF QEF_S_CB *Qef_s_cb;


/*	local structures 	*/
typedef struct _FREE_CHUNK {
    struct _FREE_CHUNK	*free_next;  /* ptr to next free chunk */
    i4	    free_rows;		/* size of free chunk (in rows) */
}	FREE_CHUNK;

static DB_DATA_VALUE hashdv = {NULL, 4, DB_INT_TYPE, 0};

/*	static functions	*/

static VOID qen_hash_verify(QEN_HASH_BASE *hbase);

static DB_STATUS
qen_hash_insert(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase,
QEF_HASH_DUP	**htable,
QEF_HASH_DUP	*brptr);

static DB_STATUS
qen_hash_recurse(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase);
 
static DB_STATUS
qen_hash_repartition(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase,
QEN_HASH_PART	*hpptr);

static DB_STATUS
qen_hash_probe(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase,
PTR		*buildptr,
PTR		probeptr,
QEN_OJMASK	*ojmask,
bool		*gotone);
 
static DB_STATUS
qen_hash_cartprod(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase,
PTR		*buildptr,
PTR		*probeptr,
QEN_OJMASK	*ojmask,
bool		*gotone);

static DB_STATUS qen_hash_reset_cpinner(
	QEE_DSH *dsh,
	QEN_HASH_BASE *hbase,
	QEN_OJMASK pojmask);
 
static VOID
qen_hash_bvinit(
QEN_HASH_BASE	*hbase,
QEN_HASH_PART	*hpptr,
i4		rowsz);
 
static bool
qen_hash_checknull(
QEN_HASH_BASE	*hbase,
PTR		bufptr);

static DB_STATUS qen_hash_palloc(
	QEF_CB		*qefcb,
	QEN_HASH_BASE	*hbase,
	SIZE_TYPE	psize,
	void		*pptr,
	DB_ERROR	*dsh_error,
	char		*what);

static DB_STATUS
qen_hash_write(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase,
QEN_HASH_FILE	*hfptr,
PTR		buffer,
i4		*currbn);

static DB_STATUS
qen_hash_read(
QEN_HASH_BASE	*hbase,
QEN_HASH_FILE	*hfptr,
PTR		buffer,
i4		*currbn);

static DB_STATUS
qen_hash_open(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase,
QEN_HASH_PART	*hpptr,
i4		flag);
/* qen_hash_open flag settings */
#define OPEN_READ	1
#define OPEN_WRITE	2
 
static DB_STATUS
qen_hash_close(
QEN_HASH_BASE	*hbase,
QEN_HASH_FILE	**hfptr,
i4		flag);
/* qen_hash_close flag settings */
#define CLOSE_RELEASE	1	
#define CLOSE_DESTROY	2
 
static DB_STATUS
qen_hash_flush(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase,
bool		nonspilled_too);
 
static DB_STATUS
qen_hash_flushone(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase,
QEN_HASH_PART	*hpptr);
 
static DB_STATUS
qen_hash_flushco(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase);
 


/*{
** Name: QEN_HASH_PARTITION - partition a set of rows
**
** Description:
**	Rows of either build or probe sources are delivered to this 
**	function to be partitioned. Function is either called directly
**	from qen_hjoin (with build source rows) or from inside qenhashutl.c
**	(with either probe or build rows, possibly while recursing on
**	a partition too large to fit into the hash table). If more rows 
**	arrive than will fit into the buffer for a partition, the blocks
**	for that partition are written to disk for later processing.
**
**	The caller can specify whether rows should be filtered based on
**	the bit-vector for the partition on the other side.  Filtering
**	is normally turned off for build rows (because there isn't any
**	"other" side yet) and on for probe rows.  Filtering might be
**	turned off if the caller is repartitioning, where it might not
**	be in a position to deal with outer-join nomatch processing.
**
** Inputs:
**	dsh		- Pointer to thread's DSH
**	hbase		- ptr to base hash structure
**	rowptr		- ptr to build/probe row to be stowed
**	filterflag	- ptr to boolean: TRUE to filter rows, FALSE to
**			  always just store the row
**
** Outputs:
**
**	filterflag	- If TRUE upon entry, remains TRUE if row was
**			  filtered out.  FALSE if row was not filtered.
**
** Side Effects:
**
**
** History:
**	4-mar-99 (inkdo01)
**	    Written.
**	jan-00 (inkdo01)
**	    Changes for bit filters.
**	28-jan-00 (inkdo01)
**	    Tidy up hash memory management.
**	7-sep-01 (inkdo01)
**	    Fix up ojpartflag set after bit vector test. Fixes OJ logic.
**	13-aug-02 (inkdo01)
**	    Add nullcheck before partitioning row (to re-fix 105710).
**	22-Jun-2005 (schka24)
**	    Bit-filter position can be based on hashno within partition
**	    for better selectivity.
**	10-Nov-2005 (schka24)
**	    "ojpartflag" is really a filtering yes/no flag, rename.
*/
 
DB_STATUS
qen_hash_partition(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase,
PTR		rowptr,
bool		*filterflag)
{

    QEF_CB		*qcb = dsh->dsh_qefcb;
    QEN_HASH_LINK	*hlink = hbase->hsh_currlink;
    QEN_HASH_PART	*hpptr;
    i4			pno, rowsz;
    u_i4		hashno, hashquo;
    i4			spillmask;
    u_i4		bvsize = hbase->hsh_bvsize * BITSPERBYTE - 1;
    i4			bvind;
    i4			sametest, sameflag;
    DB_STATUS		status;
    PTR			target;
    bool		reverse = (hbase->hsh_flags & HASH_ROLE_REVERSE) != 0;
    bool		bvuse = FALSE, bvset = FALSE;


    /* Use hash number to determine target partition, increment row counts
    ** and copy row into corresponding block. */

    hashno = *((u_i4 *)rowptr);		/* copy hash number from front 
					** of build row buffer */
    pno = hashno % hbase->hsh_pcount;
    hashquo = hashno / hbase->hsh_pcount;
    hpptr = &hlink->hsl_partition[pno]; /* target partition */

    /* "reverse" here means "loading probe rows" as opposed to loading
    ** build rows.
    */
    if (reverse)
    {
	hlink->hsl_prcount++;
	rowsz = hbase->hsh_prowsz;
	spillmask = HSP_PSPILL;
	if (hpptr->hsp_brcount > 0) bvuse = TRUE;
	else if (hpptr->hsp_flags & spillmask) bvset = TRUE;
	sametest = hpptr->hsp_prcount;
	sameflag = HSP_PALLSAME;
    }
    else 
    {
	hlink->hsl_brcount++;
	rowsz = hbase->hsh_browsz;
	spillmask = HSP_BSPILL;
	if (hpptr->hsp_prcount > 0) bvuse = TRUE;
	else if (hpptr->hsp_flags & spillmask) bvset = TRUE;
	sametest = hpptr->hsp_brcount;
	sameflag = HSP_BALLSAME;
    }

    /* Coarse checks - does row split to empty partition, or
    ** is corresponding bit vector entry empty, or is there
    ** a null key column. In any of the cases, throw the row 
    ** away (or make into OJ). */
    if (*filterflag)
    {
	if (!bvuse)
	    return (E_DB_OK);		/* Other side is empty */
	else
	{
	    bvind = hashquo % bvsize;
	    if (BTtest(bvind, hpptr->hsp_bvptr) == 0)
	    {
		hbase->hsh_bvdcnt++;	/* incr discard count */
		return(E_DB_OK);
	    }
	    /* Maybe row filters because null join col can't match anything */
	    if (hbase->hsh_flags & HASH_CHECKNULL &&
	      qen_hash_checknull(hbase, &((QEF_HASH_DUP *)rowptr)->hash_key[0]))
		return(E_DB_OK);
	}
    }
    *filterflag = FALSE;		/* Not filtering out this row */

    /* Set bit filter (if necessary). */
    if (bvset) BTset(hashquo % bvsize, hpptr->hsp_bvptr);

    /* Check for continued "all same hashno in partition" condition. */
    if (sametest > 0 && (hpptr->hsp_flags & sameflag) &&
	hashno != hpptr->hsp_prevhash) hpptr->hsp_flags &= ~sameflag;
				/* if diff hashno, turn off flag */
    hpptr->hsp_prevhash = hashno;
 
    /* Count rows added to partition. */
    if (reverse) hpptr->hsp_prcount++;
    else hpptr->hsp_brcount++;

    if (rowsz + hpptr->hsp_offset > hbase->hsh_csize)
    {
	QEF_CB		*qcb = dsh->dsh_qefcb;
	i4	chunk;
	PTR	source;
	i4	openflag;

	/* Not enough space left in partition buffer. Stuff as much
	** as will fit in this block, write it and store rest of
	** row in next block. */

	chunk = hbase->hsh_csize - hpptr->hsp_offset;
					/* amount that'll fit */
	if (chunk)
	{
	    target = (PTR)&hpptr->hsp_bptr[hpptr->hsp_offset];
	    MEcopy (rowptr, chunk, target);
					/* copy what fits (if anything) */
	}
	if (!(hpptr->hsp_flags & spillmask))
	{
	    /* Must open, before we can write. */
	    status = qen_hash_open(dsh, hbase, hpptr, OPEN_WRITE);
	    if (status != E_DB_OK) 
			    return(status);
	    hpptr->hsp_flags |= spillmask;	/* now flag it */

	    /* At this point, must also initialize bit filter with 
	    ** rows in first buffer full (if we're building, not
	    ** probing). */
	    if (!bvuse)
	    {
		qen_hash_bvinit(hbase, hpptr, rowsz);
		/* bvset wasn't on before, so set the bit for the current
		** row now.  (Perhaps redundantly if the hash number for
		** this row fit into the tail end of the buffer -- no
		** harm done though.)
		*/
		BTset(hashquo % bvsize, hpptr->hsp_bvptr);
	    }
	}

	status = qen_hash_write(dsh, hbase, (reverse) ? hpptr->hsp_pfile :
		hpptr->hsp_bfile, hpptr->hsp_bptr, &hpptr->hsp_currbn);
	if (status != E_DB_OK) 
			return(status);

	source = (PTR)&((char *)rowptr)[chunk];
	chunk = rowsz - chunk;
	hpptr->hsp_offset = chunk;	/* reset buffer offset */
	target = (PTR)hpptr->hsp_bptr;
	MEcopy (source, chunk, target);
					/* copy rest of row to front */
	return(E_DB_OK);

    }
    else
    {
	/* First check if partition buffer been allocated (& if not, do it). */
	if (hpptr->hsp_bptr == NULL)
	{
	    status = qen_hash_palloc(qcb, hbase,
			hbase->hsh_csize + hbase->hsh_bvsize,
			&hpptr->hsp_bptr,
			&dsh->dsh_error, "part_buf+bv");
	    if (status != E_DB_OK)
		return (status);

	    hpptr->hsp_bvptr = &hpptr->hsp_bptr[hbase->hsh_csize];
				/* bit filter follows partition buffer */
	}

	/* Compute target address of new row and copy it there. */
	target = (PTR)&hpptr->hsp_bptr[hpptr->hsp_offset];
	hpptr->hsp_offset += rowsz;
	MEcopy(rowptr, rowsz, target);
    }
    return(E_DB_OK);

}	/* end of qen_hash_partition */


/*{
** Name: QEN_HASH_BUILD - time to build a hash table from current
**			partition set
**
** Description:
**	Rows of build/probe source have all been delivered for this 
**	recursion level. A hash table may already have been built and
**	processed. This function creates more and more hash tables
**	for subsequent probing until all rows have been processed, or
**	there only remains partitions too big to fit (and which must 
**	be recursively partitioned).
**
**	This function is sensitive to the specific needs of CO joins.
**	The nature of CO joins prevents the use of role reversal and
**	some effort is expended in this module to assure that only the
**	original build source is used to load the hash table for CO
**	joins.
**
**	Building a hash table is complex because there are so many
**	cases to deal with.  Upon entry, we may have both sides of the
**	current partitioning level loaded, or just one.  If just one,
**	it's usually the build side, but might be the probe side (if
**	we recursed and chose the probe side to start with).  Some
**	of the partitions might fit in memory, or might not; and it
**	could be the build side that fit, or the probe side.  (But not
**	both;  if some partitions of the first side loaded fit in memory,
**	those partitions are built into a hash table and we never load
**	or spill the rows from the other side that hash to those
**	partitions.)  If just one side is loaded, partitions that spilled
**	may not have been flushed (so that the last block is still in
**	the partition buffers).
**
**	To build a hash table, there are again many possibilities.
**	- if the build side has in-memory (unspilled) partitions, use
**	them to build a hash table, plus whatever spilled partitions
**	we can fit in.
**	- If the build side has no in-memory partitions, but the probe
**	side does, do a role reversal and build a hash table out of those
**	probe partitions, plus any other probe partitions that fit.
**	- If there aren't any in-memory partitions, try to use available
**	partition buffer space to reload one or more spilled partitions.
**	Important note here: if just one side has been loaded, we can't
**	steal buffers from other spilled partitions.  We need those
**	buffers to load and maybe spill the probe side.  If we've loaded
**	both sides, than any spilled partition buffer is fair game.
**	NOTE that after loading the second side, spilled partition buffers
**	are always flushed!  either here, or in the caller.
**	- If we can't fit any build-side partitions, but we can reload
**	and fit some probe-side partitions, do a role reversal.
**	- If nothing fits, do a recursion (i.e. open a new level, choose
**	a partition, and subdivide it).
**
**	Notes to above discussion:
**	- CO joins can't do role reversal, and the code has to deal with
**	that limitation.
**	- I say "build" and "probe" side, but more accurate might be
**	"this" and "other" side.  We normally start with the build side,
**	since it's estimated to be smaller, but in the case of a recursion
**	we might start with the probe side if that was the side that was
**	initially repartitioned.
**
** Inputs:
**	dsh		- Pointer to thread's DSH
**	hbase		- ptr to base hash structure
**
** Outputs:
**
** Side Effects:
**
**
** History:
**	5-mar-99 (inkdo01)
**	    Written.
**	jan-00 (inkdo01)
**	    Changes for cartprod logic, various bug fixes.
**	28-jan-00 (inkdo01)
**	    Tidy up hash memory management.
**	8-feb-00 (inkdo01)
**	    Added logic for CO joins.
**	23-aug-01 (devjo01)
**	    Remove redundant and incorrect init of htable[0].
**	25-oct-01 (inkdo01)
**	    Fix bug with CO joins in which probe partitions fit in memory.
**	10-jan-02 (inkdo01)
**	    Fix minor bug in free chain management (106785).
**	13-mar-02 (inkdo01)
**	    Fix a bug when the free chain starts life empty.
**	28-mar-03 (inkdo01)
**	    Don't take "last block still in buffer" shortcut if FLUSHED
**	    flag is on for partition.
**	27-june-03 (inkdo01)
**	    "Last block in buffer" logic entered with faulty test that
**	    didn't work if block was almost full (bug 110498).
**	4-nov-03 (inkdo01)
**	    Another boundary condition involving overflowed partition and
**	    partition size that's a multiple of row size (bug 111226).
**	12-jan-04 (inkdo01)
**	    Fine tuning of previous fix - left out the "exact multiple" bit.
**	21-june-04 (inkdo01)
**	    Allocate hsp_bptr/bvptr in tandem to avoid later SEGVs.
**	14-july-04 (inkdo01)
**	    Add QEF_CHECK_FOR_INTERRUPT to avoid long loops in hash processing.
**	22-jul-04 (toumi01)
**	    A partition can be used even if it is an exact fit; this change
**	    keeps us from falling into a black hole looking for partitions.
**	3-Jun-2005 (schka24)
**	    Release files when a recursion level is done.  This reduces
**	    disk usage, as we'll be able to re-use the files if we return
**	    to that level later.  Also, copy back partition buffer addresses,
**	    since the just-completed level may have allocated more partition
**	    buffers (if some outer level partitions had nothing hash to them.)
**	8-Jun-2005 (schka24)
**	    Pitch out partitions that have both sources loaded, and one
**	    side is empty, and we don't need the other side for OJ.
**	    Not only does this eliminate useless partitions more quickly,
**	    but it also prevents am SC0206 "didn't need to recurse" error.
**	13-Jun-2005 (schka24)
**	    Rework partition selection.  Various comments notwithstanding,
**	    we were still grabbing partition buffers for the hash heap when
**	    they were needed to buffer/write probe rows.
**	    Fix bug related to 111226 above, a row can start at the beginning
**	    of the last-block-in-memory partition buffer even if rowsize isn't
**	    a multiple of the partition buffer size.
**	10-Jul-2005 (schka24)
**	    Last round of changes forgot to clear bufrows upon role reversal,
**	    often leading to QE0002.  Fix.
**	23-Dec-2005 (kschendel)
**	    Make sure that "firstbr" is cleared.  If last probe of prior
**	    hash table matched, it can be left on, pointing at garbage and
**	    confusing the first probe into the new hash.
**      23-Oct-2006 (huazh01)
**          check if both 'this' and 'other' side of partition are empty 
**          before calling qen_hash_insert(). This prevents setting 
**          'this_inmem', 'mustflip' ...., etc. to wrong value when there is 
**          actually nothing being inserted into the hash table. 
**          This fixes b116580.  
**           
**          
**	24-Apr-2006 (kschendel)
**	    Fix the test for when we've reached the last (in-memory) block
**	    when building spilled partitions.  Old test broke if build rows
**	    were very large (testcase had 3768 byte rows in a 16K chunk).
*/
 
DB_STATUS
qen_hash_build(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase)
{

    QEN_HASH_LINK	*hlink;
    QEN_HASH_PART	*hpptr;
    QEN_HASH_PART	*prev_hpptr;
    i4			hbucket, emptyps = 0;
    QEF_CB		*qcb = dsh->dsh_qefcb;
    QEF_HASH_DUP	*brptr, *hchain, *hprev;
    QEF_HASH_DUP	**htable;
    FREE_CHUNK		*free;
    FREE_CHUNK		*frptr;
    i4			bufrows, htsize, rowsz; 
    i4			rowcnt, hashrows, partmax, extra_rows;
    i4			smallest, smalli, chunkcnt;
    i4			offset, leftover, pmaxrows;
    i4			other_rowcnt;
    i4			split_row_adj;
    i4			i, j;
    i4			spillmask, other_spillmask, inhashmask, flushmask;
    i4			candidate;	/* BCANDIDATE or PCANDIDATE */
    PTR			bptr, source, target;
    DB_STATUS		status;
    bool		recurse;
    bool		mustflip, reverse, done, lastblock;
    bool		needoj;		/* Local-build side needed for OJ */
    bool		this_inmem, other_inmem;
    bool		readlast;
    bool		dontreverse = (hbase->hsh_nodep->hjn_kuniq);
					/* probe rows ineligible for hash */


    /* If hash table is empty, this is the call from qen_hjoin right 
    ** after the loading of the original build rows. Construct the 
    ** hash bucket array, then advance into the hash table build loop. */

    if (hbase->hsh_htable == NULL)
    {
	i4 numrows;

	numrows = hbase->hsh_brcount;
	rowsz = hbase->hsh_browsz;
	partmax = hbase->hsh_csize / rowsz;
					/* max rows in 1 partition buffer */
	bufrows = partmax * hbase->hsh_pcount;
					/* max rows in all part buffers 
					** (hash table capacity) */
	if (bufrows > numrows) bufrows = numrows;
	htsize = bufrows * 11 / 10;	/* add slack for hash dispersion */
	hbase->hsh_htsize = htsize;
	status = qen_hash_palloc(qcb, hbase,
			(htsize + 1) * sizeof(PTR),
			&htable,
			&dsh->dsh_error, "ht_ptrs");
	if (status != E_DB_OK)
	    return (status);
	hbase->hsh_htable = htable;
	hbase->hsh_flags |= HASH_NOHASHT;
    }	/* end of hash table construction */
    else 
    {
	htable = hbase->hsh_htable;
	htsize = hbase->hsh_htsize;
    }

    pmaxrows = (hbase->hsh_csize / hbase->hsh_prowsz) * hbase->hsh_pcount;
	
    for (i = 0; i < htsize; i++) htable[i] = NULL;
				/* (re)init the hash buckets */
    hbase->hsh_firstbr = NULL;		/* Clean out saved state */
    hbase->hsh_nextbr = NULL;

    /* Always start with the build side.  It's normally estimated to be
    ** the smaller one, unless we're in a CO join, in which case we have
    ** to build a hash table from the build side.
    */

    hbase->hsh_flags &= ~HASH_ROLE_REVERSE;

    /* Big loop oversees continued attempts to build the hash table. If
    ** any iteration fails because all available partitions at this level
    ** are too big to fit the hash table, a recursion is performed
    ** (by qen_hash_recurse) and we try again to build a hash table.
    ** Note that successive trips through the loop can be operating
    ** on different recursion levels.
    */

    do	/* hash table build loop - extends to end of function */
    {

	/* Set local variables, depending on current role reversal. */
	reverse = (hbase->hsh_flags & HASH_ROLE_REVERSE) != 0;
	hlink = hbase->hsh_currlink;
	free = NULL;

	if (dontreverse && reverse)
	{
	    /* Can't build a hash table with probe rows if
	    ** hjn_kuniq (CO join). */
	    reverse = FALSE;
	    hbase->hsh_flags &= ~HASH_ROLE_REVERSE;
	}

#ifdef xDEBUG
	/* Print some stuff */
	TRdisplay ("%@ Hash build of %s thread %d, level %d\n",
		reverse ? "probe" : "build",
		dsh->dsh_threadno, hlink->hsl_level);
	for (i = 0; i < hbase->hsh_pcount; i++)
	{
	    hpptr = &hlink->hsl_partition[i];
	    TRdisplay ("t%d: P[%d]: b:%d  p:%d, flags %v\n",
		dsh->dsh_threadno, i, hpptr->hsp_brcount, hpptr->hsp_prcount,
		"BSPILL,PSPILL,BINHASH,PINHASH,BFLUSHED,PFLUSHED,BALLSAME,PALLSAME,DONE",hpptr->hsp_flags);
	}
#endif

	mustflip = TRUE;		/* Will get turned off if not needed */
	other_inmem = FALSE;
	this_inmem = FALSE;		/* These are mutually exclusive */
	if (reverse)
	{
	    rowsz = hbase->hsh_prowsz;
	    spillmask = HSP_PSPILL;
	    other_spillmask = HSP_BSPILL;
	    inhashmask = HSP_PINHASH;
	    flushmask = HSP_PFLUSHED;
	    needoj = (hbase->hsh_flags & HASH_WANT_RIGHT) != 0;
	    mustflip = FALSE;		/* Can't flip if already flipped */
	    candidate = HSP_BCANDIDATE;
	}
	else
	{
	    rowsz = hbase->hsh_browsz;
	    spillmask = HSP_BSPILL;
	    other_spillmask = HSP_PSPILL;
	    inhashmask = HSP_BINHASH;
	    flushmask = HSP_BFLUSHED;
	    needoj = (hbase->hsh_flags & HASH_WANT_LEFT) != 0;
	    candidate = HSP_PCANDIDATE;
	}
	partmax = hbase->hsh_csize / rowsz;
	/* Split-row adjustment is not needed if rows exactly fit in
	** buffers.  See below.
	*/
	split_row_adj = 1;
	if (hbase->hsh_csize % rowsz == 0)
	    split_row_adj = 0;
	hashrows = 0;
	extra_rows = 0;
	done = TRUE;

	if (dontreverse) 
	{
	    mustflip = FALSE;
	    if (hlink->hsl_flags & HSL_BOTH_LOADED)
	    {
		/* Can't reverse, make sure that probe side is flushed,
		** even those that would normally stay in memory.
		*/
		status = qen_hash_flushco(dsh, hbase);
		if (status != E_DB_OK)
		    return(status);
	    }
	}
	recurse = TRUE;		/* Turned off if we can make a hash table */

	/* Usage note:
	** partmax = max "this side" rows in a partition buffer
	** rowsz = size of "this side" row in bytes
	** extra_rows = number of "this side" rows that we can put into
	**   unallocated or on-free-list buffers (or buffer pieces)
	** bufrows = max "this side" rows that could fit in all spilled
	**   partition buffers
	** pmaxrows = max probe rows that could fit in ALL partition buffers
	*/

	bufrows = 0;

	/* Loop once over partition descriptors, looking for partitions
	** that are:
	** - unused; (unallocated) buffer can be used for hash data.
	** - complete; (allocated) buffer can be used for hash data,
	**   put it on the free list.
	** - uninteresting (both sides loaded and "other" side is empty);
	**   buffer can be used for hash data, mark partition DONE and put
	**   its buffer on the free list.
	** - In memory; rows are immediately loaded into the hash table, and
	**   any remainder space in the buffer is put on the free list.
	** This is a separate pass because we want to see if there's any
	** extra buffer space that can be used for holding rows from
	** spilled partitions.
	*/
	for (i = 0; i < hbase->hsh_pcount; i++)
	{
	    hpptr = &hlink->hsl_partition[i];
	    hpptr->hsp_flags &= ~(HSP_BCANDIDATE | HSP_PCANDIDATE);
	
	    /* Skip finished, overflowed and empty partitions. */
	    if (hpptr->hsp_bptr == NULL) 
	    {
		/* Completely empty partition, never used */
		emptyps++;
		extra_rows += partmax;
		continue;
	    }
	    if (hpptr->hsp_flags & (HSP_DONE | inhashmask))
	    {
		/* Shouldn't see "inhashmask" here?  but if we do, it's
		** from last time, so it's done.
		*/
#ifdef xDEBUG
		if ((hpptr->hsp_flags & HSP_DONE) == 0)
		    TRdisplay("t%d pno %d was in-hash? is done\n",
			dsh->dsh_threadno, i);
#endif
		/* Add partition buffer to free chain. */
		((FREE_CHUNK *)hpptr->hsp_bptr)->free_next = free;
		free = (FREE_CHUNK *)hpptr->hsp_bptr;
		free->free_rows = partmax;
		extra_rows += partmax;
		continue;
	    }
	    /* Check out partition to see if it's useless.  We can ditch
	    ** the partition if the "other" side is empty, and if we know
	    ** that both sources were loaded, and if we don't need "this"
	    ** side for outer join.
	    ** (The "both loaded" test is needed so that we don't
	    ** drop initial or freshly-recursed partitions that
	    ** we haven't had a chance to load "other" side rows into.)
	    */
	    other_rowcnt = (reverse) ? hpptr->hsp_brcount : hpptr->hsp_prcount;
	    if (hlink->hsl_flags & HSL_BOTH_LOADED && !needoj && other_rowcnt == 0)
	    {
		/* This is a now-useless partition, make it look
		** DONE, free up disk, and add its buffer to the
		** free chain.
		** P1P2 in-between phase should take care of this for
		** top level, but this code might still be useful for
		** recursion levels.
		*/
#ifdef xDEBUG
		TRdisplay("t%d dropping p[%d] brcount %d prcount %d\n",
			dsh->dsh_threadno, i, hpptr->hsp_brcount,
			hpptr->hsp_prcount);
#endif
		hpptr->hsp_flags = HSP_DONE;
		status = qen_hash_close(hbase,
			    (reverse) ? &hpptr->hsp_pfile : &hpptr->hsp_bfile,
			    CLOSE_DESTROY);
		if (status != E_DB_OK)
		    return (status);
		((FREE_CHUNK *)hpptr->hsp_bptr)->free_next = free;
		free = (FREE_CHUNK *)hpptr->hsp_bptr;
		free->free_rows = partmax;
		extra_rows += partmax;
		continue;
	    }
	    /* Skip spilled partitions, we'll deal with them next loop.
	    ** Do count their potential space contribution, though.
	    */
	    if (hpptr->hsp_flags & spillmask)
	    {
		/* Don't include partition if other side is sitting in the
		** buffer and didn't spill.  We'll want to role-reverse,
		** either this pass or next time 'round.
		*/
		if (other_rowcnt > 0 && (hpptr->hsp_flags & other_spillmask) == 0)
		    other_inmem = TRUE;
		else
		    bufrows += partmax;		/* Adjusted below */
		continue;
	    }

	    /* We get here for partitions with rows, but which didn't
	    ** spill to disk. They can be loaded, in place, into the 
	    ** hash table. */

	    rowcnt = (reverse) ? hpptr->hsp_prcount : hpptr->hsp_brcount;
	    if (rowcnt == 0 &&
	      other_rowcnt > 0 && (hpptr->hsp_flags & other_spillmask) == 0)
	    {
		/* Actually, this partition is empty, but the other side isn't.
		** Don't mess with this partition, we may go thru a role
		** reversal.
		*/
		other_inmem = TRUE;
		continue;
	    }

            /* b116580: 
            ** continue looping over to the next partition buffer
            ** if both sides of the current partition has no rows.
            */  
            if (rowcnt == 0 && other_rowcnt == 0)
	       continue; 

	    hbase->hsh_flags &= ~HASH_NOHASHT;	/* allow loop exit */
	    done = FALSE;
	    recurse = FALSE;			/* Recursion not needed */
	    mustflip = FALSE;			/* nor reversal */
	    this_inmem = TRUE;
	    ++ hbase->hsh_memparts;		/* for stats */
	    hashrows += rowcnt;
	    hpptr->hsp_flags |= inhashmask;	/* part'n in hash tble */
	    brptr = (QEF_HASH_DUP *)hpptr->hsp_bptr;  /* 1st row */

	    /* Now loop over rows in partition buffer, adding each to
	    ** hash table. */
	    for (j = 0; j < rowcnt; j++)
	    {
		status = qen_hash_insert(dsh, hbase, htable, brptr);
		if (status != E_DB_OK) 
				return(status);

		brptr = (QEF_HASH_DUP *)&((char *)brptr)[rowsz];
					/* next row in partition buffer */
	    }	/* end of partition buffer loop */

/* qen_hash_verify(hbase); */

	    /* If there is still more space in this block, add to a free chain
	    ** to permit space to load disk resident partitions.
	    ** (Make sure there's space not just for a row, but for a
	    ** FREE_CHUNK!)
	    */
	    if ((partmax - rowcnt) * rowsz > sizeof(FREE_CHUNK))
	    {
		((FREE_CHUNK *)brptr)->free_next = free;
		free = (FREE_CHUNK *)brptr;
		free->free_rows = partmax - rowcnt;
		extra_rows += partmax - rowcnt;
	    }

	}	/* end of first partition loop */

	/* If we didn't start a hash table on this side from in-memory
	** partitions, perhaps there are in-memory partitions on the other
	** side -- if so, let's do a role reversal.
	** Because CO join does "flushco" above, we can't have other_inmem
	** set if dontreverse is set.  CO joins don't allow role reversal.
	*/
	if (! this_inmem && other_inmem)
	{
	    hbase->hsh_flags ^= HASH_ROLE_REVERSE;	/* Flip it */
	    continue;				/* Try again this way */
	}

	/* Make another pass thru the partitions looking for spilled
	** partitions to load, if possible.  We'll mark as candidates
	** partitions that might fit.
	** Note that there might not be any spilled partitions, in
	** which case we make a couple pointless loops thru the
	** partitions -- FIXME minor performance goof.
	*/

	/* If we haven't loaded both sides of this level, we can't use
	** spilled partition buffers for hash data.  The reason is that
	** unless we can manage to squeeze the partition into "extra"
	** space (empty partitions, done partitions, etc), we can't join
	** probe rows that map to spilled partitions, and we'll need the
	** partition buffers to write the probe rows!  It's probably a
	** bad idea to use a partition for build heap and probe output
	** simultaneously...
	**
	** We can however potentially use a candidate partition's buffer
	** (but no others).  So we can include one partition's worth in
	** the available space.  To be conservative, subtract off one
	** row (unless rows exactly fit in a buffer).  If the last block
	** of a chosen partition is in the buffer, and the front of that
	** last block is a split row, we might only be able to fit N-1
	** rows into the buffer rather than N.
	*/

	if ((hlink->hsl_flags & HSL_BOTH_LOADED) == 0)
	{
	    bufrows = partmax - split_row_adj;	/* Can't steal other buffers */
	}

#ifdef xDEBUG
	TRdisplay("t%d extra_rows %d, emptyps %d (%d rows), partmax %d, bufrows %d, pmaxrows %d\n",
		dsh->dsh_threadno, extra_rows, emptyps, emptyps*partmax,
		partmax, bufrows, pmaxrows);
#endif

	/* Now, do a loop over spilled partitions.  It's possible that
	** we might be able to fit some of them into the hash table.
	** A partition will fit if we can squeeze it into "extra" space,
	** which includes (if available) all of the spilled partition
	** buffers, including ours.
	*/
	for (i = 0; i < hbase->hsh_pcount; i++)
	{
	    hpptr = &hlink->hsl_partition[i];
	    /* Skip empty, done */
	    rowcnt = (reverse) ? hpptr->hsp_prcount : hpptr->hsp_brcount;
	    if (hpptr->hsp_bptr == NULL || (hpptr->hsp_flags & HSP_DONE)
	      || rowcnt == 0)
		continue;
	    if ((hpptr->hsp_flags & spillmask) == 0)
		continue;
	    /* Got a spilled partition that's nonempty on the side
	    ** we're interested in.  See if we can fit it into the
	    ** space we're allowed to use.
	    */
	    done = FALSE;
	    if (rowcnt <= extra_rows + bufrows)
	    {
		/* Partition fits on "this" side */
		recurse = FALSE;
		mustflip = FALSE;		/* Don't need to flip */
		hpptr->hsp_flags |= candidate;
	    }
	    /* Perhaps we could do a role reversal and fit the partition.
	    ** The space test is different for role reversal:  if it's
	    ** allowed at all, we're going to flush all partition buffers,
	    ** and there won't be any probe-side rows in memory, so the
	    ** test is "does the partition fit in the total heap space".
	    */
	    if (!dontreverse &&
		hlink->hsl_flags & HSL_BOTH_LOADED &&
		hpptr->hsp_prcount > 0 &&
		hpptr->hsp_prcount <= pmaxrows)
	    {
		/* Partition can fit on the probe side. */
		recurse = FALSE;
		hpptr->hsp_flags |= HSP_PCANDIDATE;
	    }
	    /* If both sides loaded, we can use arbitrary partition bufs for
	    ** the hash table.  In this case, the partition buf should be
	    ** flushed, so link it into the free chain.
	    ** NOTE: we could be trickier about not flushing after loading
	    ** the "other" side, eg after the repartition call below, but
	    ** it would require great care to keep straight which side was
	    ** in the buffers.  a FIXME for later.
	    */
	    if (hlink->hsl_flags & HSL_BOTH_LOADED)
	    {
		if ((hpptr->hsp_flags & flushmask) == 0)
		{
		    TRdisplay("%@ qen_hash_build: pno %d at level %d is both-loaded but not %s-flushed\n",
			i,hlink->hsl_level,reverse ? "probe" : "build");
		    dsh->dsh_error.err_code = E_QE0002_INTERNAL_ERROR;
		    return (E_DB_ERROR);
		}
		((FREE_CHUNK *)hpptr->hsp_bptr)->free_next = free;
		free = (FREE_CHUNK *)hpptr->hsp_bptr;
		free->free_rows = partmax;
		/* Don't add into extra_rows, already did */
	    }
	} /* end of spilled partition loop */

	if (done)
	{
	    if (hlink->hsl_level == 0)
	    {
		/* We appear to be finished! */
		dsh->dsh_error.err_code = E_QE0015_NO_MORE_ROWS;
		return(E_DB_WARN);
	    }
	    /* Done with this recursion level, clean it up and
	    ** return to the previous level.
	    */
	    /* Close all the files at the completed recursion level, but
	    ** retain them -- chances are that we'll be back, unless
	    ** hashing was very uneven.
	    ** Also, copy back the partition buffer addresses, just in case
	    ** we allocated more.  (We want to re-use those buffers if this
	    ** recursion level is re-entered later.)
	    */
#ifdef xDEBUG
	    TRdisplay("%@ Exiting hash recursion level %d for pno %d, thread %d\n",
		hlink->hsl_level, hlink->hsl_prev->hsl_pno, dsh->dsh_threadno);
#endif
	    prev_hpptr = &hlink->hsl_prev->hsl_partition[hbase->hsh_pcount-1];
	    for (hpptr = &hlink->hsl_partition[hbase->hsh_pcount-1];
		 hpptr >= &hlink->hsl_partition[0];
		 --hpptr, --prev_hpptr)
	    {
		if (hpptr->hsp_bfile != NULL)
		{
		    status = qen_hash_close(hbase, &hpptr->hsp_bfile,
						CLOSE_RELEASE);
		    if (status != E_DB_OK) return(status);
		}
		if (hpptr->hsp_pfile != NULL)
		{
		    status = qen_hash_close(hbase, &hpptr->hsp_pfile,
						CLOSE_RELEASE);
		    if (status != E_DB_OK) return(status);
		}
		prev_hpptr->hsp_bptr = hpptr->hsp_bptr;
		prev_hpptr->hsp_bvptr = hpptr->hsp_bvptr;
	    }
	    /* reset to previous recursion level */
	    hbase->hsh_currlink = hlink = hlink->hsl_prev;
	    hpptr = &hlink->hsl_partition[hlink->hsl_pno];
	    hpptr->hsp_flags = HSP_DONE;
	    /* Don't need partition files, close them (if any) */
	    i = CLOSE_RELEASE;
	    if (hlink->hsl_level == 0)
		i = CLOSE_DESTROY;
	    status = qen_hash_close(hbase, &hpptr->hsp_bfile, i);
	    if (status == E_DB_OK)
		status = qen_hash_close(hbase, &hpptr->hsp_pfile, i);
	    if (status != E_DB_OK) 
		return(status);
	    continue;			/* Restart at the now-current level */
	}

	if (recurse) 
	{
	    /* Couldn't build a hash table from current partition set.
	    **
	    ** If we haven't loaded both sides of this level, do so now.
	    ** If we're at the top, return.  (No probes have been loaded
	    ** at all, so load and partition the probes.)
	    ** If we're in a recursion, pull the probes out of the probe
	    ** side of the partition being recursed, and repartition them.
	    **
	    ** If we HAVE loaded both sides of this level, all the partitions
	    ** on both sides are too large.  (Or perhaps the build side
	    ** is too large and it's a check-only join, that can't be
	    ** flipped.)  Do a recursion, which will pick a partition
	    ** and repartition it into (hopefully!) smaller chunks that
	    ** can be joined.
	    **
	    ** Before all that, though, the current partition buffers must
	    ** be written to disk and the files closed. */ 

	    status = qen_hash_flush(dsh, hbase, FALSE);
	    if (status != E_DB_OK) 
		return(status);

	    if ((hlink->hsl_flags & HSL_BOTH_LOADED) == 0)
	    {
		/* We've loaded one side and can't make a hash table.
		** If top level, this is first call, just load the probe
		** side (for the first time).
		*/

		if (hlink->hsl_level == 0)
		    return (E_DB_OK);

		/* Just did recursion and first repartition. If we're 
		** back here, we still couldn't build hash table, so 
		** must repartition other source.
		** After repartitioning, try again to build a hash table
		** at this new recursion level, starting with the build
		** side.  (CO joins have to run that direction, and for
		** other joins, we'll do all the usual role reversal
		** machinery as appropriate.)
		** Turn on flag BEFORE call to repartition so it knows
		** that this is repartition of the other side.
		*/
		hlink->hsl_flags |= HSL_BOTH_LOADED;
		/* Flip role reversal so that we repartition the "other"
		** side of the previous level's recursed partition.
		*/
		hbase->hsh_flags ^= HASH_ROLE_REVERSE;	/* Flip it */
		status = qen_hash_repartition(dsh, hbase, 
			&hlink->hsl_prev->hsl_partition[hlink->
				hsl_prev->hsl_pno]);
		/* Flush the side we just loaded.  We don't have the machinery
		** to keep careful enough track of which side's in the
		** buffers.
		*/
		if (status == E_DB_OK) 
		    status = qen_hash_flush(dsh, hbase, FALSE);
		if (status != E_DB_OK) 
		    return(status);
		/* Reset back to the build side, try again. */
		hbase->hsh_flags &= ~HASH_ROLE_REVERSE;
	    }
	    else
	    {
		/* First time on this partition. Recurse and repartition
		** the smaller of the sources.
		** Leave role-reversal set whichever way recursion sets
		** it, since that's the side that's built.  Maybe the
		** recursed side can build a hash table at the new level.
		*/
		status = qen_hash_recurse(dsh, hbase);
		if (status != E_DB_OK) 
		    return(status);
	    }

	    hbase->hsh_flags |= HASH_NOHASHT;
	    if (hbase->hsh_flags & HASH_CARTPROD) return(E_DB_OK);
					/* next part is an "all same"
					** and requires CP joining */
	    continue;
	}

	/* We get here if we've either started a hash table (with memory
	** resident partitions), or there are disk resident partitions 
	** small enough to fit into the hash table. In any event, the 
	** next step is to loop over the remaining partitions, looking for
	** the largest one on disk which will fit into the remaining 
	** space in the hash table. Its blocks are then read back, and 
	** its rows are added to the table. This process continues until
	** no more of the remaining partitions will fit into the hash
	** table.
	*/

	if (mustflip)
	{
	    /* None of current source partitions could generate a hash
	    ** table. However, the other source can make a hash table
	    ** from this partitioning. Flush partition buffers of
	    ** current source to disk and close files. Then reverse
	    ** roles and drop through to next stage (hash table build 
	    ** from disk resident partitions).
	    ** Note that "mustflip" is disallowed for CO joins.
	    ** Note also that due to the way the does-spilled-partition-
	    ** fit test is coded, above, "mustflip" is really a one-way
	    ** deal.  We might flip build to probe, but never probe to
	    ** build.
	    */


	    /* Write last blocks to disk and close files. */
	    status = qen_hash_flush(dsh, hbase, FALSE);
	    if (status != E_DB_OK) 
		return(status);

	    /* We were originally not "reverse", reverse now */
	    reverse = TRUE;
	    hbase->hsh_flags |= HASH_ROLE_REVERSE;
	    rowsz = hbase->hsh_prowsz;
	    spillmask = HSP_PSPILL;
	    inhashmask = HSP_PINHASH;
	    flushmask = HSP_PFLUSHED;
	    candidate = HSP_PCANDIDATE;
	    partmax = hbase->hsh_csize / rowsz;

	    /* Because there weren't any memory resident partitions,
	    ** and because at this point we clearly have both "sides" loaded,
	    ** we can just throw away the free chain and rebuild it
	    ** to include each and every partition buffer in its entirety.
	    */
	    free = NULL;
	    extra_rows = 0;		/* start over, clear all counters */
	    emptyps = 0;
	    bufrows = 0;
	    for (hpptr = &hlink->hsl_partition[hbase->hsh_pcount-1];
		 hpptr >= &hlink->hsl_partition[0];
		 -- hpptr)
	    {
		if (hpptr->hsp_bptr == NULL)
		    ++ emptyps;
		else
		{
		    ((FREE_CHUNK *)hpptr->hsp_bptr)->free_next = free;
		    free = (FREE_CHUNK *)hpptr->hsp_bptr;
		    free->free_rows = partmax;
		}
		extra_rows += partmax;
	    }

	}	/* end of mustflip */

	/* There are now disk resident partitions small enough to fit
	** the remaining space in the current hash table heap. Nested
	** loops are executed to search successively for the smallest
	** such partition which will fit, add it to the table and keep
	** trying until no more will fit (or there are no more 
	** partitions).
	** As noted above, we may find that there aren't any spilled
	** partitions, in which case we take a harmless spin thru the
	** partition array and declare victory.
	*/

	/* If we are allowed to steal anyone's partition bufs, just
	** include spilled bufs into the total available which gets
	** counted down.
	** (Otherwise, bufrows remains what we can fit into just one
	** partition buffer.)
	*/
	if (hlink->hsl_flags & HSL_BOTH_LOADED)
	{
	    extra_rows += bufrows;
	    bufrows = 0;
	}

	for ( ; ; )	/* loop 'til no more fit */
	{
	    i4 blocksleft;
	    bool grabbed_last;

	    if (QEF_CHECK_FOR_INTERRUPT(qcb, dsh) == E_DB_ERROR)
		return (E_DB_ERROR);

	    smallest = MAXI4;
	    smalli = -1;

	    /* First loop to find next qualifying disk partition.
	    ** We'll pick the smallest candidate that fits.
	    */
	    for (i = 0; i < hbase->hsh_pcount; i++)
	    {
		hpptr = &hlink->hsl_partition[i];
		/* Only look at partitions marked as candidates */
		if ((hpptr->hsp_flags & candidate) == 0)
		    continue;
		/* We can't necessarily fit all the candidates.  Can we
		** still fit this one?
		*/
		rowcnt = (reverse) ? hpptr->hsp_prcount 
						: hpptr->hsp_brcount;
		if (rowcnt < smallest && rowcnt <= extra_rows+bufrows)
		{
		    /* New smallest. */
		    smallest = rowcnt;
		    smalli = i;
		}
	    }

	    if (smalli < 0) break;	/* no more found */

	    /* "smalli" is the next partition to be added to the hash
	    ** table. It must be opened, read and its rows added to the
	    ** current hash table (after we move them to an available
	    ** row slot in the hash table heap). 
	    **
	    ** Gobs of overlapping, tangled logic is required to keep
	    ** track of position in the read buffer, locate the next 
	    ** available row slot (sometimes requiring a whole new 
	    ** cluster buffer to be allocated) and copy whole and part
	    ** rows to the right spot. */

	    hbase->hsh_flags &= ~HASH_NOHASHT;	/* allow loop exit */
	    hpptr = &hlink->hsl_partition[smalli];
	    rowcnt = smallest;

	    /* Compute number of blocks to read including the last one
	    ** (which might be in memory already).  Don't use partmax!
	    ** Rows in the spill file split blocks, partmax doesn't.
	    ** An i4 block count is OK as long as an i4 row count is OK.
	    ** Round up to full blocks.
	    */
	    blocksleft = (i4) ( ((i8)rowcnt * (i8)rowsz + hbase->hsh_csize-1 )
					/ (i8) hbase->hsh_csize);

	    status = qen_hash_close(hbase, 
		(reverse) ? &hpptr->hsp_pfile : &hpptr->hsp_bfile, 0);
	    if (status != E_DB_OK) 
			    return(status);
	    status = qen_hash_open(dsh, hbase, hpptr, OPEN_READ);
	    if (status != E_DB_OK) 
			    return(status);

	    /* Set up heap/buffer variables. */
	    extra_rows -= rowcnt;
	    brptr = (QEF_HASH_DUP *)free;
	    if (free)
	    {
		chunkcnt = free->free_rows;
		free = free->free_next;
#ifdef xDEBUG
		TRdisplay("t%d use(1) %d rows %p..%p\n",
			dsh->dsh_threadno,chunkcnt,brptr,(char *)brptr+chunkcnt*rowsz);
#endif
	    }
	    else 
		chunkcnt = 0;

	    offset = hbase->hsh_csize;
	    leftover = 0;
	    lastblock = FALSE;
	    grabbed_last = FALSE;
	    bptr = hbase->hsh_rbptr;
	    readlast = (hpptr->hsp_flags & flushmask) != 0;
	    /* If mustflip, we flushed and flushmask is looking at the
	    ** wrong side anyway, must read last block
	    */
	    if (mustflip)
		readlast = TRUE;

/* qen_hash_verify(hbase); */
	    while (rowcnt > 0)	/* loop to load all rows in partition */
	    {
		if (offset >= hbase->hsh_csize)
		{
		    /* Need another partition block. */

/* qen_hash_verify(hbase); */
		    if (!readlast && blocksleft == 1)
		    {
			/* Last block - it's still in partition buffer. */
			source = hpptr->hsp_bptr;
			lastblock = TRUE;

			/* Add rest of current free chunk being used for heap 
			** space back to free chain.
			** If split row, we haven't finished processing it,
			** so for that case need chunkcnt gtr 1.
			*/
			if (chunkcnt > 0
			  && (leftover == 0 || chunkcnt > 1) )
			{
 
			    if (leftover > 0) 
			    {
				frptr = (FREE_CHUNK *)&((char *)brptr)[rowsz];
					/* brptr hasn't been advanced yet */
				chunkcnt--;
			    }
			    else frptr = (FREE_CHUNK *)brptr;
			    frptr->free_next = free;
			    free = frptr;
			    free->free_rows = chunkcnt;
			}
			/* If not finishing a split row, row starts at
			** front of partition buffer.
			*/
			if (leftover == 0)
			    brptr = (QEF_HASH_DUP *)hpptr->hsp_bptr;
		    }
		    else
		    {
			/* Otherwise, reread the block from disk. */
			status = qen_hash_read(hbase, (reverse) ? 
			    hpptr->hsp_pfile : hpptr->hsp_bfile, 
			    hbase->hsh_rbptr, &hpptr->hsp_currbn);
			if (status != E_DB_OK) 
					return(status);
			source = hbase->hsh_rbptr;
		    }
		    --blocksleft;
		    offset = 0;
		}

		if (leftover > 0)
		{
		    /* Current row spanned 2 blocks. Copy the rest here. */
		    MEcopy(source, leftover, target);
		    offset = leftover;
		}

		else
		{
		    /* Copy whole row. But first, assure there's enough
		    ** hash table heap space. */
		    if (chunkcnt <= 0 && !lastblock)
		    {
/* qen_hash_verify(hbase); */
			/* First check free chain. */
			if (free != NULL)
			{
			    brptr = (QEF_HASH_DUP *)free;
			    chunkcnt = free->free_rows;
			    free = free->free_next;
#ifdef xDEBUG
			    TRdisplay("t%d use(2) %d rows %p..%p\n",
				dsh->dsh_threadno,chunkcnt,brptr,(char *)brptr+chunkcnt*rowsz);
#endif
			}
			else if (emptyps > 0)
			{
			    /* use a partition which is totally unused */
			    emptyps--;
			    status = qen_hash_palloc(qcb, hbase,
					hbase->hsh_csize + hbase->hsh_bvsize,
					&brptr,
					&dsh->dsh_error, "xtr_part_buf+bv");
			    if (status != E_DB_OK)
				return (status);

			    /* Now find empty buffer slot to point to 
			    ** this buffer. */
			    for (i = 0; i < hbase->hsh_pcount; i++)
			    {
				if (hlink->hsl_partition[i].hsp_bptr == NULL)
				{
				    chunkcnt = partmax;
				    hlink->hsl_partition[i].hsp_bptr = (PTR)brptr;
				    /* bit filter follows partition buffer */
				    hlink->hsl_partition[i].hsp_bvptr =
					&hlink->hsl_partition[i].hsp_bptr[hbase->hsh_csize];
				    break;
				}
			    }
			}
			else if (!readlast && !grabbed_last)
			{
			    i8 offs;

			    /* Can use leftover in our partition buf.  Unlike
			    ** the hash heap, the dumped partition splits
			    ** bufferfuls, so don't use partmax!  watch out
			    ** for i4 overflow in <numrows * rowsize>
			    ** Note: this case must be a last resort so that
			    ** we can handle free space properly when this
			    ** partition is done.
			    */
			    offs = ((i8) smallest * (i8) rowsz);
			    i = (i4) (offs % (i8) hbase->hsh_csize);
			    brptr = (QEF_HASH_DUP *) (&hpptr->hsp_bptr[i]);
			    chunkcnt = (hbase->hsh_csize - i) / rowsz;
			    /* Remember not to free rest of this buf! */
			    grabbed_last = TRUE;
			}
			else
			{
			    /* Beeeeg trouble */
			    TRdisplay("%@ qen_hash_build: no hash heap left, %d rows to do\n",rowcnt);
			    dsh->dsh_error.err_code = E_QE0002_INTERNAL_ERROR;
			    return(E_DB_ERROR);
			}
			target = (PTR)brptr;
		    }	/* end of "need more free space" */

		    /* If not processing the last partition block, 
		    ** copy whole row to hash table heap. Otherwise, 
		    ** row is processed in place. */
		    if (lastblock) offset += rowsz;
		    else
		    {
			source = (PTR)&((char *)bptr)[offset];
			target = (PTR)brptr;
			if (hbase->hsh_csize - offset < rowsz)
				leftover = hbase->hsh_csize - offset;
			else leftover = rowsz;
			MEcopy(source, leftover, target);
			offset += leftover;
			if (leftover < rowsz)
			{
			    /* Not a whole row. Update target pointer &
			    ** loop to pick up next partition block. */
			    target = (PTR)&((char *)target)[leftover];
			    leftover = rowsz - leftover;
			    continue;
			}
		    }
		}	/* end of !(leftover > 0) */
		
		/* Finally, we have a whole row in hash space. Load it to
		** the hash table, then fiddle pointers & counters. */
		status = qen_hash_insert(dsh, hbase, htable, brptr);
		if (status != E_DB_OK) 
				return(status);
		hashrows++;
		if (!lastblock) chunkcnt--;
		leftover = 0;
		rowcnt--;
		if (lastblock && offset <= rowsz) 
			brptr = (QEF_HASH_DUP *)&((char *)hpptr->hsp_bptr)[offset];
		else brptr = (QEF_HASH_DUP *)&((char *)brptr)[rowsz];
	    }	/* end of partition row insertion loop */

	    /* Done this partition - close file & change flags accordingly. */
	    i = CLOSE_RELEASE;
	    if (hlink->hsl_level == 0)
		i = CLOSE_DESTROY;
	    status = qen_hash_close(hbase, (reverse) ? 
			&hpptr->hsp_pfile : &hpptr->hsp_bfile, i);
	    if (status != E_DB_OK) 
			    return(status);
	    hpptr->hsp_flags &= ~(spillmask | candidate | flushmask);
	    hpptr->hsp_flags |= inhashmask;

	    if (!lastblock)
	    {
		/* Add rest of normal (spilled/flushed) buffer to free chain */
		if (chunkcnt)
		{
		
		    ((FREE_CHUNK *)brptr)->free_next = free;
		    free = (FREE_CHUNK *)brptr;
		    free->free_rows = chunkcnt;
		}
	    }
	    else if (!grabbed_last && hbase->hsh_csize - offset >= rowsz)
	    {
		/* Add rest of nonflushed buffer (holding last block) to free
		** unless we already grabbed it.  (in which case any leftover
		** was freed when we reached the lastblock code.)
		*/
		((FREE_CHUNK *)brptr)->free_next = free;
		free = (FREE_CHUNK *)brptr;
		free->free_rows = (hbase->hsh_csize - offset) / rowsz;
	    }
		
	}	/* end of loop to cram disk partitions into hash table */

    }	/* end of big loop */
    while (hbase->hsh_flags & HASH_NOHASHT);

    hbase->hsh_currows_left = 0;
    status = qen_hash_flush(dsh, hbase, FALSE);
			/* flush any remaining open files */
/* qen_hash_verify(hbase); */
#ifdef xDEBUG
	/* Print some stuff */
	TRdisplay ("%@ Completed build of %s thread %d, level %d, %d rows\n",
		reverse ? "probe" : "build",
		dsh->dsh_threadno, hbase->hsh_currlink->hsl_level,hashrows);
	for (i = 0; i < hbase->hsh_pcount; i++)
	{
	    hpptr = &hbase->hsh_currlink->hsl_partition[i];
	    TRdisplay ("t%d: P[%d]: b:%d  p:%d, flags %v\n",
		dsh->dsh_threadno, i, hpptr->hsp_brcount, hpptr->hsp_prcount,
		"BSPILL,PSPILL,BINHASH,PINHASH,BFLUSHED,PFLUSHED,BALLSAME,PALLSAME,DONE",hpptr->hsp_flags);
	}
#endif


    return(status);
}


/*{
** Name: QEN_HASH_INSERT	- insert row into hash table
**
** Description:
**	Rows of build source are simply inserted into correct bucket
**	chain of hash table.
**
** Inputs:
**	dsh		- Pointer to thread's DSH
**	hbase		- ptr to base hash structure
**	htable		- Base of hash pointer array
**	brptr		- Pointer to build row to insert
**
** Outputs:
**
** Side Effects:
**
**
** History:
**	5-mar-99 (inkdo01)
**	    Written.
**	8-may-01 (inkdo01)
**	    Changed to sort on hashno, then join keys.
*/
 
static DB_STATUS
qen_hash_insert(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase,
QEF_HASH_DUP	**htable,
QEF_HASH_DUP	*brptr)

{
    u_i4	hashno;
    i4		hbucket, cmpres;
    QEF_HASH_DUP *hchain, *hprev;
    ADF_CB	*adfcb = dsh->dsh_adf_cb;
    DB_STATUS	status;


    /* Compute bucket number (from hash number). Then insert into 
    ** bucket's chain in sorted sequence. */

    hashno = *((u_i4 *)brptr);	/* load hash key from row */
    hbucket = hashno % hbase->hsh_htsize;
    if (htable[hbucket] == NULL)
    {
	/* if 1st row in bucket */
	htable[hbucket] = brptr;
	brptr->hash_next = (QEF_HASH_DUP *)NULL;
    }
    else
    {
	DB_CMP_LIST *keylist = hbase->hsh_nodep->hjn_cmplist;

	/* Add row to list of dups in this bucket, but do it
	** in sorted sequence on join key. */
	adfcb->adf_errcb.ad_errcode = 0;
	for (hchain = htable[hbucket], hprev = NULL;
		hchain != NULL; 
		hprev = hchain, hchain = hchain->hash_next)
	{
	    if (hchain->hash_number < hashno) 
		continue;
	    if (hchain->hash_number == hashno)
	    {
		/* Hash number match, now do key compares. */
		cmpres = adt_sortcmp(adfcb, keylist,
		    hbase->hsh_kcount, &(brptr->hash_key[0]), 
		    &(hchain->hash_key[0]), 0);
		if (adfcb->adf_errcb.ad_errcode != 0)
		{
		    dsh->dsh_error.err_code = adfcb->adf_errcb.ad_errcode;
		    return(E_DB_ERROR);
		}
	    }
	    else cmpres = -1;
	    if (cmpres <= 0)	/* found our spot? */
	    {
		brptr->hash_next = hchain;
		if (hprev) hprev->hash_next = brptr;
		else htable[hbucket] = brptr;
		break;		/* insertion done */
	    }
	}

	if (hchain == NULL)
	{
	    /* We fell off chain - our guy must belong at end. */
	    brptr->hash_next = NULL;
	    if (hprev) hprev->hash_next = brptr;
	    else htable[hbucket] = brptr;
				/* else shouldn't happen */
	}
    }	/* end of hash table insertion */

    return(E_DB_OK);
}


/*{
** Name: QEN_HASH_RECURSE - recursively partitions a single partition
**			too large to fit the hash table
**
** Description:
**	Another partitioning recursion level is pushed onto the "link
**	stack" and the rows of the source partition are repartitioned.
**
** Inputs:
**	dsh		- Pointer to thread's DSH
**	hbase		- ptr to base hash structure
**
** Outputs:
**
** Side Effects:
**
**
** History:
**	7-apr-99 (inkdo01)
**	    Written.
**	jan-00 (inkdo01)
**	    Changes for cartprod logic, bug fixes.
**	28-jan-00 (inkdo01)
**	    Tidy up hash memory management.
**	1-Jun-2005 (schka24)
**	    When recursing on a CO join, we must repartition the build
**	    side, not the smaller side.
**	    Copy partition bufs from previous level, not top.
**	31-may-07 (hayke02 for schka24)
**	    Split HCP_BOTH into HCP_IALLSAME and HCP_OALLSAME. This change
**	    fixes bug 118200.
**
*/
 
static DB_STATUS
qen_hash_recurse(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase)
{

    QEN_HASH_LINK	*hlink = hbase->hsh_currlink;
    QEN_HASH_LINK	*prevlink;
    QEN_HASH_PART	*hpptr;
    i4			start, bpsize, ppsize, rowsz, rowcount;
    i4			i, partmax;
    QEF_CB		*qcb = dsh->dsh_qefcb;
    DB_STATUS		status;


    /* Locate next partition in current recursion level bigger than
    ** hash table. Build new recursion level (if not already there)
    ** open partition file and repartition its rows into the new 
    ** partition set. */

    start = (hlink->hsl_pno == -1) ? 0 : hlink->hsl_pno+1;
				/* start of partition search */

    /* FIXME Try to pick the smallest, so as to get it out of the
    ** way.  This might open up enuf space at the calling level
    ** to avoid/reduce additional recursion.
    ** will need fix to the "start" business...
    ** (schka24) I might be talking nonsense here, need to reconsider.
    */
    for (i = start; i < hbase->hsh_pcount; i++)
    {
	if (hlink->hsl_partition[i].hsp_flags & (HSP_BSPILL | HSP_PSPILL))
	    break;
    }

    if (i >= hbase->hsh_pcount)
    {
	/* A recurseable partition wasn't found. This is an error. */
	TRdisplay("%@ qen_hash_recurse: no spilled partition found\n");
	dsh->dsh_error.err_code = E_QE0002_INTERNAL_ERROR;
	return(E_DB_ERROR);
    }

    /* This should be the partition we recurse on. First pick which
    ** (build source or probe source) to repartition, then add 
    ** and initialize new recursion level (if necessary). 
    **
    ** NOTE: both sources (build and probe) will be bigger 
    ** than the hash table capacity for this partition - hence 
    ** the decision as to which to repartition (the smaller, to
    ** increase the likelihood of a hashable repartitioning). */

    hpptr = &hlink->hsl_partition[i];

    /* First, check for one having all rows with same hashno. This
    ** is the ugly case which requires cross product joining on 
    ** the partition pair. */
    if (hpptr->hsp_flags & HSP_BALLSAME && hpptr->hsp_brcount ||
	hpptr->hsp_flags & HSP_PALLSAME && hpptr->hsp_prcount)
    {
	QEN_HASH_CARTPROD	*hcpptr;
	bool	leftas, rightas, leftjoin, rightjoin;

	hbase->hsh_flags |= HASH_CARTPROD;
	hbase->hsh_ccount++;
	hlink->hsl_pno = i;	/* show which partition to process */

	/* Setup cross product structure to save stuff which would
	** otherwise clog up other hash join structures. If memory
	** has already been allocated for the structure, just reuse it. 
	** Otherwise, allocate the memory first. */

	if ((hcpptr = hbase->hsh_cartprod) == NULL)
	{
	    status = qen_hash_palloc(qcb, hbase,
			    sizeof(QEN_HASH_CARTPROD),
			    &hcpptr,
			    &dsh->dsh_error, "cartprod");
	    if (status != E_DB_OK)
		return (status);
	    hbase->hsh_cartprod = hcpptr;
	}
	hcpptr->hcp_flags = HCP_FIRST;
	hcpptr->hcp_boffset = hcpptr->hcp_poffset = 0;
	hcpptr->hcp_bcurrbn = hcpptr->hcp_pcurrbn = 0;

	/* Both sides should be flushed if they are nonempty.  If one
	** side was nonempty and not flushed it would form a hash table.
	** If check-only (which prevents reversal), a flushco should have
	** been done.
	*/

	/* Determine which source is "outer" in the join, then assign
	** structure fields accordingly.
	** If it's a check-only join, make the probe side the outer
	** since, since it's easier to run the cart-prod special casing
	** that way.  If this is not an outer join (left, right, full),
	** and only one of the sources is ALLSAME, the non-ALLSAME
	** is outer (outer rows which don't match the ALLSAME hash key
	** can be skipped, reducing the number of passes of the
	** ALLSAME partition).  If both are ALLSAME and this is 
	** an outer join, the outer join outer becomes the cartprod
	** outer. Otherwise, the build source is the outer. 
	*/ 
	leftas = (hpptr->hsp_flags & HSP_BALLSAME) != 0;
	rightas = (hpptr->hsp_flags & HSP_PALLSAME) != 0;
	leftjoin = (hbase->hsh_flags & HASH_WANT_LEFT) != 0;
	rightjoin = (hbase->hsh_flags & HASH_WANT_RIGHT) != 0;

	if (hbase->hsh_nodep->hjn_kuniq
	  || !rightas || (rightjoin && !leftjoin))
	{
	    /* These conditions lead to role reversal (build source 
	    ** is inner of the cartprod). This is different from the
	    ** role reversal recorded in hsh_flags, as it applies only 
	    ** to this partition. */
	    hcpptr->hcp_flags |= HCP_REVERSE;
	    if (rightas)
		hcpptr->hcp_flags |= HCP_OALLSAME;
	    if (leftas)
		hcpptr->hcp_flags |= HCP_IALLSAME;
	    hcpptr->hcp_bfile = hpptr->hsp_pfile;
	    hcpptr->hcp_pfile = hpptr->hsp_bfile;
	    hcpptr->hcp_browcnt = hpptr->hsp_prcount;
	    hcpptr->hcp_prowcnt = hpptr->hsp_brcount;
	    hcpptr->hcp_browsz = hbase->hsh_prowsz;
	    hcpptr->hcp_prowsz = hbase->hsh_browsz;
	    hcpptr->hcp_bbptr = hpptr->hsp_bptr;
	    hcpptr->hcp_pbptr = hbase->hsh_rbptr;
	}
	else
	{
	    /* Cartprod is of outer to inner. */
	    hcpptr->hcp_flags &= ~HCP_REVERSE;
	    if (rightas)
		hcpptr->hcp_flags |= HCP_IALLSAME;
	    if (leftas)
		hcpptr->hcp_flags |= HCP_OALLSAME;
	    hcpptr->hcp_bfile = hpptr->hsp_bfile;
	    hcpptr->hcp_pfile = hpptr->hsp_pfile;
	    hcpptr->hcp_browcnt = hpptr->hsp_brcount;
	    hcpptr->hcp_prowcnt = hpptr->hsp_prcount;
	    hcpptr->hcp_browsz = hbase->hsh_browsz;
	    hcpptr->hcp_prowsz = hbase->hsh_prowsz;
	    hcpptr->hcp_pbptr = hpptr->hsp_bptr;
	    hcpptr->hcp_bbptr = hbase->hsh_rbptr;
	}
	
	hcpptr->hcp_prowsleft = hcpptr->hcp_prowcnt;
	hcpptr->hcp_partition = hpptr;
	return(E_DB_OK);
    }

    bpsize = hbase->hsh_browsz * hpptr->hsp_brcount;
			    /* build source data in partition */
    ppsize = hbase->hsh_prowsz * hpptr->hsp_prcount;
			    /* probe source data in partition */
    /* Repartition the smaller, except for CO join where we
    ** repartition the build side.
    */
    if (hbase->hsh_nodep->hjn_kuniq || bpsize < ppsize)
    {
	/* Repartition build source partition. */
	hbase->hsh_flags &= ~HASH_ROLE_REVERSE;
	rowsz = hbase->hsh_browsz;
	rowcount = hpptr->hsp_brcount;
    }
    else
    {
	/* Repartition probe source partition & do role reversal. */
	hbase->hsh_flags |= HASH_ROLE_REVERSE;
	rowsz = hbase->hsh_prowsz;
	rowcount = hpptr->hsp_prcount;
    }

    partmax = hbase->hsh_csize / rowsz;

    /* We have a partition to recurse on. Now setup the link structure. */

#ifdef xDEBUG
    TRdisplay("%@ qen_hash_recurse thread %d newlevel %d on pno %d\n",
	dsh->dsh_threadno, hlink->hsl_level+1, i);
#endif
    hlink->hsl_pno = i;
    if (hlink->hsl_next == NULL)
    {
	/* First visit to this recursion level. Allocate/format link
	** structure and corresponding partition descriptor array. */
	status = qen_hash_palloc(qcb, hbase,
		sizeof(QEN_HASH_LINK) + hbase->hsh_pcount * sizeof(QEN_HASH_PART),
		&hlink->hsl_next,
		&dsh->dsh_error, "partition_desc_array");
	if (status != E_DB_OK)
	    return (status);

	prevlink = hlink;
	hlink = hlink->hsl_next;	/* hook 'em together */
	hlink->hsl_prev = prevlink;
	hlink->hsl_next = (QEN_HASH_LINK *) NULL;
	hlink->hsl_level = prevlink->hsl_level + 1;
	hlink->hsl_partition = (QEN_HASH_PART *)&((char *)hlink)
						[sizeof(QEN_HASH_LINK)];
	for (i = 0; i < hbase->hsh_pcount; i++)
	{
	    QEN_HASH_PART   *hpart = &hlink->hsl_partition[i];

	    hpart->hsp_bptr = prevlink->hsl_partition[i].hsp_bptr;
	    hpart->hsp_bvptr = prevlink->hsl_partition[i].hsp_bvptr;
	    hpart->hsp_bfile = (QEN_HASH_FILE *) NULL;
	    hpart->hsp_pfile = (QEN_HASH_FILE *) NULL;
	    hpart->hsp_offset = 0;
	    hpart->hsp_brcount = 0;
	    hpart->hsp_prcount = 0;
	    hpart->hsp_currbn = 0;
	    hpart->hsp_reclevel = 0;
	    hpart->hsp_flags  = (HSP_BALLSAME | HSP_PALLSAME);
	}
    }
    else
    {
	prevlink = hlink;
	hlink = hlink->hsl_next;	/* new link level */
	for (i = 0; i < hbase->hsh_pcount; i++)
	{
	    QEN_HASH_PART   *hpart = &hlink->hsl_partition[i];

	    /* Reset stuff in existing partition array. */
	    hpart->hsp_bptr = prevlink->hsl_partition[i].hsp_bptr;
	    hpart->hsp_bvptr = prevlink->hsl_partition[i].hsp_bvptr;
	    hpart->hsp_bfile = (QEN_HASH_FILE *) NULL;
	    hpart->hsp_pfile = (QEN_HASH_FILE *) NULL;
	    hpart->hsp_offset = 0;
	    hpart->hsp_brcount = 0;
	    hpart->hsp_prcount = 0;
	    hpart->hsp_currbn = 0;
	    hpart->hsp_reclevel = 0;
	    hpart->hsp_flags  = (HSP_BALLSAME | HSP_PALLSAME);
	}
    }

    /* Record highest recursion level since reset */
    if (hlink->hsl_level > hbase->hsh_maxreclvl)
	hbase->hsh_maxreclvl = hlink->hsl_level;

    hlink->hsl_brcount = hlink->hsl_prcount = 0;	/* init row counts */
    hlink->hsl_pno = -1;
    hlink->hsl_flags = 0;
    hbase->hsh_currlink = hlink;	/* tell the world */
    
    /* Finally, call qen_hash_repartition to read rows from source
    ** partition and repartition them into new recursion level. */

    status = qen_hash_repartition(dsh, hbase, hpptr);
    return(status);

}	/* end of qen_hash_recurse */


/*{
** Name: QEN_HASH_REPARTITION - reads rows and repartitions from 
**			partition just subjected to recursion.
**
** Description:
**	With recursion structures in place, this function opens 
**	partition file of partition to be recursed on, reads its rows
**	and repartitions them to new level.
**
** Inputs:
**	dsh		- Pointer to thread's DSH
**	hbase		- ptr to base hash structure
**	hpptr		- ptr to partition structure of partition being
**			recursed
**
** Outputs:
**
** Side Effects:
**
**
** History:
**	8-apr-99 (inkdo01)
**	    Written.
**	19-apr-02 (inkdo01)
**	    Compute oldojp more accurately to reflect non-OJ queries (107539).
**	10-Nov-2005 (schka24)
**	    Restate ojpartflag as filterflag.
**
*/
 
static DB_STATUS
qen_hash_repartition(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase,
QEN_HASH_PART	*hpptr)
{

    QEN_HASH_LINK	*hlink = hbase->hsh_currlink;
    PTR			source, target, rowptr;
    i4			offset, remainder, rowcount, rowsz, i;
    DB_STATUS		status;
    DB_DATA_VALUE	oldhash_dv, newhash_dv;
    u_i4		hashno;
    bool		incomplete;
    bool		reverse = (hbase->hsh_flags & HASH_ROLE_REVERSE) != 0;
    bool		filter, filterflag;

    ++ hbase->hsh_rpcount;		/* For stats */

    /* Set up size and count constants (remembering possible role
    ** reversal). Close/open partition file, read its rows and 
    ** repartition using qen_hash_partition. */

#ifdef xDEBUG
    /* Print some stuff */
    i = hpptr - &hlink->hsl_prev->hsl_partition[0];
    TRdisplay ("%@ Repartition of %s thread %d pno %d into level %d\n",
		reverse ? "probe" : "build",
		dsh->dsh_threadno, i, hlink->hsl_level);
    TRdisplay ("t%d: P[%d]: b:%d  p:%d, flags %v\n",
		dsh->dsh_threadno, i, hpptr->hsp_brcount, hpptr->hsp_prcount,
		"BSPILL,PSPILL,BINHASH,PINHASH,BFLUSHED,PFLUSHED,BALLSAME,PALLSAME,DONE",hpptr->hsp_flags);
#endif

    STRUCT_ASSIGN_MACRO(hashdv, oldhash_dv);
    STRUCT_ASSIGN_MACRO(hashdv, newhash_dv);
    oldhash_dv.db_data = (PTR)&hashno;

    /* We can filter rows if we're loading the probe side and we aren't
    ** doing outer join on the side we're loading.  Loading the "probe"
    ** side here means that BOTH-LOADED is set in the link flags (the
    ** caller turns that on BEFORE calling repartition for the second
    ** side, when both sides of a partition have to be repartitioned).
    ** Filtering is excluded when outer-joining on this side, because
    ** we're not in a position to deal properly with the nomatch row.
    ** We'll consider oj a little below.
    */
    filter = (hlink->hsl_flags & HSL_BOTH_LOADED) != 0;

    status = qen_hash_close(hbase, (reverse) ? &hpptr->hsp_pfile :
				&hpptr->hsp_bfile, 0);
    if (status != E_DB_OK) 
		    return(status);
    status = qen_hash_open(dsh, hbase, hpptr, OPEN_READ);
    if (status != E_DB_OK) 
		    return(status);
    rowptr = NULL;
    incomplete = FALSE;
    offset = hbase->hsh_csize;

    if (reverse)
    {
	rowcount = hpptr->hsp_prcount;
	rowsz = hbase->hsh_prowsz;
	if (hbase->hsh_flags & HASH_WANT_RIGHT) filter = FALSE;
    }
    else
    {
	rowcount = hpptr->hsp_brcount;
	rowsz = hbase->hsh_browsz;
	if (hbase->hsh_flags & HASH_WANT_LEFT) filter = FALSE;
    }

    while (rowcount > 0)
    {
	/* Loop to read blocks of partition file, address individual
	** rows and partition them in new level. */

	if (offset >= hbase->hsh_csize)
	{
	    /* Need to read another block. */
	    status = qen_hash_read(hbase, (reverse) ? 
		hpptr->hsp_pfile : hpptr->hsp_bfile, 
		hbase->hsh_rbptr, &hpptr->hsp_currbn);
	    if (status != E_DB_OK) 
			    return(status);

	    if (incomplete)
	    {
		/* Row spans blocks - finish building it in buffer. */
		incomplete = FALSE;
		source = hbase->hsh_rbptr;
		target = (PTR)&((char *)rowptr)[rowsz - remainder];
		MEcopy(source, remainder, target);
					/* copy the rest */
		offset = remainder;
	    }
	    else
	    {
		rowptr = hbase->hsh_rbptr;
		offset = rowsz;
	    }
	}
	else
	{
	    /* Next row from this block. */
	    rowptr = (PTR)&((char *)hbase->hsh_rbptr)[offset];
	    if (offset > hbase->hsh_csize - rowsz)
	    {
		/* Row spans blocks - copy first chunk to row buffer
		** and loop for rest. */
		target = hbase->hsh_workrow;
		MEcopy(rowptr, (hbase->hsh_csize - offset), target);
		rowptr = hbase->hsh_workrow;
		remainder = rowsz - (hbase->hsh_csize - offset);
		offset = hbase->hsh_csize;
		incomplete = TRUE;
		continue;
	    }

	    /* We have a whole row in block. Just update offset and
	    **  fall through to partition call. */
	    offset += rowsz;
	}

	/* Copy old hashno, rehash it and repartition row. */
	hashno = *((u_i4 *)rowptr);
	newhash_dv.db_data = rowptr;
	status = adu_hash(dsh->dsh_adf_cb, &oldhash_dv, &newhash_dv);
	if (status != E_DB_OK) 
			return (status);

	filterflag = filter;		/* copy (hash_part changes it) */
	status = qen_hash_partition(dsh, hbase, rowptr, &filterflag);
	if (status != E_DB_OK) 
			return(status);
	rowcount--;
    }	/* end of partitioning loop */

    /* Close file and add to free chain.  Remember that hlink is for
    ** the new level - drop file if it's at the top level.
    */
    i = CLOSE_RELEASE;
    if (hlink->hsl_level <= 1)
	i = CLOSE_DESTROY;
    status = qen_hash_close(hbase, (reverse) ? &hpptr->hsp_pfile :
				&hpptr->hsp_bfile, i);
    return(status);
	
}	/* end of qen_hash_repartition */


/*{
** Name: QEN_HASH_PROBE1	- phase I of probe
**
** Description:
**	If the original build source scan resulted in a hash table, rows
**	from original probe source are sent to qen_hash_probe to attempt
**	hash joins. If the original build partitions were all too big to
**	generate a hash table, the probe rows are sent directly to
**	qen_hash_partition for storage in the partition buffers (or
**	probe spill files).  With luck, after all probes are read,
**	we may find that one or more probe partitions are small enough to
**	create a hash table (resulting in an immediate role reversal).
**	That isn't our problem here, though.
**
**	At this point we're working with rows from the real inner, can't
**	be in role reversal.  (REVERSE can be set if the driver knows
**	that there's no hash table.)
**
** Inputs:
**	dsh		- Pointer to thread's DSH
**	hbase		- ptr to base hash structure
**	buildptr	- ptr to addr of build row buffer (to return row)
**	probeptr	- addr of probe row
**	ojmask		- ptr to mask word for OJs
**	gotone		- addr of flag set to indicate successful join
**
** Outputs:
**	*buildptr	- addr of matching build row returned here if any
**	*ojmask		- OJ-RIGHT flag set if right-OJ on this probe row
**	*gotone		- set TRUE if found build row, or right-OJ and probe
**			  didn't/can't match anything
**
** Side Effects:
**
** History:
**	17-mar-99 (inkdo01)
**	    Written.
**	jan-00 (inkdo01)
**	    Changes to oj handling.
**	29-aug-04 (inkdo01)
**	    Support global base array to reduce build row movement.
*/
 
DB_STATUS
qen_hash_probe1(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase,
PTR		*buildptr,
PTR		probeptr,
QEN_OJMASK	*ojmask,
bool		*gotone)

{

    DB_STATUS	status;
    bool	filterflag = TRUE;

    /* If build phase didn't create a hash table (all partitions were
    ** too big), just load probe rows directly into partitions, too. 
    ** Maybe we can make a hash table from probe source (immediate role
    ** reversal). Otherwise, call qen_hash_probe to probe for joins. */

    *gotone = FALSE;			/* init return flag */

    if (hbase->hsh_flags & HASH_NOHASHT)
    {
	/* Driver already set REVERSE to direct rows into the probe
	** side of the partitions.
	*/
	status = qen_hash_partition(dsh, hbase, probeptr, &filterflag);
	if (! ((hbase->hsh_flags & HASH_WANT_RIGHT) && filterflag)
	  || status != E_DB_OK) 
	    return(status);

	/* This is outer join and current row has no match in the other 
	** partition (because the bit filter is 0 or there are no rows
	** in the partition). */
	*ojmask |= OJ_HAVE_RIGHT;
	*gotone = TRUE;
	return(E_DB_OK);
    }
    else
    {
	return(qen_hash_probe(dsh, hbase, buildptr, probeptr,
				ojmask, gotone));
    }

}


/*{
** Name: QEN_HASH_PROBE		- probe hash table for join
**
** Description:
**	Rows of current probe source (either actual build or actual probe
**	source, depending on current state of join) are targeted to hash 
**	table (those probe rows corresponding to the partitions which make 
**	up the hash table, that is). Probe rows targeting disk-resident 
**	partitions are themselves written to disk for phase II. If all 
**	partitions generated by preceding build phase of other source were 
**	too big to create a hash table, the probe rows are sent to 
**	qen_hash_partition in hope that one or more probe partitions 
**	are small enough to generate a hash table, resulting in an immediate 
**	role reversal. Otherwise, a recursion is done to break down the
**	build/probe partitions.
**
** Inputs:
**	dsh		- Pointer to thread's DSH
**	hbase		- ptr to base hash structure
**	buildptr	- ptr to addr of build row buffer (to return row)
**	probeptr	- addr of probe row
**	ojmask		- ptr to mask word for OJs
**	gotone		- addr of flag set to indicate successful join
**
** Outputs:
**
** Side Effects:
**
**
** History:
**	17-mar-99 (inkdo01)
**	    Written.
**	7-may-01 (inkdo01)
**	    Support hash chains sorted first on hashno, then on join keys.
**	22-aug-01 (inkdo01)
**	    Added code to precheck for null join cols (to avoid "null = null"
**	    joins).
**	7-sep-01 (inkdo01)
**	    Set HASH_PROBE_STORED flag for probe rows partitioned because their
**	    build rows are on disk - avoids premature OJ logic.
**	23-aug-01 (devjo01)
**	    Change probekey calculation to be friendly to 64bit platforms.
**	29-aug-04 (inkdo01)
**	    Support global base array to reduce build row movement.
**	6-apr-05 (inkdo01)
**	    Add hsh_prevpr to test OJ's.
**	10-Nov-2005 (schka24)
**	    fix browsz type, rows can be larger then u_i2 now.
**	    Remove prevpr, use NOJOIN flag to test probe OJ-ness.
**	23-Dec-2005 (kschendel)
**	    Check hash before chasing keys when using firstbr optimization.
*/
 
static DB_STATUS
qen_hash_probe(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase,
PTR		*buildptr,
PTR		probeptr,
QEN_OJMASK	*ojmask,
bool		*gotone)

{

    QEN_HASH_PART	*hpptr;
    QEN_HASH_LINK	*hlink = hbase->hsh_currlink;
    i4			hbucket, pno, cmpres;
    u_i4		hashno;
    i4			inhashmask;
    DB_STATUS		status;
    i4			hrowsz;
    ADF_CB		*adfcb = dsh->dsh_adf_cb;
    QEN_OJMASK		joinflag = 0;
    bool		reverse = (hbase->hsh_flags & HASH_ROLE_REVERSE) != 0;
    bool		filterflag;
    QEF_HASH_DUP	*hchain;
    char		*probekey;

    /* Offset of key area must match calculation in opc_hash_materialize */
    probekey = probeptr + ( &((QEF_HASH_DUP *)0)->hash_key[0] - (char*)0 );

    if (hbase->hsh_flags & (HASH_WANT_LEFT | HASH_WANT_RIGHT))
    {
	/* Outer join setup. */
	(*ojmask) = 0; 			/* first turn off oj flags */
	if (reverse)
	{
	    if (hbase->hsh_flags & HASH_WANT_LEFT)
	    {
		joinflag = OJ_HAVE_LEFT;
	    }
	}
	else if (hbase->hsh_flags & HASH_WANT_RIGHT)
	{
	    joinflag = OJ_HAVE_RIGHT;
	}
    }

    *gotone = FALSE;			/* & returned join flag */
    adfcb->adf_errcb.ad_errcode = 0;
    /* Size of row in the hash table */
    hrowsz = (reverse) ? hbase->hsh_prowsz : hbase->hsh_browsz;

    /* If previous call returned a matched pair of rows, restart
    ** with immediately following build row to see if there's another
    ** match. */

    if (hbase->hsh_flags & HASH_PROBE_MATCH)
    {
	DB_CMP_LIST	*keylist = hbase->hsh_nodep->hjn_cmplist;
	QEF_HASH_DUP    *nextrow = hbase->hsh_nextbr;

	if (hbase->hsh_flags & HASH_CHECKNULL &&
		qen_hash_checknull(hbase, probekey))
	    cmpres = -1;
	else cmpres = adt_sortcmp(adfcb, keylist, hbase->hsh_kcount,
			&nextrow->hash_key[0], probekey, 0);
	if (adfcb->adf_errcb.ad_errcode != 0)
	{
	    dsh->dsh_error.err_code = adfcb->adf_errcb.ad_errcode;
	    return(E_DB_ERROR);
	}

	/* If no match, turn off flag, and return for a new probe row. */
	if (cmpres != 0)
	{
	    hbase->hsh_flags &= ~HASH_PROBE_MATCH;
	    if (joinflag != 0 && hbase->hsh_flags & HASH_PROBE_NOJOIN)
	    {
		/* Probe row hasn't matched anything yet. Return as outer
		** join. */
		*ojmask |= joinflag;
		*gotone = TRUE;
	    }
	    return(E_DB_OK);
	}

	/* If match, point to next build row, and copy current to build
	** row buffer. */
	*gotone = TRUE;
	hbase->hsh_nextbr = nextrow->hash_next;
	if (hbase->hsh_nextbr == NULL)
		hbase->hsh_flags &= ~HASH_PROBE_MATCH;
	*buildptr = (PTR)nextrow;	/* save matched row addr */

	return(E_DB_OK);
    }

    /* We get here if this is a new probe row. Compute partition it 
    ** hashes to. If partition is in hash table, compute hash bucket
    ** and search chain. */

    if (reverse)
    {
	hlink->hsl_brcount++;
	inhashmask = HSP_PINHASH;
    }
    else
    {
	hlink->hsl_prcount++;
	inhashmask = HSP_BINHASH;
    }
    hashno = *((u_i4 *)probeptr);	/* copy hash number from front
					** of probe row buffer */
    pno = hashno % hbase->hsh_pcount;
    hpptr = &hlink->hsl_partition[pno]; /* target partition */

    /* If target partition is in current hash table, locate the bucket
    ** and proceed from there. Actually, first we check if current 
    ** probe row has same key as the last hash table hit. If not, we
    ** look for the bucket. */
    if (hpptr->hsp_flags & inhashmask)
    {
	DB_CMP_LIST	*keylist = hbase->hsh_nodep->hjn_cmplist;
	QEF_HASH_DUP    *firstrow = (QEF_HASH_DUP *)hbase->hsh_firstbr;

	if (firstrow && hashno == firstrow->hash_number)
	{
	    /* Check if current has same key as last one. */
	    if (hbase->hsh_flags & HASH_CHECKNULL &&
		    qen_hash_checknull(hbase, probekey))
		cmpres = -1;
	    else cmpres = adt_sortcmp(adfcb, keylist, hbase->hsh_kcount,
			probekey, &firstrow->hash_key[0], 0);
	    if (adfcb->adf_errcb.ad_errcode != 0)
	    {
		dsh->dsh_error.err_code = adfcb->adf_errcb.ad_errcode;
		return(E_DB_ERROR);
	    }

	    if (cmpres == 0)
	    {
		*gotone = TRUE;
		hchain = firstrow;
	    }
	}

	/* If previous logic didn't find one, compute hash bucket and
	** search the chain. */

	if (!(*gotone))
	 for (hbucket = hashno % hbase->hsh_htsize,
		hchain = hbase->hsh_htable[hbucket];
		hchain != NULL; hchain = hchain->hash_next)
	{
	    if (hchain->hash_number < hashno)
		continue;
	    if (hchain->hash_number == hashno)
	    {
	   	/* Matched hash number, now compare join keys. */
		if (hbase->hsh_flags & HASH_CHECKNULL &&
			qen_hash_checknull(hbase, probekey))
		    cmpres = -1;
		else cmpres = adt_sortcmp(adfcb, keylist, 
			hbase->hsh_kcount, &hchain->hash_key[0], 
			probekey, 0);
		if (adfcb->adf_errcb.ad_errcode != 0)
		{
		    dsh->dsh_error.err_code = adfcb->adf_errcb.ad_errcode;
		    return(E_DB_ERROR);
		}
	    }
	    else break;			/* no match */

	    if (cmpres < 0) continue;	/* loop 'til we find it */
	    else if (cmpres == 0)
	    {				/* got match */
		*gotone = TRUE;
		hbase->hsh_firstbr = (PTR)hchain;  /* save for next call */
		break;
	    }
	    else break;			/* no match */
	}	/* end of bucket chain loop */

	if ((*gotone) == FALSE)
	{
	    /* Check for possible probe-side outer join. */
	    if (joinflag != 0)
	    {
		*ojmask |= joinflag;
		*gotone = TRUE;
	    }
	}
	else
	{
	    /* Got a match - copy to build row buffer & set up next
	    ** row in chain. */
	    hbase->hsh_nextbr = hchain->hash_next;
	    if (hbase->hsh_nextbr == NULL)
		hbase->hsh_flags &= ~HASH_PROBE_MATCH;
	    else 
	    {
		hbase->hsh_flags |= HASH_PROBE_MATCH;
	    }
	    *buildptr = (PTR) hchain;	/* return matched row addr */
	}
	return(E_DB_OK);

    }	/* end of probe partition in hash table */

    else
    {
	/* Probe row hashes to build partition on disk. Simply call 
	** qen_hash_partition to repartition it. 
	** Note that the role reversal logic reverses for this 
	** partition call, since we're partitioning a probing
	** row.  hash-partition understands REVERSE in an absolute
	** sense;  so if we're role reversed, our "probe" row is really
	** a build side row and we'd better unreverse so that partition
	** sends it to the build side!
	*/
	hbase->hsh_flags ^= HASH_ROLE_REVERSE;
	filterflag = TRUE;
	status = qen_hash_partition(dsh, hbase, probeptr, &filterflag);
	hbase->hsh_flags ^= HASH_ROLE_REVERSE;
	if (status != E_DB_OK) 
	    return(status);

	/* See if row had no match in build partition - 0 rows or
	** bit filter is off. If so, check for OJ, else row was dropped. */
	if (filterflag && joinflag)
	{
	    *ojmask |= joinflag;
	    *gotone = TRUE;
	}
	return(E_DB_OK);
    }

}


/*{
** Name: QEN_HASH_PROBE2	- phase II of probe
**
** Description:
**	This routine oversees the processing of joins not performed
**	during the phase I probe (as rows are actually materialized 
**	from the probe source). It coordinates the building of hash
**	tables from partitions not yet processed (either build or probe
**	partitions). And if no remaining partitions are small enough
**	to fit a hash table, it drives recursive partitioning until
**	they are small enough.
**
** Inputs:
**	dsh		- Pointer to thread's DSH
**	hbase		- ptr to base hash structure
**	buildptr	- ptr to addr of build row buffer (to return row)
**	probeptr	- ptr to addr of probe row (to return row)
**	ojmask		- ptr to returned mask word for OJs
**
** Outputs:
**
** Side Effects:
**
**
** History:
**	30-mar-99 (inkdo01)
**	    Written.
**	6-dec-99 (inkdo01)
**	    Added logic for recursion processing.
**	jan-00 (inkdo01)
**	    Changes for cartprod logic, various bug fixes.
**	2-feb-00 (inkdo01)
**	    Logic for first non-OJ call to probe2, to tidy up phase1 pass.
**	9-aug-02 (inkdo01)
**	    Added "indptr" local to pass the hash table row indicator address to 
**	    qen_hash_probe (as determined by reverse flag).
**	27-june-03 (inkdo01)
**	    Dropped "*ojmask == 0" test at loop's end - it prevented OJ
**	    processing on hash table rows (bug 110499).
**	29-aug-04 (inkdo01)
**	    Support global base array to reduce build/probe  row movement.
**	8-Jun-2005 (schka24)
**	    "End" partitions that have no probe rows if between phases
**	    1 and 2 (non-OJ only).
**	17-Jun-2005 (schka24)
**	    Fix read-too-far when a partition ends exactly at the end of
**	    a block.
**	14-Nov-2005 (schka24)
**	    Tidy up indptr stuff, set NOJOIN when fetching a new probe.
**	28-nov-05 (inkdo01)
**	    Fix bug handling outer joins in which inner side is build source
**	    and some partitions have no build rows, but some probe rows 
**	    (which are immediately OJ candidates).
**	6-dec-05 (inkdo01)
**	    Quick fix to cover an edge case of above change.
*/
 
DB_STATUS
qen_hash_probe2(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase,
PTR		*buildptr,
PTR		*probeptr,
QEN_OJMASK	*ojmask)

{

    QEN_HASH_LINK  *hlink = hbase->hsh_currlink;
    QEN_HASH_PART  *hpptr;
    QEF_HASH_DUP *brptr;
    PTR		*buildp, *probep, target, rowptr;
    DB_STATUS	status = E_DB_OK;
    i4		size, rowsz, i;
    i4		inhashmask;
    u_i4	hashno;
    DB_DATA_VALUE	oldhash_dv, newhash_dv;
    QEN_OJMASK	ojset = 0;
    bool	recurse = (hlink->hsl_level > 0 && 
			(hlink->hsl_flags & HSL_BOTH_LOADED) == 0);
    bool	newrow;
    bool	reverse = (hbase->hsh_flags & HASH_ROLE_REVERSE) != 0;
    bool	gotone = FALSE;
    bool	newhasht = FALSE;
    bool	filterflag, allojs = FALSE;


    /* Main function of this routine is to read rows from a disk
    ** resident partition and probe a hash table with them. If there
    ** is no hash, it calls qen_hash_build to make one. Rows from
    ** probe partitions which match partitions loaded into hash table
    ** are read in turn for the join process. */

    /* Big loop forces rebuild of hash table, if necessary, before 
    ** partitions are read to be joined. Returns are done from inside 
    ** loop when joins are made. */

    for ( ; ; )
    {
	/* Before executing the big loop, check if we've just finished 
	** all the probe1 calls (and subsequent left join calls). If so,
	** execute logic to flush probe rows which split to partitions not 
	** in the phase 1 hash table (if there was one).
	** If we just finished probe1, but need left joins, outer loops
	** will whirl until all the left-OJ rows are sucked out,
	** then we'll do this bit.
	*/
	if (hbase->hsh_flags & HASH_BETWEEN_P1P2 && 
	    !(hbase->hsh_flags & HASH_BUILD_OJS))
	{
	    bool	flush = FALSE;

	    hbase->hsh_flags &= ~HASH_BETWEEN_P1P2;
	    hlink->hsl_flags |= HSL_BOTH_LOADED;

	    /* Loop over partitions - "end"ing those in hash table. 
	    ** Also "end" partitions with no probe rows.  Either this
	    ** was an inner join, or all OJ's have been processed.
	    ** Either way, probe-less partitions are now useless.
	    */
	    for (i = 0; i < hbase->hsh_pcount; i++)
	    {
		hpptr = &hlink->hsl_partition[i];
		if ((hpptr->hsp_flags & HSP_BINHASH) || 
		    (hpptr->hsp_prcount == 0 &&
		    !(hbase->hsh_flags & HASH_WANT_LEFT)))
		{
		    hpptr->hsp_flags = HSP_DONE;
		    status = qen_hash_close(hbase, &hpptr->hsp_bfile,
				CLOSE_DESTROY);
		    if (status != E_DB_OK) 
			return(status);
		}
		else if (hpptr->hsp_prcount > 0) flush = TRUE;
	    }

	    if (flush) 
	    {
		/* Flush probes, that's what was in the buffers */
		hbase->hsh_flags |= HASH_ROLE_REVERSE;
		status = qen_hash_flush(dsh, hbase, FALSE);
		if (status != E_DB_OK) 
		    return(status);
		hbase->hsh_flags &= ~HASH_ROLE_REVERSE;
		reverse = FALSE;
	    }
	}

	newrow = FALSE;
	if (hbase->hsh_flags & HASH_NOHASHT && 
			!(hbase->hsh_flags & HASH_CARTPROD))
	{
	    status = qen_hash_build(dsh, hbase);
	    if (status != E_DB_OK) 
				return(status);
	    newhasht = TRUE;
	    /* Hash build could have caused role reversal, or recursion */
	    reverse = (hbase->hsh_flags & HASH_ROLE_REVERSE) != 0;
	    hlink = hbase->hsh_currlink;	/* refresh, in case of
						** recursion */

	    /* See if hash build caused recursion. If so, must prepare
	    ** to read rows of corresponding partition in other source.
	    ** Its rows probe hash table when they rehash to partitions
	    ** in table, and rest are repartitioned to disk.
	    **
	    ** This logic is only applied if one source was repartitioned
	    ** in the recursion. If both sources were repartitioned 
	    ** (because the first repartitioned source was still too large
	    ** to create a hash table), regular probe2 logic is applied. */

	    if (hlink->hsl_level > 0 && !(hlink->hsl_flags & HSL_BOTH_LOADED))
	    {
		hlink = hbase->hsh_currlink;
		recurse = TRUE;
		hpptr = &hlink->hsl_prev->hsl_partition
			[hlink->hsl_prev->hsl_pno];
				/* partition to recurse */
		status = qen_hash_close(hbase, (reverse) ?
			&hpptr->hsp_bfile : &hpptr->hsp_pfile, 0);
		if (status != E_DB_OK) 
				return(status);

		hbase->hsh_flags ^= HASH_ROLE_REVERSE;
					/* flip for qen_hash_open() logic */
		status = qen_hash_open(dsh, hbase, hpptr, OPEN_READ);
		hbase->hsh_flags ^= HASH_ROLE_REVERSE;
		if (status != E_DB_OK) 
				return(status);

		hpptr->hsp_offset = hbase->hsh_csize;
		hbase->hsh_currows_left = (reverse) ? hpptr->hsp_brcount :
				hpptr->hsp_prcount;
	    }
	}
/* qen_hash_verify(hbase); */

	/* If we're recursing, set up repartitioning of recurse partition. */
	if (recurse)
	{
	    STRUCT_ASSIGN_MACRO(hashdv, oldhash_dv);
	    STRUCT_ASSIGN_MACRO(hashdv, newhash_dv);
	    oldhash_dv.db_data = (PTR)&hashno;
	    hpptr = &hlink->hsl_prev->hsl_partition
		[hlink->hsl_prev->hsl_pno];
				/* partition to recurse */
	}

	/* Check for role reversal and set appropriate flags. */

	if (reverse)	/* encapsulate build/probe sources */
	{
	    buildp = probeptr;
	    probep = buildptr;
	    rowsz = hbase->hsh_browsz;
	    inhashmask = HSP_PINHASH;
	    if (hbase->hsh_flags & HASH_WANT_RIGHT) ojset = OJ_HAVE_RIGHT;
	}
	else
	{
	    buildp = buildptr;
	    probep = probeptr;
	    rowsz = hbase->hsh_prowsz;
	    inhashmask = HSP_BINHASH;
	    if (hbase->hsh_flags & HASH_WANT_LEFT) ojset = OJ_HAVE_LEFT;
	}

	target = *probep;

	/* We have a hash table. Enter loop to produce a join. */
	while (!gotone)
	{
	    /* If we're doing a "cartprod" partition, call special func. and
	    ** get out of the way!  Let it handle all OJ testing, etc.
	    */
	    if (hbase->hsh_flags & HASH_CARTPROD)
	    {
		status = qen_hash_cartprod(dsh, hbase, buildptr, probeptr,
				ojmask, &gotone);
		if (status == E_DB_OK && !gotone) break;
					/* cartprod is done, next hash build */
		return(status);
	    }

	    /* See if we're in the midst of producing outer 
	    ** joins from previous hash table. */
	    if (hbase->hsh_flags & HASH_BUILD_OJS)
	    {
		i4	ojrowsz;
		/* Loop over all hash buckets, then all rows within
		** each bucket, looking for rows with "0" in last 
		** byte (indicator byte). They're the oj candidates.
		**
		** First determine whether we're looking for left
		** or right joins (which depends on what source 
		** is in the hash table).  */
		if (reverse) 
		{
		    if (!(hbase->hsh_flags & HASH_WANT_RIGHT)) break;
		    ojrowsz = hbase->hsh_prowsz;
		}
		else 
		{
		    if (!(hbase->hsh_flags & HASH_WANT_LEFT)) break;
		    ojrowsz = hbase->hsh_browsz;
		}

		if (hbase->hsh_currbucket == -1)
		{
		    /* First call for this hash table. Set up 
		    ** following loops. */
		    hbase->hsh_currbucket = 0;
		    hbase->hsh_nextbr = NULL;
		}

		for (i = hbase->hsh_currbucket; i < hbase->hsh_htsize; i++)
		{
		    if (hbase->hsh_nextbr == NULL) 
			hbase->hsh_nextbr = hbase->hsh_htable[i];
		    else
			hbase->hsh_nextbr = hbase->hsh_nextbr->hash_next;
		    for (brptr = hbase->hsh_nextbr;
			brptr != NULL; brptr = brptr->hash_next)
		    {
			if (((PTR)brptr)[ojrowsz-1] == 0x0)
			{
			    /* This is an outer join candidate. */
			    *buildp = (PTR)brptr;
			    (*ojmask) |= ojset;
			    hbase->hsh_nextbr = brptr;
			    hbase->hsh_currbucket = i;
					/* note where we left off */
			    return(E_DB_OK);
			}
		    }
		    hbase->hsh_nextbr = NULL;
		}
		if (i == hbase->hsh_htsize) break;
				/* no more oj candidates - leave loop */
	    }

	    /* If last trip thru produced a fresh probe row, init the
	    ** probe-side OJ indicator flag.
	    */
	    if (newrow)
		hbase->hsh_flags |= HASH_PROBE_NOJOIN;

	    /* allojs indicates a probe row from a partition with no rows 
	    ** in the hash table and an outer join candidate. We don't
	    ** need to call qen_hash_probe(). */
	    if (allojs)
	    {
		if (reverse)
		    (*ojmask) |= OJ_HAVE_LEFT;
		else (*ojmask) |= OJ_HAVE_RIGHT;
		gotone = TRUE;
		return(E_DB_OK);
	    }

	    if (hbase->hsh_flags & HASH_PROBE_MATCH || newrow)
	    {
		status = qen_hash_probe(dsh, hbase, buildp, *probep, 
					ojmask, &gotone);
		if (status != E_DB_OK) 
				return(status);
		if (gotone) return(E_DB_OK);
		newrow = FALSE;
	    }

	    /* Dropping to here means a new probe row is required
	    ** from the disk partition. Copy to probep and loop. */

	    /* If we are in a recursion level, the probe row is rehashed
	    ** in new level first. If probes to a partition in the current
	    ** hash table, process as normal. Else, repartition the row and 
	    ** loop back for another. */

	    if (recurse)
	    {
		newhasht = FALSE;		/* unset flag */
		while (hbase->hsh_currows_left > 0)
		{
/* qen_hash_verify(hbase); */
		    /* Following code extracts next row from selected disk 
		    ** partition and is identical to the code in the
		    ** non-recursion logic which follows. */
		    /* Copy the bits of the probe row. */
		    rowptr = &hbase->hsh_rbptr[hpptr->hsp_offset];
		    if ((size = hbase->hsh_csize - hpptr->hsp_offset)
			    >= rowsz) 
		    {
			/* Whole row in buffer - just copy ptr. */
			*probep = target = rowptr;
			hpptr->hsp_offset += rowsz;
		    }
		    else
		    {

			/* If we're here, we are about to copy the last bit
			** of a block or have yet to read from this file.
			** Read a block and get started, possibly finishing
			** partial row started in last block and stored in
			** work row. */
			if (size > 0)
			{
			    *probep = target = hbase->hsh_workrow;
			    MEcopy(rowptr, size, target);
			}
			else *probep = target = hbase->hsh_rbptr;
			target = &target[size];	/* reposition in probe row */
			status = qen_hash_read(hbase, (reverse) ? 
			    hpptr->hsp_bfile : hpptr->hsp_pfile, 
			    hbase->hsh_rbptr, &hpptr->hsp_currbn);
			if (status != E_DB_OK) 
				    return(status);

			size = rowsz - size;
			if (size < rowsz)
			    MEcopy(hbase->hsh_rbptr, size, target);
					/* copy the rest */
			hpptr->hsp_offset = size;
			target = *probep;
		    }

/* qen_hash_verify(hbase); */
		    /* Got a row - rehash to determine partition in new 
		    * recursion level. */
		    hbase->hsh_currows_left--;	/* one less row */
		    hashno = *((u_i4 *)target);
		    newhash_dv.db_data = target;
		    status = adu_hash(dsh->dsh_adf_cb, &oldhash_dv,
					&newhash_dv);
		    if (status != E_DB_OK) 
			return(status);

		    hashno = *((u_i4 *)target);	/* new hash number */
		    if (hlink->hsl_partition
			[hashno % hbase->hsh_pcount].hsp_flags & inhashmask)
		    {
			/* This row hashes to the current hash table. */
			newrow = TRUE;
			break;
		    }

		    /* New hashno splits to a disk partition - repartition it.
		    ** Note that the role reversal logic reverses for this 
		    ** partition call, since we're partitioning a probing
		    ** row. */
		    hbase->hsh_flags ^= HASH_ROLE_REVERSE;
		    filterflag = TRUE;
		    status = qen_hash_partition(dsh, hbase, 
						target, &filterflag);
		    hbase->hsh_flags ^= HASH_ROLE_REVERSE;
		    if (ojset && filterflag && status == E_DB_OK) 
		    {
			/* This is outer join and current row has no match in the other 
			** partition (because the bit filter is 0 or there are no rows
			** in the partition). */
			*ojmask |= ojset;
			return(E_DB_OK);
		    }

		    if (status != E_DB_OK) 
			return(status);
		}	/* end of while currows_left (recurse) */

		if (newrow) continue;		/* got a row - loop to probe */

		/* We get here when all rows in the recursed partition have been
		** processed. Close recursed file and flag partition as done. 
		** Break from !gotone loop to clean up this level's partitions,
		** check for oj's and loop back to hash table build. */
		status = qen_hash_close(hbase, (reverse) ?
		    &hpptr->hsp_bfile : &hpptr->hsp_pfile, CLOSE_DESTROY);
		if (status != E_DB_OK) 
				return(status);
		hpptr->hsp_flags = HSP_DONE;
		/* Temporarily flip role-reversal so that we flush
		** the local-probe side
		*/
		hbase->hsh_flags ^= HASH_ROLE_REVERSE;
		status = qen_hash_flush(dsh, hbase, TRUE);
		hbase->hsh_flags ^= HASH_ROLE_REVERSE;
		if (status != E_DB_OK) 
		    return(status);
		hlink->hsl_flags |= HSL_BOTH_LOADED;
		recurse = FALSE; 		/* turn this off, too */

		break;		/* from recurse */
	    }

	    /* Regular join with no recursion - get the next probe row. */

	    if (hbase->hsh_currows_left-- > 0 && !newhasht &&
			hbase->hsh_currpart >= 0)
	    {
		/* Rows left in current partition. Get the next. */
		newrow = TRUE;
		hpptr = &hlink->hsl_partition[hbase->hsh_currpart];

		/* Copy the bits of the probe row. */
		rowptr = &hbase->hsh_rbptr[hpptr->hsp_offset];
		if ((size = hbase->hsh_csize - hpptr->hsp_offset)
			    >= rowsz) 
		{
		    /* Whole row in buffer - just copy ptr. */
		    *probep = target = rowptr;
		    hpptr->hsp_offset += rowsz;
		}
		else
		{

		    /* If we're here, we are about to copy the last bit
		    ** of a block or have yet to read from this file.
		    ** Read a block and get started, possibly finishing
		    ** partial row started in last block and stored in
		    ** work row. */
		    if (size > 0)
		    {
			*probep = target = hbase->hsh_workrow;
			MEcopy(rowptr, size, target);
		    }
		    else *probep = target = hbase->hsh_rbptr;
		    target = &target[size];	/* reposition in probe row */
		    status = qen_hash_read(hbase, (reverse) ? 
			hpptr->hsp_bfile : hpptr->hsp_pfile, 
			hbase->hsh_rbptr, &hpptr->hsp_currbn);
		    if (status != E_DB_OK) 
			return(status);

		    size = rowsz - size;
		    if (size < rowsz)
			MEcopy(hbase->hsh_rbptr, size, target);
					/* copy the rest */
		    hpptr->hsp_offset = size;
		    target = *probep;
		}
		continue;		/* loop back to probe again */
	    }	/* end of hsh_currows_left */

	    /* We get here if we've used all the rows in the current 
	    ** partition or we're just starting to process a newly
	    ** built hash table. Look for the next partition in the hash 
	    ** table, open its file and start reading its rows. But
	    ** first, close and release previous partition file (if
	    ** this isn't the first partition in the hash table). */

	    if (!newhasht && hbase->hsh_currpart >= 0)
	    {
		hpptr = &hlink->hsl_partition[hbase->hsh_currpart];
		i = CLOSE_RELEASE;
		if (hlink->hsl_level == 0)
		    i = CLOSE_DESTROY;
		status = qen_hash_close(hbase, &hpptr->hsp_bfile, i);
		if (status == E_DB_OK)
		    status = qen_hash_close(hbase, &hpptr->hsp_pfile, i);
		if (status != E_DB_OK) 
		    return(status);
		hpptr->hsp_flags = HSP_DONE;
	    }
	    else 
	    {
		newhasht = FALSE;
		hbase->hsh_currpart = -1;
	    }

	    allojs = FALSE;		/* reset for next partition */
	    for (i = hbase->hsh_currpart+1; i < hbase->hsh_pcount; i++)
	    {
		/* Get next partition that maps to the hash table, or
		** has 0 rows in the hash table but may contribute
		** outer join rows. */
		i4	prcount;
		hpptr = &hlink->hsl_partition[i];
		if (hpptr->hsp_flags == HSP_DONE ||
		    hpptr->hsp_brcount == 0 && hpptr->hsp_prcount == 0)
		    continue;
		if (hpptr->hsp_flags & inhashmask)
		    break;
		if ((reverse ? hpptr->hsp_prcount : hpptr->hsp_brcount) 
			== 0 &&
		    (reverse && hbase->hsh_flags & HASH_WANT_LEFT ||
		     !reverse && hbase->hsh_flags & HASH_WANT_RIGHT))
		{
		    allojs = TRUE;
		    break;
		}
	    }
	    if (i >= hbase->hsh_pcount) break;
					/* finished hash table - loop
					** back and make another */
	    hbase->hsh_currpart = i;

	    hbase->hsh_flags ^= HASH_ROLE_REVERSE;
					/* flip for qen_hash_open() logic */
	    status = qen_hash_open(dsh, hbase, hpptr, OPEN_READ);
	    hbase->hsh_flags ^= HASH_ROLE_REVERSE;
	    if (status != E_DB_OK) 
			    return(status);

	    hpptr->hsp_offset = hbase->hsh_csize;
	    if (reverse) hbase->hsh_currows_left = hpptr->hsp_brcount;
	    else hbase->hsh_currows_left = hpptr->hsp_prcount;
	}	/* end of !gotone */

	/* Current hash table has now been processed. Loop resetting
	** partition bits. */
	for (i = 0; i < hbase->hsh_pcount; i++)
	{
	    hpptr = &hlink->hsl_partition[i];
	    if (hpptr->hsp_flags & inhashmask)
	    {
		hpptr->hsp_flags &= ~inhashmask;
		status = qen_hash_close(hbase, &hpptr->hsp_bfile,
			CLOSE_DESTROY);
		if (status != E_DB_OK) 
			return(status);
		status = qen_hash_close(hbase, &hpptr->hsp_pfile,
			CLOSE_DESTROY);
		if (status != E_DB_OK) 
			return(status);
		hpptr->hsp_flags = HSP_DONE;
	    }
	}

	/* See if hash table is potential source of outer join candidates.
	** If so, set up OJ pass and loop back. If not (or if OJ pass
	** has now been completed), loop back to rebuild hash table. */

	/* (There used to be a test for *ojmask == 0 as well, but it 
	** inhibited the OJ search - search never got off the ground - 
	** and was dropped. Comment is here in case the test had another 
	** purpose.) */
	if (hbase->hsh_flags & HASH_BUILD_OJS || 
	    (reverse && !(hbase->hsh_flags & HASH_WANT_RIGHT)) || (!reverse &&
	    !(hbase->hsh_flags & HASH_WANT_LEFT)))
	{
	    hbase->hsh_flags &= ~HASH_BUILD_OJS;
	    hbase->hsh_flags |= HASH_NOHASHT;
	}
	else
	{
	    hbase->hsh_flags |= HASH_BUILD_OJS;
	    hbase->hsh_currbucket = -1;
	}

    }	/* end of outer for-loop */
}


/*{
** Name: QEN_HASH_CARTPROD	- cross product of ALLSAME partitions
**
** Description:
**	This routine performs a cross product join of the build/probe
**	rows from a partition in which both sources won't fit the 
**	memory resident hash table and all the rows of at least one
**	source have the same hashkeys. This last factor makes 
**	recursion useless, since all rows would again be assigned to
**	the same partition and the partition would not be reduced in
**	size. 
**
**	This hopefully rare phenomenon leaves no alternative but to
**	perform a cross-product of the rows of each source for this
**	partition. One source is chosen to drive the join (the "outer"
**	source), and for each of its rows, every row is read and matched
**	from the other ("inner") source. Logic is also required to 
**	handle outer joins with such data.
**
**	In the case of a check-only cart-prod situation, the cart-prod
**	setup has arranged for the check-only side (probe input) to be
**	the "outer" of the cross-product.  No true cross-product is done
**	in this case;  we arrange to ask for a new outer each time.
**
**	For outer joins, we'll handle all the OJ checking logic here.
**	All that the top level has to do is update the OJ indicator
**	bytes properly (i.e. if the "on" clause CX says that the rows
**	really do join).
**
**	*Caution* the guy who sets up cross-producting (qen_hash_recurse)
**	can set local reversal for this one partition.  This reversal
**	flag is set in the HCP control block flags, NOT the usual
**	hashjoin hsh_flags.  Be very careful about calling utility
**	routines that might expect role reversal to be set in hsh_flags!
**
** Inputs:
**	dsh		- DSH execution control block for thread
**	hbase		- ptr to base hash structure
**	buildptr	- ptr to addr of build row buffer (to return row)
**	probeptr	- ptr to addr of probe row
**	ojmask		- ptr to returned mask word for OJs
**	gotone		- ptr to flag indicating whether a (joined) row is 
**			being returned
**
** Outputs:
**
** Side Effects:
**
**
** History:
**	7-jan-00 (inkdo01)
**	    Written.
**	22-aug-01 (inkdo01)
**	    Added code to precheck for null join cols (to avoid "null = null"
**	    joins).
**	15-jan-03 (inkdo01)
**	    Minor fixes to full/right join logic to correct failing calls
**	    to qen_hash_write.
**	24-jan-03 (inkdo01)
**	    One last change to fix full/right joins.
**	22-apr-03 (inkdo01)
**	    Really one last change to fix full/right joins.
**	29-aug-04 (inkdo01)
**	    Add logic for global base array to reduce data movement for 
**	    build/probe rows.
**	17-may-05 (inkdo01)
**	    Destroy files at end - not just free them.
**	7-Jun-2005 (schka24)
**	    cartprod during check-only join didn't work, make it work.
**	    Also, clean out partition file ptrs, since we destroy the
**	    files per the previous edit.
**	14-Nov-2005 (schka24)
**	    Don't bother passing OJ indptrs around.
**	17-Jun-2006 (kschendel)
**	    Deal with one side being empty (OJ case).
**	31-may-07 (hayke02 for schka24)
**	    Only continue/return from the outer row FOR loop if HCP_IALLSAME
**	    is set. This prevents reversed cart prod (check only) hash joins
**	    from returning too few rows. This change fixes bug 118200.
*/
 
static DB_STATUS
qen_hash_cartprod(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase,
PTR		*buildptr,
PTR		*probeptr,
QEN_OJMASK	*ojmask,
bool		*gotone)

{

    QEN_HASH_CARTPROD	*hcpptr = hbase->hsh_cartprod;
    QEN_HASH_PART *hpptr;
    DB_CMP_LIST	*keylist = hbase->hsh_nodep->hjn_cmplist;
    DB_STATUS	status;
    PTR		*otherptrp;
    QEN_OJMASK	bojmask = 0, pojmask = 0;
    ADF_CB	*adfcb = dsh->dsh_adf_cb;
    PTR		rowptr, target;
    u_i4	bhashno;
    i4		size, cmpres;
    bool	alldone = FALSE;
    bool	read_outer = (hcpptr->hcp_flags & HCP_READ_OUTER) != 0;

    if (hcpptr->hcp_flags & HCP_REVERSE)
    {
	otherptrp = probeptr;
	probeptr = buildptr;
	buildptr = otherptrp;

	if (hbase->hsh_flags & HASH_WANT_RIGHT) 
				bojmask = OJ_HAVE_RIGHT;
	if (hbase->hsh_flags & HASH_WANT_LEFT) 
				pojmask = OJ_HAVE_LEFT;
    }
    else
    {
	if (hbase->hsh_flags & HASH_WANT_LEFT) 
				bojmask = OJ_HAVE_LEFT;
	if (hbase->hsh_flags & HASH_WANT_RIGHT) 
				pojmask = OJ_HAVE_RIGHT;
    }

    /* If first call for this cartprodable partition, read 
    ** 1st blocks of each source, set some flags and do 
    ** other initialization stuff. */
    if (hcpptr->hcp_flags & HCP_FIRST)
    {
	u_i4 bhashno, phashno;

	read_outer = TRUE;
	hcpptr->hcp_flags &= ~HCP_FIRST;
	/* We work bhashno and phashno such that if one side is empty,
	** the two are forced not-equal so that we don't attempt to read
	** an empty build or probe.
	*/
	bhashno = 0;  phashno = 1;	/* Shouldn't both be empty but... */
	if (hcpptr->hcp_browcnt > 0)
	{
	    status = qen_hash_read(hbase, hcpptr->hcp_bfile, 
  		hcpptr->hcp_bbptr, &hcpptr->hcp_bcurrbn);
	    if (status != E_DB_OK) return(status);
	    bhashno = *((u_i4 *)hcpptr->hcp_bbptr);
	    phashno = ~bhashno;		/* Something not equal */
	}
  
	if (hcpptr->hcp_prowcnt > 0)
	{
	    status = qen_hash_read(hbase, hcpptr->hcp_pfile, 
  		hcpptr->hcp_pbptr, &hcpptr->hcp_pcurrbn);
	    if (status != E_DB_OK) return(status);
	    phashno = *((u_i4 *)hcpptr->hcp_pbptr);
	    if (hcpptr->hcp_browcnt == 0)
		bhashno = ~phashno;	/* Don't pass the BOTH test */
	}
	hcpptr->hcp_hashno = phashno;	/* For main loop */
  	
  	/* If hashno's of 2 ALLSAME sources don't match, 
  	** we're done with this partition, unless this is 
	** an outer join.
	** If one partition is empty, fail the test as well.  (Note
	** that all-same and hence BOTH is set if a partition is empty!)
	*/
	if ((hcpptr->hcp_flags & (HCP_OALLSAME | HCP_IALLSAME)) ==
	    (HCP_OALLSAME | HCP_IALLSAME) && bhashno != phashno)
	{
	    if (bojmask || pojmask) hcpptr->hcp_flags |= HCP_ONLYOJ;
	    else alldone = TRUE;
	}
    }

    /* Big loop, waiting for gotone to turn on. Successes
    ** return from inside loop. If we're out of rows, we just
    ** break and clean up. */
    *gotone = FALSE;
    adfcb->adf_errcb.ad_errcode = 0;

    while (!(*gotone) && !alldone)
    {
	/* First check for outer joins - left join, then right
	** join. Then it falls through to inner join logic. */
	if (hcpptr->hcp_flags & HCP_ONLYOJ)
	{
	    if (hcpptr->hcp_browcnt && bojmask)
	    {
		/* "Left" join processing. */
		rowptr = &hcpptr->hcp_bbptr[hcpptr->hcp_boffset];
		*buildptr = target = rowptr;
		if ((size = hbase->hsh_csize - hcpptr->hcp_boffset) >=
					hcpptr->hcp_browsz)
		    size = hcpptr->hcp_browsz;
		else
		{
		    /* Row spans blocks. Copy first chunk, then
		    ** read next block to get the rest. */
		    if (size > 0)
		    {
			*buildptr = target = hbase->hsh_workrow;
			MEcopy(rowptr, size, target);
			target = &target[size];
		    }
		    else *buildptr = target = hcpptr->hcp_bbptr;
		    size = hcpptr->hcp_browsz - size;
		    hcpptr->hcp_boffset = 0;
				/* reset sizes, pointers, etc. */
		    status = qen_hash_read(hbase, 
			hcpptr->hcp_bfile, hcpptr->hcp_bbptr,
			&hcpptr->hcp_bcurrbn);
		    if (status != E_DB_OK) return(status);
		    rowptr = hcpptr->hcp_bbptr;
						/* reset rowptr */
		}

		/* Copy remainder of row to DSH row buffer
		** then update ptrs, counters, etc. */
		if (size != hcpptr->hcp_browsz)
		{
		    MEcopy(rowptr, size, target);
		    target = *buildptr;
		}
		hcpptr->hcp_browcnt--;
		hcpptr->hcp_boffset += size;
		*ojmask = bojmask;
		*gotone = TRUE;
		return(E_DB_OK);
	    }	/* end of "left" join logic */

	    else if (hcpptr->hcp_prowsleft && pojmask)
	    {
		/* "Right" join processing. */
		while (!(*gotone) && hcpptr->hcp_prowsleft)
		{
		    /* We only pick probe rows with indicator byte
		    ** turned off. */
		    rowptr = &hcpptr->hcp_pbptr[hcpptr->hcp_poffset];
		    *probeptr = target = rowptr;
		    if ((size = hbase->hsh_csize - hcpptr->hcp_poffset) >
					hcpptr->hcp_prowsz)
			size = hcpptr->hcp_prowsz;
		    else
		    {
			/* Row spans blocks. Copy first chunk, then
			** read next block to get the rest. */
			if (size > 0)
			{
			    *probeptr = target = hbase->hsh_workrow2;
			    MEcopy(rowptr, size, target);
			    target = &target[size];
			}
			else *probeptr = target = hcpptr->hcp_pbptr;
			size = hcpptr->hcp_prowsz - size;
			hcpptr->hcp_poffset = 0;
				/* reset sizes, pointers, etc. */
			status = qen_hash_read(hbase, 
			    hcpptr->hcp_pfile, hcpptr->hcp_pbptr,
			    &hcpptr->hcp_pcurrbn);
			if (status != E_DB_OK) return(status);
			rowptr = hcpptr->hcp_pbptr;
						/* reset rowptr */
		    }

		    /* Copy remainder of row to DSH row buffer
		    ** then update ptrs, counters, etc. */
		    if (size > 0 && size != hcpptr->hcp_prowsz)
		    {
			MEcopy(rowptr, size, target);
			target = *probeptr;
		    }
		    hcpptr->hcp_prowsleft--;
		    hcpptr->hcp_poffset += size;
		    if (target[hcpptr->hcp_prowsz-1] != 0x0) continue;
				/* row already joined - skip to next */
		    *ojmask = pojmask;
		    *gotone = TRUE;
		    return(E_DB_OK);
		}
	    }	/* end of "right" join logic */

	    /* We get here if we're in OJ mode, and they're all done. */
	    alldone = TRUE;
	    continue;
	}	/* end of outer join processing */

	/* Inner join processing starts here. It's quite a bit simpler.
	** First read an "outer" row, if necessary. */
	for ( ; ; )	/* big loop to break from */
	{
	    if (read_outer)
	    {
		if (hcpptr->hcp_browcnt == 0)
		{
		    if (pojmask) hcpptr->hcp_flags |= HCP_ONLYOJ;
				/* just OJs left */
		    else alldone = TRUE;
				/* otherwise, we're really done */
		    break;
		}

		/* Copy row bits into place (reading new block, if
		** needed). */
		rowptr = &hcpptr->hcp_bbptr[hcpptr->hcp_boffset];
		*buildptr = target = rowptr;
		if ((size = hbase->hsh_csize - hcpptr->hcp_boffset) >=
				hcpptr->hcp_browsz)
		    size = hcpptr->hcp_browsz;
		else
		{
		    if (size > 0)
		    {
			*buildptr = target = hbase->hsh_workrow;
			MEcopy(rowptr, size, target);
			target = &target[size];
		    }
		    rowptr = hcpptr->hcp_bbptr;
		    size = hcpptr->hcp_browsz - size;
		    hcpptr->hcp_boffset = 0;
		    status = qen_hash_read(hbase, hcpptr->hcp_bfile,
				hcpptr->hcp_bbptr, &hcpptr->hcp_bcurrbn);
		    if (status != E_DB_OK) return(status);
		}
		if (size != hcpptr->hcp_browsz)
		{
		    MEcopy(rowptr, size, target);
		    target = *buildptr;
		}
		hcpptr->hcp_browcnt--;
		hcpptr->hcp_boffset += size;
		bhashno = *((u_i4 *)target);
		rowptr[size-1] = 0x0;		/* Init OJ indicator */
		if (hcpptr->hcp_flags & HCP_IALLSAME
		    && bhashno != hcpptr->hcp_hashno)
		{
		    /* Outer row hashno doesn't match inners. "Left"
		    ** join, or reject the row and loop for another. */
		    if (bojmask)
		    {
			*ojmask = bojmask;
			*gotone = TRUE;
			return(E_DB_OK);
		    }
		    else continue;
		}

		read_outer = FALSE;
		hcpptr->hcp_flags &= ~HCP_READ_OUTER;
	    }

	    /* We have an outer row in buildptr. Now get a matching 
	    ** inner. */
	    for ( ; ; )	/* another loop to break from */
	    {
		if (hcpptr->hcp_prowsleft == 0) break;

		/* If any inners left, loop 'til we find one. */
		rowptr = &hcpptr->hcp_pbptr[hcpptr->hcp_poffset];
		*probeptr = target = rowptr;
		if ((size = hbase->hsh_csize - hcpptr->hcp_poffset) >=
				hcpptr->hcp_prowsz)
		    size = hcpptr->hcp_prowsz;
		else
		{
		    if (size > 0)
		    {
			*probeptr = target = hbase->hsh_workrow2;
			MEcopy(rowptr, size, target);
			target = &target[size];
		    }
		    rowptr = hcpptr->hcp_pbptr;
		    size = hcpptr->hcp_prowsz - size;
		    hcpptr->hcp_poffset = 0;
		    if (pojmask)
		    {
			i4	currbn = hcpptr->hcp_pcurrbn-1;

			/* If probe is "outer" of an OJ, write block
			** back, c/w indicator bytes for later OJ 
			** pass. */
			status = qen_hash_write(dsh, hbase, 
				hcpptr->hcp_pfile, hcpptr->hcp_pbptr,
				&currbn);
			if (status != E_DB_OK) return(status);
		    }
		    status = qen_hash_read(hbase, hcpptr->hcp_pfile,
				hcpptr->hcp_pbptr, &hcpptr->hcp_pcurrbn);
		    if (status != E_DB_OK) return(status);
		}
		if (size != hcpptr->hcp_prowsz)
		{
		    MEcopy(rowptr, size, target);
		    target = *probeptr;
		}
		hcpptr->hcp_prowsleft--;
		hcpptr->hcp_poffset += size;
		
		/* Now have outer and inner rows. Compare keys. */
		if (hbase->hsh_flags & HASH_CHECKNULL &&
			qen_hash_checknull(hbase, 
			    &((QEF_HASH_DUP *)*buildptr)->hash_key[0]))
		    cmpres = -1;
		cmpres = adt_sortcmp(adfcb, keylist,
			hbase->hsh_kcount, 
			&((QEF_HASH_DUP *)*buildptr)->hash_key[0],
			&((QEF_HASH_DUP *)*probeptr)->hash_key[0], 0);
		if (adfcb->adf_errcb.ad_errcode != 0)
		{
		    dsh->dsh_error.err_code = adfcb->adf_errcb.ad_errcode;
		    return(E_DB_ERROR);
		}

		if (cmpres == 0)
		{
		    *gotone = TRUE;
		    /* If a check-only join, don't really do a cross-product,
		    ** just ask for the next outer (which is the probe
		    ** side in this case) and reset the inner.
		    */
		    if (hbase->hsh_nodep->hjn_kuniq)
		    {
			hcpptr->hcp_flags |= HCP_READ_OUTER;
			status = qen_hash_reset_cpinner(dsh, hbase, pojmask);
			if (status != E_DB_OK) return(status);
		    }
		    return(E_DB_OK);
		}
	    }	/* end of inner source for-loop */
	
	    /* To get here, inner rows are all used and we have
	    ** no match. Reset to start at top of inner file and
	    ** check outer row for outer join possibility. */

	    hcpptr->hcp_flags |= HCP_READ_OUTER;
	    read_outer = TRUE;

	    if (hcpptr->hcp_browcnt || pojmask)
	    {
		/* Only reset inner file if there are
		** remaining outers or inners may be in OJ. */
		status = qen_hash_reset_cpinner(dsh, hbase, pojmask);
		if (status != E_DB_OK) return(status);
	    }

	    if (bojmask && (*buildptr)[hcpptr->hcp_browsz-1] == 0x0)
	    {
		/* Outer join. */
		*ojmask = bojmask;
		*gotone = TRUE;
		return(E_DB_OK);
	    }
			
	    if (hcpptr->hcp_browcnt == 0) break;
				/* if remaining outers, stay in loop */
	}	/* end of outer source for-loop */
    }	/* end of !gotone && !alldone loop */

    /* We get here when the Cartesian product is thankfully over. 
    ** Just close the files, set/reset flags and leave. */

    status = qen_hash_close(hbase, &hcpptr->hcp_bfile, CLOSE_DESTROY);
    if (status != E_DB_OK) return(status);
    status = qen_hash_close(hbase, &hcpptr->hcp_pfile, CLOSE_DESTROY);
    if (status != E_DB_OK) return(status);

    hbase->hsh_flags &= ~HASH_CARTPROD;
    /* Clean up the partition, mark it done. */
    hpptr = hcpptr->hcp_partition;
    hpptr->hsp_flags = HSP_DONE;
    hpptr->hsp_bfile = NULL;		/* Destroyed above */
    hpptr->hsp_pfile = NULL;		/* Destroyed above */
    return(E_DB_OK);

}


/*
** Name: QEN_HASH_RESET_CPINNER - Reset cross-product inner source
**
** Description:
**	When doing a cross-product on "all same hash" partitions,
**	after reaching the end of the inner source, we need to
**	reset the inner source to the start, and ask for a fresh
**	outer source row.
**
**	The reason for making this a routine is that we also need to
**	reset the inner source if we're doing a check-only join.
**	In that case, the cartprod loop resets the inner and signals
**	for a fresh outer as soon as a match is found for the current
**	outer row.  (I.e. we don't want to do a true cross product.)
**
** Inputs:
**	dsh		- Data segment header for thread
**	hbase		- ptr to base hash structure
**	pojmask		- nonzero if doing outer-join on "inner" source
**
** Outputs:
**	Returns E_DB_OK or error status
**
** History:
**	7-Jun-2005 (schka24)
**	    Extract common code from qen_hash_cartprod.
**
*/

static DB_STATUS
qen_hash_reset_cpinner(
	QEE_DSH *dsh, QEN_HASH_BASE *hbase,
	QEN_OJMASK pojmask)

{

    DB_STATUS status;
    i4 currbn;
    QEN_HASH_CARTPROD *hcpptr = hbase->hsh_cartprod;

    /* Start over with the inner side */
    hcpptr->hcp_prowsleft = hcpptr->hcp_prowcnt;
    hcpptr->hcp_poffset = 0;
    currbn = hcpptr->hcp_pcurrbn - 1;	/* Current block in buffer */
    if (pojmask)
    {

	/* If probe is "outer" of an OJ, write block
	** back, c/w indicator bytes for later OJ 
	** pass. */
	status = qen_hash_write(dsh, hbase, 
			hcpptr->hcp_pfile, hcpptr->hcp_pbptr,
			&currbn);
	-- currbn;			/* Undo write's incr */
	if (status != E_DB_OK) return(status);
    }
    hcpptr->hcp_pcurrbn = 0;
    /* Read block 0 unless that's what's in there now (unlikely
    ** unless it's a CO join)
    */
    if (currbn > 0)
    {
	status = qen_hash_read(hbase, hcpptr->hcp_pfile,
		hcpptr->hcp_pbptr, &hcpptr->hcp_pcurrbn);
	if (status != E_DB_OK) return(status);
    }
    /* if browcnt == 0, must be "right" OJ */
    if (hcpptr->hcp_browcnt == 0) 
	hcpptr->hcp_flags |= HCP_ONLYOJ;

    /* Caller will ask for a new outer */
    return (E_DB_OK);

} /* qen_hash_reset_cpinner */

/*{
** Name: QEN_HASH_BVINIT	- initialize bit filter
**
** Description:
**	This function zeroes a bit filter and then sets bits for
**	each row currently in the partition buffer.
**
** Inputs:
**	hbase		- ptr to base hash structure
**	hpptr		- ptr to partition descriptor
**	rowsz		- size of rows in buffer
**
** Outputs:
**
** Side Effects:
**
**
** History:
**	29-dec-99 (inkdo01)
**	    Written.
**	22-Jun-2005 (schka24)
**	    Bit filtering is within partition, calculate it that way
**	    for better selectivity.
**	10-Nov-2005 (schka24)
**	    Tighten up check to make sure we don't look at a hash number
**	    that falls off the end of the buffer.  qee's layout prevents
**	    this at the moment, but belt and suspenders...
**
*/
 
static VOID
qen_hash_bvinit(
QEN_HASH_BASE	*hbase,
QEN_HASH_PART	*hpptr,
i4		rowsz)

{
    i4		i, pcount;
    i4		*bvptr = (i4 *)hpptr->hsp_bvptr;
    u_i4	bvsize = hbase->hsh_bvsize * BITSPERBYTE - 1;
    u_i4	hashno;

    pcount = hbase->hsh_pcount;
    /* First, loop over vector and zero all the bits (1 word at
    ** a time). */
    for (i = 0; i < hbase->hsh_bvsize / 4; i++)
	bvptr[i] = 0;

    /* Now loop over partition buffer, setting appropriate bit
    ** for each row.  The loop-termination test ensures that we
    ** don't look at a hash number that's not entirely in the
    ** buffer (should be impossible if properly laid out!)
    */
    for (i = 0; i < hbase->hsh_csize - sizeof(u_i4); i += rowsz)
    {
	hashno = *((u_i4 *)&hpptr->hsp_bptr[i]);
	BTset((hashno/pcount) % bvsize, hpptr->hsp_bvptr);
    }

}


/*{
** Name: QEN_HASH_CHECKNULL	- check for null join keys
**
** Description:
**	This function looks for null valued join columns. If one
**	is found, subsequent key compare is unnecessary because 
**	"null = null" joins are NOT allowed.
**
** Inputs:
**	hbase		- ptr to base hash structure
**	bufptr		- ptr to key buffer (probe or build - makes no
**			  difference)
**
** Outputs:
**	returns		TRUE - if a join column is NULL
**			FALSE - if no join column value is NULL
**
** Side Effects:
**
**
** History:
**	22-aug-01 (inkdo01)
**	    Written to prevent "null = null" joins.
**
*/
 
static bool
qen_hash_checknull(
QEN_HASH_BASE	*hbase,
PTR		bufptr)

{
    DB_CMP_LIST	*cmp = &hbase->hsh_nodep->hjn_cmplist[0];
    i4		i;
    PTR		nullp;

    /* Loop over key compare-list entries, checking null indicators of
    ** the nullable ones for non-zero. */

    i = hbase->hsh_kcount;
    while (--i >= 0)
    {
	if (cmp->cmp_type < 0)
	{
	    nullp = (PTR)(bufptr + (cmp->cmp_offset+cmp->cmp_length-1));
	    if (nullp[0])
		return(TRUE);
	}
	++cmp;
    }

    /* If we fall off loop ... */
    return(FALSE);

}


/*
** Name: qen_hash_palloc
**
** Description:
**	Allocate memory from the hash stream, making sure that we don't
**	exceed the session limit.  If the hash stream isn't open, we'll
**	open it.
**
**	This function can nap (CSms_thread_nap) if the pool is full.
**	Don't call it while holding a mutex or critical resource!
**
** Inputs:
**	qefcb			QEF session control block
**	hbase			Hash join control base
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
**	27-May-2005 (schka24)
**	    Wrote to help trace memory problem with 100Gb TPC-H.
**	24-Oct-2005 (schka24)
**	    (from Datallegro) Nap if we hit the pool limit (but NOT
**	    the session limit).  Don't nap indefinitely, though, we
**	    might be holding locks etc.
*/

static DB_STATUS
qen_hash_palloc(QEF_CB *qefcb, QEN_HASH_BASE *hbase,
	SIZE_TYPE psize, void *pptr,
	DB_ERROR *dsh_error, char *what)

{
    DB_STATUS status;		/* Usual status thing */
    i4 naptime;			/* Total time napped so far */
    ULM_RCB ulm;		/* ULM request block */

    if (qefcb->qef_h_used + psize > qefcb->qef_h_max)
    {
	TRdisplay("%@ QEF sorthash pool: hash %p alloc of %s (%u) fails because used is %u\n",
		hbase, what, psize, qefcb->qef_h_used);
	dsh_error->err_code = E_QE000D_NO_MEMORY_LEFT;
	return (E_DB_ERROR);
    }

    /* Use the sorthash pool */
    STRUCT_ASSIGN_MACRO(Qef_s_cb->qef_s_ulmcb, ulm);

    /* If stream not allocated, do that now */
    if (hbase->hsh_streamid == NULL)
    {
	ulm.ulm_streamid_p = &hbase->hsh_streamid;
	/* **** temp until I get qec straightened out */
	if (hbase->hsh_msize > 2048 * 1024)
	    ulm.ulm_blocksize = 1024 * 1024;
	naptime = 0;
	for (;;)
	{
	    status = ulm_openstream(&ulm);
	    if (status == E_DB_OK || naptime >= Qef_s_cb->qef_max_mem_nap)
		break;
	    CSms_thread_nap(500);		/* Half a second */
	    naptime += 500;
	}
	if (status != E_DB_OK)
	{
	    TRdisplay("%@ QEF sorthash pool: hash %p open of %s (%u) failed, pool full, used is %u\n",
			hbase, what, psize, qefcb->qef_h_used);
	    if (ulm.ulm_error.err_code == E_UL0005_NOMEM)
		dsh_error->err_code = E_QE000D_NO_MEMORY_LEFT;
	    else
		dsh_error->err_code = ulm.ulm_error.err_code;
	    return (E_DB_ERROR);
	}
        if (naptime > 0)
	{
	    TRdisplay("%@ QEF sorthash pool: hash %p open of %s (%u) delayed %d ms, pool full, used is %d\n",
		    hbase, what, psize, naptime, qefcb->qef_h_used);
	}
    }

    ulm.ulm_streamid = hbase->hsh_streamid;
    ulm.ulm_psize = psize;
    naptime = 0;
    for (;;)
    {
	status = ulm_palloc(&ulm);
	if (status == E_DB_OK || naptime >= Qef_s_cb->qef_max_mem_nap)
	    break;
	CSms_thread_nap(500);			/* Half a second */
	naptime += 500;
    }
    if (status != E_DB_OK)
    {
	TRdisplay("%@ QEF sorthash pool: hash %p alloc of %s (%u) failed, pool full, used is %u\n",
		hbase, what, psize, qefcb->qef_h_used);
	if (ulm.ulm_error.err_code == E_UL0005_NOMEM)
	    dsh_error->err_code = E_QE000D_NO_MEMORY_LEFT;
	else
	    dsh_error->err_code = ulm.ulm_error.err_code;
	return (E_DB_ERROR);
    }
    if (naptime > 0)
    {
	TRdisplay("%@ QEF sorthash pool: hash %p alloc of %s (%u) delayed %d ms, pool full, used is %d\n",
		hbase, what, psize, naptime, qefcb->qef_h_used);
    }
    hbase->hsh_size += psize;
    CSadjust_counter(&qefcb->qef_h_used, psize);	/* thread-safe */
    *(void **) pptr = ulm.ulm_pptr;
#ifdef xDEBUG
    TRdisplay("%@ hashalloc %p alloc %u %s at %p, new hsh_size %u used %u\n",
		hbase, psize, what, ulm.ulm_pptr, hbase->hsh_size, qefcb->qef_h_used);
#endif

    return (E_DB_OK);

} /* qen_hash_palloc */

/*{
** Name: QEN_HASH_WRITE		- write a block to a partition file
**
** Description:
**	This function writes blocks to partition files. 
**
** Inputs:
**	dsh		- DSH execution control block
**	hbase		- ptr to base hash structure
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
**	23-mar-99 (inkdo01)
**	    Written.
**	28-jan-00 (inkdo01)
**	    Tidy up hash memory management.
**	22-Jun-2005 (schka24)
**	    Keep I/O stats.
**
*/
 
static DB_STATUS
qen_hash_write(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase,
QEN_HASH_FILE	*hfptr,
PTR		buffer,
i4		*currbn)

{

    DMH_CB	*dmhcb;
    DB_STATUS	status;
    i4		kbytes;
    QEF_CB	*qcb = dsh->dsh_qefcb;

    /* If we're writing a block, we'll need to read it back sooner
    ** or later. So allocate read buffer here. */
    if (hbase->hsh_rbptr == NULL)
    {

	/* Partition read buffer has not yet been allocated. Do
	** it now. */
	status = qen_hash_palloc(qcb, hbase,
			hbase->hsh_csize,
			&hbase->hsh_rbptr,
			&dsh->dsh_error, "io_part_buf");
	if (status != E_DB_OK)
	    return (status);
    }

    /* Keep stats */
    kbytes = hbase->hsh_csize >> 10;	/* 1K is 1024 bytes = 2^10 */
    if (hfptr->hsf_isbuild)
	hbase->hsh_bwritek += kbytes;
    else
	hbase->hsh_pwritek += kbytes;

    /* Load up DMH_CB and issue the call. */
    dmhcb = (DMH_CB *)hbase->hsh_dmhcb;
    dmhcb->dmh_locidxp = &hfptr->hsf_locidx;
    dmhcb->dmh_file_context = hfptr->hsf_file_context;
    dmhcb->dmh_rbn = (*currbn)++;
    dmhcb->dmh_buffer = buffer;
    dmhcb->dmh_func = DMH_WRITE;
    dmhcb->dmh_flags_mask = 0;

    status = dmh_write(dmhcb);
    if (status != E_DB_OK)
	status = status;
    return(status);

}


/*{
** Name: QEN_HASH_OPEN 		- determines if open file is available
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
**	dsh		- DSH execution control block
**	hbase		- ptr to base hash structure
**	hpptr		- ptr to partition descriptor
**	flag		- flag indicating open for read or write
**
** Outputs:
**
** Side Effects:
**
**
** History:
**	30-mar-99 (inkdo01)
**	    Written.
**	28-jan-00 (inkdo01)
**	    Tidy up hash memory management.
**
*/
 
static DB_STATUS
qen_hash_open(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase,
QEN_HASH_PART	*hpptr,
i4		flag)

{

    DMH_CB	*dmhcb;
    QEN_HASH_FILE  **fptr;
    DB_STATUS	status;
    bool	reverse = (hbase->hsh_flags & HASH_ROLE_REVERSE) != 0;


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
    if (flag & OPEN_READ) 
	return(E_DB_OK);

    /* Load up DMH_CB and issue the call. */
    dmhcb = (DMH_CB *)hbase->hsh_dmhcb;
    fptr = (reverse) ? &hpptr->hsp_pfile : &hpptr->hsp_bfile;

    /* Check free list and reuse existing file, if possible. 
    ** Otherwise, allocate a QEN_HASH_FILE + file context block
    ** and create the file, too. */
    if (hbase->hsh_freefile != NULL)
    {
	*fptr = hbase->hsh_freefile;
	hbase->hsh_freefile = (*fptr)->hsf_nextf;
	(*fptr)->hsf_isbuild = !reverse;
	return(E_DB_OK);
    }
    else
    {
	QEF_CB		*qcb = dsh->dsh_qefcb;

	/* No existing file, but maybe there's an available context
	** block -- they aren't that big, but no point in wasting them.
	*/
	if (hbase->hsh_availfiles != NULL)
	{
	    *fptr = hbase->hsh_availfiles;
	    hbase->hsh_availfiles = (*fptr)->hsf_nextf;
	    /* Fall thru to reinit the block */
	}
	else
	{
	    /* Allocate memory for the file context (and containing
	    ** QEN_HASH_FILE block). */
	    status = qen_hash_palloc(qcb, hbase,
			sizeof(QEN_HASH_FILE) + sizeof(SR_IO),
			fptr,
			&dsh->dsh_error, "hash_file");
	    if (status != E_DB_OK)
		return (status);
	}

	(*fptr)->hsf_nextf = NULL;
	(*fptr)->hsf_nexto = hbase->hsh_openfiles;
	hbase->hsh_openfiles = (*fptr);		/* link to open list */
	(*fptr)->hsf_locidx = hbase->hsh_lastloc;
	(*fptr)->hsf_isbuild = !reverse;
	(*fptr)->hsf_file_context = (PTR)&((char *)(*fptr))
					[sizeof(QEN_HASH_FILE)];
    }
    ++ hbase->hsh_fcount;
    dmhcb->dmh_file_context = (*fptr)->hsf_file_context;
    dmhcb->dmh_locidxp = &(*fptr)->hsf_locidx;
    dmhcb->dmh_blksize = hbase->hsh_csize;
    dmhcb->dmh_func = DMH_OPEN;
    dmhcb->dmh_flags_mask = DMH_CREATE;

    status = dmh_open(dmhcb);
    if (status == E_DB_OK) hbase->hsh_lastloc = (*fptr)->hsf_locidx;
			/* save last used location index */
    else status = status;
    return(status);

}


/*{
** Name: QEN_HASH_READ 		- read a block from a partition file
**
** Description:
**	This function reads blocks from partition files. 
**
** Inputs:
**	hbase		- ptr to base hash structure
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
**	30-mar-99 (inkdo01)
**	    Written.
**
*/
 
static DB_STATUS
qen_hash_read(
QEN_HASH_BASE	*hbase,
QEN_HASH_FILE	*hfptr,
PTR		buffer,
i4		*currbn)

{

    DMH_CB	*dmhcb;
    DB_STATUS	status;

    /* Load up DMH_CB and issue the call. */
    dmhcb = (DMH_CB *)hbase->hsh_dmhcb;
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
** Name: QEN_HASH_FLUSH		- write out contents of partition buffers
**				and close files
**
** Description:
**	This function writes remaining buffer of all disk resident 
**	partition files, then closes them.
**
** Inputs:
**	dsh		- DSH execution control block
**	hbase		- ptr to base hash structure
**	nonspilled_too	- TRUE - allocate & write file for non-spilled
**			partitions, as well as those already spilled
**
** Outputs:
**
** Side Effects:
**
**
** History:
**	8-apr-99 (inkdo01)
**	    Written.
**	18-apr-02 (inkdo01)
**	    Added support to flush non-spilled p's, too, to fix 107539.
**	7-Jun-2005 (schka24)
**	    non-spilled-too case has to watch out for empty partitions.
**	    It's a bit unusual, but can happen when the input is large with
**	    a few discrete values (some partitions fill up and others stay
**	    empty).
**	17-Jun-2006 (kschendel)
**	    Break out flusher for one partition.
**
*/
 
static DB_STATUS
qen_hash_flush(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase,
bool		nonspilled_too)

{

    QEN_HASH_LINK  *hlink = hbase->hsh_currlink;
    QEN_HASH_PART  *hpptr;
    i4		i, spillmask, flushmask;
    DB_STATUS	status;
 

    /* Search partition array for "spilled" partitions. Write last
    ** block for each, then close the file. */

    if (hbase->hsh_flags & HASH_ROLE_REVERSE)
    {
	spillmask = HSP_PSPILL;
	flushmask = HSP_PFLUSHED;
    }
    else
    {
	spillmask = HSP_BSPILL;
	flushmask = HSP_BFLUSHED;
    }

    for (i = 0; i < hbase->hsh_pcount; i++)
    {
	hpptr = &hlink->hsl_partition[i];
	/* Skip flushed or nonexistent partitions */
	if (hpptr->hsp_flags & flushmask || hpptr->hsp_bptr == NULL)
	    continue;
	/* Flush if spilled, or nonspilled and caller requests */
	if ( (hpptr->hsp_flags & spillmask) || nonspilled_too)
  	{
	    status = qen_hash_flushone(dsh, hbase, hpptr);
  	    if (status != E_DB_OK)
		return (status);
  	}
    }

    return(E_DB_OK);

} /* qen_hash_flush */

/*{
** Name: QEN_HASH_FLUSHONE	- write out contents of one partition buffer
**				and close its file
**
** Description:
**	This routine writes the specified partition buffer to the
**	spill file (build, or probe if reverse set).  If the partition
**	hasn't spilled yet, we force it out assuming that the caller
**	knows what's up.  Do not call if there is no partition buffer
**	at all!
**
** Inputs:
**	dsh		- Thread execution control header
**	hbase		- ptr to base hash structure
**	hpptr		- ptr to partition to spill
**
** Outputs:
**
** Side Effects:
**
**
** History:
**	17-Jun-2006 (kschendel)
**	    Split off from qen_hash_flush, because I thought I would need
**	    this separately.  Apparently not, but seems better anyway.
**
*/
 
static DB_STATUS
qen_hash_flushone(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase,
QEN_HASH_PART	*hpptr)

{
    DMH_CB	*dmhcb;
    QEN_HASH_FILE  *fptr;
    i4		spillmask, flushmask;
    DB_STATUS	status;
    bool	reverse = (hbase->hsh_flags & HASH_ROLE_REVERSE) != 0;
 

    /* Set up masks based on reversal */

    if (reverse)
    {
	spillmask = HSP_PSPILL;
	flushmask = HSP_PFLUSHED;
    }
    else
    {
	spillmask = HSP_BSPILL;
	flushmask = HSP_BFLUSHED;
    }

    if ((hpptr->hsp_flags & spillmask) == 0)
    {
	/* Forced spill, open the appropriate spill file */
	status = qen_hash_open(dsh, hbase, hpptr, OPEN_WRITE);
	if (status != E_DB_OK)
  	    return(status);
	hpptr->hsp_flags |= spillmask;
    }

    /* There is always data in the buffer of a spilled partition.
    ** The flag isn't set (and a write isn't done) until there's
    ** enough data to partially fill the next block. */
    fptr = (reverse) ? hpptr->hsp_pfile : hpptr->hsp_bfile;
    status = qen_hash_write(dsh, hbase, fptr, hpptr->hsp_bptr, 
					&hpptr->hsp_currbn);
    if (status != E_DB_OK) 
	return(status);

    status = qen_hash_close(hbase, &fptr, 0);
    hpptr->hsp_flags |= flushmask;
    hpptr->hsp_offset = 0;
    hpptr->hsp_currbn = 0;
    return(status);
  
} /* qen_hash_flushone */

/*{
** Name: QEN_HASH_FLUSHCO	- special version of qen_hash_flush for
**				use in CO joins (hjn_kuniq == 1)
**
** Description:
**	This function allocates files and writes the buffer for each
**	probe partition of a CO join that hasn't already spilled to
**	disk. This is necessary because the corresponding build partition
**	must be reloaded and will overlay the probe rows. 
**	Probe partitions that are spilled but not flushed, are flushed.
**
**	Ordinarily the probe partitions that fit in memory would be 
**	loaded in the hash table and processed with role reversal, 
**	but CO joins can't handle role reversal.
**
** Inputs:
**	dsh		- DSH execution control block
**	hbase		- ptr to base hash structure
**
** Outputs:
**
** Side Effects:
**
**
** History:
**	25-oct-01 (inkdo01)
**	    Written to fix CO join problem.
**	29-may-03 (inkdo01)
**	    Exclude partitions already finished (bug 110329).
*/
 
static DB_STATUS
qen_hash_flushco(
QEE_DSH		*dsh,
QEN_HASH_BASE	*hbase)

{

    QEN_HASH_LINK  *hlink = hbase->hsh_currlink;
    QEN_HASH_PART  *hpptr;
    i4		i;
    DB_STATUS	status;
    

    /* Search partition array for partitions with nonempty probe
    ** side that isn't probe-flushed.  If the partition isn't probe-
    ** spilled, open a file for it.  Then, flush the last or only block,
    ** and close the file.
    */

    hbase->hsh_flags |= HASH_ROLE_REVERSE;	/* temporarily set for 
						** our callees */

    for (i = 0; i < hbase->hsh_pcount; i++)
    {
	hpptr = &hlink->hsl_partition[i];
	/* Reinit. these guys, regardless. */
	if ((hpptr->hsp_flags & (HSP_PFLUSHED | HSP_DONE)) ||
		hpptr->hsp_prcount == 0) continue;

	/* Force it out */
	status = qen_hash_flushone(dsh, hbase, hpptr);
	if (status != E_DB_OK) 
	    return(status);
    }

    hbase->hsh_flags &= ~HASH_ROLE_REVERSE;	/* extinguish flag */

    return(E_DB_OK);

}


/*{
** Name: QEN_HASH_CLOSE		- closes an existing partition file and
**				places it on free chain
**
** Description:
**	This function closes an existing partition file and, if file
**	isn't immediately being reread (to load it into the hash table
**	or to repartition it), places its context block on the free
**	chain for possible reuse. 
**
**	The flag controls what happens to the file.  No flag simply
**	turns around the file for re-reading.  (As it happens, this is
**	a no-op.)  CLOSE_RELEASE says to keep the file but put its
**	file context struct on a free-list for re-use.   CLOSE_DESTROY
**	says to close and delete the file; the file context block is
**	placed on a (different) avail-list for re-use.
**
** Inputs:
**	hbase		- ptr to base hash structure
**	hfptr		- ptr to partition descriptor ptr
**	flag		- flag: none, CLOSE_RELEASE, or CLOSE_DESTROY
**
** Outputs:
**
** Side Effects:
**
**
** History:
**	9-apr-99 (inkdo01)
**	    Written.
**	17-may-05 (inkdo01)
**	    Added code to remove destroyed files from open list.
**	3-Jun-2005 (schka24)
**	    Put destroyed file contexts on an avail list.
*/
 
static DB_STATUS
qen_hash_close(
QEN_HASH_BASE	*hbase,
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
	fptr->hsf_nextf = hbase->hsh_freefile;
	hbase->hsh_freefile = fptr;
	return(E_DB_OK);
    }

    /* Load up DMH_CB and issue the call. */
    dmhcb = (DMH_CB *)hbase->hsh_dmhcb;
    dmhcb->dmh_locidxp = &fptr->hsf_locidx;
    dmhcb->dmh_file_context = fptr->hsf_file_context;
    dmhcb->dmh_func = DMH_CLOSE;
    dmhcb->dmh_flags_mask = DMH_DESTROY;
    /* Search for file on open chain and remove it. */
    for (wfptr = &hbase->hsh_openfiles; (*wfptr) && (*wfptr) != fptr;
		wfptr = &(*wfptr)->hsf_nexto);
    if ((*wfptr))
	(*wfptr) = fptr->hsf_nexto;

    /* Add file block to re-use list */
    fptr->hsf_nextf = hbase->hsh_availfiles;
    hbase->hsh_availfiles = fptr;
    dmhcb->dmh_flags_mask = DMH_DESTROY;

    return(dmh_close(dmhcb));


}

/*{
** Name: QEN_HASH_CLOSEALL	- closes and destroys all files on hash
**				control block's file chain
**
** Description:
**	This function loops over the hsh_openfiles chain, issuing 
**	qen_hash_close calls with the DESTROY option to free them all.
**	It is called at the end of hash join processing.
**
** Inputs:
**	hbase		- ptr to base hash structure
**
** Outputs:
**
** Side Effects:
**	The open and free file lists are cleared, and all the file
**	blocks are left on the "available" list.
**
**
** History:
**	3-dec-99 (inkdo01)
**	    Written.
**
*/
 
DB_STATUS
qen_hash_closeall(
QEN_HASH_BASE	*hbase)

{
    QEN_HASH_FILE	*hfptr, *savefptr;
    DB_STATUS		status = E_DB_OK;


    /* Simply loop over file list, qen_hash_close'ing each entry. */
    for (hfptr = savefptr = hbase->hsh_openfiles; hfptr; 
				hfptr = savefptr = savefptr->hsf_nexto)
    {
	status = qen_hash_close(hbase, &hfptr, CLOSE_DESTROY);
	if (status != E_DB_OK) 
	    return(status);
    }
    hbase->hsh_openfiles = hbase->hsh_freefile = (QEN_HASH_FILE *) NULL;
    return(status);

}

static VOID qen_hash_verify(QEN_HASH_BASE *hbase)
{
    i4 	htsize = hbase->hsh_htsize;
    QEF_HASH_DUP **htable = hbase->hsh_htable;
    QEF_HASH_DUP *hchain;
    i4 	i;

    if ( htable == NULL) return;

    for (i = 0; i < htsize; i++)
    {
	for (hchain = htable[i]; hchain; hchain = hchain->hash_next);
    }
}
