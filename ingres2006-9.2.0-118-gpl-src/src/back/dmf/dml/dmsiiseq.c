/*
**Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <bt.h>
#include    <cs.h>
#include    <cv.h>
#include    <di.h>
#include    <ex.h>
#include    <me.h>
#include    <nm.h>
#include    <pc.h>
#include    <sd.h>
#include    <sr.h>
#include    <tm.h>
#include    <tr.h>
#include    <sl.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <lg.h>
#include    <lk.h>
#include    <scf.h>
#include    <st.h>
#include    <ulf.h>
#include    <sxf.h>
#include    <adf.h>
#include    <dmf.h>
#include    <dm.h>
#include    <dmp.h>
#include    <dmccb.h>
#include    <dmse.h>
#include    <dmucb.h>
#include    <dmrcb.h>
#include    <dmscb.h>
#include    <dmtcb.h>
#include    <dmxcb.h>
#include    <dmpp.h>
#include    <dm1b.h>
#include    <dm1m.h>
#include    <dm0l.h>
#include    <dm0m.h>
#include    <dm0p.h>
#include    <dm0pbmcb.h>
#include    <dm0s.h>
#include    <dm1c.h>
#include    <dm1cx.h>
#include    <dm1i.h>
#include    <dm1h.h>
#include    <dm1r.h>
#include    <dm1s.h>
#include    <dm1p.h>
#include    <dm2f.h>
#include    <dm2t.h>
#include    <dm2umct.h>
#include    <dm2umxcb.h>
#include    <dm2u.h>
#include    <dm2r.h>
#include    <dm2rlct.h>
#include    <dmftrace.h>
#include    <dmd.h>
#include    <dml.h>
#include    <dmpbuild.h>
#include    <dmpepcb.h>
#include    <dma.h>
#include    <dm1cn.h>
#include    <cui.h>
#include    <nmcl.h>

/**
**
**  Name: DMSIISEQ.C - IISEQUENCE DML operations.
**
**  Description:
**
**	Contains functions to fetch and update IISEQUENCE
**	tuples during DML operations.
**
**	external functions:
**
**          dms_fetch_iiseq  - Fetch/update an iisequence tuple
**			       into a bound DML_SEQ.
**          dms_update_iiseq - Update an iisequence tuple 
**			       from an exclusivly bound DML_SEQ
**			       at end of transaction.
**
**	internal functions:
**	    dms_iiseq	     - Common to both.
**
**  History:
**	05-Mar-2002 (jenjo02)
**	    Created.
**      18-feb-2004 (stial01)
**          Fixed incorrect casting of length arguments.
**	24-apr-2007 (dougi)
**	    Dropped DB_MAX_DECxxx's for hard coded 16 & 31 (to match 
**	    iisequence definition).
*/

#define		DMS_FETCH	0x0001L
#define		DMS_UPDATE	0x0002L

static DB_STATUS dms_iiseq(
			    i4		actions,
			    DML_XCB	*xcb,
			    DML_SEQ	*s,
			    i4		*err_code);

/* EX handler for decimal overflow, shared by dmsseq.c */
FUNC_EXTERN STATUS dms_exhandler(
			    EX_ARGS	*ex_args);

/*{
** Name: dms_fetch_iiseq - Fetch an iisequence tuple into a DML_SEQ.
**
** Description:
**
**		Read an IISEQUENCE tuple, update the Server's
**		DML_SEQ cache, then update the IISEQUENCE
**		tuple with its new "next" value.
**
** Inputs:
**	xcb				Session's transaction.
**	s				Bound DML_SEQ of interest.
**
** Outputs:
**	err_code			Reason for error return status.
**					 
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	05-Mar-2002 (jenjo02)
**	    Created.
*/
DB_STATUS
dms_fetch_iiseq(
DML_XCB		*xcb,
DML_SEQ		*s,
i4		*err_code)
{
    return( dms_iiseq( DMS_FETCH | DMS_UPDATE, xcb, s, err_code) );
}

