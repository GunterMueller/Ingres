# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
/*
**  Bug 3523 is a DBMS bug that is not going to be fixed in the DBMS for
**  the current release of the frontends.  The bug involves doing an SQL
**  PREPARE on a table immediately after having modified the structure
**  of the table.
**
**  The bug manifests itself in QBF if a user or DBA has modified a table
**  via the terminal monitor or some other program and then goes into QBF
**  and tries to do an append (INSERT) to the table.  QBF does a PREPARE and
**  EXECUTE to perform the INSERT.  
**
**  NOTE:  This is NOT a problem with retrieve or update mode of QBF.
**         Retrieve mode does no PREPAREs and update mode does a SELECT
**	   before the PREPARE and this sequence of events does not produce
**	   the DBMS error.
*/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlda.h"
# include   <compat.h>
# include   <st.h>		 
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include   <fe.h>
# include "mqbf.h"
# include   <cm.h>
# include   <er.h>
# include   <me.h>
# include   <ui.h>
# include   <uigdata.h>
# include   <afe.h>
# ifndef DGC_AOS
# define bug3523
# endif
# ifdef bug3523
# include <usererror.h>
# include <generr.h>
# endif
/**
**
**  Name: qfsql.qsc - all dynamic esql statements/routines used by qbf and
**		      QBF key checking routines.
**
**  Description:
**      This files defines:
** 
**	IIQFsda_sqlInsertDBDV -
**		    perform a dynamic SQL INSERT, using DB DATA VALUES,
**		    into a table referenced in the RELINFO structure
**
**	IIQFsqg_sqlCheckPrimary -
**		    determine if a given table has any primary keys associated
**		    with it by querying iicolumns.
**
**	IIQFsdh_sqlUpdateDBDV -
**		    perform a dynamic SQL UPDATE, using DB DATA VALUES,
**		    into a table referenced in the RELINFO structure.
**
**	IIQFsqi_sqlCheckAlternates -
**		    determine if a given table has any alternate keys by
**		    querying the tables iialternate_keys and iialt_columns.
**
**	IIQFsqj_sqlKeyCheck -
**		    First find the owner of a table and then determine what
**		    type of key to use.  The following order is used:
**		       1. Check for primary key.  2. Check for TID.
**		       3. Check for alternates.   4. Check for whole row.
**
**	IIQFsqk_sqlCheck Duplicates -
**		    Determine if duplicate rows are allowed within a table by
**		    querying iitables.  If duplicate rows are not
**		    allowed, the row may be used as a key.
**
**	IIQFsdl_sqlDeleteDBDV -
**		    perform a dynamic SQL DELETE, using DB DATA VALUES,
**		    from a table referenced in the RELINFO structure.
**
**	IIQFsdm_sqlModifyWhere -
**		    examine the WHERE clause for a dynamic DELETE or UPDATE
**		    statement and replace NULL DB DATA VALUES with the
**		    phrase "IS NULL".  Then PREPARE and EXECUTE the statement.
**
**	mqsetsqlqry -
**		    Allocate space for the SQLDA and the string needed to
**		    PREPARE the dynamic query.
**
**	mqinitsqlda -
**		    Initialize the SQLDA with the appropriate DB DATA VALUES.
**
**	mq_unrslv -
**		    Generate a fully unnormalized owner.tablename construct.
**
**
**  History:
**      9/30/87 (kenl)
**          Initial version.
**	22-apr-88 (kenl)
**		Replaced ^= with <>.
**
**	31-may-88 (kenl)
**		Updated comments and added #define for keywords in dynamic
**		query string building.
**	19-aug-88 (marian)
**		Changed select statement to reflect column name changes in
**		standard catalogs.
**	23-nov-88 (kenl)
**		ham
**	15-jan-90 (kenl)
**		Added error number E_GE80E8_LOGICAL_ERROR when checking 
**		for bug #3523.  Previously, only E_US1265_QEP_INVALID was
**		being checked but now that our DBMS returns generic errors,
**		both values need to be checked.
**	19-feb-92 (leighb) DeskTop Porting Change:
**		IIUGbmaBadMemoryAllocation() was missing its argument.
**	6-apr-92 (fraser)
**		Changed arguments to IIUGbmaBadMemoryAllocation per
**		Emerson's suggestion.
**	08-jun-92 (leighb) DeskTop Porting Change:
**		Changed 'errno' to 'errnum' cuz 'errno' is a macro in WIN/NT
**	18-jan-93 (daver)
**		Called IIUGxri_id() on the table names for each of the
**		APPEND, UPDATE, and DELETE statements being built. This wacky
**		UG routine will quote the table name if it is a delimited
**		identifier.
**	11-aug-1993 (rdrane)
**		For 6.5 connections, include the owner as required for
**		APPEND, UPDATE, and DELETE operations.  This addresses
**		bug 51773, and replaces the above modification involving
**		IIUGxri_id().  The routine to effect construction of the
**		owner.table construct is also used by other QBF routines,
**		and so is not static to this module.
**	31-dec-1993 (rdrane)
**		Use OpenSQL level to determine 6.5 syntax features support.
**	28-feb-1994 (rdrane)
**		Correct isdba reference in mq_rslv().  We were omitting
**		any owner if the invoker was the DBA (b60152).  Also, note
**		the owner may be passed as an empty string, and there's no
**		need to call the full tablename construction routine in that
**		case.
**      05-Mar-1995 (liibi01)
**              Bug 66242, if the column is not nullable and user specifies 
**              the default value for column, the default value won't show
**              up if user doesn't enter any value for the corresponding
**              column in QBF. Instead, Ingres default is given.  
**	07-Apr-1995 (liibi01)
**		Modified change for bug 66242. Removed C function calls and
**		replaced them with CL function calls.
**	27-jun-95 (teresak)
**		Added undocumented support for II_QBF_DISABLE_VIEW_CHECK
**		to allow append, update, and deletes of single table views 
**		(SIR 37839)
**	15-Oct-1997(angusm)
**		Redo allow_views as bool (bug 86168)
**	02-dec-1998 (kitch01)
**		Correct IIQFsql_check_default() to correctly convert extracted
**		default to the columns datatype. 
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      02-Jan-2002 (horda03)
**              Specify the table owner when connected to an installation that
**              supports the use of owner in commands. (106812)
*/
/*
**  The following define is used when allocating space for a dynamic query
**  string that needs to be PREPAREd.  The majority of the string is build
**  from existing strings.  However, certain words must be interspersed with
**  these strings:
**
**	For INSERTs:  'INSERT INTO '
**	For UPDATEs:  'UPDATE ', ' SET ', ' WHERE '
**	For DELETEs:  'DELETE FROM ', ' WHERE '
**
**  This define must be set to cover the largest possible case.
*/
#define	LEN_SQL_KEYWORDS	30
/*
**  The following define is used when the WHERE clause of a dynamic SQL
**  statement is modified by replacing '=?' with ' WITH NULL'.  This has
**  to be done because we cannot pass a WHERE clause containing a NULL DB
**  DATA VALUE to the backend.  The length of the string containing the
**  dynamic query must be increased by the appropriate length.
*/
#define	LEN_NULL_PHRASE		8
/*
**  Forward and/or External function references.
*/
FUNC_EXTERN VOID    mqinitsqlda();
FUNC_EXTERN VOID    mqsetsqlqry();
FUNC_EXTERN VOID    mq_unrslv();
FUNC_EXTERN ADF_CB  *FEadfcb();
static i4  Isnull( DB_DATA_VALUE *dbdv );
VOID IIQFsql_check_default( RELINFO *rinfo, char *tbl_name, char *tbl_owner);
/*
**  One or more routines contained within this module use dynamic
**  SQL statements that use the SQLDA.  The size of the SQLDA must
**  vary depending upon the table(s) being used.  Also, the 'statement'
**  part of a dynamic query must also vary depending upon the number of
**  columns being accessed.  Therefore, the SQLDA (lsqlda) and the
**  dynamic query string (sqlqry) will be declared static to this module
**  and allocated at runtime.  The space allocated for these areas
**  will grow as required, but they will never shrink.  By managing
**  the memory in this fashion, if sufficient space has already been
**  allocated, the routine will not have to do a call to MEreqmem for
**  every dynamic SQL statement.
*/
static	IISQLDA	    *lsqlda = NULL;	  /* sqlda */
/* number of variables allocated for the sqlda */
static	i4		sqldalen = 0;
  static char *sqlqry = NULL;
