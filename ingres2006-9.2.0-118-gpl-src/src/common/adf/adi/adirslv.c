/*
** Copyright (c) 1989 - 1999, Ingres Corporation
** All Rights Reserved
*/

#include    <compat.h>
#include    <gl.h>
#include    <bt.h>
#include    <sl.h>
#include    <me.h>
#include    <iicommon.h>
#include    <ulf.h>
#include    <adf.h>
#include    <adudate.h>
#include    <adfint.h>
#include    <adfops.h>

/**
**
**  Name: ADIRSLV.C - Functions for datatype resolution
**
**  Description:
**	This file contains functions which support the Jupiter datatype
**	resolution rules.  See comments to adi_resolve() function for a
**	summary of these rules.
**
**          adi_resolve	    -	resolve an operation and input datatype(s)
**
**	Static functions:
**	    ad0_type_rank   -	rank a function instance 
**
[@func_list@]...
**
**
**  History:
**      28-may-89 (jrb)
**	    Initial creation.
**	21-jul-89 (jrb)
**	    Added decimal in type ranking function.
**	25-jul-89 (jrb)
**	    Augmented datatype resolution rules slightly: now if two function
**	    instances are tied even with rankings, we give preference to those
**	    whose input datatypes are the same (this was motivated by the
**	    problem that dec=i was ambiguous because f=i and dec=dec were
**	    ranked the same; now dec=dec is preferred).
**      22-mar-91 (jrb)
**          Added #include of bt.h.
**      14-aug-91 (jrb)
**          Bug 38787.  Needed to check for illegal input datatypes.
**	05-dec-91 (jrb, merged by stevet)
**	    Assigned high ranking value for DB_ALL_TYPE so that function
**	    instances containing it would be the least preferred.  This is so
**	    users' function instances can override these catch-all function
**	    instances.  Also added code in adi_resolve so that we don't try to
**	    coerce this datatype.  This work is for the outer join project.
**      22-dec-1992 (stevet)
**          Added function prototype.
**      12-Apr-1993 (fred)
**          Added byte stream datatypes.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**      12-aug-1993 (stevet)
**          Separate rankings for UDT and INGRES abstract types.  UDT
**          may have FI with the same type of coercion as INGRES abstract
**          type.  Without this separation, ADF will consider two FI
**          to have same coercion values and fail with E_AD2063_FUNC_AMBIGUOUS
**          error.
**	25-aug-93 (ed)
**	    remove dbdbms.h
**       7-Oct-1993 (fred)
**          Add ranking for byte datatypes to disambiguate operations
**          on them.
**       1-feb-95 (inkdo01)
**          Special case code to return coerce fi for long byte/varchar
**          ADI_FIND_COMMON calls
**	16-apr-1997 (shero03)
**	    Allow DB_ALL_TYPE as input too (in support of NBR 3D project)
**      21-jan-1999 (hanch04)
**          replace nat and longnat with i4
**	09-Mar-1999 (shero03)
**	    support up to 4 operands per function
**	03-Apr-2001 (gupsh01)
**	    Added support for long nvarchar's. 
**	25-Mar-2003 (gupsh01)
**	    Added support for ADI_F64_ALLSKIPUNI flag. We now ignore function
**	    instances which have DB_ALL_TYPE parameter, if the input parameter
**	    are unicode types. 
**	04-Apr-2003 (gupsh01)
**	    Removed returning of E_AD2063_FUNC_AMBIGUOUS error. If we find 
**	    more than one best function instances that have equal weight and 
**	    equal number of coercions for the input datatypes, we will select 
**	    any one of these.
**	23-Apr-2003 (gupsh01)
**	    Put back returning of E_AD2063_FUNC_AMBIGUOUS error. If we find 
**	    more than one best function instances that have equal weight and 
**	    equal number of coercions for the input datatypes.
**	18-aug-03 (hayke02)
**	    Added new parameter varchar_precedence to adi_resolve() and
**	    ad0_type_rank(). When set TRUE - adi_resolve() called from
**	    pst_resolve() with either ps202 set or psf_vch_prec ON - then
**	    varchar will have precedence over char. This change fixes bug
**	    109134.
**      31-aug-2004 (sheco02)
**          X-integrate change 466442 to main and modify precedence number
**	    from 8, 9 to 10 & 11 based on main code line scheme.
**      08-dec-2006 (stial01)
**          Added dt_iscompat(), some fi's now accept 'compatible' input dts
**      15-dec-2006 (stial01)
**          adi_resolve() DB_LNLOC_TYPE is a unicode type
**	22-Feb-2007 (kiria01) b117892
**	    Use the table macro form of datatype definitions to build
**	    lookup tables for data class and coercion precedence.
**	    Add diagnostic trace code for validation of coercions taken.
**	22-Mar-2007 (kiria01) b117892
**	    Extended table macro use to include the ADI_*_OP constants.
**	22-Mar-2007 (kiria01) b117894/b117895
**	    Split Quel and SQL precedences apart to limit destabilising
**	    Quel. Add a further tie break for ambiguity by reducing
**	    internal conversions.
[@history_template@]...
**/

/*
[@forward_type_references@]
*/


/*
**  Forward and/or External function references.
*/

static	i4     ad0_type_rank();  /* rank a function instance */
static	i4     ad0_type_class(); /* identify base data class */
static	i4     ad0_type_prec();  /* Determine dt precedence */

static bool dt_iscompat(
	DB_DT_ID	dt1, 	 
	DB_DT_ID	dt2);

/*
** Define the ADI_RESOLVE_TRACE macro to enable the tracing of coercions.
** The code will only be compiled in if defined and the value will determnine
** the initial level of trace. The levels relate to:
**  0 - No output
**  1 - Summary only - gives request and result separated by | char
**  2 - Separate trace showing choices taken
**  3 - Choices plus summary line
*/
/*#define ADI_RESOLVE_TRACE 0 */
#ifdef ADI_RESOLVE_TRACE
int coerce_trace = ADI_RESOLVE_TRACE;
#endif

#define _DEFINEEND
/*
** dt_class - class lookup array keyed on datatype
*/
#define _DEFINE(ty, v, dc, sc, q, qv, s, sv) DB_##dc##_CLASS,
static const i4 dt_class[] = {DB_DT_ID_MACRO 0};
#undef _DEFINE

