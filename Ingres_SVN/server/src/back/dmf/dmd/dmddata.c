/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <pc.h>
#include    <st.h>
#include    <sl.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <tr.h>
#include    <lg.h>
#include    <lk.h>
#include    <me.h>
#include    <adf.h>
#include    <ulf.h>
#include    <dmf.h>
#include    <dm.h>
#include    <dmp.h>
#include    <dm1b.h>
#include    <dm1r.h>
#include    <dm1cx.h>
#include    <dm1c.h>
#include    <dm1p.h>
#include    <dmpp.h>
#include    <dm0p.h>
#include    <dm0l.h>
#include    <dm2f.h>
#include    <dmd.h>
#include    <dm0m.h>

/**
**
**  Name: DMDDATA.C - Debugging routines to print data.
**
**  Description:
**      This file contains all the routines needed to print
**      data from the database tables.  It needs the
**      table control block (TCB) and a pointer to data to 
**      print. 
**
**      The routines defined in this file are:
**          dmd_all	    - Prints an entire BTREE table.
**	      (EXTERNAL)
**          dmd_prdata	    - Prints a data page.
**	      (EXTERNAL)
**          dmd_prindex	    - Prints a BTREE index page. 
**	      (EXTERNAL)
**          dmd_prkey	    - Prints a key.
**	      (EXTERNAL)
**          dmd_prordered   - Prints all pages of BTREE in page order.
**	      (EXTERNAL)
**          dmd_prrecord    - Prints a record from a table.
**	      (EXTERNAL)
**          dmdprbkey       - Prints the key of BTREE index.
**          dmdprbrange     - Prints the range for BTREE index.
**	      (EXTERNAL)
**          dmdprbtree      - Prints the tree part of BTREE table.
**          dmdprentries    - Prints all entries in BTREE index.
**          dmdprleaves     - Prints the leafs of BTREE table.
**          dmdprtid        - Prints the tid of BTREE index record.
**        
**
**  History:
**      07-nov-85 (jennifer)
**          Created for Jupiter.
**	29-may-89 (rogerk)
**	    Check for return status of dm1c_get calls.
**	31-jul-1990 (bryanp)
**	    Print VarChar key columns in dmd_prkey().
**	    Also added support for nullable data types.
**	19-nov-1990 (bryanp)
**	    Fixed typo in return code status checking.
**	8-jan-1991 (bryanp,walt)
**	    Updated debugging support ('char' type, 'date' type added).
**	    Made dmdprbrange() external so it could be called by splits.
**	6-jun-1991 (bryanp)
**	    Size of character buffer 'prefix' in dmdprtid was too small. Fixed.
**	17-oct-1991 (rogerk)
**	    Added file extend support to dmd_prall.
**	19-feb-1992 (bryanp)
**	    Include <st.h> to pick up ST function declarations.
**	7-july-1992 (jnash)
**	    DMF function prototypes.
**	16-sep-1992 (bryanp)
**	    Added support for nullable datatypes in dmd_prrecord().
**	3-oct-1992 (bryanp)
**	    Re-arrange includes to share transaction ID type definition.
**	05-oct-1992 (rogerk)
**	    Reduced Logging Project.  In preparation for support of Partial
**	    TCB's for the new recovery schemes, changed references to
**	    some tcb fields to use the new tcb_table_io structure fields.
**	13-oct-1992 (jnash)
**	    Reduced logging project.  Move compression out of dm1c: separate
**	    tuple uncompress calls and call to dmpp_get.
**	26-oct-92 (rickh)
**	    Replaced references to DMP_ATTS with DB_ATTS.  Also replaced
**	    DM_CMP_LIST with DB_CMP_LIST.
**	30-feb-1993 (rmuth)
**	    The 6.4 btree freelist has been removed hence remove the code
**	    which displays this list.
**	24-may-1993 (bryanp)
**	    Include <pc.h> before <lk.h> to pick up PID definition.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	26-jul-1993 (rogerk)
**	    Include respective CL headers for all cl calls.
**	14-sep-1993 (pearl)
**	    In format_date(), make sure db_data is aligned.
**	06-mar-1996 (stial01 for bryanp)
**	    Don't allocate tuple buffers on the stack.
**	06-may-1996 (thaju02)
**	    New Page Format Project: Change page header references to 
**	    use macros.
**      03-june-1996 (stial01)
**          Use DMPP_INIT_TUPLE_INFO_MACRO to init DMPP_TUPLE_INFO
**          Added DMPP_TUPLE_INFO argument to dm1cxget() 
**      15-jul-1996 (ramra01/bryanp)
**          Call dm1r_cvt_row in dmd_prdata if older row version needs
**          conversion to current version.
**	13-sep-1996 (canor01)
**	    Add rcb_tupbuf to dmpp_uncompress call to pass temporary buffer
**	    for uncompression.
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          Remove unecessary tup_info initialization.
**      10-mar-97 (stial01)
**          Pass relwid to dm0m_tballoc()
**      21-apr-97 (stial01)
**          Print deleted leaf entries
**      12-jun-97 (stial01)
**          Pass tlv to dm1cxget instead of tcb.
**	01-dec-1998 (nanpr01)
**	    Get rid of DMPP_TUPLE_INFO structure. Use DMPP_TUPLE_HDR instead.
**      12-Apr-1999 (stial01)
**          Distinguish leaf/index info for BTREE/RTREE indexes
**          Added page_type parameter to dmd_prkey
**	21-oct-1999 (nanpr01)
**	    More CPU Optimization. Do not copy the tuple header when not needed.
**      12-nov-1999 (stial01)
**          Changes to how btree key info is stored in tcb
**      06-jul-2000 (stial01)
**          Changes to btree leaf/index klen in tcb (b102026)
**      23-oct-2000 (stial01)
**          Variable Page Size - Variable Page Type support (SIR 102955)
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      01-feb-2001 (stial01)
**          Arg changes for page accessors (Variable Page Type SIR 102955)
**	21-Jan-2004 (jenjo02)
**	    "partition number" parm added to dm1cxget(), dm1cxtget(),
**	    display when appropriate.
**      10-feb-2004 (stial01)
**          Support for 256K rows, rows spanning pages.
**      18-feb-2004 (stial01)
**          Fixed incorrect casting of length arguments.
**      25-jun-2004 (stial01)
**          Modularize get processing for compressed, altered, or segmented rows
**	13-Jun-2006 (jenjo02)
**	    Using DM1B_MAXSTACKKEY instead of DM1B_KEYLENGTH, call dm1b_AllocKeyBuf() 
**	    to validate/allocate Leaf work space. DM1B_KEYLENGTH is now only the 
**	    max length of key attributes, not necessarily the length of the Leaf entry.
**      29-sep-2006 (stial01)
**          Support multiple RANGE formats: OLD format:leafkey,NEW format:ixkey
*/


