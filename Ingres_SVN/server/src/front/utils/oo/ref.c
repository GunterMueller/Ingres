# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include <ooclass.h>
# include	<oodefine.h>
# include	"ooldef.h"
# include <oosymbol.h>
# include       <er.h>
# include	"eroo.h"
/**
** History:
**      22-oct-90 (sandyd)
**              Fixed #include of local ooldef.h to use "" instead of <>.
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
   OO_REFERENCE*
  REinitDb(self, name, rem, master, mdel, conn, detail, ddel)
   OO_REFERENCE *self;
  char *name, *rem;
   OOID master, detail;
   i4	mdel, ddel;
   char *conn;
  {
    i4 anyOther;
    char *O1table = O1->table_name;
    char *O2table = O2->table_name;
    char *O3table = O3->table_name;
/* # line 40 "ref.qsc" */	/* range */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"range of attribute=attribu\
te");
      IIsyncup((char *)0,0);
    }
/* # line 42 "ref.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(anyOther=any(");
      IIwritio(1,(short *)0,1,32,0,O3table);
      IIwritio(0,(short *)0,1,32,0,(char *)".id where ");
      IIwritio(1,(short *)0,1,32,0,O3table);
      IIwritio(0,(short *)0,1,32,0,(char *)".master=master and ");
      IIwritio(1,(short *)0,1,32,0,O3table);
      IIwritio(0,(short *)0,1,32,0,(char *)".id=");
      IIwritio(1,(short *)0,1,32,0,O1table);
      IIwritio(0,(short *)0,1,32,0,(char *)".id and ");
      IIwritio(1,(short *)0,1,32,0,O1table);
      IIwritio(0,(short *)0,1,32,0,(char *)".name=");
      IIputdomio((short *)0,1,32,0,name);
      IIwritio(0,(short *)0,1,32,0,(char *)"))");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&anyOther);
        if (IIerrtest() != 0) goto IIrtB1;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE1:;
    }
/* # line 45 "ref.qsc" */	/* host code */
	if (anyOther) {
		err(E_OO003A_ReferenceAlreadyExist, name,
			OOsnd(master, _name));
		return NULL;
	}
/* # line 50 "ref.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"retrieve(anyOther=any(");
      IIwritio(1,(short *)0,1,32,0,O3table);
      IIwritio(0,(short *)0,1,32,0,(char *)".id where ");
      IIwritio(1,(short *)0,1,32,0,O3table);
      IIwritio(0,(short *)0,1,32,0,(char *)".master=master and ");
      IIwritio(1,(short *)0,1,32,0,O3table);
      IIwritio(0,(short *)0,1,32,0,(char *)".id=");
      IIwritio(1,(short *)0,1,32,0,O2table);
      IIwritio(0,(short *)0,1,32,0,(char *)".id and ");
      IIwritio(1,(short *)0,1,32,0,O2table);
      IIwritio(0,(short *)0,1,32,0,(char *)
".table=attribute.attrelid and attribute.attowner=usercode and attribu\
te.attname=");
      IIputdomio((short *)0,1,32,0,name);
      IIwritio(0,(short *)0,1,32,0,(char *)"))");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&anyOther);
        if (IIerrtest() != 0) goto IIrtB2;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE2:;
    }
/* # line 55 "ref.qsc" */	/* host code */
	if (anyOther) {
		err(E_OO003B_AttributeAlreadyExist, name,
			OOsnd(master, _name));
		return NULL;
	}
	return (OO_REFERENCE*)OOsndSelf(self, _init, name, rem, master, mdel, conn, detail, ddel);
  }
OO_REFERENCE*
REinit(self, name, rem, master, mdel, conn, detail, ddel)
OO_REFERENCE *self;
char	*name, *rem;
OOID	master, detail;
i4	mdel, ddel;
char	*conn;
{
	OOsndSuper(self, _init, name, 0, _, 1, rem, NULL, NULL, NULL);
	self->master = master;
	self->mdelete = mdel;
	self->connector = conn;
	self->detail = detail;
	self->ddelete = ddel;
	return self;
}
