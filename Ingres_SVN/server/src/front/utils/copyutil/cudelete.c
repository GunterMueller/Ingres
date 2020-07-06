# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include <ooclass.h>
# include	<er.h>
# include	"ercu.h"
# include	<cu.h>
/**
** Name:	cudelete.sc -	Contains generic deletion routines.
**
** Description:
**	This files contains the routines that are used to
**	delete objects.
**
**	IICUdoDeleteObject	Delete most objects.
**	IICUdgDeleteGbfgraph	Delete gbf graph
**	IICUdaDeleteAppl	Delete application and its objects.
**
** History:
**	4-aug-1987 (Joe)
**		Initial Version
**	25-nov-1987 (peter)
**		Add delete of ii_longremarks.
**	2-feb-1988 (Joe)
**		Tooks out ranges, and made queries repeats.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**/
/*
**  This routine has been modified to use dynamic SQL when deleting an 
**  object.  The string variable into which the SQL DELETE statment is
**  to be prepared must be large enough to contain the following dynamic
**  DELETE:
**
**	STpolycat(5, ERx("DELETE FROM "), tabname, ERx(" WHERE "),
**		colname, ERx(" = ?"), stmt_buffer);
**
**  The following #DEFINE takes into account the length of the literal
**  strings and the maximum length of the string variables.
*/
#define	MAX_DEL_OBJ_LEN 12 + FE_MAXNAME + 7 + FE_MAXNAME + 10
/*{
** Name:	IICUdoDeleteObject - Delete an object.
**
** Description:
**	Given a class and an id this routine deletes all traces
**	of the object from the database.
**
** Inputs:
**	class		The class for the object.
**
**	id		The id of the object to delete.
**
** History:
**	4-aug-1987 (Joe)
**	25-nov-1987 (peter)
**		Add delete of ii_longremarks.
**	2-feb-1988 (Joe)
**		Tooks out ranges, and made queries repeats.
**	05-sep-88 (kenl)
**		Changed QUEL to SQL.
**	25-apr-1991 (jhw) - Added FAIL return for safety check on 'cu_recfor()'.
*/
STATUS
IICUdoDeleteObject ( class, id )
OOID	class;
  OOID id;
{
    CURECORD	*curec;
    CUDELLIST	*deltab;
  char *tabname;
  char *colname;
  char stmt_buffer[MAX_DEL_OBJ_LEN];
    CURECORD	*cu_recfor();
    if ((curec = cu_recfor(class)) == NULL)
    { /* unknown class */
	return FAIL;
    }
    if (curec->cudelete != NULL)
    {
	(*curec->cudelete)(id);
    }
    /* Delete the generic entries for the object */
/* # line 98 "cudelete.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cudelete1",6486,17588);
      IIputdomio((short *)0,1,30,4,&id);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cudelete1",6486,17588);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_objects where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIexDefine(0,(char *)"cudelete1",6486,17588);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 100 "cudelete.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cudelete2",6487,16829);
      IIputdomio((short *)0,1,30,4,&id);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cudelete2",6487,16829);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_longremarks where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIexDefine(0,(char *)"cudelete2",6487,16829);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 101 "cudelete.sc" */	/* host code */
    /* Now delete any specific records that need deleting */
    if (curec->cudeltab == NULL)
	return OK;
    for (deltab = curec->cudeltab; deltab->cutable != NULL; deltab++)
    {
	tabname = deltab->cutable;
	colname = deltab->cucolumn;
	/*
	** Can't make this a repeat because the table is changing
	*/
	STpolycat(5, ERx("DELETE FROM "), tabname, ERx(" WHERE "),
		colname, ERx(" = ?"), stmt_buffer);
/* # line 113 "cudelete.sc" */	/* prepare */
  {
    IIsqInit((char *)0);
    IIsqPrepare(0,(char *)"IICUdo_stmt",(char *)0,0,stmt_buffer);
  }
/* # line 114 "cudelete.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExStmt((char *)"IICUdo_stmt",1);
    IIputdomio((short *)0,1,30,4,&id);
    IIsyncup((char *)0,0);
  }
/* # line 115 "cudelete.sc" */	/* host code */
    }
    return FEinqerr();
}
/*{
** Name:	IICUdaDeleteAppl	- Delete an application and its objects.
**
** Description:
**	Deletes an application and its objects.
**
** Inputs:
**	id		The id of the application to delete.
**
** Outputs:
**	Returns:
**		OK
**
** History:
**	12-aug-1987 (Joe)
**		Initial Version
**	30-nov-1987 (Joe)
**		Added code to delete the encodings for
**		OSL frames in ii_encodings.
**		Also delete the long remarks.
**	2-feb-1988 (Joe)
**		Didn't make these query repeat since
**		it only gets called once (at this time)
*/
STATUS
IICUdaDeleteAppl(id)
  OOID id;
{
    /*
    ** Delete the encodings for an applications OSL
    ** frames and procedures.
    */
/* # line 153 "cudelete.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_encodings where object_id in(select o.object_id from i\
i_objects o, ii_abfdependencies d, ii_abfobjects a where o.object_name\
=d.abfdef_name and d.object_class=");
    IIputdomio((short *)0,1,30,sizeof(OC_ILCODE),(void *)IILQint(OC_ILCODE));
    IIwritio(0,(short *)0,1,32,0,(char *)
"and d.object_id=a.object_id and a.applid=");
    IIputdomio((short *)0,1,30,4,&id);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 162 "cudelete.sc" */	/* host code */
    if (FEinqerr() != 0)
	return FEinqerr();
    /*
    ** Delete the long remarks for the encodings for an applications OSL
    ** frames and procedures.
    */
/* # line 169 "cudelete.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_longremarks where object_id in(select o.object_id from\
 ii_objects o, ii_abfdependencies d, ii_abfobjects a where o.object_na\
me=d.abfdef_name and d.object_class=");
    IIputdomio((short *)0,1,30,sizeof(OC_ILCODE),(void *)IILQint(OC_ILCODE));
    IIwritio(0,(short *)0,1,32,0,(char *)
"and d.object_id=a.object_id and a.applid=");
    IIputdomio((short *)0,1,30,4,&id);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 178 "cudelete.sc" */	/* host code */
    if (FEinqerr() != 0)
	return FEinqerr();
    /*
    ** Delete the iiobject entries for the encodings for an applications OSL
    ** frames and procedures.
    */
/* # line 185 "cudelete.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_objects where object_name in(select d.abfdef_name from\
 ii_abfdependencies d, ii_abfobjects a where d.object_class=");
    IIputdomio((short *)0,1,30,sizeof(OC_ILCODE),(void *)IILQint(OC_ILCODE));
    IIwritio(0,(short *)0,1,32,0,(char *)
"and d.object_id=a.object_id and a.applid=");
    IIputdomio((short *)0,1,30,4,&id);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 192 "cudelete.sc" */	/* host code */
    if (FEinqerr() != 0)
	return FEinqerr();
    /*
    ** Delete the dependency records for the application
    */
/* # line 198 "cudelete.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_abfdependencies where object_id in(select object_id fr\
om ii_abfobjects where applid=");
    IIputdomio((short *)0,1,30,4,&id);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 202 "cudelete.sc" */	/* host code */
    if (FEinqerr() != 0)
	return FEinqerr();
    /*
    ** Delete the long remarks for the application and its
    ** objects.
    */
/* # line 209 "cudelete.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_longremarks where object_id in(select object_id from i\
i_abfobjects where applid=");
    IIputdomio((short *)0,1,30,4,&id);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 213 "cudelete.sc" */	/* host code */
    if (FEinqerr() != 0)
	return FEinqerr();
    /*
    ** Delete the object entries for the objects in an
    ** application.
    */
/* # line 220 "cudelete.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_objects where object_id in(select object_id from ii_ab\
fobjects where applid=");
    IIputdomio((short *)0,1,30,4,&id);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIsyncup((char *)0,0);
  }
/* # line 224 "cudelete.sc" */	/* host code */
    if (FEinqerr() != 0)
	return FEinqerr();
/* # line 226 "cudelete.sc" */	/* delete */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_abfobjects where applid=");
    IIputdomio((short *)0,1,30,4,&id);
    IIsyncup((char *)0,0);
  }
/* # line 227 "cudelete.sc" */	/* host code */
    return FEinqerr();
}