/*
** dt_precedence - precedence lookup array keyed on datatype,
** language and varchar_precedence
*/
typedef enum prec_ty {
	PREC_QUEL_CHA = 0,
	PREC_QUEL_VCHA,
	PREC_SQL_CHA,
	PREC_SQL_VCHA
} PREC_TY;
#define MIN_RANK 0
#define MAX_RANK 99
#define _DEFINE(ty, v, dc, sc, q, qv, s, sv) {q, qv, s, sv},
static const i4 dt_precedence[][4] = {DB_DT_ID_MACRO {0,0,0,0}};
#undef _DEFINE
/* Check the range of the ranks */
#define _DEFINE(ty, v, dc, sc, q, qv, s, sv) \
		MIN_RANK<=q  && q <=MAX_RANK && \
		MIN_RANK<=qv && qv<=MAX_RANK && \
		MIN_RANK<=s  && s <=MAX_RANK && \
		MIN_RANK<=sv && sv<=MAX_RANK &&
# if !(DB_DT_ID_MACRO 1)
#  error "Precedence values should be between MIN_RANK and MAX_RANK"
# endif
#undef _DEFINE
#undef _DEFINEEND

/*
[@function_reference@]...
[@#defines_of_other_constants@]
[@type_definitions@]
[@global_variable_definitions@]
[@static_variable_or_function_definitions@]
*/


/*{
** Name: adi_resolve	- Resolve an operation w/inputs to a fi descriptor
**
** Description:
**      This routine will accept an operation with 0, 1, or 2 inputs and
**	return a pointer to the function instance descriptor which should be
**	used.  To determine this, the datatype resolution rules are used.  There
**	is a document "Moving Datatype Resolution into ADF" which describes the
**	datatype resolution rules in detail; here is a summary:
**
**	1. For a given operation and 0, 1, or 2 input datatypes we look for
**	   the best function instance to use (we may not get an exact fit; we
**	   may have to coerce the inputs to other types, although we NEVER
**	   coerce an abstract type... only the intrinsic types).
**
**	2. A function instance is usable iff it has the same operation id,
**	   number of inputs, and the input datatype(s) from the user are
**	   coercible to the input datatype(s) for the function instance.
**
**	3. If more than one function instance satifies these restrictions, we
**	   choose the one with the fewest coercions on the inputs.
**
**	4. If more than one function instance satifies all above requirements,
**	   we rank the function instances by assigning values to their input
**	   datatypes and adding them up.  A further penalty is exacted against
**	   function instances whose input datatypes are not the same.  (See the
**	   ad0_type_rank function later in this file.)
**
**	5. If two or more function instances are still tied at this point,
**	   (04/04/03)
**	   We will choose just the last one found in the list as all instances 
**	   are equally suitable for this function.  
**	   
** Inputs:
**	adf_scb				Pointer to an adf session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**	adi_rslv_blk
**	    .adi_op_id			The operation being resolved. If this is
**					ADI_FIND_COMMON then we return the fi
**					descriptor found by the resolution
**					rules using equals as the operation but
**					also guaranteeing the fi's input
**					datatypes will be the same.  This is
**					used for UNIONs and equijoins and
**					always takes two input datatypes.
**	    .adi_num_dts		The number of datatypes being passed in.
**	    .adi_dt1			The first datatype, if any.
**	    .adi_dt2			The second datatype, if any.
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**	adi_rslv_blk
**	    .adi_fidesc			A pointer to the function instance
**					descriptor which was resolved from the
**					given operation and datatypes is placed
**					here.
**	
**	Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
**	      If a DB_STATUS code other than E_DB_OK is returned, the caller
**	    can look in the field adf_scb.adf_errcb.ad_errcode to determine
**	    the ADF error code.  The following is a list of possible ADF error
**	    codes that can be returned by this routine:
**
**          E_AD0000_OK                 Operation succeeded.
**	    E_AD2002_BAD_OPID		adi_op_id was invalid.
**	    E_AD2004_BAD_DTID		Found invalid datatype id.
**	    E_AD2060_BAD_DT_COUNT	Number of dts must be 0, 1, or 2.
**	    E_AD2061_BAD_OP_COUNT	Found functions for this operation, but
**					none with the proper number of params.
**	    E_AD2062_NO_FUNC_FOUND	Couldn't find a function to fit these
**					types of operands.
**	    E_AD2063_FUNC_AMBIGUOUS	Found more than one function that fit.
**	    E_AD9999_INTERNAL_ERROR	ADF incurred an internal error.
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      28-may-89 (jrb)
**	    Initial creation; compatible with old PSF/AFE code.
**	09-jun-89 (jrb)
**	    Now if we find a perfect fit (i.e. no coercions required to use a
**	    particular function instance) then we quit immediately.
**	19-apr-90 (jrb)
**	    Coerce only the intrinsic types, never the abstract types.
**      22-mar-91 (jrb)
**          Added #include of bt.h.
**      14-aug-91 (jrb)
**          Bug 38787.  Added check for illegal input datatypes and also
**          moved the initialization of the idts array out of the loop and
**          changed the idts and cdts arrays to hold mapped datatypes rather
**          than raw datatypes.
**	05-dec-91 (jrb, merged by stevet)
**	    Added support for DB_ALL_TYPE for outer join project.
**	16-apr-1997 (shero03)
**	    Allow DB_ALL_TYPE as a input data type
**      03-Apr-2001 (gupsh01)
**          Added support for long nvarchar's. 
**	29-Aug-2002 (gupsh01)
**	    Added special handling of unicode datatypes as they
**	    we do no wish to coerce any normal datatypes to unicode
**	    if our resultant datatype is not a unicode type.
**      23-Apr-2003 (gupsh01)
**          Put back returning of E_AD2063_FUNC_AMBIGUOUS error. If we find
**          more than one best function instances that have equal weight and
**          equal number of coercions for the input datatypes. 
**	24-Apr-2003 (gupsh01)
**	    For unicode types SKIP_FI should be reset once we have 
**	    reached a new function instance.
**	6-Apr-2006 (kschendel)
**	    Implement varargs functions.  The first "num-args" args are
**	    type-resolved in the usual way, but any extra args are OK and
**	    are treated as ANY type.
**	24-apr-06 (dougi)
**	    Add support for adi_dtfamily to reduce number of required 
**	    function instances.
**	25-oct-2006 (dougi)
**	    Exclude UDTs from adi_dtfamily checks.
**	22-Feb-2007 (kiria01) b117691
**	    Changes to keep unicode params from being downgraded
**	    due to apparent coercion costs. E.g. nchar LIKE 'pattern' costs
**	    more to get into nvarchar LIKE nvarchar than varchar LIKE varchar.
**	    Added diagnostic code to add tracking coercions.
**	10-Aug-2007 (kiria01) b118254
**	    Don't allow resolver to choose an FI marked for legacy
*/

