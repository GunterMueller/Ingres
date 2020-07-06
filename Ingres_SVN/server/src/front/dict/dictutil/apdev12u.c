# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<pc.h>		/* 6-x_PC_80x86 */
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ui.h>
# include	<uigdata.h>
# include	<dictutil.h>
/**
** Name:	main.c -	Main Routine and Entry Point for module cleanup
**
** Description:
**	main		The main program.
**
** History:
**	4/90 (bobm) written.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
**	3-jan-1990 (pete)
**	    Make changes for Star support. If Star, run stmts on CDB.
**	26-feb-1991 (pete)
**	    1. Make changes for Gateways. Replaced call to inline routine:
**	    grant_all_to_dba with call to IIDDga_GrantAllToDba.
**	    2. Don't issue GRANT or MODIFY statements on Gateway; replace
**	    MODIFY statements with ~equivalent CREATE INDEX statement.
**	    3. Also, Change all SELECTs of the form:
**		SELECT col1, col2 = '', col3 = 1, col4 FROM ...
**	    to:
**		SELECT col1, '', 1, col4 FROM ...
**	    The "col = value" is an Ingres-ism and fails on Gateways.
**	    The two forms are equivalent on Ingres (I checked with Andre).
**	29-jul-1991 (pete)
**	    If connected to INGRES DBMS, then get exclusive read lock on each
**	    table to be DD_ALTER_TABLE by doing SET LOCKMODE statement.
**	    Change made in conjunction with removal of -l flag on connect stmt.
**	    Also, removed the COMMIT from temp_tab() -- apparently the logging
**	    requirements of "CREATE TABLE tmp AS SELECT * FROM tbl" are small
**	    (they are small for MODIFY stmt too); should be ok to add to
**	    transaction. Use exclusive table locks during upgrades. SIR 38903
**	    Changed PCexit(stat), where stat != OK to PCexit(FAIL).
**      16-nov-1992 (larrym)
**          Added CUFLIB to NEEDLIBS.  CUF is the new common utility facility.
**	8-jan-1993 (jpk)
**		maintained FErelexists, added second parameter
**		for owner of table (FIPS support)
**	22-july-1993 (jpk)
**		replaced call to FErelexists with call to cat_exists
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	29-Sep-2004 (drivi01)
**	    Updated NEEDLIBS to link dynamic library SHQLIB to replace
**	    its static libraries.
**/
/*{
** Name:	main() -	Main Entry Point for apdev12 upgrade.
**
** Description:
**	Performs upgrade of APDEV1 from version 1 to version 2.
**
** Side Effects:
**	Updates system catalogs.
**
** History:
**	4/90 (bobm) written.
*/
/*
**	MKMFIN Hints
PROGRAM =	ad102uin
NEEDLIBS =	DICTUTILLIB SHQLIB COMPATLIB SHEMBEDLIB
UNDEFS =	II_copyright
*/
FUNC_EXTERN bool        IIDDidIsDistributed();
FUNC_EXTERN i4		loadStarStmts();
FUNC_EXTERN STATUS      IIDDccCdbConnect();
FUNC_EXTERN STATUS      IIDDcdCdbDisconnect();
FUNC_EXTERN VOID	IIDDga_GrantAllToDba();
FUNC_EXTERN bool	IIDDigIsGateway();
FUNC_EXTERN VOID	IIDDserSetExclusiveReadlock();
FUNC_EXTERN VOID	IIDDssrSetSessionReadlock();
FUNC_EXTERN	STATUS	FErelexists(char *fetable, char *feowner);
FUNC_EXTERN	STATUS  cat_exists(char *catname, char *dummy);
static STATUS col_exists();
static VOID class_create();
static VOID seq_create();
static VOID dep_mod();
static VOID obj_mod();
static VOID jd_mod();
static VOID qbf_mod();
static VOID err_check();
static VOID temp_tab();
static VOID rem_temp();
static char *tempptr;
/*
** drive table for conversion.  We check to see if conversion has been
** done by seeing either if a table exists, or it has a new column.
** if "is_done" does not return OK, we call "convert".  The conversion
** routines must assure idempotency.  6.3 FE's will continue to work
** against partially upgraded DB's - they simply don't query the new
** tables or columns.  Once everything has either happened before or
** works, we're ready for 6.4
*/
DD_DRIVE Tab[] =
{
	{ ERx("ii_abfclasses"), NULL, cat_exists, class_create,
		 DD_CREATE_TABLE, FALSE },
	{ ERx("ii_sequence_values"), NULL, cat_exists, seq_create,
		 DD_CREATE_TABLE, FALSE },
	{ ERx("ii_abfdependencies"), ERx("abf_linkname"), col_exists, dep_mod,
		 DD_ALTER_TABLE, FALSE },
	{ ERx("ii_abfobjects"), ERx("abf_flags"), col_exists, obj_mod,
		 DD_ALTER_TABLE, FALSE },
	{ ERx("ii_joindefs"), ERx("qinfo5"), col_exists, jd_mod,
		 DD_ALTER_TABLE, FALSE },
	{ ERx("ii_qbfnames"), ERx("relowner"), col_exists, qbf_mod,
		 DD_ALTER_TABLE, FALSE },
};
i4
main(argc, argv)
i4	argc;
char	**argv;
{
	STATUS stat = OK;
	DD_DRIVE *t;
	i4 i;
	if ((stat = IIDDdbcDataBaseConnect(argc,argv)) != OK)
	    goto done;
/* # line 156 "apdev12u.sc" */	/* set */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"set AUTOCOMMIT OFF");
    IIsyncup((char *)0,0);
  }
