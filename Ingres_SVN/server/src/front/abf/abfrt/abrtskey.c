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
#include	<ug.h>
#include	<erar.h>
/**
** Name:	abrtskey.sc -	ABF Run-Time Surrogate Key Function.
**
** Description:
**	Contains the routine used to implement the surrogate key system
**	function.  This function returns a surrogate key.  Define:
**
**	IIARsurrogate_key()	return surrogate key for table.
**
** History:
**	Revision 8.0  89/06  wong
**	Initial revision.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/*{
** Name:	IIARsurrogate_key() -	Return Surrogate Key for Table.
**
** Description:
**	Returns a surrogate key for a table.  This key is an integer in the
**	range of 2^31 - 1 to -2^31.  The zero key is the error key and will
**	be returned on error or when no more keys can be allocated.
**
**	This function calls a database procedure if the DBMS is INGRES.
**	Otherwise, it executes the appropriate queries directly.
**
**	The UPDATE detects that the row exists, that another key can be
**	allocated, and whether the table exists.  If this fails, then
**	either the table does not exist or no more keys can be allocated.
**	If the row for the table does not exist, but the table does exist,
**	then the row will be added to the surrogate key table.
**
**	This procedure does not account for tables that exist, but which
**	do not have INSERT or UPDATE priveleges granted.
**
** Input:
**	table_name	{char *}  The name of the table.
**
** Returns:
**	{i4}	0 on error, otherwise, the key number.
**
** History:
**	06/89 (jhw) -- Written.
**	07-oct-1993 (mgw)
**		Added <fe.h> include because of new TAGID dependency on it
**		in <ug.h>
*/
#define E_US0845_2117	0x845
i4
IIARsurrogate_key ( table_name, test )
  char *table_name;
bool	test;
{
  i4 key;
#ifndef DEBUG
#define test	TRUE
#endif
	iiuicw1_CatWriteOn();
	if ( test && IIUIdcn_ingres() )
	{ /* database procedure */
/* # line 87 "abrtskey.sc" */	/* execute procedure */
  {
    IIsqInit((char *)0);
    IILQpriProcInit(2,(char *)"ii_surrogate_key");
    IILQprvProcValio((char *)"table_name",0,(short *)0,1,32,0,table_name);
    while (IILQprsProcStatus(0) != 0) {
    } /* IILQprsProcStatus */
    IIeqiqio((short *)0,1,30,4,&key,(char *)"iiret");
  }
/* # line 89 "abrtskey.sc" */	/* host code */
		if ( key == 0 && FEinqerr() == OK )
		{ /* not found or key wrapped */
/* # line 91 "abrtskey.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select surrogate_key from ii_surrogate_key where table_name in(select\
 table_name from iitables where table_name=");
    IIputdomio((short *)0,1,32,0,table_name);
    IIwritio(0,(short *)0,1,32,0,(char *)")");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&key);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 97 "abrtskey.sc" */	/* host code */
			if ( FEinqerr() == OK && FEinqrows() == 0 )
			{ /* definitely not found */
				IIUGerr( E_US0845_2117, UG_ERR_ERROR,
						1, (PTR)table_name
				);
			}
			return 0;
		}
	}
	else
	{ /* in-line */
/* # line 109 "abrtskey.sc" */	/* repeated update */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"abrtskey1",6280,16361);
      IIputdomio((short *)0,1,32,0,table_name);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"abrtskey1",6280,16361);
        IIwritio(0,(short *)0,1,32,0,(char *)
"update ii_surrogate_key set surrogate_key=surrogate_key +1 where surr\
ogate_key<>0 and table_name in(select table_name from iitables where t\
able_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,table_name);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"abrtskey1",6280,16361);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 115 "abrtskey.sc" */	/* host code */
		if ( FEinqerr() != OK )
			return 0;
		else if ( FEinqrows() == 0 )
		{ /* check for table existence */
  i4 _exists;
/* # line 123 "abrtskey.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iitables where table_name=");
    IIputdomio((short *)0,1,32,0,table_name);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&_exists);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 125 "abrtskey.sc" */	/* host code */
			if ( FEinqerr() != OK || _exists == 0 )
			{ /* table does not exist */
				IIUGerr( E_US0845_2117, UG_ERR_ERROR,
						1, (PTR)table_name
				);
				return 0;
			}
/* # line 132 "abrtskey.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select surrogate_key from ii_surrogate_key where table_name=");
    IIputdomio((short *)0,1,32,0,table_name);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&key);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 135 "abrtskey.sc" */	/* host code */
			if ( FEinqerr() != OK ||
					( FEinqrows() != 0 && key == 0 ) )
			{ /* no more keys */
				return 0;
			}
			/* Add row for table */
/* # line 141 "abrtskey.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_surrogate_key values(");
    IIputdomio((short *)0,1,32,0,table_name);
    IIwritio(0,(short *)0,1,32,0,(char *)", 1)");
    IIsyncup((char *)0,0);
  }
/* # line 143 "abrtskey.sc" */	/* host code */
			if ( FEinqerr() != OK || FEinqrows() == 0 )
				return 0;
		}
/* # line 146 "abrtskey.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"abrtskey2",6281,12525);
      IIputdomio((short *)0,1,32,0,table_name);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"abrtskey2",6281,12525);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select surrogate_key from ii_surrogate_key where table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,table_name);
        IIexDefine(0,(char *)"abrtskey2",6281,12525);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&key);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 149 "abrtskey.sc" */	/* host code */
		if ( FEinqerr() != OK || FEinqrows() == 0 )
			return 0;
	}
	iiuicw0_CatWriteOff();
	return key;
}
