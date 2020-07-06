/*
** Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>

#include    <gcccl.h>
#include    <cv.h>		 
#include    <cvgcc.h>
#include    <me.h>
#include    <qu.h>
#include    <tm.h>
#include    <tr.h>

#include    <iicommon.h>
#include    <gca.h>
#include    <gcocomp.h>
#include    <gcoint.h>
#include    <gcc.h>
#include    <gccer.h>
#include    <gccpl.h>
#include    <gcxdebug.h>

/*
**
**  Name: GCOPDC.C - GCF Comm Server Presentation Data Conversion module
**
**  External entry points:
**	gco_convert		Convert messages using compiled descriptor
**	gco_do_16src		16 bit character set conversion.
**
**  History:
**      12-Feb-90 (seiwald)
**	    Collected from gccpl.c and gccutil.c.
**	16-Feb-90 (seiwald)
**	    Rebuilt upon CV conversion macros.
**	    Tuned somewhat.
**	    Modified gcc_pl_lod (load object descriptor) to compress
**	    runs of the same single element into an array.  In certain
**	    cases, this speeds up large data retrieves significantly.
**	15-Mar-90 (seiwald)
**	    Reworked gcc_adf_init to allocate properly sized od_ptr
**	    vectors for each triad_od.  Removed silly [1] subscript
**	    for the od array pointer in the GCA_TRIAD_OD.
**	15-Mar-90 (seiwald)
**	    Pass upper range bytes (128-255) through directly.
**	    This allows Kanji to indentify itself as ASCII (yuk).
**      23-Mar-90 (seiwald)
**          Byte align patch for gcc_pl_lod.
**	27-Mar-91 (seiwald)
**	    Included all necessary CL headers as per PC group.
**	20-May-91 (seiwald)
**	    Changes for character set negotiation: removed gcc_load_av,
**	    gcc_chk_av, and cc_ctbl_init, which have been replaced by
**	    newer routines in gcccset.c.  Made perform_conversion and
**	    its underlings refer to the new per-connection character 
**	    set transliteration table.
**	17-aug-91 (leighb) DeskTop Porting Changes:
**	    added include of cv.h;
**	    corrected func decls & defs for gcc_save_int(), gcc_ld_od(), 
**	    and gcc_adf_init();
**	17-Jun-91 (seiwald)
**	    A little non-functional regrooving:
**	    -	Removed complex_type() and triad_od() and stuffed their
**	    	contents into gcc_ld_od().  
**	    -	Removed gcc_save_int() and stuffed its contents into 
**	    	perform_conversion().
**	    -   Removed queue of DOC_EL's and just put an array of them
**	        into a new DOC_STACK; extracted elements not specific to
**		each DOC_EL into the DOC_STACK.
**	    -	Spruced up tracing to be more compact.
**  	13-Aug-91 (cmorris)
**  	    Added include of tm.h.
**	14-Aug-91 (seiwald) bug #39172
**	    Gcc_ld_od returns a failure status for 0 length char types,
**	    as well as other bogus TPL's.  Gcc_pl_lod can now return a
**	    failure status.
**	14-Aug-91 (seiwald)
**	    Linted.
**	26-Dec-91 (seiwald)
**	    Gcc_conv_vec now zeros the PAD on inbound flow.
**	14-Aug-92 (seiwald)
**	    Support for compiled descriptors: message descriptors, tuple 
**	    descriptors, and GCA_DATA_VALUES are now compiled into simple 
**	    programs interpreted by perform_conversion().
**	14-Aug-92 (seiwald)
**	   Support for blobs, by way of a new array status "GCA_VARSEGAR" 
**	   to GCA_ELEMENT.  This array status indicates that the element 
**	   may be repeated indefinitely, as controlled by the previous 
**	   element.
**	20-Aug-92 (seiwald)
**	   New interface to adf decompose functions, adc_gcc_decompose,
**	   which simple returns a list of GCA_ELEMENT's that describe
**	   all the datatypes.  OP_ERROR now takes an argument for more
**	   descriptive tracing.
**	10-Sep-92 (seiwald)
**	   ANSI fixes: cast signed quantity to unsigned before comparing
**	   against unsigned (they are both semantically unsigned); rework
**	   tabs using a macro, rather than trying to add 8 to a static
**	   pointer; removed suspected trigraph.
**	26-sep-92 (seiwald)
**	    Moved fragment handling from perform_conversion to gcc_pdc_down.
**	28-Oct-92 (seiwald)
**	    Recoded DO_16SRC after assuming that mappings don't exist
**	    between single and double byte characters; fixed vector count
**	    adjustment in DO_16SRC to represent the actual number of source
**	    bytes used.
**	4-Dec-92 (seiwald) bug #47311
**	    Allow GC_QTXT_TYPE of 0 length for STAR generated queries.
**	16-Dec-92 (seiwald)
**	    GCA_VARSEGAR (blob types) tested and fixed.
**	5-Feb-93 (seiwald)
**	    Moved allocation of GCC_DOC up into caller of perform_conversion().
**	    Removed CCB's and MDE's from perform_conversion's interface: it
**	    just deals with buffers now.
**  	10-Feb-93 (cmorris)
**  	    Removed status from GCnadds and GCngets:- never used.
**	17-Apr-93 (seiwald)
**	    Another crack at doublebyte support: sometimes character data 
**	    coming across the wire is garbage.  It is possible for the last
**	    byte of a character field to look like the first byte of a 
**	    doublebyte character, and DO_16SRC has now been modified to
**	    limit itself to a single byte conversion if the vector count
**	    indicates only one byte remaining.  Embarrassingly enough, it
**	    is the GCA_OBJECT_DESC, generated by GCA_SEND, that contains a
**	    garbage character field - the unfilled data_object_name.
**	25-May-93 (seiwald)
**	    Moved non-conversion code out to gcccomp.c.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	25-aug-93 (ed)
**	    remove dbdbms.h
**	21-Sep-93 (brucek)
**	    Cast args to MECOPY_CONST_MACRO to PTRs.
**	11-Jan-94 (mikehan)
**	    For doublebyte support: - perform_conversion() - assign the
**	    value of doc->sp[-1] to i as well before do DO_16SRC when
**	    it is OUTFLOW. For one more time, this will fix the 
**	    'No Connection' problem if there is charset conversion involved,
**	    notably that of Kanjieuc<-> Shiftjis.
**	1-Mar-94 (seiwald)
**	    Added dummy handling of new OP_VAR_BEGIN and OP_VAR_END
**	    op codes, which aren't needed by perform_conversion.
**	14-Mar-94 (seiwald)
**	    Extracted from gccpdc.c.
**	24-mar-95 (peeje01)
**	    Integrate doublebyte changes from 6500db_su4_us42
**	29-Mar-94 (seiwald)
**	    Added support for VARIMPAR in message encoding.
**	4-Apr-94 (seiwald)
**	    Spruce up tracing with OP_DEBUG_TYPE and OP_DEBUG_BUFS.
**	4-Apr-94 (seiwald)
**	    New OP_DV_BEGIN/END.
**	4-Apr-94 (seiwald)
**	    OP_MSG_BEGIN now includes the message type for tracing.
**	04-May-94 (cmorris)
**	    Bump level at which plethora of trace output appears to 3.
**	 1-May-95 (gordy)
**	    Added alignment type and length to OP_VAR_BEGIN arguments.
**	25-May-95 (gordy)
**	    Add entries for OP_BREAK and OP_MSG_RESTART which are ignored.
**	31-Aug-95 (gordy)
**	    Fix bad varchar lengths for null varchar's in OP_PUSH_VCH.
**	29-Nov-95 (gordy)
**	    Adjust previous fix.  Some 6.4 versions have bad lengths
**	    but still treat the data area as padding.  Replicating
**	    this behavior seems to work for everyone.
**	 1-Feb-96 (gordy)
**	    Added support for compressed VARCHARs.
**	 1-Mar-96 (gordy)
**	    General clean-up to reconcile gcd_convert(), gcd_encode()
**	    and gcd_decode().
**	 3-Apr-96 (gordy)
**	    Removed OP_JNOVR and added OP_CHKLEN, OP_DV_ADJUST.  Added
**	    casts to tracing.
**	21-Jun-96 (gordy)
**	    Added OP_COPY_MSG for messages which are not formatted into 
**	    elements.  The entire message is copied from input to output.
**	 5-Feb-97 (gordy)
**	    Added OP_CALL_DDT for pre-compiled DBMS datatype processing.
**	    OP_PUSH_VCH is now passed both the max and actual data lengths
**	    on the data stack.  OP_COPY_MSG now expects the element size
**	    as an argument.  Removed OP_CHKLEN, OP_COMP_PAD.
**      31-Mar-98 (gordy)
**          Include long varchar in doublebyte special string handling.
**	03-apr-1998 (canor01)
**	    The padding information needs to be skipped on two other
**	    conditions--INFLOW and truncation.
**	26-apr-1999 (hanch04)
**	    Added casts to quite compiler warnings
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      21-June-2001 (wansh01) 
**          Changed gcx_level to gcd_trace_level to seperate the gcc and gcd trace
**	22-Mar-02 (gordy)
**	    More nat to i4 cleanup.  Removed unneeded atomic types.
**	12-Dec-02 (gordy)
**	    Renamed gcd component to gco.
**	12-May-03 (gordy)
**	    Made sure current processing state is returned in all instances.
**	    VEC_MACRO now returns an indication that limit was exceeded.
**	    DOC_DONE (complete) now distinct from DOC_IDLE (initialize).
**	15-Mar-04 (gordy)
**	    Added support for eight-byte integers.
**	24-Sep-04 (gordy)
**	    Fixing a problem with character translation caused by changing
**	    u_char * types to char *.
**      18-Nov-2004 (Gordon.Thorpe@ca.com and Ralph.Loen@ca.com)
**          Renamed gcx_gcc_ops to gcx_gco_ops, as this referenced outside
**          of GCC.
**	 4-Jan-5 (gordy)
**	    Use unsigned char's in gco_do_16sr csince they used as 
**	    indices in the translation arrays.
**	30-Nov-05 (gordy)
**	    Fixed global declarations.
*/

