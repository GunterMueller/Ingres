# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include "mqbf.h"
# include	"mqeglobs.h"
# include	<er.h>
# include	"erqf.h"
/**
**
**  Name: mqretinit.qsc - Initialize the form for browsing data.
**
**  Description:
**      This files defines:
** 
**	mqretinit -
**		    Calls QG to retrieve rows from the database and 
**		    displays them on the form.
**
**	IIQFcdb_CopyDBDVs -
**		    Copy DB DATA VALUES from the SELECTed columns into
**		    hidden fields.
**
**  History:
**      11/18/87 (kenl)
**          Added routine IIQFcdb_CopyDBDVs.
**	07-dec-87 (kenl)
**	    Turned module into a .QSC file in order to incorporate
**	    the new header file: mqbf.qsh
**	20-jul-88 (kenl)
**		Added message to inform user that no details were found 
**		when M/D is displayed in all SimpleField format. (E_QF0151).
**	11/23/90 (emerson)
**		Modified IIQFcdb_CopyDBDVs (bug 8593).
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/*
**  Forward and/or External function references.
*/
GLOBALREF   i4	    key1_xref[MQ_MAXATTS][2];
GLOBALREF   i4	    next1_xref;
GLOBALREF   i4	    key2_xref[MQ_MAXATTS][2];
GLOBALREF   i4	    next2_xref;
GLOBALREF   ATTRIBINFO	*IIQFmla_MLkeyInMDArray[MQ_MAXATTS];
GLOBALREF   i4       IIQFmlc_MLkeyInMDCount;
GLOBALREF   QRY_ARGV mq1qargv[];
GLOBALREF   QRY_ARGV mq2qargv[];
FUNC_EXTERN VOID    IIQFcdb_CopyDBDVs();
FUNC_EXTERN ADF_CB  *FEadfcb();
FUNC_EXTERN STATUS  adc_cvinto();
/*
** Name: mqretinit -
**		    Calls QG to retrieve rows from the database and
**		    displays them on the form.
**
** Description:
**      This routine is called whenever an append, update, or retrieve
**	is requested by the QBF user.  This routine calls QG to retrieve
**	rows from the database.  If a row is to be displayed in a 
**	SimpleFields format, a call to PUTFORM is done, otherwise a
**	LOADTABLE is done.
**
** Inputs:
**	qdef -	    a pointer to an MQQDEF structure which contains the
**		    QDESC pointer to the description of the query to run.
**	output -    {nat *} The state in which the query ended.
**	numrows	    {nat *} The number of rows retrieved.
**
** Outputs:
**	numrows -   The number of rows retrieved.
**
**	Returns: 0 if no error
**		 INGRES error number if error occurred.
**
**	Exceptions:
**
**
** Side Effects:
**	None.
**
** History:
**      11/18/87 (kenl)
**          Added call to IIQFcdb_CopyDBDVs().
**	22-apr-88 (kenl)
**		Replaced direct assignment of DB DATA items with call
**		to adc_cvinto().
**	11/23/90 (emerson)
**		Changed calling sequence for IIQFcdb_CopyDBDVs
**		(bug 8593).
*/
i4
mqretinit(qdef, output, numrows)
MQQDEF	*qdef;
i4	*output;
i4	*numrows;
  {
	QDESC	*mqqry;
	i4	rtncode;
    char *fname;
    char *tfld;
	mqqry = qdef->mqqdesc;
	fname = mq_frame;
	tfld = mq_tbl;
	rtncode = 0;
	/*
	** reset status field for rows displayed in table field
	** This is a hidden column in the table field called
	** mqrowstat and it =0 if it's an existing row and =1 if
	** it's a row added by the user
	** Thus, initially all rows of data loaded by qbf have
	** mqrowstat = 0
	*/
	mqrowstat = 0;
/* # line 133 "mqretini.qsc" */	/* clear */
    {
      IIclrflds();
    }
/* # line 135 "mqretini.qsc" */	/* host code */
	if ((qdef_type == 1) && (!tblfield))
	{
		IIQFcdb_CopyDBDVs(qdef, mqIN_MASTER);
/* # line 138 "mqretini.qsc" */	/* putform */
    {
      if (IIfsetio(fname) != 0) {
        IIsf_param(qdef->mqmpform, qdef->mqmpargv,(int *)0);
      } /* IIfsetio */
    }
/* # line 139 "mqretini.qsc" */	/* host code */
	}
	else if ((qdef_type == 1) && (tblfield))
	{
		*numrows = 1;
		while ((*output == QO_QIN) && ((*numrows) < mqmaxrows))
		{
			IIQFcdb_CopyDBDVs(qdef, mqIN_MASTER);
/* # line 146 "mqretini.qsc" */	/* loadtable */
    {
      if (IItbact(fname,tfld,1) != 0) {
        IItsc_param(qdef->mqmpform, qdef->mqmpargv,(int *)0);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 148 "mqretini.qsc" */	/* host code */
			if ((rtncode = IIQG_generate(mqqry, QI_GET, output)) != OK)
			{
				*numrows = 0;
/* # line 151 "mqretini.qsc" */	/* clear */
    {
      IIclrflds();
    }
/* # line 152 "mqretini.qsc" */	/* host code */
				return(rtncode);
			}
			(*numrows)++;
		}
		if (*output != QO_QDONE)
		{
			IIQFcdb_CopyDBDVs(qdef, mqIN_MASTER);
/* # line 159 "mqretini.qsc" */	/* loadtable */
    {
      if (IItbact(fname,tfld,1) != 0) {
        IItsc_param(qdef->mqmpform, qdef->mqmpargv,(int *)0);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 161 "mqretini.qsc" */	/* host code */
		}
		else
		{
			(*numrows)--;
		}
	}
	else if ((qdef_type == 0) && (tblfield))
	{
		IIQFcdb_CopyDBDVs(qdef, mqIN_MASTER);
/* # line 170 "mqretini.qsc" */	/* putform */
    {
      if (IIfsetio(fname) != 0) {
        IIsf_param(qdef->mqmpform, qdef->mqmpargv,(int *)0);
      } /* IIfsetio */
    }
/* # line 171 "mqretini.qsc" */	/* host code */
		*numrows = 1;
		while ((*output != QO_DDONE) && (*numrows < mqmaxrows))
		{
			IIQFcdb_CopyDBDVs(qdef, mqIN_DETAIL);
/* # line 175 "mqretini.qsc" */	/* loadtable */
    {
      if (IItbact(fname,tfld,1) != 0) {
        IItsc_param(qdef->mqdpform, qdef->mqdpargv,(int *)0);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 177 "mqretini.qsc" */	/* host code */
			if ((rtncode = IIQG_generate(mqqry, QI_GET, output)) != OK)
			{
				*numrows = 0;
/* # line 180 "mqretini.qsc" */	/* clear */
    {
      IIclrflds();
    }
/* # line 181 "mqretini.qsc" */	/* host code */
				return(rtncode);
			}
			(*numrows)++;
		}
		if (*output != QO_DDONE)
		{
			IIQFcdb_CopyDBDVs(qdef, mqIN_DETAIL);
/* # line 188 "mqretini.qsc" */	/* loadtable */
    {
      if (IItbact(fname,tfld,1) != 0) {
        IItsc_param(qdef->mqdpform, qdef->mqdpargv,(int *)0);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 190 "mqretini.qsc" */	/* host code */
		}
		else
		{
			(*numrows)--;
		}
	}
	else if ((qdef_type == 0) && (!tblfield))
	{
		IIQFcdb_CopyDBDVs(qdef, mqIN_MASTER);
/* # line 199 "mqretini.qsc" */	/* putform */
    {
      if (IIfsetio(fname) != 0) {
        IIsf_param(qdef->mqmpform, qdef->mqmpargv,(int *)0);
      } /* IIfsetio */
    }
/* # line 201 "mqretini.qsc" */	/* host code */
# ifdef DEBUG
		if (mq_debug)
		{
			SIfprintf(mqoutf, ERx("MQRETINIT: output is %d\n"), *output);
		}
# endif
		if (*output != QO_DDONE)
		{
			IIQFcdb_CopyDBDVs(qdef, mqIN_DETAIL);
/* # line 211 "mqretini.qsc" */	/* putform */
    {
      if (IIfsetio(fname) != 0) {
        IIsf_param(qdef->mqdpform, qdef->mqdpargv,(int *)0);
      } /* IIfsetio */
    }
/* # line 212 "mqretini.qsc" */	/* host code */
		}
		else
		{
/* # line 215 "mqretini.qsc" */	/* message */
    {
      IImessage(ERget(S_QF0151_no_details_found));
    }
/* # line 216 "mqretini.qsc" */	/* sleep */
    {
      IIsleep(3);
    }
/* # line 217 "mqretini.qsc" */	/* host code */
		}
	}
	return(rtncode);
  }
/*
** Name: IIQFcdb_CopyDBDVs -
**		    Copy SELECTed DB DATA VALUES to hidden columns
**		    and/or DB DATA VALUES for appends.
**
** Description:
**	Before a LOADTABLE is done within mqretinit, hidden columns must
**	be initialized for each key that appears in the displayed row.  The
**	keys contained in the row were read from the database
**	using a SELECT (via QG).  By using the key cross-reference array,
**	key_xref, these keys are copied to the appropriate hidden columns.
**	Please refer to the module MQQDESC.QC for a more detailed 
**	description on the use of key_xref.
**
**	If master columns are being copied for a MD joindef, this routine
**	will also copy the DB DATA VALUE selected for a logical key
**	attribute to the "update" DBV for the attribute.
**	This ensures that if any details are appended to the master
**	with a user-maintained logical key joined to the master
**	logical key, they will get the proper value for the logical key.
**	(This special logic is necessary because logical keys do not appear
**	on the form).
**
** Inputs:
**	qdef -	    a pointer to an MQQDEF structure which contains the
**		    QDESC pointer to the description of the query to run.
**
**	code -	    {nat} 0 if master columns are being copied,
**			  1 if detail columns are being copied.
**
** Outputs:
**	None.
**
**	Returns:
**
**	Exceptions:
**
**
** Side Effects:
**	None.
**
** History:
**      11/18/87 (kenl)
**          Initial version.
**	10/23/90 (emerson)
**	    Copy DBV for logical key in the master 
**	    of an MD joindef; modify calling sequence (bug 8593).
*/
VOID
IIQFcdb_CopyDBDVs(qdef, code)
MQQDEF	*qdef;
i4	code;
{
    char	    **dbptrs;
    ADF_CB	    *adf_cb;
    DB_DATA_VALUE   *source;
    DB_DATA_VALUE   *destin;
    ATTRIBINFO	    *att;
    i4	    i;
    i4	    cnt;
    i4	    ms_cnt;
    adf_cb = FEadfcb();
    if (code == 0)
    {
	cnt = next1_xref;
	ms_cnt = IIQFmlc_MLkeyInMDCount;
	dbptrs = qdef->mqmpargv;
    }
    else
    {
	cnt = next2_xref;
	ms_cnt = 0;
	dbptrs = qdef->mqdpargv;
    }
    /* next_xref indicates how many key columns need to be copied */
    for (i = 0; i < cnt; i++)
    {
	if (code == 0)
	{
	    /* copy from list of master arguments */
	    source = mq1qargv[key1_xref[i][0]].qv_dbval;
	    /* get the pointer to the DB DATA VALUE for the hidden column */
	    destin = (DB_DATA_VALUE *)dbptrs[key1_xref[i][1]];
	}
	else
	{
	    /* copy from list of detail arguments */
	    source = mq2qargv[key2_xref[i][0]].qv_dbval;
	    /* get the pointer to the DB DATA VALUE for the hidden column */
	    destin = (DB_DATA_VALUE *)dbptrs[key2_xref[i][1]];
	}    
	/* copy DB DATA VALUE info from SELECTed item to hidden column */
	if (adc_cvinto(adf_cb, source, destin) != OK)
	{
	    FEafeerr(adf_cb);
	}
    }
    /*
    **  If this is the master of a master-detail joindef,
    **  then for each logical key in the master,
    **  copy the logical key value from the "retrieve" DBV
    **  to the "update" DBV.  This ensures that any details
    **  appended to this master will get the same logical key
    **  value for any detail user-maintained logical key that's
    **  joined to the master logical key.
    **  (The joined columns' "update" DBV's have the same db_data).
    */
    for (i = 0; i < ms_cnt; i++)
    {
	att = IIQFmla_MLkeyInMDArray[i];
	if (adc_cvinto(adf_cb, att->rdbv, &att->dbv) != OK)
	{
	    FEafeerr(adf_cb);
	}
    }
}
