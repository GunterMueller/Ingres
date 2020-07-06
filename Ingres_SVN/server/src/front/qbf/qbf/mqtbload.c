# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include "mqbf.h"
/*
**	MQTBLLOAD.QSC  -  Reload the proper routine after calling tables
**
**	History:
**		07-dec-87 (kenl)
**			Turned module into a .QSC file in order to incorporate
**			the new header file: mqbf.qsh
**
** Name:	mq_tbl_load	Run/edit/create a joindef.
**
** Parameters:
**	Inputs:
**		mq_qdef		QDEF structure.
**		newold		TRUE if joindef exists.
**		where		Where to return (used for overlay loading).
**
**	Outputs:
**
** History:
**	11-feb-1988 (peter)
**		Added the lastsaved parameter.
**	05-dec-89 (kenl)
**		Replaced lastsaved parameter with the global mq_lastsaved.
**		This change was required by the new calling sequence needed
**		for the -J flag.
*/
mq_tbl_load(mq_qdef, newold, where)
MQQDEF		*mq_qdef;
bool		newold;
i4		where;
{
	mq_tables(mq_qdef, newold);
	switch (where)
	{
	  case	mqCATALOGS:
		cat_reload();
		break;
	  case	mqHELP:
		hlp_reload();
		break;
	  case	mqQBFNAMES:
		nam_reload();
		break;
	  default:
		break;
	}
}
