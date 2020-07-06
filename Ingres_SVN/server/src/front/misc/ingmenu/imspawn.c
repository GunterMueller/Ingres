# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  spawn.c
**
**  Copyright (c) 2004 Ingres Corporation
**
**	History
**		6/5/85 (prs)	Fix to bug 5525.  Change INGRES to QUEL.
**		7/20/85 (prs)	Switch back to no-CFE definition.
**		4/10/87 (dpr)	Took out reference to <os.h>
**      	05-jan-90 (sylviap)
**              	Added new parameter to UTexe.
**      	08-mar-90 (sylviap)
**		  	Changed call to UTexe to pass an CL_ERR_DESC.
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
# include	<si.h>
# include	<ut.h>
# include	<ftdefs.h>
# include	<er.h>
# include	<ug.h>
# include	"erim.h"
# include	"imconst.h"
# include	"imextern.h"
FUNC_EXTERN	char	*IIxflag();
STATUS
swspawn(subsys, format, argnum, args)
char	*subsys;
char	*format;
i4	argnum;
char	*args[];
{
	STATUS	status;
	i4	otherargs = 2;
	char	buf[256];
	char	testbuf[256];
	bool	testing = FALSE;
	char	errbuf[ER_MAX_LEN];
  char reply[80];
	CL_ERR_DESC err_code;
	if ((testingSW || SWsettest) &&
	    STcompare(subsys, ERx("report")) != 0 &&
	    STcompare(subsys, ERx("sreport")) != 0 &&
	    STcompare(subsys, ERx("graph")) != 0)
	{
		testing = TRUE;
		if (SWsettest)
		{
			FEinames(testbuf);
		}
		else
		{
			FEtnames(testbuf);
		}
		otherargs++;
		if (*im_uflag != EOS)
		{
			/* VARARGS2 */
			STprintf(buf, ERx("flags = %%S, database = %%S, equel = %%S, flags = %%S, %s"), format);
			otherargs++;
		}
		else
		{
			/* VARARGS2 */
			STprintf(buf, ERx("database = %%S, equel = %%S, flags = %%S, %s"), format);
		}
	}
	else
	{
		if (*im_uflag != EOS)
		{
			/* VARARGS2 */
			STprintf(buf, ERx("flags = %%S, database = %%S, equel = %%S, %s"), format);
			otherargs++;
		}
		else
		{
			/* VARARGS2 */
			STprintf(buf, ERx("database = %%S, equel = %%S, %s"), format);
		}
	}
	FTrestore(FT_NORMAL);
	if (testing)
	{
		if (*im_uflag != EOS)
		{
			status = UTexe(UT_WAIT, NULL, (ModDictDesc *) NULL,
				NULL, subsys, &err_code, buf, 
				argnum + otherargs, im_uflag,
				im_dbname, IIxflag(), testbuf, args[0], args[1],
				args[2], args[3], args[4],
				args[5], args[6], args[7], args[8], args[9]);
		}
		else
		{
			status = UTexe(UT_WAIT, NULL, (ModDictDesc *) NULL,
				NULL, subsys, &err_code, buf, 
				argnum + otherargs, im_dbname,
				IIxflag(), testbuf, args[0], args[1], args[2],
				args[3], args[4],
				args[5], args[6], args[7], args[8], args[9]);
		}
	}
	else
	{
		if (*im_uflag != EOS)
		{
			status = UTexe(UT_WAIT, NULL, (ModDictDesc *) NULL,
				NULL, subsys, &err_code, buf, 
				argnum + otherargs, im_uflag,
				im_dbname, IIxflag(), args[0], args[1], args[2],
				args[3], args[4], args[5],
				args[6], args[7], args[8], args[9]);
		}
		else
		{
			status = UTexe(UT_WAIT, NULL, (ModDictDesc *) NULL,
				NULL, subsys, &err_code, buf, 
				argnum + otherargs, im_dbname,
				IIxflag(), args[0], args[1], args[2], args[3],
				args[4], args[5],
				args[6], args[7], args[8], args[9]);
		}
	}
#ifndef FT3270
	SIfprintf(stdout, ERx("\n"));
	SIflush(stdout);
#endif
	if (status)
	{
		if (status == FAIL)
		{
#ifndef FT3270
#ifndef MSDOS
			FTprtrestore();
/* # line 152 "imspawn.qsc" */	/* prompt */
  {
    IIprmptio(0,ERget(S_IM000D_Press_RETURN_to_clear),(short *)0,1,32,79,reply);
  }
/* # line 153 "imspawn.qsc" */	/* host code */
			FTrestore(FT_FORMS);
#else
			FEprompt(ERget(S_IM000E_Press_ENTER_to_clear_),FALSE,80,reply);
			FTrestore(FT_FORMS);
#endif	/* MSDOS */
# else
			FTmore();
#endif
		}
		else if (status == UTENOPROG)
		{
			FTrestore(FT_FORMS);
			IIUGerr(E_IM000F_Sorry____s_is_not_ava, UG_ERR_ERROR,
				1, (PTR) subsys);
		}
		else if (ERreport(status, errbuf) != OK)
		{
			FTrestore(FT_FORMS);
			IIUGerr(E_IM0010_Sorry___Could_not_sta, UG_ERR_ERROR,
				2, (PTR) subsys, (PTR) &status);
		}
		else
		{
			FTrestore(FT_FORMS);
/* # line 177 "imspawn.qsc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 178 "imspawn.qsc" */	/* host code */
			IIUGerr(E_IM0014_General_Error, UG_ERR_ERROR, 1,
				(PTR) errbuf);
		}
	}
	else
	{
#ifndef FT3270
#ifndef MSDOS
		FTrestore(FT_FORMS);
#else
		if ( (STcompare(subsys, ERx("report")) == 0)
		    || (STcompare(subsys, ERx("sreport")) == 0) )
		{
			FEprompt(ERget(S_IM000E_Press_ENTER_to_clear_),FALSE,80,reply);
		}
		FTrestore(FT_FORMS);
#endif	/* MSDOS */
#else
		if ( (STcompare(subsys, ERx("sreport")) == 0) )
		{
			FTmore();
		}
		else
		{
			FTrestore(FT_FORMS);
		}
#endif
	}
	FTfkset(FT_FUNCKEY);
	return(status);
}
