# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<cv.h>
# include	<er.h>
# include	<st.h>
# include	<me.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<stdprmpt.h>
# include <abclass.h>
# include <metafrm.h>
# include       "ervq.h"
# include       "vqescinf.h"	
/**
** Name:	vqedectf.qsc - Edit escape code in tablefield
**
** Description:
**	This file defines:
**
**	IIVQeecEditEscapeCode - Edit escape code in tablefield
**
** History:
**	8/9/89 (Mike S)	Initial version
**	25-aug-92 (blaise)
**		Added support for local procedure escape code (not really
**		escape code, but local procedure source code is edited and
**		stored like escape code).
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/* # define's */
# define UNDEFINED 0
# define NEW	   1
# define UNCHANGED 2
# define CHANGED   3
# define DELETED   4
# define BUFFERSZ 254
/* GLOBALDEF's */
/* GLOBALREF's */
GLOBALREF   APPL	*IIVQappl; /* argument of frame flow entry, the current
			     	   application we are editing in abf */
/* extern's */
FUNC_EXTERN VOID	FEhelp();
FUNC_EXTERN PTR		FEreqmem();
FUNC_EXTERN STATUS	FEtabfnd();
FUNC_EXTERN VOID	IIUFsifSetIntFmt();
FUNC_EXTERN VOID	IIUFsnfSetNormFmt();
FUNC_EXTERN VOID	IIUFtfdTblFldDelete();
FUNC_EXTERN STATUS	IIUFtfeTblFldEdit();
FUNC_EXTERN VOID	IIUFtfiTblFldInsert();
FUNC_EXTERN STATUS	IIUFtflTblFldLoad();
FUNC_EXTERN STATUS	IIUFtfsTblFldString();
FUNC_EXTERN STATUS	IIUFtfuTblFldUnload();
FUNC_EXTERN VOID        IIUGbmaBadMemoryAllocation();
FUNC_EXTERN char	*IIUGfmt();
FUNC_EXTERN STATUS	IIVQsecSaveEscCode();
FUNC_EXTERN VOID	IIVQlgLocGlobEdit();
extern ESCTYPE  vqescs[];               /* Escape type descriptors */
/* static's */
static bool esc_matches();
static MFESC *find_escape_code();
static MFESC *get_escape_code();
static VOID save_escape_code();
static	READONLY 
  char _form[]= 		ERx("vqedectf"), _header[]= 	ERx("header"), _tf[]=   		ERx("vqedectf")
  , _tfcol[]= 	ERx("vqedectf");
