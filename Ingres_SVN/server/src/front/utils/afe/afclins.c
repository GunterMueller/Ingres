/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/

# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<adf.h>
# include	<afe.h>
# include       <er.h>

/**
** Name:	afclinstd.c -	Call a function given instance id.
**
** Description:
**	This file contains afe_clinstd.  This file defines:
**
**	afclinstd	calls a funciton given its instance id.
**
** History:
**	Written	2/6/87	(dpr)
**	24-aug-1990	(teresal)
**			Added decimal support: pass db_prec.
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/

i4	adf_func();

/*{
** Name:	afe_clinstd() -	Call a function given instance ID.
**
** Description:
**	Call an ADF function given its instance ID, its operands and its result
**	type.
**
**	The type and length of the result must be known to use
**	this function.  If you don't know the result type and length,
**	call the function afe_fdesc to get this information.
**
** Inputs:
**	cb			A pointer to the current ADF_CB 
**				control block.
**
**	instd			The instance id for the function.
**
**	ops			The operands for the function.
**
**		.afe_opcnt	The number of operands.
**
**		.afe_ops	The array of operands.
**
**	result
**		.db_datatype	The datatype of the result.
**
**		.db_length	The length of the result.
**
** Outputs:
**	result
**		.db_data	Set to the result of the function.
**
**	Returns:
**		OK			If it successfully called the function.
**
**		E_AF6001_WRONG_NUMBER	If the wrong number of arguments 
**					were supplied.
**		returns from:
**			adf_func
** History:
**	Written	2/6/87	(dpr)
**	25-oct-1991     (kirke/gautam)
**		Correct initialization error of 'adf_isescape' in 'fnblk'.
*/
STATUS
afe_clinstd(cb, instd, ops, result)
ADF_CB		*cb;
ADI_FI_ID	instd;
AFE_OPERS	*ops;
DB_DATA_VALUE	*result;
{
	STATUS		rval;
	ADF_FN_BLK	fnblk;

	VOID	adu_prdv();

	/* right number of args? */ 
	if (ops->afe_opcnt > 2 || ops->afe_opcnt < 0)
	{
		return afe_error(cb, E_AF6001_WRONG_NUMBER, 0);
	}

	/*
	**	LOAD THE FUNCTION INSTANCE ID 
	*/
	fnblk.adf_fi_id = instd;	/* this is correct, but ICK */
	fnblk.adf_isescape = FALSE;	/* it's not an ESCAPE ... */

	/*
	**	LOAD THE RESULT DB_DATA_VALUE
	*/
	fnblk.adf_r_dv.db_datatype = result->db_datatype; 
	fnblk.adf_r_dv.db_length = result->db_length; 
	fnblk.adf_r_dv.db_data = result->db_data; 
	fnblk.adf_r_dv.db_prec = result->db_prec; 

	/*
	** should we if-def this out? what's the protocol?
	SIprintf("passed in result, fnblk result dbdvs follow\n");
	adu_prdv(result);
	adu_prdv(&(fnblk.adf_r_dv));
	*/

	/* 	LOAD THE OPERAND DB_DATA_VALUES		*/ 
	if (ops->afe_opcnt >= 1)
	{
		fnblk.adf_1_dv.db_datatype = ops->afe_ops[0]->db_datatype;
		fnblk.adf_1_dv.db_length = ops->afe_ops[0]->db_length;
		fnblk.adf_1_dv.db_data = ops->afe_ops[0]->db_data;
		fnblk.adf_1_dv.db_prec = ops->afe_ops[0]->db_prec;

		/*
		** should we if-def this out? what's the protocol?
		SIprintf("passed in operand 1, fnblk op1 dbdvs follow\n");
		adu_prdv(ops->afe_ops[0]);
		adu_prdv(&(fnblk.adf_1_dv));
		*/

		if (ops->afe_opcnt >= 2)
		{
			fnblk.adf_2_dv.db_datatype = ops->afe_ops[1]->db_datatype;
			fnblk.adf_2_dv.db_length = ops->afe_ops[1]->db_length;
			fnblk.adf_2_dv.db_data = ops->afe_ops[1]->db_data;
			fnblk.adf_2_dv.db_prec = ops->afe_ops[1]->db_prec;
		}
	} /* end if >1 ops */

	/*	CALL THE FUNCTION	*/
	if ((rval = adf_func(cb, &fnblk)) != OK)
	{
		return rval;
	}

	/*
	** should we if-def this out? what's the protocol?
	SIprintf("AFTER CALL, passed in result, fnblk result dbdvs follow\n");
	adu_prdv(result);
	adu_prdv(&(fnblk.adf_r_dv));
	*/

	return OK;
}
