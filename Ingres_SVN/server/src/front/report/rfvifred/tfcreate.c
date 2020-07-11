# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  Copyright (c) 1983, 2004 Ingres Corporation
*/
/*
**  TFCREATE:
**	This file contains code to allow a user to create
**	table fields in VIFRED.
**	The routines are written in EQUEL/FORMS/C with
**	Neil's enhancements.  Essentially, this is
**	table fields by table fields.
**
**  History:
**	06/01/83 (dkh)	Created
**	01/16/85 (drh)	Modified all MEcalloc calls to FEcalloc and eliminated
**			all MEfree calls since storage is not tagged and freed
**			via an FEfree.
**	01/28/85 (drh)	Eliminated newline in error message.
**	02/01/85 (drh)	Copied PeterK's change locally allocating tfcols[]
**			to avoid worst-case space usage.
**	02/14/85 (drh)	Added bug fix from VMS limiting the number of rows
**			displayed in a table fields from 1 - 99.
**	06/27/85 (gac)	Replaced sleeped messages by FEmsg.
**
**	06/22/85 (dkh)	Added support for titleless table fields.
**	08/19/85 (dkh)	Added support for invisible table fields.
**	11/23/85 (dkh)	Fix for BUG 7024.
**	11/23/85 (dkh)	Fix for BUG 6075.
**	09/10/86 (a.dea) -- Initialize invis, xbot, titledisp, & topdisp.
**	10/09/86 (sandyd)  Added Saturn support.  Must connect before
**			accessing "attribute" relation.
**	10/20/86 (KY)  -- Changed CH.h to CM.h.
**	11/18/86 (KY)  -- Changed STindex,STrindex for Double Bytes characters.
**	03/13/87 (dkh) - Added support for ADTs.
**	04/25/87 (dkh) - Deleted FEconnect and FEdisconnect references.
**	28-apr-87 (bruceb)
**		When processing data format, set or blank the flag that
**		indicates if right-to-left input and display is
**		specified or not.
**	06/22/87 (dkh) - Fixed to use NEW display submenu correctly.
**	13-jul-87 (bruceb) Changed memory allocation to use [FM]Ereqmem.
**	08/04/87 (dkh) - Changed to use compile forms in a file.
**	08/08/87 (dkh) - Usability changes for editing datatypes/formats.
**	08/14/87 (dkh) - ER changes.
**	08/29/87 (dkh) - Fixed jup bug 911.
**	10/02/87 (dkh) - Help file changes.
**	10/25/87 (dkh) - Error message cleanup.
**	11/30/87 (dkh) - Added support for tf no column separators.
**	11-jan-88 (sylviap)
**		Created the .qsc file.
**	01/22/88 (dkh) - Changed to use FErel_ffetch & FEatt_fopen() for speed.
**	02-feb-88 (sylviap)
**		Changed CVlower to IIUGlbo_lowerBEobject to accommodate mixed
**		case table names.  Internal field names remain lower case.
**	09-feb-88 (bruceb)
**		Init fhd2flags to 0 when doing same for fhdflags.
**	17-feb-88 (sylviap)
**		Lower case the table field name.
**	04/20/88 (dkh) - Made toggle on displaying table field column name
**			 a public feature.
**	11-may-88 (sylviap)
**		Added SQL COMMIT after FE calls to release the locks on
**		  the system catalogs.
**		Added FEatt_close in gettblattr at the error condition
**		  before returning from function call.
**		Added CVlower to showlines and cursortrack to accept "Y"
**		  and "N". (bug #2571)
**	05/23/88 (dkh) - Added user support for scrollable fields.
**	16-jun-88 (sylviap)
**		Took out SQL COMMIT in the FErel_ffetch loop.
**	18-jul-88 (marian)
**		Added check after call to fmt_setfmt() to see if a bad format
**		was specified.  The check is done in vifred instead of fmt 
**		because of the way report writer passes formats down to fmt. 
**		Report writer was examined to determine if it could send the
**		correct format down to fmt, but it would mean having to parse
**		the exact string that fmt would be parsing before calling fmt
**		and then calling fmt again.  Adding the check here seemed to
**		be a better solution.
**	09/21/88 (dkh) - Fixed venus bug 3425.
**	06-oct-88 (sylviap)
**		Added TITAN changes.  DB_MAX_COLS -> DB_GW2_MAX_COLS
**	16-dec-88 (bruceb)
**		Made toggle on invisibility of table fields a public feature.
**	01/18/89 (dkh) - Fixed problems with creating table fields from
**			 a table definition.
**	02/18/89 (dkh) - Yet more changes for performance. (dkh)
**	03/22/89 (dkh) - Fixed venus bug 4942.
**	07-apr-89 (bruceb)
**		New 'flags' hidden column in table field.  If table field
**		column (of edited table field) is VQ restricted, don't
**		allow deletion of the row, or modification of the internal
**		name.
**	08-apr-89 (bruceb)
**		Made disptop, dispbotx and colsep fields on tfcreate form
**		invisible (or not) rather than mode query/fill.  Corresponding
**		change to the form definition.  Note that variables
**		representing these fields are passed without being set if
**		not RTI special use; I just don't reference the values under
**		these circumstances.
**	13-jun-89 (bruceb)	Fix for bug 5357.
**		Removed activation on Name field.  Cleans up the interface.
**	11-sep-89 (bruceb)
**		On building the table field, if all columns are marked
**		invisible, set the table field unseen (invisible by any
**		other name--but a different flag).
**	19-sep-89 (bruceb)
**		Call vfgetSize() with a precision argument.  Done for decimal
**		support.
**	27-nov-89 (bruceb)
**		Find/top/bottom no longer visible menuitems.
**		Explanations for menuitems were added to matching frskeys.
**	11/16/89 (dkh) - Changed buffer size(s) for owner.table support.
**	01/24/90 (dkh) - Moved include of er.h ahead of uf.h due to changes
**			 in uf.h.
**	21-feb-90 (bruceb)
**		Implement SIR 3608:  Column titles when creating a table
**		field from a table will now be capitalized with spaces
**		replacing underscores.
**	15-mar-90 (griffin)
**		Turned optimization off for hp3_us5 port.
**	17-apr-90 (bruceb)
**		Lint changes; removed args in call on FDwtblalloc.
**	18-apr-1990 (jhw)  Modified back to call 'FEatt_open()'.
**	08-jun-90 (bruceb)
**		GetTableDef now uses a pop-up to prompt for the table name,
**		adding a ListChoices menu item off the pop-up.
**      06/09/90 (esd) - Check IIVFsfaSpaceForAttr instead of
**                       calling FTspace, so that whether or not
**                       to leave room for attribute bytes can be
**                       controlled on a form-by-form basis.
**      06/09/90 (esd) - Allow space for 3270 attributes around title.
**      06/12/90 (esd) - Add new parameter to call to palloc indicating
**                       whether the feature requires attribute bytes
**                       on each side.
**	04-oct-90 (bruceb)
**		Trim off leading blanks in format strings.
**	25-jul-91 (johnr)
**		Removed NO_OPTIM hp3_us5 - not needed in hp-ux 8.0
**	08/19/91 (dkh) - Integrated changes from PC group.
**	06/20/92 (dkh) - Added support for decimal datatype for 6.5.
**	06/30/92 (dkh) - Added support for setting attributes for all
**			 columns in a table field in one shot.
**	09/07/92 (dkh) - Updated with 6.5 call interface to FEatt_open.
**	09/07/92 (dkh) - Updated with 6.5 call interface to FErelexists.
**	09/20/92 (dkh) - Added support for owner.table.
**	08/31/93 (dkh) - Fixed up compiler complaints due to prototyping.
**	26-jan-94 (connie) Bug #57718
**		For using JoinDef in creating QbfName, validate the joindef
**		name and fix the listchoice of joindefs.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	07-mar-2001 (somsa01)
**	    Changed maxcharwidth from 10000 to FE_FMTMAXCHARWIDTH.
**      13-feb-03 (chash01) x-integrate change#461908
**          In gettable()
**          a local character array cannot be returned as value.  Its pointer
**          that is returned to caller is potentially out of scope, and 
**          in certain circumstance this pointer is not valid at all.  Declare
**          the variable tname to be static.
**	25-feb-2004 (somsa01)
**	    On Windows 64-bit, i4 does not become 8 bytes. Thus, declare
**	    ccol as SIZE_TYPE in loadtable(), declare ptr as SIZE_TYPE in
**	    unloadtbl(), and declare ccol as SIZE_TYPE in tfhandler().
*/
# include	<compat.h>
# include	<cv.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	"decls.h"
# include	"vfunique.h"
# include	"seq.h"
# include	<cm.h>
# include	<lo.h>
# include	<si.h>
# include	<er.h>
# include	<st.h>
# include	<ui.h>
# include	<uf.h>
# include	<me.h>
# include	<ug.h>
# include <ooclass.h>
# include <uigdata.h>
# include	"ervf.h"
# include	"vfhlp.h"
# define	stUNDEF		0
# define	stNEW		1
# define	stUNCHANGED	2
# define	stCHANGE	3
# define	stDELETE	4
# define	RELSPEC		0
# define	EDITING		1
# define	CHGATTR		2
# define	VFTFCREATE	0
# define	EDITED		1
# define	NEXT		0
# define	PREVIOUS	1
#define _Help_Expl	F_FE0100_ExplFrameHelp
#define _End_Expl	F_FE0102_ExplEnd
FUNC_EXTERN	STATUS		FErelexists();
FUNC_EXTERN	ADF_CB		*FEadfcb();
FUNC_EXTERN	STATUS		fmt_deffmt();
FUNC_EXTERN	DB_STATUS	adc_lenchk();
FUNC_EXTERN	STATUS		fmt_ftot();
FUNC_EXTERN	STATUS		afe_cancoerce();
FUNC_EXTERN	bool		fmt_justify();
FUNC_EXTERN	char		*IIFDftFormatTitle();
FUNC_EXTERN	char		*IITUtpTblPik();
FUNC_EXTERN     char    	*IIUFopObjPik();
FUNC_EXTERN	STATUS		vftblnmchk();
FUNC_EXTERN	void		IIVFgtmGetTblName();
struct vftf
{
	char	*title;
	char	*format;
	char	*name;
	i4	length;
	FLDCOL	*col;
	FMT	*fmt;
};
static struct vftf	*tfcols = NULL;
static i4	tfcounter = 0;
GLOBALDEF POS	edtfps = {0};
GLOBALDEF POS	*ntfps = NULL;
static POS	*stfps = NULL;
static SMLFLD	*edtfsfd = NULL;
GLOBALDEF FIELD	*edtffld = NULL;
static i4	rowwidth = 0;
static
  char _Form[]=	ERx("tfcreate"), _TableFld[]=	ERx("tfcreate"), _NameFld[]=	ERx("name")
  ;
static READONLY
  char _Empty[]=	ERx("");
