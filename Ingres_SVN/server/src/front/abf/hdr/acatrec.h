# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
/**
** Name:	acatrec.sh -	ABF Objects Catalog Record Definitions File.
**
** History:
**	Revision 6.0  87/03  bobm
**	Initial revision.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/*}
** Name:	ACATREC -	Linked List Entry of ABF Application Catalog.
**
** History:
**	3/87 (bobm)	written
**	2/88 (bobm)	6.0/0 - up version number to 2 to force OSL recompiles
**	18-may-93 (davel)
**		Added member to ACATDEP for depended-upon object owner.
*/
/* Cover for 'next' structure member since "next" is an ESQL keyword */
#define next	_next
# define AXAP_EXNAME 0
# define AXAP_DEFST 1
# define AXAP_OPTFIL 2
# define AXAP_QLANG 3
# define AXRW_CMD 0
# define AXRW_OUT 1
# define AXGB_CMD 0
# define AXGB_OUT 1
# define AXRG_CMD 0
# define AXRG_OUT 1
# define AXQF_CMD 0
# define AXQF_JDEF 1
# define AXHL_LANG 0
# define AXOF_STATIC 0
# define ACARGS 6
  typedef struct _acdp {
    struct _acdp *_next;
    i4 deptype;
    i4 id;
    char *name;
    i4 class;
    char *deplink;
    i4 deporder;
    char *owner;
  } ACATDEP;
  typedef struct _acr {
    i4 ooid;
    i4 class;
    char *name;
    char *owner;
    i4 env;
    i4 is_current;
    char *short_remark;
    char *create_date;
    char *alter_date;
    char *long_remark;
    i4 alter_cnt;
    char *altered_by;
    i4 applid;
    char *source;
    char *symbol;
    i2 retadf_type;
    char *rettype;
    i4 retlength;
    i2 retscale;
    i2 version;
    char *arg[ACARGS];
    i4 flags;
    struct _acr *_next;
    i2 tag;
    ACATDEP *dep_on;
    ACATDEP *dep_by;
  } ACATREC;