# ifdef ADF_BUILD_WITH_PROTOS
DB_STATUS
adi_resolve(
ADF_CB		*adf_scb,
ADI_RSLV_BLK	*adi_rslv_blk,
bool		varchar_precedence)
# else
DB_STATUS
adi_resolve( adf_scb, adi_rslv_blk, varchar_precedence)
ADF_CB		*adf_scb;
ADI_RSLV_BLK	*adi_rslv_blk;
bool		varchar_precedence;
# endif
{
    DB_STATUS		st;
    DB_DT_ID		idts[ADI_MAX_OPERANDS];
    DB_DT_ID		midts[ADI_MAX_OPERANDS];
    DB_DT_ID		cdts[ADI_MAX_OPERANDS];
    DB_DT_ID		fdts[ADI_MAX_OPERANDS];
    i4			num_idts = adi_rslv_blk->adi_num_dts;
    i4			num_cdts;
    ADI_OP_ID		op = adi_rslv_blk->adi_op_id;
    ADI_FI_TAB		fi_tab;
    ADI_FI_DESC		*curr_fi = NULL;
    ADI_FI_DESC		*best_fi = NULL;
    ADI_DT_BITMASK	mask;
    ADI_FI_ID		tmp_fiid;
    bool		count_fit = FALSE;
    i4			num_best_fis = 0;
    i4			curr_coerces;
    i4			best_coerces;
    i4			curr_dt;
    i4			i, j, best_i;
    i4			curr_rank, best_rank;
    i4			unicode_op = -1;
    i4			bunicode_op = 0;
    bool		SKIP_FI = FALSE;
    PREC_TY		prec_ty;
#ifdef ADI_RESOLVE_TRACE
# define _DEFINEEND
# define _DEFINE(ty, v, dc, sc, q, qv, s, sv) #ty ","
    static const char dtnames[] = DB_DT_ID_MACRO;
# undef _DEFINE
# define _DEFINE(n, v) #n ","
    static const char opnames[] = ADI_OPS_MACRO;
# define OPNAMESBIAS(v) v + 1 /* BIAS as starts at -1 */
# undef _DEFINE
# undef _DEFINEEND
#endif

    if (num_idts < 0  ||  num_idts > ADI_MAX_OPERANDS)
	return(adu_error(adf_scb, E_AD2060_BAD_DT_COUNT, 0));

    /* Put mapped input datatypes into local array and check validity */
    for (i=0; i < num_idts; i++)
    {
	ADI_DATATYPE	*dt;
	idts[i] = abs(adi_rslv_blk->adi_dt[i]);

	/* Fill in datatype family array. */
	dt = Adf_globs->Adi_dtptrs[idts[i]];
	if (idts[i] < ADI_DT_CLSOBJ_MIN && dt)
	    fdts[i] = dt->adi_dtfamily;
	else fdts[i] = 0;
	
        /* check for unicode types */
	if (unicode_op == -1 &&
		ad0_type_class(idts[i]) == DB_NCHAR_CLASS)
	    unicode_op = i;

	midts[i] = ADI_DT_MAP_MACRO(idts[i]);
        if ( (  midts[i] <= 0  ||  midts[i] > ADI_MXDTS ) ||
	     ( ( Adf_globs->Adi_dtptrs[midts[i]] == NULL) &&
	       ( idts[i] != DB_ALL_TYPE) )
            )
	{
	    return (adu_error(adf_scb, E_AD2004_BAD_DTID, 0));
	}
    }

    /* Since every datatype has an equals comparison fi, using equals as
    ** the operation for ADI_FIND_COMMON will implement the proper semantics;
    ** first look up function instances which have op as their operation
    */
    /* Above comment isn't correct for long byte/varchar. We'll trap them
    ** here and directly determine if there's a coercion which'll do the
    ** trick
    */
    if (op == ADI_FIND_COMMON && (idts[0] == DB_LVCH_TYPE || 
                 idts[0] == DB_LBYTE_TYPE || 
		idts[0] == DB_LNVCHR_TYPE))
    {
        st = adi_ficoerce(adf_scb, idts[0], idts[1], &tmp_fiid);
        if (st == E_DB_OK)
          adi_rslv_blk->adi_fidesc = Adf_globs->Adi_fi_lkup
                [ADI_LK_MAP_MACRO(tmp_fiid)].adi_fi;
        return(st);
    }
    if ((st = adi_fitab(adf_scb,
			op == ADI_FIND_COMMON  ?  ADI_EQ_OP  :  op,
			&fi_tab)
	 ) != E_DB_OK)
    {
	return(st);
    }

    if (fi_tab.adi_lntab == 0)
	return(adu_error(adf_scb, E_AD2062_NO_FUNC_FOUND, 0));

#ifdef ADI_RESOLVE_TRACE
    if (coerce_trace > 1)
	TRdisplay("%@ LOOKUP: %.2w:%d %#.2{ %.2w%}\n",
		  opnames, OPNAMESBIAS(op), fi_tab.adi_lntab, num_idts, idts, dtnames, 0);
#endif

    /* Determine type of precedence to apply */
    if (adf_scb->adf_qlang == DB_SQL)
	prec_ty = varchar_precedence ? PREC_SQL_VCHA : PREC_SQL_CHA;
    else
	prec_ty = varchar_precedence ? PREC_QUEL_VCHA : PREC_QUEL_CHA;


/* look thru fi table for operations with matching number of inputs */
    curr_fi = fi_tab.adi_tab_fi;
    for(i = 1; i <= fi_tab.adi_lntab; i++, curr_fi++)
    {
	enum DB_DC_ID_enum ret_class = ad0_type_class(curr_fi->adi_dtresult);
	i4 cunicode_op = (ret_class == DB_NCHR_TYPE);
	
	/* skip any legacy FI slots - we are not allowed to resolve to them */
	if ( (curr_fi->adi_fiflags & ADI_F4096_LEGACY) != 0)
	    continue;

	/* skip any fi's whose number of inputs don't match what we want.
	** Exact match normally;  varargs function has min number of args.
	*/
	num_cdts = curr_fi->adi_numargs;
	if (( (curr_fi->adi_fiflags & ADI_F128_VARARGS) == 0 && num_cdts != num_idts)
	  || ( (curr_fi->adi_fiflags & ADI_F128_VARARGS) && num_cdts > num_idts) )
	    continue;

	/* we've found at least one which has the right number of inputs */
	count_fit = TRUE;

	/* for ADI_FIND_COMMON we only want fi's with the same input dts */
	if (op == ADI_FIND_COMMON  &&  curr_fi->adi_dt[0] != curr_fi->adi_dt[1])
	    continue;

	/* now find the number of coercions necessary to use this fi */
	curr_coerces = 0;
	SKIP_FI = FALSE;
	for (j = 0; j < num_cdts; j++)
	{
	    cdts[j] = curr_fi->adi_dt[j];
		
	    /* check for unicode (except on varargs extra arg) */
	    if (ad0_type_class(cdts[j]) == DB_NCHAR_CLASS)
	    {
		cunicode_op++;
		if (unicode_op == -1 || unicode_op >= num_cdts)
		{
		    SKIP_FI= TRUE;
		    break;
		}
	    }

	    /* If any of the current datatype is DB_ALL_TYPE 
	    ** and ADI_F64_ALLNOTUNI is set then we will skip 
	    ** this function instance in evaluating the best 
	    ** function instance for unicode input.
	    */
	    
            if ((unicode_op >= 0 && unicode_op < num_cdts) &&
		(cdts[j] == DB_ALL_TYPE) &&
		(curr_fi->adi_fiflags & ADI_F64_ALLSKIPUNI))
	    {
		SKIP_FI= TRUE;
		break;
	    } 	
	}

	/* if the input datatype is not unicode and our
        ** function instance does include unicode types
        ** skip it.
        */ 
	if (SKIP_FI == TRUE)
	  continue;

	/* num_cdts == num_idts normally, or <= num_idts for varargs.
	** Only type-check the args defined by the FI.
	*/
	for (curr_dt = 0; curr_dt < num_cdts; curr_dt++)
	{
	    /* if the datatypes match, we don't need to coerce! */
	    if (cdts[curr_dt] == idts[curr_dt] || cdts[curr_dt] == DB_ALL_TYPE
			|| cdts[curr_dt] == fdts[curr_dt])
		continue;

	    /* if the input data type is DB_ALL_TYPE, then it matches */
	    if (idts[curr_dt] == DB_ALL_TYPE)
		continue;

	    /* if ADI_COMPAT_IDT flag... accept 'compatible' input datatype */
	    if ((curr_fi->adi_fiflags & ADI_COMPAT_IDT) &&
		dt_iscompat(cdts[curr_dt], idts[curr_dt]))
		continue;

	    /* not allowed to coerce an abstract type */
	    if ((Adf_globs->Adi_dtptrs[midts[curr_dt]]->
		    adi_dtstat_bits & AD_INTR) == 0)
	    {
		break;
	    }

	    /* see if we can coerce fi's input dt to caller's dt */
	    if ((st = adi_tycoerce(adf_scb, idts[curr_dt], &mask)) != E_DB_OK)
		return(st);

	    /* if we can't coerce, then this fi won't work  */
	    if (!BTtest((i4) ADI_DT_MAP_MACRO(cdts[curr_dt]), (char *)&mask))
		break;

	    /* if we can coerce, record that we need a coercion */
	    curr_coerces++;
	}

	/* if the fi didn't work for us, skip it */
	if (curr_dt != num_cdts)
	    continue;

	/* if we don't have best fi yet, or the current one is better
	** (because it has fewer coercions or a lower composite rank or an equal
	** composite rank but with matching datatypes), then make the current
	** one the best
	** If we're looking for a unicode operator then potentially we may need
	** to allow more coercions to pick the correct one so allow a better
	** unicode op in this case.
	*/
	if (	best_fi == NULL
	    ||  curr_coerces < best_coerces
	    ||  (   curr_coerces == best_coerces  &&
		    (curr_rank = ad0_type_rank(curr_fi, prec_ty)) <
		    (best_rank = ad0_type_rank(best_fi, prec_ty))
		)
	    || unicode_op && bunicode_op < cunicode_op
	   )
	{
#ifdef ADI_RESOLVE_TRACE
	    if (coerce_trace > 1)
		TRdisplay("%@ %s: %.2w:%d %#.2{ %.2w%} -> %.2w = %d\n",
			  best_fi ? "Better":" First", opnames, OPNAMESBIAS(op), i,
			  num_cdts, cdts, dtnames, 0, dtnames, curr_fi->adi_dtresult, curr_coerces);
#endif
	    num_best_fis = 1;
	    best_fi = curr_fi;
	    best_i = i;
	    best_coerces = curr_coerces;
	    bunicode_op = cunicode_op;

	    /* if this was a perfect fit, then let's stop now */
	    if (curr_coerces == 0)
		break;
		
	    continue;
	}

	/* if we were tied all the way down the list of rules, then we have
	** an equally suitable function instance; this doesn't mean that we
	** have an ambiguous situation yet, however, because something better
	** than all of the equivalent ones might still come along
	*/
	if (curr_coerces == best_coerces  &&  curr_rank == best_rank)
	{
		/*
		** If it looks like there is a tie but this one has
		** same parameter types and one matches input type
		** then use it to break the tie
		*/
	    if (cdts[0] == cdts[1] &&
		(cdts[0] == idts[0] || cdts[1] == idts[1] ))
	    {
#ifdef ADI_RESOLVE_TRACE
		    if (coerce_trace > 1)
			TRdisplay("%@ Boostd: %.2w:%d %#.2{ %.2w%} -> %.2w = %d\n",
			  opnames, OPNAMESBIAS(op), i,
			  num_cdts, cdts, dtnames, 0, dtnames, curr_fi->adi_dtresult, curr_coerces);
#endif
		/* Do not reset num_best_fis */
		best_fi = curr_fi;
		best_i = i;
	    }
	    else
	    {
#ifdef ADI_RESOLVE_TRACE
		if (coerce_trace > 1)
		    TRdisplay("%@  Equal: %.2w:%d %#.2{ %.2w%} -> %.2w = %d\n",
			  opnames, OPNAMESBIAS(op), i, num_cdts, cdts,
			dtnames, 0, dtnames, curr_fi->adi_dtresult, curr_coerces);
#endif
		num_best_fis++;
	    }
	}
    }

#ifdef ADI_RESOLVE_TRACE
    if (coerce_trace == 1 || coerce_trace == 3)
    {
	if (!best_fi)
	    TRdisplay("%@ LOOKUP:%s %.2w:%d %#.2{ %.2w%}|  Fail:\n",
		      adf_scb->adf_qlang == DB_SQL?"S":"",
		      opnames, OPNAMESBIAS(op), fi_tab.adi_lntab, num_idts, idts, dtnames, 0);
	else
	    TRdisplay("%@ LOOKUP:%s %.2w:%d %#.2{ %.2w%}| %s: %.2w:%d %#.2{ %.2w%} -> %.2w = %d\n",
		      adf_scb->adf_qlang == DB_SQL?"S":"",
		      opnames, OPNAMESBIAS(op), fi_tab.adi_lntab, num_idts, idts, dtnames, 0,
		      num_best_fis > 1 ? "Ambig" : " Best",
		      opnames, OPNAMESBIAS(op), best_i,
			best_fi->adi_numargs, best_fi->adi_dt, dtnames, 0,
			dtnames, best_fi->adi_dtresult, best_coerces);
    }
#endif

if (count_fit == FALSE)
	return(adu_error(adf_scb, E_AD2061_BAD_OP_COUNT, 0));

    if (num_best_fis == 0)
	return(adu_error(adf_scb, E_AD2062_NO_FUNC_FOUND, 0));

    if (num_best_fis > 1)
      return(adu_error(adf_scb, E_AD2063_FUNC_AMBIGUOUS, 0));


    /* the only possibility remaining is that everything is OK */
    adi_rslv_blk->adi_fidesc = best_fi;

    return(E_DB_OK);
}


