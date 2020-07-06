# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<ut.h>
#include	<si.h>
#include	<lo.h>
#include	<me.h>
#include	<nm.h>
#include	<st.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ug.h>
#include	<uf.h>
#include	<ft.h>
# include	<adf.h>
# include	<fmt.h>
# include	<frame.h>
#include	<menu.h>
#include	<help.h>
#include	"helphlp.h"
#include	"eruf.h"
/**
** Name:	fehframe.qsc	- help on a frame, and help utilities.
**
** Description:
**	This contains the help for a frame, plus the utility help
**	routines used by the help/keys and help/values (i.e. help
**	on help).  This file defines:
**
**	FEhframe()	- help for a frame.
**	FEhhelp()	- help on help.
**
** History:
**	Revision 4.0  01-sep-1985 (grant)
**	Initial revision.
**	08/14/87 (dkh) - ER changes.
**	08/17/87 (peter) - add call to FEhlpnam.
**	09/05/87 (dkh) - Changes for international formatting.
**	10/1/87 (daveb) -- remove erronerous declarations of CL
**		routines conflicting with correct decls. in CLentry.h
**	10/05/87 (dkh) - Help file changes.
**	11/12/87 (dkh) - Fixed so that Help-WhatToDo-Help works.
**	07/21/88 (dkh) - Added more hooks to make "help" work in vifred/rbf.
**	03/22/89 (dkh) - Fixed venus bug 4776.
**	27-nov-89 (bruceb) - Find/top/bottom no longer visible menuitems.
**	07/18/89 (skw) - Added "subtopics" support.
**	11-dec-89 (bruceb)
**		Added frskey_expls param to FEhframe to compensate for
**		addition of subtopics support.
**	16-jan-90 (bruceb)
**		Improved scroll code.
**	12/29/90 (dkh) - Fixed bug 31661, reinstated "Help" for
**			 subtopics and added support for help to be recursive.
**	03/01/91 (dkh) - Fixed bug 36208.
**	04/16/91 (dkh) - Fixed bug 37003.
**	07/05/91 (dkh) - Fixed bug 37359.
**	11/13/91 (leighb) - alloc sbtopics and mapinfo (instead of on stack)
**	20-jan-93 (blaise)
**		Increased MAXTOPIC (maximum number of subtopics) from 20 to 30.
**	04/06/93 (dkh) - Fixed bug 47214.  Code was passing a FRAME pointer
**			 rather than the name of the form to delete.
**	04/28/93 (dkh) - Fixed bug 50625.  Removed use of mapinfo in FEhhelp()
**			 since it was never set.  This prevents garbage from
**			 being used in set_frs statements
**	06/13/93 (dkh) - Added code to look for subtopic help files in
**			 the directory of the main help file in addition
**			 to the current directory and $II_CONFIG/$II_LANGUAGE.
**			 Look up order is now: 1) any path that was specified
**			 with the filename; 2) if only a filename was
**			 specified, try directory of main help file and 3) if
**			 only a filename was specified, try
**			 $II_CONFIG/$IILANGUAGE.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
# define	TITLELEN    80
# define	TABLEWIDTH  78
# define	REPLYWIDTH 132
# define	ENDITALL    47
# define	MAXSTACK    10
# define	MAXTOPIC    30	/* Maximum number of subtopics */
# define	MAXTOKEN     4
bool	FEhframe();
VOID	FEhhelp();
FUNC_EXTERN	i4	IIFDlpListPick();
FUNC_EXTERN	STATUS	IIUGhlpname();
FUNC_EXTERN	STATUS  LOwhat();
FUNC_EXTERN	i4	RTispopup();
FUNC_EXTERN	FRAME	*IIFDofOpenFrame();
FUNC_EXTERN	bool	IIFDcfCloseFrame();
FUNC_EXTERN	REGFLD	*IIFDacfAddCharFld();
FUNC_EXTERN	TBLFLD	*IIFDatfAddTblFld();
FUNC_EXTERN	FLDCOL	*IIFDaccAddCharCol();
static i4	_loadFile();
static VOID	_saveKeys();
static VOID	_restoreKeys();
static VOID	_setKeys();
static
  char _HlpTblFld[]= ERx("helptable");
  typedef struct map_info {
    char upmap[20];
    char downmap[20];
    char menu1map[20];
    char menu2map[20];
    char menu3map[20];
    char menu4map[20];
    char menu5map[20];
    char uplabel[50];
    char downlabel[50];
    char menu1label[50];
    char menu2label[50];
    char menu3label[50];
    char menu4label[50];
    char menu5label[50];
  } MAPINFO;
