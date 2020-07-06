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
# include <acatrec.h>
# include <abfdbcat.h>
#include	<ilerror.h>
/**
** Name:	iamcupd.sc -	ABF Catalog Record Update Module.
**
** Description:
**	Contains routines that update the ABF catalogs for an application
**	component object.  Defines:
**
**	iiamUpdateCat()		update abf catalog record.
**	iiamUpdateConst()	update abf catalog record for constant only.
**	iiamUOFupdateOldOsl	change an OLDOSL frame to an OSL frame
**	IIAMugcUpdateGlobCons	Update global constants due to language changes
**
** History:
**	Revision 6.0  87/05  bobm
**	5/87 (bobm)	written
**
**	Revision 6.1  88/08/18  kenl
**	Changed QUEL to SQL.
**	12-sep-88 (kenl)
**		Changed EXEC SQL COMMITs to appropriate calls to the
**		IIUI...Xaction routnies.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**
**	Revision 6.2  89/01  wong
**	Rewrote as 'iiamUpdateCat()' to only update the ABF catalogs.
**
**	Revision 6.5
**	21-oct-92 (davel)
**		Added IIAMugcUpdateGlobCons().
*/
/*{
** Name:	iiamUpdateCat() -	Update ABF Catalog Record.
**
** Description:
**	Updates a record in the ABF application catalogs.  This should
**	be used in conjunction with the OO module, which will update the
**	corresponding object record.
**
** Input:
**	arec	{ABF_DBCAT *}  The catalog record to be updated:
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
**	{STATUS}  DBMS query status for UPDATE.
**
** History:
**	5/87 (bobm)	written (as 'IIAMcuCatUpd()'.)
**	18-aug-88 (kenl)  Changed QUEL to SQL.
**	09-nov-88 (marian)  Modified column names for extended catalogs to
**		allow them to work with gateways.
**	01/89 (jhw)  Modified to use ABF_DBCAT, and to only update the ABF
**		catalog, itself.
*/
STATUS
iiamUpdateCat ( arec )
  register ABF_DBCAT *arec;
{
/* # line 87 "iamcupd.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcupd1",6611,4222);
      IIputdomio((short *)0,1,30,sizeof(ACAT_VERSION),(void *)IILQint(
      ACAT_VERSION));
      IIputdomio((short *)0,1,32,0,arec->source);
      IIputdomio((short *)0,1,32,0,arec->symbol);
      IIputdomio((short *)0,1,30,2,&arec->retadf_type);
      IIputdomio((short *)0,1,30,4,&arec->retlength);
      IIputdomio((short *)0,1,30,2,&arec->retscale);
      IIputdomio((short *)0,1,32,0,arec->rettype);
      IIputdomio((short *)0,1,32,0,arec->arg0);
      IIputdomio((short *)0,1,32,0,arec->arg1);
      IIputdomio((short *)0,1,32,0,arec->arg2);
      IIputdomio((short *)0,1,32,0,arec->arg3);
      IIputdomio((short *)0,1,32,0,arec->arg4);
      IIputdomio((short *)0,1,32,0,arec->arg5);
      IIputdomio((short *)0,1,30,4,&arec->flags);
      IIputdomio((short *)0,1,30,4,&arec->ooid);
      IIputdomio((short *)0,1,30,4,&arec->applid);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcupd1",6611,4222);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_abfobjects set abf_version=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,sizeof(ACAT_VERSION),(void *)IILQint(
        ACAT_VERSION));
        IIwritio(0,(short *)0,1,32,0,(char *)", abf_source=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,arec->source);
        IIwritio(0,(short *)0,1,32,0,(char *)", abf_symbol=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,arec->symbol);
        IIwritio(0,(short *)0,1,32,0,(char *)", retadf_type=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,2,&arec->retadf_type);
        IIwritio(0,(short *)0,1,32,0,(char *)", retlength=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&arec->retlength);
        IIwritio(0,(short *)0,1,32,0,(char *)", retscale=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,2,&arec->retscale);
        IIwritio(0,(short *)0,1,32,0,(char *)", rettype=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,arec->rettype);
        IIwritio(0,(short *)0,1,32,0,(char *)", abf_arg1=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,arec->arg0);
        IIwritio(0,(short *)0,1,32,0,(char *)", abf_arg2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,32,0,arec->arg1);
        IIwritio(0,(short *)0,1,32,0,(char *)", abf_arg3=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $9=");
        IIputdomio((short *)0,1,32,0,arec->arg2);
        IIwritio(0,(short *)0,1,32,0,(char *)", abf_arg4=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $10=");
        IIputdomio((short *)0,1,32,0,arec->arg3);
        IIwritio(0,(short *)0,1,32,0,(char *)", abf_arg5=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $11=");
        IIputdomio((short *)0,1,32,0,arec->arg4);
        IIwritio(0,(short *)0,1,32,0,(char *)", abf_arg6=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $12=");
        IIputdomio((short *)0,1,32,0,arec->arg5);
        IIwritio(0,(short *)0,1,32,0,(char *)", abf_flags=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $13=");
        IIputdomio((short *)0,1,30,4,&arec->flags);
        IIwritio(0,(short *)0,1,32,0,(char *)"where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $14=");
        IIputdomio((short *)0,1,30,4,&arec->ooid);
        IIwritio(0,(short *)0,1,32,0,(char *)"and applid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $15=");
        IIputdomio((short *)0,1,30,4,&arec->applid);
        IIexDefine(0,(char *)"iamcupd1",6611,4222);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 104 "iamcupd.sc" */	/* host code */
	return FEinqerr();
}
/* iiamucUpdateConst is for the most part a clone of iiamUpdateCat.  It differs
 * only in that an additional restriction has been added to the predicate so
 * that the update occurs for the correct language version of a constant.
 *
 * Cloned by JCF.
 */
