# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*	
**	Copyright (c) 1986, 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<er.h>
# include	<tm.h>
# include	"iamstd.h"
# include	"eram.h"
# include	<si.h>
/**
** Name:	iamfchk.c - frame existence check
**
** Description:
**	Frame existence check routine
**
*/
/*{
** Name:	IIAMfeFrmExist
**
** Description:
**	IAOM procedure to determine the existence of a frame.  Should
**	not be called by routines called from ILRF as it is too costly.	 These
**	routines will limit themselves directly to the ii_encodings table.
**
** Inputs:
**	fid	frame to check
**
** Outputs:
**	t	frame time stamp, if applicable.  NULL pointer may be passed
**		in if not needed.
**
**	return:
**		ILE_FRDWR - INGRES failure
**		ILE_FIDBAD - frame does not exist.
**		ILE_ILMISSING - exists in ii_objects, but not in ii_encodings.
**		OK - exists, and is encoded.
**
** Side effects:
**	produces non-fatal error messages when inconsistencies are detected
**	in the tables.
**
** History
**	8/86 (rlm)	written
**	18-aug-88 (kenl)
**		Changed QUEL to SQL.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
STATUS
IIAMfeFrmExist (fid,t)
  i4 fid;
SYSTIME *t;
{
  i4 err;
  i4 rows;
  char adate[80];
        IIUIcnb_CatNolockBegin(ERx("ii_objects"));
/* # line 73 "iamfchk.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select alter_date from ii_objects where object_id=");
    IIputdomio((short *)0,1,30,4,&fid);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,79,adate);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 77 "iamfchk.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
    IILQisInqSqlio((short *)0,1,30,4,&rows,8);
  }
/* # line 78 "iamfchk.sc" */	/* host code */
	/*
	** XXX Why is this call here?  This question occasioned a long debate,
	** accompanied by a lengthy and often-amusing history.  Now, everyone's
	** best guess is that this is unneccessary.  Also, for many stupid
	** reasons, it fails on STAR.  So, it's commented out and should 
	** probably be removed.
	**
	** iiuicr0_CatReadOff(ERx("ii_objects"));
	*/
	if (err != 0)
		return (ILE_FRDWR);
	if (rows <= 0)
		return (ILE_FIDBAD);
	if (rows > 1)
		IIUGerr(E_AM0015_objects_corrupt, 0, 2, (PTR)&rows, (PTR)&fid);
	rows = 0;
	if (t != NULL)
		IIUGdtsDateToSys(adate,t);
	/* 
	** XXX the following call is to a no-op routine (body of code
	** commented out.)  This is related to the CatReadOff problem.
	*/
	IIUIcne_CatNolockEnd(ERx("ii_encodings"));
/* # line 101 "iamfchk.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(distinct encode_object)from ii_encodings where encode_ob\
ject=");
    IIputdomio((short *)0,1,30,4,&fid);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&rows);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 105 "iamfchk.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 106 "iamfchk.sc" */	/* host code */
	/*
	** XXX see 'XXX' comments above.
	** iiuicr0_CatReadOff(ERx("ii_encodings"));
	*/
	if (err != 0)
		return (ILE_FRDWR);
	if (rows == 0)
		return (ILE_ILMISSING);
	return (OK);
}
/*{
** Name:	fid_touch
**
** Description:
**	IAOM internal procedure to touch the date on a frame.
**
** Inputs:
**	fid	frame to touch
**	st	modify date of source file, to correct for file-server /
**		host clock synch problems.
**
** Outputs:
**
**	none.
**
** History
**	10/86 (rlm)	written
*/
fid_touch (fid, st)
  i4 fid;
SYSTIME *st;
{
  char nowstr[40];
	SYSTIME nst;
	/*
	** if "now" is behind source file time, our clock is clearly
	** out of synch with the file server clock.  We also subtract 10
	** minutes before comparing to catch cases where we are less
	** than 10 minutes later - small clock discrepencies in this
	** direction can cause problems, also (especially for emerald
	** type frames which are often generated, then immediately
	** compiled).  In these cases, we simply stamp with the source
	** file date plus 1 second, which will assure proper functioning
	** of date comparisons, at the expense of actually having an accurate
	** date.
	*/
	TMnow(&nst);
	nst.TM_secs -= 600;
	if (TMcmp(&nst,st) < 0)
	{
		STRUCT_ASSIGN_MACRO(*st,nst);
		++(nst.TM_secs);
	}
	else
		nst.TM_secs += 600;
	IIUGdfsDateFromSys(&nst,nowstr);
	iiuicw1_CatWriteOn();
/* # line 167 "iamfchk.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_objects set alter_date=");
    IIputdomio((short *)0,1,32,0,nowstr);
    IIwritio(0,(short *)0,1,32,0,(char *)"where object_id=");
    IIputdomio((short *)0,1,30,4,&fid);
    IIsyncup((char *)0,0);
  }
/* # line 170 "iamfchk.sc" */	/* host code */
	iiuicw0_CatWriteOff();
}
