# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	MQBF.qh - MQBF's main include file
**
**	Copyright (c) 2004 Ingres Corporation
*/
# include	 <si.h>
# include	 <ft.h>
# include	 <fmt.h> 
# include	 <adf.h> 
# include	 <frame.h> 
# include	 <lo.h> 
# include	 <qg.h>
# include	 <ut.h>
EXEC SQL INCLUDE <mqtypes.sh>;
# include	"mqglobs.h"
# include 	"mqrtns.h"
# include	"mqerrs.h"
# include	"mqstates.h"
# ifdef		PCINGRES
# define	BADMEM
# define	NOLASTQRY
# endif