/*
**  Forward function references.
*/

static VOID          dmdprbkey(
			DMP_RCB		*rcb,
			DM1B_INDEX	*buffer,
			i4		i );

static VOID          dmdprbtree(
			DMP_RCB		*rcb,
			i4		subroot,
			i4		indent );

static VOID          dmdprentries(
			DMP_RCB		*rcb,
			DM1B_INDEX	*buffer,
			i4		indent );

static VOID          dmdprleaves(
			DMP_RCB		*rcb,
			i4		subroot );

static VOID          dmdprtid(
			i4		page_type,
			DM_PAGESIZE     page_size,
			DM1B_INDEX	*buffer,
			i4		i );

static VOID	     format_date(
			char		*indate,
			char		*buffer,
			ADF_CB		*adf_cb );


/*
**  Defines of other constants.
*/

/*
**      Constants and macro used for indenting. 
*/

# define    DMD_INDENT  1


/*{
** Name: dmd_**
** Description:
**      This routine prints the entire BTREE table. 
**
** Inputs:
**      rcb                             Pointer to RCB
**                                      which contains such information as
**                                      TCB, lock info, transaction id, etc.
**
** Outputs:
**      none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	07-nov-85 (jennifer)
**          Created for Jupiter.
**	17-oct-91 (rogerk)
**	    Added File Extend support.  Dont attempt to print out BTREE
**	    free list if this is a 6.5 table with FHDR,FMAP stuff instead
**	    of a free list.
**	30-feb-1993 (rmuth)
**	    The 6.4 btree freelist has been removed hence remove the code
**	    which displays this list.
**	10-May-2006 (jenjo02)
**	    Identify CLUSTERED table, give a bit more useful info.
*/
VOID
dmd_prall(
DMP_RCB    *rcb) 
{
    DMP_TCB	     *t = rcb->rcb_tcb_ptr;
    
    /* Probably should flush all pages to disk first. */

    TRdisplay("    B-tree index for %stable %~t,\n\
\t klen %d, ixklen %d, keys %d, atts %d, kperleaf %d, pgsize %d\n",
	(t->tcb_rel.relstat & TCB_CLUSTERED) ? "Clustered " : "",
        sizeof(t->tcb_rel.relid), &t->tcb_rel.relid,
	t->tcb_klen, t->tcb_ixklen, t->tcb_keys, t->tcb_leafattcnt,
	t->tcb_kperleaf, t->tcb_rel.relpgsize);
    dmdprbtree(rcb, 0L, 0L); 
    TRdisplay("    Leaf and overflow pages.\n"); 
    dmdprleaves(rcb, 0L); 

    TRdisplay("\n");
    return;
}

/*{
** Name: dmd_prdata - Prints a data page of a BTREE.
**
** Description:
**      This routine prints a data page of a BTREE.
**
** Inputs:
**      rcb                             Pointer to RCB.
**      data				Pointer to page to print.
**
** Outputs:
**      none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	07-nov-85 (jennifer)
**          Created for Jupiter.
**	 7-nov-88 (rogerk)
**	    Send compression type to dm1c_get() routine.
**	29-may-89 (rogerk)
**	    Check for return status of dm1c_get calls.
**	05-jun-92 (kwatts)
**	    MPF change, use accessor to get tuples.
**	12-oct-92 (jnash)
**	    Reduced logging project.  Move tuple uncompress calls out of
**	    dm1c and into here.  Also fix problem where if dmpp_get
**	    uncompresses a record, it is moved into never-never land.
**	06-mar-1996 (stial01 for bryanp)
**	    Don't allocate tuple buffers on the stack.
**	06-may-1996 (thaju02)
**	    New Page Format Project: Change page header references to 
**	    use macros.
**	20-may-1996 (ramra01)
**	    Added DMPP_TUPLE_INFO argument to the get accessor routine
**      03-june-1996 (stial01)
**          Use DMPP_INIT_TUPLE_INFO_MACRO to init DMPP_TUPLE_INFO
**	15-jul-1996 (ramra01/bryanp)
**          Call dm1r_cvt_row in dmd_prdata if older row version needs
**          conversion to current version.
**	13-sep-1996 (canor01)
**	    Add rcb_tupbuf to dmpp_uncompress call to pass temporary buffer
**	    for uncompression.
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          Remove unecessary tup_info initialization.
**      10-mar-97 (stial01)
**          Pass relwid to dm0m_tballoc()
**	11-aug-97 (nanpr01)
**	    Add the t->tcb_comp_atts_count with relwid.
**	15-jan-99 (nanpr01)
**	    pass ptr to ptr to dm0m_dbdealloc call.
**	13-aug-99 (stephenb)
**	    alter uncompress calls to new proto.
**	14-apr-2004 (gupsh01)
**	    Modified dm1r_cvt_row to include adf control block in the 
**	    parameter list.
*/
VOID
dmd_prdata(
DMP_RCB      *rcb, 
DMPP_PAGE   *data) 
{
    DB_STATUS   	s;
    DM_TID		tid; 
    DMPP_PAGE   	*p; 
    char        	*record;
    char		*rec_buf;
    i4     	record_size;
    i4		error;
    DMP_TCB		*t = rcb->rcb_tcb_ptr;
    i4	        row_version = 0;		
    i4		*row_ver_ptr;
 
    rec_buf = dm0m_tballoc((t->tcb_rel.relwid + t->tcb_comp_atts_count));
    if (rec_buf == 0)
	return;

    p = data; 
    s = TRdisplay("    %d: data page, %s\n", 
	DMPP_VPT_GET_PAGE_PAGE_MACRO(t->tcb_rel.relpgtype, p), 
        (DMPP_VPT_GET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype, p) & DMPP_ASSOC)?
	"associated":"dissociated"); 
    tid.tid_tid.tid_page = DMPP_VPT_GET_PAGE_PAGE_MACRO(t->tcb_rel.relpgtype, p); 
    record_size = t->tcb_rel.relwid;

    if (t->tcb_rel.relversion)
	row_ver_ptr = &row_version;
    else
	row_ver_ptr = NULL;

    for (tid.tid_tid.tid_line = 0; 
         tid.tid_tid.tid_line < 
	     (u_i2) DMPP_VPT_GET_PAGE_NEXT_LINE_MACRO(t->tcb_rel.relpgtype, p); 
         tid.tid_tid.tid_line++)
    {
	s = (*t->tcb_acc_plv->dmpp_get)(t->tcb_rel.relpgtype, 
		    t->tcb_rel.relpgsize, p, &tid, &record_size,
		    &record, row_ver_ptr, NULL, (DMPP_SEG_HDR *)0);
	if (s == E_DB_WARN)
	    continue;

	/* Additional processing if compressed, altered, or segmented */
	if (s == E_DB_OK && 
		(rcb->rcb_compress != TCB_C_NONE || 
		row_version != t->tcb_rel.relversion ||
		DMPP_VPT_PAGE_HAS_SEGMENTS(t->tcb_rel.relpgtype)))
	{
	    s = dm1c_get(rcb, data, &tid, rec_buf, &error);
	    record = rec_buf;
	}

	if (s != E_DB_OK)  
	{
	    /* Bad row on the page - report an error */
	    ule_format(E_DM938A_BAD_DATA_ROW, (CL_SYS_ERR *)0, ULE_LOG,
		NULL, (char *)0, (i4)0, (i4 *)0, &error, 4,
		sizeof(DB_DB_NAME),
		&t->tcb_dcb_ptr->dcb_name,
		sizeof(DB_TAB_NAME), &t->tcb_rel.relid,
		sizeof(DB_OWN_NAME), &t->tcb_rel.relowner,
		0, tid.tid_i4);
	    continue;
	}

	dmd_prrecord(rcb, record); 
    }

    dm0m_tbdealloc(&rec_buf);
}

