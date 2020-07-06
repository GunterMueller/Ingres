# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<ex.h>
#include	<st.h>
#include	<lo.h>
#include	<si.h>
#include	<ol.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ug.h>
#include	<uf.h>
#include	<adf.h>
#include	<afe.h>
#include	<fedml.h>
# include <abclass.h>
#include	<abfdbcat.h>
# include <oocat.h>
# include <uigdata.h>
#include	<oocatlog.h>
#include	<oodefine.h>
#include	<oosymbol.h>
#include	<feconfig.h>
#include	<abfcnsts.h>
#include	<eqrun.h>
#ifdef IMTDY
#include	<fdesc.h>
#include	<abfrts.h>
#endif
#include	<ilerror.h>
#include	<dmchecks.h>
# include "abclinfo.h"
# include "abcatrow.h"
#include	"abfgolnk.h"
#include	"abfglobs.h"
#include	"gncode.h"
#include	"erab.h"
/*
fool ming:
#include	<abclass.h>
#include	<oocat.h>
#include	"abclinfo.h"
#include	"abcatrow.h"
*/
/**
** Name:	editglo.qsc -		ABF Define Globals Frame.
**
** Description:
**	Contains the global-variable definition frame routine for ABF, which
**	allows users to define or modify globals.	 Defines:
**
**	IIABegEditGlobs()	ABF edit globals frame.
**
** History:
**	6/89 billc - Initial revision.
**	12-dec-1991 (kevinm)
**		Remove spurous semi-colons.  The compiler on the dra_us5
**		didn't like them.
**	20-jan-92 (davel)
**		Fixed bug 41920 in _c_catLoad().
** 	6-Feb-92 (blaise)
**		Added include abfglobs.h.
**	16-oct-92 (davel)
**		Minor change in _c_catLoad() (removed obsolete arg in call to
**		iiabCopyConstants() ). 
**      18-oct-1993 (kchin)
**          	Since the return type of OOsnd() has been changed to PTR,
**          	when being called in _createObj(), its return type needs to 
**	    	be cast to the proper datatype.
**      06-dec-93 (smc)
**		Bug #58882
**              Commented lint pointer truncation warning.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
char		*IIUGdmlStr();
char		*UGcat_now();
bool		IIABchkSrcDir();
bool		iiabTG_TypeGet();
APPL_COMP	*iiabGetComp();
COMP_CLASS_INFO	*iiabApCompClass();
STATUS		iiabSvApp();
i4		iiabCopyConstants();
GLOBALREF char	*IIabExename;
GLOBALREF bool	IIabWopt;
GLOBALREF bool	abFnwarn;
static READONLY char	_ArrayOf[] = ERx("array of ");
static READONLY char	_DefaultLang[] = APC_CONSTANT_LANGUAGE;
static bool	rec_changed; /* Has the record currently being edited changed */
static STATUS 		_coInit();
static VOID 		_gcatUpdate();
static VOID 		_rcatUpdate();
static STATUS 		_raInit();
static STATUS 		_glCreate();
static STATUS 		_coCreate();
static STATUS 		_rcCreate();
static STATUS 		_raCreate();
static STATUS 		_rEdit();
static STATUS 		_raEdit();
static STATUS 		_glEdit();
static STATUS		_coEdit();
static STATUS		_raDestroy();
static STATUS		_raEnd();
static char 		*_TypeName();
static APPL_COMP	*_ra_get();
static APPL_COMP	*_createObj();
static char	*_nullmsg();
static STATUS objDisplay();
/*{
** Name:	IIABegEditGlobs() -	ABF Edit Globals Frame.
**
** Input:
**	app		{APPL *}  The application.
**	noload		{bool}  Expert mode; don't load the catalogs.
**
** Side Effects:
**	Will change the ABF catalogs
**
** History:
**	Written 6/7/89 (billc)
**
**	02-dec-91 (leighb) DeskTop Porting Change:
**		Added routines to pass data across ABF/IAOM boundary.
*/
static VOID	_g_catInit();
static VOID	_r_catInit();
static VOID	_g_catLoad();
static VOID	_ra_catLoad();
static VOID	_gLoadTable();
static VOID	_rLoadTable();
static VOID	_gInsertRow();
static VOID	_gPutRow();
static VOID	_c_catInit();
static VOID	_c_catLoad();
static VOID	_cLoadTable();
static VOID	_cInsertRow();
static VOID	_cPutRow();
/* Define CATALOG of functions for globals ("G") , classes ("R","M") and
** constants ("C")
*/
static C_OO_CATALOG    _CcatClass =
{
    IIOOct0Init,
    IIOOctEmpty,
    IIOOctCheck,
    IIOOctLoad,
    IIOOctDestroy,
    IIOOctRename,
    IIOOctFetch,
    NULL,
    _c_catInit,
    _c_catLoad,
    _cLoadTable,
    _cInsertRow,
    _cPutRow
};
/* OO Catalog class for Global Variables. */
static C_OO_CATALOG	_GcatClass =
{
	IIOOct0Init,
	IIOOctEmpty,
	IIOOctCheck,
	IIOOctLoad,
	IIOOctDestroy,
	IIOOctRename,
	IIOOctFetch,
	NULL,
	_g_catInit,
	_g_catLoad,
	_gLoadTable,
	_gInsertRow,
	_gPutRow
};
/* OO Catalog class for Class Definitions. */
static C_OO_CATALOG	_RcatClass =
{
	IIOOct0Init,
	IIOOctEmpty,
	IIOOctCheck,
	IIOOctLoad,
	IIOOctDestroy,
	IIOOctRename,
	IIOOctFetch,
	IIOOctMoreInfo,
	_r_catInit,
	_g_catLoad,
	_rLoadTable,
	_gInsertRow,
	_gPutRow
};
/* OO Catalog class for Class Attributes. */
static C_OO_CATALOG	_McatClass =
{
	IIOOct0Init,
	IIOOctEmpty,
	IIOOctCheck,
	IIOOctLoad,
	IIOOctDestroy,
	IIOOctRename,
	IIOOctFetch,
	NULL,
	_g_catInit,
	_ra_catLoad,
	_gLoadTable,
	_gInsertRow,
	_gPutRow
};
GLOBALDEF AB_CATALOG    iiabCcatalog =
{
    &_CcatClass,	/* Routines which do actual work */
    ERx("abfconstants"),/* c_form   -- form name */
    ERx("constants"),	/* c_tfield -- table field name */
    NULL,		/* loaded at runtime with IIamConstCl, Associated class (see abclass.qsc (IAOM))*/	 
    0, FALSE,		/* c_maxrow, c_noload */
    OC_UNDEFINED,	/* Application id for cat. */
    OC_UNDEFINED,	/* ID for current object */
    FALSE ,		/* c_finited */
	/* See AB_CMENU (abcatrow.qsh) for description of following: */
	/* pseudo-menuitem for initialization. has title and explanation.*/
    { _coInit, F_AB0358_ConstEdit_title, NULL, F_AB0356_Const_expl, NULL},
	/* menuitem for create. */
    { _coCreate, FE_Create, NULL, F_AB0359_CreateConst_expl, NULL },
	/* menuitem for destroy. */
    { NULL, FE_Destroy, NULL, F_AB0360_DestroyConst_expl, NULL },
	/* menuitem for edit. */
    { _coEdit, FE_Edit, NULL, F_AB0361_EditConst_expl, NULL  },
	/* menuitem for end. */
    { NULL, FE_End, NULL, F_FE0102_ExplEnd, NULL },
	/* menuitem for quit. */
    { NULL, 0, NULL, 0, NULL },
    iiabGetComp,	/* (*c_getcomp)() function that retrieves a component */
    ERx("abedtcon.hlp")	/* Help file name */
};
GLOBALDEF AB_CATALOG	iiabGcatalog =
{
	&_GcatClass,
	ERx("abfglobals"),
	ERx("globals"),
	NULL,		/* loaded at runtime with IIamGlobCl */		 
	0, FALSE,
	OC_UNDEFINED, OC_UNDEFINED,
	FALSE ,
	/* func, menuitem		char  explanation	    char */
	/* pseudo-menuitem for initialization. has title and explanation. */
	{ NULL, F_AB0314_GloEdit_title, NULL, F_AB0304_Glob_expl, NULL},
	/* menuitem for create. */
	{ _glCreate, FE_Create, NULL, F_AB0315_CreateGlo_expl, NULL },
	/* menuitem for destroy. */
	{ NULL, FE_Destroy, NULL, F_AB0316_DestroyGlo_expl, NULL },
	/* menuitem for edit. */
	{ _glEdit, FE_Edit, NULL, F_AB0317_EditGlo_expl, NULL  },
	/* menuitem for end. */
	{ NULL, FE_End, NULL, F_FE0102_ExplEnd, NULL },
	/* menuitem for quit. */
	{ NULL, 0, NULL, 0, NULL },
	iiabGetComp,
	ERx("abedtglo.hlp")
};
/* ABF Catalog class for Class Definitions. */
GLOBALDEF AB_CATALOG	iiabRcatalog =
{
	&_RcatClass,
	ERx("abfrecords"),
	ERx("records"),
	NULL,		/* loaded at runtime with IIamRecCl */		 
	0, FALSE,
	OC_UNDEFINED, OC_UNDEFINED,
	FALSE,
	{ NULL, F_AB0336_RecEdit_title, NULL, F_AB0334_Rec_expl, NULL },
	{ _rcCreate, FE_Create,	NULL, F_AB0337_CreateRec_expl, NULL },
	{ NULL, FE_Destroy,	NULL, F_AB0338_DestroyRec_expl, NULL },
	{ _rEdit, FE_Edit,	NULL, F_AB0339_EditRec_expl, NULL },
	{ NULL, FE_End, 	NULL, F_FE0102_ExplEnd, NULL },
	{ NULL, 0, 		NULL, 0, NULL },
	iiabGetComp,
	ERx("abedtcls.hlp")
};
/* ABF Catalog class for Class Attributes. */
GLOBALDEF AB_CATALOG	iiabMcatalog =
{
	&_McatClass,
	ERx("abfrecd"),
	ERx("members"),
	NULL,		/* loaded at runtime with IIamRAttCl */		 
	0, FALSE,
	OC_UNDEFINED, OC_UNDEFINED,
	FALSE,
	{ _raInit, F_AB006A_AReDef_title, NULL, F_AB0344_RAtt_expl, NULL },
	{ _raCreate, FE_Create,	NULL, F_AB0347_InsertRAtt_expl, NULL },
	{ _raDestroy, FE_Destroy,	NULL, F_AB0348_DeleteRAtt_expl, NULL },
	{ _raEdit, FE_Edit,	NULL, F_AB0349_EditRAtt_expl, NULL },
	{ _raEnd, FE_End, 	NULL, F_FE0102_ExplEnd, NULL },
	{ NULL, 0, 		NULL, 0, NULL },
	_ra_get,
	ERx("abedtatt.hlp")
};
static STATUS
  _coInit( self, obj )
  AB_CATALOG *self;
