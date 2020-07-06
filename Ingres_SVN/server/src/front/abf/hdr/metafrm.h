# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
/**
** Name:	metafrm.qsh -	Metaframe definitions
**
** Description:
**	This file defines the METAFRAME structure and its 
**	associated structures and definitions.
**	This set of structures supports the elements necessary to
**	generate the default form and 4GL code for the Emerald project.
**
** History:
**	12/14/88 (tom) - created
**	09/27/89 (tom) - removed the modify and display frame types
**	21-aug-92 (blaise)
**		Increased MF_MAXESCS from 50 to 100 (SIR #44985)
**	6-dec-92 (blaise)
**		Added support for optimistic locking; see history for MFCOL
**		and METAFRAME structures, below.
**      23-feb-93 (connie) Bug #43375
**		Increased MF_MAXVARS from 50 to 100
**/
# define MF_MENU	0	/* menu frame.. NOTE: it is important
				   that the menu frame is 0, and come first
				   because it is the type of frame which
				   is automatically inserted when inserting
				   a new frame */
# define MF_APPEND	1	/* frame for appending */
# define MF_UPDATE	2	/* user qual frame for update */
# define MF_BROWSE	3	/* user qualified frame for browse */ 
# define MF_USER	4	/* user coded frame */
# define MF_QBF		5	/* qbf frame */
# define MF_REPORT	6	/* report frame */
# define MF_GRAPH	7	/* graph frame */
# define MF_OSLPROC	8	/* osl procedure */
# define MF_HLPROC	9	/* host language procedure */
# define MF_DBPROC	10	/* database procedure */
# define NUMFRMTYPES	11	/* number of types of frames that there 
				   are in the list above, if frame types
				   are added.. then this number must be 
				   increased */
/******* constants for pointer array table sizes *******/
/******* the MQ_ constants are also used for table sizes *******/
# define MF_MAXMENUS	15 	/* MAX_MENUITEM (menu.h) for the form system
				   is currently 25 but we need to reserve 
				   some for the generated code.. 15 should be 
				   a reasonable number of menu items. */
# define MF_MAXTABS	10	/* maximum number of tables in metaframe 
				   same as MQ_MAXRELS..
				   if this number is changed, then the
				   display loop must have its activation
				   code modified.. and other changes..  */
# define MF_MAXJOINS	50	/* maximum number of joins in metaframe 
				   same as MQ_MAXJOINS */
# define MF_MAXESCS	100	/* maximum number of user defined escape 
				   segments in a metaframe */
# define MF_MAXVARS	100	/* maximum number of local variables in 
				   a metaframe */
# define MF_MAXARGS	50	/* maximum number of arguments in a callframe
				   argument list */