/* # line 158 "apdev12u.sc" */	/* host code */
	if (IIDDidIsDistributed())
	{
            /* We're connected to Star; must do upgrade on CDB. */
            if ((stat = IIDDccCdbConnect()) != OK)
	    	goto done;
	}
	/* set table level lockmode to READLOCK = EXCLUSIVE */
	IIDDserSetExclusiveReadlock(Tab, (i4) (sizeof(Tab)/sizeof(Tab[0])));
	t = Tab;
	for (i=0; i < sizeof(Tab)/sizeof(Tab[0]); ++t,++i)
	{
		if ((*(t->is_done))(t->tab,t->col) == OK)
			continue;
		/*
		** redundant calls don't matter.  Do it this way so that
		** if a previous conversion routine turned it off, we
		** turn it on again.
		*/
		iiuicw1_CatWriteOn();
		(*(t->convert))();
/* # line 183 "apdev12u.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 185 "apdev12u.sc" */	/* host code */
		t->did_upgrade = TRUE;
	}
	rem_temp();
	_VOID_ IIUIffuFidFixUp();
/* # line 192 "apdev12u.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 194 "apdev12u.sc" */	/* host code */
	/* reset table lockmode to session defaults */
	IIDDssrSetSessionReadlock(Tab, (i4) (sizeof(Tab)/sizeof(Tab[0])));
	if (IIDDidIsDistributed())
	{
            /* Above work was done on CDB; register it with Star. */
	    _VOID_ loadStarStmts( sizeof(Tab)/sizeof(Tab[0]), Tab);
            if ((stat = IIDDcdCdbDisconnect()) != OK)  /* note: this COMMITs */
	        goto done;
	}
