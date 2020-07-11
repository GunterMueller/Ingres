# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 1987, 2005 Ingres Corporation
**
*/
/* NO_OPTIM = ris_u64 i64_aix */
#include	<compat.h>
#include	<st.h>
#include	<me.h> 
#include	<lo.h> 
#include	<si.h> 
#include	<er.h>
#include	<dbms.h>
#include	<generr.h>
#include	<gca.h>
#define  NOT_YET
#include	<fe.h>
#include	<adf.h>
#include	<afe.h>
#include	<qgor.h> 
#include	<eqrun.h> 
#include	<eqlang.h> 
#include	<erlq.h> 
#include	"qgi.h"
#include	<qgv0.h>
# ifdef wgl_win
# include <iisqlca.h>
# endif
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlda.h"
/**
** Name:	qg.qc -	Query Generation Module.
**
** Description:
**	The query generation module, QG, is used by QBF and the ABF run-time
**	system to execute retrieve or select queries in the DBMS.  Queries are
**	specified using the QDESC structure.  The important feature of this
**	generator is that it produces one tuple at a time each time it is
**	called.  This means that a query does not have to be run using the
**	retrieve or select loop syntax of EQUEL or ESQL.
**
**	Defines:
**
**	IIQG_generate()	generate and run query.
**	IIQG_send()	send query specification element to dbms.
**	IIQG_streamrtns() specify stream-handling routines for mapped queries
**
**	(QG Internal)
**	IIQG_alloc()	allocate an internal query state structure.
**
** History:
**	6/92 (Mike S)
**	    Add support for true singleton selects.  Previously, we
**	    did singleton selects by IIbreak'ing out of a select loop.
**	    This was done for Windows4GL, which is SQL-only; if ABF tries
**	    to use this, QUEL should be investigated.
**	4/13/92 (seg)
**	    Can't do arithmetic on PTR variables without casting.
**	3/92 (Mike S)
**	Add support for "EXECUTE IMMEDIATE ... INTO" queries.
**	
**	Revision 6.4  1989/08  wong
**	Added support for QI_DESCRIBE method.
**	1991/06/24  davel
**		Fix IIQG_generate to pass datavalues from the qg_where
**		QRY_SPEC list if it's defined. (Bug 38137).
**	1991/08/15  davel
**		Fix IIQG_generate to re-issue IIsqInit in repeat query do/while
**		loop so that LIBQ will handle SQL errors correctly on the
**		Execute pass. (Bug 39108).
**
**	Revision 6.3  90/01  wong
**	Modified for version 2 QDESC (which has longs for query ID instead
**	of i1s.)  JupBug #7899.
**
**	Revision 6.2  89/01  wong
**	Added fix for CMS problem when no rows were fetched.
**
**	Revision 6.1  88/04  wong
**	Completed adding support for repeat queries.
**	88/08  Added support for QS_VALGNSV ("no save" QS_VALGEN elements.)
**	88/10  Replaced calls to 'IIputdomio()' with 'IILQdtDbvTrim()'.
**
**	Revision 6.0  87/05  wong
**	Version 2.0.
**
**	11/22/92 (dkh) - Removed SIflush() call after each call to
**			 qg_nexttuple().  This is unnecessary and is
**			 very SLOW on the alpha.  Reason for the latter
**			 is unknown at the moment.
**
**      05/03/93 (Donc)- Reinstated SIflush call. SIwrite cannot determine
**			 if the filesystem is full. Without SIflush (which
**			 can detect full filesystems), SIwrite blithely goes
**			 about its business without writing to a full 
**			 filesystem. The lowdown is that an incorrect number
**			 of records is returned to the caller. (50215)	
**			
**	15-jul-93 (joe)
**		Added #include <qgv0.h>
**	21-jul-93 (joe)
**		Added EXEC SQL INCLUDE SQLCA to the include section
**	03-sep-93 (chan)
**		Add LOADDS modifier for multi_rows_OK.
**	12/07/93 (lan) - Fix bug #41650 where the GROUP BY clause in the SELECT
**			 query of the LOOK_UP frame was not passed to the DBMS.
**	12/93 (Mike S) - Use HUGE pointers for DB_DATA_VALUE array, pointed
**			 to by qg_base[0], which is potentially > 64K.
**			 We won't worry about the array pointed to by
**			 qg_base[1] (used in qg_describe), because describing
**			 a query can't possibly result in 5000 DB_DATA_VALUEs.
**			
**	12/93 (Mike S) 	Let caller specify stream acess routines, instead of
**			always using SI.  See header of IIQG_streamrtns for
**			details
**	18-jan-94 (chan)
**		Add missing paremeter to qg_qrygen.
**	21-mar-94 (davel)
**			Silently ignore any DBI-specific query spec (e.g.
**			QS_THOLDLOCK).
**	30-mar-94 (sandyd)
**			For DBI, ifdef'ed out anything that would generate
**			LIBQ references.  (Now that we are using the DLL's, 
**			and sharing the DLL .def files with non-DBI, we can't
**			keep this module from showing up.  So we have to make 
**			it harmless.)
**	4/93 (Mike S)
**			Optionally generate LIBQ errors on map mode file errors.
**	13-oct-94 (leighb)
**		Changed static's to static's.
**	11-jan-95 (sandyd)
**		When porting to Unix, I discovered one libq reference that
**		was never ifdef'ed out for DBI --- IIsqMods() in qg_sqmods().  
**		Why did this never show up on the PC?  I suspect the compiler 
**		just optimized it away (since the libq call was only used in a
**		static routine that never gets referenced by anyone when 
**		compiled with DBI on).
**	01-jun-95 (sandyd)
**		Replaced HUGE with HUGEPTR (now in compat.h) to clean up
**		Unix compile problems.
**	27-dec-96 (donc)
**		Clean out non-NT definitions that either do or do not exist
**		under OpenINGRES 2.0.  Do not explicitly declare MRreqmem,
**		do away with STATICDEF in favor of static, delete references
**		to HUGEPTR.
**	27-dec-1996 (donc)
**		Accidentaly left a few HUGEPTRs in, took them out.
**      02-jan-97 (mcgem01)
**              Various builds have been broken by the introduction of the
**              LOADDS compiler directive.  This modifier should have been
**              added for Windows 3.1 only.
**      22-Nov-99  (hweho01)
**              Turned off optimizer for ris_u64. Symptom: get error msg
**              "Memory allocation failure" while hit listchoice(9) during
**              FE/vision tests (#15,#30,#52 and #59 sep test).
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	16-aug-2001 (toumi01)
**	    speculative i64_aix NO_OPTIM change for beta xlc_r - FIXME !!!
**	13-Apr-2005 (hweho01)
**	    Avoid fatal compiler error on AIX, not use the old style    
**          parameter list for qg_nexttuple() routine. 
**	07-sep-2005 (abbjo03)
**	    Replace xxx_FILE constants by SI_xxx.
*/
typedef struct qg_fhandle
{
    FILE *qg_fptr;
    i4 qg_recsize;
    LOCATION qg_loc;
    char qg_locbuf[MAX_LOC + 1];
} QG_FHANDLE;
#ifndef MEcalloc
#define MEcalloc(size, status) MEreqmem(0, (size), TRUE, (status))
#endif
#ifndef MEalloc
#define MEalloc(size, status) MEreqmem(0, (size), FALSE, (status))
#endif
#define	QI_QBF	0105		/* 6.0 QBF / 5.0 OSL (do not intersect) */
static STATUS	genstatus = OK;
static i4  (*old_seterr_func)();
static bool	too_many_rows;
static STATUS	qg_ch_init();
static VOID	qg_ch_clear();
static VOID	qg_qrygen();
static STATUS	qg_describe();
static VOID	qg_qryvalue();
static STATUS	qg_nexttuple(QDESC *, i4, bool, QRY_ARGV [], DB_DATA_VALUE (*[])[]);
static VOID	qg_tupcnt();
static STATUS	qg_getdata();
static VOID	qg_sqmods();
# ifdef WIN16
static i4 LOADDS multi_rows_OK();
# else
static i4  multi_rows_OK();
# endif
static VOID	qg_flush_singleton();
static PTR 	open_rtn();
static STATUS 	rewind_rtn();
static STATUS 	read_rtn();
static STATUS	write_rtn();
static STATUS	close_rtn();
static QG_STREAM_RTN stream_rtns =
{
	open_rtn,
	rewind_rtn,
	read_rtn,
	write_rtn,
	close_rtn
};
/*
** Data to determine how to handle map mode file errors
*/
static bool fake_libq_errors = FALSE;
/*{
** Name:	IIQG_generate() -	Generate and Run Query.
**
** Description:
**	The generate procedure, called to start and run a query.  The query can
**	be either a master or detail.  The query is run according to the input
**	parameter.  It can have five possible values:
**
**		QI_START	Starts the query and gets the first record.
**		QI_GET		Gets the next record from the query.
**		QI_BREAK	Breaks out of the query.
**		QI_NMASTER	Breaks out of the detail query and gets the
**					next master record and detail.
**		QI_DESCRIBE	Allocate target list (argument vector and
**				data values) for an SQL query.
**
** Input:
**	qry	{QDESC *}  The description for the query to run.
**	input	{nat}  The action to take running the query.
**
** Output:
**	output	{nat *}  The state in which the query ended.
**
** Returns:
**	{STATUS}  OK or 1-99	A specific error code for QG.
**			>100	An INGRES error.
**
** Side Effects:
**	QI_DESCRIBE will allocate memory in the current FE memory tag region.
**
** History:
**	09/06/85 (scl) Fixed bug in CMS; temp file must be
**			opened for input as 'binary' file
**	16-jun-86 (mgw) Bug # 9582
**		Made QG_alloc() return NULL on bad memory allocation
**		and made QG_generate() understand that state of affairs
**		even if called recursively.
**	04/87 (jhw) -- Modified extensively.
**	24-jun-91 (davel)
**		For repeat queries, pass datavalues from the qg_where
**		QRY_SPEC list if it's defined. (Bug 38137).
**	1991/08/15  davel
**		Re-issue IIsqInit in repeat query do/while
**		loop so that LIBQ will handle SQL errors correctly on the
**		Execute pass. (Bug 39108).
*/
i4	(*IIseterr())();
i4	IIno_err();
STATUS
IIQG_generate (qry, input, output)
register QDESC	*qry;		/* The query to run. */
i4		input;		/* The action to take running the query. */
i4		*output;	/* The state of the query. */
{
# ifdef DBI
	return (OK);
# else /* not DBI */
    register QSTRUCT	*qint = qry->qg_internal;
    bool		repeat = (qry->qg_mode & QM_REPEAT) != 0;
    bool		singleton = (qry->qg_mode & QM_SINGLE) != 0;
    i4			qmode = qry->qg_mode& ~(QM_REPEAT|QM_EXECIMM|QM_SINGLE);
    STATUS		rval = OK;
    if (qry->qg_new != NULL)
	return QG_generate(qry, input, output);
    if (qry->qg_child != NULL && qmode != QM_CURSOR)
	qmode = QM_MAP;	/* map all non-cursor master-detail queries */
    switch (input)
    {
      case QI_DESCRIBE:
	rval = qg_describe(qry);	/* Allocate QRY_ARGV for query */
	*output = QO_QDONE;
	return rval;
      case QI_BREAK:
	if (qint == NULL)
	{
	    /* ABFRT frees the query on QO_QDONE
	    ** so QI_BREAK is a no-op in this case.
	    */
	    *output = QO_QDONE;
	    return OK;
	}
	if ((qint->qg_state & QS_IN) == 0)
	{
	    if (qry->qg_child != NULL)
		qg_ch_clear(qry);
	    *output = QO_QDONE;
	    return OK;
	}
	if (qry->qg_child != NULL && (qint->qg_state & QS_INDETAIL) != 0)
	    rval = IIQG_generate(qry->qg_child, QI_BREAK, output);
	if (qmode == QM_CURSOR)
	    /* do cursor break */;
	else if (singleton)
	    qg_flush_singleton();
	else if (qmode != QM_MAP && qry->qg_child == NULL)
	    IIbreak();
	else
	{ /* close mapped query */
	    stream_rtns.close_rtn((PTR)qint->qg_file);
	    qint->qg_file = NULL;
	}
	break; /* end query */
      case QI_START:
      case QI_USTART:
      case QI_QBF:	/* internal only (from QI_GET for detail query below) */
      {
	i4	(*_svfunc)();	/* for generation errors */
	if (qint == NULL)
	{
	    if ((qint = qry->qg_internal =
		    (QSTRUCT *)MEcalloc(sizeof(*qint), (STATUS *)NULL)) == NULL)
	    {
		IIUGerr(QE_MEM, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		return QE_MEM;
	    }
	}
	else
	{
	    qint->qg_state &= ~(QS_IN|QS_INDETAIL);
	    qint->qg_tupcnt = 0;
	}
	/* Generate query to DBMS */
	genstatus = OK;
	if (qry->qg_dml == DB_SQL)
	{
	    IIsqInit((char *)NULL);
	    /* 
	    ** Tell libq if this is an EXECUTE IMMEDIATE or a non-repeated
	    ** singleton select.
	    */
	    if (!repeat)
	    	qg_sqmods(qry->qg_mode);
	}
	/*
	** If this is a master-detail query that is not a REPEAT or a CURSOR,
	** then the where clause of the detail will have to be sent to the DBMS
	** once for each master.  To prepare for this, initialize the child's
	** internal structure so it will save the where clause of the detail.
	*/
	if (qry->qg_child != NULL && (rval = qg_ch_init(qry)) != OK)
	{
	    IIUGerr(rval, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	    return rval;
	}
	if (qry->qg_mode == QM_CURSOR)	/* (!repeat && qmode == QM_CURSOR) */
	    IIcsOpen(qry->qg_name, qry->qg_num[0], qry->qg_num[1]);
	/*
	** Send the query to the DBMS unless it is a repeat query
	** and was already defined or until it gets defined.
	*/
	do	/* ... once for non-repeat queries */
	{
	    i4		repcnt = repeat ? 0 : -1;
	    /* ... try to execute a repeat query first, then define it if that
	    ** fails and try to execute it again (hence the do loop.)  Query
	    ** definition is very similar for both repeated and non-repeated
	    ** queries.  The latter just get defined (which starts execution.)
	    **
	    ** Note:  Cursors can be repeated as well.
	    */
	    if (repeat)
	    { /* try to execute the repeated query */
		i4	type = (qmode == QM_CURSOR ? II_EXCURSOR : II_EXQUERY);
		IIsexec();
		IIexExec(type, qry->qg_name, qry->qg_num[0], qry->qg_num[1]);
		/* Send the datavalues to the DBMS */
		qg_qryvalue( *qry->qg_tlist, qry->qg_base );
		/* Also send datavalues in the whereclause
		** if qg_where is defined for this query (bug 38137)
		*/
		if ( *qry->qg_where != NULL )
		{
		    qg_qryvalue( *qry->qg_where, qry->qg_base );
		}
		if (qmode == QM_CURSOR)
		    IIcsQuery(qry->qg_name, qry->qg_num[0], qry->qg_num[1]);
		else
		    IIretinit((char *)NULL, 0);
		/* Check execution */
		if (IInexec() != 0)
		    break;	/* Query execution started:  end loop! */
		/* ... else query not executed:  Must be defined first. */
		if ( qry->qg_dml == DB_SQL )
		{
		    IIsqInit((char *)NULL);
		    qg_sqmods(qry->qg_mode);
		}
		IIexDefine( type, qry->qg_name,
				(i4)qry->qg_num[0], (i4)qry->qg_num[1]
		);
	    }
	    /* Define the query to the DBMS.  Repeated queries must send variant
	    ** datavalues specially.  See 'qg_qrygen()', below for details.
	    */
	    if ( input != QI_QBF && (qint->qg_state & QS_QBF) == 0 )
	    {
		QGSAVELIST	**save;
		save = ( (qint->qg_state & QS_SAVWHERE) != 0 )
				? &qint->qg_where : (QGSAVELIST **)NULL;
		qg_qrygen( ERx(""), *qry->qg_tlist, qry->qg_base, &repcnt,
				save, qint->qg_wtag
		);
		if ( *qry->qg_where != NULL )
		{
			qg_qrygen( ERx(""), *qry->qg_where, qry->qg_base,
				&repcnt, save, qint->qg_wtag
			);
		}
		if ( *qry->qg_from != NULL )
		{ /* from used for `rest of query' */
			qg_qrygen( ERx(""), *qry->qg_from, qry->qg_base,
				&repcnt, save, qint->qg_wtag
			);
		}
		if ( *qry->qg_order != NULL )
		{
			qg_qrygen( ERx(""), *qry->qg_order, qry->qg_base,
				&repcnt, save, qint->qg_wtag
			);
		}
	    }
	    else
	    { /* 6.0 QBF query */
		if (input == QI_USTART)
		{ /* unique target list */
			static char	*kw_unique[] = {
					    ERx("retrieve unique"),
					    ERx("select distinct ")
					};
			qg_qrygen( kw_unique[qry->qg_dml-1], *qry->qg_tlist,
				qry->qg_base, &repcnt, (QGSAVELIST **)NULL, 0
			);
		}
		else
		{ /* non-unique target list */
			static char	*kw_query[] = {
					    ERx("retrieve"),
					    ERx("select ")
			   		};
			qg_qrygen( kw_query[qry->qg_dml-1], *qry->qg_tlist,
				qry->qg_base, &repcnt, (QGSAVELIST **)NULL, 0
			);
		}
		if (qry->qg_dml == DB_SQL)
		{
			qg_qrygen( ERx(" from "), *qry->qg_from, 
				qry->qg_base, &repcnt, (QGSAVELIST **)NULL, 0
			);
		}
		qg_qrygen( ERx(" where "), *qry->qg_where, qry->qg_base,
				&repcnt,
				((qint->qg_state & QS_SAVWHERE) != 0 
					? &qint->qg_where
					: (QGSAVELIST **)NULL),
				qint->qg_wtag
		);
		qg_qrygen( qry->qg_dml == DB_SQL
					? ERx(" order by ") : ERx(" sort by "),
				*qry->qg_order, qry->qg_base,
				&repcnt, (QGSAVELIST **)NULL, 0
		);
	    }
	    if ( genstatus != OK )
	    { /* generation error! */
		/* Catch possible DBMS parsing errors.  JupBug #3648. */
		_svfunc = IIseterr( IIno_err );
		/* Make sure the query really is in error. */
		IIwritio(FALSE, (i2 *)NULL, TRUE, DB_CHR_TYPE, 0,
			ERx("$junk$")
		);
	    }
	    if (repeat)
	    {
		if (qmode == QM_CURSOR)
		    IIcsRdO(3, (char *)NULL);
		IIexDefine( II_EXQREAD, qry->qg_name,
				(i4)qry->qg_num[0], (i4)qry->qg_num[1]
		);
		if ( qry->qg_dml == DB_SQL )
		    IIsqInit((char *)NULL);
	    }
	    else if (qmode == QM_CURSOR)
		IIcsQuery(qry->qg_name,(i4)qry->qg_num[0],(i4)qry->qg_num[1]);
	    else
		IIretinit((char *)NULL, 0);
	    /* Check for query definition errors.  Errors exit this module! */
	    if ( genstatus != OK )
	    { /* generation error! */
		/* Restore LIBQ error handler */
		_VOID_ IIseterr( _svfunc );
		/* No need for 'IIbreak()' since we made sure
		** that there was an error!  (JupBug #4028.)
		*/
		qint->qg_tupcnt = 0;
		if (qry->qg_child != NULL)
		    qg_ch_clear(qry);
		*output = QO_QDONE;
		return genstatus;
	    }
	    else if (IIerrtest() != 0)
	    {
		if (qry->qg_child != NULL)
		    qg_ch_clear(qry);
		*output = QO_QDONE;
		return FEinqerr();	/* Error! */
	    }
	} while (repeat);
	/* Start fetching the data for the query */
	if (qmode == QM_MAP)
	    if ((rval = qg_getdata(qry)) != OK)
	    { /* error! */
		IIbreak();	/* make sure query is terminated */
		qint->qg_tupcnt = 0;
		if (qry->qg_child != NULL)
		    qg_ch_clear(qry);
		*output = QO_QDONE;
		if ( rval == QE_TFILE || rval == QE_TFWRITE || rval == QE_MEM )
			IIUGerr(rval, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		return rval;	/* Error! */
	    }
	qint->qg_state |= QS_IN;
      } /* fall through ... */
      /*
      ** At this point, we are ready to generate a new value.
      */
      case QI_NMASTER:
	/* Break out of child */
	if ((qint->qg_state & QS_INDETAIL) != 0 &&
		(rval = IIQG_generate(qry->qg_child, QI_BREAK, output)) != OK)
	{
	    qint->qg_state &= ~QS_INDETAIL;
	    *output = QO_DDONE;
	    return rval;	/* Error! */
	}
	qint->qg_state &= ~QS_INDETAIL;
	/* fall through ... */
      case QI_GET:
	if ((qint->qg_state & QS_IN) == 0)
	{
	    if (qry->qg_child != NULL)
		qg_ch_clear(qry);
	    *output = QO_QDONE;
	    IIUGerr(QE_NOSTART, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	    return QE_NOSTART;	/* Error! */
	}
	/* Get value(s)
	**
	** At this point, three possible actions can be taken:
	**
	**	1)  Get a tuple for the query.
	**	2)  Get a detail tuple for a master-detail.
	**	3)  Get a tuple and a detail tuple for a master-detail.
	**
	** So, either a detail is active or the next tuple for the query
	** should be retrieved (next master for a master-detail.)  In
	** either case a query will be active (mark state as such.)
	**
	** Then, either it's not a master-detail, in which case a tuple was
	** successfully retrieved.  Or it is a master-detail with either a
	** detail active or a master tuple just retrieved, in which case a
	** detail tuple should be retrieved either directly or by starting
	** the detail query.
	**
	** After this any of the three possible actions has completed
	** successfully.  If a query completed, it must have been the detail
	** query in which case mark the detail as being inactive and note that
	** state.  Otherwise, if there is a detail query, it is active at this
	** point.  Finally, return successful completion.
	*/
	if ((qint->qg_state & QS_INDETAIL) != 0 ||
		qg_nexttuple(qry, qmode, singleton, qry->qg_argv, qry->qg_base)
			== OK)
	{
	    *output = QO_QIN;
	    if (qry->qg_child == NULL ||
		    (rval = IIQG_generate(qry->qg_child,
		      ((qint->qg_state & QS_INDETAIL) != 0 ? QI_GET : 
			  ((qint->qg_state & QS_QBF) != 0 ? QI_QBF : QI_START)),
			output)) == OK)
	    {
		if (*output == QO_QDONE)
		{ /* end of child query */
		    qint->qg_state &= ~QS_INDETAIL;
		    *output = QO_DDONE;
		}
		else if (qry->qg_child != NULL)
		    qint->qg_state |= QS_INDETAIL;
		return OK;
	    }
	}
	break; /* end query */
    } /* end switch */
    /* End query:  A query has ended either successfully or not */
    qint->qg_state &= ~QS_IN;
    if (rval == OK && (qmode == QM_MAP || (rval = FEinqerr()) == OK))
	qg_tupcnt(qint->qg_tupcnt);
    qint->qg_tupcnt = 0;
    if (qry->qg_child != NULL)
	qg_ch_clear(qry);
    *output = QO_QDONE;
    return rval;
# endif /* DBI */
}
/*{
** Name:	IIQG_streamrtns - Specify stream-handling routines.
**
** Description:
**	The caller is providing a set of  QG_STREAM_RTN functions to
**	manage the stream used by map mode.  The routines 
**	which must be provided are:
**
**          PTR (*open_rtn)(recordsize);  	
**    		i4 recordsize;		-- Input -- size of tuple 
**
**	    Open the stream with the given recordsize.  Returns a handle
**	    to the stream.  Return NULL on failure.
**
**          STATUS (*rewind_rtn)(handle)
**		PTR handle;		-- stream's handle
**
**	    Rewind the stream, after having written all of the tuples,
**	    to allow reading the tuples in the order written.
**
**          STATUS (*read_rtn)(handle, buffer, cnt)
**		PTR handle;		-- stream's handle 
**		PTR buffer;		-- input buffer
**		i4 *count;		-- (output) number of bytes read
**
**	    Read 'recordsize' bytes from the stream (i.e. the next tuple).
**	    Return OK if data was available, non-OK if not.  There is no
**	    special indicator for EOF.
**
**          STATUS (*write_rtn)(handle, buffer) 
**		PTR handle;		-- stream's handle
**		PTR buffer;		-- output buffer 
**
**	    Write 'recordsize' bytes to the stream (i.e. the next tuple).
**
**          STATUS (*close_rtn)(handle)
**      	PTR handle;             -- stream's handle
**
**	    Close the stream, freeing any associated resources.
**
**      The default routines provided in this file map the stream to
**	an SI file.
**
** Inputs:
**	routines 	QG_STREAM_RTN *	New routines
**
**
** Re-entrancy:
**	In itself re-entrant, but multiple threads in a process must use
**	the same set of routines.
**
** Side Effects:
**	Overwrites the static stream_rtns structure.
**
** History:
**	12/93 (Mike S) Initial version
*/
VOID
IIQG_streamrtns(routines)
QG_STREAM_RTN * routines;
{
    STRUCT_ASSIGN_MACRO(*routines, stream_rtns);
}
/*{
** Name:	IIQG_fakelibqerrors - fake LIBQ errors on map mode file errors
**
** Description:
**	To enable users to trap errors resulting from file errors in map mode,
**	fake libq errors when that occurs.
**
** Inputs:
**	enable	bool	TRUE to create LIBQ errors; FALSE not to
**
** Re-entrancy:
**	Sets static variables; all threads in process will have the same
**	error behavior.
**
** History:
**	4/94 (Mike S) Initial version
*/
VOID
IIQG_fakelibqerrors(enable)
bool enable;
{
    fake_libq_errors = enable;
}
/*{
** Name:	qg_describe() -	Allocate Argument Vector for Query.
**
** Description:
**	Given a query descriptor determines the result columns to be returned
**	and allocates data values to hold them.  Then sets the argument vector
**	to these data values.  If the argument vector that was passed into this
**	routine is empty, it will be allocated as well.  (This should be the
**	case when the query contains a "*" specification.)
**
** Input:
**	qry	{QDESC *}  The description for the query to run.
**
** Output:
**	qry	{QDESC *}  The description for the query to run.
**		    .qg_argv	{QRY_ARGV (*)[]}  The argument vector.
**		    .qg_base[1]	{DB_DATA_VALUE (*)[]}  The data value array.
**
** Returns:
**	{STATUS}  OK if no errors.
**
** Side Effects:
**	Allocates memory in the current FE memory tag region.
**
** History:
**	08/89 (jhw) -- Written.
**	4/13/92 (seg)
**	    Can't do arithmetic on PTR variables without casting.
**	12/07/93 (lan) - Fix bug #41650 where the GROUP BY clause in the SELECT
**			 query of the LOOK_UP frame was not passed to the DBMS.
*/
#define	QG_COLS_BUF_SIZE	32
typedef IISQLDA_TYPE(da, SQLDA, QG_COLS_BUF_SIZE);
static STATUS
qg_describe ( qry )
register QDESC	*qry;
{
# ifdef DBI
	return (OK);
# else /* not DBI */
	IISQLDA			*sqlda;
	register IISQLVAR	*desc;
	register DB_DATA_VALUE	*dbv;
	register PTR		data;
	i4			size;
	ADF_CB			*cb;
	STATUS			rval = OK;
	SQLDA			local;
	ADF_CB	*FEadfcb();
	char	*FEsalloc();
	if ( qry->qg_dml != DB_SQL )
	{ /* not SQL */
		return FAIL;
	}
	genstatus = OK;
	sqlda = (IISQLDA *)&local;
	sqlda->sqld = sqlda->sqln = QG_COLS_BUF_SIZE;
	do
	{
		i4	(*_svfunc)();	/* for generation errors */
		if ( sqlda->sqld > sqlda->sqln )
		{ /* Allocate larger SQLDA */
			i4	ncols = sqlda->sqld;
			if ( sqlda != (IISQLDA *)&local )
				MEfree((PTR)sqlda);
			if ( (sqlda = (IISQLDA *)MEcalloc(
				IISQDA_HEAD_SIZE + ( ncols * IISQDA_VAR_SIZE),
					&rval) ) == NULL )
			{
				IIUGerr(QE_MEM, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
				return rval;
			}
			sqlda->sqln = ncols;
		}
		IIsqInit((char *)NULL);
		IIqry_start(GCA_QUERY, GCA_NAMES_MASK, 0, ERx("prepare"));
		IIwritio( FALSE, (i2 *)NULL, TRUE, DB_CHR_TYPE, 0,
				ERx("prepare ")
		);
		IIwritio(TRUE, (i2 *)NULL, TRUE, DB_CHR_TYPE, 0, ERx("ii_qg"));
		IIwritio( FALSE, (i2 *)NULL, TRUE, DB_CHR_TYPE, 0,
				ERx(" into iisqlda using names from ")
		);
		qg_qrygen( ERx(""), *qry->qg_tlist, qry->qg_base, NULL,
				(QGSAVELIST **)NULL, 0
		);
		/* Fix for bug #41650 */
		if ( *qry->qg_where != NULL )
		{
			qg_qrygen( ERx(""), *qry->qg_where, qry->qg_base, NULL,
				(QGSAVELIST **)NULL, 0 );
		}
		if ( *qry->qg_from != NULL )
		{	/* The GROUP BY clause got stuffed in qg_from */
			qg_qrygen( ERx(""), *qry->qg_from, qry->qg_base, NULL,
				(QGSAVELIST **)NULL, 0 );
		}
		if ( *qry->qg_order != NULL )
		{
			qg_qrygen( ERx(""), *qry->qg_order, qry->qg_base, NULL,
				(QGSAVELIST **)NULL, 0 );
		}
		/* End of fix for bug #41650 */
		if ( genstatus != OK )
		{ /* generation error! */
			/* Catch possible DBMS parsing errors. */
			_svfunc = IIseterr( IIno_err );
			/* Make sure the query really is in error. */
			IIwritio(FALSE, (i2 *)NULL, TRUE, DB_CHR_TYPE, 0,
				ERx("$junk$")
			);
		}
		IIsq_RdDesc(hostC, TRUE, sqlda);
		if ( genstatus != OK )
		{ /* generation error! */
			_VOID_ IIseterr(_svfunc); /* restore error handler */
			if ( sqlda != (IISQLDA *)&local )
				MEfree((PTR)sqlda);
			return genstatus;
		}
		else if ( (rval = FEinqerr()) != OK )
			return rval;
	} while ( sqlda->sqld > sqlda->sqln );
	cb = FEadfcb();
	/* Allocate data values and argument vector if empty. */
	if ( (dbv = (DB_DATA_VALUE *)FEcalloc( sizeof(*dbv) * sqlda->sqld,
						&rval)) == NULL
		|| ( qry->qg_argv == NULL && (qry->qg_argv =
		    (QRY_ARGV(*)[])FEcalloc( sizeof(QRY_ARGV) * (sqlda->sqld+1),
						&rval)) == NULL ) )
	{
		if ( sqlda != (IISQLDA *)&local )
			MEfree((PTR)sqlda);
		IIUGerr(QE_MEM, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		return rval;
	}
	/* Set-up data values and Sum required data area (aligned)
	**
	**	Note:  The descriptor lengths are set to be the internal
	**	length for the data value (from being the user length.)
	*/
	size = 0;
	qry->qg_base[1] = (DB_DATA_VALUE (*)[])dbv;
	for ( desc = sqlda->sqlvar ; desc < &sqlda->sqlvar[sqlda->sqld] ;
			++desc )
	{
		i4		align;
		DB_DATA_VALUE	tmp;
		/* Get internal data length */
		dbv->db_datatype = tmp.db_datatype = desc->sqltype;
		if ( AFE_DATATYPE(dbv) != DB_DEC_TYPE )
			tmp.db_length = desc->sqllen;
		else
		{ /* decimal type */
			tmp.db_prec = desc->sqllen;
			tmp.db_length =
			    DB_PREC_TO_LEN_MACRO(DB_P_DECODE_MACRO(desc->sqllen));
		}
		if ( adc_lenchk(cb, TRUE /* is user */, &tmp, dbv) != OK )
		{
			FEafeerr(cb);
			rval = FAIL;
		}
		/* size += IIAFalign(dbv, size) + dbv->db_length; */
		if ( (align = size % sizeof(ALIGN_RESTRICT)) != 0 )
			align = sizeof(ALIGN_RESTRICT) - align;
		size += align + dbv->db_length;
		++dbv;
	}
	dbv = (DB_DATA_VALUE *)qry->qg_base[1];
	/* Allocate data area */
	if ( rval == OK && (data = FEreqlng((u_i4)0, size, TRUE, &rval))
				== NULL )
	{
		IIUGerr(QE_MEM, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	}
	else if ( rval == OK )
	{ /* Set up data values */
		register QRY_ARGV	*argv;
		register i4		i;
		/* IIAFsetData(dbv, sqlda->sqld, data); */
		size = 0;
		for ( i = sqlda->sqld ; --i >= 0 ; ++dbv )
		{
			i4	align;
			if ( (align = size % sizeof(ALIGN_RESTRICT)) != 0 )
				align = sizeof(ALIGN_RESTRICT) - align;
			dbv->db_data = (char *)data + (size += align);
			size += dbv->db_length;
		}
		/* Set-up required argument vectors */
		desc = sqlda->sqlvar;
		argv = *qry->qg_argv;
		dbv = (DB_DATA_VALUE *)qry->qg_base[1];
		for ( i = sqlda->sqld ; --i >= 0 ; ++argv, ++desc )
		{
			argv->qv_dbval = dbv++;
			argv->qv_index = argv - *qry->qg_argv;
			argv->qv_base = 1;
			if ( argv->qv_name == NULL )
			{
				register char	*name = desc->sqlname.sqlnamec;
				name[desc->sqlname.sqlnamel] = EOS;
				if ( (argv->qv_name = FEsalloc(name)) == NULL )
					rval = QE_MEM;
			}
		}
	}
	if ( sqlda != (IISQLDA *)&local )
		MEfree((PTR)sqlda);
	return rval;
# endif /* DBI */
}
/*}
** Name:    SENDSTRUCT -	Structure for communication to IIQG_send
**
** Description:
**	Contains a memory tag and a savelist passed to 'IIQG_send()' through
**	the QS_VALGEN query specification interface.  Used to communicate
**	between 'qg_qrygen()' and 'IIQG_send()'.
**
** History:
**	22-may-1987 (Joe)
*/
typedef struct
{
    i2		tag;
    QGSAVELIST	*savelist;
} SENDSTRUCT;
/*
** Name:    qg_qrygen() -	Generate Query to DBMS.
**
** Description:
**	Generates and sends a part of a query from a query specification to the
**	DBMS using the LIBQ interface.  Each element of the query specification
**	is one of the following:
**
**	    A text string that contains part of the text for a query, and is
**	    sent directly to the DBMS using 'IIwritio()'.
**
**	    A dynamic value that contains part of the text for a query as a
**	    string value, which was dynamically set, and is sent as a string
**	    value to the DBMS using 'IIwritio()'.
**
**	    A data value that contains a data value for the query, and is sent
**	    directly to the DBMS (as a DB_DATA_VALUE) using 'IILQdtDbvTrim()'
**	    (cover for 'IIputdomio()'.)
**
**	    A reference to a query generator that specifies a function and
**	    data with which to generate a part of a query specification to be
**	    sent to the DBMS (recursively using 'IIQG_send()'.)
**
**	Repeated queries must define data values (i.e., QS_VALUE elements) to
**	the DBMS specially.  That is, they are preceded by a string of the form
**	" $n=" where n is the index of the data value for the query.  Each time
**	the query is executed, these variant values will be sent to the DBMS.
**
**	By definition, repeated queries cannot contain query generator elements
**	(QS_VALGENs or QS_VALGNSVs) that modify the query each time it is
**	executed (unless they simply generate only QS_VALUE elements) since
**	repeat queries must be fixed when the query is defined.
**
** Input:
**	key	{char *}  Initial keyword string for query.
**	spec	{QRYSPEC []}  Query specification.
**	base	{DB_DATA_VALUE (*[])[]}  Base reference for DB_DATA_VALUEs.
**	repcnt	{nat *}  Reference to repeat variable count (to be used and
**			incremented when query values are sent to the DBMS for
**			a repeat query.)
**	save	{bool} If TRUE, then any generated portions of the query spec.
**			(usually a where clause) resulting from a QG_VALGEN
**			QRY_SPEC should be saved.
**
**	tag	{i2}  A tag to allocate any saved portions of the query spec.
**
**	savelist {QGSAVELIST **} The reference to savelist for a where clause.
**			If this is NULL, then this clause in the query cannot
**			have any portion saved.
**			If it is not NULL, then two cases can occur:
**			In the first, the savelist is NULL.  This means that
**			nothing has been saved yet, so go ahead and save any
**			generated QRY_SPECs.
**			In the second case, the savelist is not NULL, which
**			means something has already been saved, so use it.
**
** History:
**	03/87 (jhw) -- Written.
**	04/88 (jhw) -- Added 'repcnt' as a parameter for repeat query support.
**			Also, collapsed 'save' and 'savelist'.
**	3/92 (Mike S) 	Add case for Execute Immediate TEXT.  This goes through
**			a LIBQ routine that guarantees it's a SELECT.
**  10-mar-2000 (rodjo04) bug 98775
**       Added support for Float data type.
**  
*/
static QGSAVELIST   *qgsl_alloc();
static VOID	    qgsp_output();
/*VARARGS5*/
static VOID
qg_qrygen ( key, spec, base, repcnt, savelist, tag )
char		    *key;
register QRY_SPEC   *spec;
DB_DATA_VALUE	    (*base[])[];
i4		    *repcnt;
QGSAVELIST	    **savelist;
i2		    tag;
{
# ifndef DBI
    bool	usesaved = (bool)( savelist != NULL && *savelist != NULL );
    bool	repeat = (bool)( repcnt != NULL && *repcnt >= 0 );
    bool	cursor = (bool)( repcnt == NULL);
    STATUS  IIQG_send();
    if ( spec != NULL && spec->qs_type != QS_END && genstatus == OK )
    {
	IIwritio(FALSE, (i2 *)NULL, TRUE, DB_CHR_TYPE, 0, key);
	do
	{
	    register DB_DATA_VALUE * dbvarray;
	    switch (spec->qs_type)
	    {
	      case QS_TEXT:
		IIwritio(FALSE, (i2 *)NULL, TRUE, DB_CHR_TYPE, 0,
			(char *)spec->qs_var
		);
		break;
	      case QS_EITEXT:
		IIsqExImmed((char *)spec->qs_var);
		break;
	      case QS_QRYVAL:
		IIwritio(TRUE, (i2 *)NULL, TRUE, DB_DBV_TYPE, 0,
		    (DB_DATA_VALUE *)spec->qs_var
		);
		break;
	      case QS_QRYVAL|QS_BASE:
		dbvarray = *base[spec->qs_base];
		switch (abs((dbvarray[spec->qs_index]).db_datatype))
		{
		     case  DB_INT_TYPE:
 		     case  DB_FLT_TYPE:
	            	IILQdtDbvTrim( &(dbvarray[spec->qs_index]) );
			       break;
	        default:
		          IIwritio(TRUE, (i2 *)NULL, TRUE, DB_DBV_TYPE, 0,
		          &(dbvarray[spec->qs_index]) );
				   break;
		}
		break;
	      case QS_VALUE:
	      case QS_VALUE|QS_BASE:
		if ( cursor )
		{
		    IIwritio(TRUE, (i2 *)NULL, TRUE, DB_CHR_TYPE, 0, " ? ");
		}
		else
		{
		    if (repeat)
		    { /* special variant precedent for repeated query datavalue */
			char	buf[11];
			IIwritio( TRUE, (i2 *)NULL, TRUE, DB_CHR_TYPE, 0,
					STprintf(buf, ERx(" $%d="), (*repcnt)++)
			);
		    }
		    dbvarray = *base[spec->qs_base];
		    IILQdtDbvTrim( spec->qs_type == QS_VALUE
				    ? (DB_DATA_VALUE *)spec->qs_var :
				    &(dbvarray[spec->qs_index])
		    );
		}
		break;
	      case QS_VALGNSV:	/* do not save generated query specifications */
	      case QS_VALGEN|QS_VALGNSV:
		if ( repeat )
		    genstatus = QG_BADSPEC;	/* error! */
		else
		{
			genstatus = (*((QSGEN *)spec->qs_var)->qsg_gen)(
					    ((QSGEN *)spec->qs_var)->qsg_value,
					    IIQG_send,
					    (PTR)NULL
			);
			if ( genstatus == QG_NOSPEC )
			{
				IIwritio(FALSE, (i2 *)NULL, TRUE, DB_CHR_TYPE,
						0, ERx(" 1=1 ")
				);
				genstatus = OK;
			}
		}
		break;
	      case QS_VALGEN:	/* save generated query specifications */
		if ( repeat )
		    genstatus = QG_BADSPEC;	/* error! */
		else
		{
		    if ( !usesaved )
		    { /* generate (and save) query specifications */
		   	SENDSTRUCT	sendstruct;
		    	SENDSTRUCT	*sptr = NULL;
			if ( savelist != NULL )
			{
			    if ( (*savelist = qgsl_alloc(tag)) == NULL )
			    {
				genstatus = QE_MEM;
				return;
			    }
			    (*savelist)->qgsl_specs = NULL;
			    sendstruct.tag = tag;
			    sendstruct.savelist = *savelist;
			    sptr = &sendstruct;
			}
			genstatus = (*((QSGEN *)spec->qs_var)->qsg_gen)(
				    ((QSGEN *)spec->qs_var)->qsg_value,
				    IIQG_send,
				    (PTR)sptr
			);
			if (genstatus == QG_NOSPEC)
			{
			    IIwritio(FALSE, (i2 *)NULL, TRUE, DB_CHR_TYPE, 0,
				    ERx(" 1=1 ")
			    );
			    genstatus = OK;
			}
			if ( savelist != NULL )
			{
			    savelist = &((*savelist)->qgsl_next);
			}
		    }
		    else
		    { /* use already saved list */
			/* assert:  savelist != NULL */
			if ( *savelist == NULL )
			{
			    IIwritio(FALSE, (i2 *)NULL, TRUE, DB_CHR_TYPE, 0,
				    ERx(" 1=1 ")
			    );
			}
			else
			{
			    qgsp_output((*savelist)->qgsl_specs);
			    savelist = &((*savelist)->qgsl_next);
			}
		    }
		}
		break;
	      case QS_THOLDLOCK:
	      case QS_VHOLDLOCK:
	      case QS_TFORUPDATE:
	      case QS_VFORUPDATE:
		/* these are DBI constructs only; ignore them */
		break;
	      default:    /* including QS_MARK */
		genstatus = QG_BADSPEC;	/* error! */
		break;
	    } /* end switch (same as 'IIQG_send()' below) */
	} while ((++spec)->qs_type != QS_END && genstatus == OK);
    }
# endif /* DBI */
}
/*
** Name:    qg_qryvalue() -	Send Repeat Query Values to DBMS.
**
** Description:
**	Sends the query data values from a query specification to the DBMS using
**	the LIBQ interface.  Used when a defined query (i.e., a REPEATED query)
**	is to be executed again.  (The query data values are the QS_VALUE
**	elements of the query specification.)
***
** Input:
**	spec	{QRYSPEC []}  Query specification.
**	base	{DB_DATA_VALUE (*[])[]}  Base reference for DB_DATA_VALUEs.
**
** History:
**	04/88 (jhw) -- Added to support repeat queries.
*/
static VOID
qg_qryvalue ( spec, base )
register QRY_SPEC   	*spec;
DB_DATA_VALUE		(*base[])[];
{
# ifndef DBI
	if (spec != NULL && spec->qs_type != QS_END && genstatus == OK)
	{
		do
		{
			register DB_DATA_VALUE * dbvarray;
			switch (spec->qs_type)
			{
			  case QS_VALUE:
				IILQdtDbvTrim( (DB_DATA_VALUE *)spec->qs_var );
				break;
			  case QS_VALUE|QS_BASE:
				dbvarray = *base[spec->qs_base];
				IILQdtDbvTrim( &(dbvarray[spec->qs_index]) );
				break;
			  default:	/* including QS_MARK */
				break;
			} /* end switch */
		} while ((++spec)->qs_type != QS_END && genstatus == OK);
	}
# endif /* DBI */
}
/*
** Name:    IIQG_send() -	Send Query Specification Element to DBMS.
**
** Description:
**	Sends a query specification element to the DBMS using the LIBQ
**	interface.  A query specification element is one of the following:
**
**	If the QRY_SPEC needs to be saved, it is saved first.
**
** Input:
**	data	{PTR}	     Value passed to user function that they
**			     pass back.  If NULL doe nothing.
**			     If not NULL it points to a SENDSTRUCT.
**
**	spec	{QRYSPEC *}  Query specification.
**
** Returns:
**	STATUS	OK or error from generator function.
**
** History:
**	05/87 (jhw) -- Written.
*/
static STATUS	qgsp_save();
static STATUS	qgsp_send();
STATUS
IIQG_send (data, spec)
PTR		    data;
register QRY_SPEC   *spec;
{
# ifdef DBI
	return (OK);
# else /* not DBI */
    STATUS	rval;
    if (genstatus == OK)
    {
	if (data != NULL)
	{
	    register SENDSTRUCT	*senddata;
	    senddata = (SENDSTRUCT *) data;
	    if ((rval = qgsp_save(spec, &(senddata->savelist->qgsl_specs),
				senddata->tag)) != OK)
	    {
		return rval;
	    }
	}
	return qgsp_send(spec);
    }
    return genstatus;
# endif /* DBI */
}
/*
** Name:    qgsp_send() -	Send Query Specification Element to DBMS.
**
** Description:
**	Sends a query specification element to the DBMS using the LIBQ
**	interface.  A query specification element is one of the following:
**
**	    A text string that contains part of the text for a query, and is
**	    sent directly to the DBMS using 'IIwritio()'.
**
**	    A dynamic value that contains part of the text for a query as a
**	    string value, which was dynamically set, and is sent as a string
**	    value to the DBMS using 'IIwritio()'.
**
**	    A data value that contains a data value for the query, and is sent
**	    directly to the DBMS (as a DB_DATA_VALUE) using 'IILQdtDbvTrim()'
**	    (cover for 'IIputdomio()'.)
**
** Input:
**	spec	{QRYSPEC *}  Query specification.
**
** Returns:
**	STATUS	OK or error from generator function.
**
** History:
**	22-may-1987 (Joe)
**		Extracted from IIQG_send.
*/
static STATUS
qgsp_send (spec)
QRY_SPEC	*spec;
{
# ifdef DBI
	return (OK);
# else /* not DBI */
    /* Note:  This can never be for a repeat query.  Also, assumes the
    ** specification is a direct reference, i.e., no base references.
    */
    switch (spec->qs_type)
    {
      case QS_TEXT:
	IIwritio(FALSE, (i2 *)NULL, TRUE, DB_CHR_TYPE, 0, (char *)spec->qs_var);
	break;
      case QS_QRYVAL:
	IIwritio(TRUE, (i2 *)NULL, TRUE, DB_DBV_TYPE, 0,
		(DB_DATA_VALUE *)spec->qs_var
	);
	break;
      case QS_VALUE:
	IILQdtDbvTrim( (DB_DATA_VALUE *)spec->qs_var );
	break;
      case QS_VALGEN:
      case QS_VALGNSV:
      case QS_VALGEN|QS_VALGNSV:
	genstatus = (*((QSGEN *)spec->qs_var)->qsg_gen)(
		    ((QSGEN *)spec->qs_var)->qsg_value, IIQG_send, (PTR)NULL
	);
	if (genstatus == QG_NOSPEC)
	{
	    IIwritio(FALSE, (i2 *)NULL, TRUE, DB_CHR_TYPE, 0, ERx(" 1=1 "));
	    genstatus = OK;
	}
	break;
      case QS_THOLDLOCK:
      case QS_VHOLDLOCK:
      case QS_TFORUPDATE:
      case QS_VFORUPDATE:
	/* these are DBI constructs only; ignore them */
	break;
      default:
	genstatus = QG_BADSPEC;
	break;
    } /* end switch */
    return genstatus;
# endif /* DBI */
}
/*
** Name:    qg_nexttuple() -	Get Next Tuple for Query.
**
** Description:
**	Get the next tuple for a query from the DBMS.
**
**	Note:  LIBQ reports an error if either too few or
**	too many column values are retrieved.
**
** Input:
**	q	{QDESC *}  Query description structure.
**	qmode	{nat}  Mode for query.
**	singleton {bool} Singleton select?
**	argv	{QRY_ARGV []}  Retrieved value descriptors.
**	base	{DB_DATA_VALUE (*[])[]}  Base references for DB_DATA_VALUEs.
**
** History:
**	03/87 (jhw) -- Written.
**	01/89 (jhw) -- Added check for 'qg_file' NULL in QM_MAP case.
*/
static STATUS
qg_nexttuple (register QDESC    *qp,
              i4                qmode,
              bool              singleton,
              QRY_ARGV          *argv,
              DB_DATA_VALUE     (*base[])[] )
{
# ifdef DBI
	return (OK);
# else /* not DBI */
    register QRY_ARGV	*ap;
    register QSTRUCT	*qsp = qp->qg_internal;
    QRY_ARGV		dbref[DB_MAX_COLS+1];
    /* Reference DB_DATA_VALUEs */
    if (base != NULL && base[0] != NULL)
    { /* Offset DBVs */
	register QRY_ARGV	*tp = dbref;
	for (ap = argv ; ap->qv_dbval != NULL || ap->qv_name != NULL ; ++ap)
	{
	    register DB_DATA_VALUE * dbvarray;
	    dbvarray = *base[ap->qv_base];
	    tp->qv_dbval = &(dbvarray[ap->qv_index]);
	    tp->qv_name = ap->qv_name;
	    ++tp;
	}
	tp->qv_dbval = NULL;
	argv = dbref;
    }
    /* Retrieve data */
    if (qmode == QM_CURSOR)
    {
	if ( IIcsRetrieve(qp->qg_name, (i4)qp->qg_num[0], (i4)qp->qg_num[1])
		== 0 )
	    return FAIL;
	for (ap = argv ; ap->qv_dbval != NULL ; ++ap)
	    IIcsGetio((i2 *)NULL, TRUE, DB_DBV_TYPE, 0, ap->qv_dbval);
	IIcsERetrieve();
    }
    else if (singleton)
    {
	if (IInextget() == 0)
	{
	    qg_flush_singleton();
	    return FAIL;
	}
	for (ap = argv ; ap->qv_dbval != NULL ; ++ap)
	    IIgetdomio((i2 *)NULL, TRUE, DB_DBV_TYPE, 0, ap->qv_dbval);
    }
    else if (qmode != QM_MAP)
    {
	do
	{
	    if (IInextget() == 0)
	    {
		IIflush((char *)NULL, 0);
		return FAIL;
	    }
	    for (ap = argv ; ap->qv_dbval != NULL ; ++ap)
		IIgetdomio((i2 *)NULL, TRUE, DB_DBV_TYPE, 0, ap->qv_dbval);
	} while (IIerrtest() != 0);
    }
    else
    { /* map mode */
	i4		    cnt;
	register PTR	    *margv;
	if ( qsp->qg_file == NULL )
		return FAIL;	/* no rows fetched */
	if (stream_rtns.read_rtn((PTR)qsp->qg_file, qsp->qg_buffer, &cnt) != OK 
		||
		cnt < qsp->qg_bsize)
	{
	    stream_rtns.close_rtn((PTR)qsp->qg_file);
	    qsp->qg_file = NULL;
	    return FAIL;
	}
	/* If not in QBF, map values in buffer to value DB_DATA_VALUEs */
	if ((qsp->qg_state & QS_QBF) == 0 && qsp->qg_margv != NULL)
	{
	    margv = qsp->qg_margv;
	    for (ap = argv ; ap->qv_dbval != NULL ; ++ap)
	    {
		register DB_DATA_VALUE	*dbv = ap->qv_dbval;
		MEcopy(*margv++, dbv->db_length, dbv->db_data);
	    }
	}
    }
    ++qsp->qg_tupcnt;
    qsp->qg_state |= QS_IN;
    return OK;
# endif /* DBI */
}
/*
** Name:    IIQG_alloc() -	Allocate an Internal Query State Structure.
**
** History:
**	16-jun-86 (mgw) Bug # 9582
**		Get a proper error msg out to the real world by returning
**		a NULL ptr if unable to allocate the memory.
*/
QSTRUCT *
IIQG_alloc()
{
    return (QSTRUCT *)MEcalloc(sizeof(QSTRUCT), (STATUS *)NULL);
}
/*
** Name:    qg_tupcnt()	-	Set the tuple count.
**
** Description:
**	Set the tuple count.  This is called at the end of running a
**	query, and it sets it to the value of tuples returned.
**
** Inputs:
**	cnt	{nat}  The tuple count to which to set the row count.
**
** History
**	16-jan-1986 (joe)
**		First Written
*/
static VOID
  qg_tupcnt(cnt)
  i4 cnt;
  {
# ifndef DBI
/* # line 1636 "qg.qsc" */	/* set_equel */
    {
      IIeqstio((char *)"rowcount",(short *)0,1,30,4,&cnt);
    }
/* # line 1638 "qg.qsc" */	/* host code */
# endif /* DBI */
  }
/*
** Name:    qg_getdata() -	Fetch Query Values into File.
**
** Description:
**	Fetches the values for a query and places them into a temporary file.
**	This supports the QM_MAP functionality.
**
** Input:
**	qp	{QDESC *}  Query descriptor.
**
** History:
**	04/87 (jhw) -- Written.
**	01/89 (jhw) -- Added check for no rows returned, which deletes
**		the file and leaves the 'qg_file' element NULL.  (This
**		will now be checked in 'qg_nexttuple()'.)
**	09-26-1997 (i4jo01)
**		We will now set the error number when the disk write
**		fails (perhaps the temp file space fills up). b78054.
**	22-Mar-2005 (lakvi01)
**		Corrected the prototype of qg_nexttupl().
*/
static QRY_ARGV	*qg_buf();
static STATUS
qg_getdata (qp)
register QDESC    *qp;
{
    register QSTRUCT	*qsp = qp->qg_internal;
    register QRY_ARGV   *argv;
    QRY_ARGV		dbref[DB_MAX_COLS+1];
    LOCATION		loc;
    i4			cnt;
  i4 errn = OK;
    i4			numrows;
    /* Reference DB_DATA_VALUEs */
    argv = *qp->qg_argv;
    if (qp->qg_base[0] != NULL)
    { /* Offset DBVs */
	register QRY_ARGV	*tp = dbref;
	register QRY_ARGV	*ap;
	register DB_DATA_VALUE	(**base)[] = qp->qg_base;
	for (ap = argv ; ap->qv_dbval != NULL || ap->qv_name != NULL ; ++ap)
	{
	    register DB_DATA_VALUE * dbvarray;
	    dbvarray = *base[ap->qv_base];
	    tp->qv_dbval = &(dbvarray[ap->qv_index]);
	    tp->qv_name = ap->qv_name;
	    ++tp;
	}
	tp->qv_dbval = NULL;
	argv = dbref;
    }
    _VOID_ FEbegintag();
    if ((argv = qg_buf(qsp, argv)) == NULL)
    {
	IIUGtagFree(FEendtag());
	return QE_MEM;
    }
    /* Open the file */
    qsp->qg_file = (FILE *)stream_rtns.open_rtn(qsp->qg_bsize);
    if (!qsp->qg_file)
    {
	IIUGtagFree(FEendtag());
	return QE_TFILE;
    }
    /* Map input buffers */
    /* Read tuples */
    while (qg_nexttuple(qp, QM_NORMAL, (bool)FALSE, argv, NULL) == OK)
    {
	if (stream_rtns.write_rtn((PTR)qsp->qg_file, qsp->qg_buffer) != OK)
	{
	    errn = QE_TFWRITE;
/* # line 1557 "qg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(2,(short *)0,1,30,4,&errn);
  }
/* # line 1558 "qg.sc" */	/* host code */
	    break;
	}
    }
    IIUGtagFree(FEendtag());
    qsp->qg_tupcnt = 0;
    /*
    ** BUG 4102
    **
    ** Must check if there was an error in the retrieve or the read.
    ** Also check if no rows were retrieved, which returns OK,
    ** but does not open the file again leaving 'qg_file' NULL.
    */
    if ( errn == OK && (errn = FEinqerr()) == OK);
    {
	numrows = FEinqrows();
	if (stream_rtns.rewind_rtn((PTR)qsp->qg_file) != OK)
	    errn = QE_TFWRITE;
    }
    if (errn != OK || numrows == 0)
    {
	stream_rtns.close_rtn((PTR)qsp->qg_file);
	qsp->qg_file = NULL;
	return errn;
    }
    return OK;
}
/*
** Name:    qg_buf() -	Allocate Buffer for Mapped Query.
**
** Description:
**	Allocates a buffer and related structures for a query that is to be
**	mapped into a file.  The related structures are tag allocated so that
**	they can be freed by the caller.
**
** Input:
**	qsp	{QSTRUCT *}  QG internal state structure for query.
**	argv	{QRY_ARGV *}  Query retrieved value descriptors.
**
** Returns:
**	{QRY_ARGV *}  Temporary retrieved value descriptors for buffer.
**
** History:
**	04/87 (jhw) -- Modified for Version 1.
**	01/90 (jhw) -- Changed 'alloc' to be i4 for portability.
*/
#define qg_align(p) (((p) + (sizeof(ALIGN_RESTRICT)-1)) & ~(sizeof(ALIGN_RESTRICT)-1))
static QRY_ARGV *
qg_buf (qsp, argv)
register QSTRUCT    *qsp;
QRY_ARGV	    *argv;
{
    register QRY_ARGV	*ap;
    DB_DATA_VALUE	*dbv;
    PTR			*mp;
    if ((qsp->qg_state & QS_QBF) != 0)
	return argv;
    if (qsp->qg_buffer == NULL)
    {
	register u_i4	alloc;
	/* Determine allocation sizes */
	qsp->qg_mcnt = 0;
	alloc = 0;
	for (ap = argv ; ap->qv_dbval != NULL ; ++ap)
	{
	    alloc = qg_align(alloc) + ap->qv_dbval->db_length;
	    qsp->qg_mcnt += 1;
	}
	qsp->qg_bsize = alloc;
	/* Allocate buffer and map */
    	if ((qsp->qg_buffer =
		     (char *)MEalloc(qsp->qg_bsize, (STATUS *)NULL)) == NULL ||
	    (qsp->qg_margv =
	       (PTR*)MEalloc((qsp->qg_mcnt*sizeof(PTR)), (STATUS*)NULL)) == NULL
	   )
	{
	    return NULL;
	}
	/* Set map */
	alloc = 0;
	mp = qsp->qg_margv;
	for (ap = argv ; ap->qv_dbval != NULL ; ++ap)
	{
	    *mp++ = qsp->qg_buffer + (alloc = qg_align(alloc));
	    alloc += ap->qv_dbval->db_length;
	}
    }
    /* Allocate (tagged) input structures */
    ap = argv;	/* save input */
    if (((argv = (QRY_ARGV *)FEalloc(((qsp->qg_mcnt + 1) * sizeof(*argv)),
	    (STATUS *)NULL)) == NULL) ||
	((dbv = (DB_DATA_VALUE *)FEalloc((qsp->qg_mcnt * sizeof(*dbv)),
	    (STATUS *)NULL)) == NULL))
    {
	return NULL;
    }
    /* Set temporary query value descriptors */
    MEcopy((PTR)ap, (qsp->qg_mcnt+1)*sizeof(*argv), (PTR)argv); /* copy input */
    mp = qsp->qg_margv;
    for (ap = argv ; ap->qv_dbval != NULL ; ++ap)
    {
	MEcopy((PTR)ap->qv_dbval,sizeof(*dbv),(PTR)dbv);/* copy DB_DATA_VALUE */
	ap->qv_dbval = dbv++;
	ap->qv_dbval->db_data = *mp++;
    }
    return argv;
}
/*{
** Name:    qg_ch_init() -	Mark a child query to save where clause.
**
** Description:
**	If a master-detail query is not a REPEAT or a CURSOR, then the where
**	clause of the child (detail) will be sent to the DBMS once for each
**	master.  Portions of the child's where clause must be saved from the
**	first time the child query is executed.  In particular, any QS_VALGEN
**	QRY_SPEC elements in the child's where clause must be saved.
**
**	This routine marks the child's internal structure to show that
**	the where clause needs to be saved.
**
** Inputs:
**	qry	{QRY *}	The master-detail query.  This MUST BE A
**			master-detail query.
**
** Returns:
**	{STATUS}  OK if initialized successfully.  (Some types of queries
**			do no require initialization.)
**		  FAIL otherwise.
**
** Side Effects:
**	Allocates the child's internal structure, and marks it for
**	where clause saving.
**
** History:
**	22-may-1987 (Joe)
**		Initial Version.
*/
static STATUS
qg_ch_init (qry)
QDESC	*qry;
{
    register QDESC	*child = qry->qg_child;
    register QSTRUCT	*qint;
    if ((qry->qg_mode & (QM_CURSOR|QM_REPEAT)) != 0)
	return OK;
    if (child->qg_internal == NULL &&
	(child->qg_internal = (QSTRUCT *)MEcalloc(sizeof(*qint), (STATUS*)NULL))
	    == NULL)
    {
	return QE_MEM;
    }
    qint = child->qg_internal;
    qint->qg_state |= QS_SAVWHERE;
    qint->qg_where = NULL;
    qint->qg_wtag = FEgettag();
    return OK;
}
/*{
** Name:    qg_ch_clear() -	 Clear a child's internal structure.
**
** Description:
**	This routine clears a child's internal structure of any state
**	information set by 'qg_ch_init()'.
**	
**	This means that the next time the master-detail is run, any QS_VALGEN
**	portion of the child's where clause will get re-evaluated and saved.
**
** Inputs:
**	qry	{QRY *}  The master-detail query.  IT MUST BE A MASTER-DETAIL.
**
** Side Effects:
**	Clears the state information in the child's internal structure.
**
** History:
**	22-may-1987 (Joe)
**		Initial Version
*/
static VOID
qg_ch_clear(qry)
QDESC	*qry;
{
    register QDESC	*child = qry->qg_child;
    register QSTRUCT	*qint;
    if ((qry->qg_mode & (QM_CURSOR|QM_REPEAT)) == 0 &&
		(qint = child->qg_internal) != NULL)
    {
	qint->qg_state &= ~QS_SAVWHERE;
	IIUGtagFree(qint->qg_wtag);
	qint->qg_where = NULL;
    }
}
/*{
** Name:    qgsl_alloc() -	Allocate a QGSAVELIST
**
** Description:
**	This function allocates a QGSAVELIST node.
**
** Inputs:
**	tag		The tag to use to do the allocation.
**
** Outputs:
**	Returns:
**		The pointer to the QGSAVELIST
**		NULL if problems allocating.
**
** History:
**	22-may-1987 (Joe)
*/
static QGSAVELIST *
qgsl_alloc(tag)
i2	tag;
{
    QGSAVELIST	*qs = NULL;
    if ((qs = (QGSAVELIST*)FEreqmem(tag, sizeof(*qs), FALSE, (STATUS*)NULL))
	  != NULL)
    {
	qs->qgsl_next = NULL;
	qs->qgsl_specs = NULL;
    }
    return qs;
}
/*{
** Name:    qgsp_output() -	Send the QRY_SPEC in a QGSAVESPEC to BE.
**
** Description:
**	This routine traverses a QGSAVESPEC and sends all the QRY_SPEC's it
**	contains to the DBMS.
**
**	If the QGSAVESPEC is NULL, then it sends down 1=1.
**
** Inputs:
**	savespecs	The QGSAVESPEC to traverse.
**
** Side Effects:
**	Set genstatus is something bad happens.
**
** History:
**	22-may-1987 (Joe)
**		Initial Version.
*/
static VOID
qgsp_output(savespecs)
QGSAVESPEC	*savespecs;
{
# ifndef DBI
    QGSPCHUNK	*cur;
    i4		numcur;
    if (savespecs == NULL)
    {
	IIwritio(FALSE, (i2 *)NULL, TRUE, DB_CHR_TYPE, 0, ERx(" 1=1 "));
    }
    else
    {
	for (cur = savespecs->qgsp_first, numcur = 0;
	     !(cur == savespecs->qgsp_cur && numcur >= savespecs->qgsp_numcur);
	     numcur++)
	{
	    if (numcur >= QGSPSIZE)
	    {
		cur = cur->qgsp_next;
		numcur = 0;
	    }
	    qgsp_send(&(cur->qgsp_specs[numcur]));
	}
    }
# endif /* DBI */
}
/*{
** Name:    qgsp_save() -	Save a QRY_SPEC into a QGSAVSPC
**
** Description:
**	Saves the QRY_SPEC and the data it contains in the QGSAVSPC.
**
**	The QRY_SPEC is put into the currently available slot
**	in the QGSAVSPC.
**
** Inputs:
**	spec		The QRY_SPEC to save.
**
**	savespecs	The QGSAVSPEC to put it into.
**
**	tag		The tag to use for any allocation.
**
** Outputs:
**	Returns:
**		Any errors associated with allocation.
**
** History:
**	22-may-1987 (Joe)
**		Initial Version
*/
static QGSAVESPEC   *qgsp_alloc();
static QRY_SPEC	    *qgsp_next();
static PTR	    qgsp_spec();
static STATUS
qgsp_save(spec, savespecs, tag)
QRY_SPEC	*spec;
QGSAVESPEC	**savespecs;
i2		tag;
{
    QRY_SPEC	*specholder;
    if (*savespecs == NULL && (*savespecs = qgsp_alloc(tag)) == NULL)
    {
	FEfree(tag);
	genstatus = QE_MEM;
	return QE_MEM;
    }
    if ((specholder = qgsp_next(*savespecs, tag)) == NULL)
	return QE_MEM;
    STRUCT_ASSIGN_MACRO(*spec, *specholder);
    if ((specholder->qs_var = (PTR) qgsp_spec(spec, tag)) == NULL)
	return QE_MEM;
    return OK;
}
/*{
** Name:    qgsp_spec() -	Save the data a spec points at.
**
** Description:
**	Allocate memory and copy the data a QRY_SPEC points to to here.
**
** Inputs:
**	spec		The QRY_SPEC whose data is to be saved.
**
**	tag		The tag to allocate any memory with.
**
** Outputs:
**	Returns:
**		A pointer to the saved data.
**		NULL if problems.
**
** History:
**	22-may-1987 (Joe)
**		Initial Version
*/
static PTR
qgsp_spec(spec, tag)
QRY_SPEC	*spec;
i2		tag;
{
    i4			type = spec->qs_type;
    DB_DATA_VALUE	*dbv;
    switch (type)
    {
      case QS_TEXT:
	return (PTR) FEtsalloc(tag, (char *)spec->qs_var);
      case QS_VALUE:
      case QS_QRYVAL:
	if ((dbv = (DB_DATA_VALUE *)FEreqmem(tag, sizeof(*dbv),
		FALSE, (STATUS *)NULL)) == NULL)
	{
	    return NULL;
	}
	STRUCT_ASSIGN_MACRO(*((DB_DATA_VALUE *)spec->qs_var), *dbv);
	if ((dbv->db_data = FEreqmem(tag, dbv->db_length, FALSE,
		(STATUS *)NULL)) == NULL)
	{
	    return NULL;
	}
	MEcopy(((DB_DATA_VALUE *)spec->qs_var)->db_data,
			dbv->db_length, dbv->db_data);
	return (PTR) dbv;
    }
    return NULL;
}
/*{
** Name:    qgsp_next() -	Get the next QRY_SPEC slot from QGSAVESPEC
**
** Description:
**	This function returns the next available slot for a QRY_SPEC in
**	a QGSAVESPEC.
**
** Inputs:
**	savespecs	The QGSAVESPEC to get the slot from.
**
**	tag		The tag to use for any allocation.
**
** Outputs:
**	Returns:
**		The next availabe QRY_SPEC.
**		NULL if problem allocating memory.
**		<function return values>
**
** History:
**	22-may-1987 (Joe)
**		Initial Version
*/
static QRY_SPEC *
qgsp_next (savespecs, tag)
QGSAVESPEC	*savespecs;
u_i4		tag;
{
    if (savespecs->qgsp_numcur >= QGSPSIZE)
    {
	/*
	** Allocate a new chunk of QRY_SPECs.
	*/
	if ((savespecs->qgsp_cur->qgsp_next = (QGSPCHUNK *)FEreqmem(tag,
		sizeof(*(savespecs->qgsp_cur)), FALSE, (STATUS *)NULL)) == NULL)
	{
	    return NULL;
	}
	savespecs->qgsp_cur = savespecs->qgsp_cur->qgsp_next;
	savespecs->qgsp_cur->qgsp_next = NULL;
	savespecs->qgsp_numcur = 0;
    }
    return (&(savespecs->qgsp_cur->qgsp_specs[(savespecs->qgsp_numcur)++]));
}
/*{
** Name:    qgsp_alloc() -	Allocate a QGSAVESPEC
**
** Description:
**	Allocates a QGSAVESPEC and its first chunk of QRYSPECs.
**
** Inputs:
**	tag	{u_i4}  The tag with which to allocate the QGSAVESPEC.
**
** Returns:
**	{QGSAVESPEC *}  The address of the allocated QGSAVESPEC;
**			NULL if a problem allocating.
**
** History:
**	22-may-1987 (Joe)
*/
static QGSAVESPEC *
qgsp_alloc (tag)
u_i4	tag;
{
    QGSAVESPEC	*qs;
    if ((qs = (QGSAVESPEC*)FEreqmem(tag, sizeof(*qs), FALSE, (STATUS*)NULL))
		== NULL ||
    	(qs->qgsp_cur = (QGSPCHUNK *)FEreqmem(tag, sizeof(*(qs->qgsp_cur)),
						FALSE, (STATUS *)NULL)) == NULL)
    {
	return NULL;
    }
    qs->qgsp_first = qs->qgsp_cur;
    qs->qgsp_cur->qgsp_next = NULL;
    qs->qgsp_numcur = 0;
    return qs;
}
/*{
** Name:	qg_sqmods	- Generate correct IIsqMods call
**
** Description:
**	We generate the correct call to IIsqMods based on the query mode.
**	There may be no call required.
**
** Inputs:
**	qmode	u_i1	query mode
**
** History:
**	5/92 (Mike S) Initial version
*/
static VOID
qg_sqmods(qmode)
u_i1	qmode;
{
# ifndef DBI
    i4  mods = 0;
    if (qmode & QM_EXECIMM)
	mods |= IImodDYNSEL;
    if (qmode & QM_SINGLE)
	mods |= IImodSINGLE;
    if (mods != 0)
	IIsqMods(mods);
# endif /* DBI */
}
/*{
** Name:	qg_flush_singleton - call IIsqFLush for a singleton select.
**
** Description:
**	After a singleton SELECT completes, we call IIsqFlush to clean up.
**	This, unfortunately, will gnenerate an error if the query actually
**	returned more than one row.  We've always allowed this in the 4GLs
**	before, so, we have to do the following silliness for compatibility:
**	
**	  Use IIseterr to trap the error.  If it occurs, reset LIBQ's
**	  idea of the last error to 0.  If an unexpected error occurs, pass
**	  it on to the previous error handler.
**
** History:
**	5/92 (Mike S) Initial version
*/
static VOID
qg_flush_singleton()
{
# ifndef DBI
    too_many_rows = FALSE;
    old_seterr_func = IIseterr(multi_rows_OK);
    IIsqFlush((char *)NULL, 0);
    _VOID_ IIseterr(old_seterr_func);
    if (too_many_rows)
/* # line 2049 "qg.sc" */	/* set_sql */
  {
    IILQssSetSqlio(2,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
    IILQssSetSqlio(0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
  }
/* # line 2050 "qg.sc" */	/* host code */
# endif /* DBI */
}
/*{
** Name:	multi_rows_OK - Allow multiple rows from a singleton SELECT
**
** Description:
**	We have always allowed a singleton SELECT in the 4GL to return
**	multiple row without an error.  This IIseterr function tests for
**	those errors; if it sees any other errors, it allows the previous
**	IIseterr function, if any, to process them.
**
** Inputs:
**	err	i4 *	Error number
**
**	Returns:
**		0	If we are rejecting this error
**		Result of previous error handler otherwise.
**
** History:
**	5/92 (Mike S) Initial version
**      02-jan-97 (mcgem01)
**          Various builds have been broken by the introduction of the
**          LOADDS compiler directive.  This modifier should have been
**          added for Windows 3.1 only.
*/
# ifdef WIN16
static i4 LOADDS
# else
static i4 
# endif
multi_rows_OK(err)
i4	*err;
{
    if (*err == GE_CARDINALITY || *err == E_LQ006A_SQMULTSEL)
    {
	too_many_rows = TRUE;
	return 0;
    }
    else
    {
	return old_seterr_func(err);
    }
}
/*
** Default stream management routines.  These are simply covers for
** SI routines.
*/
/*
** Open the file.
*/
static PTR
open_rtn(recsize)
i4  recsize;
{
    QG_FHANDLE *handle;
    LOCATION loc;
    handle = (QG_FHANDLE *)MEreqmem(0, sizeof(QG_FHANDLE), 
				    FALSE, (STATUS *)NULL);
    /*
    ** BUG 3778
    ** Call FEt_open_bin instead of NMt_open so that
    ** the file is a variable length no attribute file.
    */
    if (FEt_open_bin(ERx("w"), ERx("qg"), ERx(""), 
		     &loc, &(handle->qg_fptr)) != OK)
    {
	MEfree(handle);
	return NULL;
    }
    LOcopy(&loc, handle->qg_locbuf, &handle->qg_loc);
    handle->qg_recsize = (i4)recsize;
    return (PTR)handle;
}
/*
** Rewind the file back to its begining
*/
static STATUS
rewind_rtn(hd)
PTR hd;
{
    STATUS status = OK;
    QG_FHANDLE *handle = (QG_FHANDLE *)hd;;
    /*
    ** Bug 50215 (donc): Put an SIflush in case the filesystem is full
    ** and SIwrites went to the bit-bucket.  SIflush will return an error
    ** in this case which we want to notify the user of. This is put outside
    ** the qg_nexttuple loop because repeated SIflushes are superfluous and we
    ** just need to get the "final" status.
    */
    if( SIflush(handle->qg_fptr) != OK )
        status = FAIL;
    SIclose(handle->qg_fptr);
    handle->qg_fptr = NULL;
    if (status == OK)
    	status = SIfopen(&handle->qg_loc, ERx("r"), SI_VAR, 
			 0, &handle->qg_fptr) ;
    return status;
}
/*
** Write to the file
*/
static STATUS
write_rtn(hd, buffer)
PTR hd;
PTR buffer;
{
    i4 dummy;
    QG_FHANDLE *handle = (QG_FHANDLE *)hd;;
    return SIwrite(handle->qg_recsize, buffer, &dummy, handle->qg_fptr);
}
/*
** Read a record and return the number of bytes read
*/
static STATUS
read_rtn(hd, buffer, count)
PTR hd;
PTR buffer;
i4 *count;
{
    QG_FHANDLE *handle = (QG_FHANDLE *)hd;
    return SIread(handle->qg_fptr, handle->qg_recsize, count, buffer);
}
/*
** General cleanup.  Close and delete the file, and free the memory
** for the handle.
*/
static STATUS
close_rtn(hd)
PTR hd;
{
    QG_FHANDLE *handle = (QG_FHANDLE *)hd;;
    if (handle->qg_fptr)
	SIclose(handle->qg_fptr);
    LOdelete(&handle->qg_loc);
    MEfree(handle);
    return OK;
}