# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	valerr.qsc
*/
/*
** Handle validation/derivation syntax errors at save time.
**
** Copyright (c) 2004 Ingres Corporation
**
** History:
**	02/04/85 (drh)	Added calls to vfdmpon and vfdmpoff in integration
**			of bug fixes from vms.
**	03/23/87 (dkh) -Added support for ADTs.
**	08/04/87 (dkh) - Changed to use compiled forms in a file.
**	08/14/87 (dkh) - ER changes.
**	10/02/87 (dkh) - Help file changes.
**	11-jan-88 (sylviap)
**		Created the .qsc file.
**	11/01/88 (dkh) - Performance changes.
**	21-jun-89 (bruceb)
**		Changed to handle derivation formulas as well.
**		Also, added Forget menuitem.
**	01/24/90 (dkh) - Moved include of er.h ahead of uf.h due to changes
**			 in uf.h.
**	12/05/90 (dkh) - Fixed code to handle screen updates correctly
**			 if an error occurs during save sequence.
**	12/05/90 (dkh) - Fixed typo: changed GLOBALDEF to GLOBALREF for
**			 IIVFneedclr.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
# include	<compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<si.h>
# include	<er.h>
# include	<uf.h>
# include	"decls.h"
# include	<ex.h>
# include	"ervf.h"
# include	"vfhlp.h"
GLOBALREF	bool	IIVFneedclr;
static	bool	valerr_alloc = FALSE;
VOID
allocvalerr()
{
	if (!valerr_alloc)
	{
		if (IIUFgtfGetForm(IIUFlcfLocateForm(), ERx("valerr")) != OK)
		{
			syserr(ERget(S_VF00F8_Can_not_access_form__));
		}
		valerr_alloc = TRUE;
	}
}
/*
** writeout is true if frame is being written out
** only allow changes to validation string
*/
i4
valerrCom(hdr, type, err)
FLDHDR	*hdr;
FLDTYPE *type;
char	*err;
{
  char *cp;
	char	buf[BUFSIZ];
	i4	forget = FALSE;
	/*
	** first fill in the attributes from the field
	** structure and then drive the form
	** when control comes back fill in the field
	*/
	vfdmpon();
	IIVFneedclr = TRUE;
/* # line 92 "valerr.qsc" */	/* display */
  {
    if (IIdispfrm(ERx("valerr"),(char *)"u") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 95 "valerr.qsc" */	/* host code */
		if (hdr->fhd2flags & fdDERIVED)
		{
			cp = ERget(F_VF0071_Der_Form);
/* # line 98 "valerr.qsc" */	/* putform */
      {
        if (IIfsetio((char *)"valerr") != 0) {
          IIputfldio((char *)"valstrtitle",(short *)0,1,32,0,cp);
          IIputfldio((char *)"error_type",(short *)0,1,32,0,cp);
        } /* IIfsetio */
      }
/* # line 99 "valerr.qsc" */	/* host code */
		}
		else
		{
			cp = ERget(F_VF0070_Val_Str);
/* # line 103 "valerr.qsc" */	/* putform */
      {
        if (IIfsetio((char *)"valerr") != 0) {
          IIputfldio((char *)"valstrtitle",(short *)0,1,32,0,cp);
          IIputfldio((char *)"error_type",(short *)0,1,32,0,cp);
        } /* IIfsetio */
      }
/* # line 104 "valerr.qsc" */	/* host code */
		}
		cp = hdr->fhdname;
/* # line 107 "valerr.qsc" */	/* putform */
      {
        if (IIfsetio(ERx("valerr")) != 0) {
          IIputfldio((char *)"name",(short *)0,1,32,0,cp);
        } /* IIfsetio */
      }
/* # line 109 "valerr.qsc" */	/* host code */
		cp = err;
/* # line 110 "valerr.qsc" */	/* putform */
      {
        if (IIfsetio(ERx("valerr")) != 0) {
          IIputfldio((char *)"error",(short *)0,1,32,0,cp);
        } /* IIfsetio */
      }
/* # line 112 "valerr.qsc" */	/* host code */
		cp = type->ftvalstr;
/* # line 113 "valerr.qsc" */	/* putform */
      {
        if (IIfsetio(ERx("valerr")) != 0) {
          IIputfldio((char *)"check",(short *)0,1,32,0,cp);
        } /* IIfsetio */
      }
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 118 "valerr.qsc" */	/* host code */
		forget = TRUE;	/* Don't continue the 'save'. */
/* # line 119 "valerr.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 123 "valerr.qsc" */	/* host code */
		if (hdr->fhd2flags & fdDERIVED)
		{
		    STcopy(ERget(S_VF0138_Correcting_Derivation), buf);
		    FEhelp(VFH_DERERR, buf);
		}
		else
		{
		    STcopy(ERget(S_VF00F9_Correcting_Validation), buf);
		    FEhelp(VFH_VALERR, buf);
		}
        }
      } else if (IIretval() == 3) {
        {
/* # line 136 "valerr.qsc" */	/* host code */
		cp = buf;
/* # line 137 "valerr.qsc" */	/* getform */
          {
            if (IIfsetio(ERx("valerr")) != 0) {
              IIgetfldio((short *)0,1,32,0,cp,(char *)"check");
            } /* IIfsetio */
          }
/* # line 138 "valerr.qsc" */	/* host code */
		type->ftvalstr = saveStr(cp);
		if ((hdr->fhd2flags & fdDERIVED) && (type->ftvalstr[0] == EOS))
		{
		    /*
		    ** User has blanked out the derivation formula.  Turn
		    ** off the derived and displayonly flags.
		    */
		    hdr->fhd2flags &= ~(fdDERIVED | fdREADONLY);
		}
/* # line 147 "valerr.qsc" */	/* breakdisplay */
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
    if (IInmuact(ERget(FE_Forget),(char *)0,0,2,1) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,0,2,1) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),(char *)0,0,2,2) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,2,2) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_End),(char *)0,2,2,3) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 149 "valerr.qsc" */	/* host code */
	FTclear();
	vfdmpoff();
	if (forget)
	{
		/* Enddisplay iisave if necessary. */
		IIpopfrm(ERx("vfcatalog"));
		IIVTlf(frame, TRUE, FALSE);
		/* Longjump back to getFrmName(). */
		EXsignal(EXVFLJMP, 0);
	}
}
