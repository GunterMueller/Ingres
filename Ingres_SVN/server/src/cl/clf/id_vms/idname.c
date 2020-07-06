# include	<jpidef.h>
# include	<compat.h> 
# include	<gl.h>
# include	<st.h> 
# include	<cv.h>
# include	<starlet.h>
# include	<vmstypes.h>

/* static char Sccsid[]= "@(#)IDname.c	1.1	4/14/83";	*/


/*{
** Name:  IDname
**
** Description:	
**	set the passed in argument 'name' to contain the name
**	of the user who is executing this process.
**
** Inputs:
**	Name
**
** Outputs:
**	Returns:
**		The name NULL ended in lowercase.
**
** History:
**		9-1-83	Made for VMS compat lib (dd)
**		6/14/84 (kooi) -- change JPI$ to $JPIDEF to follow change in
**			jpidef.h
**		17-jan-85 (fhc) -- sys$waitfr sys$getjpi for vms 4.0 support
**		19-jun-90 (Mike S) -- trim trailing spaces
**      16-jul-93 (ed)
**	    added gl.h
**      17-oct-97 (rosga02)
**        added a null item terminator for getjpi() call item list
**        to avoid potential accvio
**	19-jul-2000 (kinte01)
**	   Correct prototype definitions by adding missing includes
**	29-aug-2003 (abbjo03)
**	    Changes to use VMS headers provided by HP.
-*/
VOID
IDname(name)
char	**name;
{
	i2	length;
	i4	iosb[2];
	II_VMS_ITEM_LIST_3	itmlst[2] =
		{{12, JPI$_USERNAME, *name, &length},{0,0,0,0}};

	sys$getjpiw(0, 0, 0, &itmlst, iosb, 0, 0);
	(*name)[length] = NULL;
	STtrmwhite(*name);
	CVlower(*name);	
}
