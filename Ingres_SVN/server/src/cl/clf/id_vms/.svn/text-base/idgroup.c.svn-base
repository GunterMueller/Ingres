# include	<jpidef.h>
# include	<compat.h> 
# include	<gl.h>
# include	<id.h> 
# include	<starlet.h>
# include	<vmstypes.h>

/*{
** Name:  IDgroup
**
** Description:	
**	Set the passed in argument 'gid' to contain the user id
**	of the user who is executing this process.
** Inputs:
**	gid		group identificaton
**
** Outputs:
**	gid		group identification
**
** History:
**		10/5/83 (dd) --	Made for VMS compat lib
**		17-jan-85 (fhc) -- sys$waitfr sys$getjpi for vms 4.0 support
**      16-jul-93 (ed)
**	    added gl.h
**      17-oct-97 (rosga02)
**        added a null item terminator for getjpi() call item list
**        to avoid potential accvio
**	19-jul-2000 (kinte01)
**	   Correct prototype definitions by adding missing includes
**	26-oct-2001 (kinte01)
**	   Clean up compiler warnings
**	29-aug-2003 (abbjo03)
**	    Changes to use VMS headers provided by HP.
*/

VOID
IDgroup(gid)
GID	*gid;
{
	II_VMS_ITEM_LIST_3 itmlst[2] =
		{{4, JPI$_GRP, (PTR)gid, 0},{0, 0, 0, 0}};

	sys$getjpi(0, 0, 0, &itmlst, 0, 0, 0);
	sys$waitfr(0);
}
