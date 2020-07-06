/*
**Copyright (c) 2004 Ingres Corporation
*/

/**
** Name: QEFNODE.H - Describe the nodes of a Query Execution Plan (QEP)
**
** Description:
**      A QEP is composed of one or more nodes describing some set of
**  required actions. The node types are
**  QEN_TJOIN   - tid join
**  QEN_KJOIN   - key probe join
**  QEN_SJOIN   - full sort/merge, inner_sorted, and cart_prod joins
**  QEN_HJOIN   - hash join
**  QEN_SEJOIN	- perform a join where inner is QEN_QP and executing
**		  a subselect
**  QEN_SORT    - initiate a sort
**  QEN_TSORT   - top sort node - load sorter and retrieve rows
**  QEN_ORIG    - get a row from a relation or sort file
**  QEN_QP      - get a row from a query plan
**  QEN_EXCH	- exchange rows between different threads executing
**		  the same plan
**  
**
** History:
**      19-mar-86 (daved)
**          written
**      7-apr-86 (daved)
**          references to sub nodes are now pointers to facilitate
**          argument passing. The use of pointers is OK in this instance
**          because they are not data dependent. Initialization
**          requires that the postfix notation be converted to pointers.
**	29-jan-87 (daved)
**	    added qen_sejoin
**	24-jul-87 (daved & puree)
**	    add qen_pjoin
**	25-jan-88 (puree)
**	    redefine literals for sejn_junc in QEN_SEJOIN.
**	26-sep-88 (puree)
**	    modified QEN_SORT and QET_TSORT for QEF in-memory sorter.
**	    moved QEN_HOLD into QEFDSH.H
**	29-jan-89 (paul)
**	    Aded support for rules rpocessing.
**	27-mar-89 (jrb)
**	    Added .attr_prec field to QEF_KATT struct for decimal project.
**      02-nov-89 (davebf)
**          Modified to support outer join.
**	17-dec-90 (stec)
**	    Modified type and meaning of sejn_hcreate in QEN_SEJOIN struct.
**	18-dec-90 (kwatts)
**	    Smart Disk integration: Added sd_program and sd_pgmlength to
**	    the QEN_NODE structure.
**	25-mar-92 (rickh)
**	    To support right joins, upgraded the tid sort array to be a
**	    hold file.  This meant replacing oj_shd with oj_tidHoldFile.
**	    Removed oj_create and oj_load, which weren't being used.
**	    Added oj_ijFlagsFile for right fsmjoins.
**	11-aug-92 (rickh)
**	    Added QEN_TEMP_TABLE, the descriptor for temporary tables.
**	04-dec-1992 (kwatts)
**	    Introduced QEN_TSB structure for Smart Disk and changed 
**	    description of QEN_ORIG to have this one field for Smart Disk.
**	26-mar-93 (jhahn)
**	    Added ttb_attr_count to QEN_TEMP_TABLE.
**	20-apr-93 (rickh)
**	    Added new DSH row to the OJINFO structure.  This row is where
**	    fulljoins expose to higher nodes the special equivalence classes
**	    they set.
**	20-jul-1993 (pearl)
**          Bug 45912.  Add qef_kor field to QEN_NOR structure to keep track
**          of which QEF_KOR structure it is associated with.
**          Add kor_id field to QEF_KOR structure.
**	07-feb-94 (rickh)
**	    Added key array and count to the temp table descriptor.
**      01-nov-95 (ramra01)
**          Join node optimization. Prepare noadf_mask to indicate 
**          presence of adf cbs for request codes, actually the
**          lack of adf cbs. 
**	7-nov-95 (inkdo01)
**	    Reorganized various node structures.
**	06-mar-96 (nanpr01)
**	    Added fields in QEN_SORT, QEN_TSORT, QEN_TEMP_TABLE for
**	    variable page support.
**      13-jun-97 (dilma04)
**          Added QEN_CONSTRAINT flag to QEN_ADF which indicates that 
**          procedure supports integrity constraint.
**      21-jan-1999 (hanch04)
**          replace nat and longnat with i4
**	26-feb-99 (inkdo01)
**	    Added hash join structure (QEN_HASHJOIN)
**      15-may-2000 (stial01)
**          Remove Smart Disk code
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	7-nov-03 (inkdo01)
**	    Added exchange node structure (QEN_EXCH)
**	7-Dec-2005 (kschendel)
**	    Rearrange a couple of the more common nodes to reduce
**	    excess intra-struct padding on 64-bits.  (Pointers to the
**	    front.)
**	12-Jan-2006 (kschendel)
**	    Get rid of the xcxcb.  Get rid of all the noadf-mask's, no
**	    longer used since '03.
**/

/*
**  Forward and/or External typedef/struct references.
*/
typedef struct _QEN_ADF QEN_ADF;
typedef struct _QEN_SJOIN QEN_SJOIN;
typedef struct _QEN_KJOIN QEN_KJOIN;
typedef struct _QEN_HJOIN QEN_HJOIN;
typedef struct _QEN_NKEY QEN_NKEY;
typedef struct _QEN_PART_INFO QEN_PART_INFO;
typedef struct _QEN_PART_QUAL QEN_PART_QUAL;
typedef struct _QEN_OJINFO QEN_OJINFO;
typedef struct _QEN_NODE QEN_NODE;
typedef struct _QEN_NOR QEN_NOR;
typedef struct _QEN_ORIG QEN_ORIG;
typedef struct _QEN_QP QEN_QP;
typedef struct _QEN_SEJOIN QEN_SEJOIN;
typedef struct _QEN_SORT QEN_SORT;
typedef struct _QEN_TJOIN QEN_TJOIN;
typedef struct _QEN_TKEY QEN_TKEY;
typedef struct _QEN_TSORT QEN_TSORT;
typedef struct _QEN_EXCH QEN_EXCH;
typedef struct _QEN_TEMP_TABLE QEN_TEMP_TABLE;


typedef struct _QEF_AHD	QEF_AHD;
typedef struct _QEF_KAND QEF_KAND;
typedef struct _QEF_KATT QEF_KATT;
typedef struct _QEF_KEY QEF_KEY;
typedef struct _QEF_KOR	QEF_KOR;
typedef struct _QEF_QP_CB QEF_QP_CB;

/*}
** Name: QEN_BASE - initialize the base array for CX structs
**
** Description:
**      These elements describe the contents of array elements in the
**  ADE_CX struct.
**
** History:
**      31-jul-86 (daved)
**          written
**	23-aug-04 (inkdo01)
**	    Add union to store dsh_row index for parms.
[@history_line@]...
[@history_template@]...
*/
typedef struct _QEN_BASE
{
    i4	    qen_index;		/* offset in next array to find object
				** this value should be zero if the array
				** is QEN_VTEMP or QEN_DMF_ALLOC
				*/
    i4	    qen_array;		/* what array is offset in */
#define QEN_ROW		0	/* get a row address */
#define	QEN_KEY		1	/* get a key. offset should be set to 0
				** because all keys are in the same buffer
				*/
#define	QEN_PARM	2	/* get a user defined parameter */
#define	QEN_OUT		3	/* user's output buffer. offset should be 0 */
#define	QEN_VTEMP	4	/* space holder for CX VLUP temp buffer */
#define	QEN_DMF_ALLOC	5	/* space holder for tuple provided by DMF */
#define QEN_ADF_AG	6	/* adf_ag_struct address in dsh_row array */
#define	QEN_PLIST	7	/* Get a call procedure parameter list	    */
				/* address */
    union {
	i4	qen_parm_row;	/* dsh_row buffer index for QEN_PARM */
	i4	qen_fid;	/* function instance if QEN_ADF_AG (obsolete
				** with BE aggregate rewrite) */
    } u;
    i4	    qen_ag_len;		/* length of intermediate aggregate value for
				** adf aggregate processing.
				*/
}   QEN_BASE;


