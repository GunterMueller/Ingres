# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*}
** Copyright (c) 2004 Ingres Corporation
** Name:	TBLINFO - structure for 1 piece of table information.
**
** Description:
**	an array of these guys gets filled in based on UIrel_help output.
**
** History:
**	25-may-1989 (pete) written.
*/
  typedef struct {
    bool disp;
    char *form_fldname;
    char value[FE_MAXNAME +1];
  } TBLINFO;
