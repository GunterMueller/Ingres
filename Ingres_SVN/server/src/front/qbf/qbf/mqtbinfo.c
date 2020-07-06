# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
#include	<adf.h>
#include	<afe.h>
#include	<ui.h>
#include	<ug.h>
#include	"erqf.h"
/**
** Name:    mqtblinfo.qsc -	 This module performs special table lookup
**			 functions.
**
**	Routines:
**	     mqgetcols()   -Get list of attributes in a table
**
**	Written: 7/14/83 (nml)
*/
/*{
** Name:	mqgetcols()
**
** History:
**	30-sep-1986 (sandyd)
**		Saturn changes.	 Must connect to local database before
**		retrieving from "attribute" relation.
**		Failure from FEconnect()
**		is treated as a "table not found" condition.
**	7/87 (peterk) changed accesses to "relation" and "attribute" to
**		calls on FErel_* and FEatt_* routines.
**	01-jan-88 (kenl)
**		Turned file into a .QSC file.
**	28-jan-1988 (peter)
**		Make performance changes using FEatt_fopen.  Also, change
**		the tname parameter to a FE_REL_INFO for performance.
**	14-apr-1988 (danielt)
**		added commit work after FEatt* calls.
**	14-mar-89 (kenl)
**		When running against a gateway, convert datatypes to common.
**	23-dec-1993 (rdrane)
**		Don't forget to propagate the precision in the DB_DATA_VALUE
**		(b57864).
*/
bool
  mqgetcols(relinfo, fname, tfname)
FE_REL_INFO	*relinfo;
  char *fname;
  char *tfname;
  {
    FE_ATT_QBLK	aqblk;
    ADF_CB	*FEadfcb();
/* # line 62 "mqtbinfo.qsc" */	/* message */
    {
      IImessage(ERget(S_QF007C_Retrieving_column_inf));
    }
/* # line 63 "mqtbinfo.qsc" */	/* host code */
    if (FEatt_fopen(&aqblk, relinfo) == OK)
    {
	FE_ATT_INFO	attinfo;
	ADF_CB		*cb;
	cb = FEadfcb();
/* # line 69 "mqtbinfo.qsc" */	/* inittable */
    {
      if (IItbinit(fname,tfname,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 70 "mqtbinfo.qsc" */	/* host code */
	while (FEatt_fetch(&aqblk, &attinfo) == OK)
	{
	    DB_DATA_VALUE	dbv;
	    DB_USER_TYPE	text;
    char *c_name;
    char *c_form;
	    c_name = attinfo.column_name;
	    dbv.db_datatype = attinfo.adf_type;
	    dbv.db_length = attinfo.intern_length;
	    dbv.db_prec = attinfo.intern_prec;
	    if (IIUIdml() == UI_DML_GTWSQL)
	    {
		_VOID_ IIAFgtm_gtwTypeMap(&dbv, &text);
		c_form = text.dbut_name;
	    }
	    else if (afe_tyoutput(cb, &dbv, &text) != OK)
	    {
		IIUGerr(E_QF0135_unrecognized_datatype,0,1,c_name);
		c_form = ERx(" ??? ");
	    }
	    else
	    {
		c_form = text.dbut_name;
	    }
/* # line 97 "mqtbinfo.qsc" */	/* loadtable */
    {
      if (IItbact(fname,tfname,1) != 0) {
        IItcoputio((char *)"colname",(short *)0,1,32,0,c_name);
        IItcoputio((char *)"datatype",(short *)0,1,32,0,c_form);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 101 "mqtbinfo.qsc" */	/* host code */
	}
	_VOID_ FEatt_close(&aqblk);
	return TRUE;
    }
    return FALSE;
  }
