# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# ifndef OOCLASS_INCLUDE
# define OOCLASS_INCLUDE
# ifdef major
# undef	major
# undef	minor
# endif
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
/**
** Name:	ooclass.qsh -	Object Instance Structure Declarations.
**
** Description:
**	Contains object instance structure declarations, and defines
**	for object ids of OO kernel objects and FE class objects.
**
**	Each Class has a corresponding instance structure declaration.
**	The typedef for a given class's instance structure is named
**	by uppercasing the class name and prepending "OO_".
**
**	The #define for the object id of a class is denoted by
**	uppercasing the class name and prepending "OC_".
**
**	Each class instance structure shares the same initial set of
**	members as its superclass.  Since all classes are subclasses
**	of class Object all instance structures begin with the
**	structure members defined for Object.
**
**	In object instance structures all character or date
**	attributes are represented by (char *) members containing
**	a pointer to a separately allocated character buffer.
**	All other attribute types are contained directly in the
**	instance structure as members.
**
** History:
**	24-jan-80 (mgw)
**		Added comments for Sequent bug.
**	04/07/91 (emerson)
**		Modifications for local procedures:
**		Added OC_OSLLOCALPROC (does not appear in catalogs;
**		used only by abfrt).
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      30-Oct-2002 (hweho01)
**          For AIX 32/64 hybrid build, ALIGN_RESTRICT is set to  
**          double in bzarch.h, but front end binaries are compiled    
**          with 32-bit mode, that caused alignment error in    
**          iiooNumDecode() routine when handling floating number.
**          To fix error "E_OO0044 unexpected attribute reference in   
**          front end test suite (vis02.sep) :
**          1) added integer "filler" (4 bytes) before double type  
**             field "mgleft" in struct OO_GRCOMP to ensure   
**             the 8-byte alignment,
**          2) to get around the redeclaration error from eqc,
**             defined the structure to _OO_GRCOMP within the sql   
**             "DECLARE" section first, then defined into OO_GRCOMP.
**	28-Mar-2005 (lakvi01)
**	    Added prototypes for IIOOid*() functions.
**	09-Apr-2005 (lakvi01)
**	    Moved the prototypes added above into #ifdef NT_GENERIC since
**	    they were causing compilation problems on non-windows compilers.
**/
EXEC SQL BEGIN DECLARE SECTION;
/*}
** Name:	OOID - 	typedef for Object Id
**
** Description:
**	Represents an object id.  Probably several places in code
**	that assume an OOID is same size as an i4, and can be
**	represented (and passed) as a single word.
**
*/
  typedef i4 OOID;
/*}
** Name:	dataWord	- status word in object structures.
**
** Description:
**	Represents various bits of information pertaining to the
**	status of the "in-memory" representation of an object.
**
*/
EXEC SQL BEGIN EXCLUDE SECTION;
typedef	struct {
    u_i4	levelbits:15;
    u_i4	dirty:1;
    u_i4	inDatabase:1;
    u_i4	dbObject:1;
    u_i4	tag:14;		/* memory tag */
} dataWord;
EXEC SQL END EXCLUDE SECTION;
/*}
** Name:	OO_ATTRIB	- attribute descriptor
**
** Description:
**	Represents information pertaining to an attribute
**	(structure member) of an object.
**
*/
  typedef struct OO_ATTRIB {
    i4 attnum;
    char *name;
    i4 frmt;
    i4 frml;
  } OO_ATTRIB;
/*
**	Object -- instance C structure declaration 
*/
  typedef struct OO_OBJECT {
    OOID ooid;
EXEC SQL BEGIN EXCLUDE SECTION;
 	dataWord	data;
EXEC SQL END EXCLUDE SECTION;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
  } OO_OBJECT;
/*
**	Class -- instance C structure declaration 
*/
  typedef struct OO_CLASS {
    OOID ooid;
EXEC SQL BEGIN EXCLUDE SECTION;
 	dataWord	data;
EXEC SQL END EXCLUDE SECTION;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    char *table_name;
    char *surrogate;
    OOID super;
    i4 level;
    i4 keepstatus;
    i4 hasarray;
    i4 memsize;
    OOID subClasses;
    OOID instances;
    OOID masterRefs;
    OOID detailRefs;
    OOID methods;
    OOID attributes;
  } OO_CLASS;
