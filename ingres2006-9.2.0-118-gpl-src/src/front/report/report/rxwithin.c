/*
** Copyright (c) 2004 Ingres Corporation
*/

/* static char	Sccsid[] = "@(#)rxwithin.c	30.1	11/14/84"; */

# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<adf.h>
# include	<fmt.h>
# include	 <rtype.h> 
# include	 <rglob.h> 

/*
**   R_X_WITHIN - set up the pointers used in the .WITHIN blocks.
**
**	Parameters:
**		tcmd	first .WI command.
**
**	Returns:
**		none.
**
**	Called by:
**		r_x_tcmd, r_scn_tcmd.
**
**	Side Effects:
**		Sets St_cwithin, St_ccoms, Cact_tcmd.
**
**	Error Messages:
**		none.
**
**	Trace Flags:
**		150, 154.
**
**	History:
**		1/11/82 (ps)	written.
*/



r_x_within(tcmd)
register TCMD	*tcmd;
{
	/* start of routine */



	St_cwithin = tcmd;

	for (St_ccoms = St_cwithin; 
		(St_ccoms!=NULL)&&(St_ccoms->tcmd_code==P_WITHIN);
		(St_ccoms=St_ccoms->tcmd_below))
			;

	r_psh_be(tcmd);

	r_nxt_within((ATTRIB)tcmd->tcmd_val.t_v_long);

	Cact_tcmd = St_ccoms;


	return;
}