# define VEC_MACRO( vec, limit, sz ) \
	((((limit)/(sz)) < (vec)) ? (((vec) = (limit)/(sz)),TRUE) : FALSE)

GLOBALREF GCXLIST gcx_gco_ops[];
GLOBALREF GCXLIST gcx_gcc_eop[];


/*
** Name: gco_convert - convert messages using compiled descriptor
**
** Description:
**	Translates all or part of a GCA message from local to network
**	or network to local transfer syntax, using simple programs
**	produced by compiling message descriptors.
**
**	Returns successfully if input data depleted or output data area
**	full, expecting to be called again.  May return with input data
**	still present if input data contains only a fragment of an
**	atomic datatype.
**
** Input:
**	doc		GCO_DOC (state info for conversion)
**	    state		DOC_IDLE on first call
**	    src1		start of input data
**	    src2		end of input data 
**	    dst1		start of output buffer
**	    dst2		end of output buffer
**	    message_type	GCA message type
**	    eod 		this is the last segment of a message
**	    tod_prog 		program for GCA_TUPLE data
**	inout		INFLOW = convert to local; OUTFLOW = convert to network
**	char_tbl	character set transliteration table
**
** Output:
**	doc->src1		unused part of input buffer
**	doc->src2		unused part of output buffer
**	doc->state		DOC_MOREIN - more input required
**				DOC_MOREOUT - more output required
**				DOC_DONE - conversion complete
**
** Returns:
**	STATUS		OK or error code.
**
** History
**	?-?-88 (berrow)
**	    Written.
**	24-may-89 (seiwald)
**	    Revamped and made sane.
**	06-jun-89 (seiwald)
**	    Don't consider abrupt end of message fatal.  Just convert
**	    what data there is and leave the error control to the clients.
**	08-jun-89 (seiwald)
**	    When checking for abrupt end of message, check for fragment.
**	14-Aug-92 (seiwald)
**	    Rewritten to use compiled descriptors.
**  	11-Dec-92 (cmorris)
**  	    Renamed the confusingly named "mop2" argument to "mopend".
**  	    This is consistent with the naming of the p... variables in
**  	    an MDE:- p2 refers to the end of the _data_ whereas "pend" 
**  	    refers to the end of the available buffer for data. The latter
**  	    is what is implied by this argument.
**	5-Feb-93 (seiwald)
**	    Interface rid of references to MDE's or CCB's.
**	 1-May-95 (gordy)
**	    Added alignment type and length to OP_VAR_BEGIN arguments.
**	31-Aug-95 (gordy)
**	    Fix bad varchar lengths for null varchar's in OP_PUSH_VCH.
**	29-Nov-95 (gordy)
**	    Adjust previous fix.  Some 6.4 versions have bad lengths
**	    but still treat the data area as padding.  Replicating
**	    this behavior seems to work for everyone.
**	 1-Feb-96 (gordy)
**	    Added support for compressed VARCHARs.
**	 1-Mar-96 (gordy)
**	    General clean-up to reconcile gcd_convert(), gcd_encode()
**	    and gcd_decode().
**	 3-Apr-96 (gordy)
**	    Removed OP_JNOVR and added OP_CHKLEN, OP_DV_ADJUST.  Added
**	    casts to tracing.
**	21-Jun-96 (gordy)
**	    Added OP_COPY_MSG for messages which are not formatted into 
**	    elements.  The entire message is copied from input to output.
**	 5-Feb-97 (gordy)
**	    Added OP_CALL_DDT for pre-compiled DBMS datatype processing.
**	    OP_PUSH_VCH is now passed both the max and actual data lengths
**	    on the data stack.  OP_COPY_MSG now expects the element size
**	    as an argument.  Removed OP_CHKLEN, OP_COMP_PAD.
**      31-Mar-98 (gordy)
**          Include long varchar in doublebyte special string handling.
**	 1-Mar-01 (gordy)
**	    OP_PUSH_VCH now takes argument which is the size of the array
**	    elements (to support Unicode).  New OP_DIVIDE operation.
**	12-May-03 (gordy)
**	    Made sure current processing state is returned in all instances.
**	    DOC_DONE (complete) now distinct from DOC_IDLE (initialize).
**	    May be called in DOC_DONE state (will return with no processing
**	    since already complete) but DOC_IDLE must be used for new message.
**	    OP_JNINP now returns DOC_MOREIN for case where input exhausted 
**	    but message continued (processing resumes with OP_JNINP).
**	15-Mar-04 (gordy)
**	    Added support for eight-byte integers.
**	24-Sep-04 (gordy)
**	    A previous change made many of the binary pointers to be
**	    char * to match GCO_DOC definitions rather than u_char *.
**	    This causes problems when using the pointer to do character
**	    translation using the character as an index.  Signed chars
**	    will produce an invalid index when characters above 0x7F
**	    are translated.  All binary pointers should actually be
**	    of type u_i1 *, but for now just casting the x2y[] indexes.
**	08-Mar-07 (gordy, gupsh01)
**	    Fixed the case OP_PUSH_LIT, to correctly obtain the values.
*/

