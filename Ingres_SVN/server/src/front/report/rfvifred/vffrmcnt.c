/*
** Copyright (c) 2004 Ingres Corporation
*/

/*
** vffrmcnt.c
**
** vffrmcount - inform the frame struct that there has been a change
**		in the number of a particular kind of feature. 
**
** history:
**	1/28/85 (peterk) - split off from frame.c
**	24-apr-89 (bruceb)
**		No longer any nonseq fields to deal with.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/

# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	"decls.h"

/*
** change the frame counts for a particular feature.
*/
vffrmcount(ps, val)
reg POS		*ps;
i4		val;
{
	switch (ps->ps_name)
	{
	case PTRIM:
		frame->frtrimno += val;
		break;

# ifndef FORRBF
	case PTABLE:
# endif
	case PFIELD:
		frame->frfldno += val;
		break;
	}
}
