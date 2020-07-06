# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 1985, 2005 Ingres Corporation
**
*/
# include	<compat.h>
# include	<cv.h>		/* 6-x_PC_80x86 */
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ug.h>
# include	"decls.h"
# include	"vffiles.h"
# include	<encoding.h>
# include	<ds.h>
# include	<feds.h>
# include	<lo.h>
# include	<si.h>
# include	<nm.h>
# include <ooclass.h>
# include	<me.h>
# include	<er.h>
# include	<ui.h>
# include	<cm.h>
# include	"ervf.h"
# ifdef	__HIGHC__
#pragma    Static_segment("VIF1DATA");
# endif
/**
** Name: fasttodb.qsc -	Saves a frame out to the database
**
** Description:
**	Fasttodb saves a frame to the database in two different ways.  If
**	running against an INGRES back end, the frame (fields, trim
**	are dumped into intermediate files.  SQL COPY then
**	copies the frame into the database.  If running against a gateway,
**	the frame is saved directly to the database, using SQL REPEAT INSERT. 
**	The reason being only INGRES SQL supports COPY.
**
**
** Defines: 
**	vfallfiles()		- sets up the name of the intermediate files
**	vfopfiles()		- opens the intermediate files
**	vfclfiles()		- closes the intermediate files
**	dumpFrm(frame)		- saves frame to ii_forms
**	IIVFdfd_dumpFds(frame, fl, numFields, dest)
**				- dumps all fields of the frame to destination
**	vfcpflds(file) 		- copies the fields from the intermediate file
**				  to the database
**	IIVFdrg_DumpReg(fld, outfp, dest)
**				- sets up parameters to dump regular fields 
**	IIVFdtb_DumpTab(tab, outfp, dest)
**				- dumps the table field to file/database, sets
**				  up parameters to dump regular fields
**	IIVFcfd_CopyFds(hdr, type, val, fldtype, outfp, dest)
**				- dumps fields to file/database
**	IIVFctr_CopyTrim(frame, outfp, dest)
**				- dumps trim to file/database
**	vfcptrms(file)		- copies the trim from the intermediate file
**				  to the database
**	IIVFccf_CompFrm(frame)	- dumps the encoded forms to the database
**	vfbslash(src, dest)	- searches for backslashes in string
**	IIVFint_InitSub()	- initializes before saving the frame.
**
** History:
**	2/85	(ac)	- Made necessary changes for vifred
**			  calling writefrm module at different
**			  point.
**	10/10/85 (dkh)	- Changed to not syserr if temp files
**			  could not be opened.
**	03/14/87 (dkh) - Added support for ADTs.
**	2-dec-86 (bruceb)	Fix for QA test nf012 in fefunc.
**		Changed INnetrequest() to INdiffarch(0) in decision
**		to use iicompfrm catalog.  (Done since INnetrequest()
**		always returns FALSE on UNIX)
**	05/06/87 (dkh) - Fixed VMS compile problems. (dkh)
**	05/21/87 (dkh) - Integrated VMS changes. (dkh)
**	13-jul-87 (bruceb) Changed memory allocation to use [FM]Ereqmem.
**	08/14/87 (dkh) - ER changes.
**	09/30/87 (dkh) - Changed to do appends for encoded forms update.
**	09/30/87 (dkh) - Code cleanup.
**	10/03/87 (dkh) - Deleted reference to INnetrequest.
**	10/12/87 (dkh) - Changed to use repeat appends for encoded forms.
**	10/17/87 (dkh) - Changed to not use single letter range vars.
**	10/22/87 (dkh) - Changed INdiffarch to IIUGdaDiffArch.
**	10/25/87 (dkh) - Error message cleanup.
**	11/28/87 (dkh) - Freed memory allocated in dumpCompFrm().
**	03-dec-87 (bruceb)
**		Changed from a qc to a qsc file; include qsh files.
**	04-dec-87 (bruceb)
**		Save the real value of frmflags instead of just 0.
**	12/19/87 (dkh) - Including .qsh instead of .qh headers.
**	30-dec-87 (sylviap)
**		Changed the QUEL statements to SQL.
**		Deleted the ifdef HACKFOR50.
**		Integrated the 'inserts' for copying in a compiled form.
**		Changed the FEmsg to IIUGerr.		
**	01/01/88 (dkh) - Fixed jup bug 1701.
**	07-jan-88 (sylviap)
**		If running against a gateway, forms are copied to database w/o
**		  using an intermediate file.
**		Deleted all references to VFCF - intermediate file for compiled
**		  forms.
**		Combined dumpCompFrm and convCompFrm into one routine, 
**		  IIVFccf_CompFrm.
**	11-jan-88 (bruceb)
**		Zap reference to 'in' CL module.  No more checks on
**		INnetrequest(), and IIUGdaDiffArch() used in place of
**		INdiffarch(0).  (Premature addition of changes already
**		made by dkh on dragon.)
**	20-jan-88 (sylviap)
**		Integrated the function of writing out to an intermediate file 
**		  and database into one routine.
**		Deleted procedure vfnonode.
**	11-feb-88 (sylviap)
**		Created IIVFint_InitSub to set subseq to zero.
**	17-feb-88 (sylviap)
**		When dumping frame to ii_forms, sets fr2flags.  This 
**		accommodates the new scrolling bit.
**	24-feb-88 (sylviap)
**		Lower case all internal field names.
**	01-mar-88 (sylviap)
**		Need to lower case table name when writing out to 
**		ii_encoded_forms.
**	03/03/88 (dkh) - Changes to save routines called by OO.
**	14-mar-88 (sylviap)
**		Deleted reference to FEingenqerr.
**	15-mar-88 (sylviap)
**		Added to SQL INSERT the null string to accommodate backends
**		that do not allow default columns.
**	17-may-88 (sylviap)
**		Replaced constants with fdTILEN, fdSTRLEN and fdTRIMLEN.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**	01/13/89 (dkh) - Added code to clear out checksum values for
**			 field names before writing to db.
**	05-apr-89 (sylviap)
**		Modified INSERT to REPEAT INSERT. Gateway SQL now supports
**		REPEAT queries.
**	28-feb-1989 (wolf)	PREPARE and EXECUTE insert queries on SQL/DS
**				long fields
**	31-may-89 (bruceb)
**		Don't do anything with compiled version of the form if on
**		a different architecture--move the IIUGda check before the call
**		on DSstore.
**	07/22/89 (dkh) - Fixed bug 7017.
**	07/23/89 (dkh) - Integrated changes from IBM.
**	09/27/89 (dkh) - Porting changes integration.
**	10/06/89 (dkh) - Fixed bug 2115.
**	11/10/89 (dkh) - Added changes to handle different size for
**			 ii_encoded_forms catalog due to B1.
**	13-dec-89 (bruceb)	Fix for bug 8413.
**		In IIVFccf_compfrm(), use SIfopen() for everyone, not just
**		for CMS.  This is the mode used by DSstore().
**	12/15/89 (dkh) - VMS shared lib changes.
**	09/05/90 (dkh) - Changed to use IIFDgfdGetFdDstab() instead of
**			 FdDStab.
**	03/24/91 (dkh) - Integrated changes from PC group.
**	08/19/91 (dkh) - Integrated changes from PC group.
**	10/14/92 (dkh) - Removed use of ArrayOf in favor of DSTARRAY.
**			 ArrayOf confused the alpha C compiler.
**	19-aug-93 (kirke)
**	    Bug #51348 - Rewrote vfbslash() using CM routines to handle
**	    multibyte properly.
**	08/31/93 (dkh) - Fixed up compiler complaints due to prototyping.
**	10/10/93 (dkh) - Moved include of fe.h before ug.h to fix compile
**			 problems.
**	22-jun-1999 (musro02) - Revised pragma to #pragma
**      15-feb-2000 (musro02)
**              Corrected pragma to #pragma
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	07-sep-2005 (abbjo03)
**	    Replace xxx_FILE constants by SI_xxx.
**	15-oct-2007 (toumi01)
**	    For UTF8 force ii_encoded_forms.cfdata type to DB_VBYTE_TYPE.
*/
/* # define's */
# define	TO_DB		FALSE
# define	TO_FILE		TRUE
/* GLOBALDEF's */
GLOBALDEF 	LOCATION	VFFLD = {0};
GLOBALDEF 	LOCATION	VFTRD = {0};
GLOBALDEF	bool		vfuseiicomp = TRUE;
/* extern's */
FUNC_EXTERN	DSTARRAY	*IIFDgfdGetFdDstab();
FUNC_EXTERN	i4		IIUIdml();
FUNC_EXTERN	VOID		vfallfiles();
FUNC_EXTERN	VOID		vfbslash();
FUNC_EXTERN	i4		IIFDesEncodeSize();
/* static's */
static	LOCATION	INGTEMP = {0};
static	char		vffldbuf[MAX_LOC + 1] = {0};
static	char		vftrdbuf[MAX_LOC + 1] = {0};
static	char		*ASTRING = ERx("\t%s");
static	char		*ANUMBER = ERx("\t%d");
  static i4 subseq = 0;
  static i4 cfsize = 0;
  static char *cfblock = NULL;
  GLOBALDEF char *fname = NULL;
  GLOBALDEF OOID vffrmid = 0;
