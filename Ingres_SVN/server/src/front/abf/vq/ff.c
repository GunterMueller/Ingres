# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<nm.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<er.h>
# include	<abclass.h>
# include	<adf.h>
# include	<ft.h>
# include	<fmt.h>
# include	<frame.h>
# include	<oocat.h>
# include	<ooclass.h>
# include	<oodefine.h>
# include	<st.h>
# include	<cm.h>
# include	<ci.h>
# include	<tm.h>
# include	<cv.h>
# include	<me.h>
# include	<afe.h>
# include	<flddesc.h>
# include	<stdprmpt.h>
# include	<ug.h>
# include	<erug.h>
# include <ui.h>
/*
#include        <ui.h>
	..so ming knows about the dependency
*/
# include	<uigdata.h>
# include	<metafrm.h>
# include	"ervq.h"
# include	"vqloc.h"
# include	"vqhotspo.h"
# include	"vqescinf.h"
/**
** Name:	ff.c -	make frame flow diagram frame
**
** Description:
**	This file has routines to dynamically create the frame flow
*	diagram frame.
**
**	This file defines:
**		IIVQffeFrameFlowEntry - initiate edit of frame flow diag.
**
** History:
**	03/26/89 (tom) - created
**	12/19/89 (dkh) - VMS shared lib changes.  No more direct access
**			 to "frterminfo" - must now use FTterminfo().
**	03/22/91 (emerson)
**		Fix interoperability bug 36589:
**		Change all calls to abexeprog to remove the 3rd parameter
**		so that images generated in 6.3/02 will continue to work.
**		(Generated C code may contain calls to abexeprog).
**		This parameter was introduced in 6.3/03/00, but all calls
**		to abexeprog specified it as TRUE.  See abfrt/abrtexe.c
**		for further details.
**	03/28/91 (dkh) - Replaced old inquire_frs syntax with new.
**	09-apr-91 (seng)
**		Because _nullstring is declared as a char array of size 1 
**		instead of a char, the initializer needs be surrounded by
**		curly braces.  This is a generic change.
**	24-apr-91 (blaise)
**		Don't test the return value of IIABdcvDestroyCompVision(),
**		since it doesn't return anything.
**	16-jan-92 (pearl)
**		Bug 41647.  For abexeprog call to RBF, change 'report'
**		parameter to 'edit_restrict'.
**	21-jan-92 (pearl)
**		Bug 41791.  For abexeprog call to RBF, change 'table'
**		parameter to 'rpt_object'.
**	11-aug-92 (blaise)
**		Added entry to the ff_edinfo array for the new local procedures
**		edit option.
**	9-sept-92 (blaise)
**		Replaced IIUGlbo_lowerBEobject() with IIUGdbo_dlmBEobject() to
**		handle delimited ids, and added new argument to FErelexists().
**	14-dec-92 (blaise)
**		Added support for import menuitem (for copying frames across
**		applications): Added new Import menuitem when creating a
**		frame; and renamed static function ff_CF2 to IIVQcfCopyFrame
**		and made it public.
**	 2-mar-93 (rudyw)
**		Bug 49869. Add check so ff_table is not overrun during frame
**		tree search which continues beyond the maximum levels allowed.
**		A loophole in visions 'insert' allows a frame with multiple
**		sublevels to be added without check on total level violation.
**	27-may-93 (blaise)
**		Add the ability to duplicate the form with the VQ.
**      31-aug-93 (kchin)
**              Changed type of fr in IIVQffeFrameFlowEntry() and frame in
**              ff_SelFr() to PTR, since they hold pointers.  Also changed
**              the cast of frm to PTR when assigns it to fr.  A cast to
**              i4  will result in truncation of 64-bit address.
**	15-sep-93 (connie) Bug #54047
**		Use FE_fullresolve in place of IIUGdbo_dlmBEobject and
**		FErelexists so that owner qualification of table names and
**		delimiter identifiers are handled properly
**	15-sep-93 (connie) Bug #54207
**		Destroyed the frame if ff_MakeVq returns FALSE which means
**		failure in adding table to the frame
**	15-nov-93 (connie) Bug #40333
**		Call IIAMmtdMenuTextDel to remove the ii_menuargs entries
**		corresponding to a removed/destroyed frame
**	08-dec-93 (connie) Bug #50142
**		If there is change in short_remark, update the parent's flag
**		for regeneration.
**	03-feb-94 (connie) Bug #50142
**		For the previous fix, need to check for top frame which
**		will have no parent for updating
**	22-feb-1994 (daver)
**		Normalized the table name in ff_CheckRel() to fix DB2
**		gateway bugs 58834 and 59546. Bugs caused compiler errors
**		when users typed in lower-cased table names.
**      29-sep-96 (mcgem01)
**              Global data moved to vqdata.c
**	25-feb-1998 (kitch01)
**		Bug 88725. During duplication of a frame in vision local procedures are
**		incorrectly copied to local variables. Amend IIVQcfCopyFrame to copy the 
**		old vartype to the new MFVAR structure.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	21-jan-03 (hayke02)
**	    Remove local prototyping of IIUFccConfirmChoice() - this is now
**	    done in fe.h.
**      11-Jun-2004 (hanch04)
**          Removed reference to CI for the open source release.
**/
/* # define's */
/* frame insertion direction */
# define INS_RIGHT	0
# define INS_LEFT	1
# define INS_DOWN	2
# define INS_FALSE	3 /* used for when we are really not inserting,
				but need to use some of the logic that
				operates during insertions */
# define FF_MAXDEPTH  25	/* the maximum depth of an application being
				created with the frame flow diagram */
GLOBALREF APPL *IIVQappl; /* argument of frame flow entry, the current 
			   application we are editing in abf */
GLOBALREF PTR IIVQcontext;	/* argument of frame flow entry, the context
			   ptr used to call back into abf */
/* maximum number of rows and columns of frames 
   allowed in the frame flow diagram */ 
GLOBALREF i4  IIVQrowmax; 
GLOBALREF i4  IIVQcolmax;
/* memory of the hotspots and information of what to do if the
   user clicks on one of them */
GLOBALREF HOTSPOT IIVQhsaHotSpotArray[FF_MAXHOTSPOTS];
GLOBALREF i4  IIVQhsiHotSpotIdx;
/* extern's */
	/* abf functions */
FUNC_EXTERN STATUS IIABcotChangeObjectType();
FUNC_EXTERN STATUS IIABmdoMakeDefaultObject();
FUNC_EXTERN STATUS IIABucvUpdCompVision();
FUNC_EXTERN STATUS IIABecvEditCompVision();
FUNC_EXTERN STATUS IIABfrcCompile();
FUNC_EXTERN STATUS IIABgcvGoCompVision();
FUNC_EXTERN VOID   IIABdcvDestroyCompVision();
FUNC_EXTERN FRAME *IIVQmffMakeFfFrame();
FUNC_EXTERN VOID IIAMufqUserFrameQuick();
FUNC_EXTERN STATUS IIAMmpMetaPopulate();
FUNC_EXTERN STATUS IIAMmfMetaFree();
FUNC_EXTERN bool IIABffsFrameFlowState();
/*VARARGS1*/
FUNC_EXTERN VOID abexeprog(); 
	/* misc. vq functions */
FUNC_EXTERN STATUS IIAMmnaMetaNewArray();
FUNC_EXTERN i4   IIVQotObjectType();
FUNC_EXTERN VOID IIVQmfnMakeFrameName(); 
FUNC_EXTERN VOID IIVQcsCenterString();
FUNC_EXTERN VOID IIVQmdjMakeDefaultJoin();
FUNC_EXTERN i4   IIVQvdlVqDisplayLoop();
FUNC_EXTERN VOID IIVQegfEditGenForm();
FUNC_EXTERN VOID IIVQegcEditGenCode();
FUNC_EXTERN VOID IIVQgeGlobalEdit();
FUNC_EXTERN VOID IIVQparams();
FUNC_EXTERN STATUS IIVQefeEditFrameEscapes();
FUNC_EXTERN STATUS IIVQpePatternEncode();
FUNC_EXTERN bool IIVQpmPatternMatch();
FUNC_EXTERN VOID IIVQfvtFreeVqTag();
FUNC_EXTERN PTR  IIVQ_VAlloc();
FUNC_EXTERN bool IIVQantAddNewTable();
FUNC_EXTERN bool IIVQdceDisplayCompErrs();
FUNC_EXTERN i4   IIVQfcsFrameCompSort();
FUNC_EXTERN i4   IIVQnmNumMenus(); 
FUNC_EXTERN STATUS IIVQrmeRenameMenuEscapes();
FUNC_EXTERN i4	 IIVQimport();
FUNC_EXTERN i4   IIVQcfCopyFrame();
	/* misc utility functions */
FUNC_EXTERN STATUS FErelexists();
FUNC_EXTERN VOID IIdelfrm();
FUNC_EXTERN i4	IIFRmdfMakeDefaultForm();
FUNC_EXTERN VOID IIUGqsort();
FUNC_EXTERN PTR IIVQ_MAlloc();
FUNC_EXTERN i4  IIFDlpListPick();
FUNC_EXTERN char *IITUtpTblPik();   	/* Tables Utility ListPick */
FUNC_EXTERN PTR  IIFRkeystroke();	/* keystroke intercept function */
FUNC_EXTERN VOID FTterminfo();
FUNC_EXTERN FRAME *FDfrcreate();
GLOBALREF bool IIabAbf; 	/* flag to say that we have ABF capability */
GLOBALREF char *IIabExename;
GLOBALREF ESCTYPE vqescs[];
/* static's */
static OO_OBJECT *_looperr; /* ptr to the frame object which was found
				twice in the tree descent */
  static char _nullstring[1]= {'\0'};
  static char _col1[]= ERx("c1");
  static char _col2[]= ERx("c2");
  static char _col3[]= ERx("c3");
  static i4 _our_to;
static i4	_lastupd;		/* last update, for refresh check */
static DB_DATA_VALUE _dbv1 = 		/* the name of the frame */
{
	NULL,
	FE_MAXNAME,
	DB_CHR_TYPE,
	0
};
static DB_DATA_VALUE _dbv2 =		/* the type of frame */ 
{
	NULL,
	14,
	DB_CHR_TYPE,
	0
};
static DB_DATA_VALUE _dbv3 =		/* the short remark */ 
{
	NULL,
	OOSHORTREMSIZE,
	DB_CHR_TYPE,
	0
};
/* field descriptors for paste frame popup form generation */
static FLD_DESC _fd[3] = 
{
	{
		_col1,		/* column name */
		NULL,		/* title (filled in at runtime) */
		&_dbv1,		/* data type */
		16		/* maximum display width */
	},
	{
		_col2,		/* column name */
		NULL,		/* title (filled in at runtime) */
		&_dbv2,		/* data type */
		14		/* maximum display width */
	},
	{
		_col3,		/* column name */
		NULL,		/* title (filled in at runtime) */
		&_dbv3,		/* data type */
		30		/* maximum display width */
	}
};
/* paste form name */
static READONLY
  char _selfr_form[]= ERx("selfr");
/* name of the form used in the display of the frame flow diagram */
static READONLY
  char ffform[]= ERx("ff");
/* screen location for several popups */
static i4	screen_loc = LPGR_FLOAT;
# define LOC static
LOC bool ff_NeedRef();
LOC bool ff_Elock();
LOC VOID ff_RemELocks();
LOC VOID ff_Find(); 
LOC VOID ff_NxtFr(); 
LOC i4   ff_KeyFunc(); 
LOC VOID ff_LocFrame(); 
LOC bool ff_LocFr(); 
LOC VOID ff_GoUp(); 
LOC VOID ff_GoDown(); 
LOC VOID ff_CheckBelow(); 
LOC VOID ff_ZapBelow(); 
LOC VOID ff_GoLeft(); 
LOC VOID ff_GoRight(); 
LOC i4   ff_Insert(); 
LOC i4   ff_IF2(); 
LOC i4   ff_NewType(); 
LOC i4   ff_NT2(); 
LOC i4   ff_NewVqFrame(); 
LOC bool ff_CheckMenuItem(); 
LOC bool ff_MakeNew();
LOC STATUS ff_UpdDepends(); 
LOC VOID ff_NewMenuFrame(); 
LOC VOID ff_NewAbfFrame(); 
LOC VOID ff_NewHLProc(); 
LOC STATUS ff_HLhook(); 
LOC bool ff_NewFrame(); 
LOC bool ff_NewMItem(); 
LOC bool ff_Add2Menu(); 
LOC VOID ff_DelMenu(); 
LOC STATUS ff_Delmenuargs();
LOC i4   ff_Paste(); 
LOC i4   ff_Copy(); 
LOC i4   ff_SelFr(); 
LOC i4   ff_PF2(); 
LOC bool ff_LoopCheck(); 
LOC VOID ff_Destroy(); 
LOC bool ff_RefFound();
LOC VOID ff_Cut(); 
LOC VOID ff_DelFixup(); 
LOC i4   ff_Edit(); 
LOC i4   ff_ED2(); 
LOC i4   ff_EditVq(); 
LOC VOID ff_EditMenu(); 
LOC i4   ff_MakeVq(); 
LOC STATUS ff_CheckRel(); 
LOC VOID ff_SetAo(); 
LOC bool ff_AtTop(); 
LOC VOID ff_ProcessHotSpot(); 
LOC bool ff_TouchParentCheck(); 
LOC bool ff_TouchCheck(); 
LOC bool ff_Touch(); 
/**************************************** could be a state info structure */
		/* table of the state information for each displayed row of the
		   displayed frame flow diagram */
static FF_STATE ff_table[FF_MAXDEPTH + 1];
		/* ptr to the table above.. this pointer points to the topmost
		   displayed frame of the diagram */
static FF_STATE *ff;
		/* the current Frame Flow ROW relative 
		   to the top row on the display */
static i4  ffrow;
		/* ptr to the current Frame Flow Application Object structure */
static OO_OBJECT *ffao;
		/* language information for HL procedures */
static i4  ffol_lang;
static i4  ffdml;
/**************************************** could be a state info structure */
static i4  _command;		/* last command captured by keystroke 
				   function */
static i4  _hotspot;		/* hotspot # captured by keystroke function */
static char _last_find[FE_PROMPTSIZE + 1] = {0}; /* last frame name to find */
static i4  _direction;		/* direction of insertion */
static i4  _type;		/* type of frame to be created */
/* form name/flds for getting the menuitem & remark of a new frame */
  static char *newfr_form = ERx("newfr");
  static char *newmi_form = ERx("newmi");
  static char *newvq_form = ERx("newvq");
  static char *_master = ERx("master");
  static char *_master_in_tf = ERx("master_in_tf");
  static char *_detail = ERx("detail");
  static char *_menuitem = ERx("menuitem");
  static char *_remark = ERx("remark");
static char *_copymnu;	/* copy command memory for menuitem name and remark */
static char *_copyrem;
/*
** we set this whenever we lock a frame for edit, and check in the main
** display loop, rather than trying to bracket all the individual bits of
** of logic.  We implement this as a small stack just to be able to lock
** limited numbers of simultaneous frames.
*/
static OOID _elock[3];
static i4  _num_elock;
/* frame type information table... 
   this structure supplies information about the various frame types.
   The order of items in this table is unimportant.
   The name/description strings are read from the message file
   upon entry to the frame flow diagram (vqloc.h) */
static FRMINFO frm_info[NUMFRMTYPES] = 
{
/*nam/des frame type   table type   flags controlling operations allowed */
{ 0,0,  OC_MUFRAME,  0,           HAS_SRC | HAS_FORM | HAS_MENU 	  },
{ 0,0,  OC_APPFRAME, TAB_APPEND,  HAS_SRC | HAS_FORM | HAS_MENU | HAS_VQ  },
{ 0,0,  OC_UPDFRAME, TAB_UPDATE,  HAS_SRC | HAS_FORM | HAS_MENU | HAS_VQ  },
{ 0,0,  OC_BRWFRAME, TAB_DISPLAY, HAS_SRC | HAS_FORM | HAS_MENU | HAS_VQ  },
{ 0,0,  OC_OSLFRAME, 0,           HAS_SRC | HAS_FORM | ABF_TYPE           },
{ 0,0,  OC_QBFFRAME, 0,           HAS_QBF | HAS_FORM  			  },
{ 0,0,  OC_RWFRAME,  0,           HAS_RBF | HAS_FORM | HAS_SRC		  },
{ 0,0,  OC_GRFRAME,  0,           HAS_GRF 				  },
{ 0,0,  OC_OSLPROC,  0,           HAS_SRC | ABF_TYPE 			  },
{ 0,0,  OC_HLPROC,   0,           HAS_SRC | ABF_TYPE 			  },
{ 0,0,  OC_DBPROC,   0,           ABF_TYPE 			  	  }
};
/* edit option info table...
   this array supplies information about the various 
   edit options used to construct a popup menu which 
   displays the options.  (vqloc.h) */
static EDINFO ff_edinfo[] = 
{	 /* message id           type        flags */
	{ F_VQ00AA_eop_visqry,	EOP_VISQRY, HAS_VQ   },
	{ F_VQ00B4_eop_qbfdef,	EOP_QBFDEF, HAS_QBF  },
	{ F_VQ00B5_eop_rbfdef,	EOP_RBFDEF, HAS_RBF  },
	{ F_VQ00B6_eop_grfdef,	EOP_GRFDEF, HAS_GRF  },
	{ F_VQ00AB_eop_genfrm,	EOP_GENFRM, HAS_FORM },
	{ F_VQ00AC_eop_mnutxt,	EOP_MNUTXT, ~0       },
	{ F_VQ00AD_eop_frparm,	EOP_FRPARM, ~0       },
	{ F_VQ00AE_eop_locvar,	EOP_LOCVAR, HAS_MENU },
	{ F_VQ0151_eop_locprc,	EOP_LOCPRC, HAS_MENU },
	{ F_VQ00AF_eop_globob,	EOP_GLOBOB, ~0       },
	{ F_VQ00B0_eop_esccod,	EOP_ESCCOD, HAS_MENU },
	{ F_VQ00B1_eop_srcfil,	EOP_SRCFIL, HAS_SRC  },
	{ F_VQ00B2_eop_errors,	EOP_ERRORS, ~0       },
	{ 0 }	/* must be null terminated */ 
};
/* frame order table...
   this array gives the order that the items are to 
   appear in the popup which displays when the user
   wants to create a frame of a different type. */
