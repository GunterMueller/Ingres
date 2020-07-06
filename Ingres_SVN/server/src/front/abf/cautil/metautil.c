# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<cv.h>		/* 6-x_PC_80x86 */
# include	<lo.h>
# include	<si.h>
# include	<st.h>
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ooclass.h>
# include	<cu.h>
# include	<abclass.h>
# include	<metafrm.h>
# include	<acatrec.h>
/**
** Name:	metautil.sc -	Metaframe type utilities
**
** Description:
**	Utilities for handling Metaframe types:
**
**	IICAmtMetaTwiddle
**	IICAmfwMetaFrameWrite
**	IICApfdPatchFormDate
**	IICAbdfBackDateForm
**	IICAaslApplSourceLoc
**	IICA4nf4GLNewFrame
**
** History:
**	3/90 (bobm)	Initial Version
**	10/90 (bobm)	Remove REPEAT from queries because we suspect that
**			the REPEAT's are making the server crash & burn in
**			some circumstances.
**	8-march-91 (blaise)
**	    Integrated PC porting changes.
**	5-nov-92 (essi)
**		Fixed bug 43378.
**	09-May-2005 (kodse01)
**	    replace %ld with %d for old nat and long nat variables.
**
** NOTE:
**	All of these routines should only be getting invoked out of
**	copyapp - the ones that do writes call CatWriteOn() to make sure,
**	but don't call CatWriteOff to avoid turning off write priveledge
**	for some calling routine.  This reflects an assumption that redundant
**	CatWriteOn's don't hurt anything, and CatWrite should be on for
**	the duration of a copyapp in anyway.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/*
** ESCSPLIT is a maximum buffer size to output for escape code records.
** It should come out to be 500.  The /4 represents the fact that things
** can theoretically inflate by a factor of 4 when backslash encoded,
** although they will usually remain about the same size.
**
** The 500 will cause existing 1990 byte records to be split into 4
** roughly equal pieces, while not creating file lines too long for most
** editors.  Making it larger creates longer file lines, but makes for
** fewer records in the target database.  500 seems like a good compromise.
*/
#define ESCSPLIT (500 > CU_LINEMAX/4 - 20 ? CU_LINEMAX/4 - 20 : 500)
char *UGcat_now();
static LOCATION Sloc;
static char Sbuf[MAX_LOC+1];
/*
** Back_date and Funky_date MUST be different.
** Funky_date is a place holder for form gen dates which must be updated
** to a date LATER that of their form.  Back_date is a date stamped on
** gendates to assure that it is older than the form.
** We cannot make Funky_date a date in the future, either.  If Funky_date
** is somehow not updated, having those fields be very ancient is less
** of a problem then having them be some future date.
*/
static char *Funky_date = "1900_01_01 00:00:00 GMT";
  static char *Back_date = "1970_01_01 00:00:00 GMT";