STATUS
gco_convert( GCO_DOC *doc, i4  inout, PTR char_tbl 
# ifdef DOUBLEBYTE
	     , i4  *split
# endif /* ifdef DOUBLEBYTE */
)
{
    i4		i;			/* for counting converted items */
    GCO_OP	arg;			/* arguments in the program */
    GCA_ELEMENT	ela;			/* to hold GCA_DATA_VALUES */
    STATUS	status;			/* for conversion status */
    i4		t_i4;			/* for PUSH_I4 */
    u_i2	t_ui2;			/* for PUSH_I2 */
    char	*s;			/* for PUSH_* */
    u_char	*x2y;			/* transliteration table */
    u_i2	*x2y16;			/* 16 bit trans table */
    char	*src = doc->src1;
    char	*dst = doc->dst1;
    i4		src_limit = doc->src2 - src;
    i4		dst_limit = doc->dst2 - dst;

# ifdef GCXDEBUG
    static GCXLIST gcx_doc_state[] =
    {
	{ DOC_IDLE,	"IDLE" },
	{ DOC_DONE,	"DONE" },
	{ DOC_MOREIN,	"INPUT" },
	{ DOC_MOREOUT,	"OUTPUT" },
	{ 0, NULL },
    };

    static GCXLIST gcx_gco_flow[] =
    {
	{ INFLOW,	"IN" },
	{ OUTFLOW,	"OUT" },
	{ 0, NULL }
    };

    if ( gco_trace_level >= 2 )
	TRdisplay( "GCO convert: state = %s, flow = %s, msg = %s\n",
		    gcx_look( gcx_doc_state, doc->state ), 
		    gcx_look( gcx_gco_flow, inout ), 
		    gcx_look( gcx_gca_msg, doc->message_type ) );
# endif /* GCXDEBUG */

    /* 
    ** If fresh message, load program according to message type 
    */
    if ( doc->state == DOC_IDLE )
	gco_init_doc( doc, doc->message_type );
    else
	doc->state = DOC_IDLE;	/* clear prior state */

    /* 
    ** Step through instructions.
    */

top:

# ifdef GCXDEBUG
    if ( *doc->pc != OP_DEBUG_TYPE  &&  *doc->pc != OP_DEBUG_BUFS )
	if ( gco_trace_level >= 3 )
	{
	    i4 *i;

	    TRdisplay( "%s-- %23s\t[", tabs( doc->depth ), 
			gcx_look( gcx_gco_ops, (i4)*doc->pc ) );

	    for( i = doc->val_stack; i < doc->sp; i++ )  
		TRdisplay( "%d ", *i );

	    TRdisplay( "]\n" );
	}
	else  if ( gco_trace_level >= 2 )
	{
	    TRdisplay( "%s-- %s\n", tabs( doc->depth ), 
			gcx_look( gcx_gco_ops, (i4)*doc->pc ) );
	}
# endif /* GCXDEBUG */

    switch( *doc->pc++ )
    {
    case OP_ERROR:		/* error has occured */
	arg = *doc->pc++;

# ifdef GCXDEBUG
	if ( gco_trace_level >= 1 )
	    TRdisplay( "GCO Error: %s\n", gcx_look( gcx_gcc_eop, (i4)arg ) );
# endif /* GCXDEBUG */

	doc->src1 = src;
	doc->dst1 = dst;
	doc->pc -= 2;
	return( E_GC2410_PERF_CONV );

    case OP_DONE:		/* complete msg converted */
# ifdef GCXDEBUG
	if ( gco_trace_level >= 2 )  
	    TRdisplay( "GCO convert: processing complete!\n" );
# endif /* GCXDEBUG */

	doc->src1 = src;
	doc->dst1 = dst;
	doc->pc--;
	doc->state = DOC_DONE;
	return( OK );

    case OP_DOWN: 		/* decend into complex type */
	doc->depth++;
	break;

    case OP_UP: 		/* finish complex type */
	doc->depth--;
	break;

    case OP_DEBUG_TYPE:		/* trace the current type */
	TRdisplay( "%s== (%s, %d, %d)\n", tabs( doc->depth ),
		   gcx_look( gcx_datatype, (i4)doc->pc[0] ),
		   (i4)doc->pc[1], (i4)doc->pc[2] );
	doc->pc += 3;
	break;

    case OP_DEBUG_BUFS:		/* trace buffer info */
	TRdisplay( "%s++ ilen %d olen %d\n", 
		   tabs( doc->depth ), src_limit, dst_limit );
	break;

    case OP_JZ:			/* pop top-of-stack; branch if zero */
	arg = *doc->pc++;
	if ( ! *(--doc->sp) )  doc->pc += arg;
	break;

    case OP_DJNZ:		/* if !top-of-stack pop, else decr & branch */
	arg = *doc->pc++;
	if ( doc->sp[-1]-- )
	    doc->pc += arg;
	else
	    --doc->sp;
	break;

    case OP_JNINP:		/* branch if input depleted */
	arg = *doc->pc++;		/* branch offset */

	if ( ! src_limit )		/* input buffer empty */
	    if ( doc->eod )		/* message end-of-data */
		doc->pc += arg;		/* branch */
	    else
	    {
		/*
		** The input buffer is empty but the end-of-data
		** flag is not set: request more input.  There
		** may be no additional data available, just
		** the end-of-data flag on an empty message
		** segment, so reset the PC to redo the test.
		*/
# ifdef GCXDEBUG
		if ( gco_trace_level >= 2 )
		    TRdisplay( "GCO convert: state = %s\n",
				gcx_look( gcx_doc_state, DOC_MOREIN ) );
# endif /* GCXDEBUG */

		doc->state = DOC_MOREIN;
		doc->src1 = src;
		doc->dst1 = dst;
		doc->pc -= 2;
		return( OK );
	    }
	break;

    case OP_GOTO: 		/* branch unconditionally */
	arg = *doc->pc++;
	doc->pc += arg;
	break;

    case OP_CALL_DDT:		/* call DBMS datatype program */
	arg = *doc->pc++;
	*doc->csp++ = doc->pc;
	doc->depth++;
	doc->pc = (GCO_OP *)gco_ddt_map[ arg ];
	break;

    case OP_CALL_DV: 		/* call data value program */
	ela.tpl.length = *(--doc->sp);
	ela.tpl.precision = *(--doc->sp);
	ela.tpl.type = *(--doc->sp);
	gco_comp_dv( &ela, doc->od_dv_prog );
	*doc->csp++ = doc->pc;
	doc->depth++;
	doc->pc = doc->od_dv_prog;
	break;

    case OP_CALL_TUP:		/* call tuple program */
	*doc->csp++ = doc->pc;
	doc->depth++;
	doc->pc = doc->tod_prog ? (GCO_OP *)doc->tod_prog : gco_tuple_prog;
# ifdef DOUBLEBYTE
	doc->tuple_flag = 1;
# endif /* ifdef DOUBLEBYTE */
	break;

    case OP_RETURN:		/* return from program call */
	doc->pc = *(--doc->csp);
	doc->depth--;
# ifdef DOUBLEBYTE
	doc->tuple_flag = 0;
# endif /* ifdef DOUBLEBYTE */
	break;

    case OP_PUSH_LIT:		/* push 2 byte signed quanity */
	*doc->sp++ = (u_i2) *doc->pc++;
	break;

    case OP_PUSH_I4:		/* push i4 from data stream */
# ifdef DOUBLEBYTE
	if ( doc->data_len_flag == 1 )
	    doc->data_len_ptr = dst - sizeof(t_i4);
# endif /* ifdef DOUBLEBYTE */

	s = inout == INFLOW ? dst : src;
	MECOPY_CONST_MACRO( s - sizeof(t_i4), sizeof(t_i4), (PTR)&t_i4 );
	*doc->sp++ = t_i4;
	break;

    case OP_PUSH_I2:		/* push i2 from data stream */
# ifdef DOUBLEBYTE
	if ( doc->data_len_flag == 1 )
	{
	    doc->data_len_ptr = dst - sizeof(t_ui2);
	    doc->data_len_flag = 3;
	}
# endif /* ifdef DOUBLEBYTE */

	s = inout == INFLOW ? dst : src;
	MECOPY_CONST_MACRO( s - sizeof(t_ui2), sizeof(t_ui2), (PTR)&t_ui2 );
	*doc->sp++ = t_ui2;
	break;

    case OP_PUSH_VCH:		/* compute varchar padding length */
	/*
	** Adjust lengths for processing data and padding.
	** The array element size is provided as argument.
	** The stack contains the number of elements from
	** the embedded length field just processed and
	** the maximum length of the object (includes the
	** embedded length field which is removed).
	*/
	arg = *doc->pc++;				/* Size of elements */
	t_ui2 = *(--doc->sp);			/* Number of elements */
	t_i4 = *(--doc->sp) - sizeof( t_ui2 );	/* Max data length (bytes) */

	/*
	** Length can be bogus on nullable variable length types.
	** Adjust if greater than max data length of varchar.  Also
	** fix the output stream just to be a little bit safer.
	*/
	if ( (t_ui2 * arg) > (u_i2)t_i4 )
	{
	    t_ui2 = 0;

	    if ( inout == INFLOW )
	    {
		MECOPY_CONST_MACRO( (PTR)&t_ui2, 
				    sizeof(t_ui2), dst - sizeof(t_ui2) );
	    }
	    else
	    {
		CV2N_I2_MACRO( (PTR)&t_ui2, dst - sizeof( t_ui2 ), &status );
	    }
	}

	*doc->sp++ = t_i4 - (t_ui2 * arg);	/* length of pad (bytes) */
	*doc->sp++ = t_ui2;		/* Number of elements */
	break;

    case OP_MSG_BEGIN:		/* note beginning of message */
    	/* 
	** Beginning of a message: we do nothing.
	**
	** arg1 = message number for tracing
	** arg2 = size of fixed part of message structure 
	*/
	arg = *(doc->pc)++;		/* message number - for tracing */

# ifdef GCXDEBUG
	if ( gco_trace_level >= 3 )
	    TRdisplay( "%s++ message %s %d bytes\n", 
		       tabs( doc->depth ), gcx_look( gcx_gca_msg, (i4)arg ), 
		       (i4)*doc->pc );
# endif /* GCXDEBUG */

	doc->pc++;
	break;

    case OP_COPY_MSG:		/* Copy remaining input to output */
	/*
	** Copy as much of the message to the output 
	** buffer as possible.  Since we are not at
	** end-of-input, make sure we ask for at least
	** one element so that the conversion routines
	** will get more input if needed.
	*/
	arg = *(doc->pc)++;	/* Size of array element */
	*doc->sp++ = max( 1, (doc->src2 - src) / arg );
	break;

    case OP_BREAK:		/* end of data segment */
	break;

    case OP_IMP_BEGIN:		/* note beginning of GCA_VARIMPAR */
	/* 
	** Beginning of a VARIMPAR array: we do nothing.
	**
	** arg1 = size of substructure 
	*/
	arg = *(doc->pc++);

# ifdef GCXDEBUG
	if ( gco_trace_level >= 3 )
	    TRdisplay( "%s++ impar %d bytes\n", tabs( doc->depth ), (i4)arg );
# endif /* GCXDEBUG */

	break;

    case OP_MARK:		/* save current processing state */
	/*
	** Conversion does not overflow.
	*/
	break;

    case OP_REWIND:		/* backup to saved processing state */
	/*
	** No saved processing state to restore.
	*/
# ifdef GCXDEBUG
	if ( gco_trace_level >= 1 )
	    TRdisplay( "GCO internal error: REWIND with no MARK\n" );
# endif /* GCXDEBUG */

	doc->src1 = src;
	doc->dst1 = dst;
	doc->pc--;
	return( E_GC2410_PERF_CONV );

    case OP_VAR_BEGIN:		/* note beginning of GCA_VAREXPAR */
	/* 
	** Beginning of VAREXPAR array: we do nothing
	**
	** Note: the number of array elements is on the top of the
	** stack.  It is updated/popped by a OP_DJNZ operation.
	**
	** arg1 = size of substructure
	** arg2 = alignment type
	** arg3 = alignment length
	*/

# ifdef GCXDEBUG
	if ( gco_trace_level >= 3 )
	    TRdisplay( "%s++ expar %d els * %d bytes\n", 
		       tabs( doc->depth ), doc->sp[-1], (i4)doc->pc[0] );
# endif /* GCXDEBUG */

	doc->pc += 3;
	break;

    case OP_VAR_END:		/* note end of GCA_VAREXPAR */
	break;

    case OP_DV_BEGIN:		/* note beginning of GCA_DATA_VALUE value */
	/* 
	** Beginning of GCA_DATA_VALUE value: we do nothing.
	**
	** arg1 = length
	*/

# ifdef GCXDEBUG
	if ( gco_trace_level >= 3 )
	    TRdisplay( "%s++ data value %d bytes\n", 
		       tabs( doc->depth ), (i4)*doc->pc );
# endif /* GCXDEBUG */

	doc->pc++;
	break;

    case OP_DV_END:		/* note end of GCA_DATA_VALUE value */
	break;

    case OP_DV_ADJUST:		/* Adjust buffer space for GCA_DATA_VALUE */
	break;

    case OP_PAD_ALIGN:		/* alignment pad for structures */
	doc->pc++;
	break;

    case OP_DIVIDE:		/* Divide stack by argument */
	arg = *doc->pc++;				/* Divisor */
	t_i4 = *(--doc->sp);			/* Dividend */
	*doc->sp++ = (arg < 2) ? t_i4 : (t_i4 / arg);
	break;

    /*
    ** Start of conversion operations.
    **
    ** Array conversions are limited to the available
    ** space in our input and output buffers.  Remaining
    ** elements are converted after requesting more
    ** input or output space.
    */

    case OP_CV_I1:
	if ( inout == INFLOW )
	{
	    i = doc->sp[-1];
	    if (VEC_MACRO(i, dst_limit, CV_L_I1_SZ)) doc->state = DOC_MOREOUT;
	    if (VEC_MACRO(i, src_limit, CV_N_I1_SZ)) doc->state = DOC_MOREIN;
	    doc->sp[-1] -= i;

	    while( i-- )
	    {
		CV2L_I1_MACRO( src, dst, &status );
		src += CV_N_I1_SZ;
		dst += CV_L_I1_SZ;
	    }
	}
	else
	{
	    i = doc->sp[-1];
	    if (VEC_MACRO(i, dst_limit, CV_N_I1_SZ)) doc->state = DOC_MOREOUT;
	    if (VEC_MACRO(i, src_limit, CV_L_I1_SZ)) doc->state = DOC_MOREIN;
	    doc->sp[-1] -= i;

	    while( i-- )
	    {
		CV2N_I1_MACRO( src, dst, &status );
		src += CV_L_I1_SZ;
		dst += CV_N_I1_SZ;
	    }
	}
	goto converted;

    case OP_CV_I2:
# ifdef DOUBLEBYTE
	/*
	** If this is the I2 of a varchar,
	** set up pointer to the dest I2.
	*/
	if ( doc->type_flag )  doc->i2_ptr = dst;
# endif /* ifdef DOUBLEBYTE */
	if ( inout == INFLOW )
	{
	    i = doc->sp[-1];
	    if (VEC_MACRO(i, dst_limit, CV_L_I2_SZ)) doc->state = DOC_MOREOUT;
	    if (VEC_MACRO(i, src_limit, CV_N_I2_SZ)) doc->state = DOC_MOREIN;
	    doc->sp[-1] -= i;

	    while( i-- )
	    {
		CV2L_I2_MACRO( src, dst, &status );
		src += CV_N_I2_SZ;
		dst += CV_L_I2_SZ;
	    }
	}
	else
	{
	    i = doc->sp[-1];
	    if (VEC_MACRO(i, dst_limit, CV_N_I2_SZ)) doc->state = DOC_MOREOUT;
	    if (VEC_MACRO(i, src_limit, CV_L_I2_SZ)) doc->state = DOC_MOREIN;
	    doc->sp[-1] -= i;

	    while( i-- )
	    {
		CV2N_I2_MACRO( src, dst, &status );
		src += CV_L_I2_SZ;
		dst += CV_N_I2_SZ;
	    }
	}
	goto converted;

    case OP_CV_I4:
	if ( inout == INFLOW )
	{
	    i = doc->sp[-1];
	    if (VEC_MACRO(i, dst_limit, CV_L_I4_SZ)) doc->state = DOC_MOREOUT;
	    if (VEC_MACRO(i, src_limit, CV_N_I4_SZ)) doc->state = DOC_MOREIN;
	    doc->sp[-1] -= i;

	    while( i-- )
	    {
		CV2L_I4_MACRO( src, dst, &status );
		src += CV_N_I4_SZ;
		dst += CV_L_I4_SZ;
	    }
	}
	else
	{
	    i = doc->sp[-1];
	    if (VEC_MACRO(i, dst_limit, CV_N_I4_SZ)) doc->state = DOC_MOREOUT;
	    if (VEC_MACRO(i, src_limit, CV_L_I4_SZ)) doc->state = DOC_MOREIN;
	    doc->sp[-1] -= i;

	    while( i-- )
	    {
		CV2N_I4_MACRO( src, dst, &status );
		src += CV_L_I4_SZ;
		dst += CV_N_I4_SZ;
	    }
	}
	goto converted;

    case OP_CV_I8:
	if ( inout == INFLOW )
	{
	    i = doc->sp[-1];
	    if (VEC_MACRO(i, dst_limit, CV_L_I8_SZ)) doc->state = DOC_MOREOUT;
	    if (VEC_MACRO(i, src_limit, CV_N_I8_SZ)) doc->state = DOC_MOREIN;
	    doc->sp[-1] -= i;

	    while( i-- )
	    {
		CV2L_I8_MACRO( src, dst, &status );
		src += CV_N_I8_SZ;
		dst += CV_L_I8_SZ;
	    }
	}
	else
	{
	    i = doc->sp[-1];
	    if (VEC_MACRO(i, dst_limit, CV_N_I8_SZ)) doc->state = DOC_MOREOUT;
	    if (VEC_MACRO(i, src_limit, CV_L_I8_SZ)) doc->state = DOC_MOREIN;
	    doc->sp[-1] -= i;

	    while( i-- )
	    {
		CV2N_I8_MACRO( src, dst, &status );
		src += CV_L_I8_SZ;
		dst += CV_N_I8_SZ;
	    }
	}
	goto converted;

    case OP_CV_F4:
	if ( inout == INFLOW )
	{
	    i = doc->sp[-1];
	    if (VEC_MACRO(i, dst_limit, CV_L_F4_SZ)) doc->state = DOC_MOREOUT;
	    if (VEC_MACRO(i, src_limit, CV_N_F4_SZ)) doc->state = DOC_MOREIN;
	    doc->sp[-1] -= i;

	    while( i-- )
	    {
		CV2L_F4_MACRO( src, dst, &status );
		src += CV_N_F4_SZ;
		dst += CV_L_F4_SZ;
	    }
	}
	else
	{
	    i = doc->sp[-1];
	    if (VEC_MACRO(i, dst_limit, CV_N_F4_SZ)) doc->state = DOC_MOREOUT;
	    if (VEC_MACRO(i, src_limit, CV_L_F4_SZ)) doc->state = DOC_MOREIN;
	    doc->sp[-1] -= i;

	    while( i-- )
	    {
		CV2N_F4_MACRO( src, dst, &status );
		src += CV_L_F4_SZ;
		dst += CV_N_F4_SZ;
	    }
	}
	goto converted;

    case OP_CV_F8:
	if ( inout == INFLOW )
	{
	    i = doc->sp[-1];
	    if (VEC_MACRO(i, dst_limit, CV_L_F8_SZ)) doc->state = DOC_MOREOUT;
	    if (VEC_MACRO(i, src_limit, CV_N_F8_SZ)) doc->state = DOC_MOREIN;
	    doc->sp[-1] -= i;

	    while( i-- )
	    {
		CV2L_F8_MACRO( src, dst, &status );
		src += CV_N_F8_SZ;
		dst += CV_L_F8_SZ;
	    }
	}
	else
	{
	    i = doc->sp[-1];
	    if (VEC_MACRO(i, dst_limit, CV_N_F8_SZ)) doc->state = DOC_MOREOUT;
	    if (VEC_MACRO(i, src_limit, CV_L_F8_SZ)) doc->state = DOC_MOREIN;
	    doc->sp[-1] -= i;

	    while( i-- )
	    {
		CV2N_F8_MACRO( src, dst, &status );
		src += CV_L_F8_SZ;
		dst += CV_N_F8_SZ;
	    }
	}
	goto converted;

    case OP_CV_BYTE:
	i = doc->sp[-1];
	if ( VEC_MACRO( i, dst_limit, 1 ) )  doc->state = DOC_MOREOUT;
	if ( VEC_MACRO( i, src_limit, 1 ) )  doc->state = DOC_MOREIN;
	doc->sp[-1] -= i;

	MEcopy( (PTR)src, i, (PTR)dst );
	src += i;
	dst += i;
	goto converted;

    case OP_CV_CHAR:
	if ( inout == INFLOW )
	{
# ifdef DOUBLEBYTE
	    if ( x2y16 = ((GCC_CSET_XLT *)char_tbl)->n2l16 )
	    {
		i4 vec;
		i1 frag = DOC_IDLE;

		/*
		** Several different factors control the amount of
		** processing performed.  Their precedence and
		** related processing status is as follows:
		**
		** vec		When 0, current input object completed
		** max_len	When 0 (check_len > 0), output object completed
		** frag		If not DOC_IDLE then DOC_MOREIN/DOC_MOREOUT
		** dst_limit	If 0 then DOC_MOREOUT
		** src_limit	If 0 then DOC_MOREIN
		*/
		for( 
		     vec = doc->sp[-1];
		     vec  &&  (! doc->check_len  ||  doc->max_len > 0)  &&
		     frag == DOC_IDLE  &&  dst_limit  &&  src_limit;
		   )
		    gco_do_16src( x2y16, (u_char **)&src, &src_limit, 
				  (u_char **)&dst, &dst_limit, &vec, 
				  &doc->max_len, doc->check_len, 
				  &doc->len_adjust, &frag );

		doc->sp[-1] = vec;

		switch (doc->type_flag)
		{
		case DB_CHR_TYPE: /* c */
		case -DB_CHR_TYPE: /* nullable c */
		case DB_CHA_TYPE: /* char */
		case -DB_CHA_TYPE: /* nullable char */
		    if ( vec == 0 )
		    {
			/* the entire char was converted */
			if ( doc->data_len_flag > 0 )
			{
			    /* GCA_VAREXPAR variable length, adjust length */
			    MECOPY_CONST_MACRO( doc->data_len_ptr,
						sizeof(t_i4), &t_i4 );
			    t_i4 -= doc->len_adjust;
			    doc->len_adjust = 0;
			    MECOPY_CONST_MACRO( &t_i4, sizeof(t_i4),
						doc->data_len_ptr);
			}
			else  if ( doc->len_adjust < 0 )    /* got longer */
			    doc->len_adjust = 0;
			else 				    /* got shorter */
			{
			    /* Fixed length string, adjust blanks */
			    for( ; doc->len_adjust  &&  dst_limit;
				 doc->len_adjust--, dst_limit-- )
				*(dst++) = 0x20;	/* blank fill */
			}

			if ( doc->len_adjust )
			    doc->state = DOC_MOREOUT;
			else
			{
			    *split = 0;
			    doc->type_flag = 0;
			    doc->len_adjust = 0;
			    doc->max_len = 0;
			    doc->data_len_flag = 0;
			}
		    }
		    else  if ( doc->check_len  &&  doc->max_len <= 0 )
		    {
			/* Reached output max_len, discard rest of source */
			i = doc->sp[-1];
			if ( VEC_MACRO( i, src_limit, CV_N_CHAR_SZ ) ) 
			    doc->state = DOC_MOREIN;
			doc->sp[-1] -= i;

			src += i * CV_N_CHAR_SZ;

			*split = 0;
			doc->type_flag = 0;
			doc->len_adjust = 0;
			doc->max_len = 0;
			doc->data_len_flag = 0;
		    }
		    else
		    {
			/* not finished with this element */
			*split = 1;

			if ( frag != DOC_IDLE )
			    doc->state = frag;
			if ( ! dst_limit )
			    doc->state = DOC_MOREOUT;
			else  if ( ! src_limit )
			    doc->state = DOC_MOREIN;
		    }
		    break;

		case DB_VCH_TYPE: /* varchar */
		case -DB_VCH_TYPE: /* nullable varchar */
		case DB_TXT_TYPE: /* text */
		case -DB_TXT_TYPE: /* nullable text */
		case DB_LVCH_TYPE: /* long varchar */
		case -DB_LVCH_TYPE: /* nullable long varchar */
		    if ( vec == 0 )
		    {
			i4 len_delta = 0;

			/* adjust actual data -- len field */
			MECOPY_CONST_MACRO(doc->i2_ptr, sizeof(t_ui2), &t_ui2);
			t_ui2 -= doc->len_adjust;
			len_delta -= doc->len_adjust;
			MECOPY_CONST_MACRO(&t_ui2, sizeof(t_ui2), doc->i2_ptr);

			/* adjust pad only if not long varchar */
			if ( doc->type_flag != DB_LVCH_TYPE  &&
			     doc->type_flag != -DB_LVCH_TYPE )
			{
			    /* adjust pad -- length of pad is in doc->sp[-2] */
			    doc->sp[-2] += doc->len_adjust;
			    len_delta += doc->len_adjust;

			    if ( doc->sp[-2] < 0 )  
			    {
				/* padding eliminated and data increased */
				len_delta -= doc->sp[-2];
				doc->sp[-2] = 0;
			    }
			}

			/* adjust gca_data_value len */
			if ( doc->data_len_flag > 0  &&  len_delta )
			{
			    MECOPY_CONST_MACRO( doc->data_len_ptr,
						sizeof(t_i4), &t_i4);
			    t_i4 += len_delta;
			    MECOPY_CONST_MACRO(&t_i4, sizeof(t_i4),
					doc->data_len_ptr);
			}

			*split = 0;
			doc->type_flag = 0;
			doc->len_adjust = 0;
			doc->max_len = 0;
			doc->data_len_flag = 0;
		    }
		    else  if ( doc->check_len  &&  doc->max_len <= 0 )
		    {
			/* have reached max_len, discard rest of src */
			i = doc->sp[-1];
			if ( VEC_MACRO( i, src_limit, CV_N_CHAR_SZ ) ) 
			    doc->state = DOC_MOREIN;
			doc->sp[-1] -= i;

			src += i * CV_N_CHAR_SZ;

			/* adjust actual data -- len field */
			MECOPY_CONST_MACRO(doc->i2_ptr, sizeof(t_ui2), &t_ui2);
			t_ui2 -= doc->len_adjust;
			if ( (i4)t_ui2 > doc->check_len )
			    t_ui2 = doc->check_len;
			MECOPY_CONST_MACRO(&t_ui2, sizeof(t_ui2), doc->i2_ptr);

			/* adjust pad only if not long varchar */
			if ( doc->type_flag != DB_LVCH_TYPE  &&
			     doc->type_flag != -DB_LVCH_TYPE )
			{
			    /* adjust pad, length of pad is doc->sp[-2] */
			    doc->sp[-2] += doc->len_adjust;
			    if ( doc->sp[-2] < 0 || t_ui2 == doc->check_len )
				doc->sp[-2] = 0;
			}

			*split = 0;
			doc->type_flag = 0;
			doc->len_adjust = 0;
			doc->max_len = 0;
			doc->data_len_flag = 0;
		    }
		    else
		    {
			/* not finished with this element */
			*split = 1;

			if ( frag != DOC_IDLE )
			    doc->state = frag;
			else  if ( ! dst_limit )
			    doc->state = DOC_MOREOUT;
			else  if ( ! src_limit )
			    doc->state = DOC_MOREIN;
		    }
		    break;

		case DB_QTXT_TYPE: /* query text */
		    if ( vec == 0 )
		    {
			/* adjust gca_data_value len */
			if ( doc->data_len_flag > 0 )
			{
			    MECOPY_CONST_MACRO( doc->data_len_ptr,
						sizeof(t_i4), &t_i4 );
			    t_i4 -= doc->len_adjust;
			    MECOPY_CONST_MACRO( &t_i4, sizeof(t_i4),
						doc->data_len_ptr );
			}

			*split = 0;
			doc->type_flag = 0;
			doc->len_adjust = 0;
			doc->max_len = 0;
			doc->data_len_flag = 0;
		    }
		    else  if ( doc->check_len  &&  doc->max_len <= 0 )
		    {
			/* have reached max_len, discard rest of src */
			i = doc->sp[-1];
			if ( VEC_MACRO( i, src_limit, CV_N_CHAR_SZ ) ) 
			    doc->state = DOC_MOREIN;
			doc->sp[-1] -= i;

			src += i * CV_N_CHAR_SZ;

			*split = 0;
			doc->type_flag = 0;
			doc->len_adjust = 0;
			doc->max_len = 0;
			doc->data_len_flag = 0;
		    }
		    else
		    {
			/* not finished with this element */
			*split = 1;

			if ( frag != DOC_IDLE )
			    doc->state = frag;
			else  if ( ! dst_limit )
			    doc->state = DOC_MOREOUT;
			else  if ( ! src_limit )
			    doc->state = DOC_MOREIN;
		    }
		    break;

		default:
		    /* should be impossible to get here */
		    doc->type_flag = 0;
		    break;
		} /* switch (doc->type_flag) */
	    }
	    else
# endif /* DOUBLEBYTE */
	    {
		i = doc->sp[-1];
		if ( VEC_MACRO( i, dst_limit, CV_L_CHAR_SZ ) ) 
		    doc->state = DOC_MOREOUT;
		if ( VEC_MACRO( i, src_limit, CV_N_CHAR_SZ ) ) 
		    doc->state = DOC_MOREIN;
		doc->sp[-1] -= i;
		x2y = (u_char *)((GCC_CSET_XLT *)char_tbl)->n2l;

		while( i-- )
		{
		    *dst = x2y[ *(u_i1 *)src ];
		    src += CV_N_CHAR_SZ;
		    dst += CV_L_CHAR_SZ;
		}
	    }
	}
	else
	{
# ifdef DOUBLEBYTE
	    if ( x2y16 = ((GCC_CSET_XLT *)char_tbl)->l2n16 )
	    {
		i4 vec;
		i1 frag = DOC_IDLE;

		/*
		** Several different factors control the amount of
		** processing performed.  Their precedence and
		** related processing status is as follows:
		**
		** vec		When 0, current input object completed
		** max_len	When 0 (check_len > 0), output object completed
		** frag		If not DOC_IDLE then DOC_MOREIN/DOC_MOREOUT
		** dst_limit	If 0, DOC_MOREOUT
		** src_limit	If 0, DOC_MOREIN
		*/
		for( 
		     vec = doc->sp[-1]; 
		     vec  &&  (! doc->check_len  ||  doc->max_len > 0)  &&
		     frag == DOC_IDLE  &&  dst_limit  &&  src_limit; 
		   )
		    gco_do_16src( x2y16, (u_char **)&src, &src_limit, 
				  (u_char **)&dst, &dst_limit, &vec, 
				  &doc->max_len, doc->check_len,
				  &doc->len_adjust, &frag );

		doc->sp[-1] = vec;

		switch(doc->type_flag)
		{
		case DB_CHR_TYPE:
		case -DB_CHR_TYPE:
		case DB_CHA_TYPE:
		case -DB_CHA_TYPE:
		    if ( vec == 0 )
		    {
			/* the entire tuple was converted */
			if (doc->data_len_flag > 0)
			{
			    /* GCA_VAREXPAR variable length, adjust length */
			    CV2L_I4_MACRO( doc->data_len_ptr, &t_i4, &status );
			    t_i4 -= doc->len_adjust;
			    doc->len_adjust = 0;
			    CV2N_I4_MACRO( &t_i4, doc->data_len_ptr, &status );
			}
			else  if ( doc->len_adjust < 0 )    /* got longer */
			    doc->len_adjust = 0;
			else				    /* got shorter */
			{
			    /* fixed length string, adjust blanks */
			    for ( ; doc->len_adjust  &&  dst_limit;
				  doc->len_adjust--, dst_limit-- )
				*(dst++) = 0x20;	/* blank fill */
			}

			if ( doc->len_adjust  )
			    doc->state = DOC_MOREOUT;
			else
			{
			    *split = 0;
			    doc->type_flag = 0;
			    doc->len_adjust = 0;
			    doc->max_len = 0;
			    doc->data_len_flag = 0;
			}
		    }
		    else  if ( doc->check_len  &&  doc->max_len <= 0 )
		    {
			/* have reached max_len, discard rest of src */
			i = doc->sp[-1];
			if ( VEC_MACRO( i, src_limit, CV_L_CHAR_SZ ) ) 
			    doc->state = DOC_MOREIN;
			doc->sp[-1] -= i;

			src += i * CV_L_CHAR_SZ;

			*split = 0;
			doc->type_flag = 0;
			doc->len_adjust = 0;
			doc->max_len = 0;
			doc->data_len_flag = 0;
		    }
		    else
		    {
			/* not finished with this element */
			*split = 1;

			if ( frag != DOC_IDLE )
			    doc->state = frag;
			else  if ( ! dst_limit )
			    doc->state = DOC_MOREOUT;
			else  if ( ! src_limit )
			    doc->state = DOC_MOREIN;
		    }
		    break;

		case DB_VCH_TYPE: /* varchar */
		case -DB_VCH_TYPE: /* nullable varchar */
		case DB_TXT_TYPE: /* text */
		case -DB_TXT_TYPE: /* nullable text */
		case DB_LVCH_TYPE: /* long varchar */
		case -DB_LVCH_TYPE: /* nullable long varchar */
		    if ( vec == 0 )
		    {
			i4 len_delta = 0;

			/* adjust actual data -- len field */
			CV2L_I2_MACRO( doc->i2_ptr, &t_ui2, &status );
			t_ui2 -= doc->len_adjust;
			len_delta -= doc->len_adjust;
			CV2N_I2_MACRO(&t_ui2, doc->i2_ptr, &status);

			/* adjust pad only if not long varchar */
			if ( doc->type_flag != DB_LVCH_TYPE  &&
			     doc->type_flag != -DB_LVCH_TYPE )
			{
			    /* adjust pad -- length of pad is in doc->sp[-2] */
			    i4 new_pad = doc->sp[-2] + doc->len_adjust;
			    len_delta += doc->len_adjust;

			    if ( new_pad < 0 )  
			    {
				/* padding eliminated and data increased */
				len_delta -= new_pad;
				new_pad = 0;
			    }
			}

			/* adjust gca_data_value len */
			if ( doc->data_len_flag > 0  &&  len_delta )
			{
			    CV2L_I4_MACRO( doc->data_len_ptr, &t_i4, &status );
			    t_i4 += len_delta;
			    CV2N_I4_MACRO( &t_i4, doc->data_len_ptr, &status );
			}

			*split = 0;
			doc->type_flag = 0;
			doc->len_adjust = 0;
			doc->max_len = 0;
			doc->data_len_flag = 0;
		    }
		    else
		    if ( doc->check_len  &&  doc->max_len <= 0 )
		    {
			/* have reached max_len, discard rest of src */
			i = doc->sp[-1];
			if ( VEC_MACRO( i, src_limit, CV_L_CHAR_SZ ) ) 
			    doc->state = DOC_MOREIN;
			doc->sp[-1] -= i;

			src += i * CV_L_CHAR_SZ;

			/* adjust actual data -- len field */
			CV2L_I2_MACRO( doc->i2_ptr, &t_ui2, &status );
			t_ui2 -= doc->len_adjust;
			if ( (i4)t_ui2 > doc->check_len )
			    t_ui2 = doc->check_len;
			CV2N_I2_MACRO( &t_ui2, doc->i2_ptr, &status );

			*split = 0;
			doc->type_flag = 0;
			doc->len_adjust = 0;
			doc->max_len = 0;
			doc->data_len_flag = 0;
		    }
		    else
		    {
			/* not finished with this element */
			*split = 1;

			if ( frag != DOC_IDLE )
			    doc->state = frag;
			else  if ( ! dst_limit )
			    doc->state = DOC_MOREOUT;
			else  if ( ! src_limit )
			    doc->state = DOC_MOREIN;
		    }
		    break;

		case DB_QTXT_TYPE: /* query text */
		    if ( vec == 0 )
		    {
			/* adjust gca_data_value len */
			if ( doc->data_len_flag > 0 )
			{
			    CV2L_I4_MACRO( doc->data_len_ptr, &t_i4, &status );
			    t_i4 -= doc->len_adjust;
			    CV2N_I4_MACRO( &t_i4, doc->data_len_ptr, &status );
			}

			*split = 0;
			doc->type_flag = 0;
			doc->len_adjust = 0;
			doc->max_len = 0;
			doc->data_len_flag = 0;
		    }
		    else  if ( doc->check_len  &&  doc->max_len <= 0 )
		    {
			/* have reached max_len, discard rest of src */
			i = doc->sp[-1];
			if ( VEC_MACRO( i, src_limit, CV_L_CHAR_SZ ) ) 
			    doc->state = DOC_MOREIN;
			doc->sp[-1] -= i;

			src += i * CV_L_CHAR_SZ;

			*split = 0;
			doc->type_flag = 0;
			doc->len_adjust = 0;
			doc->max_len = 0;
			doc->data_len_flag = 0;
		    }
		    else
		    {
			/* not finished with this element */
			*split = 1;

			if ( frag != DOC_IDLE )
			    doc->state = frag;
			else  if ( ! dst_limit )
			    doc->state = DOC_MOREOUT;
			else  if ( ! src_limit )
			    doc->state = DOC_MOREIN;
		    }
		    break;

		default:
		    doc->type_flag = 0;
		    break;
		} /* switch (doc->type_flag) */
	    }
	    else
# endif /* DOUBLEBYTE */
	    {
		i = doc->sp[-1];
		if ( VEC_MACRO( i, dst_limit, CV_N_CHAR_SZ ) ) 
		    doc->state = DOC_MOREOUT;
		if ( VEC_MACRO( i, src_limit, CV_L_CHAR_SZ ) ) 
		    doc->state = DOC_MOREIN;
		doc->sp[-1] -= i;
		x2y = (u_char *)((GCC_CSET_XLT *)char_tbl)->l2n;

		while( i-- )
		{
		    *dst = x2y[ *(u_i1 *)src ];
		    src += CV_L_CHAR_SZ;
		    dst += CV_N_CHAR_SZ;
		}
	    }
	}
	goto converted;

    case OP_CV_PAD:
	/*
	** Padding does not flow across the net.  Outbound the input
	** padding is skipped.  Inbound the padding is reinserted in
	** output.
	*/
	if ( inout == INFLOW )
	{
	    i = doc->sp[-1];
	    if (VEC_MACRO(i, dst_limit, CV_L_CHAR_SZ)) doc->state = DOC_MOREOUT;
	    doc->sp[-1] -= i;

	    /* Create padding in output stream */ 
	    MEfill( i * CV_L_CHAR_SZ, (u_char)0, (PTR)dst );
	    dst += i * CV_L_CHAR_SZ;
	}
	else
	{
	    i = doc->sp[-1];
	    if (VEC_MACRO(i, src_limit, CV_L_CHAR_SZ)) doc->state = DOC_MOREIN;
	    doc->sp[-1] -= i;

	    /* Skip padding in input stream */
	    src += i * CV_L_CHAR_SZ;
	}
	goto converted;

    converted:

	src_limit = doc->src2 - src;
	dst_limit = doc->dst2 - dst;

	if ( doc->state != DOC_IDLE )	/* Need more input/output? */
	{
	    /*
	    ** Return current input/output positions and
	    ** set PC to re-execute current operation.
	    */
# ifdef GCXDEBUG
	    if ( gco_trace_level >= 2 )
		TRdisplay( "GCO convert: state = %s\n",
			    gcx_look( gcx_doc_state, doc->state ) );
# endif /* GCXDEBUG */

	    doc->src1 = src;
	    doc->dst1 = dst;
	    doc->pc--;
	    return( OK );
	}

	/* 
	** All items converted - pop vector count
	*/
	--doc->sp;
	break;

# ifdef DOUBLEBYTE
    case OP_CHAR_TYPE:
	doc->type_flag = doc->pc[0];
	doc->pc++;
	doc->max_len = doc->pc[0];
	if (doc->max_len > 0)
	{
		if (doc->tuple_flag == 0)
			doc->max_len = 2000;

		doc->check_len = doc->max_len;
	}
	else
	{
		doc->check_len = 0;
	}
	doc->pc++;
	break;

    case OP_DATA_LEN:
	doc->data_len_flag = 1;
	break;

    case OP_OBJ_DESC:
	doc->tuple_flag = 1;
	break;

    case OP_OBJ_DESC_DONE:
	doc->tuple_flag = 0;
	break;
# endif /* #ifdef DOUBLEBYTE */

    }

    goto top;
}