static READONLY	char _empty[] 	= ERx("");
static READONLY	char _nl[] 	= ERx("\n");
static READONLY	char routine[] 	= ERx("IIVQeecEditEscapeCode");
static bool init_vqedectf = FALSE;	/* Have we init'ed this form */
/*{
** Name:	IIVQeecEditEscapeCode  - Edit escape code in tablefield
**
** Description:
**	The display loop for editing escape code in a tablefield.
**
** Inputs:
**	escinfo		ESCINFO	*	Decription of escape code to edit
**		metaframe		metaframe to edit
**		esctype			type of escape code to edit
**		item			field activation or menuitem name
**
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
**	Escape code may be added, deleted, or modified in the metaframe.
**
** History:
**	8/9/89 (Mike S)	Initial version
**	14-jan-93 (blaise)
**		Added help for ESC_LOC_PROC escape type; since local procedure
**		source isn't really escape code it needs its own help files.
**	29-jan-93 (blaise)
**		Added a differenct "end without saving changes?..." prompt
**		for local procedures; again the standard one doesn't work
**		because it refers to escape code.
*/
VOID
IIVQeecEditEscapeCode(escinfo)
ESCINFO *escinfo;
{
	MFESC	*mfesc;		/* Escape code structure */
	bool	new;		/* Is this a new piece of escape code */
  char buffer[BUFFERSZ];
  char *msg;
	/* 
	** Find the existing escape code (if any).
	*/
	if ((MFESC *)NULL == (mfesc = get_escape_code(escinfo, &new)))
		return; 
	/* Bring up the form */
	if (!init_vqedectf)
	{
		IIARiaForm(_form);
		init_vqedectf = TRUE;
	}
/* # line 150 "vqedectf.qsc" */	/* display */
  {
    if (IIdispfrm(_form,(char *)"u") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
      i4 startline;
      i4 numlines;
/* # line 156 "vqedectf.qsc" */	/* inittable */
      {
        if (IItbinit(_form,_tf,(char *)"f") != 0) {
          IItfill();
        } /* IItbinit */
      }
/* # line 157 "vqedectf.qsc" */	/* host code */
		/* Format header line */
		_VOID_ IIUGfmt(buffer, BUFFERSZ, 
			       ERget(vqescs[escinfo->esctype].hdr_name),
			       2, ERget(vqescs[escinfo->esctype].esc_name),
			       escinfo->item);
/* # line 162 "vqedectf.qsc" */	/* putform */
      {
        if (IIfsetio((char *)0) != 0) {
          IIputfldio(_header,(short *)0,1,32,0,buffer);
        } /* IIfsetio */
      }
/* # line 164 "vqedectf.qsc" */	/* host code */
		/* Load the tablefield, if there's anything to load */
		if ((mfesc->text != NULL) && (*mfesc->text != EOS))
			_VOID_ IIUFtfsTblFldString(mfesc->text, _form, _tf, 
						   _tfcol);
		else if (escinfo->esctype == ESC_LOC_PROC)
		{
			/*
			** Local procedure escape code: if no code exists, we
			** display the skeleton of the procedure.
			*/
			STprintf(buffer, "PROCEDURE %s() =\nBEGIN\nEND",
					escinfo->item);
			IIUFtfsTblFldString(buffer, _form, _tf, _tfcol);
		}
		/* Scroll the tablefield to the start line */
		if ((startline = escinfo->line) > 1)
		{
/* # line 183 "vqedectf.qsc" */	/* inquire_frs */
      {
        if (IIiqset(3,0,_form,(char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,30,4,&numlines,32,_tf,0);
        } /* IIiqset */
      }
/* # line 184 "vqedectf.qsc" */	/* host code */
			if (startline <= numlines)
/* # line 185 "vqedectf.qsc" */	/* scroll */
      {
        if (IItbsetio(1,_form,_tf,-3) != 0) {
          IItbsmode((char *)"to");
          if (IItscroll(0,startline) != 0) {
          } /* IItscroll */
        } /* IItbsetio */
      }
/* # line 186 "vqedectf.qsc" */	/* host code */
		}
		/* Turn off the change bit */
/* # line 189 "vqedectf.qsc" */	/* set_frs */
      {
        if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIstfsio(20,_form,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        } /* IIiqset */
      }
/* # line 191 "vqedectf.qsc" */	/* host code */
		/* Use internal format for wrapping */
		IIUFsifSetIntFmt();
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 198 "vqedectf.qsc" */	/* host code */
		if (IIUFtfeTblFldEdit(_form, _tf, _tfcol, &msg) != OK)
		{
/* # line 200 "vqedectf.qsc" */	/* message */
          {
            IImessage(msg);
          }
/* # line 201 "vqedectf.qsc" */	/* sleep */
          {
            IIsleep(2);
          }
/* # line 202 "vqedectf.qsc" */	/* host code */
		}
		/* Turn on the change bit */
/* # line 205 "vqedectf.qsc" */	/* set_frs */
          {
            if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(20,_form,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
            } /* IIiqset */
          }
/* # line 207 "vqedectf.qsc" */	/* host code */
	/* 
	** ----WORKAROUND ----
	** we need the redisplay to get the whole stack of active forms to
	** be displayed.  If the forms system is fixed, we can remove it.
	*/
/* # line 212 "vqedectf.qsc" */	/* redisplay */
          {
            IIredisp();
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 218 "vqedectf.qsc" */	/* host code */
                for (;;)
                {
/* # line 220 "vqedectf.qsc" */	/* submenu */
          {
            goto IImuI2;
IIfdB2:
            IIrunmu(0);
            if (IIretval() == 1) {
              {
/* # line 224 "vqedectf.qsc" */	/* host code */
				if (IIUFtflTblFldLoad(_form, _tf, _tfcol, &msg)
					 != OK)
				{
/* # line 227 "vqedectf.qsc" */	/* message */
                {
                  IImessage(msg);
                }
/* # line 228 "vqedectf.qsc" */	/* sleep */
                {
                  IIsleep(2);
                }
/* # line 229 "vqedectf.qsc" */	/* host code */
				}
				/* Turn on the change bit */
/* # line 231 "vqedectf.qsc" */	/* set_frs */
                {
                  if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                    IIstfsio(20,_form,0,(short *)0,1,30,sizeof(1),
                    (void *)IILQint(1));
                  } /* IIiqset */
                }
/* # line 233 "vqedectf.qsc" */	/* host code */
				break;
              }
            } else if (IIretval() == 2) {
              {
/* # line 238 "vqedectf.qsc" */	/* host code */
				bool	dummy;
				if (IIUFtfuTblFldUnload(_form, _tf, _tfcol, 
							&msg, &dummy) != OK)
				{
/* # line 243 "vqedectf.qsc" */	/* message */
                {
                  IImessage(msg);
                }
/* # line 244 "vqedectf.qsc" */	/* sleep */
                {
                  IIsleep(2);
                }
/* # line 245 "vqedectf.qsc" */	/* host code */
				}
				break;
              }
            } else if (IIretval() == 3) {
              {
/* # line 252 "vqedectf.qsc" */	/* host code */
				if (escinfo->esctype == ESC_LOC_PROC)
                                	FEhelp(ERx("vqlpfil.hlp"),
                                        ERget(S_VQ00F2_lpEditFileHelpBanner));
				else
                                	FEhelp(ERx("vqescfil.hlp"),
                                        ERget(S_VQ000D_EscEditFileHelpBanner));
                                continue;
              }
            } else if (IIretval() == 4) {
              {
/* # line 264 "vqedectf.qsc" */	/* host code */
                                break;
              }
            } else {
              if (1) goto IIfdE2;
            } /* IIretval */
            goto IIfdE2;
IImuI2:
            IImuonly();
            if (IIinitmu() == 0) goto IIfdE2;
            if (IInmuact(ERget(FE_ReadFile),ERget(F_FE010D_ReadExpl),2,2,1) ==
             0) goto IIfdE2;
            if (IInmuact(ERget(FE_WriteFile),ERget(F_FE010E_WriteExpl),2,2,2) 
            == 0) goto IIfdE2;
            if (IInmuact(ERget(FE_Help),ERget(F_FE0101_ExplKeyHelp),2,2,3) == 
            0) goto IIfdE2;
            IIFRInternal(0);
            if (IInfrskact(1,(char *)0,2,2,3) == 0) goto IIfdE2;
            if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),2,2,4) == 0) 
            goto IIfdE2;
            IIFRInternal(0);
            if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE2;
            if (IIendmu() == 0) goto IIfdE2;
            goto IIfdB2;
IIfdE2:;
          }
/* # line 266 "vqedectf.qsc" */	/* host code */
                }
        }
      } else if (IIretval() == 3) {
        {
/* # line 273 "vqedectf.qsc" */	/* inittable */
          {
            if (IItbinit(_form,_tf,(char *)"f") != 0) {
              IItfill();
            } /* IItbinit */
          }
/* # line 274 "vqedectf.qsc" */	/* host code */
		/* Turn on the change bit */
/* # line 275 "vqedectf.qsc" */	/* set_frs */
          {
            if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(20,_form,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
            } /* IIiqset */
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 282 "vqedectf.qsc" */	/* host code */
		for (;;)
		{	
/* # line 284 "vqedectf.qsc" */	/* submenu */
          {
            goto IImuI3;
IIfdB3:
            IIrunmu(0);
            if (IIretval() == 1) {
              {
/* # line 288 "vqedectf.qsc" */	/* host code */
				IIUFtfiTblFldInsert(_form, _tf);
				/* Turn on the change bit */
/* # line 290 "vqedectf.qsc" */	/* set_frs */
                {
                  if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                    IIstfsio(20,_form,0,(short *)0,1,30,sizeof(1),
                    (void *)IILQint(1));
                  } /* IIiqset */
                }
/* # line 292 "vqedectf.qsc" */	/* host code */
				break;
              }
            } else if (IIretval() == 2) {
              {
/* # line 298 "vqedectf.qsc" */	/* host code */
				IIUFtfdTblFldDelete(_form, _tf); 
				/* Turn on the change bit */
/* # line 300 "vqedectf.qsc" */	/* set_frs */
                {
                  if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                    IIstfsio(20,_form,0,(short *)0,1,30,sizeof(1),
                    (void *)IILQint(1));
                  } /* IIiqset */
                }
/* # line 302 "vqedectf.qsc" */	/* host code */
				break;
              }
            } else if (IIretval() == 3) {
              {
/* # line 308 "vqedectf.qsc" */	/* host code */
				IIUFtfbTblFldBreak(_form, _tf, _tfcol); 
				/* Turn on the change bit */
/* # line 310 "vqedectf.qsc" */	/* set_frs */
                {
                  if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                    IIstfsio(20,_form,0,(short *)0,1,30,sizeof(1),
                    (void *)IILQint(1));
                  } /* IIiqset */
                }
/* # line 312 "vqedectf.qsc" */	/* host code */
				break;
              }
            } else if (IIretval() == 4) {
              {
/* # line 318 "vqedectf.qsc" */	/* host code */
				IIUFtfjTblFldJoin(_form, _tf, _tfcol); 
				/* Turn on the change bit */
/* # line 320 "vqedectf.qsc" */	/* set_frs */
                {
                  if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                    IIstfsio(20,_form,0,(short *)0,1,30,sizeof(1),
                    (void *)IILQint(1));
                  } /* IIiqset */
                }
/* # line 322 "vqedectf.qsc" */	/* host code */
				break;
              }
            } else if (IIretval() == 5) {
              {
/* # line 329 "vqedectf.qsc" */	/* host code */
				if (escinfo->esctype == ESC_LOC_PROC)
                                	FEhelp(ERx("vqesclin.hlp"),
                                        ERget(S_VQ00F3_lpEditLineHelpBanner));
				else
                                	FEhelp(ERx("vqesclin.hlp"),
                                        ERget(S_VQ000E_EscEditLineHelpBanner));
                                continue;
              }
            } else if (IIretval() == 6) {
              {
/* # line 341 "vqedectf.qsc" */	/* host code */
                                break;
              }
            } else {
              if (1) goto IIfdE3;
            } /* IIretval */
            goto IIfdE3;
IImuI3:
            IImuonly();
            if (IIinitmu() == 0) goto IIfdE3;
            if (IInmuact(ERget(FE_InsLine),ERget(F_FE0110_InsertExpl),2,2,1) 
            == 0) goto IIfdE3;
            if (IInmuact(ERget(FE_DelLine),ERget(F_FE0111_DeleteExpl),2,2,2) 
            == 0) goto IIfdE3;
            if (IInmuact(ERget(FE_SplitLine),ERget(F_FE0113_SplitExpl),2,2,3) 
            == 0) goto IIfdE3;
            if (IInmuact(ERget(FE_JoinLines),ERget(F_FE0112_JoinExpl),2,2,4) 
            == 0) goto IIfdE3;
            if (IInmuact(ERget(FE_Help),ERget(F_FE0101_ExplKeyHelp),2,2,5) == 
            0) goto IIfdE3;
            IIFRInternal(0);
            if (IInfrskact(1,(char *)0,2,2,5) == 0) goto IIfdE3;
            if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),2,2,6) == 0) 
            goto IIfdE3;
            IIFRInternal(0);
            if (IInfrskact(3,(char *)0,2,2,6) == 0) goto IIfdE3;
            if (IIendmu() == 0) goto IIfdE3;
            goto IIfdB3;
