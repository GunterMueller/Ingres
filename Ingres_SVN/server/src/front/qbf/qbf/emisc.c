# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include	<compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include "mqbf.h"
# include	"mqeglobs.h"
# include	<me.h>
# include	<er.h>
# include 	"erqf.h"
# include	<ug.h>
/*
** emisc.qsc
**
** some useful things which don't belong anyplace
**
**	Routines:	mqsaveStr()
**
**	Copyright (c) 2004 Ingres Corporation
**
**	History:
**		13-jul-87 (bab) Changed memory allocation to use [FM]Ereqmem.
**		12-aug-1987 (danielt)
**			changed syserr() call to 
**			IIUGerr(msgid,UG_ERR_FATAL) call
**		07-dec-87 (kenl)
**			Turned module into a .QSC file in order to incorporate
**			the new header file: mqbf.qsh
/*
** save a string
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
char *
mqsaveStr(str)
char	*str;
{
	char	*cp;
	if ((cp = (char *)MEreqmem((u_i4)0, (u_i4)(STlength(str)+1), TRUE,
	    (STATUS *)NULL)) == NULL)
	{
# ifdef BADMEM
		FEbadmem();
# else
		IIUGerr(E_QF0029_mqsaveStr__out_of_spa,UG_ERR_FATAL,0);
# endif
	}
	STcopy(str, cp);
	return(cp);
}
mq_exinit()
{
	mq_tids[0] = '\0';
	mq_mwhereend = 0;
	mq_dwhereend = 0;
}