static	bool	tfalloced = FALSE;
static	bool	tnmalloced = FALSE;
static VOID	tfhandler();
/*
** allocTF:
**	Allocate the create table field frame.
*/
VOID
allocTF ()
{
	if (!tfalloced)
	{
		FDsetparse(FALSE);
		if (IIUFgtfGetForm(IIUFlcfLocateForm(), _Form) != OK)
		{
			syserr(ERget(S_VF00D0_NoForm));
		}
		FDsetparse(TRUE);
		tfalloced = TRUE;
	}
}
/*
** vfdflname:
**	Creates a default internal name for
**	a column in a table field and returns
**	the pointer to it.
*/
static char	dflname[FE_MAXNAME + 1] = {0};
static i4	dflnmcount = 1;
static char *
vfdflname()
{
	return STprintf(dflname, ERget(F_VF002C_NEW), dflnmcount++);
}
/*
** vfxname:
**	Transfer a column title to be the internal
**	name of a column.  Maximum of FE_MAXNAME chars and
**	only characters, digits, and underscores
**	are used.  Will stop at first blank if
**	we have at least one good character.
*/
static VOID
vfxname ( dest, src )
char	*dest;
char	*src;
{
	reg	char	*cp1;
	reg	char	*cp2;
		char	buffer[FE_MAXNAME + 1];
	reg	i4	i;
	cp1 = src;
	cp2 = buffer;
	for (i = 0; *cp1 != EOS && i < FE_MAXNAME; CMnext(cp1))
	{
		if (CMspace(cp1) && cp2 != buffer)
		{
			break;
		}
		if (CMnmchar(cp1))
		{
			CMbyteinc(i, cp1);
			if (i <= FE_MAXNAME)
			{
				CMcpychar(cp1, cp2);
				CMnext(cp2);
			}
		}
	}
	*cp2 = EOS;
	if (cp2 == buffer)
	{
		*dest = 0;
	}
	else
	{
		STcopy(buffer, dest);
		CVlower(dest);
	}
}
/*
** gettblattr:
**	Look in the attribute relation to find the attributes for
**	the passed in relation.
**
** History:
**	09-oct-1986 (sandyd)
**		Added FEconnect(), FEdisconnect() for Saturn support.
**		This routine never tested for retrieve errors, so I'm
**		not going to try to add that now.  Fixed an undiscovered
**		bug:  retrieve on "attribute" was not qualified by owner.
**	07-mar-2001 (somsa01)
**	    Changed maxcharwidth from 10000 to FE_FMTMAXCHARWIDTH.
*/
static i4
gettblattr(table, owner)
char	*table;
char	*owner;
{
  char attrname[FE_MAXNAME + 1];
	char		fmtbuf[100];
	char		buf2[100];
	char		*cptr;
	DB_DATA_VALUE	fltdbv;
	FE_ATT_QBLK	qblk;
	FE_ATT_INFO	attinfo;
	DB_DATA_VALUE	ldbv;
	FLDCOL		*col;
	FLDTYPE		*type;
	ADF_CB		*ladfcb;
	bool		numeric = FALSE;
	i4		bad_dtype_count = 0;
        if (FEatt_open(&qblk, table, owner) != OK)
	{
		return(bad_dtype_count);
	}
	ladfcb = FEadfcb();
	fltdbv.db_datatype = DB_FLT_TYPE;
	while(FEatt_fetch(&qblk, &attinfo) == OK)
	{
		FEatt_dbdata(&attinfo, &ldbv);
		if (!IIAFfedatatype(&ldbv))
		{
			bad_dtype_count++;
			continue;
		}
		if (tfcounter >= DB_GW2_MAX_COLS)
		{
			i4 tmp;
			_VOID_ FEatt_close(&qblk);
			tmp = DB_GW2_MAX_COLS;
			IIUGerr(E_VF00D6_TooManyCols, UG_ERR_ERROR, 1, &tmp);
			return(bad_dtype_count);
		}
		IIUGfnFeName(attinfo.column_name, attrname);
		STtrmwhite(attrname);
		CVlower(attrname);
		if (fmt_deffmt( ladfcb, &ldbv, FE_FMTMAXCHARWIDTH,
				(bool) FALSE, buf2 ) != OK)
		{	
			_VOID_ FEatt_close(&qblk);
			/*
			**  Commit to release locks on the system catalogs.
			*/
			IIUGerr(E_VF00D1_BadColDefault, UG_ERR_ERROR, 0);
			return(bad_dtype_count);
		}
		/*
		**  Datatype is compatible with a float (i.e., a numeric),
		**  then put a leading minus sign on the format.
		*/
		cptr = (afe_cancoerce(ladfcb, &ldbv, &fltdbv, &numeric) == OK &&
					numeric)
			? STprintf(fmtbuf, ERx("-%s"), buf2) : buf2;
		/*
		**  Create new column structure so we can save
		**  the appropriate datatype information for
		**  the column.
		*/
		col = FDnewcol(attrname, tfcounter);
		type = &col->fltype;
		type->ftvalstr = _Empty;
		type->ftvalmsg = _Empty;
		type->ftdefault = _Empty;
		type->ftdatatype = ldbv.db_datatype;
		type->ftlength = ldbv.db_length;
		type->ftprec = ldbv.db_prec;
		tfcols[tfcounter].col = col;
		tfcols[tfcounter].title = saveStr(IIFDftFormatTitle(attrname));
		tfcols[tfcounter].format = saveStr(cptr);
		++tfcounter;
	}
	_VOID_ FEatt_close(&qblk);
	/*
	**  Commit to release locks on the system catalogs.
	*/
	return(bad_dtype_count);
}
/*
** loadtable:
**	Load the table field in form tfcreate with information
**	created by gettblattr.
*/
static VOID
loadtable ( loadcode )
i4	loadcode;
{
  char *ctitle;
  char *cformat;
  char *cname;
  SIZE_TYPE ccol;
  i4 flg;
	i4		i;
	FLDCOL		*col;
	FLDHDR		*hdr;
	for (i = 0; i < tfcounter; i++)
	{
		ctitle = tfcols[i].title;
		cformat = tfcols[i].format;
		ccol = (SIZE_TYPE) tfcols[i].col;
		cname = tfcols[i].name;
		switch (loadcode)
		{
			case RELSPEC:
				/* Col name is already set by gettblattr(). */
				col = (FLDCOL *)ccol;
				hdr = &col->flhdr;
				cname = hdr->fhdname;
				/* New col, so no possibility of restriction. */
/* # line 504 "tfcreate.qsc" */	/* loadtable */
  {
    if (IItbact(_Form,_TableFld,1) != 0) {
      IItcoputio((char *)"title",(short *)0,1,32,0,ctitle);
      IItcoputio((char *)"format",(short *)0,1,32,0,cformat);
      IItcoputio((char *)"internname",(short *)0,1,32,0,cname);
      IItcoputio((char *)"clptr",(short *)0,1,30,4,&ccol);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 507 "tfcreate.qsc" */	/* host code */
				break;
			case EDITING:
				col = (FLDCOL *)ccol;
				hdr = &col->flhdr;
				flg = hdr->fhd2flags & fdVQLOCK;
/* # line 513 "tfcreate.qsc" */	/* loadtable */
  {
    if (IItbact(_Form,_TableFld,1) != 0) {
      IItcoputio((char *)"title",(short *)0,1,32,0,ctitle);
      IItcoputio((char *)"format",(short *)0,1,32,0,cformat);
      IItcoputio((char *)"internname",(short *)0,1,32,0,cname);
      IItcoputio((char *)"clptr",(short *)0,1,30,4,&ccol);
      IItcoputio((char *)"flags",(short *)0,1,30,4,&flg);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 516 "tfcreate.qsc" */	/* host code */
				break;
			case CHGATTR:
				col = (FLDCOL *)ccol;
				hdr = &col->flhdr;
				flg = hdr->fhd2flags & fdVQLOCK;
				if ( !STequal(cname, hdr->fhdname) )
				{
					cname = hdr->fhdname;
				}
/* # line 526 "tfcreate.qsc" */	/* loadtable */
  {
    if (IItbact(_Form,_TableFld,1) != 0) {
      IItcoputio((char *)"title",(short *)0,1,32,0,ctitle);
      IItcoputio((char *)"format",(short *)0,1,32,0,cformat);
      IItcoputio((char *)"internname",(short *)0,1,32,0,cname);
      IItcoputio((char *)"clptr",(short *)0,1,30,4,&ccol);
      IItcoputio((char *)"flags",(short *)0,1,30,4,&flg);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 529 "tfcreate.qsc" */	/* host code */
				break;
		}
	}
}
/*
** gettable:
**	Code to get the name of table from user and
**	set up the table field.
**
**	Should write a routine to decompose the line into
**	an array of table names.
*/
static char *
gettable()
{
	reg	char	*cp;
	reg	char	*cp1;
	reg	LIST	*lt = NULL;
	reg	LIST	*lt1 = NULL;
	reg	LIST	*rlist = NULL;
  char tables[FE_PROMPTSIZE + 1];
		char	tmp[FE_PROMPTSIZE + 1];
		char	name[FE_MAXTABNAME + 1]; /* owner.table */
		LIST	*nlist = NULL;
		LIST	*olist = NULL;
		LIST	*olt = NULL;
	static  char	tname[FE_MAXTABNAME + 1];
		i4	count;
		i4	bad_count = 0;
	tables[0] = EOS;
	for (;;)
	{
		IIVFgtmGetTblName(tables, ERget(F_VF00BD_ENTER_TNAME),
			TF_TBL_PROMPT);
		if (tables[0] == EOS)
		{
			return(NULL);
		}
		STcopy(tables, tmp);
# ifdef junk
/*
**		If running against a gateway, table names may be mixed case.
**		Lower case table names if running against the INGRES DBMS.
*/
		IIUGlbo_lowerBEobject(tables);
# endif
/* # line 580 "tfcreate.qsc" */	/* message */
  {
    IImessage(ERget(S_VF00D3_Working));
  }
/* # line 582 "tfcreate.qsc" */	/* host code */
		if (vftblnmchk(tmp, &nlist, &olist, &count) != OK)
		{
			continue;
		}
		if (nlist == NULL)
		{
			continue;
		}
		for (lt = nlist, olt = olist; lt != NULL;
			lt = lt->lt_next, olt = olt->lt_next)
		{
			bad_count += gettblattr(lt->lt_feat, olt->lt_feat);
		}
		/*
		**  If there were unsupported datatypes,
		**  issue a single error here.
		*/
		if (bad_count)
		{
			IIUGerr(E_VF0172_BAD_DTYPE_FOUND, UG_ERR_ERROR, 0);
		}
		loadtable(RELSPEC);
		/*
		**  Need to make sure that the name returned is
		**  a valid FE name.
		*/
		IIUGfnFeName(nlist->lt_feat, tname);
		/*
		**  Free up the LIST structs before returning.
		*/
		for ( ; nlist != NULL; )
		{
			lt = nlist;
			olt = olist;
			nlist = nlist->lt_next;
			olist = olist->lt_next;
			lfree(lt);
			lfree(olt);
		}
		return(tname);
	}
}
/*
** unloadtbl:
**	Code to unload the table field in form "tfcreate"
**	and to generate a table field defined from what
**	is unloaded.
**
**	Returns True if no errors found during the unloading.
**	Otherwise, False is returned.
**
**	NEED TO CHECK FOR NULL TITLE, FORMAT, AND NAME during unload!!!
*/
static bool
unloadtbl ()
{
  char utitle[100];
  char uformat[100];
  char uname[FE_MAXNAME + 1];
  i4 ustate;
  i4 urow;
  SIZE_TYPE ptr;
	char		*str;
	register ADF_CB	*ladfcb;
	i4		uerrs = 0;
	i4		nmerrs = 0;
	ladfcb = FEadfcb();
	vfstdel(ATBLFD);
	tfcounter = 0;
	rowwidth = 0;
/* # line 665 "tfcreate.qsc" */	/* unloadtable */
  {
    if (IItbact(_Form,_TableFld,0) == 0) goto IItbE1;
    while (IItunload() != 0) {
      IItcogetio((short *)0,1,32,99,utitle,(char *)"title");
      IItcogetio((short *)0,1,32,99,uformat,(char *)"format");
      IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,uname,(char *)
"internname");
      IItcogetio((short *)0,1,30,4,&ptr,(char *)"clptr");
      IItcogetio((short *)0,1,30,4,&ustate,(char *)"_STATE");
      IItcogetio((short *)0,1,30,4,&urow,(char *)"_RECORD");
      IITBceColEnd();
      {
/* # line 670 "tfcreate.qsc" */	/* host code */
 	    register FMT	*ufmt;
	    str = uformat;
	    switch (ustate)
	    {
		case stNEW:
		case stUNCHANGED:
		case stCHANGE:
		    if ((utitle[0] == EOS) && (uformat[0] == EOS)
			&& (uname[0] == EOS))
		    {
			/* This is really a 'deleted' row.  Ignore it. */
			break;
		    }
		    str = STskipblank(str, STlength(str));
		    if ( str == NULL || (ufmt = vfchkFmt(str)) == NULL )
		    {
			/* Use uformat here because str could be NULL. */
			IIUGerr(E_VF00D5_InvalidColFmt, UG_ERR_ERROR,
				3, uname, &urow, uformat
			);
			++uerrs;
		    }
		    else
		    {
			register FLDCOL	*col;
			i4		x;
			i4		y;
			i4		len;
			if ( tfcounter >= DB_GW2_MAX_COLS )
			{
			    i4 tmp;
			    tmp = DB_GW2_MAX_COLS;
			    IIUGerr(E_VF00E5_TooManyCols, UG_ERR_ERROR,
				1, &tmp
			    );
			    ++uerrs;
/* # line 710 "tfcreate.qsc" */	/* endloop */
        {
          if (1) goto IItbE1;
        }
/* # line 711 "tfcreate.qsc" */	/* host code */
			}
			/*
			**  Check for compatibility of datatype
			**  with format spec.
			*/
			if ( (col = (FLDCOL *) ptr) != NULL )
			{
			    register FLDTYPE	*ctype;
			    ctype = &(col->fltype);
			    if ( ctype->ftdatatype != DB_NODT )
			    {
				bool		is_str;
				bool		valid = FALSE;
				DB_DATA_VALUE	dbv;
				dbv.db_datatype = ctype->ftdatatype;
				dbv.db_length = ctype->ftlength;
				dbv.db_prec = ctype->ftprec;
				if ( fmt_vfvalid(ladfcb, ufmt, &dbv, &valid,
						&is_str) != OK || !valid )
				{
				    IIUGerr(E_VF00D7_BadFmtType,
					UG_ERR_ERROR, 2, uname, &urow
				    );
				    ++uerrs;
				    break;
				}
				/*
				**  If the datatype is a decimal datatype,
				**  make sure we have the latest information
				**  on the precision/scale values.
				*/
				if (abs(dbv.db_datatype) == DB_DEC_TYPE)
				{
					ctype->ftlength = dbv.db_length;
					ctype->ftprec = dbv.db_prec;
				}
				if ( is_str )
				{
				    /*
				    **  Check for scrollability.  If column is
				    **  scrollable, then leave 'ftlength' alone.
				    ** 	We already know that the datatype is one
				    **  of the string types, otherwise, we would
				    **  not be here.
				    */
				    if ( (((FLDHDR *)&(col->flhdr))->fhd2flags &
						fdSCRLFD) != 0 )
				    { /* scrollable */
				        _VOID_ fmt_size(ladfcb, ufmt,
							(DB_DATA_VALUE *)NULL,
							&y, &x
					);
				        if ( y != 1 || 
						fmt_justify(ladfcb, ufmt) )
				        { /* multiple row or justified */
					    /*
					    **  Format no longer qualifies
					    **  column for scrollability.
					    */
					    IIUGerr(E_VF011E_ColScrollFmt,
							UG_ERR_ERROR,
							3, uname, &urow, str
					    );
					    ++uerrs;
					    break;
					}
					if ( x >= ctype->ftlength )
					{ /* too long */
					    /*
					    **  Format length has increased
					    **  so that column no longer
					    **  qualifies for scrollability.
					    */
					    IIUGerr(E_VF011F_ColScrollSize,
							UG_ERR_ERROR,
							3, uname, &urow, str
					    );
					    ++uerrs;
					    break;
					}
				    }
				    else
				    {
				        ctype->ftlength = dbv.db_length;
				    }
				}
			    }
			}
			if ( uname[0] == EOS )
			{
			    if ( utitle[0] == EOS )
			    {
				STcopy(vfdflname(), uname);
				CVlower(uname);
			    }
			    else
			    {
				vfxname(uname, utitle);
				if ( uname[0] == EOS )
				{
				    STcopy(vfdflname(), uname);
				    CVlower(uname);
				}
			    }
			}
			else if ( !vffnmchk(uname, ERget(F_VF0032_column)) )
			{
			    ++uerrs;
			    break;
			}
			vfgetSize(ufmt, 0, 0, 0, &y, &x, &len);
			CVlower(uname);
			if ( vfnmunique(uname, TRUE, ATBLFD) == FOUND )
			{
			    IIUGerr(E_VF00D8_DupColName, UG_ERR_ERROR,
					1, uname
			    );
			    ++nmerrs;
			}
			tfcols[tfcounter].title = saveStr(utitle);
			tfcols[tfcounter].format = saveStr(str);
			tfcols[tfcounter].name = saveStr(uname);
			tfcols[tfcounter].length = x;
			tfcols[tfcounter].fmt = ufmt;
			tfcols[tfcounter].col = (FLDCOL *)ptr;
			++tfcounter;
			rowwidth = max(rowwidth, y);
		    }
		    break;
		default:
		    break;
	    }
      }
    } /* IItunload */
IItbE1:
    IItunend();
  }
/* # line 850 "tfcreate.qsc" */	/* host code */
	if ( nmerrs > 0 || uerrs > 0 )
	{
		return FALSE;
	}
	else if ( tfcounter == 0 )
	{
		IIUGerr(E_VF00D9_NoColsSpecified, UG_ERR_ERROR, 0);
		return FALSE;
	}
	return TRUE;
}
/*
** tfbuild:
**	Build the frame structures, etc. for a table
**	field in VIFRED.
*/
static FIELD *
tfbuild(name, rows, showlines, curtrack, titledisp, stat, invisible,
						topdisp, xbot, xcolsep)
char	*name;
i4	rows;
char	*showlines;
char	*curtrack;
char	*titledisp;
i4	stat;
char	*invisible;
char	*topdisp;
char	*xbot;
char	*xcolsep;
{
	i4		i;
	i4		linetot;
	i4		length;
	i4		nmlen;
	bool		displines;
	i4		colsep;
	bool		exists;
	i4		colwidth;
	i4		spacesize;
	FIELD		*fld;
	FLDHDR		*hdr;
	TBLFLD		*tf;
	ADF_CB		*ladfcb;
	DB_DATA_VALUE	fltdbv;
	bool		visible_column = FALSE;
	spacesize = IIVFsfaSpaceForAttr;
	fltdbv.db_datatype = DB_FLT_TYPE;
	displines = ( showlines[0] == *IIVF_yes1 );
	/* Calculate table field width based on title and column width */
	linetot = 1;
	colsep = ( !iivfRTIspecial || xcolsep[0] == *IIVF_yes1 ) ? 1 : 0;
	for (i = 0; i < tfcounter; i++)
	{
		nmlen = STlength(tfcols[i].title);
		length = tfcols[i].length;
		colwidth = max(length, nmlen) + spacesize + spacesize;
		linetot += colwidth + colsep;
	}
	/*
	**  Add one for the right vertical line of the table field
	**  if there are no column separators.
	*/
	if ( colsep == 0 )
	{
		++linetot;
	}
	if (linetot > endxFrm + 1)
	{
		vfwider(frame, linetot - endxFrm - 1);
	}
	/* Create or get Table Field form object. */
	if (stat == VFTFCREATE)
	{
		fld = FDnewfld(name, tfcounter, FTABLE);
		tf = fld->fld_var.fltblfld;
		hdr = &tf->tfhdr;
		/* Clear table field title */
		hdr->fhtitle = _Empty;
		hdr->fhtity = 1;
		hdr->fhtitx = 1 + spacesize;
		hdr->fhposx = 0;
		hdr->fhposy = 0;
	}
	else
	{
		fld = (FIELD *)stfps->ps_feat;
		tf = fld->fld_var.fltblfld;
		hdr = &tf->tfhdr;
		STcopy(name, hdr->fhdname);
		fld->fltag = FTABLE;
	}
	/* Set table field header flags */
	if (!displines)
	{
		hdr->fhdflags |= fdNOTFLINES;
	}
	else
	{
		hdr->fhdflags &= ~fdNOTFLINES;
	}
	if (curtrack[0] == *IIVF_yes1)
	{
		hdr->fhdflags |= fdROWHLIT;
	}
	else
	{
		hdr->fhdflags &= ~fdROWHLIT;
	}
	if (titledisp[0] == *IIVF_no1)
	{
		hdr->fhdflags |= fdNOTFTITLE;
	}
	else
	{
		hdr->fhdflags &= ~fdNOTFTITLE;
	}
	if ( invisible[0] == *IIVF_yes1 )
	{
		hdr->fhdflags |= fdINVISIBLE;
	}
	else
	{
		hdr->fhdflags &= ~fdINVISIBLE;
	}
	if ( iivfRTIspecial )
	{
		if ( topdisp[0] == *IIVF_no1 )
		{
			hdr->fhdflags |= fdNOTFTOP;
		}
		else
		{
			hdr->fhdflags &= ~fdNOTFTOP;
		}
		if ( xbot[0] == *IIVF_yes1 )
		{
			hdr->fhdflags |= fdTFXBOT;
		}
		else
		{
			hdr->fhdflags &= ~fdTFXBOT;
		}
		if ( xcolsep[0] == *IIVF_no1 )
		{
			hdr->fhdflags |= fdNOCOLSEP;
		}
		else
		{
			hdr->fhdflags &= ~fdNOCOLSEP;
		}
	}
	tf->tfrows = rows;
	tf->tfcols = tfcounter;
	tf->tfwidth = rowwidth;
	tf->tfstart = 0;
	if ( !(hdr->fhdflags & fdNOTFTOP) )
		tf->tfstart += 1;	/* table field top line */
	if ( hdr->fhtitle != NULL && *hdr->fhtitle != EOS )
		tf->tfstart += 2;	/* table field title + separator */
	if ( !(hdr->fhdflags & fdNOTFTITLE) )
		tf->tfstart += 2;	/* column title + separator */
	hdr->fhmaxx = linetot;
	hdr->fhmaxy = (rows * rowwidth) + (displines ? rows : 1) + tf->tfstart;
	if (FDwtblalloc(tf) == FALSE)
	{
		return(NULL);
	}
	linetot = 1;
	exists = FALSE;
	ladfcb = FEadfcb();
	for (i = 0; i < tfcounter; i++)
	{
		reg FLDCOL	*col;
		reg FLDTYPE	*type;
		char		*fmtstr;
		FMT		*fmtptr;
		i4		fmtsize;
		i4		x;
		i4		y;
		bool		numeric = FALSE;
		bool		reverse = FALSE;
		DB_DATA_VALUE	dbv;
		if (tfcols[i].col == NULL)
		{
			col = tf->tfflds[i] = FDnewcol(tfcols[i].name, i);
		}
		else
		{
			exists = TRUE;
			col = tf->tfflds[i] = tfcols[i].col;
		}
		hdr = &col->flhdr;
		type = &col->fltype;
		if (exists)
		{
			hdr->fhseq = i;
			if ( !STequal(hdr->fhdname, tfcols[i].name) )
			{
				STcopy(tfcols[i].name, hdr->fhdname);
			}
		}
		hdr->fhtitle = tfcols[i].title;
		hdr->fhtitx = linetot + spacesize;
		hdr->fhtity = tf->tfstart - 2;
		type->ftdatax = linetot + spacesize;
		/*
		**  Check for adjustments to the format
		**  for numeric datatypes.
		*/
		fmtstr = tfcols[i].format;
		fmtptr = tfcols[i].fmt;
		if (!exists || type->ftdatatype == DB_NODT)
		{
			if (fmt_ftot(ladfcb, fmtptr, &dbv) != OK)
			{
				IIUGerr(E_VF00DA_BadFmtConvert,
					UG_ERR_ERROR, 0
				);
				return(NULL);
			}
			type->ftdatatype = dbv.db_datatype;
			type->ftlength = dbv.db_length;
			type->ftprec = dbv.db_prec;
		}
		else
		{
			dbv.db_datatype = type->ftdatatype;
		}
		if (afe_cancoerce(ladfcb, &dbv, &fltdbv, &numeric) == OK &&
			numeric)
		{
			if (*fmtstr != '-' && *fmtstr != '+')
			{
				PTR	blk;
				i4	fmtlen;
				char	fmtbuf[100];
				fmtstr = saveStr( STprintf(fmtbuf, ERx("-%s"),
								fmtstr)
				);
				/*
				**  All the checks should already
				**  have been done by previous
				**  calls to vfchkFmt().  So
				**  retrun values for these fmt
				**  calls will be ignored.
				*/
				_VOID_ fmt_areasize(ladfcb, fmtstr, &fmtsize);
				if ((blk = FEreqmem((u_i4)0, (u_i4)fmtsize,
				    TRUE, (STATUS *)NULL)) == NULL)
				{
					IIUGbmaBadMemoryAllocation(
						ERx("tfbuild"));
					return(NULL);
				}
				_VOID_ fmt_setfmt(ladfcb, fmtstr, blk,
					&fmtptr, &fmtlen);
			}
		}
		type->ftfmtstr = fmtstr;
		type->ftfmt = fmtptr;
		nmlen = STlength(tfcols[i].title);
		length = tfcols[i].length;
		vfgetSize(type->ftfmt, type->ftdatatype, type->ftlength,
			type->ftprec, &y, &x, &(type->ftwidth));
		colwidth = max(length, nmlen) + spacesize + spacesize;
		linetot += colwidth;
		if (linetot < endxFrm + 1)
		{
			type->ftdataln = length;
			hdr->fhmaxx = colwidth;
			hdr->fhmaxy = 1;
			hdr->fhposy = 0;
			hdr->fhposx = linetot - colwidth;
			linetot += colsep;
		}
		else
		{
			return(NULL);
		}
		if (!exists)
		{
			type->ftvalstr = _Empty;
			type->ftvalmsg = _Empty;
			type->ftdefault = _Empty;
			hdr->fhintrp = 0;
			hdr->fhdflags = 0;
			hdr->fhd2flags = 0;
		}
		exists = FALSE;
		/*
		** Ignore return value since only checks are that second and
		** third args aren't NULL, and here that's guaranteed.
		** Just using to see if reversed.
		*/
		_VOID_ fmt_isreversed(ladfcb, type->ftfmt, &reverse);
		if (reverse)
			hdr->fhdflags |= fdREVDIR;
		else
			hdr->fhdflags &= ~fdREVDIR;
		if (!(hdr->fhdflags & fdINVISIBLE))
		{
			visible_column = TRUE;
		}
	} /* end for */
	/* If all cols of the table field are invisible, mark the TF unseen. */
	if (visible_column)
	{
		tf->tfhdr.fhdflags &= ~fdTFINVIS;
	}
	else
	{
		tf->tfhdr.fhdflags |= fdTFINVIS;
	}
	return(fld);
}
/*
** createtbfl:
**	Create the table field and exit if no errors occurred.
*/
static VOID
createtbfl(fd, stat)
FIELD	*fd;
i4	stat;
{
	reg	i4	i;
		TBLFLD	*tab;
		FLDHDR	*hdr;
		i4	y;
		i4	x;
		i4	start;
		POS	*ps;
	tab = fd->fld_var.fltblfld;
	hdr = &tab->tfhdr;
	if (stat == VFTFCREATE)
	{
		x = globx;
		y = globy;
	}
	else
	{
		x = hdr->fhposx;
		y = hdr->fhposy;
	}
	if (stat == VFTFCREATE)
	{
		setGlobUn(enTABLE, NULL, NULL);
	}
	else
	{
		/* NOSTRICT */
		edtfps.ps_feat = (i4 *)edtffld;
		setGlobUn(edTABLE, &edtfps, edtfsfd);
		unLinkPos(stfps);
		vfersPos(stfps);
		savePos(stfps);
	}
	if (x + hdr->fhmaxx > endxFrm + 1)
	{
		IIUGerr(E_VF00DB_TooBigTblFld, UG_ERR_ERROR, 0);
		hdr->fhposx = endxFrm + 1 - hdr->fhmaxx;
		hdr->fhposy = y;
	}
	else
	{
		hdr->fhposx = x;
		hdr->fhposy = y;
	}
	start = hdr->fhposy;
	if (stat == VFTFCREATE)
	{
		ps = palloc(PTABLE, start, hdr->fhposx, hdr->fhmaxy,
			hdr->fhmaxx, (char *) fd, FALSE);
		hdr->fhseq = NEWFLDSEQ;	   /* added to fix bug #1673 (nml) */
	}
	else
	{
		ps = stfps;
		ps->ps_begy = start;
		ps->ps_begx = hdr->fhposx;
		ps->ps_endy = hdr->fhmaxy + start - 1;
		ps->ps_endx = hdr->fhmaxx + hdr->fhposx - 1;
		/* NOSTRICT */
		ps->ps_feat = (i4 *)fd;
	}
	ntfps = ps;
	fitPos(ps, TRUE);
	insPos(ps, TRUE);
	vfTabdisp(tab, FALSE);
	if (stat == VFTFCREATE)
	{
		vffrmcount(ps, 1);
	}
	for (i = 0; i < tfcounter; i++)
	{
		tfcols[i].col = 0;
	}
	globy = ps->ps_begy;
	globx = ps->ps_begx;
}
/*
** nxtfattr:
**	Routine to get the attributes of the
**	next column of a table field for the user
**	to change.
*/
i4
nxtfattr (type, hdrptr, typeptr, seq )
i4	type;
FLDHDR	**hdrptr;
FLDTYPE **typeptr;
i4	seq;
{
	i4	num;
	FLDCOL	*col;
	if (tfcounter == 1)
	{
		return(-1);
	}
	num = seq;
	switch(type)
	{
	case NEXT:
		if (seq == tfcounter)
		{
			num = 1;
		}
		else
		{
			num++;
		}
		col = tfcols[num-1].col;
		break;
	case PREVIOUS:
		if (seq == 1)
		{
			num = tfcounter;
		}
		else
		{
			--num;
		}
		col = tfcols[num-1].col;
		break;
	default:
		col = tfcols[num].col;
		IIUGerr(E_VF00DC_nxtfattrErr, UG_ERR_ERROR, 0);
		break;
	}
	*hdrptr = &col->flhdr;
	*typeptr = &col->fltype;
	return num;
}
/*
** tfedit:
**	Set up form to handle editing a table
**	field.	Calls tfhandler to do all the work.
*/
tfedit(sfd, ps)
SMLFLD	*sfd;
POS	*ps;
{
	reg	i4	i;
	reg	FLDTYPE *type;
	reg	FLDTYPE *stype;
		FIELD	*fld;
		TBLFLD	*tf;
		TBLFLD	*stf;
		FLDHDR	*hdr;
		FLDHDR	*shdr;
		FLDCOL	*col;
		char	*tfname;
		i4	tfrows;
		i4	columns;
		bool	showlines;
		bool	disptitle;
		bool	curtrack;
		bool	invisible;
		bool	topdisp;
		bool	xbot;
		bool	xcolsep;
	struct	vftf	ltfcols[DB_GW2_MAX_COLS];
	allocTF();
	tfcols = ltfcols;	/* pt tfcols to the local array */
	copyBytes(&edtfps, ps, sizeof(POS));
	stfps = ps;
	edtfsfd = sfd;
	fld = (FIELD *) ps->ps_feat;
	tf = fld->fld_var.fltblfld;
	hdr = &tf->tfhdr;
	tfname = hdr->fhdname;	/* was &hdr->fhdname but & was ignored - nml*/
	/*
	**  Lower case the table field name.
	*/
	CVlower(tfname);
	tfrows = tf->tfrows;
	columns = tf->tfcols;
	curtrack = (bool)( (hdr->fhdflags & fdROWHLIT) != 0 );
	invisible = (bool)( (hdr->fhdflags & fdINVISIBLE) != 0 );
	if ((edtffld = (FIELD *)FEreqmem((u_i4)0, (u_i4)sizeof(FIELD), TRUE,
	    (STATUS *)NULL)) == NULL)
	{
		IIUGbmaBadMemoryAllocation(ERx("tfedit"));
	}
	MEcopy((PTR) fld, (u_i2) sizeof(FIELD), (PTR) edtffld);
	if ((edtffld->fld_var.fltblfld = (TBLFLD *)FEreqmem((u_i4)0,
	    (u_i4)sizeof(TBLFLD), TRUE, (STATUS *)NULL)) == NULL)
	{
		IIUGbmaBadMemoryAllocation(ERx("tfedit"));
	}
	stf = edtffld->fld_var.fltblfld;
	shdr = &stf->tfhdr;
	MEcopy((PTR) tf, (u_i2) sizeof(TBLFLD), (PTR) stf);
	if (!FDwtblalloc(stf))
	{
		IIUGbmaBadMemoryAllocation(ERx("tfedit"));
	}
	if (hdr->fhd2flags & fdVQLOCK)
	{
/* # line 1417 "tfcreate.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,_Form,(char *)0, (char *)0) != 0) {
      IIstfsio(53,_NameFld,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
    } /* IIiqset */
  }
/* # line 1418 "tfcreate.qsc" */	/* host code */
	}
	else
	{
/* # line 1421 "tfcreate.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,_Form,(char *)0, (char *)0) != 0) {
      IIstfsio(53,_NameFld,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 1422 "tfcreate.qsc" */	/* host code */
	}
	showlines = (bool)( (hdr->fhdflags & fdNOTFLINES) == 0 );
	disptitle = (bool)( (hdr->fhdflags & fdNOTFTITLE) == 0 );
	/*
	** Set topdisp, xbot, and xcolsep whether or not in RTI special
	** mode.  Just won't use the values later if a normal user.
	*/
	topdisp = (bool)( (hdr->fhdflags & fdNOTFTOP) == 0 );
	xbot = (bool)( (hdr->fhdflags & fdTFXBOT) != 0 );
	xcolsep = (bool)( (hdr->fhdflags & fdNOCOLSEP) == 0 );
	for (i = 0; i < columns; i++)
	{
		if ((stf->tfflds[i] = (FLDCOL *)FEreqmem((u_i4)0,
		    (u_i4)sizeof(FLDCOL), TRUE, (STATUS *)NULL)) == NULL)
		{
			IIUGbmaBadMemoryAllocation(ERx("tfedit"));
		}
		shdr = &stf->tfflds[i]->flhdr;
		stype = &stf->tfflds[i]->fltype;
		col = tf->tfflds[i];
		hdr = &col->flhdr;
		type = &col->fltype;
		MEcopy((PTR) hdr, (u_i2) sizeof(FLDHDR), (PTR) shdr);
		MEcopy((PTR) type, (u_i2) sizeof(FLDTYPE), (PTR) stype);
		shdr->fhtitle = saveStr(hdr->fhtitle);
		stype->ftvalstr = saveStr(type->ftvalstr);
		stype->ftvalmsg = saveStr(type->ftvalmsg);
		stype->ftdefault = saveStr(type->ftdefault);
		stype->ftfmt = vfchkFmt(type->ftfmtstr);
		stype->ftfmtstr = saveStr(type->ftfmtstr);
		tfcols[i].title = hdr->fhtitle;
		tfcols[i].format = type->ftfmtstr;
		tfcols[i].name = hdr->fhdname;
		tfcols[i].col = col;
	}
	tfcounter = i;
/* # line 1458 "tfcreate.qsc" */	/* inittable */
  {
    if (IItbinit(_Form,_TableFld,(char *)"f") != 0) {
      IIthidecol((char *)"clptr",(char *)"i4");
      IIthidecol((char *)"flags",(char *)"i4");
      IItfill();
    } /* IItbinit */
  }
/* # line 1459 "tfcreate.qsc" */	/* host code */
	tfhandler(tfname, tfrows, showlines, curtrack, disptitle, invisible,
		topdisp, xbot, xcolsep);
}
/*
** tblfldcreate:
**	Routine to set up form to create new
**	table fields.  Calls tfhandler to do all the work.
*/
i4
tblfldcreate()
{
	struct	vftf	ltfcols[DB_GW2_MAX_COLS];
	allocTF();
	tfcols = ltfcols;	/* pt tfcols to the local array */
/* # line 1477 "tfcreate.qsc" */	/* set_frs */
  {
    if (IIiqset(2,0,_Form,(char *)0, (char *)0) != 0) {
      IIstfsio(53,_NameFld,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 1478 "tfcreate.qsc" */	/* inittable */
  {
    if (IItbinit(_Form,_TableFld,(char *)"f") != 0) {
      IIthidecol((char *)"clptr",(char *)"i4");
      IIthidecol((char *)"flags",(char *)"i4");
      IItfill();
    } /* IItbinit */
  }
/* # line 1479 "tfcreate.qsc" */	/* host code */
	tfhandler((char *)NULL, 0, (bool)FALSE, (bool)FALSE,
	    (bool)TRUE, (bool)FALSE, (bool)TRUE, (bool)FALSE, (bool)TRUE);
}
static VOID
vffmttot(fmtstr, dbvptr)
char		*fmtstr;
DB_DATA_VALUE	*dbvptr;
{
	FMT	*tfmt;
	ADF_CB	*ladfcb;
	PTR	blk;
	i4	fmtsize;
	i4	fmtlen;
	dbvptr->db_datatype = DB_NODT;
	dbvptr->db_length = 0;
	dbvptr->db_prec = 0;
	if (*fmtstr == EOS)
	{
		return;
	}
	ladfcb = FEadfcb();
	if (fmt_areasize(ladfcb, fmtstr, &fmtsize) != OK)
	{
		return;
	}
	if ((blk = (PTR) MEreqmem((u_i4)0, (u_i4)fmtsize, TRUE,
	    (STATUS *)NULL)) == NULL)
	{
		return;
	}
	if (fmt_setfmt(ladfcb, fmtstr, blk, &tfmt, &fmtlen) != OK)
	{
		MEfree(blk);
		return;
	}
	else
	{
		/* 
		** Check to see that the length of fmtstr is the same as
		** fmtlen.  If it isn't, there is an invalid format string.
		*/
		if (STlength(fmtstr) != fmtlen)
		{
			IIUGerr(E_VF006B_Invalid_format_specif, UG_ERR_ERROR,0);
			MEfree(blk);
			return;
		}
	}
	if (fmt_ftot(ladfcb, tfmt, dbvptr) != OK)
	{
		dbvptr->db_datatype = DB_NODT;
		dbvptr->db_length = 0;
		dbvptr->db_prec = 0;
	}
	MEfree(blk);
}
static VOID
tfhandler ( tfname, numrows, displines, curtrack, disptitle, invisible,
						    topdisp, xbot, xcolsep )
  char *tfname;
  i4 numrows;
bool	displines;
bool	curtrack;
bool	disptitle;
bool	invisible;
bool	topdisp;
bool	xbot;
bool	xcolsep;
  {
	bool		attrchanged = FALSE;
    i4 istable;
    i4 clflg;
    SIZE_TYPE ccol;
    char cltitle[100];
    char clformat[100];
    char clname[FE_MAXNAME + 1];
    char tfldname[FE_MAXNAME + 1];
    i4 cmd;
	char		buf[80];
/* return of _LASTROW and _MAXROW may not be necessary */
	vfdmpoff();
/* # line 1567 "tfcreate.qsc" */	/* clear */
    {
      IIclrscr();
    }
/* # line 1568 "tfcreate.qsc" */	/* display */
    {
      if (IIdispfrm(_Form,(char *)"u") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 1571 "tfcreate.qsc" */	/* clear */
        {
          IIclrflds();
        }
/* # line 1572 "tfcreate.qsc" */	/* host code */
		if ( iivfRTIspecial )
		{
/* # line 1574 "tfcreate.qsc" */	/* set_frs */
        {
          if (IIiqset(2,0,_Form,(char *)0, (char *)0) != 0) {
            IIstfsio(54,(char *)"disptop",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
            IIstfsio(54,(char *)"dispbotx",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
            IIstfsio(54,(char *)"colsep",0,(short *)0,1,30,sizeof(0),
            (void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 1577 "tfcreate.qsc" */	/* host code */
		}
		else
		{
/* # line 1580 "tfcreate.qsc" */	/* set_frs */
        {
          if (IIiqset(2,0,_Form,(char *)0, (char *)0) != 0) {
            IIstfsio(54,(char *)"disptop",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(54,(char *)"dispbotx",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
            IIstfsio(54,(char *)"colsep",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 1583 "tfcreate.qsc" */	/* host code */
		}
		if (tfname != NULL)
		{ /* Edit existing table field ... */
/* # line 1586 "tfcreate.qsc" */	/* putform */
        {
          if (IIfsetio((char *)0) != 0) {
            IIputfldio(_NameFld,(short *)0,1,32,0,tfname);
            IIputfldio((char *)"rows",(short *)0,1,30,4,&numrows);
            IIputfldio((char *)"coltitle",(short *)0,1,32,0,
            ERget(disptitle ? F_UG0001_Yes1 : F_UG0006_No1));
            IIputfldio((char *)"lines",(short *)0,1,32,0,
            ERget(displines ? F_UG0001_Yes1 : F_UG0006_No1));
            IIputfldio((char *)"trackcursor",(short *)0,1,32,0,
            ERget(curtrack ? F_UG0001_Yes1 : F_UG0006_No1));
            IIputfldio((char *)"invisfld",(short *)0,1,32,0,
            ERget(invisible ? F_UG0001_Yes1 : F_UG0006_No1));
            IIputfldio((char *)"disptop",(short *)0,1,32,0,
            ERget(topdisp ? F_UG0001_Yes1 : F_UG0006_No1));
            IIputfldio((char *)"dispbotx",(short *)0,1,32,0,
            ERget(xbot ? F_UG0001_Yes1 : F_UG0006_No1));
            IIputfldio((char *)"colsep",(short *)0,1,32,0,
            ERget(xcolsep ? F_UG0001_Yes1 : F_UG0006_No1));
          } /* IIfsetio */
        }
/* # line 1602 "tfcreate.qsc" */	/* host code */
			loadtable(EDITING);
/* # line 1603 "tfcreate.qsc" */	/* resume */
        {
          IIresfld(_TableFld);
          if (1) goto IIfdB1;
        }
/* # line 1604 "tfcreate.qsc" */	/* host code */
		}
		else
		{ /* Create new table field ... */
/* # line 1607 "tfcreate.qsc" */	/* putform */
        {
          if (IIfsetio((char *)0) != 0) {
            IIputfldio((char *)"rows",(short *)0,1,30,sizeof(4),
            (void *)IILQint(4));
            IIputfldio((char *)"lines",(short *)0,1,32,0,ERget(F_UG0001_Yes1));
            IIputfldio((char *)"coltitle",(short *)0,1,32,0,
            ERget(F_UG0001_Yes1));
            IIputfldio((char *)"trackcursor",(short *)0,1,32,0,
            ERget(F_UG0006_No1));
            IIputfldio((char *)"invisfld",(short *)0,1,32,0,
            ERget(F_UG0006_No1));
            IIputfldio((char *)"disptop",(short *)0,1,32,0,
            ERget(F_UG0001_Yes1));
            IIputfldio((char *)"dispbotx",(short *)0,1,32,0,
            ERget(F_UG0006_No1));
            IIputfldio((char *)"colsep",(short *)0,1,32,0,ERget(F_UG0001_Yes1));
          } /* IIfsetio */
        }
/* # line 1615 "tfcreate.qsc" */	/* host code */
		}
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 1619 "tfcreate.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_Form,_TableFld,-2) != 0) {
                IItcogetio((short *)0,1,30,4,&clflg,(char *)"flags");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 1620 "tfcreate.qsc" */	/* host code */
		if (clflg & fdVQLOCK)
		{
/* # line 1622 "tfcreate.qsc" */	/* inquire_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&cmd,38,(char *)0,0);
              } /* IIiqset */
            }
/* # line 1623 "tfcreate.qsc" */	/* host code */
			/*
			** If moving into a restricted location (internal
			** name column on a VQ locked row) then move to
			** an unrestricted column on that row.  If moving
			** forward, go to the format column.  If moving
			** up, down, backwards, ... go to title column.
			*/
			if (cmd == CMD_NEXT || cmd == CMD_RET
			    || cmd == CMD_NXITEM)
			{
/* # line 1633 "tfcreate.qsc" */	/* resume */
            {
              IIrescol(_TableFld,(char *)"format");
              if (1) goto IIfdB1;
            }
/* # line 1634 "tfcreate.qsc" */	/* host code */
			}
			else
			{
/* # line 1637 "tfcreate.qsc" */	/* resume */
            {
              IIrescol(_TableFld,(char *)"title");
              if (1) goto IIfdB1;
            }
/* # line 1638 "tfcreate.qsc" */	/* host code */
			}
		}
          }
        } else if (IIretval() == 2) {
          {
            i4 currow;
            char *newname;
/* # line 1646 "tfcreate.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,_Form,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&istable,28,(char *)0,0);
              } /* IIiqset */
            }
/* # line 1647 "tfcreate.qsc" */	/* host code */
		if (!istable)
		{
			_VOID_ STlcopy(ERget(S_VF00E1_InsertAction), buf,
					sizeof(buf) - 1);
			IIUGerr(E_VF00E0_TblFldMenu, UG_ERR_ERROR, 2,
				ERget(FE_Insert), buf);
		}
		else
		{
/* # line 1656 "tfcreate.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,_Form,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&currow,29,_TableFld,0);
              } /* IIiqset */
            }
/* # line 1657 "tfcreate.qsc" */	/* host code */
			newname = vfdflname();
			STcopy(newname, clname);
			CVlower(clname);
			--currow;
/* # line 1661 "tfcreate.qsc" */	/* insertrow */
            {
              if (IItbsetio(5,_Form,_TableFld,currow) != 0) {
                if (IItinsert() != 0) {
                  IItcoputio((char *)"title",(short *)0,1,32,0,newname);
                  IItcoputio((char *)"format",(short *)0,1,32,0,ERx("c1"));
                  IItcoputio((char *)"internname",(short *)0,1,32,0,clname);
                  IITBceColEnd();
                } /* IItinsert */
              } /* IItbsetio */
            }
/* # line 1664 "tfcreate.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,_Form,0,(short *)0,1,30,sizeof(1),(void *)IILQint(
                1));
              } /* IIiqset */
            }
/* # line 1665 "tfcreate.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 3) {
          {
/* # line 1669 "tfcreate.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,_Form,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&istable,28,(char *)0,0);
              } /* IIiqset */
            }
/* # line 1670 "tfcreate.qsc" */	/* host code */
		if (!istable)
		{
			_VOID_ STlcopy(ERget(S_VF00E2_DeleteAction), buf,
					sizeof(buf) - 1);
			IIUGerr(E_VF00E0_TblFldMenu, UG_ERR_ERROR, 2,
				ERget(FE_Delete), buf);
		}
		else
		{
/* # line 1679 "tfcreate.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_Form,_TableFld,-2) != 0) {
                IItcogetio((short *)0,1,30,4,&clflg,(char *)"flags");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 1680 "tfcreate.qsc" */	/* host code */
			if (clflg & fdVQLOCK)
			{
			    IIUGerr(E_VF0123_field_restricted, UG_ERR_ERROR, 0);
			}
			else
			{
/* # line 1686 "tfcreate.qsc" */	/* deleterow */
            {
              if (IItbsetio(4,_Form,_TableFld,-2) != 0) {
                if (IItdelrow(0) != 0) {
                } /* IItdelrow */
              } /* IItbsetio */
            }
/* # line 1687 "tfcreate.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,_Form,0,(short *)0,1,30,sizeof(1),(void *)IILQint(
                1));
              } /* IIiqset */
            }
/* # line 1688 "tfcreate.qsc" */	/* host code */
			}
		}
          }
        } else if (IIretval() == 4) {
          {
/* # line 1693 "tfcreate.qsc" */	/* host code */
	    ADF_CB	*ladfcb;
            i4 currec;
            i4 rec;
            i4 astate;
            i4 rowtype;
            i4 recptr;
/* # line 1701 "tfcreate.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,_Form,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&istable,28,(char *)0,0);
              } /* IIiqset */
            }
/* # line 1702 "tfcreate.qsc" */	/* host code */
	    if ( !istable )
	    {
		_VOID_ STlcopy(ERget(S_VF00E3_EditAction), cltitle,
				sizeof(cltitle) - 1);
		IIUGerr(E_VF00E0_TblFldMenu, UG_ERR_ERROR, 2,
			ERget(F_VF0030_EditAttr), cltitle);
/* # line 1708 "tfcreate.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1709 "tfcreate.qsc" */	/* host code */
	    }
/* # line 1710 "tfcreate.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_Form,_TableFld,-2) != 0) {
                IItcogetio((short *)0,1,30,4,&astate,(char *)"_STATE");
                IItcogetio((short *)0,1,30,4,&rec,(char *)"_RECORD");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 1711 "tfcreate.qsc" */	/* host code */
	    if ( astate == stUNDEF || astate == stDELETE )
	    { /* should not happen! */
		_VOID_ STlcopy(ERget(S_VF00E3_EditAction), cltitle,
				sizeof(cltitle) - 1);
		IIUGerr(E_VF00E0_TblFldMenu, UG_ERR_ERROR, 2,
			ERget(F_VF0030_EditAttr), cltitle);
/* # line 1717 "tfcreate.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1718 "tfcreate.qsc" */	/* host code */
	    }
	    ladfcb = FEadfcb();
	    vfstdel(ATBLFD);
	    tfcounter = 0;
	    /* need to eliminate UNDEFINED or DELETED row during unload */
/* # line 1723 "tfcreate.qsc" */	/* unloadtable */
            {
              if (IItbact(_Form,_TableFld,0) == 0) goto IItbE2;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,32,99,cltitle,(char *)"title");
                IItcogetio((short *)0,1,32,99,clformat,(char *)"format");
                IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,clname,(char *)"\
internname");
                IItcogetio((short *)0,1,30,4,&ccol,(char *)"clptr");
                IItcogetio((short *)0,1,30,4,&rowtype,(char *)"_STATE");
                IItcogetio((short *)0,1,30,4,&currec,(char *)"_RECORD");
                IITBceColEnd();
                {
/* # line 1727 "tfcreate.qsc" */	/* host code */
		FLDCOL		*acol;
		FLDTYPE		*type;
		DB_DATA_VALUE	dbv;
		switch (rowtype)
		{
		    case stNEW:
		    case stCHANGE:
		    case stUNCHANGED:
			if (tfcounter >= DB_GW2_MAX_COLS)
			{
				i4	tmp;
				tmp = DB_GW2_MAX_COLS;
				IIUGerr(E_VF00E5_TooManyCols, UG_ERR_ERROR,
					1, &tmp);
/* # line 1743 "tfcreate.qsc" */	/* resume */
                  {
                    IItunend();
                    if (1) goto IIfdB1;
                  }
/* # line 1744 "tfcreate.qsc" */	/* host code */
			}
			acol = (FLDCOL *) ccol;
			if (acol == NULL)
			{
			    if (clname[0] == EOS)
			    {
				vfxname(clname, cltitle);
				STcopy(cltitle[0] == EOS ? vfdflname()
							: cltitle,
					clname);
				CVlower(clname);
			    }
			    acol = FDnewcol(clname, tfcounter);
			    type = &acol->fltype;
			    type->ftvalstr = _Empty;
			    type->ftvalmsg = _Empty;
			    type->ftdefault = _Empty;
			    type->ftdatatype = DB_NODT;
			    /*
			    **  Create datatype from format only if a good
			    **  format is found.  Not using 'vfchkFmt()'
			    **  since it uses ''FEreqmem()'.
			    */
			    if (*clformat != EOS)
			    {
				vffmttot(clformat, &dbv);
				type->ftdatatype = dbv.db_datatype;
				type->ftlength = dbv.db_length;
				type->ftprec = dbv.db_prec;
			    }
			}
			else
			{
			    FLDHDR	*hdr;
			    hdr = &acol->flhdr;
			    if (clname[0] == EOS)
			    {
				vfxname(clname, cltitle);
				STcopy(cltitle[0] == EOS ? vfdflname()
								: cltitle,
					clname);
				CVlower(clname);
			    }
			    if ( !STequal(clname, hdr->fhdname) )
			    {
				STcopy(clname, hdr->fhdname);
			    }
			    type = &acol->fltype;
			    if (type->ftdatatype == DB_NODT)
			    {
				vffmttot(clformat, &dbv);
				type->ftdatatype = dbv.db_datatype;
				type->ftlength = dbv.db_length;
				type->ftprec = dbv.db_prec;
			    }
			}
			if (type->ftfmt)
			{
			    char    fmtbuf[100];
			    /*
			    **  A FMT struct exists.  Check to
			    **  see if things have changed.
			    */
			    _VOID_ fmt_fmtstr(ladfcb, type->ftfmt, fmtbuf);
			    if ( !STequal(fmtbuf, clformat) )
			    {
				type->ftfmt = vfchkFmt(clformat);
			    }
			}
			else
			{
			    /*
			    **  Need a new FMT struct.
			    */
			    type->ftfmt = vfchkFmt(clformat);
			}
			if (currec == rec)
			{
			    recptr = tfcounter;
			    rec = recptr + 1;
			}
			tfcols[tfcounter].col = acol;
			tfcols[tfcounter].title = saveStr(cltitle);
			tfcols[tfcounter].format = saveStr(clformat);
			tfcols[tfcounter].name = saveStr(clname);
			tfcounter++;
			if (vfnmunique(clname, TRUE, ATBLFD) == FOUND)
			{
				IIUGerr(E_VF00D8_DupColName, UG_ERR_ERROR,
					1, clname);
			}
			break;
		    default:
			break;
		} /* end switch */
                }
              } /* IItunload */
IItbE2:
              IItunend();
            }
/* # line 1845 "tfcreate.qsc" */	/* host code */
	    attrchanged = TRUE;
	    tfattr(tfcols[recptr].col, FALSE, rec);
/* # line 1847 "tfcreate.qsc" */	/* inittable */
            {
              if (IItbinit(_Form,_TableFld,(char *)"f") != 0) {
                IIthidecol((char *)"clptr",(char *)"i4");
                IIthidecol((char *)"flags",(char *)"i4");
                IItfill();
              } /* IItbinit */
            }
/* # line 1848 "tfcreate.qsc" */	/* host code */
	    loadtable(CHGATTR);
          }
        } else if (IIretval() == 5) {
          {
            i4 moverec;
            i4 currow;
/* # line 1855 "tfcreate.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,_Form,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&istable,28,(char *)0,0);
              } /* IIiqset */
            }
/* # line 1856 "tfcreate.qsc" */	/* host code */
		if (istable)
            {
              i4 state;
/* # line 1860 "tfcreate.qsc" */	/* getrow */
              {
                if (IItbsetio(2,_Form,_TableFld,-2) != 0) {
                  IItcogetio((short *)0,1,30,4,&state,(char *)"_STATE");
                  IITBceColEnd();
                } /* IItbsetio */
              }
/* # line 1861 "tfcreate.qsc" */	/* host code */
			if (state == stUNDEF || state == stDELETE)
			{
				IIUGmsg(ERget(S_VF00E7_EmptyColMove), TRUE, 0);
/* # line 1864 "tfcreate.qsc" */	/* resume */
              {
                if (1) goto IIfdB1;
              }
            }
/* # line 1866 "tfcreate.qsc" */	/* host code */
			else
			{
/* # line 1868 "tfcreate.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,_Form,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&currow,29,_TableFld,0);
              } /* IIiqset */
            }
/* # line 1870 "tfcreate.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_Form,_TableFld,currow) != 0) {
                IItcogetio((short *)0,1,30,4,&moverec,(char *)"_RECORD");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 1872 "tfcreate.qsc" */	/* host code */
			}
		}
		else
		{
			_VOID_ STlcopy(ERget(S_VF00E4_MoveAction), buf,
				sizeof(buf) - 1);
			IIUGerr(E_VF00E0_TblFldMenu, UG_ERR_ERROR, 2,
				ERget(FE_Move), buf);
/* # line 1880 "tfcreate.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1881 "tfcreate.qsc" */	/* host code */
		}
/* # line 1883 "tfcreate.qsc" */	/* display */
            {
              if (IInestmu() == 0) goto IIfdE2;
              goto IImuI2;
IIfdI2:;
IIfdB2:
              while (IIrunnest() != 0) {
                if (IIretval() == 1) {
                  {
/* # line 1886 "tfcreate.qsc" */	/* inquire_frs */
                    {
                      if (IIiqset(2,0,_Form,(char *)0, (char *)0) != 0) {
                        IIiqfsio((short *)0,1,30,4,&istable,28,(char *)0,0);
                      } /* IIiqset */
                    }
/* # line 1887 "tfcreate.qsc" */	/* host code */
			if (!istable)
			{
				IIUGmsg(ERget(S_VF00E9_BadMoveCol), TRUE, 0);
/* # line 1890 "tfcreate.qsc" */	/* resume */
                    {
                      if (1) goto IIfdB2;
                    }
/* # line 1891 "tfcreate.qsc" */	/* host code */
			}
			else
                    {
                      i4 destrec;
/* # line 1896 "tfcreate.qsc" */	/* inquire_frs */
                      {
                        if (IIiqset(3,0,_Form,(char *)0, (char *)0) != 0) {
                          IIiqfsio((short *)0,1,30,4,&currow,29,_TableFld,0);
                        } /* IIiqset */
                      }
/* # line 1898 "tfcreate.qsc" */	/* getrow */
                      {
                        if (IItbsetio(2,_Form,_TableFld,currow) != 0) {
                          IItcogetio((short *)0,1,30,4,&destrec,(char *)
"_RECORD");
                          IITBceColEnd();
                        } /* IItbsetio */
                      }
/* # line 1900 "tfcreate.qsc" */	/* host code */
				if (destrec == moverec)
				{
/* # line 1902 "tfcreate.qsc" */	/* breakdisplay */
                      {
                        if (1) goto IIfdE2;
                      }
/* # line 1903 "tfcreate.qsc" */	/* host code */
				}
/* # line 1904 "tfcreate.qsc" */	/* scroll */
                      {
                        if (IItbsetio(1,_Form,_TableFld,-3) != 0) {
                          IItbsmode((char *)"to");
                          if (IItscroll(0,moverec) != 0) {
                          } /* IItscroll */
                        } /* IItbsetio */
                      }
/* # line 1905 "tfcreate.qsc" */	/* getrow */
                      {
                        if (IItbsetio(2,_Form,_TableFld,-2) != 0) {
                          IItcogetio((short *)0,1,32,99,cltitle,(char *)
"title");
                          IItcogetio((short *)0,1,32,99,clformat,(char *)
"format");
                          IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,clname,
                          (char *)"internname");
                          IItcogetio((short *)0,1,30,4,&ccol,(char *)
"clptr");
                          IItcogetio((short *)0,1,30,4,&clflg,(char *)
"flags");
                          IITBceColEnd();
                        } /* IItbsetio */
                      }
/* # line 1911 "tfcreate.qsc" */	/* deleterow */
                      {
                        if (IItbsetio(4,_Form,_TableFld,-2) != 0) {
                          if (IItdelrow(0) != 0) {
                          } /* IItdelrow */
                        } /* IItbsetio */
                      }
/* # line 1912 "tfcreate.qsc" */	/* host code */
				if (moverec < destrec)
				{
					destrec--;
				}
/* # line 1916 "tfcreate.qsc" */	/* scroll */
                      {
                        if (IItbsetio(1,_Form,_TableFld,-3) != 0) {
                          IItbsmode((char *)"to");
                          if (IItscroll(0,destrec) != 0) {
                          } /* IItscroll */
                        } /* IItbsetio */
                      }
/* # line 1917 "tfcreate.qsc" */	/* inquire_frs */
                      {
                        if (IIiqset(3,0,_Form,(char *)0, (char *)0) != 0) {
                          IIiqfsio((short *)0,1,30,4,&currow,29,_TableFld,0);
                        } /* IIiqset */
                      }
/* # line 1919 "tfcreate.qsc" */	/* host code */
				currow--;
/* # line 1920 "tfcreate.qsc" */	/* insertrow */
                      {
                        if (IItbsetio(5,_Form,_TableFld,currow) != 0) {
                          if (IItinsert() != 0) {
                            IItcoputio((char *)"title",(short *)0,1,32,0,
                            cltitle);
                            IItcoputio((char *)"format",(short *)0,1,32,0,
                            clformat);
                            IItcoputio((char *)"internname",(short *)0,1,32,0,
                            clname);
                            IItcoputio((char *)"clptr",(short *)0,1,30,4,&ccol);
                            IItcoputio((char *)"flags",(short *)0,1,30,4,&
                            clflg);
                            IITBceColEnd();
                          } /* IItinsert */
                        } /* IItbsetio */
                      }
/* # line 1926 "tfcreate.qsc" */	/* set_frs */
                      {
                        if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0
                        ) {
                          IIstfsio(20,_Form,0,(short *)0,1,30,sizeof(1),
                          (void *)IILQint(1));
                        } /* IIiqset */
                      }
                    }
/* # line 1928 "tfcreate.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE2;
                    }
                  }
                } else if (IIretval() == 2) {
                  {
/* # line 1934 "tfcreate.qsc" */	/* host code */
			IIUFfieldHelp();
                  }
                } else if (IIretval() == 3) {
                  {
/* # line 1939 "tfcreate.qsc" */	/* host code */
			FEhelp(VFH_TFMOVE, ERget(S_VF00EA_TblFldMoveCol));
                  }
                } else if (IIretval() == 4) {
                  {
/* # line 1944 "tfcreate.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE2;
                    }
                  }
                } else {
                  if (1) goto IIfdE2;
                } /* IIretval */
              } /* IIrunnest */
IIfdF2:
              if (IIchkfrm() == 0) goto IIfdB2;
              goto IIfdE2;
IImuI2:
              if (IIinitmu() == 0) goto IIfdE2;
              if (IInmuact(ERget(FE_Place),(char *)0,2,2,1) == 0) goto IIfdE2;
              if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,2) == 0) 
              goto IIfdE2;
              IIFRInternal(0);
              if (IInfrskact(10,(char *)0,0,0,2) == 0) goto IIfdE2;
              if (IInmuact(ERget(FE_Help),ERget(_Help_Expl),2,2,3) == 0) goto 
              IIfdE2;
              IIFRInternal(0);
              if (IInfrskact(1,(char *)0,2,2,3) == 0) goto IIfdE2;
              if (IInmuact(ERget(FE_End),ERget(_End_Expl),2,2,4) == 0) goto 
              IIfdE2;
              IIFRInternal(0);
              if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE2;
              if (IIendmu() == 0) goto IIfdE2;
              goto IIfdI2;
IIfdE2:;
              IIendnest();
            }
          }
        } else if (IIretval() == 6) {
          {
            char *tblname;
/* # line 1951 "tfcreate.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,tfldname,_NameFld);
              } /* IIfsetio */
            }
/* # line 1952 "tfcreate.qsc" */	/* host code */
		STtrmwhite(tfldname);
		tfcounter = 0;
		if ( (tblname = gettable()) != NULL )
		{
			if ( tfldname[0] == EOS )
			{
/* # line 1959 "tfcreate.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio(_NameFld,(short *)0,1,32,0,tblname);
              } /* IIfsetio */
            }
/* # line 1960 "tfcreate.qsc" */	/* host code */
			}
/* # line 1961 "tfcreate.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,_Form,0,(short *)0,1,30,sizeof(1),(void *)IILQint(
                1));
              } /* IIiqset */
            }
/* # line 1962 "tfcreate.qsc" */	/* resume */
            {
              IIresfld(_TableFld);
              if (1) goto IIfdB1;
            }
/* # line 1963 "tfcreate.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 7) {
          {
/* # line 1969 "tfcreate.qsc" */	/* host code */
		if (tfname != NULL && attrchanged)
		{
/* # line 1971 "tfcreate.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 1972 "tfcreate.qsc" */	/* host code */
			IIUGmsg(ERget(S_VF00EB_EndRequired), TRUE, 0);
/* # line 1973 "tfcreate.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1974 "tfcreate.qsc" */	/* host code */
		}
/* # line 1975 "tfcreate.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 8) {
          {
/* # line 1979 "tfcreate.qsc" */	/* host code */
		/* Implement the Find command. */
            i4 nrows;
/* # line 1982 "tfcreate.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,_Form,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&nrows,31,_TableFld,0);
              } /* IIiqset */
            }
/* # line 1983 "tfcreate.qsc" */	/* host code */
		if (nrows == 0)
		{
			IIUGmsg(ERget(S_VF00EC_NoCols), TRUE, 0);
/* # line 1986 "tfcreate.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1987 "tfcreate.qsc" */	/* host code */
		}
		FEtabfnd( _Form, _TableFld );
          }
        } else if (IIretval() == 9) {
          {
/* # line 1993 "tfcreate.qsc" */	/* host code */
		/* Implement the Top command. */
/* # line 1994 "tfcreate.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_Form,_TableFld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 10) {
          {
/* # line 1998 "tfcreate.qsc" */	/* host code */
		/* Implement the Bottom command. */
/* # line 1999 "tfcreate.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_Form,_TableFld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 11) {
          {
/* # line 2005 "tfcreate.qsc" */	/* host code */
		FEhelp(VFH_TBLFLD, ERget(S_VF00ED_CreateEditTblFld));
          }
        } else if (IIretval() == 12) {
          {
            i4 maxrow;
/* # line 2011 "tfcreate.qsc" */	/* host code */
		i4	stat;
		FIELD	*tffield;
            char showlines[10];
            char titledisp[10];
            char cursortrack[10];
            char invisfield[10];
            char tpdisp[10];
            char xbottom[10];
            char xclsep[10];
/* # line 2021 "tfcreate.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&maxrow,20,_Form,0);
              } /* IIiqset */
            }
/* # line 2022 "tfcreate.qsc" */	/* host code */
		if ( maxrow == 0 && !attrchanged )
/* # line 2023 "tfcreate.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 2025 "tfcreate.qsc" */	/* host code */
		/* Note:  All validations moved to form or to activate on
		** _NameFld above.  Also, all boolean (y/n) display attributes
		** are now forced lowercase so there is no need to do it here.
		*/
/* # line 2029 "tfcreate.qsc" */	/* message */
            {
              IImessage(
              ERget(tfname != NULL ? S_VF00EE_ChangingTblFld
						: S_VF00EF_CreatingTblFld
		));
            }
/* # line 2032 "tfcreate.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,tfldname,_NameFld);
                IIgetfldio((short *)0,1,30,4,&maxrow,(char *)"rows");
                IIgetfldio((short *)0,1,32,9,showlines,(char *)"lines");
                IIgetfldio((short *)0,1,32,9,cursortrack,(char *)
"trackcursor");
                IIgetfldio((short *)0,1,32,9,titledisp,(char *)"coltit\
le");
                IIgetfldio((short *)0,1,32,9,invisfield,(char *)"invis\
fld");
              } /* IIfsetio */
            }
/* # line 2036 "tfcreate.qsc" */	/* host code */
		(VOID) STtrmwhite(tfldname);
		if (!vffnmchk(tfldname, ERget(F_VF0031_TblFld)))
		{
/* # line 2039 "tfcreate.qsc" */	/* resume */
            {
              IIresfld(_NameFld);
              if (1) goto IIfdB1;
            }
/* # line 2040 "tfcreate.qsc" */	/* host code */
		}
		CVlower(tfldname);
		if ( iivfRTIspecial )
		{
/* # line 2045 "tfcreate.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,9,tpdisp,(char *)"disptop");
                IIgetfldio((short *)0,1,32,9,xbottom,(char *)"dispbotx");
                IIgetfldio((short *)0,1,32,9,xclsep,(char *)"colsep");
              } /* IIfsetio */
            }
/* # line 2048 "tfcreate.qsc" */	/* host code */
		}
		if ((tfname != NULL) && !STequal(tfname, tfldname))
		{
			if (vfnmunique(tfldname, TRUE, AFIELD) != FOUND)
			{
				vfdelnm(tfname, AFIELD);
			}
			else
			{
				IIUGerr(E_VF000A_internal_name_exists,
					UG_ERR_ERROR, 1, tfldname);
/* # line 2059 "tfcreate.qsc" */	/* resume */
            {
              IIresfld(_NameFld);
              if (1) goto IIfdB1;
            }
/* # line 2060 "tfcreate.qsc" */	/* host code */
			}
		}
		vfstdel(ATBLFD);
		if ( !unloadtbl() )
		{
			/*
			** If errors were found in the table field
			** specification, then new name of table field
			** must be deleted and the old restored.
			*/
			if ((tfname != NULL) && !STequal(tfname, tfldname))
			{
				vfdelnm(tfldname, AFIELD);
				_VOID_ vfnmunique(tfname, TRUE, AFIELD);
			}
/* # line 2075 "tfcreate.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 2076 "tfcreate.qsc" */	/* host code */
		}
		stat = (tfname != NULL) ? EDITED : VFTFCREATE;
		if ( (tffield = tfbuild(tfldname, maxrow, showlines,
				cursortrack, titledisp, stat,
				invisfield, tpdisp, xbottom, xclsep)) == NULL )
		{
			/*
			** If errors were found in the table field
			** specification, then new name of table field
			** must be deleted and the old restored.
			*/
			if ((tfname != NULL) && !STequal(tfname, tfldname))
			{
				vfdelnm(tfldname, AFIELD);
				_VOID_ vfnmunique(tfname, TRUE, AFIELD);
			}
			IIUGmsg(ERget(S_VF00F0_TooWideTblFld), TRUE, 0);
/* # line 2093 "tfcreate.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 2094 "tfcreate.qsc" */	/* host code */
		}
		if ((tfname == NULL)
			&& (vfnmunique(tfldname, TRUE, AFIELD) == FOUND))
		{
			IIUGerr(E_VF000A_internal_name_exists, UG_ERR_ERROR,
				1, tfldname);
/* # line 2101 "tfcreate.qsc" */	/* resume */
            {
              IIresfld(_NameFld);
              if (1) goto IIfdB1;
            }
/* # line 2102 "tfcreate.qsc" */	/* host code */
		}
		createtbfl(tffield, stat);
/* # line 2105 "tfcreate.qsc" */	/* enddisplay */
            {
              if (1) goto IIfdF1;
            }
          }
        } else {
          if (1) goto IIfdE1;
        } /* IIretval */
      } /* IIrunform */
