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
** Name:	uiupdate.sc - general relation access using cursors.
**
** Description:
**
**	IIUIupdate() Update rows in a relation, given the name
**		of the relation, the names of the columns to
**		be updated, an array of DB_DATA_VALUEs containing
**		the new values for the given columns, and the 
**		qualification clause for the update.
**	
**/
/* defines */
/* 
** default for the maximum columns in a relation.  If there are
** more columns in the table to be updated, a bigger
** sqlda will be allocated.
*/
#define		UI_MAX_REL_COLS 20  
/* statics */
/* 
** address of SQLDA:  initialize if NULL and re-use if possible (if
** SQLDA is big enough)
*/
static	IISQLDA	*sqlda = NULL;
static  i2	tag;
static  i4	num_allocated;
/*{
** Name:	IIUIupdate() - 
**
** Description:
**
** Inputs:
**	relname		Relation name, for update specification 
**
**	set_list	Target column names, for update specification 
**
**	target_addr	array of DB_DATA_VALUES , for update specification.
**
**	qual		Selection criteria, for update specification 
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
**
** History:
**	05-oct-1987 (danielt) 
**		written
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
DB_STATUS
IIUIupdate(relname, set_list, target_addr, qual, num_columns)
char		*relname;
char		*set_list;
DB_DATA_VALUE	target_addr[];
char		*qual;
i4		num_columns;
{
	STATUS	req_status;
	i4	i;
  char qbuf[UI_MAX_QBUF];
  i4 err_num;
	if(sqlda == NULL)
	{
		tag = FEbegintag();
		sqlda = (IISQLDA *) FEreqmem(0, IISQDA_HEAD_SIZE +
				UI_MAX_REL_COLS*IISQDA_VAR_SIZE, 
				FALSE, &req_status);
		sqlda->sqln = UI_MAX_REL_COLS;
	}
	if (num_columns > sqlda->sqln)	/* need to allocate a bigger sqlda */
	{
		FEfree(tag);
		tag = FEbegintag();
		sqlda = (IISQLDA *) FEreqmem(0, IISQDA_HEAD_SIZE +
				num_columns*IISQDA_VAR_SIZE, 
				FALSE, &req_status);
	}
	sqlda->sqld = num_columns;
	sqlda->sqln = num_columns;
	STprintf(qbuf,ERx("UPDATE %s SET %s"), relname, set_list);
	if (qual != (char *) NULL)
	{
		STcat(qbuf," WHERE ");
		STcat(qbuf, qual);
	}
/* # line 113 "uiupdate.sc" */	/* prepare */
  {
    IIsqInit((char *)0);
    IIsqPrepare(0,(char *)"uiup_s1",(char *)0,0,qbuf);
  }
/* # line 114 "uiupdate.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err_num,2);
  }
/* # line 115 "uiupdate.sc" */	/* host code */
	if(err_num != 0)
		return ((DB_STATUS) err_num);
	for(i=0; i<num_columns ; i++)
	{
		sqlda->sqlvar[i].sqltype = DB_DBV_TYPE; 
		sqlda->sqlvar[i].sqllen  = sizeof(DB_DATA_VALUE);
		sqlda->sqlvar[i].sqldata = (PTR) &target_addr[i];
	}
/* # line 123 "uiupdate.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExStmt((char *)"uiup_s1",0);
    IIsqDaIn(0,sqlda);
    IIsyncup((char *)0,0);
  }
/* # line 124 "uiupdate.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err_num,2);
  }
/* # line 125 "uiupdate.sc" */	/* host code */
	return ((DB_STATUS) err_num);
}		