static i4  frm_order[] = 
{
	MF_MENU,
	MF_APPEND,
	MF_UPDATE,
	MF_BROWSE,
	MF_QBF,
	MF_REPORT,
	MF_GRAPH,
	MF_USER,
	MF_OSLPROC,
	MF_HLPROC,
	MF_DBPROC,
	-1  /* must have neg. termination */
};
/*---- list pick prompts and help titles ----*/
# define _pstdir_prmpt		(ERget(F_VQ00A0_pstdir_prmpt))
# define _pstlr_opt 		(ERget(F_VQ00A1_pstlr_opt))
# define _pstdn_opt 		(ERget(F_VQ00A2_pstdn_opt))
# define _insdir_prmpt		(ERget(F_VQ0095_insdir_prmpt))
# define _inslr_opt 		(ERget(F_VQ0096_inslr_opt))
# define _insdn_opt 		(ERget(F_VQ0097_insdn_opt))
# define _newvq_prmpt 		(ERget(F_VQ0098_newvq_prmpt))
# define _insnew_prmpt 		(ERget(F_VQ0099_insnew_prmpt))
# define _paste_prmpt 		(ERget(F_VQ009A_paste_prmpt))
# define _frmmod_prmpt 		(ERget(F_VQ009B_frmmod_prmpt))
# define _frmvis_opt 		(ERget(F_VQ009C_frmvis_opt))
# define _frmtyp_opt 		(ERget(F_VQ009D_frmtyp_opt))
# define _newtyp_prmpt 		(ERget(F_VQ009E_newtyp_prmpt))
# define _newtyp_tit 		(ERget(F_VQ009F_newtyp_tit))
# define _ffed_tit		(ERget(F_VQ00B7_ffed_tit))
# define _mnuedt_tit		(ERget(F_VQ00B8_mnuedt_tit))
/*{
** Name:	IIVQffeFrameFlowEntry	- frame flow entry point 
**
** Description:
**	This is the entry point from ABF code into the frame flow
**	diagram.
**
** Inputs:
**	APPL *app;		- application struct pointer
**	USER_FRAME *top;	- ptr to user frame at the top of the app
**	OO_OBJECT **cur;	- ptr to ptr to the current frame
**	i4 *exit_flag;		- ptr to caller's exit flag
**	PTR context;		- context pointer for calling abf functions 
**
** Outputs:
**	Returns:
**		STATUS	- OK for success, FAIL for fatal error
**
**	Exceptions:
** Side Effects:
**
** History:
**	06/01/89  (tom)  - new interface
*/
STATUS
IIVQffeFrameFlowEntry(app, top, cur, exit_flag, context)
APPL *app;
USER_FRAME *top;
OO_OBJECT **cur;
i4  *exit_flag;
PTR context;
{
	FRAME	*frm;
  i4 old_to;
  PTR fr;
  char *cat_menuitem;
	bool again;
	PTR old_keyfunc;
	FT_TERMINFO	terminfo;
	char	*ptr;
	/* only allow calling the catalog if the user bought ABF */ 
	cat_menuitem = IIabAbf 
		? ERget(FE_Catalog)
		: _nullstring;
	/* if first time called, intialize the number of rows and columns
	   of frames that can be displayed in the frame flow diagram,
	   read message fild, and add form 
	   (could be in an initialization function) */ 
	if (IIVQrowmax == 0)
	{
		/* get terminal parms */
		FTterminfo(&terminfo);
		/* disallow edit if window is less than 80 cols */
		if (terminfo.cols < 80 || terminfo.lines < 23)
		{
			IIVQer0(E_VQ0073_TinyScreen);
			*exit_flag = 1;
			return (OK);
		}
		_our_to = 300;
		NMgtAt("II_AFD_TIMEOUT", &ptr);
		if (ptr != NULL && *ptr != EOS)
		{
			_VOID_ CVan(ptr,&_our_to);
			if (_our_to != 0 && _our_to < 20)
				_our_to = 20;
		}
		IIVQrowmax =  terminfo.lines / 8;	
		IIVQcolmax = terminfo.cols / 20;	
		IIARiaForm(newmi_form);
		IIARiaForm(newfr_form);
		IIARiaForm(newvq_form);
		frm_info[ MF_MENU    ].name = ERget(F_VQ0001_menu); 
		frm_info[ MF_APPEND  ].name = ERget(F_VQ0002_append); 
		frm_info[ MF_UPDATE  ].name = ERget(F_VQ0003_update); 
		frm_info[ MF_BROWSE  ].name = ERget(F_VQ0004_browse); 
		frm_info[ MF_USER    ].name = ERget(F_VQ0005_user); 
		frm_info[ MF_QBF     ].name = ERget(F_VQ0006_qbf); 
		frm_info[ MF_REPORT  ].name = ERget(F_VQ0007_report); 
		frm_info[ MF_GRAPH   ].name = ERget(F_VQ0008_graph); 
		frm_info[ MF_OSLPROC ].name = ERget(F_VQ0009_oslproc); 
		frm_info[ MF_HLPROC  ].name = ERget(F_VQ000A_hlproc); 
		frm_info[ MF_DBPROC  ].name = ERget(F_VQ000B_dbproc); 
		frm_info[ MF_MENU    ].desc = ERget(F_VQ000C_menu_desc); 
		frm_info[ MF_APPEND  ].desc = ERget(F_VQ000D_append_desc); 
		frm_info[ MF_UPDATE  ].desc = ERget(F_VQ000E_update_desc); 
		frm_info[ MF_BROWSE  ].desc = ERget(F_VQ000F_browse_desc); 
		frm_info[ MF_USER    ].desc = ERget(F_VQ0010_user_desc); 
		frm_info[ MF_QBF     ].desc = ERget(F_VQ0011_qbf_desc); 
		frm_info[ MF_REPORT  ].desc = ERget(F_VQ0012_report_desc); 
		frm_info[ MF_GRAPH   ].desc = ERget(F_VQ0013_graph_desc);
		frm_info[ MF_OSLPROC ].desc = ERget(F_VQ0014_oslproc_desc); 
		frm_info[ MF_HLPROC  ].desc = ERget(F_VQ0015_hlproc_desc); 
		frm_info[ MF_DBPROC  ].desc = ERget(F_VQ0016_dbproc_desc); 
	}
	/* post frame flow globals */
	IIVQcontext = context;
	IIVQappl = app;
	_lastupd = TMsecs();
	/*
	** Timeouts -
	**
	** The FRS forces us to set timeout = 0 and restore around any
	** prompts or other form displays to avoid activating and dropping
	** them.  Since most of our menuitems end in "breakdisplay", the
	** way we handle this is to set timeout at the top of the for loop,
	** and set timeout = 0 at the start of each menuitem.  We then only
	** have to set it back in the menuitem if we don't break the display.
	** We might make "set_frs" calls more often than strictly neccesary,
	** but it keeps us from having to remember which of our actions
	** really need the them.
	*/
/* # line 627 "ff.qsc" */	/* inquire_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIiqfsio((short *)0,1,30,4,&old_to,50,(char *)0,0);
    } /* IIiqset */
  }
/* # line 629 "ff.qsc" */	/* host code */
	/* locate and position on the frame indicated by the caller */
	ff_LocFrame(top, *cur);
	_num_elock = 0;
	for (again = TRUE; again == TRUE; )
	{
/* # line 636 "ff.qsc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&_our_to);
    } /* IIiqset */
  }
/* # line 637 "ff.qsc" */	/* host code */
		frm = IIVQmffMakeFfFrame(ffform, ff_table, ff, ffao, frm_info);
		fr = (PTR)frm;
		if (frm == NULL)
		{
			*exit_flag = 1;
			return (OK);
		}
/* # line 646 "ff.qsc" */	/* addform */
  {
    IIaddform(fr);
  }
/* # line 648 "ff.qsc" */	/* host code */
		/*
		** ff_NeedRef() -
		**
		** called with FALSE to poke the DB only if enough time
		** has elapsed.  Called with TRUE to poke the DB
		** unconditionally.  The former is used in the initialize
		** block, so that we check every time we redo the form.
		** The latter is used on the FRS timeout, and BEFORE
		** such operations as "create", "insert".  We do it there
		** so you get warnings about needing refresh before going
		** through the interaction rather than seeing it right
		** after completing the operation on a possible out of
		** date tree.  ff_NeedRef has to be called while inside
		** the display loop so that ffform does not disappear
		** from the screen when a popup is displayed.  If it
		** returns TRUE, indicating that the frame-flow may have
		** been changed by another user AND update is required,
		** we have to break the loop in a way analogous to 
		** handling the catlog display.
		*/
/* # line 669 "ff.qsc" */	/* display */
  {
    if (IIdispfrm(ffform,(char *)"f") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 672 "ff.qsc" */	/* host code */
			/* register our keystroke intercept function */
			old_keyfunc = IIFRkeystroke(ff_KeyFunc);
			if (ff_NeedRef(FALSE))
			{
				goto EXIT_POINT;
			}
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 687 "ff.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 688 "ff.qsc" */	/* host code */
			switch (_command)
			{
			case fdopUP:
				ff_GoUp();
				break;
			case fdopDOWN:
				ff_GoDown();	
				break;
			case fdopLEFT:
			case fdopPREV:
				ff_GoLeft();	
				break;
			case fdopRIGHT:
			case fdopNEXT:
				ff_GoRight();	
				break;
			case fdopHSPOT:
				ff_ProcessHotSpot();
				break;
			}
/* # line 712 "ff.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 717 "ff.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 718 "ff.qsc" */	/* host code */
			if (ff_NeedRef(TRUE))
			{
				goto EXIT_POINT;
			}
/* # line 722 "ff.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&_our_to);
            } /* IIiqset */
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 728 "ff.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 729 "ff.qsc" */	/* host code */
			if (ff_NeedRef(TRUE))
			{
				goto EXIT_POINT;
			}
			if (ff_Insert(ff_NewType) & VQ_QUIT)
			{
				goto QUIT_POINT;
			}
/* # line 737 "ff.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 742 "ff.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 743 "ff.qsc" */	/* host code */
			if (ff_NeedRef(TRUE))
			{
				goto EXIT_POINT;
			}
			if (ff_TouchParentCheck(FE_Destroy))
			{
/* # line 749 "ff.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&_our_to);
            } /* IIiqset */
          }
/* # line 750 "ff.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 751 "ff.qsc" */	/* host code */
			}
			ff_Destroy();	
/* # line 753 "ff.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 758 "ff.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 759 "ff.qsc" */	/* host code */
			/* if the user said to quit then do it */
			if (ff_Edit() & VQ_QUIT)
			{
				goto QUIT_POINT;
			}
/* # line 764 "ff.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 6) {
        {
/* # line 769 "ff.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 771 "ff.qsc" */	/* host code */
			switch (ffao->class)
			{
			case OC_QBFFRAME:
			case OC_GRFRAME:
			case OC_GBFFRAME:
			case OC_DBPROC:
				IIVQer2(E_VQ00D3_No_Compile, 
					frm_info[IIVQotObjectType(ffao)].name,
					ffao->name);
				break;
			default:
				IIABfrcCompile(IIVQappl, ffao);
				break;
			}
/* # line 786 "ff.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 7) {
        {
/* # line 791 "ff.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 792 "ff.qsc" */	/* host code */
			IIABgcvGoCompVision(IIVQappl, ffao, IIVQcontext);
/* # line 793 "ff.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 8) {
        {
/* # line 798 "ff.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 799 "ff.qsc" */	/* host code */
			if (ff_NeedRef(TRUE))
			{
				goto EXIT_POINT;
			}
			if (ff_Insert(ff_Paste) & VQ_QUIT)
			{
				goto QUIT_POINT;
			}
/* # line 807 "ff.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 9) {
        {
/* # line 812 "ff.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 813 "ff.qsc" */	/* host code */
			if (ff_NeedRef(TRUE))
			{
				goto EXIT_POINT;
			}
			if (ff_TouchParentCheck(F_VQ002E_Remove))
			{
/* # line 819 "ff.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&_our_to);
            } /* IIiqset */
          }
/* # line 820 "ff.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 821 "ff.qsc" */	/* host code */
			}
			ff_Cut();	
/* # line 823 "ff.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 10) {
        {
/* # line 828 "ff.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 829 "ff.qsc" */	/* host code */
			if ( ! ff_Elock(ffao, FALSE))
			{
				char    old_remark[OOSHORTREMSIZE + 1];
				METAFRAME *parent;
				STcopy(ffao->short_remark,old_remark);
				IIABecvEditCompVision(IIVQappl,
						ffao, IIVQcontext);
				if (!ff_AtTop() &&
					!STequal(old_remark,ffao->short_remark))
				{ /* short remark has been changed */
					parent = ((USER_FRAME *)ff[ffrow - 1].ao)->mf;
					_VOID_ ff_UpdDepends(parent, FALSE);
				}
/* # line 844 "ff.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
/* # line 845 "ff.qsc" */	/* host code */
			}
        }
      } else if (IIretval() == 11) {
        {
/* # line 851 "ff.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 852 "ff.qsc" */	/* host code */
			if (ff_NeedRef(TRUE))
			{
				goto EXIT_POINT;
			}
			/* restore the old key intercept function cause
			   we are going to be running a sub-menu */
			_VOID_ IIFRkeystroke((i4(*)())old_keyfunc);
			IIAButilities(IIVQappl, IIVQappl->name, 
				ABUT_AFD, FALSE);
			/* register our keystroke intercept function */
			old_keyfunc = IIFRkeystroke(ff_KeyFunc);
/* # line 867 "ff.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 12) {
        {
/* # line 873 "ff.qsc" */	/* host code */
		   EXIT_POINT:
			*exit_flag = 0;
			again = FALSE;
			*cur = ffao;
/* # line 877 "ff.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 13) {
        {
/* # line 882 "ff.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 883 "ff.qsc" */	/* host code */
			ff_LocFrame(top, (OO_OBJECT*)top); 
/* # line 884 "ff.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 14) {
        {
/* # line 889 "ff.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 890 "ff.qsc" */	/* host code */
			ff_Find();
/* # line 891 "ff.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 15) {
        {
/* # line 896 "ff.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,sizeof(0),
              (void *)IILQint(0));
            } /* IIiqset */
          }
/* # line 897 "ff.qsc" */	/* host code */
			FEhelp(ERx("vqfrmflo.hlp"), ERget(F_VQ00F7_FF_ident));
/* # line 898 "ff.qsc" */	/* set_frs */
          {
            if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
              IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&_our_to);
            } /* IIiqset */
          }
        }
      } else if (IIretval() == 16) {
        {
/* # line 903 "ff.qsc" */	/* host code */
			*exit_flag = 1;
			again = FALSE;
/* # line 905 "ff.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 17) {
        {
/* # line 910 "ff.qsc" */	/* host code */
		    QUIT_POINT:
			*exit_flag = -1;
			again = FALSE;
/* # line 913 "ff.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
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
    IIFRInternal(0);
    if (IInfrskact(0,(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IIFRtoact(0,2) == 0) goto IIfdE1;
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Create),ERget(F_VQ0041_CreateExpl),2,2,3) == 0) 
    goto IIfdE1;
    if (IInmuact(ERget(FE_Destroy),ERget(F_VQ0042_DestroyExpl),2,2,4) == 0) 
    goto IIfdE1;
    if (IInmuact(ERget(FE_Edit),ERget(F_VQ0043_EditExpl),2,2,5) == 0) goto 
    IIfdE1;
    if (IInmuact(ERget(FE_Compile),ERget(F_VQ0046_CompileExpl),2,2,6) == 0) 
    goto IIfdE1;
    if (IInmuact(ERget(FE_Go),ERget(F_VQ0047_TestExpl),2,2,7) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,7) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Insert),ERget(F_VQ0045_InsertExpl),2,2,8) == 0) 
    goto IIfdE1;
    if (IInmuact(ERget(F_VQ002E_Remove),ERget(F_VQ0044_RemoveExpl),2,2,9) == 0
    ) goto IIfdE1;
    if (IInmuact(ERget(FE_MoreInfo),ERget(F_VQ0048_DetailsExpl),2,2,10) == 0) 
    goto IIfdE1;
    if (IInmuact(ERget(FE_Utilities),ERget(F_VQ004F_AFD_UtilExpl),2,2,11) == 0
    ) goto IIfdE1;
    if (IInmuact(cat_menuitem,ERget(F_VQ0049_CatalogExpl),2,2,12) == 0) goto 
    IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(5,ERget(F_VQ004A_AFD_TopExpl),2,2,13) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(7,ERget(F_VQ004B_AFD_FindExpl),2,2,14) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(F_VQ004C_AFD_HelpExpl),2,2,15) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,15) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_End),ERget(F_VQ004D_AFD_EndExpl),2,2,16) == 0) goto 
    IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,16) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Quit),ERget(F_VQ004E_AFD_QuitExpl),2,2,17) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(2,(char *)0,2,2,17) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 917 "ff.qsc" */	/* host code */
		/* zap any edit locks we created */
		ff_RemELocks();
		/* restore the old key intercept function */
		_VOID_ IIFRkeystroke((i4(*)())old_keyfunc);
		/* tell the form system to free the dynamically created
		   form that we have created. this removes the form 
		   from the FRS's lists, and frees the memory associated
		   with it */
		IIdelfrm(ffform);
	}
/* # line 930 "ff.qsc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(50,(char *)0,0,(short *)0,1,30,4,&old_to);
    } /* IIiqset */
  }