IIfdF1:
      if (IIchkfrm() == 0) goto IIfdB1;
      goto IIfdE1;
IImuI1:
      if (IITBcaClmAct(_TableFld,(char *)"internname",1,1) == 0) goto IIfdE1;
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Insert),(char *)0,2,2,2) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Delete),(char *)0,2,2,3) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_VF0030_EditAttr),(char *)0,2,2,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Move),(char *)0,2,2,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_GetTableDef),(char *)0,2,0,6) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Forget),(char *)0,0,2,7) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,0,2,7) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,8) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,9) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,10) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(_Help_Expl),0,0,11) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,11) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(_End_Expl),2,2,12) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,12) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 2107 "tfcreate.qsc" */	/* host code */
	FTclear();
  }
/*{
** Name:	IIVFgtmGetTblName - prompt user for table name.
**
** Description:
**	Display a pop-up screen to ask user for name of a table.  A pop-up
**	list of tables is also available.
**
** Inputs:
**
** Outputs:
**	tbls	List of one or more table names, or EOS.
**
**	Returns:
**		none
**
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	08-jun-90 (bruceb)	Swiped from IITUprTablePrompt.
*/
void
IIVFgtmGetTblName(tbls, titlestr, prompttype)
  char *tbls;
  char *titlestr;
i4	prompttype;
{
  char *promptform = ERx("vftableprompt");
  char *p_tbl;
    if (!tnmalloced)
    {
	if ( IIUFgtfGetForm(IIUFlcfLocateForm(), promptform) != OK )
	{
	    syserr(ERget(S_VF0001_Can_not_access_form), promptform);
	}
	tnmalloced = TRUE;
    }
/* # line 2153 "tfcreate.qsc" */	/* display */
  {
    if (IIdispfrm(promptform,(char *)"f") == 0) goto IIfdE3;
    goto IImuI3;
IIfdI3:;
    if (IIfsetio((char *)0) == 0) goto IIfdE3;
    {
/* # line 2156 "tfcreate.qsc" */	/* putform */
      {
        if (IIfsetio((char *)0) != 0) {
          IIputfldio((char *)"title",(short *)0,1,32,0,titlestr);
        } /* IIfsetio */
      }
/* # line 2157 "tfcreate.qsc" */	/* putform */
      {
        if (IIfsetio((char *)0) != 0) {
          IIputfldio((char *)"tablename",(short *)0,1,32,0,tbls);
        } /* IIfsetio */
      }
    }
IIfdB3:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 2162 "tfcreate.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,0,tbls,(char *)"tablename");
            } /* IIfsetio */
          }