/*}
** Name: QEN_ADF - QEF's ADF CX strucuture initializer
**
** Description:
**	This structure contains all the elements necessary
**  to process ADF compiled buffers. This includes
**  initialization parameters and the actual compiled buffer.
**
** History:
**     08-May-86 (daved)
**          written
**     01-sep-88 (puree)
**	    rename qen_adf to qen_ade_cx.
**	    rename qen_param to qen_pcx_idx and used it as the index (relative
**	    to 1) to the QEE_VLT array in QEE_DSH area.  This allows us to
**	    associate a CX with user's parameters to its VLUP/VLT space.
**      15-may-94 (eric)
**          Added QEN_HAS_PARAMS so that CX knows whether repeat query 
**          or db proc params are used in the CX.
**	16-oct-95 (inkdo01)
**	    Added QEN_HAS_PERIPH_OPND to indicate long_something operand
**	    (knowledge of this speeds up ADF execution logic)
**      13-jun-97 (dilma04)
**          Added QEN_CONSTRAINT flag to indicate that procedure supports
**          integrity constraint.
*/
struct _QEN_ADF
{
    PTR		qen_ade_cx;	/* the compiled expression. Will be ADE_CX.
				** If the pointer to the qen-adf in a node or
				** action header is non-NULL, this will be
				** non-NULL (ie there will be a CX there).
				*/
    QEN_BASE    *qen_base;	/* array of values to initialize entries in
				** base array from location ADE_ZBASE on.
				** Entry N in this array initiliazes entry
				** ADE_ZBASE+N in the base array.
				*/
    i4		qen_pos;	/* index into DSH structure (dsh_cbs)
				** containing the ADE_EXCB parameter
				*/
    i4		qen_max_base;	/* number of elements in qen_base array */
    i4		qen_sz_base;	/* Number of elements used in qen_base.
				** the base array in the ADE_EXCB struct
				** will be ADE_ZBASE + qen_sz_base elements 
				** long.
				*/
    i4		qen_pcx_idx;	/* TRUE if there parameters in the qen_base
				** This forces the base array to be initialized
				** at validation time.
				**
				*/
    i4		qen_uoutput;	/* index into qen_base containing QEN_BASE entry
				** for user defined output row if output
				** is user's output buffer. This only applies
				** for ACT_GET action items. When compiling
				** expressions, make no assumptions as to the
				** time the output buffer will be placed in the
				** CX struct.
				** If this is a QEA_RUP action, this output
				** value is used to contain the tuple
				** in the updatable relation.
				*/
    i4		qen_output;	/* Index into DSH->DSH_ROW containing output
				** row. Set to -1 if output is not a row
				*/
    i4		qen_input;	/* index into qen_base containing initializer
				** for DMF supplied tuple buffer. 
				*/
    i4		qen_mask;	/* Bit mask of CX info */
	/* The QEN_SVIRGIN, QEN_SINIT, QEN_SMAIN, and/or QEN_SFINIT bit(s)
	** will be on if the CX has virgin, init, main, and/or finit
	** segment instructions compiled into it.
	*/
#define	    QEN_SVIRGIN	    0x001
#define	    QEN_SINIT	    0x002
#define	    QEN_SMAIN	    0x004
#define	    QEN_SFINIT	    0x008

	/* QEN_HAS_PARAMS is set if the base array uses either QEN_PARM or
	** QEN_PLIST elements.
	*/
#define	    QEN_HAS_PARAMS  0x010
	/* QEN_HAS_PERIPH_OPND indicates that at least one operand of at
	** least one operation of this code block is long_something.
	*/
#define     QEN_HAS_PERIPH_OPND 0x020

        /* QEN_CONSTRAINT indicates that procedure supports integrity
        ** constraint.
        */
#define     QEN_CONSTRAINT      0x040
};



/*}									     
** Name: QEN_TEMP_TABLE - Descriptor for temporary tables
**
** Description:								       
**	This descriptor contains all the information needed to allocate
**	a run time temporary table.
**
**
**
** History:								       
**	11-aug-92 (rickh)
**	    Created.
**	28-Oct-92 (jhahn)
**	    Added ttb_attr_list.
**	26-mar-93 (jhahn)
**	    Added ttb_attr_count.
**	07-feb-94 (rickh)
**	    Added key array and count to the temp table descriptor.
**	06-mar-96 (nanpr01)
**	    Added page size for temp table .
*/									       
struct	_QEN_TEMP_TABLE
{
    i4		ttb_tempTableIndex;	/* index into dsh array of temp table
					** descriptors.  -1 means no temp
					** table */

    i4		ttb_tuple;		/* index into dsh rows of storage
					** for this table's tuple */

    DMT_ATTR_ENTRY **ttb_attr_list;	/* pointer to list of attributes
					** for this table. If NULL, create
					** attribute list at run time. */
    i4		ttb_attr_count;		/* number of attributes or 0 */

    DMT_KEY_ENTRY  **ttb_key_list;	/* pointer to array of keys for this
					** table. */
    i4		ttb_key_count;		/* number of keys or 0 */
    i4	ttb_pagesize;		/* Temp table page size */
};

/*}									     
** Name: QEN_OJINFO - This structure holds information that is		        
**		      specific to "outer" join processing                       
**
** Description:								       
**  This structure holds information that is common to all join nodes that     
**  is required for "outer" join processing.				       
**									       
**  Oj_jntype tells the type of join to do. The join types INNER, LEFT,        
**  RIGHT, and FULL are legal here.					       
**									       
**  If oj_jntype indicates that a RIGHT or FULL join is to be done then	       
**  oj_rqual will be used to qualify right joins. In order for a right tuple   
**  to satisfy the outer join qualification, oj_rqual must return TRUE for     
**  each left tuple. Once a right tuple satisfies an outer join qualification  
**  oj_rnull should be executed, which will fill in NULL/default values for    
**  the left side tuple, before the right tuple is returned.		       
**									       
**  Oj_lqual and oj_lnull should be used for the left tuples in a like manner  
**  if oj_jntype is LEFT or FULL.					       
**									       
** History:								       
**	28-mar-86 (eric) 						       
**          written							       
**	25-mar-92 (rickh)
**	    To support right joins, upgraded the tid sort array to be a
**	    hold file.  This meant replacing oj_shd with oj_tidHoldFile.
**	    Removed oj_create and oj_load, which weren't being used.
**	    Added oj_heldTidRow also.  Added oj_ijFlagsFile for right fsmjoins.
**	20-apr-93 (rickh)
**	    Added new DSH row to the OJINFO structure.  This row is where
**	    fulljoins expose to higher nodes the special equivalence classes
**	    they set.
**	7-nov-95 (inkdo01)
**	    Replaced QEN_TEMP_TABLE instance by pointer.
*/									       
struct _QEN_OJINFO						       
{									       
	/* *Oj_oqual qualifies a join which has passed the kqual join.         
	** If oqual is true, we have an inner join candidate.  If not,       
	** we may still have an outer join candidate.
	*/
    QEN_ADF	*oj_oqual;						       

	/* *Oj_equal processes a pair of rows which have passed both  
	** the kqual join and the oqual join  (ie. an inner join candidate)
	*/
    QEN_ADF	*oj_equal;						       

	/* Oj_rnull and oj_lnull provide the appropriate null/default values   
	** for a satisfied right or left outer join (respectively).
	*/								       
    QEN_ADF	*oj_rnull;						       
    QEN_ADF	*oj_lnull;						       

	/*
	** This describes a temporary table which remembers the TIDs of
	** inner hold file tuples which inner joined.  At the end of 
	** ISJOIN processing, this table is rescanned.  All inner tuples not 
	** represented in this file are emitted as outer joins.
	*/
    QEN_TEMP_TABLE	*oj_innerJoinedTIDs;

	/*								       
	/* Oj_jntype tells whether we are doing an inner, left, right, or
	** full join
	*/
    DB_JNTYPE	oj_jntype;						       
									       
	/*
	** oj_tidHoldFile is an index into the DSH_HOLD array for the hold
	** file of tids used for right joins.  oj_heldTidRow is the dsh row
	** holding the tid.
	*/								       
    i4         oj_tidHoldFile;
    i4		oj_heldTidRow;

	/*
	** oj_ijFlagsFile is an index into the DSH_HOLD array for the hold
	** file of flags indicating which inner tuples inner joined with some
	** outer.  This is used for detecting right fsmjoins.  When a given
	** outer key has been exhausted, this hold file is rescanned.  Any
	** inner tuple on this key which hasn't yet joined with an outer is
	** then emitted as a right join.
	*/
    i4		oj_ijFlagsFile;
    i4		oj_ijFlagsRow;

	/*
	** This DSH row holds the equivalence classes overwritten by this
	** node.  Descendants of this node may force their own special
	** eqcs and fill in other eqcs.  It would be bad policy to overwrite
	** our descendants' eqcs
	** with transitory values.  For instance, it used to
	** be that an RNULL would overwrite eqcs from the
	** driving stream.  Unfortunately,
	** it could happen that after emitting a right join, an fsmjoin
	** could immediately emit its current outer tuple as a left join--
	** inheriting the overwritten eqcs from the driving stream.
	*/

    i4			oj_resultEQCrow;

