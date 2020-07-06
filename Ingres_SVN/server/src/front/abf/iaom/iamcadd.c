# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 1987, 2004 Ingres Corporation
*/
#include	<compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<cv.h>		/* 6-x_PC_80x86 */
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ui.h>
# include <ooclass.h>
# include <abfdbcat.h>
# include <acatrec.h>
# include	<uigdata.h>
#include	<ilerror.h>
#include	"iamint.h"
/**
** Name:	iamcadd.sc - ABF Catalog Record Insert Module.
**
** Description:
**	Contains the routine that adds an entry for an application object
**	into the application catalogs.  Defines:
**
**	iiamInsertCat()		insert application catalog record into catalogs.
**
** History:
**	Revision 6.0  87/05  bobm
**	Initial revision.
**
**	Revision 6.1  88/05  wong
**	Rewrote in ESQL/C.
**	12-sep-88 (kenl)
**		Changed EXEC SQL COMMITs to call to appropriate IIUI...Xaxtion
**		routines.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**
**	Revision 6.2  89/01  wong
**	Rewrote as 'iiamInsertCat()'.
**
**	Revision 6.3/03/00  90/01  wong
**	Added 'iiamCnInsertCat()'.
**
**	Revision 6.5
**	16-oct-92 (davel)
**		Removed obsolete iiamCnInsertCat(), which was the special case
**		for adding new Global Cosntants.  No longer to we need to add
**		an ii_abfobjects entry for all previously used natural 
**		languages.
**/
/*{
** Name:	iiamInsertCat() -	Insert ABF Catalog Record into Catalogs.
**
** Description:
**	Inserts a record into the ABF application catalogs.  This should
**	be used in conjunction with the OO module, which will save the
**	corresponding object record.
**
** Input:
**	rec	{ABF_DBCAT *}  The catalog record to be inserted:
**			applid	{OOID}  The application ID.
**			ooid	{OOID}  The object ID.
**			source	{char *}  The source file/directory.
**			symbol	{char *}  The object-code symbol.
**			retadf_type  {DB_DT_ID}  The return type.
**			retlength  {longnat}  The return type length.
**			retscale  {i2}  The return type scale (DB_DEC_TYPE.)
**			rettype {char *}  The return type description.
**			arg0-5	{char *}  Object specific values.
**
** Returns:
**	{STATUS}  DBMS query status for INSERT INTO.
**
** History:
**	5/87 (bobm)	written (as 'IIAMcaCatAdd()'.)
**	03/88 (jhw)  Changed to use SQL.
**	01/89 (jhw)  Modified to use ABF_DBCAT, and to only insert
**			to the ABF catalog, itself.
*/
STATUS
iiamInsertCat ( rec )
  ABF_DBCAT *rec;
{
/* # line 92 "iamcadd.sc" */	/* repeat insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcadd1",6612,380);
      IIputdomio((short *)0,1,30,sizeof(ACAT_VERSION),(void *)IILQint(
      ACAT_VERSION));
      IIputdomio((short *)0,1,30,4,&rec->applid);
      IIputdomio((short *)0,1,30,4,&rec->ooid);
      IIputdomio((short *)0,1,32,0,rec->source);
      IIputdomio((short *)0,1,32,0,rec->symbol);
      IIputdomio((short *)0,1,30,2,&rec->retadf_type);
      IIputdomio((short *)0,1,30,4,&rec->retlength);
      IIputdomio((short *)0,1,30,2,&rec->retscale);
      IIputdomio((short *)0,1,32,0,rec->rettype);
      IIputdomio((short *)0,1,32,0,rec->arg0);
      IIputdomio((short *)0,1,32,0,rec->arg1);
      IIputdomio((short *)0,1,32,0,rec->arg2);
      IIputdomio((short *)0,1,32,0,rec->arg3);
      IIputdomio((short *)0,1,32,0,rec->arg4);
      IIputdomio((short *)0,1,32,0,rec->arg5);
      IIputdomio((short *)0,1,30,4,&rec->flags);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcadd1",6612,380);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_abfobjects(abf_version, applid, object_id, abf_source,\
 abf_symbol, retadf_type, retlength, retscale, rettype, abf_arg1, abf_\
arg2, abf_arg3, abf_arg4, abf_arg5, abf_arg6, abf_flags)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,sizeof(ACAT_VERSION),(void *)IILQint(
        ACAT_VERSION));
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&rec->applid);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&rec->ooid);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,rec->source);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,rec->symbol);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,2,&rec->retadf_type);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,30,4,&rec->retlength);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,30,2,&rec->retscale);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,32,0,rec->rettype);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $9=");
        IIputdomio((short *)0,1,32,0,rec->arg0);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $10=");
        IIputdomio((short *)0,1,32,0,rec->arg1);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $11=");
        IIputdomio((short *)0,1,32,0,rec->arg2);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $12=");
        IIputdomio((short *)0,1,32,0,rec->arg3);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $13=");
        IIputdomio((short *)0,1,32,0,rec->arg4);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $14=");
        IIputdomio((short *)0,1,32,0,rec->arg5);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $15=");
        IIputdomio((short *)0,1,30,4,&rec->flags);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"iamcadd1",6612,380);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 109 "iamcadd.sc" */	/* host code */
	return FEinqerr();
}
#ifndef Catalog
GLOBALREF char	*Catowner;
GLOBALREF bool	Timestamp;
#endif
GLOBALREF bool Xact;
STATUS abort_it();
/*{
** Name:	IIAMcaCatAdd() -	Add Application Catalog Record.
**
** Description:
**	Adds an entry for an application object into the application catalogs.
**	The object should have already been added into the object catalogs.
**
**	NOTE:
**		The "depends on" list is scanned for adds to
**		the dependency table.  This list is not usually valid;
**		it must be set up specially by the caller.
**
** Inputs:
**	apid	{OOID}  Application ID, irrelevent if rec->iclass == OC_APPL
**	catrec	{ACATREC *}  Application Catalog Record.
**
** Outputs:
**	catrec->id filled in with new id, creation & modification time.
**
** Return:
**	{STATUS}  OK		success
**		  ILE_ARGBAD	bad record
**		  ILE_FAIL	database write failure
**
** History:
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
*/
STATUS
IIAMcaCatAdd ( apid, catrec )
OOID	apid;
  ACATREC *catrec;
{
	OOID		oapid;
	ABF_DBCAT	abf_obj;
	char	*UGcat_now();
	if ( clean_record(catrec) != OK )
		return ILE_ARGBAD;
#ifndef Catalog
	if ( catrec->ooid == OC_UNDEFINED || (oapid = apid) == OC_UNDEFINED )
		catrec->ooid = IIOOnewId();
	if ( apid <= OC_UNDEFINED )
		apid = catrec->ooid;
#endif
	iiuicw1_CatWriteOn();
	if ( Xact )
	{ /* begin transaction */
		IIUIbeginXaction();
	}
	abf_obj.applid = apid;
	abf_obj.ooid = catrec->ooid;
	abf_obj.source = catrec->source;
	abf_obj.symbol = catrec->symbol;
	abf_obj.retadf_type = catrec->retadf_type;
	abf_obj.retlength = catrec->retlength;
	abf_obj.retscale = catrec->retscale;
	abf_obj.rettype = catrec->rettype;
	abf_obj.arg0 = catrec->arg[0];
	abf_obj.arg1 = catrec->arg[1];
	abf_obj.arg2 = catrec->arg[2];
	abf_obj.arg3 = catrec->arg[3];
	abf_obj.arg4 = catrec->arg[4];
	abf_obj.arg5 = catrec->arg[5];
	if ( iiamInsertCat(&abf_obj) != OK ||
	   	IIAMadlAddDepList( catrec->dep_on, apid, catrec->ooid ) != OK )
		return abort_it();
#ifndef Catalog
	if ( oapid != catrec->ooid )
	{
  char lname[FE_MAXNAME+1];
		catrec->owner = Catowner != NULL ? Catowner :IIUIdbdata()->user;
		if ( Timestamp )
		{
			catrec->create_date = catrec->alter_date = UGcat_now();
		}
		STcopy(catrec->name, lname);
		CVlower(lname);
/* # line 215 "iamcadd.sc" */	/* repeat insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcadd2",6613,2684);
      IIputdomio((short *)0,1,30,4,&catrec->ooid);
      IIputdomio((short *)0,1,30,4,&catrec->class);
      IIputdomio((short *)0,1,32,0,lname);
      IIputdomio((short *)0,1,32,0,catrec->owner);
      IIputdomio((short *)0,1,32,0,catrec->short_remark);
      IIputdomio((short *)0,1,32,0,catrec->create_date);
      IIputdomio((short *)0,1,32,0,catrec->alter_date);
      IIputdomio((short *)0,1,32,0,catrec->owner);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcadd2",6613,2684);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_objects(object_id, object_class, object_name, object_o\
wner, object_env, is_current, short_remark, object_language, create_da\
te, alter_date, alter_count, last_altered_by)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&catrec->ooid);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&catrec->class);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,lname);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,catrec->owner);
        IIwritio(0,(short *)0,1,32,0,(char *)", 0, 0, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,catrec->short_remark);
        IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,catrec->create_date);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,catrec->alter_date);
        IIwritio(0,(short *)0,1,32,0,(char *)", 0, ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,catrec->owner);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"iamcadd2",6613,2684);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 227 "iamcadd.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return abort_it();
	}
#endif
	if ( Xact )
		IIUIendXaction();
	iiuicw0_CatWriteOff();
	return OK;
}
