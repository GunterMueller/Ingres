# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<cv.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<st.h>
# include	<fe.h>
# include	<stype.h>
# include	<sglob.h>
# include	<lo.h>
# include <ui.h>
# include	<ug.h>
# include	<er.h>
# include	"ersr.h"
/*{
** Name:	s_copy_new() -	Copy Report Commands to Database.
**
** Description:
**   S_COPY_NEW - copy in the report to the database from the Command
**	file, and appends the Report data.
**
** Parameters:
**	none.
**
** Returns:
**	none.
**
** Side Effects:
**	Adds the reports to the database.
**
** Called by:
**	s_main.
**
** Error Messages:
**	Syserr: Bad close of file.
**
** Trace Flags:
**	11.0, 11.7.
**
** History:
**	6/4/81 (ps)	written.
**	12/22/81 (ps)	modified to two table version.
**	2/13/84 (gac)	added multi-statement transactions.
**	6/14/84 (gac)	added network stuff for equel copy.
**	5/08/86 (jhw)	Removed 'En_temp_loc' since it was used incorrectly.
**	23-oct-1987 (peter)
**		Get rid of INnetrequest.
**	10-aug-1988 (danielt)
**		SQL conversion ofDB access.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**  07/24/89 (martym)
**		Fixed bug #6883(#6879), by modifying error 902 calls
**      to send in a string of blanks if En_sfile happens to
**      be null.
**	12/21/89 (elein)
**		Changed r_syserr to use file name NOT file pointer
**	09-jul-90 (cmr) fix for bug #30813
**		Moved CatWriteOn/Off calls to main instead of scattered in
**		other routines.  RBF and sreport were not detecting DBMS
**		errors or deadlock (CatWriteOff() resets the errorno).
**	15-oct-90 (steveh) part of fix for bug 32954
**		call make_En_rcommands to insure that En_rcommands has been
**		initialized.
**	19-oct-90 (steveh) call to make_En_rcommands should be placed
**		above close of file.
**	26-aug-1992 (rdrane)
**		Converted r_error() err_num value to hex to facilitate
**		lookup in errw.msg file. Eliminate references to
**		r_syserr() and use IIUGerr() directly.  Tag parse_fields()
**		as returning VOID.
**	
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
  typedef struct {
    i4 cid;
    char ctype[3];
    char cattid[33];
    i4 csequence;
    char ccommand[13];
    char ctext[101];
    char csection[13];
  } RCOMM_ROW;
static  VOID	parse_fields();
VOID
s_copy_new()
{
	bool	deadlock();
	extern	bool	ing_deadlock;
	/* internal declarations */
  char *fname;
	char	net_name[MAX_LOC];
	/* first add the data to the REPORTS table */
	s_app_reports();
	if (ing_deadlock)
		return;
	/* add in the rows to the RCOMMAND table */
	/* Insure that En_rcommands has been initialized.
	   this fixes bug 32954. If the input file is empty
	   then s_w_row will not be called and En_rcommands
	   will not be initialized. Calling make_En_rcommands
	   insures initialization.  (steveh)                */
	make_En_rcommands();
	if (St_rco_open)
	{
		if (r_fclose(Fl_rcommand) < 0)
		{
			IIUGerr(E_SR0004_s_copy_new_Bad_close_,UG_ERR_FATAL,
				1,En_rcommands);
		}
		St_rco_open = FALSE;
	}
	/*
	** Removed much about putting a node on for network
	** access here.
	*/
	fname = En_rcommands;
	if (IIUIdml() == UI_DML_GTWSQL)
	{
		IISRsr_saveRcommands();
	}
	else
	{
/* # line 138 "scopynew.sc" */	/* copy table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"copy table ii_rcommands(object_id=c0tab, rcotype=c0tab, rcosequence=c\
0tab, rcosection=c0tab, rcoattid=c0tab, rcocommand=c0tab, rcotext=c0nl\
)from ");
    IIvarstrio((short *)0,1,32,0,fname);
    IIsyncup((char *)0,0);
  }
/* # line 147 "scopynew.sc" */	/* host code */
	}
	if (!deadlock())
		if (!St_keep_files)
			r_fdelete(En_rcommands);
	return;
}
STATUS
IISRsr_saveRcommands()
{
	char 	line[MAXFLINE + 1];
  i4 ret_code = OK;
  RCOMM_ROW rcomm;
	/* read commands from temporary file created by SREPORT routines */
	if (r_fopen(En_rcommands, &Fl_rcommand) < 0)
	{       /* bad file */
		if (En_sfile == NULL)
			r_error(0x386, FATAL, " ", NULL);	/* ABORT!!!*/
		else
			r_error(0x386, FATAL, En_sfile, NULL);	/* ABORT!!!*/
	}
	while (r_readln(Fl_rcommand, line, MAXFLINE) > 0)
	{
		parse_fields(line, &rcomm);
/* # line 172 "scopynew.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"scopynew1",7367,25917);
      IIputdomio((short *)0,1,30,4,&rcomm.cid);
      IIputdomio((short *)0,1,32,0,rcomm.ctype);
      IIputdomio((short *)0,1,30,4,&rcomm.csequence);
      IIputdomio((short *)0,1,32,0,rcomm.csection);
      IIputdomio((short *)0,1,32,0,rcomm.cattid);
      IIputdomio((short *)0,1,32,0,rcomm.ccommand);
      IIputdomio((short *)0,1,32,0,rcomm.ctext);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"scopynew1",7367,25917);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_rcommands(object_id, rcotype, rcosequence, rcosection,\
 rcoattid, rcocommand, rcotext)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&rcomm.cid);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,rcomm.ctype);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&rcomm.csequence);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,rcomm.csection);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,rcomm.cattid);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,rcomm.ccommand);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,rcomm.ctext);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"scopynew1",7367,25917);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 188 "scopynew.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&ret_code,2);
  }
/* # line 189 "scopynew.sc" */	/* host code */
		if (ret_code != OK)
			break;
	}
	r_fclose(Fl_rcommand);
	return (ret_code);
}
static 
VOID
parse_fields(line, rcomm_ptr)
char		*line;
RCOMM_ROW	*rcomm_ptr;
{
    char	*start;
    i4		seq;
    start = line;
    line = STindex(line, F_DELIM, 0);
    *line++ = EOS;
    CVan(start, &(rcomm_ptr->cid));
    start = line;
    line = STindex(line, F_DELIM, 0);
    *line++ = EOS;
    STcopy(start, rcomm_ptr->ctype);
    start = line;
    line = STindex(line, F_DELIM, 0);
    *line++ = EOS;
    CVan(start, &(seq));
    rcomm_ptr->csequence = seq;
    start = line;
    line = STindex(line, F_DELIM, 0);
    *line++ = EOS;
    STcopy(start, rcomm_ptr->csection);
    start = line;
    line = STindex(line, F_DELIM, 0);
    *line++ = EOS;
    STcopy(start, rcomm_ptr->cattid);
    start = line;
    line = STindex(line, F_DELIM, 0);
    *line++ = EOS;
    STcopy(start, rcomm_ptr->ccommand);
    STcopy(line, rcomm_ptr->ctext);
    return;
}