/*
** Name: ad0_type_rank	- Rank datatypes for coercion precedence
**
** Description:
**      This function figures out the precedence ranking of datatypes for
**	coercion.  A datatype ID of DB_NODT will get a ranking of zero.  An
**	abstract type will be ranked 1, then float 2, and so forth.
**
**	This function considers any type it doesn't know about to be abstract.
**	That is, if it's not float, decimal, int, c, text, char, varchar, or
**	longtext, it's abstract.  The routine adds up the rankings of all the
**	input datatypes for the function instance descriptor passed in, and
**	returns the sum.
**
**	DB_ALL_TYPE is handled as a special case: if it is encountered as an
**	input type, the ranking is returned as 99 regardless of the ranking of
**	the other input.  This is intended to make it the worst choice.
**
** Inputs:
**	fi		function instance we are ranking
**
** Outputs:
**      NONE
**
**	Returns:
**	    Sum of rankings for input types (called the composite ranking).
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	28-may-89 (jrb)
**          Initial creation; adapted from PSF.
**	21-jul-89 (jrb)
**	    Added decimal between float and integer.  Also changed ranking
**	    function to favor function instances with matching input datatypes.
**	18-dec-89 (jrb)
**	    Added ranking value for DB_ALL_TYPE to make it the least preferred.
**      12-Apr-1993 (fred)
**          Added byte and byte varying datatypes.  These are now
**          equivalent to the char and varchar datatypes,
**          respectively.  Since these types are exactly equivalent,
**          they may be ambiguous -- which is good.  The user should
**          be required to specify which is desired in cases where we
**          cannot tell.
**      12-aug-1993 (stevet)
**          Separated ranking for UDT and INGRES abstract types.
**       7-Oct-1993 (fred)
**          Add ranking for byte datatypes to disambiguate operations
**          on them.
**      05-Apr-2001 (gupsh01)
**          Added ranking for nchar, nvarchar, and long nvarchar
**          data type for unicode support.
**      12-Apr-2002 (gupsh01)
**          Switched the rankings of nchar and nvarchar types so that
**          when coercing from UTF8 to UCS2 the comparison is between
**          the unicode types not the character data types.
**	05-May-2004 (gupsh01)
**	    Switched the rankings of nchar and nvarchar types with c 
**	    and text types. Since we now allow coercion between these 
**	    types, this switch will ensure that a function instance 
**	    which involves coercing from non unicode types to unicode 
**	    will have a lower weight and hence preferable, than the
**	    one which coerces from unicode types to non unicode.   
**	08-Mar-2007 (kiria01) b117892
**	    Moved bias values into iicommon.h table macro
*/
static i4
ad0_type_rank(fi, prec_ty)
ADI_FI_DESC	*fi;
PREC_TY		prec_ty;
{
    i4	    rank = MIN_RANK;
    i4	    i;

    for (i = 0; i < fi->adi_numargs; i++)
    {
	i1 prec = ad0_type_prec(fi->adi_dt[i], prec_ty);
	
	switch (prec)
	{
	case MAX_RANK:
	    return(MAX_RANK);
	default:
	    rank += prec;
	    break;
	case MIN_RANK:
	    if (fi->adi_dt[i])
	    {
		/* Must be abstract or UDT */
		DB_DT_ID mdts = ADI_DT_MAP_MACRO(fi->adi_dt[i]);
		if ((Adf_globs->Adi_dtptrs[mdts]->adi_dtstat_bits) & AD_USER_DEFINED)
		    rank += 2; /* UDT */
		else
		    rank += 1;
	    }
	    break;
	}
    }

    /* extra 1 unit penalty if datatypes are different */
    rank += (fi->adi_dt[0] != fi->adi_dt[1]);

    return (rank);
}