/* # line 932 "ff.qsc" */	/* host code */
	return OK;
}
/*{
** Name:	ff_KeyFunc	- process keystroke capture
**
** Description:
**	This function is called upon each keystroke which is 
**	decoded by the form system. It is this function's 
**	responsibility to return a value to the FRS to indicate
**	what action it is to take with the keystroke.
**	This routine is used by the frame flow diagram.
**
** Inputs:
**	i4 command;		- fdop commands
**	i4 key_type;		- basic keystroke type
**	char *char_buf;		- character buffer of the actual keystrokes
**	
**
** Outputs:
**	Returns:
**		i4	- indicator to the form system as to how it is
**			  to handle the processing of the keystroke.
**			  (see frame.h)
**				
**	Exceptions:
**
** Side Effects:
**
** History:
**	09/12/89 (tom) - created
*/
/*ARGSUSED*/
LOC i4  
ff_KeyFunc(command, key_type, char_buf)
i4  command;
i4  key_type;
char *char_buf;
{
	/* check the command to see if it is one we are interested in */
	switch (command)
	{
	case fdopHSPOT:
		_hotspot = key_type;
		/* fall thru */
	case fdopUP:
	case fdopDOWN:
	case fdopRIGHT:
	case fdopLEFT:
	case fdopNEXT:
	case fdopPREV:
		_command = command;
		return (FRS_RETURN);
	}
	/* tell the form system to do whatever it would normally do */
	return (FRS_PROCESS);
}
/*{
** Name:	ff_Find	- find a frame by searching for the frame name
**
** Description:
**	This routine first prompts for the name of a frame to search for,
**	then searches the frame tree for given frame and positions on it.
**	The search always starts at the current position.
**	The search will wrap around if necessary.
**	If nothing is entered, and there was a previous search string
**	then we use it.
**	
**
** Inputs:
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects: 
**
** History:
**	03/31/89 (tom) - created
*/
LOC VOID
ff_Find()
{
	register i4  old_depth;		/* memory of how deep the stack was
				   when we started */
	register i4  i;
	register FF_STATE *cur_ff; 	/* temps for stack compare */
	register FF_STATE *old_ff;
	char buf[FE_PROMPTSIZE + 1];
	char pbuf[100];
	AFE_DCL_TXT_MACRO(FE_PROMPTSIZE + 1) work;
	i4 t_ffrow;		/* local storage for state */
	FF_STATE *t_ff;
	OO_OBJECT *t_ffao;
	FF_STATE t_ff_table[FF_MAXDEPTH + 1];
	/* prompt for the frame name to find */
	STcopy(ERget(S_VQ0010_Find_prmpt), pbuf);
	IIUFask(pbuf, FALSE, buf, 0);
	/* we will allow a single '.' to mean "find again", to
	   be consistent with the table field find */
	if (buf[0] == '.' && buf[1] == EOS)
	{
		buf[0] = EOS;
	}
	/* if the user didn't enter anything, or they entered just '.', 
	   then use last find */
	if (buf[0] == EOS)
	{
		STcopy(_last_find, buf);
	}
	/* if there have been no finds done yet */
	if (buf[0] == EOS)
	{
		FTbell();
		return;
	}
	/* save the last find string */
	STcopy(buf, _last_find);
	/* save our state.. incase the search fails */
	t_ffrow = ffrow;
	t_ff = ff;
	t_ffao = ffao;
	MEcopy((PTR)ff_table, sizeof(ff_table), (PTR)t_ff_table);
	old_depth = (ff - ff_table) + ffrow;
	/* encode pattern characters in string */
	if (IIVQpePatternEncode(buf, (DB_TEXT_STRING*)&work) != OK)
		return;
	/* loop as long as we haven't matched */
	for (;;)
	{
		/* move to the next frame, wrap if we hit the end,
		   this routine will set the global state info */
		ff_NxtFr();
		/* did we wrap all the way around? 
		   to check this we must compare our current stack
		   to the stack that we saved when we started the search */
		/* first we test if we are pointed to the same 
		   frame as we were in the beginning.. */
		if (t_ffao == ffao)
		{
			/* if the depth is not the same then just continue */
			if (old_depth != (ff - ff_table) + ffrow)
			{
				goto CONTINUING;
			}
			/* full compare of the stack */
			cur_ff = ff_table;
			old_ff = t_ff_table;
			for (i = 0; i < old_depth; i++, cur_ff++, old_ff++)
			{
				/* if they are not the same then continue */
				if (  cur_ff->ao != old_ff->ao
				   || (   cur_ff->disp + cur_ff->col 
				      !=  old_ff->disp + old_ff->col 
				      )
				   )
				{
					goto CONTINUING;
				}
			}
			/* we've gone all the way though, and the stack
			   traces are the same, so beep and restore
			   the old position */
			break;
		}
	    CONTINUING:
		/* is this it? if so return, the global state is
		   set to display us in the new position */
		if (IIVQpmPatternMatch(ffao->name) == TRUE)
		{
			return;	/* <---- successfull return */
		}
	}
	/* the search failed, beep and restore state */
	FTbell();
	ffrow = t_ffrow;
	ff = t_ff;
	ffao = t_ffao;
	MEcopy((PTR)t_ff_table, sizeof(ff_table), (PTR)ff_table);
	/* <---- failure return */
}
/*{
** Name:	ff_NxtFr - Move to the next frame
**
** Description:
**	This routine is called by the "Find" command.
**	It traverses to the next frame to consider for a match
**	(using a depth first traversal) and sets all of the state 
**	variables accordingly.
**
** Inputs:
**		<global state variables>
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects: 
**
** History:
**	10/20/89 (tom) - created
*/
LOC VOID
ff_NxtFr()
{
	/* if we can go down.. then do it (depth first) */
	if (IIVQnmNumMenus(ffao))
	{
		/* must kill the memory of the line below so that
		   the ff_GoDown() function will correctly start on
		   the first child of the current frame */
		ff[ffrow + 1].ao = (OO_OBJECT*)NULL;
		ff_GoDown();
		return;
	}
	/* we will now attempt to go right.. but if we can't then
	   we will go up.. when we can go right we will.. else
	   we will stop at the top of the tree */
	for (;;)
	{
		/* if we are at the top.. then we're done */
		if (ff_AtTop())
		{
			break;
		}
		/* if we can go right.. then do it */
		if (ff[ffrow].disp + ff[ffrow].col 
			!= IIVQnmNumMenus(ff[ffrow - 1].ao) - 1)
		{
			ff_GoRight();
			break;
		}
		/* otherwise go up */
		ff_GoUp();
	}
}
/*{
** Name:	ff_LocFrame	- locate and position on the indicated frame 
**
** Description:
**	This routine searches the frame tree for given frame to position on.
**
** Inputs:
**	USER_FRAME *top;	- ptr to user frame at the top of the app
**	OO_OBJECT *match;	- ptr to the requested current frame
**	
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects: 
**
** History:
**	03/31/89 (tom) - created
*/
LOC VOID
ff_LocFrame(top, match)
USER_FRAME *top;
OO_OBJECT *match;
{
	i4 dif;
	ffrow = 0;
	ff = ff_table;
	ff_table[0].ao = ffao = (OO_OBJECT*)top;
	ff_table[0].disp = 0;
	ff_table[0].col = 0;
	/* try to locate the object by recursive depth first search */ 
	_VOID_ ff_LocFr(match);
	/* handle scrolling */
	if ( (dif = ffrow - (IIVQrowmax - 2)) > 0)
	{
		ffrow -= dif;
		ff += dif;
	}
	/* null out all lines below us */
	ff_ZapBelow();
	/* expand children below us if necessary */
	ff_CheckBelow(FALSE);
}
/*{
** Name:	ff_LocFr	- position on the indicated frame (recursive) 
**
** Description:
**	This routine searches the frame tree for given frame to position on.
**	This routine does a depth first recursive tree search.
**
** Inputs:
**	OO_OBJECT *match;	- ptr to ptr to the requested current frame
**	
**
** Outputs:
**	Returns:
**		bool		- TRUE for found
**				  FALSE for not found
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/31/89 (tom) - created
*/
LOC bool 
ff_LocFr(match)
register OO_OBJECT *match;
{
	register i4  i;
	register i4  num;
	register MFMENU **menus;
	OO_OBJECT *old;
	/* is this the one.. */
	if (ffao == match)
	{
		return (TRUE);
	}
	/* if not, and this one has menus, 
	   and child frames then recurse on the children */
	if (num = IIVQnmNumMenus(ffao))
	{
		menus = ((USER_FRAME*)ffao)->mf->menus;
		old = ffao;
		ffrow++;	/* bump row */
		/* Detect if new row level is still within ff_table bounds.
		** When vision code allowing 'insert' to add a frame with
		** sublevels does total level checking, this will not be needed
		*/
		if ( ffrow <=  FF_MAXDEPTH )
		{
			ff_table[ffrow].col = 0;
			/* loop on all of the menus.. trying out each one */
			for (i = 0; i < num; i++)
			{
				/* setup the global state variables */
				ff_table[ffrow].ao = ffao = menus[i]->apobj;
				if (i < IIVQcolmax)
				{
					ff_table[ffrow].col = i;
					ff_table[ffrow].disp = 0;
				}
				else
				{
					ff_table[ffrow].col = IIVQcolmax - 1;
					ff_table[ffrow].disp = i - (IIVQcolmax - 1);
				}
				/* recurse on the child frame */
				if (ff_LocFr(match) == TRUE)
				{
					/* if one our descendants finds a match
				   	then we return.. else keep searching
				   	on this level */
					return (TRUE);
				}
			}
		}
		ffrow--;	/* decrement row.. cause we failed */
		ffao = old;
	}
	/* unable to find on this level or below.. so we return false
	   so we will continue to search on the level above */
	return (FALSE);
}
/*{
** Name:	ff_Insert	- create/paste a frame in the frame flow diagram
**
** Description:
**	This routine supervises the insertion create or paste of a 
**	frame into the frame flow diagram.
**
**	screen_loc logic: if this routine bypasses the display
**		of the direction popup.. then we will post a screen_loc of
**		3 indicating that the next popup should be positioned at
**		3, 3 instead of the default -1,-1 which would float it
**		over the other popup.
**
** Inputs:
**		< the global state information >
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/31/89 (tom) - created
*/
LOC i4
ff_Insert(func)
i4  (*func)();
{
	i4	nrows;
	char	*p;
	char	*q;
	char	options[500];
	bool	junk;
	char	*mnuitm;
	char	*prmpt;
	char	*hfile;
	char	*lr_opt;
	char	*dn_opt;
	i4 	stat = VQ_DONE;
	/* depending on the reason for which we were called.. */
	if (func == ff_Paste)
	{
		mnuitm = ERget(FE_Paste);
		hfile = ERx("vqpstdir.hlp");
		prmpt = _pstdir_prmpt;
		lr_opt = _pstlr_opt;
		dn_opt = _pstdn_opt;
	}
	else
	{
		mnuitm = ERget(FE_Create);
		hfile = ERx("vqinsdir.hlp");
		prmpt = _insdir_prmpt;
		lr_opt = _inslr_opt;
		dn_opt = _insdn_opt;
	}
	/* if we're at the very top of the tree */
	if (ff_AtTop())
	{
		/* if the top frame can't have a menu.. 
		   then special error message */
		if ((frm_info[IIVQotObjectType(ffao)].flags & HAS_MENU) 
			== FALSE)
		{
			IIVQer1(E_VQ0074_NoInsertAllowed, ffao->name);
			return (stat);	
		}
		/* if the top of the tree already has menus..
		   then we just tell them to go down and do the insert */
		if (IIVQnmNumMenus(ffao))
		{
			IIVQer1(E_VQ0041_Insert_at_top, mnuitm);
			return (stat);
		}
		/* if there are no menus from the top of the tree.. 
		   then we just automatically do a downward insertion */
		else
		{	/* only down is valid */
			screen_loc = 3;
			return ff_IF2((PTR) func, INS_DOWN, &junk);
		}
	}
	/* initially assume just the "right" and "left" options */
	nrows = 2;
	p = lr_opt;
	q = options;
	while (*p)
	{
		*q++ = *p++;
	}
	/* if the current frame's type allows menus.. 
	   and it currently has no menus..
	   and we are not trying to exceed depth limits..
	   then we allow insertion below, so concatenate 
	   the "Down" option, and bump the row count */
	if (	(frm_info[IIVQotObjectType(ffao)].flags & HAS_MENU) 
	   && 	IIVQnmNumMenus(ffao) == 0
	   &&	ffrow + (ff - ff_table) < FF_MAXDEPTH - 2 /* depth limitation */
	   )
	{
		p = dn_opt; 
		while (*p)
			*q++ = *p++;
		nrows++;
	}
	*q = EOS;
	/* if the parent frame is already max'ed out then we cannot 
	   allow left/right insertion */
	if (IIVQnmNumMenus(ff[ffrow - 1].ao) >= MF_MAXMENUS)
	{
		i4 mmenu = MF_MAXMENUS;
		i4 mdepth = FF_MAXDEPTH;
		if (nrows > 2)	/* were we allowed to go down */
		{
			/* output a warning message */
			IIVQer2(E_VQ0042_Max_mitems, (PTR)&mmenu, mnuitm);
			/* ask if the user really wants to insert downward */
			if (IIUFver(ERget(S_VQ0001_Insert_down), 0) == TRUE)
			{
				screen_loc = 3;
				stat = ff_IF2((PTR) func, INS_DOWN, &junk);
			}
			return (stat);
		}
		else
		{
			if ((frm_info[IIVQotObjectType(ffao)].flags & HAS_MENU) 
			   && IIVQnmNumMenus(ffao) == 0
			   )
			{
				IIVQer3(E_VQ0043_Max_mitems, 
					(PTR)&mmenu, (PTR)&mdepth, (PTR)mnuitm);
			}
			else
			{
				IIVQer2(E_VQ0044_Max_mitems, 
					(PTR)&mmenu, (PTR)mnuitm);
			}
			return (stat);
		}
	}
	screen_loc = LPGR_FLOAT;
	return (IIFDlpListPick(prmpt, options, nrows, 3, 3, prmpt, 
			hfile, ff_IF2, (PTR)func));
}
/*{
** Name:	ff_IF2	- nested function call for insertion
**
** Description:
**	This funciton is called after list picking occurs 
**	on the direction. It is called as a handler function
**	of the list pick code (setup is in ff_Insert).
**
** Inputs:
**	PTR dat;	- really points to a function pointer
**			  to be called to continue with the 
**			  creation or insertion.
**	i4 choice;	- the choice from the user 
**	bool *resume;	- ptr to listpick resume flag 
**
** Outputs:
**	Returns:
**		returns VQ_DONE if the user said to forget,
**		else return what the function returns.. so we
**		can reflect back the desire to quit the program
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	04/10/89 (tom) - created
*/
LOC i4
ff_IF2(dat, dir, resume)
PTR dat;
i4  dir;
bool *resume;
{
	i4 (*func)() = (i4 (*)())dat;
	ER_MSGID menuitem;
	*resume = FALSE; /* we never resume.. */
	/* the use said forget.. so we return without doing anything. */
	if (dir < 0)	
	{
		return (VQ_DONE);
	}
	/* set static */
	_direction = dir;
	/* setup the menuitem which originally invoked us */
	menuitem = func == ff_Paste 
			? FE_Insert 
			: FE_Create;
	/* now that we know the direction.. we can insure that the 
	   user knows that he is messing with a customized frame. */
	if ( dir == INS_DOWN 
		? ff_TouchCheck(menuitem)
		: ff_TouchParentCheck(menuitem)
	   )
	{
		return (VQ_DONE);
	}
	/* call the function to continue the create or paste operation */
	return (*func)();
}
/*{
** Name:	ff_NewType	- solicit the desired frame type 
**
** Description:
**	This routine supervises the selection of a frame type
**	when the user is creating a new frame.
**
** Inputs:
**		< the global state information >
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/31/89 (tom) - created
*/
LOC i4
ff_NewType()
{
	i4 has_menu;
	register i4  cur;
	register i4  i;
	register char *p;
	register char *q;
	i4 num, x;
	i4 type_tab[NUMFRMTYPES];
	char buf[500];
	q = buf;
	/* this loop constructs a list of the possible frame types
	   which can be placed into the indicated position.. this
	   drives off the frame order table */
	for (num = i = 0; (cur = frm_order[i]) >= 0; i++)
	{
		/* if the user doesn't have abf capability, 
		   don't allow creation of certain frame types */
		if (  IIabAbf == FALSE
		   && frm_info[cur].flags & ABF_TYPE
		   )
		{
			continue;
		}
		/* copy the name into buffer */
		for (p = frm_info[cur].name; *q++ = *p++; );
		/* concatenate the column separator */
		q[-1] = ';';
		/* copy the description into buffer */
		for (p = frm_info[cur].desc; *q++ = *p++; );
		/* concatenate the trailing line feed */
		q[-1] = '\n';
		/* post the type so we can interpret the selection value */
		type_tab[num++] = cur;
	}
	q[-1] = EOS;	/* terminate the string.. and kill last line feed */
	/* allow the user to pick from the list */
	return (IIFDlpListPick(_newtyp_prmpt, buf, num, 
		screen_loc, screen_loc, _newtyp_tit, ERx("vqnewtyp.hlp"),
		ff_NT2, (PTR)type_tab));
}
/*{
** Name:	ff_NT2	- nested call for selecting frame type 
**
** Description:
**	Called as a handler function of the list pick code 
**	after the new frame type has been chosen by the user.
**	(setup is in ff_NewType).
**
** Inputs:
**	PTR dat;	- pointer to callers data area.. 
**			  this is really a pointer to an array
**			  of nats which are used to translate the
**			  the user's choice into a type, 
**			  (see setup function)
**	i4 choice;	- the user's choice
**	bool *resume;	- ptr to caller's resume flag 
**
**		< the global state information >
**
** Outputs:
**
**	sets static _type
**
**	Returns:
**		i4 - always 0.. which is ignored
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	04/10/89 (tom) - created
*/
LOC i4
ff_NT2(dat, choice, resume)
PTR dat;
i4  choice; 
bool *resume;
  {
	i4 stat = VQ_DONE;
	*resume = FALSE;
	/* did the user actually select something */
	if (choice >= 0)
	{
		/* translate the selection index into a frame type, 
		   and post to static */
		_type = ((i4*)dat)[choice];
		/* based on frame type call the appropriate function */
		if (frm_info[_type].flags & HAS_VQ)
		{
			stat = ff_NewVqFrame();
		}
		else if (_type == MF_MENU)
		{
			ff_NewMenuFrame();
		}
		else if (_type == MF_HLPROC )
		{
			ff_NewHLProc();
		}
		else
		{
			ff_NewAbfFrame();
		}
	}
	return (stat);
  }
