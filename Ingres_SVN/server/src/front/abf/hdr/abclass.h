# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include <ooclass.h>
/**
** Name:	abclass.qsh -	ABF Class Definitions File.
**
** Description:
**	Contains the definitions of the application class structures.  This
**	includes its component classes as well.  Defines:
**
**	APPL		application object.
**	APPL_COMP	application component base object.
**	FORM_REF	application form reference component object.
**	USER_FRAME	application user frame component object.
**	QBF_FRAME	application QBF frame component object.
**	REPORT_FRAME	application report frame component object.
**	GRAPH_FRAME	application graph frame component object.
**	_4GLPROC	application 4GL procedure component object.
**	DBPROC		application database procedure component object.
**	HLPROC		application host-language procedure component object.
**      CONSTANT        application constant object.
**	GLOBVAR		application global variable object.
**	RECDEF		application record definition object.
**	RECMEM		application record attribute object.
**	
** History:
**	Revision 6.2  89/01  wong
**	Initial revision.
**	20-sep-1989 kevinm (JupBug #7289)
**	Added another member (dml) to the HLPROC structure to support creating
**	procedures through the OtherLanguages menu item in ABF.
**	12/10/89 (tom) - changed the comments bracketing the overlapping
**		structure definitions so that they were correct..
**		NOTE: this practice of overlapping structures on top of one
**		      another use here should be discouraged. This procedure
**		      is error prone and overly confusing.. if the first part
**		      of a structure is to consist of another structure.. the
**		      the embedded structure should actually be embedded rather
**		      than just have it's members duplicated.
**	6/20/90 (Mike S)
**		Make dep_on a member of every APPL_COMP
**	08-jan-92 (davel)
**		modify CONSTANT structure to support a default constant
**		value stored in the catalogs - this will be stored as
**		value[0], thus requiring that the value array be sized
**		at ER_MAXLANGUAGES+1.
**	13-aug-92 (davel)
**		Modified DB_DATA_DESC to correctly match a DB_DATA_VALUE,
**		as lots of abf code relies on being able to cast a 
**		(DB_DATA_DESC) * to a (DB_DATA_VALUE *).  Left the db_data
**		as a char* though (instead of a PTR), as existing code
**		is assuming this to be a char*.
**	19-oct-92 (davel)
**		Changed the APC_DEFAULT_LANGUAGE to " default" from "default",
**		so that the default constants sort before any language-specific
**		ones.
**	04-dec-92 (davel)
**		Reserve bit in the flags word for "decimals passed as decimal"
**		for HLPROC.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	12-Jan-2005 (schka24)
**	    Reflect changes to DB_DATA_VALUE in stupid DB_DATA_DESC struct.
*/
  typedef struct {
    OOID ooid;
	dataWord	data;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    char *altered_by;
    OO_OBJECT *comps;
    i4 version;
    char modified[25+1];
    char *source;
    char *executable;
    char *start_name;
    char *link_file;
    u_i4 dml : 2;
    u_i4 start_proc : 1;
    u_i4 batch : 1;
    u_i4 has_meta : 1;
    char *roleid;
    char *password;
    i4 flags;
# define	APC_DBFORMS	1	/* Some non-compiled forms  */
# define	APC_DEFDBFORMS	2	/* Default to non-compiled forms  */
/*
** In VISION, the mask 0xF000 (bits 0x1000 - 0x8000) is reserved for default 
** menu style. For translations of the various mask values, see APPL_COMP
** below.
*/
  } APPL;
  typedef struct {
    char *db_data;
    i4 db_length;
    i2 db_datatype;
    i2 db_scale;
    i2 db_collID;
  } DB_DATA_DESC;
  typedef struct _APPL_COMP {
    OOID ooid;
	dataWord	data;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    char *altered_by;
    struct _APPL_COMP *_next;
    APPL *appl;
    i4 version;
    char *locsym;
    char *compdate;
    i4 flags;
# define	APC_UFERRS	1	/* Unfixed compilation errors
						   (only used for HLPROCs) */
# define	TREE_MARK	2	/* used to mark subtrees -
						runtime bit - catalog
						value irrelevent */
# define	CUSTOM_EDIT	4	/* bit to say that either:
					   - a user frame/proc was edited 
					   - an emerald frame was customized */
# define	FORM_CUST	8	/* form is customized - used only
					   within copyapp */
# define	APC_OSLWARN	0x10	/* OSL compiler generated warnings but 
					   no errors */
# define	APC_DBFORM	0x20	/* Don't build a compiled form */
# define	APC_PASSDEC	0x40	/* pass decimal values as packed 
					   decimal? (only used for HLPROCs) */
# define	APC_RECOMPILE	0x100	/* This frame must be recompiled.  One
					   of the following bits tells why */
# define	APC_RETCHANGE	0x200	/* The return type of a called 
					   frame changed */
# define	APC_GLOBCHANGE	0x400	/* A global or record definition used by
					   this frame has changed */
# define	APC_DEFCHANGE	0x800	/* The frame's definition has changed */
# define 	APC_INVALID	(APC_RECOMPILE|APC_RETCHANGE|APC_GLOBCHANGE|APC_DEFCHANGE)
/* The following block is VISION-only */
# define	APC_MS_MASK	0xF000	/* Menu style mask */
# define	APC_MS_CHOICES	16	/* Possible menu choices (4 bits) */
# define	APC_MS_OFFSET	12	/* Menu style mask bit offset */
# define	APC_MS1LINE	0x0000	/* Original, single-line menu */
# define	APC_MSTAB	0x1000	/* Tablefield-style menu */
    OO_OBJECT *dep_on;
  } APPL_COMP;
  typedef struct {
    OOID ooid;
	dataWord	data;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    char *altered_by;
    APPL_COMP *_next;
    APPL *appl;
    i4 version;
    char *locsym;
    char *compdate;
    i4 flags;
    OO_OBJECT *dep_on;
    char *source;
    char *symbol;
    i4 refs;
  } FORM_REF;
  typedef struct {
    OOID ooid;
	dataWord	data;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    char *altered_by;
    APPL_COMP *_next;
    APPL *appl;
    i4 version;
    char *locsym;
    char *compdate;
    i4 flags;
    OO_OBJECT *dep_on;
    DB_DATA_DESC return_type;
    char *source;
    FORM_REF *form;
    char *symbol;
    bool is_static;
    OOID fid;
	struct _metaframe *mf;		/* metaframe structure */
  } USER_FRAME;
  typedef struct {
    OOID ooid;
	dataWord	data;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    char *altered_by;
    APPL_COMP *_next;
    APPL *appl;
    i4 version;
    char *locsym;
    char *compdate;
    i4 flags;
    OO_OBJECT *dep_on;
    OOID qry_type;
    char *qry_object;
    FORM_REF *form;
    char *cmd_flags;
  } QBF_FRAME;
  typedef struct {
    OOID ooid;
	dataWord	data;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    char *altered_by;
    APPL_COMP *_next;
    APPL *appl;
    i4 version;
    char *locsym;
    char *compdate;
    i4 flags;
    OO_OBJECT *dep_on;
    char *report;
    char *source;
    FORM_REF *form;
    char *output;
    char *cmd_flags;
  } REPORT_FRAME;
  typedef struct {
    OOID ooid;
	dataWord	data;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    char *altered_by;
    APPL_COMP *_next;
    APPL *appl;
    i4 version;
    char *locsym;
    char *compdate;
    i4 flags;
    OO_OBJECT *dep_on;
    char *graph;
    char *output;
    char *cmd_flags;
  } GRAPH_FRAME;
  typedef struct {
    OOID ooid;
	dataWord	data;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    char *altered_by;
    APPL_COMP *_next;
    APPL *appl;
    i4 version;
    char *locsym;
    char *compdate;
    i4 flags;
    OO_OBJECT *dep_on;
    DB_DATA_DESC return_type;
    char *source;
    char *symbol;
    OOID fid;
  } _4GLPROC;
  typedef struct {
    OOID ooid;
	dataWord	data;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    char *altered_by;
    APPL_COMP *_next;
    APPL *appl;
    i4 version;
    char *locsym;
    char *compdate;
    i4 flags;
    OO_OBJECT *dep_on;
    DB_DATA_DESC return_type;
  } DBPROC;
  typedef struct {
    OOID ooid;
	dataWord	data;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    char *altered_by;
    APPL_COMP *_next;
    APPL *appl;
    i4 version;
    char *locsym;
    char *compdate;
    i4 flags;
    OO_OBJECT *dep_on;
    DB_DATA_DESC return_type;
    char *source;
    char *symbol;
    i4 ol_lang;
    i4 dml;
  } HLPROC;
  typedef struct {
    OOID ooid;
	dataWord        data;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    char *altered_by;
    APPL_COMP *_next;
    APPL *appl;
    i4 version;
    char *locsym;
    char *compdate;
    i4 flags;
    OO_OBJECT *dep_on;
    DB_DATA_DESC data_type;
    char *language;
# define	APC_CONSTANT_LANGUAGE	ERx(" default")	/* default language */
    char *value[ER_MAXLANGUAGES+1];
  } CONSTANT;
  typedef struct {
    OOID ooid;
	dataWord        data;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    char *altered_by;
    APPL_COMP *_next;
    APPL *appl;
    i4 version;
    char *locsym;
    char *compdate;
    i4 flags;
    OO_OBJECT *dep_on;
    DB_DATA_DESC data_type;
  } GLOBVAR;
  typedef struct _rec_mem {
    OOID ooid;
	dataWord        data;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    char *altered_by;
    APPL_COMP *_next;
    APPL *appl;
    i4 version;
    char *locsym;
    char *compdate;
    i4 flags;
    OO_OBJECT *dep_on;
    DB_DATA_DESC data_type;
    APPL_COMP *parent;
    struct _rec_mem *next_mem;
    i4 att_order;
  } RECMEM;
  typedef struct {
    OOID ooid;
	dataWord        data;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    char *altered_by;
    APPL_COMP *_next;
    APPL *appl;
    i4 version;
    char *locsym;
    char *compdate;
    i4 flags;
    OO_OBJECT *dep_on;
    RECMEM *recmems;
  } RECDEF;
/* definition of one of the arguements to IIAButilities() 
   which identify the caller, these arguments determine what the utility
   functions will do in regard to loading and freeing of app. components */
#define ABUT_AFD	1	/* called from the application flow diagram */
#define ABUT_APPCAT	2	/* called from the application catalog */
#define ABUT_FRMCAT	3	/* called from the frame catalog */