APPL_COMP	*obj;
  {
    /* ever since we got rid of the "language" field, this routine does
    ** nothing. Leave in for now....
    */
    return OK;
  }
static VOID
  _c_catInit( self, mode )
  AB_CATALOG *self;
char		*mode;
  {
    char *rmode;
	rmode = (mode != NULL ? mode : ERx("update"));
/* # line 368 "editglo.qsc" */	/* inittable */
    {
      if (IItbinit(self->c_form,self->c_tfield,rmode) != 0) {
        IIthidecol(_id,(char *)"i4");
        IIthidecol(_class,(char *)"i4");
        IIthidecol(_env,(char *)"i4");
        IIthidecol(_owner,(char *)"c32");
        IIthidecol(_is_current,(char *)"i4");
        IIthidecol(_create_date,(char *)"c25");
        IIthidecol(_altr_date,(char *)"c25");
        IIthidecol(_altr_count,(char *)"i4");
        IIthidecol(_language,(char *)"c32");
        IIthidecol(_last_altered_by,(char *)"c32");
        IItfill();
      } /* IItbinit */
    }
  }
/* # line 382 "editglo.qsc" */	/* host code */
static VOID
  _cLoadTable( self, csp )
  AB_CATALOG *self;
  AB_CATREC *csp;
  {
    char *owner = IIOOmapOwner(csp->owner);
    AB_CONSTANT *constant = (AB_CONSTANT *) csp ;
/* # line 390 "editglo.qsc" */	/* loadtable */
    {
      if (IItbact(self->c_form,self->c_tfield,1) != 0) {
        IItcoputio(_id,(short *)0,1,30,4,&constant->id);
        IItcoputio(_class,(short *)0,1,30,4,&constant->class);
        IItcoputio(_name,(short *)0,1,32,0,constant->name);
        IItcoputio(_env,(short *)0,1,30,4,&constant->env);
        IItcoputio(_owner,(short *)0,1,32,0,owner);
        IItcoputio(_is_current,(short *)0,1,30,4,&constant->is_current);
        IItcoputio(_short_remark,(short *)0,1,32,0,constant->short_remark);
        IItcoputio(_create_date,(short *)0,1,32,0,constant->create_date);
        IItcoputio(_altr_date,(short *)0,1,32,0,constant->alter_date);
        IItcoputio(_altr_count,(short *)0,1,30,4,&constant->alter_cnt);
        IItcoputio(_last_altered_by,(short *)0,1,32,0,constant->altered_by);
        IItcoputio(_data_type,(short *)0,1,32,0,constant->type);
        IItcoputio(_value,(short *)0,1,32,0,constant->value);
        IItcoputio(_language,(short *)0,1,32,0,constant->language);
        IITBceColEnd();
      } /* IItbact */
    }
  }
/* # line 408 "editglo.qsc" */	/* host code */
/*
** Name:	_c_catLoad() -	Load ABF Application Constants Catalogs.
**
** Description:
**	Loads constants information from ii_objects and ii_abfobjects into
**	Application Definition structure.
**
**	It should be noted that ii_abfobjects.abf_source contains the constant's
**	value, and, ii_abfobjects.abf_arg1 contains the ascii representation of
**	the user language code defined at the time the constant was defined.
**
** Input:
**	self	{AB_CATALOG *}  the application whose constants are to be loaded.
**
** Side Effects:
**
** History:
**	7-jan-92 (blaise)
**	    Made a small change to the order of the where clause of the
**	    select statement in this function (changed 'a OR b' to 'b OR a')
**	    The DB2 gateway wasn't accepting the original version and the
**	    change has no effect elsewhere.
**	20-jan-92 (davel)  bug 41920
**		Part of fix for bug 41920 - clear the tablefield before
**		re-selecting and re-displaying constants rows if
**		iiabCopyConstants() is called to insert a new set of
**		constant entries for a newly encountered language.	
**	16-oct-92 (davel)
**		Remove obsolete arg from iiabCopyConstants().
**	18-Jan-2005 (schka24)
**	    DB_DATA_VALUE changed, select its components explicitly.
*/
static VOID
  _c_catLoad( self, pattern, lastname, prevrow )
  AB_CATALOG *self;