/*
** Name: ad0_type_class	- Identify base datatypes
**
** Description:
**      This function figures out what the base datatype is
**
** Inputs:
**	dt		datatype we are to classify
**
** Outputs:
**      NONE
**
**	Returns:
**	    The base type.
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	22-Feb-2007 (kiria01) b117892
**	    Created.
*/
static i4
ad0_type_class(dt)
i4	dt;
{
    if (dt < 0)
	dt = -dt;
    if (dt < DB_MAX_TYPE)
	return dt_class[dt];
    return DB_NONE_CLASS;
}

/*
** Name: ad0_type_class	- Identify base datatypes
**
** Description:
**      This function figures out what the base datatype is
**
** Inputs:
**	dt		datatype we are to lookup
**	prec_ty		Type of precedence bias to be applied
**
** Outputs:
**      NONE
**
**	Returns:
**	    The precedence for the given type.
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	22-Feb-2007 (kiria01) b117892
**	    Created.
*/
static i4
ad0_type_prec(dt, prec_ty)
i4		dt;
PREC_TY		prec_ty;
{
    if (dt < 0)
	dt = -dt;
    if (dt >= DB_MAX_TYPE)
	return 0;
    return dt_precedence[dt][prec_ty];
}
/*
[@function_definition@]...
*/

