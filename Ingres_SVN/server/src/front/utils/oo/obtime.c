# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<si.h>		 
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<tm.h>
# include <ooclass.h>
# include	<oodefine.h>
# include <oosymbol.h>
# include	<er.h>
/*
**	17-aug-91 (leighb) DeskTop Porting Change: must decl all 10 PTR's
**      18-oct-1993 (kchin)
**          	Since the return type of OOsndSelf() has been changed to PTR,
**          	when being called in OBtime(), its return
**          	type needs to be cast to the proper datatype.
*/
OOID
  OBtime(self, msg, a1, a2,a3,a4,a5,a6,a7,a8,a9,a10)
  register OO_OBJECT *self;
  register char *msg;
  PTR a1, a2, a3, a4, a5, a6, a7, a8, a9, a10;
  {
	register i4	rtime;
	register OOID	ret;
    i4 et1, et2, pf1, pf2;
    i4 cpu1, cpu2, dio1, dio2;
	rtime = TMsecs();
/* # line 37 "obtime.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(et1=_et_sec, cpu1=_cpu_ms, dio1=_dio_cnt, pf1=_pfault_cnt)");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&et1);
        IIgetdomio((short *)0,1,30,4,&cpu1);
        IIgetdomio((short *)0,1,30,4,&dio1);
        IIgetdomio((short *)0,1,30,4,&pf1);
        if (IIerrtest() != 0) goto IIrtB1;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE1:;
    }
/* # line 38 "obtime.qsc" */	/* host code */
	ret = (OOID)OOsndSelf(self, msg, a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,0);
	rtime = TMsecs() - rtime;
/* # line 41 "obtime.qsc" */	/* retrieve */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(et2=_et_sec, cpu2=_cpu_ms, dio2=_dio_cnt, pf2=_pfault_cnt)");
      IIretinit((char *)0,0);
      if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
      while (IInextget() != 0) {
        IIgetdomio((short *)0,1,30,4,&et2);
        IIgetdomio((short *)0,1,30,4,&cpu2);
        IIgetdomio((short *)0,1,30,4,&dio2);
        IIgetdomio((short *)0,1,30,4,&pf2);
        if (IIerrtest() != 0) goto IIrtB2;
      } /* IInextget */
      IIflush((char *)0,0);
IIrtE2:;
    }
/* # line 42 "obtime.qsc" */	/* host code */
	et2 -= et1;
	cpu2 -= cpu1;
	dio2 -= dio1;
	pf2 -= pf1;
/* # line 47 "obtime.qsc" */	/* append */
    {
      IIwritio(0,(short *)0,1,32,0,(char *)"append iiperform(stamp=");
      IIputdomio((short *)0,1,32,0,ERx("now"));
      IIwritio(0,(short *)0,1,32,0,(char *)", id=");
      IIputdomio((short *)0,1,30,4,&self->ooid);
      IIwritio(0,(short *)0,1,32,0,(char *)", class=");
      IIputdomio((short *)0,1,30,4,&self->class);
      IIwritio(0,(short *)0,1,32,0,(char *)", op=");
      IIputdomio((short *)0,1,32,0,msg);
      IIwritio(0,(short *)0,1,32,0,(char *)", et=");
      IIputdomio((short *)0,1,30,4,&et2);
      IIwritio(0,(short *)0,1,32,0,(char *)", cpu=");
      IIputdomio((short *)0,1,30,4,&cpu2);
      IIwritio(0,(short *)0,1,32,0,(char *)", dio=");
      IIputdomio((short *)0,1,30,4,&dio2);
      IIwritio(0,(short *)0,1,32,0,(char *)", pf=");
      IIputdomio((short *)0,1,30,4,&pf2);
      IIwritio(0,(short *)0,1,32,0,(char *)")");
      IIsyncup((char *)0,0);
    }
/* # line 58 "obtime.qsc" */	/* host code */
	SIprintf(ERx("real=%d, et=%d, cpu=%d, dio=%d, pf=%d\n"),
		rtime, et2, cpu2, dio2, pf2);
	return ret;
  }
