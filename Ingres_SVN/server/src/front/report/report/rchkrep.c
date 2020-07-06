# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<me.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<adf.h>
# include	<fmt.h>
# include	<rtype.h> 
# include	<rglob.h> 
# include	<oodefine.h>
/*{
**   R_CHK_REP - check for the existence of a report in the
**	database by checking the REPORTS table.
**
**	Parameters:
**		ren		REN structure containing name,owner
**				and to return the other info.
**
**	Returns:
**		NULL if none found.  If found, return the address
**		of ren, with fields filled in.
**
**	Called by:
**		cr_dmp_rep, s_del_old, r_env_set.
**
**	Side Effects:
**		none.
**
**	History:
**		9/12/82 (ps)	written.
**		03-feb-1988 (rdesmond)
**			changed call to OOIdCheck to look for OC_REPORT only.
**			removed initialization of En_rid;
**		18-feb-1988 (rdesmond)
**			Removed range statement.
**		26-aug-1988 (danielt)
**			SQLized database access.
**		09-nov-88 (marian)
**			Modified column names for extended catalogs to allow 
**			them to work with gateways.
**		09-oct-90 (sylviap)
**			Checks if OOp returns NULL. Done to avoid a core dump. 
**		13-sep-1993 (rdrane)
**			Add retrieval of report level column, and propagate
**			it to the REN structure.  Since we force FE catalog
**			upgrade, there's no need to conditionalize the query.
**			Also, fix-up the REN screening statements and cast
**			the return type to (REN *) in all cases.
**		27-oct-1993 (rdrane)
**			Before going to all the trouble of looking up the
**			alleged report name in the OO catalogs, check to make
**			sure it's a regular identifier (we don't support
**			delimited FE object names).  This avoids the problem
**			of the OO catalog lookup failing because the name has
**			embedded single quotes (SQL syntax error).
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
REN *
r_chk_rep(ren)
REN	*ren;
{
  char *_reptype;
  i4 _acount;
  i4 _qcount;
  i4 _scount;
  i4 _bcount;
  i4 _replevel;
  bool r_found;
  i4 rid;
	register OO_OBJECT	*obj;
	i4		id_type;
	OOID			class;
	/* start of routine */
	if ((ren == (REN *)NULL) || (ren->ren_report == NULL))
	{
		return((REN *)NULL);
	}
	/*
	** Disallow all alleged report names which are not valid FE names.
	** This quickly eliminates delimited identifiers, and avoids the
	** query problems associated with strings containing embedded single
	** quotes (which should go away once OOLIB is fixed ...).
	*/
	if  (FEchkname(ren->ren_report) == FAIL)
	{
		return((REN *)NULL);
	}
	class = OC_REPORT;
	if ( IIOOidCheck(&class, &En_rid, ren->ren_report, (char *)NULL) != OK )
	{
		return((REN *)NULL);
	}
	if ((obj = OOp(En_rid)) == (OO_OBJECT *)NULL)
	{
		return((REN *)NULL);
	}
	ren->ren_id = En_rid;
	ren->ren_owner = obj->owner;
	ren->ren_created = obj->create_date;
	ren->ren_modified = obj->alter_date;
	ren->ren_shortrem = obj->short_remark;
	ren->ren_longrem = obj->long_remark;
	_reptype = (char *) MEreqmem(0,2,TRUE,NULL);
	r_found = FALSE;
	rid = En_rid;
/* # line 118 "rchkrep.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select reptype, repacount, repqcount, repscount, repbcount, replevel \
from ii_reports where object_id=");
    IIputdomio((short *)0,1,30,4,&rid);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,0,_reptype);
      IIgetdomio((short *)0,1,30,4,&_acount);
      IIgetdomio((short *)0,1,30,4,&_qcount);
      IIgetdomio((short *)0,1,30,4,&_scount);
      IIgetdomio((short *)0,1,30,4,&_bcount);
      IIgetdomio((short *)0,1,30,4,&_replevel);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 134 "rchkrep.sc" */	/* host code */
		ren->ren_type = _reptype;
		ren->ren_acount = _acount;
		ren->ren_qcount = _qcount;
		ren->ren_scount = _scount;
		ren->ren_bcount = _bcount;
		ren->ren_level	= _replevel;
		r_found = TRUE;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	if  (r_found)
	{
		return(ren);
	}
	return((REN *)NULL);
}
