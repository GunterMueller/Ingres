# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ooclass.h>
# include	<erui.h>
# include	<dictutil.h>
# include	<ui.h>
# include	<ug.h>
/**
** Name:	cleanup.sc - remove garbage from fe catalogs
**
** Description:
**	Cleanup for FE catalogs.  Defines:
**
**	IIUIdgDeleteGarbage
**	IIUInslNoSessionLocks
*	IIUIadaAddDepAppl
**
** History:
**
**	2/90 (bobm)	Written
**	7/18/91 (bobm)	Fix for 38709 - check ii_encodings against ii_entities
**			(Windows4GL table) as well as ii_objects.
**	7/19/91 (pete)	Followup to what Bob did above: change table of
**			cleanup info so ii_encodings is only cleaned up for
**			CORE. Was being cleaned for both CORE and for APPDEV1.
**			The ii_encodings cleanup is considerably slower now.
**	21-jul-1993 (rdrane)
**		Extensive modification to ensure that FErelexists() is always
**		passed the catalog name in the proper case, so that FIPS
**		databases are supported.
**	05-Aug-1993 (fredv)
**		The arguments in the STcopy call were reversed;thus, Cats
**		got corrupted, fixed it.
**  28-mar-95 (angusm)
**          Since default in server is not to flatten queries,
**          some of the subselects here perform badly.
**          Force flattening with SET TRACE POINT OP133 at start
**          of this portion of processing
**	04-mar-95 (lawst01)
**	   Windows 3.1 port changes - added include <ui.h.> and <ug.h>
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/*
** Buffer in which to effect casing of table names
** prior to passing to FErelexists().  Since we deal
** exclusively with non-compound, regular identifiers, we don't
** need to size the buffer as FE_UNRML_MAXNAME or FE_MAXTABNAME.
*/
static	char	table_buf[(FE_MAXNAME + 1)];
STATUS IIUIffuFidFixUp();
STATUS IIUIadaAddDepAppl();
static struct driver
{
	char *table;
	char *column;
	u_i4 mask;	/* module(s) table applies to */
	bool doit;	/* set at runtime */
	bool chkent;	/* check ii_entities table */
}
Cats[] =
{
	{ ERx("ii_abfobjects"), ERx("object_id"), DMB_APPDEV1, TRUE, FALSE },
	{ ERx("ii_abfobjects"), ERx("applid"), DMB_APPDEV1, TRUE, FALSE },
	{ ERx("ii_encodings"), ERx("encode_object"), DMB_CORE, TRUE, TRUE },
	{ ERx("ii_framevars"), ERx("object_id"), DMB_APPDEV3, TRUE, FALSE },
	{ ERx("ii_longremarks"), ERx("object_id"), DMB_CORE, TRUE, FALSE },
	{ ERx("ii_menuargs"), ERx("object_id"), DMB_APPDEV3, TRUE, FALSE },
	{ ERx("ii_abfclasses"), ERx("catt_id"), DMB_APPDEV1, TRUE, FALSE },
	{ ERx("ii_vqjoins"), ERx("object_id"), DMB_APPDEV3, TRUE, FALSE },
	{ ERx("ii_vqtabcols"), ERx("object_id"), DMB_APPDEV3, TRUE, FALSE },
	{ ERx("ii_vqtables"), ERx("object_id"), DMB_APPDEV3, TRUE, FALSE }
};
static char *Fstr =
ERx("DELETE FROM %s WHERE %s NOT IN ( SELECT object_id FROM ii_objects )");
static char *Fstr2 =
ERx("DELETE FROM %s WHERE %s NOT IN ( SELECT object_id FROM ii_abfobjects )");
static char *FstrWEnt =
ERx("DELETE FROM %s WHERE %s NOT IN ( SELECT object_id FROM ii_objects ) AND %s NOT IN ( SELECT entity_id FROM ii_entities )");
/*{
** Name:	IIUIdgDeleteGarbage
**
** Description:
**	Removes unused records from the FE catalogs.  These records may
**	be left around when FE objects are deleted - one only has to
**	delete the ii_objects record to make an FE object unreachable.
**	Rather than delete records from the other catalogs joined to
**	ii_objects, making for slow deletes, we leave them for eventual
**	garbage collection by this routine.
**
** Inputs:
**	which	bit mask of which modules to cleanup, 0 for all.
**	verbose	TRUE to produce informational messages during cleanup.
**	
**
** Outputs:
**
**	count - count of rows deleted.
**
**	returns STATUS of first failed query.  Will go ahead and continue
**	with all queries in any case, in order to clean up all the junk we
**	can, at the cost of getting more failed queries.
**
** History:
**
**	2/90 (bobm)	Written
**	7/91 (bobm)	Fix for 38709 - check ii_encodings against ii_entities
**			(Windows4GL table) as well as ii_objects.
**	27-jul-1992 (rdrane)
**		Change parameterization of call to FErelexists() to include
**		owner (actually, we can get away with (char *)NULL -
**		users shouldn't have ii* tables).
**	22-jul-1993 (rdrane)
**		Effect case translation of catalog names prior to calling
**		FErelexists() for FIPS support.
*/
STATUS
IIUIdgDeleteGarbage(which,verbose,count)
u_i4 which;
bool verbose;
i4  *count;
{
	i4	i;
	i4	tot;
	STATUS	rval;
	STATUS	op;
	i4	depcount;
	struct driver	*tn;
	bool	have_ent;
  i4 ooclass;
  char tbuf[240];
  i4 rc;
	if (which == 0)
		which = 0xffffL;
	iiuicw1_CatWriteOn();
	rval = OK;
	*count = 0;
/*
**  bug 65653:
**  force flattening for the "delete from where not in (select * from y)"
**  type of query which we use here.
**
*/
/* # line 170 "cleanup.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set TRACE POINT OP133");
    IIsyncup((char *)0,0);
  }
/* # line 173 "cleanup.sc" */	/* host code */
	/*
	** get rid of unused abfdependencies before finding unreferenced
	** IL records.  Use the first condition to gate the case translation
	** of the ii_abfdependencies catalog name.
	*/
	if  ((which & DMB_APPDEV1) != 0)
	{
		STcopy(ERx("ii_abfdependencies"),&table_buf[0]);
		IIUGdbo_dlmBEobject(&table_buf[0],FALSE);
	}
	if ((which & DMB_APPDEV1) != 0 &&
			FErelexists(&table_buf[0],NULL) == OK)
	{
		if (verbose)
			IIUGmsg(ERget(S_UI0020_Depend),FALSE,0);
		STprintf(tbuf,Fstr,ERx("ii_abfdependencies"),ERx("object_id"));
/* # line 189 "cleanup.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(tbuf);
    IIsyncup((char *)0,0);
  }
/* # line 190 "cleanup.sc" */	/* host code */
		op = FEinqerr();
		if (op == OK)
			*count += FEinqrows();
		else
		{
			if (rval == OK)
				rval = op;
		}
		STprintf(tbuf,Fstr2,ERx("ii_abfdependencies"),ERx("object_id"));
/* # line 199 "cleanup.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(tbuf);
    IIsyncup((char *)0,0);
  }
/* # line 200 "cleanup.sc" */	/* host code */
		op = FEinqerr();
		if (op == OK)
			*count += FEinqrows();
		else
		{
			if (rval == OK)
				rval = op;
		}
		/* Check for ii_abfdependencies records which 6.3 has mangled */
		_VOID_ IIUIadaAddDepAppl(-1, verbose, TRUE, &depcount);
		count += depcount;
		/*
		** Delete unused IL object records from ii_objects.  This will
		** allow unused IL to get deleted from ii_encodings.
		**
		** Background:
		**
		** We are worried about two ways IL dependencies have been
		** entered into ii_abfdependencies and ii_objects.  Some
		** names have "fid" prepended to the number, and later simply
		** the id without the "fid" prefix was getting used for the
		** dependency.  BUT, the name was STILL being entered with
		** "fid" into ii_objects.  Currently, both are
		** entered as pure numeric.
		**
		** Because of difficulties with open SQL (no conversion
		** of numeric to character), we check for old style
		** names, and convert them before doing the delete.
		**
		** This will involve cursors, so we avoid it if possible.  If
		** conversion fails, we make sure we do not do the delete from
		** ii_objects.
		*/
		ooclass = OC_ILCODE;
/* # line 238 "cleanup.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from ii_objects where object_name like 'fid%' and obje\
ct_class=");
    IIputdomio((short *)0,1,30,4,&ooclass);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&rc);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 243 "cleanup.sc" */	/* host code */
		op = FEinqerr();
		if (op == OK && rc == 0)
		{
/* # line 247 "cleanup.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from ii_abfdependencies where abfdef_name like 'fid%' \
and object_class=");
    IIputdomio((short *)0,1,30,4,&ooclass);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&rc);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 252 "cleanup.sc" */	/* host code */
			op = FEinqerr();
		}
		if (op == OK && rc > 0)
		{
			if (verbose)
				IIUGmsg(ERget(S_UI0021_FidFix),FALSE,0);
			op = IIUIffuFidFixUp();
			iiuicw1_CatWriteOn();
		}
		if (op == OK)
		{
/* # line 266 "cleanup.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_objects where object_class=");
    IIputdomio((short *)0,1,30,4,&ooclass);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and object_name not in(select abfdef_name from ii_abfdependencies whe\
re object_class=");
    IIputdomio((short *)0,1,30,4,&ooclass);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 271 "cleanup.sc" */	/* host code */
			op = FEinqerr();
		}
		if (op != OK)
		{
			if (rval == OK)
				rval = op;
		}
		else
		{
			*count += FEinqrows();
		}
	}
	/*
	** if ii_entities table exists, we have to check some tables
	** against it as well as ii_objects.
	*/
	STcopy(ERx("ii_entities"),&table_buf[0]);
	IIUGdbo_dlmBEobject(&table_buf[0],FALSE);
	if (FErelexists(&table_buf[0],NULL) == OK)
		have_ent = TRUE;
	else
		have_ent = FALSE;
	/*
	** we scan through the table setting doit bits if we are going
	** to do the table.  The only real reason we scan twice is to have
	** a count of the total number of tables we are processing for
	** the "verbose" message, and we don't want to invoke relexists
	** twice.  Yeah, it's a little silly ....
	*/
	tn = Cats;
	tot = 0;
	for (i=0; i < sizeof(Cats)/sizeof(Cats[1]); ++i,++tn)
	{
		tn->doit = FALSE;
		if ((which & tn->mask) == 0)
			continue;
		STcopy(tn->table, &table_buf[0]);
		IIUGdbo_dlmBEobject(&table_buf[0],FALSE);
		if (FErelexists(&table_buf[0],NULL) != OK)
			continue;
		++tot;
		tn->doit = TRUE;
	}
	tn = Cats;
	for (i=0; i < sizeof(Cats)/sizeof(Cats[1]); ++i,++tn)
	{
		if (! tn->doit)
			continue;
		if (verbose)
			IIUGmsg(ERget(S_UI0022_Table),
					FALSE, 3, &i, &tot, tn->table);
		if (tn->chkent && have_ent)
			STprintf(tbuf,FstrWEnt,tn->table,tn->column,tn->column);
		else
			STprintf(tbuf,Fstr,tn->table,tn->column);
/* # line 329 "cleanup.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(tbuf);
    IIsyncup((char *)0,0);
  }
/* # line 330 "cleanup.sc" */	/* host code */
		op = FEinqerr();
		if (op != OK)
		{
			if (rval == OK)
				rval = op;
		}
		else
		{
			*count += FEinqrows();
		}
	}
	iiuicw0_CatWriteOff();
	return rval;
}
/*{
** Name:	IIUInslNoSessionLocks
**
** Description:
**	Removes all records from ii_locks having a non-zero session id
**	This may be done at times when we know that no real user sessions
**	can exist, which is the reason it is not lumped together with the
**	cleanup routine, which is called from ABF.
**
** History:
**
**	2/90 (bobm)	Written
**	27-jul-1992 (rdrane)
**		Change parameterization of call to FErelexists() to include
**		owner (actually, we can get away with (char *)NULL -
**		users shouldn't have ii* tables).
**	22-jul-1993 (rdrane)
**		Effect case translation of catalog names prior to calling
**		FErelexists() for FIPS support.
*/
STATUS
IIUInslNoSessionLocks()
{
	STATUS rval;
	STcopy(ERx("ii_locks"),&table_buf[0]);
	IIUGdbo_dlmBEobject(&table_buf[0],FALSE);
	if  (FErelexists(&table_buf[0],NULL) != OK)
	{
		return OK;
	}
	iiuicw1_CatWriteOn();
/* # line 382 "cleanup.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_locks where session_id<>0");
    IIsyncup((char *)0,0);
  }
/* # line 384 "cleanup.sc" */	/* host code */
	rval = FEinqerr();
	iiuicw0_CatWriteOff();
	return rval;
}
/*{
** Name:	IIUIffuFidFixUp
**
** Description:
**	This takes care of old style fid names, so that we can easily
**	delete unused IL records.
**
** History:
**
**	3/90 (bobm)	Written
*/
STATUS
IIUIffuFidFixUp()
{
	STATUS rval;
  i4 cls;
  char nm[FE_MAXNAME+1];
  char *ptr;
  i4 done;
  i4 eno;
	cls = OC_ILCODE;
	ptr = nm+3;
	iiuicw1_CatWriteOn();
/* # line 426 "cleanup.sc" */	/* open */
  {
    IIsqInit((char *)0);
    IIcsOpen((char *)"cobj",6498,21166);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select object_name from ii_objects where object_name like 'fid%' and \
object_class=");
    IIputdomio((short *)0,1,30,4,&cls);
    IIwritio(0,(short *)0,1,32,0,(char *)"for update of object_name");
    IIcsQuery((char *)"cobj",6498,21166);
  }
/* # line 428 "cleanup.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&eno,2);
  }
/* # line 430 "cleanup.sc" */	/* host code */
	if (eno != 0)
		goto errobj;
	for (;;)
	{
/* # line 435 "cleanup.sc" */	/* fetch */
  {
    IIsqInit((char *)0);
    if (IIcsRetScroll((char *)"cobj",6498,21166,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,FE_MAXNAME+1-1,nm);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 437 "cleanup.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
    IILQisInqSqlio((short *)0,1,30,4,&eno,2);
  }
/* # line 439 "cleanup.sc" */	/* host code */
		if (done)
			goto endobj;
		if (eno != 0)
			goto errobj;
/* # line 445 "cleanup.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_objects set object_name=");
    IIputdomio((short *)0,1,32,0,ptr);
    IIcsERplace((char *)"cobj",6498,21166);
  }
/* # line 448 "cleanup.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&eno,2);
  }
/* # line 450 "cleanup.sc" */	/* host code */
		if (eno != 0)
			goto errobj;
	}
errobj:
	rval = FEinqerr();
/* # line 456 "cleanup.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"cobj",6498,21166);
  }
/* # line 457 "cleanup.sc" */	/* host code */
	goto errout;
endobj:
/* # line 460 "cleanup.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"cobj",6498,21166);
  }
/* # line 468 "cleanup.sc" */	/* open */
  {
    IIsqInit((char *)0);
    IIcsOpen((char *)"cdep",6499,24202);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select abfdef_name from ii_abfdependencies where abfdef_name like 'fi\
d%' and object_class=");
    IIputdomio((short *)0,1,30,4,&cls);
    IIwritio(0,(short *)0,1,32,0,(char *)"for update of abfdef_name");
    IIcsQuery((char *)"cdep",6499,24202);
  }
/* # line 470 "cleanup.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&eno,2);
  }
/* # line 472 "cleanup.sc" */	/* host code */
	if (eno != 0)
		goto errdep;
	for (;;)
	{
/* # line 477 "cleanup.sc" */	/* fetch */
  {
    IIsqInit((char *)0);
    if (IIcsRetScroll((char *)"cdep",6499,24202,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,FE_MAXNAME+1-1,nm);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 479 "cleanup.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
    IILQisInqSqlio((short *)0,1,30,4,&eno,2);
  }
/* # line 481 "cleanup.sc" */	/* host code */
		if (done)
			goto enddep;
		if (eno != 0)
			goto errdep;
/* # line 487 "cleanup.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_abfdependencies set abfdef_name=");
    IIputdomio((short *)0,1,32,0,ptr);
    IIcsERplace((char *)"cdep",6499,24202);
  }
/* # line 490 "cleanup.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&eno,2);
  }
/* # line 492 "cleanup.sc" */	/* host code */
		if (eno != 0)
			goto errdep;
	}
enddep:
/* # line 498 "cleanup.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"cdep",6499,24202);
  }
/* # line 499 "cleanup.sc" */	/* host code */
	iiuicw0_CatWriteOff();
	return OK;
errdep:
	rval = FEinqerr();
/* # line 504 "cleanup.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"cdep",6499,24202);
  }
/* # line 506 "cleanup.sc" */	/* host code */
errout:
	iiuicw0_CatWriteOff();
	if (rval == OK)
		rval = FAIL;
	return rval;
}
/*{
** Name:	IIUIadaAddDepAppl
**
** Description:
**	If 6.3 or earlier ABF has been running against 6.4 or later catalogs, it
**	may have added dependencies with NULL abfdef_applid values.  We will
**	fix them up here by joining agains ii_abfobjects to get the application
**	id.  This isn't quick, but we don't expect to do it often (if at all).
**
** Inputs:
**	applid		i4		Application to do -- -1 for all
**	verbose		bool		Whether to yakety-yak 
**	catwrite	bool		Is CatWrite privilege enabled?
**
** Outputs:
**	rows		i4 *		How many rows we touched
**
**	Returns:
**		STATUS		Database status
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	8/90 (Mike S)	Initial version
*/
STATUS
IIUIadaAddDepAppl(applid, verbose, catwrite, count)
  i4 applid;
bool	verbose;
bool	catwrite;
i4	*count;
{
  i4 local_count = 0;
  i4 status;
  i4 done;
  i4 objid;
  i4 nullapplid;
  i4 rows;
  i2 ind;
	bool	wasinxact;
	*count = 0;
	/* 
	** Look for NULL application ID's.  Usually, there won't be any.
	** Even if we're only concerned with one application, we'll do this
	** quick check.
	*/
/* # line 569 "cleanup.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from ii_abfdependencies where abfdef_applid is null");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&local_count);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 574 "cleanup.sc" */	/* host code */
	if ((status = FEinqerr()) != OK)
		return status;
	if (local_count == 0)
		return OK;
	/* 
	** There were some.  Let's fix them.  If we're fixing all of them, we
	** need a (gasp!) cursor.
	*/
	if (verbose)
	{
		if (applid > 0)
			IIUGerr(S_UI002E_DepFix, 0, 0);
		IIUGmsg(ERget(S_UI0021_FidFix), FALSE, 0);
	}
	if (!catwrite)
		iiuicw1_CatWriteOn();
	if (applid > 0)
	{
		/* One application only */
/* # line 597 "cleanup.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_abfdependencies set abfdef_applid=");
    IIputdomio((short *)0,1,30,4,&applid);
    IIwritio(0,(short *)0,1,32,0,(char *)
"where object_id in(select d.object_id from ii_abfobjects o, ii_abfdep\
endencies d where o.object_id=d.object_id and o.applid=");
    IIputdomio((short *)0,1,30,4,&applid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and abfdef_applid is null)");
    IIsyncup((char *)0,0);
  }
/* # line 605 "cleanup.sc" */	/* host code */
		status = FEinqerr();
		local_count = FEinqrows();
	}
	else
	{
		/* Do the whole catalog */
		wasinxact = IIUIinXaction();
		if (!wasinxact)
			IIUIbeginXaction();
/* # line 620 "cleanup.sc" */	/* open */
  {
    IIsqInit((char *)0);
    IIcsOpen((char *)"cabfdepupd",6500,196);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select object_id, abfdef_applid from ii_abfdependencies where abfdef_\
applid is null for update of abfdef_applid");
    IIcsQuery((char *)"cabfdepupd",6500,196);
  }
/* # line 621 "cleanup.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&status,2);
  }
/* # line 622 "cleanup.sc" */	/* host code */
		if (status != OK)
			goto curerr;
		/* Update each record we find */
		for ( local_count = 0; ;)
		{
/* # line 628 "cleanup.sc" */	/* fetch */
  {
    IIsqInit((char *)0);
    if (IIcsRetScroll((char *)"cabfdepupd",6500,196,-1,-1) != 0) {
      IIcsGetio((short *)0,1,30,4,&objid);
      IIcsGetio(&ind,1,30,4,&nullapplid);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 630 "cleanup.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&status,2);
    IILQisInqSqlio((short *)0,1,30,4,&done,61);
  }
/* # line 632 "cleanup.sc" */	/* host code */
			if (done || status != OK)
				break;
/* # line 635 "cleanup.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select applid, object_id from ii_abfobjects where ii_abfobjects.objec\
t_id=");
    IIputdomio((short *)0,1,30,4,&objid);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&applid);
      IIgetdomio((short *)0,1,30,4,&objid);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 639 "cleanup.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&status,2);
    IILQisInqSqlio((short *)0,1,30,4,&rows,8);
  }
/* # line 641 "cleanup.sc" */	/* host code */
			if (status != OK)
				break;
			if (rows == 0)
			{
				/* we'll let cleanup delete it */
				continue;
			}
/* # line 649 "cleanup.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_abfdependencies set abfdef_applid=");
    IIputdomio((short *)0,1,30,4,&applid);
    IIcsERplace((char *)"cabfdepupd",6500,196);
  }
/* # line 652 "cleanup.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&status,2);
  }
/* # line 653 "cleanup.sc" */	/* host code */
			if (status != OK)
				break;
			local_count++;		/* We updated one */
		}
/* # line 658 "cleanup.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"cabfdepupd",6500,196);
  }
/* # line 659 "cleanup.sc" */	/* host code */
curerr:
		if (!wasinxact)
			if (status == OK)
				IIUIendXaction();
			else
				IIUIabortXaction();
	}
	if (!catwrite)
		iiuicw0_CatWriteOff();
	if (status == OK)
		*count = local_count;
	return status;
}
