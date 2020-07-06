# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
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
# include	<uf.h>
# include	<ui.h>
# include	"ervf.h"
# include	"vfhlp.h"
/**
** Name:	vfcrtfrm.qsc - Routines used in creation of new forms.
**
** Description:
**	This file contains routines used in the creation of new forms.
**	The central routine is a handler called by ListPick from fdcats().
**	Various internal routines contain display loops for the various
**	methods of creating a form.
**
**	IIVFcfCreateForm	Handler from ListPick for forms creation.
**
** History:
**	26-oct-89 (bruceb)	Written.
**	29-nov-89 (bruceb)
**		Attached menuitem explanations to the matching frskeys.
**	03/28/91 (dkh) - Replaced old inquire_frs syntax with new.
**	04/06/91 (dkh) - Fixed bug 36725.  Name checking must be done
**			 in code and not form due to international characters.
**	08/19/91 (dkh) - Integrated changes from PC group.
**	09/20/92 (dkh) - Added support for owner.table.
**	04-jan-92 (sylviap)
**		Added a new menuitem "Edit" to create joindefs when creating a
**		form based on a joindef.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
GLOBALREF FILE	*vfdfile;
FUNC_EXTERN	STATUS	vftblnmchk();
FUNC_EXTERN	char	*IITUtpTblPik();
FUNC_EXTERN	char	*IIUFopObjPik();
FUNC_EXTERN	FRAME	*vfgetFrame();
FUNC_EXTERN	i4	vffnmchk();
FUNC_EXTERN     char    *IIxflag();
static	i4	cr_f_tbl();
static	i4	cr_f_dup();
static	i4	cr_f_jdef();
# define NO_FORM	-1	/* "Cancel" */
# define DUP_FORM	0
# define BLNK_FORM	1
# define TBL_FORM	2
# define JDEF_FORM	3
static	bool	cre_t_alloced = FALSE;	/* Create from table. */
static	bool	cre_d_alloced = FALSE;	/* Create from duplicated form. */
static	bool	cre_j_alloced = FALSE;	/* Create from joindef. */
  static char *tf_str;
  static char *sf_str;
  static char *vfcrform_tbl = ERx("vfcrform_tbl");
  static char *vfcrform_dup = ERx("vfcrform_dup");
  static char *vfcrform_jdef = ERx("vfcrform_jdef");
