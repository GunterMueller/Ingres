/*
** Copyright (c) 2004 Ingres Corporation
*/

# include <compat.h>
# include <st.h>		/* 6-x_PC_80x86 */
# include <tm.h>
# include <si.h>
# include <equel.h>
# include <eqstmt.h>

/**
+* Filename:	eqqryid.c
** Purpose:	Defines EQUEL query-id-handling routines.
**
** Defines:
-*	edb_unique( qry_id, qry_name )	- Fill in a query id
** Notes:
**
** History:
**	15-aug-86	- Written. (mrw)
**	28-aug-86	- (barbara) Fixed bug #7253 whereby applications
**			  compiled with WORD directives fail because of
**			  size of ids.  Mask was masking 16 instead of 15 bits.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/

/*{
+* Procedure:	edb_unique
** Purpose:	Assign a unique identifier to a query.
** Inputs:
**	qry_id	 - EDB_QUERY_ID *	- Pointer to the query id to fill.
**	qry_name - char *		- The "user" name of the query.
** Outputs:
**	Fills the edb_qry_id and edb_qry_name arrays of the query id.
**	Returns:
**		Nothing.
**	Errors:
-*		None.
** Description:
**	Call TMnow to get the time; spin until it's different from the
**	last time.  Mix in a sequence number to the low-order bits of the
**	millisecond count.  Ensure that we won't collide with an earlier
**	id.  Mix in the sum of the chars of the name with the highest-order
**	byte of the (2-byte) millisecond count.  Mask both words to 15-bits
**	(for portability to the PC).
**
**	If the EQ_QRYTST flag is on in the EQ structure, then generate
**	repeatable names (don't use the time).
** Side Effects:
**	Fills the query id name and numbers.
*/

void
edb_unique( qry_id, qry_name )
EDB_QUERY_ID
	*qry_id;
char	*qry_name;
{
	static SYSTIME	edb_saved = {0, 0};
	static i4	edb_sequence = 1;
	SYSTIME		edb_curtime;
	i4		temp;
	i4		sum;

	STlcopy( qry_name, qry_id->edb_qry_name, EDB_QRY_MAX );

	if (eq->eq_flags & EQ_QRYTST)
	{
	    edb_curtime.TM_secs = 0;
	    edb_curtime.TM_msecs = 0;
	} else
	{
	    do {
		TMnow( &edb_curtime );
	    } while (edb_curtime.TM_secs == edb_saved.TM_secs
		    && edb_curtime.TM_msecs <= edb_saved.TM_msecs+edb_sequence);
	}

	temp = (edb_curtime.TM_msecs & 0xFFFF) - edb_sequence++;
	edb_sequence &= 0x3FF;		/* to avoid long while loops */
	if (edb_sequence == 0)
	    edb_sequence++;
	sum = 0;
	while (*qry_name)
	    sum += *qry_name++ & I1MASK;
	temp ^= (sum << 10);		/* 1000 ms <= 10 bits */

	qry_id->edb_qry_id[0] = (edb_curtime.TM_secs + edb_sequence) & 0x7FFF;
	qry_id->edb_qry_id[1] = temp & 0x7FFF;

	edb_saved.TM_secs = edb_curtime.TM_secs;
	edb_saved.TM_msecs = edb_curtime.TM_msecs;
}