/*{
** Name: dmd_prindex - Prints an index page of a BTREE.
**
** Description:
**      This routine prints an index page of a BTREE.
**
** Inputs:
**      rcb				Pointer to RCB.
**      buffer                          Pointer to page to print.
**      indent                          Value indicating indentation level.
**
** Outputs:
**      none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	07-nov-85 (jennifer)
**          Created for Jupiter.
**	06-may-1996 (thaju02)
**	    New Page Format Project: Change page header references to use
**	    macros.
*/
VOID
dmd_prindex(
DMP_RCB	    *rcb, 
DM1B_INDEX  *buffer, 
i4      indent) 
{
    DB_STATUS     s;
    DMP_TCB       *t = rcb->rcb_tcb_ptr;
	
    if (DM1B_VPT_GET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype,
	buffer) & DMPP_INDEX)
    {
	s = TRdisplay("%#* <<page:%d>> %s\n", indent * 4,
	    DM1B_VPT_GET_PAGE_PAGE_MACRO(t->tcb_rel.relpgtype,
		buffer),
	    (DM1B_VPT_GET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype,
		buffer) & DMPP_SPRIG) ? "sprig page" : "");
    }
    else if (!(DM1B_VPT_GET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype,
	buffer) & DMPP_DATA))
    {
        s = TRdisplay("%#* Page:%d\t is %s page. \n", indent * 4,
	    DM1B_VPT_GET_PAGE_PAGE_MACRO(t->tcb_rel.relpgtype,
		buffer),
	    (DM1B_VPT_GET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype,
	    buffer) & DMPP_LEAF) ? "A LEAF" : "AN OVFL" );
        s = TRdisplay("%#*  Data page is:%d\t Next page is:%d\t Ovfl page is:%d\n",   
            indent * 4, 
	    DM1B_VPT_GET_BT_DATA_MACRO(t->tcb_rel.relpgtype,
		buffer), 
	    DM1B_VPT_GET_BT_NEXTPAGE_MACRO(t->tcb_rel.relpgtype,
		buffer), 
            DM1B_VPT_GET_PAGE_OVFL_MACRO(t->tcb_rel.relpgtype,
		buffer)); 
    }
    else
        s = TRdisplay("%#* Page:%d\n", indent * 4, 
		DM1B_VPT_GET_PAGE_PAGE_MACRO(t->tcb_rel.relpgtype,
		buffer)); 
    dmdprentries(rcb, buffer, indent); 
}

