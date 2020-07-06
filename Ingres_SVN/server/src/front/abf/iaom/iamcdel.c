# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 1987, 2004 Ingres Corporation
*/
#include	<compat.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ui.h>
# include <ooclass.h>
#include	"iamint.h"
/**
** Name:	iamcdel.sc -	Application Catalog Object Delete Module.
**
** Description:
**	Delete catalog record for an ABF object.  Defines:
**
**	IIAMcdCatDel()	delete application catalog rows.
**
** History:
**	Revision 6.2  89/01  wong
**	Merged queries and added IL encodings delete with 'class' parameter.
**
**	Revision 6.0  87/05  bobm
**	Initial revision.
*/
STATUS abort_it();
/*{
** Name:	IIAMcdCatDel() -	Delete Application Catalog Rows.
**
** Description:
**	Delete catalog record for an ABF object, which can be the entire
**	application.  The catalog record consists of several rows from
**	several tables.
**
** Inputs:
**	appid	{OOID}  Application ID.
**	objid	{OOID}  Object ID; OC_UNDEFINED for entire application.
**	fid	{OOID}	Fid for objects which got 'em (OSL objects).
**	class	{OOID}  Object class.
**
** Returns:
**	{STATUS}  OK		success
**		  ILE_NOOBJ	doesn't exist
**		  ILE_FAIL	DB failure
**
** History:
**	5/87 (bobm)	written
**	18-aug-18 (kenl)
**		Changed QUEL to SQL
**	12-sep-88 (kenl)
**		Changed EXEC SQL COMMITs to appropriate call to IIUI...Xaction
**		routines.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**	20-jan-1988 (jhw)  Added 'class' parameter and delete for IL encodings,
**		and merged queries.
**	(9/89) bobm	add fid argument.
*/
STATUS
IIAMcdCatDel ( appid, objid, fid, class )
  OOID appid;
  OOID objid;
  OOID fid;
