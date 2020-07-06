/*
**Copyright (c) 2004 Ingres Corporation
*/

#include <compat.h>
#include <gl.h>
#include <cm.h>
#include <cs.h>

/*
** Name:	cmdata.c
**
** Description:	Global data for cm facility.
**
LIBRARY = IMPCOMPATLIBDATA
**
** History:
**      12-Sep-95 (fanra01)
**          Created.
**	23-sep-1996 (canor01)
**	    Updated.
**	11-apr-2007 (gupsh01)
**	    Added Global to indicate if CM 
**	    refers to UTF8 characters.
**	22-may-2007 (gupsh01)
**	    Added CM_utf8casetab and CM_utf8attrtab.
*/

extern u_i2 CM_DefAttrTab[];
extern char CM_DefCaseTab[];
extern i4   CM_BytesForUTF8[];
extern CM_UTF8ATTR CM_utf8attrtab[];
extern CM_UTF8CASE CM_utf8casetab[];

GLOBALDEF u_i2 *CM_AttrTab = CM_DefAttrTab;
GLOBALDEF char *CM_CaseTab = CM_DefCaseTab;
GLOBALDEF char CM_isUTF8 = 0;
GLOBALDEF i4 *CM_UTF8Bytes = CM_BytesForUTF8;
GLOBALDEF CM_UTF8ATTR *CM_UTF8AttrTab = CM_utf8attrtab;
GLOBALDEF CM_UTF8CASE *CM_UTF8CaseTab = CM_utf8casetab;
