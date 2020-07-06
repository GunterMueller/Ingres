# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/* 
**	Copyright (c) 2004 Ingres Corporation  
*/
#include	<compat.h> 
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h> 
#include 	<er.h>
#include 	<adf.h>
#include 	<ft.h>
#include 	<fmt.h>
#include 	<frame.h>
#include 	<uf.h>
#include 	<ug.h>
#include 	<ooclass.h>
#include 	<abclass.h>
#include 	<stdprmpt.h>
# include <metafrm.h>
/*
#include	<metafrm.h>
  ..so ming knows about the dependancy 
*/
#include	"ervq.h"
/**
** Name:	vqcomprs -	compressed view of visual query
**
** Description:
**	Process the compressed view of the visual query.
**	
**
**	This file defines:
**		IIVQdcvDisplayCompView - display compressed view
** History:
**	12/27/89  (tom) - extracted from vqdloop.qsc 
**	08/18/91 (dkh) - Fixed bug 38752.  Put in some workarounds to
**			 make things work for the compressed view of
**			 a vision visual query.  The real fix should
**			 be to change vision to create more sociable forms.
**	09/10/93 (kchin) 
**		Changed type of cvq_nat to PTR in IIVQdcvDisplayCompView(), 
**		also  changed the cast of cvq to PTR when assigns it to
**		cvq_nat. This variable is holding pointer.  Cast to 
**		i4 will result in truncation of 64-bit address.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
FUNC_EXTERN PTR  IIFRkeystroke();
FUNC_EXTERN FRAME *IIVQvmfVqMakeFrame();
  GLOBALREF char *IIVQcform;
  GLOBALREF char *IIVQccCompCursor;
static i4   vq_KeyCapture();
static i4  _command;		/* last command seen by key capture function */ 
/*{
** Name:	IIVQdcvDisplayCompView	- display the compressed view 
**
** Description:
**	This function displays the compressed view of the visual
**	query. This view depicts only the table names.. and not
**	the column information.
**
** Inputs:
**	METAFRAME *mf;
**
** Outputs:
**	Returns:
**		bool	TRUE - if success
**			FALSE - if allocation error
**	
**	Exceptions:
**
** Side Effects:
**
** History:
**	04/06/89	- tom created
*/
i4  
IIVQdcvDisplayCompView(mf, curtab)
METAFRAME *mf;
i4  curtab;
{
	FRAME *cvq;
	i4 done;
  PTR cvq_nat;
	PTR old_keyfunc;
	FT_TERMINFO terminfo;
	/* get terminal information */
	FTterminfo(&terminfo);
	for (done = FALSE; done == FALSE; )
	{
		/* create a dynamic frame which is a compressed view */
		cvq = IIVQvmfVqMakeFrame(mf, TRUE, curtab);
		if (cvq->frmaxy < terminfo.lines)
			cvq->frmaxy = terminfo.lines;
		if (cvq->frmaxx < terminfo.cols)
			cvq->frmaxx = terminfo.cols;
		/*
		**  Mark this form as a special vision compressed visual
		**  query form.  This tells the forms system to do special
		**  tricks/hacks to make large compressed view forms scroll
		**  and display correctly.  The real fix to the problem
		**  is to change the forms that are generated here so that
		**  the special hacks in the forms system are not needed.
		**  Since that is even tricker, the forms hacks will have
		**  to do for now.
		**
		**  WARNING: THIS LEADS TO BOTTOMLESS PITS IF THE BEHAVIOR
		**  OF THIS CODE CHANGES WITHOUT COORDINATING WITH THE
		**  FORMS SYSTEM OWNER.  OF COURSE, THIS CAN BE ELIMINATED
		**  IF WE FIX UP THE FORMS THAT ARE GENERATED HERE.
		*/
		cvq->frmflags |= fdVIS_CMPVW;
		cvq_nat = (PTR) cvq;
/* # line 132 "vqcomprs.qsc" */	/* addform */
  {
    IIaddform(cvq_nat);
  }
/* # line 134 "vqcomprs.qsc" */	/* display */
  {
    if (IIdispfrm(IIVQcform,(char *)"f") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 138 "vqcomprs.qsc" */	/* host code */
			old_keyfunc = IIFRkeystroke(vq_KeyCapture);
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 143 "vqcomprs.qsc" */	/* host code */
			if (_command == fdopUP || _command == fdopPREV)
			{ 
				if (curtab)
					curtab--;
				else 
					curtab = mf->numtabs - 1;
			}
			else /* the only other possibility is fdopDOWN */
			{
			    NEXT_TABLE:
				if (curtab < mf->numtabs - 1)
					curtab++;
				else
					curtab = 0;
			}
/* # line 159 "vqcomprs.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 164 "vqcomprs.qsc" */	/* host code */
			goto NEXT_TABLE;
        }
      } else if (IIretval() == 3) {
        {
/* # line 169 "vqcomprs.qsc" */	/* host code */
			FEhelp(ERx("vqcompre.hlp"), 
				ERget(F_VQ00BA_compre_view));
        }
      } else if (IIretval() == 4) {
        {
/* # line 175 "vqcomprs.qsc" */	/* host code */
			done = TRUE;
/* # line 176 "vqcomprs.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 180 "vqcomprs.qsc" */	/* resume */
          {
            IIresmu();
            if (1) goto IIfdB1;
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
    IIFRInternal(0);
    if (IInfrskact(0,(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(ERget(F_VQ002D_NextTable),ERget(F_VQ0061_NextTableExpl),2,2,2)
     == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),(char *)0,2,2,3) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,ERget(F_VQ006D_HelpExpl),2,2,3) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_End),(char *)0,2,2,4) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,ERget(F_VQ006E_EndExpl),2,2,4) == 0) goto IIfdE1;
    if (IIFRafActFld(IIVQccCompCursor,1,5) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 184 "vqcomprs.qsc" */	/* host code */
		/* restore the old key intercept function */
		_VOID_ IIFRkeystroke(old_keyfunc);
		/* tell the form system to free the dynamically created
		   form that we have created. this removes the form 
		   from the FRS's lists, and frees the memory associated
		   with it */
		IIdelfrm(IIVQcform);
	}
	return (curtab);
}
/*{
** Name:	vq_KeyCapture	- process keystroke capture
**
** Description:
**	This function is called upon each keystroke which is 
**	decoded by the form system. It is this function's 
**	responsibility to return a value to the FRS to indicate
**	what action it is to take with the keystroke.
**	This routine is used by the VQ compressed view frame
**	and is only interested in UP and DOWN keystrokes.
**
** Inputs:
**	i4 command;		- fdop commands
**	i4 key_type;		- basic keystroke type
**	char *char_buf;		- character buffer of the actual keystrokes
**	
**
** Outputs:
**	Returns:
**		i4	- indicator to the form system as to how it is
**			  to handle the processing of the keystroke.
**				
**	Exceptions:
**
** Side Effects:
**
** History:
**	08/22/89 (tom) - created
*/
/*ARGSUSED*/
static i4  
vq_KeyCapture(command, key_type, char_buf)
i4  command;
i4  key_type;
char *char_buf;
{
	/* check the command to see if it is one we are interested in */
	switch (command)
	{
	case fdopNEXT:
	case fdopPREV:
	case fdopUP:
	case fdopDOWN:
		/* it is interesting, so remember the command */
		_command = command;
		/* tell the form system to return control to FRSkey0 */
		return (FRS_RETURN);
	}
	/* tell the form system to do whatever it would normally do */
	return (FRS_PROCESS);
}