/*{
** Name:	ff_NewVqFrame	- create a new vq frame
**
** Description:
**	This routine supervises the display loop which prompts for the
**	menu item, short remark, master and (optional) detail table
**	for the new vq type frame. It then calls functions to create the 
**	new frame and the vq and invokes editing of the new vq.
**
** Inputs:
**	< the global state information >
**	..and file statics: 
**		_direction
**		_type
**
** Outputs:
**	Returns:
**		i4
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	04/01/89 (tom) - created
**	31-jul-91 (blaise)
**		Added logic to check whether the user has changed m_in_tf,
**		and only change the Detail field if this is the case.
**		Bug #38327.
**	14-dec-92 (blaise)
**		Added new Import menuitem.
*/
LOC i4  
ff_NewVqFrame()
  {
    char mnuitm[FE_MAXNAME + 1];
    char master[FE_UNRML_MAXNAME + 1];
    char m_in_tf[2];
    bool m_in_tf_new;
    bool m_in_tf_org = FALSE;
    char detail[FE_UNRML_MAXNAME + 1];
    char remark[OOSHORTREMSIZE + 1];
    char fname[FE_MAXNAME + 1];
    i4 cmd;
	i4 stat = VQ_DONE;
/* # line 1776 "ff.qsc" */	/* display */
    {
      if (IIdispfrm(newvq_form,(char *)"f") == 0) goto IIfdE2;
      goto IImuI2;
IIfdI2:;
      if (IIfsetio((char *)0) == 0) goto IIfdE2;
      {
/* # line 1779 "ff.qsc" */	/* host code */
		/* initially set to no */
		m_in_tf[0] = ERget(F_UG0006_No1)[0];
		m_in_tf[1] = '\0';
/* # line 1782 "ff.qsc" */	/* putform */
        {
          if (IIfsetio((char *)0) != 0) {
            IIputfldio(_master_in_tf,(short *)0,1,32,0,m_in_tf);
          } /* IIfsetio */
        }
      }
IIfdB2:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 1786 "ff.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,1,m_in_tf,_master_in_tf);
              } /* IIfsetio */
            }
/* # line 1787 "ff.qsc" */	/* host code */
		m_in_tf_new = IIUGyn(m_in_tf, (STATUS*)NULL);
		/* has the "master in tablefield" field changed? */
		if (m_in_tf_new != m_in_tf_org)
		{
			if (m_in_tf_new == TRUE)
			{
/* # line 1794 "ff.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio(_detail,(short *)0,1,32,0,ERget(F_VQ010D_n_a));
              } /* IIfsetio */
            }
/* # line 1795 "ff.qsc" */	/* host code */
			}
			else
			{
/* # line 1798 "ff.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio(_detail,(short *)0,1,32,0,ERx(""));
              } /* IIfsetio */
            }
/* # line 1799 "ff.qsc" */	/* host code */
			}
			m_in_tf_org = m_in_tf_new;
		}
/* # line 1802 "ff.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB2;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 1806 "ff.qsc" */	/* host code */
		/* if the master in table field option is set, then
		   don't let them enter the detail field */
		if (IIUGyn(m_in_tf, (STATUS*)NULL) == TRUE)
		{
			/* was the user going 'previous field' */
/* # line 1811 "ff.qsc" */	/* inquire_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&cmd,38,(char *)0,0);
              } /* IIiqset */
            }
/* # line 1812 "ff.qsc" */	/* host code */
			if (cmd == 5) 
			{
/* # line 1814 "ff.qsc" */	/* resume */
            {
              IIresfld(_master_in_tf);
              if (1) goto IIfdB2;
            }
/* # line 1815 "ff.qsc" */	/* host code */
			}
			else
			{
/* # line 1818 "ff.qsc" */	/* resume */
            {
              IIresfld(_menuitem);
              if (1) goto IIfdB2;
            }
/* # line 1819 "ff.qsc" */	/* host code */
			}
		}
          }
        } else if (IIretval() == 3) {
          {
/* # line 1826 "ff.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,mnuitm,_menuitem);
                IIgetfldio((short *)0,1,32,OOSHORTREMSIZE + 1-1,remark,_remark);
                IIgetfldio((short *)0,1,32,FE_UNRML_MAXNAME + 1-1,master,
                _master);
                IIgetfldio((short *)0,1,32,FE_UNRML_MAXNAME + 1-1,detail,
                _detail);
              } /* IIfsetio */
            }
/* # line 1829 "ff.qsc" */	/* host code */
		/* if the master in  table field is 'yes' then we
		   null out the detail table, even though it contains
		   an N/A string for the user's benefit */
		if (IIUGyn(m_in_tf, (STATUS*)NULL) == TRUE)
		{
			detail[0] = EOS;
		}
		/* check if menuitem is unique for the parent frame */
		if (ff_CheckMenuItem(mnuitm) == FALSE)
		{
/* # line 1840 "ff.qsc" */	/* resume */
            {
              IIresfld(_menuitem);
              if (1) goto IIfdB2;
            }
/* # line 1841 "ff.qsc" */	/* host code */
		}
		if (master[0] == EOS) /* mandatory field */
		{
			IIVQer2(E_VQ0047_No_master_table, 
					ERget(FE_Cancel),
					ERget(FE_Lookup));
/* # line 1848 "ff.qsc" */	/* resume */
            {
              IIresfld(_master);
              if (1) goto IIfdB2;
            }
/* # line 1849 "ff.qsc" */	/* host code */
		}
		if (ff_CheckRel(master) == FALSE) /* table must exist */
		{
			IIVQer3(E_VQ0048_No_Table, master,
					ERget(FE_Lookup),
					ERget(FE_Cancel));
/* # line 1855 "ff.qsc" */	/* resume */
            {
              IIresfld(_master);
              if (1) goto IIfdB2;
            }
/* # line 1856 "ff.qsc" */	/* host code */
		}
		if (detail[0] != EOS)
		{
			if (ff_CheckRel(detail) == FALSE) /* table must exist */
			{
				IIVQer3(E_VQ0048_No_Table, detail,
						ERget(FE_Lookup),
						ERget(FE_Cancel));
/* # line 1864 "ff.qsc" */	/* resume */
            {
              IIresfld(_detail);
              if (1) goto IIfdB2;
            }
/* # line 1865 "ff.qsc" */	/* host code */
			}
		}
		/* create the new frame */
		if (ff_MakeNew(mnuitm, remark) == FALSE)
		{
/* # line 1871 "ff.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
/* # line 1872 "ff.qsc" */	/* host code */
		}
		/* create the visual query from the tables given */
		if ((stat = ff_MakeVq(master, detail, m_in_tf)) == FALSE)
		{
 			/* put out user information message */
	 		IIUGmsg(ERget(S_VQ000B_Destroy_Frm_Msg), FALSE, 1, 
				ffao->name);
		 	IIABdcvDestroyCompVision(IIVQappl, ffao, IIVQcontext);
			/* fix up state info structures */
			ff_DelFixup();
		}
/* # line 1886 "ff.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 1892 "ff.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 1898 "ff.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,mnuitm,_menuitem);
                IIgetfldio((short *)0,1,32,OOSHORTREMSIZE + 1-1,remark,_remark);
              } /* IIfsetio */
            }
/* # line 1900 "ff.qsc" */	/* host code */
		/* check if menuitem is unique for the parent frame */
		if (ff_CheckMenuItem(mnuitm) == FALSE)
		{
/* # line 1903 "ff.qsc" */	/* resume */
            {
              IIresfld(_menuitem);
              if (1) goto IIfdB2;
            }
/* # line 1904 "ff.qsc" */	/* host code */
		}
		/* remember the menuitem and remark for when we 
		   are creating the frame */
		_copymnu = mnuitm;
		_copyrem = remark;
		/* select a frame to duplicate, and do it */
		stat = ff_Copy();
/* # line 1915 "ff.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 1922 "ff.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXNAME + 1-1,mnuitm,_menuitem);
                IIgetfldio((short *)0,1,32,OOSHORTREMSIZE + 1-1,remark,_remark);
              } /* IIfsetio */
            }
/* # line 1924 "ff.qsc" */	/* host code */
		/* check if menuitem is unique for the parent frame */
		if (ff_CheckMenuItem(mnuitm) == FALSE)
		{
/* # line 1927 "ff.qsc" */	/* resume */
            {
              IIresfld(_menuitem);
              if (1) goto IIfdB2;
            }
/* # line 1928 "ff.qsc" */	/* host code */
		}
		/* remember the menuitem and remark for when we 
		   are creating the frame */
		_copymnu = mnuitm;
		_copyrem = remark;
		/* select a frame to import, and do it */
		stat = IIVQimport();
/* # line 1937 "ff.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE2;
            }
          }
        } else if (IIretval() == 7) {
          {
            char tblname[FE_MAXTABNAME+1];
            char *p_tbl;
/* # line 1946 "ff.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,fname,22,newvq_form,
                0);
              } /* IIiqset */
            }
/* # line 1947 "ff.qsc" */	/* host code */
                if (STequal(fname, _master) || (STequal(fname, _detail)))
                {
/* # line 1949 "ff.qsc" */	/* getform */
            {
              if (IIfsetio(newvq_form) != 0) {
                IIgetfldio((short *)0,1,32,FE_MAXTABNAME+1-1,tblname,fname);
              } /* IIfsetio */
            }
/* # line 1951 "ff.qsc" */	/* host code */
                	/* If "tblname" is empty string, then IITUtpTblPik
			** treats it as a wildcard, otherwise it is used
			** as a literal string. If "tblname" contains
			** pattern match characters, then an SQL LIKE
			** clause will be used.
			*/
			p_tbl = IITUtpTblPik(tblname, IIabExename); 
			if (p_tbl != NULL && *p_tbl != EOS)
                        {
/* # line 1961 "ff.qsc" */	/* putform */
            {
              if (IIfsetio(newvq_form) != 0) {
                IIputfldio(fname,(short *)0,1,32,0,p_tbl);
              } /* IIfsetio */
            }
/* # line 1962 "ff.qsc" */	/* host code */
                        }
                }
                else if (STequal(fname, _master_in_tf))
		{
			IIUFfieldHelp();
		}
		else
                {
                        /* report error that lookups are not supported
                           on the current field */
                        IIVQer2(E_VQ0056_No_Lookup,
                                ERget(FE_Lookup), ERget(FE_Forget));
                }
          }
        } else if (IIretval() == 8) {
          {
/* # line 1980 "ff.qsc" */	/* host code */
		FEhelp(ERx("vqnewvq.hlp"), _insnew_prmpt);  
          }
        } else {
          if (1) goto IIfdE2;
        } /* IIretval */
      } /* IIrunform */
IIfdF2:
      if (IIchkfrm() == 0) goto IIfdB2;
      if (IIfsetio((char *)0) == 0) goto IIfdE2;
      goto IIfdE2;
IImuI2:
      if (IIFRafActFld(_master_in_tf,0,1) == 0) goto IIfdE2;
      if (IIFRafActFld(_detail,1,2) == 0) goto IIfdE2;
      if (IIinitmu() == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_OK),ERget(F_VQ0050_OKExpl),2,2,3) == 0) goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,3) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_Cancel),ERget(F_VQ0051_CancelExpl),0,0,4) == 0) 
      goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,0,0,4) == 0) goto IIfdE2;
      if (IInmuact(ERget(F_VQ002F_Duplicate),ERget(F_VQ0052_DuplicateExpl),0,0
      ,5) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_Import),ERget(F_VQ0163_ImportExpl),0,0,6) == 0) 
      goto IIfdE2;
      if (IInmuact(ERget(FE_Lookup),ERget(F_VQ0053_LookupExpl),0,0,7) == 0) 
      goto IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,7) == 0) goto IIfdE2;
      if (IInmuact(ERget(FE_Help),ERget(F_VQ0054_HelpExpl),0,0,8) == 0) goto 
      IIfdE2;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,8) == 0) goto IIfdE2;
      if (IIendmu() == 0) goto IIfdE2;
      goto IIfdI2;
IIfdE2:;
      IIendfrm();
    }
/* # line 1984 "ff.qsc" */	/* host code */
	return (stat);
  }
/*{
** Name:	ff_CheckMenuitem	- check menuitem text 
**
** Description:
**	This function checks menuitem text for existence and uniqueness.
**	If there are problems, we report an error here and return FALSE.
**
** Inputs:
**	char *mnuitm;	- menu item text
**
** Outputs:
**	Returns:
**		bool	- TRUE all ok
**			  FALSE duplicate found
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	04/01/89 (tom) - created
*/
LOC bool 
ff_CheckMenuItem(mnuitm)
register char *mnuitm;
{
	register METAFRAME *mf;
	register i4  i;
	mf = ((USER_FRAME*) ff[ffrow - (_direction != INS_DOWN)].ao)->mf; 
	/* if the user entered a null menuitem text give error */
	if (mnuitm[0] == EOS)
	{
		IIVQer2(E_VQ0045_No_menuitem, mf->apobj->name, 
			ERget(FE_Cancel));
		return (FALSE);
	}
	/* loop through the existing menuitems of the parent frame
	   and check for duplications */
	for (i = 0; i < mf->nummenus; i++)
	{
		/* if we are not really inserting.. just editing..
		   and we are looking at the current object,
		   then don't bother with it. */
		if (  _direction == INS_FALSE 
		   && i == ff[ffrow].disp + ff[ffrow].col
		   )
		{
			continue;
		}
		if (STbcompare(mf->menus[i]->text, 0, mnuitm, 0, TRUE) == 0)
		{
			IIVQer3(E_VQ006C_Dup_menuitem, mnuitm, 
				ERget(FE_Cancel), mf->apobj->name);
			return (FALSE);
		}
	}
	return (TRUE);
}
/*{
** Name:	ff_MakeNew	- create a new frame 
**
** Description:
**	This function creates a new frame in the application
**	and manages inserting it into the structures, positioning
**	on it etc..
**
** Inputs:
**	char *mnuitm;	- menu item text
**	char *remark;	- short remark
**
** Outputs:
**	Returns:
**		bool	- TRUE all ok
**			  FALSE error occured
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	04/01/89 (tom) - created
**	02/20/90 (Mike S) - Set DOGEN and DOFORM flags for menu frame
*/
LOC bool 
ff_MakeNew(mnuitm, remark)
char *mnuitm;
char *remark;
{
	USER_FRAME *parent;	
	USER_FRAME *newobj;	
	i4 idx;	
	char fname[FE_MAXNAME + 1];
	/* create a unique frame name from the menu item text */
	IIVQmfnMakeFrameName(IIVQappl, mnuitm, fname);
	/*
	** create the default object
	** ffol_lang and ffdml only apply to HL procedures - whatever
	** trash is passed in other cases doesn't matter.
	*/
	if (IIABmdoMakeDefaultObj(IIVQappl, fname, 
			frm_info[_type].class, IIVQcontext,
			(OO_OBJECT**)&newobj, ffol_lang, ffdml) != OK)
	{
		/* an error has already been issued */
		return (FALSE);
	}
	_VOID_ ff_Elock((OO_OBJECT*)&newobj, TRUE);
	newobj->short_remark = (char*)IIVQ_MAlloc(0, remark); 
	/* setup the parent and the menuitem index */
	if (_direction == INS_DOWN)
	{
		parent = (USER_FRAME*)ffao;
		idx = 0;
	}
	else
	{
		parent = (USER_FRAME*)ff[ffrow - 1].ao;
		idx = ff[ffrow].disp + ff[ffrow].col;
		if (_direction == INS_RIGHT)
			idx++;
	}
	/* must update the caller's menu array */
	if (ff_Add2Menu(parent, idx, mnuitm, newobj) == FALSE)
	{
		/* if there is an allocation error then
		   ff_Add2Menu will be processing it */
		return (FALSE);
	}
	/* metaframe classes need to have their state flag set to
	   indicate that they are new frames */
	switch (newobj->class)
	{
	case OC_MUFRAME:
		/* 
		** Menu frames need DOGEN and DOFORM set here, since they
		** won't be set in the VQ edit.
		*/
		newobj->mf->state = MFST_NEWFR|MFST_DOGEN|MFST_DOFORM;
		break;
	case OC_APPFRAME:
	case OC_UPDFRAME:
	case OC_BRWFRAME:
		newobj->mf->state = MFST_NEWFR;
		break;
	}
	/* !!! the next two calls should really be in one transaction,
	   but even though they are not, the code that reads the
	   application into the frame flow diagram will fix the 
	   structures if needed (right BobM?) */
	/* must update the new child.. */
	if (IIABucvUpdCompVision(IIVQappl, (OO_OBJECT*)newobj, IIVQcontext) 
		!= OK)
	{
		/* errors will already be reported */
		return (FALSE);
	}
	/* ...and the now changed parent frame */
	if (ff_UpdDepends(parent->mf, TRUE) != OK)
	{
		/* errors will already be reported */
		return (FALSE);
	}
	/* position on the new frame */
	if (_direction == INS_DOWN)
	{
		/* this call will insure scroll if necessary */
		ff_CheckBelow(TRUE);
		ff_GoDown();
	}
	else if (_direction == INS_RIGHT)
	{
		ff_GoRight();
	}
	else
	{
		ffao = ff[ffrow].ao = (OO_OBJECT*)newobj;
	}
	return (TRUE);
}
 /*{
** Name:	ff_UpdDepends	- update dependancys of given frame
**
** Description:
**	This routine marks the frame as needing regen'ing and writes out it's
**	menu dependancies.
**
**	NOTE:
**		All interactions which change the frame flow diagram seem
**		to come through here, and only come though here once.  This
**		makes sense since Create / Destroy / Cut / Paste / Edit
**		menuitem will force you to update a parent frame, and should
**		only force ytou to update 1.  Given this, we place a call to
**		IIABffcFrameFlowChanged() here.
**
**	The dependency update is now conditional, making this routine
**	something of a misnomer, but such is life.
**
** Inputs:
**	METAFRAME *mf;		- ptr to metaframe to write
**	bool dep;		- update dependencies.
**
** Outputs:
**	Returns:
**		STATUS	
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	09/02/89 (tom) - created
*/
LOC STATUS 
ff_UpdDepends(mf, dep)
METAFRAME *mf;
bool dep;
{
	/* set the flags, if it is a menu.. then it also requires formgen */
	mf->state |= 
		( MFST_DOGEN 
		  | ( 
			IIVQotObjectType((OO_OBJECT*)mf->apobj) == MF_MENU
			? MFST_DOFORM : 0
		    )   
		); 
	/* flag the fact that we changed the frame flow diagram */
	IIABffcFrameFlowChanged(IIVQappl->ooid);
	/* write the flags to the database */
	IIAMufqUserFrameQuick((OO_OBJECT*)mf->apobj);
	/* udpate the dependancies */
	if (dep)
		return IIAMwmdWriteMenuDep(mf, TRUE);
	return OK;
}
/*{
** Name:	ff_NewMenuFrame	- create a new menu frame
**
** Description:
**	This routine supervises the creation of a new menu type frame.
**
** Inputs:
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	04/01/89 (tom) - created
*/
LOC VOID 
ff_NewMenuFrame()
{
	char mnuitm[FE_MAXNAME + 1];
	char remark[OOSHORTREMSIZE + 1];
	/* prompt for the frame name, and remark */
	if (ff_NewFrame(mnuitm, remark) == FALSE)
	{
		return;
	}
	/* create the new frame */
	if (ff_MakeNew(mnuitm, remark) == FALSE)
	{
		return;
	}
}
/*{
** Name:	ff_NewAbfFrame	- create a new traditional ABF type frame
**
** Description:
**	This routine supervises the creation of a new ABF type frame.
**
** Inputs:
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	04/01/89 (tom) - created
*/
LOC VOID 
ff_NewAbfFrame()
{
	char mnuitm[FE_MAXNAME + 1];
	char remark[OOSHORTREMSIZE + 1];
	/* prompt for the frame name, and remark */
	if (ff_NewFrame(mnuitm, remark) == FALSE)
	{
		return;
	}
	/* create the new frame */
	if (ff_MakeNew(mnuitm, remark) == FALSE)
	{
		return;
	}
	/* invoke an edit of the newly created frame */
# if 0	/* let's not call the edit for now */
	IIABecvEditCompVision(IIVQappl, ffao, IIVQcontext);
# endif
}
/*{
** Name:	ff_NewHLProc - create a new host language procedure.
**
** Description:
**	This routine supervises the creation of a new host language procedure.
**
** Inputs:
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	12/89 (bobm) - created
*/
LOC VOID 
ff_NewHLProc()
{
	STATUS rc;
	rc = IIABcpvCreateProcVision( IIVQappl, OC_HLPROC,
					ff_HLhook, IIVQcontext);
	/* invoke an edit of the newly created frame */
	if (rc == OK)
	{
# if 0		/* let's not call the edit for now */
		IIABecvEditCompVision(IIVQappl, ffao, IIVQcontext);
# endif
	}
}
/*{
** Name:	ff_HLhook	- hook for host language menu in abf
**
** Description:
**	this routine intercepts the normal abf create operation, and allows
**	entry into the VQ logic.  The HLPROC passed in is actually a
**	dummy is used only to set language parameters.
**
** Inputs:
**	HLPROC *proc	- procedure information.
**
** Outputs:
**	Returns:
**		STATUS
*/
LOC STATUS
ff_HLhook(proc)
HLPROC *proc;
{
	char mnuitm[FE_MAXNAME + 5];
	char remark[OOSHORTREMSIZE + 1];
	ffol_lang = proc->ol_lang;
	ffdml = proc->dml;
	/* prompt for the frame name, and remark */
	if (ff_NewFrame(mnuitm, remark) == FALSE)
	{
		return FAIL;
	}
	/* create the new frame */
	if (ff_MakeNew(mnuitm, remark) == FALSE)
	{
		return FAIL;
	}
	return OK;
}
/*{
** Name:	ff_NewFrame	- prompt for new frame name and remark
**
** Description:
**	This frame runs a display loop which prompts for the 
**	menu item name and the remark for a new frame.
**
** Inputs:
**	char *mnuitm;	- pointer to caller's buffer for menu item text
**	char *remark;	- pointer to caller's buffer for short remark
**		Note: these buffers must be at least as large as the
**			form fields.
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	04/01/89 (tom) - created
*/
LOC bool 
ff_NewFrame(mnuitm, remark)
  char *mnuitm;
  char *remark;
  {
	i4 retval;
/* # line 2437 "ff.qsc" */	/* display */
    {
      if (IIdispfrm(newfr_form,(char *)"f") == 0) goto IIfdE3;
      goto IImuI3;
IIfdI3:;
      if (IIfsetio((char *)0) == 0) goto IIfdE3;
      {
      }
IIfdB3:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 2442 "ff.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,0,mnuitm,_menuitem);
                IIgetfldio((short *)0,1,32,0,remark,_remark);
              } /* IIfsetio */
            }