done:
	iiuicw0_CatWriteOff();
	PCexit(stat == OK ? OK : FAIL);
}
/*
** this routine !! does NOT !! return for failure to fetch the info.
** we return OK to indicate that the column exists, other to indicate that
** we should try to add it.
*/
static STATUS
col_exists(tab,col)
  char *tab;
  char *col;
{
  i4 cnt;
	STATUS stat;
/* # line 230 "apdev12u.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"apdev12u1",7673,20957);
      IIputdomio((short *)0,1,32,0,tab);
      IIputdomio((short *)0,1,32,0,col);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"apdev12u1",7673,20957);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iicolumns where table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,tab);
        IIwritio(0,(short *)0,1,32,0,(char *)"and column_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,col);
        IIexDefine(0,(char *)"apdev12u1",7673,20957);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 236 "apdev12u.sc" */	/* host code */
	if ((stat = FEinqerr()) != OK)
		PCexit(FAIL);
	if (cnt > 0)
		return OK;
	return FAIL;
}
static VOID
class_create()
{
/* # line 248 "apdev12u.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table ii_abfclasses(appl_id integer not null, class_id integer\
 not null, catt_id integer not null, class_order smallint not null, ad\
f_type integer not null, type_name varchar(32)not null, adf_length int\
eger not null, adf_scale integer not null)with");
    IIwritio(0,(short *)0,1,32,0,(char *)" NODUPLICATES");
    IIsyncup((char *)0,0);
  }
/* # line 259 "apdev12u.sc" */	/* host code */
	err_check();
	if (IIDDigIsGateway() == FALSE)
	{
	    /* Can't issue GRANT or MODIFY stmts on Ingres/Gateway */
	    IIDDga_GrantAllToDba(ERx("ii_abfclasses"), &tempptr);
	    err_check();
/* # line 269 "apdev12u.sc" */	/* modify */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify ii_abfclasses to CBTREE unique on class_id, catt_id");
    IIsyncup((char *)0,0);
  }
/* # line 271 "apdev12u.sc" */	/* host code */
	    err_check();
	}
	else
	{
	    /* On Gateways we create an Index equivalent to the MODIFY */
/* # line 277 "apdev12u.sc" */	/* create unique index */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create unique index ii_abfclasses_mod on ii_abfclasses(class_id, catt\
_id)");
    IIsyncup((char *)0,0);
  }
/* # line 280 "apdev12u.sc" */	/* host code */
	    err_check();
	}
}
static VOID
seq_create()
{
	STATUS stat;
/* # line 289 "apdev12u.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table ii_sequence_values(sequence_key char(32)not null, sequen\
ce_value integer not null)with NODUPLICATES");
    IIsyncup((char *)0,0);
  }
/* # line 294 "apdev12u.sc" */	/* host code */
	err_check();
	if (IIDDigIsGateway() == FALSE)
	{
	    /* Can't issue GRANT or MODIFY stmts on Ingres/Gateway */
	    IIDDga_GrantAllToDba(ERx("ii_sequence_values"), &tempptr);
	    err_check();
/* # line 304 "apdev12u.sc" */	/* modify */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify ii_sequence_values to HASH unique on sequence_key where minpag\
es=32, fillfactor=1");
    IIsyncup((char *)0,0);
  }
/* # line 307 "apdev12u.sc" */	/* host code */
	    err_check();
	}
	else
	{
	    /* On Gateways we create an Index equivalent to the MODIFY */
	    /* 18 character name limit; truncate name and add "_mod" */
/* # line 315 "apdev12u.sc" */	/* create unique index */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create unique index ii_sequence_va_mod on ii_sequence_values(sequence\
_key)");
    IIsyncup((char *)0,0);
  }
/* # line 318 "apdev12u.sc" */	/* host code */
	    err_check();
	}
}
static VOID
dep_mod()
{
	temp_tab("ii_abfdependencies");
/* # line 327 "apdev12u.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table ii_abfdependencies(object_id integer not null, abfdef_na\
me varchar(32)not null, abfdef_owner varchar(32)not null, object_class\
 integer not null, abfdef_deptype integer not null, abf_linkname varch\
ar(32)with null, abf_deporder integer with ");
    IIwritio(0,(short *)0,1,32,0,(char *)"null)with NODUPLICATES");
    IIsyncup((char *)0,0);
  }
/* # line 337 "apdev12u.sc" */	/* host code */
	err_check();
/* # line 339 "apdev12u.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_abfdependencies(object_id, abfdef_name, abfdef_owner, \
object_class, abfdef_deptype, abf_linkname, abf_deporder)select object\
_id, abfdef_name, abfdef_owner, object_class, abfdef_deptype, '', 0 fr\
om ii_temp_apdev12");
    IIsyncup((char *)0,0);
  }
/* # line 357 "apdev12u.sc" */	/* host code */
	err_check();
	if (IIDDigIsGateway() == FALSE)
	{
	    /* Can't issue GRANT or MODIFY stmts on Ingres/Gateway */
	    IIDDga_GrantAllToDba(ERx("ii_abfdependencies"), &tempptr);
	    err_check();
/* # line 367 "apdev12u.sc" */	/* modify */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify ii_abfdependencies to BTREE on object_id");
    IIsyncup((char *)0,0);
  }
/* # line 369 "apdev12u.sc" */	/* host code */
	    err_check();
	}
	else
	{
	    /* On Gateways we create an Index equivalent to the MODIFY */
	    /* 18 character name limit; truncate tbl name and add "_mod" */
/* # line 377 "apdev12u.sc" */	/* create index */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create index ii_abfdependen_mod on ii_abfdependencies(object_id)");
    IIsyncup((char *)0,0);
  }