/*{
** Name: adi_dtfamily_resolve() - Get function instance for a coercion.
**
** Description:
**      This function is the external ADF call "adi_dtfmly_resolve()" which
**	resolves a generic function instance for the family of the input 
**	operands to a new function instance value where result, and 
**	operands are updated with the specific values of the operands.
**
** Inputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.
**		.ad_ebuflen		The length, in bytes, of the buffer
**					pointed to by ad_errmsgp.
**		.ad_errmsgp		Pointer to a buffer to put formatted
**					error message in, if necessary.
**      oldfidesc 			The "from" function instance descriptor.
**      newfidesc 			The "from" function instance descriptor.
**
** Outputs:
**	adf_scb				Pointer to an ADF session control block.
**	    .adf_errcb			ADF_ERROR struct.  If an
**					error occurs the following fields will
**					be set.  NOTE: if .ad_ebuflen = 0 or
**					.ad_errmsgp = NULL, no error message
**					will be formatted.
**		.ad_errcode		ADF error code for the error.
**		.ad_errclass		Signifies the ADF error class.
**		.ad_usererr		If .ad_errclass is ADF_USER_ERROR,
**					this field is set to the corresponding
**					user error which will either map to
**					an ADF error code or a user-error code.
**		.ad_emsglen		The length, in bytes, of the resulting
**					formatted error message.
**		.adf_errmsgp		Pointer to the formatted error message.
**
**      newfidesc 			The "into" function instance descriptor.
**					updated with information about particular
**					family members and result of the operation.
**
**      Returns:
**	      The following DB_STATUS codes may be returned:
**	    E_DB_OK, E_DB_WARN, E_DB_ERROR, E_DB_SEVERE, E_DB_FATAL
**
**	      If a DB_STATUS code other than E_DB_OK is returned, the caller
**	    can look in the field adf_scb.adf_errcb.ad_errcode to determine
**	    the ADF error code. 
**
**      Exceptions:
**          none
**
** Side Effects:
**          none
**
** History:
**      30-aug-06 (gupsh01)
**          Initial creation.
**	13-sep-06 (gupsh01)
**	    Properly fix the length of the result type.
**	11-oct-2006 (gupsh01)
**	    Fix the case where a generic function instance has
**	    more than one argument which is different from 
**	    datatype of the family.
**	25-oct-2006 (dougi)
**	    Exclude UDTs from adi_dtfamily checks.
*/
# ifdef ADF_BUILD_WITH_PROTOS
DB_STATUS
adi_dtfamily_resolve(
ADF_CB              *adf_scb,
ADI_FI_DESC         *oldfidesc,
ADI_FI_DESC         *newfidesc, 
ADI_RSLV_BLK	     *adi_rslv_blk
)
# else
DB_STATUS
adi_dtfamily_resolve( adf_scb, oldfidesc, newfidesc, adi_rslv_blk)
ADF_CB               *adf_scb;
ADI_FI_DESC          *oldfidesc;
ADI_FI_DESC          *newfidesc;
ADI_RSLV_BLK	     *adi_rslv_blk;
# endif
{
    DB_DT_ID            fdt = 0;
    DB_DT_ID            member = 0; /* dominent member for this resolution */
    DB_DT_ID            family = 0; /* family of the dominant member */
    DB_DT_ID            idts[ADI_MAX_OPERANDS];
    i4			len[ADI_MAX_OPERANDS];
    i4                  numdts = adi_rslv_blk->adi_num_dts;
    ADI_DATATYPE	*dt;
    i4			i = 0;

    if (oldfidesc && newfidesc)
    {
      MEcopy (oldfidesc, sizeof(ADI_FI_DESC), newfidesc);

      for (i = 0; i < numdts; i++)
      {
        idts[i] = abs(adi_rslv_blk->adi_dt[i]);
	dt = Adf_globs->Adi_dtptrs[idts[i]];
	len[i] = oldfidesc->adi_lenspec.adi_fixedsize; /* result length */

	if (idts[i] < ADI_DT_CLSOBJ_MIN && dt)
	    fdt = dt->adi_dtfamily;

	if (fdt == 0)
	  continue;
	else
	{
	  if (family == 0)
	    family = fdt;
	  else if (fdt != family)
	    /* datatype of more than one family cannot be resolved */
      	    return(adu_error(adf_scb, E_AD9998_INTERNAL_ERROR, 0));
	}

	if ((oldfidesc->adi_dt[i] == fdt) && (abs(idts[i]) != fdt))
	{
	  if (member == 0)
	    member = abs(idts[i]); /* first encountered is the dominant member */

	  /* confirm that the datatype family and its member is valid */
	  /* Only one family type DB_DTE_TYPE, is supported right now. */
          if (fdt == DB_DTE_TYPE)
          {
	    switch (abs(idts[i]))
	    {
		case DB_DTE_TYPE:
		  /* nothing to do here */
		  return(E_DB_OK);
		  break;
		case DB_ADTE_TYPE:
		  len[i] = ADF_ADATE_LEN;
		  break;
		case DB_TMWO_TYPE:
		case DB_TMW_TYPE:
		case DB_TME_TYPE:
		  len[i] = ADF_TIME_LEN;
		  break;
		case DB_TSWO_TYPE:
		case DB_TSW_TYPE:
		case DB_TSTMP_TYPE:
		  len[i] = ADF_TSTMP_LEN;
		  break;
		case DB_INYM_TYPE:
		  len[i] = ADF_INTYM_LEN;
		  break;
		case DB_INDS_TYPE:
		  len[i] = ADF_INTDS_LEN;
		  break;
		default:
      		  return(adu_error(adf_scb, E_AD9998_INTERNAL_ERROR, 0));
		  break;
	    }
          }
	  else
      	    return(adu_error(adf_scb, E_AD9998_INTERNAL_ERROR, 0));

	  newfidesc->adi_dt[i]= abs(idts[i]);
	}
      }

      /* Update If not all family id's in new function instance 
      ** have been updated with member information. */
      for (i = 0; i < numdts; i++)
      {
        if ((newfidesc->adi_dt[i] == family) && (member != 0))
	  newfidesc->adi_dt[i]= member;
      }

      /* Now set the result of the new function instance 
      ** If we are instructed to set the result as per the 
      ** input flag ie the first or the second operand
      */
      if ((oldfidesc->adi_fiflags & ADI_F512_RES1STARG) &&
                (adi_rslv_blk->adi_num_dts > 0))
      {
	  newfidesc->adi_dtresult = abs(adi_rslv_blk->adi_dt[0]);
          /* set the result length if it is fixed length */
          if (newfidesc->adi_lenspec.adi_lncompute == ADI_FIXED)
	     newfidesc->adi_lenspec.adi_fixedsize = len[0];
      }
      else if ((oldfidesc->adi_fiflags & ADI_F1024_RES2NDARG) &&
                 (adi_rslv_blk->adi_num_dts > 1))
      {
	  newfidesc->adi_dtresult = abs(adi_rslv_blk->adi_dt[1]);
          /* set the result length if it is fixed length */
          if (newfidesc->adi_lenspec.adi_lncompute == ADI_FIXED)
	     newfidesc->adi_lenspec.adi_fixedsize = len[1];
      }
    }
    else
        return(adu_error(adf_scb, E_AD9998_INTERNAL_ERROR, 0));

    return (E_DB_OK);    
}

