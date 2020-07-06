# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<cv.h>
# include	<me.h>
# include	<lo.h>
# include	<st.h>
# include	<si.h>
# include	<ut.h>
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	"decls.h"
# include	<ug.h>
# include <ooclass.h>
# include <oocat.h>
# include <uigdata.h>
# include	<oodefine.h>
# include	<oosymbol.h>
# include	<uf.h>
# include	<ui.h>
# include	<stdprmpt.h>
# include	"ervf.h"
# include	"vfhlp.h"
# ifdef		SCROLLBARS
# include	<wn.h>
# endif
/*
** Name:	vfcat.qsc -	ViFred Catalog Frame Module.
**
** Description:
**	Contains routines used to run the catalog frame for ViFred.  Defines:
**
**	fdcats()	catalog frame.
**	vfsvdummy()	dummy routine.
**	vfcallqbf()	call QBF.
**	vfLast()	set last edited form.
**
**	vfoocatrec	catalog record for current row.
**
**  History:
**	02/04/85 (drh)	Added Dave H.'s VMS bug fixes.
**	2/85	(ac)	-Added one argument to UTexe for passing file
**			 descriptors of -I -D files to the spawned process.
**	06/27/85 (gac)	Replaced sleeped messages by FEmsg.
**	10/21/85 (prs)	Add DUMPFORM from printform.
**	01/13/86 (prs)	Change call to FEfehelp.
**	7/9/86 (bruceb)	Fix for bug 9713.  Allow DBA forms to be compiled
**			and QbfNames created by other users.
**	03/23/87 (dkh) -Added support for ADTs.
**	10/20/86 (KY)  -- Changed CH.h to CM.h.
**	11/18/86 (KY)  -- changed STindex,STrindex for Double Bytes characters.
**	05/04/87 (dkh) - Changed for generic catalogs.
**	05/18/87 (dkh) - Updated for OOid*() changes.
**	05/21/87 (dkh) - Integrated VMS changes.
**	05/30/87 (dkh) - Changed transaction processing.
**	06/30/87 (dkh) - Fixed jup bug 392.
**	07/11/87 (dkh) - Fixed jup bug 460.
**	08/10/87 (dkh) - Fixed jup bug 664.
**	08/14/87 (dkh) - ER changes.
**	09/05/87 (dkh) - Changes for 8000 series error numbers.
**	10/02/87 (dkh) - Help file changes.
**	10/17/87 (dkh) - Changed to not use single letter range vars.
**	10/17/87 (dkh) - Code cleanup.
**	10/25/87 (dkh) - Error message cleanup.
**	11/25/87 (dkh) - Fixed jup bug 1411.
**	11/28/87 (dkh) - Added testing support for calling QBF.
**	03-dec-87 (bruceb)
**		Changed to a qsc file; include qsh files.
**	12/19/87 (dkh) - Including .qsh instead of .qh headers.
**	12/21/87 (dkh) - Fixed to correctly find duplicate qbfnames entry.
**	21-dec-87 (sylviap)
**		Changed QUEL statements to SQL
**		Took out all 'ifdef HACKFOR50'
**	02-feb-88 (sylviap)
**		Changed from CVlower to IIUGlbo_lowerBEobject to accommodate
**		   upper case table names (gateway).
**	22-feb-88 (sylviap)
**		Change transaction code handle gateways.
**	03/03/88 (dkh) - Changes to save routines called by OO.
**	29-mar-88 (sylviap)
**		Added commit work before call to OOdetail.
**		Commits work after renaming a form.
**		If DG, disables GO (to qbf) option.
**	11-may-88 (sylviap)
**	 	If DG, turned off GO/COMPILE options.
**		Added SQL COMMIT after OO and FE calls to release locks on
**		  the system catalogs.
**		Took out commit before OOdetail call.
**	17-jun-88 (sylviap)
**		Added in fdcats, IIUGlbo_lowerBEobject if -t flag (tables).
**		On DG, tablenames can be mixed-case.
**	17-jun-88 (bruceb)
**		If DG, redisplay the screen after dumpform().  Needed
**		to restore the screen after DG's CEO document/folder/file
**		questions.
**	13-jul-88 (sylviap)
**		When destroying/renaming a form, moved SQL COMMIT's after
**		OOp call.
**	20-july-88 (sylviap)
**		Deletes the id before the detail information (ii_fields,
**		  ii_trim, ii_encoded_forms). #2941
**		Reloads the forms catalog after renaming a form. #2947
**		Changed error message E_VF001D to state the maximum numbers of
**	 	  characters in a form name. #2957
**	28-jul-88 (marian)
**		Put back si.h for vms.
**	12-aug-88 (bruceb)	Fix for bug 2900
**		Call dumpform with a third argument, a function pointer.
**		The new function, IIVFcsr_ClrScrRedisp, if DG, will clear
**		and redisplay the screen so as to handle DG's CEO interface.
**	31-aug-88
**		Converted ..msg to ..err. for 2 errors
**	10/28/88 (dkh) - Performance changes.
**	08-nov-88 (sylviap)
**		Added NULL to the parameter list call to OOdetail.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**	11/30/88 (tom) - CMS changes.
**	08-dec-88 (sylviap)
**		Took out error message 11A.  If DG, user can compile files and
**		call QBF from VIFRED.
**	01/10/89 (dkh) - Removed reference to "IIooObjects".
**	02/18/89 (dkh) - Yet more performance changes.
**	22-mar-89 (bruceb)
**		Move cursor to proper row of the catalog upon a rename.
**	04/06/89 (dkh) - Fixed 6.2 bug 5337.
**	04/12/89 (dkh) - Changed fix to 6.2 bug 5337 to force table field
**			 in iicatalog to be initialized.
**	04/19/89 (dkh) - Put in generic error changes.
**	05/18/89 (dkh) - Fixed bug 5718.
**	02-jun-89 (bruceb)
**		Free up the buffer allocated when Renaming a form.
**	28-jun-89 (bruceb)
**		Use vfdetail and vfcatalog instead of iidetail and iicatalog.
**	26-oct-89 (bruceb)
**		Use popups for form creation.  Use popups when form isn't
**		found on being called from ABF (-F flag.)
**		Also, remove 'isProcess' code; multi-process FEs never used.
**	27-nov-89 (bruceb)
**		Find/top/bottom no longer visible menuitems.
**		Use standard confirmation prompt mechanism for destroying forms.
**	12/31/89 (dkh) - Integrated IBM porting changes.
**	05-jan-90 (sylviap)
**		Added new parameter to UTexe.
**	21-feb-90 (bruceb)
**		Added End menuitem to the frame.
**      08-mar-90 (sylviap)
**              Changed UTexe call to pass an CL_ERR_DESC.
**	15-mar-90 (griffin)
**		Turned optimization off for hp3_us5.
**	18-apr-90 (bruceb)
**		Lint cleanup; removed second param in call on fdqbfmap.
**	01-jun-90 (bruceb)
**		Call to IIOOidCheck for destroy is called with dba-name
**		instead of NULL.  This results in proper positioning of the
**		cursor in the catalog after deleting a form.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
**	08/31/90 (dkh) - Integrated porting change ingres6202p/131116.
**	10/25/90 (dkh) - Fixed bug 34092.
**	10/29/90 (dkh) - Fixed bug 34202.  Don't free buffer space allocated
**			 for a new name if the rename succeeded.  It appears
**			 that OO holds on to the buffer in the OO_OBJECT struct.
**	03/25/91 (dkh) - Replaced NULL with '\0' as a case label to
**			 avoid problems on SCO.
**	07/25/91 (johnr) - Removed NO_OPTIM hp3_us5 for hp-ux 8.0.
**	26-may-92 (rogerf) DeskTop Porting Change:
**		Added scrollbar processing inside SCROLLBARS ifdef.
**	08-jun-92 (leighb) DeskTop Porting Change:
**		Changed 'errno' to 'errnum' cuz 'errno' is a macro in WIN/NT.
**	09/07/92 (dkh) - Updated with 6.5 call interface to FErelexists.
**	09/18/92 (dkh) - Fixed bug 43696.  Destroying a form will cause
**			 the DBA's copy to be relisted in the catalog.
**	09/20/92 (dkh) - Added support for owner.table.
**	06/19/93 (dkh) - Added additional checks to make sure that user
**			 did not enter a table name that begins or ends
**			 with a period that is not part of a delimited id.
**      18-oct-1993 (kchin)
**          	Since the return type of OOsnd() has been changed to PTR,
**          	when being called, its return type needs to be cast to the 
**		proper datatype.
**	10/28/93 (donc)  Cast calls to OOsnd as returning an OOID. oodefine.h
**			 now has OOsnd returning a PTR.
**	20-jan-1994 (rdrane)
**		Don't case or otherwise mess with any tablename - let
**		vftblnmchk() do the work.  This was overlooked during the
**		initial 6.5 modifications.  Bug 58272.
**      08/12/93 (smc)
**		Bug #58882
**		Commented lint pointer truncation warning.
**      25-sep-96 (mcgem01)
**              Global data moved to vfdata.c
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      09-May-2003 (bonro01)
**              Added prototype for OOdetail() to prevent core dump.
**	21-jan-03 (hayke02)
**	    Remove local prototyping of IIUFccConfirmChoice() - this is now
**	    done in fe.h.
*/
OO_OBJECT	*OOp();
VOID	IIVFcsr_ClrScrRedisp();
GLOBALREF bool	vfuseiicomp;
GLOBALREF FILE	*vfdfile;
FUNC_EXTERN	i4	IIVFcfCreateForm();
FUNC_EXTERN	i4	IIFRrlpRunListPick();
FUNC_EXTERN	i4	(*IIseterr())();
static	i4	getQbfNames();
FUNC_EXTERN	char	*IIxflag();
FUNC_EXTERN	STATUS	vftblnmchk();
FUNC_EXTERN	VOID	vfcallqbf();
FUNC_EXTERN	VOID	vfLast();
FUNC_EXTERN	VOID	vfexe();
FUNC_EXTERN	bool	OOdetail ( OOID class, char *catform, char *cattabl,
			OO_CATREC *row, char *detfrm, char *title, PTR data,
			STATUS (*display)(), STATUS (*reinit)(),
			STATUS (*rename)(), STATUS (*destroy)(),
			STATUS (*save)(), MENUPARAM *menup,
			char *help_scr);