/*
**	Reference -- instance C structure declaration 
*/
  typedef struct OO_REFERENCE {
    OOID ooid;
EXEC SQL BEGIN EXCLUDE SECTION;
 	dataWord	data;
EXEC SQL END EXCLUDE SECTION;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    OOID master;
    i4 mdelete;
    i4 moffset;
    char *connector;
    OOID detail;
    i4 ddelete;
    i4 doffset;
  } OO_REFERENCE;
/*
**	Method -- instance C structure declaration 
*/
  typedef struct OO_METHOD {
    OOID ooid;
EXEC SQL BEGIN EXCLUDE SECTION;
 	dataWord	data;
EXEC SQL END EXCLUDE SECTION;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    OOID mclass;
    char *procname;
EXEC SQL BEGIN EXCLUDE SECTION;
 	OOID	(*entrypt)();
EXEC SQL END EXCLUDE SECTION;
    i4 argcount;
    i4 defaultperm;
    i4 keepstatus;
    i4 fetchlevel;
    i4 classmeth;
  } OO_METHOD;
/*
**	Collection -- instance C structure declaration 
*/
  typedef struct OO_COLLECTION {
    OOID ooid;
EXEC SQL BEGIN EXCLUDE SECTION;
 	dataWord	data;
EXEC SQL END EXCLUDE SECTION;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    i4 size;
    i4 current_obj;
    i4 atend;
    OOID *array;
  } OO_COLLECTION;
/*
**	Encoding -- instance C structure declaration 
*/
  typedef struct OO_ENCODING {
    OOID ooid;
EXEC SQL BEGIN EXCLUDE SECTION;
 	dataWord	data;
EXEC SQL END EXCLUDE SECTION;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    i4 object;
    i4 sequence;
    char *estring;
  } OO_ENCODING;
/*
**	UnknownRef -- instance C structure declaration 
*/
  typedef struct OO_UNKNOWNREF {
    OOID ooid;
EXEC SQL BEGIN EXCLUDE SECTION;
 	dataWord	data;
EXEC SQL END EXCLUDE SECTION;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
  } OO_UNKNOWNREF;
/*
**	Graph -- instance C structure declaration 
**
**	Note: Due to a bug in the Sequent compiler, a constant is
**	      used for the initialization of the rO123[].moffset
**	      structure member in front!graphics!graf!grclass.qsc.
**	      If you change the size of this structure, please change
**	      that offset constant in grclass.qsc
*/
  typedef struct OO_GRAPH {
    OOID ooid;
EXEC SQL BEGIN EXCLUDE SECTION;
 	dataWord	data;
EXEC SQL END EXCLUDE SECTION;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    char *viewname;
    char *indep;
    char *dep;
    char *series;
    char *label;
    i4 flags;
    char *sortstring;
    char *grtype;
    i4 encgraph;
    OOID components;
  } OO_GRAPH;
EXEC SQL END DECLARE SECTION;
/*
**	GrComp -- instance C structure declaration 
**
**	Note: Due to a bug in the Sequent compiler, constants are
**	      used for the initialization of the rO124[].moffset
**	      structure members in front!graphics!graf!grclass.qsc.
**	      If you change the size of this structure, please change
**	      those offset constants in grclass.qsc
*/
#if defined(rs4_us5) && defined(conf_ADD_ON64)
EXEC SQL BEGIN DECLARE SECTION;
  typedef struct _OO_GRCOMP {
    OOID ooid;
EXEC SQL BEGIN EXCLUDE SECTION;
 	dataWord	data;
EXEC SQL END EXCLUDE SECTION;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    OOID graph;
    i4 seq;
    i4 filler;
    double mgleft;
    double mgright;
    double mgtop;
    double mgbottom;
    double xlow;
    double xhi;
    double ylow;
    double yhi;
    i4 bckcolor;
    i4 flags;
    OOID legends;
    OOID depdats;
    OOID axdats;
  } _OO_GRCOMP;
