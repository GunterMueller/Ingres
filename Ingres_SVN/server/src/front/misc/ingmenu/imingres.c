# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  ingres.qsc
**
**  Copyright (c) 2004 Ingres Corporation
**
**	History:
**		6/5/85 (prs)	Fix bug 5525.  Change INGRES to QUEL.
**		11/11/85 (prs)	Put in IIbreak to clean up after call
**				to FSTM.
**		11/26/85 (prs)	Change SQL to ISQL and QUEL to IQUEL.
**		10/05/89 (mdw)	When in CMS, make the sysname "fstm"
**				instead of "isql". This is required
**				because IBM's own interactive terminal
**				monitor in CMS is called "ISQL".
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
# include	<er.h>
# include	"erim.h"
# include	"imconst.h"
# include	"imextern.h"
VOID
ingresstart(cmd)
i4	cmd;
{
	char	buffer[256];
	char	*sysname = ERx("iquel");
	char	*args[10];
	i4	argcount = 0;
	if (cmd == H_SQL)
	{
/* # line 45 "imingres.qsc" */	/* message */
  {
    IImessage(ERget(F_IM0030_Starting_ISQL));
  }
/* # line 46 "imingres.qsc" */	/* host code */
#ifdef CMS
		sysname = ERx("fstm");		/* mdw */
#else
		sysname = ERx("isql");
#endif /* CMS */
	}
	else
	{
/* # line 54 "imingres.qsc" */	/* message */
  {
    IImessage(ERget(F_IM002F_Starting_IQUEL));
  }
/* # line 55 "imingres.qsc" */	/* host code */
		sysname = ERx("iquel");
	}
	buffer[0] = EOS;
	if (swspawn(sysname, buffer, argcount, args) != OK)
	{
		starterr(sysname);
	}
	/* VARARGS2 */
/* # line 64 "imingres.qsc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 65 "imingres.qsc" */	/* host code */
}
