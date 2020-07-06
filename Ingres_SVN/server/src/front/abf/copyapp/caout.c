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
# include <ooclass.h>
# include <oodep.h>
# include	<oocat.h>
# include	<cu.h>
# include	<acatrec.h>
# include	"copyappl.h"
/**
** Name:	caout.c -	 Copy an application out.
**
** Description:
**	Contains the routine that copies an application out.
**
** History:
**	30-jul-1987 (Joe)
**		Started.
**	18-dec-1987 (Joe)
**		Fixes for BUG 1511 and 1519.  Changed
**		To use select distinct and not copy objects
**		with blank name.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
char	*IICUrtoRectypeToObjname();
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
/* # line 49 "caout.sc" */	/* host code */
/*{
** Name:	IICAcaoCopyApplOut	-  Write application objects to file.
**
** Description:
**	This routine writes all the application objects to a file.
**	It uses the copyutil write routines to do this.
**
** Inputs:
**	appname		The name of the application.
**
**	fp		The file to write to.  The FILE pointer
**			must be open and pointing to a writeable
**			file.
**
** Outputs:
**		STATUS of the write.
**
** History:
**	30-jul-1987 (Joe)
**		Initial version.
**
**	18-dec-1987 (Joe)
**		Fixes for BUGs 1511 and 1519. See below for
**		details.
**
**	2/90 (bobm)
**		Prevent "copying" message for non-existent objects
**		(bug 9049).  This means we're saying "copying" after
**		we've already copied, but ...
**		Also produce error message if what fails is a form
**		(bug 20154)
**	22-apr-91 (blaise)
**		When copying out objects stored in the database, don't attempt
**		to copy out database procedures. bug #36919.
**	26-jul-93 (blaise)
**		Preparing for internationalization: replaced literal string
**		with message string.
*/
STATUS
IICAcaoCopyApplOut ( appname, fp )
char	*appname;
FILE	*fp;
{
  OOID class;
  OOID id;
    ACATREC		*app;
    ACATREC		*olist;		/* List of objects in app. */
    register ACATREC	*obj;
    STATUS		rval = OK;
    char		long_remark[OOLONGREMSIZE+1];
    CURECORD	*cu_recfor();
    IIUGmsg( ERget(S_CA0016_COPYOUT), FALSE, 2, (PTR) appname,
		(PTR) ERget(F_CA0005_Application)
	);
    cu_wrhdr(ERx("COPYAPP"), fp);
    if ( IIAMoiObjId(appname, OC_APPL, 0, &id, &class) != OK ||
		IIAMcgCatGet(id, id, &app) != OK ||
	IICUgtLongRemark(id, long_remark) != OK )
    {
	IIUGerr(E_CA0013_NOSUCHAPP, 0, 1, (PTR) appname);
	return E_CA0013_NOSUCHAPP;
    }
    IICAaslApplSourceLoc(app->source);
    iicaWrite(app, long_remark, 0, fp);
    if ( (rval = IIAMcgCatGet(id, (OOID)OC_UNDEFINED, &olist)) != OK)
    {
	IIUGerr(E_CA0014_GETOBJS, 0, 2, (PTR) appname, (PTR) &rval);
	return E_CA0014_GETOBJS;
    }
    for (obj = olist; obj != NULL; obj = obj->next)
    {
	if ( obj->class != OC_APPL )
	{
		if ( IICUgtLongRemark(obj->ooid, long_remark) != OK )
		{
			IIUGerr( E_CA0018_ErrLongRemark, 0,
				2, (PTR)appname, (PTR)obj->name
			);
		}
		else
		{
			iicaWrite(obj, long_remark, 1, fp);
		}
	}
    }
    /*
    ** At this point all the records for the application and the
    ** objects in the application have been written.  Now must
    ** write the definitions for the dependent objects in the application
    ** like the forms and report.
    **
    ** Start a transaction so that the cursor
    ** and other queries can run at the same time
    **
    */
    IIUIbeginXaction();
    /*
    ** Fix for BUG 1519.
    ** Do a select distinct so only one copy of
    ** the object is written to the file.
    **
    ** Need to join to ii_objects so we only fetch legitimate objects
    ** which have not been deleted.
    **
    ** No nned to join to ii_abfobjects now that application id is in 
    ** ii_abfdependncies.
    */
/* # line 171 "caout.sc" */	/* open */
  {
    IIsqInit(&sqlca);
    IIcsOpen((char *)"iidepcur",6953,10998);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct d.abfdef_name, d.object_class from ii_abfdependencies\
 d, ii_objects o where d.abfdef_applid=");
    IIputdomio((short *)0,1,30,4,&id);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and d.object_id=o.object_id and d.abfdef_deptype=");
    IIputdomio((short *)0,1,30,sizeof(OC_DTDBREF),(void *)IILQint(OC_DTDBREF));
    IIcsQuery((char *)"iidepcur",6953,10998);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 172 "caout.sc" */	/* host code */
    /*
    ** Loop is broken out of inside
    */
    while (sqlca.sqlcode == 0)
    {
	register CURECORD	*curec;
    	char			*classname;
  char name[FE_MAXNAME+1];
/* # line 183 "caout.sc" */	/* fetch */
  {
    IIsqInit(&sqlca);
    if (IIcsRetScroll((char *)"iidepcur",6953,10998,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,FE_MAXNAME+1-1,name);
      IIcsGetio((short *)0,1,30,4,&class);
      IIcsERetrieve();
    } /* IIcsRetrieve */
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 184 "caout.sc" */	/* host code */
	if (sqlca.sqlcode != 0)
	    break;
	/*
	** Fix for BUG 1511.
	** If the name if empty don't try and
	** copy the object.
	*/
	if ( STtrmwhite(name) <= 0 )
	    continue;
	/*
	** If the dependency is an AFORMREF, then the object
	** to copy is a form.
	*/
	if (class == OC_AFORMREF)
	    class = OC_FORM;
	/*
	** Don't try to copy out ILCODE, database procedures or dummy records.
	** (fix for bug #36919: add OC_DBPROC to the list below)
	*/
	if (class == OC_ILCODE || class == OC_DBPROC || class == 0)
	    continue;
	if ( (curec = cu_recfor(class)) == NULL || curec->cuoutfunc == NULL )
	{
	    i4	t;
	    t = class;
	    IIUGerr(E_CA0015_NOOUTFUNC, 0, 2, (PTR) name, (PTR) &t);
	    rval = E_CA0015_NOOUTFUNC;
	    continue;
	}
	/*
	** print "copying" messages AFTER the fact, even though
	** it is a little misleading.  We don't want to print
	** messages for things we did not copy.  We DON'T call with
	** TRUE for "notthereok", because then we would not get
	** reasonable return status back.
	*/
	if ((*curec->cuoutfunc)(name, class, FALSE, fp) == OK)
	{
	    if ((classname = IICUrtoRectypeToObjname(class)) != NULL)
	    {
		IIUGmsg(ERget(S_CA0016_COPYOUT), FALSE, 2, (PTR) name,
			(PTR) classname);
	    }
	}
	else
	{
	    if (class == OC_FORM)
		IIUGerr(E_CA0020_NoForm, 0, 1, (PTR) name);
	}
    }
/* # line 235 "caout.sc" */	/* close */
  {
    IIsqInit(&sqlca);
    IIcsClose((char *)"iidepcur",6953,10998);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 236 "caout.sc" */	/* host code */
    IIUIendXaction();
    return rval;
}
