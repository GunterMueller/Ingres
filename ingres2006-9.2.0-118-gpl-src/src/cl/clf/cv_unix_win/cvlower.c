/*
**Copyright (c) 2004 Ingres Corporation
*/

# include	<compat.h>
# include	<gl.h>
# include	<cm.h>


/**
** Name: CVLOWER.C - Make string lowercase
**
** Description:
**      This file contains the following external routines:
**    
**      CVlower()          -  Make string lowercase
**
** History:
 * Revision 1.1  90/03/09  09:14:24  source
 * Initialized as new code.
 * 
 * Revision 1.2  90/02/12  09:40:07  source
 * sc
 * 
 * Revision 1.1  89/05/26  15:45:40  source
 * sc
 * 
 * Revision 1.1  89/05/11  01:05:25  source
 * sc
 * 
 * Revision 1.1  89/01/13  17:11:22  source
 * This is the new stuff.  We go dancing in.
 * 
 * Revision 1.2  88/12/13  11:51:23  jrb
 * Changed str++ to CMnext(str) for doublebyte character set support
 * 
 * Revision 1.1  88/08/05  13:28:48  roger
 * UNIX 6.0 Compatibility Library as of 12:23 p.m. August 5, 1988.
 * 
**      Revision 1.3  87/11/17  16:54:59  mikem
**      changed to not use CH anymore
**      
**      Revision 1.2  87/11/10  12:37:34  mikem
**      Initial integration of 6.0 changes into 50.04 cl to create 
**      6.0 baseline cl
**      
**      Revision 1.3  87/07/27  11:23:42  mikem
**      Updated to conform to jupiter coding standards.
**      
**      20-jul-87 (mmm)
**          Updated to meet jupiter coding standards.      
**	15-jul-93 (ed)
**	    adding <gl.h> after <compat.h>
**	20-Jul-2004 (lakvi01)
**		SIR #112703, cleaned-up warnings.
**      26-Jul-2004 (lakvi01)
**          Backed-out the above change to keep the open-source stable.
**          Will be revisited and submitted at a later date. 
**/

/* # defines */
/* typedefs */
/* forward references */
/* externs */
/* statics */


/*{
** Name: CVLOWER.C - Make string lowercase
**
** Description:
**    Make string lowercase
**
**    Convert all chars in string to lowercase.
**
** Inputs:
**	string				    String to lowercase.
**
** Output:
**	string				    String is lowercased in place.
**                                          E_DM0000_OK                
**
**      Returns:
**	    none. (VOID)
**
** History:
**      20-jul-87 (mmm)
**          Updated to meet jupiter coding standards.
**	13-dec-88 (jrb)
**	    Changed str++ to CMnext(str) for doublebyte character support.
*/
VOID
CVlower(string)
char	*string;
{
	register char	*sp = string;


	if (sp != NULL)
	{
		/* make sure you got at least a ptr */

		while (*sp != NULL)
		{
			/*
				split assignment and increment as
					order of evaluation is not
					guaranteed by C.
			*/

			CMtolower(sp, sp);

			CMnext(sp);
		}
	}

	return;
}