EXEC SQL END DECLARE SECTION;
typedef _OO_GRCOMP OO_GRCOMP;
#else  
EXEC SQL BEGIN DECLARE SECTION;
  typedef struct OO_GRCOMP {
    OOID ooid;
EXEC SQL BEGIN EXCLUDE SECTION;
 	dataWord	data;
EXEC SQL END EXCLUDE SECTION;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    OOID graph;
    i4 seq;
    double mgleft;
    double mgright;
    double mgtop;
    double mgbottom;
    double xlow;
    double xhi;
    double ylow;
    double yhi;
    i4 bckcolor;
    i4 flags;
    OOID legends;
    OOID depdats;
    OOID axdats;
  } OO_GRCOMP;
EXEC SQL END DECLARE SECTION;
#endif  /* if defined(rs4_us5)  */
EXEC SQL BEGIN DECLARE SECTION;
/*
**	GrAx -- instance C structure declaration 
*/
  typedef struct OO_GRAX {
    OOID ooid;
EXEC SQL BEGIN EXCLUDE SECTION;
 	dataWord	data;
EXEC SQL END EXCLUDE SECTION;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    OOID graph;
    i4 seq;
    double mgleft;
    double mgright;
    double mgtop;
    double mgbottom;
    double xlow;
    double xhi;
    double ylow;
    double yhi;
    i4 bckcolor;
    i4 flags;
    OOID legends;
    OOID depdats;
    OOID axdats;
    double baseline;
    i4 dsset;
    i4 chartval;
    char *indep;
    i4 flag2;
  } OO_GRAX;
/*
**	GrPie -- instance C structure declaration 
*/
  typedef struct OO_GRPIE {
    OOID ooid;
EXEC SQL BEGIN EXCLUDE SECTION;
 	dataWord	data;
EXEC SQL END EXCLUDE SECTION;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    OOID graph;
    i4 seq;
    double mgleft;
    double mgright;
    double mgtop;
    double mgbottom;
    double xlow;
    double xhi;
    double ylow;
    double yhi;
    i4 bckcolor;
    i4 flags;
    OOID legends;
    OOID depdats;
    OOID axdats;
    double rotate;
    double explosion;
    i4 expfirst;
    i4 explast;
    i4 flag2;
    char *slice;
  } OO_GRPIE;
/*
**	GrLegend -- instance C structure declaration 
*/
  typedef struct OO_GRLEGEND {
    OOID ooid;
EXEC SQL BEGIN EXCLUDE SECTION;
 	dataWord	data;
EXEC SQL END EXCLUDE SECTION;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    OOID graph;
    i4 seq;
    double mgleft;
    double mgright;
    double mgtop;
    double mgbottom;
    double xlow;
    double xhi;
    double ylow;
    double yhi;
    i4 bckcolor;
    i4 flags;
    OOID legends;
    OOID depdats;
    OOID axdats;
    OOID agrcomp;
    i4 tfont;
    i4 tcolor;
    i4 dfont;
    i4 dcolor;
    i4 flag2;
  } OO_GRLEGEND;
/*
**	GrText -- instance C structure declaration 
*/
  typedef struct OO_GRTEXT {
    OOID ooid;
EXEC SQL BEGIN EXCLUDE SECTION;
 	dataWord	data;
EXEC SQL END EXCLUDE SECTION;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    OOID graph;
    i4 seq;
    double mgleft;
    double mgright;
    double mgtop;
    double mgbottom;
    double xlow;
    double xhi;
    double ylow;
    double yhi;
    i4 bckcolor;
    i4 flags;
    OOID legends;
    OOID depdats;
    OOID axdats;
    i4 font;
    i4 color;
    char *colum;
    i4 flag2;
  } OO_GRTEXT;
