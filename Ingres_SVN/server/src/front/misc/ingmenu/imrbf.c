# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  rbfstart.qc
**
**  Copyright (c) 2004 Ingres Corporation
**
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
rbfstart()
{
	char	buffer[256];
	char	ebuf[4];
	char	*args[10];
	i4	argcount = 0;
/* # line 27 "imrbf.qsc" */	/* message */
  {
    IImessage(ERget(F_IM002B_Starting_RBF));
  }
/* # line 29 "imrbf.qsc" */	/* host code */
	args[0] = NULL;
	if (im_empty_catalogs)
	{
		argcount++;
		STcopy(ERx("emptycat = %S"), buffer);
		STcopy(ERx("-e"), ebuf);
		args[0] = ebuf;
	}
	if (swspawn(ERx("rbf"), buffer, argcount, args) != OK)
	{
		starterr(ERx("rbf"));
	}
/* # line 44 "imrbf.qsc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 45 "imrbf.qsc" */	/* host code */
}