static VOID
allocCT()
{
    LOCATION	*fnx;
    if (!cre_t_alloced)
    {
	FDsetparse(FALSE);
	fnx = IIUFlcfLocateForm();
	if (IIUFgtfGetForm(fnx, vfcrform_tbl) != OK)
	{
	    syserr(ERget(S_VF0001_Can_not_access_form), vfcrform_tbl);
	}
	tf_str = STalloc(ERget(S_VF014E_tablefield));
	sf_str = STalloc(ERget(S_VF014F_simplefields));
	cre_t_alloced = TRUE;
	FDsetparse(TRUE);
    }
}
static VOID
allocCD()
{
    LOCATION	*fnx;
    if (!cre_d_alloced)
    {
	FDsetparse(FALSE);
	fnx = IIUFlcfLocateForm();
	if (IIUFgtfGetForm(fnx, vfcrform_dup) != OK)
	{
	    syserr(ERget(S_VF0001_Can_not_access_form), vfcrform_dup);
	}
	cre_d_alloced = TRUE;
	FDsetparse(TRUE);
    }
}
static VOID
allocCJ()
{
    LOCATION	*fnx;
    if (!cre_j_alloced)
    {
	FDsetparse(FALSE);
	fnx = IIUFlcfLocateForm();
	if (IIUFgtfGetForm(fnx, vfcrform_jdef) != OK)
	{
	    syserr(ERget(S_VF0001_Can_not_access_form), vfcrform_jdef);
	}
	cre_j_alloced = TRUE;
	FDsetparse(TRUE);
    }
}
/*{
** Name:	IIVFcfCreateForm	- Handler for forms creation.
**
** Description:
**	Handler from ListPick used for forms creation.  Calls routines used
**	for specifying details dependent on form creation method and then
**	returns on up to fdcats().
**
** Inputs:
**	dummy	Not used, but required for ListPick handlers.
**	choice	ListPick choice number (-1/NO_FORM for 'Cancel').
**
** Outputs:
**	resume	Whether or not to remain on the ListPick form.
**
**	Returns:
**		choice	ListPick choice number (may be different than orig val.)
**
**	Exceptions:
**		None.
**
** Side Effects:
**	None.
**
** History:
**	26-oct-89 (bruceb)	Written.
*/
i4
IIVFcfCreateForm(dummy, choice, resume)
PTR	dummy;		/* Not used. */
i4	choice;		/* Method of form creation. */
bool	*resume;	/* Another chance to select form creation method? */
{
	if (choice == DUP_FORM)	/* Form created from duplicate form. */
	{
	    if (cr_f_dup() == NO_FORM)
	    {
		*resume = TRUE;	/* Resume back to choice of creation method. */
	    }
	    /* Not really 'editing' a form, so type 'OT_BLANK' is proper. */
	    vforigintype = OT_BLANK;
	}
	else if (choice == BLNK_FORM)	/* Form created from blank screen. */
	{
	    vforigintype = OT_BLANK;
	}
	else if (choice == TBL_FORM) /* Form created from table(s). */
	{
	    if (cr_f_tbl() == NO_FORM)
	    {
		*resume = TRUE;	/* Resume back to choice of creation method. */
	    }
	}
	else if (choice == JDEF_FORM) /* Form created from JoinDef. */
	{
	    if (cr_f_jdef() == NO_FORM)
	    {
		*resume = TRUE;	/* Resume back to choice of creation method. */
	    }
	}
	return(choice);
}
/*{
** Name:	cr_f_tbl	- Display loop for form creation from a table.
**
** Description:
**	User has specified creation of a form from a table.  Allow user to
**	specify the name of the table used as the basis for the form, and
**	whether the form style should be a tablefield or simple fields.
**
** Inputs:
**
** Outputs:
**
**	Returns:
**		NO_FORM		User has 'Cancel'd.
**		TBL_FORM	User wishes to create the form.
**
**	Exceptions:
**		None.
**
** Side Effects:
**	None.
**
** History:
**	26-oct-89 (bruceb)	Written.
**	20-jan-1994 (rdrane)
**		Don't case or otherwise mess with any tablename - let
**		vftblnmchk() do the work.  This was overlooked during the
**		initial 6.5 modifications.  Bug 58272.
*/
static i4
cr_f_tbl()
{
	bool	tblfield = TRUE;
	LIST	*nlist = NULL;
	LIST	*olist = NULL;
	LIST	*lt;
	LIST	*olt;
	i4	i;
	i4	retval = TBL_FORM;
  char *tblpik;
  char dispfmt[20];
  char onename[FE_MAXNAME + 1];
	char	hlpbuf[132+1];
  char names[201];
	allocCT();
/* # line 260 "vfcrtfrm.qsc" */	/* display */
  {
    if (IIdispfrm(vfcrform_tbl,(char *)"f") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 263 "vfcrtfrm.qsc" */	/* validate */
          {
            if (IIvalfld((char *)"format") == 0) {
              goto IIfdB1;
            } /* IIvalfld */
          }
/* # line 264 "vfcrtfrm.qsc" */	/* getform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIgetfldio((short *)0,1,32,19,dispfmt,(char *)"format");
            } /* IIfsetio */
          }
/* # line 266 "vfcrtfrm.qsc" */	/* host code */
	    if (dispfmt[0] == tf_str[0])
	    {
		/* It's a request for a table field. */
/* # line 269 "vfcrtfrm.qsc" */	/* putform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIputfldio((char *)"format",(short *)0,1,32,0,tf_str);
            } /* IIfsetio */
          }
/* # line 270 "vfcrtfrm.qsc" */	/* host code */
		tblfield = TRUE;
	    }
	    else
	    {
		/* It's a request for simple fields. */
/* # line 275 "vfcrtfrm.qsc" */	/* putform */
          {
            if (IIfsetio((char *)0) != 0) {
              IIputfldio((char *)"format",(short *)0,1,32,0,sf_str);
            } /* IIfsetio */
          }
/* # line 276 "vfcrtfrm.qsc" */	/* host code */
		tblfield = FALSE;
	    }
/* # line 279 "vfcrtfrm.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 283 "vfcrtfrm.qsc" */	/* getform */
          {
            if (IIfsetio(vfcrform_tbl) != 0) {
              IIgetfldio((short *)0,1,32,200,names,(char *)"tbl_name");
            } /* IIfsetio */
          }
/* # line 285 "vfcrtfrm.qsc" */	/* host code */
	    if (names[0] == EOS)
	    {
		IIUGerr(E_VF0150_no_table_name, UG_ERR_ERROR, 0);
/* # line 288 "vfcrtfrm.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 289 "vfcrtfrm.qsc" */	/* host code */
	    }
	    /*
	    ** Since we know that we have a tablename, let
	    ** vftblnmchk() handle the decomposition and casing.
	    */
	    if ((vftblnmchk(names, &nlist, &olist, &i) != OK) ||
		(nlist == NULL))
	    {
/* # line 298 "vfcrtfrm.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 299 "vfcrtfrm.qsc" */	/* host code */
	    }
	    IIUGmsg(ERget(S_VF0014_Creating_default_form), FALSE, 1, names);
	    if ((frame = vfgetRelFrame(nlist, olist, i, tblfield)) == NULL)
	    {
/* # line 304 "vfcrtfrm.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 305 "vfcrtfrm.qsc" */	/* host code */
	    }
	    /*
	    **  Free up LIST structs before continuing.
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
	    if (!VTnewfrm(frame))
	    {
/* # line 322 "vfcrtfrm.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 323 "vfcrtfrm.qsc" */	/* host code */
	    }