# define MFCOMMENT	50	/* maximum size of a variable comment */
  typedef struct {
    char *name;
    char *alias;
    PTR utility;
    DB_DATA_VALUE type;
    i4 flags;
# define COL_USED	1	/* column is to be used on form/report */
# define COL_DETJOIN	2	/* column is joined to a detail table, 
				   and must be displayed */
# define COL_LUPJOIN	4	/* column is joined to a lookup table,
				   and must be displayed */
# define COL_SUBJOIN	8	/* column is a subordinate join field 
				   this means that it can't be displayed */
# define COL_SEQUENCE	0x10	/* column is sequenced */
# define COL_DESCEND	0x20	/* column is descending (for sort) */
# define COL_UNIQKEY	0x40	/* column is part of the unique key for
				   this table */
# define COL_OPTLOCK	0x80	/* column is used in optimistic locking code
				   to store a counter, date/time stamp or
				   username */
# define COL_DEFAULT	0x100	/* does this column allow defaults */
/*
** The following flag is only used during form fixup; they have no meaning
** at any other time.
*/
# define COL_FOUND	0x200	/* Was this column found in existing form */
# define COL_VARIABLE	0x400	/* Column is to generate a local variable or
				   hidden column.  COL_USED will override this*/
# define COL_TYPECHANGE 0x2000	/* Has this column's datatype changed */
    i4 corder;
    char *info;
  } MFCOL;
  typedef struct _mf_tab {
    char *name;
    PTR utility;
    i4 tabsect;
#define TAB_MASTER	0	/* table is in master section */
#define TAB_DETAIL	1	/* table is in detail section */
		/* when multiple detail sections are supported
		   they will be assigned values 2, 3, 4, ... */
    i4 usage;
# define TAB_APPEND	0	/* append table */
# define TAB_UPDATE	1	/* update table */
# define TAB_DISPLAY	2	/* display table */
# define TAB_LOOKUP	3	/* lookup table */
# define TAB_REPORT	4	/* report table */
# define NUMTABTYPES	5	/* maximum number of different types of 
				   tables, if a new type is added above,
				   increase this */
    i4 flags;
		/* ..for lookup tables */
# define TAB_QUALLUP	0x1	/* qualified lookup bit */
		/* ..for update tables */
# define TAB_INSFLG	0x1	/* are insertions allowed into detail tf 
				   ..only relevent to the detail table,
				   and to masters in table field */
# define TAB_DELFLG	0x2	/* can we delete master or detail records */
# define TAB_UPDRULE	0x4	/* update ref. integ. rule
				    if set = restrict, else cascade */
# define TAB_DELRULE	0x8	/* delete ref. integ. rule
				    if set = restrict, else cascade */
# define TAB_NOKEY	0x10	/* set this bit if the table has no 
				   unique keys */
# define TAB_UPDNONE	0x20	/* no update ref. integ. rule
				    dbms is assumed to handle it */
# define TAB_DELNONE	0x40	/* no delete ref. integ. rule
				    dbms is assumed to handle it */
/* 
** The following flag is used only during query generation.  It has no meaning
** at any other time.  It can be set only for lookup tables.
*/
# define TAB_NODISPC	0x1000	/* No columns from this table are displayed */
    i4 numcols;
    MFCOL **cols;
  } MFTAB;
  typedef struct {
    i4 type;
# define JOIN_MASDET	0	/* this join is from master set to detail */
# define JOIN_MASLUP	1	/* this join is from master to lookup */
# define JOIN_DETLUP	2	/* this join is from detail to lookup */ 
    i4 tab_1;
    i4 tab_2;
    i4 col_1;
    i4 col_2;
  } MFJOIN;
  typedef struct {
    i4 type;
# define ESC_FORM_START		1
# define ESC_FORM_END		2
# define ESC_QRY_START		3
# define ESC_QRY_NEW_DATA	4
# define ESC_QRY_END		5
# define ESC_APP_START		6
# define ESC_APP_END		7
# define ESC_UPD_START		8
# define ESC_UPD_END		9
# define ESC_DEL_START		10
# define ESC_DEL_END		11
# define ESC_MENU_START		12
# define ESC_MENU_END		13
# define ESC_FLD_ENTRY		14
# define ESC_FLD_CHANGE		15
# define ESC_FLD_EXIT		16
# define ESC_TF_MNUITM		17
# define ESC_ML_MNUITM		18
# define ESC_BEF_LOOKUP		19
# define ESC_AFT_LOOKUP		20
# define ESC_ON_TIMEOUT		21
# define ESC_ON_DBEVENT		22
# define ESC_LOC_PROC		23
# define NUMESCTYPES		23
    i4 tabsect;
    char *item;
    char *text;
  } MFESC;
  typedef struct {
    char *var;
    char *expr;
  } MFCFARG;
  typedef struct {
    char *text;
	OO_OBJECT *apobj;	/* pointer to the application object
				   to call if this menu item is selected */
    i4 numargs;
    MFCFARG **args;
  } MFMENU;
  typedef struct {
    char *name;
    i4 vartype;
# define VAR_LVAR	0	/* local variable */
# define VAR_LPROC	1	/* local procedure */
    char *dtype;
    char *comment;
  } MFVAR;
  typedef struct _metaframe {
/*
** bits for IIAMmpMetaPopulate.  Each code is a single bit.
** MF_P_NUM is the number of bits involved - used to allocate array
** of tagid's for allocations (see below).
*/
# define MF_P_VQ	1	/* populate visual query portion */
# define MF_P_ESC	2	/* escape codes portion */
# define MF_P_ARG	4	/* menuarguments portion */
# define MF_P_FLD	8	/* field arguments portion */
/*
** indices into ptag for tags corresponding to individual pieces.
*/
# define MF_PT_VQ	0
# define MF_PT_ESC	1
# define MF_PT_ARG	2
# define MF_PT_FLD	3
# define MF_P_NUM	4
	TAGID	ptag[MF_P_NUM];	/* array to hold the allocation tags
				   corresponding to the various portions
				   of the structures associated with this
				   metaframe */
	PTR	tptr;		/* temp pointer used during construction */
	i4	popmask;	/* bitmask to tell which portions
				   of the structures have been populated */
	i4	updmask;	/* bitmask to tell which portions
				   of the structures need to be updated
				   to the catalogs */
	i1	flags;		/* flags for the metaframe, these are **NOT**
				   written to the database. */
# define MF_CHANGED	1	/* flag bit: the user changed something */
# define MF_MARKED	0x10	/* flag bit: there is a paste operation 
				   active.. this bit is used to detect
				   loops in the tree */
# define MF_ABFONLY	2	/* fetched under ABF - no menu deps */
    i2 mode;
# define MF_NOTABLES	0	/* no tables in the query */
# define MF_MASDET	1	/* master detail */
# define MF_MASTAB	2	/* master only in a table field */
# define MF_MASSIM	3	/* master only in simple fields  */
# define MFMODE		0x0f	/* mask for the major mode of the frame */ 
				/* leave 2 spare bits for more mode types */
				/* now for some flag bits */
# define MF_NOQUAL	0x10	/* flag bit: there is no qualification phase
				   associated with the runtime operation
				   of this frame (Query is run in 4gl 
				   initialize block) */
# define MF_NONEXT	0x20	/* flag bit: there is no next menuitem
				   associated with the runtime operation
				   of this frame (Query is only allowed
				   to return 1 master row) */
/*
** Note on MF_DBMSLOCK and MF_OPTIM. The only valid combinations of
** settings for these two flag bits are:
**	MF_DBMSLOCK SET and MF_OPTIM NOT SET
**		- dbms locks held on displayed data
**	MF_DBMSLOCK NOT SET and MF_OPTIM NOT SET
**		- no locks held on displayed data
**	MF_DBMSLOCK NOT SET and MF_OPTIM SET
**		- no locks held; generated code performs optimistic locking
*/
# define MF_DBMSLOCK	0x40	/* flag bit: if set indicates that locks
				   are are to be held on the data that
				   is on the screen (this is only
				   relevant to an update frame). Re-named 
				   when optimistic locking was added; was
				   MF_CONSIST */
# define MF_OPTIM	0x80	/* flag bit: if set indicates that the
				   code generator will generate code to
				   perform optimistic locking (this is
				   only relevant to an update frame) */
# define MF_LOCKS_MASK	0xC0	/* Mask to isolate lock bits */
# define MF_LOCK_OFFSET	6	/* Lock bit offset */
# define MF_UNSET_LOCKS	~(MF_DBMSLOCK | MF_OPTIM) /* mask to unset lock bits */
	OO_OBJECT	*apobj;	/* abf application object */
    i4 nummenus;
    MFMENU **menus;
    i4 numtabs;
    MFTAB **tabs;
    i4 numjoins;
    MFJOIN **joins;
    i4 numescs;
    MFESC **escs;
    i4 numvars;
    MFVAR **vars;
    i4 state;
	/*
	** the MFST_ settings for the state item are to be used as
	** a bit map - meke new definitions single bit values.
	*/
#define	MFST_GENBAD	0x01	/* the metaframe does not have the tables
				   joined properly (most likely), or an error 
				   in the metaframe data structure was found 
				   when attempting to generate the osl code */
#define	MFST_VQEERR	0x02	/* an error in the metaframe data structure
				   was found when saving the VQ edit,
				   this precludes frame generation, if this
				   bit is set, then the MFST_ERMSK bits are
				   set with the error number */
#define MFST_DOFORM	0x04	/* form generation / fixup operation is
				   needed */
#define MFST_DOGEN	0x08	/* osl generation is needed */
#define MFST_NEWFR	0x10	/* the frame is new and has not been
				   generated yet.. this is like DOGEN..
				   it is here only so we can display the "New"
				   state information for the user in the 
				   application flow diagram */
#define MFST_ERMSK	0x0F00	/* mask to isolate the high byte of the 
				   state member for the purposes of 
				   storing an error number in cases of
				   an incomplete, or erroneous VQ, this
				   contains a value if the MFST_VQEERR
				   bit (above) is set */
#define MFST_MIMASK	0x0F00  /* mask off the number of menuitems which
				   existed the last time the form was created
				   or fixed up.  Only used ofr menu frames,
				   so it's OK that this is the same mask as 
				   MFST_ERMSK. */
#define MFST_MIOFFSET	8	/* 8 bit offset */
/* macros to get/set the vq error number from the state member */
#define MFST_ER_GET_MACRO(mf) ((mf->state & MFST_ERMSK) >> 8)
#define MFST_ER_SET_MACRO(mf, err) (	(mf->state &= ~MFST_ERMSK),\
					(mf->state |= (err  << 8)) ) 
/* possible error codes returned by vq_ChkValid(), values stored
   in MFST_ERMSK */
# define VQ_NOMCOL 1		/* no column of the master table displayed */
# define VQ_NODCOL 2		/* no column of the detail table displayed */
# define VQ_NOJOIN 3 		/* no join on one of the tables */
# define VQ_NOMDJO 4		/* no join between the master and detail */
# define VQ_BADMDJ 5		/* no master detail join column is displayed
				   or has assignment expression (append frames
				   only) */
    char *gendate;
    char *formgendate;
  } METAFRAME;