/* # line 380 "apdev12u.sc" */	/* host code */
	    err_check();
	}
}
static VOID
obj_mod()
{
	temp_tab("ii_abfobjects");
/* # line 389 "apdev12u.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table ii_abfobjects(applid integer not null, object_id integer\
 not null, abf_source varchar(180)not null, abf_symbol varchar(32)not \
null, retadf_type smallint not null, rettype varchar(32)not null, retl\
ength integer not null, retscale integer not ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"null, abf_version smallint not null, abf_flags integer with null, abf\
_arg1 varchar(48)not null, abf_arg2 varchar(48)not null, abf_arg3 varc\
har(48)not null, abf_arg4 varchar(48)not null, abf_arg5 varchar(48)not\
 null, abf_arg6 varchar(48)not null)with ");
    IIwritio(0,(short *)0,1,32,0,(char *)"NODUPLICATES");
    IIsyncup((char *)0,0);
  }
/* # line 408 "apdev12u.sc" */	/* host code */
	err_check();
/* # line 410 "apdev12u.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_abfobjects(applid, object_id, abf_source, abf_symbol, \
retadf_type, rettype, retlength, retscale, abf_version, abf_flags, abf\
_arg1, abf_arg2, abf_arg3, abf_arg4, abf_arg5, abf_arg6)select applid,\
 object_id, abf_source, abf_symbol, ");
    IIwritio(0,(short *)0,1,32,0,(char *)
"retadf_type, rettype, retlength, retscale, abf_version, 0, abf_arg1, \
abf_arg2, abf_arg3, abf_arg4, abf_arg5, abf_arg6 from ii_temp_apdev12");
    IIsyncup((char *)0,0);
  }
/* # line 446 "apdev12u.sc" */	/* host code */
	err_check();
	if (IIDDigIsGateway() == FALSE)
	{
	    /* Can't issue GRANT or MODIFY stmts on Ingres/Gateway */
	    IIDDga_GrantAllToDba(ERx("ii_abfobjects"), &tempptr);
	    err_check();
/* # line 456 "apdev12u.sc" */	/* modify */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify ii_abfobjects to CBTREE on applid, object_id");
    IIsyncup((char *)0,0);
  }
/* # line 458 "apdev12u.sc" */	/* host code */
	    err_check();
	}
	else
	{
	    /* On Gateways we create an Index equivalent to the MODIFY */
/* # line 464 "apdev12u.sc" */	/* create index */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create index ii_abfobjects_mod on ii_abfobjects(applid, object_id)");
    IIsyncup((char *)0,0);
  }