/* # line 2164 "tfcreate.qsc" */	/* host code */
	if (tbls[0] == EOS)     /* if it's a null name */
	{
	    	if (prompttype == QN_JDF_PROMPT)
	    		IIUGerr(E_VF0006_no_joindef_name, UG_ERR_ERROR,
			2, (PTR) ERget(FE_OK), (PTR) ERget(FE_Cancel));
	    	else
	    		IIUGerr(E_VF0165_no_table_name, UG_ERR_ERROR,
			2, (PTR) ERget(FE_OK), (PTR) ERget(FE_Cancel));
/* # line 2172 "tfcreate.qsc" */	/* resume */
          {
            if (1) goto IIfdB3;
          }
/* # line 2173 "tfcreate.qsc" */	/* host code */
	}
	/* Validate the existence of the joindef name */
	/* For the case of TF_TBL_PROMPT : validation is done is gettable */
	/*                 QN_TBL_PROMPT : validation is in qfaddrow */
	if (prompttype == QN_JDF_PROMPT)
	{
		OOID    classid = OC_JOINDEF;
		OOID	addid = OC_UNDEFINED;
		if (!vffnmchk(tbls, ERget(FE_JoinDef)))
		{
/* # line 2185 "tfcreate.qsc" */	/* resume */
          {
            if (1) goto IIfdB3;
          }
/* # line 2186 "tfcreate.qsc" */	/* host code */
		}
		if (IIOOidCheck(&classid, &addid, tbls, 
			IIUIdbdata()->user) != OK)
		{
			IIUGerr(E_VF0009_Can_t_retrieve_QBF_Jo, UG_ERR_ERROR, 
				1, tbls);
/* # line 2193 "tfcreate.qsc" */	/* resume */
          {
            if (1) goto IIfdB3;
          }
/* # line 2194 "tfcreate.qsc" */	/* host code */
		}
	}