typedef struct sub_topic_info
{
	char	_subtopics[MAXTOPIC][TITLELEN+1]; /* related topic names */
	char	_subfnames[MAXTOPIC][MAX_LOC+1];  /* related topic files */
	char	_substring[MAXTOPIC*(TITLELEN+1)+1]; /* listpick format  */
} SUBTOPIC_INFO;
static	i4	frmcount = 0;
/*{
** Name:	FEhframe() -	Print Help on a Frame.
**
** Description:
**	Print help on a frame, in a table field, with menu items for
**	scrolling, finding a word and help on the help menu itself.
**	This is called in response to the HELPFILE EQUEL command,
**	and for the HELP/FRAME combination in the HELP_FRS EQUEL
**	command.
**
** Inputs:
**	flnm	name of text file containing help text.
**	subj	a name to be used at the top of the help screen, or
**		printed as a message if the file is not found.
**	hasmenu TRUE if you want to put up the help submenu, or
**		FALSE if you want to simply display the help file
**		and return.
**	menu	Menu on which to display information if Help Keys is called.
**	frs_lbls	Information about frs commands for Help Keys.
**	frskey_expls	Information about frskeys active for the calling
**			display loop or submenu.
**	vfrfiq	Indicator of source of call on FEhframe.
**
** Returns:
**	{bool}	TRUE if successful.
**
** History:
**	01-sep-1985 (grant)	Written.
**	30-jan-1986 (peter)	changed for code review.
**	19-jan-87 (bruceb) Fix for bug 9120
**		When editing a new help file (II_HELP_EDIT) is on,
**		and 'Edit' has been selected), if user quits without
**		saving the help file, catch this gracefully (AV's aren't
**		suitable.)  Check return status when trying to open up
**		the file so as to avoid reading should the SIopen fail.
**	11/13/91 (leighb) - alloc sbtopics and mapinfo (instead of on stack)
**	02/12/1992 (wong) - Remove magic form building constants.
**      23-nov-1993 (smc)
**		Bug #58882
**          	Made frmaddr a portable PTR rather than i4.
*/
bool
FEhframe ( flnm, subj, hasmenu, menu, frs_lbls, frskey_expls, vfrfiq )
char	*flnm;				/* file to retrieve message from */
char	*subj;				/* subject of message */
bool	hasmenu;
MENU	menu;
char	*frs_lbls[];
char	*frskey_expls[];
i4	vfrfiq;
  {
	FILE	*fp;			/* pointer to help file */
	FRAME	*frm;
	char	*editname;
	i4	offset;			/* offset to scroll menuitems   */
	TBLFLD	*tbl;
	REGFLD	*rf;
    PTR frmaddr;
    i4 currrec;
    i4 numrecs;
    i4 numrows;
    char *nextmenu;
    char *prevmenu;
    char *editmenu;
    char *relmenu;
    char *prevtopic;
    char *keyhelp;
    char frmname[FE_MAXNAME + 1];
    char buff[TABLEWIDTH+1];
	LOCATION	loc;
	char		filename[MAX_LOC+1];	/* filename buffer */
	char		dev[MAX_LOC+1];		/* device buffer   */
	char		path[MAX_LOC+1];	/* path buffer	   */
	char		dummy[MAX_LOC+1];	/* dummy buffer    */
	char		subject[TITLELEN+1];	/* subject buffer */
	bool		file_opened;
	LOCATION	path_loc;
	char		help_path[MAX_LOC + 1];
	LOCATION	*ptr_to_path_loc = NULL;
/* Declarations to handle recursion */
	SUBTOPIC_INFO	*sbtopics;
	i4	_stackidx;
	i4	_nsubtopic;
	char	_rfilename[MAXSTACK][MAX_LOC+1];  /* filename stack */
	char	_rsubject[MAXSTACK][TITLELEN+1];  /* subject stack */
	MAPINFO	*mapinfo;
/*
**	Initialize help frame stack pointer
*/
	if (flnm == NULL || subj == NULL)
		return FALSE;
/*
**	Allocate memory for sbtopics & mapinfo structures.
**	Pad allocation with sizeof(ALIGN_RESTRICT) for alignment restrictions.
*/
	if ( (sbtopics =
		(SUBTOPIC_INFO *)MEreqmem(0,
					sizeof(SUBTOPIC_INFO) + sizeof(MAPINFO)
						    + sizeof(ALIGN_RESTRICT),
					FALSE, (STATUS *)NULL)) == NULL )
	{
		return FALSE;
	}
	mapinfo = (MAPINFO *)ME_ALIGN_MACRO(
				(PTR)sbtopics + sizeof(SUBTOPIC_INFO),
				sizeof(ALIGN_RESTRICT)
	);
	_VOID_ STlcopy(flnm, filename, sizeof(filename)-1);
	_VOID_ STlcopy(subj, subject, sizeof(subject)-1);
	/*
	**  Determine if initial help file has a leading path.
	**  If so, save this off so that we can prepend it
	**  subtopic filenames, if necessary.
	*/
	(void) STcopy(filename, help_path);
	(void) STtrmwhite(help_path);
	(void) LOfroms(FILENAME & PATH, help_path, &path_loc);
	(void) LOdetail(&path_loc, dev, path, dummy, dummy, dummy);
	if (dev[0] != EOS || path[0] != EOS)
	{
		(void) LOcompose(dev, path, NULL, NULL, NULL, &path_loc);
		ptr_to_path_loc = &path_loc;
	}
	editmenu = NULL;
#ifndef	PCINGRES
	NMgtAt(ERx("II_HELP_EDIT"), &editname);
	if ( editname != NULL && *editname != NULL )
	{ /* Edit in effect */
		editmenu = ERget(FE_Edit);
	}
#endif	/* PCINGRES */
	keyhelp = ( vfrfiq != H_EQ ) ? ERget(F_UF000D_Keys) : NULL;
	/*
	**  Save current key mapping and label information.
	*/
	_VOID_ _saveKeys(mapinfo);
	_stackidx = 0;
	do
	{
/*
**	Save filename and subject onto help stack.
*/
	_VOID_ STtrmwhite(filename);
	_VOID_ STtrmwhite(subject);
	_VOID_ STlcopy(	filename,
			&_rfilename[_stackidx][0],
			sizeof(filename)-1
	);
	_VOID_ STlcopy(	subject,
			&_rsubject[_stackidx][0],
			sizeof(subject)-1
	);
        /*
	**  See if a path specification is part of the filename.  If not,
	**  then file is stored in standard Ingres help file location
	**  and that path needs to be added to the filename.  Otherwise,
	**  the help file path was either added above, or the help file
	**  is a user file that has the path specified.
	*/
	file_opened = FALSE;
	LOfroms(FILENAME & PATH, filename, &loc);
	if (SIopen(&loc, ERx("r"), &fp) != OK)
	{
		LOdetail(&loc, dev, path, dummy, dummy, dummy);
		if (EOS == *dev && EOS == *path)
		{
			/*
			**  Next add path from main help file, if any,
			**  to see if subtopic file can be found there.
			*/
			if (ptr_to_path_loc != NULL)
			{
				LOcopy(ptr_to_path_loc, filename, &loc);
				LOfstfile(&_rfilename[_stackidx][0], &loc);
				if (SIopen(&loc, ERx("r"), &fp) == OK)
				{
					file_opened = TRUE;
				}
			}
			if (!file_opened)
			{
				IIUGhlpname(&_rfilename[_stackidx][0],
					filename);
				LOfroms(FILENAME & PATH, filename, &loc);
			}
		}
		if (!file_opened)
		{
			if (SIopen(&loc, ERx("r"), &fp) != OK)
			{
				subject[ENDITALL] = EOS;
				_VOID_ STprintf(buff,
					ERget(S_UF0044_Sorry_No_help_avail),
					subject);
				_nsubtopic = 0;
			}
			else
			{
				file_opened = TRUE;
			}
		}
	}
	else
	{
		file_opened = TRUE;
	}
	/*
	**  Generate the form to be used.
	**  Note that the code here does not handle
	**  recursion well, if at all.  The state
	**  information held in statics must be
	**  made local to this routine to handle recursion.
	*/
	(VOID) STprintf(frmname, ERx("hframe%d"), frmcount++);
	if ((frm = IIFDofOpenFrame(frmname)) == NULL)
	{
		IIUGerr(E_UF0026_BadForm, UG_ERR_FATAL, 1, frmname);
	}
/* # line 384 "fehframe.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&numrecs,45,(char *)0,0);
        IIiqfsio((short *)0,1,30,4,&numrows,44,(char *)0,0);
      } /* IIiqset */
    }
/* # line 386 "fehframe.qsc" */	/* host code */
	/*
	**  Add fields.
	*/
	tbl = IIFDatfAddTblFld( frm, ERx("helptable"),
				/* row = */ 1, 0, numrows - 5,
				(bool) FALSE, (bool) FALSE
	);
	/*
	**  Turn off row highlighting.
	*/
	tbl->tfhdr.fhdflags &= ~fdROWHLIT;
	(VOID) IIFDaccAddCharCol( frm, tbl, ERx("text"), ERx(""),
					numrecs - 2, 1, numrecs - 2, fd1COLOR
	);
	rf = IIFDacfAddCharFld( frm, ERx("title"), 0, 0, ERx(""), 0, 0,
					numrecs, 1, numrecs, fd2COLOR
	);
	/*
	**  Make title display only so user can't move the cursor there.
	*/
	rf->flhdr.fhd2flags |= fdREADONLY;
	if (IIFDcfCloseFrame(frm) == FALSE)
	{
		IIUGerr(E_UF0026_BadForm, UG_ERR_FATAL, 1, frmname);
	}
	frmaddr = (PTR) frm;
/* # line 419 "fehframe.qsc" */	/* addform */
    {
      IIaddform(frmaddr);
    }
/* # line 421 "fehframe.qsc" */	/* host code */
	if (file_opened)
	{
		_nsubtopic = _loadFile(frmname, fp, subject, sbtopics);
		if ( _stackidx == MAXSTACK - 1 && _nsubtopic > 0 )
		{
			register i4	stki;
			/*
			** Make sure stack doesn't overflow and cause problems;
			** shift stack and drop off oldest item.
			*/
			for ( stki = MAXSTACK ; --stki > 0 ; )
			{
				_VOID_ STlcopy( &_rfilename[stki][0],
						&_rfilename[stki-1][0],
						sizeof(filename)-1
				);
				_VOID_ STlcopy( &_rsubject[stki][0],
						&_rsubject[stki-1][0],
						sizeof(filename)-1
				);
			}
			--_stackidx;
		}
		SIclose(fp);	/* close up file */
	}
	else
	{
/* # line 450 "fehframe.qsc" */	/* putform */
    {
      if (IIfsetio(frmname) != 0) {
        IIputfldio((char *)"title",(short *)0,1,32,0,buff);
      } /* IIfsetio */
    }
/* # line 451 "fehframe.qsc" */	/* inittable */
    {
      if (IItbinit(frmname,_HlpTblFld,(char *)"f") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 452 "fehframe.qsc" */	/* host code */
	}
	/* This section of code is used only if hasmenu is false.
	** THIS IS ONLY CALLED BY THE HELP/HELP SEQUENCE, where we
	** want to display the screen of help text without a menu
	** and the help will only fit on one screen.  Because the
	** forms system does not support this very well, special
	** calls are made to IIqbfclr to first set, and then unset
	** a logical used in IIendfrm of runtime.  This logical
	** tells the system not to clear the screen at the end of
	** a display loop, so that the help form is retained on
	** the screen.
	*/
	if (!hasmenu)
	{
/* # line 468 "fehframe.qsc" */	/* display */
    {
      if (IIdispfrm(frmname,(char *)"r") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 471 "fehframe.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(FE_End),(char *)0,2,2,1) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 473 "fehframe.qsc" */	/* host code */
		if (!RTispopup(NULL))
		{
/* # line 475 "fehframe.qsc" */	/* clear */
    {
      IIclrscr();
    }
/* # line 476 "fehframe.qsc" */	/* host code */
		}
/* # line 477 "fehframe.qsc" */	/* redisplay */
    {
      IIredisp();
    }
/* # line 479 "fehframe.qsc" */	/* host code */
		/*
		**  Free up form allocated above.
		*/
		IIdelfrm(frm->frname);
		MEfree(sbtopics);
		return TRUE;
	}
	offset = keyhelp == NULL ? 0 : 1;
	if (_nsubtopic > 0)
	{ /* Related topics present */
		relmenu = ERget(F_UF0055_SubTopics);
		++offset;
	}
	else
	{ /* No related topics */
		relmenu = NULL;
	}
	if (_stackidx > 0)
	{ /* Previous topic available */
		prevtopic = ERget(F_UF0056_PrevTopic);
		++offset;
	}
	else
	{ /* No previous topic */
		prevtopic = NULL;
	}
/* # line 509 "fehframe.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,frmname,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&numrecs,32,_HlpTblFld,0);
        IIiqfsio((short *)0,1,30,4,&numrows,30,_HlpTblFld,0);
      } /* IIiqset */
    }
/* # line 511 "fehframe.qsc" */	/* host code */
	if (numrecs > numrows)
	{ /* More data than shown on one screen add top and bottom */
		nextmenu = ERget(FE_NextPage);
		prevmenu = ERget(FE_PrevPage);
		_VOID_ _setKeys(offset, mapinfo);
	}
	else
	{ /* All fits on one screen */
		nextmenu = NULL;
		prevmenu = NULL;
	}
/* # line 523 "fehframe.qsc" */	/* display */
    {
      if (IIdispfrm(frmname,(char *)"r") == 0) goto IIfdE2;
      goto IImuI2;
IIfdI2:;
IIfdB2:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
            char bigbuf[2*TABLEWIDTH+1];
/* # line 528 "fehframe.qsc" */	/* host code */
		_VOID_ STprintf(bigbuf, ERget(S_UF0040_HELP_Command_Key),
				subject);
		if (STlength(bigbuf) > TABLEWIDTH)
			_VOID_ STcopy(ERget(S_UF0048_HELP_Command_Key), bigbuf);
		_VOID_ _restoreKeys(mapinfo);
		FEhkeys(bigbuf, menu, frs_lbls, frskey_expls, vfrfiq);
		if (numrecs > numrows)
		{
		    _VOID_ _setKeys(offset, mapinfo);
		}
          }
        } else if (IIretval() == 2) {
          {
/* # line 541 "fehframe.qsc" */	/* host code */
		i4	numsel;		/* index to listpick selection  */
		_VOID_ _restoreKeys(mapinfo);
		numsel = IIFDlpListPick( ERget(F_UF0057_ChooseTopic),
					 sbtopics->_substring,
					 15,
					 2,
					 2,
					 ERget(F_UF0058_ChooseTopicHelp),
					 ERx("frsubtop.hlp"),
					 (char *)NULL,
					 (char *)NULL);
		if (numsel >= 0)
		{
			++_stackidx;
			_VOID_ STlcopy(	&sbtopics->_subfnames[numsel][0],
					filename,
					sizeof(filename)-1
			);
			_VOID_ STlcopy(	&sbtopics->_subtopics[numsel][0],
					buff,
					sizeof(subject)-1
			);
		        _VOID_ STprintf(subject,
					ERget(S_UF003F_HELP_sub),
					buff
			);
/* # line 568 "fehframe.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
/* # line 569 "fehframe.qsc" */	/* host code */
		}
		if (numrecs > numrows)
		{
		    _VOID_ _setKeys(offset, mapinfo);
		}
          }
        } else if (IIretval() == 3) {
          {
/* # line 577 "fehframe.qsc" */	/* inittable */
            {
              if (IItbinit(frmname,_HlpTblFld,(char *)"r") != 0) {
                IItfill();
              } /* IItbinit */
            }
/* # line 578 "fehframe.qsc" */	/* host code */
		if ( --_stackidx >= 0 )
		{
			_VOID_ STlcopy(	&_rfilename[_stackidx][0],
					filename,
					sizeof(filename)-1
			);
			_VOID_ STlcopy(	&_rsubject[_stackidx][0],
					subject,
					sizeof(subject)-1
			);
		}
/* # line 589 "fehframe.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 593 "fehframe.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,frmname,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&numrecs,32,_HlpTblFld,0);
                IIiqfsio((short *)0,1,30,4,&numrows,30,_HlpTblFld,0);
              } /* IIiqset */
            }
/* # line 597 "fehframe.qsc" */	/* getrow */
            {
              if (IItbsetio(2,frmname,_HlpTblFld,1) != 0) {
                IItcogetio((short *)0,1,30,4,&currrec,(char *)"_RECORD");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 598 "fehframe.qsc" */	/* host code */
		currrec = min(currrec + 2 * (numrows - 1), numrecs);
/* # line 599 "fehframe.qsc" */	/* scroll */
            {
              if (IItbsetio(1,frmname,_HlpTblFld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,currrec) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 603 "fehframe.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,frmname,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&numrows,30,_HlpTblFld,0);
              } /* IIiqset */
            }
/* # line 604 "fehframe.qsc" */	/* getrow */
            {
              if (IItbsetio(2,frmname,_HlpTblFld,1) != 0) {
                IItcogetio((short *)0,1,30,4,&currrec,(char *)"_RECORD");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 605 "fehframe.qsc" */	/* host code */
		currrec = max(1, currrec - (numrows - 1));
/* # line 606 "fehframe.qsc" */	/* scroll */
            {
              if (IItbsetio(1,frmname,_HlpTblFld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,currrec) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 610 "fehframe.qsc" */	/* host code */
#ifndef	PCINGRES
		STATUS	retstat;
		/* Only if HELPEDIT defined */
		FTrestore(FT_NORMAL);
		if ( (retstat = UTedit((char *)NULL, &loc)) != OK )
		{
			i4	longstat = retstat;
			_VOID_ STprintf(buff, ERget(S_UF0045_Sorry_Cannot_edit),
					filename, &longstat
			);
			FTrestore(FT_FORMS);
/* # line 623 "fehframe.qsc" */	/* putform */
            {
              if (IIfsetio(frmname) != 0) {
                IIputfldio((char *)"title",(short *)0,1,32,0,buff);
              } /* IIfsetio */
            }
/* # line 624 "fehframe.qsc" */	/* inittable */
            {
              if (IItbinit(frmname,_HlpTblFld,(char *)"f") != 0) {
                IItfill();
              } /* IItbinit */
            }
/* # line 625 "fehframe.qsc" */	/* host code */
		}
		else
		{
			FTrestore(FT_FORMS);
/* # line 629 "fehframe.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
/* # line 630 "fehframe.qsc" */	/* host code */
		}
/* # line 631 "fehframe.qsc" */	/* clear */
            {
              IIclrscr();
            }
/* # line 632 "fehframe.qsc" */	/* host code */
#endif	/* PCINGRES */
          }
        } else if (IIretval() == 7) {
          {
/* # line 636 "fehframe.qsc" */	/* host code */
		/* Implement the Find command. */
		FEtabfnd(frmname, _HlpTblFld);
          }
        } else if (IIretval() == 8) {
          {
/* # line 641 "fehframe.qsc" */	/* host code */
		/* Implement the Top command. */
/* # line 642 "fehframe.qsc" */	/* scroll */
            {
              if (IItbsetio(1,frmname,_HlpTblFld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 9) {
          {
/* # line 646 "fehframe.qsc" */	/* host code */
		/* Implement the Bottom command. */
/* # line 647 "fehframe.qsc" */	/* scroll */
            {
              if (IItbsetio(1,frmname,_HlpTblFld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 10) {
          {
/* # line 651 "fehframe.qsc" */	/* host code */
		STcopy(ERget(S_UF0046_HELP_Main_Help), buff);
		if (numrecs > numrows)
		{
			_VOID_ _restoreKeys(mapinfo);
		}
		FEhhelp(HLPFRM_HLP, buff);
		if (numrecs > numrows)
		{
		    _VOID_ _setKeys(offset, mapinfo);
		}
          }
        } else if (IIretval() == 11) {
          {
/* # line 666 "fehframe.qsc" */	/* inittable */
            {
              if (IItbinit(frmname,_HlpTblFld,(char *)"r") != 0) {
                IItfill();
              } /* IItbinit */
            }
/* # line 667 "fehframe.qsc" */	/* host code */
		_stackidx = -1;
/* # line 668 "fehframe.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
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
      if (IInmuact(keyhelp,(char *)0,2,2,1) == 0) goto IIfdE2;
      if (IInmuact(relmenu,(char *)0,2,2,2) == 0) goto IIfdE2;
      if (IInmuact(prevtopic,(char *)0,2,2,3) == 0) goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,2,2,3) == 0) goto IIfdE2;
      if (IInmuact(nextmenu,(char *)0,2,2,4) == 0) goto IIfdE2;
      if (IInmuact(prevmenu,(char *)0,2,2,5) == 0) goto IIfdE2;
      if (IInmuact(editmenu,(char *)0,2,2,6) == 0) goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,7) == 0) goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,8) == 0) goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,9) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_Help),(char *)0,2,2,10) == 0) goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,2,2,10) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_End),(char *)0,2,2,11) == 0) goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,11) == 0) goto IIfdE2;
      if (IIendmu() == 0) goto IIfdE2;
      goto IIfdI2;
IIfdE2:;
      IIendfrm();
    }
/* # line 671 "fehframe.qsc" */	/* host code */
	/*
	** If still nested, display new/previous topic.
	*/
	} while ( _stackidx >= 0 );
	/*
	**  Restore original key mappings.
	*/
	_VOID_ _restoreKeys(mapinfo);
	/*
	**  Free up form allocated at beginning of this routine.
	*/
	IIdelfrm(frm->frname);
	MEfree(sbtopics);
	return TRUE;
  }
/*{
** Name:	FEhhelp() -	Help on Help Frame.
**
** Description:
**	Gives help on help form for frames.
**
** Inputs:
**	filename	{char *}  name of file containing help on help text.
**	subject		{char *}  name of subject to use at top of form.
**
** History:
**	01-sep-1985 (grant)	Written.
**	17-aug-a987 (peter)	Change to call FEhlpnam.
**	11/13/91 (leighb) - alloc sbtopics and mapinfo (instead of on stack)
**	02/14/1992 (wong) - Remove magic form building constants.
**      23-nov-1993 (smc)
**		Bug #58882
**          	Made frmaddr a portable PTR rather than i4.
*/
VOID
FEhhelp ( filename, subject )
char	*filename;
char	*subject;
  {
	FILE		*fp;		/* pointer to help file */
    i4 numrecs, numrows;
    i4 currrec;
    char *nextmenu;
    char *prevmenu;
    char xupmap[20];
    char xdownmap[20];
    char xmenu1map[20];
    char xmenu2map[20];
    char xuplabel[50];
    char xdownlabel[50];
    char xmenu1label[50];
    char xmenu2label[50];
    char frmname[FE_MAXNAME + 1];
	FRAME		*frm;
    PTR frmaddr;
	TBLFLD		*tbl;
	REGFLD		*rf;
	SUBTOPIC_INFO	*sbtopics;
	i4		_nsubtopic;
	LOCATION	loc;
	char		nam_buf[MAX_LOC+1];
	if ( IIUGhlpname(filename, nam_buf) != OK )
	{
		return;
	}
/*
**	Allocate memory for sbtopics structures.
**	Pad allocation with sizeof(ALIGN_RESTRICT) for alignment restrictions.
*/
	if ( (sbtopics =
		(SUBTOPIC_INFO *)MEreqmem(0,
					sizeof(SUBTOPIC_INFO) + sizeof(MAPINFO)
						+ sizeof(ALIGN_RESTRICT),
					FALSE, (STATUS *)NULL)) == NULL )
	{
		return;
	}
	/*
	**  Generate the form to be used.
	**  This should allow this routine to
	**  be called recursively.
	*/
	(VOID) STprintf(frmname, ERx("h2frame%d"), frmcount++);
	if ((frm = IIFDofOpenFrame(frmname)) == NULL)
	{
		IIUGerr(E_UF0026_BadForm, UG_ERR_FATAL, 1, frmname);
	}
/* # line 768 "fehframe.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&numrecs,45,(char *)0,0);
        IIiqfsio((short *)0,1,30,4,&numrows,44,(char *)0,0);
      } /* IIiqset */
    }
/* # line 770 "fehframe.qsc" */	/* host code */
	/*
	**  Add fields.
	*/
	tbl = IIFDatfAddTblFld( frm, ERx("helptable"),
				/*row = */1, 0, numrows - 5,
				(bool) FALSE, (bool) FALSE
	);
	/*
	**  Turn off row highlighting.
	*/
	tbl->tfhdr.fhdflags &= ~fdROWHLIT;
	(VOID) IIFDaccAddCharCol( frm, tbl, ERx("text"), ERx(""),
					numrecs - 2, 1, numrecs - 2, fd1COLOR
	);
	rf = IIFDacfAddCharFld( frm, ERx("title"), 0, 0, ERx(""), 0, 0,
					numrecs, 1, numrecs, fd2COLOR
	);
	/*
	**  Make title display only so user can't move the cursor there.
	*/
	rf->flhdr.fhd2flags |= fdREADONLY;
	if (IIFDcfCloseFrame(frm) == FALSE)
	{
		IIUGerr(E_UF0026_BadForm, UG_ERR_FATAL, 1, frmname);
	}
	frmaddr = (PTR) frm;
/* # line 804 "fehframe.qsc" */	/* addform */
    {
      IIaddform(frmaddr);
    }
/* # line 806 "fehframe.qsc" */	/* host code */
	LOfroms(PATH & FILENAME, nam_buf, &loc);
	if (SIopen(&loc, ERx("r"), &fp) != OK)
    {
      char buf[TABLEWIDTH+1];
		subject[ENDITALL] = EOS;
		_VOID_ STprintf(buf, ERget(S_UF0044_Sorry_No_help_avail),
				subject
		);
/* # line 815 "fehframe.qsc" */	/* putform */
      {
        if (IIfsetio(frmname) != 0) {
          IIputfldio((char *)"title",(short *)0,1,32,0,buf);
        } /* IIfsetio */
      }
/* # line 816 "fehframe.qsc" */	/* inittable */
      {
        if (IItbinit(frmname,_HlpTblFld,(char *)"f") != 0) {
          IItfill();
        } /* IItbinit */
      }
    }
/* # line 818 "fehframe.qsc" */	/* host code */
	else
	{
		_nsubtopic = _loadFile( frmname, fp, subject, sbtopics);
		SIclose(fp);
	}
/* # line 824 "fehframe.qsc" */	/* inquire_frs */
    {
      if (IIiqset(3,0,frmname,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&numrecs,32,_HlpTblFld,0);
        IIiqfsio((short *)0,1,30,4,&numrows,30,_HlpTblFld,0);
      } /* IIiqset */
    }
/* # line 828 "fehframe.qsc" */	/* host code */
	if (numrecs > numrows)
	{ /* More data than shown on one screen add top and bottom */
		nextmenu = ERget(FE_NextPage);
		prevmenu = ERget(FE_PrevPage);
/* # line 832 "fehframe.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,19,xupmap,8,(char *)0,2017);
        IIiqfsio((short *)0,1,32,19,xdownmap,8,(char *)0,2018);
        IIiqfsio((short *)0,1,32,19,xmenu1map,8,(char *)0,3000);
        IIiqfsio((short *)0,1,32,19,xmenu2map,8,(char *)0,3001);
        IIiqfsio((short *)0,1,32,49,xuplabel,9,(char *)0,2017);
        IIiqfsio((short *)0,1,32,49,xdownlabel,9,(char *)0,2018);
        IIiqfsio((short *)0,1,32,49,xmenu1label,9,(char *)0,3000);
        IIiqfsio((short *)0,1,32,49,xmenu2label,9,(char *)0,3001);
      } /* IIiqset */
    }
/* # line 841 "fehframe.qsc" */	/* host code */
		if (xupmap[0] != EOS)
		{
/* # line 843 "fehframe.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3000,(short *)0,1,32,0,xupmap);
        IIstfsio(9,(char *)0,3000,(short *)0,1,32,0,xuplabel);
      } /* IIiqset */
    }
/* # line 845 "fehframe.qsc" */	/* host code */
		}
		if (xdownmap[0] != EOS)
		{
/* # line 848 "fehframe.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3001,(short *)0,1,32,0,xdownmap);
        IIstfsio(9,(char *)0,3001,(short *)0,1,32,0,xdownlabel);
      } /* IIiqset */
    }
/* # line 850 "fehframe.qsc" */	/* host code */
		}
	}
	else
	{	/* All fits on one screen */
		nextmenu = NULL;
		prevmenu = NULL;
	}
/* # line 858 "fehframe.qsc" */	/* display */
    {
      if (IIdispfrm(frmname,(char *)"r") == 0) goto IIfdE3;
      goto IImuI3;
IIfdI3:;
IIfdB3:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 861 "fehframe.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,frmname,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&numrecs,32,_HlpTblFld,0);
                IIiqfsio((short *)0,1,30,4,&numrows,30,_HlpTblFld,0);
              } /* IIiqset */
            }
/* # line 865 "fehframe.qsc" */	/* getrow */
            {
              if (IItbsetio(2,frmname,_HlpTblFld,1) != 0) {
                IItcogetio((short *)0,1,30,4,&currrec,(char *)"_RECORD");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 866 "fehframe.qsc" */	/* host code */
		currrec = min(currrec+2*(numrows-1), numrecs);
/* # line 867 "fehframe.qsc" */	/* scroll */
            {
              if (IItbsetio(1,frmname,_HlpTblFld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,currrec) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 871 "fehframe.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,frmname,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&numrows,30,_HlpTblFld,0);
              } /* IIiqset */
            }
/* # line 872 "fehframe.qsc" */	/* getrow */
            {
              if (IItbsetio(2,frmname,_HlpTblFld,1) != 0) {
                IItcogetio((short *)0,1,30,4,&currrec,(char *)"_RECORD");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 873 "fehframe.qsc" */	/* host code */
		currrec = max(1, currrec - (numrows - 1));
/* # line 874 "fehframe.qsc" */	/* scroll */
            {
              if (IItbsetio(1,frmname,_HlpTblFld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,currrec) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 878 "fehframe.qsc" */	/* host code */
		/* Implement the Find command. */
		FEtabfnd(frmname, _HlpTblFld);
          }
        } else if (IIretval() == 4) {
          {
/* # line 883 "fehframe.qsc" */	/* host code */
		/* Implement the Top command. */
/* # line 884 "fehframe.qsc" */	/* scroll */
            {
              if (IItbsetio(1,frmname,_HlpTblFld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 888 "fehframe.qsc" */	/* host code */
		/* Implement the Bottom command. */
/* # line 889 "fehframe.qsc" */	/* scroll */
            {
              if (IItbsetio(1,frmname,_HlpTblFld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 893 "fehframe.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE3;
            }
          }
        } else {
          if (1) goto IIfdE3;
        } /* IIretval */
      } /* IIrunform */
IIfdF3:
      if (IIchkfrm() == 0) goto IIfdB3;
      if (IIfsetio((char *)0) == 0) goto IIfdE3;
      goto IIfdE3;
IImuI3:
      if (IIinitmu() == 0) goto IIfdE3;
      if (IInmuact(nextmenu,(char *)0,2,2,1) == 0) goto IIfdE3;
      if (IInmuact(prevmenu,(char *)0,2,2,2) == 0) goto IIfdE3;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,3) == 0) goto IIfdE3;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,4) == 0) goto IIfdE3;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,5) == 0) goto IIfdE3;
      if (IInmuact(ERget(FE_End),(char *)0,2,2,6) == 0) goto IIfdE3;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,6) == 0) goto IIfdE3;
      if (IIendmu() == 0) goto IIfdE3;
      goto IIfdI3;
IIfdE3:;
      IIendfrm();
    }
/* # line 898 "fehframe.qsc" */	/* host code */
	if (numrecs > numrows)
	{
		if (xupmap[0] != EOS)
		{
/* # line 902 "fehframe.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,2017,(short *)0,1,32,0,xupmap);
        IIstfsio(9,(char *)0,2017,(short *)0,1,32,0,xuplabel);
      } /* IIiqset */
    }
/* # line 904 "fehframe.qsc" */	/* host code */
		}
		if (xmenu1map[0] != EOS)
		{
/* # line 907 "fehframe.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3000,(short *)0,1,32,0,xmenu1map);
        IIstfsio(9,(char *)0,3000,(short *)0,1,32,0,xmenu1label);
      } /* IIiqset */
    }
/* # line 909 "fehframe.qsc" */	/* host code */
		}
		if (xdownmap[0] != EOS)
		{
/* # line 912 "fehframe.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,2018,(short *)0,1,32,0,xdownmap);
        IIstfsio(9,(char *)0,2018,(short *)0,1,32,0,xdownlabel);
      } /* IIiqset */
    }
/* # line 914 "fehframe.qsc" */	/* host code */
		}
		if (xmenu2map[0] != EOS)
		{
/* # line 917 "fehframe.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3001,(short *)0,1,32,0,xmenu2map);
        IIstfsio(9,(char *)0,3001,(short *)0,1,32,0,xmenu2label);
      } /* IIiqset */
    }
/* # line 919 "fehframe.qsc" */	/* host code */
		}
	}
/* # line 922 "fehframe.qsc" */	/* inittable */
    {
      if (IItbinit(frmname,_HlpTblFld,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 924 "fehframe.qsc" */	/* host code */
	/*
	**  Free up form allocated above.
	*/
	IIdelfrm(frm->frname);
	MEfree(sbtopics);
  }
/*{
** Name:	_loadFile() -	Load a help table field with file.
**
** Description:
**	Load up a help form table field with the contents of a file.
**
** Inputs:
**	_form	{char *}  Name of help form.
**	fp	{FILE *}  FILE pointer for help file text.
**	subject	{char *}  Subject string for top of form.
**
** History:
**	01-sep-1985 (grant)	Written.
**	18-jul-1989 (skw)	Add support for handling subtopics
**	01-apr-1993 (swm)	Removed forward declaration of STindex(), this
**				is now inherited from <st.h>.
*/
static i4
  _loadFile(_form, fp, subject, stopics)
  char *_form;
FILE		*fp;
char		*subject;
SUBTOPIC_INFO	*stopics;
  {
	i4	numrel = 0;		/* number of related topics found */
	char	*names[MAXTOKEN];	/* tokens from directives */
	bool	anylines = FALSE;
	bool	directive;		/* indicates known directive found */
    char buffer[512];
    char heading[TITLELEN+1];
/* # line 962 "fehframe.qsc" */	/* inittable */
    {
      if (IItbinit(_form,_HlpTblFld,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 964 "fehframe.qsc" */	/* host code */
	while ( SIgetrec(buffer, sizeof(buffer), fp) == OK )
	{
		char	*cp;
		anylines = TRUE;
		/*
		**  Throw away any newline characters so
		**  adc_cvinto won't complain about it.
		*/
		if ((cp = STindex(buffer, ERx("\n"), 0)) != NULL)
		{
			if (cp < (buffer + TABLEWIDTH))
			{
				*cp = EOS;
			}
			else
			{
				buffer[TABLEWIDTH] = EOS;
			}
		}
/*
**		Check for directive lines.  If one is found
**		(line begins with "/#type"), then check for directive type.
**		Otherwise, load tablefield with line.
*/
		directive = FALSE;
		if (buffer[0] == ERx('/') && buffer[1] == ERx('#'))
		{
			i4	count;		/* count of words */
			char	tmpbuf[512]; /* temporary buffer to save line */
			/* Break directive into tokens */
			_VOID_ STlcopy( buffer, tmpbuf, sizeof(tmpbuf) - 1 );
			count = MAXTOKEN;
			_VOID_ STgetwords( &tmpbuf[2],
					   &count,
					   names
			);
			if (count >= 1)
			{
			    if ((STbcompare(names[0], 0,
				 ERget(F_UF0059_SubTopicToken), 0, TRUE) == 0)
				&& (numrel < MAXTOPIC))
			    {
				/*
				**  Found a subtopic - retrieve the topic
				**  name and filename.  Build the listpick
				**  string.
				*/
				directive = TRUE;
				cp = names[1];
				_VOID_ STlcopy( cp,
						&stopics->_subtopics[numrel][0],
						TITLELEN
				);
				if (numrel == 0)
				{
				    _VOID_ STlcopy ( cp, stopics->_substring,
					TITLELEN);
				}
				else
				{
				    _VOID_ STcat(stopics->_substring,ERx("\n"));
				    _VOID_ STcat(stopics->_substring, cp);
				}
				cp = names[2];
				_VOID_ STlcopy( cp,
						&stopics->_subfnames[numrel][0],
						MAX_LOC);
				numrel++;
			    }
			    else if (STbcompare(names[0], 0, ERx("--"), 0, TRUE)
				    == 0)
			    {
				/*
				**  Comment line - throw it away!
				*/
				directive = TRUE;
			    }
			}
		}
		if (!directive)	    /* load into tablefield if not directive */
		{
/* # line 1051 "fehframe.qsc" */	/* loadtable */
    {
      if (IItbact(_form,_HlpTblFld,1) != 0) {
        IItcoputio((char *)"text",(short *)0,1,32,0,buffer);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 1052 "fehframe.qsc" */	/* host code */
		}
	} /* end while */
	if (anylines)
	{
		STcopy(subject, heading);
	}
	else
	{
		_VOID_ STprintf(heading, ERget(S_UF0044_Sorry_No_help_avail),
				subject
		);
	}
/* # line 1065 "fehframe.qsc" */	/* putform */
    {
      if (IIfsetio(_form) != 0) {
        IIputfldio((char *)"title",(short *)0,1,32,0,heading);
      } /* IIfsetio */
    }
/* # line 1066 "fehframe.qsc" */	/* host code */
	return (numrel);
  }
/*{
** Name:	_saveKeys() -	Saves the key mapping definitions.
**
** Description:
**	Saves the current key mapping definitions, and sets them up for
**	the restore operation.
**
** Inputs:
**
** Returns:
**	N/A.
**
** History:
**	15-mar-1990 (swilson)   first edition.
*/
static VOID
  _saveKeys(mapinfo)
  MAPINFO *mapinfo;
  {
	/*
	**  Save current key mapping and label information.
	*/
/* # line 1094 "fehframe.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,19,mapinfo->upmap,8,(char *)0,2017);
        IIiqfsio((short *)0,1,32,19,mapinfo->downmap,8,(char *)0,2018);
        IIiqfsio((short *)0,1,32,19,mapinfo->menu1map,8,(char *)0,3000);
        IIiqfsio((short *)0,1,32,19,mapinfo->menu2map,8,(char *)0,3001);
        IIiqfsio((short *)0,1,32,19,mapinfo->menu3map,8,(char *)0,3002);
        IIiqfsio((short *)0,1,32,19,mapinfo->menu4map,8,(char *)0,3003);
        IIiqfsio((short *)0,1,32,19,mapinfo->menu5map,8,(char *)0,3004);
        IIiqfsio((short *)0,1,32,49,mapinfo->uplabel,9,(char *)0,2017);
        IIiqfsio((short *)0,1,32,49,mapinfo->downlabel,9,(char *)0,2018);
        IIiqfsio((short *)0,1,32,49,mapinfo->menu1label,9,(char *)0,3000);
        IIiqfsio((short *)0,1,32,49,mapinfo->menu2label,9,(char *)0,3001);
        IIiqfsio((short *)0,1,32,49,mapinfo->menu3label,9,(char *)0,3002);
        IIiqfsio((short *)0,1,32,49,mapinfo->menu4label,9,(char *)0,3003);
        IIiqfsio((short *)0,1,32,49,mapinfo->menu5label,9,(char *)0,3004);
      } /* IIiqset */
    }
/* # line 1109 "fehframe.qsc" */	/* host code */
	return;
  }
/*{
** Name:	_restoreKeys() - Restores the key mapping definitions.
**
** Description:
**	Restores the previous key mapping definitions.
**
** Inputs:
**
** Returns:
**	N/A.
**
** History:
**	15-mar-1990 (swilson)   first edition.
*/
static VOID
  _restoreKeys(mapinfo)
  MAPINFO *mapinfo;
  {
	/*
	**  Restore original key mappings.
	*/
	if (mapinfo->upmap[0] != EOS)
	{
/* # line 1138 "fehframe.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,2017,(short *)0,1,32,0,mapinfo->upmap);
        IIstfsio(9,(char *)0,2017,(short *)0,1,32,0,mapinfo->uplabel);
      } /* IIiqset */
    }
/* # line 1140 "fehframe.qsc" */	/* host code */
	}
	if (mapinfo->downmap[0] != EOS)
	{
/* # line 1143 "fehframe.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,2018,(short *)0,1,32,0,mapinfo->downmap);
        IIstfsio(9,(char *)0,2018,(short *)0,1,32,0,mapinfo->downlabel);
      } /* IIiqset */
    }
/* # line 1145 "fehframe.qsc" */	/* host code */
	}
	if (mapinfo->menu1map[0] != EOS)
	{
/* # line 1148 "fehframe.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3000,(short *)0,1,32,0,mapinfo->menu1map);
        IIstfsio(9,(char *)0,3000,(short *)0,1,32,0,mapinfo->menu1label);
      } /* IIiqset */
    }
/* # line 1150 "fehframe.qsc" */	/* host code */
	}
	if (mapinfo->menu2map[0] != EOS)
	{
/* # line 1153 "fehframe.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3001,(short *)0,1,32,0,mapinfo->menu2map);
        IIstfsio(9,(char *)0,3001,(short *)0,1,32,0,mapinfo->menu2label);
      } /* IIiqset */
    }
/* # line 1155 "fehframe.qsc" */	/* host code */
	}
	if (mapinfo->menu3map[0] != EOS)
	{
/* # line 1158 "fehframe.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3002,(short *)0,1,32,0,mapinfo->menu3map);
        IIstfsio(9,(char *)0,3002,(short *)0,1,32,0,mapinfo->menu3label);
      } /* IIiqset */
    }
/* # line 1160 "fehframe.qsc" */	/* host code */
	}
	if (mapinfo->menu4map[0] != EOS)
	{
/* # line 1163 "fehframe.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3003,(short *)0,1,32,0,mapinfo->menu4map);
        IIstfsio(9,(char *)0,3003,(short *)0,1,32,0,mapinfo->menu4label);
      } /* IIiqset */
    }
/* # line 1165 "fehframe.qsc" */	/* host code */
	}
	if (mapinfo->menu5map[0] != EOS)
	{
/* # line 1168 "fehframe.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3004,(short *)0,1,32,0,mapinfo->menu5map);
        IIstfsio(9,(char *)0,3004,(short *)0,1,32,0,mapinfo->menu5label);
      } /* IIiqset */
    }
/* # line 1170 "fehframe.qsc" */	/* host code */
	}
	return;
  }
/*
** Name:	_setKeys() - Sets the key mapping definitions for Help.
**
** Description:
**	Sets the key mappings for NextPage and PrevPage.
**
** Inputs:
**	i4	offset		relative position for keys.
**
** Returns:
**	N/A.
**
** History:
**	15-mar-1990 (swilson)   first edition.
*/
static VOID
  _setKeys(offset, mapinfo)
i4		offset;
  MAPINFO *mapinfo;
  {
	/*
	**  Set key mappings.
	*/
	if ( mapinfo->upmap[0] != EOS )
	{
	    switch (offset)
	    {
	    case 0:
/* # line 1204 "fehframe.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3000,(short *)0,1,32,0,mapinfo->upmap);
        IIstfsio(9,(char *)0,3000,(short *)0,1,32,0,mapinfo->uplabel);
      } /* IIiqset */
    }
/* # line 1206 "fehframe.qsc" */	/* host code */
		break;
	    case 1:
/* # line 1208 "fehframe.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3001,(short *)0,1,32,0,mapinfo->upmap);
        IIstfsio(9,(char *)0,3001,(short *)0,1,32,0,mapinfo->uplabel);
      } /* IIiqset */
    }
/* # line 1210 "fehframe.qsc" */	/* host code */
		break;
	    case 2:
/* # line 1212 "fehframe.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3002,(short *)0,1,32,0,mapinfo->upmap);
        IIstfsio(9,(char *)0,3002,(short *)0,1,32,0,mapinfo->uplabel);
      } /* IIiqset */
    }
/* # line 1214 "fehframe.qsc" */	/* host code */
		break;
	    case 3:
/* # line 1216 "fehframe.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3003,(short *)0,1,32,0,mapinfo->upmap);
        IIstfsio(9,(char *)0,3003,(short *)0,1,32,0,mapinfo->uplabel);
      } /* IIiqset */
    }
/* # line 1218 "fehframe.qsc" */	/* host code */
		break;
	    }
	}
	if ( mapinfo->downmap[0] != EOS )
	{
	    switch (offset)
	    {
	    case 0:
/* # line 1226 "fehframe.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3001,(short *)0,1,32,0,mapinfo->downmap);
        IIstfsio(9,(char *)0,3001,(short *)0,1,32,0,mapinfo->downlabel);
      } /* IIiqset */
    }
/* # line 1228 "fehframe.qsc" */	/* host code */
		break;
	    case 1:
/* # line 1230 "fehframe.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3002,(short *)0,1,32,0,mapinfo->downmap);
        IIstfsio(9,(char *)0,3002,(short *)0,1,32,0,mapinfo->downlabel);
      } /* IIiqset */
    }
/* # line 1232 "fehframe.qsc" */	/* host code */
		break;
	    case 2:
/* # line 1234 "fehframe.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3003,(short *)0,1,32,0,mapinfo->downmap);
        IIstfsio(9,(char *)0,3003,(short *)0,1,32,0,mapinfo->downlabel);
      } /* IIiqset */
    }
/* # line 1236 "fehframe.qsc" */	/* host code */
		break;
	    case 3:
/* # line 1238 "fehframe.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3004,(short *)0,1,32,0,mapinfo->downmap);
        IIstfsio(9,(char *)0,3004,(short *)0,1,32,0,mapinfo->downlabel);
      } /* IIiqset */
    }
/* # line 1240 "fehframe.qsc" */	/* host code */
		break;
	    }
	}
  }