/* length of memory allocated for dynamic query */
static	i4		qrylen = 0;
/*
** Name: IIQFsda_sqlInsertDBDV -
**		    Perform an ESQL INSERT, using DB DATA VALUES, into the
**		    table specified in the structure RELINFO.
**
** Description:
**      This routine is called from the QBF modules 'append.qc' and
**	'wrtupd.qc'.  This ESQL routine replaces a parameterized EQUEL
**	statement that uses DB DATA VALUES.  The routine is called
**	from either the QBF modes APPEND or UPDATE when an append is
**	required to the table.  The original APPEND statement appears
**	in the above two modules as follows:
**		APPEND tbl (param(onerel->mqaparam,onerel->mqaargv))
**
** Inputs:
**	rinfo -	    a pointer to a structure of type RELINFO.  This
**		    structure contains three pieces of information that
**		    are required by this routine.
**
**		    mqrelid - points to the table to INSERT into.
**		    mqaparam - append parameter string.
**		    mqaargvs - pointer to array of DB DATA VALUES.
**
** Outputs:
**	None.  Input parameters are untouched.
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
**      9/30/87 (kenl)
**          Initial version.
**      05-Mar-1995 (liibi01)
**              Bug 66242, if the column is not nullable and user specifies 
**              the default value for column, the default value won't show
**              up if user doesn't enter any value for the corresponding
**              column in QBF. Instead, Ingres default is given.  
**  02-may-2000 (fanch01)
**      Bug 101416.  'WITH DEFAULT' user values not being queried
**      correctly when the user, table owner and database owner are
**      different.  Pass IIQFsql_check_default() table name instead
**      of owner.tablename to correct query.
*/
VOID
IIQFsda_sqlInsertDBDV(rinfo)
RELINFO	*rinfo;
{
# ifdef bug3523
  i4 errnum;
# endif
    i4	    i;
    i4	    numdbdv;
    char    unrml_buf[FE_MAXTABNAME]; /* unormalized ids */
    /* calculate the number of DB DATA VALUES in the list */
    for (numdbdv = 0; rinfo->mqaargv[numdbdv] != NULL; numdbdv++);
    mq_unrslv(rinfo->mqrelown,rinfo->mqrelid,&unrml_buf[0]);
    IIQFsql_check_default( rinfo, rinfo->mqrelid, rinfo->mqrelown );
    i = LEN_SQL_KEYWORDS + STlength(unrml_buf) +
   	 STlength(rinfo->mqaparam);
    mqsetsqlqry(numdbdv, i);
    mqinitsqlda(numdbdv, rinfo->mqaargv);
    sqlqry[0] = '\0';
    STpolycat(3, ERx("INSERT INTO "), unrml_buf, rinfo->mqaparam, sqlqry);
# ifdef bug3523
    errnum = 0;
    do
    {
# endif
/* # line 297 "qfsql.sc" */	/* prepare */
  {
    IIsqInit((char *)0);
    IIsqPrepare(0,(char *)"instmt",(char *)0,0,sqlqry);
  }
/* # line 298 "qfsql.sc" */	/* host code */
        if (FEinqerr() == OK)
        {
# ifdef bug3523
/* # line 301 "qfsql.sc" */	/* set_ingres */
  {
    IILQssSetSqlio(41,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
  }
/* # line 302 "qfsql.sc" */	/* host code */
# endif
/* # line 303 "qfsql.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExStmt((char *)"instmt",0);
    IIsqDaIn(0,lsqlda);
    IIsyncup((char *)0,0);
  }
/* # line 304 "qfsql.sc" */	/* host code */
# ifdef bug3523
/* # line 305 "qfsql.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&errnum,2);
  }
/* # line 306 "qfsql.sc" */	/* host code */
	    if ((errnum != E_US1265_QEP_INVALID)
			&&  (errnum != E_GE80E8_LOGICAL_ERROR))
	    {
/* # line 309 "qfsql.sc" */	/* set_ingres */
  {
    IILQssSetSqlio(40,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
  }
/* # line 310 "qfsql.sc" */	/* host code */
	    }
/* # line 311 "qfsql.sc" */	/* set_ingres */
  {
    IILQssSetSqlio(41,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
  }
/* # line 312 "qfsql.sc" */	/* host code */
# endif
        }
# ifdef bug3523
    } while ( (errnum == E_US1265_QEP_INVALID)
			|| (errnum == E_GE80E8_LOGICAL_ERROR) );
# endif
}
i4
IIQFsqg_sqlCheckPrimary(tbl, tbl_owner, tbl_keys)
  char *tbl;
  char *tbl_owner;
char	tbl_keys[][FE_MAXNAME + 1];
{
    i4	    kcount;
  char col_name[FE_MAXNAME + 1];
    kcount = 0;
/* # line 332 "qfsql.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qfsql1",7167,9499);
      IIputdomio((short *)0,1,32,0,tbl_owner);
      IIputdomio((short *)0,1,32,0,tbl);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qfsql1",7167,9499);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select column_name from iicolumns where table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,tbl_owner);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,tbl);
        IIwritio(0,(short *)0,1,32,0,(char *)"and key_sequence<>0");
        IIexDefine(0,(char *)"qfsql1",7167,9499);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,col_name);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 338 "qfsql.sc" */	/* host code */
	tbl_keys[kcount][0] = '\0';
	STtrmwhite(col_name);
	STcopy(col_name, tbl_keys[kcount++]);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
    if (kcount != 0)
    {
	tbl_keys[kcount][0] = '\0';
    }
    return(kcount);
}
/*
** Name: IIQFsql_check_default
**
** Description:
**	If the user did not specify a value for a column to be appended
**	to the table, extract default column values from the catalog.
**
** Inputs:
**	rinfo -	    a pointer to a structure of type RELINFO.  This
**		    structure contains three pieces of information that
**		    are required by this routine.
**
**		    mqrelid - points to the table to INSERT into.
**		    mqaparam - append parameter string.
**		    mqaargvs - pointer to array of DB DATA VALUES.
**
**	tbl_name - pointer to a string containing the table name
**
**	tbl_owner - pointer to a string containing the table owner
**
** Outputs:
**	None.  rinfo->mqaargvs is modified to insert default column values.
**
**	Returns:
**
**	Exceptions:
**
** Side Effects:
**	None.
**
** History:
**      05-Mar-1995 (liibi01)
**		Initial version.
**              Bug 66242, if the column is not nullable and user specifies 
**              the default value for column, the default value won't show
**              up if user doesn't enter any value for the corresponding
**              column in QBF. Instead, Ingres default is given.  
**	06-jun-95 (rudtr01)
**		avoid checking for default column values in catalogs
**		that do not support them, e.g. gateways and Star.
**	28-dec-95 (prida01)
**		Before checking whether the column is nullable or not
**		first check it is not a system maintained object_key
**		fixes bug 73325.
**  02-dec-98 (kitch01)
**		Bug 79618. 'WITH DEFAULT' clause value was not being handled
**		correctly. Check the value and if it should be NULL set the
**		rinfo->mqaargvs accordingly. If it should be a value then
**		convert the extracted value to the correct datatype for the
**		column.
**		Also removed some unrefenced local variables.
**	07-sep-06 (gupsh01)
**	    Added support for new ANSI date/time types.
*/
VOID 
IIQFsql_check_default( rinfo, tbl_name, tbl_owner )
  char *tbl_name;
  char *tbl_owner;
RELINFO *rinfo;
{
    i4    kcount;
    i4    len;
    ADF_CB *adf_cb;
    DB_DATA_VALUE idbv;
	bool  nullable;
  i4 count;
  char default_col_val[1501];
    if (STcompare(IIUIscl_StdCatLevel(),UI_LEVEL_65) < 0)
	return;
    kcount = 0;
    adf_cb = FEadfcb();
/* # line 423 "qfsql.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qfsql2",7168,12154);
      IIputdomio((short *)0,1,32,0,tbl_name);
      IIputdomio((short *)0,1,32,0,tbl_owner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qfsql2",7168,12154);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select distinct column_sequence, column_default_val from iicolumns wh\
ere column_defaults='Y' and table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,tbl_name);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,tbl_owner);
        IIexDefine(0,(char *)"qfsql2",7168,12154);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&count);
      IIgetdomio((short *)0,1,32,1500,default_col_val);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 429 "qfsql.sc" */	/* host code */
    /* 
    ** If user defined defaults exist, then kcount should be greater than
    ** zero.
    */  
    kcount = count;
    if ( kcount > 0 ) 
	{ 
       /* Check this is not a system maintained object key first
       ** for bug 73325
       */
       if ((rinfo->mqaargv[kcount-1]) && ( Isnull(rinfo->mqaargv[kcount-1]) ))
       {  
          if ( !STcompare( default_col_val, "CURRENT_USER") ||
               !STcompare(default_col_val, "SESSION_USER")  ||
               !STcompare(default_col_val, "INITIAL_USER") ) 
          {
             STcopy(IIUIdbdata()->suser, default_col_val); 
          }
          else 
             if ( !STcompare( default_col_val, "SYSTEM_USER"))
             {
                STcopy(IIUIdbdata()->dba, default_col_val);
             }
          if ( STcompare(default_col_val, "NULL"))
          {
             /* If default value is a string remove quotes. These are
			 ** reapplied later during query execution
			 */
             if  ( default_col_val[0] == '\'' )
             {
                STcopy(default_col_val + 1, default_col_val);
                len = STlength(default_col_val) - 1;
                default_col_val[len] = '\0';
                idbv.db_datatype = DB_CHA_TYPE;
             }
             else
                len = STtrmwhite(default_col_val);
             idbv.db_data = &default_col_val[0];
             idbv.db_datatype = DB_CHA_TYPE;
             idbv.db_length = len;
          }
          else
          {
             /* Bug 79618. Default value is NULL set idbv accordingly */
             len = STtrmwhite(default_col_val);
             default_col_val[0] = '\0';
             idbv.db_data = &default_col_val[0];
             idbv.db_datatype = DB_CHA_TYPE;
             idbv.db_length = len;
             AFE_MKNULL( &idbv );
             AFE_SETNULL( &idbv );
          }
          /* If afe_cvinto fails do the conversion the hard way */
          if ( afe_cvinto( adf_cb, &idbv, rinfo->mqaargv[kcount - 1]) != OK );
          {
             /* See if the output is nullable and temporarily make
             ** it not nullable
             */
             nullable = AFE_NULLABLE(rinfo->mqaargv[kcount-1]->db_datatype);
             if (nullable)
                AFE_UNNULL(rinfo->mqaargv[kcount-1]);
             switch (AFE_DATATYPE(rinfo->mqaargv[kcount-1]))
             {
                /* -- integer type -- */ 
                case DB_INT_TYPE:
                   adu_1int_coerce( adf_cb, &idbv, rinfo->mqaargv[kcount-1]);
                   break;
                /* -- floating point type -- */
                case DB_FLT_TYPE:
                   adu_1flt_coerce( adf_cb, &idbv, rinfo->mqaargv[kcount-1]);
                   break; 
                /* -- money type -- */
                case DB_MNY_TYPE:
                   adu_2strtomny( adf_cb, &idbv, rinfo->mqaargv[kcount-1]);  
                   break;    
                /* -- decimal type -- */
                case DB_DEC_TYPE:
                   adu_1dec_coerce( adf_cb, &idbv, rinfo->mqaargv[kcount-1]);
                   break;
                /* -- date type -- */
                case DB_DTE_TYPE:
                   adu_14strtodate( adf_cb, &idbv, rinfo->mqaargv[kcount-1]);
                   break;
                case DB_ADTE_TYPE:
                case DB_TMWO_TYPE:
                case DB_TMW_TYPE:
                case DB_TME_TYPE:
                case DB_TSWO_TYPE:
                case DB_TSW_TYPE:
                case DB_TSTMP_TYPE:
                case DB_INDS_TYPE:
                case DB_INYM_TYPE:
                   adu_21ansi_strtodt( adf_cb, &idbv, rinfo->mqaargv[kcount-1]);
                   break;
                /* -- varchar type -- */
                case DB_TXT_TYPE:
                case DB_VCH_TYPE:
                /* -- char/c/ type -- */
                case DB_CHR_TYPE:
                case DB_CHA_TYPE:
                   adu_1strtostr( adf_cb, &idbv, rinfo->mqaargv[kcount-1]);
                   break;
                default:
                   break;
             }
			 /* Reset the output to nullable if it was originally */
             if (nullable)
                AFE_SET_NULLABLE(nullable, rinfo->mqaargv[kcount-1]);
          }
       }
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
}
/*
** Name: Isnull
**
** Description:
**	Check to see if the user keyed data into a column to be appended
**	to the table.
**
** Inputs:
**	string - a pointer to the string containing the DBDV data.
**	len    - length of data.
**	type   - type of data.
**
** Outputs:
**
**	Returns:
**	    TRUE - if user entered nothing
**	    FALSE - if user entered something
**
**	Exceptions:
**
** Side Effects:
**	None.
**
** History:
**      05-Mar-1995 (liibi01)
**		Initial version.
**              Bug 66242, if the column is not nullable and user specifies 
**              the default value for column, the default value won't show
**              up if user doesn't enter any value for the corresponding
**              column in QBF. Instead, Ingres default is given.  
**	27-Jun-1995 (rudtr01)
**		cleaned up. conform to standards.
*/
static
i4  
Isnull( DB_DATA_VALUE *dbdv_p )
{
    i4  i, len;
    DB_DATA_VALUE ldbv;
    /*
    ** Make a local copy of the DB_DATA_VALUE structure for this column
    ** so that we don't mess up the original.
    */
    STRUCT_ASSIGN_MACRO( *dbdv_p, ldbv );
    /*
    ** This test will suffice for columns that are defined as NULLable
    ** (e.g. not NOT NULL).  If the column is NULL, return TRUE.
    */
    if ( AFE_ISNULL( &ldbv ) )
	return TRUE;
    /*
    ** The column may be defined as NOT NULL, and the user may not have 
    ** entered any data.  So, let's alter our local copy of the DB_DATA_VALUE
    ** structure to deactivate the NULLable flags, and then test the hard
    ** way.
    */
    AFE_UNNULL( &ldbv );
    if ( ldbv.db_datatype == DB_VCH_TYPE || ldbv.db_datatype == DB_TXT_TYPE )
 	ldbv.db_length = 2;
    else
    if  ( ldbv.db_datatype == DB_DEC_TYPE )
	ldbv.db_length -= 1;
    else
    if ( ldbv.db_datatype == DB_CHA_TYPE || ldbv.db_datatype == DB_CHR_TYPE )
    {
	for ( i = 0; i < ldbv.db_length; i++ )
	    if ( ldbv.db_data[i] != '\0' && ldbv.db_data[i] != ' ' )
		return FALSE;
	return TRUE;
    }
    for ( i = 0; i <ldbv.db_length; i++ )
	if ( ldbv.db_data[i] != '\0' )
	    return FALSE;
    return TRUE;
}
VOID
IIQFsdh_sqlUpdateDBDV(rinfo)
RELINFO	*rinfo;
{
    i4	    i;
    i4	    numdbdv;
    ADF_CB  *adf_cb;
    i4	    isnull;
    i4	    nullcnt;
    i4	    wherecnt;
    char    unrml_buf[FE_MAXTABNAME]; /* unormalized ids */
    adf_cb = FEadfcb();
    /* calculate the number of DB DATA VALUES in the list */
    for (numdbdv = 0; rinfo->mquargv[numdbdv] != NULL; numdbdv++);
    for (nullcnt = 0, wherecnt= 0; rinfo->mqdargv[wherecnt]!= NULL; wherecnt++)
    {
	_VOID_ adc_isnull(adf_cb, rinfo->mqdargv[wherecnt], &isnull);
	if (isnull)
	{
	    nullcnt++;
	}
    }
    mq_unrslv(rinfo->mqrelown,rinfo->mqrelid,&unrml_buf[0]);
    i = LEN_SQL_KEYWORDS + STlength(unrml_buf) +
	STlength(rinfo->mquparam) + STlength(rinfo->mqdparam) +
	(nullcnt * LEN_NULL_PHRASE);
    mqsetsqlqry(numdbdv - nullcnt, i);
    sqlqry[0] = '\0';
    STpolycat(5, ERx("UPDATE "), unrml_buf, ERx(" SET "), rinfo->mquparam,
	    " WHERE ", sqlqry);
    if (nullcnt == 0)
    {
	STcat(sqlqry, rinfo->mqdparam);
	mqinitsqlda(numdbdv, rinfo->mquargv);
/* # line 651 "qfsql.sc" */	/* prepare */
  {
    IIsqInit((char *)0);
    IIsqPrepare(0,(char *)"updstmt",(char *)0,0,sqlqry);
  }
/* # line 652 "qfsql.sc" */	/* host code */
	if (FEinqerr() == OK)
	{
/* # line 654 "qfsql.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExStmt((char *)"updstmt",0);
    IIsqDaIn(0,lsqlda);
    IIsyncup((char *)0,0);
  }
/* # line 655 "qfsql.sc" */	/* host code */
	}
    }
    else
    {
	mqinitsqlda(numdbdv - wherecnt, rinfo->mquargv);
	IIQFsdm_sqlModifyWhere(rinfo, numdbdv - wherecnt);
    }
}
i4
IIQFsqi_sqlCheckAlternates(tbl, tbl_owner, tbl_keys)
  char *tbl;
  char *tbl_owner;
char	tbl_keys[][FE_MAXNAME + 1];
{
    i4	    kcount;
    i4	    first_id;
  char col_name[FE_MAXNAME + 1];
  i4 altid;
    kcount = 0;
/* # line 678 "qfsql.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select key_id, column_name from iialt_columns where table_owner=");
    IIputdomio((short *)0,1,32,0,tbl_owner);
    IIwritio(0,(short *)0,1,32,0,(char *)"and table_name=");
    IIputdomio((short *)0,1,32,0,tbl);
    IIwritio(0,(short *)0,1,32,0,(char *)"order by key_id");
    IIretinit((char *)0,0);
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&altid);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,col_name);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 684 "qfsql.sc" */	/* host code */
	if (kcount == 0)
	{
	    first_id = altid;
	}
	if (first_id != altid)
	{
/* # line 690 "qfsql.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE3;
      }
/* # line 691 "qfsql.sc" */	/* host code */
	}
        tbl_keys[kcount][0] = '\0';
        STtrmwhite(col_name);
        STcopy(col_name, tbl_keys[kcount++]);
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
    if (kcount != 0)
    {
        tbl_keys[kcount][0] = '\0';
    }
    return(kcount);
}
i4
IIQFsqj_sqlKeyCheck(tbl, tbl_owner, tbl_keys)
  char *tbl;
  char *tbl_owner;
char	tbl_keys[][FE_MAXNAME + 1];
{
    i4	    kcount;
    i4      tbl_is_view = FALSE;
  char table_type[2];
    table_type[0] = '\0';
    if (allow_views)
        {
/* # line 718 "qfsql.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select table_type from iitables where table_owner=");
    IIputdomio((short *)0,1,32,0,tbl_owner);
    IIwritio(0,(short *)0,1,32,0,(char *)"and table_name=");
    IIputdomio((short *)0,1,32,0,tbl);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,1,table_type);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 722 "qfsql.sc" */	/* host code */
           if (STcompare(table_type,"T") != 0)
           {
                tbl_is_view = TRUE;
           }
        }
    if ( !tbl_is_view && IIUIdml() != UI_DML_GTWSQL)
    {
        kcount = mqUSETID;
    }
    else
    {
        if ((kcount = IIQFsqi_sqlCheckAlternates(tbl, tbl_owner,
		tbl_keys)) == 0)
        {
	    kcount = IIQFsqk_sqlCheckDuplicates(tbl, tbl_owner, tbl_keys);
	}
    }
    return(kcount);
}
i4
IIQFsqk_sqlCheckDuplicates(tbl, tbl_owner, tbl_keys)
  char *tbl;
  char *tbl_owner;
char	tbl_keys[][FE_MAXNAME + 1];
{
    i4	    kcount;
    i4	    kfound;
  char col_name[FE_MAXNAME + 1];
  char dup_rows[9];
  i4 row_count;
    kcount = 0;
    kfound = FALSE;
/* # line 758 "qfsql.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qfsql3",7169,1914);
      IIputdomio((short *)0,1,32,0,tbl_owner);
      IIputdomio((short *)0,1,32,0,tbl);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"qfsql3",7169,1914);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select duplicate_rows from iitables where table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,tbl_owner);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,tbl);
        IIexDefine(0,(char *)"qfsql3",7169,1914);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,8,dup_rows);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 762 "qfsql.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&row_count,8);
  }
/* # line 763 "qfsql.sc" */	/* host code */
    if (row_count == 1)
    {
	if (dup_rows[0] == 'U')
	{
	    kcount = mqUSEROW;
	}
    }
    return(kcount);
}
VOID
IIQFsdl_sqlDeleteDBDV(rinfo)
RELINFO	*rinfo;
{
    i4	    i;
    i4	    numdbdv;
    ADF_CB  *adf_cb;
    i4	    isnull;
    i4	    nullcnt;
    char    unrml_buf[FE_MAXTABNAME]; /* unormalized ids */
    adf_cb = FEadfcb();
    /* calculate the number of DB DATA VALUES in the list */
    for (nullcnt = 0, numdbdv = 0; rinfo->mqdargv[numdbdv] != NULL; numdbdv++)
    {
	_VOID_ adc_isnull(adf_cb, rinfo->mqdargv[numdbdv], &isnull);
	if (isnull)
	{
	    nullcnt++;
	}
    }
    mq_unrslv(rinfo->mqrelown,rinfo->mqrelid,&unrml_buf[0]);
    i = LEN_SQL_KEYWORDS + STlength(unrml_buf) +
	STlength(rinfo->mqdparam) + (nullcnt * LEN_NULL_PHRASE);
    mqsetsqlqry(numdbdv - nullcnt, i);
    sqlqry[0] = '\0';
    STpolycat(3, ERx("DELETE FROM "), unrml_buf, ERx(" WHERE "),
	    sqlqry);
    if (nullcnt == 0)
    {
	STcat(sqlqry, rinfo->mqdparam);
	mqinitsqlda(numdbdv, rinfo->mqdargv);
/* # line 803 "qfsql.sc" */	/* prepare */
  {
    IIsqInit((char *)0);
    IIsqPrepare(0,(char *)"delstmt",(char *)0,0,sqlqry);
  }
/* # line 804 "qfsql.sc" */	/* host code */
	if (FEinqerr() == OK)
	{
/* # line 806 "qfsql.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExStmt((char *)"delstmt",0);
    IIsqDaIn(0,lsqlda);
    IIsyncup((char *)0,0);
  }
/* # line 807 "qfsql.sc" */	/* host code */
	}
    }
    else
    {
	IIQFsdm_sqlModifyWhere(rinfo, (i4)0);
    }
}
IIQFsdm_sqlModifyWhere(rinfo, sqlda_offset)
RELINFO	*rinfo;
i4	sqlda_offset;
{
    ADF_CB  *adf_cb;
    i4	    isnull;
    char    *str;
    char    *qstr;
    char    *qcomma;
    i4	    varcnt;
    i4	    i;
    adf_cb = FEadfcb();
    str = rinfo->mqdparam;
    qcomma = str;
    varcnt = 0;
    for (i = 0; rinfo->mqdargv[i] != NULL; i++)
    {
	qstr = sqlqry + STlength(sqlqry);
	while (CMcmpcase(str, ERx("=")) != 0)
	{
	    CMcpyinc(str, qstr);
	}
	*qstr = EOS;
	_VOID_ adc_isnull(adf_cb, rinfo->mqdargv[i], &isnull);
	if (isnull)
	{
	    STcat(sqlqry, ERx(" IS NULL"));
	}
	else
	{
	    STcat(sqlqry, ERx("=?"));
	    lsqlda->sqlvar[sqlda_offset + varcnt].sqltype = DB_DBV_TYPE;
	    lsqlda->sqlvar[sqlda_offset + varcnt].sqllen
		    = sizeof(DB_DATA_VALUE);
	    lsqlda->sqlvar[sqlda_offset + varcnt].sqldata = 
		    (char *)rinfo->mqdargv[i];
	    varcnt++;
	}
	while ((CMcmpnocase(str, ERx("D")) != 0) && (*str != NULLCHAR))
	{
	    CMnext(str);
	}
	if (*str != NULLCHAR)
	{
	    CMnext(str);
	    CMnext(str);
	}
	qcomma = sqlqry + STlength(sqlqry);
	STcat(sqlqry, ERx(" AND "));
    }
    *qcomma = EOS;
    /* insert into the SQLDA the number of 'sqlvar' variables being used */
    lsqlda->sqln = sqlda_offset + varcnt;
    lsqlda->sqld = sqlda_offset + varcnt;
/* # line 868 "qfsql.sc" */	/* prepare */
  {
    IIsqInit((char *)0);
    IIsqPrepare(0,(char *)"mdelstmt",(char *)0,0,sqlqry);
  }
/* # line 869 "qfsql.sc" */	/* host code */
    if (FEinqerr() == OK)
    {
	if (sqlda_offset + varcnt == 0)
	{
/* # line 873 "qfsql.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExStmt((char *)"mdelstmt",0);
    IIsyncup((char *)0,0);
  }
/* # line 874 "qfsql.sc" */	/* host code */
	}
	else
	{
/* # line 877 "qfsql.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExStmt((char *)"mdelstmt",0);
    IIsqDaIn(0,lsqlda);
    IIsyncup((char *)0,0);
  }
/* # line 878 "qfsql.sc" */	/* host code */
	}
    }
}
VOID
mqsetsqlqry(dbdvcount, strlen)
i4	dbdvcount;
i4	strlen;
{
    /* determine if more memory needs to be allocated for this query */
    if ((dbdvcount > sqldalen) || ((dbdvcount == 0) && (sqldalen == 0)))
    {
	/* always allocate at least one variable */
	if (dbdvcount == 0)
	{
	    dbdvcount++;
	}
	if (sqldalen != 0)  MEfree(lsqlda);
	sqldalen = dbdvcount;
        if ((lsqlda = (IISQLDA *)MEreqmem((u_i4)0,
		(u_i4)(IISQDA_HEAD_SIZE + (dbdvcount * IISQDA_VAR_SIZE)),
		TRUE, (STATUS *)NULL)) == NULL)
        {
	    IIUGbmaBadMemoryAllocation(ERx("qfsql.qsc"));  
        }
    }
    /* determine if more memory is required for the query string */
    if (strlen > qrylen)
    {
	if (qrylen != 0)  MEfree(sqlqry);
	qrylen = strlen;
        if ((sqlqry = (char *)MEreqmem((u_i4)0,
		(u_i4)qrylen, TRUE, (STATUS *)NULL)) == NULL)
        {
	    IIUGbmaBadMemoryAllocation(ERx("qfsql.qsc"));  
        }
    }
}
VOID
mqinitsqlda(numdbdv, dbvals)
i4		numdbdv;
DB_DATA_VALUE	**dbvals;
{
    i4	    i;
    /* insert into the SQLDA the number of 'sqlvar' variables being used */
    lsqlda->sqln = numdbdv;
    lsqlda->sqld = numdbdv;
    /* set the sqlvars to the values indicated by the DB DATA VALUEs */
    for (i = 0; i < numdbdv; i++)
    {
	lsqlda->sqlvar[i].sqltype = DB_DBV_TYPE;
	lsqlda->sqlvar[i].sqllen = sizeof(DB_DATA_VALUE);
	lsqlda->sqlvar[i].sqldata = (char *)dbvals[i];
    }
}
VOID
mq_unrslv(own,tbl,buf)
char	*own;
char	*tbl;
char	*buf;
{
    /* Old method didn't include owner if the table owner was the DBA or
    ** the catalog owner; but this is too restrictive, as any user may
    ** create a table with the same name as tables created by the DBA or
    ** the catalog owner. If this is the case, and the user tries to
    ** update the table not owned by it, then the wrong table will be
    ** updated.
    */
    /*
    ** If the table owner is blank or the session user
    ** then we need only be concerned with ensuring that the
    ** table name is properly unnormalized in case it is a delimited id.
    ** Otherwise, we need to generate a fully unnormalized owner.tablename
    ** construct so that we're sure to access the correct table.
    ** And if we're not at least 6.5, none of the above applies ...
    */
    if  ((*own == EOS) ||
	 (STcompare(IIUIcsl_CommonSQLLevel(),UI_LEVEL_65) < 0) ||
         (STequal(own,IIUIdbdata()->suser)))
    {
    	IIUGxri_id(tbl,buf);
    }
    else
    {
	IIUIxrt_tbl(own,tbl,buf);
    }
    return;
}