char	*pattern;
char	*lastname;
i4	*prevrow;
  {
    bool	constants;
    bool	tryagain;
    i4		nrows;
  OOID applid;
  OOID class;
  AB_CONSTANT constant_row;
  char pattern_buf[FE_MAXNAME*DB_MAX_COLS];
  char *language;
  DB_DATA_DESC dbdv;
    char	db_data[FE_MAXNAME+1];
    dbdv.db_data = db_data;
    dbdv.db_collID = 0;
    if ( pattern == NULL || *pattern == EOS )
	STcopy( ERx("%"), pattern_buf );
    else
    { /* change the pattern matching to SQL */
	i4	sql_wild;
	STcopy( pattern, pattern_buf );
	IIUGwildcard( pattern_buf, &sql_wild );
    }
    language = _DefaultLang;
    class = self->c_class->ooid;
    applid = self->c_appid;
    do
    {
	constants = FALSE;
	tryagain = FALSE;
	nrows = 0;
/* # line 446 "editglo.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"editglo1",6919,15475);
      IIputdomio((short *)0,1,32,0,pattern_buf);
      IIputdomio((short *)0,1,30,4,&class);
      IIputdomio((short *)0,1,30,4,&applid);
      IIputdomio((short *)0,1,32,0,language);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"editglo1",6919,15475);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select ii_objects.object_id, object_class, object_name, object_owner,\
 object_env, is_current, short_remark, create_date, alter_date, '', al\
ter_count, last_altered_by, rettype, '', abf_source, abf_arg1, rettype\
, retlength, retadf_type, retscale from ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"ii_objects, ii_abfobjects where ii_objects.object_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,pattern_buf);
        IIwritio(0,(short *)0,1,32,0,(char *)"and ii_objects.object_cl\
ass=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&class);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and ii_objects.object_id=ii_abfobjects.object_id and ii_abfobjects.ap\
plid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&applid);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and(ii_abfobjects.abf_arg1='' or ii_abfobjects.abf_arg1=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,language);
        IIwritio(0,(short *)0,1,32,0,(char *)
")order by object_name, object_owner");
        IIexDefine(0,(char *)"editglo1",6919,15475);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&(constant_row).id);
      IIgetdomio((short *)0,1,30,4,&(constant_row).class);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,(constant_row).name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,(constant_row).owner);
      IIgetdomio((short *)0,1,30,4,&(constant_row).env);
      IIgetdomio((short *)0,1,30,4,&(constant_row).is_current);
      IIgetdomio((short *)0,1,32,OOSHORTREMSIZE+1-1,
      (constant_row).short_remark);
      IIgetdomio((short *)0,1,32,OODATESIZE+1-1,(constant_row).create_date);
      IIgetdomio((short *)0,1,32,OODATESIZE+1-1,(constant_row).alter_date);
      IIgetdomio((short *)0,1,32,OOLONGREMSIZE+1-1,(constant_row).long_remark);
      IIgetdomio((short *)0,1,30,4,&(constant_row).alter_cnt);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,(constant_row).altered_by);
      IIgetdomio((short *)0,1,32,TYPE_LEN+1-1,(constant_row).type);
      IIgetdomio((short *)0,1,32,Y_N_LEN+1-1,(constant_row).nullable);
      IIgetdomio((short *)0,1,32,VALUE_LEN+1-1,(constant_row).value);
      IIgetdomio((short *)0,1,32,LANGUAGE_LEN+1-1,(constant_row).language);
      IIgetdomio((short *)0,1,32,0,dbdv.db_data);
      IIgetdomio((short *)0,1,30,4,&dbdv.db_length);
      IIgetdomio((short *)0,1,30,2,&dbdv.db_datatype);
      IIgetdomio((short *)0,1,30,2,&dbdv.db_scale);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 464 "editglo.sc" */	/* host code */
	{
		STlcopy( dbdv.db_data, constant_row.type,
				sizeof(constant_row.type) - 1
		);
		if ( STtrmwhite(constant_row.language) > 0 )
			constants = TRUE;
		_send(catRowLoad, self)( self, (AB_CATREC *) &constant_row );
		if ( ++nrows == self->c_maxrow )
/* # line 519 "editglo.qsc" */	/* redisplay */
    {
      IIredisp();
    }
/* # line 520 "editglo.qsc" */	/* host code */
		if ( lastname != NULL && STequal(lastname, constant_row.name)
				&& prevrow != NULL )
		{
			*prevrow = nrows;
		}
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
	if (!constants && FEinqerr() == OK)
	{
		if (iiabCopyConstants( applid, language ) > 0)
		{
			_send(catEmpty, self)( self );
			tryagain = TRUE;
		}
	}
    } while ( tryagain );
    if ( FEinqerr() != OK )
	EXsignal(EXDECLARE, 0);
  }
static VOID
  _cInsertRow( self, obj, rowno )
  AB_CATALOG *self;
  OO_OBJECT *obj;
  i4 rowno;
  {
/* # line 548 "editglo.qsc" */	/* insertrow */
    {
      if (IItbsetio(5,self->c_form,self->c_tfield,rowno) != 0) {
        if (IItinsert() != 0) {
          IITBceColEnd();
        } /* IItinsert */
      } /* IItbsetio */
    }
/* # line 549 "editglo.qsc" */	/* host code */
	if ( rowno < self->c_maxrow )
		++rowno;
	_cPutRow(self, obj, rowno );
  }
static VOID
  _cPutRow( self, obj, rowno )
  AB_CATALOG *self;
  OO_OBJECT *obj;
  i4 rowno;
  {
    char *owner = IIOOmapOwner(obj->owner);
    CONSTANT *constant = (CONSTANT *)obj;
    char *value;
    char dtype[TYPE_LEN + 1];
    value = constant->value[0];
    STlcopy(constant->data_type.db_data, dtype, sizeof(dtype) - 1);
/* # line 568 "editglo.qsc" */	/* putrow */
    {
      if (IItbsetio(3,self->c_form,self->c_tfield,rowno) != 0) {
        IItcoputio(_name,(short *)0,1,32,0,constant->name);
        IItcoputio(_id,(short *)0,1,30,4,&constant->ooid);
        IItcoputio(_class,(short *)0,1,30,4,&constant->class);
        IItcoputio(_env,(short *)0,1,30,4,&constant->env);
        IItcoputio(_owner,(short *)0,1,32,0,owner);
        IItcoputio(_is_current,(short *)0,1,30,4,&constant->is_current);
        IItcoputio(_short_remark,(short *)0,1,32,0,constant->short_remark);
        IItcoputio(_create_date,(short *)0,1,32,0,constant->create_date);
        IItcoputio(_altr_date,(short *)0,1,32,0,constant->alter_date);
        IItcoputio(_altr_count,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IItcoputio(_last_altered_by,(short *)0,1,32,0,owner);
        IItcoputio(_data_type,(short *)0,1,32,0,dtype);
        IItcoputio(_value,(short *)0,1,32,0,value);
        IITBceColEnd();
      } /* IItbsetio */
    }
  }
/* # line 585 "editglo.qsc" */	/* host code */
/*
** _*catUpdate 'methods' - Update a single row of the class catalog.
**
** Inputs:
**	self		{AB_CATALOG*} descriptor for the catalog class.
**	obj		{APPL_COMP*}  The specific object that's being changed.
*/
static VOID
  _gcatUpdate( self, obj )
  AB_CATALOG *self;
  APPL_COMP *obj;
  {
    char *nmsg;
    char *typename;
    DB_DATA_DESC *desc;
	if (obj->class == OC_RECMEM)
		desc = &(((RECMEM *)obj)->data_type);
	else if (obj->class == OC_GLOBAL)
		desc = &(((GLOBVAR *)obj)->data_type);
	typename = _TypeName(desc);
	nmsg = _nullmsg(desc);
/* # line 610 "editglo.qsc" */	/* putrow */
    {
      if (IItbsetio(3,self->c_form,self->c_tfield,-2) != 0) {
        IItcoputio(_name,(short *)0,1,32,0,obj->name);
        IItcoputio(_short_remark,(short *)0,1,32,0,obj->short_remark);
        IItcoputio(_data_type,(short *)0,1,32,0,typename);
        IItcoputio(_type,(short *)0,1,32,0,typename);
        IItcoputio(_nullable,(short *)0,1,32,0,nmsg);
        IITBceColEnd();
      } /* IItbsetio */
    }
  }
/* # line 619 "editglo.qsc" */	/* host code */
static VOID
  _rcatUpdate( self, rdef )
  AB_CATALOG *self;
  RECDEF *rdef;
  {
/* # line 624 "editglo.qsc" */	/* putrow */
    {
      if (IItbsetio(3,self->c_form,self->c_tfield,-2) != 0) {
        IItcoputio(_name,(short *)0,1,32,0,rdef->name);
        IItcoputio(_short_remark,(short *)0,1,32,0,rdef->short_remark);
        IITBceColEnd();
      } /* IItbsetio */
    }
  }
/* # line 630 "editglo.qsc" */	/* host code */
/*
** _*_catInit 'methods' - Initialize a tablefield for a catalog class.
**
**	This pseudo-method sets up some hidden columns in the tablefield.
**
** Inputs:
**	self		{AB_CATALOG*} descriptor for the catalog class.
**	mode		{char*}  The mode of the tablefield.
*/
static VOID
  _g_catInit( self, mode )
  AB_CATALOG *self;
char		*mode;
  {
    char *rmode;
	rmode = (mode != NULL ? mode : ERx("update"));
/* # line 649 "editglo.qsc" */	/* inittable */
    {
      if (IItbinit(self->c_form,self->c_tfield,rmode) != 0) {
        IIthidecol(_id,(char *)"i4");
        IIthidecol(_class,(char *)"i4");
        IIthidecol(_env,(char *)"i4");
        IIthidecol(_owner,(char *)"c32");
        IIthidecol(_is_current,(char *)"i4");
        IIthidecol(_create_date,(char *)"c25");
        IIthidecol(_altr_date,(char *)"c25");
        IIthidecol(_altr_count,(char *)"i4");
        IIthidecol(_type,(char *)"c32");
        IIthidecol(_last_altered_by,(char *)"c32");
        IItfill();
      } /* IItbinit */
    }
  }
/* # line 663 "editglo.qsc" */	/* host code */
static VOID
  _r_catInit( self, mode )
  AB_CATALOG *self;
char		*mode;
  {
    char *rmode;
	rmode = (mode != NULL ? mode : ERx("update"));
	/*
	** create dummy columns for nullable and data_type, so we can re-use
	** the other catalog methods for both globals and class defs.
	*/
/* # line 676 "editglo.qsc" */	/* inittable */
    {
      if (IItbinit(self->c_form,self->c_tfield,rmode) != 0) {
        IIthidecol(_id,(char *)"i4");
        IIthidecol(_class,(char *)"i4");
        IIthidecol(_env,(char *)"i4");
        IIthidecol(_owner,(char *)"c32");
        IIthidecol(_is_current,(char *)"i4");
        IIthidecol(_create_date,(char *)"c25");
        IIthidecol(_altr_date,(char *)"c25");
        IIthidecol(_altr_count,(char *)"i4");
        IIthidecol(_last_altered_by,(char *)"c32");
        IIthidecol(_nullable,(char *)"c8");
        IIthidecol(_type,(char *)"c32");
        IIthidecol(_data_type,(char *)"c32");
        IItfill();
      } /* IItbinit */
    }
  }
/* # line 692 "editglo.qsc" */	/* host code */
/*
** _*_catLoad 'methods' - Load the catalog tablefield for a class.
**
** Inputs:
**	self		{AB_CATALOG*} descriptor for the catalog class.
**	pattern		{char*}	search pattern for object name.
**	lastname	{char*}	accounting stuff for keeping track of rows.
**	prevrow		{nat*}	more accounting stuff for keeping track of rows.
*/
static VOID
_g_catLoad ( self, pattern, lastname, prevrow )
  AB_CATALOG *self;
char		*pattern;
char		*lastname;
i4		*prevrow;
  {
	i4		nrows = 0;
  OOID applid;
  OOID class;
  AB_CATREC row;
  char pattern_buf[FE_MAXNAME*DB_MAX_COLS];
  char symbol[FE_MAXNAME+1];
  char source[FE_MAXNAME+1];
  DB_DATA_DESC dbdv;
	char		db_data[FE_MAXNAME+1];
	dbdv.db_data = db_data;
	dbdv.db_collID = 0;
	if ( pattern == NULL || *pattern == EOS )
		STcopy( ERx("%"), pattern_buf );
	else
	{ /* change the pattern matching to SQL */
		i4	sql_wild;
		STcopy( pattern, pattern_buf );
		IIUGwildcard( pattern_buf, &sql_wild );
	}
	class = self->c_class->ooid;
	applid = self->c_appid;
/* # line 709 "editglo.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"editglo2",6920,7765);
      IIputdomio((short *)0,1,32,0,pattern_buf);
      IIputdomio((short *)0,1,30,4,&class);
      IIputdomio((short *)0,1,30,4,&applid);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"editglo2",6920,7765);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select ii_objects.object_id, object_class, object_name, object_owner,\
 object_env, is_current, short_remark, create_date, alter_date, '', al\
ter_count, last_altered_by, rettype, '', abf_symbol, abf_source, retty\
pe, retlength, retadf_type, retscale from ");
        IIwritio(0,(short *)0,1,32,0,(char *)
"ii_objects, ii_abfobjects where ii_objects.object_name like");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,pattern_buf);
        IIwritio(0,(short *)0,1,32,0,(char *)"and ii_objects.object_cl\
ass=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&class);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and ii_objects.object_id=ii_abfobjects.object_id and ii_abfobjects.ap\
plid=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&applid);
        IIwritio(0,(short *)0,1,32,0,(char *)
"order by object_name, object_owner");
        IIexDefine(0,(char *)"editglo2",6920,7765);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&(row).id);
      IIgetdomio((short *)0,1,30,4,&(row).class);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,(row).name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,(row).owner);
      IIgetdomio((short *)0,1,30,4,&(row).env);
      IIgetdomio((short *)0,1,30,4,&(row).is_current);
      IIgetdomio((short *)0,1,32,OOSHORTREMSIZE+1-1,(row).short_remark);
      IIgetdomio((short *)0,1,32,OODATESIZE+1-1,(row).create_date);
      IIgetdomio((short *)0,1,32,OODATESIZE+1-1,(row).alter_date);
      IIgetdomio((short *)0,1,32,OOLONGREMSIZE+1-1,(row).long_remark);
      IIgetdomio((short *)0,1,30,4,&(row).alter_cnt);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,(row).altered_by);
      IIgetdomio((short *)0,1,32,TYPE_LEN+1-1,(row).type);
      IIgetdomio((short *)0,1,32,Y_N_LEN+1-1,(row).nullable);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,symbol);
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,source);
      IIgetdomio((short *)0,1,32,0,dbdv.db_data);
      IIgetdomio((short *)0,1,30,4,&dbdv.db_length);
      IIgetdomio((short *)0,1,30,2,&dbdv.db_datatype);
      IIgetdomio((short *)0,1,30,2,&dbdv.db_scale);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 724 "editglo.sc" */	/* host code */
	{
		STlcopy(_nullmsg(&dbdv), row.nullable, sizeof(row.nullable)-1);
		STlcopy(_TypeName(&dbdv), row.type, sizeof(row.type) - 1);
		_send(catRowLoad, self)( self, &row );
		if ( ++nrows == self->c_maxrow )
/* # line 761 "editglo.qsc" */	/* redisplay */
    {
      IIredisp();
    }
/* # line 762 "editglo.qsc" */	/* host code */
		if ( lastname != NULL && STequal(lastname, row.name) &&
				prevrow != NULL )
		{
			*prevrow = nrows;
		}
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
	if ( FEinqerr() != OK )
		EXsignal(EXDECLARE, 0);
  }
static VOID
  _ra_catLoad( self, pattern, lastname, prevrow )
  AB_CATALOG *self;
char		*pattern;
char		*lastname;
i4		*prevrow;
  {
	register RECMEM *rap;
	register RECDEF *rec = (RECDEF*) OOp(iiabRcatalog.c_ooid);
	i4		nrows = 0;
	for ( rap = rec->recmems ; rap != NULL ; rap = rap->next_mem )
	{
		AB_CATREC	row;
		DB_DATA_DESC *dbdv = &(rap->data_type);
		STlcopy(rap->name, row.name, sizeof(row.name) - 1);
		STlcopy(_nullmsg(dbdv), row.nullable, sizeof(row.nullable) - 1);
		STlcopy(_TypeName(dbdv), row.type, sizeof(row.type) - 1);
		STlcopy(rap->owner, row.owner, sizeof(row.owner) - 1);
		STlcopy(rap->short_remark, row.short_remark,
						sizeof(row.short_remark) - 1);
		STlcopy(rap->create_date, row.create_date,
						sizeof(row.create_date) - 1);
		STlcopy(rap->alter_date, row.alter_date,
						sizeof(row.alter_date) - 1);
		STlcopy(rap->altered_by, row.altered_by,
						sizeof(row.altered_by) - 1);
		row.id = (OOID) rap->ooid;
		row.class = (OOID) OC_RECMEM;
		row.env = 0;
		row.is_current = 0;
		_send(catRowLoad, self)( self, &row );
		if ( ++nrows == self->c_maxrow )
/* # line 813 "editglo.qsc" */	/* redisplay */
    {
      IIredisp();
    }
/* # line 814 "editglo.qsc" */	/* host code */
		if (lastname != NULL && STequal(lastname, row.name)
		  && prevrow != NULL)
		{
			*prevrow = nrows;
		}
	} /* end for */
  }
/*
** _*LoadTable 'methods' - Load the catalog tablefield for a class.
**
** Inputs:
**	self		{AB_CATALOG*} descriptor for the catalog class.
**	csp		{AB_CATREC*}  struct describing what goes into the row.
*/
static VOID
  _gLoadTable( self, csp )
  AB_CATALOG *self;
  AB_CATREC *csp;
  {
    char *owner = IIOOmapOwner(csp->owner);
/* # line 837 "editglo.qsc" */	/* loadtable */
    {
      if (IItbact(self->c_form,self->c_tfield,1) != 0) {
        IItcoputio(_id,(short *)0,1,30,4,&csp->id);
        IItcoputio(_class,(short *)0,1,30,4,&csp->class);
        IItcoputio(_name,(short *)0,1,32,0,csp->name);
        IItcoputio(_env,(short *)0,1,30,4,&csp->env);
        IItcoputio(_owner,(short *)0,1,32,0,owner);
        IItcoputio(_is_current,(short *)0,1,30,4,&csp->is_current);
        IItcoputio(_short_remark,(short *)0,1,32,0,csp->short_remark);
        IItcoputio(_create_date,(short *)0,1,32,0,csp->create_date);
        IItcoputio(_altr_date,(short *)0,1,32,0,csp->alter_date);
        IItcoputio(_altr_count,(short *)0,1,30,4,&csp->alter_cnt);
        IItcoputio(_last_altered_by,(short *)0,1,32,0,csp->altered_by);
        IItcoputio(_data_type,(short *)0,1,32,0,csp->type);
        IItcoputio(_nullable,(short *)0,1,32,0,csp->nullable);
        IITBceColEnd();
      } /* IItbact */
    }
  }
/* # line 854 "editglo.qsc" */	/* host code */
static VOID
  _rLoadTable( self, csp )
  AB_CATALOG *self;
  AB_CATREC *csp;
  {
    char *owner = IIOOmapOwner(csp->owner);
    char dtype[TYPE_LEN + 1];
	STlcopy(OOpclass(OC_RECORD)->name, dtype, sizeof(dtype) - 1);
/* # line 864 "editglo.qsc" */	/* loadtable */
    {
      if (IItbact(self->c_form,self->c_tfield,1) != 0) {
        IItcoputio(_id,(short *)0,1,30,4,&csp->id);
        IItcoputio(_class,(short *)0,1,30,4,&csp->class);
        IItcoputio(_name,(short *)0,1,32,0,csp->name);
        IItcoputio(_env,(short *)0,1,30,4,&csp->env);
        IItcoputio(_owner,(short *)0,1,32,0,owner);
        IItcoputio(_is_current,(short *)0,1,30,4,&csp->is_current);
        IItcoputio(_short_remark,(short *)0,1,32,0,csp->short_remark);
        IItcoputio(_create_date,(short *)0,1,32,0,csp->create_date);
        IItcoputio(_altr_date,(short *)0,1,32,0,csp->alter_date);
        IItcoputio(_altr_count,(short *)0,1,30,4,&csp->alter_cnt);
        IItcoputio(_last_altered_by,(short *)0,1,32,0,csp->altered_by);
        IItcoputio(_type,(short *)0,1,32,0,dtype);
        IItcoputio(_data_type,(short *)0,1,32,0,csp->type);
        IItcoputio(_nullable,(short *)0,1,32,0,csp->nullable);
        IITBceColEnd();
      } /* IItbact */
    }
  }
/* # line 882 "editglo.qsc" */	/* host code */
/*
** _*InsertRow 'methods' - Insert a row into the catalog tablefield for a class.
**
** Inputs:
**	self		{AB_CATALOG*} descriptor for the catalog class.
**	obj		{OO_OBJECT*}  struct describing what goes into the row.
**	rowno		{nat}	the row number.
*/
static VOID
  _gInsertRow( self, obj, rowno )
  AB_CATALOG *self;
  OO_OBJECT *obj;
  i4 rowno;
  {
/* # line 897 "editglo.qsc" */	/* insertrow */
    {
      if (IItbsetio(5,self->c_form,self->c_tfield,rowno) != 0) {
        if (IItinsert() != 0) {
          IITBceColEnd();
        } /* IItinsert */
      } /* IItbsetio */
    }
/* # line 898 "editglo.qsc" */	/* host code */
	if ( rowno < self->c_maxrow )
		++rowno;
	_gPutRow(self, obj, rowno );
  }
/*
** _*PutRow 'methods' - Put a row into the catalog tablefield for a class.
**
** Inputs:
**	self		{AB_CATALOG*} descriptor for the catalog class.
**	obj		{OO_OBJECT*}  struct describing what goes into the row.
**	rowno		{nat}	the row number.
*/
static VOID
  _gPutRow( self, obj, rowno )
  AB_CATALOG *self;
  register OO_OBJECT *obj;
  i4 rowno;
  {
    char *owner = IIOOmapOwner(obj->owner);
    char *nmsg;
    char dtype[TYPE_LEN + 1];
	DB_DATA_DESC *desc;
	desc = NULL;
	nmsg = ERx("");
	dtype[0] = EOS;
	if (obj->class == OC_RECMEM)
	{
		desc = &(((RECMEM *)obj)->data_type);
	}
	else if (obj->class == OC_GLOBAL)
	{
		desc = &(((GLOBVAR *)obj)->data_type);
	}
	else if (obj->class == OC_RECORD)
	{
		STlcopy(OOpclass(OC_RECORD)->name, dtype, sizeof(dtype) - 1);
	}
	if (desc != NULL)
	{
		nmsg = _nullmsg(desc);
		STlcopy(_TypeName(desc), dtype, sizeof(dtype) - 1);
	}
/* # line 946 "editglo.qsc" */	/* putrow */
    {
      if (IItbsetio(3,self->c_form,self->c_tfield,rowno) != 0) {
        IItcoputio(_name,(short *)0,1,32,0,obj->name);
        IItcoputio(_id,(short *)0,1,30,4,&obj->ooid);
        IItcoputio(_class,(short *)0,1,30,4,&obj->class);
        IItcoputio(_env,(short *)0,1,30,4,&obj->env);
        IItcoputio(_owner,(short *)0,1,32,0,owner);
        IItcoputio(_is_current,(short *)0,1,30,4,&obj->is_current);
        IItcoputio(_short_remark,(short *)0,1,32,0,obj->short_remark);
        IItcoputio(_create_date,(short *)0,1,32,0,obj->create_date);
        IItcoputio(_altr_date,(short *)0,1,32,0,obj->alter_date);
        IItcoputio(_altr_count,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IItcoputio(_last_altered_by,(short *)0,1,32,0,owner);
        IItcoputio(_data_type,(short *)0,1,32,0,dtype);
        IItcoputio(_type,(short *)0,1,32,0,dtype);
        IItcoputio(_nullable,(short *)0,1,32,0,nmsg);
        IITBceColEnd();
      } /* IItbsetio */
    }
  }
/* # line 965 "editglo.qsc" */	/* host code */
/*
** What follows are AB_CATALOG 'methods'
*/
/*
** _*_get 'methods' - given an object id, return a pointer to the object.
**
** Inputs:
**	app		{APPL*}		Descriptor of the application.
**	id		{OOID}  	Id of object to find.
*/
/* _ra_get -- get a class attribute */
static APPL_COMP *
  _ra_get( app, id )
  APPL *app;
  OOID id;
  {
	RECMEM *rap = (RECMEM *) OOp(id);
	return (APPL_COMP *) rap;
  }
/*
** _*Init 'methods' - Initialization for specific catalog classes.
**
** Inputs:
**	self		{AB_CATALOG*}	Descriptor of the class.
**	obj		{APPL_COMP*}
*/
STATUS	IIAMraReadAtts();
static STATUS
  _raInit( self, ratt )
  AB_CATALOG *self;
  RECMEM *ratt;
  {
    RECDEF *parent = (RECDEF*) OOp(iiabRcatalog.c_ooid);
/* # line 1005 "editglo.qsc" */	/* putform */
    {
      if (IIfsetio(self->c_form) != 0) {
        IIputfldio((char *)"record_name",(short *)0,1,32,0,parent->name);
      } /* IIfsetio */
    }
/* # line 1007 "editglo.qsc" */	/* host code */
	rec_changed = FALSE;
	/* load the class attributes */
	return IIAMraReadAtts(parent);
  }
/*
** Name:	_rcCreate() -	Create Class Definition.
**
** Description:
**	Object-creation actions specific to OC_RECORD objects.
**
** Input:
**	self	{AB_CATALOG *}  the object description.
**	obj	{APPL_COMP *}  the object that we're creating.
*/
static READONLY
  char Rec_cr_form[]= ERx("abfreccreate");
static bool	Rec_fi_form_inited = FALSE;
static STATUS
_rcCreate ( app, class, self )
APPL	*app;
OOID	class;
AB_CATALOG *self;
  {
	RECDEF	*obj;
	char	title[63+4];
	RECDEF 	new;
	new.class = class;
	new.appl = app;
	if ( !Rec_fi_form_inited )
	{
		IIARiaForm(Rec_cr_form);
		Rec_fi_form_inited = TRUE;
	}
	IIUGfmt( title,sizeof(title)-1,ERget(F_AB031D_ARecCrt_title),
		1,IIabExename
	);
	obj = (RECDEF *)_createObj((APPL_COMP *)&new, self, title, Rec_cr_form);
	if (obj == NULL)
		return FAIL;
	self->c_ooid = obj->ooid;
	/* Now do catalog edit on child class */
	IIABcaCatalogEdit ( obj->appl, OC_RECMEM, self->c_noload, FALSE );
	self->c_ooid = OC_UNDEFINED;
	return OK;
  }
/*
** Name:	_glCreate() -	Create an object.
**
** Input:
**	self	{AB_CATALOG *}  the object description.
**	obj	{APPL_COMP *}  the object that we're creating.
**
*/
static READONLY
  char Gl_cr_form[]= ERx("abfpartcreate");
static bool	Gl_fi_form_inited = FALSE;
static STATUS
_glCreate ( app, class, self )
APPL	*app;
OOID	class;
AB_CATALOG *self;
  {
	char	title[63+4];
	GLOBVAR new;
	new.class = class;
	new.appl = app;
	if ( !Gl_fi_form_inited )
	{
		IIARiaForm(Gl_cr_form);
		Gl_fi_form_inited = TRUE;
	}
	IIUGfmt( title, sizeof(title)-1, ERget(F_AB031C_AGloCrt_title),
			1, IIabExename
	);
	if ( _createObj( (APPL_COMP *)&new, self, title, Gl_cr_form ) == NULL )
		return FAIL;
	return OK;
  }
/*
** Name:	_coCreate() -	Create an object.
**
** Input:
**	self	{AB_CATALOG *}  the object description.
**	obj	{APPL_COMP *}  the object that we're creating.
**
*/
static READONLY char	Co_cr_form[] = ERx("abfconstedit");
static bool		Co_fi_form_inited = FALSE;
static STATUS
  _coCreate( app, class, self )
APPL	*app;
OOID	class;
  AB_CATALOG *self;
  {
	CONSTANT	new;
 	char		title[63+4];
	new.class = OC_CONST;
	new.appl = app;
	new.language = _DefaultLang;
	new.data_type.db_datatype = DB_NODT;
	new.data_type.db_length = 0;
	new.data_type.db_scale = 0;
	new.data_type.db_data = ERx("none");
	if ( !Co_fi_form_inited )
	{
		IIARiaForm(Co_cr_form);
		Co_fi_form_inited = TRUE;
	}
	IIUGfmt( title, sizeof(title)-1, ERget(F_AB031E_AConstCrt_title),
		1, IIabExename
	);
	if ( _createObj( (APPL_COMP*)&new, self, title, Co_cr_form ) == NULL )
		return FAIL;
	return OK;
  }
/*
** Name:	_raDestroy() -	Destroy Class Attribuge.
**
** Description:
**	Object-destruction actions specific to OC_RECMEM objects.
**
** Input:
**	self	{AB_CATALOG *}  the object description.
**	obj	{APPL_COMP *}  the object that we're creating.
*/
/*ARGSUSED*/
static STATUS
_raDestroy( class, self )
{
	rec_changed = TRUE;	/* Redo the dependencies */
}
/*
** Name:	_raCreate() -	Create Class Attribuge.
**
** Description:
**	Object-creation actions specific to OC_RECMEM objects.
**
** Input:
**	self	{AB_CATALOG *}  the object description.
**	obj	{APPL_COMP *}  the object that we're creating.
*/
static READONLY
  char Ra_cr_form[]= ERx("abfpartcreate");
static bool	Ra_fi_form_inited = FALSE;
static STATUS
  _raCreate( app, class, self )
APPL	*app;
OOID	class;
  AB_CATALOG *self;
  {
	RECDEF *parent = (RECDEF*) OOp(iiabRcatalog.c_ooid);
	char	title[63+4];
	RECMEM	new;
	STATUS status;
	if ( !Ra_fi_form_inited )
	{
		IIARiaForm(Ra_cr_form);
		Ra_fi_form_inited = TRUE;
	}
	IIUGfmt( title, sizeof(title)-1, ERget(F_AB031F_ARattCrt_title),
			1, IIabExename
	);
	new.class = OC_RECMEM;
	new.appl = app;
	new.parent = (APPL_COMP *) parent;
	status = ( _createObj( (APPL_COMP *)&new, self, title, Ra_cr_form ) == 
		  NULL)
		? FAIL : OK;
	if (status == OK)
		rec_changed = TRUE;
	return status;
  }
/*
** Name:	_raEnd() -	End edit of record attributes
**
** Description:
**	Edit-end actions specific to OC_RECMEM objects.
**
** Input:
**	self	{AB_CATALOG *}  the object description.
**	dummy	{APPL_COMP *}
*/
/*ARGSUSED*/
static STATUS
_raEnd(self, dummy)
AB_CATALOG *self;
APPL_COMP *dummy;
{
	RECDEF *parent = (RECDEF*) OOp(iiabRcatalog.c_ooid);
	if (rec_changed)
	{
		/* Redo the inter-record dependencies */
		IIAMxrdRecordDependencies(parent);
                /* Invalidate our callers */
                IIAMxdsDepSearch((APPL_COMP *)parent, IIAM_zccCompChange,
                                 (PTR)NULL);
	}
	return OK;
}
/*
** Name:	_coEdit() -	Edit a Constant.
**
** Description:
**	Object-edit actions specific to OC_CONST objects.
**
** Input:
**	self	{AB_CATALOG *}  the object description.
**	obj	{APPL_COMP *}  the object that we're editing.
**
** History:
** 	11/89 (jhw) - Copied from '_glEdit()'.
*/
static STATUS
  _coEdit( self, obj )
  AB_CATALOG *self;
APPL_COMP	*obj;
  {
	((CONSTANT *)obj)->language = _DefaultLang;
	IIABedtComp(obj->appl, obj, self);
	_send(catPutRow, self)(self, obj, rowCURR);
	return OK;
  }
/*
** Name:	_glEdit() -	Edit a Global Variable.
**
** Description:
**	Object-edit actions specific to OC_GLOBAL objects.
**
** Input:
**	self	{AB_CATALOG *}  the object description.
**	obj	{APPL_COMP *}  the object that we're editing.
**
** Side Effects:
**
** History:
** 	8/89 (billc) - first written.
*/
static STATUS
_glEdit ( self, obj )
AB_CATALOG *self;
APPL_COMP *obj;
  {
	IIABedtComp( obj->appl, obj, self);
	_gcatUpdate(self, obj);
	return OK;
  }
/*
** Name:	_rEdit() -	Edit a Class Definition.
**
** Description:
**	Object-edit actions specific to OC_RECORD objects.
**
** Input:
**	self	{AB_CATALOG *}  the object description.
**	obj	{APPL_COMP *}  the object that we're editing.
**
** Side Effects:
**
** History:
** 	8/89 (billc) - first written.
*/
static STATUS
_rEdit ( self, obj )
AB_CATALOG *self;
APPL_COMP *obj;
  {
	COMP_CLASS_INFO *cp;
	/* edit class members */
	IIABcaCatalogEdit( obj->appl, OC_RECMEM, self->c_noload, FALSE );
	cp = iiabApCompClass(obj->class, FALSE /* no form init */);
	_VOID_ (*cp->save)( obj->appl, obj );
	_rcatUpdate(self, (RECDEF *) obj);
	return OK;
  }
/*
** Name:	_raEdit() -	Edit a Class Attribute.
**
** Description:
**	Object-edit actions specific to OC_RECMEM objects.
**
** Input:
**	self	{AB_CATALOG *}  the object description.
**	obj	{APPL_COMP *}  the object that we're editing.
**
** Side Effects:
**
** History:
** 	8/89 (billc) - first written.
*/
static STATUS
_raEdit ( self, obj )
AB_CATALOG *self;
APPL_COMP *obj;
  {
	IIABedtComp( obj->appl, obj, self);
	_gcatUpdate(self, obj);
	rec_changed = TRUE;	/* Assume it changed */
	return OK;
  }
/*
** Name:	objDisplay() -   Global variable Create Frame Display.
**
** Description:
**	Displays application component specific information on the create form.
**
** Inputs:
**	id	{OOID}  The ID of the object being created.
**	new     {APPL_COMP *}  The temporary application component.
**
** Returns:
**	{STATUS}  OK
**
** History:
**	06/89 (billc)  Written.
*/
static STATUS
  objDisplay( id, new, formname )
OOID    id;
APPL_COMP	*new;
  char *formname;
  {
/* # line 1391 "editglo.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIstfsio(17,_name,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(53,_name,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(54,_create_date,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1
        ));
        IIstfsio(54,_owner,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(54,_altr_date,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(54,_last_altered_by,0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 1401 "editglo.qsc" */	/* host code */
	switch (new->class)
	{
	  case OC_CONST:
		{
			/* sneaky trick, again.  Fixes US370. */
/* # line 1406 "editglo.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,formname,(char *)0, (char *)0) != 0) {
        IIstfsio(53,_data_type,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 1407 "editglo.qsc" */	/* clear */
    {
      IIfldclear(_value);
      IIfldclear(_data_type);
    }
/* # line 1408 "editglo.qsc" */	/* host code */
			break;
		}
	  case OC_RECMEM:
	  case OC_GLOBAL:
		/* globals and class attributes can be nullable or
		** can be arrays.  These are mutually exclusive.  
		** We'll start off with 'nullable' shown.
		*/
		iiabTD_TypeDisplay(formname, 
				(DB_DATA_DESC*)NULL, (i4) new->appl->dml
			);
		break;
	  case OC_RECORD:
	  default:
		break;
	}
	return OK;
  }
/*
** Name:	iiabCrGlobal() -       Create Application Data Object Component.
**
** Description:
**	Initializes and writes the application catalog entry for a data
**	component -- a class, class attribute, global variable, or constant.
**	This is somewhat like f_create in abcreate.qsc.
**
** Inputs:
**	id	{OOID}  The variable component object.
**	name    {char *}  The object name.
**	data	{PTR}  The temporary object used during the create.
**
** Outputs
**	obj	{APP_VAR *}  The variable component object.
**			data_type	{DB_DATA_DESC}  The data-type
**								descriptor.
**
** History:
*/
static STATUS createCOcomp();
static STATUS createGLcomp();
static STATUS createRCcomp();
static STATUS createRAcomp();
STATUS
iiabCrGlobal ( id, data )
OOID	id;
PTR	data;
{
	register APPL_COMP	*obj = (APPL_COMP *)OOp(id);
	STATUS			stat;
	switch (obj->class)
	{
	  case OC_GLOBAL:
		stat = createGLcomp((GLOBVAR *) obj, data);
		break;
	  case OC_CONST:
		stat = createCOcomp((CONSTANT *) obj, data);
		break;
	  case OC_RECORD:
		stat = createRCcomp((RECDEF *) obj, data);
		break;
	  case OC_RECMEM:
		stat = createRAcomp((RECMEM *) obj, data);
		break;
	  default:
		stat = FAIL;
		break;
	}
	if ( stat == OK )
	{
		/* Insert into ordered list of components */
		IIAMinsComp(obj->appl, obj);
	}
	return stat;
}
/*
** Name:	createCOcomp() -	Create Application Variable Component.
**
** Description:
**	Initializes and writes the application catalog entry for a global var.
**
** Inputs:
**	obj	{CONSTANT*}  The variable component object.
**	data    {PTR ==> CONSTANT *}  The temporary object used during the
**					create.
**
** Outputs
**	obj	{APP_VAR *}  The variable component object.
**			data_type	{DB_DATA_DESC}  The data-type
**								descriptor.
**
** History:
**	06/89 (jfried)  Cloned from Bill C's createGLcomp.
*/
static STATUS
createCOcomp ( obj, data )
CONSTANT	*obj;
PTR		data;
{
    /* get the datatype info off the form (editutil.qsc) */
    if ( iiabVG_ValueGet( obj, Co_cr_form ) != OK )
	return FAIL;
    return IIAMwrConst(obj);
}
/*
** Name:	createGLcomp() -	Create Application Variable
**						Component.
** Description:
**	Initializes and writes the application catalog entry for a global var.
**
** Inputs:
**	obj	{GLOBVAR*}  The variable component object.
**	data	{PTR ==> GLOBVAR *}  The temporary object used during the
**					create.
**
** Outputs
**	obj	{APP_VAR *}  The variable component object.
**			data_type	{DB_DATA_DESC}  The data-type
**								descriptor.
**
** History:
**	06/89 (billc)  Written.
*/
static STATUS
createGLcomp ( obj, data )
GLOBVAR		*obj;
PTR		data;
{
	/* initialize before iiabTG_TypeGet fills it in. */
	obj->data_type.db_datatype = DB_NODT;
	obj->data_type.db_length = 0;
	obj->data_type.db_scale = 0;
	obj->data_type.db_data = ERx("none");
	/* get the datatype info off the form */
  	if ( !iiabTG_TypeGet( (APPL_COMP*) obj, Gl_cr_form, &obj->data_type ) )
	{
		return FAIL;
	}
	return IIAMwrGlobal(obj);
}
/*
** Name:	createRCcomp() -	Create Class Definition
**						Component.
** Description:
**	Initializes and writes the application catalog entry for a Class Def.
**
** Inputs:
**	obj	{RECDEF*}  The variable component object.
**	data	{PTR ==> RECDEF *}  The temporary object used during the create.
**
** Outputs
**	obj	{APP_VAR *}  The rec.def. component object.
**
** History:
**	06/89 (billc)  Written.
*/
static STATUS
createRCcomp ( obj, data )
RECDEF	*obj;
PTR	data;
{
	return IIAMwrRecord(obj);
}
/*
** Name:	createRAcomp() -	Create Class Attribute
**						Component.
** Description:
**	Initializes and writes the application catalog entry for a Class Att.
**
** Inputs:
**	obj	{RECMEM*}  The variable component object.
**	data	{PTR ==> RECMEM *}  The temporary object used during the create.
**
** Outputs
**	obj	{APP_VAR *}  The rec.def. component object.
**
** History:
**	06/89 (billc)  Written.
*/
static STATUS
createRAcomp ( obj, data )
RECMEM	*obj;
PTR	data;
{
	RECDEF *parent = (RECDEF*) OOp(iiabRcatalog.c_ooid);
	RECMEM	dummy;
	RECMEM	*rp;
  AB_CATALOG *self = &iiabMcatalog;
	/* initialize before iiabTG_TypeGet fills it in. */
	obj->data_type.db_datatype = DB_NODT;
	obj->data_type.db_length = 0;
	obj->data_type.db_scale = 0;
	obj->data_type.db_data = ERx("none");
	/* get the datatype info off the form */
  	if ( !iiabTG_TypeGet( (APPL_COMP*) obj, Ra_cr_form, &obj->data_type ) )
		return FAIL;
	/* Insert the new object in the classes list. */
	obj->next_mem = parent->recmems;
	parent->recmems = obj;
	/* att_order no longer used. */
	obj->att_order = 0;
	return IIAMwaWriteAtt( obj );
}
char	*UGcat_now();
#define HELPEXPL	F_FE0100_ExplFrameHelp
#define FORGETEXPL	F_FE0109_ExplForget
#define BADNAME		E_UG000E_BadName
/*
** Name:	_createObj() -	Generic Create Data Object Routine.
**
** Description:
**	Look familiar?  This duplicates a lot of code in editcomp.qsc, and
**	all this stuff should probably be tucked in there.
**	The OO material is all stolen directly from oo/oocreate.qsc.
**
** Output params:
**	new	{APPL_COMP *}	- dummy, filled in with info for created object
**	catalog {AB_CATALOG*}	- info on the current display catalog.
**	title	{char *}	- title to put on the form.
**	formname {char*}	- name of the form
**
** Returns:
**	{APPL_COMP *}	-- the newly created object.
**
** Side Effects:
**	displays Save sequence sub-menu; may invoke Save frame; object
**	may be saved in database.
**
** History:
**	12/89 (billc) -- Written.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
*/
static APPL_COMP *
  _createObj( new, catalog, title, formname )
  APPL_COMP *new;
  AB_CATALOG *catalog;
  char *title;
  char *formname;
  {
    APPL_COMP *obj = NULL;
    COMP_CLASS_INFO *cp;
    UIDBDATA *uidbdata;
	uidbdata = IIUIdbdata();
	new->ooid = OC_UNDEFINED;
	new->data.tag = new->appl->data.tag ;
	new->data.inDatabase = FALSE;
	/*
	** We'll cheat -- there is a useful datatype verification routine in
	** this class's edit template, so get the template and use it.  This
	** cheat takes advantage of the fact that we use the same form for
	** editing and creating these things.  The edit routines do twiddle
	** the form, so this is important.  (For classes, the forms are
	** different, but we don't do a typecheck for classes.)
	*/
	cp = iiabApCompClass(new->class, FALSE /* no form init */);
	if ( cp == NULL )
		return NULL;
/* # line 1694 "editglo.qsc" */	/* display */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
      IIFRgpcontrol(2,0);
      if (IIdispfrm(formname,(char *)"f") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 1698 "editglo.qsc" */	/* putform */
        {
          if (IIfsetio((char *)0) != 0) {
            IIputfldio(_title,(short *)0,1,32,0,title);
            IIputfldio(_name,(short *)0,1,32,0,_iiOOempty);
            IIputfldio(_owner,(short *)0,1,32,0,uidbdata->user);
            IIputfldio(_create_date,(short *)0,1,32,0,ERx(UGcat_now()));
          } /* IIfsetio */
        }
/* # line 1705 "editglo.qsc" */	/* host code */
		/* Object specific attributes */
		if ( objDisplay(new->ooid, new, formname) != OK )
/* # line 1707 "editglo.qsc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
/* # line 1709 "editglo.qsc" */	/* resume */
        {
          IIresfld(_name);
          if (1) goto IIfdB1;
        }
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
            i4 change;
/* # line 1716 "editglo.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,_iiOOempty,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&change,20,_name,0);
              } /* IIiqset */
            }
/* # line 1717 "editglo.qsc" */	/* host code */
		if ( change != 0 )
            {
              char name[FE_MAXNAME+1];
/* # line 1721 "editglo.qsc" */	/* getform */
              {
                if (IIfsetio((char *)0) != 0) {
                  IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,name,_name);
                } /* IIfsetio */
              }
/* # line 1722 "editglo.qsc" */	/* host code */
			if ( FEchkname(name) != OK )
			{
				IIUGerr( BADNAME, UG_ERR_ERROR, 1, name);
/* # line 1725 "editglo.qsc" */	/* resume */
              {
                IIresfld(_name);
                if (1) goto IIfdB1;
              }
/* # line 1726 "editglo.qsc" */	/* host code */
			}
            }
/* # line 1728 "editglo.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 2) {
          {
            i4 change;
/* # line 1735 "editglo.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,_iiOOempty,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&change,20,cp->type_check,0);
              } /* IIiqset */
            }
/* # line 1736 "editglo.qsc" */	/* host code */
		if ( change != 0 && cp->type_valid != NULL )
            {
              char desc[FE_MAXNAME+1];
			DB_DATA_VALUE   temp;
			/* Note:  The Nullable field relies on an
			** internal validation and is not relevant
			** when validating the Data Type.
			*/
/* # line 1745 "editglo.qsc" */	/* getform */
              {
                if (IIfsetio((char *)0) != 0) {
                  IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,desc,
                  cp->type_check);
                } /* IIfsetio */
              }
/* # line 1746 "editglo.qsc" */	/* host code */
			if ( STtrmwhite(desc) <= 0
			  || (*cp->type_valid)(new, desc, &temp) != OK)
			{
				/* invalid Type */
/* # line 1750 "editglo.qsc" */	/* resume */
              {
                IIresfld(cp->type_check);
                if (1) goto IIfdB1;
              }
/* # line 1751 "editglo.qsc" */	/* host code */
			}
            }
/* # line 1753 "editglo.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 1761 "editglo.qsc" */	/* host code */
		char	*srem;
		char	*lrem;
		OOID	newid;
            char currname[FE_MAXNAME+1];
            char buf[OOLONGREMSIZE+1];
/* # line 1767 "editglo.qsc" */	/* validate */
            {
              if (IIchkfrm() == 0) {
                goto IIfdB1;
              } /* IIchkfrm */
            }
/* # line 1769 "editglo.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,currname,_name);
              } /* IIfsetio */
            }
/* # line 1771 "editglo.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,OOLONGREMSIZE+1-1,buf,_short_remark);
              } /* IIfsetio */
            }
