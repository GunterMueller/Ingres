# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<er.h> 
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<adf.h>
#include	<feconfig.h>
#include	<abfcnsts.h>
#include	<fdesc.h>
#include	<abfrts.h>
/**
** Name:    abrtvig.qc -	ABF Run-Time System Run a Vigraph Frame.
**
** Description:
**	Contains the runtime code that runs a Vigraph frame.  Defines:
**
**	IIARvgfGraph()		run a Vigraph frame.
**
** History:
**	Revision 6.4
**	03/22/91 (emerson)
**		Fix interoperability bug 36589:
**		Change all calls to abexeprog to remove the 3rd parameter
**		so that images generated in 6.3/02 will continue to work.
**		(Generated C code may contain calls to abexeprog).
**		This parameter was introduced in 6.3/03/00, but all calls
**		to abexeprog specified it as TRUE.  See abfrt/abrtexe.c
**		for further details.
**
**	Revision 5.1  86/08/14  joe
**	Extracted from "abfrts.qc" for PC overlay work.
**
**	Revision 4.0  86/02/10  peter
**	Initial revision.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
# ifndef NOVIGRAPH
/*{
** Name:    IIARvgfGraph() -	Run a Vigraph Frame.
**
** Description:
**	Run a Vigraph frame.  This just amounts to calling Rungraph.
**
** Input:
**	frm	{ABRTSFRM *}  The run-time frame structure.
**	param	{ABRTSPRM *}  Any parameters passed to the frame; ignored.
**
** Called by:
**	abrtscall()
**
** History:
**	Written 02/10/86 (prs)
**	
**	26-may-89 (kenl)
**		Added code to conditionaly build the FLAGS part of the
**		VIGRAPH command line string.
*/
#define _Draw_Expl	F_AR000D_DrawExpl
#define _Help_Expl	F_FE0100_ExplFrameHelp
#define _End_Expl	F_FE0102_ExplEnd
VOID
IIARvgfGraph (frm, param)
ABRTSFRM	*frm;
ABRTSPRM	*param;
{
  {
    register ABRTSGBF	*gbffrm = &(frm->abrfrgbf);
    bool		end;
    i4			argcnt;
    char		*args[2];
    char		abexestr[256];
    char		combuf[ABCOMSIZE+1];
    for (end = FALSE ; !end ;)
    {
    char *blankname;
	FUNC_EXTERN char *iiar_blankform();
	blankname = iiar_blankform();
/* # line 97 "abrtvig.qsc" */	/* clear */
    {
      IIclrscr();
    }
/* # line 98 "abrtvig.qsc" */	/* display */
    {
      if (IIdispfrm(blankname,(char *)"f") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 103 "abrtvig.qsc" */	/* clear */
            {
              IIclrscr();
            }
/* # line 105 "abrtvig.qsc" */	/* host code */
	    argcnt = 0;
	    args[argcnt] = gbffrm->abrgfname;
	    STcopy(ERx("graph = %S"), abexestr);
	    if (gbffrm->abrgfcomline != NULL && *(gbffrm->abrgfcomline) != EOS)
	    {
		STcopy(gbffrm->abrgfcomline, combuf);
		argcnt++;
		args[argcnt] = combuf;
		STcat(abexestr, ERx(", flags = %S"));
	    }
	    abexeprog(ERx("rungraph"), abexestr,
				++argcnt, args[0], args[1]);
	    /*
	    ** RunGraph waits for the user to push a key before quitting.
	    */
/* # line 122 "abrtvig.qsc" */	/* clear */
            {
              IIclrscr();
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 128 "abrtvig.qsc" */	/* host code */
	    FEhelp(hlpVIGMU, ERget(S_AR0024_RunGraphHelpTitle));
          }
        } else if (IIretval() == 3) {
          {
/* # line 134 "abrtvig.qsc" */	/* host code */
	    end = TRUE;
/* # line 135 "abrtvig.qsc" */	/* breakdisplay */
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
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      goto IIfdE1;
IImuI1:
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Plot),ERget(_Draw_Expl),2,2,1) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(_Help_Expl),0,2,2) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,2) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(_End_Expl),2,2,3) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,2,3) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 139 "abrtvig.qsc" */	/* host code */
    }
    return;
  }
}
# else
static	int	_junk = 0;	/* To shut up the librarian */
# endif /* NOVIGRAPH */
