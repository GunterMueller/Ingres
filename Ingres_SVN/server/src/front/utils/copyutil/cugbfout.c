# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<cv.h>
# include	<st.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include <ooclass.h>
# include	<cu.h>
# include	<er.h>
# include	"ercu.h"
# include	<si.h>
# include	<uigdata.h>
/**
** Name:	cugbfout.c -    Copy a gbf graph out.
**
** Description:
**	Copies a gbf graph to a copyutil file.
**
**
** History:
**	30-Jul-1987 (Joe)
**		First Written
**	2-Feb-1988 (Joe)
**		Took out ranges and made queries repeats.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	13-May-2005 (kodse01)
**	    replace %ld with %d for old nat and long nat variables.
**/
/* # define's */
/* GLOBALDEF's */
/* extern's */
/* static's */
/*{
** Name:	IICUcgoCopyGbfgraphOut	-  Copy a gbf graph to a file.
**
** Description:
**	Given a gbf graph name this copies the gbf graph's
**	definition to a copyutil file.  The layout for the fields
**	of the different gbf graph tables is given in the CURECORD
**	for the gbf graph objects.
**
** Inputs:
**	name		The name of the gbf graph to copy out.
**
**	class		The class of the gbf graph to copy out.
**			This is not used in this routine.  We copy
**			out any gbf graph we find.
**
**	notthereok	If this is TRUE, then the gbf graph not being
**			there is not an error.  Otherwise it is an
**			error.
**
**	fp		The file the output to.
**
** Outputs:
**	Returns:
**		OK if succeeded.
**		If notthereok is false and the gbf graph is not present
**		returns a failure status.
**
** History:
**	30-jul-1987 (Joe)
**		Taken from 5.0 copyapp.
**	2-Feb-1988 (Joe)
**		Took out ranges and made queries repeats.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
*/
STATUS
IICUcgoCopyGbfgraphOut(name, class, notthereok, fp)
char	*name;
i4	class;
bool	notthereok;
FILE	*fp;
{
	bool	found = FALSE;		/* TRUE if graph found in catalogs */
  char t_graphname[13];
  char t_user[10];
  i4 g_gdate;
  char g_gtupcnt[14];
  i4 g_gvalcnt;
  i4 g_gseries;
  i4 g_gserlen;
  i4 g_glabels;
  i4 g_glbllen;
  i4 g_xtype;
  i4 g_ytype;
  i4 g_ztype;
  i4 c_gseqno;
  i4 c_gcontinue;
  char c_gmodule[13];
  char c_gcommand[13];
  i4 c_gint;
  f8 c_gflt;
  char c_gstr[256];
  char *user;
  char *dba;
	user = IIUIdbdata()->user;
	dba = IIUIdbdata()->dba;
	/* copy arguments into local variables, then force lowercase */
	STcopy(name, t_graphname);
	CVlower(t_graphname);
	/* read in the graph and, if found, write it to the file */
	if (STcompare(user, dba) < 0)
	{
/* # line 121 "cugbfout.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cugbfout1",6484,27387);
      IIputdomio((short *)0,1,32,0,t_graphname);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cugbfout1",6484,27387);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select gowner, gdate, gtupcnt, gvalcnt, gseries, gserlen, glabels, gl\
bllen, xtype, ytype, ztype from graphs where graphic=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,t_graphname);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by gowner ASC");
        IIexDefine(0,(char *)"cugbfout1",6484,27387);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,9,t_user);
      IIgetdomio((short *)0,1,30,4,&g_gdate);
      IIgetdomio((short *)0,1,32,13,g_gtupcnt);
      IIgetdomio((short *)0,1,30,4,&g_gvalcnt);
      IIgetdomio((short *)0,1,30,4,&g_gseries);
      IIgetdomio((short *)0,1,30,4,&g_gserlen);
      IIgetdomio((short *)0,1,30,4,&g_glabels);
      IIgetdomio((short *)0,1,30,4,&g_glbllen);
      IIgetdomio((short *)0,1,30,4,&g_xtype);
      IIgetdomio((short *)0,1,30,4,&g_ytype);
      IIgetdomio((short *)0,1,30,4,&g_ztype);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 132 "cugbfout.sc" */	/* host code */
	        SIfprintf(fp, ERx("CUO_GBF:\t%s\t%s\n"), t_graphname, t_user);
	        SIfprintf(fp, ERx("\t%s\t%s\t%d\t%ld\t%d\t%d\t%d\t%d"),
		    t_graphname, t_user, g_gdate, g_gtupcnt,
		    g_gvalcnt, g_gseries, g_gserlen, g_glabels);
	        SIfprintf(fp, ERx("\t%d\t%d\t%d\t%d\n"),
		    g_glbllen, g_xtype, g_ytype, g_ztype);
	        found = TRUE;
/* # line 139 "cugbfout.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE1;
      }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 141 "cugbfout.sc" */	/* host code */
	}
	else
	{
/* # line 144 "cugbfout.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cugbfout2",6485,27618);
      IIputdomio((short *)0,1,32,0,t_graphname);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cugbfout2",6485,27618);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select gowner, gdate, gtupcnt, gvalcnt, gseries, gserlen, glabels, gl\
bllen, xtype, ytype, ztype from graphs where graphic=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,t_graphname);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by gowner DESC");
        IIexDefine(0,(char *)"cugbfout2",6485,27618);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,9,t_user);
      IIgetdomio((short *)0,1,30,4,&g_gdate);
      IIgetdomio((short *)0,1,32,13,g_gtupcnt);
      IIgetdomio((short *)0,1,30,4,&g_gvalcnt);
      IIgetdomio((short *)0,1,30,4,&g_gseries);
      IIgetdomio((short *)0,1,30,4,&g_gserlen);
      IIgetdomio((short *)0,1,30,4,&g_glabels);
      IIgetdomio((short *)0,1,30,4,&g_glbllen);
      IIgetdomio((short *)0,1,30,4,&g_xtype);
      IIgetdomio((short *)0,1,30,4,&g_ytype);
      IIgetdomio((short *)0,1,30,4,&g_ztype);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 155 "cugbfout.sc" */	/* host code */
	        SIfprintf(fp, ERx("CUO_GBF:\t%s\t%s\n"), t_graphname, t_user);
	        SIfprintf(fp, ERx("\t%s\t%s\t%d\t%ld\t%d\t%d\t%d\t%d"),
		    t_graphname, t_user, g_gdate, g_gtupcnt,
		    g_gvalcnt, g_gseries, g_gserlen, g_glabels);
	        SIfprintf(fp, ERx("\t%d\t%d\t%d\t%d\n"),
		    g_glbllen, g_xtype, g_ytype, g_ztype);
	        found = TRUE;
/* # line 162 "cugbfout.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE2;
      }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
/* # line 164 "cugbfout.sc" */	/* host code */
	}
	if (FEinqerr() != 0)
	    return FEinqerr();
	if (!found)
	{
	    return (notthereok ? OK : FAIL);
	}
	cu_wrcomp(CUC_GCOMMANDS, fp);
		/* GCOMMANDS output line */
/* # line 173 "cugbfout.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cugbfout3",6486,31921);
      IIputdomio((short *)0,1,32,0,t_graphname);
      IIputdomio((short *)0,1,32,0,t_user);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cugbfout3",6486,31921);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select gseqno, gcontinue, gmodule, gcommand, gint, gflt, gstr from gc\
ommands where graphid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,t_graphname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and gowner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,t_user);
        IIexDefine(0,(char *)"cugbfout3",6486,31921);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&c_gseqno);
      IIgetdomio((short *)0,1,30,4,&c_gcontinue);
      IIgetdomio((short *)0,1,32,12,c_gmodule);
      IIgetdomio((short *)0,1,30,4,&c_gint);
      IIgetdomio((short *)0,1,31,8,&c_gflt);
      IIgetdomio((short *)0,1,32,255,c_gstr);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 180 "cugbfout.sc" */	/* host code */
	    SIfprintf(fp, ERx("\t%s\t%s\t%d\t%d\t%s\t%s\t%d\t%f\t%s\n"),
		    t_graphname, t_user, c_gseqno, c_gcontinue,
		    c_gmodule, c_gcommand, c_gint, c_gflt, c_gstr);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
	return FEinqerr();
}