/*{
** Name: dmd_prkey	- {@comment_text@}
**
** Description:
**	This routine formats the columns in a Btree index entry and prints
**	them out using TRdisplay.
**
**	If the index uses compression, then this routine must be passed the
**	UNCOMPRESSED index entry, not the compressed entry.
**
** Inputs:
**      keys_given                      Number of key values in key structure.
[@PARAM_DESCR@]...
**
** Outputs:
[@PARAM_DESCR@]...
**	Returns:
**	    {@return_description@}
**	Exceptions:
**	    [@description_or_none@]
**
** Side Effects:
**	    [@description_or_none@]
**
** History:
**	07-nov-85 (jennifer)
**          Created for Jupiter.
**	31-jul-1990 (bryanp)
**	    Added support for key columns of type varchar.
**	    Also added support for nullable data types.
**	07-dec-1990 (walt)
**	    Added type ATT_CHA to pick up "char" (type 20) fields.
**	12-dec-1990 (walt)
**	    Added format_date() to call ADF to convert dates to strings.
**	23-jan-1991 (bryanp)
**	    Added argument 'suppress_newline' to control whether or not the
**	    key should have a newline printed at the end. Some callers would
**	    rather not have the newline, since they have their own symbols
**	    they'd like to print after the key on the same line (e.g., 
**	    dmdprbrange wants to enclose the key in '[]' symbols).
**	    Fixed nullable data type support to not print the NULL-value
**	    indicator byte.
**	06-feb-1992 (ananth)
**	    Pass new argument r->rcb_adf_cb' into format_date(). format_date()
**	    doesn't allocate a ADF_CB on the stack anymore.
**      21-jun-1993 (mikem)
**          su4_us5 port.  Added include of me.h to pick up new macro
**          definitions for MEcopy, MEfill, ...
**	25-Apr-2006 (jenjo02)
**	    If INDEX page, use tcb_ixkeys attributes, not tcb_key_atts.
**	20-nov-2006 (wanfr01)
**	    Bug 117160
**	    Print out NULL for null values.
**	20-nov-2006 (wanfr01)
**	    SIR 117586
**	    Enable printing of partial keys via parameter
*/
VOID
dmd_prkey(
DMP_RCB			*rcb,
char			*key,
i4                      page_type,
DB_ATTS			**atts,
i4			suppress_newline,
i4			keys_given)
{
    DMP_TCB		*t = rcb->rcb_tcb_ptr;
    DB_ATTS		**katt;
    DB_ATTS		*att;
    DB_ANYTYPE		anytype;
    i4			i;
    i4			number;
    double		fnumber;
    i4			column_data_type;
    char		string[100];
    i4			null_byte_len = 0;
    char	*attp;

    if (key == NULL)
    {
	TRdisplay("Key is null\n");
	return;
    }

    /* sanity check keys_given */
    if ((keys_given <= 0) || (keys_given > t->tcb_keys)) 
	keys_given = t->tcb_keys;

    if ( (katt = atts) == (DB_ATTS**)NULL )
	katt = (page_type == DM1B_PLEAF) ? t->tcb_leafkeys : t->tcb_ixkeys;

    if (page_type == DM1B_PLEAF)
	TRdisplay("KEY (leaf):<");
    else
	TRdisplay("KEY:(indx)<");

    for (i = 0; i < keys_given; i++)
    {
	att = katt[i];
	column_data_type = att->type;
	null_byte_len = 0;
	if (column_data_type < 0)
	{
	    TRdisplay("[N]");	    /* flag as nullable */
	    column_data_type = - column_data_type;
	    null_byte_len = 1;
	}

	attp=(char *)(key + att->key_offset);
	if (*((char *)attp + att->length - 1) & ADF_NVL_BIT)
	    TRdisplay ("[NULL]");
	else
	switch (column_data_type)
	{
	case ATT_INT:
	    MEcopy((char *)(key + att->key_offset), 
                   att->length - null_byte_len, (char *)&anytype);
	    if (att->length == 1)
		number = anytype.db_i1type;
	    else if (att->length == 2)
		number = anytype.db_i2type;
	    else
		number = anytype.db_i4type;
	    TRdisplay("%12d", number);
	    break;

	case ATT_FLT:
	    MEcopy((char *)(key + att->key_offset), 
                   att->length - null_byte_len, (char *)&anytype);
	    if (att->length == 4)
		fnumber = anytype.db_f4type;
	    else
		fnumber = anytype.db_f8type;
	    TRdisplay("%f", fnumber);
	    break;

	case ATT_CHA:
	case ATT_CHR:
	    TRdisplay("%t", att->length - null_byte_len, key + att->key_offset);
	    break;

	case ATT_TXT:
	case ATT_VCH:
	    MEcopy((char *)(key + att->key_offset),
		   DB_CNTSIZE, (char *)&anytype);
	    TRdisplay("%t%#*.", anytype.db_i2type, key +  att->key_offset + DB_CNTSIZE,
		att->length - DB_CNTSIZE - anytype.db_i2type - null_byte_len);
	    break;

	case ATT_MNY:
	    MEcopy((char *)(key + att->key_offset), 
                   att->length - null_byte_len, (char *)&anytype);
	    TRdisplay("$%f", anytype.db_f8type);
	    break;

	case ATT_DTE:
	    format_date(
		(char *)(key + att->key_offset), 
		string,
		(ADF_CB *)rcb->rcb_adf_cb);

	    TRdisplay("%s", string);
	    break;

	default:
	    TRdisplay("(type %d) 0x", att->type);
	    TRdisplay("%#.1{%2.1x%}", (att->length - null_byte_len),
	    	key + att->key_offset, 0);
	    break;
	}

	if (i < keys_given - 1)
	    TRdisplay("|");
    }
    if (suppress_newline)
	TRdisplay(">");
    else
	TRdisplay(">\n");		
}

/*{
** Name: dmd_prordered - Prints BTREE table in page order.
**
** Description:
**      This routine prints the pages of a BTREE table in page order.
**
** Inputs:
**      rcb				Pointer to RCB.
**
** Outputs:
**      none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	07-nov-85 (jennifer)
**          Created for Jupiter.
**	19-nov-1990 (bryanp)
**	    Fixed typo in return code status checking.
**	06-may-1996 (thaju02)
**	    New Page Format Project: Change page header references to 
**	    use macros.
*/
VOID
dmd_prordered(
DMP_RCB	*rcb)
{
    DM_PAGENO        pageno;
    DMPP_PAGE        *buffer;
    DMPP_PAGE        *page;
    u_i4             pagestat;
    DM_PAGENO        last;
    DB_STATUS	     s;
    i4          err_code;
    DMP_TCB          *t = rcb->rcb_tcb_ptr;
    
    /* Probably should flush all pages to disk first. */

    s = dm2f_sense_file(t->tcb_table_io.tbio_location_array,
                t->tcb_table_io.tbio_loc_count, 
		&t->tcb_rel.relid, 
                &t->tcb_dcb_ptr->dcb_name, 
		(i4 *)&last, &err_code);
    if (s != E_DB_OK)
	TRdisplay("Can't DIsense end of file. DIERR = %d",s);
    for (pageno = 2; pageno <= last; pageno++)
    {
	s = dm0p_fix_page(rcb, pageno, DM0P_RPHYS, &buffer, &err_code); 
        if (s)
	{
	    TRdisplay("Can't read page %d (%x), trying next...\n",
			pageno, s );
	    continue;
	}
	page = buffer;
        pagestat = DMPP_VPT_GET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype, page);
        if (pagestat & DMPP_DATA)
            dmd_prdata(rcb, buffer);
        else if (pagestat & DMPP_FREE)
            TRdisplay("    %d:free page.\n", 
		DMPP_VPT_GET_PAGE_PAGE_MACRO(t->tcb_rel.relpgtype, page));
        else
            dmd_prindex(rcb, (DM1B_INDEX *)page, (i4)0);

	s = dm0p_unfix_page(rcb, DM0P_RELEASE, &buffer, &err_code);
	if (s)
	{
	    TRdisplay("Error (%x) unfixing page %d. Continuing...\n",
			s, pageno );
	}
    }
    return;
}

