/*
** Copyright (C) 1987, 2004 Ingres Corporation
*/

# include <compat.h>
# include <sys\types.h>
# include <sp.h>
# include <pc.h>
# include <cs.h>
# include <mo.h>
# include <pc.h>
# include <csinternal.h>
# include "cslocal.h"

# include "csmgmt.h"

/**
** Name:	csmocnd.c	- CS MO condition management
**
** Description:
**	Defines MO classes for conditions, and functions to
**	be used to attach and detach them.
**
**	CS_cnd_index	- MO index for CS_CONDITIONS
**
** History:
**	29-Oct-1992 (daveb)
**	    documented
**	18-Nov-1992 (daveb)
**	    correct "cs.cnd.cnd_index" to "cs.cnd_index"
**	13-Jan-1993 (daveb)
**	    Rename MO objects to include a .unix component.
**      12-Dec-95 (fanra01)
**          Extracting data for DLLs on windows NT
**      06-aug-1999 (mcgem01)
**          Replace nat and longnat with i4.
**	22-jul-2004 (somsa01)
**	    Removed unnecessary include of erglf.h.
**/


/*{
** Name:	CS_cnd_index	- MO index for CS_CONDITIONS
**
** Description:
**	Standard MO index function for CS_CONDITIONS.
**
** Re-entrancy:
**	yes.
**
** Inputs:
**	msg		MO_GET or MO_GETNEXT
**	cdata		ignored
**	linstance	length of instance buffer
**	instance	the instance in question
**
** Outputs:
**	instance	on GETNEXT, stuffed with found instance
**	instdata	stuffed with a pointer to the CS_CONDITION.
**
** Returns:
**	OK or various MO index function returns.
**
** History:
**	28-Oct-1992 (daveb)
**	    documented
*/
STATUS
CS_cnd_index(i4  msg,
	     PTR cdata,
	     i4  linstance,
	     char *instance,
	     PTR * instdata)
{
	STATUS          stat = OK;
	PTR             ptr;

	switch (msg) {
	case MO_GET:

		if (OK == (stat = CS_get_block(instance,
					       Cs_srv_block.cs_cnd_ptree,
					       &ptr)))
			*instdata = ptr;
		break;

	case MO_GETNEXT:

		if (OK == (stat = CS_nxt_block(instance,
					       Cs_srv_block.cs_cnd_ptree,
					       &ptr))) {
			*instdata = ptr;
			stat = MOulongout(MO_INSTANCE_TRUNCATED,
					  (u_i4) ptr,
					  linstance,
					  instance);
		}
		break;

	default:

		stat = MO_BAD_MSG;
		break;
	}
	return (stat);
}

/* end of csmocnd.c */