/* # line 324 "vfcrtfrm.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 331 "vfcrtfrm.qsc" */	/* host code */
	    retval = NO_FORM;
/* # line 332 "vfcrtfrm.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 338 "vfcrtfrm.qsc" */	/* inquire_frs */
          {
            if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,onename,22,
              vfcrform_tbl,0);
            } /* IIiqset */
          }
/* # line 339 "vfcrtfrm.qsc" */	/* host code */
	    /* Lookup available only for field 'tbl_name'. */
	    if (STcompare(onename, ERx("tbl_name")) == 0)
	    {
		/* Don't allow table creation, examination. */
		set_IituMainform(FALSE);
		/*
		**  IITUtpTblPik will do an unresolve on the
		**  selected owner.table name before returning
		**  it so we can just put it out for the user
		**  to edit.
		*/
		tblpik = IITUtpTblPik(NULL, ERx("VIFRED"));
		if ((tblpik != NULL) && (*tblpik != EOS))
		{
/* # line 354 "vfcrtfrm.qsc" */	/* putform */
          {
            if (IIfsetio(vfcrform_tbl) != 0) {
              IIputfldio((char *)"tbl_name",(short *)0,1,32,0,tblpik);
            } /* IIfsetio */
          }
/* # line 355 "vfcrtfrm.qsc" */	/* host code */
		}
	    }
	    else
	    {
		IIUFfieldHelp();
	    }
        }
      } else if (IIretval() == 5) {
        {
/* # line 367 "vfcrtfrm.qsc" */	/* host code */
	    STcopy(ERget(S_VF0153_Form_from_Table), hlpbuf);
	    FEhelp(VFH_TABCR, hlpbuf);
        }
      } else {
        if (1) goto IIfdE1;
      } /* IIretval */
    } /* IIrunform */
IIfdF1:
    if (IIchkfrm() == 0) goto IIfdB1;
    goto IIfdE1;