/*{
** Name: dmd_prrecord	- {@comment_text@}
**
** Description:
**
** Inputs:
**
** Outputs:
**	Returns:
**	    {@return_description@}
**	Exceptions:
**	    [@description_or_none@]
**
** Side Effects:
**	    [@description_or_none@]
**
** History:
**	07-nov-85 (jennifer)
**          Created for Jupiter.
**	16-sep-1992 (bryanp)
**	    Added support for nullable datatypes in dmd_prrecord().
*/
VOID
dmd_prrecord(
DMP_RCB			*rcb,
char			*record)
{
    DMP_TCB             *t = rcb->rcb_tcb_ptr;
    DB_ATTS		*att = t->tcb_atts_ptr;
    DB_ANYTYPE		anytype;
    i4		i;
    i4		number;
    i4		fnumber;
    i4			column_data_type;
    char		string[100];
    i4			null_byte_len = 0;

    TRdisplay("RECORD:<");
    for (i = 1; i <= t->tcb_rel.relatts; i++)
    {
	column_data_type = att[i].type;
	null_byte_len = 0;
	if (column_data_type < 0)
	{
	    TRdisplay("[N]");	    /* flag as nullable */
	    column_data_type = - column_data_type;
	    null_byte_len = 1;
	}

	switch (column_data_type)
	{
	case ATT_INT:
	    MEcopy((char *)(record + att[i].offset), 
                   att[i].length - null_byte_len, (char *)&anytype);
	    if (att[i].length == 1)
		number = anytype.db_i1type;
	    else if (att[i].length == 2)
		number = anytype.db_i2type;
	    else
		number = anytype.db_i4type;
	    TRdisplay("%12d", number);
	    break;

	case ATT_FLT:
	    MEcopy((char *)(record + att[i].offset), 
                   att[i].length - null_byte_len, (char *)&anytype);
	    if (att[i].length == 4)
		fnumber = anytype.db_f4type;
	    else
		fnumber = anytype.db_f8type;
	    TRdisplay("%f", fnumber);
	    break;

	case ATT_CHA:
	case ATT_CHR:
	    TRdisplay("%t", att[i].length - null_byte_len,
			record + att[i].offset);
	    break;

	case ATT_TXT:
	    MEcopy((char *)(record + att[i].length), 
                   DB_CNTSIZE, (char *)&anytype);
	    TRdisplay("%t%#*.", anytype.db_i2type, record +  att[i].offset +
		DB_CNTSIZE,
		att[i].length - DB_CNTSIZE - anytype.db_i2type - null_byte_len);
	    break;

	case ATT_MNY:
	    MEcopy((char *)(record + att[i].offset), 
                   att[i].length - null_byte_len, (char *)&anytype);
	    TRdisplay("$%f", anytype.db_f8type);
	    break;

	case ATT_DTE:
	    format_date(
		(char *)(record + att[i].offset), 
		string,
		(ADF_CB *)rcb->rcb_adf_cb);

	    TRdisplay("%s", string);
	    break;

	default:
	    TRdisplay("(type %d) 0x", att[i].type);
	    TRdisplay("%#.1{%2.1x%}", (att[i].length - null_byte_len),
	    	record + att[i].offset, 0);
	    break;
	}

	if (i < t->tcb_rel.relatts)
	    TRdisplay("|");
    }
    TRdisplay(">\n");		
}

