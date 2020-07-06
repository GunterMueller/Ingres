/*
** Copyright (c) 2004 Ingres Corporation
*/
#include <compat.h>
#include <cm.h>
#include <rpu.h>

/*
** Name:	validnm.c - is it a valid Ingres name
**
** Description:
**	Defines
**		RPvalid_name - is it a valid Ingres name?
**
** History:
**	16-dec-96 (joea)
**		Created based on validnm.c in replicator library.
**/

/*{
** Name:	RPvalid_name - is it a valid Ingres name?
**
** Description:
**	Verifies that the name argument is a valid Ingres name, that is
**	starts with an alphabetic character and contains no embedded
**	blanks or other illegal characters.
**
** Inputs:
**	name	- character array
**
** Returns:
**	TRUE	- name is valid
**	FALSE	- illegal name error
**/
bool
RPvalid_name(
char		*name)
{
	if (!CMnmstart(name))
		return (FALSE);

	while (CMnext(name) != EOS)
	{
		if (*name == EOS)
			return (TRUE);
		if (!CMnmchar(name))
			return (FALSE);
	}
}
