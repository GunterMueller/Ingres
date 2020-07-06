/*
**  vtundo.c
**
**  Copyright (c) 2004 Ingres Corporation
**
**  History:
**	3-5-86 (wsj) Zerofill unlast and unimage
**	08/14/87 (dkh) - ER changes.
**	01-oct-87 (bruceb)
**		Allow for operation with unusable top line(s) on the terminal.
**	31-may-89 (bruceb)
**		If unLast (unImage) is non-NULL when VTundoalloc is called
**		then free it before allocating a new window.
**      24-sep-96 (mcgem01)
**              Global data moved to vtdata.c
*/

# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	"vtframe.h"
# include	<er.h>

GLOBALREF	WINDOW	*unLast;
GLOBALREF	WINDOW	*unImage;

VTundoalloc(frm)
FRAME	*frm;
{
	if (unLast)
		TDdelwin(unLast);
	if (unImage)
		TDdelwin(unImage);

	if ((unLast = TDnewwin(frm->frmaxy, frm->frmaxx, frm->frposy,
		frm->frposx)) == NULL)
	{
		IIUGbmaBadMemoryAllocation(ERx("VTundoalloc-1"));
	}
	if ((unImage = TDnewwin(frm->frmaxy, frm->frmaxx, frm->frposy,
		frm->frposx)) == NULL)
	{
		IIUGbmaBadMemoryAllocation(ERx("VTundoalloc-2"));
	}
	TDoverwrite(frm->frscr, unImage);
	TDoverwrite(frm->frscr, unLast);
	unImage->_starty = frm->frscr->_starty;
	unLast->_starty = frm->frscr->_starty;
}


VTundosave(frm)
FRAME	*frm;
{
	TDoverwrite(frm->frscr, unLast);
}

VTundores(frm)
FRAME	*frm;
{
	TDoverwrite(unLast, frm->frscr);
}


VTsvimage(frm)
FRAME	*frm;
{
	TDoverwrite(frm->frscr, unImage);
}

VTresimage(frm)
FRAME	*frm;
{
	TDoverwrite(unImage, frm->frscr);
}

VTswapimage(frm)
FRAME	*frm;
{
	TDoverwrite(frm->frscr, frm->untwo);
	TDoverwrite(unLast, frm->frscr);
	TDoverwrite(frm->untwo, unLast);
}