/*{
** Name: dmdprbkey - Prints a key from an index/leaf page of a BTREE.
**
** Description:
**      This routine prints a key from an index page of a BTREE.
**
** Inputs:
**      rcb                             Pointer to RCB.
**      buffer                          Pointer to page to print.
**      i				Value indicating indentation level.
**
** Outputs:
**      none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	07-nov-85 (jennifer)
**          Created for Jupiter.
**	10-jan-1991 (bryanp)
**	    Use the dm1cx() routines to support Btree index compression.
**	    Pass '0' to dmd_prkey; we want the newline printed.
**	06-may-1996 (thaju02 & nanpr01)
**	    New Page Format Project: Change page header references to use
**	    macros.
**      03-june-1996 (stial01)
**          Added DMPP_TUPLE_INFO argument to dm1cxget() 
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          Remove unecessary tup_info initialization.
**      21-apr-97 (stial01)
**          Print deleted leaf entries
**      12-jun-97 (stial01)
**          dmdprbkey() Pass tlv to dm1cxget instead of tcb.
*/
static VOID
dmdprbkey(
DMP_RCB	    *rcb, 
DM1B_INDEX  *buffer, 
i4      i)
{
    DMP_TCB	*t = rcb->rcb_tcb_ptr;
    DM_TID      tid; 
    char	*AllocKbuf;
    char	key_buf[DM1B_MAXSTACKKEY];
    i4		key_len;
    char        *keypos; 
    DB_STATUS	s;
    DB_ATTS	**atts_array;
    i4		atts_count;
    i4		compression_type;
    bool        is_index;
    i4		local_err_code;

    is_index = 
    ((DM1B_VPT_GET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype, buffer) & DMPP_INDEX) != 0);
    if (is_index)
    {
	atts_array = t->tcb_ixatts;
	atts_count = t->tcb_ixattcnt;
	key_len = t->tcb_ixklen;
    }
    else
    {
	atts_array = t->tcb_leafatts;
	atts_count = t->tcb_leafattcnt;
	key_len = t->tcb_klen;
    }
    compression_type = DM1B_INDEX_COMPRESSION(rcb);

    if ( dm1b_AllocKeyBuf(key_len, key_buf, &keypos, &AllocKbuf, &local_err_code) )
    {
	TRdisplay("*** dmbprbkey: Unable to allocate %d bytes of memory for key\n",
			key_len);
	return;
    }

    s = dm1cxget( t->tcb_rel.relpgtype, t->tcb_rel.relpgsize, buffer,
		t->tcb_acc_tlv, atts_array, atts_count, i, &keypos,
		&tid, (i4*)NULL, &key_len, compression_type, NULL );
    if (s == E_DB_OK)
    {
	dmd_prkey(rcb, keypos, is_index ? DM1B_PINDEX : DM1B_PLEAF, (DB_ATTS**)NULL, (i4)0, (i4)0);
    }
    else if (s == E_DB_WARN 
	&& (t->tcb_rel.relpgtype != TCB_PG_V1) )
    {
	    dmd_prkey(rcb, keypos, is_index ? DM1B_PINDEX : DM1B_PLEAF, (DB_ATTS**)NULL, (i4)1, (i4)0);
	    TRdisplay(" DELETED\n"); 
    }
    else
    {
	TRdisplay("***Unable to retrieve entry %d from page %d\n",
		    i, DM1B_VPT_GET_PAGE_PAGE_MACRO(t->tcb_rel.relpgtype, buffer));
    }

    /* Discard any allocated key buffer */
    if ( AllocKbuf )
	dm1b_DeallocKeyBuf(&AllocKbuf, &keypos);

    return;
}

/*{
** Name: dmdprbrange - Prints low and high range of BTREE leaf page.
**
** Description:
**      This routine prints the low and high range keys of a BTREE
**      leaf page.
**
** Inputs:
**      rcb                             Pointer to RCB.
**      buffer                          Pointer to page to print.
**
** Outputs:
**      none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	07-nov-85 (jennifer)
**          Created for Jupiter.
**	10-jan-1991 (bryanp)
**	    Use the dm1cx() routines to support Btree index compression.
**	    Made function non-VOID so that split processing could call it.
**	06-may-1996 (thaju02 & nanpr01)
**	    New Page Format Project: Change page header references to use
**	    macros.
**      03-june-1996 (stial01)
**          Added DMPP_TUPLE_INFO argument to dm1cxget() 
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          Remove unecessary tup_info initialization.
**      06-jun-97 (stial01)
**          dmdprbrange() Pass tlv to dm1cxget instead of tcb.
**	13-Apr-2006 (jenjo02)
**	    Range entry attributes are now properly described by
**	    tcb_ixatts, tcb_ixattcnt, not tcb_leafatts, tcb_leafattcnt.
*/
VOID
dmdprbrange(
DMP_RCB		*rcb, 
DM1B_INDEX	*buffer) 
{
    i4		infinity; 
    DMP_TCB	*t = rcb->rcb_tcb_ptr;
    DM_TID	tid; 
    char	*AllocKbuf, *KeyBuf;
    char	key_buf[DM1B_MAXSTACKKEY];
    i4		key_len;
    char        *keypos; 
    DB_STATUS	s;
    i4		compression_type;
    i4		is_index;
    i4		local_err_code;

    is_index = 
    ((DM1B_VPT_GET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype, buffer) & DMPP_INDEX) != 0);

    compression_type = DM1B_INDEX_COMPRESSION(rcb);

    if ( dm1b_AllocKeyBuf(t->tcb_ixklen, key_buf,
			    &KeyBuf, &AllocKbuf, &local_err_code) )
    {
	TRdisplay("*** dmbprbrange: Unable to allocate %d bytes of memory for key\n",
			t->tcb_ixklen);
	return;
    }


    dm1cxtget(t->tcb_rel.relpgtype, t->tcb_rel.relpgsize, buffer, 
	DM1B_LRANGE, (DM_TID *)&infinity, (i4*)NULL); 
    if (infinity)
    {
	TRdisplay("LRANGE:[infinity]\n"); 
    }
    else 
    {
	keypos = KeyBuf;
	key_len = t->tcb_ixklen;
	s = dm1cxget( t->tcb_rel.relpgtype, t->tcb_rel.relpgsize, buffer,
			t->tcb_acc_tlv, 
			t->tcb_rngatts, t->tcb_rngattcnt, DM1B_LRANGE,
			&keypos, &tid, (i4*)NULL, &key_len,
			compression_type, NULL );
	if (s != E_DB_OK)
	{
	    TRdisplay("***Unable to retrieve entry %d from page %d\n",
			DM1B_LRANGE, 
			DM1B_VPT_GET_PAGE_PAGE_MACRO(t->tcb_rel.relpgtype, buffer));
	}
	else
	{
	    TRdisplay("LRANGE:["); 
	    dmd_prkey(rcb, keypos, is_index ? DM1B_PINDEX : DM1B_PLEAF, t->tcb_ixatts, (i4)1, (i4)0);
	    TRdisplay("]\n"); 
	}
    }

    dm1cxtget(t->tcb_rel.relpgtype, t->tcb_rel.relpgsize, buffer, 
		DM1B_RRANGE, (DM_TID *)&infinity, (i4*)NULL); 
    if (infinity)
    {
	TRdisplay("RRANGE:[infinity]\n"); 
    }
    else 
    {
	keypos = KeyBuf;
	key_len = t->tcb_ixklen;
	s = dm1cxget( t->tcb_rel.relpgtype, t->tcb_rel.relpgsize, buffer,
			t->tcb_acc_tlv, 
			t->tcb_rngatts, t->tcb_rngattcnt, DM1B_RRANGE,
			&keypos, &tid, (i4*)NULL, &key_len,
			compression_type, NULL );
	if (s != E_DB_OK)
	{
	    TRdisplay("***Unable to retrieve entry %d from page %d\n",
			DM1B_RRANGE, 
			DM1B_VPT_GET_PAGE_PAGE_MACRO(t->tcb_rel.relpgtype, 
			    buffer));
	}
	else
	{
	    TRdisplay("RRANGE:["); 
	    dmd_prkey(rcb, keypos, is_index ? DM1B_PINDEX : DM1B_PLEAF, t->tcb_ixatts, (i4)1, (i4)0);
	    TRdisplay("]\n"); 
	}
    }

    /* Discard any allocated key buffer */
    if ( AllocKbuf )
	dm1b_DeallocKeyBuf(&AllocKbuf, &KeyBuf);
    return;
}

