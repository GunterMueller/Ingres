/*
**  VTerase.c
**
**  Copyright (c) 2004 Ingres Corporation
**
**  static	char	Sccsid[] = "@(#)vterase.c	30.1	12/3/84";
*/

# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	 "vtframe.h"

VTerase(frm)
FRAME	*frm;
{
	TDerase(frm->frscr);
}