IIfdE3:;
          }
/* # line 343 "vqedectf.qsc" */	/* host code */
                }
        }
      } else if (IIretval() == 5) {
        {
/* # line 350 "vqedectf.qsc" */	/* host code */
		IIVQlgLocGlobEdit(IIVQappl, NULL, escinfo->metaframe->apobj);
        }
      } else if (IIretval() == 6) {
        {
/* # line 356 "vqedectf.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_form,_tf,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 7) {
        {
/* # line 362 "vqedectf.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_form,_tf,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,-1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 8) {
        {
/* # line 368 "vqedectf.qsc" */	/* host code */
		FEtabfnd(_form, _tf);
        }
      } else if (IIretval() == 9) {
        {
/* # line 375 "vqedectf.qsc" */	/* host code */
		/* Save it, and mark that it's now unchanged */
		save_escape_code(escinfo, mfesc, &new);
        }
      } else if (IIretval() == 10) {
        {
/* # line 383 "vqedectf.qsc" */	/* host code */
		if (escinfo->esctype == ESC_LOC_PROC)
			FEhelp(ERx("vqedlptf.hlp"),
				ERget(S_VQ00F1_lpEditSrcHelpBanner));
		else
			FEhelp(ERx("vqedectf.hlp"),
				ERget(S_VQ0007_EscEditHelpBanner));
        }
      } else if (IIretval() == 11) {
        {
          i4 cbit;
/* # line 396 "vqedectf.qsc" */	/* host code */
		i4 choice;
		/* See if it has changed */
/* # line 399 "vqedectf.qsc" */	/* inquire_frs */
          {
            if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&cbit,20,_form,0);
            } /* IIiqset */
          }