	/*
	** This DSH row holds the "indicator" variable for the source of
	** an outer join whose columns are referenced higher in the query
	** plan.
	*/

    i4			oj_specialEQCrow;
};									       


/*}									     
** Name: QEN_PART_INFO	- common information on partitioned tables
**
** Description:								       
**	This descriptor contains information necessary to execute
**	KJOIN, ORIG and TJOIN nodes involving partitioned tables.
**
**	There are really two main parts here:  zero or more
**	QEN_PART_QUAL entries, which are for partition qualification
**	(aka partition elimination);  and optional information
**	for doing partition compatible joining, PC-joining.
**	For now, we can qualify against multiple partitioning
**	dimensions, but only PC-join along one dimension.
**	Since single dimensions are almost sure to be the most
**	common, the part-info struct has a single part-qual
**	entry;  more can be chained to if needed.
**
**	For partition qualification we need a CX;  partition
**	dimension info;  and constant row info.  The CX holds or
**	generates a series of query-constant values that are stuffed
**	through the which-partition computer to qualify partitions.
**	The result of partition qualification is a physical partition
**	bit-map, with qualifying bits turned on.
**
**	For PC joining, we need partition dimension info and a group
**	count, indicating how many logical partitions should be
**	returned before indicating end-of-group.  For details see
**	qenorig.c.
**
** History:								       
**	2-feb-04 (inkdo01)
**	    Written for table partitioning.
**	13-apr-04 (inkdo01)
**	    Additions/deletions for partition compatible join logic.
**	29-apr-04 (inkdo01)
**	    A few more for combined partition compatible joins/partition
**	    qualification.
**	22-Jul-2005 (schka24)
**	    Add a work bitmap row index for multi-dimensional qualification.
**	16-Jun-2006 (kschendel)
**	    Factor out partition qualification stuff so that we can
**	    qualify against multiple dimensions.
*/

/* Qualification info struct, need one per dimension.  The first one
** is built into the part-info struct.  (It might not be in use,
** check part_qdim != NULL or part_qpix >= 0 to make sure qual is
** being done at all.)
*/

struct _QEN_PART_QUAL
{
    QEN_PART_QUAL *part_qnext;	/* Next QUAL struct, if multi dim qual */
    QEN_ADF	*part_qpart;	/* qualify partitions */
    DB_PART_DIM	*part_qdim;	/* ptr to dimension descriptor to 
				** generate partitions from qpart output
				** for partition qualification */
    i4		part_qpcix;	/* index to dsh_row ptr to partition
				** qualification constant buffer */
    i4		part_prowcnt;	/* number of "rows" in partition
				** qualification constant buffer */
    i4		part_qflags;	/* Qualification flags (for between) */
				/* Note: at present, only LE and GE are used */

#define PART_QUAL_ALL	0x1	/* TRUE if all partitions of a partitioned table
				** are to be read */
#define PART_QUAL_RANGE	0x2	/* TRUE if range notation is to be used to
				** process entries in partition array */
#define PART_QUAL_GROUP	0x4	/* TRUE if groups of partitions are to be read
				** with "end of partition" after each */
#define	PART_QUAL_LE	0x8	/* TRUE if qualified partitioned table and 
				** qualification is range LT or LE */
#define	PART_QUAL_GE	0x10	/* TRUE if qualified partitioned table and 
				** qualification is range GT or GE */
};

struct	_QEN_PART_INFO
{
    QEN_PART_QUAL part_qual;	/* Qualification struct (not ptr!) for first
				** qualification dimension */
    DB_PART_DIM	*part_pcdim;	/* ptr to dimension descriptor to 
				** generate partitions for partition
				** compatible join */
    i4		part_gcount;	/* count of log. partitions in a "group"
				** (before end of partition is returned) */
    i4		part_totparts;	/* number of physical partitions in table */
    i4		part_qpix;	/* index to dsh_row ptr to qualified 
				** (physical) partition bit map */
    i4		part_qpwlix;	/* index to dsh_row ptr to work bit-map for
				** partition qualification.  */
    i4		part_qpwpix;	/* Index for another partition qual work map
				** this one is for logical-to-physical map */
    i4		part_dmrix;	/* index to dsh_cbs entry for 1st 
				** partition DMR_CB */
    i4		part_dmtix;	/* index to dsh_cbs entry for 1st 
				** partition DMT_CB */
    i4		part_threads;	/* count of threads accessing partitions
				** (for || query plans), or 1 */
};

/*}
** Name: QEN_SJOIN  - Join info for QE_FSMJOIN, QE_ISJOIN, and QE_CPJOIN nodes 
**									       
** Description:								       
**	This structure is used for the nodes QE_FSMJOIN (full sort merge),     
**	QE_ISJOIN (inner sort join), and QE_CPJOIN (cart-prod join).	       
**	The full sort merge join assumes that the data coming from both	       
**	the inner and outer nodes is sorted. The inner sort join assumes       
**	that the data from the inner node is sorted (not that the inner node   
**	is a sort node), and that the outer is not sorted. Finally, the	       
**	cart-prod join assumes that the data from neither the inner nor the    
**	outer is sorted. These assumptions about the sortedness of data	       
**	are used to minimize the number of tuple fetches and comparisons.      
**									       
**	The QE_ISJOIN and QE_CPJOIN nodes will need to rescan the data from    
**	the inner node. Sometimes it is more efficient to store the inner data 
**	in a hold file and rescan this data. Other times is is more efficient  
**	to simply rescan the inner node directly. The optimizer will make      
**	this decision, and indicate the decision to QEF through the sjn_hget   
**	and sjn_hcreate fields. When RIGHT, or FULL joins are required, then   
**	hold files will always be provided, since they are required for	       
**	processing.							       
**									       
**	The QE_FSMJOIN will never rescan the inner data from the beginning.    
**	Instead, a full sort merge will, at times, rescan small portions of    
**	of the inner data. Since the inner node is only able to reset itself   
**	to the beginning of it's data, a hold file will always be provided     
**	for a full sort merge.						       
**									       
** History:								       
**	14-July-89 (eric)						       
**	    created from the old QEN_JOIN.				       
**	25-mar-92 (rickh)
**	    Removed sjn_hcreate and sjn_hget, which weren't being used.
**      01-nov-95 (ramra01)
**          Join node optimization. Prepare noadf_mask to indicate
**          presence of adf cbs for request codes, actually the
**          lack of adf cbs.
**	7-nov-95 (inkdo01)
**	    Replaced QEN_ADF and QEN_OJINFO structure instances by pointers.
*/									       
struct _QEN_SJOIN						       
{									       
    QEN_NODE   *sjn_out;       /* outer node */				       
    QEN_NODE   *sjn_inner;     /* inner node */				       
									       
	/*								       
	** sjn_hfile is an index into the DSH_HOLD array for the hold	       
	** file structure.
	*/								       
    i4         sjn_hfile;						       
									       
    i4		sjn_krow;	/* DSH buffer for key value comparison
				*/
    bool        sjn_kuniq;      /* is the inner relation uniquely keyed	       
				** or do we only want one value from inner     
				** (existence only) TRUE-FALSE		       
				*/					       
    QEN_ADF    *sjn_itmat;	/* materialize the inner tuple */	       
    QEN_ADF    *sjn_okmat;	/* materialize the join key into the	       
				** a row buffer				       
				*/					       
    QEN_ADF    *sjn_okcompare;	/* compare current key created by okmat	       
				** to the next key in sjn_outer.	       
				*/					       
    QEN_ADF    *sjn_joinkey;	/* qualify tuples on the join key */	       
    QEN_ADF    *sjn_jqual;	/* qualify the joined tuple */		       
    QEN_OJINFO *sjn_oj;		/* "outer" join information */		       

};									       

