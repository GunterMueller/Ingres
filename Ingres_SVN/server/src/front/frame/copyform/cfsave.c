# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 1989, 2005 Ingres Corporation
**
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<me.h>
# include	<st.h>
# include	<si.h>
# include	<ds.h>
# include	<feds.h>
# include	<er.h>
# include	<fmt.h>
# include	<adf.h>
# include	<afe.h>
# include	<ft.h>
# include	<frame.h>
# include	<lo.h>
# include	<ug.h>
# include	"ercf.h"
# include	<encoding.h>
# include	<uigdata.h>
# include <ooclass.h>
/**
** Name:	cfsave.qsc - Add a form to the database.
**
** Description:
**	This routine adds a form (pointed to by the passed pointer)
**	to the database without going through vifred.
**
** History:
**	07/15/89 (dkh) - Initial version.
**	11/10/89 (dkh) - Added changes to handle different size for
**			 ii_encoded_forms catalog due to B1.
**	25-jan-90 (bruceb)
**		Set the form's frversion before saving.
**	5-apr-90 (Mike S)
**		Insure none of the insert fields are NULL.
**	09/06/90 (dkh) - Changed to use IIFDgfdGetFdDstab() instead of
**			 IIFDdstab directly.
**	19-oct-1990 (mgw)
**		Fixed #include of local ercf.h to use "" instead of <>
**	03/03/91 (dkh) - Fixed bug 36144.
**	10/14/92 (dkh) - Removed use of ArrayOf in favor of DSTARRAY.
**			 ArrayOf confused the alpha C compiler.
**	08/31/93 (dkh) - Fixed up compiler complaints due to prototyping.
**      11-jan-1996 (toumi01; from 1.1 axp_osf port)
**              Added kchin's change (from 6.4) for axp_osf
**              10/15/93 (kchin)
**              Cast argument to PTR when calling IIOOidWrite(), this
**              is due to the change of datatype of variable parameters
**              to PTR in IIOOidWrite() routine.  The change here is made in
**              IICFsave().
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	07-sep-2005 (abbjo03)
**	    Replace xxx_FILE constants by SI_xxx.
**	25-oct-2007 (toumi01)
**	    For UTF8 force ii_encoded_forms.cfdata type to DB_VBYTE_TYPE.
*/
FUNC_EXTERN	i4	IIFDesEncodeSize();
FUNC_EXTERN	DSTARRAY	*IIFDgfdGetFdDstab();
FUNC_EXTERN	char	*UGcat_now();
  static i4 total_fields = 0;
/*{
** Name:	IICFsfSaveFlds - Save a row into ii_fields.
**
** Description:
**	This routine saves a new row into the ii_fields catalog
**	for the form being saved.
**
** Inputs:
**	id		Object id to use.
**	fldtype		Field entity to be saved.
**	hdr		Pointer to a FLDHDR structure.
**	type		Pointer to a FLDTYPE structure.
**	val		Pointer to a FLDTYPE structure.
**	tbl		Pointer to a TBLFLD structure.  This only
**			applies if the field entity is a FTABLE.
**
** Outputs:
**	None.
**
**	Returns:
**		OK	If save was sucessful.
**		FAIL	If save failed.
**
**	Exceptions:
**		None.
**
** Side Effects:
**	None.
**
** History:
**	07/17/89 (dkh) - Initial version.
*/
STATUS
IICFsfSaveFlds(id, fldtype, hdr, type, val, tbl)
  OOID id;
  i4 fldtype;