/* # line 400 "vqedectf.qsc" */	/* host code */
		if (1 == cbit)
		{
			/* See what the user want to do */
			if (escinfo->esctype == ESC_LOC_PROC)
			{
				choice = IIUFccConfirmChoice(
					CONF_END, (char *)NULL, 
					ERget(F_VQ0151_eop_locprc), 
					ERget(F_VQ0168_EndLocProcEdit), 
					ERx("vqlpeend.hlp"));
			}
			else
			{
				choice = IIUFccConfirmChoice(
					CONF_END, (char *)NULL, 
					ERget(F_VQ00B0_eop_esccod), 
					ERget(F_VQ00F3_EndEscEdit), 
					ERx("vqeceend.hlp"));
			}
			switch (choice)
			{
			    case CONFCH_YES:
				/* Save, then end */
				save_escape_code(escinfo, mfesc, &new);
				break;
			    case CONFCH_NO:
				/* Just end */
				break;
			    case CONFCH_CANCEL:
				/* Don't do anything */
/* # line 432 "vqedectf.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 433 "vqedectf.qsc" */	/* host code */
			}
		}
		/* End */
/* # line 437 "vqedectf.qsc" */	/* clear */
          {
            IIfldclear(_tf);
          }
/* # line 438 "vqedectf.qsc" */	/* breakdisplay */
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
    if (IInmuact(ERget(FE_Edit),ERget(F_FE0115_EditExpl),2,2,1) == 0) goto 
    IIfdE1;
    if (IInmuact(ERget(FE_File),ERget(F_FE010C_FileExpl),2,2,2) == 0) goto 
    IIfdE1;
    if (IInmuact(ERget(FE_Blank),ERget(F_FE0114_BlankExpl),2,2,3) == 0) goto 
    IIfdE1;
    if (IInmuact(ERget(FE_LineEdit),ERget(F_FE010F_LineEditExpl),2,2,4) == 0) 
    goto IIfdE1;
    if (IInmuact(ERget(F_VQ00A8_vqloc_glob_menu),
    ERget(F_VQ00A9_vqloc_glob_expl),0,0,5) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,6) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,7) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,8) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Save),ERget(FE_SaveExpl),2,2,9) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(8,(char *)0,2,2,9) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(F_FE0101_ExplKeyHelp),2,2,10) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,10) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),2,2,11) == 0) goto 
    IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,11) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 441 "vqedectf.qsc" */	/* host code */
	/* Revert to normal wrap format */
	IIUFsnfSetNormFmt();
}
/*
**	Check whether an escape code structure matches the
**	one in the escape code editing structure.
*/
static bool 
esc_matches(escinfo, mfesc)
ESCINFO	*escinfo;	/* Escape code editing info */
MFESC	*mfesc;		/* Escape code struct */
{
	if (mfesc->type == escinfo->esctype)
	{
		switch (escinfo->esctype)
		{
		    /* These require a further match */
		    case ESC_MENU_START:
		    case ESC_MENU_END:
		    case ESC_LOC_PROC:
			if (STcompare(mfesc->item, escinfo->item) == 0)
				return (TRUE);
			break;
		    /*
		    ** These require a case-insensitive match, since we 
		    ** uppercase the "ALL" activation names in escinfo.
		    */
		    case ESC_FLD_ENTRY:
		    case ESC_FLD_CHANGE:
		    case ESC_FLD_EXIT:
			if (STbcompare(mfesc->item, 0, escinfo->item, 0, TRUE)
				== 0)
				return (TRUE);
			break;
		     default:
			/* These don't  require an item match */
			return(TRUE);
			break;
		}
	}
	return (FALSE);
}
/* Save escape code to the database */
static VOID
save_escape_code(escinfo, mfesc, new)
ESCINFO		*escinfo;/* Escape code information */
MFESC		*mfesc;	/* Escape code structure */
bool		*new;	/* (in/out) 
			** in: Must this be added to metaframe 
			** out: Is this in the metaframe after the save
			*/
  {
	METAFRAME *mf = escinfo->metaframe;	/* Metaframe we're saving */
    char buffer[BUFFERSZ];
	bool nonempty;
	i4 size = 0;
	i4 blanks;
    i4 state;
	char *ptr;
    i4 cbit;
	i4  i;
	/* 
	** Calculate size of escape code.  We will delete leading 
	** and trailing blank lines.
	*/
/* # line 511 "vqedectf.qsc" */	/* inquire_frs */
    {
      if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&cbit,20,_form,0);
      } /* IIiqset */
    }