/*}
** Name: QEN_KJOIN - keyed lookup on inner node
**
** Description:
**      The outer node generates a set of attributes that can
**  be used to key the inner relation. For each outer tuple, the
**  inner relation is accessed for tuples that match the joining attributes
**  and satisfy applicable qualifications.
**
**	In the keyed join, the outer tuple is set by calling the proper
**  function on the kjoin_out node. Kjoin_key produces the attributes in the
**  join that can facilitate access of the inner relation. These attributes
**  are materialized into a row buffer stored in a row buffer, the row
**  buffer number is the one used in the output slot of the ADE param in
**  kjoin_key. This row buffer is also used by kjoin_get. Once the keys
**  are materialized, kjoin_get is used to access the inner relation and
**  generate joined tuples. kjoin_jqual is used to further qualify the tuple.
**  
**  Kjoin_kys specifies the keys (attribute number, operator (<,>,=,etc),
**  and offset within key row buffer. 
**
**  Kjoin_kuniq is used to limit scans of the inner relation. If this field
**  is set (TRUE), then for a unique outer, only one inner tuple will be
**  found. Thus, kuniq should only be set if doing an equality join.
**  Kjoin_kcompare is used to compare the current key values to the
**  previous key values. This is needed so that we can tell if the next join
**  value is the same as the current join value.
**
**  NOTE on qualifying tuple. If a tid is in the qualification, coming from
**  the inner relation, kjoin_tqual must be set, else cleared. If it is set,
**  kjoin will qualify the tuple, else DMF will qualify the tuple. In the latter
**  case, the tuple begin qualified will be a QEN_DMF_ALLOC in the 
**  qualification's QEN_ADF struct. As such, the qen_input field of the
**  qualification's QEN_ADF struct will need to be initialized with the
**  base number of the QEN_DMF_ALLOC row.
**
** History:
**     28-mar-86 (daved)
**          written
**      01-nov-95 (ramra01)
**          Join node optimization. Prepare noadf_mask to indicate
**          presence of adf cbs for request codes, actually the
**          lack of adf cbs.
**	7-nov-95 (inkdo01)
**	    Replaced QEN_ADF and QEN_OJINFO structure instances by pointers.
**	12-jun-96 (inkdo01)
**	    Added kjoin_nloff to locate null indicator of  nullable outer.
**	12-nov-98 (inkdo01)
**	    Added kjoin_ixonly.
**	13-apr-04 (inkdo01)
**	    Added kjoin_pcompat for partitioning.
**	21-jan-05 (inkdo01)
**	    Added kjoin_find_only.
*/
struct _QEN_KJOIN
{
    QEN_NODE   *kjoin_out;      /* outer node */
    QEN_ADF    *kjoin_key;	/* produce the join key */
				/* not used for QEN_PJOIN */
    QEN_ADF    *kjoin_kcompare;	/* compare current join value to previous */
				/* not used for QEN_PJOIN */
    QEN_ADF    *kjoin_kqual;	/* high level outer-inner compare */
    QEN_ADF    *kjoin_jqual;	/* qualify joined rows */
    QEN_ADF    *kjoin_iqual;	/* qualify inner tuple only */
    QEF_KEY    *kjoin_kys;      /* description of keys */
    QEN_OJINFO *kjoin_oj;	/* "outer" join information */		       
    QEN_PART_INFO *kjoin_part;	/* partitioning info (if target is
				** partitioned table) */
    i4         kjoin_get;      /* Index into DSH->DSH_CBS to find DMR_CB.*/
    i2		kjoin_nloff;	/* offset in key buffer of null indicator
				** (if outer is nullable) - Rtree only */
    bool        kjoin_kuniq;    /* is the inner relation uniquely keyed
				** or do we only want one value from inner
				** (existence only) (TRUE-FALSE).
				*/
    bool	kjoin_ixonly;	/* TRUE if access is to index leaf nodes of
				** inner table base table (Btree), because all
				** needed cols are index keys.
				*/
    bool	kjoin_pcompat;	/* TRUE if join partition definition and
				** key structure are identical; i.e., if each 
				** group of outers map to exactly one 
				** partition */
    bool	kjoin_find_only; /* TRUE if row doesn't need to be materialized
				** for QEF (semijoin) */
    i4		kjoin_tid;	/* Offset of location to place tid if needed.
				** else, set to -1.
				*/
    i4		kjoin_tqual;	/* set to true if a tid is used to qualify
				** a tuple in the inner relation. QEF must
				** perform the qualification itself because
				** QEF will put the tid into place.
				*/
    i4		kjoin_krow;	/* DSH row buffer to hold key for compares
				*/

};

/*}
** Name: QEN_HJOIN  - Join info for QE_HASHJOIN node
**									       
** Description:								       
**	This structure is used for the node QE_HASHJOIN. Hash joins make 
**	no assumption concerning input source ordering. Instead, they 
**	build a hash table from the outer join source and probe it with
**	rows of the inner source (using the same hash function on the 
**	join columns of each source) looking for matching rows (i.e. join
**	candidates). 
**									       
** History:								       
**	26-feb-99 (inkdo01)
**	    Created (cloned) from QEN_SJOIN.
**	20-dec-99 (inkdo01)
**	    Dropped a few superfluous fields and added hjoin_bmem.
**	8-feb-00 (inkdo01)
**	    Tidied up header.
**	7-Dec-2005 (kschendel)
**	    Replace attr descriptors with DB_CMP_LIST entries.
*/
struct _QEN_HJOIN
{
    QEN_NODE   *hjn_out;        /* outer node */
    QEN_NODE   *hjn_inner;      /* inner node */
    QEN_ADF    *hjn_btmat;	/* materialize the build (outer) tuple */
    QEN_ADF    *hjn_ptmat;	/* materialize the probe (inner) tuple */
    QEN_ADF    *hjn_jqual;	/* qualify the joined tuple */
    QEN_OJINFO *hjn_oj;		/* "outer" join information */
    DB_CMP_LIST *hjn_cmplist;	/* Join column key descriptors.  Note:
				** If all keys are non-nullable, opc reduces
				** this to a single BYTE(n) key of the
				** appropriate length.
				*/
    i4		hjn_hkcount;	/* count of key descriptors */

    i4		hjn_brow;	/* buffer into which build rows materialize */
    i4		hjn_prow;	/* buffer into which probe rows materialize */
    i4		hjn_bmem;	/* estimated memory for whole build source
				** (est. tuples * build row size) */
    i4  	hjn_hash;	/* is the index into the dsh_hash array
				** of the hash structure entry for this join
				*/

    i4		hjn_dmhcb;	/* index into dsh_cbs of DMH_CB for hash
				** partition file operations
				*/

    bool	hjn_kuniq;	/* existence only hash join */
    bool	hjn_nullablekey;  /* TRUE if any key is nullable */

				/* If we get any more bools make them flags */
};									       

/*}
** Name: QEN_TJOIN - Perform a tid lookup into the inner node
**
** Description:
**   An outer row value is positioned by calling the function that
**  will produce the outer row. The node CB for the outer tuple is in
**  tjoin_out. Tjoin_get is an index into the DSH->DSH_CBS that refers
**  to a positioned DMR_CB. tjoin_key contains the structures necessary
**  to materialize the tid into the DMR_CB in tjoin_get so that the
**  inner row can be fetched. tjoin_jqual is used to qualify
**  the joined rows. Rows are retrieved from the outer and inner until
**  a qualified row is found.
**	
**
** History:
**     28-mar-86 (daved)
**          written
**	oct-90 (stec)
**	    introduced tjoin_coerce, tjoin_otidval, tjoin_crow, tjoin_coffset
**	22-oct-90 (davebf)
**	    removed tjoin_coerce, tjoin_otidval, tjoin_crow, tjoin_coffset
**      01-nov-95 (ramra01)
**          Join node optimization. Prepare noadf_mask to indicate
**          presence of adf cbs for request codes, actually the
**          lack of adf cbs.
**	7-nov-95 (inkdo01)
**	    Replaced QEN_ADF and QEN_OJINFO structure instances by pointers.
**	3-may-05 (inkdo01)
**	    Added tjoin_flag to house TJOIN_4BYTE_TIDP.
*/
struct _QEN_TJOIN
{
    QEN_NODE	    *tjoin_out;     /* outer node */
    i4		    tjoin_get;      /* Index into DSH->DSH_CBS for the DMR_CB
				    ** pointer.
				    */
    i4		    tjoin_orow;	    /* index into DSH->DSH_ROW for row 
				    ** containing tid for the join
				    ** (from the outer).
				    */
    i4		    tjoin_ooffset;  /* offset in above row of tid. */
    i4		    tjoin_irow;	    /* index into DSH->DSH_ROW for row 
				    ** containing tid for the join
				    ** (from the inner).
				    */
    i4		    tjoin_ioffset;  /* offset in above row of tid. */
    QEN_ADF	    *tjoin_jqual;   /* qualification for join */
    QEN_ADF	    *tjoin_isnull;  /* If not null, points to a CX to be
				    ** executed to determine if the TID from
				    ** the outer (described by tjoin_orow,
				    ** tjoin_ooffset) has a NULL value.
				    */ 		       
    QEN_OJINFO	    *tjoin_oj;	    /* "outer" join information */
    QEN_PART_INFO   *tjoin_part;    /* partitioning info (if target is
				    ** partitioned table) */
    i4		    tjoin_flag;	    /* flag mask */
#define	    TJOIN_4BYTE_TIDP	0x0001	/* TID is 4 bytes (probably a tidp
				    ** column from an index on a non-
				    ** partitioned table) - else 8 bytes */

};