/* # line 467 "apdev12u.sc" */	/* host code */
	    err_check();
	}
}
static VOID
jd_mod()
{
	temp_tab("ii_joindefs");
/* # line 476 "apdev12u.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table ii_joindefs(object_id integer not null, qtype integer no\
t null, qinfo1 varchar(32)not null, qinfo2 varchar(32)not null, qinfo3\
 varchar(32)not null, qinfo4 varchar(32)not null, qinfo5 varchar(32)wi\
th null)");
    IIsyncup((char *)0,0);
  }
/* # line 486 "apdev12u.sc" */	/* host code */
	err_check();
/* # line 488 "apdev12u.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_joindefs(object_id, qtype, qinfo1, qinfo2, qinfo3, qin\
fo4, qinfo5)select object_id, qtype, qinfo1, qinfo2, qinfo3, qinfo4, '\
' from ii_temp_apdev12");
    IIsyncup((char *)0,0);
  }
/* # line 506 "apdev12u.sc" */	/* host code */
	err_check();
	if (IIDDigIsGateway() == FALSE)
	{
	    /* Can't issue GRANT or MODIFY stmts on Ingres/Gateway */
	    IIDDga_GrantAllToDba(ERx("ii_joindefs"), &tempptr);
	    err_check();
/* # line 516 "apdev12u.sc" */	/* modify */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify ii_joindefs to CBTREE unique on object_id, qtype");
    IIsyncup((char *)0,0);
  }
/* # line 518 "apdev12u.sc" */	/* host code */
	    err_check();
	}
	else
	{
	    /* On Gateways we create an Index equivalent to the MODIFY */
/* # line 524 "apdev12u.sc" */	/* create unique index */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create unique index ii_joindefs_mod on ii_joindefs(object_id, qtype)");
    IIsyncup((char *)0,0);
  }
/* # line 527 "apdev12u.sc" */	/* host code */
	    err_check();
	}
}
static VOID
qbf_mod()
{
	temp_tab("ii_qbfnames");
/* # line 536 "apdev12u.sc" */	/* create table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create table ii_qbfnames(object_id integer not null, relname varchar(\
32)not null, relowner varchar(32)with null, frname varchar(32)not null\
, qbftype smallint not null)");
    IIsyncup((char *)0,0);
  }
/* # line 544 "apdev12u.sc" */	/* host code */
	err_check();
/* # line 546 "apdev12u.sc" */	/* insert */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_qbfnames(object_id, relname, relowner, frname, qbftype\
)select object_id, relname, '', frname, qbftype from ii_temp_apdev12");
    IIsyncup((char *)0,0);
  }
/* # line 560 "apdev12u.sc" */	/* host code */
	err_check();
	if (IIDDigIsGateway() == FALSE)
	{
	    /* Can't issue GRANT or MODIFY stmts on Ingres/Gateway */
	    IIDDga_GrantAllToDba(ERx("ii_qbfnames"), &tempptr);
	    err_check();
/* # line 570 "apdev12u.sc" */	/* modify */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"modify ii_qbfnames to CBTREE unique on object_id");
    IIsyncup((char *)0,0);
  }
/* # line 572 "apdev12u.sc" */	/* host code */
	    err_check();
	}
	else
	{
	    /* On Gateways we create an Index equivalent to the MODIFY */
/* # line 578 "apdev12u.sc" */	/* create unique index */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"create unique index ii_qbfnames_mod on ii_qbfnames(object_id)");
    IIsyncup((char *)0,0);
  }
/* # line 581 "apdev12u.sc" */	/* host code */
	    err_check();
	}
}
/*
** copy catalog into ii_temp_apdev12 and drop it.
*/
static VOID
temp_tab(tname)
char *tname;
{
  char cmd[120];
	rem_temp();
	STprintf(cmd,
		ERx("create table ii_temp_apdev12 as select * from %s"),
		tname);
/* # line 602 "apdev12u.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(cmd);
    IIsyncup((char *)0,0);
  }
/* # line 604 "apdev12u.sc" */	/* host code */
	err_check();
	/* keep transaction open */
	STprintf(cmd,"DROP TABLE %s",tname);
/* # line 610 "apdev12u.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(cmd);
    IIsyncup((char *)0,0);
  }
/* # line 612 "apdev12u.sc" */	/* host code */
	err_check();
}
static VOID
rem_temp()
{
	if (cat_exists(ERx("ii_temp_apdev12"), "") == OK)
	{
/* # line 620 "apdev12u.sc" */	/* drop */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)"drop table ii_temp_apdev12");
    IIsyncup((char *)0,0);
  }
/* # line 622 "apdev12u.sc" */	/* host code */
		err_check();
	}
}
static VOID
err_check()
{
	STATUS stat;
	stat = FEinqerr();
	if (stat != OK)
	{
/* # line 635 "apdev12u.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 636 "apdev12u.sc" */	/* host code */
		PCexit(FAIL);
	}
}
