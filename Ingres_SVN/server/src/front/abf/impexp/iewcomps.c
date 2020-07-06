# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 1993, 2004 Ingres Corporation
*/
# include       <compat.h>
# include	<si.h>
# include       <st.h>
# include       <gl.h>
# include       <iicommon.h>
# include       <fe.h>
# include	<ooclass.h>
# include	<oocat.h>
# include	<oodep.h>
# include	<abfdbcat.h>
# include	<cu.h>
# include	"erie.h"
# include       "ieimpexp.h"
/**
** Name: iewcomps.sc
** 
** Defines:
**	IIIEwad_WriteAbfDepend()	- populates ii_abfdependencies
**	IIIEwfc_WriteField()		- populates ii_fields
**	IIIEwtc_WriteTrim()		- populates ii_trim
**	IIIEwrc_WriteRCommands()	- populates ii_rcomands
**	IIIEwvj_WriteVQJoins()		- populates ii_vqjoins
**	IIIEwvt_WriteVQTabs()		- populates ii_vqtables
**	IIIEwvc_WriteVQCols()		- populates ii_vqtabcols
**	IIIEwma_WriteMenuArgs()		- populates ii_menuargs
**	IIIEwfv_WriteFrameVars()	- populates ii_framevars
**	IIIEwec_WriteEscape()		- populates ii_encodings
**
** Description:
**
**	This family of routines write the actual components of a
** particular object out to its respective catalog. The routines, in
** general, do no i/o of themselves; these routines are called from 
** IIEdcr_DoComponentRecs(), in a nice little loop. That wacky and
** ubiquitous input buffer, char *inbuf, is passed along, but in reality
** cu_gettok() takes care of dicing the buffer up into the appropriate
** fields of the catalog. Then the insert is sent to the database
** (component records tend to be in a 1:N relationship, so we couldn't
** issue updates; previous routines have done the delete, now we need to do
** the insert).
**
** NOTE: these routines all use cu_gettok(). This routine is first
** "primed" by IIEdcr_DoComponentRecs(), which allows us to simply call the
** thing. cu_gettok() uses some internal statics. We continue to pass inpuf
** around just 'cause it aids in debugging. You'll see what I mean if
** you look at IIEdcr_DoComponentRecs(), and at cu_gettok() in
** utils!copyutil!cuutils.c
**
** History:
**	15-sep-1993 (daver)
**		First Documented.
**	22-dec-1993 (daver)
**		Related to bug 56731 in IIIEwad_WriteAbfDepend(); fixes
**		problem of owner.table in dependency records.
**	27-dec-1993 (daver)
**		Call copyutil routine IICUbsdBSDecode() to "decode"
**		backslash characters (\n, \t, \r, etc) into a newline,
**		tab, etc when populating ii_encodings. Fixes bug 58031.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
/* # line 77 "iewcomps.sc" */	/* host code */
/*{
** Name: IIIEwad_WriteAbfDepend		- populate ii_abfdependencies
**
** Description:
**	Similar to the other routines in this file, this routine populates
** ii_abfdependencies with the information in the input buffer inbuf. 
**
** If we decide to copy application BRANCHES, this is one of the routines
** which need to change: we'll need to allow and verify the copying of
** types OC_DTMREF, OC_DTCALL, or OC_DTRCALL.
**
** Input:
**	char		*inbuf		current line in file
**	IEOBJREC	*objrec		object in question
**	i4		appid		id of application object belongs to
**	char		*appowner	owner of application 
**
** Output:
**	None
**
** Returns:
**	Whatever the status of the query in question returns.
**
** History:
**	14-sep-1993 (daver)
**		First Documented.
**	22-dec-1993 (daver)
**		Related to bug 56731. Don't use the appowner field
**		when the dependency is table related. The table owner (if any)
**		will exist in the defowner field in the file.
*/
STATUS
IIIEwad_WriteAbfDepend(inbuf, objrec, appid, appowner)
char		*inbuf;
IEOBJREC	*objrec;
  i4 appid;
  char *appowner;
{
  i4 objid;
  char *defname;
  char *defowner;
  i4 class;
  i4 deptype;
  char *linkname;
  i4 deporder;
  char right_owner[FE_MAXNAME + 1];
	if (IEDebug)
		SIprintf("In IIIEwad_WriteAbfDepend...\n\n");
	objid = objrec->id;
	/*
	** cu_gettok has been primed by the caller; we really don't even need
	** to pass the inbuf around, but we do 'cause it used to 
	** not work as is a pain if you're debugging.
	*/
	defname = cu_gettok((char *)NULL);
	defowner = cu_gettok((char *)NULL);
	CVal(cu_gettok((char *)NULL), &class);
	CVal(cu_gettok((char *)NULL), &deptype);
	linkname = cu_gettok((char *)NULL);
	CVal(cu_gettok((char *)NULL), &deporder);
	/* 
	** We don't want to add the dependencies for any menutypes 
	** (OC_DTMREF) -- THIS IS THE PLACE TO ADD IT if we ever 
	** copy over whole application BRANCHES. Those menutypes determine 
	** which frames are beneath a vision (menu, update, insert, browse) 
	** frame, if any. 
	**
	** This is because on update we don't want to disturb the
	** existing application flow; and on insert, we don't want to carry
	** over menutype "stubs" when we don't know if the frames these
	** stubs reference exist. 
	**
	** This also includes dependencies on OC_DTRCALL and OC_DTCALL,
	** which are calls to procedures/frames not yet defined (or non
	** existant in the target application), with or without a 'R'eturn
	** value.
	**
	** Furthermore, while ILCODE dependencies shouldn't be written
	** out to the iiexport files anyway, we'll completely ignore these
	** things if we're updating an existing object. If we're inserting,
	** there may have been some rationale for including this ILCODE
	** object in the file, so go ahead and add it.
	*/
	if ( (deptype == OC_DTMREF) || (deptype == OC_DTCALL) || 
	     (deptype == OC_DTRCALL)  ||
	     ((objrec->update) && (class == OC_ILCODE)) ) 
	{
		return (OK);
	}
	/*
	** If its a "type of table" thang, or if something's been
	** specified, use that. Otherwise, use the application owner
	** in the abfdef_owner field.
	*/
	if ( (deptype == OC_DTTABLE_TYPE) || (*defowner != EOS) )
		STcopy(defowner, right_owner);
	else
		STcopy(appowner, right_owner);
/* # line 188 "iewcomps.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_abfdependencies(object_id, abfdef_applid, abfdef_name,\
 abfdef_owner, object_class, abfdef_deptype, abf_linkname, abf_deporde\
r)values(");
    IIputdomio((short *)0,1,30,4,&objid);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&appid);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,defname);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,right_owner);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&class);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&deptype);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,linkname);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&deporder);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 196 "iewcomps.sc" */	/* host code */
	return(FEinqerr()); 
}
/*{
** Name: IIIEwfc_WriteField		- populate ii_fields
**
** Description:
**	Similar to the other routines in this file, this routine populates
** a catalog (ii_fields) with the information in the input buffer inbuf. 
**
** Input:
**	char		*inbuf		current line in file
**	i4		objid		id of the object in question
**
** Output:
**	None
**
** Returns:
**	Whatever the status of the query in question returns.
**
** History:
**	14-sep-1993 (daver)
**		First Documented.
*/
STATUS
IIIEwfc_WriteField(inbuf, objid)
char		*inbuf;
  i4 objid;
{
  i4 flseq;
  char *fldname;
  i4 fldatatype;
  i4 fllength;
  i4 flprec;
  i4 flwidth;
  i4 flmaxlin;
  i4 flmaxcol;
  i4 flposy;
  i4 flposx;
  i4 fldatawidth;
  i4 fldatalin;
  i4 fldatacol;
  char *fltitle;
  i4 fltitcol;
  i4 fltitlin;
  i4 flintrp;
  i4 fldflags;
  i4 fld2flags;
  i4 fldfont;
  i4 fldptsz;
  char *fldefault;
  char *flformat;
  char *flvalmsg;
  char *flvalchk;
  i4 fltype;
  i4 flsubseq;
	if (IEDebug)
		SIprintf("In IIIEwfc_WriteField, doing one line...\n\n");
	CVal(cu_gettok((char *)NULL), &flseq);
	fldname = cu_gettok((char *)NULL);
	CVan(cu_gettok((char *)NULL), &fldatatype);
	CVan(cu_gettok((char *)NULL), &fllength);
	CVan(cu_gettok((char *)NULL), &flprec);
	CVan(cu_gettok((char *)NULL), &flwidth);
	CVan(cu_gettok((char *)NULL), &flmaxlin);
	CVan(cu_gettok((char *)NULL), &flmaxcol);
	CVan(cu_gettok((char *)NULL), &flposy);
	CVan(cu_gettok((char *)NULL), &flposx);
	CVan(cu_gettok((char *)NULL), &fldatawidth);
	CVan(cu_gettok((char *)NULL), &fldatalin);
	CVan(cu_gettok((char *)NULL), &fldatacol);
	fltitle = cu_gettok((char *)NULL);
	CVan(cu_gettok((char *)NULL), &fltitcol);
	CVan(cu_gettok((char *)NULL), &fltitlin);
	CVan(cu_gettok((char *)NULL), &flintrp);
	CVal(cu_gettok((char *)NULL), &fldflags);
	CVal(cu_gettok((char *)NULL), &fld2flags);
	CVan(cu_gettok((char *)NULL), &fldfont);
	CVan(cu_gettok((char *)NULL), &fldptsz);
	fldefault = cu_gettok((char *)NULL);
	flformat = cu_gettok((char *)NULL);
	flvalmsg = cu_gettok((char *)NULL);
	flvalchk = cu_gettok((char *)NULL);
	CVan(cu_gettok((char *)NULL), &fltype);
	CVan(cu_gettok((char *)NULL), &flsubseq);
/* # line 293 "iewcomps.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_fields(object_id, flseq, fldname, fldatatype, fllength\
, flprec, flwidth, flmaxlin, flmaxcol, flposy, flposx, fldatawidth, fl\
datalin, fldatacol, fltitle, fltitcol, fltitlin, flintrp, fldflags, fl\
d2flags, fldfont, fldptsz, fldefault, flformat");
    IIwritio(0,(short *)0,1,32,0,(char *)
", flvalmsg, flvalchk, fltype, flsubseq)values(");
    IIputdomio((short *)0,1,30,4,&objid);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&flseq);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,fldname);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&fldatatype);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&fllength);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&flprec);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&flwidth);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&flmaxlin);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&flmaxcol);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&flposy);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&flposx);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&fldatawidth);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&fldatalin);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&fldatacol);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,fltitle);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&fltitcol);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&fltitlin);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&flintrp);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&fldflags);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&fld2flags);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&fldfont);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&fldptsz);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,fldefault);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,flformat);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,flvalmsg);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,flvalchk);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&fltype);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&flsubseq);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 307 "iewcomps.sc" */	/* host code */
	return(FEinqerr()); 
}
/*{
** Name: IIIEwtc_WriteTrim		- populate ii_trim
**
** Description:
**	Similar to the other routines in this file, this routine populates
** a catalog (ii_trim) with the information in the input buffer inbuf. 
**
** Input:
**	char		*inbuf		current line in file
**	i4		objid		id of the object in question
**
** Output:
**	None
**
** Returns:
**	Whatever the status of the query in question returns.
**
** History:
**	14-sep-1993 (daver)
**		First Documented.
*/
STATUS
IIIEwtc_WriteTrim(inbuf, objid)
char		*inbuf;
  i4 objid;
{
  i4 trim_col;
  i4 trim_lin;
  char *trim_trim;
  i4 trim_flags;
  i4 trim2_flags;
  i4 trim_font;
  i4 trim_ptsz;
	CVan(cu_gettok((char *)NULL), &trim_col);
	CVan(cu_gettok((char *)NULL), &trim_lin);
	trim_trim = cu_gettok((char *)NULL);
	CVal(cu_gettok((char *)NULL), &trim_flags);
	CVal(cu_gettok((char *)NULL), &trim2_flags);
	CVan(cu_gettok((char *)NULL), &trim_font);
	CVan(cu_gettok((char *)NULL), &trim_ptsz);
	if (IEDebug)
		SIprintf("In IIIEwtc_WriteTrim, doing one line...\n\n");
/* # line 365 "iewcomps.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_trim(object_id, trim_col, trim_lin, trim_trim, trim_fl\
ags, trim2_flags, trim_font, trim_ptsz)values(");
    IIputdomio((short *)0,1,30,4,&objid);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&trim_col);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&trim_lin);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,trim_trim);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&trim_flags);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&trim2_flags);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&trim_font);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&trim_ptsz);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 373 "iewcomps.sc" */	/* host code */
	return(FEinqerr()); 
}
/*{
** Name: IIIEwrc_WriteRCommands		- populate ii_rcommands
**
** Description:
**	Similar to the other routines in this file, this routine populates
** a catalog (ii_rcommands) with the information in the input buffer inbuf. 
**
** Input:
**	char		*inbuf		current line in file
**	i4		objid		id of the object in question
**
** Output:
**	None
**
** Returns:
**	Whatever the status of the query in question returns.
**
** History:
**	14-sep-1993 (daver)
**		First Documented.
*/
STATUS
IIIEwrc_WriteRCommands(inbuf, objid)
char		*inbuf;
  i4 objid;
{
  char *rcotype;
  i4 rcosequence;
  char *rcosection;
  char *rcoattid;
  char *rcocommand;
  char *rcotext;
	rcotype = cu_gettok((char *)NULL);
	CVan(cu_gettok((char *)NULL), &rcosequence);
	rcosection = cu_gettok((char *)NULL);
	rcoattid = cu_gettok((char *)NULL);
	rcocommand = cu_gettok((char *)NULL);
	rcotext = cu_gettok((char *)NULL);
	if (IEDebug)
		SIprintf("In IIIEwrc_WriteRCommands ...\n\n");
/* # line 428 "iewcomps.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_rcommands(object_id, rcotype, rcosequence, rcosection,\
 rcoattid, rcocommand, rcotext)values(");
    IIputdomio((short *)0,1,30,4,&objid);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,rcotype);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&rcosequence);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,rcosection);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,rcoattid);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,rcocommand);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,rcotext);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 436 "iewcomps.sc" */	/* host code */
	return(FEinqerr()); 
}
/*{
** Name: IIIEwvj_WriteVQJoins		- populate ii_vqjoins
**
** Description:
**	Similar to the other routines in this file, this routine populates
** a catalog (ii_vqjoins) with the information in the input buffer inbuf. 
**
** Input:
**	char		*inbuf		current line in file
**	i4		objid		id of the object in question
**
** Output:
**	None
**
** Returns:
**	Whatever the status of the query in question returns.
**
** History:
**	14-sep-1993 (daver)
**		First Documented.
*/
STATUS
IIIEwvj_WriteVQJoins(inbuf, objid)
char		*inbuf;
  i4 objid;
{
  i4 vq_seq;
  i4 join_type;
  i4 join_tab1;
  i4 join_tab2;
  i4 join_col1;
  i4 join_col2;
	CVan(cu_gettok((char *)NULL), &vq_seq);
	CVan(cu_gettok((char *)NULL), &join_type);
	CVan(cu_gettok((char *)NULL), &join_tab1);
	CVan(cu_gettok((char *)NULL), &join_tab2);
	CVan(cu_gettok((char *)NULL), &join_col1);
	CVan(cu_gettok((char *)NULL), &join_col2);
	if (IEDebug)
		SIprintf("In IIIEwvj_WriteVQJoins, doing one line...\n\n");
/* # line 491 "iewcomps.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_vqjoins(object_id, vq_seq, join_type, join_tab1, join_\
tab2, join_col1, join_col2)values(");
    IIputdomio((short *)0,1,30,4,&objid);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&vq_seq);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&join_type);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&join_tab1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&join_tab2);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&join_col1);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&join_col2);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 500 "iewcomps.sc" */	/* host code */
	return(FEinqerr()); 
}
/*{
** Name: IIIEwvt_WriteVQTabs		- populate ii_vqtables
**
** Description:
**	Similar to the other routines in this file, this routine populates
** a catalog (ii_vqtables) with the information in the input buffer inbuf. 
**
** Input:
**	char		*inbuf		current line in file
**	i4		objid		id of the object in question
**
** Output:
**	None
**
** Returns:
**	Whatever the status of the query in question returns.
**
** History:
**	14-sep-1993 (daver)
**		First Documented.
*/
STATUS
IIIEwvt_WriteVQTabs(inbuf, objid)
char		*inbuf;
  i4 objid;
{
  i4 vq_seq;
  i4 vq_mode;
  char *tab_name;
  char *tab_owner;
  i4 tab_section;
  i4 tab_usage;
  i4 tab_flags;
	if (IEDebug)
		SIprintf("In IIIEwvt_WriteVQTabs, doing one line...\n\n");
	CVan(cu_gettok((char *)NULL), &vq_seq);
	CVan(cu_gettok((char *)NULL), &vq_mode);
	tab_name = cu_gettok((char *)NULL);
	tab_owner = cu_gettok((char *)NULL);
	CVan(cu_gettok((char *)NULL), &tab_section);
	CVan(cu_gettok((char *)NULL), &tab_usage);
	CVal(cu_gettok((char *)NULL), &tab_flags);
/* # line 560 "iewcomps.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_vqtables(object_id, vq_seq, vq_mode, tab_name, tab_own\
er, tab_section, tab_usage, tab_flags)values(");
    IIputdomio((short *)0,1,30,4,&objid);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&vq_seq);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&vq_mode);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,tab_name);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,tab_owner);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&tab_section);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&tab_usage);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&tab_flags);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 568 "iewcomps.sc" */	/* host code */
	return(FEinqerr()); 
}
/*{
** Name: IIIEwvc_WriteVQCols		- populate ii_vqtabcols
**
** Description:
**	Similar to the other routines in this file, this routine populates
** a catalog (ii_vqtabcols) with the information in the input buffer inbuf. 
**
** Input:
**	char		*inbuf		current line in file
**	i4		objid		id of the object in question
**
** Output:
**	None
**
** Returns:
**	Whatever the status of the query in question returns.
**
** History:
**	14-sep-1993 (daver)
**		First Documented.
*/
STATUS
IIIEwvc_WriteVQCols(inbuf, objid)
char		*inbuf;
  i4 objid;
{
  i4 vq_seq;
  i4 tvq_seq;
  char *col_name;
  char *ref_name;
  i4 adf_type;
  i4 adf_length;
  i4 adf_scale;
  i4 col_flags;
  i4 col_sortorder;
  char *col_info;
	CVan(cu_gettok((char *)NULL), &vq_seq);
	CVan(cu_gettok((char *)NULL), &tvq_seq);
	col_name = cu_gettok((char *)NULL);
	ref_name = cu_gettok((char *)NULL);
	CVan(cu_gettok((char *)NULL), &adf_type);
	CVal(cu_gettok((char *)NULL), &adf_length);
	CVal(cu_gettok((char *)NULL), &adf_scale);
	CVal(cu_gettok((char *)NULL), &col_flags);
	CVan(cu_gettok((char *)NULL), &col_sortorder);
	col_info = cu_gettok((char *)NULL);
	if (IEDebug)
		SIprintf("In IIIEwvc_WriteVQCols, doing one line...\n\n");
/* # line 631 "iewcomps.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_vqtabcols(object_id, vq_seq, tvq_seq, col_name, ref_na\
me, adf_type, adf_length, adf_scale, col_flags, col_sortorder, col_inf\
o)values(");
    IIputdomio((short *)0,1,30,4,&objid);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&vq_seq);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&tvq_seq);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,col_name);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,ref_name);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&adf_type);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&adf_length);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&adf_scale);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&col_flags);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&col_sortorder);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,col_info);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 639 "iewcomps.sc" */	/* host code */
	return(FEinqerr()); 
}
/*{
** Name: IIIEwma_WriteMenuArgs		- populate ii_menuargs
**
** Description:
**	Similar to the other routines in this file, this routine populates
** a catalog (ii_menuargs) with the information in the input buffer inbuf. 
**
** Input:
**	char		*inbuf		current line in file
**	i4		objid		id of the object in question
**
** Output:
**	None
**
** Returns:
**	Whatever the status of the query in question returns.
**
** History:
**	14-sep-1993 (daver)
**		First Documented.
*/
STATUS
IIIEwma_WriteMenuArgs(inbuf, objid)
char		*inbuf;
  i4 objid;
{
  char *mu_text;
  char *mu_field;
  char *mu_column;
  char *mu_expr;
  i4 mu_seq;
	mu_text = cu_gettok((char *)NULL);
	mu_field = cu_gettok((char *)NULL);
	mu_column = cu_gettok((char *)NULL);
	mu_expr = cu_gettok((char *)NULL);
	CVan(cu_gettok((char *)NULL), &mu_seq);
	if (IEDebug)
		SIprintf("In IIIEwma_WriteMenuArgs, doing one line...\n\n");
/* # line 691 "iewcomps.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_menuargs(object_id, mu_text, mu_seq, mu_field, mu_colu\
mn, mu_expr)values(");
    IIputdomio((short *)0,1,30,4,&objid);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,mu_text);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&mu_seq);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,mu_field);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,mu_column);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,mu_expr);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 697 "iewcomps.sc" */	/* host code */
	return(FEinqerr()); 
}
/*{
** Name: IIIEwfv_WriteFrameVars		- populate ii_framevars
**
** Description:
**	Similar to the other routines in this file, this routine populates
** a catalog (ii_framevars) with the information in the input buffer inbuf. 
**
** Input:
**	char		*inbuf		current line in file
**	i4		objid		id of the object in question
**
** Output:
**	None
**
** Returns:
**	Whatever the status of the query in question returns.
**
** History:
**	14-sep-1993 (daver)
**		First Documented.
*/
STATUS
IIIEwfv_WriteFrameVars(inbuf, objid)
char		*inbuf;
  i4 objid;
{
  i4 fr_seq;
  char *var_field;
  char *var_column;
  char *var_datatype;
  char *var_comment;
	CVan(cu_gettok((char *)NULL), &fr_seq);
	var_field = cu_gettok((char *)NULL);
	var_column = cu_gettok((char *)NULL);
	var_datatype = cu_gettok((char *)NULL);
	var_comment = cu_gettok((char *)NULL);
	if (IEDebug)
		SIprintf("In IIIEwfv_WriteFrameVars n\n");
/* # line 750 "iewcomps.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_framevars(object_id, fr_seq, var_field, var_column, va\
r_datatype, var_comment)values(");
    IIputdomio((short *)0,1,30,4,&objid);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&fr_seq);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,var_field);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,var_column);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,var_datatype);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,var_comment);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 756 "iewcomps.sc" */	/* host code */
	return(FEinqerr()); 
}
/*{
** Name: IIIEwec_WriteEscape		- populate ii_encodings
**
** Description:
**	Similar to the other routines in this file, this routine populates
** a catalog (ii_encodings) with the information in the input buffer inbuf. 
**
** Input:
**	char		*inbuf		current line in file
**	i4		objid		id of the object in question
**
** Output:
**	None
**
** Returns:
**	Whatever the status of the query in question returns.
**
** History:
**	14-sep-1993 (daver)
**		First Documented.
**	27-dec-1993 (daver)
**		Use our super-decoder ring to translate the characters '\n' 
**		into a newline (and other backslash chars like \t, \r, etc).
**		Fixes bug 58031.
*/
STATUS
IIIEwec_WriteEscape(inbuf, objid)
char		*inbuf;
  i4 objid;
{
  i4 encode_sequence;
  char *encode_estring;
	if (IEDebug)
		SIprintf("In IIIEwec_WriteEscape, doing one line...\n\n");
	CVan(cu_gettok((char *)NULL), &encode_sequence);
	encode_estring = cu_gettok((char *)NULL);
	/* 
	** decode the backslash chars, like \n, \t, \r, etc, into their
	** real life counter parts (newline, tab, whatever \r is...)
	*/
	IICUbsdBSDecode(encode_estring);
	/*
	** For some reason, ii_encodings has both object_id AND encode_object.
	** The manuals say that object_id is currently unused. Go figure. 
	** I insert it anyway. So there.
	*/
/* # line 818 "iewcomps.sc" */	/* insert */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_encodings(object_id, encode_object, encode_sequence, e\
ncode_estring)values(");
    IIputdomio((short *)0,1,30,4,&objid);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&objid);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&encode_sequence);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,32,0,encode_estring);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 824 "iewcomps.sc" */	/* host code */
	return(FEinqerr()); 
}