/* # line 2197 "tfcreate.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE3;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 2203 "tfcreate.qsc" */	/* host code */
	tbls[0] = EOS;
/* # line 2204 "tfcreate.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE3;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 2210 "tfcreate.qsc" */	/* host code */
	if ( prompttype == QN_JDF_PROMPT )
	{
		char	hlpbuf[132+1];
		STcopy(ERget(S_VF0157_JoinDefs), hlpbuf);
        	p_tbl = IIUFopObjPik(hlpbuf, VFH_QJDFLS, OC_JOINDEF,
        		ERget(F_VF0087_JoinDefs), 
			ERget(F_VF0084_Selecting_JDef_names),
			ERget(F_VF0085_JoinDef_Name));
	}
	else
	{
		/* Don't allow table creation, examination. */
		set_IituMainform(FALSE);
		/*
		**  IITUtpTblPik will do an unresolve on the
		**  selected owner.table name before returning
		**  it so we can just put it out for the user
		**  to edit.
		*/
		p_tbl = IITUtpTblPik(NULL, ERx("VIFRED"));
	}
	if ((p_tbl != NULL) && (*p_tbl != EOS))
	{
/* # line 2236 "tfcreate.qsc" */	/* putform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIputfldio((char *)"tablename",(short *)0,1,32,0,p_tbl);
            } /* IIfsetio */
          }
