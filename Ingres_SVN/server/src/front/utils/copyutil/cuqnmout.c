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
# include	<si.h>
# include	<er.h>
# include	"ercu.h"
/**
** Name:	cuqnmout.c -    Copy a qbfname out.
**
** Description:
**	Copies a qbfname to a copyutil file.
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
** Name:	IICUcqoCopyQbfnameOut	-  Copy a qbfname to a file.
**
** Description:
**	Given a qbfname name this copies the qbfname's
**	definition to a copyutil file.  The layout for the fields
**	of the different qbfname tables is given in the CURECORD
**	for the qbfname objects.
**
** Inputs:
**	name		The name of the qbfname to copy out.
**
**	class		The class of the qbfname to copy out.
**			This is not used in this routine.  We copy
**			out any qbfname we find.
**
**	notthereok	If this is TRUE, then the qbfname not being
**			there is not an error.  Otherwise it is an
**			error.
**
**	fp		The file the output to.
**
** Outputs:
**	Returns:
**		OK if succeeded.
**		If notthereok is false and the qbfname is not present
**		returns a failure status.
**
** History:
**	30-jul-1987 (Joe)
**		First Written
**	2-Feb-1988 (Joe)
**		Took out ranges and made queries repeats.
*/
STATUS
IICUcqoCopyQbfnameOut(name, class, notthereok, fp)
char	*name;
i4	class;
bool	notthereok;
FILE	*fp;
{
  i4 qnmid;
  char relname[FE_MAXNAME+1];
  char frname[FE_MAXNAME+1];
  i4 qbftype;
    STATUS	rval;
	/*
	** Write header records for object.
	*/
	if ((rval = cu_writeobj(OC_QBFNAME, 0, name, fp, &qnmid)) != OK)
	{
	    return (notthereok ? OK : rval);
	}
/* # line 95 "cuqnmout.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cuqnmout1",6483,14332);
      IIputdomio((short *)0,1,30,4,&qnmid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cuqnmout1",6483,14332);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select relname, frname, qbftype from ii_qbfnames where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&qnmid);
        IIexDefine(0,(char *)"cuqnmout1",6483,14332);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,relname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,frname);
      IIgetdomio((short *)0,1,30,4,&qbftype);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 100 "cuqnmout.sc" */	/* host code */
	    SIfprintf(fp, ERx("\t%s\t%s\t%d\n"),
		relname,
		frname,
		qbftype);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	return FEinqerr();
}
