# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<lo.h>
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	"ervq.h"
/**
** Name:	vqlisfil.qsc	- Display 4GL listing file on full screen
**
** Description:
**	IIVQdlfDisplayListFile	- Display 4GL lising file on full screen
**
** History:
**	11/24/89 (Mike S)	Initial Version
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/* # define's */
/* GLOBALDEF's */
/* extern's */
FUNC_EXTERN VOID	FEhelp();
FUNC_EXTERN VOID	IIUGerr();
/* static's */
static READONLY
  char _form[]= ERx("vqlisfil"), _frame[]= ERx("frame"), _file[]= ERx("file"), 
  _text[]= ERx("text"), _summary[]= ERx("summary"), _texttf[]= ERx("current_error")
  ;
static bool init_done = FALSE;
/*{
** Name:	IIVQdlfDisplayListFile	-Display 4GL listing file on full screen
**
** Description:
**	Give a full screen display of the 4GL listing file in a readonly
**	tablefield.  This is used as an "explosion" of the smaller display
**	on the error display screen.  We load the tablefield in this frame from
**	the one in vqcmperr, and begin by displaying the same portion of the 
**	file.
**
**	We get information from these fields in forms vqcmperr and vqcmper2:
**
**		Field	summary		Name, type, and errors of current frame
**		Field	filename	Name of listing file
**		Tablefield current_error
**		    Column text		Listing file text
**
** Inputs:
**	srcfrm		char []		Form to read from
**					(vqcmperr or vqcmper2)
**	filename	char *		Name of listing file
**	startline	i4		Line to display
**
** Outputs:
**	none
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
**	11/24/89 (Mike S)	Initial version
*/
VOID
IIVQdlfDisplayListFile(srcfrm, filename, startline)
  char *srcfrm;
  char *filename;
  i4 startline;
{
	/* Init form, if need be */
	if (!init_done)
	{
		IIARiaForm(_form);
		init_done = TRUE;
	}
	/* Display form */
/* # line 98 "vqlisfil.qsc" */	/* display */
  {
    if (IIdispfrm(_form,(char *)"r") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
      char buffer[80];
      char summary[78];
      i4 no_lines;
/* # line 105 "vqlisfil.qsc" */	/* host code */
		/* Fill in title */
/* # line 106 "vqlisfil.qsc" */	/* putform */
      {
        if (IIfsetio(_form) != 0) {
          IIputfldio(_file,(short *)0,1,32,0,filename);
        } /* IIfsetio */
      }
/* # line 108 "vqlisfil.qsc" */	/* getform */
      {
        if (IIfsetio(srcfrm) != 0) {
          IIgetfldio((short *)0,1,32,77,summary,_summary);
        } /* IIfsetio */
      }
/* # line 109 "vqlisfil.qsc" */	/* putform */
      {
        if (IIfsetio((char *)0) != 0) {
          IIputfldio(_frame,(short *)0,1,32,0,summary);
        } /* IIfsetio */
      }
/* # line 111 "vqlisfil.qsc" */	/* host code */
		/* Load tablefield */
/* # line 112 "vqlisfil.qsc" */	/* inittable */
      {
        if (IItbinit(_form,_text,(char *)"r") != 0) {
          IItfill();
        } /* IItbinit */
      }
/* # line 113 "vqlisfil.qsc" */	/* unloadtable */
      {
        if (IItbact(srcfrm,_texttf,0) == 0) goto IItbE1;
        while (IItunload() != 0) {
          IItcogetio((short *)0,1,32,79,buffer,_text);
          IITBceColEnd();
          {
/* # line 115 "vqlisfil.qsc" */	/* loadtable */
            {
              if (IItbact(_form,_text,1) != 0) {
                IItcoputio(_text,(short *)0,1,32,0,buffer);
                IITBceColEnd();
              } /* IItbact */
            }
          }
        } /* IItunload */
IItbE1:
        IItunend();
      }
/* # line 118 "vqlisfil.qsc" */	/* host code */
		/* scroll to desired line */
/* # line 119 "vqlisfil.qsc" */	/* inquire_frs */
      {
        if (IIiqset(3,0,_form,(char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,30,4,&no_lines,32,_text,0);
        } /* IIiqset */
      }
/* # line 120 "vqlisfil.qsc" */	/* host code */
		if (startline <= 0 || startline > no_lines)
			startline = 1;
/* # line 122 "vqlisfil.qsc" */	/* scroll */
      {
        if (IItbsetio(1,_form,_text,-3) != 0) {
          IItbsmode((char *)"to");
          if (IItscroll(0,startline) != 0) {
          } /* IItscroll */
        } /* IItbsetio */
      }
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 129 "vqlisfil.qsc" */	/* host code */
		FEhelp(ERx("vqlisfil.hlp"),
		       ERget(S_VQ0028_DisplayListFile));
        }
      } else if (IIretval() == 2) {
        {
/* # line 137 "vqlisfil.qsc" */	/* clear */
          {
            IIfldclear(_text);
          }
/* # line 138 "vqlisfil.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 143 "vqlisfil.qsc" */	/* host code */
                /* top */
/* # line 144 "vqlisfil.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_form,_text,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 149 "vqlisfil.qsc" */	/* host code */
                /* bottom */
          i4 no_rows;
/* # line 152 "vqlisfil.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,_form,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&no_rows,32,_text,0);
            } /* IIiqset */
          }
/* # line 154 "vqlisfil.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_form,_text,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,no_rows) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 159 "vqlisfil.qsc" */	/* host code */
                /* find */
                FEtabfnd(_form, _text);
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
    if (IInmuact(ERget(FE_Help),ERget(F_FE0101_ExplKeyHelp),2,2,1) == 0) goto 
    IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),2,2,2) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,2) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,3) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,4) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,5) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 163 "vqlisfil.qsc" */	/* host code */
	return;
}