IImuI1:
    if (IIFRafActFld((char *)"format",0,1) == 0) goto IIfdE1;
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_OK),(char *)0,2,2,2) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,2) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),0,0,3) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(9,ERget(F_FE0103_ExplCancel),0,0,3) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,4) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(10,(char *)0,0,0,4) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,5) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,ERget(F_FE0100_ExplFrameHelp),0,0,5) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 371 "vfcrtfrm.qsc" */	/* host code */
	return(retval);
}
/*{
** Name:	cr_f_dup	- Display loop for form creation from another
**				  form.
**
** Description:
**	User has specified creation of a form from another form.  Allow user
**	to specify the name of the form used as the basis for the new form.
**
** Inputs:
**
** Outputs:
**
**	Returns:
**		NO_FORM		User has 'Cancel'd.
**		DUP_FORM	User wishes to create the form.
**
**	Exceptions:
**		None.
**
** Side Effects:
**	None.
**
** History:
**	26-oct-89 (bruceb)	Written.
*/
static i4
cr_f_dup()
{
	i4	retval = DUP_FORM;
  char *frmpik;
  char fname[FE_MAXNAME + 1];
	char	hlpbuf[132+1];
	allocCD();
/* # line 410 "vfcrtfrm.qsc" */	/* display */
  {
    if (IIdispfrm(vfcrform_dup,(char *)"f") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
IIfdB2:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 413 "vfcrtfrm.qsc" */	/* getform */
          {
            if (IIfsetio(vfcrform_dup) != 0) {
              IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,fname,(char *)
"dup_name");
            } /* IIfsetio */
          }
/* # line 415 "vfcrtfrm.qsc" */	/* host code */
	    if (fname[0] == EOS)
	    {
		IIUGerr(E_VF0151_no_form_name, UG_ERR_ERROR, 0);
/* # line 418 "vfcrtfrm.qsc" */	/* resume */
          {
            if (1) goto IIfdB2;
          }
/* # line 419 "vfcrtfrm.qsc" */	/* host code */
	    }
	    if (!vffnmchk(fname, ERget(FE_Table)))
	    {
/* # line 423 "vfcrtfrm.qsc" */	/* resume */
          {
            if (1) goto IIfdB2;
          }
/* # line 424 "vfcrtfrm.qsc" */	/* host code */
	    }
	    IIUGmsg(ERget(F_VF0080_Create_Duplicate_Form), FALSE, 1, fname);
	    if ((frame = vfgetFrame(fname)) == NULL)
	    {
		/* Error message has already been sent. */
/* # line 431 "vfcrtfrm.qsc" */	/* resume */
          {
            if (1) goto IIfdB2;
          }
/* # line 432 "vfcrtfrm.qsc" */	/* host code */
	    }
	    else
	    {
		vforgname[0] = EOS;	/* No 'original' name. */
		/* Ensure proper setup for 'Save' time. */
		frame->frname[0] = EOS;
	    }
/* # line 441 "vfcrtfrm.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 448 "vfcrtfrm.qsc" */	/* host code */
	    retval = NO_FORM;
/* # line 449 "vfcrtfrm.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE2;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 455 "vfcrtfrm.qsc" */	/* host code */
	    STcopy(ERget(S_VF0156_Existing_Forms), hlpbuf);
	    frmpik = IIUFopObjPik(hlpbuf, VFH_LSDUP, OC_FORM,
		ERget(F_VF0086_forms),
		ERget(F_VF0082_Selecting_form_names),
		ERget(F_VF0083_Form_Name));
	    if ((frmpik != NULL) && (*frmpik != EOS))
	    {
/* # line 462 "vfcrtfrm.qsc" */	/* putform */
          {
            if (IIfsetio(vfcrform_dup) != 0) {
              IIputfldio((char *)"dup_name",(short *)0,1,32,0,frmpik);
            } /* IIfsetio */
          }
/* # line 463 "vfcrtfrm.qsc" */	/* host code */
	    }
        }
      } else if (IIretval() == 4) {
        {
/* # line 470 "vfcrtfrm.qsc" */	/* host code */
	    STcopy(ERget(S_VF0154_Form_from_Dup_Form), hlpbuf);
	    FEhelp(VFH_CRDUP, hlpbuf);
        }
      } else {
        if (1) goto IIfdE2;
      } /* IIretval */
    } /* IIrunform */