/*{
** Name:	IICAmtMetaTwiddle
**
** Description:
**	Set flags and formgen dates prior to copyapp out.
**
**	Basically, set the CUSTOM_EDIT bit correctly, and flags all form
**	generation dates which should be updated to "now" on copyapp in.
**
** Inputs:
**	cr - catalog record.
**
** History:
**	3/90 (bobm)	initial version.
*/
VOID
IICAmtMetaTwiddle(arec)
register ACATREC	*arec;
{
	LOCATION loc;
	char lbuf[MAX_LOC+1];
	LOINFORMATION info;
	char *gdstr;
	i4 flags;
	i4 iflags;
	SYSTIME gen, fdate;
	OOID junk;
	ACATDEP *d;
	char *fname;
	if ((gdstr = STindex((arec->arg)[5],ERx("@"),0)) != NULL)
							/* 6-x_PC_80x86 */
		++gdstr;
	else
		gdstr = (arec->arg)[5];
	LOcopy(&Sloc,lbuf,&loc);
	LOfstfile(arec->source,&loc);
	/*
	** if LOinfo fails, we have to assume the file doesn't exist,
	** and NOT set the CUSTOM_EDIT bit.
	*/
	flags = 0;
	iflags = LO_I_LAST;
	if (LOinfo(&loc, &iflags, &info) == OK)
	{
		IIUGdtsDateToSys(gdstr,&gen);
		if (TMcmp(&gen,&(info.li_last)) < 0)
			flags = CUSTOM_EDIT;
	}
	/*
	** Now mess with the form date.
	*/
	fname = NULL;
	for (d = arec->dep_on; d != NULL; d = d->_next)
	{
		if (d->class == OC_FORM || d->class == OC_AFORMREF)
		{
			fname = d->name;
			break;
		}
	}
	/*
	** for all forms which have NOT been user
	** editted, we replace the date with an
	** artificial date which will replaced by
	** NOW on copyapp in, thereby preserving
	** the direction.
	*/
	if (fname != NULL && *((arec->arg)[4]) != EOS)
	{
		IIAMfoiFormInfo(fname, lbuf, &junk);
		IIUGdtsDateToSys(lbuf,&fdate);
		IIUGdtsDateToSys((arec->arg)[4],&gen);
		if (TMcmp(&fdate,&gen) < 0)
			(arec->arg)[4] = Funky_date;
	}
	arec->flags |= flags;	/* ORed to fix bug 43378 */
}
/*{
** Name:	IICAbdfBackDateForm
**
** Description:
**	Assure that form is backdated.  This is used when we do not
**	copy in a new form because one already exists in the database.
**	We want to assure a gendate previous to the form date.
**
** Inputs:
**	applid - application id.
**	name - form name.
**
** History:
**	3/90 (bobm)	initial version.
*/
VOID
IICAbdfBackDateForm(applid,name)
  i4 applid;
  char *name;
{
  i4 c1, c2;
	iiuicw1_CatWriteOn();
	c1 = OC_FORM;
	c2 = OC_AFORMREF;
/* # line 208 "metautil.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_abfobjects set abf_arg5=");
    IIputdomio((short *)0,1,32,0,Back_date);
    IIwritio(0,(short *)0,1,32,0,(char *)"where applid=");
    IIputdomio((short *)0,1,30,4,&applid);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and object_id in(select object_id from ii_abfdependencies where abfde\
f_name=");
    IIputdomio((short *)0,1,32,0,name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and(object_class=");
    IIputdomio((short *)0,1,30,4,&c1);
    IIwritio(0,(short *)0,1,32,0,(char *)"or object_class=");
    IIputdomio((short *)0,1,30,4,&c2);
    IIwritio(0,(short *)0,1,32,0,(char *)"))");
    IIsyncup((char *)0,0);
  }
/* # line 215 "metautil.sc" */	/* host code */
}
/*{
** Name:	IICApfdPatchFormDates
**
** Description:
**	Patch up formgen dates following a copyapp in.  All those with "Funky
**	date" should be replaced by "now".
**
** Inputs:
**	applid - application id.
**
** History:
**	3/90 (bobm)	initial version.
*/
VOID
IICApfdPatchFormDates(applid)
OOID applid;
{
  char wbuf[120];
  char *nowdate;
	nowdate = UGcat_now();
	iiuicw1_CatWriteOn();
	STprintf(wbuf, ERx("abf_arg5 = '%s' and applid = %d"),
				Funky_date, applid);
/* # line 245 "metautil.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_abfobjects set abf_arg5=");
    IIputdomio((short *)0,1,32,0,nowdate);
    IIwritio(0,(short *)0,1,32,0,(char *)"where ");
    IIwritio(1,(short *)0,1,32,0,wbuf);
    IIsyncup((char *)0,0);
  }
/* # line 248 "metautil.sc" */	/* host code */
}
/*{
** Name:	IICAmfwMetaFrameWrite
**
** Description:
**	Write out the auxiliary catalogs for a metaframe.  The access used
**	by copyapp doesn't construct the metaframe, and we don't want to
**	bother unravelling it anyway, so we just come through here to
**	fetch the extra catalogs and write them.
**
** Inputs:
**	id - object id of frame.
**	fp - file.
**
** History:
**	3/90 (bobm)	initial version.
*/
VOID
IICAmfwMetaFrameWrite(id,fp)
  i4 id;