FLDHDR	*hdr;
FLDTYPE	*type;
FLDVAL	*val;
TBLFLD	*tbl;
{
	char	*empty = ERx("");
  i4 f_seq;
  char *f_name;
  i4 f_dtype;
  i4 f_length;
  i4 f_prec;
  i4 f_width;
  i4 f_maxy;
  i4 f_maxx;
  i4 f_posy;
  i4 f_posx;
  i4 f_dataln;
  i4 f_ydata;
  i4 f_xdata;
  char *f_title;
  i4 f_xtitle;
  i4 f_ytitle;
  i4 f_intrp;
  i4 f_flags;
  i4 f_2flags;
  i4 f_font;
  i4 f_ptsz;
  char *f_default;
  char *f_format;
  char *f_valmsg;
  char *f_valstr;
	hdr->fhdfont = 0;
	f_seq = hdr->fhseq;
	f_name = hdr->fhdname;
	if ((char *)NULL == f_name)
		f_name = empty;
	f_maxx = hdr->fhmaxx;
	f_maxy = hdr->fhmaxy;
	f_posx = hdr->fhposx;
	f_posy = hdr->fhposy;
	f_title = hdr->fhtitle;
	if ((char *)NULL == f_title)
		f_title = empty;
	f_xtitle = hdr->fhtitx;
	f_ytitle = hdr->fhtity;
	f_intrp = hdr->fhintrp;
	f_flags = hdr->fhdflags;
	f_2flags = hdr->fhd2flags;
	if (fldtype == FREGULAR || fldtype == FCOLUMN)
	{
		f_dtype = type->ftdatatype;
		f_length = type->ftlength;
		f_prec = type->ftprec;
		f_width = type->ftwidth;
		f_dataln = type->ftdataln;
		if (fldtype == FREGULAR)
			f_ydata = val->fvdatay;
		else
			f_ydata = tbl->tfstart;
		f_xdata = type->ftdatax;
		f_font = hdr->fhdfont;
		f_ptsz = hdr->fhdptsz;
		f_default = type->ftdefault;
		if ((char *)NULL == f_default)
			f_default = empty;
		f_format = type->ftfmtstr;
		if ((char *)NULL == f_format)
			f_format = empty;
		f_valmsg = type->ftvalmsg;
		if ((char *)NULL == f_valmsg)
			f_valmsg = empty;
		f_valstr = type->ftvalstr;
		if ((char *)NULL == f_valstr)
			f_valstr = empty;
	}
	else if (fldtype == FTABLE)
	{
		f_dtype = 0;
		f_length = tbl->tfrows;
		f_prec = 0;
		f_width = tbl->tfcols;
		f_dataln = tbl->tfwidth;
		f_ydata = tbl->tfstart;
		f_xdata = 0;
		f_font = 0;
		f_ptsz = 0;
		f_default = empty;
		f_format = empty;
		f_valmsg = empty;
		f_valstr = empty;
	}
	else
	{
		return(FAIL);
	}
/* # line 209 "cfsave.sc" */	/* repeat insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cfsave1",6468,26646);
      IIputdomio((short *)0,1,30,4,&id);
      IIputdomio((short *)0,1,30,4,&f_seq);
      IIputdomio((short *)0,1,32,0,f_name);
      IIputdomio((short *)0,1,30,4,&f_dtype);
      IIputdomio((short *)0,1,30,4,&f_length);
      IIputdomio((short *)0,1,30,4,&f_prec);
      IIputdomio((short *)0,1,30,4,&f_width);
      IIputdomio((short *)0,1,30,4,&f_maxy);
      IIputdomio((short *)0,1,30,4,&f_maxx);
      IIputdomio((short *)0,1,30,4,&f_posy);
      IIputdomio((short *)0,1,30,4,&f_posx);
      IIputdomio((short *)0,1,30,4,&f_dataln);
      IIputdomio((short *)0,1,30,4,&f_ydata);
      IIputdomio((short *)0,1,30,4,&f_xdata);
      IIputdomio((short *)0,1,32,0,f_title);
      IIputdomio((short *)0,1,30,4,&f_xtitle);
      IIputdomio((short *)0,1,30,4,&f_ytitle);
      IIputdomio((short *)0,1,30,4,&f_intrp);
      IIputdomio((short *)0,1,30,4,&f_flags);
      IIputdomio((short *)0,1,30,4,&f_2flags);
      IIputdomio((short *)0,1,30,4,&f_font);
      IIputdomio((short *)0,1,30,4,&f_ptsz);
      IIputdomio((short *)0,1,32,0,f_default);
      IIputdomio((short *)0,1,32,0,f_format);
      IIputdomio((short *)0,1,32,0,f_valmsg);
      IIputdomio((short *)0,1,32,0,f_valstr);
      IIputdomio((short *)0,1,30,4,&fldtype);
      IIputdomio((short *)0,1,30,4,&total_fields);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cfsave1",6468,26646);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_fields(object_id, flseq, fldname, fldatatype, fllength\
, flprec, flwidth, flmaxlin, flmaxcol, flposy, flposx, fldatawidth, fl\
datalin, fldatacol, fltitle, fltitcol, fltitlin, flintrp, fldflags, fl\
d2flags, fldfont, fldptsz, fldefault, flformat");
        IIwritio(0,(short *)0,1,32,0,(char *)
", flvalmsg, flvalchk, fltype, flsubseq)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&f_seq);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,f_name);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&f_dtype);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&f_length);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,4,&f_prec);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,30,4,&f_width);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,30,4,&f_maxy);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,30,4,&f_maxx);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $9=");
        IIputdomio((short *)0,1,30,4,&f_posy);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $10=");
        IIputdomio((short *)0,1,30,4,&f_posx);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $11=");
        IIputdomio((short *)0,1,30,4,&f_dataln);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $12=");
        IIputdomio((short *)0,1,30,4,&f_ydata);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $13=");
        IIputdomio((short *)0,1,30,4,&f_xdata);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $14=");
        IIputdomio((short *)0,1,32,0,f_title);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $15=");
        IIputdomio((short *)0,1,30,4,&f_xtitle);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $16=");
        IIputdomio((short *)0,1,30,4,&f_ytitle);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $17=");
        IIputdomio((short *)0,1,30,4,&f_intrp);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $18=");
        IIputdomio((short *)0,1,30,4,&f_flags);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $19=");
        IIputdomio((short *)0,1,30,4,&f_2flags);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $20=");
        IIputdomio((short *)0,1,30,4,&f_font);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $21=");
        IIputdomio((short *)0,1,30,4,&f_ptsz);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $22=");
        IIputdomio((short *)0,1,32,0,f_default);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $23=");
        IIputdomio((short *)0,1,32,0,f_format);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $24=");
        IIputdomio((short *)0,1,32,0,f_valmsg);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $25=");
        IIputdomio((short *)0,1,32,0,f_valstr);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $26=");
        IIputdomio((short *)0,1,30,4,&fldtype);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $27=");
        IIputdomio((short *)0,1,30,4,&total_fields);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"cfsave1",6468,26646);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 224 "cfsave.sc" */	/* host code */
	if (FEinqerr())
	{
		return(FAIL);
	}
	return(OK);
}
/*{
** Name:	IICFefEncodeForm - Encode a form into the database.
**
** Description:
**	This routine will encoded a form into the ii_encoded_forms
**	catalog in the database.
**
** Inputs:
**	id		Object id to use.
**	form		Pointer to form to be encoded.
**
** Outputs:
**	None.
**
**	Returns:
**		OK	If form was encoded sucessfully.
**		FAIL	If form could not be encoded.
**	Exceptions:
**		None.
**
** Side Effects:
**	None.
**
** History:
**	07/17/89 (dkh) - Initial version.
**	22-jan-92 (leighb) DeskTop Porting Change:
**		Call SIfopen for the PC instead of SIopen.
**	13-mar-92 (leighb) DeskTop Porting Change:  Cleanup above change.
**	27-dec-95 (emmag)
**		Call SIfopen to open file in binary mode on NT and OS/2.
*/
STATUS
IICFefEncodeForm(id, form)
  OOID id;
