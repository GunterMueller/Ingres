/*
**    Copyright (c) 1985, 2000 Ingres Corporation
*/

#include	<compat.h>
#include	<si.h>
#include	<gl.h>
#include	"cilocal.h"
#include	<ci.h>
#include	<st.h>
#include	<descrip.h>
#include	<jpidef.h>
#include	<ssdef.h>
#include	<lib$routines.h>
#include	<starlet.h>
/**
** Name:    CIvms - Contains all VMS system dependent routines.
**
** Description:
**      This file is by definition applicable only to VMS.
**  This file defines:
**
**      CIgtsid		get the system's SID number.
**      CIgtmodelnum	get the system's cpu model number.
**	CIiscluster	returns TRUE if machine in cluster, else FALSE.
**	CImk_logical	assign an equivalence string to a logical name
**			at the specified table level.
**	CIrm_logical	delete a logical name from specified table.
**
** History:
**	10-Sep-85 (ericj) - written.
**      16-jul-93 (ed)
**	    added gl.h
**	19-jul-2000 (kinte01)
**	    Correct prototype definitions by adding missing includes
**	01-dec-2000	(kinte01)
**		Bug 103393 - removed nat, longnat, u_nat, & u_longnat
**		from VMS CL as the use is no longer allowed
**/

/* # defines */
/* Define SYI item list codes - should be put in syidef.h */
# define SYI$_SID		4097
# define SYI$_CLUSTER_MEMBER	4303
# define SYI$_CPU		8192
/* Define LNM item list codes - should be put in lnmdef.h */
# define LNM$_STRING		2

/* typedefs */
/* forward references */
/* externs */
/* statics */


/*{
** Name:	CIgtsid		- get system's id number
**
** Description:
**	    This routine gets a VMS system id number via the system
**	service sys$getsyi.
**
** Outputs:
**	mach_id		pointer to character array to return the serial
**			number in.  The array should have MAX_SER_NUM
**			bytes allocated to it.
**	Returns:
**		OK	if serial number was successfully converted.
**		CI_BADERROR	if system service call failed, which is
**			an unexpected error.
**
** History:
**	10-03-85 (ericj) -- written.
*/
STATUS
CIgtsid(mach_id)
u_i4	*mach_id;
{
	i4	status;
	u_i4	sid;

	struct
	{
		i2	buf_len;
		i2	syi_code;
		PTR	buf_addr;
		PTR	ret_addr;
	} itmlst[]
	=
	{
		{4, SYI$_SID, (PTR)&sid, 0},
		{0, 0, 0, 0}
	};

	if ((status = sys$getsyi(0, 0, 0, itmlst, 0, 0, 0)) != SS$_NORMAL)
	{
		status = CI_BADERROR;
	}
	else {
		status = OK;
		*mach_id = sid;
	}

	return(status);	
}


/*{
** Name:	CIgtmodelnum	- get the VMS system's model number.
**
** Description:
**	    This routine gets a VMS system's model number via a
**	system service call to sys$getsyi.
**
** Outputs:
**	model_id	pointer to an i4 to return model number code in.
**	Returns:
**		OK	if model number correctly returned.
**		CI_BADERROR	if unexpected error occurred on call to
**				system service routine.
**
** History:
**	10-03-85 (ericj) -- written.
*/
STATUS
CIgtmodelnum(model_id)
i4	*model_id;
{
	i4	status;

	struct
	{
		i2	buf_len;
		i2	syi_code;
		PTR	buf_addr;
		PTR	ret_addr;
	} itmlst[]
	=
	{
		{4, SYI$_CPU, (PTR)model_id, 0},
		{0, 0, 0, 0}
	};

	if ((status = sys$getsyi(0, 0, 0, itmlst, 0, 0, 0)) != SS$_NORMAL)
		status = CI_BADERROR;
	else
		status = OK;

	return(status);	
}


