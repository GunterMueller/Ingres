# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	txtcreate.qsc
**
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
/**
** Name:	txtcreate.sc - Create form form catalogs.
**
** Description:
**	Contains routines to create a form from system catalogs.
**	Routines defined:
**	- FDtxtcreate - Create form from system catalogs.
**
** History:
**	04/10/85 (dkh) - Resurrection.
**	02/17/87 (dkh) - Added procedure header.
**	07/14/86 (scl) Removed FTmxcol wide form support;
**		no longer required for CMS in 5.0
**	25-jun-87 (bab)	Code cleanup.
**	08/14/87 (dkh) - ER changes.
**	08/26/87 (dkh) - Changed for 8000 series error numbers.
**	11/17/87 (danielt) SQLized the database access.
**	21-mar-88 (sylviap)
**		Created .qsc.
**	05/27/88 (dkh) - Changed to not increase size of form to width of
**			 terminal since this does not work for popups.
**	06/21/88 (dkh) - Changed to use retrieved values for frposx
**			 and frposy since they may be non-zero for popups.
**	13-oct-88 (sylviap)
**		Code clean up.  Deleted buffers numbuf, colbuf and sizebuf.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
# include	<compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ft.h>
# include	<fmt.h>
# include	<adf.h>
# include	<frame.h>
# include	<frserrno.h>
# include	"fdfuncs.h"
# include	<er.h>
GLOBALREF	i4	fdprform;
/*{
** Name:	FDtxtcreate - Create form from system catalogs.
**
** Description:
**	Build a form from the old (fdframes, fdfields and fdtrim)
**	system catalogs.  Use the passed in name and owner in the
**	catalog lookup.  A user can only access a form owned
**	by him/herself or the DBA.
**
** Inputs:
**	frmid	Name of form to look for (in catalog) and build.
**	wins	Flag to control building of FT specific data
**		structures for form.  FT structures are built
**		if this is TRUE.
**	owner	Owner name to use in catalog lookup.
**
** Outputs:
**	Returns:
**		NULL	If form could not be found in the catalogs.
**		-1	If an error occured while building the
**			form.  Implies form was found in the old catalogs.
**		frm	Pointer to a completely built form.
**	Exceptions:
**		None.
**
** Side Effects:
**	None.
**
** History:
**	04/10/85 (dkh) - Resurrection.
**	02/17/87 (dkh) - Added procedure header.
**	11/17/87 (danielt) SQLized the database access.
*/
FRAME	*
FDtxtcreate(frmid, wins, ooid)
  char *frmid;
  bool wins;
  i4 ooid;
{
	FRAME	*frm;
	TRIM	*trm;
	i4	i = 0;
  i4 maxcol;
  i4 maxlin;
  i4 posy;
  i4 posx;
  i4 fldno;
  i4 nsno;
  i4 trimno;
  i4 flags;
  i4 numflds;
  i4 trimx;
  i4 trimy;
  i4 trmflg;
  i4 trm2flg;
  i4 trmfont;
  i4 trmptsz;
  i4 verno;
  i4 flags2;
  i4 scrtype;
  i4 scrmaxx;
  i4 scrmaxy;
  i4 scrxdpi;
  i4 scrydpi;
  char name[FE_MAXNAME + 1];
  char trimstr[151];
	IIUIcnb_CatNolockBegin(ERx("ii_forms"));
	STcopy(frmid, name);
/* # line 128 "txtcreat.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select frmaxcol, frmaxlin, frposy, frposx, frfldno, frnsno, frtrimno,\
 frtotflds, frflags, fr2flags, frversion, frscrtype, frscrmaxx, frscrm\
axy, frscrdpix, frscrdpiy from ii_forms where object_id=");
    IIputdomio((short *)0,1,30,4,&ooid);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&maxcol);
      IIgetdomio((short *)0,1,30,4,&maxlin);
      IIgetdomio((short *)0,1,30,4,&posy);
      IIgetdomio((short *)0,1,30,4,&posx);
      IIgetdomio((short *)0,1,30,4,&fldno);
      IIgetdomio((short *)0,1,30,4,&nsno);
      IIgetdomio((short *)0,1,30,4,&trimno);
      IIgetdomio((short *)0,1,30,4,&numflds);
      IIgetdomio((short *)0,1,30,4,&flags);
      IIgetdomio((short *)0,1,30,4,&flags2);
      IIgetdomio((short *)0,1,30,4,&verno);
      IIgetdomio((short *)0,1,30,4,&scrtype);
      IIgetdomio((short *)0,1,30,4,&scrmaxx);
      IIgetdomio((short *)0,1,30,4,&scrmaxy);
      IIgetdomio((short *)0,1,30,4,&scrxdpi);
      IIgetdomio((short *)0,1,30,4,&scrydpi);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 165 "txtcreat.sc" */	/* host code */
		i++;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	IIUIcne_CatNolockEnd(ERx("ii_forms"));
	if (IItest_err() || i == 0)
	{
		return(NULL);
	}
	if (i > 1)
	{
		/*
		**  more than one form retrieved.
		*/
		IIFDerror(CFFRMS, 2, &i, frmid);
		return((FRAME *) -1);
	}
	STtrmwhite(name);
	frm = FDnewfrm(name);
	frm->frfldno = fldno;
	frm->frnsno = nsno;
	frm->frtrimno = trimno;
	/*
	**  Code no longer increases the size of the form to the
	**  size of the terminal since this is not right for popups.
	*/
	frm->frmaxx = maxcol;
	frm->frmaxy = maxlin;
	frm->frposx = posx;
	frm->frposy = posy;
	frm->frmflags = flags;
	frm->frmode = fdcmINSRT;
	frm->frversion = verno;
	frm->frm2flags = flags2;
	frm->frscrtype = scrtype;
	frm->frscrxmax = scrmaxx;
	frm->frscrymax = scrmaxy;
	frm->frscrxdpi = scrxdpi;
	frm->frscrydpi = scrydpi;
	if (!FDfralloc(frm))
	{
		return((FRAME *) -1);
	}
	i = 0;
	IIUIcnb_CatNolockBegin(ERx("ii_trim"));
/* # line 208 "txtcreat.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select trim_col, trim_lin, trim_trim, trim_flags, trim2_flags, trim_f\
ont, trim_ptsz from ii_trim where object_id=");
    IIputdomio((short *)0,1,30,4,&ooid);
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&trimx);
      IIgetdomio((short *)0,1,30,4,&trimy);
      IIgetdomio((short *)0,1,32,150,trimstr);
      IIgetdomio((short *)0,1,30,4,&trmflg);
      IIgetdomio((short *)0,1,30,4,&trm2flg);
      IIgetdomio((short *)0,1,30,4,&trmfont);
      IIgetdomio((short *)0,1,30,4,&trmptsz);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 227 "txtcreat.sc" */	/* host code */
		STtrmwhite(trimstr);
		if (i >= frm->frtrimno)
		{
			IIFDerror(CFFRAL, 1, frmid);
			IIUIcne_CatNolockEnd(ERx("ii_trim"));
			return((FRAME *) -1);
		}
		trm = FDnewtrim(trimstr, trimy, trimx);
		trm->trmflags = trmflg;
		trm->trm2flags = trm2flg;
		trm->trmfont = trmfont;
		trm->trmptsz = trmptsz;
		frm->frtrim[i] = trm;
		i++;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
	IIUIcne_CatNolockEnd(ERx("ii_trim"));
	if (i < frm->frtrimno)
	{
		IIFDerror(CFFRAL, 1, frmid);
		return((FRAME *) -1);
	}
	if (!FDtxtfldcre(frmid, ooid, frm, numflds, wins))
	{
		return((FRAME *) -1);
	}
	if (wins)
	{
		FTnewfrm(frm);
	}
	return(frm);
}