FILE *fp;
{
  i4 n1, n2, n3, n4, n5, n6, n7;
  char nm1[FE_MAXNAME+1], nm2[FE_MAXNAME+1], nm3[FE_MAXNAME+1];
  char a[DB_GW3_MAXTUP], *p1, *p2;
	bool first;
	i4 count;
	char tmp;
	char be[CU_LINEMAX];
	/*
	** NOTE:
	**
	** The order of writing tokens out to the file here MUST
	** match the order defined in the CURECORD templates used to
	** read them back in.  Those templates are defined in the
	** copyutil library.
	*/
	first = TRUE;
/* # line 294 "metautil.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select vq_seq, join_type, join_tab1, join_tab2, join_col1, join_col2 \
from ii_vqjoins where object_id=");
    IIputdomio((short *)0,1,30,4,&id);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&n1);
      IIgetdomio((short *)0,1,30,4,&n2);
      IIgetdomio((short *)0,1,30,4,&n3);
      IIgetdomio((short *)0,1,30,4,&n4);
      IIgetdomio((short *)0,1,30,4,&n5);
      IIgetdomio((short *)0,1,30,4,&n6);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 300 "metautil.sc" */	/* host code */
		if (first)
		{
			cu_wrcomp(CUC_VQJOIN, fp);
			first = FALSE;
		}
		SIfprintf(fp, ERx("\t%d\t%d\t%d\t%d\t%d\t%d\n"),
			n1, n2, n3, n4, n5, n6);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	first = TRUE;
/* # line 310 "metautil.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select vq_seq, vq_mode, tab_name, tab_owner, tab_section, tab_usage, \
tab_flags from ii_vqtables where object_id=");
    IIputdomio((short *)0,1,30,4,&id);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&n1);
      IIgetdomio((short *)0,1,30,4,&n2);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,nm1);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,nm2);
      IIgetdomio((short *)0,1,30,4,&n3);
      IIgetdomio((short *)0,1,30,4,&n4);
      IIgetdomio((short *)0,1,30,4,&n5);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 316 "metautil.sc" */	/* host code */
		if (first)
		{
			cu_wrcomp(CUC_VQTAB, fp);
			first = FALSE;
		}
		SIfprintf(fp, ERx("\t%d\t%d\t%s\t%s\t%d\t%d\t%d\n"),
			n1, n2, nm1, nm2, n3, n4, n5);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
	first = TRUE;
/* # line 326 "metautil.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select vq_seq, tvq_seq, col_name, ref_name, adf_type, adf_length, adf\
_scale, col_flags, col_sortorder, col_info from ii_vqtabcols where obj\
ect_id=");
    IIputdomio((short *)0,1,30,4,&id);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&n1);
      IIgetdomio((short *)0,1,30,4,&n2);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,nm1);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,nm2);
      IIgetdomio((short *)0,1,30,4,&n3);
      IIgetdomio((short *)0,1,30,4,&n4);
      IIgetdomio((short *)0,1,30,4,&n5);
      IIgetdomio((short *)0,1,30,4,&n6);
      IIgetdomio((short *)0,1,30,4,&n7);
      IIgetdomio((short *)0,1,32,DB_GW3_MAXTUP-1,a);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 333 "metautil.sc" */	/* host code */
		if (first)
		{
			cu_wrcomp(CUC_VQTCOL, fp);
			first = FALSE;
		}
		SIfprintf(fp,
		ERx("\t%d\t%d\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%s\n"),
		n1, n2, nm1, nm2, n3, n4, n5, n6, n7, a);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
	first = TRUE;
/* # line 344 "metautil.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select mu_text, mu_field, mu_column, mu_expr, mu_seq from ii_menuargs\
 where object_id=");
    IIputdomio((short *)0,1,30,4,&id);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE4;
IIrtB4:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,nm1);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,nm2);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,nm3);
      IIgetdomio((short *)0,1,32,DB_GW3_MAXTUP-1,a);
      IIgetdomio((short *)0,1,30,4,&n1);
      if (IIerrtest() != 0) goto IIrtB4;
/* # line 349 "metautil.sc" */	/* host code */
		if (first)
		{
			cu_wrcomp(CUC_MENUARG, fp);
			first = FALSE;
		}
		SIfprintf(fp, ERx("\t%s\t%s\t%s\t%s\t%d\n"), 
			  nm1, nm2, nm3, a, n1);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE4:;
  }
	first = TRUE;
	p1 = a;
	p2 = a+DB_GW3_MAXTUP/2;
/* # line 361 "metautil.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select fr_seq, var_field, var_column, var_datatype, var_comment from \
ii_framevars where object_id=");
    IIputdomio((short *)0,1,30,4,&id);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE5;
IIrtB5:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&n1);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,nm1);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,nm2);
      IIgetdomio((short *)0,1,32,0,p1);
      IIgetdomio((short *)0,1,32,0,p2);
      if (IIerrtest() != 0) goto IIrtB5;
/* # line 367 "metautil.sc" */	/* host code */
		if (first)
		{
			cu_wrcomp(CUC_FRMVAR, fp);
			first = FALSE;
		}
		SIfprintf(fp, ERx("\t%d\t%s\t%s\t%s\t%s\n"),
				n1, nm1, nm2, p1, p2);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE5:;
  }
	first = TRUE;
	count = 0;
