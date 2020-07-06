# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<er.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<stype.h>
# include	<sglob.h>
# include	<si.h>
# include	"ersr.h"
/*{
** Name:	s_del_old() -	Delete Old Report Specification.
**
** Description:
**   S_DEL_OLD - get rid of the old version of the report in the database,
**	if one exists.
**
** Parameters:
**	none.
**
** Returns:
**	none.
**
** Side Effects:
**	May delete rows from the system relations.
**
** Called by:
**	s_main, rFget.
**
** Error Messages:
**	none.
**
** Trace Flags:
**	11.0, 11.6.
**
** History:
**	6/4/81 (ps)	written.
**	12/22/81 (ps)	modified for two table version.
**	4/4/82 (ps)	add RBF references.
**	9/12/82 (ps)	add call to r_chk_rep.
**	8/22/83 (gac)	add trace for deleting and adding reports.
**	2/22/84 (gac)	added multi-statement transactions.
**	10/9/86 (sandyd)added support for distributed database.
**	4/28/87 (grant) removed ddb support kludge.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**	09-jul-90 (cmr) fix for bug #30813
**		Moved CatWriteOn/Off calls to main instead of scattered in
**		other routines.  RBF and sreport were not detecting DBMS
**		errors or deadlock (CatWriteOff() resets the errorno).
*/
VOID
s_del_old()
{
  i4 rid;
	register REN	*ren;			/* ptr to REN structure */
	bool		r_found;		/* true if report found */
	char		rtype[2];		/* hold type of report */
	bool	deadlock();
	/* start of routine */
	/* set up range statements */
	/* go through list of specified reports */
	for (ren = Ptr_ren_top ; ren != NULL ; ren = ren->ren_below)
	{
		/* see if old report exists */
		r_found = (OOhash(ren->ren_id, NULL) != 0);
		/* write out appropriate message to user */
		if (!St_silent)
		{
			if (En_program != PRG_RBF)
			{
				SIfprintf(stdout,
				    r_found ?
ERget(S_SR0006_Replacing_report):
ERget(S_SR0007_Adding_report),
				    ren->ren_report, En_database);
				SIflush(stdout);
			}
		}
		if (r_found)
		{ /* old report exists */
			/* delete old one */
			rid = ren->ren_id;
/* # line 93 "sdelold.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_reports where object_id=");
    IIputdomio((short *)0,1,30,4,&rid);
    IIsyncup((char *)0,0);
  }
/* # line 94 "sdelold.sc" */	/* host code */
			if (deadlock())
				break;
/* # line 97 "sdelold.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_rcommands where object_id=");
    IIputdomio((short *)0,1,30,4,&rid);
    IIsyncup((char *)0,0);
  }
/* # line 98 "sdelold.sc" */	/* host code */
			if (deadlock())
				break;
		}
	}	/* end for */
}