/*}
** Name: QEN_SEJOIN  - select join node
**
** Description:
**	This join node is used when performing a subselect join. The inner
**  node must always be a QEN_QP node. When called, this node will get
**  an outer row and try to satisfy the qualification according to the
**  semantics of the select type {all or blank}. If all, then the inner
**  will be scanned and must reach EOF to succeed. If blank, then, if another
**  inner can be found, an error condition exists.
**
**  QEN_JUNC (junction type)
**	QESE1_CONT_AND
**	    The inner AND outer values must be TRUE before returning success.
**	    If either the inner or the outer fail then CONTinue processing
**	    with the next outer until there are no more outer rows.
**
**	QESE2_CONT_OR
**	    Like above, the inner OR outer values must be true before 
**	    returning success.  If the outer is TRUE, return success.  If
**	    the outer value fails but the inner is TRUE, return success.
**	    If both the outer and inner fail, the CONTinue processing with
**	    the next outer unless a ZERO_ONE error was generated.  In this 
**	    case, all processing should stop.
**
**	QESE3_RETURN_OR
**	    The inner OR outer values must be TRUE before returning success.
**	    If the outer is TRUE, return success.  If the outer fails but the
**	    inner value is TRUE, return success.  If both the outer and the
**	    inner fail, then return the worse error.  Notice: ZERO_ONE error
**	    is worse than NOT_ALL.
**
**	QESE4_RETURN_AND
**	    Like above, both the inner AND outer values must be TRUE before
**	    returning sucess.  However, if either the inner or the outer
**	    fails then return the error instead of continuing with the
**	    next outer.
**
**	    (per Eric memo - puree).
**
** History:
**     29-jan-87 (daved)
**          written
**     25-jan-88 (puree)
**	    redefine literals for sejn_junc.
**	17-dec-90 (stec)
**	    Modified type and meaning of sejn_hcreate in QEN_SEJOIN struct.
**	7-nov-95 (inkdo01)
**	    Replaced QEN_ADF structure instances by pointers.
**	12-Dec-2005 (kschendel)
**	    Replaced remaining QEN_ADF instances by pointers.
*/
struct _QEN_SEJOIN
{
    QEN_NODE   *sejn_out;       /* outer node */
    QEN_NODE   *sejn_inner;     /* inner node */
    i4		sejn_hget;	/* Index into DSH->DSH_CB for the DMR_CB 
				** to get/put tuple to hold file. 
				*/
    bool	sejn_hcreate;	/* A flag indicating if a hold file is to be
				** created by this node. Normally will be set to
				** TRUE. In certain cases, when inner QP trees
				** are shared by SEjoin nodes (and so is the 
				** corresponding hold file), all SEjoin nodes
				** concerned will have this flag set to FALSE
				** with the exception of the node that will be
				** executed first.
				*/ 
    i4          sejn_hfile;     /* hold file number - index into DSH->DSH_HOLD
				** structure. This number may be identical in 
				** more than one SEjoin node, which indicates
				** that the hold file is to be shared. See
				** description of sejn_hcreate.
				*/
    QEN_ADF	*sejn_itmat;	/* materialize the inner tuple */
    QEN_ADF	*sejn_okmat;	/* materialize the join key and join
				** correlation into the row buffer
				*/
    QEN_ADF	*sejn_ccompare; /* compare correlation key created by okmat
				** to the next key in sejn_outer.
				*/
    QEN_ADF	*sejn_kcompare;	/* compare current key created by okmat
				** to the next key in sejn_outer.
				*/
    QEN_ADF	*sejn_kqual;	/* qualify tuples on the join key. that's
				** all there can be.
				*/
    QEN_ADF	*sejn_oqual;	/* qualify the outer tuple */
    i4		sejn_seltype;	/* select node type */
#define SEL_ALL		    0	/* select all */
#define SEL_ZERO_ONE	    1	/* select */
#define SEL_ANY		    2	/* select any,some */
    i4		sejn_junc;	/* AND/OR processing semantics */
#define QESE1_CONT_AND	    1
#define QESE2_CONT_OR	    2
#define QESE3_RETURN_OR	    3 	
#define QESE4_RETURN_AND    4
/* fix me */
#define QESE1_CHECK_AND	    QESE1_CONT_AND
#define QESE2_RETURN_AND    QESE2_CONT_OR
#define QESE4_CHECK_OR	    QESE4_RETURN_AND
/* */
    i4		sejn_correlated;/* TRUE if correlated subselect */
    i4		sejn_oper;	/* operator type */
#define SEL_LESS	    -1	/* <,<= */
#define SEL_EQUAL	    0	/* = */
#define SEL_GREATER	    1	/* >, >= */
#define SEL_OTHER	    2	/* any other operator: !=, ...	*/
};

/*}
** Name: QEN_SORT - load the sort table
**
** Description:
**	Read rows into a temporary sort table. This node is used if its
**  parent is a QEN_SIJOIN and this node is the inner relation.
**  This node creates, opens and loads the sort relation. At a future time,
**  the table could be created with a separate action and opened in the
**  validation list.
**  
**
** History:
**     1-apr-86 (daved)
**          written
**     26-sep-88 (puree)
**	    Modified for QEF in-memory sorter.  Added sort_shd to identify
**	    the sort node number.
**	7-nov-95 (inkdo01)
**	    Replaced QEN_ADF structure instance by pointer.
**     06-mar-96 (nanpr01)
**	    Add page size parameter to pick up the right size page for
**	    sorting if DMF sort is used.
**	2-Dec-2005 (kschendel)
**	    Add compare-list pointer-array pointer.
[@history_line@]...
*/
struct _QEN_SORT
{
    QEN_NODE       *sort_out;   /* outer node */
    i4              sort_create;/* Index into DSH->DSH_CBS to find DMT_CB to
				** create sort temp.
				*/
    i4              sort_load;  /* Index into DSH->DSH_CBS to find DMR_CB to
				** load rows. The row used in this CB is found
				** in the qen_output field of the sort_mat
				** QEN_ADF struct.
				*/
    QEN_ADF	   *sort_mat;	/* materialize input tuple */
    /* elements used to initialize runtime environment */
    i4		    sort_dups;	/* DMR_NODUPLICATES if duplicates should 
				** be removed. Else, set to zero.
				*/
    i4		    sort_acount;/* number of attributes in created table */
    DMT_ATTR_ENTRY  **sort_atts;/* attributes to create in table */
    i4		    sort_sacount;/* number of attributes to sort on */
    DMT_KEY_ENTRY   **sort_satts;/* attributes to sort on */
    DB_CMP_LIST	    *sort_cmplist;  /* Compare-list array for sort keys */
    i4		    sort_shd;	/* index into dsh_shd to find the sort buffer
				** descriptor for this node */
    i4	    sort_pagesize;/* Sort page size     */
};



/*}
** Name: QEN_TSORT - top sort node
**
** Description:
**   The same as the QEN_SORT node except that subsequent calls to this
**  node return sorted tuples. That is, on the first call the sort table
**  is loaded. The first sorted tuple is then returned. On subsequent calls
**  the other sorted tuples are returned. This node is used as an outer
**  node to a join node or as the top sort node in a QEP.  Pretty much
**  anywhere that sorting is needed, other than the inside of an FSMJOIN.
**
** History:
**     1-apr-86 (daved)
**          written
**     26-sep-88 (puree)
**	    Modified for QEF in-memory sorter.  Added tsort_shd to identify
**	    the sort node number.
**	7-nov-95 (inkdo01)
**	    Replaced QEN_ADF structure instance by pointer.
**     06-mar-96 (nanpr01)
**	    Add page size parameter to pick up the right size page for
**	    sorting if DMF sort is used.
**	2-Dec-2005 (kschendel)
**	    Add compare-list pointer-array pointer.
[@history_line@]...
*/
struct _QEN_TSORT
{
    QEN_NODE           *tsort_out;      /* outer node */
    i4                  tsort_create;   /* Index into DSH->DSH_CBS to find
					** DMT_CB to create sort table
					*/
    i4                  tsort_load;     /* Index into DSH->DSH_CBS to find
					** DMR_CB to load sort table.
					** The row used in this CB is found
					** in the qen_output field of the 
					** tsort_mat QEN_ADF struct.
					*/
    QEN_ADF	       *tsort_mat;	/* materialize input tuple */
    i4                  tsort_get;      /* Index into DSH->DSH_CBS to find
					** DMR_CB control block for get call
					** to return sorted tuples
					** The row used in this CB is found
					** in the qen_output field of the 
					** tsort_mat QEN_ADF struct.
					*/
    /* elements used to initialize runtime environment */
    i4			tsort_dups;	/* DMR_NODUPLICATES if duplicates 
					** should be removed. Else, set to zero.
					*/
    i4			tsort_acount;	/* number of attributes in created 
					** table 
					*/
    DMT_ATTR_ENTRY	**tsort_atts;	/* attributes to create in table */
    i4			tsort_scount;	/* number of attributes to sort on */
    DMT_KEY_ENTRY	**tsort_satts;	/* attributes to sort on */
    DB_CMP_LIST		*tsort_cmplist;	/* Compare-list array for keys */
    i4			tsort_shd;	/* index to into dsh_shd to find the
					** sort buffer descriptor */
    i4		tsort_pagesize; /* Top Sort page size     */
};


