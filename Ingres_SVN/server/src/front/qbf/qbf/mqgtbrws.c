# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	MQGETBRWS.QSC  - This module constitutes the data
**		      browsing support routines of MQBF. It is
**		      called after all the query strings,
**		      param args, etc. have been set up.
**
**	Routines:
**
**	Copyright (c) 2004 Ingres Corporation
**
**	History:
**		29-sep-86 (marian)	bug # 10423
**			Check to see if *output = QO_QDONE (or Q0_DDONE)
**			before loadtable.
**		07-dec-87 (kenl)
**			Turned module into a .QSC file in order to incorporate
**			the new header file: mqbf.qsh.  Added code to copy
**			DB Data Values (using routine IIQFcdb_CopyDBDVs) from
**			SELECTed table columns into hidden columns.  This is
**			necessary to remember columns values required for keys.
**			Technical note: the hidden column information MUST map
**			to the mqu[md]rec arrays to insure that the information
**			will get passed along to WRTUPD for updates.
**			IIQFcdb_CopyDBDVs must be called prior to each 
**			PUTFORM or LOADTABLE.
**		20-jul-88 (kenl)
**			Added messages to inform user that no details were
**			retrieved for a M/D JoinDef when the JoinDef is all
**			in SimpleField form.
**		11/23/90 (emerson)
**			Changed calling sequence for IIQFcdb_CopyDBDVs
**			(bug 8593).
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
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
FUNC_EXTERN VOID    IIQFcdb_CopyDBDVs();
/*
** mqgetmaster	-Get next master for a Master/Detail qdef.
**
**	Returns	  0:	  if OK
**		 -1:	  if no more masters
**		 errnum:  INGRES error occurred on retrieve
*/
i4
mqgetmaster(qdef, output, numrows)
MQQDEF	*qdef;
i4	*output;
i4	*numrows;
  {
	i4	rcode;
	QDESC	*mqqry;
    char *fname;
    char *tfld;
	mqqry = qdef->mqqdesc;
	fname = mq_frame;
	tfld = mq_tbl;
	rcode = 0;
	if (*output == QO_QDONE)
	{
/* # line 78 "mqgtbrws.qsc" */	/* message */
    {
      IImessage(ERget(S_QF0078_No_more_masters));
    }
/* # line 79 "mqgtbrws.qsc" */	/* sleep */
    {
      IIsleep(3);
    }
/* # line 80 "mqgtbrws.qsc" */	/* host code */
		rcode = -1;
		return(rcode);
	}
	if (*output == QO_DDONE)
	{
		if ((rcode = IIQG_generate(mqqry, QI_GET, output)) != OK)
			return(rcode);
	}
	else
	{
		if ((rcode = IIQG_generate(mqqry, QI_NMASTER, output)) != OK)
			return(rcode);
	}
	if (*output == QO_QDONE)
	{
/* # line 95 "mqgtbrws.qsc" */	/* message */
    {
      IImessage(ERget(S_QF0078_No_more_masters));
    }
/* # line 96 "mqgtbrws.qsc" */	/* sleep */
    {
      IIsleep(3);
    }
/* # line 97 "mqgtbrws.qsc" */	/* host code */
		/*
		**mqresetwhere(mqqry);
		*/
		rcode = -1;
	}
	else
	{
		IIQFcdb_CopyDBDVs(qdef, mqIN_MASTER);
/* # line 105 "mqgtbrws.qsc" */	/* clear */
    {
      IIclrflds();
    }
/* # line 106 "mqgtbrws.qsc" */	/* putform */
    {
      if (IIfsetio(fname) != 0) {
        IIsf_param(qdef->mqmpform, qdef->mqmpargv,(int *)0);
      } /* IIfsetio */
    }
/* # line 108 "mqgtbrws.qsc" */	/* host code */
		if (tblfield)
		{
			(*numrows) = 1;
			mqrowstat = 0;	/* init to no rows added by user */
			while ((*output != QO_DDONE) && ((*numrows) < mqmaxrows))
			{
				IIQFcdb_CopyDBDVs(qdef, mqIN_DETAIL);
/* # line 115 "mqgtbrws.qsc" */	/* loadtable */
    {
      if (IItbact(fname,tfld,1) != 0) {
        IItsc_param(qdef->mqdpform, qdef->mqdpargv,(int *)0);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 117 "mqgtbrws.qsc" */	/* host code */
				if ((rcode = IIQG_generate(mqqry, QI_GET, output)) != OK)
				{
					*numrows = 0;
					return(rcode);
				}
				(*numrows)++;
			}
			if (*output != QO_DDONE)
			{
				IIQFcdb_CopyDBDVs(qdef, mqIN_DETAIL);
/* # line 127 "mqgtbrws.qsc" */	/* loadtable */
    {
      if (IItbact(fname,tfld,1) != 0) {
        IItsc_param(qdef->mqdpform, qdef->mqdpargv,(int *)0);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 129 "mqgtbrws.qsc" */	/* host code */
			}
			else
			{
				(*numrows)--;
			}
		}
		else
		{
# ifdef DEBUG
			if (mq_debug)
			{
				SIfprintf(mqoutf, ERx("MQGETMASTER: output is %d\n"), *output);
			}
# endif
			if (*output != QO_DDONE)
			{
				IIQFcdb_CopyDBDVs(qdef, mqIN_DETAIL);
/* # line 147 "mqgtbrws.qsc" */	/* putform */
    {
      if (IIfsetio(fname) != 0) {
        IIsf_param(qdef->mqdpform, qdef->mqdpargv,(int *)0);
      } /* IIfsetio */
    }
/* # line 149 "mqgtbrws.qsc" */	/* host code */
			}
			else
			{
/* # line 152 "mqgtbrws.qsc" */	/* message */
    {
      IImessage(ERget(S_QF0151_no_details_found));
    }
/* # line 153 "mqgtbrws.qsc" */	/* sleep */
    {
      IIsleep(3);
    }
/* # line 154 "mqgtbrws.qsc" */	/* host code */
			}
		}
	}
	return(rcode);
  }
i4
mqgetrecord(qdef, output)
MQQDEF	*qdef;
i4	*output;
  {
    char *fname;
	QDESC	*mqqry;
	i4	rcode;
	fname = mq_frame;
	mqqry = qdef->mqqdesc;
	rcode = 0;
	if (*output == QO_QDONE)
	{
/* # line 176 "mqgtbrws.qsc" */	/* message */
    {
      IImessage(ERget(S_QF0079_No_more_rows_in_query));
    }
/* # line 177 "mqgtbrws.qsc" */	/* sleep */
    {
      IIsleep(3);
    }
/* # line 178 "mqgtbrws.qsc" */	/* host code */
		rcode = -1;
		return(rcode);
	}
	if ((rcode = IIQG_generate(mqqry, QI_GET, output)) != OK)
		return(rcode);
	if (*output == QO_QIN)
	{
		IIQFcdb_CopyDBDVs(qdef, mqIN_MASTER);
/* # line 187 "mqgtbrws.qsc" */	/* clear */
    {
      IIclrflds();
    }
/* # line 188 "mqgtbrws.qsc" */	/* putform */
    {
      if (IIfsetio(fname) != 0) {
        IIsf_param(qdef->mqmpform, qdef->mqmpargv,(int *)0);
      } /* IIfsetio */
    }
/* # line 189 "mqgtbrws.qsc" */	/* host code */
	}
	else
	{
/* # line 192 "mqgtbrws.qsc" */	/* message */
    {
      IImessage(ERget(S_QF0079_No_more_rows_in_query));
    }
/* # line 193 "mqgtbrws.qsc" */	/* sleep */
    {
      IIsleep(3);
    }
/* # line 194 "mqgtbrws.qsc" */	/* host code */
		/*
		**mqresetwhere(mqqry);
		*/
		rcode = -1;
	}
	return(rcode);
  }
i4
mqgetdtl(qdef, output)
MQQDEF	*qdef;
i4	*output;
  {
    char *fname;
	QDESC	*mqqry;
	i4	rcode;
	rcode = 0;
	mqqry = qdef->mqqdesc;
	fname = mq_frame;
	if (*output == QO_QDONE)
	{
/* # line 218 "mqgtbrws.qsc" */	/* message */
    {
      IImessage(ERget(S_QF007A_No_more_records));
    }
/* # line 219 "mqgtbrws.qsc" */	/* sleep */
    {
      IIsleep(3);
    }
/* # line 220 "mqgtbrws.qsc" */	/* host code */
		rcode = -1;
		return(rcode);
	}
	if (*output == QO_DDONE)
	{
		goto nmaster;
	}
	if ((rcode = IIQG_generate(mqqry, QI_GET, output)) != OK)
		return(rcode);
	if (*output == QO_QDONE)
	{
/* # line 232 "mqgtbrws.qsc" */	/* message */
    {
      IImessage(ERget(S_QF007A_No_more_records));
    }
/* # line 233 "mqgtbrws.qsc" */	/* sleep */
    {
      IIsleep(3);
    }
/* # line 234 "mqgtbrws.qsc" */	/* host code */
		/*
		**mqresetwhere(mqqry);
		*/
		rcode = -1;
		return(rcode);
	}
	else if (*output == QO_DDONE)
	{
	nmaster:
/* # line 243 "mqgtbrws.qsc" */	/* message */
    {
      IImessage(ERget(S_QF007B_No_more_details_for_t));
    }
/* # line 244 "mqgtbrws.qsc" */	/* host code */
		if ((rcode = IIQG_generate(mqqry, QI_GET, output)) != OK)
			return(rcode);
		if (*output != QO_QDONE)
		{
			IIQFcdb_CopyDBDVs(qdef, mqIN_MASTER);
/* # line 249 "mqgtbrws.qsc" */	/* clear */
    {
      IIclrflds();
    }
/* # line 250 "mqgtbrws.qsc" */	/* putform */
    {
      if (IIfsetio(fname) != 0) {
        IIsf_param(qdef->mqmpform, qdef->mqmpargv,(int *)0);
      } /* IIfsetio */
    }
/* # line 251 "mqgtbrws.qsc" */	/* host code */
			if (*output == QO_DDONE)
			{
/* # line 253 "mqgtbrws.qsc" */	/* message */
    {
      IImessage(ERget(S_QF0151_no_details_found));
    }
/* # line 254 "mqgtbrws.qsc" */	/* sleep */
    {
      IIsleep(3);
    }
/* # line 255 "mqgtbrws.qsc" */	/* host code */
			}
		}
		else
		{
/* # line 259 "mqgtbrws.qsc" */	/* message */
    {
      IImessage(ERget(S_QF007A_No_more_records));
    }
/* # line 260 "mqgtbrws.qsc" */	/* sleep */
    {
      IIsleep(3);
    }
/* # line 261 "mqgtbrws.qsc" */	/* host code */
			/*
			**mqresetwhere(mqqry);
			*/
			rcode = -1;
			return(rcode);
		}
	}
# ifdef DEBUG
	if (mq_debug)
	{
		SIfprintf(mqoutf, ERx("MQGETDTL: output is %d\n"), *output);
	}
# endif
	if (*output != QO_DDONE)
	{
		IIQFcdb_CopyDBDVs(qdef, mqIN_DETAIL);
/* # line 278 "mqgtbrws.qsc" */	/* putform */
    {
      if (IIfsetio(fname) != 0) {
        IIsf_param(qdef->mqdpform, qdef->mqdpargv,(int *)0);
      } /* IIfsetio */
    }
/* # line 279 "mqgtbrws.qsc" */	/* host code */
	}
	return(rcode);
  }
i4
mqgetscroll(qdef, output, numrows)
MQQDEF	*qdef;
i4	*output;
i4	*numrows;
  {
	QDESC	*mqqry;
	i4	i;
	i4	rcode;
    char *fname;
    char *tfld;
	mqqry = qdef->mqqdesc;
	fname = mq_frame;
	tfld = mq_tbl;
	rcode = 0;
	if (*output == QO_QDONE)
	{
		return(rcode);
	}
	if (qdef_type == 0)
	{
		for (i=0; ((i<10) && (*output != QO_DDONE)); i++)
		{
			if ((rcode = IIQG_generate(mqqry, QI_GET, output)) != OK)
			{
				return(rcode);
			}
			/* bug 10423
			**	Check to see if you are done before loading.
			*/
			if (*output == QO_DDONE)
			{
				return(rcode);
			}
			mqrowstat = 0;
			IIQFcdb_CopyDBDVs(qdef, mqIN_DETAIL);
/* # line 324 "mqgtbrws.qsc" */	/* loadtable */
    {
      if (IItbact(fname,tfld,1) != 0) {
        IItsc_param(qdef->mqdpform, qdef->mqdpargv,(int *)0);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 326 "mqgtbrws.qsc" */	/* host code */
			(*numrows)++;
		}
	}
	else
	{
		for (i=0; ((i<10) && (*output != QO_QDONE)); i++)
		{
			if ((rcode = IIQG_generate(mqqry, QI_GET, output)) != OK)
			{
				return(rcode);
			}
			/* bug 10423
			**	Check to see if you are done before loading.
			*/
			if (*output == QO_QDONE)
			{
				return(rcode);
			}
			mqrowstat = 0;
			IIQFcdb_CopyDBDVs(qdef, mqIN_MASTER);
/* # line 348 "mqgtbrws.qsc" */	/* loadtable */
    {
      if (IItbact(fname,tfld,1) != 0) {
        IItsc_param(qdef->mqmpform, qdef->mqmpargv,(int *)0);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 350 "mqgtbrws.qsc" */	/* host code */
			(*numrows)++;
		}
	}
	return(rcode);
  }
