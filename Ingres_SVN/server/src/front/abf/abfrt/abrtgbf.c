# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<adf.h>
#include	<feconfig.h>
#include	<abfcnsts.h>
#include	<fdesc.h>
#include	<abfrts.h>
#include	<rtsdata.h>
/**
** Name:	abrtgbf.qc -	ABF Run-Time System Run a Gbf Frame.
**
** Description:
**	Contains the runtime code that runs a Gbf frame.  Defines:
**
**	IIARgbfGraph()	run a Gbf frame.
**
** History:
**	Revision 5.1  86/08/14  joe
**	Extracted from "abfrts.qc" for PC overlay work.
**
**	Revision 2.0  82/10/29  joe
**	Initial revision.
*/
# ifndef NOGBF
/*{
** Name:	IIARgbfGraph() -	Run a GBF Frame.
**
** Description:
**	Run a GBF frame.  This just amounts to calling Graph.
**
** Input:
**	frm	{ABRTSFRM *}  The run-time frame structure.
**	param	{ABRTSPRM *}  Any parameters passed to the frame; ignored.
**
** Called by:
**	abrtscall()
**
** History:
**	Written 10/29/82 (jrc)
*/
#define _Draw_Expl	F_AR000D_DrawExpl
#define _Help_Expl	F_FE0100_ExplFrameHelp
#define _End_Expl	F_FE0102_ExplEnd
VOID
IIARgbfGraph (frm, param)
ABRTSFRM	*frm;
ABRTSPRM	*param;
{
  {
	register ABRTSGBF	*gbffrm = &(frm->abrfrgbf);
	bool				end;
	for (end = FALSE ; !end ;)
	{
    char *blankname;
		FUNC_EXTERN char *iiar_blankform();
		blankname = iiar_blankform();
/* # line 73 "abrtgbf.qsc" */	/* clear */
    {
      IIclrscr();
    }
/* # line 74 "abrtgbf.qsc" */	/* display */
    {
      if (IIdispfrm(blankname,(char *)"f") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 77 "abrtgbf.qsc" */	/* clear */
            {
              IIclrscr();
            }
/* # line 78 "abrtgbf.qsc" */	/* host code */
			abgbfgraf(IIarDbname, gbffrm->abrgfname, (char *)NULL, (char*)NULL);
			/*
			** Graph waits for the user to push a
			** key before quiting
			*/
/* # line 83 "abrtgbf.qsc" */	/* clear */
            {
              IIclrscr();
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 89 "abrtgbf.qsc" */	/* host code */
			FEhelp(hlpGBFMU, ERget(S_AR0024_RunGraphHelpTitle));
          }
        } else if (IIretval() == 3) {
          {
/* # line 95 "abrtgbf.qsc" */	/* host code */
			end = TRUE;
/* # line 96 "abrtgbf.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(FE_Graph),ERget(_Draw_Expl),2,2,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(_Help_Expl),0,2,2) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,2) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(_End_Expl),0,2,3) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,2,3) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 100 "abrtgbf.qsc" */	/* host code */
	}
	return;
  }
}
# else
static	int	_junk = 0;	/* To shut up the librarian */
# endif /* NOGBF */