/*{
** Name: dmdprbtree - Recursive preorder traversal of BTREE for printing.
**
** Description:
**      This routine prints index pages  of a BTREE by a preordered
**      traversal of the index.
**
** Inputs:
**      rcb                             Pointer to RCB.
**      subroot                         Pointer to page to start traversal.
**      indent                          Value indicating indentation level.
**
** Outputs:
**      none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	07-nov-85 (jennifer)
**          Created for Jupiter.
**	10-jan-1991 (bryanp)
**	    Call the dm1cx() routines to support Btree index compression
**	06-may-1996 (thaju02)
**	    New Page Format Project: Change page header references to use
**	    macros.
*/
static VOID
dmdprbtree(
DMP_RCB       *rcb, 
i4      subroot, 
i4      indent) 
{
    i4      i; 
    DMPP_PAGE    *buffer; 
    DM1B_INDEX   *bufpage; 
    DM_TID      tid; 
    DB_STATUS    s;
    i4      err_code;
    DMP_TCB	*t = rcb->rcb_tcb_ptr;
    
    s = dm0p_fix_page(rcb, (DM_PAGENO  )subroot, DM0P_RPHYS, &buffer, 
			    &err_code);
    bufpage = (DM1B_INDEX *)buffer; 
    if ( !(DM1B_VPT_GET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype,
				    bufpage) & DMPP_INDEX) )
    {
	s = dm0p_unfix_page(rcb, DM0P_RELEASE, &buffer, &err_code);
        return; 
    }
    if (subroot == 0)
        dmd_prindex(rcb, bufpage, (i4)0); 
    else 
        dmd_prindex(rcb, bufpage, indent); 
    for (i=0; i<DM1B_VPT_GET_BT_KIDS_MACRO(t->tcb_rel.relpgtype,bufpage); i++)
    {
        dm1cxtget(t->tcb_rel.relpgtype, t->tcb_rel.relpgsize, bufpage, i, 
			&tid, (i4*)NULL); 
	s = dm0p_unfix_page(rcb, DM0P_RELEASE, &buffer, &err_code);
        dmdprbtree(rcb, (i4)tid.tid_tid.tid_page, indent+DMD_INDENT); 
	s = dm0p_fix_page(rcb, (DM_PAGENO  )subroot, DM0P_RPHYS, 
                            &buffer,
			    &err_code);
        bufpage = (DM1B_INDEX *)buffer; 
    }
    s = dm0p_unfix_page(rcb, DM0P_RELEASE, &buffer, &err_code);
}

/*{
** Name: dmdprentries - Prints entries of an index page of a BTREE.
**
** Description:
**      This routine prints the entries of the index page of a BTREE.
**
** Inputs:
**      rcb                             Pointer to RCB.
**      buffer                          Pointer to page to print.
**      indent                          Value indicating indentation level.
**
** Outputs:
**      none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	07-nov-85 (jennifer)
**          Created for Jupiter.
**	06-may_1996 (thaju02)
**	    New Page Format Project: Change page header references to use
**	    macros.
*/
static VOID
dmdprentries(
DMP_RCB     *rcb, 
DM1B_INDEX  *buffer, 
i4      indent) 
{
    i4      i; 
    DB_STATUS    s;
    DMP_TCB     *t = rcb->rcb_tcb_ptr;

    s = TRdisplay("    %d:children.\n", 
	DM1B_VPT_GET_BT_KIDS_MACRO(t->tcb_rel.relpgtype, buffer)); 
    if (DM1B_VPT_GET_BT_KIDS_MACRO(t->tcb_rel.relpgtype,buffer) == 0)
    {
        if (DM1B_VPT_GET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype,
	    buffer) & DMPP_LEAF || 
	    DM1B_VPT_GET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype,
	    buffer) & DMPP_CHAIN)
            dmdprbrange(rcb, buffer); 
        return; 
    }
    if (DM1B_VPT_GET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype,
	buffer) & DMPP_INDEX)
    {
        dmdprtid(t->tcb_rel.relpgtype, t->tcb_rel.relpgsize, buffer, (i4)0); 
	TRdisplay("\n");
        for (i=0; i<DM1B_VPT_GET_BT_KIDS_MACRO(t->tcb_rel.relpgtype,
	    buffer)-1; i++)
        {
            dmdprtid(t->tcb_rel.relpgtype, t->tcb_rel.relpgsize, buffer, i+1); 
            dmdprbkey(rcb, buffer, i); 
        }
    }
    else if (DM1B_VPT_GET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype,
	buffer) & DMPP_LEAF || 
	DM1B_VPT_GET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype,
	buffer) & DMPP_CHAIN)
    {
        if (DM1B_VPT_GET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype,
	    buffer) & DMPP_LEAF || 
	    DM1B_VPT_GET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype,
	    buffer) & DMPP_CHAIN)
            dmdprbrange(rcb, buffer); 
        for (i=0; i<DM1B_VPT_GET_BT_KIDS_MACRO(t->tcb_rel.relpgtype,
	    buffer); i++)
        {
            dmdprtid(t->tcb_rel.relpgtype, t->tcb_rel.relpgsize, buffer, i); 
            dmdprbkey(rcb, buffer, i); 
        }
    }
}