/*
**	GrDepDat -- instance C structure declaration 
*/
  typedef struct OO_GRDEPDAT {
    OOID ooid;
EXEC SQL BEGIN EXCLUDE SECTION;
 	dataWord	data;
EXEC SQL END EXCLUDE SECTION;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    OOID agrcomp;
    i4 seq;
    char *dfield;
    i4 dcolor;
    i4 fcolor;
    i4 mark;
    i4 dash;
    i4 cnct;
    i4 lcolor;
    i4 lfont;
    char *lformat;
    char *lfield;
    i4 flags;
  } OO_GRDEPDAT;
/*
**	GrAxDt1 -- instance C structure declaration 
*/
  typedef struct OO_GRAXDT1 {
    OOID ooid;
EXEC SQL BEGIN EXCLUDE SECTION;
 	dataWord	data;
EXEC SQL END EXCLUDE SECTION;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    i4 agrcomp;
    i4 axindex;
    i4 ticks;
    i4 grids;
    i4 major;
    double rnglo;
    double rnghi;
    double margin;
    char *header;
    i4 labsup;
    i4 datefmt;
    i4 numfmt;
    i4 hdjust;
    i4 tickloc;
    i4 dsidx;
    i4 hbcolor;
    i4 htcolor;
    i4 labcolor;
    i4 lincolor;
    i4 hfont;
    i4 lfont;
    i4 flags;
    double inc;
    i4 prec;
  } OO_GRAXDT1;
/*
**	Gropt -- instance C structure declaration 
*/
  typedef struct OO_GROPT {
    OOID ooid;
EXEC SQL BEGIN EXCLUDE SECTION;
 	dataWord	data;
EXEC SQL END EXCLUDE SECTION;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    char *plotdev;
    char *plotloc;
    char *fc;
    i4 mainlevel;
    i4 editlevel;
    i4 layerlevel;
    i4 plotlevel;
    i4 dtrunc;
    i4 flags;
  } OO_GROPT;
/*
**	GrBar -- instance C structure declaration 
*/
  typedef struct OO_GRBAR {
    OOID ooid;
EXEC SQL BEGIN EXCLUDE SECTION;
 	dataWord	data;
EXEC SQL END EXCLUDE SECTION;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    OOID graph;
    i4 seq;
    double mgleft;
    double mgright;
    double mgtop;
    double mgbottom;
    double xlow;
    double xhi;
    double ylow;
    double yhi;
    i4 bckcolor;
    i4 flags;
    OOID legends;
    OOID depdats;
    OOID axdats;
    double baseline;
    i4 dsset;
    i4 chartval;
    char *indep;
    i4 flag2;
  } OO_GRBAR;
/*
**	GrLine -- instance C structure declaration 
*/
  typedef struct OO_GRLINE {
    OOID ooid;
EXEC SQL BEGIN EXCLUDE SECTION;
 	dataWord	data;
EXEC SQL END EXCLUDE SECTION;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    OOID graph;
    i4 seq;
    double mgleft;
    double mgright;
    double mgtop;
    double mgbottom;
    double xlow;
    double xhi;
    double ylow;
    double yhi;
    i4 bckcolor;
    i4 flags;
    OOID legends;
    OOID depdats;
    OOID axdats;
    double baseline;
    i4 dsset;
    i4 chartval;
    char *indep;
    i4 flag2;
  } OO_GRLINE;
/*
**	GrScat -- instance C structure declaration 
*/
  typedef struct OO_GRSCAT {
    OOID ooid;
EXEC SQL BEGIN EXCLUDE SECTION;
 	dataWord	data;
EXEC SQL END EXCLUDE SECTION;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    OOID graph;
    i4 seq;
    double mgleft;
    double mgright;
    double mgtop;
    double mgbottom;
    double xlow;
    double xhi;
    double ylow;
    double yhi;
    i4 bckcolor;
    i4 flags;
    OOID legends;
    OOID depdats;
    OOID axdats;
    double baseline;
    i4 dsset;
    i4 chartval;
    char *indep;
    i4 flag2;
  } OO_GRSCAT;