/* # line 2444 "ff.qsc" */	/* host code */
		/* check if menuitem is unique for the parent frame */
		if (ff_CheckMenuItem(mnuitm) == FALSE)
		{
/* # line 2447 "ff.qsc" */	/* resume */
            {
              IIresfld(_menuitem);
              if (1) goto IIfdB3;
            }
/* # line 2448 "ff.qsc" */	/* host code */
		}
		retval = TRUE;
/* # line 2450 "ff.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE3;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 2456 "ff.qsc" */	/* host code */
		retval = FALSE;
/* # line 2457 "ff.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE3;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 2462 "ff.qsc" */	/* host code */
		FEhelp(ERx("vqnewfr.hlp"), _insnew_prmpt);  
          }
        } else {
          if (1) goto IIfdE3;
        } /* IIretval */
      } /* IIrunform */
IIfdF3:
      if (IIchkfrm() == 0) goto IIfdB3;
      if (IIfsetio((char *)0) == 0) goto IIfdE3;
      goto IIfdE3;
IImuI3:
      if (IIinitmu() == 0) goto IIfdE3;
      if (IInmuact(ERget(FE_OK),ERget(F_VQ0055_OKExpl),2,2,1) == 0) goto IIfdE3;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE3;
      if (IInmuact(ERget(FE_Cancel),ERget(F_VQ0051_CancelExpl),2,2,2) == 0) 
      goto IIfdE3;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,2,2,2) == 0) goto IIfdE3;
      if (IInmuact(ERget(FE_Help),ERget(F_VQ0054_HelpExpl),2,2,3) == 0) goto 
      IIfdE3;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,2,2,3) == 0) goto IIfdE3;
      if (IIendmu() == 0) goto IIfdE3;
      goto IIfdI3;
IIfdE3:;
      IIendfrm();
    }
/* # line 2466 "ff.qsc" */	/* host code */
	return (retval);
  }
/*{
** Name:	ff_NewMItem	- prompt for new menu item name for a paste
**
** Description:
**	This frame runs a display loop which prompts for the 
**	menu item name of a frame to be pasted into the application tree.
**
** Inputs:
**	char *mnuitm;	- pointer to caller's buffer for menu item text
**		Note: this buffer must be at least as large as the
**			form fields.
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	08/23/89 (tom) - created
*/
LOC bool 
ff_NewMItem(menuitm, hlp_file, hlp_prmpt)
  char *menuitm;
char *hlp_file;
char *hlp_prmpt;
  {
	i4 retval;
/* # line 2500 "ff.qsc" */	/* display */
    {
      if (IIdispfrm(newmi_form,(char *)"f") == 0) goto IIfdE4;
      goto IImuI4;
IIfdI4:;
      if (IIfsetio((char *)0) == 0) goto IIfdE4;
      {
/* # line 2503 "ff.qsc" */	/* putform */
        {
          if (IIfsetio((char *)0) != 0) {
            IIputfldio(_menuitem,(short *)0,1,32,0,menuitm);
          } /* IIfsetio */
        }
      }
IIfdB4:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 2508 "ff.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,0,menuitm,_menuitem);
              } /* IIfsetio */
            }
/* # line 2510 "ff.qsc" */	/* host code */
		/* check if menuitem is unique for the parent frame */
		if (ff_CheckMenuItem(menuitm) == FALSE)
		{
/* # line 2513 "ff.qsc" */	/* resume */
            {
              IIresfld(_menuitem);
              if (1) goto IIfdB4;
            }
/* # line 2514 "ff.qsc" */	/* host code */
		}
		retval = TRUE;
/* # line 2516 "ff.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE4;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 2522 "ff.qsc" */	/* host code */
		retval = FALSE;
/* # line 2523 "ff.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE4;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 2528 "ff.qsc" */	/* host code */
		FEhelp(hlp_file, hlp_prmpt);  
          }
        } else {
          if (1) goto IIfdE4;
        } /* IIretval */
      } /* IIrunform */
IIfdF4:
      if (IIchkfrm() == 0) goto IIfdB4;
      if (IIfsetio((char *)0) == 0) goto IIfdE4;
      goto IIfdE4;
IImuI4:
      if (IIinitmu() == 0) goto IIfdE4;
      if (IInmuact(ERget(FE_OK),ERget(F_VQ0056_OKExpl),2,2,1) == 0) goto IIfdE4;
      IIFRInternal(0);
      if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE4;
      if (IInmuact(ERget(FE_Cancel),ERget(F_VQ0057_CancelExpl),2,2,2) == 0) 
      goto IIfdE4;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,2,2,2) == 0) goto IIfdE4;
      if (IInmuact(ERget(FE_Help),ERget(F_VQ0058_HelpExpl),2,2,3) == 0) goto 
      IIfdE4;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,2,2,3) == 0) goto IIfdE4;
      if (IIendmu() == 0) goto IIfdE4;
      goto IIfdI4;
IIfdE4:;
      IIendfrm();
    }
/* # line 2532 "ff.qsc" */	/* host code */
	return (retval);
  }
