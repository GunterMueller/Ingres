# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	MQEDATA.QSC	- Variable definitions for MQBF
**
**	This module defines the global C variables used by execution
**	phase of QBF.
**
**	12/11/84 - kira.
**	06/13/86 - Cleanup initialization for CMS (wolf)
**	07-dec-87 (kenl)
**		Turned module into a .QSC file in order to incorporate
**		the new header file: mqbf.qsh
**	22-apr-88 (kenl)
**		Increased length of mq_tids. Set equal to MQ_HIDDENMAX, the
**		maximum length of a hidden column string allowed by the
**		forms system.
**
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include "mqbf.h"
# include	"mqeglobs.h"
# include	<er.h>
# include	"erqf.h"
GLOBALDEF i4	mq_mwhereend	ZERO_FILL;
GLOBALDEF i4	mq_dwhereend	ZERO_FILL;
GLOBALDEF i4	mqrstate	ZERO_FILL;
GLOBALDEF i4	mq_record	ZERO_FILL;
GLOBALDEF char	mq_tids[MQ_HIDDENMAX] = ERx("");
GLOBALDEF bool	mq_second = FALSE;
/* for recover (kira) */
GLOBALDEF FILE *mqb_file	= NULL;