/*
** Name: gco_do_16src
**
** Description:
**	16 bit character set conversion.
**
** Input:
**	x2y		Doublebyte character translation matrix.
**	src		Source data
**	srclim		Source bytes available, must be >= 1
**	dst		Distination buffer.
**	dstlim		Destination space available, must be >= 1
**	vec		Object size remaining, must be >= 1
**	vec_out		Object output limit, must be >= 1 (see check_len)
**	check_len	If 0, ignore vec_out
**	len_adjust	Accumulated change in data length.
**
** Output:
**	frag		DOC_IDLE, DOC_MOREIN, DOC_MOREOUT
**
** Returns:
**	VOID
**
** History:
**	24-mar-95 (peeje01)
**	    Integrate doublebyte changes from 6500db_su4_us42
**	12-May-03 (gordy)
**	    Return DOC status in frag to indicate the fragment type.
*/

# ifdef DOUBLEBYTE
VOID
gco_do_16src
( 
    u_i2	*x2y,
    u_char	**src,
    i4		*srclim,
    u_char	**dst,
    i4		*dstlim,
    i4		*vec,
    i4		*vec_out,
    i4		check_len,
    i4		*len_adjust,
    i1		*frag
)
{
    u_i2 val = x2y[ (*src)[0] ];

    if ( val  ||  ! (*src)[0]  ||  *vec < 2 )
    {
	/* 
	** 1 source byte.
	*/
	if ( val > 0xff )
	{
	    /* 
	    ** 2 destination bytes.
	    */
	    if ( check_len  &&  *vec_out < 2 )
	    {
		(*dst)[0] = val >> 8;
		(*dst)++;
		(*dstlim)--;
		(*vec_out)--;
		(*src)++;
		(*srclim)--;
		(*vec)--;
	    }
	    else  if ( *dstlim < 2 )
	    {
		*frag = DOC_MOREOUT;
	    }
	    else
	    {
		(*dst)[0] = val >> 8;
		(*dst)[1] = val & 0xff;
		*dst += 2;
		*dstlim -= 2;
		if ( check_len )   *vec_out -= 2;
		(*len_adjust)--;
		(*src)++;
		(*srclim)--;
		(*vec)--;
	    }
	}
	else
	{
	    /* 
	    ** 1 destination byte.
	    */
	    (*dst)[0] = val;
	    (*dst)++;
	    (*dstlim)--;
	    if ( check_len )  (*vec_out)--;
	    (*src)++;
	    (*srclim)--;
	    (*vec)--;
	}
    }
    else  if ( *srclim < 2 )
    {
	/*
	** Need 2 source bytes.
	*/
	*frag = DOC_MOREIN;
    }
    else
    {
	/* 
	** 2 source bytes.
	*/
	val = x2y[ ( (*src)[0] << 8 ) +  (*src)[1] ];

	if ( ! val  ||  val > 0xff )
	{
	    /* 
	    ** 2 destination bytes.
	    */
	    if ( check_len  &&  *vec_out < 2 )
	    {
		(*dst)[0] = val >> 8; /* should this be zero? */
		(*dst)++;
		(*dstlim)--;
		(*vec_out)--;
		(*len_adjust)++;
		*src += 2;
		*srclim -= 2;
		*vec -= 2;
	    }
	    else  if ( *dstlim < 2 )
	    {
		*frag = DOC_MOREOUT;
	    }
	    else
	    {
		(*dst)[0] = val >> 8;
		(*dst)[1] = val & 0xff;
		*dst += 2;
		*dstlim -= 2;
		if ( check_len )  *vec_out -= 2;
		*src += 2;
		*srclim -= 2;
		*vec -= 2;
	    }
	}
	else
	{
	    /* 
	    ** 1 destination byte.
	    */
	    (*dst)[0] = val;
	    (*dst)++;
	    (*dstlim)--;
	    if ( check_len )  (*vec_out)--;
	    (*len_adjust)++;
	    *src += 2;
	    *srclim -= 2;
	    *vec -= 2;
	}
    }

    return;
}
# endif /* #ifdef DOUBLEBYTE */