/*{
** Name: dmdprleaves - Prints contents of BTREE leaf page.
**
** Description:
**      This routine prints the contents of a BTREE leaf page.
**
** Inputs:
**      rcb                             Pointer to RCB.
**      subroot				Pointer to page to start.
**
** Outputs:
**      none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	07-nov-85 (jennifer)
**          Created for Jupiter.
**	10-jan-1991 (bryanp)
**	    Call the dm1cx() routines to support Btree index compression.
**	14-dec-1991 (rogerk)
**	    Removed updating of page to set the DMPP_CHAIN bit for overflow
**	    pages (presumably in case they were for some reason not already
**	    set).  It seems bad to fix a page in read mode and then update it.
**	06-may-1996 (thaju02)
**	    New Page Format Project: Change page header references to macros.
*/
static VOID
dmdprleaves(
DMP_RCB	    *rcb, 
i4     subroot) 
{
    i4      i; 
    DMPP_PAGE    *buffer; 
    DM_TID      tid; 
    DM_PAGENO    next; 
    DM1B_INDEX   *bufpage; 
    DB_STATUS    s;
    i4      err_code;
    DMP_TCB      *t = rcb->rcb_tcb_ptr;
    
    s = dm0p_fix_page(rcb, (DM_PAGENO  )subroot, DM0P_RPHYS, &buffer,
			&err_code); 
    bufpage = (DM1B_INDEX *)buffer; 
    if (DM1B_VPT_GET_PAGE_STAT_MACRO(t->tcb_rel.relpgtype, 
	bufpage) & DMPP_LEAF)
    {
        do 
        {
            dmd_prindex(rcb, bufpage, (i4)0); 
            next = DM1B_VPT_GET_PAGE_OVFL_MACRO(t->tcb_rel.relpgtype, bufpage);
            if (next == NULL)
                break; 
	    s = dm0p_unfix_page(rcb, DM0P_RELEASE, &buffer, &err_code);
	    s = dm0p_fix_page(rcb, next, DM0P_RPHYS, &buffer,
                               &err_code);
            bufpage = (DM1B_INDEX *)buffer; 
        }
        while (1); 
	s = dm0p_unfix_page(rcb, DM0P_RELEASE, &buffer, &err_code);
        return; 
    }
    for (i = 0 ; i < 
	DM1B_VPT_GET_BT_KIDS_MACRO(t->tcb_rel.relpgtype, bufpage); 
	i++)
    {
        dm1cxtget(t->tcb_rel.relpgtype, t->tcb_rel.relpgsize, bufpage, i, 
			&tid, (i4*)NULL); 
	s = dm0p_unfix_page(rcb, DM0P_RELEASE, &buffer, &err_code);
        dmdprleaves(rcb, (i4)tid.tid_tid.tid_page); 
	s = dm0p_fix_page(rcb, (DM_PAGENO  )subroot, DM0P_RPHYS,
			    &buffer, &err_code);
        bufpage = (DM1B_INDEX *)buffer; 
    }
    s = dm0p_unfix_page(rcb, DM0P_RELEASE, &buffer, &err_code);
}

/*{
** Name: dmdprtid - Prints a TID from an index page of a BTREE.
**
** Description:
**      This routine prints the i'th TID of a page of a BTREE.
**
** Inputs:
**      buffer                          Pointer to page to print.
**      i				Value indicating line table entry
**                                      for TID to print.
**
** Outputs:
**      none
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	07-nov-85 (jennifer)
**          Created for Jupiter.
**	10-jan-1991 (bryanp)
**	    Call the dm1cx() routines to support Btree index compression.
**	    Number the tids for easier readability when many tids are printed.
**	6-jun-1991 (bryanp)
**	    Size of character buffer 'prefix' in dmdprtid was too small. Fixed.
**	06-may-1996 (nanpr01)
**	    Added page_size parameter for the New Page Format project.
*/
static VOID
dmdprtid(
i4	    page_type,
DM_PAGESIZE page_size,
DM1B_INDEX   *buffer, 
i4      i) 
{
    DM_TID     tid; 
    i4		partno;
    char	prefix [10];
    
    dm1cxtget(page_type, page_size, buffer, i, &tid, &partno); 

    if ((i % 5) == 0)
	STprintf(prefix, "%4d ", i);
    else
	STcopy("     ", prefix);

    TRdisplay("%.5sTID:{%d,%d} ", prefix,
                tid.tid_tid.tid_page, tid.tid_tid.tid_line); 

    if ( partno )
	TRdisplay(" Partno:%d ", partno);
}

/*
** Name: format_date		- format a date column for display
**
** Description:
**	This routine calls ADF to format a date column for display.
**
** Inputs:
**	indate			- pointer to the date to be formatted
**
** Outputs:
**	buffer			- filled in with the formatted date
**
** Returns:
**	VOID
**
** History:
**	10-jan-1991 (walt,bryanp)
**	    Added as part of the Btree index compression project.
**	06-feb-1992 (ananth)
**	    Added new argument 'adf_cb'. The ADF_CB isn't allocated on the
**	    stack anymore.
**	14-sep-1993 (pearl)
**	    In format_date(), make sure db_data is aligned.
**	05-oct-2006 (gupsh01)
**	    DB_DTE_TYPE is now renamed as ingresdate.
*/
static VOID
format_date(
char		*indate,
char		*buffer,
ADF_CB		*adf_cb)
{
	DB_DATA_VALUE	date, string;
	STATUS		status;
	DB_DATE         date_buf;

	date.db_datatype = DB_DTE_TYPE;
	MEcopy (indate, DB_DTE_LEN, date_buf.db_date);
	date.db_data     = date_buf.db_date;

	date.db_length   = DB_DTE_LEN;
	date.db_prec	 = 0;

	string.db_datatype = DB_CHA_TYPE;
	string.db_length   = 25;
	string.db_prec	   = 0;
	string.db_data	   = buffer;

	status = adc_cvinto(adf_cb, &date, &string);

	if (status != E_DB_OK)
	    STcopy("ingresdate", buffer);
	
	buffer[25] = EOS;

	return;
}
