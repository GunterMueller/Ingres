# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**Copyright (c) 1987, 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<cv.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<lo.h>
# include	<si.h>
# include	<er.h>
# include	<uf.h>
# include	<ifid.h>
# include	"ilargs.h"
# include	"il.h"
/**
** Name:	iltmode.qsc	-   Routines that execute test mode.
**
** Description:
**
**	IIITtrmTestRunMode	Run the interpreter in testmode.
**
** History:
**	4-oct-1988 (Joe)
**		First Written
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
** Name:	<proc name>	- <short comment>
**
** Description:
**	<comments>
**
** Inputs:
**	<param name>	<input value description>
**
** Outputs:
**	<param name>	<output value description>
**
**	Returns:
**		<function return values>
**
**	Exceptions:
**		<exception codes>
**
** Side Effects:
**
** History:
**	<manual entries>
*/
  VOID
  IIITtrmTestRunMode(args)
ILARGS	*args;
  {
    char appl[FE_MAXNAME+1];
    char frameproc[FE_MAXNAME+1];
    char objname[FE_MAXNAME+1];
    char formname[FE_MAXNAME+1];
    i4 fidnumber;
    LOCATION	*loc;
    if ((loc = IIUFlcfLocateForm()) == NULL)
    {
	SIfprintf(stderr, "\r\nIIUFlcfLocateForm returns NULL\n");
	return;
    }
    if (IIUFgtfGetForm(loc, "IIITitfInternalTestForm") != OK)
    {
	SIfprintf(stderr, "\r\nCan't get form IIITitfInternalTestForm\n");
	return;
    }
/* # line 87 "iltmode.qsc" */	/* display */
    {
      if (IIdispfrm((char *)"IIITitfInternalTestForm",(char *)"f") == 0) goto 
      IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
            char whole_name[FE_MAXNAME+1];
            char shifted_name[FE_MAXNAME+1];
            i4 class;
/* # line 97 "iltmode.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,appl,(char *)
"application");
                IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,frameproc,(char *)"\
frameproc");
              } /* IIfsetio */
            }
/* # line 103 "iltmode.qsc" */	/* range */
            {
              IIwritio(0,(short *)0,1,32,0,(char *)"range of ai=ii_obj\
ects");
              IIsyncup((char *)0,0);
            }
/* # line 104 "iltmode.qsc" */	/* range */
            {
              IIwritio(0,(short *)0,1,32,0,(char *)"range of fi=ii_obj\
ects");
              IIsyncup((char *)0,0);
            }
/* # line 105 "iltmode.qsc" */	/* range */
            {
              IIwritio(0,(short *)0,1,32,0,(char *)
"range of fa=ii_abfobjects");
              IIsyncup((char *)0,0);
            }
/* # line 106 "iltmode.qsc" */	/* range */
            {
              IIwritio(0,(short *)0,1,32,0,(char *)
"range of ad=ii_abfdependencies");
              IIsyncup((char *)0,0);
            }
/* # line 107 "iltmode.qsc" */	/* retrieve */
            {
              IIwritio(0,(short *)0,1,32,0,(char *)
"retrieve(whole_name=ad.abfdef_name, shifted_name=shift(ad.abfdef_name\
, -3), class=ad.object_class)where ai.object_name=");
              IIputdomio((short *)0,1,32,0,appl);
              IIwritio(0,(short *)0,1,32,0,(char *)
"and ai.object_class=2001 and ai.object_id=fa.applid and fa.object_id=\
fi.object_id and fi.object_name=");
              IIputdomio((short *)0,1,32,0,frameproc);
              IIwritio(0,(short *)0,1,32,0,(char *)
"and fi.object_class=2210 and ad.object_id=fi.object_id and(ad.object_\
class=2010 or ad.object_class=3001)");
              IIretinit((char *)0,0);
              if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
              while (IInextget() != 0) {
                IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,whole_name);
                IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,shifted_name);
                IIgetdomio((short *)0,1,30,4,&class);
                if (IIerrtest() != 0) goto IIrtB1;
                {
/* # line 124 "iltmode.qsc" */	/* host code */
	    if (class == 2010)	/* This is the FID record */
	    {
		CVan(shifted_name, &fidnumber);
/* # line 127 "iltmode.qsc" */	/* putform */
                  {
                    if (IIfsetio((char *)0) != 0) {
                      IIputfldio((char *)"fid",(short *)0,1,30,4,&fidnumber);
                    } /* IIfsetio */
                  }
/* # line 131 "iltmode.qsc" */	/* host code */
	    }
	    else		/* This is the form record */
	    {
/* # line 134 "iltmode.qsc" */	/* putform */
                  {
                    if (IIfsetio((char *)0) != 0) {
                      IIputfldio((char *)"form",(short *)0,1,32,0,whole_name);
                    } /* IIfsetio */
                  }
/* # line 138 "iltmode.qsc" */	/* host code */
	    }
                }
              } /* IInextget */
              IIflush((char *)0,0);
IIrtE1:;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 143 "iltmode.qsc" */	/* host code */
	IIITiteIlTabEdit();
          }
        } else if (IIretval() == 3) {
          {
/* # line 147 "iltmode.qsc" */	/* host code */
        FID     fid;
	char	*formnameptr;
/* # line 150 "iltmode.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,objname,(char *)"\
frameproc");
                IIgetfldio((short *)0,1,30,4,&fidnumber,(char *)"fid");
                IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,formname,(char *)"\
form");
              } /* IIfsetio */
            }
/* # line 156 "iltmode.qsc" */	/* host code */
        fid.name = objname;
        fid.id = fidnumber;
	formnameptr = (formname[0] == EOS ? NULL : formname);
	if (formnameptr != NULL)
	{
/* # line 161 "iltmode.qsc" */	/* forminit */
            {
              IIforminit(formname);
            }
/* # line 162 "iltmode.qsc" */	/* host code */
	}
	/*
	** The ABF runtime system needs its stack set up.
	*/
	IIARctsCreateTestStack(objname);
        IIITeilExecuteIL(&fid, ERx("TestObject"), formnameptr, FALSE);
          }
        } else if (IIretval() == 4) {
          {
/* # line 171 "iltmode.qsc" */	/* enddisplay */
            {
              if (1) goto IIfdF1;
            }
          }
        } else {
          if (1) goto IIfdE1;
        } /* IIretval */
      } /* IIrunform */
IIfdF1:
      if (IIchkfrm() == 0) goto IIfdB1;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      goto IIfdE1;
IImuI1:
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact((char *)"Find",(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact((char *)"IlTab",(char *)0,2,2,2) == 0) goto IIfdE1;
      if (IInmuact((char *)"Go",(char *)0,2,2,3) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,3) == 0) goto IIfdE1;
      if (IInmuact((char *)"Quit",(char *)0,2,2,4) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(2,(char *)0,2,2,4) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
    {
    }
  }
