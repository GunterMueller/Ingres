# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include <ooclass.h>
/**
** Name:	oocat.qh -	header file for OO catalog functions.
**
** Description:
**	Contains #defines and declaration of OO_CATREC structure
**	corresponding to info in OO Object catalog (and catalog frame
**	table field).
**
**	Also declares string constant variables representing OO
**	catalog EQL entities (tables, form names, columns, fields)
**
** History:
**	Revision 6.2  88/12  wong
**	Added 'alter_cnt' and 'altered_by' to OO_CATREC.
**	89/01  Added 'IIOOmapOwner()' macro.
**  12/19/89 (dkh) - VMS shared lib changes - references to iiOODBA is now
**		     through IIOO_DBA.
**	09/06/90 (dkh) - Changed to use IIOOgdGetDBA instead of IIOO_DBA
**			 so we don't export data references through the
**			 shared library layer.
**	13-dec-1991 (kevinm)
**		Changed declaration of _iiOOempty, _class, and _name to
**		be declared as GLOBALCONSTREFs to be consistent with their
**		respective declarations elsewhere.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	28-Mar-2005 (lakvi01)
**	    Added some OOcat* function prototypes.
**/
/*}
** Name:	OO_CATREC -	OO Catalog TableField Entry Structure.
**
** Description:
**	Declares structure for row of an object catalog table field.
**
** History:
**	01/86 (peterk) - Defined.
**	4/87 (peterk) - modified for 6.0 FE's; name of struct
**		changed to OO_CATREC.
**	4/26/87 (peterk) - added externs for char string constants
**		used by and with OO catalog routines.
**	4/26/87 (peterk) - put ##defines for sizes here.
**	12/11/88 (jhw) - Added 'alter_cnt' and 'altered_by' to OO_CATREC.
*/
# define OOSHORTREMSIZE 60
# define OOLONGREMSIZE 600
# define OODATESIZE 25
  typedef char OOCAT_DATE[OODATESIZE+1];
  typedef struct OO_CATREC {
    i4 id;
    i4 class;
    char name[FE_MAXNAME+1];
    char owner[DB_MAXNAME+1];
    i4 env;
    i4 is_current;
    char short_remark[OOSHORTREMSIZE+1];
    OOCAT_DATE create_date;
    OOCAT_DATE alter_date;
    char long_remark[OOLONGREMSIZE+1];
    i4 alter_cnt;
    char altered_by[DB_MAXNAME+1];
  } OO_CATREC;
  typedef struct {
    char *mpstring;
    i4 mpfrskey;
	STATUS	(*mpaction)();	/* function implementing action */
  } MENUPARAM;
# define _	_iiOOempty
  GLOBALCONSTREF char _iiOOempty[];
  GLOBALREF char _altr_date[];
  GLOBALREF char _altr_count[];
  GLOBALREF char _cattable[];
  GLOBALCONSTREF char _class[];
  GLOBALREF char _create_date[];
  GLOBALREF char _env[];
  GLOBALREF char _estring[];
  GLOBALREF char _id[];
  GLOBALREF char _ii_longremarks[];
  GLOBALREF char _ii_objects[];
  GLOBALREF char _iicatalog[];
  GLOBALREF char _iidetail[];
  GLOBALREF char _iisave[];
  GLOBALREF char _is_current[];
  GLOBALREF char _long_remark[];
  GLOBALREF char _last_altered_by[];
  GLOBALCONSTREF char _name[];
  GLOBALREF char _object[];
  GLOBALREF char _owner[];
  GLOBALREF char _sequence[];
  GLOBALREF char _short_remark[];
  GLOBALREF char _title[];
/*{
** Name:	IIOOmapOwner() -	Map Owner to Representation.
**
** Description:
**	Maps the owner's name (of an object) to its representation,
**	which is either itself or the DBA string.
**
** Input:
**	owner	{char *}  The owner's name.
**
** Returns:
**	{char *}  The name representation.
**
** History:
**	01/89 (jhw) -- Written.
**	11/30/89 (Mike S) -- Use structure field names for UI data
**	28-aug-1990 (Joe)
**	    Changed from IIUIgdata to IIUIdbdata().
**	09/06/90 (dkh) - Changed to use IIOOgdGetDBA() instead of IIOO_DBA.
*/
FUNC_EXTERN char	*IIOOgdGetDBA();
#define IIOOmapOwner(owner) \
	( ( !STequal((owner), IIUIdbdata()->dba) || IIUIdbdata()->isdba ) \
					? (owner) : IIOOgdGetDBA() )
FUNC_EXTERN VOID	IIOOinit(OO_OBJECT *[]);
FUNC_EXTERN VOID	iiooInteractive();
FUNC_EXTERN VOID	OOcatTableLoad(OOID, char *, char *, bool, char *,
				       char *, char *);
FUNC_EXTERN VOID	OOcatEmpty(OOID, char *, char *);
FUNC_EXTERN STATUS	OOcatCheck(OOID, char *, char *, OO_CATREC *);
FUNC_EXTERN STATUS	OOcatDestroy(OOID, char *, char *, OO_CATREC *, bool);
FUNC_EXTERN STATUS	OOcatRename(OOID, char *, char *, OO_CATREC *, bool);
FUNC_EXTERN bool	OOdetail(OOID, char *, char *, register OO_CATREC *,
				 char *, char *, PTR, STATUS (*)(),
				 STATUS (*)(), STATUS (*)(), STATUS (*)(),
				 STATUS (*)(), MENUPARAM *, char *);