/*{
** Name: vfallfiles - sets up the name for the intermediate files
**
** Description:
**	Sets up the intermediate file names with a unique extension.
**
** Inputs:
**	None.
**
** Outputs:
**	None.
**
** History:
**
*/
VOID
vfallfiles()
{
	NMloc(TEMP, PATH, NULL, &INGTEMP);
	LOcopy(&INGTEMP, vffldbuf, &VFFLD);
	LOuniq(ERx("vffld"), NULL, &VFFLD);
	LOcopy(&INGTEMP, vftrdbuf, &VFTRD);
	LOuniq(ERx("vftrd"), NULL, &VFTRD);
}
/*{
** Name: vfopfiles - opens the intermediate files.
**
** Description:
**	Open the intermediate files used to dump the frame.
**
**  	VIFRED now just puts out an error message if it could not open
**  	temp files instead of causing a syserr. (dkh)
**
** Inputs:
**	None.
**
** Outputs:
**	None.
** History:
**	<manual entries>
*/
STATUS
vfopfiles()
{
	vfallfiles();
	/*
	** We try and open the copy files before doing any
	** processing so no changes will be made to the database
	** unless all copy processing can be completed.
	*/
	if (SIopen(&VFFLD, ERx("w"), &vfFldFile) != OK)
	{
		IIUGerr(E_VF0060_Unable_to_open_a_temp, UG_ERR_ERROR, 0);
		return(FALSE);
	}
	if (SIopen(&VFTRD, ERx("w"), &vfTrFile) != OK)
	{
		SIclose(vfFldFile);
		LOdelete(&VFFLD);
		IIUGerr(E_VF0060_Unable_to_open_a_temp, UG_ERR_ERROR, 0);
		return(FALSE);
	}
	return(TRUE);
}
/*{
** Name: vfclfiles - closes the intermediate files.
**
** Description:
**	Closes the intermediate files used to dump the frame.
**
** Inputs:
**	None.
**
** Outputs:
**	None.
** History:
**	<manual entries>
*/
VOID
vfclfiles()
{
	SIclose(vfFldFile);
	SIclose(vfTrFile);
}
/*{
** Name: dumpFrm - saves the frame definition to ii_forms
**
** Description:
**	Deletes the old frame definition if any exists.  Then saves the 
**	current frame definition to ii_forms.
**
** Inputs:
**	FRAME	*frm			- contains the frame definition
**		i4	frfldno;		- number of fields
**		i4	frnsno;			- num of disponly fields ( 0 )
**		i4	frtrimno;		- number of trim strings 
**		char	frname[36];		- frame name 
**		i4	frmaxx,frmaxy;		- frame size 
**		i4	frposx,frposy;		- frame position
**		i4	frmflags;		- flags (box,standout,etc.)
**		(partial definition)
**
** Outputs:
**	None.
**
** History:
**	<manual entries>
*/
STATUS
dumpFrm(frm)
FRAME	*frm;
{
  i4 numFld;
  i4 numNonSeq;
  i4 numTrm;
  char *name;
  i4 col;
  i4 lines;
  i4 posy;
  i4 posx;
  i4 flagval;
  i4 version;
  i4 flag2val;
  i4 scrtype = 0;
  i4 scrmaxx = 0;
  i4 scrmaxy = 0;
  i4 scrdpix = 0;
  i4 scrdpiy = 0;
	/*
	**  Fix for BUG 7768. (dkh)
	**  This fix is in the "frflags" line below where it used to
	**  just assign a 0.  Now "flagval" is assigned to "frflags".
	**  This is necessary to sidestep a backend parser problem
	**  where II_DECIMAL is set to ",".
	*/
	numFld = frm->frfldno;
	numNonSeq = frm->frnsno;
	numTrm = frm->frtrimno;
	name = frm->frname;
	CVlower(name);
	col = frm->frmaxx;
	lines = frm->frmaxy;
	posy = frm->frposy;
	posx = frm->frposx;
	version = FRMVERSION;
	flagval = frm->frmflags;
	flag2val = frm->frm2flags;
/* # line 359 "fasttodb.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_forms(object_id, frmaxcol, frmaxlin, frposy, frposx, f\
rfldno, frnsno, frtrimno, frflags, frversion, frscrtype, frscrmaxx, fr\
scrmaxy, frscrdpix, frscrdpiy, fr2flags, frtotflds)values(");
    IIputdomio((short *)0,1,30,4,&vffrmid);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&col);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&lines);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&posy);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&posx);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&numFld);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&numNonSeq);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&numTrm);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&flagval);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&version);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&scrtype);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&scrmaxx);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&scrmaxy);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&scrdpix);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&scrdpiy);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&flag2val);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&subseq);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 367 "fasttodb.sc" */	/* host code */
	if (FEinqerr())
	{
		return(FAIL);
	}
	return(OK);
}
/*{
** Name: IIVFdfd_DumpFds - dumps all fields to destination
**
** Description:
**	All fields, normal and non-sequenced fields are dumped out to the
**	destination.  Normal fields can be of two types, regular (simple)
**	or table fields.  Non-sequenced (display opnly) fields are of the 
**	regular field type only.
**
**	Destination is determined by the type of back-end.  If running against 
**	an INGRES back-end, SQL COPY is used to dump the frame into the 
**	database, so destination is TO_FILE (TRUE).  The frame is copied to an
**	intermediate file, then copied into the database.  If running against
**	a gateway, destination is TO_DB (FALSE), because SQL COPY is not 
**	supported.  The frame is dumped directly into the database using SQL
**	INSERT.
**
** Inputs:
**	 FRAME		*frm 		- the frame to be saved
**	 FIELD		**fl   		- the array of fields
**	 i4		numFields	- number of fields
**	 FILE		*outfp		- output file
**	 bool		dest		- destination of dump
**					    TO_FILE = TRUE
**					    TO_DB   = FALSE
**
**
** Outputs:
**	None
**
** History:
**
*/
STATUS
IIVFdfd_DumpFds(frm, fl, numFields, outfp, dest)
reg	 FRAME	*frm;
reg	 FIELD	**fl;
register i4	numFields;
FILE		*outfp;
bool		dest;
{
	register FIELD	*fd;
	for (; numFields > 0; numFields--, fl++)
	{
		fd = *fl;
		if (fd->fltag == FREGULAR)
		{
			if (IIVFdrg_DumpReg 
				(fd->fld_var.flregfld, outfp, dest) != OK)
				return (FAIL);
		}
		else
		{
			if (IIVFdtb_DumpTab 
				(fd->fld_var.fltblfld, outfp, dest) != OK)
				return (FAIL);
		}
	}
	return (OK);
}
/*{
** Name: vfcpflds - copies the fields into ii_fields
**
** Description:
**	Copies the fields of a frame from an intermediate file into ii_fields.
**      This routine should only be use against an INGRES back-end or a back-end
**	that supports SQL COPY.
**
** Inputs:
**	LOCATION	*file - intermediate file to copy from
**
** Outputs:
**	None.
**
** History:
**
*/
STATUS
vfcpflds(file)
LOCATION	*file;
{
	char	*filename;
	LOtos(file, &filename);
	fname = filename;
# ifdef CMS
	/*
	**    Since there is no such thing as a null (empty) file in CMS
	**    we must check to make sure the location physically exists.
	**    If it does not, if was probably supposed to be a null
	**    dataset, so we will just return and skip the 'copy from'.
	*/
	if (LOexist(file)!=OK) return(FAIL);
# endif
/* # line 466 "fasttodb.sc" */	/* copy table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"copy table ii_fields(object_id=c0tab, flseq=c0tab, fldname=c0tab, fld\
atatype=c0tab, fllength=c0tab, flprec=c0tab, flwidth=c0tab, flmaxlin=c\
0tab, flmaxcol=c0tab, flposy=c0tab, flposx=c0tab, fldatawidth=c0tab, f\
ldatalin=c0tab, fldatacol=c0tab, fltitle=c0tab");
    IIwritio(0,(short *)0,1,32,0,(char *)
", fltitcol=c0tab, fltitlin=c0tab, flintrp=c0tab, fldflags=c0tab, fld2\
flags=c0tab, fldfont=c0tab, fldptsz=c0tab, fldefault=c0tab, flformat=c\
0tab, flvalmsg=c0tab, flvalchk=c0tab, fltype=c0tab, flsubseq=c0nl)from\
 ");
    IIvarstrio((short *)0,1,32,0,fname);
    IIsyncup((char *)0,0);
  }
/* # line 497 "fasttodb.sc" */	/* host code */
	if (FEinqerr())
	{
		return(FAIL);
	}
	return(OK);
}
/*{
** Name: IIVFdrg_DumpReg - sets up parameters to dump regular fields 
**
** Description:
**	Calls IIVFcfd_CopyFds to copy the fields to the destination.  This
**	routine was implemented to be able to stay within the same structure
**	when saving table fields.
**
** Inputs:
**	REGFLD	*fld 	- field to be dumped
**	FILE	*outfp	- output file
**	bool	dest	- destination of dump
**			    TO_FILE = TRUE
**			    TO_DB   = FALSE
**
** Outputs:
**	None.
**
** Side Effects:
**
** History:
**	<manual entries>
*/
STATUS
IIVFdrg_DumpReg(fld, outfp, dest)
REGFLD	*fld;
FILE	*outfp;
bool	dest;
{
	if (IIVFcfd_CopyFds(&fld->flhdr, &fld->fltype, &fld->flval,
		(i4) FREGULAR, outfp, dest) != OK)
		return (FAIL);
	return (OK);
}
/*{
** Name: IIVFdtb_DumpTab - dumps a table field to the destination
**
** Description:
**	Dumps a table field out to the destination specified in the parameter
**	dest.  When running against an INGRES back-end only, backslashes in 
**	the internal name and title are doubled.  The single backslash is 
**	interpreted as an ESC character.
**
**	Actual table rows are dumped out by calling IIVFcfd_CopyFds.
**
** Inputs:
**	TBLFLD	*tab	- table to be dumped
**		FLDHDR	tfhdr			- table field header info.
**			char	*fhtitle	- title for field 
**			i4	fhseq		- field sequence number 
**			i4	fhmaxx,fhmaxy   - number of cols and lines 
**			i4	fhposx,fhposy	- position of field window 
**			char	fhdname[36]	- field name 
**			i4	fhtitx,fhtity   - position of title in field 
**			i4	fhintrp		- interrupt code for field 
**			i4	fhdflags	- flags (standout,box,etc.) 
**						- for a table field header 
**						  this is a mode field taken 
**						  from fdtf values 
**			i4	fhd2flags	- more space for flags 
**		i4	tfrows			- number of rows displayed 
**		i4	tfcols			- number of columns 
**	    	i4	tfstart			- line number of start of data 
**						  rows 
**	    	i4	tfwidth			- number of physical lines 
**						  per row 
**		(partial definition of TBLFLD)
**	FILE	*outfp	- output file
**	bool	dest	- destination of dump
**			    TO_FILE = TRUE
**			    TO_DB   = FALSE
**
** Outputs:
**	None.
**
** History:
**	<manual entries>
*/
STATUS
IIVFdtb_DumpTab(tab, outfp, dest)
TBLFLD	*tab;
FILE	*outfp;
bool	dest;
{
  char title[fdTILEN + 1];
  char intrname[FE_MAXNAME + 1];
  i4 t_fhseq;
  i4 t_fhtitx;
  i4 t_fhtity;
  i4 t_fhposx;
  i4 t_fhposy;
  i4 t_fhmaxx;
  i4 t_fhmaxy;
  i4 t_fhintrp;
  i4 t_fhdflags;
  i4 t_fhd2flags;
  i4 t_tfrows;
  i4 t_tfcols;
  i4 t_tfstart;
  i4 t_tfwidth;
  i4 t_ftable;
  i4 t_subseq;
  char nchar[2];
	FLDHDR	*hdr;
	i4	i;
	FLDCOL	**col;
	FLDVAL	*win;
	hdr = &tab->tfhdr;
	if (dest == TO_FILE)
	{
		/*
		**	Check for backslashes in internal names and titles.
		*/
		CVlower(hdr->fhdname);
		vfbslash(hdr->fhdname, intrname);
		vfbslash(hdr->fhtitle, title);
		hdr->fhdfont = 0;
		SIfprintf(outfp, ERx("%d"), vffrmid);
		SIfprintf(outfp, ANUMBER, hdr->fhseq);
		SIfprintf(outfp, ASTRING, intrname);
		SIfprintf(outfp, ANUMBER, 0);			/*fldatatype*/
		SIfprintf(outfp, ANUMBER, tab->tfrows); 	/*fllength*/
		SIfprintf(outfp, ANUMBER, 0);			/* flprec */
		SIfprintf(outfp, ANUMBER, tab->tfcols);	   	/*flwidth*/
		SIfprintf(outfp, ANUMBER, hdr->fhmaxy);
		SIfprintf(outfp, ANUMBER, hdr->fhmaxx);
		SIfprintf(outfp, ANUMBER, hdr->fhposy);
		SIfprintf(outfp, ANUMBER, hdr->fhposx);
		SIfprintf(outfp, ANUMBER, tab->tfwidth);	/*fldatawidth*/
		SIfprintf(outfp, ANUMBER, tab->tfstart);   	/* fldatalin */
		SIfprintf(outfp, ANUMBER, 0);			/* fldatacol */
		SIfprintf(outfp, ASTRING, title);
		SIfprintf(outfp, ANUMBER, hdr->fhtitx);
		SIfprintf(outfp, ANUMBER, hdr->fhtity);
		SIfprintf(outfp, ANUMBER, hdr->fhintrp);
		SIfprintf(outfp, ANUMBER, hdr->fhdflags);
		SIfprintf(outfp, ANUMBER, hdr->fhd2flags);
		SIfprintf(outfp, ANUMBER, 0);			/* fldfont */
		SIfprintf(outfp, ANUMBER, 0);			/* fldptsz */
		SIfprintf(outfp, ASTRING, ERx("\0"));		/*fldefault*/
		SIfprintf(outfp, ASTRING, ERx("\0"));		/*flformat*/
		SIfprintf(outfp, ASTRING, ERx("\0"));		/*flvalmsg*/
		SIfprintf(outfp, ASTRING, ERx("\0"));		/*flvalstr*/
		SIfprintf(outfp, ANUMBER, FTABLE);
		SIfprintf(outfp, ERx("\t%d\n"), subseq++);
	}
	else			/*  destination is the database */
	{
		/*	
		**  Temporary variables are assigned before the SQL INSERT.  
		**  This is done to keep frame.h instead of changing it to 
		**  frame.qsh.  Now SQL only needs to know the temp variables 
		**  as oppposed to knowing the definitions of the data 
		**  structures.  The cost is in the performance to copy the 
		**  values to the temp. variables.  Advantage is in this
		**  file ONLY does SQL need to know the structure definitions, 
		**  so will not need to change all the INCLUDEs in other files 
		**  to frame.qsh. (sp)
		*/ 
		CVlower(hdr->fhdname);
		STcopy(hdr->fhdname, intrname);
		STcopy(hdr->fhtitle, title);
		hdr->fhdfont = 0;
		nchar[0] = EOS;				/* create a null 
							   string */
		t_fhseq = hdr->fhseq;
		t_tfrows = tab->tfrows;			/*fllength*/
		t_tfcols = tab->tfcols;			/*flwidth*/
		t_fhmaxy = hdr->fhmaxy;
		t_fhmaxx = hdr->fhmaxx;
		t_fhposy = hdr->fhposy;
		t_fhposx = hdr->fhposx;
		t_tfwidth = tab->tfwidth;		/*fldatawidth*/
		t_tfstart = tab->tfstart;		/* fldatalin */
		t_fhtitx = hdr->fhtitx;
		t_fhtity = hdr->fhtity;
		t_fhintrp = hdr->fhintrp; 
		t_fhdflags = hdr->fhdflags;
		t_fhd2flags =  hdr->fhd2flags;
		t_ftable = FTABLE;
		t_subseq = subseq++;
		/*
		**  	Fields - fldefault, flformat, flvalmsg, flvalstr are 
		**	NULL in table fields, so they are not part of the 
		**	INSERT.
		*/
/* # line 691 "fasttodb.sc" */	/* repeat insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"fasttodb1",7350,1211);
      IIputdomio((short *)0,1,30,4,&vffrmid);
      IIputdomio((short *)0,1,30,4,&t_fhseq);
      IIputdomio((short *)0,1,32,0,intrname);
      IIputdomio((short *)0,1,30,4,&t_tfrows);
      IIputdomio((short *)0,1,30,4,&t_tfcols);
      IIputdomio((short *)0,1,30,4,&t_fhmaxy);
      IIputdomio((short *)0,1,30,4,&t_fhmaxx);
      IIputdomio((short *)0,1,30,4,&t_fhposy);
      IIputdomio((short *)0,1,30,4,&t_fhposx);
      IIputdomio((short *)0,1,30,4,&t_tfwidth);
      IIputdomio((short *)0,1,30,4,&t_tfstart);
      IIputdomio((short *)0,1,32,0,title);
      IIputdomio((short *)0,1,30,4,&t_fhtitx);
      IIputdomio((short *)0,1,30,4,&t_fhtity);
      IIputdomio((short *)0,1,30,4,&t_fhintrp);
      IIputdomio((short *)0,1,30,4,&t_fhdflags);
      IIputdomio((short *)0,1,30,4,&t_fhd2flags);
      IIputdomio((short *)0,1,30,4,&t_ftable);
      IIputdomio((short *)0,1,30,4,&t_subseq);
      IIputdomio((short *)0,1,32,0,nchar);
      IIputdomio((short *)0,1,32,0,nchar);
      IIputdomio((short *)0,1,32,0,nchar);
      IIputdomio((short *)0,1,32,0,nchar);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"fasttodb1",7350,1211);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_fields(object_id, flseq, fldname, fldatatype, fllength\
, flprec, flwidth, flmaxlin, flmaxcol, flposy, flposx, fldatawidth, fl\
datalin, fldatacol, fltitle, fltitcol, fltitlin, flintrp, fldflags, fl\
d2flags, fldfont, fldptsz, fltype, flsubseq, ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"fldefault, flformat, flvalmsg, flvalchk)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&vffrmid);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&t_fhseq);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,intrname);
        IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&t_tfrows);
        IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&t_tfcols);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,4,&t_fhmaxy);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,30,4,&t_fhmaxx);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,30,4,&t_fhposy);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,30,4,&t_fhposx);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $9=");
        IIputdomio((short *)0,1,30,4,&t_tfwidth);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $10=");
        IIputdomio((short *)0,1,30,4,&t_tfstart);
        IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $11=");
        IIputdomio((short *)0,1,32,0,title);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $12=");
        IIputdomio((short *)0,1,30,4,&t_fhtitx);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $13=");
        IIputdomio((short *)0,1,30,4,&t_fhtity);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $14=");
        IIputdomio((short *)0,1,30,4,&t_fhintrp);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $15=");
        IIputdomio((short *)0,1,30,4,&t_fhdflags);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $16=");
        IIputdomio((short *)0,1,30,4,&t_fhd2flags);
        IIwritio(0,(short *)0,1,32,0,(char *)", 0, 0, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $17=");
        IIputdomio((short *)0,1,30,4,&t_ftable);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $18=");
        IIputdomio((short *)0,1,30,4,&t_subseq);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $19=");
        IIputdomio((short *)0,1,32,0,nchar);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $20=");
        IIputdomio((short *)0,1,32,0,nchar);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $21=");
        IIputdomio((short *)0,1,32,0,nchar);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $22=");
        IIputdomio((short *)0,1,32,0,nchar);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"fasttodb1",7350,1211);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 708 "fasttodb.sc" */	/* host code */
		if (FEinqerr())
		{
			return(FAIL);
		}
	}
	for (i = 0, col = tab->tfflds, win = tab->tfwins; i < tab->tfcols;
	    i++, col++, win++)
	{
		if (IIVFcfd_CopyFds(&(*col)->flhdr, &(*col)->fltype,
			win, (i4) FCOLUMN, outfp, dest) != OK)
			return(FAIL);
	}
	return(OK);
}
/*{
** Name: IIVFcfd_CopyFds - dumps the fields to the destination
**
** Description:
**	Dumps fields out to the destination specified in the parameter
**	dest.  When running against an INGRES back-end only, backslashes in 
**	the validation string, title and internal name are doubled.  The 
**	single backslash is interpreted as an ESC character.
**
** Inputs:
**	FLDHDR	*hdr	- field header information
**		char	*fhtitle	- title for field 
**		char	fhdname[36]	- field name 
**		i4	fhseq		- field sequence number 
**		i4	fhmaxx,fhmaxy   - number of cols and lines 
**		i4	fhposx,fhposy	- position of field window 
**		i4	fhtitx,fhtity   - position of title in field 
**		i4	fhintrp		- interrupt code for field 
**		i4	fhdflags	- flags (standout,box,etc.) 
**					- for a table field header this is a 
**					  mode field taken from fdtf values 
**		i4	fhd2flags	- more space for flags 
**		i4	fhdfont		- font information 
**		i4	fhdptsz		- point size information 
**
**	FLDTYPE *type 	- field type information
**		char	*ftvalstr	- validation string 
**		i4	ftdatatype	- data type for field 
**		i4	ftlength	- length of data type 
**		i4	ftprec		- precision of data type
**		i4	ftwidth		- width of field 
**		i4	ftdataln	- max. line length for data 
**		i4	ftdatax		- position of data string 
**		char	*ftdefault	- char default value 
**		char	*ftfmtstr	- format string 
**		char	*ftvalmsg	- message returned after valid 
**
**	FLDVAL  *val	- field value information
**		i4	fvdatay		- data window's y coordinate 
**
**	i4	fldtype - field type
**	FILE	*outfp	- output file
**	bool	dest	- destination of dump
**			    TO_FILE = TRUE
**			    TO_DB   = FALSE
**
**
** Outputs:
**	None.
**
** Side Effects:
**
** History:
**	<manual entries>
*/
STATUS
IIVFcfd_CopyFds(hdr, type, val, fldtype, outfp, dest)
reg	FLDHDR	*hdr;
reg	FLDTYPE *type;
  i4 fldtype;