/*
**	GrAxDat -- instance C structure declaration 
*/
  typedef struct OO_GRAXDAT {
    OOID ooid;
EXEC SQL BEGIN EXCLUDE SECTION;
 	dataWord	data;
EXEC SQL END EXCLUDE SECTION;
    OOID class;
    char *name;
    i4 env;
    char *owner;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    OOID agrcomp;
    i4 axindex;
    double ticks;
    double grids;
    double rnglo;
    double rnghi;
    double margin;
    char *header;
    i4 labsup;
    i4 datefmt;
    i4 numfmt;
    i4 hdjust;
    i4 tickloc;
    i4 dsidx;
    i4 hbcolor;
    i4 htcolor;
    i4 labcolor;
    i4 lincolor;
    i4 hfont;
    i4 lfont;
    i4 flags;
    double inc;
    i4 prec;
  } OO_GRAXDAT;
/*
**	##defines -- pre-defined class object ids
*/
# define nil 0
# define OC_UNDEFINED -1
# define OC_OBJECT 1
# define OC_CLASS 2
# define OC_REFERENCE 3
# define OC_METHOD 4
# define OC_COLLECTION 5
# define OC_ENCODING 12
# define OC_UNKNOWNREF 75
# define OC_GRAPH 123
# define OC_GRCOMP 124
# define OC_GRAX 125
# define OC_GRPIE 126
# define OC_GRLEGEND 127
# define OC_GRTEXT 128
# define OC_GRDEPDAT 129
# define OC_GRAXDT1 130
# define OC_GROPT 131
# define OC_GRBAR 132
# define OC_GRLINE 133
# define OC_GRSCAT 134
# define OC_GRAXDAT 151
# define OC_JOINDEF 1002
# define OC_REPORT 1501
# define OC_RWREP 1502
# define OC_RBFREP 1511
# define OC_FORM 1601
# define OC_APPL 2001
# define OC_ILCODE 2010
/*
** OC_APPLPROC - OC_AMAXPROC  and OC_APPLFRAME - OC_AMAXFRAME
** are ranges used in abf for checking frame and procedure namespaces
** OC_AMAXFRAME encompasses future EMERALD frame types to properly
** detect what is happening when a 6.2 FE is running over the net
** on a later database.
*/
# define OC_APPLPROC 2020
# define OC_HLPROC 2021
# define OC_OSLPROC 2050
# define OC_DBPROC 2075
# define OC_OSLLOCALPROC 2099
# define OC_UNPROC 2190
# define OC_AMAXPROC 2100
# define OC_GLOBAL 2110
# define OC_CONST 2120
# define OC_RECORD 2130
# define OC_RECMEM 2133
# define OC_APPLFRAME 2200
# define OC_QBFNAME 2201
# define OC_OSLFRAME 2210
# define OC_OLDOSLFRAME 2219
# define OC_RWFRAME 2220
# define OC_QBFFRAME 2230
# define OC_GRFRAME 2240
# define OC_GBFFRAME 2249
# define OC_UNFRAME 2250
# define OC_MUFRAME 2260
# define OC_APPFRAME 2261
# define OC_UPDFRAME 2262
					/* 2263 - modify frame was removed */
# define OC_BRWFRAME 2264
					/* 2265 - display frame was removed */
# define OC_AMAXFRAME 2300
# define OC_AFORMREF 3001
EXEC SQL END DECLARE SECTION;
#ifdef NT_GENERIC
FUNC_EXTERN STATUS	IIOOidCheck(OOID *, OOID *, char *, char *, ...);
FUNC_EXTERN STATUS	IIOOidObject(OOID, OOID *, ...);
FUNC_EXTERN STATUS	IIOOidDestroy(OOID *, OOID *, char *, char *);
FUNC_EXTERN STATUS	IIOOidRename(OOID *, OOID *, char *, char *,
				     char *, ...);
FUNC_EXTERN STATUS	IIOOidFetch(OOID *, OOID *, char *, char [], ...);
FUNC_EXTERN STATUS	IIOOidWrite(OOID, OOID *, ...);
#endif /* NT_GENERIC */
# endif /* OOCLASS_INCLUDE */