FRAME	*form;
{
	i4				encode_size;
	i4				size;
	DB_TEXT_STRING			*txtptr;
	AFE_DCL_TXT_MACRO(MAX_CFTXT)	txtdata;
	SH_DESC				sh_desc;
	char				filebuf[MAX_LOC + 1];
	LOCATION			fileloc;
	FILE				*fptr;
	i4				count;
	STATUS				retval;
  i4 rowsize;
  i4 seqno = 0;
  char *addr;
  i4 bytes_sent = 0;
  DB_DATA_VALUE dbv;
  char sql_buf[128];
  i4 cfsize;
  char *cfblock;
	/*
	**  Note that we assume that MAX_CFTXT is large enough
	**  for whatever IIFDesEncodeSize() returns.
	*/
	encode_size = IIFDesEncodeSize();
	sh_desc.sh_type = IS_FILE;
	sh_desc.sh_reg.file.fp = NULL;
	sh_desc.sh_keep = NULL;
	if (DSstore(&sh_desc, DS_FRAME, form, IIFDgfdGetFdDstab()) != OK)
	{
		return(FAIL);
	}
	STcopy(sh_desc.sh_reg.file.name, filebuf);
	if (LOfroms(PATH & FILENAME, filebuf, &fileloc) != OK)
	{
		return(FAIL);
	}
# if defined(MSDOS) || defined(DESKTOP) || defined(CMS)
	if ( SIfopen(&fileloc, ERx("r"), SI_VAR, 4096, &fptr) != OK )
# else
	if ( SIopen(&fileloc, ERx("r"), &fptr) != OK )
# endif
	{
		return(FAIL);
	}
	cfsize = sh_desc.size;
	if ((cfblock = (char *) MEreqmem((u_i4) 0, (u_i4) cfsize, TRUE,
		(STATUS *) NULL)) == NULL)
	{
		return(FAIL);
	}
	retval = SIread(fptr, cfsize, &count, cfblock);
	SIclose(fptr);
	LOdelete(&fileloc);
	if (retval != OK)
	{
		MEfree(cfblock);
		return(FAIL);
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
		txtptr->db_t_count = rowsize;
		MEcopy((PTR) addr, (u_i2) rowsize, (PTR) txtptr->db_t_text);
# ifdef IBM370
		STpolycat(3, ERx("INSERT INTO ii_encoded_forms (object_id, "),
			ERx("cfseq, cfdatsie, cftotdat, cfdata)"),
			ERx(" VALUES (?, ?, ?, ?, ?)"), sqp_buf);
/* # line 352 "cfsave.sc" */	/* prepare */
  {
    IIsqInit((char *)0);
    IIsqPrepare(0,(char *)"iicfenc_stmt",(char *)0,0,sql_buf);
  }
/* # line 353 "cfsave.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExStmt((char *)"iicfenc_stmt",1);
    IIputdomio((short *)0,1,30,4,&id);
    IIputdomio((short *)0,1,30,4,&seqno);
    IIputdomio((short *)0,1,30,4,&rowsize);
    IIputdomio((short *)0,1,30,4,&cfsize);
    IIputdomio((short *)0,1,44,0,&dbv);
    IIsyncup((char *)0,0);
  }
/* # line 355 "cfsave.sc" */	/* host code */
# else
/* # line 356 "cfsave.sc" */	/* repeat insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cfsave2",6469,30709);
      IIputdomio((short *)0,1,30,4,&id);
      IIputdomio((short *)0,1,30,4,&seqno);
      IIputdomio((short *)0,1,30,4,&rowsize);
      IIputdomio((short *)0,1,30,4,&cfsize);
      IIputdomio((short *)0,1,44,0,&dbv);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cfsave2",6469,30709);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_encoded_forms(object_id, cfseq, cfdatsize, cftotdat, c\
fdata)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
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
        IIexDefine(0,(char *)"cfsave2",6469,30709);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 359 "cfsave.sc" */	/* host code */
# endif /* IBM370 */
		if (FEinqerr())
		{
			MEfree(cfblock);
			return(FAIL);
		}
		if (size != 0)
		{
			seqno++;
			bytes_sent += rowsize;
			addr = &cfblock[bytes_sent];
		}
	}
	/*
	**  Free up allocated memory.
	*/
	MEfree(cfblock);
	return(OK);
}
/*{
** Name:	IICFsave - Save a form (created by VQ) into the database.
**
** Description:
**	Save a user generated form into the database.
**	Any error except "Duplicate name" generates a visible error message.
**	Assumes that the form has been created properly and that
**	there are no fields on the "fdnsfld" chain.
**
**	Note that if "overwrt" is TRUE, the old object is destroyed
**	whether the new object was successfully saved or not.
**
** Inputs:
**	form		Form to be saved.
**	tmpdir		A directory to place any needed temporary files.
**	overwrt		TRUE if an existing form should be overwritten.
**	short_msg	Short message for object registration.
**	long_msg	Long message for object registration.
**
** Outputs:
**	None.
**
**	Returns:
**		-1	Form already exists and "overwrt" flag was FALSE.
**		0	Form successfully saved.
**		1	Save failed with error message from IICFsave.
**	Exceptions:
**		<exception codes>
**
** Side Effects:
**	None.
**
** History:
**	07/15/89 (dkh) - Initial version.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
*/
i4
IICFsave(form, tmpdir, overwrt, short_msg, long_msg)
FRAME		*form;
LOCATION	*tmpdir;
bool		overwrt;
char		*short_msg;
char		*long_msg;
{
	TRIM	**tl;
	FIELD	**fl;
	FLDCOL	**cols;
	TBLFLD	*tbl;
	i4	ntrim;
	i4	nfld;
	i4	ncols;
	i4	i;
	i4	j;
	i4	class = OC_FORM;
	char	*fname;
  char *time;
  char *t_str;
  i4 t_x;
  i4 t_y;
  i4 t_flags;
  i4 t_2flags;
  i4 t_font;
  i4 t_ptsz;
  OOID id = -1;
  i4 f_fldno;
  i4 f_nsfldno;
  i4 f_trimno;
  i4 f_maxx;
  i4 f_maxy;
  i4 f_posx;
  i4 f_posy;
  i4 f_version;
  i4 f_flags;
  i4 f_2flags;
  i4 f_scrtype = 0;
  i4 f_scrxmax = 0;
  i4 f_scrymax = 0;
  i4 f_scrxdpi = 0;
  i4 f_scrydpi = 0;
	UIDBDATA	*uidbdata;
	uidbdata = IIUIdbdata();
	/*
	**  If object exists, check if we can overwrite it.
	*/
	if (cf_objexist(class, form->frname, uidbdata->user, &id))
	{
		if (overwrt)
		{
			iiuicw1_CatWriteOn();
/* # line 471 "cfsave.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_forms where object_id=");
    IIputdomio((short *)0,1,30,4,&id);
    IIsyncup((char *)0,0);
  }
/* # line 472 "cfsave.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_fields where object_id=");
    IIputdomio((short *)0,1,30,4,&id);
    IIsyncup((char *)0,0);
  }
/* # line 473 "cfsave.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_trim where object_id=");
    IIputdomio((short *)0,1,30,4,&id);
    IIsyncup((char *)0,0);
  }
/* # line 475 "cfsave.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_encoded_forms where object_id=");
    IIputdomio((short *)0,1,30,4,&id);
    IIsyncup((char *)0,0);
  }
/* # line 476 "cfsave.sc" */	/* host code */
			iiuicw0_CatWriteOff();
		/*
			cf_delobj(class, id);
		*/
		}
		else
		{
			return(-1);
		}
	}
	else
	{
		id = -1;
	}
	/*
	**  Create new object, if necessary.
	*/
	time = STalloc(UGcat_now());
	if (id == -1)
	{
		/*
		**  Need to allocate a permanent copy of the form
		**  name since abf/vision may delete the form and
		**  leave us with a pointer to deallocated memory
		**  or into someone else's area.
		*/
		fname = STalloc(form->frname);
		if (IIOOidWrite(class, &id, fname, (PTR)0, uidbdata->user,
			(PTR)1, short_msg, time, time, long_msg) != OK)
		{
			/*
			**  Couldn't create object.
			*/
			IIUGerr(E_CF0023_OBJ_SAVE, UG_ERR_ERROR, 1,
				form->frname);
			return(1);
		}
	}
	else
	{
		/*
		**  Just update modified time for existing object.
		**  Alter_count not set currently.
		*/
		iiuicw1_CatWriteOn();
/* # line 521 "cfsave.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_objects set alter_date=");
    IIputdomio((short *)0,1,32,0,time);
    IIwritio(0,(short *)0,1,32,0,(char *)"where object_id=");
    IIputdomio((short *)0,1,30,4,&id);
    IIsyncup((char *)0,0);
  }
/* # line 523 "cfsave.sc" */	/* host code */
		iiuicw0_CatWriteOff();
	}
	iiuicw1_CatWriteOn();
	IIUIbeginXaction();
	total_fields = 0;
	for (i = 0, nfld = form->frfldno, fl = form->frfld; i < nfld;
		i++, fl++)
	{
		if ((*fl)->fltag == FREGULAR)
		{
			if (IICFsfSaveFlds(id, (*fl)->fltag,
				&((*fl)->fld_var.flregfld->flhdr),
				&((*fl)->fld_var.flregfld->fltype),
				&((*fl)->fld_var.flregfld->flval), NULL) != OK)
			{
				/*
				**  Error trying to write to db.
				*/
				IIUGerr(E_CF0022_SAVE_ERROR, UG_ERR_ERROR,
					2, form->frname,
					ERget(F_CF0002_FIELDS));
				IIUIabortXaction();
				IIOOidDestroy(&class, &id, form->frname,
					uidbdata->user);
				return(1);
			}
			total_fields++;
		}
		else
		{
			tbl = (*fl)->fld_var.fltblfld;
			if (IICFsfSaveFlds(id, (*fl)->fltag,
				&(tbl->tfhdr), NULL, NULL, tbl) != OK)
			{
				/*
				**  Error trying to write to db.
				*/
				IIUGerr(E_CF0022_SAVE_ERROR, UG_ERR_ERROR,
					2, form->frname,
					ERget(F_CF0002_FIELDS));
				IIUIabortXaction();
				IIOOidDestroy(&class, &id, form->frname,
					uidbdata->user);
				return(1);
			}
			total_fields++;
			ncols = tbl->tfcols;
			for (j = 0, cols = tbl->tfflds; j < ncols; j++, cols++)
			{
				if (IICFsfSaveFlds(id, FCOLUMN,
					&((*cols)->flhdr), &((*cols)->fltype),
					(FLDVAL *)NULL, tbl) != OK)
				{
					/*
					**  Error trying to write to db.
					*/
					IIUGerr(E_CF0022_SAVE_ERROR,
						UG_ERR_ERROR, 2, form->frname,
						ERget(F_CF0002_FIELDS));
					IIUIabortXaction();
					IIOOidDestroy(&class, &id,
						form->frname, uidbdata->user);
					return(1);
				}
				total_fields++;
			}
		}
	}
	/*
	**  Saving trim.
	*/
	for (i = 0, ntrim = form->frtrimno, tl = form->frtrim; i < ntrim;
		i++, tl++)
	{
		t_str = (*tl)->trmstr;
		t_x = (*tl)->trmx;
		t_y = (*tl)->trmy;
		t_flags = (*tl)->trmflags;
		t_2flags = (*tl)->trm2flags;
		t_font = (*tl)->trmfont;
		t_ptsz = (*tl)->trmptsz;
/* # line 604 "cfsave.sc" */	/* repeat insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cfsave3",6470,24506);
      IIputdomio((short *)0,1,30,4,&id);
      IIputdomio((short *)0,1,30,4,&t_x);
      IIputdomio((short *)0,1,30,4,&t_y);
      IIputdomio((short *)0,1,30,4,&t_flags);
      IIputdomio((short *)0,1,30,4,&t_2flags);
      IIputdomio((short *)0,1,30,4,&t_font);
      IIputdomio((short *)0,1,30,4,&t_ptsz);
      IIputdomio((short *)0,1,32,0,t_str);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cfsave3",6470,24506);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_trim(object_id, trim_col, trim_lin, trim_flags, trim2_\
flags, trim_font, trim_ptsz, trim_trim)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&t_x);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&t_y);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,4,&t_flags);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&t_2flags);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,4,&t_font);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,30,4,&t_ptsz);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,t_str);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"cfsave3",6470,24506);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 609 "cfsave.sc" */	/* host code */
		if (FEinqerr())
		{
			/*
			**  Error trying to write to db.
			*/
			IIUGerr(E_CF0022_SAVE_ERROR, UG_ERR_ERROR,
				2, form->frname, ERget(F_CF0003_TRIM));
			IIUIabortXaction();
			IIOOidDestroy(&class, &id, form->frname, uidbdata->user);
			return(1);
		}
	}
	form->frversion = FRMVERSION;	/* Value not previously set. */
	/*
	**  Saving into ii_forms.
	*/
	f_fldno = form->frfldno;
	f_nsfldno = form->frnsno;
	f_trimno = form->frtrimno;
	f_maxx = form->frmaxx;
	f_maxy = form->frmaxy;
	f_posx = form->frposx;
	f_posy = form->frposy;
	f_version = form->frversion;
	f_flags = form->frmflags;
	f_2flags = form->frm2flags;
/* # line 635 "cfsave.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_forms(object_id, frmaxcol, frmaxlin, frposy, frposx, f\
rfldno, frnsno, frtrimno, frflags, frversion, frscrtype, frscrmaxx, fr\
scrmaxy, frscrdpix, frscrdpiy, fr2flags, frtotflds)values(");
    IIputdomio((short *)0,1,30,4,&id);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&f_maxx);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&f_maxy);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&f_posy);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&f_posx);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&f_fldno);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&f_nsfldno);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&f_trimno);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&f_flags);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&f_version);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&f_scrtype);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&f_scrxmax);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&f_scrymax);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&f_scrxdpi);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&f_scrydpi);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&f_2flags);
    IIwritio(0,(short *)0,1,32,0,(char *)", ");
    IIputdomio((short *)0,1,30,4,&total_fields);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 644 "cfsave.sc" */	/* host code */
	if (FEinqerr())
	{
		/*
		**  Error trying to write to db.
		*/
		IIUGerr(E_CF0022_SAVE_ERROR, UG_ERR_ERROR,
			2, form->frname, ERget(F_CF0004_FORMS));
		IIUIabortXaction();
		IIOOidDestroy(&class, &id, form->frname, uidbdata->user);
		return(1);
	}
	/*
	**  Now, do encoded forms.  Can't use similar routines
	**  in vifred/copyform since they are too hardwired
	**  to their respective environments.
	*/
	if (!IIUGdaDiffArch())
	{
		if (IICFefEncodeForm(id, form) != OK)
		{
			/*
			**  Error trying to write to db.
			*/
			IIUGerr(E_CF0022_SAVE_ERROR, UG_ERR_ERROR,
				2, form->frname, ERget(F_CF0005_ENCODED));
			IIUIabortXaction();
			IIOOidDestroy(&class, &id, form->frname, uidbdata->user);
			return(1);
		}
	}
	IIUIendXaction();
	iiuicw0_CatWriteOff();
	return(OK);
}