OOID	class;
{
	if ( objid == appid )
		objid = OC_UNDEFINED;
	iiuicw1_CatWriteOn();
	IIUIbeginXaction();	/* begin transaction */
#ifdef FULLDELETE
	if ( objid == OC_UNDEFINED || class == OC_OSLFRAME ||
		  class == OC_MUFRAME || class == OC_APPFRAME ||
		  class == OC_UPDFRAME || class == OC_BRWFRAME ||
			class == OC_OSLPROC )
	{ /* Delete encoded IL objects */
		if (objid == OC_UNDEFINED)
		{
/* # line 92 "iamcdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcdel1",6612,18027);
      IIputdomio((short *)0,1,30,4,&appid);
      IIputdomio((short *)0,1,30,sizeof(OC_ILCODE),(void *)IILQint(OC_ILCODE));
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcdel1",6612,18027);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_encodings where encode_object in(select ii_objects.obj\
ect_id from ii_objects, ii_abfdependencies dep, ii_abfobjects appobj w\
here abfdef_name=ii_objects.object_name and dep.object_id=appobj.objec\
t_id and applid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&appid);
        IIwritio(0,(short *)0,1,32,0,(char *)"and dep.object_class=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,sizeof(OC_ILCODE),(void *)IILQint(OC_ILCODE
        ));
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"iamcdel1",6612,18027);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 102 "iamcdel.sc" */	/* host code */
			if ( FEinqerr() != OK )
				return abort_it();
/* # line 106 "iamcdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcdel2",6613,19660);
      IIputdomio((short *)0,1,30,4,&appid);
      IIputdomio((short *)0,1,30,sizeof(OC_ILCODE),(void *)IILQint(OC_ILCODE));
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcdel2",6613,19660);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_objects where object_id in(select dep.object_id from i\
i_abfdependencies dep, ii_abfobjects appobj where abfdef_name=ii_objec\
ts.object_name and dep.object_id=appobj.object_id and applid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&appid);
        IIwritio(0,(short *)0,1,32,0,(char *)"and dep.object_class=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,sizeof(OC_ILCODE),(void *)IILQint(OC_ILCODE
        ));
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"iamcdel2",6613,19660);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 116 "iamcdel.sc" */	/* host code */
			if ( FEinqerr() != OK )
				return abort_it();
		}
		else
		{
/* # line 122 "iamcdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcdel3",6614,17258);
      IIputdomio((short *)0,1,30,4,&fid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcdel3",6614,17258);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_encodings where encode_object=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&fid);
        IIexDefine(0,(char *)"iamcdel3",6614,17258);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 124 "iamcdel.sc" */	/* host code */
			if ( FEinqerr() != OK )
				return abort_it();
/* # line 128 "iamcdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcdel4",6615,16474);
      IIputdomio((short *)0,1,30,4,&fid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcdel4",6615,16474);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_objects where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&fid);
        IIexDefine(0,(char *)"iamcdel4",6615,16474);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 130 "iamcdel.sc" */	/* host code */
			if ( FEinqerr() != OK )
				return abort_it();
		}
	}
#endif
	if (objid != OC_UNDEFINED)
	{
#ifdef FULLDELETE
/* # line 140 "iamcdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcdel5",6616,23930);
      IIputdomio((short *)0,1,30,4,&objid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcdel5",6616,23930);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_longremarks where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&objid);
        IIexDefine(0,(char *)"iamcdel5",6616,23930);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 142 "iamcdel.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return abort_it();
/* # line 146 "iamcdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcdel6",6617,23112);
      IIputdomio((short *)0,1,30,4,&objid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcdel6",6617,23112);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_abfdependencies where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&objid);
        IIexDefine(0,(char *)"iamcdel6",6617,23112);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 148 "iamcdel.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return abort_it();
/* # line 152 "iamcdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcdel7",6618,22391);
      IIputdomio((short *)0,1,30,4,&objid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcdel7",6618,22391);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_vqtabcols where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&objid);
        IIexDefine(0,(char *)"iamcdel7",6618,22391);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 154 "iamcdel.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return abort_it();
/* # line 158 "iamcdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcdel8",6619,19541);
      IIputdomio((short *)0,1,30,4,&objid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcdel8",6619,19541);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_menuargs where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&objid);
        IIexDefine(0,(char *)"iamcdel8",6619,19541);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 160 "iamcdel.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return abort_it();
/* # line 164 "iamcdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcdel9",6620,29027);
      IIputdomio((short *)0,1,30,4,&objid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcdel9",6620,29027);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_vqtables where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&objid);
        IIexDefine(0,(char *)"iamcdel9",6620,29027);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 166 "iamcdel.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return abort_it();
/* # line 170 "iamcdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcdel10",6621,4676);
      IIputdomio((short *)0,1,30,4,&objid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcdel10",6621,4676);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_framevars where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&objid);
        IIexDefine(0,(char *)"iamcdel10",6621,4676);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 172 "iamcdel.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return abort_it();
/* # line 176 "iamcdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcdel11",6622,5986);
      IIputdomio((short *)0,1,30,4,&objid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcdel11",6622,5986);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_vqjoins where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&objid);
        IIexDefine(0,(char *)"iamcdel11",6622,5986);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 178 "iamcdel.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return abort_it();
#endif
		/* Note:  ii_objects delete cannot be last! */
/* # line 185 "iamcdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcdel12",6623,7280);
      IIputdomio((short *)0,1,30,4,&objid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcdel12",6623,7280);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_objects where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&objid);
        IIexDefine(0,(char *)"iamcdel12",6623,7280);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 187 "iamcdel.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return abort_it();
#ifdef FULLDELETE
/* # line 192 "iamcdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcdel13",6624,6609);
      IIputdomio((short *)0,1,30,4,&objid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcdel13",6624,6609);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_abfobjects where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&objid);
        IIexDefine(0,(char *)"iamcdel13",6624,6609);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 194 "iamcdel.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return abort_it();
#endif
	}
	else
	{
#ifdef FULLDELETE
/* # line 202 "iamcdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcdel14",6625,1758);
      IIputdomio((short *)0,1,30,4,&appid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcdel14",6625,1758);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_longremarks where object_id in(select object_id from i\
i_abfobjects where applid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&appid);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"iamcdel14",6625,1758);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 206 "iamcdel.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return abort_it();
/* # line 210 "iamcdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcdel15",6626,976);
      IIputdomio((short *)0,1,30,4,&appid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcdel15",6626,976);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_abfdependencies where object_id in(select object_id fr\
om ii_abfobjects where applid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&appid);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"iamcdel15",6626,976);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 214 "iamcdel.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return abort_it();
/* # line 218 "iamcdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcdel16",6627,334);
      IIputdomio((short *)0,1,30,4,&appid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcdel16",6627,334);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_vqtabcols where object_id in(select object_id from ii_\
abfobjects where applid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&appid);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"iamcdel16",6627,334);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 222 "iamcdel.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return abort_it();
/* # line 226 "iamcdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcdel17",6628,1724);
      IIputdomio((short *)0,1,30,4,&appid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcdel17",6628,1724);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_menuargs where object_id in(select object_id from ii_a\
bfobjects where applid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&appid);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"iamcdel17",6628,1724);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 230 "iamcdel.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return abort_it();
/* # line 234 "iamcdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcdel18",6629,15338);
      IIputdomio((short *)0,1,30,4,&appid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcdel18",6629,15338);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_vqtables where object_id in(select object_id from ii_a\
bfobjects where applid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&appid);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"iamcdel18",6629,15338);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 238 "iamcdel.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return abort_it();
/* # line 242 "iamcdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcdel19",6630,14537);
      IIputdomio((short *)0,1,30,4,&appid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcdel19",6630,14537);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_framevars where object_id in(select object_id from ii_\
abfobjects where applid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&appid);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"iamcdel19",6630,14537);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 246 "iamcdel.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return abort_it();
/* # line 250 "iamcdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcdel20",6631,6745);
      IIputdomio((short *)0,1,30,4,&appid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcdel20",6631,6745);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_vqjoins where object_id in(select object_id from ii_ab\
fobjects where applid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&appid);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"iamcdel20",6631,6745);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 254 "iamcdel.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return abort_it();
#endif
		/* Note:  ii_objects delete cannot be last! */
/* # line 261 "iamcdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcdel21",6632,6088);
      IIputdomio((short *)0,1,30,4,&appid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcdel21",6632,6088);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_objects where object_id in(select object_id from ii_ab\
fobjects where applid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&appid);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"iamcdel21",6632,6088);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 265 "iamcdel.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return abort_it();
#ifdef FULLDELETE
/* # line 270 "iamcdel.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcdel22",6633,11575);
      IIputdomio((short *)0,1,30,4,&appid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcdel22",6633,11575);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_abfobjects where applid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&appid);
        IIexDefine(0,(char *)"iamcdel22",6633,11575);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 272 "iamcdel.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return abort_it();
#endif
	}
	IIUIendXaction();	/* end transaction */
	iiuicw0_CatWriteOff();
	return OK;
}