IIfdF2:
    if (IIchkfrm() == 0) goto IIfdB2;
    goto IIfdE2;
IImuI2:
    if (IIinitmu() == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_OK),(char *)0,2,2,1) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),0,0,2) == 0) 
    goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(9,ERget(F_FE0103_ExplCancel),0,0,2) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,3) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(10,(char *)0,0,0,3) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,4) == 0) 
    goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(1,ERget(F_FE0100_ExplFrameHelp),0,0,4) == 0) goto IIfdE2;
    if (IIendmu() == 0) goto IIfdE2;
    goto IIfdI2;
IIfdE2:;
    IIendfrm();
  }
/* # line 474 "vfcrtfrm.qsc" */	/* host code */
	return(retval);
}
/*{
** Name:	cr_f_jdef	- Display loop for form creation from a joindef.
**
** Description:
**	User has specified creation of a form from a joindef.  Allow user
**	to specify the name of the joindef used as the basis for the form.
**
** Inputs:
**
** Outputs:
**
**	Returns:
**		NO_FORM		User has 'Cancel'd.
**		JDEF_FORM	User wishes to create the form.
**
**	Exceptions:
**		None.
**
** Side Effects:
**	None.
**
** History:
**	26-oct-89 (bruceb)	Written.
**	26-jan-1994 (rdrane)
**		Unnormalize userid as required before calling UTexe() (aspect
**		of b57678).
*/
static i4
cr_f_jdef()
{
	i4	retval = JDEF_FORM;
  char *jdefpik;
  char jname[FE_MAXNAME + 1];
	char	hlpbuf[132+1];
	char	unrml_uid[(FE_UNRML_MAXNAME + 1)];
	allocCJ();
/* # line 516 "vfcrtfrm.qsc" */	/* display */
  {
    if (IIdispfrm(vfcrform_jdef,(char *)"f") == 0) goto IIfdE3;
    goto IImuI3;
IIfdI3:;
IIfdB3:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 519 "vfcrtfrm.qsc" */	/* getform */
          {
            if (IIfsetio(vfcrform_jdef) != 0) {
              IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,jname,(char *)
"jdef_name");
            } /* IIfsetio */
          }
/* # line 521 "vfcrtfrm.qsc" */	/* host code */
	    if (jname[0] == EOS)
	    {
		IIUGerr(E_VF0177_no_jd_name, UG_ERR_ERROR, 0);
/* # line 524 "vfcrtfrm.qsc" */	/* resume */
          {
            if (1) goto IIfdB3;
          }
/* # line 525 "vfcrtfrm.qsc" */	/* host code */
	    }
	    if (!vffnmchk(jname, ERget(FE_JoinDef)))
	    {
/* # line 529 "vfcrtfrm.qsc" */	/* resume */
          {
            if (1) goto IIfdB3;
          }
/* # line 530 "vfcrtfrm.qsc" */	/* host code */
	    }
	    IIUGmsg(ERget(F_VF0081_Create_from_JoinDef), FALSE,
		1, jname);
	    if (!qdefcom(jname))
	    {
		/* Error message has already been sent. */
/* # line 538 "vfcrtfrm.qsc" */	/* resume */
          {
            if (1) goto IIfdB3;
          }
/* # line 539 "vfcrtfrm.qsc" */	/* host code */
	    }
/* # line 540 "vfcrtfrm.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE3;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 545 "vfcrtfrm.qsc" */	/* host code */
                STATUS          rval;           /* return value from UTexe */
                LOCATION        *temp_loc;      /* temporary location */
                i4              mode;           /* batch or interactive mode */
                CL_ERR_DESC     err_code;       /* error status from UTexe */
                char            *cmdline;       /* cmdline for UTexe */
                i4              run_params;     /* # of parameters */
		char   		*name = ERx("qbf"); 
                /*
                ** Get the user specified object name:
                */
/* # line 558 "vfcrtfrm.qsc" */	/* getform */
          {
            if (IIfsetio(vfcrform_jdef) != 0) {
              IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,jname,(char *)
"jdef_name");
            } /* IIfsetio */
          }