/*}
** Name: QEN_ORIG - The origination node
**
** Description:
**	Zero or more keys are used to access a base relation and 
**  originate tuples into the QEP stream. If there are no keys, the
**  relation is scanned. Keys are used to limit the scan of the relation.
**
**  NOTE: The use of keys can often get tricky. Take for example, r.tid =
**	s.a + 3. If 'r' is the inner, there will be a TJOIN node performing
**	the join. The TJOIN node needs to read the tid (s.a + 3) from the
**	orig row buffer. DMF will currently use the user's row buffer
**	to qualify tuples (because some rows are compressed). In this case,
**	the qualification CX struct should also contain code to produce
**	the tid value. For non-TJOIN types of joins, CX structs exist to
**	produce all function values.
**  
**  Note on qualifying tuples:
**	In retrieves where the tid is not involved in the qualification,
**	use the QEN_DMF_ALLOC field in the QEN_BASE for qualifying the tuple.
**	If a tid is used in the qual, QEN_ORIG will move the tid into the
**	this node's row buffer and qualify the tuple. Thus, the ADF_QUAL
**	should use the QEN_ROW field in the QEN_BASE to mark the incoming
**	row (instead of QEN_DMF_ALLOC). 
**
**  Note, if the tid is used for lookup (a key) qen_ukey must be set.
**
** History:
**     19-mar-86 (daved)
**          written
**	18-dec-90 (kwatts)
**	    Smart Disk integration: Added sd_program and sd_pgmlength.
**	04-dec-1992 (kwatts)
**	    Smart Disk development - changed to one orig_sdinfo pointer.
**      19-jun-95 (inkdo01)
**          Collapsed orig_tkey/ukey nat's into single flag word orig_flag
**	7-nov-95 (inkdo01)
**	    Replaced QEN_ADF structure instance by pointer.
**	11-feb-97 (inkdo01)
**	    Changes for MS Access OR transformed in-memory constant tables.
**	11-aug-98 (inkdo01)
**	    Changes for descending sort top sort removal.
**	10-nov-98 (inkdo01)
**	    Added ORIG_IXONLY to indicate we only need to access index pages
**	    and NOT base table data pages.
**	3-nov-99 (inkdo01)
**	    Added orig_qualrows to help group read determination.
**	18-dec-03 (inkdo01)
**	    Added various fields, flags for partitioned table handling.
**	21-jan-04 (inkdo01)
**	    Added orig_qpart, orig_pdim for qualification of partitions
**	    in partitioned table (since subsumed into *orig_part).
**	13-jan-05 (inkdo01)
**	    Added ORIG_FIND_ONLY flag.
[@history_line@]...
*/
struct _QEN_ORIG
{
    QEF_KEY    *orig_pkeys;     /* list of possible keys for this node. set
				** to zero if not used.
				 */
    QEN_ADF	*orig_qual;	/* qualify tuples */
    QEN_PART_INFO *orig_part;	/* partitioning info (if target is
				** partitioned table) */

    i4		orig_keys;	/* control block number of actual keys 
				** dsh_cbs[entry] contains pointer to
				** QEN_TKEY. Set to 0 if not used.
				** if pkeys is set, this value must be valid.
				*/
    i4          orig_get;       /* Index into DSH->DSH_CBS for DMR_CB used
				** to get rows from the base relation
				*/
    i4          orig_tid;       /* set if tid needs to be returned in 
                                ** row buffer. The value here is the offset
				** into the row buffer where the tid should
				** be placed and will be refered to in any
				** qualification. Set to -1 if tid not needed
                                */
    i4          orig_flag;      /* flag word
                                */
#define ORIG_TKEY    0x01	/* TRUE if tids are in the qualification. */
#define ORIG_UKEY    0x02	/* unique key relation - ie we are using
				** unique keys (TIDS or full keys). TRUE-FALSE
				** This should NOT be set if doing a range
				** lookup (even if keys are unique). If this
				** field is set, only one attempt to find
				** a record will be made for each key.
				*/
#define ORIG_MAXOPT  0x04       /* TRUE if optimizeable max being executed
				*/           
#define ORIG_MINOPT  0x08       /* TRUE if optimizeable min being executed
				*/           
#define ORIG_RTREE   0x10	/* TRUE if this is Rtree retrieval
				*/           
#define ORIG_MCNSTTAB 0x20	/* TRUE if this ORIG corresponds to a
				** MS Acess OR transformed in memory constant
				** table (orig_get is then index to table addr)
				*/
#define ORIG_READBACK 0x40	/* TRUE if we're supposed to read backwards
				** through this structure (better be Btree!).
				** It implements topsort-less descending sort. 
				*/
#define ORIG_IXONLY   0x80	/* TRUE if we can get by only reading base table 
				** index pages and NOT data pages. Index covers
				** all cols ref'ed in this table */
#define	ORIG_FIND_ONLY	0x100	/* TRUE if rows of the table don't actually
				** need to be materialized for QEF */
#define	ORIG_MAXMIN	0x200	/* TRUE if max/min may result in aggregate
				** optimization - NOT key columns */
    i4          orig_ordcnt;    /* For READBACK nodes, count of elements in
				** corresponding ORDER BY clause - used to 
				** create correct DMF position calls.
				*/
    i4		orig_qualrows;	/* estimated count of rows qualified by
				** keycolumn predicates, used to predict
				** utility of readahead. -1 if no key
				** selectivity, or if selectivity based on
				** default histograms.
				*/
};

/*}
** Name: QEN_QP - get a row from another query plan
**
** Description:
**      This node makes a recursive call to the query
**  plan executor to retrieve rows from an associated
**  query plan. This is most often used to compute unions,
**  where the associated QP has a list of action items,
**  each a fetch from a QEP.
**
**  Currently, the results of this node (like an orig node)
**  are viewed as a heap relation. That is, the associated
**  QP will not except parameters that differ from those
**  in effect at the time QEF was called. The use of this
**  node should buffer the results if they are to be viewed
**  more than once (ie use a hold file).
**
**  This 'query plan' uses the same DSH as the calling query plan; thus,
**  the QP header (QEF_QP_CB) is largely a place keeper. In order to
**  conserve space, this node will point to a list of actions (QEF_AHD).
**
** History:
**     22-apr-86 (daved)
**          written
**	7-nov-95 (inkdo01)
**	    Replaced QEN_ADF structure instance by pointer.
**	8-mar-04 (inkdo01)
**	    Added flag to indicate QP as owner of || union.
[@history_line@]...
*/
struct _QEN_QP
{
    QEF_AHD		*qp_act;	    /* associated query plan */
    QEN_ADF		*qp_qual;	    /* qualify tuples */
    i4			qp_flag;	    /* flag field */
#define	QP_PARALLEL_UNION 0x01		    /* TRUE if QP owns union
					    ** under exchange node */
};

