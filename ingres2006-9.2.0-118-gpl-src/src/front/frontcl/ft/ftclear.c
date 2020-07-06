/*
**  FTclear
**
**  Copyright (c) 2004 Ingres Corporation
**
**  static	char	*Sccsid = "%W%	%G%";
**
**  History:
**	10/27/88 (dkh) - Performance changes.
**      23-sep-96 (mcgem01)
**              Global data moved to ftdata.c
*/

# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<termdr.h> 


GLOBALREF	bool	IIFTscScreenCleared;

FTclear()
{
	TDclear(curscr);

	IIFTscScreenCleared = TRUE;

	/*
	**  TDclear already calls TDrefresh.
	TDrefresh(curscr);
	*/
}