/*{
** Name: dms_update_iiseq - Update the next value of an
**			    exclusively bound DML_SEQ's
**			    IISEQUENCE tuple.
**
** Description:
**
**		At Transaction commit time, any exclusively bound
**		(X-locked) DML_SEQ's which have been updated by
**		NEXT VALUE operations must have their corresponding
**		iisequence tuple's next value updated.
**
** Inputs:
**	xcb				Session's transaction.
**	s				Bound DML_SEQ of interest.
**
** Outputs:
**	err_code			Reason for error return status.
**					 
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	05-Mar-2002 (jenjo02)
**	    Created.
*/
DB_STATUS
dms_update_iiseq(
DML_XCB		*xcb,
DML_SEQ		*s,
i4		*err_code)
{
    return( dms_iiseq(DMS_UPDATE, xcb, s, err_code) );
}

/*{
** Name: dms_iiseq - Fetch and/or update an iisequence tuple.
**
** Description:
**
**		Fetch the next cache of values from IISEQUENCE
**		and/or update a bound DML_SEQ's iisequence tuple.
**
**		All updates to IISEQUENCE by way of DML
**		operations (NEXT VALUE) are done using
**		the DML transaction, but the value
**		of the Sequence is not related to
**		the success or failure of any transaction.
**		As such, these updates cannot be rolled
**		back. They are, however, logged inside
**		a mini-transaction so that the most recent
**		Sequence value will be available if the 
**		database is rolled forward.
**
**		Note that IISEQUENCE tuples are locked
**		using physical locking.
**
** Inputs:
**	actions				What to do:
**					    DMS_FETCH
**					    DMS_UPDATE
**	xcb				Session's transaction.
**	s				Bound DML_SEQ of interest
**
** Outputs:
**	err_code			Reason for error return status.
**					 
**	Returns:
**	    E_DB_OK
**	    E_DB_ERROR
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	05-Mar-2002 (jenjo02)
**	    Created.
**	24-Feb-2003 (jenjo02)
**	    Force no logging, no journalling in RCB.
**	    iisequence updates are performed under-the-covers
**	    ala "update_rel()" and are neither logged nor
**	    journalled (BUG 109703, STAR 12526687).
**	    When wrapping, compute next value whether there's
**	    "spare_cache" or not (BUG 109704).
**	06-Mar-2003 (jenjo02)
**	    Add EX handler to catch those nasty decimal
**	    overflow situations.
**	    Cleaned up handling of extreme ranges, cast
**	    values to float to avoid overflow.
**	    Added non-CYCLE DBS_EXHAUSTED tests and flag,
**	    set when the last possible value of a non-cycling
**	    sequence has been cached. The flag may be reset 
**	    by ALTER SEQUENCE if the sequence is altered such
**	    that additional values can be generated.
**	03-Dec-2004 (jenjo02)
**	    Check and reset DBS_RESTART rather than relying
**	    on start value comparisons which fails to detect
**	    a restart unless we've incremented though at least
**	    one cached range of values.
**	03-may-2006 (toumi01)
**	    For the DMS_UPDATE action, if we copy s-> values to
**	    stup-> and write to II_SEQUENCE at a transaction boundary,
**	    set s->seq_cache = 0 so that we'll go back to the II_SEQUENCE
**	    well for fresh values when we next use the sequence.
**	13-Jul-2006 (kiria01)  b112605
**	    Extended use of the seq_cache member such that a negative value
**	    is to be interpreted as value exhaustion. Zero now uniquiely
**	    means 'cache reload' needed. Positive values indicate remaining
**	    cache. Dropped use of seq_version.
**	05-Sep-2006 (jonj)
**	    Reinstate logging of IISEQUENCE updates so that rollforwarddb
**	    will work correctly. READONLY transactions are a special case,
**	    "ILOG" flags to dmxe, dm0l, lg denote and permit "internal logging"
**	    in otherwise conflicting transaction states.
**      06-feb-2005 (horda03) Bug 48659/INGSRV 2716 Addition
**          If in a non-journaled TX, and a Non-Journaled BT record has
**          already been written, don't write a BT record.
*/
static DB_STATUS
dms_iiseq(
i4		actions,
DML_XCB		*xcb,
DML_SEQ		*s,
i4		*err_code)
{
    DMP_DCB		*dcb = xcb->xcb_odcb_ptr->odcb_dcb_ptr;
    DMP_RCB		*r = (DMP_RCB*)NULL;
    DM_TID		tid;
    DB_TAB_ID		table_id;
    DB_IISEQUENCE	stuple, *stup = &stuple;
    DB_TAB_TIMESTAMP	timestamp;
    DM2R_KEY_DESC	qual_list[2];
    DB_STATUS		status, local_status;
    i4			local_error;
    LG_LSN		bm_lsn, lsn;
    i4			lock_mode, access_mode;
    u_char  		cache[16];
    u_char  		next[16];
    u_char  		work[16];
    u_char  		zero[16];
    u_i2		DL = 16;
    i4			DP = 31;
    i4      		prec, scale;
    i4			asc;
    i4			rcache, ucache, max_cache, spare_cache;
    f8			f8min, f8max, f8next, f8incr, f8temp, f8maxi4;
    EX_CONTEXT		ex;

    if ( actions & DMS_UPDATE )
    {
	/* Intend to update */
	lock_mode = DM2T_IX;
	access_mode = DM2T_A_WRITE;
    }
    else 
    {
	/* Just looking */
	lock_mode = DM2T_IS;
	access_mode = DM2T_A_READ;
    }

    /*
    ** Open IISEQUENCE and fetch the record describing the sequence.
    */
    table_id.db_tab_base = DM_B_SEQ_TAB_ID;;
    table_id.db_tab_index = DM_I_SEQ_TAB_ID;

    status = dm2t_open(dcb, &table_id, lock_mode, DM2T_UDIRECT,
	access_mode, (i4)0, (i4)20, (i4)0,
	xcb->xcb_log_id, xcb->xcb_lk_id, (i4)0, (i4)0, DM2T_S,
	&xcb->xcb_tran_id, &timestamp, &r, (DML_SCB *)0, err_code);

    if ( status == E_DB_OK )
    {
	/*
	** While updates to IISEQUENCE are never rolled back,
	** they must be logged if the DB is journaled in
	** the event of rollforward, even if this is a 
	** READONLY transaction.
	*/
	if ( (xcb->xcb_flags & XCB_NOLOGGING) == 0 &&
	      dcb->dcb_status & DCB_S_JOURNAL )
	{
	    r->rcb_journal = RCB_JOURNAL;
	    r->rcb_logging = RCB_LOGGING;
	}
	else
	{
	    r->rcb_journal = 0;
	    r->rcb_logging = 0;
	}

	r->rcb_k_duration |= RCB_K_TEMPORARY;

	qual_list[0].attr_number = DM_1_SEQ_KEY;
	qual_list[0].attr_operator = DM2R_EQ;
	qual_list[0].attr_value = (char *)&s->seq_name;
	qual_list[1].attr_number = DM_2_SEQ_KEY;
	qual_list[1].attr_operator = DM2R_EQ;
	qual_list[1].attr_value = (char *)&s->seq_owner;

	status = dm2r_position(r, DM2R_QUAL, qual_list,
	    (i4)2, (i4)0, (i4)0, (DM_TID *)0, err_code);
    }

    /* Find the iisequence that matches name+owner+seq_id */
    while ( status == E_DB_OK )
    {
	status = dm2r_get(r, &tid, DM2R_GETNEXT,
			  (char *)stup, err_code);
	if ( status  == E_DB_OK &&
	    (MEcmp((PTR)&stup->dbs_name, 
		(PTR)&s->seq_name, sizeof(DB_NAME))) == 0
		 &&
	    (MEcmp((PTR)&stup->dbs_owner, 
		(PTR)&s->seq_owner, sizeof(DB_OWN_NAME))) == 0
		 &&
	     stup->dbs_uniqueid.db_tab_base == s->seq_id )
	{
	    break;
	}
    }

    if ( status == E_DB_OK && actions & DMS_FETCH )
    {
	s->seq_dtype = stup->dbs_type;
	s->seq_dlen  = stup->dbs_length;
	s->seq_prec  = stup->dbs_prec;

	/* Off all but "updated" flag */
	s->seq_flags &= SEQ_UPDATED;

	if ( stup->dbs_flag & DBS_CYCLE )
	    s->seq_flags |= SEQ_CYCLE;
	if ( stup->dbs_flag & DBS_ORDER )
	    s->seq_flags |= SEQ_ORDER;
	if ( stup->dbs_flag & DBS_CACHE )
	    s->seq_flags |= SEQ_CACHE;

	/* Sequence exhausted? */
	if ( stup->dbs_flag & DBS_EXHAUSTED )
	{
	    s->seq_cache = -1; /* b112605 */
	    actions &= ~DMS_UPDATE;
	}

	else if ( s->seq_dtype == DB_INT_TYPE )
	{
	    s->seq_min.intval = stup->dbs_min.intval;
	    s->seq_max.intval = stup->dbs_max.intval;

	    /* Ascending, or descending sequence? */
	    asc = (stup->dbs_incr.intval > 0);

	    /*
	    ** If there are remaining cached values, then
	    ** we've arrived here because of an ALTER SEQUENCE
	    ** which has updated the sequence version.
	    **
	    ** If the DBS_RESTART flag is set, we toss the
	    ** remaining cached values and re-cache
	    ** from IISEQUENCE.
	    **
	    ** We must also check for any alterations to values
	    ** which affect our cache, a change in increment, or
	    ** max/min values. We'll try to adjust our cache
	    ** to reflect those changes, but if the result is 
	    ** that the new next value is outside of the 
	    ** allotted value range, we toss the cache and 
	    ** re-cache from IISEQUENCE.
	    */
	    if ( s->seq_cache > 0) /* b112605 */
	    {
		/* Check for a change in min/max that voids our cache */
		if ( stup->dbs_flag & DBS_RESTART ||
		     stup->dbs_min.intval > s->seq_begin.intval ||
		     stup->dbs_max.intval < s->seq_end.intval )
		{
		    s->seq_cache = 0;
		}
		else if ( s->seq_incr.intval != stup->dbs_incr.intval )
		{
		    /*
		    ** Increment has changed; adjust cached
		    ** next value down by the old 
		    ** increment and up by the new increment,
		    ** recompute cache size.
		    */
		    s->seq_next.intval -= s->seq_incr.intval;
		    s->seq_next.intval += stup->dbs_incr.intval;

		    /* Note that next has been changed */
		    s->seq_flags |= SEQ_UPDATED;

		    /* If new next is out of range, toss the cache */
		    if ( s->seq_next.intval > s->seq_end.intval ||
		         s->seq_next.intval < s->seq_begin.intval )
		    {
			s->seq_cache = 0;
		    }
		    else
		    {
			/* Check for change of direction */
			if ( (s->seq_incr.intval > 0 && asc) ||
			     (s->seq_incr.intval < 0 && !asc) )
			    s->seq_cache = abs(((s->seq_end.intval 
					       - s->seq_next.intval)
					       / stup->dbs_incr.intval))
					   + 1;
			else
			    s->seq_cache = abs(((s->seq_next.intval 
					       - s->seq_begin.intval)
					       / stup->dbs_incr.intval))
					   + 1;

			s->seq_incr.intval = stup->dbs_incr.intval;
		    }
		}
		/* If cache is still valid, cancel IISEQUENCE update */
		if ( s->seq_cache )
		    actions &= ~DMS_UPDATE;
	    }

	    if ( s->seq_cache == 0 )
	    {
		/* Reload the cache */
		s->seq_flags &= ~SEQ_UPDATED;

		/* Copy incr, start, begin, assign nextval */
		s->seq_incr.intval = stup->dbs_incr.intval;
		s->seq_start.intval = stup->dbs_start.intval;
		s->seq_next.intval = s->seq_begin.intval
			    = stup->dbs_next.intval;


		/* IISEQUENCE defined cache size, may be zero */
		ucache = (stup->dbs_cache) ? stup->dbs_cache : 1;

		/* Use float to guard against extreme ranges */
		f8max  = (f8)s->seq_max.intval;
		f8min  = (f8)s->seq_min.intval;
		f8next = (f8)s->seq_next.intval;
		f8incr = (f8)abs(s->seq_incr.intval);
		f8maxi4 = (f8)MAXI4;

		/* Determine maximum number of cached values */
		if ( (f8temp = (f8max - f8min) / f8incr) >= f8maxi4 )
		    max_cache = MAXI4;
		else
		    max_cache = (i4)f8temp;

		/* Use the lesser of the two */
		ucache = (ucache < max_cache) ? ucache : max_cache;

		/*
		** Compute rcache as number of values 
		** remaining between next value and end of 
		** the sequence.
		*/
		if ( asc )
		{
		    if ( (f8temp = (f8max - f8next) / f8incr) >= f8maxi4 )
			rcache = MAXI4;
		    else
			rcache = (i4)f8temp+1;
		}
		else
		{
		    if ( (f8temp = (f8next - f8min) / f8incr) >= f8maxi4 )
			rcache = MAXI4;
		    else
			rcache = (i4)f8temp+1;
		}

		/* Allot the smaller cache size, note spare */
		if ( rcache <= ucache )
		{
		    s->seq_cache = rcache;

		    if ( stup->dbs_flag & DBS_CYCLE )
		    {
			spare_cache = ucache - rcache;
			ucache = rcache;
		    }
		    else
		    {
			/* Then this is the last set of values */
			stup->dbs_flag |= DBS_EXHAUSTED;
			/* Note this flag is reset by ALTER SEQ */

			/* Update IISEQUENCE to last cached valued */
			ucache = --rcache;
		    }
		}
		else
		{
		    s->seq_cache = ucache;
		    spare_cache = 0;
		}

		/* In-memory sequence complete, update IISEQUENCE */


		/* Compute next IISEQUENCE value beyond cache */
		stup->dbs_next.intval +=
			(ucache * stup->dbs_incr.intval);
	    
		/* Wrap if boundary is blown */
		if ( stup->dbs_next.intval > stup->dbs_max.intval ||
		     stup->dbs_next.intval < stup->dbs_min.intval )
		{
		    stup->dbs_next.intval = 
			spare_cache * stup->dbs_incr.intval;
		    if ( asc )
			stup->dbs_next.intval += stup->dbs_min.intval;
		    else
			stup->dbs_next.intval += stup->dbs_max.intval;

		    s->seq_cache += spare_cache;
		}

		/* Set server's ending cached value */
		s->seq_end.intval = stup->dbs_next.intval;

		/* If no values cached, cancel the update */
		if ( s->seq_cache == 0 )
		    actions &= ~DMS_UPDATE;
	    }
	}
	else if ( s->seq_dtype == DB_DEC_TYPE )
	{
	    /*
	    ** Repeat all of the above, this time
	    ** in dewey decimal:
	    */
	    
	    MEcopy(&stup->dbs_min.decval, DL, &s->seq_min.decval);
	    MEcopy(&stup->dbs_max.decval, DL, &s->seq_max.decval);

	    /* Make a decimal zero for compares */
	    CVlpk((i4)0,DP,0, (PTR)&zero);

	    /* Ascending, or descending sequence? */
	    asc = ((MHpkcmp(stup->dbs_incr.decval,DP,0,
	    		    zero,DP,0)) > 0);
	    
	    /*
	    ** If there are remaining cached values, then
	    ** we've arrived here because of an ALTER SEQUENCE
	    ** which has updated the sequence version.
	    **
	    ** If the DBS_RESTART flag is set, we toss the
	    ** remaining cached values and re-cache
	    ** from IISEQUENCE.
	    **
	    ** We must also check for any alterations to values
	    ** which affect our cache, a change in increment, or
	    ** max/min values. We'll try to adjust our cache
	    ** to reflect those changes, but if the result is 
	    ** that the new next value is outside of the 
	    ** allotted value range, we toss the cache and 
	    ** re-cache from IISEQUENCE.
	    */
	    if ( s->seq_cache )
	    {
		/* Check for a change in min/max that voids the cache */
		if ( stup->dbs_flag & DBS_RESTART
		    ||
		     MHpkcmp(stup->dbs_min.decval,DP,0,
			     s->seq_begin.decval,DP,0) > 0
		    ||
		     MHpkcmp(stup->dbs_max.decval,DP,0,
			     s->seq_end.decval,DP,0) < 0 )
		{
		    s->seq_cache = 0;
		}
		else if ( MEcmp(stup->dbs_incr.decval, 
			        s->seq_incr.decval, DL) )
		{
		    /*
		    ** Increment has changed; adjust cached
		    ** next value down by the old 
		    ** increment and up by the new increment,
		    ** recompute cache size.
		    */
		    if ( EXdeclare(dms_exhandler, &ex) )
			/* Sequence overflowed, toss the cache */
			s->seq_cache = 0;
		    else
		    {
			MHpksub(s->seq_next.decval,DP,0,
				s->seq_incr.decval,DP,0,
				next, &prec, &scale);
			MHpkadd(next,DP,0,
				stup->dbs_incr.decval,DP,0,
				s->seq_next.decval, &prec, &scale);

			/* Note that next has been changed */
			s->seq_flags |= SEQ_UPDATED;

			/* If new next is out of range, toss the cache */
			if ( MHpkcmp(s->seq_next.decval,DP,0,
				     s->seq_end.decval,DP,0) > 0
			    ||
			     MHpkcmp(s->seq_next.decval,DP,0,
				     s->seq_begin.decval,DP,0) < 0 )
			{
			    s->seq_cache = 0;
			}
			else
			{
			    /* Check for change of direction */
			    if ( (MHpkcmp(s->seq_incr.decval,DP,0,
					  zero,DP,0) > 0 && asc)
				||
				 (MHpkcmp(s->seq_incr.decval,DP,0,
					  zero,DP,0) < 0 && !asc) )
			    {
				MHpksub(s->seq_end.decval,DP,0,
					s->seq_next.decval,DP,0,
					work, &prec, &scale);
			    }
			    else
			    {
				MHpksub(s->seq_next.decval,DP,0,
					s->seq_begin.decval,DP,0,
					work, &prec, &scale);
			    }

			    MHpkdiv(work,DP,0,
				    stup->dbs_incr.decval,DP,0,
				    cache, &prec, &scale);
			    CVpkl((PTR)&cache, prec, scale, &s->seq_cache);
			    s->seq_cache = abs(s->seq_cache) + 1;

			    /* Set the new increment */
			    MEcopy(&stup->dbs_incr.decval, DL,
				   &s->seq_incr.decval);
			}
		    }
		    EXdelete();
		}
		/* If cache still valid, no need to update IISEQ */
		if ( s->seq_cache )
		    actions &= ~DMS_UPDATE;
	    }

	    if ( s->seq_cache == 0 )
	    {
		/* Get new range of values from IISEQUENCE */
		s->seq_flags &= ~SEQ_UPDATED;

		/* Copy incr, start, begin, assign nextval */
		MEcopy(&stup->dbs_incr.decval, DL,
		       &s->seq_incr.decval);
		MEcopy(&stup->dbs_start.decval, DL,
		       &s->seq_start.decval);
		MEcopy(&stup->dbs_next.decval, DL,
		       &s->seq_next.decval);
		MEcopy(&stup->dbs_next.decval, DL,
		       &s->seq_begin.decval);

		/* User defined cache size */
		ucache = (stup->dbs_cache) ? stup->dbs_cache : 1;

		/* Use float to guard against extreme ranges */
		CVpkf(s->seq_max.decval, DP, 0, &f8max);
		CVpkf(s->seq_min.decval, DP, 0, &f8min);
		CVpkf(s->seq_next.decval, DP, 0, &f8next);
		/* Need abs(incr) for a bit... */
		MHpkabs(&s->seq_incr.decval, DP, 0, &work);
		CVpkf((PTR)&work, DP, 0, &f8incr);
		f8maxi4 = (f8)MAXI4;

		/* Determine maximum number of cached values */
		if ( (f8temp = (f8max - f8min) / f8incr) >= f8maxi4 )
		    max_cache = MAXI4;
		else
		    max_cache = (i4)f8temp;

		/* Use the lesser of the two */
		ucache = (ucache < max_cache) ? ucache : max_cache;

		/*
		** Compute rcache as number of values 
		** remaining between next value and end of 
		** the sequence.
		*/
		if ( asc )
		{
		    if ( (f8temp = (f8max - f8next) / f8incr) >= f8maxi4 )
			rcache = MAXI4;
		    else
			rcache = (i4)f8temp+1;
		}
		else
		{
		    if ( (f8temp = (f8next - f8min) / f8incr) >= f8maxi4 )
			rcache = MAXI4;
		    else
			rcache = (i4)f8temp+1;
		}

		/* Allot the smaller cache size, note spare */
		if ( rcache <= ucache )
		{
		    s->seq_cache = rcache;

		    if ( stup->dbs_flag & DBS_CYCLE )
		    {
			spare_cache = ucache - rcache;
			ucache = rcache;
		    }
		    else
		    {
			/* Then this is the last set of values */
			stup->dbs_flag |= DBS_EXHAUSTED;
			/* Note this flag is reset by ALTER SEQ */

			/* Update IISEQUENCE to last cached valued */
			ucache = --rcache;
		    }
		}
		else
		{
		    s->seq_cache = ucache;
		    spare_cache = 0;
		}


		/* In-memory sequence complete, update IISEQUENCE */
		    

		/* Compute next IISEQUENCE value beyond cache */
		CVpkf(s->seq_incr.decval, DP, 0, &f8incr);

		f8next += ((f8)ucache * f8incr);
	    
		/* Wrap if boundary is blown */
		if ( f8next > f8max || f8next < f8min )
		{
		    f8next = ((f8)spare_cache * f8incr);
		    if ( asc )
			f8next += f8min;
		    else
			f8next += f8max;

		    s->seq_cache += spare_cache;
		}

		/* Convert float "next" to dec in IISEQUENCE tuple */
		CVfpk(f8next, DP, 0, (PTR)&stup->dbs_next.decval);


		/* Set server's ending cached value */
		MEcopy(&stup->dbs_next.decval, DL,
		       &s->seq_end.decval);

		/* If no values cached, cancel the update */
		if ( s->seq_cache == 0 )
		    actions &= ~DMS_UPDATE;
	    }
	}
    }
    else if ( status == E_DB_OK && actions & DMS_UPDATE )
    {
	/* No fetch, just update dbs_next from DML_SEQ */

	/* 
	** Note that this can only be done if another
	** server has not drawn from the sequence, thereby
	** updating IISEQUENCE's next value.
	*/
	if ( s->seq_dtype == DB_INT_TYPE )
	{
	    if ( s->seq_end.intval == stup->dbs_next.intval )
	    {
		stup->dbs_next.intval = s->seq_next.intval;
		s->seq_cache = 0;
	    }
	    else
		actions &= ~DMS_UPDATE;
	}
	else
	{
	    if ( MEcmp(s->seq_end.decval, 
		   stup->dbs_next.decval, DL) == 0 )
	    {
		MEcopy(&s->seq_next.decval, DL,
		       &stup->dbs_next.decval);
		s->seq_cache = 0;
	    }
	    else
		actions &= ~DMS_UPDATE;
	}
    }

    if ( status == E_DB_OK )
    {
	if ( actions & DMS_UPDATE || stup->dbs_flag & DBS_RESTART )
	{
	    /* Must always reset DBS_RESTART, as restart is complete */
	    stup->dbs_flag &= ~DBS_RESTART;

	    if ( r->rcb_logging )
	    {
		/* Tell dmxe, etal, we're doing internal logging */
		xcb->xcb_flags |= XCB_ILOG;

		/* Write the BT if not already written */
		if ( xcb->xcb_flags & XCB_DELAYBT )
		    status = dmxe_writebt(xcb, TRUE, err_code);

		/* Start a mini-txn to prevent rollback of this update */
		if ( status == E_DB_OK )
		    status = dm0l_bm(r, &bm_lsn, err_code);
	    }


	    /* Update the iisequence row */
	    if ( status == E_DB_OK )
		status = dm2r_replace(r, &tid, DM2R_BYPOSITION,
				(char *)stup, (char *)0, err_code);

	    if ( status == E_DB_OK && r->rcb_logging )
		status = dm0l_em(r, &bm_lsn, &lsn, err_code);

	    s->seq_flags &= ~SEQ_UPDATED;

	    if ( status )
		xcb->xcb_state |= XCB_TRANABORT;
	}
    }

    if ( status && *err_code == E_DM0055_NONEXT )
	*err_code = E_DM9D00_IISEQUENCE_NOT_FOUND;

    if ( r )
    {
	/* Unfix iisequence */
	local_status = dm2r_unfix_pages(r, err_code);
	local_status = dm2t_close(r, DM2T_NOPURGE, &local_error);
    }

    return (status);
}

/*{
** Name: dms_exhandler - EX handler to catch decimal overflow
**			 signals.
**
** Description:
**
**		Return EXDECLARE to handle decimal overflow.
**		When this occurs, the sequence is cycled
**		to its minimum value, if cyclable.
**
** Inputs:
**	ex_args				From EX
**
** Outputs:
**	none
**
** Side Effects:
**	    none
**
** History:
**	06-Mar-2003 (jenjo02)
**	    Created.
*/
STATUS
dms_exhandler(
EX_ARGS		*ex_args)
{
    return(EXDECLARE);
}
