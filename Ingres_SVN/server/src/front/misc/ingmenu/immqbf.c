# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  mqbf.qc
**
**  Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<er.h>
# include	"erim.h"
# include	"imconst.h"
# include	"imextern.h"
VOID
IIIMmqs_mqbfstart(type)
i4	type;
{
	char	mqfflags[10];
	char	buffer[80];
	char	*sysname = ERx("qbf");
	char	*args[10];
	char	**argp = args;
	i4	argcount = 0;
	argp = args;
	argcount = 0;
	mqfflags[0] = '\0';
/* # line 34 "immqbf.qsc" */	/* message */
  {
    IImessage(ERget(F_IM002A_Starting_QBF));
  }
/* # line 36 "immqbf.qsc" */	/* host code */
	if (type == QBF_START || type == QBF_JOIN)
	{
		buffer[0] = '\0';
		if (type == QBF_JOIN)
		{
			STcat(mqfflags, ERx(" -j"));
		}
		if (im_empty_catalogs)
		{
			STcat(mqfflags, ERx(" -e"));
		}
		if (mqfflags[0])
		{
			STcat(buffer, ERx("flags = %S"));
			*argp++ = mqfflags;
			argcount++;
		}
		if (swspawn(sysname, buffer, argcount, args) != OK)
		{
			starterr(sysname);
		}
/* # line 60 "immqbf.qsc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 61 "immqbf.qsc" */	/* host code */
	}
}