/*}
** Name: QEN_EXCH - exchange buffers between different threads executing
**	the same query plan
**
** Description:
**	Generate one or more threads to execute plan fragment beneath this
**	node and use exchange buffer concept to pass retrieved row images
**	back to thread executing plan fragment above this node. Additional
**	semantics (union, merge, dups removal, ...) may be applied
**	depending on query requirements. This node implements parallel
**	query processing, both inter-node and intra-node, in Ingres.
**
** History:
**	7-nov-03 (inkdo01)
**          Written.
**	8-feb-04 (toumi01)
**	    Add exch_exchcb.
**	26-feb-04 (inkdo01)
**	    Added bit map to track structures to be allocated for 
**	    exchange children.
**	13-may-04 (toumi01)
**	    Add exch_tot_cnt = total bits in DSH child copy map.
**	19-may-04 (inkdo01)
**	    Add exch_key_cnt for QEN_TKEY structures.
**	21-july-04 (inkdo01)
**	    Change bit map to pair of arrays of DSH ptr array indexes.
[@history_line@]...
*/
struct _QEN_EXCH
{
    QEN_NODE	*exch_out;	/* outer node */
    QEN_ADF	*exch_mat;	/* materialize result row into exch buffer */
    i4		exch_ccount;	/* child thread count */
    i4		exch_flag;	/* flag indicating node specifics */
#define	EXCH_NCHILD	0x01	/* TRUE subtree under exchnage is executed
				** by more than one thread (could mean
				** a higher parent exch is 1:n). Used for 
				** child DSH formatting */
#define	EXCH_UNION	0x02	/* TRUE if subtree is QP node atop a set
				** of union'ed selects */
    i4		exch_dups;	/* DMR_NODUPLICATES if duplicates should 
				** be removed. Else, set to zero.
				*/
    i4		exch_exchcb;	/* index into dsh_cbs of EXCH_CB to hold
				** node context across calls
				*/
    i4		exch_acount;	/* number of attributes in result row */
    DMT_ATTR_ENTRY **exch_atts;	/* attributes in result row */
    i4		exch_macount;	/* number of attributes to merge on */
    DMT_KEY_ENTRY **exch_matts;	/* attributes to merge on */

/* Child threads created from an exchange node require their own DSH
** structures to assure thread safety of || query execution. The count
** values and arrays that follow are designed to allow the minimal number
** of buffers and control structures to be allocated with the DSH for 
** each child thread. The arrays are divided into subarrays containing 
** indexes within the various DSH pointer arrays for the buffers and control 
** blocks to be allocated. The count values show the number of entries in each
** subarray and, when added up, the start of each successive subarray.
** A i4 array is used for the indexes into dsh_cbs because it is anticipated
** that the number of dsh_cbs entries may be quite large for complex queries
** involving partitioned tables.
**
** Finally, there is an array of the count and array information in the 
** event of a parallel union, in which each select in the union has its own
** set of counts and indexes.
**
** The index array scheme replaces an earlier attempt to use a subdivided
** bit map. The bit map used excessive amounts of storage due to its
** sparseness and was dropped in favour of the current arrays.
*/
    struct _EXCH_IX {
	i2	exch_row_cnt;	/* number of array entries for row buffers */
	i2	exch_hsh_cnt;	/* number of array entries for hash structs */
	i2	exch_hsha_cnt;	/* number of array entries for hash agg structs */
	i2	exch_stat_cnt;	/* number of array entries for node statuses */
	i2	exch_ttab_cnt;	/* number of array entries for temp table strs */
	i2	exch_hld_cnt;	/* number of array entries for hold structs */
	i2	exch_shd_cnt;	/* number of array entries for SHD structs */
	i2	exch_cx_cnt;	/* number of array entries for ADE_EXCBs */
	i2	exch_dmr_cnt;	/* number of array entries for DMR_CBs */
	i2	exch_dmt_cnt;	/* number of array entries for DMT_CBs */
	i2	exch_dmh_cnt;	/* number of array entries for DMH_CBs */
	i2	*exch_array1;	/* Array of i2 indexes for row, hash, hash
				** aggregate, node status, temp table, hold
				** and SHD ptr arrays */
	i4	*exch_array2;	/* array of i4 indexes for ADE_EXCB, DMR_CB, 
				** DMT_CB and DMH_CB entries in dsh_cbs array */
    } exch_ixes[1];
};



/*}
** Name: QEF_KEY - describe the key structure
**
** Description:
**      The key structure is used to control which
**  constants are to be used for keying into the base relation.
**  There are two parts to keying a relation. The first is to determine
**  what keys should be used and in what order. The second part is to
**  set the key values into the DMF struct necessary to position the
**  cursor. There are also two different sources of keys. The first is
**  parameter and constant values set by the user. The second is variable
**  values determined by the outer relation in a join. They are
**  described separately below.
**  
**  Constants:
**      1) ADF is used to move parameter values into the key buffer area.
**         ADF will also move constants into the key buffer area. The
**         constant values and parameter values will be converted to key values.
**      2) If there are multiple equality keys, the key list will be sorted.
**         If there are multiple minimum or maximum value keys, the minimum
**         or maximum of the key value will be found.
**      3) The DMF control block will be updated to reflect the location of the
**         values used for keying. This requires that for each element in the key list,
**         the appropriate field in the control block gets set with a pointer.
**      4) The key can now be used.
**  Variable:
**      1) Set the DMF position control block to refer to specific buffers.
**         Also set up a special buffer to handle values whose originating position
**         will always be changing or for values which must be materialized (ie
**         for joins on functions).
**         This operation is done at initialization time.
**      2) ADF is then called to move any values that need moving.
**      3) The key can now be used.
**
**      
**      A key for a node is a moderately complicated structure. A key can
**  be one of four types, equality, minimum, maximum or range.
**  For example: r.a = 65, r.a < 65, r.a > 65, or r.a >65 and r.a < 75.
**  There can also be multiple values for any key (r.a=65 or r.a=75) or
**  (r.a < 65 and r.a < 75). When there are multiple values, we usually want
**  to sort the equality keys (for join nodes) or find the largest or smallest
**  minimum or maximum (to reduce the range of the search).
**
**  The way this information is represented is to have a min and a max list
**  of keys for each node. The max list only exists if there is a maximum or
**  a range lookup. The min list only exists if there is a equality, minimum
**  or range lookup. The list for min or max contains the possible values used
**  for keying.
**
**  The key structure has the following appearance:
**
**	QEF_KEY (top of structure)
**	    |
**	QEF_KOR (first set of keys) - QEF_KAND (first keyed att) - QEF_KAND
**	    |			     (min, max, or eq)
**	    |				|
**	QEF_KOR			      QEF_KATT (attribute desc)
**	    |				|
**	    |			      QEF_KATT
**	QEF_KOR (third set of keys)
**
**  Each QEF_KOR represents a list of keys. In range type lookups there
**  will be only one 'KOR' node. However, in equality lookups there may 
**  be many 'KOR' nodes. For example, r.a = 5 or r.a = 6 will produce
**  two 'KOR' nodes so that QEF can create two ranges to search for tuples.
**
**  Each QEF_KAND node represents a list of keys for an attribute within
**  a range (QEF_KOR) list. This list should NEVER contain multiple
**  equality attributes; however, this list can contain mulitple range keys
**  of the same key type (MIN, MAX). At processing type, QEF will use the
**  largest MIN or the smallest MAX to limit the range. If there is a MIN
**  and a MAX on the same INGRES attribute, there should be two QEF_KATT
**  lists (one for the MIN and one for the MAX).
**
**  RESTRICTIONS:
**	You cannot mix TID and non-TID keys. In pre-jupiter backends, this
**  is not a problem because a key has to be in a boolfact. A boolfact is
**  only good for keying if it has one equiv class. Thus, multiple values
**  on the same attribute works fine; however, multiple values for different
**  attributes do not because they will be in different equiv classes.
**	In future backends it may be possible to have ranges with mulitple
**  attributes or a mixture of equality and ranges using the structure
**  designed above. In this case, do not mix TID and non-TID keys:
**  they are handled differently in DMF.
**  
**
** History:
**     29-jul-86 (daved)
**          re-written
[@history_line@]...
*/
struct _QEF_KEY
{
    QEF_KEY    *key_chain;      /* chain of all key structures */
    i4     key_width;	/* maximum number of QEF_KAND nodes off
				** of one QEF_KOR nodes.
				*/
    QEF_KOR    *key_kor;	/* first range of values */
};

/*}
** Name: QEF_KOR - list of key ranges
**
** Description:
**      Multiple key ranges are created by specifying 'OR' clauses
**  in a qualifier. For example, r.a = 5 or r.a = 6, specifies two
**  ranges for keying. That is, if the table is isam, first set the
**  min and max tids so r.a = 5 is retrieved. When all rows in that range
**  are retrieved, set the range so r.a=6 can be retrieved.
**
** History:
**      29-jul-86 (daved)
**          written
**	20-jul-1993 (pearl)
**          Bug 45912.  Add qef_kor field to QEN_NOR structure to keep track
**          of which QEF_KOR structure it is associated with.
**          Add kor_id field to QEF_KOR structure.
[@history_template@]...
*/
struct _QEF_KOR
{
    QEF_KOR	    *kor_next;		/* next range */
    QEF_KAND	    *kor_keys;		/* keys for this range */
    i4	    kor_numatt;		/* number of keys in this range */
    i4         kor_id;             /* integer id unique to the QEF_QP_CB */
};