/* # line 512 "vqedectf.qsc" */	/* host code */
	{
		/* If it changed, read the string */
		blanks = 0;
		nonempty = FALSE;
/* # line 516 "vqedectf.qsc" */	/* unloadtable */
    {
      if (IItbact(_form,_tf,0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,BUFFERSZ-1,buffer,_tfcol);
        IItcogetio((short *)0,1,30,4,&state,(char *)"_state");
        IITBceColEnd();
        {
/* # line 518 "vqedectf.qsc" */	/* host code */
			switch (state)
			{
			    /* An empty row */
			    case UNDEFINED:
				if (nonempty) blanks++;
				break;
			    case NEW:
			    case UNCHANGED:
			    case CHANGED:
				STtrmwhite(buffer);
				if (*buffer == EOS)
				{
					if (nonempty) blanks++;
				}
				else
				{
					size += STlength(buffer) + 1 + blanks;
					blanks = 0;
					nonempty = TRUE;
				}
				break;
				}
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
/* # line 542 "vqedectf.qsc" */	/* host code */
		size++;
		/* Allocate new string space, if we have to */
		if (size > STlength(mfesc->text))
		{
			mfesc->text = (char *)FEreqmem(
						mf->ptag[MF_PT_ESC],
						(u_i4)size,
						FALSE, NULL);
			if (mfesc->text == NULL)
			{
				IIUGbmaBadMemoryAllocation(routine);
			}
		}
		/* Copy into the text string */
		ptr = mfesc->text;
		nonempty = FALSE;
		blanks = 0;
/* # line 561 "vqedectf.qsc" */	/* unloadtable */
    {
      if (IItbact(_form,_tf,0) == 0) goto IItbE2;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,32,BUFFERSZ-1,buffer,_tfcol);
        IItcogetio((short *)0,1,30,4,&state,(char *)"_state");
        IITBceColEnd();
        {
/* # line 563 "vqedectf.qsc" */	/* host code */
			switch (state)
			{
			    /* An empty row */
			    case UNDEFINED:
				if (nonempty) blanks++;
				break;
			    case NEW:
			    case UNCHANGED:
			    case CHANGED:
				STtrmwhite(buffer);
				if (*buffer == EOS)
				{
					if (nonempty) blanks++;
				}
				else
				{
					for (i = 0; i < blanks; i++)
					{
						STcopy(_nl, ptr);
						ptr += sizeof(_nl) - 1;
					}
					STpolycat(2, buffer, _nl, ptr);
					ptr += STlength(ptr);
					blanks = 0;
					nonempty = TRUE;
				}
				break;
			}
        }
      } /* IItunload */
IItbE2:
      IItunend();
    }
/* # line 593 "vqedectf.qsc" */	/* host code */
		*ptr = EOS;
	}
	/* Insert it, if need be */
	if (*new)
	{
		/* 
		** We checked above that this fits in the "escs" array.
		** Currently, this array is always created at its maximum
		** size.  If this changes, we'll have to allocate a new
		** pointer array here.
		*/
		mf->escs[mf->numescs++] = mfesc;
	}
	/* Save them */
	mf->updmask |= MF_P_ESC;
	_VOID_ IIVQsecSaveEscCode(mf, FALSE);
	/* Reset change bit */
/* # line 613 "vqedectf.qsc" */	/* set_frs */
    {
      if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(20,_form,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 615 "vqedectf.qsc" */	/* host code */
	/* 
	** Now try to find it again.  We may have added it, deleted it, or
	** neither.  The mfesc structure is still valid, in any case.
	*/
	*new = (NULL == find_escape_code(escinfo));	
  }
/*
**	Find the escape code we want to edit, or create a new piece.
**	
**	Returns		NULL	If it couldn't be created
**			Pointer to escape code structure
*/
static MFESC *
get_escape_code(escinfo, new)
ESCINFO	*escinfo;	/* Description of escape code we're looking for */
bool	*new;		/* (output) TRUE if we had to create it */
{
	STATUS	status;
	MFESC	*mfesc;
	METAFRAME *metaframe = escinfo->metaframe;
	/* Try to find it */
	if (NULL != (mfesc = find_escape_code(escinfo)))
	{
		*new = FALSE;
		return (mfesc);
	}
	/* If we didn't find it, try to create a new MFESC */
	if (metaframe->numescs >= MF_MAXESCS)
	{
		IIUGerr(E_VQ005D_TooManyEscapes, 0, 1, 
			&metaframe->numescs);
		return(NULL);
	}
	mfesc = (MFESC *)FEreqmem(metaframe->ptag[MF_PT_ESC],
				       (u_i4)sizeof(MFESC), 
				       FALSE, &status);
	if (mfesc == NULL)
	{
		IIUGbmaBadMemoryAllocation(routine);
	}
	mfesc->type = escinfo->esctype;
	if (*escinfo->item != EOS)
	{
		mfesc->item = STtalloc(metaframe->ptag[MF_PT_ESC], 
				       escinfo->item);
		/* Make field activation names lower case */
		if ((mfesc->type == ESC_FLD_ENTRY) ||
		    (mfesc->type == ESC_FLD_CHANGE) ||
		    (mfesc->type == ESC_FLD_EXIT))
			CVlower(mfesc->item);
	}
	else
	{
		mfesc->item = _empty;
	}
	mfesc->text = _empty;
	*new = TRUE;
	return (mfesc);
}
/* 
** See if the escape code is currently in the metaframe
*/
static MFESC *
find_escape_code(escinfo)
ESCINFO	*escinfo;
{
	i4	i;
	MFESC	*mfesc;
	METAFRAME *metaframe = escinfo->metaframe;
	register MFESC	**escs = metaframe->escs;
	/* Try to find it */
	for (i = 0; i < metaframe->numescs; i++)
	{
		if (esc_matches(escinfo, escs[i]))
		{
			mfesc = escs[i];
			return (mfesc);
		}
	}
	/* It's not there */
	return (NULL);
}