/*{
** Name: adi_need_dtfamily_resolve() - Finds out if pst_resolve should
** allocate space for a new function instance for adi_resolve.  
**
** Description:
**      This function is the external ADF call "adi_need_dtfmly_resolve()"
**	which tells the caller if the function instance descriptor passed
**	in is a generic function descriptor for the family of the input 
**	operands.
**
** Inputs:
**	adf_scb			Pointer to an ADF session control block.
**      infidesc 		The input function instance descriptor.
**      adi_rslv_blk 		The location for the input operands.
**
** Outputs:
**	need			Pointer to integer: 
**				indicating 1 if resolution of family is 
**				needed and 0 if not needed.
**  Returns:
**	E_DB_OK				if successfull.	
**  Exceptions:
**	E_AD9998_INTERNAL_ERROR		if invalid input
**
** Side Effects:
**          none
**
** History:
**      30-aug-06 (gupsh01)
**          Initial creation.
**	25-oct-2006 (dougi)
**	    Exclude UDTs from adi_dtfamily checks.
*/
# ifdef ADF_BUILD_WITH_PROTOS
DB_STATUS
adi_need_dtfamily_resolve(
ADF_CB              *adf_scb,
ADI_FI_DESC         *infidesc,
ADI_RSLV_BLK	    *adi_rslv_blk,
i2		    *need
)
# else
DB_STATUS
adi_need_dtfamily_resolve( adf_scb, infidesc, adi_rslv_blk, need)
ADF_CB               *adf_scb;
ADI_FI_DESC          *infidesc;
ADI_RSLV_BLK	     *adi_rslv_blk;
i2		     *need;
# endif
{
    DB_DT_ID            fdt;
    DB_DT_ID            idts;
    i4                  numdts = adi_rslv_blk->adi_num_dts;
    ADI_DATATYPE	*dt;
    i4			i = 0;

    *need = 0;
    if (infidesc && need)
    {
      for (i = 0; i < numdts; i++)
      {
        idts = abs(adi_rslv_blk->adi_dt[i]);
	dt = Adf_globs->Adi_dtptrs[idts];
	if (idts < ADI_DT_CLSOBJ_MIN && dt)
	    fdt = dt->adi_dtfamily;
	else fdt = 0;

	if (fdt == 0)
	  continue;

	if ((infidesc->adi_dt[i] == fdt) && (idts != fdt))
	{
	   *need = 1;
	   return (E_DB_OK);
	}
      }
    }
    else
        return(adu_error(adf_scb, E_AD9998_INTERNAL_ERROR, 0));

    return (E_DB_OK);
}

/*{
** Name: adi_coerce_dtfamily_resolve() - Resolves the coercion routines
**	 for generic dtfamily case to a coercion routine for a member of
**	 the datatype family.
**
** Description:
**      This function is the external ADF call "adi_coerce_dtfamily_resolve()"
**	which resolves generic coercion routines for datatype family to a 
**	specific routine for a member of the family. This is created so that
**	when setting up the result area for coercion function instances, we
**	can get the right datatype set up for the result. Without this process
**	coercion will always go throught the data type of the dtfamily, which
**	may not give desired results.
**
** Inputs:
**	adf_scb			Pointer to an ADF session control block.
**	oldfidesc		Pointer to original ADF function descriptor.
**	input 	 		input datatype from which the coercion happens.
**	output 	 		output datatype to which the coercion happens.
**
** Outputs:
**	newfidesc		Pointer to new ADF function descriptor.
**  Returns:
**	E_DB_OK				if successfull.	
**  Exceptions:
**	E_AD9998_INTERNAL_ERROR		if invalid input
**
** Side Effects:
**          none
**
** History:
**      12-oct-06 (gupsh01)
**          Initial creation.
**	25-oct-2006 (dougi)
**	    Exclude UDTs from adi_dtfamily checks.
*/

