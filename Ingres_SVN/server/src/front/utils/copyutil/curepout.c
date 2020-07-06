# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<st.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include <ooclass.h>
# include	<cu.h>
# include	<si.h>
# include	<er.h>
# include	"ercu.h"
/**
** Name:	curepout.sc -    Copy a report out.
**
** Description:
**	Copies a report to a copyutil file.
**
**
** History:
**	30-Jul-1987 (Joe)
**		First Written
**	2-Feb-1988 (Joe)
**		Took out ranges and made queries repeats.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**      11-apr-95 (kch)
**            Added repbcount (number of breaks in report) to be copied
**            to file. This change fixes bug 56115.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/* # define's */
/* GLOBALDEF's */
/* extern's */
/* static's */
/*{
** Name:	IICUcroCopyReportOut	-  Copy a report to a file.
**
** Description:
**	Given a report name and its claass, this copies the report's
**	definition to a copyutil file.  The layout for the fields
**	of the different report writer tables is given in the CURECORD
**	for the report objects.
**
** Inputs:
**	name		The name of the report to copy out.
**
**	class		The class of the report to copy out.
**			This is not used in this routine.  We copy
**			out any report we find.
**
**	notthereok	If this is TRUE, then the report not being
**			there is not an error.  Otherwise it is an
**			error.
**
**	fp		The file the output to.
**
** Outputs:
**	Returns:
**		OK if succeeded.
**		If notthereok is false and the report is not present
**		returns a failure status.
**
** History:
**	30-jul-1987 (Joe)
**		First Written
**	2-Feb-1988 (Joe)
**		Took out ranges and made queries repeats.
*/
STATUS
IICUcroCopyReportOut(name, class, notthereok, fp)
char	*name;
i4	class;
bool	notthereok;
FILE	*fp;
{
  char r_reptype[2];
  i4 r_repacount;
  i4 r_repscount;
  i4 r_repqcount;
  i4 r_repbcount;
  char c_rcotype[3];
  i4 c_rcoseq;
  char c_rcosection[13];
  char c_rcoattid[FE_MAXNAME+1];
  char c_rcocommand[13];
  char c_rcotext[101];
  i4 repid;
	STATUS	rval;
	/*
	** Write header records for object.
	*/
	if ((rval = cu_writeobj(OC_REPORT, 0, name, fp, &repid)) != OK)
	{
	    return (notthereok ? OK : rval);
	}
	/*
	** Write out detail lines for ii_reports.
	*/
/* # line 113 "curepout.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"curepout1",6482,9837);
      IIputdomio((short *)0,1,30,4,&repid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"curepout1",6482,9837);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select reptype, repacount, repscount, repqcount, repbcount from ii_re\
ports where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&repid);
        IIexDefine(0,(char *)"curepout1",6482,9837);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,1,r_reptype);
      IIgetdomio((short *)0,1,30,4,&r_repacount);
      IIgetdomio((short *)0,1,30,4,&r_repscount);
      IIgetdomio((short *)0,1,30,4,&r_repqcount);
      IIgetdomio((short *)0,1,30,4,&r_repbcount);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 120 "curepout.sc" */	/* host code */
            SIfprintf(fp, ERx("\t%s\t%d\t%d\t%d\t%d\n"),
			r_reptype,
			r_repacount,
			r_repscount,
                        r_repqcount,
                        r_repbcount);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	if (FEinqerr() != 0)
	    return FEinqerr();
		/* RCOMMANDS output line */
	cu_wrcomp(CUC_RCOMMANDS, fp);
/* # line 131 "curepout.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"curepout2",6483,11898);
      IIputdomio((short *)0,1,30,4,&repid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"curepout2",6483,11898);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select rcotype, rcosequence, rcosection, rcoattid, rcocommand, rcotex\
t from ii_rcommands where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&repid);
        IIexDefine(0,(char *)"curepout2",6483,11898);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,2,c_rcotype);
      IIgetdomio((short *)0,1,30,4,&c_rcoseq);
      IIgetdomio((short *)0,1,32,12,c_rcosection);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,c_rcoattid);
      IIgetdomio((short *)0,1,32,12,c_rcocommand);
      IIgetdomio((short *)0,1,32,100,c_rcotext);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 140 "curepout.sc" */	/* host code */
		STtrmwhite(c_rcotype);
		STtrmwhite(c_rcosection);
		STtrmwhite(c_rcoattid);
		STtrmwhite(c_rcocommand);
		STtrmwhite(c_rcotext);
		SIfprintf(fp, ERx("\t%s\t%d\t%s\t%s\t%s\t%s\n"),
			c_rcotype,
			c_rcoseq,
			c_rcosection,
			c_rcoattid,
			c_rcocommand,
			c_rcotext);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
	return FEinqerr();
}