STATUS
iiamucUpdateConst ( arec )
  register ABF_DBCAT *arec;
{
/* # line 120 "iamcupd.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcupd2",6612,4765);
      IIputdomio((short *)0,1,30,sizeof(ACAT_VERSION),(void *)IILQint(
      ACAT_VERSION));
      IIputdomio((short *)0,1,32,0,arec->source);
      IIputdomio((short *)0,1,32,0,arec->symbol);
      IIputdomio((short *)0,1,30,2,&arec->retadf_type);
      IIputdomio((short *)0,1,30,4,&arec->retlength);
      IIputdomio((short *)0,1,30,2,&arec->retscale);
      IIputdomio((short *)0,1,32,0,arec->rettype);
      IIputdomio((short *)0,1,32,0,arec->arg0);
      IIputdomio((short *)0,1,32,0,arec->arg1);
      IIputdomio((short *)0,1,32,0,arec->arg2);
      IIputdomio((short *)0,1,32,0,arec->arg3);
      IIputdomio((short *)0,1,32,0,arec->arg4);
      IIputdomio((short *)0,1,32,0,arec->arg5);
      IIputdomio((short *)0,1,30,4,&arec->flags);
      IIputdomio((short *)0,1,30,4,&arec->ooid);
      IIputdomio((short *)0,1,30,4,&arec->applid);
      IIputdomio((short *)0,1,32,0,arec->arg0);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcupd2",6612,4765);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_abfobjects set abf_version=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,sizeof(ACAT_VERSION),(void *)IILQint(
        ACAT_VERSION));
        IIwritio(0,(short *)0,1,32,0,(char *)", abf_source=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,arec->source);
        IIwritio(0,(short *)0,1,32,0,(char *)", abf_symbol=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,arec->symbol);
        IIwritio(0,(short *)0,1,32,0,(char *)", retadf_type=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,30,2,&arec->retadf_type);
        IIwritio(0,(short *)0,1,32,0,(char *)", retlength=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,30,4,&arec->retlength);
        IIwritio(0,(short *)0,1,32,0,(char *)", retscale=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,30,2,&arec->retscale);
        IIwritio(0,(short *)0,1,32,0,(char *)", rettype=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,arec->rettype);
        IIwritio(0,(short *)0,1,32,0,(char *)", abf_arg1=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $7=");
        IIputdomio((short *)0,1,32,0,arec->arg0);
        IIwritio(0,(short *)0,1,32,0,(char *)", abf_arg2=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $8=");
        IIputdomio((short *)0,1,32,0,arec->arg1);
        IIwritio(0,(short *)0,1,32,0,(char *)", abf_arg3=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $9=");
        IIputdomio((short *)0,1,32,0,arec->arg2);
        IIwritio(0,(short *)0,1,32,0,(char *)", abf_arg4=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $10=");
        IIputdomio((short *)0,1,32,0,arec->arg3);
        IIwritio(0,(short *)0,1,32,0,(char *)", abf_arg5=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $11=");
        IIputdomio((short *)0,1,32,0,arec->arg4);
        IIwritio(0,(short *)0,1,32,0,(char *)", abf_arg6=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $12=");
        IIputdomio((short *)0,1,32,0,arec->arg5);
        IIwritio(0,(short *)0,1,32,0,(char *)", abf_flags=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $13=");
        IIputdomio((short *)0,1,30,4,&arec->flags);
        IIwritio(0,(short *)0,1,32,0,(char *)"where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $14=");
        IIputdomio((short *)0,1,30,4,&arec->ooid);
        IIwritio(0,(short *)0,1,32,0,(char *)"and applid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $15=");
        IIputdomio((short *)0,1,30,4,&arec->applid);
        IIwritio(0,(short *)0,1,32,0,(char *)"and abf_arg1=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $16=");
        IIputdomio((short *)0,1,32,0,arec->arg0);
        IIexDefine(0,(char *)"iamcupd2",6612,4765);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 138 "iamcupd.sc" */	/* host code */
	return FEinqerr();
}
/*{
** Name:	iiamUOFupdateOldOsl     change an OLDOSL frame to an OSL frame
**
** Description:
**	Change the objectclass of an OLDOSL Frame from OC_OLDOSL to OC_OSLFRAME
**
** Inputs:
**	ooid		Object id
**
** Outputs:
**
**	Returns:
**		Ddtabase status
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	11/90 (Mike S) Initial version
*/
STATUS
iiamUOFupdateOldOsl(ooid)
  OOID ooid;
{
/* # line 169 "iamcupd.sc" */	/* update */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_objects set object_class=");
    IIputdomio((short *)0,1,30,sizeof(OC_OSLFRAME),(void *)IILQint(OC_OSLFRAME
    ));
    IIwritio(0,(short *)0,1,32,0,(char *)"where object_id=");
    IIputdomio((short *)0,1,30,4,&ooid);
    IIsyncup((char *)0,0);
  }
