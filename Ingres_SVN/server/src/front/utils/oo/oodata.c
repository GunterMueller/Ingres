# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
/*
**
LIBRARY = IMPFRAMELIBDATA
**
*/
/*
**      OO data
**
**  Description
**      File added to hold all GLOBALDEFs in front Copy facility.
**      This is for use in DLLs only.
**
**  History:
**      07-Dec-95 (fanra01)
**          Created.
**      14-Feb-96 (fanra01)
**          Added iiOODBAsize to get sizeof string.
**	21-nov-96 (hanch04)
**	    Problems linking for unix
**
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      22-Jun-2001 (horda03)
**          The number of entries in the iiOOtable is determined in
**          ooinit().
**          (45781)
**	29-Sep-2004 (drivi01)
**	    Added LIBRARY jam hint to put this file into IMPFRAMELIBDATA
**	    in the Jamfile.
*/
#include	<compat.h>
#include	<st.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<ug.h>
#include	<erui.h>
# include <ooclass.h>
# include <oocat.h>
# include <oocatlog.h>
#include	<oodefine.h>
#include	<oosymbol.h>
#include	<eqrun.h>
#include        "ooldef.h"
#include	"eroo.h"
/*
**  Data from catclass.c
*/
GLOBALDEF C_OO_CATALOG	IIooCatClass =
{
	IIOOct0Init,
	IIOOctEmpty,
	IIOOctCheck,
	IIOOctLoad,
	IIOOctDestroy,
	IIOOctRename,
	IIOOctFetch,
	IIOOctMoreInfo,
	IIOOctTFldInit,
	IIOOctxTFldLoad,
	IIOOctRowLoad,
	IIOOctInsertRow,
	IIOOctPutRow
};
/*
**  Data from oodetail.qsc
*/
# define	MAXMENUPARAM	10
  GLOBALDEF MENUPARAM lmenup[MAXMENUPARAM]= {0};
/*
**  Data from ooinit.c
*/
# if defined(NT_GENERIC)
GLOBALDEF OO_OBJECT	iiOOnil[1] = {
	0, {0x7fff, 0, 1, 0, 0}, 1, ERx("nil"), 0, ERx(""), 1,
	ERx("the undefined object"),
};
GLOBALDEF i4		IIuserIndex = 0;
GLOBALDEF OO_OBJECT	**iiOOtable ZERO_FILL;
GLOBALDEF i4	iiOOsize	= 0;
GLOBALDEF bool	DDebug		= FALSE;
GLOBALDEF char	iiOODBA[FE_MAXNAME+1] ZERO_FILL;
GLOBALDEF i4    iiOODBAsize = sizeof(iiOODBA);
/*
**  Data fromt send.c
*/
GLOBALDEF	OO_METHOD	*_CurMeth = (OO_METHOD*) iiOOnil;
GLOBALDEF	OO_OBJECT	*Self = iiOOnil;
# endif             /* NT_GENERIC  */
