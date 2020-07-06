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
# include	<erlq.h>
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlda.h"
/**
** Name:	uicatacc.sc - general relation access using cursors.
**
** Description:
**	Presents a generalized cursor-like interface for access to 
**	any relation. 
**
**	This file defines:
**
**	IIUIrao_relOpen() Defines a cursor to a relation, given the relation name,
**		the columns to retrieve, and the sort order.  Opens the cursor
**		in order to facilitate later cursor fetches.
**
**	IIUIraf_relFetch() Fetches a tuple from a cursor previously
**		opened using IIUIrao_relOpen(). 
**
**	IIUIrac_relClose() Closes the cursor opened in IIUIrao_relOpen()
**			
**  History:
**	12/08/88 (dkh) - Fixed venus bug 4033.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/* defines */
/* 
** default for the maximum columns in a relation.  If there are
** more columns in the table to be selected from, a bigger
** sqlda will be allocated.
*/
/* statics */
static	bool	is_open = FALSE;
static  i4      num_cols = 0;
static	bool	trm_char = FALSE;
static  DB_DATA_VALUE **dbdv_list;
/*{
** Name:	IIUIrao_relOpen() - open a cursor to any relation.
**
** Description:
**	This function simulates a cursor to a relation, given the name of the 
**	relation, the columns that are to be selected, the section 
**	criteria, the sort specification, and the addresses to put the 
**	values from the given columns.  
**
** Inputs:
**	relname		Relation name, for query specification 
**
**	target_list	Target column names, for query specification 
**
**	target_addr	Target addresses, for query specification.  These
**			are pointers to DB_DATA_VALUEs
**
**	qual		Selection criteria, for query specification 
**	
**	sortspec	Sort field(s), for query specification 
**
**	trim		Indicates whether "char" values are to be trimmed.
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
*/
DB_STATUS
IIUIrao_relOpen(relname, target_list, target_addr, qual, sortspec, num_atts,
    trim)
char		*relname;
char		*target_list;
DB_DATA_VALUE	**target_addr;
char		*qual;
char		*sortspec;
i4		num_atts;
bool		trim;
{
	STATUS	req_status;
	i4	i;
  char qbuf[UI_MAX_QBUF];
  i4 err_num;
	STpolycat(4, "SELECT ", target_list , " FROM ",	
		relname, qbuf);
	if ((qual != (char *) NULL) && (*qual != EOS))
	{
		STcat(qbuf," WHERE ");
		STcat(qbuf, qual);
	}
	if ((sortspec != (char *) NULL) && (*sortspec != EOS))
	{
		STcat(qbuf," ORDER BY ");
		STcat(qbuf, sortspec);
	}
	IIsqInit((char *) NULL);
	IIwritio(0, (i2 *) 0, 1, 32, 0, qbuf);
	IIretinit((char *)0, 0);
/* # line 119 "uirelacc.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err_num,2);
  }
/* # line 120 "uirelacc.sc" */	/* host code */
	if(err_num != 0)
		return ((DB_STATUS) err_num);
	is_open = TRUE;
	dbdv_list = target_addr;
	num_cols = num_atts;
	trm_char = trim;
	return ((DB_STATUS) OK);
}		
/*{
** Name:	IIUIraf_relFetch() - fetch a row from a simulated cursor to 
					a relation.
**
** Description:
**	This function fetchs a row from a simulated cursor to a relation 
**	previously opened using IIUIrao_relOpen().
** Inputs:
**	none
** Outputs:
**	The columns from the row fetched are copied into the 
**	addresses of the DBDV's passed into IIUIrao_relOpen() in the target_addr
**	parameter.
**
**	Returns:
**		OK			successful
**		E_UI0002_EOS		no more tuples in active set
**		E_UI0001_BLOCK_CLOSED	query block is not open (from status)
**		other			other dbms error
**	Exceptions:
**		none
**
** Side Effects:
**
** History:
**	05-oct-1987 (danielt) 
**		written
*/
DB_STATUS
IIUIraf_relFetch()
{
  i4 err_code;
	DB_DATA_VALUE	**p;
	DB_DATA_VALUE	*dbdv_ptr;
	i4		i;
	i4		data_type;
	if (is_open == FALSE)
	{
		return ((DB_STATUS) E_UI0001_BLOCK_CLOSED);
	}
	if (IInextget() == 0)
		return ((DB_STATUS) E_UI0002_EOS);
	for(i= 0, p = dbdv_list; i<num_cols; i++, p++)
	{
		dbdv_ptr = *p;
		IIgetdomio((i2 *)0, 1, DB_DBV_TYPE, 0, dbdv_ptr);
/* # line 176 "uirelacc.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err_code,2);
  }
/* # line 177 "uirelacc.sc" */	/* host code */
		if (err_code == 0)
		{
			if (dbdv_ptr->db_datatype < 0)
				data_type = -(dbdv_ptr->db_datatype);
			else
				data_type = dbdv_ptr->db_datatype;
			/*
			** if the data is DB_CHA_TYPE, 
			** then we must trim and NULL terminate it.
			*/
			if (trm_char && data_type == DB_CHA_TYPE)
			{
				char	*p;
				p = (char *) dbdv_ptr->db_data;
				*(p + dbdv_ptr->db_length)  = EOS;
				STtrmwhite((char *) dbdv_ptr->db_data);
			}
		}
	}
	return ((DB_STATUS) err_code);
}
/*{
** Name:	IIUIrac_relClose() - close a simulated cursor to a relation.
**
** Description:
**	This function closes a cursor to a relation 
**	previously opened using IIUIrao_relOpen().
** Inputs:
**	none
** Outputs:
**	none
**	
** Returns:
**	none
** Exceptions:
**	none
**
** Side Effects:
**
** History:
**	05-oct-1987 (danielt) 
**		written
*/
VOID
IIUIrac_relClose()
{
	if (is_open == FALSE)
		return;
	IIflush((char *) 0, 0);
	is_open = FALSE;
}