/* # line 1772 "editglo.qsc" */	/* host code */
		srem = STtrmwhite(buf) <= 0 ? _iiOOempty : STalloc(buf);
		OOget_long_remark(formname, buf);
		lrem = STtrmwhite(buf) <= 0 ? _iiOOempty : STalloc(buf);
		if ( new->ooid == OC_UNDEFINED )
		{ /* allocate new DB object */
			/* lint truncation warning if size of ptr > OOID, 
		      	   but code valid */
			new->ooid = (OOID)OOsnd( new->class, _newDb, FALSE, currname,
					0, uidbdata->user, 0,
					srem, (char *)NULL, (char *)NULL, lrem
			);
			if ( new->ooid == nil )
			{
				obj = NULL;
/* # line 1789 "editglo.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 1790 "editglo.qsc" */	/* host code */
			}
		}
		obj = (APPL_COMP *)OOp(new->ooid);
		/* for memory allocation */
		obj->data.tag = new->data.tag;
		obj->appl = new->appl;
		obj->altered_by = uidbdata->user;
		if (new->class == OC_RECMEM)
		{
			((RECMEM*)obj)->parent = ((RECMEM*)new)->parent;
		}
		else if (new->class == OC_CONST)
		{
			((CONSTANT*)obj)->language = ((CONSTANT*)new)->language;
		}
    		/* lint truncation warning if size of ptr > OOID, 
		   but code valid */
		if ((newid = (OOID)OOsnd( new->ooid, _confirmName, currname, FALSE ))
		    == nil)
