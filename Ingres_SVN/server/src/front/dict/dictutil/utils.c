# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<er.h>
# include	<st.h>
# include	<fe.h>
# include       <ui.h>
# include	<ug.h>
# include       <uigdata.h>
# include       <generr.h>
# include       <dictutil.h>
# include       "erdu.h"
/**
** Name:	utils.qsc - Dictionary  utilities
**
** Description:
**	These utilities allow nested transactions without the need for any
**	user of them to be aware of whether they are currently in a
**	transaction.  As long as all paths through any procedure match
**	either a call to IIDDenxEndNestedXact or IIDDanxAbortNestedXact with
**	each call to IIDDbnxBeginNestedXact then these are GUARANTEED to work.
**	(Lifetime guarantee not valid in Minnesota or Alaska; check your
**	local dealer for details).
**	This file defines:
**
**	IIDDbnxBeginNestedXact	begin a nested transaction
**	IIDDenxEndNestedXact	end a nested transaction
**	IIDDanxAbortNestedXact	abort a nested transaction
**	IIDDnr1NestedReadOn	execute before catalog read (possibly in xact)
**	IIDDnr0NestedReadOff	execute after catalog read (possibly in xact)
**	IIDDnw1NestedWriteOn	execute before catalog write (possibly in xact)
**	IIDDnr0NestedReadOff	execute after catalog write (possibly in xact)
**
**	IIDDga_GrantAllToDba	grant all privs on a table to the DBA.
**	IIDDserSetExclusiveReadlock
**	IIDDssrSetSessionReadlock
**
** History:
**	1/5/90 rdesmond		First Written
**	2/26/91	(pete)
**		Changed to .qsc file and added routine: IIDDga_GrantAllToDba.
**	29-jul-1991 (pete)
**		Added routines IIDDserSetExclusiveReadlock and
**		IIDDssrSetSessionReadlock. SIR 38903.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
static	i4	xact_nesting_level = 0;
static	i4	catwrite_nesting_level = 0;
  static char Stmtbuf[100];
VOID
IIDDbnxBeginNestedXact()
{
    if (xact_nesting_level++ == 0)
	IIUIbeginXaction();
    return;
}
VOID
IIDDenxEndNestedXact()
{
    if (--xact_nesting_level == 0)
        IIUIendXaction();
    if (xact_nesting_level < 0)
	xact_nesting_level = 0;
    return;
}
VOID
IIDDanxAbortNestedXact()
{
    if (xact_nesting_level != 0)
    {
        IIUIabortXaction();
        xact_nesting_level = 0;
    }
    return;
}
VOID
IIDDnr1NestedReadOn(catname)
char	*catname;
{
    /* may only be called when not in a transaction */
    if (xact_nesting_level == 0)
	iiuicr1_CatReadOn(catname);
    return;
}
VOID
IIDDnr0NestedReadOff(catname)
char	*catname;
{
    /* may only be called when not in a transaction */
    if (xact_nesting_level == 0)
	iiuicr0_CatReadOff(catname);
    return;
}
VOID
IIDDnw1NestedWriteOn()
{
    /* only needs to be called when not currently in protected mode */
    if (catwrite_nesting_level++ == 0)
	iiuicw1_CatWriteOn();
    return;
}
VOID
IIDDnw0NestedWriteOff()
{
    /* may only be called when not in a transaction */
    if (--catwrite_nesting_level == 0)
	iiuicw0_CatWriteOff();
    return;
}
/*{
** Name:	IIDDga_GrantAllToDba - grant all privs on table to DBA.
**
** Description:
**	issue a "grant all on <tablename> to <dbaname>" statement.
**
** Inputs:
**	table_name	- name of table to issue grant on.
**	ptr		- (this is an Output -- see below)
**
** Outputs:
**
**	ptr		- write statement buffer here.
**
**	Returns:
**		VOID
**
** Side Effects:
**
** History:
**	feb-26-1991 (pete)
**		Initial Version.
*/
VOID
IIDDga_GrantAllToDba(table_name, ptr)
char    *table_name;
char	**ptr;
{
        STcopy(ERx("GRANT ALL ON "), Stmtbuf);
        STcat(Stmtbuf, table_name);
        STcat(Stmtbuf, ERx(" TO '"));
        STcat(Stmtbuf, IIUIdbdata()->dba);
        STcat(Stmtbuf, ERx("'"));
	*ptr = Stmtbuf;
/* # line 155 "utils.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(Stmtbuf);
    IIsyncup((char *)0,0);
  }
/* # line 156 "utils.sc" */	/* host code */
}
/*{
** Name:	IIDDserSetExclusiveReadlock - Set exclusive readlocks
**
** Description:
**	Set table level readlock = exclusive locking for tables that
**	will be altered.
**
** Inputs:
**	DD_DRIVE        *t	array of table upgrade info.
**	i4             tsize	number of items in array.
**
** Outputs:
**
**	Returns:
**		VOID
**
** Side Effects:
**
**	COMMITs transaction.
**
** History:
**	jul-29-1991 (pete)
**		Initial Version.
*/
VOID
IIDDserSetExclusiveReadlock(t, tsize)
DD_DRIVE        *t;
i4              tsize;
{
  char cmd[120];
        i4  i;
        if ((IIUIdcn_ingres() == TRUE) || (IIDDidIsDistributed()) )
        {
            /* We're attached to either INGRES or Star. OK to issue
            ** SET LOCKMODE statements.
            */
            /* COMMIT just in case (SET LOCKMODE not valid in transaction). */
/* # line 196 "utils.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 197 "utils.sc" */	/* host code */
            for (i=0; i < tsize; ++t,++i)
            {
                /* only want to set lockmode on tables we'll be doing an
                ** alter table type operation on.
                */
                if (t->upgrade_type == DD_ALTER_TABLE)
                {
                    STprintf(cmd,
            ERx("SET LOCKMODE ON %s WHERE LEVEL = TABLE, READLOCK = EXCLUSIVE"),
                        t->tab);
/* # line 207 "utils.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(cmd);
    IIsyncup((char *)0,0);
  }
/* # line 208 "utils.sc" */	/* host code */
                    /* don't care about errors */
                }
            }
/* # line 211 "utils.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 212 "utils.sc" */	/* host code */
        }
}
/*{
** Name:	IIDDssrSetSessionReadlock - Set readlocks back to default
**
** Description:
**	Set table level readlock back to session defaults. Called by module
**	upgrade programs.
**
** Inputs:
**	DD_DRIVE        *t	array of table upgrade info.
**	i4             tsize	number of items in array.
**
** Outputs:
**
**	Returns:
**		VOID
**
** Side Effects:
**
**	COMMITs transaction.
**
** History:
**	jul-29-1991 (pete)
**		Initial Version.
*/
VOID
IIDDssrSetSessionReadlock(t, tsize)
DD_DRIVE        *t;
i4              tsize;
{
  char cmd[120];
        i4  i;
        if ((IIUIdcn_ingres() == TRUE) || (IIDDidIsDistributed()) )
        {
            /* We're attached to either INGRES or Star. OK to issue
            ** SET LOCKMODE statements.
            */
            /* COMMIT just in case (SET LOCKMODE not valid in transaction). */
/* # line 253 "utils.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 254 "utils.sc" */	/* host code */
            for (i=0; i < tsize; ++t,++i)
            {
                /* only want to set lockmode on tables we'll be doing an
                ** alter table type operation on.
                */
                if (t->upgrade_type == DD_ALTER_TABLE)
                {
                    STprintf(cmd,
            ERx("SET LOCKMODE ON %s WHERE LEVEL = SESSION, READLOCK = SESSION"),
                        t->tab);
/* # line 264 "utils.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(cmd);
    IIsyncup((char *)0,0);
  }
/* # line 265 "utils.sc" */	/* host code */
                    /* don't care about errors */
                }
            }
/* # line 268 "utils.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 269 "utils.sc" */	/* host code */
        }
}