# define    START_TRANSACTION	    FALSE
# define    COMMIT_TRANSACTION	    TRUE
# define    ROLLBACK_ENTIRE_XACT    TRUE
# define    ALL_BACKENDS	    FALSE
/*
**  Fix for jup bug 392. (dkh)
*/
static char	Lastname[FE_MAXNAME + 1]  ZERO_FILL ;
static bool	Lastdifferent = FALSE;
static bool	Lastsaved     = FALSE;
static bool	det_alloced = FALSE;
static bool	cat_alloced = FALSE;
static bool	cre_alloced = FALSE;
static bool	abcre_alloced = FALSE;
/* Form names. */
  static char *_vfcatalog = ERx("vfcatalog");
static char	*_vfdetail = ERx("vfdetail");
  static char *vfcrform = ERx("vfcrform_menu");
  static char *vfabcrform = ERx("vfabcrform");
  static char *form_type = ERx("form_type");
GLOBALREF OO_CATREC	vfoocatrec;
# ifdef VMS
# define	MACRO	ERx("-m")
# else
# define	MACRO	ERx("")
# endif /* VMS */
STATUS
vfsvdummy(id, aptr)
OOID	id;
PTR	aptr;
{
	return OK;
}
static VOID
detail_alloc()
{
    LOCATION	*fnx;
    if (!det_alloced)
    {
	FDsetparse(FALSE);
	fnx = IIUFlcfLocateForm();
	if ( IIUFgtfGetForm(fnx, _vfdetail) != OK )
	{
	    syserr(ERget(S_VF0001_Can_not_access_form), _vfdetail);
	}
	det_alloced = TRUE;
	FDsetparse(TRUE);
    }
}
static VOID
allocCat()
{
    LOCATION	*fnx;
    if (!cat_alloced)
    {
	/*
	**  Start up Object Manager.
	*/
	IIOOinit((OO_OBJECT **)NULL);
	FDsetparse(FALSE);
	fnx = IIUFlcfLocateForm();
	if ( IIUFgtfGetForm(fnx, _vfcatalog) != OK )
	{
	    syserr(ERget(S_VF0001_Can_not_access_form), _vfcatalog);
	}
	cat_alloced = TRUE;
	FDsetparse(TRUE);
    }
}
static VOID
loadCreateTbl(frm)
  char *frm;
{
/* # line 327 "vfcat.qsc" */	/* inittable */
  {
    if (IItbinit(frm,form_type,(char *)"r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 328 "vfcat.qsc" */	/* loadtable */
  {
    if (IItbact(frm,form_type,1) != 0) {
      IItcoputio((char *)"type",(short *)0,1,32,0,ERget(S_VF0145_Duplicate));
      IItcoputio((char *)"description",(short *)0,1,32,0,
      ERget(S_VF0146_Duplicate_expl));
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 330 "vfcat.qsc" */	/* loadtable */
  {
    if (IItbact(frm,form_type,1) != 0) {
      IItcoputio((char *)"type",(short *)0,1,32,0,ERget(S_VF0147_Blank));
      IItcoputio((char *)"description",(short *)0,1,32,0,
      ERget(S_VF0148_Blank_expl));
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 332 "vfcat.qsc" */	/* loadtable */
  {
    if (IItbact(frm,form_type,1) != 0) {
      IItcoputio((char *)"type",(short *)0,1,32,0,ERget(S_VF0149_Table));
      IItcoputio((char *)"description",(short *)0,1,32,0,
      ERget(S_VF014A_Table_expl));
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 334 "vfcat.qsc" */	/* loadtable */
  {
    if (IItbact(frm,form_type,1) != 0) {
      IItcoputio((char *)"type",(short *)0,1,32,0,ERget(S_VF014B_JoinDef));
      IItcoputio((char *)"description",(short *)0,1,32,0,
      ERget(S_VF014C_JoinDef_expl));
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 336 "vfcat.qsc" */	/* host code */
}
static VOID
allocCreate()
{
    LOCATION	*fnx;
    if (!cre_alloced)
    {
	FDsetparse(FALSE);
	fnx = IIUFlcfLocateForm();
	if (IIUFgtfGetForm(fnx, vfcrform) != OK)
	{
	    syserr(ERget(S_VF0001_Can_not_access_form), vfcrform);
	}
	loadCreateTbl(vfcrform);
	cre_alloced = TRUE;
	FDsetparse(TRUE);
    }
}
static VOID
allocABCreate()
{
    LOCATION	*fnx;
    if (!abcre_alloced)
    {
	FDsetparse(FALSE);
	fnx = IIUFlcfLocateForm();
	if (IIUFgtfGetForm(fnx, vfabcrform) != OK)
	{
	    syserr(ERget(S_VF0001_Can_not_access_form), vfabcrform);
	}
	loadCreateTbl(vfabcrform);
	abcre_alloced = TRUE;
	FDsetparse(TRUE);
    }
}
static	i4
noErrMessages(errnum)
i4	*errnum;
{
	return(0);
}
#define _HelpExpl		F_FE0100_ExplFrameHelp
#define _EndExpl		F_FE0102_ExplEnd
#define _CancelExpl		F_FE0103_ExplCancel
#define _InfoExpl		F_FE0107_ExplMoreInfo
#define _CreateExpl		F_VF0057_CreateExpl
#define _BlnkExpl		F_VF0058_BlnkFormExpl
#define _TblDefExpl		F_VF0059_TblDefExpl
#define _SimpleFldExpl	F_VF005A_SimpleFldExpl
#define _TblFldExpl		F_VF005B_TblFldExpl
#define _JoinDefExpl	F_VF005C_JoinDefExpl
#define _DestroyExpl	F_VF005D_DestroyExpl
#define _EditExpl		F_VF005E_EditExpl
#define _RenameExpl		F_VF005F_RenameExpl
#define _UtilExpl		F_VF0060_UtilExpl
#define _UtilRTIExpl	F_VF0061_UtilRTIExpl
#define _CompileExpl	F_VF0062_CompileExpl
#define _PrintExpl		F_VF0063_PrintExpl
#define _QBFNamesExpl	F_VF0064_QBFNamesExpl
#define _CopyFormExpl	F_VF0065_CopyFormExpl
#define _QBFrunExpl		F_VF0066_QBFrunExpl
#define _QuitVF			F_VF0067_QuitVF
# define DUP_FORM	0
# define BLNK_FORM	1
# define TBL_FORM	2
# define JDEF_FORM	3
VOID
fdcats()
  {
  OOID vfooid;
  UIDBDATA *uidbdata;
	OOID		vfid_frm = OC_FORM;
	ER_MSGID	uexplid = iivfRTIspecial ? _UtilRTIExpl : _UtilExpl;
	OOID		idlist[200];
	i4		cre_choice;	/* Type of form creation. */
	char		hlpbuf[132+1];
    char *frm_name;
	i4		(*oldproc)();
	char		*help_subject;
	vfdmpoff();
	allocCat();
	vfoocatrec.id = UNKNOWN;
	uidbdata = IIUIdbdata();
/* # line 439 "vfcat.qsc" */	/* display */
    {
      if (IIdispfrm(_vfcatalog,(char *)"r") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 442 "vfcat.qsc" */	/* putform */
        {
          if (IIfsetio(_vfcatalog) != 0) {
            IIputfldio((char *)"title",(short *)0,1,32,0,
            ERget(F_VF0012_VIFRED_Forms_Catalo));
          } /* IIfsetio */
        }
/* # line 444 "vfcat.qsc" */	/* host code */
		vfLast(ERx(""));
		if (Vfformname != NULL)
		{
			/*
			**  If -t flag, then tablename can be any case
			** and have an owner specification.  Let
			** vftblnmchk() handle the decomposition and casing.
			*/
			if (vfspec != 't')
			{
				CVlower(Vfformname);
			}
			OOcatEmpty(OC_FORM, _vfcatalog, _cattable);
			switch (vfspec)
			{
			case 'F':
				IIUGmsg(ERget(S_VF0013_Retrieving_forms),
					FALSE, 1, Vfformname);
				/* Turn off any err msgs from vfgetFrame(). */
				oldproc = IIseterr(noErrMessages);
				frame = vfgetFrame(Vfformname);
				_VOID_ IIseterr(oldproc);
				if (frame != NULL)	/* form exists */
				{
				    /* Note:  'IIOOidCheck()' will query
				    ** the object catalogs again to get
				    ** the ID AND the rest of object
				    ** description necessary to determine
				    ** ownership and fill in 'vfoocatrec'.
				    */
				    if (IIOOidCheck(&vfid_frm,
						    &vfoocatrec.id,
						    Vfformname,
						    (char *)NULL) != OK ||
					    OOcatRecord(vfoocatrec.id,
						    &vfoocatrec) != OK  ||
				      !STequal(vfoocatrec.owner,
						uidbdata->user))
				    {
					/* new form from DBA's */
                			/* lint truncation warning if size of 
					** ptr > OOID, but code valid. 
					*/
					vfoocatrec.id =
					  (OOID)OOsnd(vfoocatrec.id, _copy);
					*vfoocatrec.create_date = EOS;
					*vfoocatrec.alter_date = EOS;
					STcopy(uidbdata->user,
						vfoocatrec.owner);
				    }
				    vforigintype = OT_EDIT;
				    STcopy(Vfformname, vforgname);
				    vfcursor(TRUE, TRUE);
				}
				else	/* Create new form. */
				{
				    allocABCreate();
				    /* Display the name of requested form. */
				    frm_name = Vfformname;
/* # line 508 "vfcat.qsc" */	/* putform */
        {
          if (IIfsetio(vfabcrform) != 0) {
            IIputfldio((char *)"form_name",(short *)0,1,32,0,frm_name);
          } /* IIfsetio */
        }
/* # line 510 "vfcat.qsc" */	/* host code */
				    vfoocatrec.id = UNKNOWN;
				    vfoocatrec.name[0] = EOS;
				    vfoocatrec.owner[0] = EOS;
				    vfoocatrec.short_remark[0] = EOS;
				    vfoocatrec.create_date[0] = EOS;
				    vfoocatrec.alter_date[0] = EOS;
				    vfoocatrec.long_remark[0] = EOS;
				    STcopy(ERget(S_VF014D_Form_Creation),
					hlpbuf);
				    cre_choice
					= IIFRrlpRunListPick(vfabcrform,
					form_type, -1, -1, hlpbuf, VFH_CREATE,
					IIVFcfCreateForm, NULL);
				    switch(cre_choice)
				    {
				      case(BLNK_FORM):
					blankcom();
					break;
				      case(DUP_FORM):
				      case(TBL_FORM):
				      case(JDEF_FORM):
					vfcursor(TRUE, TRUE);
					break;
				    }
				    /* 'Cancel' will simply return to caller. */
				}
/* # line 539 "vfcat.qsc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
/* # line 541 "vfcat.qsc" */	/* host code */
			case '\0':
			case 'f':
				IIUGmsg(ERget(S_VF0013_Retrieving_forms),
					FALSE, 1, Vfformname);
				if ((frame = vfgetFrame(Vfformname)) != NULL)
				{ /* form exists */
					/* Note:  'IIOOidCheck()' will query
					** the object catalogs again to get
					** the ID AND the rest of object
					** description necessary to determine
					** ownership and fill in 'vfoocatrec'.
					*/
					if ( IIOOidCheck(&vfid_frm,
							&vfoocatrec.id,
							Vfformname,
							(char *)NULL) != OK ||
						OOcatRecord(vfoocatrec.id,
							&vfoocatrec) != OK  ||
					  !STequal(vfoocatrec.owner,
						   uidbdata->user) )
					{ /* new form from DBA's */
						register OO_OBJECT	*obj;
                                       		/* lint truncation warning if 
						** size of ptr > OOID, but 
						** code valid.
						*/
 						vfoocatrec.id =
						  (OOID)OOsnd(vfoocatrec.id,
							 _copy);
						*vfoocatrec.create_date = EOS;
						*vfoocatrec.alter_date = EOS;
						STcopy(uidbdata->user,
							vfoocatrec.owner);
					}
					vforigintype = OT_EDIT;
					STcopy(Vfformname, vforgname);
					vfLast(Vfformname);
					vfcursor(TRUE, TRUE);
				}
				Vfformname = NULL;
				break;
			case 't':
			{
				LIST	*tbllist = NULL;
				LIST	*ownlist = NULL;
				LIST	*lt = NULL;
				LIST	*olt = NULL;
				i4	i;
				IIUGmsg(ERget(S_VF0014_Creating_default_form),
				      FALSE, 1, Vfformname);
				if (vftblnmchk(Vfformname, &tbllist, &ownlist,
					&i) == OK && tbllist != NULL &&
				    (frame = vfgetRelFrame(tbllist, ownlist,
					i, FALSE)) != NULL && VTnewfrm(frame))
				{
					/*
					**  Free up the LIST structs
					**  before we continue.
					*/
					for ( ; tbllist != NULL; )
					{
						lt = tbllist;
						olt = ownlist;
						tbllist = tbllist->lt_next;
						ownlist = ownlist->lt_next;
						lfree(lt);
						lfree(olt);
					}
					vfcursor(TRUE, TRUE);
				}
				Vfformname = NULL;
				break;
			}
			case 'j':
				if (qdefcom(Vfformname))
				{
					vfcursor(TRUE, TRUE);
				}
				Vfformname = NULL;
				break;
			}
		}
		if (vfnoload)
		{
			OOcatEmpty(OC_FORM, _vfcatalog, _cattable);
		}
		else
		{
			char	*editname;
			editname = ( Lastname[0] != EOS ) ? Lastname : NULL;
			IIUGmsg(ERget(S_VF008B_Retrieving_catalogs),
				(bool) FALSE, 0);
			OOcatTableLoad(OC_FORM, _vfcatalog, _cattable,
				(bool) FALSE, ERx("*"), editname, NULL);
/* # line 641 "vfcat.qsc" */	/* set_frs */
        {
          if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(21,(char *)0,0,(short *)0,1,32,0,(char *)"r");
          } /* IIiqset */
        }
/* # line 642 "vfcat.qsc" */	/* host code */
		}
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 647 "vfcat.qsc" */	/* host code */
		allocCreate();
		vfoocatrec.id = UNKNOWN;
		vfoocatrec.name[0] = EOS;
		vfoocatrec.owner[0] = EOS;
		vfoocatrec.short_remark[0] = EOS;
		vfoocatrec.create_date[0] = EOS;
		vfoocatrec.alter_date[0] = EOS;
		vfoocatrec.long_remark[0] = EOS;
		vfLast(ERx(""));
		STcopy(ERget(S_VF014D_Form_Creation), hlpbuf);
		cre_choice = IIFRrlpRunListPick(vfcrform, form_type,
		    -1, -1, hlpbuf, VFH_CREATE, IIVFcfCreateForm, NULL);
		if (cre_choice >= 0)	/* User didn't "Cancel". */
		{
		    switch(cre_choice)
		    {
		      case(BLNK_FORM):
			blankcom();
			break;
		      case(DUP_FORM):
		      case(TBL_FORM):
		      case(JDEF_FORM):
			vfcursor(TRUE, TRUE);
			break;
		    }
		    if (vfspec == 'F')
		    {
			/* Never display the catalog when called from ABF. */
/* # line 681 "vfcat.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 682 "vfcat.qsc" */	/* host code */
		    }
		    if (vfnoload)
		    {
			    OOcatEmpty(OC_FORM, _vfcatalog, _cattable);
		    }
		    else if ( Lastname[0] != EOS )
		    {
			    OOcatTableLoad(OC_FORM, _vfcatalog, _cattable,
				    (bool) FALSE, ERx("*"), Lastname, NULL);
		    }
		}
          }
        } else if (IIretval() == 2) {
          {
/* # line 696 "vfcat.qsc" */	/* host code */
  register char *cname;
  OOID qid;
		register char	*classname;
		bool	completed;
		i4		tries;
		i4		errnum;
		i4		dlock = FALSE;
		if (OOcatCheck(OC_FORM, _vfcatalog, _cattable,
			&vfoocatrec) != OK)
		{
/* # line 709 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 710 "vfcat.qsc" */	/* host code */
		}
		cname = vfoocatrec.name;
		vfooid = vfoocatrec.id;
		classname = ((OO_CLASS *)OOp(OC_FORM))->name;
		help_subject = STalloc(ERget(S_VF0158_Destroy_Form));
		if (IIUFccConfirmChoice(CONF_DESTROY, cname, classname,
		    help_subject, VFH_DSTFRM) != CONFCH_YES)
		{
/* # line 722 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 723 "vfcat.qsc" */	/* host code */
		}
		IIUGmsg(ERget(S_VF0019_Destroying), FALSE, 2, classname, cname);
		if ( IIOOidDestroy(&vfid_frm, &(vfoocatrec.id),
				vfoocatrec.name, vfoocatrec.owner) != OK )
		{
			IIUGmsg(ERget(S_VF0110_Destroy_error), TRUE,
				2, classname, cname);
/* # line 733 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 734 "vfcat.qsc" */	/* host code */
		}
		iiuicw1_CatWriteOn();
		completed = FALSE;
		for (tries = 0; !completed && tries < MAXDLOCK_TRIES; tries++)
		{
			i4	i;
			IIUIbeginXaction();
			i = 1;	/* first action */
			do
			{
			    switch (i)
			    {
				case 1:
/* # line 756 "vfcat.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_forms where object_id=");
    IIputdomio((short *)0,1,30,4,&vfooid);
    IIsyncup((char *)0,0);
  }
/* # line 757 "vfcat.sc" */	/* host code */
					break;
				case 2:
/* # line 760 "vfcat.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_fields where object_id=");
    IIputdomio((short *)0,1,30,4,&vfooid);
    IIsyncup((char *)0,0);
  }
/* # line 761 "vfcat.sc" */	/* host code */
					break;
				case 3:
/* # line 764 "vfcat.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_trim where object_id=");
    IIputdomio((short *)0,1,30,4,&vfooid);
    IIsyncup((char *)0,0);
  }
/* # line 765 "vfcat.sc" */	/* host code */
					break;
				case 4:
					/*
					** Delete compiled version of form from
					** iicompfrm catalog.
					*/
					if (vfuseiicomp)
					{
/* # line 775 "vfcat.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_encoded_forms where object_id=");
    IIputdomio((short *)0,1,30,4,&vfooid);
    IIsyncup((char *)0,0);
  }
/* # line 776 "vfcat.sc" */	/* host code */
					    break;
				 	}
					else
					{
					    ++i;
					}
					/* fall through ... */
				case 5:
				{
					register i4	j;
					register i4	idindex;
					idindex = getQbfNames(cname, idlist);
					for (j = 0; j < idindex; j++)
					{
					    qid = idlist[j];
/* # line 792 "vfcat.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_qbfnames where object_id=");
    IIputdomio((short *)0,1,30,4,&qid);
    IIsyncup((char *)0,0);
  }
/* # line 794 "vfcat.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_objects where object_id=");
    IIputdomio((short *)0,1,30,4,&qid);
    IIsyncup((char *)0,0);
  }
/* # line 796 "vfcat.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_longremarks where object_id=");
    IIputdomio((short *)0,1,30,4,&qid);
    IIsyncup((char *)0,0);
  }
/* # line 797 "vfcat.sc" */	/* host code */
					}
					break;
				}
				case 6:		/* Done! */
					IIUIendXaction();
					completed = TRUE;
					break;
			    } /* end switch */
			    if ( ! completed )
			    {
				if ((errnum = FEinqerr()) != OK)
				{
					IIUIabortXaction();
					if (IIUIdeadlock(errnum))
					{
						dlock = TRUE;
						break;
					}
					iiuicw0_CatWriteOff();
/* # line 814 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 816 "vfcat.qsc" */	/* host code */
				}
				else
				{
					i++;
				}
			    }
		    } while ( ! completed );
		} /* end for */
		iiuicw0_CatWriteOff();
		if (!completed)
		{
			IIUGerr( dlock ? E_VF001A_Deadlock_has_occurred
					: E_VF001B_Error_occurred_in_del,
				UG_ERR_ERROR, 0
			);
/* # line 833 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 834 "vfcat.qsc" */	/* host code */
		}
		if (vfnoload)
		{
			OOcatEmpty(OC_FORM, _vfcatalog, _cattable);
		}
		else
		{
			bool	found = FALSE;
			if ( !uidbdata->isdba )
			{
				/*
				**  Set the ID to be zero so that
				**  IIOOidCheck won't try to look
				**  for the form that was just destroyed.
				*/
				vfooid = 0;
				if ( IIOOidCheck(&vfid_frm, &vfooid, cname,
							uidbdata->dba) == OK )
				{
					/*
					**  Found DBA's version of object
					**  with same name.
					*/
					found = TRUE;
				}
			}
			if (found)
			{
				OOcatTableLoad(OC_FORM, _vfcatalog, _cattable,
					(bool) FALSE, ERx("*"), cname, NULL);
			}
			else
			{
/* # line 869 "vfcat.qsc" */	/* deleterow */
            {
              if (IItbsetio(4,_vfcatalog,_cattable,-2) != 0) {
                if (IItdelrow(0) != 0) {
                } /* IItdelrow */
              } /* IItbsetio */
            }
/* # line 870 "vfcat.qsc" */	/* host code */
			}
		}
          }
        } else if (IIretval() == 3) {
          {
/* # line 875 "vfcat.qsc" */	/* host code */
		register char	*cname;
		if (OOcatCheck(OC_FORM, _vfcatalog, _cattable,
			&vfoocatrec) != OK)
		{
/* # line 880 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 881 "vfcat.qsc" */	/* host code */
		}
#ifdef SCROLLBARS
        WNSbarDestroy();
#endif /* SCROLLBARS */
                cname = vfoocatrec.name;
		/*
		**  If owner of form is not the user, then must
		**  be owned by DBA, just id to UNKNOWN to indicate
		**  this.
		*/
		if ( !STequal(vfoocatrec.owner, uidbdata->user) )
		{
			vfoocatrec.id = UNKNOWN;
		}
		vfLast(cname);
		Lastdifferent = FALSE;
		Lastsaved = FALSE;
		IIUGmsg(ERget(S_VF0013_Retrieving_forms), FALSE, 1, cname);
		if ((frame = vfgetFrame(cname)) != NULL)
		{ /* form exists */
			vforigintype = OT_EDIT;
			STcopy(cname, vforgname);
			vfcursor(TRUE, TRUE);
			if ( vfspec == 'F' && STequal(Lastname, Vfformname) )
			{
/* # line 910 "vfcat.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 911 "vfcat.qsc" */	/* host code */
			}
			if (vfnoload)
			{
			    OOcatEmpty(OC_FORM, _vfcatalog, _cattable);
			}
			else if (Lastdifferent)
			{
			    OOcatTableLoad(OC_FORM, _vfcatalog, _cattable,
				    (bool) FALSE, ERx("*"), Lastname, NULL);
			}
			else if (Lastsaved)
			{
			    OOcatTableLoad(OC_FORM, _vfcatalog, _cattable,
				    (bool) FALSE, ERx("*"), cname, NULL);
			}
		}
          }
        } else if (IIretval() == 4) {
          {
/* # line 931 "vfcat.qsc" */	/* host code */
  register char *cname;
  char bufr[136];
  OOID qid;
		register char	*classname;
		bool	completed;
		i4	tries;
		i4	idindex;
		i4	i;
		bool	qbffound;
		bool	qbfduplicate;
		i4	dlock = FALSE;
		char	*bf = NULL;
		if (OOcatCheck(OC_FORM, _vfcatalog, _cattable,
			&vfoocatrec) != OK)
		{
/* # line 949 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 950 "vfcat.qsc" */	/* host code */
		}
		classname = ((OO_CLASS *)OOp(OC_FORM))->name;
		cname = vfoocatrec.name;
/* # line 956 "vfcat.qsc" */	/* prompt */
            {
              IIprmptio(0,ERget(S_VF001C_Enter_new_name),(short *)0,1,32,135,
              bufr);
            }
/* # line 957 "vfcat.qsc" */	/* host code */
		if ( STtrmwhite(bufr) <= 0 || bufr[0] == EOS )
		{
/* # line 959 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 960 "vfcat.qsc" */	/* host code */
		}
		if (FEchkname(bufr) != OK)
		{
			i = FE_MAXNAME;
			IIUGerr(E_VF001D_not_legal_name, UG_ERR_ERROR,
				3, bufr, classname, (PTR) &i);
/* # line 966 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 967 "vfcat.qsc" */	/* host code */
		}
		CVlower(bufr);
		qid = UNKNOWN;
		if ( IIOOidCheck(&vfid_frm, &qid, bufr, uidbdata->user) == OK )
		{
			IIUGerr(E_VF001E_Form_already_exists, UG_ERR_ERROR,
				1, bufr);
/* # line 975 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 976 "vfcat.qsc" */	/* host code */
		}
		vfooid = vfoocatrec.id;
		qbffound = FALSE;
/* # line 1011 "vfcat.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select ii_qbfnames.object_id from ii_qbfnames, ii_objects where ii_qb\
fnames.object_id=ii_objects.object_id and ii_objects.object_class=");
    IIputdomio((short *)0,1,30,sizeof(OC_QBFNAME),(void *)IILQint(OC_QBFNAME));
    IIwritio(0,(short *)0,1,32,0,(char *)"and ii_objects.object_name=");
    IIputdomio((short *)0,1,32,0,cname);
    IIwritio(0,(short *)0,1,32,0,(char *)"and ii_objects.object_owner=");
    IIputdomio((short *)0,1,32,0,uidbdata->user);
    IIwritio(0,(short *)0,1,32,0,(char *)"and ii_qbfnames.frname=");
    IIputdomio((short *)0,1,32,0,cname);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&qid);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 1020 "vfcat.sc" */	/* host code */
			qbffound = TRUE;
/* # line 1021 "vfcat.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE1;
      }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 1023 "vfcat.sc" */	/* host code */
		if (qbffound)	/* found qbfname whose name is same as form */
		{
			qbfduplicate = FALSE;
/* # line 1026 "vfcat.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select object_id from ii_objects where object_class=");
    IIputdomio((short *)0,1,30,sizeof(OC_QBFNAME),(void *)IILQint(OC_QBFNAME));
    IIwritio(0,(short *)0,1,32,0,(char *)"and object_name=");
    IIputdomio((short *)0,1,32,0,bufr);
    IIwritio(0,(short *)0,1,32,0,(char *)"and object_owner=");
    IIputdomio((short *)0,1,32,0,uidbdata->user);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&qid);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 1033 "vfcat.sc" */	/* host code */
				qbfduplicate = TRUE;
/* # line 1034 "vfcat.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE2;
      }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
/* # line 1036 "vfcat.sc" */	/* host code */
			if (qbfduplicate)
			{
				if (!IIUFver(ERget(S_VF001F_QBFName_exists),
					1, bufr))
				{
/* # line 1015 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1016 "vfcat.qsc" */	/* host code */
				}
			}
		}
		iiuicw1_CatWriteOn();
		IIUGmsg(ERget(S_VF0020_Renaming), FALSE, 3, classname,
			cname, bufr);
		completed = FALSE;
		for (tries = 0; tries < MAXDLOCK_TRIES; tries++)
		{
			register i4	j;
			bool		getout;
			i4		errnum;
			IIUIbeginXaction();
			if (qbffound)
			{
				if (qbfduplicate)
				{
					idindex = getQbfNames(bufr, idlist);
					getout = FALSE;
					for (j = 0; j < idindex; j++)
					{
					    qid = idlist[j];
/* # line 1069 "vfcat.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_objects where object_id=");
    IIputdomio((short *)0,1,30,4,&qid);
    IIsyncup((char *)0,0);
  }
/* # line 1070 "vfcat.sc" */	/* host code */
					    if ((errnum = FEinqerr()))
					    {
                                        	IIUIabortXaction();
						if (IIUIdeadlock(errnum))
						{
						    getout = TRUE;
						    dlock = TRUE;
						    break;
						}
						iiuicw0_CatWriteOff();
/* # line 1054 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1055 "vfcat.qsc" */	/* host code */
					    }
/* # line 1087 "vfcat.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_qbfnames where object_id=");
    IIputdomio((short *)0,1,30,4,&qid);
    IIsyncup((char *)0,0);
  }
/* # line 1088 "vfcat.sc" */	/* host code */
					    if ((errnum = FEinqerr()))
					    {
                                        	IIUIabortXaction();
						if (IIUIdeadlock(errnum))
						{
						    getout = TRUE;
						    dlock = TRUE;
						    break;
						}
						iiuicw0_CatWriteOff();
/* # line 1069 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1070 "vfcat.qsc" */	/* host code */
					    }
/* # line 1106 "vfcat.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_longremarks where object_id=");
    IIputdomio((short *)0,1,30,4,&qid);
    IIsyncup((char *)0,0);
  }
/* # line 1107 "vfcat.sc" */	/* host code */
					    if ((errnum = FEinqerr()))
					    {
                                        	IIUIabortXaction();
						if (IIUIdeadlock(errnum))
						{
						    getout = TRUE;
						    dlock = TRUE;
						    break;
						}
						iiuicw0_CatWriteOff();
/* # line 1085 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1086 "vfcat.qsc" */	/* host code */
					    }
					}
					if (getout)
					{
					    continue;
					}
				}
				idindex = 0;
/* # line 1130 "vfcat.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select ii_qbfnames.object_id from ii_qbfnames, ii_objects where ii_qb\
fnames.object_id=ii_objects.object_id and ii_objects.object_class=");
    IIputdomio((short *)0,1,30,sizeof(OC_QBFNAME),(void *)IILQint(OC_QBFNAME));
    IIwritio(0,(short *)0,1,32,0,(char *)"and ii_objects.object_name=");
    IIputdomio((short *)0,1,32,0,cname);
    IIwritio(0,(short *)0,1,32,0,(char *)"and ii_objects.object_owner=");
    IIputdomio((short *)0,1,32,0,uidbdata->user);
    IIwritio(0,(short *)0,1,32,0,(char *)"and ii_qbfnames.frname=");
    IIputdomio((short *)0,1,32,0,cname);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&qid);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 1140 "vfcat.sc" */	/* host code */
					idlist[idindex++] = qid;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
				getout = FALSE;
				for (j = 0; j < idindex; j++)
				{
					qid = idlist[j];
/* # line 1146 "vfcat.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_objects set object_name=");
    IIputdomio((short *)0,1,32,0,bufr);
    IIwritio(0,(short *)0,1,32,0,(char *)"where object_id=");
    IIputdomio((short *)0,1,30,4,&qid);
    IIsyncup((char *)0,0);
  }
/* # line 1149 "vfcat.sc" */	/* host code */
					if ((errnum = FEinqerr()))
					{
                                            IIUIabortXaction();
					    if (IIUIdeadlock(errnum))
					    {
						getout = TRUE;
						dlock = TRUE;
						break;
					    }
					    iiuicw0_CatWriteOff();
/* # line 1125 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1126 "vfcat.qsc" */	/* host code */
					}
/* # line 1165 "vfcat.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"update ii_qbfnames set frnam\
e=");
    IIputdomio((short *)0,1,32,0,bufr);
    IIwritio(0,(short *)0,1,32,0,(char *)"where object_id=");
    IIputdomio((short *)0,1,30,4,&qid);
    IIsyncup((char *)0,0);
  }
/* # line 1168 "vfcat.sc" */	/* host code */
					if ((errnum = FEinqerr()))
					{
                                            IIUIabortXaction();
					    if (IIUIdeadlock(errnum))
					    {
						getout = TRUE;
						dlock = TRUE;
						break;
					    }
					    iiuicw0_CatWriteOff();
/* # line 1141 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1142 "vfcat.qsc" */	/* host code */
					}
				}
				if (getout)
				{
					continue;
				}
			}
			idindex = getQbfNames(cname, idlist);
			getout = FALSE;
			for (j = 0; j < idindex; j++)
			{
				qid = idlist[j];
/* # line 1195 "vfcat.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"update ii_qbfnames set frnam\
e=");
    IIputdomio((short *)0,1,32,0,bufr);
    IIwritio(0,(short *)0,1,32,0,(char *)"where object_id=");
    IIputdomio((short *)0,1,30,4,&qid);
    IIsyncup((char *)0,0);
  }
/* # line 1198 "vfcat.sc" */	/* host code */
				if ((errnum = FEinqerr()))
				{
                                       	IIUIabortXaction();
					if (IIUIdeadlock(errnum))
					{
						getout = TRUE;
						dlock = TRUE;
						break;
					}
					iiuicw0_CatWriteOff();
/* # line 1170 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1171 "vfcat.qsc" */	/* host code */
				}
			}
			if (getout)
			{
				continue;
			}
			completed = TRUE;
			break;
		} /* end for */
		iiuicw0_CatWriteOff();
		if (!completed)
		{
			IIUGerr( dlock ? E_VF0021_Deadlock_has_occurred
					: E_VF0022_Error_occurred_in_cha,
					UG_ERR_ERROR, 0
			);
/* # line 1189 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1190 "vfcat.qsc" */	/* host code */
		}
		if ( IIOOidRename(&vfid_frm, &(vfoocatrec.id),
				vfoocatrec.name, vfoocatrec.owner,
				(bf = STalloc(bufr))) == OK )
		{
	                IIUIendXaction();
		}
		else
		{
			IIUGerr(E_VF0023_Renaming, UG_ERR_ERROR,
				2, classname, vfoocatrec.name);
			if (bf)
			{
				MEfree(bf);
			}
/* # line 1207 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1208 "vfcat.qsc" */	/* host code */
		}
		if (vfnoload)
		{
			OOcatEmpty(OC_FORM, _vfcatalog, _cattable);
		}
		else
		{
			vfLast(bufr);
			OOcatTableLoad(OC_FORM, _vfcatalog, _cattable,
				(bool) FALSE, ERx("*"), Lastname, NULL);
		}
          }
        } else if (IIretval() == 5) {
          {
/* # line 1222 "vfcat.qsc" */	/* host code */
		if (OOcatCheck(OC_FORM, _vfcatalog, _cattable,
			&vfoocatrec) != OK)
		{
/* # line 1225 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1226 "vfcat.qsc" */	/* host code */
		}
		detail_alloc();
		_VOID_ OOdetail(OC_FORM, _vfcatalog, _cattable, &vfoocatrec,
			_vfdetail, ERget(F_VF0017_VIFRED_Detail_on_a),
			NULL, NULL, NULL, NULL, NULL, vfsvdummy, NULL, NULL);
		if (vfnoload)
		{
			OOcatEmpty(OC_FORM, _vfcatalog, _cattable);
		}
          }
        } else if (IIretval() == 6) {
          {
            char *copyout;
            char fname[150];
/* # line 1245 "vfcat.qsc" */	/* host code */
		copyout = iivfRTIspecial ? ERget(FE_Write) : NULL;
		for (;;)
		{
/* # line 1248 "vfcat.qsc" */	/* submenu */
            {
              goto IImuI2;
IIfdB2:
              IIrunmu(0);
              if (IIretval() == 1) {
                {
/* # line 1251 "vfcat.qsc" */	/* host code */
				register char	*cname;
				i4	rval;
				i4	mac;
				/* "FALSE" for fix to bug 9713 */
				if (OOcatCheck(OC_FORM, _vfcatalog,
					_cattable, &vfoocatrec) != OK)
				{
/* # line 1259 "vfcat.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB1;
                  }
/* # line 1260 "vfcat.qsc" */	/* host code */
				}
/* # line 1262 "vfcat.qsc" */	/* prompt */
                  {
                    IIprmptio(0,ERget(S_VF0024_File_in_which_to_comp),
                    (short *)0,1,32,149,fname);
                  }
/* # line 1264 "vfcat.qsc" */	/* host code */
				if ( STtrmwhite(fname) <= 0 || fname[0] == EOS )
				{
/* # line 1266 "vfcat.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB1;
                  }
/* # line 1267 "vfcat.qsc" */	/* host code */
				}
# ifdef CMS
				TOwscnme(fname);
# endif
				cname = vfoocatrec.name;
				IIUGmsg(ERget(S_VF0025_Compiling_Form),
					FALSE, 0);
				/* BUG #1495
				**  Must call UTcommand to get the
				**  proper command.  Args to UTcommand
				**  used name and should be cname (nml).
				**  Change fixes bug # xxxx -11/2/83.
				*/
				FDfrprint( 1 );
				/*
				** Changed to use new UTexe module interface
				** peterk - 12/13/84
				*/
				mac = (*MACRO != EOS ? 1 : 0);
				rval = IIcompfrm(cname, NULL, cname,fname, mac);
				/*
				**  Set validation back to parsing state
				**  since IIcompfrm changed it.
				*/
				FDsetparse(TRUE);
				FDfrprint( 0 );
				/*
				**  Bug fix # 4193 and # 6922 (dkh)
				*/
				if (rval != OK)
				{
					IIUGerr(E_VF0111_COMPILE_FAILED, 0, 2,
						cname, fname);
/* # line 1307 "vfcat.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB1;
                  }
/* # line 1308 "vfcat.qsc" */	/* host code */
				}
				if (vfnoload)
				{
					OOcatEmpty(OC_FORM, _vfcatalog,
						_cattable);
				}
				break;
                }
              } else if (IIretval() == 2) {
                {
/* # line 1319 "vfcat.qsc" */	/* host code */
				if (OOcatCheck(OC_FORM, _vfcatalog,
					_cattable, &vfoocatrec) != OK)
				{
/* # line 1322 "vfcat.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB1;
                  }
/* # line 1323 "vfcat.qsc" */	/* host code */
				}
/* # line 1325 "vfcat.qsc" */	/* prompt */
                  {
                    IIprmptio(0,ERget(S_VF0026_File_in_which_to_prin),
                    (short *)0,1,32,149,fname);
                  }
/* # line 1327 "vfcat.qsc" */	/* host code */
				if ( STtrmwhite(fname) <= 0 || fname[0] == EOS )
				{
/* # line 1329 "vfcat.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB1;
                  }
/* # line 1330 "vfcat.qsc" */	/* host code */
				}
# ifdef CMS
				TOwscnme(fname);
# endif
				dumpform(vfoocatrec.name, fname,
					IIVFcsr_ClrScrRedisp);
				if (vfnoload)
				{
					OOcatEmpty(OC_FORM, _vfcatalog,
						_cattable);
				}
				break;
                }
              } else if (IIretval() == 3) {
                {
/* # line 1347 "vfcat.qsc" */	/* host code */
				i4	rval;
				/* "FALSE" for fix to bug 9713 */
				if (OOcatCheck(OC_FORM, _vfcatalog,
					_cattable, &vfoocatrec) != OK)
				{
/* # line 1353 "vfcat.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB1;
                  }
/* # line 1354 "vfcat.qsc" */	/* host code */
				}
				detail_alloc();
				fdqbfmap(vfoocatrec.name);
				if (vfnoload)
				{
					OOcatEmpty( OC_FORM, _vfcatalog,
						_cattable );
				}
				break;
                }
              } else if (IIretval() == 4) {
                {
/* # line 1370 "vfcat.qsc" */	/* host code */
				if ( OOcatCheck(OC_FORM, _vfcatalog,
					_cattable, &vfoocatrec ) != OK)
				{
/* # line 1373 "vfcat.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB1;
                  }
/* # line 1374 "vfcat.qsc" */	/* host code */
				}
# ifdef CMS
				TOwscnme(fname);
# endif /* CMS */
/* # line 1379 "vfcat.qsc" */	/* prompt */
                  {
                    IIprmptio(0,ERget(S_VF012A_OutputFile),(short *)0,1,32,149
                    ,fname);
                  }
/* # line 1380 "vfcat.qsc" */	/* host code */
				if ( STtrmwhite(fname) <= 0 || fname[0] == EOS )
				{
/* # line 1382 "vfcat.qsc" */	/* resume */
                  {
                    if (1) goto IIfdB1;
                  }
/* # line 1383 "vfcat.qsc" */	/* host code */
				}
				vfexe( ERx("copyform"), ERx("CopyForm"),
						ERx("database = %S, equel = %S, file = %S, form = %S"),
						2, fname, vfoocatrec.name
				);
				break;
                }
              } else if (IIretval() == 5) {
                {
/* # line 1394 "vfcat.qsc" */	/* host code */
				STcopy(ERget(S_VF0027_Utilities_Submenu),
					hlpbuf);
				FEhelp(VFH_UTILSMU, hlpbuf);
				continue;
                }
              } else if (IIretval() == 6) {
                {
/* # line 1402 "vfcat.qsc" */	/* host code */
				break;
                }
              } else {
                if (1) goto IIfdE2;
              } /* IIretval */
              goto IIfdE2;
IImuI2:
              IImuonly();
              if (IIinitmu() == 0) goto IIfdE2;
              if (IInmuact(ERget(FE_Compile),ERget(_CompileExpl),2,2,1) == 0) 
              goto IIfdE2;
              if (IInmuact(ERget(FE_Print),ERget(_PrintExpl),2,2,2) == 0) 
              goto IIfdE2;
              if (IInmuact(ERget(FE_QBFNames),ERget(_QBFNamesExpl),2,2,3) == 0
              ) goto IIfdE2;
              if (IInmuact(copyout,ERget(_CopyFormExpl),2,2,4) == 0) goto 
              IIfdE2;
              if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),2,2,5) == 0) goto 
              IIfdE2;
              IIFRInternal(0);
              if (IInfrskact(1,(char *)0,2,2,5) == 0) goto IIfdE2;
              if (IInmuact(ERget(FE_End),ERget(_CancelExpl),2,2,6) == 0) goto 
              IIfdE2;
              IIFRInternal(0);
              if (IInfrskact(3,(char *)0,2,2,6) == 0) goto IIfdE2;
              if (IIendmu() == 0) goto IIfdE2;
              goto IIfdB2;
IIfdE2:;
            }
/* # line 1404 "vfcat.qsc" */	/* host code */
		} /* end submenu for */
          }
        } else if (IIretval() == 7) {
          {
/* # line 1409 "vfcat.qsc" */	/* host code */
		OOID	go_class = OC_QBFNAME;
		OOID	go_qnmid = UNKNOWN;
		char	*go_qflag = ERx("-F");
            char go_table[FE_MAXTABNAME + 1];
		char	go_args[200];
		if (OOcatCheck(OC_FORM, _vfcatalog, _cattable,
			&vfoocatrec) != OK)
		{
/* # line 1419 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1420 "vfcat.qsc" */	/* host code */
		}
		if ( IIOOidCheck(&go_class, &go_qnmid, vfoocatrec.name,
					(char *)NULL) == OK )
		{
			go_qflag = ERx("-f");
			go_table[0] = EOS;
		}
		else
		{
            char prbuf[200];
			FE_RSLV_NAME	delim_id;
			char		table[FE_UNRML_MAXNAME + 1];
			char		owner[FE_UNRML_MAXNAME + 1];
			char		normal[FE_UNRML_MAXNAME + 1];
			IIUGfmt(prbuf, sizeof(prbuf),
				ERget(S_VF0028_Table_to_use_with_for),
				1, vfoocatrec.name);
/* # line 1439 "vfcat.qsc" */	/* prompt */
            {
              IIprmptio(0,prbuf,(short *)0,1,32,FE_MAXTABNAME + 1-1,go_table);
            }
/* # line 1440 "vfcat.qsc" */	/* host code */
			if ( STtrmwhite(go_table) <= 0 || go_table[0] == EOS )
			{
/* # line 1442 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1443 "vfcat.qsc" */	/* host code */
			}
			/*
			**  Run user entered name through delim
			**  checking code so we can determine
			**  if the table exists.
			**
			**  Note that we leave what the user entered
			**  alone so we can use it directly as a parameter
			**  when calling QBF.  This way, we don't have
			**  to requote as the user should have done it
			**  on entry.
			*/
			delim_id.is_nrml = FALSE;
			delim_id.owner_dest = owner;
			delim_id.name_dest = table;
			delim_id.name = go_table;
			FE_decompose(&delim_id);
			/*
			**  Check to make sure user did not enter a name that
			**  simply begins or ends in a period outside of a
			**  delimited identifier.  If the name
			**  begins with a period, FE_decompose will have set
			**  the owner_spec member of FE_RESLV_NAME but that
			**  owner_dest contains an empty string.  If the name
			**  ends in a period, then the name_dest member will
			**  contain an empty string.
			*/
			if ((delim_id.owner_spec &&
				delim_id.owner_dest[0] == EOS) ||
				delim_id.name_dest[0] == EOS)
			{
				IIUGerr(E_VF0171_BAD_TBL_NAME, UG_ERR_ERROR,
					1, go_table);
/* # line 1479 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1480 "vfcat.qsc" */	/* host code */
			}
			if (IIUGdlm_ChkdlmBEobject(table, normal,
				(bool) FALSE) == UI_BOGUS_ID)
			{
				IIUGerr(E_VF0171_BAD_TBL_NAME,
					UG_ERR_ERROR, 1, table);
/* # line 1487 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1488 "vfcat.qsc" */	/* host code */
                        }
			STcopy(normal, table);
			if (owner[0] != EOS)
			{
				if (IIUGdlm_ChkdlmBEobject(owner,
					normal, (bool) FALSE) == UI_BOGUS_ID)
				{
					IIUGerr(E_VF0170_BAD_OWNER_NAME,
						UG_ERR_ERROR, 1, owner);
/* # line 1497 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1498 "vfcat.qsc" */	/* host code */
				}
				STcopy(normal, owner);
			}
			if (FErelexists(table, owner) != OK)
			{
				IIUGerr(E_VF0029_Table_does_not,
					UG_ERR_ERROR, 1, go_table);
/* # line 1505 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1506 "vfcat.qsc" */	/* host code */
			}
		}
		/*
		**  No testing flags passed for now.
		*/
		vfcallqbf( ERx("database = %S, equel = %S, flags = %S"),
				STprintf(go_args, ERx("%s %s %s %s %s"),
					vfuname != NULL ? vfuname : ERx(""),
					ERx("-mall"), go_qflag,
					vfoocatrec.name, go_table
				)
		);
          }
        } else if (IIretval() == 8) {
          {
/* # line 1521 "vfcat.qsc" */	/* host code */
		/* Implement the Find command. */
            i4 formcount;
/* # line 1524 "vfcat.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,_vfcatalog,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&formcount,31,_cattable,0);
              } /* IIiqset */
            }
/* # line 1525 "vfcat.qsc" */	/* host code */
		if (formcount == 0)
		{
			IIUGerr(E_VF0012_Empty_Find, UG_ERR_ERROR, 0);
/* # line 1528 "vfcat.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1529 "vfcat.qsc" */	/* host code */
		}
		FEtabfnd(_vfcatalog, _cattable);
          }
        } else if (IIretval() == 9) {
          {
/* # line 1535 "vfcat.qsc" */	/* host code */
		/* Implement the Top command. */
/* # line 1536 "vfcat.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_vfcatalog,_cattable,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 10) {
          {
/* # line 1540 "vfcat.qsc" */	/* host code */
		/* Implement the Bottom command. */
/* # line 1541 "vfcat.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_vfcatalog,_cattable,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 11) {
          {
/* # line 1546 "vfcat.qsc" */	/* host code */
		STcopy(ERget(S_VF002A_Form_Catalogs), hlpbuf);
		FEhelp(VFH_CATALOG, hlpbuf);
          }
        } else if (IIretval() == 12) {
          {
/* # line 1552 "vfcat.qsc" */	/* host code */
		/* Use _QuitVF since legitimately the same explanation. */
/* # line 1553 "vfcat.qsc" */	/* clear */
            {
              IIclrscr();
            }
/* # line 1554 "vfcat.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 13) {
          {
/* # line 1559 "vfcat.qsc" */	/* clear */
            {
              IIclrscr();
            }
/* # line 1560 "vfcat.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
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
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Create),ERget(_CreateExpl),2,2,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Destroy),ERget(_DestroyExpl),2,2,2) == 0) goto 
      IIfdE1;
      if (IInmuact(ERget(FE_Edit),ERget(_EditExpl),2,2,3) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Rename),ERget(_RenameExpl),2,2,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_MoreInfo),ERget(_InfoExpl),2,2,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Utilities),ERget(uexplid),2,2,6) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Go),ERget(_QBFrunExpl),2,2,7) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,7) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,8) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,9) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,10) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,2,11) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,11) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(_QuitVF),0,2,12) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,2,12) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Quit),ERget(_QuitVF),0,2,13) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(2,(char *)0,0,2,13) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 1562 "vfcat.qsc" */	/* host code */
	return;
  }
static i4
getQbfNames ( name, list )
  char *name;
register OOID	list[];
{
	register i4	idindex;
  OOID qid;
  UIDBDATA *uidbdata;
	idindex = 0;
	uidbdata = IIUIdbdata();
/* # line 1782 "vfcat.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"vfcat1",7505,31703);
      IIputdomio((short *)0,1,30,sizeof(OC_QBFNAME),(void *)IILQint(OC_QBFNAME
      ));
      IIputdomio((short *)0,1,32,0,uidbdata->user);
      IIputdomio((short *)0,1,32,0,name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"vfcat1",7505,31703);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select ii_qbfnames.object_id from ii_qbfnames, ii_objects where ii_qb\
fnames.object_id=ii_objects.object_id and ii_objects.object_class=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,sizeof(OC_QBFNAME),(void *)IILQint(
        OC_QBFNAME));
        IIwritio(0,(short *)0,1,32,0,(char *)"and ii_objects.object_ow\
ner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,uidbdata->user);
        IIwritio(0,(short *)0,1,32,0,(char *)"and ii_qbfnames.frname=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,name);
        IIexDefine(0,(char *)"vfcat1",7505,31703);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE4;
IIrtB4:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&qid);
      if (IIerrtest() != 0) goto IIrtB4;
/* # line 1790 "vfcat.sc" */	/* host code */
		list[idindex++] = qid;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE4:;
  }
	return idindex;
}
/*VARARGS5*/
VOID
vfexe ( program, name, alist, nargs, arg0, arg1, arg2 )
char	*program;
char	*name;
char	*alist;
i4		nargs;
char	*arg0;
char	*arg1;
char	*arg2;
{
	CL_ERR_DESC err_code;
	STATUS	    rval;
	FTrestore(FT_NORMAL);
	rval = UTexe(UT_WAIT, NULL, (ModDictDesc *) NULL, (PTR)NULL, program,
		&err_code, alist, nargs + 2, dbname, IIxflag(), arg0, arg1, arg2
	);
	FTrestore(FT_FORMS);
	if ( rval != OK )
	{
		i4	err = rval;
		IIUGerr(E_VF002B_ExecutionError, UG_ERR_ERROR, 2, (PTR)name, (PTR)&err);
	}
/* # line 1620 "vfcat.qsc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 1621 "vfcat.qsc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 1622 "vfcat.qsc" */	/* host code */
}
VOID
vfLast(name)
char	*name;
{
	if ( !Lastdifferent && Lastname[0] != EOS )
	{
		if ( !STequal(name, Lastname) )
		{
			Lastdifferent = TRUE;
		}
	}
	Lastsaved = TRUE;
	STcopy(name, Lastname);
}
VOID
vfcallqbf(go_param, go_args)
char	*go_param;
char	*go_args;
{
	char	*args;
	char	argbuf[500];
	char	testbuf[200];
	if (IIVFiflg || IIVFzflg)
	{
		if (IIVFiflg)
		{
			FEinames(testbuf);
		}
		else
		{
			FEtnames(testbuf);
		}
		args = STprintf(argbuf, ERx("%s %s"), go_args, testbuf);
	}
	else
	{
		args = go_args;
	}
	vfexe( ERx("qbf"), ERx("QBF"), go_param, 1, args );
}
/*
** Name: IIVFcsr_ClrScrRedisp	- clear and redisplay the screen
**
** Description:
**	If DG, clear and redisplay the screen, otherwise do nothing.
**	This is so that when running on DG in CEO, the dumpform routine
**	can quickly regain control of the screen contents from CEO's
**	drawer/folder/file prompt screen.
**
** Inputs:
**
** Outputs:
**
**	Return:
**		VOID
**
** Side Effects:
**	Clears the screen and redisplays the FRS contents (if DG).
*/
VOID
IIVFcsr_ClrScrRedisp()
{
# ifdef	DGC_AOS
/* # line 1693 "vfcat.qsc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 1694 "vfcat.qsc" */	/* redisplay */
  {
    IIredisp();
  }
/* # line 1695 "vfcat.qsc" */	/* host code */
# endif
}
