# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/* 
**	Copyright (c) 2004 Ingres Corporation  
*/
#include	<compat.h> 
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h> 
#include	<st.h> 
#include	<cv.h> 
/**
** Name:	vqcurfld -	return the index of the current field
**
** Description:
**	Return the index of the current field while in the  visual query.
**	also sets global which contains the field name
**
**	This file defines:
**		IIVQvdlVqDisplayLoop - run the vq display loop
**		IIVQptPrimaryTab - find the primary table in a section
**		IIVQcvqCopyVQ - find the primary table in a section
**
** History:
**	12/27/89  (tom) - extracted from vqdloop.qsc
**      29-sep-96 (mcgem01)
**              Global data moved to vqdata.c
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
  GLOBALREF char IIVQcfnCurFldName[20];
  GLOBALREF char *IIVQtfname_fmt;
  GLOBALREF char *IIVQform;
/*{
** Name:	IIVQcfCurFld	- return the index of the current table 
**
** Description:
**	Inquire of the form system as to what the current field is 
**	and return it's index.. based on the name of the field.
**
** Inputs:
**	none
**
** Outputs:
**	Returns:
**		i4   - index of the current field
**
**	Exceptions:
** Side Effects:
**
** History:
**	04/20/89 (tom) - created
*/
i4
IIVQcfCurFld()
{
	i4 curfld;
	char save;
/* # line 68 "vqcurfld.qsc" */	/* inquire_frs */
  {
    if (IIiqset(2,0,IIVQform,(char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,32,19,IIVQcfnCurFldName,19,(char *)0,0);
    } /* IIiqset */
  }
/* # line 71 "vqcurfld.qsc" */	/* host code */
	/* translate the name into an index, start at the 2nd character
	   since the first character is an underscore, and make sure
	   to null terminate (see the format string)*/
	save = IIVQcfnCurFldName[4];
	IIVQcfnCurFldName[4] = EOS;
	_VOID_ CVan(IIVQcfnCurFldName + 1, &curfld);
	IIVQcfnCurFldName[4] = save;
	return (curfld);
}