/* # line 560 "vfcrtfrm.qsc" */	/* host code */
            	if (jname[0] == EOS)
            	{
                	IIUGerr(E_VF0177_no_jd_name, UG_ERR_ERROR, 0);
/* # line 563 "vfcrtfrm.qsc" */	/* resume */
          {
            if (1) goto IIfdB3;
          }
/* # line 564 "vfcrtfrm.qsc" */	/* host code */
            	}
                mode = UT_WAIT;         /* execute interactive, not batch */
                temp_loc = NULL;        /* no log file */
                cmdline=ERx("database=%S,nogo=%S,user=%S,equel=%S");
                run_params = 4;
/* # line 572 "vfcrtfrm.qsc" */	/* clear */
          {
            IIclrscr();
          }
/* # line 575 "vfcrtfrm.qsc" */	/* host code */
		/*
		** Unnormalize the userid as required.
		*/
		_VOID_ IIUGxri_id(IIUIdbdata()->suser,&unrml_uid[0]);
                rval = UTexe(mode, temp_loc, (ModDictDesc *) NULL,
                        (PTR)NULL, name, &err_code, cmdline,
                        run_params,dbname,jname,&unrml_uid[0],
                        IIxflag());
                /* Check if any errors occurred */
                if (rval != OK)
                {
			i4 err = rval;
			IIUGerr(E_VF002B_ExecutionError, UG_ERR_ERROR, 2, 
				(PTR)name, (PTR)&err);
                }
/* # line 594 "vfcrtfrm.qsc" */	/* clear */
          {
            IIclrscr();
          }
/* # line 596 "vfcrtfrm.qsc" */	/* redisplay */
          {
            IIredisp();
          }
/* # line 597 "vfcrtfrm.qsc" */	/* resume */
          {
            if (1) goto IIfdB3;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 605 "vfcrtfrm.qsc" */	/* host code */
	    retval = NO_FORM;
/* # line 606 "vfcrtfrm.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE3;
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 612 "vfcrtfrm.qsc" */	/* host code */
	    STcopy(ERget(S_VF0157_JoinDefs), hlpbuf);
	    jdefpik = IIUFopObjPik(hlpbuf, VFH_LSJDEF, OC_JOINDEF,
		ERget(F_VF0087_JoinDefs),
		ERget(F_VF0084_Selecting_JDef_names),
		ERget(F_VF0085_JoinDef_Name));
	    if ((jdefpik != NULL) && (*jdefpik != EOS))
	    {
/* # line 619 "vfcrtfrm.qsc" */	/* putform */
          {
            if (IIfsetio(vfcrform_jdef) != 0) {
              IIputfldio((char *)"jdef_name",(short *)0,1,32,0,jdefpik);
            } /* IIfsetio */
          }
/* # line 620 "vfcrtfrm.qsc" */	/* host code */
	    }
        }
      } else if (IIretval() == 5) {
        {
/* # line 627 "vfcrtfrm.qsc" */	/* host code */
	    STcopy(ERget(S_VF0155_Form_from_JoinDef), hlpbuf);
	    FEhelp(VFH_CRJDEF, hlpbuf);
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
    if (IInmuact(ERget(FE_OK),(char *)0,2,2,1) == 0) goto IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE3;
    if (IInmuact(ERget(FE_Edit),ERget(F_VF00C0_EditExpl),2,2,2) == 0) goto 
    IIfdE3;
    if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),0,0,3) == 0) 
    goto IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(9,ERget(F_FE0103_ExplCancel),0,0,3) == 0) goto IIfdE3;
    if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,4) == 0) goto IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(10,(char *)0,0,0,4) == 0) goto IIfdE3;
    if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,5) == 0) 
    goto IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(1,ERget(F_FE0100_ExplFrameHelp),0,0,5) == 0) goto IIfdE3;
    if (IIendmu() == 0) goto IIfdE3;
    goto IIfdI3;
IIfdE3:;
    IIendfrm();
  }
/* # line 631 "vfcrtfrm.qsc" */	/* host code */
	return(retval);
}
