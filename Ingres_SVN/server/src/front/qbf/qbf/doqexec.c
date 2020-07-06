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
# include	 "mqeglobs.h"
# include	<er.h>
# include	"erqf.h"
/*
**	DOEXEC.QSC -
**
**	History:
**		07-dec-87 (kenl)
**			Turned module into a .QSC file in order to incorporate
**			the new header file: mqbf.qsh
**		11-apr-1988 (danielt)
**			added commit after ## FORMINIT to release locks
**			on forms catalogs.
**		30-aug-1988 (danielt)
**			removed commit (we use QUEL transactions now)
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
/*
**
**	Begin_qexec() -- builds a default form and initializes
**	query descriptors for query execution phase.
**
*/
begin_qexec(mq_qdef, tblwidth)
MQQDEF	*mq_qdef;
i4	tblwidth;
{
  char *name;
	i4		msecs;
	mq_exinit();
	/* inititalize mqexecute frm */
	if (qinitfrms() == FALSE)
		return(-1);
	if (mq_deffrm == TRUE) {
		if (mqbldform(mq_qdef, tblwidth) < 0) {
			return(-1);
		}
	}
	else {
		/** form exists **/
		if (mq_second)
		{
			name = mq_frame;
/* # line 67 "doqexec.qsc" */	/* message */
  {
    IImessage(ERget(S_QF0028_Initializing_form____));
  }
/* # line 68 "doqexec.qsc" */	/* forminit */
  {
    IIforminit(name);
  }
/* # line 69 "doqexec.qsc" */	/* host code */
		}
	}
	if (mqinitqdesc(mq_qdef) == FALSE)
	{
		return(-1);
	}
# ifdef DEBUG
	if (mq_debug)
	{
		msecs = TMcpu();
		SIfprintf(mqoutf, ERx("BEGIN_QEXEC: after mqinitqdesc, msecs = %d\n"), msecs);
		SIfprintf(mqoutf,ERx("BQ:rparam=\n%s\n"), mq_qdef->mqqdesc->qg_param);
		SIfprintf(mqoutf,ERx("BQ:wparam=\n%s\n"), mq_qdef->mqqdesc->qg_wparam);
		if (qdef_type == 0)
		{
			SIfprintf(mqoutf,ERx("BQ :rparam of child=\n%s\n"), mq_qdef->mqqdesc->qg_child->qg_param);
			SIfprintf(mqoutf,ERx("BQ:wparam of child=\n%s\n"),mq_qdef->mqqdesc->qg_child->qg_wparam);
		}
	}
# endif
	return(mqqexecute(mq_qdef));
}
