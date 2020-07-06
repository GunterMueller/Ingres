# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<si.h>
# include	<st.h>
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include <ooclass.h>
# include	<cf.h>
/*{
** Name:	cf_writeobj - write object header to file if exists
**
** Description:
**	Writes the header to the intermediate file for one object.  
**	Retrieves object descriptive info from ii_objects and ii_longremarks
**	and incorporates that into the header.
**
** Input params:
**	class		object class of object for which to write header.
**	name		name of the object.
**	owner		owner of the object.
**	dbacode		user code of the dba.
**	fd		file descriptor of the open file to write to.
**	id		pointer to id to have object id returned in.
**			may be NULL if id or existence is not desired.
**
** Output params:
**	id		pointer to id to have object id returned in;
**			if object doesn't exist, *id is set to -1.
**
** Returns:
**	OK		No errors (object may not exist though).
**	FALSE		Error writing header to file.
**
** Exceptions:
**	none
**
** Side Effects:
**
** History:
**	22-Apr-87 (rdesmond) - written.
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
cf_writeobj(class, name, owner, dbacode, fd, id)
  i4 class;
  char *name;
  char *owner;
  char *dbacode;
FILE	*fd;
i4	*id;
{
  i4 temp_id;
  char short_remark[61];
  char long_remark[601];
  char obj_owner[FE_MAXNAME+1];
    char	*title;
    bool	found = FALSE;
    char	linebuf[1024];
    i4		count;
    switch(class)
    {
	case OC_QBFNAME:
		title = ERx("QBFNAME:");
		break;
	case OC_JOINDEF:
		title = ERx("JOINDEF:");
		break;
	case OC_FORM:
	default:
		title = ERx("FORM:");
		break;
    }
/* # line 91 "cfwrobj.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cfwrobj1",6468,4336);
      IIputdomio((short *)0,1,32,0,name);
      IIputdomio((short *)0,1,30,4,&class);
      IIputdomio((short *)0,1,32,0,owner);
      IIputdomio((short *)0,1,32,0,dbacode);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"cfwrobj1",6468,4336);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select object_id, short_remark, object_owner from ii_objects where ob\
ject_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_class=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&class);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,owner);
        IIwritio(0,(short *)0,1,32,0,(char *)"or object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,dbacode);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"cfwrobj1",6468,4336);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&temp_id);
      IIgetdomio((short *)0,1,32,60,short_remark);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,obj_owner);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 98 "cfwrobj.sc" */	/* host code */
	    STcopy(title, linebuf);
	    STcat(linebuf, ERx("\t"));
	    STcat(linebuf, name);
	    STcat(linebuf, ERx("\t"));
	    STcat(linebuf, short_remark);
	    found = TRUE;
	    if (STcompare(obj_owner, owner) == 0)
	    {
/* # line 106 "cfwrobj.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE1;
      }
/* # line 107 "cfwrobj.sc" */	/* host code */
	    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
    if(found == TRUE)
    {
	STcat(linebuf, ERx("\t"));
	long_remark[0] = '\0';
/* # line 113 "cfwrobj.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"cfwrobj2",6469,4601);
      IIputdomio((short *)0,1,30,4,&temp_id);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"cfwrobj2",6469,4601);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select long_remark from ii_longremarks where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&temp_id);
        IIexDefine(0,(char *)"cfwrobj2",6469,4601);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,600,long_remark);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 117 "cfwrobj.sc" */	/* host code */
	STcat(linebuf, long_remark);
	STcat(linebuf, ERx("\n"));
	if (SIwrite(STlength(linebuf), linebuf, &count, fd) != OK)
	    return (FAIL);
	if (id != NULL)
	    *id = temp_id;
    }
    else
    {
	if (id != NULL)
	    *id = 0;
    }
    return(OK);
}