/* # line 378 "metautil.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select encode_sequence, encode_estring from ii_encodings where encode\
_object=");
    IIputdomio((short *)0,1,30,4,&id);
    IIwritio(0,(short *)0,1,32,0,(char *)"order by encode_sequence");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE6;
IIrtB6:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&n1);
      IIgetdomio((short *)0,1,32,DB_GW3_MAXTUP-1,a);
      if (IIerrtest() != 0) goto IIrtB6;
/* # line 384 "metautil.sc" */	/* host code */
		if (first)
		{
			cu_wrcomp(CUC_ESCAPE, fp);
			first = FALSE;
		}
		/*
		** we split these overly long records into multiple
		** records, so the lines in the file aren't too long.
		** It should not matter that the column is not
		** filled out on the target side.
		*/
		p1 = a;
		while (*p1 != EOS)
		{
			n2 = STlength(p1);
			if (n2 > ESCSPLIT)
			{
				/*
				** if splitting on ESCSPLIT would leave
				** a really short buffer, split more
				** evenly.  This is a cosmetic nicety.
				*/
				if (n2 > (ESCSPLIT+20))
					n2 = ESCSPLIT;
				else
					n2 = ESCSPLIT/2;
			}
			tmp = p1[n2];
			p1[n2] = EOS;
			/*
			** This turns tabs and newlines into \t, \n so
			** that they don't confuse copyapp token parsing.
			** For consistency, it translates all controls.
			*/
			IICUbseBSEncode(p1,be);
			SIfprintf(fp, ERx("\t%d\t%s\n"), count, be);
			++count;
			p1 += n2;
			*p1 = tmp;
		}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE6:;
  }
}
/*{
** Name:	IICAaslApplSourceLoc
**
** Description:
**	Set application source directory location for use in
**	IICAmtMetaTwiddle.
**
** Inputs:
**	path - path from catalog.
**
** History:
**	3/90 (bobm)	initial version.
*/
VOID
IICAaslApplSourceLoc(path)
char *path;
{
	STcopy(path,Sbuf);
	LOfroms(PATH,Sbuf,&Sloc);
}
/*{
** Name:	IICA4nf4GLNewFrame
**
** Description:
**	Update a single frame to indicate that we need regeneration.
**	If CUSTOM_EDIT is set, this means that we think we're supposed
**	to generate the 4GL, but we found a source file in the target
**	location.  In this case, we will simply back date the gen date,
**	rather than flagging as new.
**
** Inputs:
**	oid - object id for frame.
**	flags - new flags for frame.
**
** History:
**	3/90 (bobm)	initial version.
*/
VOID
IICA4nf4GLNewFrame(oid, flags)
  i4 oid;
  i4 flags;
{
  char buf[49];
	i4 mf;
	char *ptr;
	buf[0] = EOS;
	iiuicw1_CatWriteOn();
/* # line 488 "metautil.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select abf_arg6 from ii_abfobjects where object_id=");
    IIputdomio((short *)0,1,30,4,&oid);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,48,buf);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 491 "metautil.sc" */	/* host code */
	if (buf[0] != EOS)
	{
		mf = 0;
		ptr = STindex(buf,"@",0);		/* 6-x_PC_80x86 */
		if (ptr != NULL)
		{
			*ptr = EOS;
			CVan(buf,&mf);
		}
		if ((flags & CUSTOM_EDIT) == 0)
		{
			mf |= MFST_NEWFR;
			if ((mf & MFST_VQEERR) != 0)
				mf |= MFST_DOGEN|MFST_DOFORM;
			STprintf(buf,"%d@",mf);
		}
		else
		{
			mf &= ~(MFST_NEWFR|MFST_DOGEN|MFST_DOFORM);
			STprintf(buf,"%d@%s",mf,Back_date);
		}
/* # line 514 "metautil.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_abfobjects set abf_arg6=");
    IIputdomio((short *)0,1,32,0,buf);
    IIwritio(0,(short *)0,1,32,0,(char *)", abf_flags=");
    IIputdomio((short *)0,1,30,4,&flags);
    IIwritio(0,(short *)0,1,32,0,(char *)"where object_id=");
    IIputdomio((short *)0,1,30,4,&oid);
    IIsyncup((char *)0,0);
  }
/* # line 517 "metautil.sc" */	/* host code */
	}
}