/* # line 1812 "editglo.qsc" */	/* resume */
            {
              IIresfld(_name);
              if (1) goto IIfdB1;
            }
/* # line 1814 "editglo.qsc" */	/* host code */
		new->ooid = newid;
		if ( IIOOsave( &(new->ooid), (PTR)obj, iiabCrGlobal ) == OK )
		{
			/* update row on main catalog form */
			_send(catInsertRow, catalog)( catalog, obj,
				FEtbsins( catalog->c_form, catalog->c_tfield,
					currname, _name, FALSE
				)
			);
/* # line 1824 "editglo.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 1825 "editglo.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 4) {
          {
/* # line 1831 "editglo.qsc" */	/* host code */
		obj = NULL;
/* # line 1832 "editglo.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 5) {
          {
            char fld[FE_MAXNAME+1];
/* # line 1840 "editglo.qsc" */	/* host code */
		FLD_VALUE	fv;
		fv._form = formname;
		fv._field = fld;
		fv._column = NULL;
/* # line 1846 "editglo.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,fld,22,(char *)0,0);
              } /* IIiqset */
            }
/* # line 1848 "editglo.qsc" */	/* host code */
		if ( cp->type_check != NULL && STequal(fld, cp->type_check) )
		{
			IIUFhlpType( cp->types, cp->ntypes,
					(bool)(new->class != OC_CONST), &fv
			);
		}
		else
		{
			IIUFfldHelp(&fv);
		}
          }
        } else if (IIretval() == 6) {
          {
/* # line 1864 "editglo.qsc" */	/* host code */
		FEhelp( ERx("abcmpcrt.hlp"), title );
          }
        } else {
          if (1) goto IIfdE1;
        } /* IIretval */
      } /* IIrunform */