/* # line 173 "iamcupd.sc" */	/* host code */
	return FEinqerr();
}
/*{
** Name:	IIAMugcUpdateGlobCons - Update Global Constant components.
**
** Description:
**	Change the language of a set of global constant values.  This serves to
**	upgrade the global constant components in the catalogs from the pre-6.5
**	natural langauge scheme to the current scheme.
**
** Inputs:
**	applid		Application OOID.
**	new_lang	The new language (always " default")
**	old_lang	The langauge corresponding to the set of constants
**			that will be changed to the new language.
**
** Outputs:
**
** Returns:
**	Database status.
**
** Exceptions:
**
** Side Effects:
**
** History:
**	20-oct-92 (davel)
**		Initial version.
*/
STATUS
IIAMugcUpdateGlobCons(applid, new_lang, old_lang)
  OOID applid;
  char *new_lang;
  char *old_lang;
{
/* # line 211 "iamcupd.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"iamcupd3",6613,5820);
      IIputdomio((short *)0,1,32,0,new_lang);
      IIputdomio((short *)0,1,30,4,&applid);
      IIputdomio((short *)0,1,30,sizeof(OC_CONST),(void *)IILQint(OC_CONST));
      IIputdomio((short *)0,1,32,0,old_lang);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"iamcupd3",6613,5820);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_abfobjects a from ii_objects o set abf_arg1=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,new_lang);
        IIwritio(0,(short *)0,1,32,0,(char *)"where a.applid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&applid);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and a.object_id=o.object_id and o.object_class=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,sizeof(OC_CONST),(void *)IILQint(OC_CONST));
        IIwritio(0,(short *)0,1,32,0,(char *)"and a.abf_arg1=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,old_lang);
        IIexDefine(0,(char *)"iamcupd3",6613,5820);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 217 "iamcupd.sc" */	/* host code */
	return FEinqerr();
}