reg	FLDVAL	*val;
FILE		*outfp;
bool		dest;
{
  char valstr[fdVSTRLEN + 1];
  char intrname[FE_MAXNAME + 1];
  char title[fdTILEN + 1];
  char ftdefault[fdDEFLEN + 1];
  char ftfmtstr[fdFMTLEN + 1];
  char ftvalmsg[fdVMSGLEN + 1];
  i4 t_fhseq;
  i4 t_fhtitx;
  i4 t_fhtity;
  i4 t_fhposx;
  i4 t_fhposy;
  i4 t_fhmaxx;
  i4 t_fhmaxy;
  i4 t_fhintrp;
  i4 t_fhdflags;
  i4 t_fhd2flags;
  i4 t_fhdfont;
  i4 t_fhdptsz;
  i4 t_fvdatay;
  i4 t_ftdatatype;
  char *t_ftdefault;
  i4 t_ftlength;
  i4 t_ftprec;
  i4 t_ftwidth;
  i4 t_ftdatax;
  i4 t_ftdataln;
  char *t_ftfmtstr;
  char *t_ftvalmsg;
  i4 t_subseq;
	if (dest == TO_FILE)
	{
		/*
		**  Check to see if title has a '\' in it.  If so, then
		**  double it for the copy command. (dkh)
		**  Fix for bug 5250.
		**
		*/
		vfbslash(type->ftvalstr, valstr);
		vfbslash(hdr->fhtitle, title);
		CVlower(hdr->fhdname);
		vfbslash(hdr->fhdname, intrname);
		vfbslash(type->ftvalmsg, ftvalmsg);
		vfbslash(type->ftfmtstr, ftfmtstr);
		vfbslash(type->ftdefault, ftdefault);
		hdr->fhdfont = 0;
		SIfprintf(outfp, ERx("%d"), vffrmid);
		SIfprintf(outfp, ANUMBER, hdr->fhseq);
		SIfprintf(outfp, ASTRING, intrname);
		SIfprintf(outfp, ANUMBER, type->ftdatatype);
		SIfprintf(outfp, ANUMBER, type->ftlength);
		SIfprintf(outfp, ANUMBER, type->ftprec);
		SIfprintf(outfp, ANUMBER, type->ftwidth);
		SIfprintf(outfp, ANUMBER, hdr->fhmaxy);
		SIfprintf(outfp, ANUMBER, hdr->fhmaxx);
		SIfprintf(outfp, ANUMBER, hdr->fhposy);
		SIfprintf(outfp, ANUMBER, hdr->fhposx);
		SIfprintf(outfp, ANUMBER, type->ftdataln);
		SIfprintf(outfp, ANUMBER, val->fvdatay);
		SIfprintf(outfp, ANUMBER, type->ftdatax);
		SIfprintf(outfp, ASTRING, title);
		SIfprintf(outfp, ANUMBER, hdr->fhtitx);
		SIfprintf(outfp, ANUMBER, hdr->fhtity);
		SIfprintf(outfp, ANUMBER, hdr->fhintrp);
		SIfprintf(outfp, ANUMBER, hdr->fhdflags);
		SIfprintf(outfp, ANUMBER, hdr->fhd2flags);
		SIfprintf(outfp, ANUMBER, hdr->fhdfont);
		SIfprintf(outfp, ANUMBER, hdr->fhdptsz);
		SIfprintf(outfp, ASTRING, ftdefault);
		SIfprintf(outfp, ASTRING, ftfmtstr);
		SIfprintf(outfp, ASTRING, ftvalmsg);
		SIfprintf(outfp, ASTRING, valstr);
		SIfprintf(outfp, ANUMBER, fldtype);
		SIfprintf(outfp, ERx("\t%d\n"), subseq++);
	}
	else			/*  destination is the database  */
	{	
		/*
		**   Assign temp. variables before insert.  This is done so the
		**   file frame.h does not change to a .qsh.  Use temp. 
		**   variables so SQL does not need to know the frame structure.
		*/
		STcopy(type->ftvalstr, valstr);
		STcopy(hdr->fhtitle, title);
		CVlower(hdr->fhdname);
		STcopy(hdr->fhdname, intrname);
		hdr->fhdfont = 0;
		t_fhseq = hdr->fhseq;
		t_ftdatatype = type->ftdatatype;
		t_ftlength = type->ftlength;
		t_ftprec = type->ftprec; 
		t_ftwidth = type->ftwidth; 
		t_fhmaxy = hdr->fhmaxy;
		t_fhmaxx = hdr->fhmaxx; 
		t_fhposy = hdr->fhposy;
		t_fhposx = hdr->fhposx;
		t_ftdataln = type->ftdataln;
		t_fvdatay = val->fvdatay;
		t_ftdatax = type->ftdatax;
		t_fhtitx = hdr->fhtitx;
		t_fhtity = hdr->fhtity;
		t_fhintrp = hdr->fhintrp;
		t_fhdflags = hdr->fhdflags;
		t_fhd2flags = hdr->fhd2flags;
		t_fhdfont = hdr->fhdfont;
		t_fhdptsz = hdr->fhdptsz;
		t_ftdefault = type->ftdefault;
		t_ftfmtstr = type->ftfmtstr;
		t_ftvalmsg = type->ftvalmsg;
		t_subseq = subseq++;  
/* # line 899 "fasttodb.sc" */	/* repeat insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"fasttodb2",7351,8186);
      IIputdomio((short *)0,1,30,4,&vffrmid);
      IIputdomio((short *)0,1,30,4,&t_fhseq);
      IIputdomio((short *)0,1,32,0,intrname);
      IIputdomio((short *)0,1,30,4,&t_ftdatatype);
      IIputdomio((short *)0,1,30,4,&t_ftlength);
      IIputdomio((short *)0,1,30,4,&t_ftprec);
      IIputdomio((short *)0,1,30,4,&t_ftwidth);
      IIputdomio((short *)0,1,30,4,&t_fhmaxy);
      IIputdomio((short *)0,1,30,4,&t_fhmaxx);
      IIputdomio((short *)0,1,30,4,&t_fhposy);
      IIputdomio((short *)0,1,30,4,&t_fhposx);
      IIputdomio((short *)0,1,30,4,&t_ftdataln);
      IIputdomio((short *)0,1,30,4,&t_fvdatay);
      IIputdomio((short *)0,1,30,4,&t_ftdatax);
      IIputdomio((short *)0,1,32,0,title);
      IIputdomio((short *)0,1,30,4,&t_fhtitx);
      IIputdomio((short *)0,1,30,4,&t_fhtity);
      IIputdomio((short *)0,1,30,4,&t_fhintrp);
      IIputdomio((short *)0,1,30,4,&t_fhdflags);
      IIputdomio((short *)0,1,30,4,&t_fhd2flags);
      IIputdomio((short *)0,1,30,4,&t_fhdfont);
      IIputdomio((short *)0,1,30,4,&t_fhdptsz);
      IIputdomio((short *)0,1,32,0,t_ftdefault);
      IIputdomio((short *)0,1,32,0,t_ftfmtstr);
      IIputdomio((short *)0,1,32,0,t_ftvalmsg);
      IIputdomio((short *)0,1,32,0,valstr);
      IIputdomio((short *)0,1,30,4,&fldtype);
      IIputdomio((short *)0,1,30,4,&t_subseq);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"fasttodb2",7351,8186);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_fields(object_id, flseq, fldname, fldatatype, fllength\
, flprec, flwidth, flmaxlin, flmaxcol, flposy, flposx, fldatawidth, fl\
datalin, fldatacol, fltitle, fltitcol, fltitlin, flintrp, fldflags, fl\
d2flags, fldfont, fldptsz, fldefault, flformat");
        IIwritio(0,(short *)0,1,32,0,(char *)
", flvalmsg, flvalchk, fltype, flsubseq)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&vffrmid);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&t_fhseq);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,intrname);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&t_ftdatatype);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&t_ftlength);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,4,&t_ftprec);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,30,4,&t_ftwidth);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,30,4,&t_fhmaxy);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,30,4,&t_fhmaxx);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $9=");
        IIputdomio((short *)0,1,30,4,&t_fhposy);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $10=");
        IIputdomio((short *)0,1,30,4,&t_fhposx);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $11=");
        IIputdomio((short *)0,1,30,4,&t_ftdataln);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $12=");
        IIputdomio((short *)0,1,30,4,&t_fvdatay);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $13=");
        IIputdomio((short *)0,1,30,4,&t_ftdatax);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $14=");
        IIputdomio((short *)0,1,32,0,title);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $15=");
        IIputdomio((short *)0,1,30,4,&t_fhtitx);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $16=");
        IIputdomio((short *)0,1,30,4,&t_fhtity);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $17=");
        IIputdomio((short *)0,1,30,4,&t_fhintrp);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $18=");
        IIputdomio((short *)0,1,30,4,&t_fhdflags);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $19=");
        IIputdomio((short *)0,1,30,4,&t_fhd2flags);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $20=");
        IIputdomio((short *)0,1,30,4,&t_fhdfont);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $21=");
        IIputdomio((short *)0,1,30,4,&t_fhdptsz);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $22=");
        IIputdomio((short *)0,1,32,0,t_ftdefault);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $23=");
        IIputdomio((short *)0,1,32,0,t_ftfmtstr);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $24=");
        IIputdomio((short *)0,1,32,0,t_ftvalmsg);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $25=");
        IIputdomio((short *)0,1,32,0,valstr);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $26=");
        IIputdomio((short *)0,1,30,4,&fldtype);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $27=");
        IIputdomio((short *)0,1,30,4,&t_subseq);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"fasttodb2",7351,8186);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 916 "fasttodb.sc" */	/* host code */
		if (FEinqerr())
		{
			return(FAIL);
		}
	}
	return(OK);
}
/*{
** Name: IIVFctr_CopyTrim - dumps trim to the destination 
**
** Description:
**	Dumps trim out to the destination specified in the parameter
**	dest.  When running against an INGRES back-end only, backslashes in 
**	the trim are doubled.  The single backslash is interpreted as an 
**	ESC character.
**
** Inputs:
**	FRAME	*frm  - frame of the trim to be dumped
**		TRIM	**frtrim		- frame trim strings
**			i4	trmx		- x coordinate of trim start 
**						  position 
**			i4	trmy		- y coordinate of trim start 
**						  position 
**			i4	trmflags	- flags for trim
**			i4	trm2flags	- more flags for trim
**			i4	trmfont		- font information
**			i4	trmptsz		- point size information
**		i4	frtrimno		- number of trim strings 
**		char	frname[36]		- frame name 
**
**	FILE	*outfp	- output file
**	bool	dest	- destination of dump
**			    TO_FILE = TRUE
**			    TO_DB   = FALSE
**
**
** Outputs:
**	None.
**
** Side Effects:
**
** History:
**	<manual entries>
*/
STATUS
IIVFctr_CopyTrim(frm, outfp, dest)
reg	FRAME	*frm;
FILE		*outfp;
bool		dest;
{
  char buf[fdTRIMLEN + 1];
  i4 t_trmx;
  i4 t_trmy;
  i4 t_trmflags;
  i4 t_trm2flags;
  i4 t_trmfont;
  i4 t_trmptsz;
	reg	TRIM	**tl;
	register i4	numTrims;
	reg	TRIM	*tr;
	tl = frm->frtrim;
	numTrims = frm->frtrimno;
	for (; numTrims > 0; numTrims--, tl++)
	{
		tr = *tl;
		if (dest == TO_FILE)
		{
			/*
			**  Check for backslashes in the trim. (dkh)
			*/
			vfbslash(tr->trmstr, buf);
			SIfprintf(outfp, ERx("%d"), vffrmid);
			SIfprintf(outfp, ANUMBER, tr->trmx);
			SIfprintf(outfp, ANUMBER, tr->trmy);
			SIfprintf(outfp, ANUMBER, tr->trmflags);
			SIfprintf(outfp, ANUMBER, tr->trm2flags);
			SIfprintf(outfp, ANUMBER, tr->trmfont);
			SIfprintf(outfp, ANUMBER, tr->trmptsz);
			SIfprintf(outfp, ERx("\t%s\n"), buf);
		}
		else			/*  destination is the database   */
		{
			/*
			**   Assign temp. variables before insert.  This is 
			**   done so the file frame.h does not change to a 
			**   .qsh.  Use temp. variables so SQL does not need 
			**   to know the frame structure.
			*/
			STcopy(tr->trmstr, buf);
			t_trmx = tr->trmx; 
			t_trmy = tr->trmy;
			t_trmflags = tr->trmflags; 
			t_trm2flags = tr->trm2flags; 
			t_trmfont = tr->trmfont;
			t_trmptsz = tr->trmptsz; 
/* # line 1013 "fasttodb.sc" */	/* repeat insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"fasttodb3",7352,24762);
      IIputdomio((short *)0,1,30,4,&vffrmid);
      IIputdomio((short *)0,1,30,4,&t_trmx);
      IIputdomio((short *)0,1,30,4,&t_trmy);
      IIputdomio((short *)0,1,30,4,&t_trmflags);
      IIputdomio((short *)0,1,30,4,&t_trm2flags);
      IIputdomio((short *)0,1,30,4,&t_trmfont);
      IIputdomio((short *)0,1,30,4,&t_trmptsz);
      IIputdomio((short *)0,1,32,0,buf);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"fasttodb3",7352,24762);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_trim(object_id, trim_col, trim_lin, trim_flags, trim2_\
flags, trim_font, trim_ptsz, trim_trim)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&vffrmid);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&t_trmx);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&t_trmy);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&t_trmflags);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&t_trm2flags);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,4,&t_trmfont);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,30,4,&t_trmptsz);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,buf);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"fasttodb3",7352,24762);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 1020 "fasttodb.sc" */	/* host code */
			if (FEinqerr())
			{
				return(FAIL);
			}
		}
	}
	return(OK);
}
/*{
** Name: vfcptrms - copies the trim into ii_trim
**
** Description:
**	Copies the trim of a frame from an intermediate file into ii_trim.
**      This routine should only be use against an INGRES back-end or a back-end
**	that supports SQL COPY.
**
** Inputs:
**	LOCATION	*file - intermediate file to copy from
**
** Outputs:
**	None.
**
** History:
**
*/
STATUS
vfcptrms(file)
LOCATION	*file;
{
	char	*filename;
	LOtos(file, &filename);
	fname = filename;
# ifdef CMS
	/*
	**    Since there is no such thing as a null (empty) file in CMS
	**    we must check to make sure the location physically exists.
	**    If it does not, if was probably supposed to be a null
	**    dataset, so we will just return and skip the 'copy from'.
	*/
	if (LOexist(file)!=OK) return(OK);
# endif
/* # line 1061 "fasttodb.sc" */	/* copy table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"copy table ii_trim(object_id=c0tab, trim_col=c0tab, trim_lin=c0tab, t\
rim_flags=c0tab, trim2_flags=c0tab, trim_font=c0tab, trim_ptsz=c0tab, \
trim_trim=c0nl)from ");
    IIvarstrio((short *)0,1,32,0,fname);
    IIsyncup((char *)0,0);
  }
/* # line 1072 "fasttodb.sc" */	/* host code */
	if (FEinqerr())
	{
		return(FAIL);
	}
	return(OK);
}
/*{
** Name: IIVFccf_CompFrm - dumps the encoded form to the database
**
** Description:
**	Dumps the FRAME structure (encoded form) out to the database.
**	All pointers will be relocated to be offsets from the base.
**
**	Changed to not syserr when something is wrong.	We now just
**	give an error message and return FALSE.
**
** Inputs:
**	FRAME	*frm  - frame of the trim to be dumped
**
** Outputs:
**	TRUE		- no errors occurred
**	FALSE		- unable to save encoded form to database
**
** Side Effects:
**
** History:
**	<manual entries>
*/
STATUS
IIVFccf_CompFrm(frm)
FRAME	*frm;
{
  i4 rowsize;
  i4 seqno = 0;
  char *addr;
  i4 bytes_sent = 0;
  DB_DATA_VALUE dbv;
  char sql_buf[128];
	i4		size;
	i4		encode_size;
	DB_TEXT_STRING	*txtptr;
	AFE_DCL_TXT_MACRO(MAX_CFTXT)	txtdata;
	SH_DESC		sh_desc;
	char		filebuf[MAX_LOC + 1];
	LOCATION	fileloc;
	FILE		*fptr;
	i4		count;
# ifdef PMFE
	char		fbuf[MAX_CFTXT+2];
# endif /* PMFE */
	/* if this is a network request */
	if (IIUGdaDiffArch())
	{
		/*
		**  Don't copy across a network since we don't
		**  know if the two nodes are compatible.  Can't
		**  use new 4.0 stuff yet since we need to run
		**  against a 3.0 backend as well. (dkh)
		*/
		return(OK);
	}
	/*
	**  Note that we assume that MAX_CFTXT is large enough
	**  no matter what IIFDesEncodeSize() returns.
	*/
	encode_size = IIFDesEncodeSize();
	/*
	** temporarily use IS_FILE until DSstore with IS_MEM is implemented
	*/
	sh_desc.sh_type = IS_FILE;
	sh_desc.sh_reg.file.fp = NULL;
	sh_desc.sh_keep = NULL;
	if (DSstore(&sh_desc, DS_FRAME, frm, IIFDgfdGetFdDstab()) != OK)
	{
		IIUGerr(E_VF0064_Unable_to_relo, UG_ERR_ERROR, 0);
		return(FAIL);
	}
	STcopy(sh_desc.sh_reg.file.name, filebuf);
	if (LOfroms(PATH & FILENAME, filebuf, &fileloc) != OK)
	{
		IIUGerr(E_VF0065_Unable_to_conv, UG_ERR_ERROR, 0);
		return(FAIL);
	}
	if (SIfopen(&fileloc, ERx("r"), SI_VAR, 4096, &fptr) != OK)
	{
		IIUGerr(E_VF0066_Unable_to_open, UG_ERR_ERROR, 0);
		return(FAIL);
	}
	cfsize = sh_desc.size;
# ifndef PMFE
	cfblock = (char *)MEreqmem((u_i4)0, (u_i4)cfsize, TRUE, 
		(STATUS *)NULL);
	if (SIread(fptr, cfsize, &count, cfblock) != OK)
	{
		syserr(ERget(S_VF0067_read_err));
	}
	SIclose(fptr);
	LOdelete(&fileloc);
# else
	cfblock = fbuf;
# endif /* PMFE */
	/* if this is a network request */
	if (IIUGdaDiffArch())
	{
		/*
		**  Don't copy across a network since we don't
		**  know if the two nodes are compatible.  Can't
		**  use new 4.0 stuff yet since we need to run
		**  against a 3.0 backend as well. (dkh)
		*/
		return(OK);
	}
	dbv.db_datatype = DB_VBYTE_TYPE;
	dbv.db_length = sizeof(txtdata);
	dbv.db_prec = 0;
	dbv.db_data = (PTR) &txtdata;
	txtptr = (DB_TEXT_STRING *) &txtdata;
	addr = cfblock;
	size = cfsize;
	while (size > 0)
	{
		if (size > encode_size)
		{
			rowsize = encode_size;
			size -= encode_size;
		}
		else
		{
			rowsize = size;
			size = 0;
		}
# ifdef PMFE
		if (SIread(fptr, rowsize, &count, cfblock) != OK)
		{
			syserr(ERget(S_VF0067_read_err));
		}
# endif /* PMFE */
		txtptr->db_t_count = rowsize;
		MEcopy((PTR) addr, (u_i2) rowsize, (PTR) txtptr->db_t_text);
# ifdef IBM370
		STpolycat(2,
		 ERx("INSERT INTO ii_encoded_forms (object_id, cfseq, cfdatsize, cftotdat, cfdata)"),
		 ERx(" VALUES (?, ?, ?, ?, ?)"), sql_buf);
/* # line 1217 "fasttodb.sc" */	/* prepare */
  {
    IIsqInit((char *)0);
    IIsqPrepare(0,(char *)"iivfccf_stmt",(char *)0,0,sql_buf);
  }
/* # line 1218 "fasttodb.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExStmt((char *)"iivfccf_stmt",1);
    IIputdomio((short *)0,1,30,4,&vffrmid);
    IIputdomio((short *)0,1,30,4,&seqno);
    IIputdomio((short *)0,1,30,4,&rowsize);
    IIputdomio((short *)0,1,30,4,&cfsize);
    IIputdomio((short *)0,1,44,0,&dbv);
    IIsyncup((char *)0,0);
  }
/* # line 1220 "fasttodb.sc" */	/* host code */
# else
/* # line 1221 "fasttodb.sc" */	/* repeat insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"fasttodb4",7353,29946);
      IIputdomio((short *)0,1,30,4,&vffrmid);
      IIputdomio((short *)0,1,30,4,&seqno);
      IIputdomio((short *)0,1,30,4,&rowsize);
      IIputdomio((short *)0,1,30,4,&cfsize);
      IIputdomio((short *)0,1,44,0,&dbv);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"fasttodb4",7353,29946);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_encoded_forms(object_id, cfseq, cfdatsize, cftotdat, c\
fdata)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&vffrmid);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&seqno);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&rowsize);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&cfsize);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,44,0,&dbv);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"fasttodb4",7353,29946);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 1226 "fasttodb.sc" */	/* host code */
# endif
		if (size != 0)
		{
			seqno++;
			bytes_sent += rowsize;
# ifndef PMFE
			addr = &cfblock[bytes_sent];
# endif /* PMFE */
		}
	}
	/*  
	**  Free up allocated memory.
	*/
# ifndef PMFE
	MEfree(cfblock);
# else
	SIclose(fptr);
	LOdelete(&fileloc);
# endif /* PMFE */
	if (FEinqerr())
	{
		return(FAIL);
	}
	return(OK);
}
/*{
** Name: vfbslash - searches for backslashes in the string
**
** Description:
**	Copies the source string into the destination string.  If any
**	backslashes are in the source string, a second backslash is inserted
**	into the destination string.  This is done for an INGRES back-end.
**	Backslashes are interpreted as an ESC character, so the first backslash
**	is stripped off.
**
**  	Added for fix to BUG 5250. (dkh)
**
** Inputs:
**	char	*src	- source string
**	char	*dest	- destination string
**
** Outputs:
**	dest - a copy of the source string with any backslashes doubled.
**
** Side Effects:
**
** History:
**	19-aug-93 (kirke)
**	    Bug #51348 - An unwanted backslash was inserted if the second
**	    byte of a two byte character was equivalent to a backslash.
**	    Rewrote using CM routines to handle multibyte properly.
*/
VOID
vfbslash(src, dest)
char	*src;
char	*dest;
{
	char	*backslash = ERx("\\");
	char	*cp;
	cp = dest;
	while (*src)
	{
		if (!CMcmpcase(src, backslash))
		{
			/* insert a backslash */
			CMcpychar(backslash, cp);
			CMnext(cp);
		}
		CMcpyinc(src, cp);
	}
	*cp = '\0';
}
/*{
** Name: IIVFint_InitSub - initializes before saving the form.
**
** Description:
**	Deletes the old form from ii_forms, ii_trim, ii_fields and
**	ii_encoded_forms.  Initializes subseq to zero.
**
** Inputs:
**	None.
**
** Outputs:
**	None.
** History:
**	<manual entries>
*/
STATUS
IIVFint_InitSub()
{
/* # line 1317 "fasttodb.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_forms where object_id=");
    IIputdomio((short *)0,1,30,4,&vffrmid);
    IIsyncup((char *)0,0);
  }
/* # line 1318 "fasttodb.sc" */	/* host code */
	if (FEinqerr())
	{
		return(FAIL);
	}
/* # line 1323 "fasttodb.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_trim where object_id=");
    IIputdomio((short *)0,1,30,4,&vffrmid);
    IIsyncup((char *)0,0);
  }
/* # line 1324 "fasttodb.sc" */	/* host code */
	if (FEinqerr())
	{
		return(FAIL);
	}
/* # line 1329 "fasttodb.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_fields where object_id=");
    IIputdomio((short *)0,1,30,4,&vffrmid);
    IIsyncup((char *)0,0);
  }
/* # line 1330 "fasttodb.sc" */	/* host code */
	if (FEinqerr())
	{
		return(FAIL);
	}
	/*
	** Delete any earlier version of form in iicompfrm catalog.
	*/
/* # line 1338 "fasttodb.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_encoded_forms where object_id=");
    IIputdomio((short *)0,1,30,4,&vffrmid);
    IIsyncup((char *)0,0);
  }
/* # line 1339 "fasttodb.sc" */	/* host code */
	if (FEinqerr())
	{
		return(FAIL);
	}
	subseq = 0;
	return(OK);
}
