# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	DEFFORM.QSC -
**
**	Copyright (c) 2004 Ingres Corporation
**
**	History:
**		07-dec-87 (kenl)
**			Turned module into a .QSC file in order to incorporate
**			the new header file: mqbf.qsh
**		03-jan-88 (kenl)
**			Added CVlower to handle case-sensitive table names
**			which become form names.  Added a return to handle
**			success (for FEingferr call).
**		09-mar-93 (connie) Bug #46785
**			Check for return value from mq_makefrm
**              11-jan-1996 (toumi01; from 1.1 axp_osf port)
**                      Added dkhor's change for axp_osf
**                      10-sep-93 (dkhor)
**                      defform is holding pointer, change its type to PTR.
**                      Declared defform as i4 will result in truncation
**                      of address on 64-bit platform such Alpha OSF/1.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
# include	<compat.h>
# include	<cv.h>		/* 6-x_PC_80x86 */
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include "mqbf.h"
# include	<er.h>
# include	"erqf.h"
/*
**	Mqbldform() -- taken from bldquery.qc to delay
**	building of a default form intil query execution
**	phase.
**
*/
mqbldform(mq_qdef, tblwidth)
MQQDEF		*mq_qdef;
i4		tblwidth;
{
  char *name;
  PTR defform;
	FRAMEUNION	dfu;		/* to keep lint happy */
	/* delayed for two process scheme */
	if (mqgetwidth(mq_qdef) == -1)
	    return -1;
	if ((dfu.fuframe = mq_makefrm(mq_qdef, tblwidth)) == NULL ) return(-1);
	defform = dfu.fui4;
	name = dfu.fuframe->frname;
	STcopy(name, mq_frame);
	CVlower(mq_frame);
/* # line 67 "defform.qsc" */	/* addform */
  {
    IIaddform(defform);
  }
/* # line 68 "defform.qsc" */	/* host code */
	if (FEinqferr())
	{
		IIUGerr(E_QF0027_Can_t_initialize_QBF_, 0, 0);
		return(-1);
	}
	else
	{
		return (0);
	}
}
