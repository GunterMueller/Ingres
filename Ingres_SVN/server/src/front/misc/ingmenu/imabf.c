# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  abf.qc
**
**  Copyright (c) 2004 Ingres Corporation
**
**  History:
**	4/18/89 (pete)	Remove "application =%s" from subsystem call.
**			so abf will start in catalog frame.
**	11/1/89 (mdw)	Remove UTfence call for CMS due to revision
**			of ABF and repackaging of all FE components.
**	2/5/90	(pete)	Add support for -v flag to differentiate between
**			Vision and ABF. Added "item" parameter to abfstart().
**	3/20/90 (pete)	Rather than calling iiabf with -v, change to call
**			either 'abf' or 'vision'.
**	5/3/90	(pete)	Invoke xxx rather than vision.
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<er.h>
# include	<ug.h>
# include	"erim.h"
# include	"imconst.h"
# include	"imextern.h"
static READONLY char _abf[]		= "abf";
static READONLY char _vision[]		= "vision";
static READONLY char _empty[]		= "";
VOID
abfstart(item)
ER_MSGID        item;
{
	char	*appl;
	PTR	args[10];
	if (*im_xpipe != EOS)
	{
		IIUGerr(E_IM0002_INGRES_MENU_was_not_i, UG_ERR_ERROR, 0);
		return;
	}
	if (item == F_IM0012_Applications) 	/* ABF only; no Vision */
	{
	    appl = _abf;
/* # line 49 "imabf.qsc" */	/* message */
  {
    IImessage(ERget(F_IM0029_Starting_ABF));
  }
/* # line 50 "imabf.qsc" */	/* host code */
	}
	else					/* customer has Vision!! */
	{
	    appl = _vision;		/* note: during project Vision was
					** known as "Vision".
					*/
/* # line 56 "imabf.qsc" */	/* message */
  {
    IImessage(ERget(F_IM0035_Starting_Vision));
  }
/* # line 57 "imabf.qsc" */	/* host code */
	}
	if (swspawn(appl, _empty, 0, args) != OK)
	{
		starterr(appl);
	}
/* # line 64 "imabf.qsc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 65 "imabf.qsc" */	/* host code */
}
