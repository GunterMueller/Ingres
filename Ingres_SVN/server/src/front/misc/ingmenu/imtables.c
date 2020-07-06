# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  tblstart.qc
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
tablestart()
{
	char	buffer[256];
	char	specbuf[6];
	char	ebuf[4];
	char	*args[10];
	i4	argcount;
/* # line 29 "imtables.qsc" */	/* message */
  {
    IImessage(ERget(F_IM002C_Starting_Tblutil));
  }
/* # line 31 "imtables.qsc" */	/* host code */
	argcount = 1;
	STcopy(ERx("specflag = %S"), buffer);
	STcopy(ERx("3"), specbuf);
	args[0] = specbuf;
	if (im_empty_catalogs)
	{
		argcount++;
		STcat(buffer, ERx(", emptycat = %S"));
		STcopy(ERx("-e"), ebuf);
		args[1] = ebuf;
	}
	if (swspawn(ERx("tables"), buffer, argcount, args) != OK)
	{
		starterr(ERx("tables"));
	}
/* # line 50 "imtables.qsc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 51 "imtables.qsc" */	/* host code */
}