/*{
** Name:	CIiscluster	- Determine if machine is part of a cluster.
**
** Description:
**	    Routine which returns sets output parameter to TRUE if machine on 
**	which procedure is invoked is part of a cluster, else FALSE.
**
** Outputs:
**	is_cluster		pointer to a i4 which is set to TRUE if this
**				machine is part of a cluster, else FALSE.
**	Returns:
**		OK		if call was successful.
**		CIBADERROR	otherwise.
**
** History:
**	8-Jan-86
*/
CIiscluster(is_cluster)
i4	*is_cluster;
{
	i4	status;
	char	value;

	struct	{
		i2	buf_len;
		i2	syi_code;
		PTR	buf_addr;
		PTR	ret_addr;
	}	itmlst[]
	=
	{
		{1, SYI$_CLUSTER_MEMBER, &value, 0},
		{0, 0, 0, 0}
	};

	if ((status = sys$getsyi(0, 0, 0, itmlst, 0, 0, 0)) != SS$_NORMAL)
	{
		return(CI_BADERROR);
	}
	else {
		if (value & 01)
			*is_cluster = TRUE;
		else
			*is_cluster = FALSE;
		return(OK);
	}
}



/*{
** Name:	CImk_logical	- make a logical name in environment.
**
** Description:
**	    This routine makes a logical name in the logical name
**	table given.  This assignment is done at the supervisor level as
**	if DEFINE were called from DCL.  This routine must be called by
**	an image associated with a CLI for it to work properly.  This
**	routine is only intended for local support, at RTI, of the
**	CI module.
**
** Inputs:
**	log_str		logical name to be made.
**	equiv_str	logical name's equivalence string.
**	log_table	logical table where logical name is to be
**		made.
**
**
** Side Effects:
**	    Makes and assigns an equivlence string to the logical name
**	given in the logical table given.  Note, if the table is at the
**	job level,  you may have to increase your job table byte limit
**	quota, jtquota, for this to work correctly.
**
** History:
**	10-06-85 (ericj) -- written.
*/
VOID
CImk_logical(log_str, equiv_str, tab_str)
PTR	log_str;
PTR	equiv_str;
PTR	tab_str;
{
	struct	dsc$descriptor_s	log_name;
	struct	dsc$descriptor_s	tab_name;
	i4	status;

	struct
	{
		i2	buf_len;
		i2	itm_code;
		PTR	buf_address;
		i4	*ret_len;
	} itmlst[]
	=
	{
		{STlength(equiv_str), LNM$_STRING, equiv_str, 0},
		{0, 0, 0, 0}
	};

	log_name.dsc$w_length = STlength(log_str);
	log_name.dsc$b_class = DSC$K_CLASS_S;
	log_name.dsc$b_dtype = DSC$K_DTYPE_T;
	log_name.dsc$a_pointer = log_str;

	tab_name.dsc$w_length = STlength(tab_str);
	tab_name.dsc$b_class = DSC$K_CLASS_S;
	tab_name.dsc$b_dtype = DSC$K_DTYPE_T;
	tab_name.dsc$a_pointer = tab_str;

	status = lib$set_logical(&log_name, 0, &tab_name, 0, itmlst);
	if (status != SS$_NORMAL && status != SS$_SUPERSEDE)
		SIprintf("The status returned is: %d\n", status);
	return;
}




/*{
** Name:	CIrm_logical	- remove a logical name in environment.
**
** Description:
**	    This routine removes the given logical name from the given
**	logical name table.  See CImk_logical for relevant restrictions.
**
** History:
**	10-06-85 (ericj) -- written.
*/
VOID
CIrm_logical(log_str, tab_str)
PTR	log_str;
PTR	tab_str;
{
	struct	dsc$descriptor_s	log_name;
	struct	dsc$descriptor_s	tab_name;
	i4	status;
	i4	retlen;

	log_name.dsc$w_length = STlength(log_str);
	log_name.dsc$b_class = DSC$K_CLASS_S;
	log_name.dsc$b_dtype = DSC$K_DTYPE_T;
	log_name.dsc$a_pointer = log_str;

	tab_name.dsc$w_length = STlength(tab_str);
	tab_name.dsc$b_class = DSC$K_CLASS_S;
	tab_name.dsc$b_dtype = DSC$K_DTYPE_T;
	tab_name.dsc$a_pointer = tab_str;

	status = lib$delete_logical(&log_name, &tab_name);
	if (status != SS$_NORMAL && status != SS$_NOLOGNAM)
		SIprintf("WARNING: sys$dellnm returned status: %d\n", status);
	return;
}