IIfdF1:
      if (IIchkfrm() == 0) goto IIfdB1;
      goto IIfdE1;
IImuI1:
      if (IIFRafActFld(_name,0,1) == 0) goto IIfdE1;
      if (IIFRafActFld(cp->type_check,0,2) == 0) goto IIfdE1;
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_OK),catalog->c_create.m_expl,1,1,3) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,1,1,3) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Forget),ERget(FORGETEXPL),0,0,4) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,0,0,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,5) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(HELPEXPL),0,0,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,ERget(HELPEXPL),0,0,6) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 1867 "editglo.qsc" */	/* host code */
	catalog->c_ooid = new->ooid;
	return obj;
  }
/*
** _nullmsg -- handy utility for getting a string, "yes" or "no", depending
**		on nullability of the given datatype.  strings are cached.
*/
static	char *_No;
static	char *_Yes;
static char *
_nullmsg(dbdv)
DB_DATA_DESC *dbdv;
{
	if (_Yes == NULL)
	{
		_Yes = ERget(F_UG0002_Yes2);
		_No = ERget(F_UG0007_No2);
	}
	return ( dbdv->db_datatype == DB_NODT
			|| dbdv->db_datatype == DB_DMY_TYPE
			|| !AFE_NULLABLE(dbdv->db_datatype) ) ? _No : _Yes;
}
/*
** _TypeName -- handy utility that, given a dbdv, returns a string
**		description of the datatype.  useful, since we want to print
**		'array of xxx'' if it is an array.
*/
static char typeBuffer[TYPE_LEN+1];
static char 
*_TypeName(dbdd)
DB_DATA_DESC *dbdd;
{
	if (dbdd->db_datatype != DB_DMY_TYPE || dbdd->db_length == 0 )
	{
		/* If it's not an array, just return the data pointer */
		return dbdd->db_data;
	}
	else
	{
		/* Construct "array of xxx" */
		STprintf(typeBuffer, ERx("%s%s"), _ArrayOf, dbdd->db_data);
		return typeBuffer;
	}
}
