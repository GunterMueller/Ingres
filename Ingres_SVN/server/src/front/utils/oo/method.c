# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include <ooclass.h>
# include	<oodefine.h>
# include	"ooldef.h"
# include <oosymbol.h>
# include	"eroo.h"
#include        <ercl.h>
/**
** History:
**      22-oct-90 (sandyd)
**              Fixed #include of local ooldef.h to use "" instead of <>.
**      18-oct-1993 (kchin)
**          	Since the return type of OOsndSelf() has been changed to PTR,
**          	when being called in MinitDb(), its return type needs to be 
**		cast to the proper datatype.
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
OOID
  MinitDb(self, name, rem, mclass, proc, argcount, perm, keep, flevel)
   OO_METHOD	*self;
  char *name, *rem;
  OOID mclass;
   char		*proc;
   i4		argcount, perm, keep, flevel;
  {
    i4 tnum, anyOther;
    char *O1table = O1->table_name;
    char *O4table = O4->table_name;
  i4 tmpid;
/* # line 50 "method.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"method1",6311,5868);
      IIputdomio((short *)0,1,30,4,&mclass);
      IIputdomio((short *)0,1,32,0,name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"method1",6311,5868);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select iimethod.id from iimethod, ii_objects where iimethod.mclass=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&mclass);
        IIwritio(0,(short *)0,1,32,0,(char *)"and ii_objects.object_na\
me=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,name);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and ii_objects.object_id=iimethod.id");
        IIexDefine(0,(char *)"method1",6311,5868);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&tmpid);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 57 "method.sc" */	/* host code */
	    anyOther = 1;
/* # line 58 "method.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE1;
      }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 60 "method.sc" */	/* host code */
	if (anyOther) {
		err(E_OO001E_Method_already_exists, name,
			OOsnd(mclass, _name));
		return NULL;
	}
	return ((OOID)OOsndSelf(self, _init, name, rem, mclass, proc, argcount, 
				perm, keep, flevel));
  }
OOID
Minit(self, name, rem, mclass, proc, argcount, perm, keep, flevel)
OO_METHOD *self;
char	*name, *rem;
OOID	mclass;
char	*proc;
i4	argcount, perm, keep, flevel;
{
	OOsndSuper(self, _init, name, 0, _, 1, rem, NULL, NULL, NULL);
	self->mclass = mclass;
	self->procname = proc;
	self->argcount = argcount;
	self->defaultperm = perm;
	self->keepstatus = keep;
	self->fetchlevel = flevel;
	return self->ooid;
}