# ifdef ADF_BUILD_WITH_PROTOS
DB_STATUS
adi_coerce_dtfamily_resolve(
ADF_CB               *adf_scb,
ADI_FI_DESC          *oldfidesc,
DB_DT_ID	     input, 	 
DB_DT_ID	     output,	 
ADI_FI_DESC          *newfidesc
)
# else
adi_coerce_dtfamily_resolve( adf_scb, oldfidesc, input, output, newfidesc)
ADF_CB		*adf_scb;
ADI_FI_DESC	*oldfidesc;
DB_DT_ID	input; 	 
DB_DT_ID	output;
ADI_FI_DESC	*newfidesc;
# endif
{
    ADI_DATATYPE    *dtp;  
    DB_DT_ID	fi_input = abs(oldfidesc->adi_dt[0]);
    DB_DT_ID	in_fdt;
    DB_DT_ID	fi_output = abs(oldfidesc->adi_dtresult);
    DB_DT_ID	out_fdt;

    if (oldfidesc && newfidesc)
    {
 	  MEcopy (oldfidesc, sizeof(ADI_FI_DESC), newfidesc);
    }

    dtp = Adf_globs->Adi_dtptrs[input];
    if (input < ADI_DT_CLSOBJ_MIN && dtp)
      in_fdt = dtp->adi_dtfamily;
    else in_fdt = 0;

    /* Check input and change the data type if necessary */
    if (in_fdt && (abs(input) != fi_input) && (fi_input == in_fdt))
    {
      /* replace the FI input data type with input */
      newfidesc->adi_dt[0]= abs(input);
    }

    dtp = Adf_globs->Adi_dtptrs[output];
    if (output < ADI_DT_CLSOBJ_MIN && dtp)
      out_fdt = dtp->adi_dtfamily;
    else out_fdt = 0;

    /* Check for supported family types */
    if ((out_fdt != DB_DTE_TYPE) && (in_fdt != DB_DTE_TYPE))
     return (E_DB_OK);

    if ((abs(output) != fi_output) && (fi_output == out_fdt))
    {
      /* replace the FI result data type with output */
      newfidesc->adi_dtresult = abs(output);

      /* Change result length if necessary */
      if (newfidesc->adi_lenspec.adi_lncompute == ADI_FIXED)
      {
	/* DATE Family types */
	if (out_fdt == DB_DTE_TYPE)
	{
	  switch (abs(output))
	  {
            case DB_DTE_TYPE:
             /* Never happens FI is already set*/
              break;
            case DB_ADTE_TYPE:
 	      newfidesc->adi_lenspec.adi_fixedsize = ADF_ADATE_LEN;
              break;
            case DB_TMWO_TYPE:
            case DB_TMW_TYPE:
            case DB_TME_TYPE:
 	      newfidesc->adi_lenspec.adi_fixedsize = ADF_TIME_LEN;
              break;
            case DB_TSWO_TYPE:
            case DB_TSW_TYPE:
            case DB_TSTMP_TYPE:
 	      newfidesc->adi_lenspec.adi_fixedsize = ADF_TSTMP_LEN;
              break;
            case DB_INYM_TYPE:
 	      newfidesc->adi_lenspec.adi_fixedsize = ADF_INTYM_LEN;
              break;
            case DB_INDS_TYPE:
 	      newfidesc->adi_lenspec.adi_fixedsize = ADF_INTDS_LEN;
              break;
            default:
              return(adu_error(adf_scb, E_AD9998_INTERNAL_ERROR, 0));
              break;
          }
	}
      }
    }
    return (E_DB_OK);
}

/*
** Function to define ADI_COMPAT_IDT 'compatible' input datatypes:
** Types:
** STR		DB_CHR_TYPE, DB_CHA_TYPE, DB_TXT_TYPE, DB_VCH_TYPE
** UNISTR	DB_NCHR_TYPE, DB_NVCHR_TYPE
** BYTESTR	DB_BYTE_TYPE, DB_VBYTE_TYPE
** Alternative to this routine would be to add a new element to ADI_DATATYPE,
** and assign each of the above datatypes the corresponding datatype class.
**
** Most routines that take one of these types, can in fact handle any 
** of the compatible types, requiring less instances in the fi table.
** The motivation for adding this routine is the FI for the position function,
** which would require a ridiculous number of FI's because it takes 2-3 args.
**
** If ALL possible inputs are not listed in the function instance table,
** the optimizer will generate unecessary coercions. e.g.
** select position(nchr, nvchr) will generates unecessary coercion unless 
** there is a function instance for position(nchr, nvchr).
**
** If DB_ALL_TYPE is used for input arguments, the optimizer would not
** generate coercions in cases that it should (e.g. position(vch, nvch))
** (or the routine has to be able to handle mixed datatypes)
**
** NOTE: ADI_COMPAT_IDT flag should be used ONLY for fi's that are
** EXACTLY the same except for varying (but compatible) input datatypes.
**
** Inputs:
**      dt1
**      dt2
**
**  Returns:
**      TRUE                            Datatypes are compatible
**      FALSE                           Datatypes are not compatible
**                                      (As defined by this function)
** Side Effects:
**          none
**
** History:
**      07-dec-06 (stial01)
**          Created.
*/
static bool
dt_iscompat(
DB_DT_ID	dt1, 	 
DB_DT_ID	dt2)
{
    if (dt1 == DB_CHR_TYPE || dt1 == DB_CHA_TYPE || dt1 == DB_TXT_TYPE ||
		dt1 == DB_VCH_TYPE)
    {
	if (dt2 == DB_CHR_TYPE || dt2 == DB_CHA_TYPE || dt2 == DB_TXT_TYPE ||
		dt2 == DB_VCH_TYPE)
	    return (TRUE);
	else
	    return (FALSE);
    }
    
    if (dt1 == DB_NCHR_TYPE || dt1 == DB_NVCHR_TYPE)
    {
	if (dt2 == DB_NCHR_TYPE || dt2 == DB_NVCHR_TYPE)
	    return (TRUE);
	else
	    return (FALSE);
    }

    if (dt1 == DB_BYTE_TYPE || dt1 == DB_VBYTE_TYPE)
    {
	if (dt2 == DB_BYTE_TYPE || dt2 == DB_VBYTE_TYPE)
	    return (TRUE);
 	else
	    return (FALSE);
    }

    return (FALSE);

}
