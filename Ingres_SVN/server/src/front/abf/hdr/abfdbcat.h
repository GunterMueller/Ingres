# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
/**
** Name:	abfdbcat.sh -	ABF DB Catalog Record Definitions File.
**
** History:
**	Revision 6.0  87/03  bobm
**	Initial revision.
**	2/88 (bobm)	6.0/0 - up version number to 2 to force OSL recompiles
**
**	Revision 6.2  89/01  wong
**	Changed to be one-to-one map with retrieved attributes, renamed as
**	ABF_DBCAT from ACATREC.
**	10/89 (Mike S)	Add flags word
**
**	Revision 6.5
**	24-aug-92 (davel)
**		incremented version to 3, and added #defines for previous
**		version numbers.  Version was incremented to allow for 
**		changes to global constant objects in release 6.5.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
# define ACAT_VERSION 3
# define ACAT_VERSION_REL_50 1
# define ACAT_VERSION_REL_60 2
# define ACAT_VERSION_REL_65 3
  typedef struct {
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
#define	ABFSRC_SIZE	180
    char *source;
    char *symbol;
    i2 retadf_type;
    char *rettype;
    i4 retlength;
#define				START_FRAME	0x0	/* start frame */
#define				START_PROC	0x1	/* start procedure */
#define				APPL_BATCH	(0x2|START_PROC)
    i2 retscale;
    i2 version;
#define ABFARGS		6
#define	ABFARG_SIZE	48
#define AppExecutable	arg0
#define AppStartName	arg1
#define AppLinkFile	arg2
#define AppDML		arg3
#define AppRole		arg4
#define RepCmd		arg0
#define RepOutput	arg1
#define GBFCmd		arg0
#define GBFOutput	arg1
#define GraphCmd	arg0
#define GraphOutput	arg1
#define QBFCmd		arg0
#define QBFQryType	arg1
#define HostLanguage	arg0
#define UserStatic	arg0
#define Compdate	arg3
#define UserFormgen	arg4
#define UserGendate	arg5
#define ConstLanguage	arg0
    char *arg0;
    char *arg1;
    char *arg2;
    char *arg3;
    char *arg4;
    char *arg5;
    char *dname;
    char *dlink;
    i4 flags;
    i4 deptype;
    i4 deporder;
    i4 dclass;
  } ABF_DBCAT;
