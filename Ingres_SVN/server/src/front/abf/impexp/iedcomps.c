# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  Copyright (c) 1986, 2004 Ingres Corporation
*/
# include       <compat.h>
# include	<si.h>
# include       <st.h>
# include       <gl.h>
# include       <iicommon.h>
# include       <fe.h>
# include	<ooclass.h>
# include	<oocat.h>
# include	<abfdbcat.h>
# include	<cu.h>
# include	<ug.h>
# include	"erie.h"
# include       "ieimpexp.h"
/**
** Name: iedcomps.sc
** 
** Defines:
**	IIIEdoc_DeleteOldComponent()	- delete an existing catalog entry
**
** History:
**	18-aug-1993 (daver)
**		First Written/Documented
**	14-sep-1993 (daver)
**		Moved error message over to erie.msg, random cleanup shme.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
/* # line 40 "iedcomps.sc" */	/* host code */
/*{
** Name: IIIEdoc_DeleteOldComponent      - delete existing catalog entry
**
** Description:
**	This routine is needed to remove entries in catalogs where an
**	object is described via multiple rows. When the catalogs use a 1:1,
**	normalized mapping between rows and objects, we can simply update those
**	rows. But when multiple rows are stored per object, we need to delete
**	them all, and insert the new ones.
**
** Input:
**	IEOBJREC	*objrec		pointer to object (and id) to delete.
**	i4		appid		application id of object
**	i4		comptype	type of component it is
**
** Output:
**	None
**
** Returns:
**	Whatever the status of the query in question returns.
**
** Side Effects:
**	Deletes the specified component from the appropriate catalog.
**
** History:
**	18-aug-1993 (daver)
**		First Written.
**	14-sep-1993 (daver)
**		Moved error message over to erie.msg, random cleanup shme.
*/
STATUS
IIIEdoc_DeleteOldComponent(objrec, appid, comptype)
IEOBJREC	*objrec;
  i4 appid;
i4		comptype;
{
  i4 objid;
	if (IEDebug)
		SIprintf("In DeleteOldComponent, deleting %s's %s...\n\n",
		objrec->name,IICUrtnRectypeToName(comptype));
	objid = objrec->id;
	switch (comptype)
	{
	  case CUC_AODEPEND:
	  case CUC_ADEPEND:
		/*
		** Don't delete the ILCODE dependencies that this 
		** object has (class = 2010) or the Menu dependencies
		** (dependency type = 3505) -- the menu deps indicate that
		** this frame is a vision frame and has child frames beneath it
		** in the frame flow diagram.
		**
		** For frames, this will delete the formref dependencies,
		** which is a good thing, since they will be added later, and
		** the new form associated with this frame may not necessarily
		** be the same name as the old one
		**
		** PS. Careful about using defined symbols rather than
		** the raw numbers 2010 and 3505; I've seen these changed
		** back to numbers elsewhere, claiming gateway/porting problems.
		*/
/* # line 110 "iedcomps.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_abfdependencies where object_id=");
    IIputdomio((short *)0,1,30,4,&objid);
    IIwritio(0,(short *)0,1,32,0,(char *)"and abfdef_applid=");
    IIputdomio((short *)0,1,30,4,&appid);
    IIwritio(0,(short *)0,1,32,0,(char *)
"and object_class<>2010 and abfdef_deptype<>3505");
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 114 "iedcomps.sc" */	/* host code */
		break;
	  case CUC_FIELD:
/* # line 118 "iedcomps.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_fields where object_id=");
    IIputdomio((short *)0,1,30,4,&objid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 119 "iedcomps.sc" */	/* host code */
		break;
	  case CUC_TRIM:
/* # line 123 "iedcomps.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_trim where object_id=");
    IIputdomio((short *)0,1,30,4,&objid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 124 "iedcomps.sc" */	/* host code */
		break;
	  case CUC_RCOMMANDS:
/* # line 128 "iedcomps.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_rcommands where object_id=");
    IIputdomio((short *)0,1,30,4,&objid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 129 "iedcomps.sc" */	/* host code */
		break;
	  case CUC_VQJOIN:
/* # line 133 "iedcomps.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_vqjoins where object_id=");
    IIputdomio((short *)0,1,30,4,&objid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 134 "iedcomps.sc" */	/* host code */
		break;
	  case CUC_VQTAB:
/* # line 138 "iedcomps.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_vqtables where object_id=");
    IIputdomio((short *)0,1,30,4,&objid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 139 "iedcomps.sc" */	/* host code */
		break;
	  case CUC_VQTCOL:
/* # line 143 "iedcomps.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_vqtabcols where object_id=");
    IIputdomio((short *)0,1,30,4,&objid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 144 "iedcomps.sc" */	/* host code */
		break;
	  case CUC_MENUARG:
/* # line 148 "iedcomps.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_menuargs where object_id=");
    IIputdomio((short *)0,1,30,4,&objid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 149 "iedcomps.sc" */	/* host code */
		break;
	  case CUC_FRMVAR:
/* # line 153 "iedcomps.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_framevars where object_id=");
    IIputdomio((short *)0,1,30,4,&objid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 154 "iedcomps.sc" */	/* host code */
		break;
	  case CUC_ESCAPE:
/* # line 158 "iedcomps.sc" */	/* delete */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_encodings where object_id=");
    IIputdomio((short *)0,1,30,4,&objid);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 159 "iedcomps.sc" */	/* host code */
		break;
	  default:
		IIUGerr(E_IE0029_Unknown_Component, UG_ERR_ERROR, 2,
			(PTR)objrec->name, (PTR)comptype);
		return (CUQUIT);
	} /* end switch */
	return(FEinqerr()); 
}