/*{
** Name:	ff_Add2Menu	- add menuitem into parent for new child 
**
** Description:
**	This routine supervises the insertion of a new menuitem
**	into the parent frame for a newly created child.
**
** Inputs:
**	USER_FRAME *parent;	- parent frame
**	i4 idx;		- index at which to place new child
**	char *mnuitm;		- menuitem text 
**	USER_FRAME *newobj;	- new object
**
** Outputs:
**	Returns:
**		bool	- currently always TRUE
**
**	Exceptions:
**
** Side Effects:
**		allocates memory
**
** History:
**	03/31/89 (tom) - created
*/
LOC bool
ff_Add2Menu(parent, idx, mnuitm, newobj)
USER_FRAME *parent;
i4  idx;
char *mnuitm;
USER_FRAME *newobj;
{
	register i4  i;
	register MFMENU *menu;
	register MFMENU **menus;
	menu = (MFMENU*)IIVQ_MAlloc(sizeof(MFMENU), (char*)NULL);
	menu->text = (char*)IIVQ_MAlloc(0, mnuitm);
	menus = parent->mf->menus;
	for (i = parent->mf->nummenus++ - 1; i >= idx; i--)
	{
		menus[i+1] = menus[i];
	}
	menus[idx] = menu; 
	menu->apobj = (OO_OBJECT*)newobj;
	return (TRUE);
}
/*{
** Name:	ff_DelMenu	- delete a menuitem from a parent frame
**
** Description:
**	This routine supervises the deletion of a menuitem
**	from a parent frame.
**
** Inputs:
**	USER_FRAME *parent;	- parent frame
**	i4 idx;		- index at which to place new child
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**		allocates memory
**
** History:
**	03/31/89 (tom) - created
*/
LOC VOID 
ff_DelMenu(parent, idx)
USER_FRAME *parent;
register i4  idx;
{
	register MFMENU **menus;
	menus = parent->mf->menus;
	for ( ; idx < parent->mf->nummenus; idx++)
	{
		menus[idx] = menus[idx + 1];
	}
	parent->mf->nummenus--;
}
/*{
** Name:	IIVQfcsFrameCompSort	- sort function to pass to IIUGqsort
**
** Description:
**	This function's address is passed to the IIUGqsort function
**	to sort the components of the application for the purposes of
**	displaying them to the user during a paste operation.
**	This routine returns the value returned from STcompare
**	on the frame names.
**
** Inputs:
**	char *x1;	- first component to compare
**	char *x2;	- 2nd component to compare
**	
** Outputs:
**	Returns:
**		i4	-    1 or -1 depending on name order.	
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/31/89 (tom) - created
*/
i4  
IIVQfcsFrameCompSort(x1, x2)
char *x1;
char *x2;
{
	return (STcompare( 
			(*(APPL_COMP**)x1)->name, 
			(*(APPL_COMP**)x2)->name
			)
		); 
}
/*{
** Name:	ff_Paste	- Paste in an existing frame
**
** Description:
**	This routine supervises the pasting in of an existing frame
**	into the frame flow diagram. First we get the menuitem text.
**	Then we  go through the application structures and construct
**	a sorted list of frames that may be inserted and call list
**	pick with them. 
**
** Inputs:
**		< the global state information >
**
** Outputs:
**	Returns:
**		i4
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/31/89 (tom) - created
*/
LOC i4
ff_Paste()
{
	register i4  stat;
	stat = ff_SelFr(ff_PF2);
	if (stat & VQ_NONE)
	{
		IIVQer0(E_VQ0075_No_Paste_Frames);
		return (VQ_DONE);
	}
	else if ((stat & VQ_ERRM) || (stat & VQ_ERRF))
	{
		IIVQer0(E_VQ006E_Bad_Sel_Form);
		return (VQ_DONE);
	}
	return (stat);
} 
/*{
** Name:	ff_Copy		- Copy an existing Vq
**
** Description:
**	This routine supervises the pasting in of an existing frame
**	into the frame flow diagram. First we get the menuitem text.
**	Then we  go through the application structures and construct
**	a sorted list of frames that may be inserted and call list
**	pick with them. 
**
** Inputs:
**		< the global state information >
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/31/89 (tom) - created
*/
LOC i4
ff_Copy()
{
	register i4  stat;
	/* set the screen location of the SelFr popup to float */
	screen_loc = LPGR_FLOAT;
	stat = ff_SelFr(IIVQcfCopyFrame);
	if (stat & VQ_NONE)
	{
		IIVQer0(E_VQ0076_No_Copy_Frames);
		return (VQ_DONE);
	}
	else if ((stat & VQ_ERRM) || (stat & VQ_ERRF))
	{
		IIVQer0(E_VQ006E_Bad_Sel_Form);
		return (VQ_DONE);
	}
	else
	{
		return (stat);
	}
} 
/*{
** Name:	ff_SelFr	- create select a frame list and form
**
** Description:
**	This routine supervises the pasting in of an existing frame
**	into the frame flow diagram. First we get the menuitem text.
**	Then we  go through the application structures and construct
**	a sorted list of frames that may be inserted and call list
**	pick with them. 
**
** Inputs:
**	i4 (*func)()	- function to pass to list pick
**			  (this function indicates the usage that is
**			  being made of this function)
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/31/89 (tom) - created
*/
LOC i4
ff_SelFr(func)
i4  (*func)();
{
	register APPL_COMP *comp;
	register i4  i;
	register i4  count;
	APPL_COMP **array;
	i4 status;
  PTR frame;
  char *obj_name;
  char *class_name;
  char *obj_desc;
	char *frtitle;
	char *helptit;
	char *helpfil;
	i4 stat = VQ_DONE;
	/* if this is to be a paste (insert).. then we set up the
	   messages one way, else it's a vq copy.. so we set up 
	   another way. */
	if (func == ff_PF2)
	{
		frtitle = ERget(F_VQ00EF_Select_Paste_Frame);
		helptit = ERget(F_VQ00F0_Select_P_Frame);
		helpfil = ERx("vqselpfr.hlp");
	}
	else if (func == IIVQcfCopyFrame) /* we're called from the copy a vq */
	{
		frtitle = ERget(F_VQ00F1_Select_Copy_Frame);
		helptit = ERget(F_VQ00F2_Select_C_Frame);
		helpfil = ERx("vqselcfr.hlp");
	}
	else
	{
		return (stat);	/* internal error */
	}
	/* first go through the application components and count them */
	comp = (APPL_COMP*)IIVQappl->comps;
	for (count = 0; comp != (APPL_COMP*)NULL; comp = comp->_next)
	{
		switch (comp->class)
		{
		case OC_MUFRAME: 
		case OC_OSLFRAME: 
		case OC_OLDOSLFRAME: 
		case OC_RWFRAME:
		case OC_QBFFRAME: 
		case OC_GRFRAME: 
		case OC_GBFFRAME:
		case OC_HLPROC: 
		case OC_OSLPROC:
		case OC_DBPROC:
				/* if not for pasting.. then just ignore */
				if (func != ff_PF2)
				{
					continue;
				}
				/* else fall thru */
		case OC_APPFRAME: 
		case OC_UPDFRAME:
		case OC_BRWFRAME: 
				count++;
		}
	}
	/* if there is 1 or less frames.. then pasting makes no sense
	   copy is only disallowed if there are absolutely no frames */
	if (count - (func == ff_PF2) <= 0)
	{
		return (VQ_NONE);
	}
	/* allocate an array to contain the component list */
	if ( (array = (APPL_COMP**) MEreqmem((u_i4)0, 
			(u_i4)(count * sizeof(PTR)), 
			FALSE, (STATUS*)NULL)) == NULL)
	{
		return (VQ_ERRM);
	}
	/* fill in the allocated array */
	comp = (APPL_COMP*)IIVQappl->comps;
	for (i = 0; i < count; comp = comp->_next)
	{
		/* first.. if it's not a VQ type, and it is not a paste,
		   then we ignore it */
		switch (comp->class)
		{
		case OC_MUFRAME:
		case OC_OSLFRAME:
		case OC_OLDOSLFRAME:
		case OC_RWFRAME:
		case OC_QBFFRAME:
		case OC_GRFRAME:
		case OC_GBFFRAME:
		case OC_HLPROC:
		case OC_OSLPROC:
		case OC_DBPROC:
			if (func != ff_PF2) 
			{
				continue;
			}
		}
		switch (comp->class)
		{
		case OC_MUFRAME:	/* ..appears here and above */
		case OC_APPFRAME:
		case OC_UPDFRAME:
		case OC_BRWFRAME:
			/* since it's a frame type which supports children
			   unmark the frame now, latter we will be
			   scanning the tree looking for recursive
			   form system calls, and that code requires
			   that the frames not be marked (paste case only
			   requires this but copy case will tolerate) */
			((USER_FRAME*)comp)->mf->flags &= ~MF_MARKED;
			/* fall thru */
		case OC_OSLFRAME:
		case OC_OLDOSLFRAME:
		case OC_RWFRAME:
		case OC_QBFFRAME:
		case OC_GRFRAME:
		case OC_GBFFRAME:
		case OC_HLPROC:
		case OC_OSLPROC:
		case OC_DBPROC:
			array[i++] = comp; 
		}
	}
	/* sort the array of frames */
	IIUGqsort((char*) array, count, sizeof(APPL_COMP*), 
		IIVQfcsFrameCompSort);
	/* make default form */
	_fd[0].title = ERget(F_VQ00F4_Frame_Name);
	_fd[1].title = ERget(F_VQ00F5_Frame_Type);
	_fd[2].title = ERget(F_VQ00F6_Frame_Desc);
	status = IIFRmdfMakeDefaultForm( frtitle, _selfr_form, TRUE,
			3, _fd, min(15, count), &frame);
	if (status != OK)
	{
		MEfree(array);
		return (VQ_ERRF);
	}
/* # line 2935 "ff.qsc" */	/* addform */
  {
    IIaddform(frame);
  }
/* # line 2937 "ff.qsc" */	/* inittable */
  {
    if (IItbinit(_selfr_form,_selfr_form,(char *)"r") != 0) {
      IItfill();
    } /* IItbinit */
  }
/* # line 2939 "ff.qsc" */	/* host code */
	for (i = 0; i < count; i++)
	{
		obj_name = array[i]->name;
		class_name = OOpclass(array[i]->class)->name;
		obj_desc = array[i]->short_remark;
/* # line 2945 "ff.qsc" */	/* loadtable */
  {
    if (IItbact(_selfr_form,_selfr_form,1) != 0) {
      IItcoputio(_col1,(short *)0,1,32,0,obj_name);
      IItcoputio(_col2,(short *)0,1,32,0,class_name);
      IItcoputio(_col3,(short *)0,1,32,0,obj_desc);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 2951 "ff.qsc" */	/* host code */
	}
	/* run the list pick display loop */
	stat = IIFRrlpRunListPick(_selfr_form, _selfr_form, 
			screen_loc, screen_loc,
			helptit, helpfil, func, (PTR)array);
	/* free the dynamic form */
	IIdelfrm(_selfr_form);
	MEfree(array);
	return (stat);
} 
/*{
** Name:	ff_PF2	- nested call for pasting a frame
**
** Description:
**	Called as a handler function of the list pick code 
**	after the existing frame to be pasted is chosen by the user.
**	(setup is in ff_SelFr).
**
** Inputs:
**	PTR dat;	- pointer to callers data area.. 
**			  really a pointer to an array of components
**			  (see setup function)
**	i4 choice;	- the user's choice
**	bool *resume;	- ptr to caller's resume flag 
**
**		< the global state information >
**
** Outputs:
**
**	Returns:
**		i4 - return status (always VQ_DONE for now)
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	04/10/89 (tom) - created
*/
LOC i4
ff_PF2(dat, choice, resume)
PTR dat;
i4  choice; 
bool *resume;
  {
	APPL_COMP *comp;
	USER_FRAME *parent;
	i4 idx;
	char mnuitm[FE_MAXNAME + 1];
	i4 stat = VQ_DONE;
	*resume = FALSE;
	/* did the user say to forget the operation */
	if (choice < 0)
	{
		return (stat);
	}
	mnuitm[0] = EOS;
	/* first we get the menuitem text, 
	   at which time the user may cancel the operation  */
	if (ff_NewMItem(mnuitm, ERx("vqnewmi.hlp"), _paste_prmpt) == FALSE)
	{
		return (stat);
	}
	/* translate the user's choice into a frame from the array */ 
	comp = ((APPL_COMP**)dat)[choice];
	/* setup the parent and the menuitem index */
	if (_direction == INS_DOWN)
	{
		parent = (USER_FRAME*)ffao;
		idx = 0;
	}
	else
	{
		parent = (USER_FRAME*)ff[ffrow - 1].ao;
		idx = ff[ffrow].disp + ff[ffrow].col;
		if (_direction == INS_RIGHT)
			idx++;
	}
	/* must update the parent's menu array, note: we must insert
	   the pasted item into the tree and then do a loop check to
	   make sure that there is not an error.. 
	   if there is an error we take the pasted object out */
	if (ff_Add2Menu(parent, idx, mnuitm, (USER_FRAME*)comp) == FALSE)
	{
		/* if there is an allocation error then
		   ff_Add2Menu will be processing it */
		return (stat);
	}
	/* check for loops, starting at the top of the tree */
	if (ff_LoopCheck(ff_table[0].ao) == FALSE)
	{
		/* output an error message, message used depends
		   upon whether or not the error occurs on the 
		   frame we were trying to paste in, or one of it's children */
		if (_looperr == (OO_OBJECT*)comp)
		{
			IIVQer1(E_VQ006F_Paste_Err1, 
					_looperr->name);
		}
		else
		{
			IIVQer2(E_VQ0070_Paste_Err2, 
					comp->name, _looperr->name);
		}
		/* remove the item cause there was an error */
		ff_DelMenu(parent, idx);
		*resume = TRUE;
		return (stat);
	}
	/* ...and the now changed parent frame */
	if (ff_UpdDepends(parent->mf,TRUE) != OK)
	{
		/* errors will already be reported */
		return (stat);
	}
	/* position on the new frame */
	if (_direction == INS_DOWN)
	{
		/* this call will insure scroll if necessary */
		ff_CheckBelow(TRUE);
		ff_GoDown();
	}
	else if (_direction == INS_RIGHT)
	{
		ff_GoRight();
	}
	else
	{
		ffao = ff[ffrow].ao = (OO_OBJECT*)comp;
	}
	return (stat);
  }
/*{
** Name:	IIVQcfCopyFrame	- nested call for copying a frame
**
** Description:
**	Called as a handler function of the list pick code 
**	after the existing frame to be copied is chosen by the user.
**	(setup is in ff_SelFr).
**
** Inputs:
**	PTR dat;	- pointer to callers data area.. 
**			  really a pointer to an array of components
**			  (see setup function)
**	i4 choice;	- the user's choice
**	bool *resume;	- ptr to caller's resume flag 
**
**		< the global state information >
**
** Outputs:
**
**	Returns:
**		i4 - always VQ_DONE for now
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	04/10/89 (tom) - created
**	12-dec-92 (blaise)
**		Changed name from ff_CF2 to IIVQcfCopyFrame, and made public.
**	27-may-93 (blaise)
**	        Add the ability to duplicate the form as well.
**	25-feb-1998 (kitch01)
**		Bug 88725. During duplication of a frame in vision local procedures are
**		incorrectly copied to local variables. Amend IIVQcfCopyFrame to copy the 
**		old vartype to the new MFVAR structure.
*/
i4
IIVQcfCopyFrame(dat, choice, resume)
PTR dat;
i4  choice; 
bool *resume;
  {
	register i4  i;
	register i4  j;
	bool do_col_info;
	APPL_COMP *comp;
	METAFRAME *omf;
	METAFRAME *nmf;
	MFTAB	*otab;
	MFTAB	*ntab;
	MFCOL	*ocol;
	MFCOL	*ncol;
	MFESC	*oesc;
	MFESC	*nesc;
	MFVAR	*ovar;
	MFVAR	*nvar;
	i4 frame_mask;
	i4 stat = VQ_DONE;
	i4	dupform;	/* response to "duplicate the form?" popup */
	*resume = FALSE;
	/* did the user say to forget the operation */
	if (choice < 0)
	{
		return (stat);
	}
	/* translate the user's choice into a frame from the array */ 
	comp = ((APPL_COMP**)dat)[choice];
	omf = ((USER_FRAME*)comp)->mf; 
	/* find out whether the user wants to duplicate the form */
	dupform = IIUFccConfirmChoice(CONF_GENERIC, comp->name,
		ERget(FE_Frame), ERget(F_VQ0170_DupformTitle),
		ERx("vqdupfrm.hlp"), F_VQ0171_DupformPrmpt,
		F_VQ0172_DupformYes, F_VQ0173_DupformNo, ERx(""), TRUE);
	/* did the user hit Cancel */
	if (dupform == CONFCH_CANCEL)
	{
		return (stat);
	}
	/* create the new frame */
	if (ff_MakeNew(_copymnu, _copyrem) == FALSE)
	{
		return (stat);
	}
	/* get ptr to the new metaframe */
	nmf = ((USER_FRAME*)ffao)->mf;
	/* allocate the arrays which the metaframe points to, since this
	   is a new creation */
	if (IIAMmnaMetaNewArray(nmf, MF_P_VQ | MF_P_ESC | MF_P_FLD) != OK)
	{
		return (VQ_ERRM);
	}
	/* frame mask so we can cull out escape code types which are not
	   relevent to the new frame */
	frame_mask = 1 << IIVQotObjectType((OO_OBJECT*)comp); 
	/* populate all relevent portions of the metaframe on the frame
	   to be copied. */
	if (IIAMmpMetaPopulate(omf, MF_P_VQ | MF_P_ESC | MF_P_FLD) != OK)
	{
		return (stat);
	}
	/* if only one of the frames is an append frame then we surpress the
	   transfer of column info */
	if (omf->apobj->class == OC_APPFRAME ^ nmf->apobj->class == OC_APPFRAME)
	{
		do_col_info = FALSE;
	}
	else
	{
		do_col_info = TRUE; 
	}
	/* for all the tables in the old, copy to the new */
	for (i = 0; i < omf->numtabs; i++)
	{
		/* allocate a new table struct */
		ntab = nmf->tabs[i] = 
			(MFTAB*) IIVQ_VAlloc(sizeof(MFTAB), (char*)NULL);
		otab = omf->tabs[i];
		/* copy the bulk of the table definition */
		MEcopy((PTR) otab, sizeof(MFTAB), (PTR) ntab);
		/* change the usage type of the non-lookup tables */
		if (otab->usage != TAB_LOOKUP)
		{
			ntab->usage = frm_info[_type].tabtype;
		}
		/* if it's not an update table then we get rid of the
		   flags associated with update tables, note that
		   different table types have some overlaping defintions
		   for some of these bits so we mustn't do this for
		   lookup tables.. but the are not having their type 
		   change anyway */
		if (  ntab->usage != TAB_UPDATE 
		   && ntab->usage != TAB_LOOKUP
		   )
		{
			ntab->flags &= ~( TAB_INSFLG 
					| TAB_DELFLG
					| TAB_UPDRULE
					| TAB_DELRULE
					);
		}
		/* if the new one is update, and the old one was not..
		   then we will impose our defaults */
		if (  ntab->usage == TAB_UPDATE 
		   && otab->usage != TAB_UPDATE
		   )
		{
			ntab->flags |= TAB_DELFLG;
			/* inserts are also allowed for the detail section */
			if (ntab->tabsect != TAB_MASTER)
			{
				ntab->flags |= TAB_INSFLG;
			}
		}
		/* make new copy of the table name */
		ntab->name = (char*) IIVQ_VAlloc(0, otab->name);
		/* allocate the array of column pointers */
		ntab->cols = (MFCOL**) IIVQ_VAlloc( 
			sizeof(MFCOL*) * otab->numcols, (char*)NULL); 
		/* now copy the column info for this table */
		for (j = 0; j < otab->numcols; j++)
		{
			/* allocate the new column struct */
			ncol = ntab->cols[j] = (MFCOL*) IIVQ_VAlloc(
					sizeof(MFCOL), (char*)NULL);
			ocol = otab->cols[j];
			/* copy each member */
			ncol->name = (char*) IIVQ_VAlloc(0, ocol->name);
			ncol->alias = (char*) IIVQ_VAlloc(0, ocol->alias);
			MEcopy((PTR) &ocol->type, sizeof(DB_DATA_VALUE), 
					(PTR) &ncol->type); 
			ncol->corder = ocol->corder;
			ncol->flags = ocol->flags;
			/* depending on the table usage we have to adjust
			   the flags a little */
			switch (ntab->usage)
			{
			/* append tables do not have sort order issues */
			case TAB_APPEND:
				ncol->flags &= ~COL_DESCEND;
				ncol->corder = 0;
				break;
			/* update/browse tables do not have sequence issues */
			case TAB_UPDATE:
			case TAB_DISPLAY:
				ncol->flags &= ~COL_SEQUENCE;
				break;
			case TAB_LOOKUP:
				break;
			}
			if (do_col_info || ntab->usage == TAB_LOOKUP)
			{
				ncol->info = (char*) IIVQ_VAlloc(0, ocol->info);
			}
			else
			{
				ncol->info = _nullstring; 
			}
		}
	}
	nmf->numtabs = i;
	/* for all the joins in the old, copy to the new */
	for (i = 0; i < omf->numjoins; i++)
	{
		/* allocate the new column struct */
		nmf->joins[i] = (MFJOIN*) IIVQ_VAlloc(
			sizeof(MFJOIN), (char*)NULL);
		MEcopy((PTR)omf->joins[i], sizeof(MFJOIN), 
				(PTR)nmf->joins[i]);
	}
	nmf->numjoins = i;
	/* for all the escapes in the old, copy to the new */
	for (j = i = 0; i < omf->numescs; i++)
	{
		oesc = omf->escs[i];
		/* see if the escape code is compatible with the
		   new frame if it isn't we just eliminate it */
		if ((vqescs[oesc->type].frame_mask & frame_mask) == 0) 
		{
			continue;
		}
		/* allocate the new escape struct */
		nesc = nmf->escs[j++] = (MFESC*) IIVQ_VAlloc(
			sizeof(MFESC), (char*)NULL);
		nesc->type = oesc->type;
		nesc->tabsect = oesc->tabsect;
		nesc->item = (char*) IIVQ_VAlloc(0, oesc->item);
		nesc->text = (char*) IIVQ_VAlloc(0, oesc->text);
	}
	nmf->numescs = j;
	/* for all the local vars in the old, copy to the new */
	for (i = 0; i < omf->numvars; i++)
	{
		ovar = omf->vars[i];
		/* allocate the new escape struct */
		nvar = nmf->vars[i] = (MFVAR*) IIVQ_VAlloc(
			sizeof(MFVAR), (char*)NULL);
		nvar->name = (char*) IIVQ_VAlloc(0, ovar->name);
		/* Bug 88725. Copy vartype as well to ensure local procedures are copied correctly */
		nvar->vartype = ovar->vartype;
		nvar->dtype = (char*) IIVQ_VAlloc(0, ovar->dtype);
		nvar->comment = (char*) IIVQ_VAlloc(0, ovar->comment);
	}
	nmf->numvars = omf->numvars;
	nmf->flags = omf->flags;
	nmf->mode = omf->mode;
	/* we have copied everything free the old frame's metaframe memory */
	_VOID_ IIAMmfMetaFree(omf, MF_P_VQ | MF_P_ESC | MF_P_FLD);
	/* we must say that they are populated so that the information
	   will be saved at the proper time. */
	nmf->popmask |= MF_P_VQ | MF_P_ESC | MF_P_FLD;
	nmf->updmask |= MF_P_VQ | MF_P_ESC | MF_P_FLD;
	/* Duplicate the form as well, if the user wanted to do this */
	if (dupform == CONFCH_YES)
	{
		FRAME	*frm;
		char	*newname;
		/* display a message to say what we're doing */
		IIUGmsg(ERget(S_VQ00F7_DupForm), FALSE, 0);
		/* check whether the existing frame has a form */
		if ((frm = FDfrcreate(((USER_FRAME *)comp)->form->name))
			== NULL)
		{
			IIUGerr(E_VQ00F6_NoForm, UG_ERR_ERROR, 1, comp->name);
		}
		else
		{
			/*
			** Build a new form identical to the existing one;
			** replace the old name with the new name, and write
			** the new form back to the system catalogs
			*/
			newname = ((USER_FRAME *)ffao)->form->name;
			STcopy(newname, frm->frname);
			IIFGscfSaveCompForm(nmf->apobj, frm);
			/*
			** set formgendate equal to gendate, so vision knows
			** there's a form for this frame.
			*/
			nmf->formgendate = (char *)IIVQ_VAlloc(0, nmf->gendate);
		}
	}
	/* allow the user to modify the visual query.. */
	stat = ff_EditVq(ffao, /*new*/TRUE);
	/* now we have to manually unpopulate */
	nmf->popmask &= ~(MF_P_VQ | MF_P_ESC | MF_P_FLD);
	_VOID_ IIABucvUpdCompVision(IIVQappl, ffao, IIVQcontext); 
	return (stat);
  }
/*{
** Name:	ff_LoopCheck	- check for loops in the app tree
**
** Description:
**	This routine does a recursive descent of the application tree
**	marking nodes as it goes.. if it encounters a marked node, then
**	it knows that it has encountered a loop in the tree.
**
** Inputs:
**	OO_OBJECT *obj;	- currently considered node of the tree
**
** Outputs:
**	Returns:
**		bool	- TRUE no loops found in the subtree..
**			  FALSE a loop was found
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/31/89 (tom) - created
*/
LOC bool
ff_LoopCheck(obj)
OO_OBJECT *obj;
{
	METAFRAME *mf;
	i4 i;
	switch (obj->class)
	{
	case OC_MUFRAME:
	case OC_APPFRAME:
	case OC_UPDFRAME:
	case OC_BRWFRAME:
		mf = ((USER_FRAME*)obj)->mf;
		/* if we have visited this frame before in our travels
		   then we have found a loop.. so return error */
		if (mf->flags & MF_MARKED)
		{
			_looperr = obj; /* remember the frame in error */
			return (FALSE);
		}
		/* first mark this frame as having been visited */
		mf->flags |= MF_MARKED; 
		/* now visit all children of this frame */
		for (i = 0; i < mf->nummenus; i++)
		{
			/* if any descendents have problems.. then we
			   just return the error  */
			if (ff_LoopCheck(mf->menus[i]->apobj) == FALSE)
			{
				/* unmark.. so that we will still be
				   able to use the flags */
				mf->flags &= ~MF_MARKED;
				return (FALSE);
			}
		}
		/* unmark this frame cause we have scanned 
		   all it's children */
		mf->flags &= ~MF_MARKED;
	}
	return (TRUE);
}
/*{
** Name:	ff_Destroy	- destroy a frame from the frame flow diagram
**
** Description:
**	This routine supervises the removal of a new frame from 
**	the frame flow diagram.
**
** Inputs:
**		< the global state information >
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/31/89 (tom) - created
*/
LOC VOID
ff_Destroy()
{
	bool didprompt;
	/* if we are at the top of the tree.. we disallow destroy */
	if (ff_AtTop())
	{
		IIVQer1(E_VQ0046_Remove_at_top, ERget(FE_Destroy));
		return;
	}
	/* only cut if there is another reference in the tree.. else destroy */
	if (ff_RefFound(ff[ffrow - 1].ao, 
		ff[ffrow].disp + ff[ffrow].col, ffao))
	{
		IIVQer1(E_VQ006D_Destroy_RefFound, ffao->name);
		ff_Cut();
		return;
	}
	if (IIABcdlCheckDestroyLock(IIVQappl, ffao, TRUE, TRUE, &didprompt) 
			!= OK)
	{
		return;
	}
	if ( ! didprompt &&
		IIUFccConfirmChoice(CONF_DESTROY, ffao->name, ERget(FE_Frame),
		ERget(F_VQ0088_DelfrTitle), ERx("vqdelfr.hlp")) == CONFCH_NO)
	{
		return;
	}
	/* put out user information message */
	IIUGmsg(ERget(S_VQ000B_Destroy_Frm_Msg), FALSE, 1, ffao->name); 
	/* delete the ii_menuargs entries corresponding to this frame */
	if ( ff_Delmenuargs() != OK ) return;
	/* destroy the frame, this is assumed to fixup 
	   the parent's menu structures */
	IIABdcvDestroyCompVision(IIVQappl, ffao, IIVQcontext);
	/* !!! at this point we would like to recurse the tree to destroy all 
	   frames which are not referenced elsewhere
	*/
	/* fix up state info structures */
	ff_DelFixup();
}
/*{
** Name:	ff_RefFound	- look for a 2nd reference to a frame.
**
** Description:
**	This routine searches for references to a particular frame (excluding
**	a reference from a known parent.
**
** Inputs:
**	OO_OBJECT *parent;	- parent
**	i4 idx;		- index that the child is located at
**	OO_OBJECT *child;	- child to be checked 
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	08/20/89 (tom) - created
*/
LOC bool 
ff_RefFound(parent, idx, child)
OO_OBJECT *parent;
i4  idx;
OO_OBJECT *child;
{
	register APPL_COMP *comp;
	register METAFRAME *mf;
	register i4  i;
	/* go through the application components and count them */
	for (	comp = (APPL_COMP*)(IIVQappl->comps);
		comp != (APPL_COMP*)NULL; 
		comp = comp->_next
	    )
	{
		switch (comp->class)
		{
		case OC_MUFRAME: 
		case OC_APPFRAME: 
		case OC_UPDFRAME:
		case OC_BRWFRAME: 
			mf = ((USER_FRAME*)(comp))->mf;
			/* scan the call frame references of the child */
			i = mf->nummenus;
			while (i--) 
			{
				/* ignore reference to the known parent,
				   at given index */
				if (	comp == (APPL_COMP*)parent
				   &&   idx == i 
				   )
				{
					continue;
				}
				/* if found then return.. */
				if (mf->menus[i]->apobj == child)
				{
					return (TRUE);
				}
			}
		}
	}
	return (FALSE); 
}
/*{
** Name:	ff_Cut	- cut a frame from the frame flow diagram
**
** Description:
**	This routine supervises the removal of a new frame from 
**	the frame flow diagram. This is not a destroy, merely a
**	cut from the diagram. The frame can be pasted in latter.
**
** Inputs:
**		< the global state information >
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/31/89 (tom) - created
*/
LOC VOID
ff_Cut()
{
	USER_FRAME *parent;
	bool didprompt;
	/* if we are at the top of the tree.. we disallow cut */
	if (ff_AtTop())
	{
		IIVQer1(E_VQ0046_Remove_at_top, ERget(FE_Cut));
		return;
	}
	if (IIABcdlCheckDestroyLock(IIVQappl, ffao, TRUE, FALSE, &didprompt) 
			!= OK)
	{
		return;
	}
	if ( ! didprompt &&
		IIUFccConfirmChoice(CONF_REMOVE, ffao->name, ERget(FE_Frame),
		ERget(F_VQ0089_RemfrTitle), ERx("vqremfr.hlp")) == CONFCH_NO)
	{
		return;
	}
	/* put out user information message */
	IIUGmsg(ERget(S_VQ000C_Cut_Frm_Msg), FALSE, 1, ffao->name); 
	/* delete the ii_menuargs entries corresponding to this frame */
	if ( ff_Delmenuargs() != OK ) return;
	/* delete the menu struct from the parent */
	ff_DelMenu(parent = (USER_FRAME*)ff[ffrow - 1].ao, 
		ff[ffrow].disp + ff[ffrow].col);
	/* ...update the parent frame table definitions */
	if (ff_UpdDepends(parent->mf,TRUE) != OK)
	{
		/* error is already reported */
	}
	/* fix up state info structures */
	ff_DelFixup();
}
/*{
** Name:	ff_Delmenuargs	- delete from the ii_menuargs
**
** Description:
**	This routine removes the ii_menuargs entries corresponding to
**	the removed/destroyed frame
**
** Inputs:
**		< the global state information >
**
** Outputs:
**	Returns:
**		STATUE
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	11/15/93 (connie) - created
*/
LOC STATUS
ff_Delmenuargs()
{
METAFRAME *mf;
mf = ((USER_FRAME*)ff[ffrow - 1].ao)->mf;
return(IIAMmtdMenuTextDel(mf->apobj->ooid,
	mf->menus[ff[ffrow].disp + ff[ffrow].col]->text));
}
/*{
** Name:	ff_DelFixup	- fix up the state info after a cut/del
**
** Description:
**	This routine is called during a destroy or cut to fixup 
**	the state information.
**
** Inputs:
**		< the global state information >
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	08/18/89 (tom) - created
*/
LOC VOID
ff_DelFixup()
{
	register METAFRAME *parent;
	parent = ((USER_FRAME*)ff[ffrow - 1].ao)->mf;
	/* if there was only one menu item for the parent, we must 
	   effectively move up.. note that this routine assumes
	   that the parent frame's nummenus has already been decremented
	   to account for the destroy or cut we are doing */
	if (parent->nummenus == 0)
	{
		/* do we need to scroll up */
		if (ffrow == 1 && ff != ff_table)
		{
			ff--;
		}
		else	/* otherwise just move up */
		{
			ffrow--;
		}
	}	
	/* else if we there are some frames scrolled off to the right.. */
	else if (ff[ffrow].disp + IIVQcolmax < parent->nummenus + 1)
	{
		/* they will just naturally move over so we do nothing */
	}
	/* else if there are some scrolled off to the left.. */
	else if (ff[ffrow].disp)
	{
		/* then we scroll them over */
		ff[ffrow].disp--;		
	}
	else if (ff[ffrow].col)
	{
		/* else just move left */
		ff[ffrow].col--;
	}
	/* set the current ffao ptr */
	ff_SetAo();
	/* null out all lines below us */
	ff_ZapBelow();
	/* expand children below us if necessary */
	ff_CheckBelow(FALSE);
}
/*{
** Name:	ff_Edit	- edit a frame 
**
** Description:
**	This routine supervises the edit of frame. This is done
**	by constructing a list (based on the frame type, and
**	other criteria) via a table which defines this edit
**	context. Once this is done, we call list pick with 
**	the list and finish the edit request in the handler function. 
**
** Inputs:
**		< the global state information >
**
** Outputs:
**	Returns:
**		i4	VQ_xxxx  values which may indicate
**			that quiting is desired.
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/31/89 (tom) - created
*/
LOC i4  
ff_Edit()
{
	i4 type;
	char buf[500];
	register i4  i;
	i4 num;
	i4 typetab[VQMAXEOPS];
	type = IIVQotObjectType(ffao);
	buf[0] = EOS;
	/* loop through the table for this edit menu and 
	   construct the menu to be presented to the user */
	for (num = i = 0; num < VQMAXEOPS && ff_edinfo[i].type != 0; i++)
	{
		/* see if the flag that this edit option requires
		   are present in the frame info, if not pass on this one */ 
		if ((frm_info[type].flags & ff_edinfo[i].flags) == NULL)
		{
			continue;
		}
		switch (ff_edinfo[i].type)
		{
		/* menuitems and parameters don't make sense if we
		   are on the top frame */
		case EOP_FRPARM:
			/* QBF and GRAPH frames do not take parameters */ 
			if (type == MF_QBF || type == MF_GRAPH) 
			{
				continue;
			}
			/* else fall thru */
		case EOP_MNUTXT:
			if (ff_AtTop() == TRUE)
			{
				continue;
			}
			break;
		/* only edit the forms of qbf or report frames if they exist */
		case EOP_GENFRM:
			if (type == MF_REPORT)
			{
				if ( ((REPORT_FRAME*)ffao)->form 
					== (FORM_REF*)NULL)
				{
					continue;
				}
			}
			else if (type == MF_QBF)
			{
				if ( ((QBF_FRAME*)ffao)->form 
					== (FORM_REF*)NULL)
				{
					continue;
				}
			}
			break;
		/* only allow edit of code if they have the capability,
		   note that report frames can be edited, assuming that
		   there is a source file */
		case EOP_SRCFIL:
			if (type == MF_REPORT)
			{
				if (	((REPORT_FRAME*)ffao)->source 
						== (char*)NULL
				   ||	((REPORT_FRAME*)ffao)->source[0]
						== EOS
				   )
				{
					continue;
				}
			}
			else if (IIabAbf == FALSE)
			{
				continue;
			}
			break;
		/* if the report has a source file.. then it doesn't
		   have an RBF definition */
		case EOP_RBFDEF:
			if (	((REPORT_FRAME*)ffao)->source 
					!= (char*)NULL
			   &&	((REPORT_FRAME*)ffao)->source[0]
					!= EOS
			   )
			{
				continue;
			}
			break;
		}
		if (num) /* if it's not the first item put in seperator */
		{
			STcat(buf, ERx("\n"));
		}
		STcat(buf, ERget(ff_edinfo[i].mitem));
		typetab[num++] = ff_edinfo[i].type;
	}
	return (IIFDlpListPick(_ffed_tit, buf, num, 
		3, 3, _ffed_tit, ERx("vqedmen.hlp"),
		ff_ED2, (PTR)typetab));
}
/*{
** Name:	ff_ED2	- edit a frame, handler function 
**
** Description:
**	This routine supervises the edit of frame. 
**	This is a handler function of the list pick code,
**	which call is set up in ff_Edit.
**	
** Inputs:
**	PTR dat;	- really points to a table of nat's which
**			  is used to translate the user's choice into
**			  the type of edit desired.
**	i4 choice;	- the choice from the user 
**	bool *resume;	- ptr to list pick's resume flag 
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/31/89 (tom) - created
**	10/90 (Mike S) - Refresh application data before editing errors
**	11-aug-92 (blaise)
**		Added option to edit local procedures.
*/
LOC i4  
ff_ED2(dat, choice, resume)
PTR dat;
i4  choice;
bool *resume;
{
	i4 *typetab = (i4*) dat;
	i4 stat = VQ_DONE;
	QBF_FRAME *qbf;
	*resume = FALSE;
	/* did the user actually select something */
	if (choice >= 0)
	{
		switch (typetab[choice])
		{
		case EOP_VISQRY:			/* visual query */
			if (ff_Elock(ffao, FALSE))
			{
				break;
			}
			if (ff_TouchCheck(FE_Edit))
			{
				break;
			}
			stat = ff_EditVq(ffao, /*not new*/FALSE);
			break;
		case EOP_GENFRM:			/* generated form */
			if (ff_Elock(ffao, FALSE))
			{
				break;
			}
			IIVQegfEditGenForm(ffao);
			break;
		case EOP_MNUTXT:			/* menu text */
			if (ff_Elock(ff[ffrow - 1].ao, FALSE))
			{
				break;
			}
			if (ff_TouchParentCheck(FE_Edit))
			{
				break;
			}
			ff_EditMenu();
			break;
		case EOP_FRPARM:			/* frame parameters */
			if (ff_Elock(ff[ffrow - 1].ao, FALSE))
			{
				break;
			}
			if (ff_TouchParentCheck(FE_Edit))
			{
				break;
			}
			IIVQparams(((USER_FRAME *)ff[ffrow - 1].ao)->mf,
				ff[ffrow].disp + ff[ffrow].col);
			break;
		case EOP_LOCVAR:			/* local variables */
			if (ff_Elock(ffao, FALSE))
			{
				break;
			}
			if (ff_TouchCheck(FE_Edit))
			{
				break;
			}
			IIVQlocals(ffao);
			break;
		case EOP_LOCPRC:			/* local procedures */
			if (ff_Elock(ffao, FALSE))
			{
				break;
			}
			if (ff_TouchCheck(FE_Edit))
			{
				break;
			}
			IIVQlprocs(ffao);
			break;
		case EOP_GLOBOB:			/* global objects */
			IIVQgeGlobalEdit(IIVQappl, FALSE);
			break;
		case EOP_ESCCOD:			/* escape code */
			if (ff_Elock(ffao, FALSE))
			{
				break;
			}
			if (ff_TouchCheck(FE_Edit))
			{
				break;
			}
			_VOID_ IIVQefeEditFrameEscapes(((USER_FRAME*)ffao)->mf);
			break;
		case EOP_SRCFIL:			/* source code file */
			if (ff_Elock(ffao, FALSE))
			{
				break;
			}
			if (ffao->class == OC_RWFRAME)
			{
				iiabFileEdit(IIVQappl, (APPL_COMP*)NULL,
					((REPORT_FRAME*)ffao)->source); 
			}
			else
			{
				IIVQegcEditGenCode(ffao);
			}
			break;
		case EOP_ERRORS:			/* errors */
			if (IIAMapFetch(IIVQappl, OC_UNDEFINED, TRUE) == OK)
			{
				_VOID_ IIVQdceDisplayCompErrs(IIVQappl, ffao, 
							      FALSE);
			}
			break;
		case EOP_QBFDEF:			/* run qbf, or tables */
			qbf = (QBF_FRAME*) ffao;
			if (ff_Elock(ffao, FALSE))
			{
				break;
			}
			if (qbf->qry_type == OC_JOINDEF)
			{
				abexeprog(ERx("qbf"), ERx("editjoindef = %S"),
					1, qbf->qry_object);
			}
			else
			{
				abexeprog(ERx("tables"), _nullstring, 0);
			}
			break;
		case EOP_RBFDEF:			/* run rbf */
			if (ff_Elock(ffao, FALSE))
			{
				break;
			}
			abexeprog(ERx("rbf"),  
				ERx("rpt_object = %S, edit_restrict = %S"),
				2, ((REPORT_FRAME*)ffao)->report, 
				_nullstring);
			break;
		case EOP_GRFDEF:			/* run vigraph */
			if (ff_Elock(ffao, FALSE))
			{
				break;
			}
			abexeprog(ERx("vigraph"), ERx("graph = %S"),
				1, ((GRAPH_FRAME*)ffao)->graph);
			break;
		}
	}
	return (stat);
}
/*{
** Name:	ff_EditMenu	- to edit the menuitem of the current frame
**
** Description:
**	Edit the menuitem text for the current frame.
**	We will also update the dependencies of the parent frame.
**
** Inputs:
**
** Outputs:
**	Returns:
**		VOID
**	Exceptions:
**
** Side Effects:
**
** History:
**	06/16/89 (tom) - created
*/
LOC VOID 
ff_EditMenu()
{
	char mnuitm[FE_MAXNAME + 1];
	METAFRAME *parent;
	char *oldtext, *newtext;
	char **mtxtp;
	/* get a pointer to the parent's metaframe,
	   NOTE: the parent must be a generatable frame or else
	   we couldn't be here */
	parent = ((USER_FRAME *)ff[ffrow - 1].ao)->mf;
	mtxtp = &parent->menus[ff[ffrow].disp + ff[ffrow].col]->text;
	oldtext = *mtxtp;
	STcopy(oldtext, mnuitm);
	/* just say that the insertion direction is right.. cause
	   check routine has to figure out what the parent frame is
	   and it uses the insertion direction to do it */
	_direction = INS_FALSE; 
	if (ff_NewMItem(mnuitm, ERx("vqedmnu.hlp"), _mnuedt_tit) == FALSE)
	{
		return;
	} 
	/* if they didn't really change it.. then just return */
	if (STcompare(oldtext, mnuitm) == 0)
	{
		return;
	}
	/* since the menuitem text is being changed.. we must
	   rename the escape code */
	_VOID_ IIVQrmeRenameMenuEscapes(parent, oldtext, mnuitm);
	/* update the parent's menu list */
	*mtxtp = newtext = (char*)IIVQ_MAlloc(0, mnuitm); 
	/* update menu item text in catalogs */
	_VOID_ IIAMmtcMenuTextChange((parent->apobj)->ooid, oldtext, newtext);
	/* update the parent's flags, etc */
	_VOID_ ff_UpdDepends(parent, FALSE);
}
/*{
** Name:	ff_MakeVq	- make a new visual query type frame 
**
** Description:
**	Create a new visual query frame by 
**	adding the tables which are passed as arguments.
**
**
** Inputs:
**	char *master;	- name of the master table
**	char *detail;	- name of the detail table
**				These buffers must be FE_MAXTABNAME+1..
** Outputs:
**	Returns:
**		VQ_DONE for SUCCESS
**		VQ_ERRM for ERROR  
**              FALSE   if fails to add table to frame
**	Exceptions:
**
** Side Effects:
**
** History:
**	06/16/89 (tom) - created
*/
LOC i4  
ff_MakeVq(master, detail, m_in_tf)
char *master;
char *detail;
char *m_in_tf;
{
	METAFRAME *mf;
	i4 stat = VQ_DONE;
	mf = ((USER_FRAME*)ffao)->mf;
	if (IIAMmnaMetaNewArray(mf, MF_P_VQ) != OK)
	{
		return (VQ_ERRM);
	}
	/* add master table.. must be present */
	if (IIVQantAddNewTable( mf, master, 0,
		frm_info[_type].tabtype, TAB_MASTER) == FALSE)
	{
		/* error has already been reported, only allocation
		   errors are relevent */
		return (FALSE);
	}
	if (detail[0] != EOS)
	{
		if (IIVQantAddNewTable(mf, detail, 1,
			frm_info[_type].tabtype, TAB_DETAIL) == FALSE)
		{
			/* error has already been reported, only allocation
			   errors are relevent */
			return (FALSE);
		}
		/* attempt to insert as many default joins as possible..
			(last arg == FALSE means not just one) */
		IIVQmdjMakeDefaultJoin(mf, 0, 1);
		mf->mode = MF_MASDET;
	}
	else
	{
		if (IIUGyn(m_in_tf, (STATUS*)NULL) == TRUE)
		{
			/* Masters in a tablefield has no 'Next' menuitem */
			mf->mode = MF_MASTAB|MF_NONEXT;
		}
		else
		{
			mf->mode = MF_MASSIM;
		}
	}
	/* allow the user to modify the visual query.. */
	stat = ff_EditVq(ffao, /*new*/TRUE);
	_VOID_ IIABucvUpdCompVision(IIVQappl, ffao, IIVQcontext); 
	return (stat);
}
/*{
** Name:	ff_CheckRel	- check for existance of a relation
**
** Description:
**	Function to check for the existance of a relation.
**	Also has the side effect of setting the case of the
**	name to the current gateway convention.
**
** Inputs:
**	char *name;	- name of relation to check for
**
** Outputs:
**	Returns:
**		STATUS	- TRUE for success
**			  FALSE for failure
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/31/89 (tom) - created
**	22-feb-1994 (daver)
**		Normalized the table name; added the owner part if the
**		table does exist, its not owned by the current user, and 
**		we're talking to a 6.5 or greater database. Fixes DB2
**		gateway bugs 58834 and 59546.
*/
LOC STATUS 
ff_CheckRel(name)
char *name;
{
	/* for 6.5 ["]owner["].["]tablename["] support */
	FE_RSLV_NAME    resolved_name;
	char    owner_part[FE_MAXNAME + 1];
	char    table_part[FE_MAXNAME + 1];
	bool	it_exists;
	resolved_name.name = name;
	resolved_name.owner_dest = owner_part;
	resolved_name.name_dest = table_part;
	resolved_name.is_nrml = FALSE;
	it_exists = FE_fullresolve(&resolved_name);
	if (it_exists)
	{
		/*
		** We don't need to be concerned with the owner name if
		** there isn't one (!), or if the table owner is the 
		** session user, or if we're running against a 6.4
		** database.
		*/
		if ( (resolved_name.owner_spec == FALSE) ||
		     (STcompare(IIUIcsl_CommonSQLLevel(),UI_LEVEL_65) < 0) ||
		     (STequal(owner_part, IIUIdbdata()->suser)) )
		{
			STcopy(resolved_name.name_dest, name);
		}
		else
		{
			STpolycat(3, owner_part, ".", table_part, name);
		}
	}
	return (it_exists);
}
/*{
** Name:	ff_EditVq	- Edit the frame's Visual Query
**
** Description:
**	This routine supervises the modification of a frame's 
**	Visual Query. Handles populating the VQ defintion part
**	of the metaframe, and updating after the edit.
**
** Inputs:
**	OO_OBJECT *ao;	- pointer to application object to edit
**	bool new;	- flag to say if we just created this visual
**			  query.
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/31/89 (tom) - created
*/
LOC i4
ff_EditVq(ao, new)
OO_OBJECT *ao;
bool new;
{
	METAFRAME *mf;
	i4 stat;
	mf = ((USER_FRAME*)ao)->mf;
	/* first populate the visual query portion of the metaframe,
	   if it's not a new creation */
	if (new == FALSE)
	{
		if (IIAMmpMetaPopulate(mf, MF_P_VQ) != OK)
		{
			return (VQ_DONE);
		}
	}
	else 	/* make sure that everyone knows it's populated. */
	{
		mf->popmask |= MF_P_VQ;
	}
	/* run the visual query display loop */
	stat = IIVQvdlVqDisplayLoop(mf, new);
	/* free all VQ structure stuff */
	if (new == FALSE)
	{
		IIAMmfMetaFree(mf, mf->popmask);
	}
	else
	{
		mf->popmask &= ~MF_P_VQ;
	}
	IIVQfvtFreeVqTag();
	return (stat);
}
/*{
** Name:	ff_GoUp	- move up in the frame flow diagram
**
** Description:
**	This routine supervises the movement up in the frame flow 
**	diagram.
**
** Inputs:
**		< the global state information >
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/28/89 (tom) - created
*/
LOC VOID
ff_GoUp()
{
	/* if the current row is greater than one we can
	   just go up.. no scolling necessary. */
	if (ffrow > 1)
	{
		ffao = ff[--ffrow].ao; 
	}
	else if (ffrow == 1)
	{
		/* if we're not at the extreme top... */	
		if (ff != ff_table)
		{
			ff--;		/* then scroll up */
			ffao = ff[ffrow].ao;
		}
		else
		{
			ffrow--;
			ffao = ff[0].ao;
		}
	}
	else
	{
		FTbell();
	}
}
/*{
** Name:	ff_GoDown	- move down in the frame flow diagram
**
** Description:
**	This routine supervises the movement downward in the frame flow 
**	diagram.
**
** Inputs:
**		< the global state information >
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/28/89 (tom) - created
*/
LOC VOID
ff_GoDown()
{
	/* first see if we can go down at all */
	if (IIVQnmNumMenus(ffao))
	{
		/* note: we can safely increment the ffrow variable, 
		   because we know that we would not be  positioned
		   on the bottom row unless we had no children, and
		   because of the test above.. that couldn't be the case */
		ffrow++;
		if (ff[ffrow].ao == (OO_OBJECT*)NULL)
		{
			ff[ffrow].disp = ff[ffrow].col = 0;
			/* set the current ffao ptr */
			ff_SetAo();
			ff[ffrow].ao = ffao;
		}
		else
		{
			ffao = ff[ffrow].ao;
		}
		/* expand children below us if necessary */
		ff_CheckBelow(TRUE);
	}
	else
	{
		FTbell();
	}
}
/*{
** Name:	ff_CheckBelow	- check if we are on the bottom line
**
** Description:
**	We have just been positioned to a new frame, either by going
**	left, right, or down. 
**	First we see if we can expand our children without scrolling,
**	and if necessary we will scroll to expose our children.
**
** Inputs:
**	bool down; - flag to say if we have just moved down
**		< the global state information >
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/28/89 (tom) - created
*/
LOC VOID
ff_CheckBelow(down)
bool down;
{
	ff[ffrow].ao = ffao;
	/* if the current frame has children */
	if (IIVQnmNumMenus(ffao))
	{
		if (!down)
		{
			ff[ffrow + 1].disp = ff[ffrow + 1].col = 0;
		}
		/* if there is not another row that is visible, scroll */
		if (ffrow >= IIVQrowmax - 1)
		{
			ff++;
			ffrow--;
		}
	}
	else
	{
		/* row has no children */
		ff[ffrow + 1].ao = (OO_OBJECT*)NULL;
	}
}
/*{
** Name:	ff_ZapBelow	- null out the lines below us
**
** Description:
**	Because we have moved right or left.. we need to 
**	null out all lines below us, because they are no longer 
**	valid.
**
** Inputs:
**		< the global state information >
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/28/89 (tom) - created
*/
LOC VOID
ff_ZapBelow()
{
	FF_STATE *ffp;	
	for (ffp = &ff[ffrow + 1]; ffp < &ff_table[FF_MAXDEPTH]; ffp++)
	{
		ffp->ao = (OO_OBJECT*)NULL;
		ffp->disp = 0;
		ffp->col = 0;
	}
}
/*{
** Name:	ff_GoLeft	- move left in the frame flow diagram
**
** Description:
**	This routine supervises the movement left in the frame flow 
**	diagram.
**
** Inputs:
**		< the global state information >
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/28/89 (tom) - created
*/
LOC VOID
ff_GoLeft()
{
	/* if we're at the very top of the tree */
	if (ff_AtTop())
	{
		FTbell();
		return;
	}
	if (ff[ffrow].col == 0)
	{
		if (ff[ffrow].disp)
		{
			ff[ffrow].disp--;
		}
		else
		{
			FTbell();
			return;
		}
	}
	else
	{
		ff[ffrow].col--;
	}
	/* set the current ffao ptr */
	ff_SetAo();
	/* null out all lines below us */
	ff_ZapBelow();
	/* expand children below us if necessary */
	ff_CheckBelow(FALSE);
}
/*{
** Name:	ff_GoRight	- move right in the frame flow diagram
**
** Description:
**	This routine supervises the movement right in the frame flow 
**	diagram.
**
** Inputs:
**		< the global state information >
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	03/28/89 (tom) - created
*/
LOC VOID
ff_GoRight()
{
	i4 num;
	/* if we're on the first line */
	if (ff_AtTop())
	{
		FTbell();
		return;
	}
	num = IIVQnmNumMenus(ff[ffrow - 1].ao);
	/* if we're at the end of the line */
	if (ff[ffrow].disp + ff[ffrow].col == num - 1)
	{
		FTbell();
		return;
	}
	if (ff[ffrow].col == IIVQcolmax - 1)
	{
		ff[ffrow].disp++;
	}
	else
	{
		ff[ffrow].col++;
	}
	/* set the current ffao ptr */
	ff_SetAo();
	/* null out all lines below us */
	ff_ZapBelow();
	/* expand children below us if necessary */
	ff_CheckBelow(FALSE);
}
/*
** Name:	ff_SetAo - set the current metaframe struct
**
** Description:
**	This function sets the 'current' metaframe ptr.
**
** Inputs:
**
** Outputs:
**	Returns:
**		none
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	04/13/89 (tom) - created
*/
LOC VOID
ff_SetAo()
{
	if (ff_AtTop())
	{
		ffao = ff[0].ao;
	}
	else
	{
		ffao = ff[ffrow].ao = ((USER_FRAME*)ff[ffrow - 1].ao)->mf
			->menus[ff[ffrow].disp + ff[ffrow].col]->apobj;
	}
}
/*{
** Name:	IIVQnmNumMenus - return # of menuitems for current app. object
**
** Description:
**	This function returns the number of menuitems for the current 
**	application object.
**
** Inputs:
**	OO_OBJECT ao;	- application object
**
** Outputs:
**	Returns:
**		none
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	04/13/89 (tom) - created
*/
i4  
IIVQnmNumMenus(ao)
register OO_OBJECT *ao;
{
	switch(ao->class)
	{
				/* emerald type frames */
	case OC_MUFRAME:
	case OC_APPFRAME:
	case OC_UPDFRAME:
	case OC_BRWFRAME:
		return ((USER_FRAME*)ao)->mf->nummenus;
				/* traditional abf frames */
	default:
		return 0;
	}
}
/*{
** Name:	ff_AtTop - return if we are at the top of the application
**
** Description:
**	Return boolean result of test as to whether we are at the top of 
**	the application tree.
**
** Inputs:
**	none
** Outputs:
**	Returns:
**		bool	- result of test
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	06/16/89 (tom) - created
*/
LOC bool 
ff_AtTop()
{
	return (ffrow == 0 && ff == ff_table);
}
/*{
** Name:	ff_NeedRef - check on need for catalog update
**
** Description:
**	If timeout has occurred, check to see if a concurrent user
**	has updated the frame flow, and possibly refresh application.
**
**	This may be enhanced to use alerters on databases which can provide
**	such a feature.
**
** Inputs:
**	force	- if TRUE, force check.
**
** Outputs:
**	Returns:
**		TRUE if refresh was made.
**
** Side Effects:
**
** History:
**	12/89 (bobm) - created
*/
LOC bool
ff_NeedRef(force)
bool force;
{
	i4 elapsed;
	if (_our_to == 0)
		return FALSE;
	if (! force)
	{
		elapsed = TMsecs() - _lastupd;
	}
	if (force || elapsed > _our_to )
	{
		if (IIABffsFrameFlowState(IIVQappl))
		{
			IIABvrVisionRefresh(IIVQappl, IIVQcontext, ffao);
			_lastupd = TMsecs();
			return TRUE;
		}
		_lastupd = TMsecs();
	}
	return FALSE;
}
/*{
** Name:	ff_Elock - edit lock
**
** Description:
**	Place an edit lock on an object, if we don't already have one.
**
** Inputs:
**	comp - component to lock
**	new - TRUE if brand new component.
**
** Outputs:
**	Returns TRUE if user was interrogated for existing lock, and
**	chose to renege.  If there were DB errors, error messages were
**	printed, but we proceed normally (FALSE returned).
**
** Side Effects:
**	DB update.
**
** History:
**	12/89 (bobm) - created
*/
LOC bool
ff_Elock(comp, new)
OO_OBJECT *comp;
bool new;
{
	i4 i;
	bool rval;
	/*
	** if we already locked it, don't do it again.
	*/
	for (i=0; i < _num_elock; ++i)
	{
		if (_elock[i] == comp->ooid)
		{
			return FALSE;
		}
	}
	/*
	** this AIN'T supposed to happen, but let's not screw things
	** up for the user simply because our lock logic is - just
	** a non-fatal error and continue.
	*/
	if (_num_elock >= 3)
	{
		IIUGerr(E_VQ0099_LockStack, 0, 0);
		return FALSE;
	}
	/*
	** both of these routines lock our component.  IIABcelCheckEditLock
	** checks to see if somebody else is editing it, and returns
	** FAIL if user wants to renege.  A FAIL return also indicates
	** it did not set a lock.  DB Error handling in these routines
	** may result in an OK return with no lock set.  That does
	** no real harm here - we simply record that we have a lock that
	** we don't, and eventually "remove" the non-existent lock, at
	** most generating another DB error.  The abf level generates
	** error messages warning the user that concurrency is not being
	** handled properly for DB errors.
	*/
	if (new)
	{
		IIABnelNewEditLock(comp->ooid);
		rval = FALSE;
	}
	else
	{
		rval = IIABcelCheckEditLock(comp) != OK;
	}
	if (!rval)
	{
		_elock[_num_elock] = comp->ooid;
		++_num_elock;
	}
	return rval;
}
/*{
** Name:	ff_RemELocks - remove edit lock(s)
**
** Description:
**	Remove any edit locks we've accumulated.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Side Effects:
**	DB update.
**
** History:
**	12/89 (bobm) - created
*/
LOC VOID
ff_RemELocks()
{
	i4 i;
	for (i=0; i < _num_elock; ++i)
	{
		IIABrlRemLock(_elock[i]);
	}
	_num_elock = 0;
}
/*{
** Name:	ff_ProcessHotSpot - the user activated a hot spot
**
** Description:
**	Process a user activated hot spot.
**
** Inputs:
**	none
**
** Outputs:
**	none
**
** Side Effects:
**	DB update.
**
** History:
**	1/90  (tom) - created
*/
LOC VOID
ff_ProcessHotSpot()
{
	register i4  i;
	register HOTSPOT *hs;
	i4 col;
	for (hs = IIVQhsaHotSpotArray, i=0; i < IIVQhsiHotSpotIdx; ++i, hs++)
	{
		/* if we don't match on the hotspot.. */
		if (hs->num != _hotspot)
		{
			continue;	/* conserve indentation */
		}
		if (hs->row < ffrow)
		{
			for (i = ffrow - hs->row; i-- > 0; )
			{
				ff_GoUp();
			}
		}
		else
		{
			for (i = hs->row - ffrow; i-- > 0; )
			{
				ff_GoDown();
			}
		}
		/* ok.. now we are on the correct row */
		/* special case for when we want to go above the top */
		if (hs->row == -1)
		{
			break;
		}
		col = ff[ffrow].disp + ff[ffrow].col;
		if (hs->col < col)
		{
			for (i = col - hs->col; i-- > 0; )
			{
				ff_GoLeft();
			}
		}
		else
		{
			for (i = hs->col - col; i-- > 0; )
			{
				ff_GoRight();
			}
		}
		/* ok.. now we are on the correct column */
		/* if user clicked on one of the downward expansions.. */
		if (hs->down)
		{
			ff_GoDown();
		}
		break;
	}
}
/*{
** Name:	ff_TouchCheck - test for custom frame, if so confirm operation
**
** Description:
**	Cover for ff_Touch (for the current frame).
**	
** Inputs:
**	ER_MSGID msgid;
**
** Outputs:
**	none
**
** Side Effects:
**
** History:
**	2/14/90 (tom) - created
*/
LOC bool
ff_TouchCheck(msgid)
ER_MSGID msgid;
{
	return (ff_Touch(ffao, msgid));
}
/*{
** Name:	ff_TouchParentCheck - test for custom frame, if so confirm op.
**
** Description:
**	Cover for ff_Touch (for the parent frame).
**	
** Inputs:
**	ER_MSGID msgid;
**
** Outputs:
**	none
**
** Side Effects:
**
** History:
**	2/14/90 (tom) - created
*/
LOC bool
ff_TouchParentCheck(msgid)
ER_MSGID msgid;
{
	if (ff_AtTop())
	{
		return (FALSE);
	}
	return (ff_Touch(ff[ffrow - 1].ao, msgid));
}
/*{
** Name:	ff_Touch - test for custom frame, if so confirm operation
**
** Description:
**	We check the given object and if it is a customized emerald type
**	frame.. then we will prompt if the user wants to continue with
**	the operation.
**	
** Inputs:
**	OO_OBJECT  *ao;	- application object being potentially changed.
**	ER_MSGID msgid;
**
** Outputs:
**	none
**
** Side Effects:
**
** History:
**	2/14/90 (tom) - created
*/
LOC bool
ff_Touch(ao, msgid)
OO_OBJECT *ao;
ER_MSGID msgid;
{
	if ( ((APPL_COMP*)ao)->flags & CUSTOM_EDIT)
	{
		return (IIUFccConfirmChoice(CONF_GENERIC, 
			ao->name, ERget(msgid), 
			ERget(F_VQ0121_CustFrEdHelp),
			ERx("vqcusted.hlp"), 
			F_VQ0122_CustFrEdTitle,
			F_VQ0123_CustFrEdYes, 
			F_VQ0124_CustFrEdNo, 
			ERx(""), TRUE)     !=    CONFCH_YES);
	}
	return (FALSE);
}