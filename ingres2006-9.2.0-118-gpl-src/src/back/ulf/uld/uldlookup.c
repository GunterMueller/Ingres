/*
** Copyright 2004 Ingres Corporation
*/

#include <compat.h>
#include <gl.h>
#include <st.h>
#include <iicommon.h>




/* File: uldlookup.c - Utility code-to-text lookups
**
** Description:
**	This file contains some utility lookup routines for general back-
**	end use with error messages, debug output, and the like.
**
**	These lookup routines tend to all be pretty much alike, and
**	there's no law against having a generic one.  At the time of this
**	writing, though, I've deemed it preferable to avoid callers
**	needing to import lookup tables cross-facility.
**
** History:
**	25-Jan-2004 (schka24)
**	    Invent as someplace to put query-mode text lookups.
*/

/* Lookup tables defined in ulddata.roc: */

GLOBALREF READONLY INTXLATE Uld_ahd_xlatetab[];
GLOBALREF READONLY INTXLATE Uld_psq_xlatetab[];
GLOBALREF READONLY INTXLATE Uld_struct_xlatetab[];
GLOBALREF READONLY INTXLATE Uld_syscat_xlatetab[];


/*
** Name: uld_psq_modestr - PSQ_XXX query mode lookup
**
** Description:
**	This routine is given a "query mode", defined as a parser PSQ_XXXX
**	symbol (PSQ_CREATE, etc), and returns a short text description.
**	This is mainly for error and debug messages, so speed is not
**	critical - a simple linear seach is good enough.
**
** Inputs:
**	qmode		The query mode
**
** Outputs:
**	Returns char * pointer to short description (pointer to an
**	empty string if no description found).
**
** History:
**	25-Jan-2004 (schka24)
**	    Invent to make generalizing some parser productions and their
**	    error messages a bit easier and less tedious.
*/

char *
uld_psq_modestr(i4 qmode)
{
    READONLY INTXLATE *xlatePtr;	/* Translation table pointer */

    xlatePtr = &Uld_psq_xlatetab[0];
    while (xlatePtr->string != NULL)
    {
	if (xlatePtr->code == qmode)
	    return (xlatePtr->string);
	++ xlatePtr;
    }
    return ("");
} /* uld_psq_modestr */

/*
** Name: uld_qef_actstr - QEA_XXX Action header type lookup
**
** Description:
**	This routine is given an action header type, defined as a QEA_XXX
**	symbol (QEA_GET, etc), and returns a short text description.
**	This is mainly for error and debug messages, so speed is not
**	critical - a simple linear seach is good enough.
**
** Inputs:
**	ahd_type		The action header type
**
** Outputs:
**	Returns char * pointer to short description (pointer to an
**	empty string if no description found).
**
** History:
**	5-Feb-2004 (schka24)
**	    OP150 trace point not entirely up-to-date, fix this way.
*/

char *
uld_qef_actstr(i4 ahd_type)
{
    READONLY INTXLATE *xlatePtr;	/* Translation table pointer */

    xlatePtr = &Uld_ahd_xlatetab[0];
    while (xlatePtr->string != NULL)
    {
	if (xlatePtr->code == ahd_type)
	    return (xlatePtr->string);
	++ xlatePtr;
    }
    return ("");
} /* uld_qef_actstr */


/*
** Name: uld_struct_xlate - Translate a structure name
**
** Description:
**	This routine takes a storage structure name (heap, hash, etc)
**	and returns the corresponding DB_xxx_STORE code.
**
**	The compressed versions (e.g. cbtree) are not recognized, because
**	the compression flag is separate from the STORE codes.  It's better
**	if the caller strips off any leading letter 'C' and sets a
**	compression flag appropriate to the context.
**
** Inputs:
**	strname		The storage structure name
**
** Outputs:
**	The DB_xxx_STORE code
**	Zero is returned if no match (as it happens, zero is not
**	a valid storage structure code)
**
** History:
**	23-Nov-2005 (kschendel)
**	    Invent to reduce duplication.
*/

int
uld_struct_xlate(char *strname)
{
    READONLY INTXLATE *xlatePtr;	/* Translation table pointer */

    xlatePtr = &Uld_struct_xlatetab[0];
    while (xlatePtr->string != NULL)
    {
	if (STcasecmp(strname, xlatePtr->string) == 0)
	    return (xlatePtr->code);
	++ xlatePtr;
    }
    return (0);
} /* uld_struct_xlate */

/*
** Name: uld_syscat_namestr - System catalog name lookup
**
** Description:
**	This routine is given a system catalog table number, either
**	reltid or reltidx as appropriate, and returns the catalog name.
**	This is mainly for error and debug messages, so speed is not
**	critical - a simple linear seach is good enough.
**
** Inputs:
**	table_id		The table ID number (baseid or indexid)
**
** Outputs:
**	Returns char * pointer to catalog name (pointer to an
**	empty string if no match found).
**
** History:
**	30-Dec-2004 (schka24)
*	    Fix up another tedious switch statement while I'm in there.
*/

char *
uld_syscat_namestr(i4 table_id)
{
    READONLY INTXLATE *xlatePtr;	/* Translation table pointer */

    xlatePtr = &Uld_syscat_xlatetab[0];
    while (xlatePtr->string != NULL)
    {
	if (xlatePtr->code == table_id)
	    return (xlatePtr->string);
	++ xlatePtr;
    }
    return ("");
} /* uld_syscat_namestr */
