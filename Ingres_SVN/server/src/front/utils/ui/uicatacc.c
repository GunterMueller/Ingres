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
# include	<generr.h> 
# include	<fe.h> 
# include	<ui.h> 
# include	<erlq.h>
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlda.h"
/**
** Name:	uicatacc.sc - general catalog access using cursors.
**
** Description:
**	Presents a generalized cursor-based implementation for access to 
**	any catalog.  The routines in this module do NOT function
**	as generalized relation browsers for an arbitrarily complex
**	relation.  The most important restriction is in the data types
**	that they expect:  they only expect to see a relation with data types
**	of integer, float, or some kind of textual character field(e.g. text,
**	vchar, c).  In the case of text field, they expect to see a char *
**	in the target_addr pointer list, so they do any conversion needed (or
**	rather it has libq do it).  They also trim any textual field.
**
**	This file defines:
**
**	IIUIcao_catOpen() Defines a cursor to a catalog, given the catalog name,
**		the columns to retrieve, and the sort order.  Opens the cursor
**		in order to facilitate later cursor fetches.
**
**	IIUIcaf_catFetch() Fetches a tuple from a cursor previously
**		opened using IIUIcao_open(). 
**
**	IIUIcac_catClose() Closes the cursor opened in IIUIcao_catOpen()
**			
**/
/* defines */
static IISQLDA	_sqlda;
static IISQLDA	*sqlda = &_sqlda;
static	bool	is_open = FALSE;
/*{
** Name:	IIUIcao_catOpen() - open a cursor to any catalog.
**
** Description:
**	This function opens a cursor to a catalog, given the name of the 
**	catalog, the columns that are to be selected, the section 
**	criteria, the sort specification, and the addresses to put the 
**	values from the given columns.  
**
** Inputs:
**	relname		Relation name, for query specification 
**
**	target_list	Target column names, for query specification 
**
**	target_addr	Target addresses, for query specification.  These
**		should only be pointers to int data types (i2, i4) float
**		data types (f4, f8), or char pointers.  This routine will
**		set up the SQLDA so that it will indicate any conversion that
**		might need to be done on textual data types to return them
**		as char pointers.  
**
**	qual		Selection criteria, for query specification 
**	
**	sortspec	Sort field(s), for query specification 
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
**	10-dec-2000 (gupsh01)
**	    Added support for ANSI date/time types.
**	11-dec-2000 (gupsh01)
**	    Fixed to use the declared constants for
**	    the lengths.
*/
DB_STATUS
IIUIcao_catOpen(relname, target_list, target_addr, qual, sortspec)
char		*relname;
char		*target_list;
PTR		target_addr[];
char		*qual;
char		*sortspec;
{
	STATUS	req_status;
	i4	i;
  char qbuf[UI_MAX_QBUF];
  i4 err_num;
	i2	null_ind;
	bool	nullable;
	i2	dtype;
	sqlda->sqln = IISQ_MAX_COLS;
	STpolycat(6, "SELECT ", target_list, " FROM ",	
		relname," WHERE ", qual, qbuf);
	if (sortspec != NULL)
	{
		STcat(qbuf," ORDER BY ");
		STcat(qbuf, sortspec);
	}
/* # line 118 "uicatacc.sc" */	/* prepare */
  {
    IIsqInit((char *)0);
    IIsqPrepare(0,(char *)"uica_s1",(char *)0,0,qbuf);
  }
/* # line 119 "uicatacc.sc" */	/* describe */
  {
    IIsqInit((char *)0);
    IIsqDescribe(0,(char *)"uica_s1",sqlda,0);
  }
/* # line 120 "uicatacc.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err_num,2);
  }
/* # line 121 "uicatacc.sc" */	/* host code */
	if(err_num != 0)
		return ((DB_STATUS) err_num);
	for(i=0; i<sqlda->sqld; i++)
	{
		if (sqlda->sqlvar[i].sqltype < 0) 
		{
			nullable = TRUE;
			sqlda->sqlvar[i].sqlind = &null_ind;
			dtype = -(sqlda->sqlvar[i].sqltype);
 		}
		else
		{
			dtype = sqlda->sqlvar[i].sqltype;
			nullable = FALSE;
		}
		switch (dtype)
		{
			/*
			** if the type of the column is any sort of char text,
			** have libq do the conversion to (char *)
			*/
			case IISQ_DTE_TYPE:
			case IISQ_ADTE_TYPE:
			case IISQ_TMWO_TYPE:
			case IISQ_TMW_TYPE:
			case IISQ_TME_TYPE:
			case IISQ_TSWO_TYPE:
			case IISQ_TSW_TYPE:
			case IISQ_TSTMP_TYPE:
			case IISQ_INYM_TYPE:
			case IISQ_INDS_TYPE:
			if (dtype == IISQ_DTE_TYPE)
				sqlda->sqlvar[i].sqllen = IISQ_DTE_LEN;
			else if (dtype == IISQ_ADTE_TYPE)
				sqlda->sqlvar[i].sqllen = IISQ_ADTE_LEN;
			else if (dtype == IISQ_TMWO_TYPE)
				sqlda->sqlvar[i].sqllen = IISQ_TMWO_LEN;
			else if (dtype == IISQ_TMW_TYPE)
				sqlda->sqlvar[i].sqllen = IISQ_TSW_LEN;
			else if (dtype == IISQ_TME_TYPE)
				sqlda->sqlvar[i].sqllen = IISQ_TME_LEN;
			else if (dtype == IISQ_TSWO_TYPE)
				sqlda->sqlvar[i].sqllen = IISQ_TSWO_LEN;
			else if (dtype == IISQ_TSW_TYPE)
				sqlda->sqlvar[i].sqllen = IISQ_TSW_LEN;
			else if (dtype == IISQ_TSTMP_TYPE)
				sqlda->sqlvar[i].sqllen = IISQ_TSTMP_LEN;
			else if (dtype == IISQ_INYM_TYPE)
				sqlda->sqlvar[i].sqllen = IISQ_INTYM_LEN;
			else if (dtype == IISQ_INDS_TYPE)
				sqlda->sqlvar[i].sqllen = IISQ_INTDS_LEN;
			/* fall through into  ... */
			case IISQ_CHA_TYPE:
			case IISQ_VCH_TYPE:
				if (nullable)
				{
				    sqlda->sqlvar[i].sqltype= -(IISQ_CHA_TYPE);
				}
				else
				{
				    sqlda->sqlvar[i].sqltype=IISQ_CHA_TYPE;
				}
				break;	
			/*
			** if the type is an int or float, we are still OK
			*/
			case	IISQ_INT_TYPE:		
			case	IISQ_FLT_TYPE:
					break;
			/*
			** this routine can only be passed pointers to
			** chars, ints, or floats, so anything else is an
			** error.
			*/
			default:
				return (FAIL);
		}
		sqlda->sqlvar[i].sqldata = (PTR) target_addr[i];
	}
	IIUIcac_catClose();
/* # line 202 "uicatacc.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err_num,2);
  }
/* # line 203 "uicatacc.sc" */	/* host code */
	if(err_num != 0)
		return ((DB_STATUS) err_num);
/* # line 205 "uicatacc.sc" */	/* open */
  {
    IIsqInit((char *)0);
    IIcsOpen((char *)"c1",6502,30315);
    IIwritio(0,(short *)0,1,32,0,(char *)"uica_s1");
    IIcsQuery((char *)"c1",6502,30315);
  }
/* # line 206 "uicatacc.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err_num,2);
  }
/* # line 207 "uicatacc.sc" */	/* host code */
	if(err_num != 0)
		return ((DB_STATUS) err_num);
	is_open = TRUE;
	return ((DB_STATUS) OK);
}		
/*{
** Name:	IIUIcaf_catFetch() - fetch a row from a cursor to a catalog.
**
** Description:
**	This function fetchs a row from a cursor to a catalog 
**	previously opened using IIUIcao_catOpen().
** Inputs:
**	none
** Outputs:
**	The columns from the row fetched are copied into the 
**	addresses passed into IIUIcao_catOpen() in the target_addr
**	parameter.  All the text fields are trimed.
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
**	13-dec-1988 (neil) 
**		Changed to check for generic error as well.
*/
DB_STATUS
IIUIcaf_catFetch()
{
	i4	i;
  bool last_row;
  i4 err_code;
	if (is_open == FALSE)
	{
		return ((DB_STATUS) E_UI0001_BLOCK_CLOSED);
	}
/* # line 253 "uicatacc.sc" */	/* fetch */
  {
    IIsqInit((char *)0);
    if (IIcsRetScroll((char *)"c1",6502,30315,-1,-1) != 0) {
      IIcsDaGet(0,sqlda);
      IIcsERetrieve();
    } /* IIcsRetrieve */
  }
/* # line 254 "uicatacc.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,1,&last_row,61);
    IILQisInqSqlio((short *)0,1,30,4,&err_code,2);
  }
/* # line 255 "uicatacc.sc" */	/* host code */
	if (last_row)
	{
			return ((DB_STATUS) E_UI0002_EOS);
	}
	if (err_code == 0)
	{
		for(i=0;i<sqlda->sqld;i++)
		{
			if (sqlda->sqlvar[i].sqltype == DB_CHA_TYPE)
			{
				STtrmwhite((char *) sqlda->sqlvar[i].sqldata);
			}
		}
		return ((DB_STATUS) OK);
	}
	if (err_code == E_LQ0058_CSNOTOPEN || err_code == GE_CURSOR_UNKNOWN)
		return ((DB_STATUS) E_UI0001_BLOCK_CLOSED);
	return (err_code);
}
/*{
** Name:	IIUIcac_catClose() - close a cursor to a catalog.
**
** Description:
**	This function closes a cursor to a catalog 
**	previously opened using IIUIcao_catOpen().
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
IIUIcac_catClose()
{
	if (is_open == FALSE)
		return;
/* # line 301 "uicatacc.sc" */	/* close */
  {
    IIsqInit((char *)0);
    IIcsClose((char *)"c1",6502,30315);
  }
/* # line 302 "uicatacc.sc" */	/* host code */
	is_open = FALSE;
}
