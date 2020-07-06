# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  vifred.qc
**
**  Copyright (c) 2004 Ingres Corporation
**
**	History:
**	Revision 1.2  86/10/20	15:03:40  john
**	Set up for new Ingres/Menu.  No more intermediate forms.
**
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
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
vifredstart()
{
	char	buffer[80];
	char	*sysname = ERx("vifred");
	char	*args[10];
	char	**argp = args;
	i4	argcount = 0;
/* # line 38 "imvifred.qsc" */	/* message */
  {
    IImessage(ERget(F_IM002D_Starting_VIFRED));
  }
/* # line 40 "imvifred.qsc" */	/* host code */
	argp = args;
	argcount = 0;
	buffer[0] = '\0';
	if (im_empty_catalogs)
	{
		STcopy(ERx("flags = %S"), buffer);
		*argp++ = ERx("-e");
		argcount++;
	}
	if (swspawn(sysname, buffer, argcount, args) != OK)
	{
		starterr(sysname);
	}
/* # line 55 "imvifred.qsc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 56 "imvifred.qsc" */	/* host code */
}