/*}
** Name: QEF_NAND - descriptions of keys for an attribute
**
** Description:
**      This structure points to a list of one or more keys for an
**  attribute. If the list is of MIN keys, the MAX of the list will
**  be used for a key. If the list is of MAX keys, the MIN of the list
**  will be used. Do not make lists of equality keys.
**
** History:
**      29-jul-86 (daved)
**          written
[@history_template@]...
*/
struct _QEF_KAND
{
    i4	kand_type;	/* type of list */
#define QEK_EQ  0               /* equality chain of length one */
#define QEK_MIN 1               /* get the min of this list */
#define QEK_MAX 2               /* get the max of this list */
    QEF_KAND	*kand_next;	/* next attribute list */
    i4	kand_attno;	/* attribute number */
    QEF_KATT	*kand_keys;	/* list of keys */
};

/*}
** Name: QEF_KATT - describe a key
**
** Description:
**      This struct gives the operator and value for a key
**
** History:
**      29-jul-86 (daved)
**          written
**	27-mar-89 (jrb)
**	    Added .attr_prec field for decimal project.
**	16-dec-04 (inkdo01)
**	    Added .attr_collID for column level collation project.
[@history_template@]...
*/
struct _QEF_KATT
{
    i4	    attr_attno;		    /* attribute number */
    DB_DT_ID	    attr_type;		    /* its type - use values in DBMS.h*/
    i4	    attr_length;	    /* its length */
    i2		    attr_prec;		    /* its precision and scale */
    i2		    attr_collID;	    /* its collationID */
    i4	    attr_operator;	    /* dmf operator values for 
					    ** dmr_position call.
					    */
    i4	    attr_value;		    /* offset in dsh->dsh_key array
					    ** where value will be found.
					    ** If this attribute is used
					    ** in a join node (as opposed to
					    ** an orig node), this value is
					    ** an offset into the qen_current
					    ** row in the ADE_CX structure
					    ** that produces the join key.
					    */
    QEF_KATT	    *attr_next;		    /* next attribute in key list */
};

/*}
** Name: QEN_TKEY - top level runtime key list
**
** Description:
**      This structure maintains the lists of keys available
**  for orig nodes after parameters have been substituted.
**  The structure that this node points to resembles the QEF_KEY structure
**  with the exception that the 'OR' nodes point to a list of attribute
**  pointers - there are no lists of attributes. The MIN or MAX values
**  for a range have been selected.
**  QEN_TKEY corresponds to QEF_KEY
**  QEN_NOR  corresponds to QEF_KOR
**  QEN_NKEY corresponds to QEF_KAND
**  
**  nothing corresponds to QEF_KATT
**
** History:
**     28-jul-86 (daved)
**          written
[@history_line@]...
*/
struct _QEN_TKEY
{
    QEN_NOR	    *qen_nkey;		/* next key to use for keying
					** This is a list of 'or'd values.
					*/
    QEN_NOR	    *qen_keys;		/* list of qen_nkey structs */
};

/*}
** Name: QEN_NOR - a range of keys 
**
** Description:
**	This structure contains a list of keys in the proper order
**  for keying orig nodes.
**
** History:
**     28-jul-86 (daved)
**          written
**	20-jul-1993 (pearl)
**          Bug 45912.  Add qef_kor field to QEN_NOR structure to keep track
**          of which QEF_KOR structure it is associated with.
**          Add kor_id field to QEF_KOR structure.
[@history_line@]...
*/
struct _QEN_NOR
{
    QEN_NOR	    *qen_next;		/* next key range */
    QEN_NKEY	    *qen_key;		/* the keys for this range */
    QEF_KOR         *qef_kor;           /* corresponding qef_kor structure */
};

/*}
** Name: QEN_NKEY - the key
**
** Description:
**      This structure points to the QEF_KATT struct containing
**  the key value and operator for the attribute.
**
** History:
[@history_template@]...
*/
struct _QEN_NKEY
{
    QEN_NKEY	    *nkey_next;		    /* next attribute being keyed */
    QEF_KATT	    *nkey_att;		    /* attribute being keyed */
    i4	    nkey_type;		    /* type of key. see types
					    ** for QEF_KAND
					    */
};


/*}
** Name: QEN_NODE - The union of all possible nodes
**
** Description:
**	Nodes are the largest elements in a QEP. The node is a data structure
**  that, together with a function, will produce tuples.
**  
**
** History:
**     28-mar-86 (daved)
**          written
**	7-nov-95 (inkdo01)
**	    Replaced QEN_ADF structure instance by pointer and moved nodeqen 
**	    node structure to end to permit minimal memory allocation.
**	26-feb-99 (inkdo01)
**	    Added hash join node descriptor.
**	7-nov-03 (inkdo01)
**	    Added exchange node descriptor.
**	16-dec-03 (inkdo01)
**	    Added qen_high/low for 1:n exchange nodes.
**	2-jan-04 (inkdo01)
**	    Added qen_flags to contain partitioning (and other) directives.
**	17-june-04 (inkdo01)
**	    Added QEN_PQ_RESET flag.
**	27-may-05 (inkdo01)
**	    Added qen_nthreads for possible use in hash join prep (if not
**	    elsewhere).
*/
struct _QEN_NODE
{
    i4                  qen_num;        /* node number. This should be
					** unique for all nodes in the QP
					*/
    i4                  qen_size;       /* size of this node in bytes*/
    i4                  qen_type;       /* type of this node. Use the
					** types defined in QEF_FUNC
					*/
    QEN_NODE           *qen_postfix;    /* next node in postfix list */
    DB_STATUS	       (*qen_func)();	/* function to compute this node 
					** QEF fills in this value. the
					** compiler does not.
					*/
    /* function attribute computation
    ** for join and orig nodes.
    */
    QEN_ADF		*qen_fatts;	/* compute function attributes. set
					** to zero if none.
					*/
    /* initialization info */
    i4			qen_natts;	/* number of attributes */
    DMT_ATT_ENTRY	**qen_atts;     /* an array of pointers to attributes
					** available at this node. 
                                        */
    QEN_ADF		*qen_prow;	/* print current row- for debugging */
    i4                  qen_row;        /* Index into DSH->DSH_ROW to find
					** row associated with this node.
					** this row is used to initialize
					** the DMR_CB for those nodes
					** that have them.
					** 
					*/
    /* NOTE: the following 2 elements could probably be moved to the 
    ** EXCHANGE node header because it is in exchange processing that
    ** they are used. However, they were initially placed here in
    ** anticipation that they might be used with other node headers, too. */
    i4			qen_high;	/* high buffer no allocated with
					** this node. Used by 1:n exch.
					*/
    i4			qen_low;	/* low buffer no allocated in current
					** subtree. Used by 1:n exch.
					*/
    i4			qen_nthreads;	/* no. of threads this node will 
					** execute under - may be of some 
					** use, but also for documentation
					*/
    i4			qen_frow;	/* DSH result row for qen_fatts
					** code (or -1).
					*/
    i4			qen_flags;	/* flag field */
#define	QEN_PART_SEPARATE	0x1	/* node should split partition groups */
#define QEN_PUPDDEL_NODE	0x2	/* TRUE if node fetches rows that are
					** potential targets of pos upd/del */
#define	QEN_PQ_RESET		0x4	/* TRUE if node is positioned such that
					** a RESET may be performed after
					** node initialization - allows early
					** cleanup of exchange threads */

    i4                  qen_rsz;        /* size of row for this node */
    i4		qen_est_tuples;	/* estimated number of tuples for
					** DMF sorter */
    i4		qen_dio_estimate;   /* Logical IO estimate. */
    i4		qen_cpu_estimate;   /* CPU cost estimate */
    /* inkdo01 - moved node_qen from after qen_func to here to permit */
    /* allocation of minimal QEN_NODE - 1/11/95 */
    union
    {
        QEN_ORIG        qen_orig;
	QEN_SJOIN	qen_sjoin;					       
        QEN_TJOIN       qen_tjoin;
        QEN_KJOIN       qen_kjoin;
        QEN_HJOIN       qen_hjoin;
	QEN_SEJOIN	qen_sejoin;
        QEN_SORT        qen_sort;
        QEN_TSORT       qen_tsort;
        QEN_QP          qen_qp;
	QEN_EXCH	qen_exch;
    } node_qen;
};
