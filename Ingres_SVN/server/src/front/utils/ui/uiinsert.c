# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h> 
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h> 
# include	<ui.h> 
# include	<er.h>
# include	<erlq.h>
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlda.h"
/**
** Name:	uiinsert.sc - general relation access using cursors.
**
** Description:
**
**	IIUIinsert() Insert a row in a relation, given the name
**		of the relation, the names of the columns to
**		be inserted, and an array of DB_DATA_VALUEs containing
**		the new values for the given columns.
**	
**/
/* defines */
/* 
** default for the maximum columns in a relation.  If there are
** more columns in the table to be updated, a bigger
** sqlda will be allocated.
*/
/* statics */
/*{
** Name:	IIUIinsert() - 
**
** Description:
**
** Inputs:
**	relname		Relation name, for update specification 
**
**	target_list	Target column names, for update specification 
**
**	target_addr	array of DB_DATA_VALUES , for update specification.
**
**	num_columns	number of columns to be inserted.
**	
**
** Outputs:
**	Returns:
**		OK			successful
**		other			dbms error
**	Exceptions:
**		none
**
** Side Effects:
**		If successful, a row is added into the named relation.
** History:
**	19-nov-1987 (danielt) 
**		written
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
DB_STATUS
IIUIinsert(relname, target_list, target_addr,  num_columns)
char		*relname;
char		*target_list;
DB_DATA_VALUE	target_addr[];
i4		num_columns;
{
	IISQLDA	insert_sqlda;
	IISQLDA	*sqlda = &insert_sqlda;
	STATUS	req_status;
	i4	i;
  char qbuf[DB_MAX_COLS*FE_MAXNAME + 40];
  char question_list[DB_MAX_COLS*3 + 1];
  i4 err_num;
	sqlda->sqln = num_columns;
	sqlda->sqld = num_columns;
	STcopy(ERx("?"),question_list);
	for(i=1; i<num_columns; i++)
	{
		STcat(question_list, ERx(", ?"));
	}	
	STprintf(qbuf,ERx("INSERT INTO %s (%s) VALUES (%s)"), 
				relname,
				target_list,
				question_list);
/* # line 93 "uiinsert.sc" */	/* prepare */
  {
    IIsqInit((char *)0);
    IIsqPrepare(0,(char *)"uiup_s1",(char *)0,0,qbuf);
  }
/* # line 94 "uiinsert.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err_num,2);
  }
/* # line 95 "uiinsert.sc" */	/* host code */
	if(err_num != 0)
		return ((DB_STATUS) err_num);
	for(i=0; i<num_columns ; i++)
	{
		sqlda->sqlvar[i].sqltype = DB_DBV_TYPE; 
		sqlda->sqlvar[i].sqllen  = sizeof(DB_DATA_VALUE);
		sqlda->sqlvar[i].sqldata = (PTR) &target_addr[i];
	}
/* # line 103 "uiinsert.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExStmt((char *)"uiup_s1",0);
    IIsqDaIn(0,sqlda);
    IIsyncup((char *)0,0);
  }
/* # line 104 "uiinsert.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err_num,2);
  }
/* # line 105 "uiinsert.sc" */	/* host code */
	return ((DB_STATUS) err_num);
}		
