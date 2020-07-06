/*
**  VTdelstr.c
**
**  Copyright (c) 2004 Ingres Corporation
**
**  static	char	Sccsid[] = "@(#)vtdelstr.c	30.1	12/3/84";
*/

# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	"vtframe.h"

VTdelstring(frm, y, x, length)
FRAME	*frm;
i4	y;
i4	x;
i4	length;
{
	reg	WINDOW	*win;
	reg	i4	i;

	win = frm->frscr;
	TDmove(win, y, x);
	for (i = 0; i < length; i++)
	{
		TDersdispattr(win);
		TDadch(win, ' ');
	}
}
