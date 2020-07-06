/*
** Copyright (c) 1989, Ingres Corporation
*/

# include <compat.h>
#include    <gl.h>
# include <cm.h>

/**
**
**  Name: CMGLOBS.C -- Universal symbols for CM
**
**  Description:
**	The file contains data which must be known outside the CL.  The
**	front-end CL transfer vector (front!frontcl!sharesys!cl.mar)
**	contains copies of this data.  When the front-end CL shareable image
**	is linked, this module will not be included.
**
**
**  History:
**	4/1 (Mike S) Initial version
**      16-jul-93 (ed)
**	    added gl.h
**	11-apr-2007 (gupsh01)
**	    Added CM_isUTF8.
**/

/*
** the default, compiled in translation.
*/
GLOBALREF u_i2 CM_DefAttrTab[];
GLOBALREF char CM_DefCaseTab[];
GLOBALREF char CM_isUTF8;
GLOBALREF char CM_BytesForUTF8[];

GLOBALDEF {"iicl_gvar$not_ro"} noshare u_i2 *CM_AttrTab = CM_DefAttrTab;
GLOBALDEF {"iicl_gvar$not_ro"} noshare char *CM_CaseTab = CM_DefCaseTab;
GLOBALDEF {"iicl_gvar$not_ro"} noshare char CM_isUTF8 = 0;
GLOBALDEF {"iicl_gvar$not_ro"} noshare char *CM_UTF8Bytes = CM_BytesForUTF8;
