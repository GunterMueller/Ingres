# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include <compat.h>
# include <st.h>
# include <lo.h>
# include <si.h>
# include <tm.h>
# include <er.h>
# include <gl.h>
# include <iicommon.h>
# include <fe.h>
# include <ug.h>
# include "errm.h"
/**
** Name:	dbrpt.sc - Replicated databases report
**
** Description:
**	Defines
**		databases_report - Replicated databases report
**		date_time	- Write current date and time to report file
**
** FIXME:
**	date_time() writes directly to the report and gets called
**	on every page in multiple page reports.  It would be better
**	to pass two arguments to store date and time and use them
**	over and over in each page.
**
**	Since TMinit() is not available in II_COMPATLIB, it is not called
**	and VMS times are not corrected for difference from GMT.
**
** History:
**	09-jan-97 (joea)
**		Created based on dbrpt.sc in replicator library.
**	09-sep-98 (abbjo03)
**		Replace ddba_messageit with IIUGerr. Replace string literals
**		with ERget calls.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
void date_time();
/*{
** Name:	databases_report - Replicated databases report
**
** Description:
**	Create a report from the dd_connections table.
**
** Inputs:
**	filename	- full path and name of the output file
**
** Outputs:
**	none
**
** Returns:
**	OK, E_RM0087_Err_open_report or the sqlca.sqlcode if an error occurs.
**
** Side effects:
**	A report file is created.  A COMMIT is issued after the report is
**	created.
*/
STATUS
databases_report(
char	*filename)
{
  char localdb[DB_MAXNAME*2+3];
  i2 database_no;
  char vnode_name[DB_MAXNAME+1];
  char database_name[DB_MAXNAME+1];
  char database_owner[DB_MAXNAME+1];
  char dbms_type[9];
  char remark[81];
	char	tmp[128];	/* strings for the report */
	u_i4	i;
	FILE	*report_fp;
	char	report_filename[MAX_LOC+1];
	LOCATION	loc;
	STATUS	status;
	char	*p;
	char	*line =
		ERx("------------------------------------------------------------------------------");
	STcopy(filename, report_filename);
	LOfroms(PATH & FILENAME, report_filename, &loc);
	if (SIfopen(&loc, ERx("w"), SI_TXT, SI_MAX_TXT_REC, &report_fp) != OK)
	{
		IIUGerr(E_RM0087_Err_open_report, UG_ERR_ERROR, 1,
			report_filename);
		return (E_RM0087_Err_open_report);
	}
	date_time(report_fp);
	p = ERget(F_RM00A9_Prod_name);
	i = 40 + STlength(p) / (u_i4)2;
	SIfprintf(report_fp, ERx("%*s\n"), i, p);
	p = ERget(F_RM00AE_Replic_dbs_rpt);
	i = 40 + STlength(p) / (u_i4)2;
	SIfprintf(report_fp, ERx("%*s\n"), i, p);
/* # line 112 "dbrpt.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select TRIM(database_name) +' ' +VARCHAR(database_no)from dd_database\
s where local_db=1");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME*2+3-1,localdb);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto rpt_err;
  }
/* # line 117 "dbrpt.sc" */	/* host code */
	SIfprintf(report_fp, ERx("%s\n\n"), localdb);
	SIfprintf(report_fp, ERx("%s\n"), line);
	SIfprintf(report_fp, ERx("%-12s"), ERget(F_RM00AF_Db_no));
	SIfprintf(report_fp, ERx("%-57s"), ERget(F_RM00B0_Vnode_db_name));
	SIfprintf(report_fp, ERx("%s\n"), ERget(F_RM00B1_Dbms_type));
	SIfprintf(report_fp, ERx("%12s%s\n"), ERx(" "),
		ERget(F_RM00B2_Db_owner_remarks));
	SIfprintf(report_fp, ERx("%s\n"), line);
/* # line 126 "dbrpt.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select database_no, database_name, vnode_name, database_owner, dbms_t\
ype, remark from dd_databases order by 1");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,2,&database_no);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,database_name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,vnode_name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,database_owner);
      IIgetdomio((short *)0,1,32,8,dbms_type);
      IIgetdomio((short *)0,1,32,80,remark);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 133 "dbrpt.sc" */	/* host code */
		STtrmwhite(vnode_name);
		STtrmwhite(database_name);
		STtrmwhite(database_owner);
		SIfprintf(report_fp, ERx("\n     %5d  "), database_no);
		if ((i4)(STlength(vnode_name) + STlength(database_name)) + 2
			< 57)
		{
			STprintf(tmp, ERx("%s::%s"), vnode_name, database_name);
			p = NULL;
		}
		else
		{
			STprintf(tmp, ERx("%s::"), vnode_name);
			p = database_name;
		}
		SIfprintf(report_fp, ERx("%-57s"), tmp);
		SIfprintf(report_fp, ERx("%s\n"), dbms_type);
		if (p != NULL)
			SIfprintf(report_fp, ERx("%12s%s\n"), ERx(" "), p);
		if (*database_owner != EOS)
			SIfprintf(report_fp, ERx("%12s%s\n"), ERx(" "),
				database_owner);
		for (p = remark; *p != EOS; p += i)
		{
			i = STlength(p);
			i = i > 66 ? 66 : i;
			SIfprintf(report_fp, ERx("%12s%-.*s\n"), ERx(" "),i, p);
		}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
    if (sqlca.sqlcode < 0) 
      goto rpt_err;
  }
/* # line 166 "dbrpt.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      goto rpt_err;
  }
/* # line 167 "dbrpt.sc" */	/* host code */
	SIclose(report_fp);
	return (OK);
rpt_err:
/* # line 172 "dbrpt.sc" */	/* host code */
	status = -sqlca.sqlcode;
/* # line 173 "dbrpt.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 174 "dbrpt.sc" */	/* host code */
	SIclose(report_fp);
	return (status);
}
/*{
** Name:	date_time - write current date and time to report file
**
** Description:
**	Obtains system time and date and inserts it into report file.
**
** Inputs:
**	report_fp - pointer to report file
**
** Outputs:
**	none
**
** Returns:
**	none
**
** Exceptions:
**	none
**
** Side effects:
**	?
*/
void
date_time(
FILE	*report_fp)
{
	SYSTIME	nowsec;
	struct TMhuman	now;
	TMnow(&nowsec);
	TMbreak(&nowsec, &now);
	SIfprintf(report_fp, ERx("%2s-%3s-%4s"), now.day, now.month, now.year);
	SIfprintf(report_fp, ERx("%59s%2s:%2s:%2s\n"), ERx(""), now.hour,
		now.mins, now.sec);
}
