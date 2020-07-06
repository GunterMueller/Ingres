# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <gl.h>
# include <iicommon.h>
# include <fe.h>
# include <ug.h>
# include <rpu.h>
# include "errm.h"
/**
** Name:	tblexist.sc - does table exist
**
** Description:
**	Defines
**		table_exist	- does table exist?
**
** History:
**	16-dec-96 (joea)
**		Created based on tblexist.sc in replicator library.
**	21-sep-98 (abbjo03)
**		Replace ddba_messageit with IIUGerr. Use RPdb_error_check
**		instead of INQUIRE_SQL.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/*{
** Name:	table_exist - does table exist?
**
** Description:
**	Find out if the specified table exists.
**
** Inputs:
**	table_no	- table number
**
** Outputs:
**	none
**
** Returns:
**	OK or E_RM00DB_Err_ck_exist
*/
STATUS
table_exists(
char	*tablename,
char	*tableowner,
i4	*exists)
# if 0
  char *tablename;
  char *tableowner;
# endif
{
  i4 row_count = 0;
/* # line 63 "tblexist.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iitables where table_name=");
    IIputdomio((short *)0,1,32,0,tablename);
    IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
    IIputdomio((short *)0,1,32,0,tableowner);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&row_count);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 68 "tblexist.sc" */	/* host code */
	if (RPdb_error_check(0, NULL) != OK)
	{
		IIUGerr(E_RM00DB_Err_ck_exist, UG_ERR_ERROR, 2, tableowner,
			tablename);
		return (E_RM00DB_Err_ck_exist);
	}
	*exists = row_count;
	return (OK);
}
