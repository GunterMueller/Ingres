# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <cm.h>
# include <er.h>
# include <gl.h>
# include <iicommon.h>
# include <rpu.h>
/**
** Name:	ckunqkey.sc - check unique keys
**
** Description:
**	Defines
**		check_unique_keys	- check for unique keys
**		check_indexes		- check for unique indexes
**
** History:
**	30-dec-96 (joea)
**		Created based on ckunqkey.sc in replicator library.
**	20-jan-97 (joea)
**		Add msg_func parameter to check_unique_keys and check_indexes.
**	21-jun-97 (joea)
**		If table doesn't exist, return OK.  This can occur with
**		horizontally partitioned tables.
**	02-oct-97 (joea) bug 86076
**		Do not single-quote delimit table and owner names in
**		check_indexes, since the precompiler does that already.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
typedef void MSG_FUNC(i4 msg_level, i4  msg_num, ...);
static STATUS check_indexes(char *table_name, char *table_owner, i4 table_no,
	MSG_FUNC *msg_func);
/*{
** Name:	check_unique_keys - check for unique keys
**
** Description:
**	Checks to see if keys or indexes force the columns marked as
**	replication keys to be unique.
**
** Inputs:
**	table_name	- the name of the table to be checked
**	table_owner	- the name of the owner
**	table_no	- the table number
**	dbname		- the name of the database in which the table is to be
**			  found
**	msg_func	- function to display messages
**
** Outputs:
**	none
**
** Returns:
**	OK - table has unique keys
**	-1 - no unique keys
**	1523 - INGRES error looking for table
**	1524 - INGRES error looking up columns
**	Other error returns from check_indexes (see below)
*/
STATUS
check_unique_keys(
char	*table_name,
char	*table_owner,
i4	table_no,
char	*dbname,
MSG_FUNC	*msg_func)
# if 0
  char *table_name;
  char *table_owner;
  i4 table_no;
# endif
{
  char unique_rule[2];
  char table_indexes[2];
  i4 cnt1 = 0;
  i4 cnt2 = 0;
  i4 err = 0;
/* # line 92 "ckunqkey.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"ckunqkey1",7194,1781);
      IIputdomio((short *)0,1,32,0,table_name);
      IIputdomio((short *)0,1,32,0,table_owner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"ckunqkey1",7194,1781);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select unique_rule, table_indexes from iitables where table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,table_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,table_owner);
        IIexDefine(0,(char *)"ckunqkey1",7194,1781);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,1,unique_rule);
      IIgetdomio((short *)0,1,32,1,table_indexes);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 97 "ckunqkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
    IILQisInqSqlio((short *)0,1,30,4,&cnt1,8);
  }
/* # line 98 "ckunqkey.sc" */	/* host code */
	if (err)
	{
		(*msg_func)(1, 1523, err, table_owner, table_name, dbname);
		return (1523);
	}
	else if (cnt1 == 0)
	{
		/* table doesn't exist. Probably horizontal partition */
		return (OK);
	}
	if (CMcmpcase(unique_rule, ERx("U")) == 0)
	{
/* # line 111 "ckunqkey.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"ckunqkey2",7195,3572);
      IIputdomio((short *)0,1,32,0,table_name);
      IIputdomio((short *)0,1,32,0,table_owner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"ckunqkey2",7195,3572);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iicolumns where table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,table_name);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and key_sequence>0 and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,table_owner);
        IIexDefine(0,(char *)"ckunqkey2",7195,3572);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt1);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 116 "ckunqkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 117 "ckunqkey.sc" */	/* host code */
		if (err)
		{
			(*msg_func)(1, 1524, err, table_owner, table_name,
				dbname);
			return (1524);
		}
/* # line 123 "ckunqkey.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"ckunqkey3",7196,1001);
      IIputdomio((short *)0,1,32,0,table_name);
      IIputdomio((short *)0,1,32,0,table_owner);
      IIputdomio((short *)0,1,30,4,&table_no);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"ckunqkey3",7196,1001);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iicolumns i, dd_regist_columns d where i.column_n\
ame=d.column_name and i.table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,table_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and i.table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,table_owner);
        IIwritio(0,(short *)0,1,32,0,(char *)"and d.table_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&table_no);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and i.key_sequence>0 and d.key_sequence>0");
        IIexDefine(0,(char *)"ckunqkey3",7196,1001);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 131 "ckunqkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 132 "ckunqkey.sc" */	/* host code */
		if (err)
		{
			(*msg_func)(1, 1524, err, table_owner, table_name,
				dbname);
			return (1524);
		}
		if (cnt1 == cnt2)	/* base table makes keys unique */
			return (OK);
	}
	if (CMcmpcase(table_indexes, ERx("Y")) == 0)
		return (check_indexes(table_name, table_owner, table_no,
			msg_func));
	return (-1);
}
/*{
** Name:	check_indexes - check for unique indexes
**
** Description:
**	Checks to see if indexes force the columns marked as
**	replication keys to be unique.
**
** Inputs:
**	table_name	- the name of the table to be checked
**	table_owner	- the name of the owner
**	table_no	- the table number
**	msg_func	- function to display messages
**
** Outputs:
**	none
**
** Returns:
**	OK - the table has a unique index
**	1527 - Cursor open error
**	1525 - Cursor fetch error
**	-1 - table does not have unique indexes
**/
static STATUS
check_indexes(
char	*table_name,
char	*table_owner,
i4	table_no,
MSG_FUNC	*msg_func)
# if 0
  char *table_name;
  char *table_owner;
  i4 table_no;
