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
# include <ooclass.h>
# include	<cu.h>
# include	<er.h>
# include	"ercu.h"
# include	<si.h>
/**
** Name:	cujdout.c -    Copy a joindef out.
**
** Description:
**	Copies a joindef to a copyutil file.
**
**
** History:
**	30-Jul-1987 (Joe)
**		First Written
**	2-Feb-1988 (Joe)
**		Took out ranges, and made queries repeats
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
** Name:	IICUcjoCopyJoindefOut	-  Copy a joindef to a file.
**
** Description:
**	Given a joindef name this copies the joindef's
**	definition to a copyutil file.  The layout for the fields
**	of the different joindef tables is given in the CURECORD
**	for the joindef objects.
**
** Inputs:
**	name		The name of the joindef to copy out.
**
**	class		The class of the joindef to copy out.
**			This is not used in this routine.  We copy
**			out any joindef we find.
**
**	notthereok	If this is TRUE, then the joindef not being
**			there is not an error.  Otherwise it is an
**			error.
**
**	fp		The file the output to.
**
** Outputs:
**	Returns:
**		OK if succeeded.
**		If notthereok is false and the joindef is not present
**		returns a failure status.
**
** History:
**	30-jul-1987 (Joe)
**		First Written
**	2-Feb-1988 (Joe)
**		Took out ranges, and made queries repeats
**	05-sep-88 (kenl)
**		Changed QUEL to SQL.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
*/
STATUS
IICUcjoCopyJoindefOut(name, class, notthereok, fp)
char	*name;
i4	class;
bool	notthereok;
FILE	*fp;
{
  i4 jdefid;
  i4 qtype;
  char qinfo1[FE_MAXNAME+1];
  char qinfo2[FE_MAXNAME+1];
  char qinfo3[FE_MAXNAME+1];
  char qinfo4[FE_MAXNAME+1];
    STATUS	rval;
	/*
	** Write header records for object.
	*/
	if ((rval = cu_writeobj(OC_JOINDEF, 0, name, fp, &jdefid)) != OK)
	{
	    return (notthereok ? OK : rval);
	}
/* # line 101 "cujdout.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cujdout1",6484,6907);
      IIputdomio((short *)0,1,30,4,&jdefid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cujdout1",6484,6907);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select qtype, qinfo1, qinfo2, qinfo3, qinfo4 from ii_joindefs where o\
bject_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&jdefid);
        IIexDefine(0,(char *)"cujdout1",6484,6907);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&qtype);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,qinfo1);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,qinfo2);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,qinfo3);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,qinfo4);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 108 "cujdout.sc" */	/* host code */
	    SIfprintf(fp, ERx("\t%d\t%s\t%s\t%s\t%s\n"),
		qtype,
		qinfo1,
		qinfo2,
		qinfo3,
		qinfo4);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	return FEinqerr();
}
