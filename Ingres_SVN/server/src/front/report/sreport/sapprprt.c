# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<st.h>
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<stype.h> 
# include	<sglob.h> 
# include	<oodefine.h>
# include	<oosymbol.h>
/*
**   S_APP_REPORTS - append the rows in the REPORTS table from the reports
**	added in this run.
**
**	Parameters:
**		none.
**
**	Returns:
**		none.
**
**	Called by:
**		s_main, s_rep_set, rFsave.
**
**	Side Effects:
**		none.
**
**	Error Messages:
**		none.
**
**	Trace Flags:
**		11.0, 11.4.
**
**	History:
**		6/1/81 (ps)	written.
**		12/22/81 (ps)	modified for two table version.
**		4/4/82 (ps)	added RBF references.
**		2/13/84	(gac)	added multi-statement transactions.
**		09-nov-88 (marian)
**			Modified column names for extended catalogs to allow 
**			them to work with gateways.
**		15-feb-90 (sylviap)
**			If the rport type is 's' (rather than 'f' for a copyform
**			report) class is set to OC_RWREP (1502). US #402.
**	09-jul-90 (cmr) fix for bug #30813
**		Moved CatWriteOn/Off calls to main instead of scattered in
**		other routines.  RBF and sreport were not detecting DBMS
**		errors or deadlock (CatWriteOff() resets the errorno).
**	26-aug-1992 (rdrane)
**		Fix-up external declarations - they've been moved to hdr files.
**	3-feb-1993 (rdrane)
**		Changed _flushAll to ii_flushAll (keep MS_DOS happy and
**		and unconfused).
**	13-sep-1993 (rdrane)
**		Add insertion of report level column, as reflected in the
**		REN structure.  Since we force FE catalog upgrade, there's
**		no need to conditionalize the query.
**	10/15/93 (kchin)
**		Cast argument to PTR when calling IIOOidObject(), this
**		is due to the change of datatype of variable parameters
**		in IIOOidObject() routine itself.  The change is made in
**		s_app_reports().
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
s_app_reports()
{
	/* internal declarations */
	register REN	*ren;			/* fast ptr to REN structures */
  i4 _rid;
  char *_reptype;
  i4 _repacount;
  i4 _repqcount;
  i4 _repscount;
  i4 _repbcount;
  i4 _replevel;
	OOID	class;
	/* start of routine */
	for (ren=Ptr_ren_top; ren!=NULL; ren=ren->ren_below)
	{
		_rid = ren->ren_id;
		_reptype = ren->ren_type;
		_repacount = ren->ren_acount;
		_repqcount = ren->ren_qcount;
		_repscount = ren->ren_scount;
		_repbcount = ren->ren_bcount;
		_replevel = ren->ren_level;
/* # line 98 "sapprprt.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_reports(object_id, reptype, repacount, repqcount, reps\
count, repbcount, replevel)values(");
    IIputdomio((short *)0,1,30,4,&_rid);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,_reptype);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&_repacount);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&_repqcount);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&_repscount);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&_repbcount);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&_replevel);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 114 "sapprprt.sc" */	/* host code */
		if (deadlock())
		{
			break;
		}
		/* actually save object */
		if (OOhash(ren->ren_id, (OO_OBJECT *)NULL) == 0)
		{ /* create a new report */
			class = ( STequal(ren->ren_type, ERx("s")) )
					? OC_RWREP : OC_RBFREP;
			_VOID_ IIOOidObject(class, &(ren->ren_id),
						ren->ren_report, (PTR)0,
						ren->ren_owner, (PTR)1,
						ren->ren_shortrem,
						ren->ren_created,
						ren->ren_modified,
						ren->ren_longrem
			);
		}
		else
		{ /* modify an old report */
			register OO_OBJECT	*obj;
			obj = OOp(ren->ren_id);
			/* set report class, US #402 */
			obj->class = ( STequal(ren->ren_type, ERx("s")) )
					? OC_RWREP : OC_RBFREP;
			obj->short_remark = ren->ren_shortrem;
			obj->long_remark = ren->ren_longrem;
			obj->alter_date = ren->ren_modified;
		}
		if (En_program == PRG_SREPORT)
		{
			OOsnd(ren->ren_id, ii_flushAll);
		}
	}
	return;
}