# endif
{
  char index_name[DB_MAXNAME+1];
  char index_owner[DB_MAXNAME+1];
  i4 cnt1 = 0;
  i4 cnt2 = 0;
  i4 err = 0;
  i4 last = 0;
/* # line 199 "ckunqkey.sc" */	/* open */
  {
    IIsqInit((char *)0);
    IIcsOpen((char *)"c3",7197,23034);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select index_name, index_owner from iiindexes where base_name=");
    IIputdomio((short *)0,1,32,0,table_name);
    IIwritio(0,(short *)0,1,32,0,(char *)"and base_owner=");
    IIputdomio((short *)0,1,32,0,table_owner);
    IIwritio(0,(short *)0,1,32,0,(char *)"and unique_rule='U'");
    IIwritio(0,(short *)0,1,32,0,(char *)" for readonly ");
    IIcsQuery((char *)"c3",7197,23034);
  }
/* # line 200 "ckunqkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
  }
/* # line 201 "ckunqkey.sc" */	/* host code */
	if (err)
	{
		/* open cursor failed */
		(*msg_func)(1, 1527, err);
		return (1527);
	}
	while (!last)
	{
/* # line 209 "ckunqkey.sc" */	/* fetch */
  {
    IIsqInit((char *)0);
    if (IIcsRetScroll((char *)"c3",7197,23034,-1,-1) != 0) {
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,index_name);
      IIcsGetio((short *)0,1,32,DB_MAXNAME+1-1,index_owner);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 210 "ckunqkey.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err,2);
    IILQisInqSqlio((short *)0,1,30,4,&last,61);
  }
/* # line 211 "ckunqkey.sc" */	/* host code */
		if (err)
		{
/* # line 213 "ckunqkey.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"c3",7197,23034);
  }
/* # line 214 "ckunqkey.sc" */	/* host code */
			/* Fetch of index names for table '%s.%s' failed */
			(*msg_func)(1, 1525, table_owner, table_name, err);
			return (1525);
		}
		if (!last)
		{
/* # line 220 "ckunqkey.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"ckunqkey4",7198,31218);
      IIputdomio((short *)0,1,32,0,index_name);
      IIputdomio((short *)0,1,32,0,index_owner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"ckunqkey4",7198,31218);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iiindex_columns where index_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,index_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and index_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,index_owner);
        IIexDefine(0,(char *)"ckunqkey4",7198,31218);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt1);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 224 "ckunqkey.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"ckunqkey5",7199,1783);
      IIputdomio((short *)0,1,32,0,index_name);
      IIputdomio((short *)0,1,32,0,index_owner);
      IIputdomio((short *)0,1,30,4,&table_no);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"ckunqkey5",7199,1783);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iiindex_columns i, dd_regist_columns d where i.in\
dex_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,index_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and i.index_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,index_owner);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and i.column_name=d.column_name and d.table_no=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,30,4,&table_no);
        IIwritio(0,(short *)0,1,32,0,(char *)"and d.key_sequence>0");
        IIexDefine(0,(char *)"ckunqkey5",7199,1783);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&cnt2);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 231 "ckunqkey.sc" */	/* host code */
			/*
			** this index is unique and only uses key replicator
			** columns
			*/
			if (cnt1 == cnt2)
			{
/* # line 237 "ckunqkey.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"c3",7197,23034);
  }
/* # line 238 "ckunqkey.sc" */	/* host code */
				return (OK);
			}
		}
	}
/* # line 242 "ckunqkey.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"c3",7197,23034);
  }
/* # line 243 "ckunqkey.sc" */	/* host code */
	return (-1);	/* no unique indexes */
}
