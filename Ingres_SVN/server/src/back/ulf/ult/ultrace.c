/*
**Copyright (c) 2004 Ingres Corporation
*/
#include    <compat.h>
#include    <gl.h>
#include    <sl.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <ulf.h>

/**
**
**  Name: ULTRACE.C - Routines to associate values with trace bits
**
**  Description:
**      This file contains the routines which associate values with 
**      particular trace flags.  For all of these routines, some number
**	of value pairs is kept.  If an attempt is made to add more values
**	than that, an error is returned.  There is no LRU algorithm
**	employed by these routines.
**
**          ult_getval - retrieve (or select) values from the trace vector
**          ult_putval - place a value in the trace vector
**          ult_clrval - remove a value from the trace vector
**
**
**  History:    $Log-for RCS$
**      15-Apr-86 (fred)    
**          Created on Jupiter.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/

/*{
** Name: ult_getval -	get value from the array indicated in the vector
**
** Description:
**      This routine searches the vector give to find the values for a 
**      particular flag.  If they are present, they are returned, and 
**      a status of TRUE is returned.  Otherwise, FALSE is returned. 
**
**
** Inputs:
**      vector                          the trace vector in question
**      bit                             which trace flag we are looking for
**      v1, v2                          pointers to where to put the values
**
** Outputs:
**      *v1, *v2                        filled with the values as above
**	Returns:
**	    bool
**	Exceptions:
**
**
** Side Effects:
**
**
** History:
**	15-Apr-86 (fred)
**          Created on Jupiter.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	17-Feb-2004 (schka24)
**	    Don't return values if the caller v1/v2 pointers are NULL.
**	    It's probably a call buried in a ult-check-macro usage
**	    and the caller doesn't expect values.
**	    (There are a bunch of scf calls this way, easier to fix here.)
*/
bool
ult_getval( ULT_TVECT *vector, i4  bit, i4 *v1, i4 *v2 )
{
    i4		    i;

    for (i = 0; i < vector->ulvao; i++)
    {
	if (vector->ulvp->val_elem[i].elem_flag == bit)
	    break;
    }
    if (i >= vector->ulvao)
    {
	return(0);
    }
    else
    {
	/* Just in case the caller doesn't bother with the result
	** pointers.  (My problems with this were probably a build
	** screw-up, but SCF *does* do ult-check-macro's without any
	** int result pointers, so this is safety.)  (schka24)
	*/
	if (v1 != NULL)
	    *v1 = vector->ulvp->val_elem[i].elem_v1;
	if (v2 != NULL)
	    *v2 = vector->ulvp->val_elem[i].elem_v2;
	return(1);
    }
}

/*{
** Name: ult_putval	- put a value in the list
**
** Description:
**      This routine searches the value list for a free space. 
**      If a free space is found, then the values are placed there. 
**      (If a space is found which currently is holding values for this
**      flag, these values are replaced.) 
**      If there is no space, an FALSE is returned. 
**      Otherwise, the space is claimed and TRUE is returned.
**
** Inputs:
**      vector                          the trace vector on which to operate
**      flag                            which flag to operate on.
**      v1, v2                          the values in question
**
** Outputs:
**      *vect                           is updated.
**	Returns:
**	    DB_STATUS
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	15-Apr-86 (fred)
**          Created on Jupiter.
*/
DB_STATUS
ult_putval( ULT_TVECT *vector, i4  flag, i4 v1, i4 v2 )
{
    i4		i;


    for (i = 0; i < vector->ulvao; i++)
    {
	if ((vector->ulvp->val_elem[i].elem_flag == flag)
	    || (vector->ulvp->val_elem[i].elem_flag == 0))
	    break;
    }
    if (i >= vector->ulvao)
    {
	/* first unset the bit set by the macro, then return false */

	vector->ulbp->btm_elem[flag/ULBPL] &= ~(ULBTM(flag));
	return(E_DB_ERROR);
    }
    else
    {
	vector->ulvp->val_elem[i].elem_v1 = v1;
	vector->ulvp->val_elem[i].elem_v2 = v2;
	vector->ulvp->val_elem[i].elem_flag = flag;
	return(E_DB_OK);
    }
}

/*{
** Name: ult_clrval	- remove values from the list
**
** Description:
**      This routine removes values from the list. 
**      If they are not present, this is a NO-OP.
**
** Inputs:
**      vector                          trace vector
**      flag                            which flag to remove values for
**
** Outputs:
**	Returns:
**	    DB_STATUS
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	15-Apr-86 (fred)
**          Created on Jupiter.
*/
DB_STATUS
ult_clrval( ULT_TVECT *vector, i4  flag )
{
    i4	    i;


    for (i = 0; i < vector->ulvao; i++)
    {
	if (vector->ulvp->val_elem[i].elem_flag == flag)
	    break;
    }
    if (i <= vector->ulvao)
    {
	vector->ulvp->val_elem[i].elem_v1 = 0L;
	vector->ulvp->val_elem[i].elem_v2 = 0L;
    }
    return(E_DB_OK);
}
