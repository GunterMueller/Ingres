# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  Copyright (c) 2004 Ingres Corporation
*/
/*
** Name:	 report.qc
**
** History:
**	? (dh)	written.
**	21-mar-1985 (peter)
**		Fix bug 5221. If report type 'any' selected, send 
**		through style flag.
**	22-oct-1987 (peter)
**		Take out calls to FEmsg.
**	15-may-1990 (pete)
**		Took out "-p" flag, which is no longer supported in 6.4.
**		Also took out the PCsleep(1000) and ##clear screen;
**		both were just before the FEspawn call.
**	15-apr-1993 (rogerl)
**		No more tuckname here; the tablename has already been verified.
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
# include	<er.h>
# include	"ertu.h"
# include	"tuconst.h"
VOID
reportstart(repname, reptype)
char	*repname;
i4	reptype;
{
  char flags[6];
  char outfile[FE_PROMPTSIZE];
  char buffer[256];
	char	*cmdflag = ERx("");
	char	*sysname = ERx("report");
	char	repflags[10];
	char	params[256];
	char	*args[10];
	char	**argp = args;
	i4	argcount = 0;
	char	*stylestr;
	char	*retstr;
	STcopy(ERget(F_TU0038_Enter_file_name), buffer);
/* # line 59 "report.qsc" */	/* prompt */
  {
    IIprmptio(0,buffer,(short *)0,1,32,FE_PROMPTSIZE-1,outfile);
  }
/* # line 61 "report.qsc" */	/* message */
  {
    IImessage(ERget(F_TU0039_Start_REPORT_WRITER));
  }
/* # line 63 "report.qsc" */	/* host code */
	buffer[0] = '\0';
	repflags[0] = '\0';
	params[0] = '\0';
	argp = args;
	argcount = 0;
	/* its ALWAYS a table (a default report) from the tables utility */
	STcopy(ERx("name = %S"), buffer);
	*argp++ = repname;
	argcount++;
	if (reptype == H_WRAP)
	{
		stylestr = ERx("wrap");
	}
	else if (reptype == H_COLUMN)
	{
		stylestr = ERx("column");
	}
	else if (reptype == H_BLOCK)
	{
		stylestr = ERx("block");
	}
	else
	{
		stylestr = ERx("");
	}
	STcat(buffer, ERx(", mode = %S"));
	STcat(params, stylestr);
	*argp++ = stylestr;
	argcount++;
	if (outfile[0] != '\0')
	{
		STcat(buffer, ERx(", file = %S"));
		STcat(params, outfile);
		*argp++ = outfile;
		argcount++;
	}
	if (FEspawn(sysname , buffer, argcount, args) != OK)
	{
		starterr(sysname);
	}
/* # line 106 "report.qsc" */	/* clear */
  {
    IIclrscr();
  }
/* # line 107 "report.qsc" */	/* host code */
}