/* # line 2237 "tfcreate.qsc" */	/* host code */
	}
        }
      } else if (IIretval() == 4) {
        {
/* # line 2243 "tfcreate.qsc" */	/* host code */
	if (prompttype == TF_TBL_PROMPT)
	{
		FEhelp(ERx("vftblprm.hlp"), ERget(F_VF0094_Help_Table_Prompt));
	}
	else if (prompttype == QN_TBL_PROMPT)
	{
		FEhelp(VFH_QTBLNM, ERget(S_VF0174_TBL_PROMPT_QN));
	}
	else
	{
		FEhelp(VFH_QJDFNM, ERget(S_VF0175_JDF_PROMPT_QN));
	}
        }
      } else {
        if (1) goto IIfdE3;
      } /* IIretval */
    } /* IIrunform */
IIfdF3:
    if (IIchkfrm() == 0) goto IIfdB3;
    goto IIfdE3;
IImuI3:
    if (IIinitmu() == 0) goto IIfdE3;
    if (IInmuact(ERget(FE_OK),ERget(F_VF0095_ExplOKTable),2,2,1) == 0) goto 
    IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE3;
    if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),0,2,2) == 0) 
    goto IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,0,2,2) == 0) goto IIfdE3;
    if (IInmuact(ERget(FE_Lookup),ERget(F_FE0116_LookupExpl),0,2,3) == 0) 
    goto IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(10,(char *)0,0,2,3) == 0) goto IIfdE3;
    if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,2,4) == 0) 
    goto IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,2,4) == 0) goto IIfdE3;
    if (IIendmu() == 0) goto IIfdE3;
    goto IIfdI3;
IIfdE3:;
    IIendfrm();
  }
/* # line 2256 "tfcreate.qsc" */	/* host code */
}