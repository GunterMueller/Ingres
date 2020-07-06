#define	YYFUNCTIONS
#define	YY_1HANDLER	psl_1handle
#define	YY_2HANDLER	psl_2handle
#define	YYREENTER
#define	YYPRINTF	TRdisplay

/* # line 7 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <ci.h>
#include    <cv.h>
#include    <mh.h>
#include    <qu.h>
#include    <st.h>
#include    <tr.h>
#include    <tm.h>
#include    <tmtz.h>
#include    <cm.h>
#include    <er.h>
#include    <me.h>
#include    <bt.h>
#include    <pc.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <gca.h>
#include    <ddb.h>
#include    <dmf.h>
#include    <dmacb.h>
#include    <dmccb.h>
#include    <dmtcb.h>
#include    <dmrcb.h>
#include    <dmucb.h>
#include    <adf.h>
#include    <ade.h>
#include    <adfops.h>
#include    <ulf.h>
#include    <ulm.h>
#include    <qsf.h>
#include    <qefrcb.h>
#include    <rdf.h>
#include    <sxf.h>
#include    <copy.h>
#include    <qefmain.h>
#include    <qefqeu.h>
#include    <qeuqcb.h>
#include    <qefcopy.h>
#include    <qefcb.h>
#include    <opfcb.h>
#include    <scf.h>
#include    <ex.h>
#include    <cui.h>
#include    <psfparse.h>
#include    <psfindep.h>
#include    <pshparse.h>
#include    "pslgram.h"
#include    "pslscan.h"
#include    <yacc.h>
#include    <usererror.h>
#include    <dudbms.h>
#include    <psftrmwh.h>
#include    <psldef.h>
#include    <psqcvtdw.h>
#include    <psqmonth.h>
#include    <psttprpnd.h>
#include    <pstwindup.h>
#include    <psyaudit.h>

/* 
** NO_OPTIM=dgi_us5 int_lnx int_rpl ibm_lnx i64_aix
*/

/* look in yyvars.h in local directory for local variables.*/
#define YYDEBUG 1
#define YACC_VARS 1
#define MAX_CREATE_CHARS 3
#define MAX_INDEX_CHARS 12

/* # line 735 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
/*[@forward_type_references@]*/

/*
**  Forward and/or External function references.
*/

/*[@function_reference@]...*/

/*
**  Defines of other constants.
*/

/*[@group_of_defined_constants@]...*/
/*[@type_definitions@]*/
/*[@global_variable_definitions@]*/
/*[@static_variable_or_function_definitions@]*/

/* # line 763 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
#define yyclearin pslchar = -1
#define yyerrok pslerrflag = 0
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
typedef i4  yytabelem;
# define YYERRCODE 256

/* # line 11410 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */



/*{
** Name: getfacil	- Decode a facility code from a "set trace point"
**			    command.
**
** Description:
**      This function decodes a facility code from a "set trace point" command.
**	The codes come as strings, of the form "ffnnnn", where "ff" is a
**	two-letter code standing for a facility, and "nnnn" is a number from 0
**	to 9999, standing for the trace flag to be set within the facility.
**	The possible facilities are:
**
**	  Facility Code		Meaning
**	    dm			DMF
**	    ps			PSF
**	    sc			SCF
**	    rd			RDF
**	    qe			QEF
**	    op			OPF
**	    qs			QSF
**	    ad			ADF
**
**	The facility code can be in either upper-case or lower-case.
**
** Inputs:
**      code                            String to be decoded
**	facility			Place to put facility code
**	flagno				Place to put flag number
**
** Outputs:
**      facility                        Filled in with facility code
**      flagno                          Filled in with flag number
**	Returns:
**	    E_PS0000_OK			Success
**	    5966L	Bad facility in facility code
**	    5967L	Bad flag number in facility code
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	25-feb-86 (jeff)
**          written
**      6-sep-86 (seputis)
**          fixed parameters to compare routines
**	16-jun-93 (robf)
**	    Added SXF to facilities
*/
i4
getfacil(
	char               *code,
	i4		   *facility,
	i4                 *flagno)
{
    char                facstring[3];

    STncpy(facstring, code, 2);
    facstring[2] = '\0';
    if (STlen(facstring) != 2)
    {
	return (5966L);
    }

    if (CVal(code + 2, flagno) != OK)
    {
	return (5967L);
    }

    if (*flagno < 0 || *flagno > 9999)
    {
	return (5967L);
    }

    CMtoupper((char *) facstring, (char *) facstring);
    CMtoupper((char *) (facstring + 1), (char *) (facstring + 1));

    if (!STcompare(facstring, "DM"))
    {
	*facility = PSQ_DMFTRACE;
	return (E_PS0000_OK);
    }

    if (!STcompare(facstring, "PS"))
    {
	*facility = PSQ_PSFTRACE;
	return (E_PS0000_OK);
    }

    if (!STcompare(facstring, "SC"))
    {
	*facility = PSQ_SCFTRACE;
	return (E_PS0000_OK);
    }

    if (!STcompare(facstring, "RD"))
    {
	*facility = PSQ_RDFTRACE;
	return (E_PS0000_OK);
    }

    if (!STcompare(facstring, "QE"))
    {
	*facility = PSQ_QEFTRACE;
	return (E_PS0000_OK);
    }

    if (!STcompare(facstring, "OP"))
    {
	*facility = PSQ_OPFTRACE;
	return (E_PS0000_OK);
    }

    if (!STcompare(facstring, "QS"))
    {
	*facility = PSQ_QSFTRACE;
	return (E_PS0000_OK);
    }

    if (!STcompare(facstring, "AD"))
    {
	*facility = PSQ_ADFTRACE;
	return (E_PS0000_OK);
    }

    if (!STcompare(facstring, "GW"))
    {
	*facility = PSQ_GWFTRACE;
	return (E_PS0000_OK);
    }

    if (!STcompare(facstring, "SX"))
    {
	*facility = PSQ_SXFTRACE;
	return (E_PS0000_OK);
    }

    return (5966L);
}

/*{
** Name: sconvert	- Convert a text string to a null-terminated string.
**
** Description:
**      This function takes a text string (of type DB_TEXT_STRING) and converts
**	it to a null-terminated string.  It does the conversion in place.  It
**	returns a pointer to the converted string.
**
** Inputs:
**      string                          The string to be converted.
**
** Outputs:
**      string                          The string is converted in place.
**	Returns:
**	    Pointer to the converted string.
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	25-feb-86 (jeff)
**	    adapted from pre-Jupiter version
*/
char *
sconvert(
	DB_TEXT_STRING     *string)
{
    i4                  count;

    count = string->db_t_count;
    MEcopy((PTR) string->db_t_text, count, (PTR) string);
    ((char *) string)[count] = '\0';
    return ((char *) string);
}

/*{
** Name: qdata_cvt - Convert data passed as DB_DATA_VALUE to a text string.
**
** Description:
**      This function takes data in DB_DATA_VALUE and converts it to
**	a text string (of type DB_TEXT_STRING).
** Inputs:
**	cb		session control block
**	psq_cb		query control block
**	fval		db_data_value with data to be converted.
**	totype		dbms data type to which convert
**      value		address of pointer, which will point to
**			DB_TEXT_STRING with converted data upon return.
**
** Outputs:
**      value		Pointer to DB_TEXT_STRING with converted data.
**	Returns:
**	    status.
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	11-mar-88 (stec)
**	    STtrmwhite call removed.
**	04-mar-96 (mckba01)
**	    Fix #74286, varchar string truncation problem, initialise
**	    db_datatype field of resultant DB_DATA_VALUE before call
**	    to adi_0calclen. Not specifying a resultant db_datatype 
**	    when calling this function with a Nullable source causes
**	    the length returned to be 1 short.
**	19-mar-96 (pchang)
**	    Fixed bug 70204.  Incorrect test on the next symbol location for
**	    byte alignment prevented a new symbol block to be allocated when
**	    there are exactly 2 bytes left on the current symbol table, and
**	    subsequent alignment operation on the next symbol pointer caused
**	    it to advance beyond the current symbol block and thus, corrupting
**	    adjacent object when the next symbol was assigned.  SEGVIO followed
**	    when the trashed object was later referenced.
**	22-Feb-1999 (shero03)
**	    Support 4 operands in adi_0calclen
*/
DB_STATUS
qdata_cvt(
	PSS_SESBLK	*cb,
	PSQ_CB		*psq_cb,
	DB_DATA_VALUE	*fval,
	DB_DT_ID	totype,
	PTR		*value)
{
    DB_STATUS		status;
    ADF_CB	    	adf_cb;
    ADF_CB		*adf_scb = (ADF_CB*) cb->pss_adfcb;
    ADF_FN_BLK	    	adffn;
    i4		    	dv_size;
    i2			dv_prec;
    ADI_FI_DESC	    	*fdesc;
    ADI_DT_NAME		dt_fname;
    ADI_DT_NAME		dt_tname;
    i4		err_code;
    i4			left;

    if (fval->db_datatype == totype)
	dv_size = fval->db_length;
    else
	dv_size = 0;

    /* Copy the session ADF block into local one */
    STRUCT_ASSIGN_MACRO(*adf_scb, adf_cb);

    adf_cb.adf_errcb.ad_ebuflen	   = 0;
    adf_cb.adf_errcb.ad_errmsgp	   = 0;

    /* get the function instance id for this conversion */
    status = adi_ficoerce(&adf_cb, fval->db_datatype, totype, &adffn.adf_fi_id);
    if (status != E_DB_OK)
    {
	goto exit;
    }         

    /* determine the result size. */
    if (!dv_size)
    {
	/* Now lets fill in the datatype length info and
	** allocate space for the data.
	*/
	status = adi_fidesc(&adf_cb, adffn.adf_fi_id, &fdesc);
	if (status != E_DB_OK)
	{
	    goto exit;
	}

	/*	Initialise resultant datatype (used by adi_0calclen)   */

	adffn.adf_r_dv.db_datatype = totype;

	status = adi_0calclen(&adf_cb, &fdesc->adi_lenspec, 1, &fval, 
		&adffn.adf_r_dv);
	dv_size = adffn.adf_r_dv.db_length;
	dv_prec = adffn.adf_r_dv.db_prec;

	if (status != E_DB_OK)
	{
	     goto exit;
	}
    }
# ifdef BYTE_ALIGN
    left = &cb->pss_symblk->pss_symdata[PSS_SBSIZE] - cb->pss_symnext;
    /* 
    ** If not aligned and it is not possible to align or
    ** it doesn't make sense to align, allocate symbol memory block
    */
    if ((((PSS_SBSIZE - DB_CNTSIZE - left) % DB_ALIGN_SZ) != 0) &&
	(left <= DB_ALIGN_SZ)
       )
    {
	status = psf_symall(cb, psq_cb, PSS_SBSIZE);
	if (DB_FAILURE_MACRO(status))
	{
	    return(E_DB_ERROR);	/* error */
	}
    }
    /* always start with aligned values */
    cb->pss_symnext = (u_char *) ME_ALIGN_MACRO(cb->pss_symnext, DB_ALIGN_SZ);
# endif /* BYTE_ALIGN */
    /* Make sure there's room for piece pointer */
    /* add null terminator in case conv to string. */
    if ((((char*) cb->pss_symnext) + dv_size + 1) >=
	(char *)&cb->pss_symblk->pss_symdata[PSS_SBSIZE - 1])
    {
	status = psf_symall(cb, psq_cb, PSS_SBSIZE);
	if (DB_FAILURE_MACRO(status))
	{
	    return(E_DB_ERROR);	/* error */
	}
    }
    *value = (PTR) cb->pss_symnext;
    cb->pss_symnext = (u_char *)((char*)cb->pss_symnext) + dv_size +  1;
    
    ((char*)*value)[dv_size] = EOS;
    adffn.adf_r_dv.db_datatype	= totype;
    adffn.adf_r_dv.db_length	= dv_size;
    adffn.adf_r_dv.db_prec	= dv_prec;
    adffn.adf_r_dv.db_data	= *value;
    STRUCT_ASSIGN_MACRO(*fval, adffn.adf_1_dv);
    adffn.adf_isescape		= FALSE;    /* not the LIKE op, so no esc */
    if ((status = adf_func(&adf_cb, &adffn)) != E_DB_OK)
    {
	goto exit;
    }

exit:
    if (status != E_DB_OK)
    {
	(VOID) adi_tyname(&adf_cb, fval->db_datatype, &dt_fname);
	(VOID) adi_tyname(&adf_cb, totype, &dt_tname);
	(VOID) psf_error(2911L, 0L, PSF_USERERR,
	    &err_code, &psq_cb->psq_error, 3,
	    sizeof (cb->pss_lineno), &cb->pss_lineno,
	    psf_trmwhite(sizeof(dt_fname), (char *) &dt_fname), &dt_fname, 
	    psf_trmwhite(sizeof (dt_tname), (char *) &dt_tname), &dt_tname);
        return (E_DB_ERROR);    
    }
    return (status);
}

/*
** Name: psl_quel_shareable - Apply QUEL-specific checking to determine if a
**			      given QUEL repeat query is shareable
**
** Description: QUEL repeat query will be deemed shareable if it meets the
**		requirements for an SQL repeat query (this was checked before
**		psy_qrymod() was called) AND all of the following
**		hold:
**		    - the query contained no non-repeat parameters,
**		    - repeat query parameters contained no pattern matching
**		      characters, and
**		    - permits required for the query to proceed did not have
**		      qualifications 
**
** Input:
**	sess_cb		session CB
**	qry_mask	mask used to contain all sorts of useful info about a
**			query
**
** Output:
**	*shareable	indicator of whether the query is shareable
**
** Returns:
**	none
**
** History:
**
**	01-21-91 (andre)
**	    written
**	11-oct-93 (swm)
**	    Bug #56448
**	    Declared trbuf for psf_display() to pass to TRformat.
**	    TRformat removes `\n' chars, so to ensure that psf_scctrace()
**	    outputs a logical line (which it is supposed to do), we allocate
**	    a buffer with one extra char for NL and will hide it from TRformat
**	    by specifying length of 1 byte less. The NL char will be inserted
**	    at the end of the message by psf_scctrace().
*/
VOID
psl_quel_shareable(
	PSS_SESBLK	*sess_cb,
	i4		qry_mask,
	bool		*shareable)
{
    i4	val1 = 0, val2 = 0;
    char	trbuf[PSF_MAX_TEXT + 1]; /* last char for `\n' */

    /*
    ** In addition to the requirements currently imposed on SQL queries, we need
    ** to ascertain that all of the following held:
    **  - the query contained no non-repeat parameters,
    **  - repeat query parameters contained no pattern matching characters,
    **    and
    **  - permits required for the query to proceed did not have
    **    qualifications.
    */

    if (!(*shareable))
    {
	/*
	** looks like the query is not shareable, and the caller has not
	** realized it
	*/
    }
    else if (qry_mask & PSS_NONREPEAT_PARAM)
    {
	*shareable = FALSE;

	if (ult_check_macro(&sess_cb->pss_trace, 4, &val1, &val2))
	{
	    psf_display(psf_scctrace, 0, trbuf, sizeof(trbuf) - 1,
		"\nQUEL repeat query is not shareable\n");
	    psf_display(psf_scctrace, 0, trbuf, sizeof(trbuf) - 1,
		"because it involves non-repeat parameters.\n");
	}
    }
    else if (qry_mask & PSS_PM_IN_REPEAT_PARAM)
    {
	*shareable = FALSE;

	if (ult_check_macro(&sess_cb->pss_trace, 4, &val1, &val2))
	{
	    psf_display(psf_scctrace, 0, trbuf, sizeof(trbuf) - 1,
		"\nQUEL repeat query is not shareable\n");
	    psf_display(psf_scctrace, 0, trbuf, sizeof(trbuf) - 1,
		"because it involves repeat parameters with pattern matching characters.\n");
	}
    }
    else if (sess_cb->pss_stmt_flags & PSS_QUAL_IN_PERMS)
    {
	*shareable = FALSE;

	if (ult_check_macro(&sess_cb->pss_trace, 4, &val1, &val2))
	{
	    psf_display(psf_scctrace, 0, trbuf, sizeof(trbuf) - 1,
		"\nQUEL repeat query is not shareable\n");
	    psf_display(psf_scctrace, 0, trbuf, sizeof(trbuf) - 1,
		"because permits with qualifications were used.\n");
	}
    }

    return;
}

/*
** Name: psl_rptqry_tblids - build a list of ids of tables used in this query
**
** Description: build an ORDERED list of ids of tables used in this query.
**		Order is determined by the position in the LRU list of range
**		variable entries in this range variable table.  Order is
**		important since this list will be used to determine if a given
**		query is IDENTICAL to another query, which determination will be
**		based on comparison of texts and lists of table ids of the two
**		queries.  This relies entirely on the fact that whenever a range
**		variable is used, it is brought to the head of the list, thus
**		(I am almost certain that this is so) ensuring that the order of
**		range variable entries after a given query will ALWAYS be the
**		same.
**
** Input:
**	sess_cb		    pointer to sessions' PSS_SESBLK
**	rngtab		    range variable table
**	    pss_qhead	    head of the LRU list
**	    pss_rsrng	    contains the result variable (used for target of
**			    APPEND)
**	    pss_maxrng	    indicates how many range variables were used (count
**			    starts at 0 and may include the variable in
**			    pss_rsrng if qry_mode == PSQ_APPEND)
**	mstream		    memory stream to be used whwn allocting a list of
**			    table ids
**	qry_mode	    query mode (actual query mode, not PSQ_DEFCURS)
**
** Output:
**	num_ids		    number of ids in the list
**	id_list		    ORDERED list of ids of tables used in the query
**	err_blk		    will be filled in if an error occurs
**
** Returns:
**	E_DB_OK		    success
**	E_DB_ERROR	    allocation of memory for table id list failed, or
**			    the assumption that the first N entries in the LRU
**			    range entry list were used in the query was wrong;
**			    the algorithm for determining if a given QUEL repeat
**			    query is shareable esta descompuesto (is broken)
**	
** History:
**
**	25-jan-91 (andre)
**	    written.
**	08-may-91 (rog)
**	    If *num_ids is zero, set *id_list to NULL and return.
**	    (fix for bug 37410)
*/
DB_STATUS
psl_rptqry_tblids(
	PSS_SESBLK	*sess_cb,
	PSS_USRRANGE	*rngtab,
	PSF_MSTREAM     *mstream,
	i4		qry_mode,
	i4		*num_ids,
	DB_TAB_ID	**id_list,
	DB_ERROR        *err_blk)
{
    DB_STATUS		    status;
    register i4	    id_num;
    register DB_TAB_ID	    *idp;
    register PSS_RNGTAB	    *rngvar;

    /*
    ** If *num_ids is zero, we don't have any table ids to process.
    */
    if (!(*num_ids = rngtab->pss_maxrng + 1))
    {
	*id_list = (DB_TAB_ID *) NULL;
	return(E_DB_OK);
    }

    status = psf_malloc(sess_cb, mstream, *num_ids * sizeof(DB_TAB_ID), (PTR *) id_list,
	err_blk);

    if (DB_FAILURE_MACRO(status))
    {
	return(status);
    }

    idp = *id_list;
    id_num = 0;

    /* for APPEND, the first table id will come from rngtab->pss_rsrng */
    if (qry_mode == PSQ_APPEND)
    {
	idp->db_tab_base  = rngtab->pss_rsrng.pss_tabid.db_tab_base;
	idp->db_tab_index = rngtab->pss_rsrng.pss_tabid.db_tab_index;
	id_num++;
	idp++;
    }

    for (rngvar = (PSS_RNGTAB *) rngtab->pss_qhead.q_next;
         id_num < *num_ids;
	 rngvar = (PSS_RNGTAB *) rngvar->pss_rngque.q_next, id_num++, idp++)
    {
	if (!rngvar->pss_used || rngvar->pss_rgno == -1)
	{
	    i4	    err_code;

	    /* We are in deep ... ah ... trouble */
	    (VOID) psf_error(E_PS0521_BAD_TBLID_LIST, 0L, PSF_INTERR,
		&err_code, err_blk, 0);
	    return(E_DB_ERROR);
	}

	idp->db_tab_base  = rngvar->pss_tabid.db_tab_base;
	idp->db_tab_index = rngvar->pss_tabid.db_tab_index;
    }

    return(E_DB_OK);
}
extern WSCREADONLY yytabelem pslexca[];
# define YYNPROD 562
# define YYLAST 984
extern WSCREADONLY yytabelem pslact[];
extern WSCREADONLY yytabelem pslpact[];
extern WSCREADONLY yytabelem pslpgo[];
extern WSCREADONLY yytabelem pslr1[];
extern WSCREADONLY yytabelem pslr2[];
extern WSCREADONLY yytabelem pslchk[];
extern WSCREADONLY yytabelem psldef[];
typedef struct { char *t_name; int t_val; } yytoktype;
#ifndef YYDEBUG
#	define YYDEBUG	0	/* don't allow debugging */
#endif

#if YYDEBUG

GLOBALDEF READONLY	yytoktype psltoks[] =
{
	"ABORT",	257,
	"AGOP",	258,
	"ALL",	259,
	"APPEND",	260,
	"AS",	261,
	"AT",	262,
	"BAOP",	263,
	"BAOPH",	264,
	"BDOP",	265,
	"BGNXACT",	266,
	"BY",	267,
	"CLSCURSOR",	268,
	"COLNO",	269,
	"COLON",	270,
	"COMMA",	271,
	"COMMITTED",	272,
	"COPY",	273,
	"CREATE",	274,
	"DECCONST",	275,
	"DEFCURSOR",	276,
	"DEFERUPD",	277,
	"DEFINTEG",	278,
	"DEFLOC",	279,
	"DEFPERM",	280,
	"DEFQRY",	281,
	"DEFREF",	282,
	"DEFVIEW",	283,
	"DELIM_IDENT",	284,
	"DELETE",	285,
	"DESTROY",	286,
	"DESTINTEG",	287,
	"DESTPERM",	288,
	"DESTREF",	289,
	"DIRECTUPD",	290,
	"DISTRIBUTE",	291,
	"DOLLAR",	292,
	"ENDXACT",	293,
	"EOP",	294,
	"EXECUTE",	295,
	"EXCLUDING",	296,
	"F4CONST",	297,
	"F8CONST",	298,
	"FOR",	299,
	"FORREADONLY",	300,
	"FORUPDATE",	301,
	"FROM",	302,
	"HELP",	303,
	"HEXCONST",	304,
	"I2CONST",	305,
	"I4CONST",	306,
	"IN",	307,
	"INDEX",	308,
	"INTO",	309,
	"INTEGRITY",	310,
	"IS",	311,
	"ISOLATION",	312,
	"ISOP",	313,
	"ISNULL",	314,
	"JOURNALING",	315,
	"LBOP",	316,
	"LEVEL",	317,
	"LOGGING",	318,
	"LPAREN",	319,
	"MODIFY",	320,
	"MOVE",	321,
	"NAME",	322,
	"NOT",	323,
	"NOTNULL",	324,
	"OF",	325,
	"ON",	326,
	"ONLY",	327,
	"ONLYWHERE",	328,
	"OPENCURSOR",	329,
	"ORDERBY",	330,
	"PERIOD",	331,
	"PERMIT",	332,
	"PRINT",	333,
	"QDATA",	334,
	"QUERYID",	335,
	"RANGE",	336,
	"READ",	337,
	"RELOCATE",	338,
	"REPEATABLE",	339,
	"REPLACE",	340,
	"REPCURSOR",	341,
	"REPLICATE",	342,
	"RETCURSOR",	343,
	"RETRIEVE",	344,
	"RPAREN",	345,
	"SAVE",	346,
	"SAVEPOINT",	347,
	"SCONST",	348,
	"SERIALIZABLE",	349,
	"SET",	350,
	"SETAUTOCOMMIT",	351,
	"SETAGGR",	352,
	"SETCPUFACT",	353,
	"SETDATEFMT",	354,
	"SETDECIMAL",	355,
	"SETFLATTEN",	356,
	"SETHASH",	357,
	"SETIOTRACE",	358,
	"SETJOINOP",	359,
	"SETJOURNAL",	360,
	"SETLOCKMODE",	361,
	"SETLTRACE",	362,
	"SETLOGGING",	363,
	"SETLOGTRACE",	364,
	"SETMNYFMT",	365,
	"SETMXIO",	366,
	"SETMXROW",	367,
	"SETMXCPU",	368,
	"SETMXPAGE",	369,
	"SETMXCOST",	370,
	"SETMNYPREC",	371,
	"SETOJFLATTEN",	372,
	"SETOPTIMIZEONLY",	373,
	"SETPARALLEL",	374,
	"SETPRINTQRY",	375,
	"SETQEP",	376,
	"SETRANDOMSEED",	377,
	"SETRETINTO",	378,
	"SETRSTRUCT",	379,
	"SETSESSION",	380,
	"SETSQL",	381,
	"SETSTATS",	382,
	"SETTRACE",	383,
	"SETTRANSACTION",	384,
	"SETUPDROWCNT",	385,
	"SORT",	386,
	"TABID",	387,
	"TO",	388,
	"UAOP",	389,
	"UNCOMMITTED",	390,
	"UNIQUE",	391,
	"UNTIL",	392,
	"UPDATE",	393,
	"VAR",	394,
	"VIEW",	395,
	"WHERE",	396,
	"WITH",	397,
	"WITHNULL",	398,
	"WRITE",	399,
	"unaryop",	400,
	"-unknown-",	-1	/* ends search */
};

GLOBALDEF READONLY	char * pslreds[] =
{
	"-no such reduction-",
	"stmnt : abort",
	"stmnt : append",
	"stmnt : btransact",
	"stmnt : closecurs",
	"stmnt : copy",
	"stmnt : create",
	"stmnt : defloc",
	"stmnt : defcursor",
	"stmnt : defref",
	"stmnt : defrptcurs",
	"stmnt : delete",
	"stmnt : destinteg",
	"stmnt : destperm",
	"stmnt : destref",
	"stmnt : destroy",
	"stmnt : etransact",
	"stmnt : execute",
	"stmnt : index",
	"stmnt : defintegrity",
	"stmnt : modify",
	"stmnt : defpermit",
	"stmnt : query",
	"stmnt : range",
	"stmnt : relocate",
	"stmnt : replace",
	"stmnt : replcursor",
	"stmnt : retcursor",
	"stmnt : retrieve",
	"stmnt : save",
	"stmnt : savepoint",
	"stmnt : setautocommit",
	"stmnt : setaggr",
	"stmnt : setcpufact",
	"stmnt : setdatefmt",
	"stmnt : setdecimal",
	"stmnt : setflatten",
	"stmnt : sethash",
	"stmnt : setiotrace",
	"stmnt : setjoinop",
	"stmnt : setjournal",
	"stmnt : setjtimeout",
	"stmnt : setlockmode",
	"stmnt : setlogging",
	"stmnt : setlogtrace",
	"stmnt : setltrace",
	"stmnt : setrstruct",
	"stmnt : setmnyfmt",
	"stmnt : setmnyprec",
	"stmnt : setmxio",
	"stmnt : setmxrow",
	"stmnt : setmxcpu",
	"stmnt : setmxpage",
	"stmnt : setmxcost",
	"stmnt : setojflatten",
	"stmnt : setoptimizeonly",
	"stmnt : setparallel",
	"stmnt : setprintqry",
	"stmnt : setqep",
	"stmnt : setrandomseed",
	"stmnt : setretinto",
	"stmnt : setsession",
	"stmnt : setsql",
	"stmnt : setstats",
	"stmnt : settrace",
	"stmnt : settransaction",
	"stmnt : setupdrowcnt",
	"stmnt : defview",
	"stmnt : error",
	"abort : abstmnt abortto NAME",
	"abort : abstmnt abortto intconst",
	"abort : abstmnt",
	"abstmnt : ABORT",
	"abortto : TO",
	"abortto : /* empty */",
	"append : apstmt apto apprname tlclause qualclause",
	"apstmt : APPEND",
	"apto : TO",
	"apto : /* empty */",
	"apprname : NAME",
	"apprname : nonkeyword PERIOD NAME",
	"btransact : BGNXACT",
	"closecurs : CLSCURSOR QUERYID",
	"copy : copstmnt copytable LPAREN coparam RPAREN keywd copyfile copywith",
	"copstmnt : COPY",
	"copytable : NAME",
	"coparam : cospecs",
	"coparam : /* empty */",
	"cospecs : entname is fmtspec",
	"cospecs : cospecs COMMA entname is fmtspec",
	"entname : NAME",
	"fmtspec : coent",
	"fmtspec : coent WITHNULL",
	"fmtspec : coent WITHNULL LPAREN covalue RPAREN",
	"nonkey_or_sconst : NAME",
	"nonkey_or_sconst : SCONST",
	"cp_delim : /* empty */",
	"cp_delim : nonkey_or_sconst",
	"coent : nonkey_or_sconst",
	"coent : NAME LPAREN intconst RPAREN cp_delim",
	"coent : NAME LPAREN intconst COMMA intconst RPAREN cp_delim",
	"covalue : sign_op I2CONST",
	"covalue : sign_op I4CONST",
	"covalue : sign_op DECCONST",
	"covalue : sign_op F4CONST",
	"covalue : sign_op F8CONST",
	"covalue : HEXCONST",
	"covalue : SCONST",
	"sign_op : UAOP",
	"sign_op : /* empty */",
	"keywd : INTO",
	"keywd : FROM",
	"copyfile : strconst",
	"copyfile : NAME",
	"copywith : WITH copyclause",
	"copywith : /* empty */",
	"copyclause : copyoption",
	"copyclause : copyclause COMMA copyoption",
	"copyoption : nonkeyword is nonkeyword",
	"copyoption : nonkeyword is sconst",
	"copyoption : nonkeyword is int2_int4",
	"copyoption : nonkeyword is QDATA",
	"copyoption : nonkeyword",
	"create : crestmnt crlocationname LPAREN specs RPAREN crwith",
	"crestmnt : CREATE",
	"crlocationname : NAME COLON crname",
	"crlocationname : crname",
	"crname : NAME",
	"specs : newcolname is typedesc",
	"specs : specs COMMA newcolname is typedesc",
	"newcolname : NAME",
	"typedesc : tname null_default",
	"typedesc : NAME LPAREN intconst RPAREN null_default",
	"typedesc : NAME LPAREN intconst COMMA intconst RPAREN null_default",
	"tname : NAME",
	"tname : NAME NAME",
	"null_default : /* empty */",
	"null_default : WITHNULL",
	"null_default : NOTNULL",
	"null_default : NOTNULL WITH nonkeyword",
	"null_default : NOTNULL NOT nonkeyword",
	"null_default : NOT nonkeyword",
	"crwith : WITH crwithlist",
	"crwith : crwithnull",
	"crwithnull : WITHNULL",
	"crwithnull : /* empty */",
	"crwithlist : crwithelem",
	"crwithlist : crwithlist COMMA crwithelem",
	"crwithelem : cr_single_kwd",
	"crwithelem : cr_nm_eq_no",
	"crwithelem : cr_lst_cl",
	"cr_single_kwd : nonkeyword",
	"cr_nm_eq_no : nonkeyword ISOP intconst",
	"cr_lst_cl : cr_lst_cl_prefix LPAREN cr_lst RPAREN",
	"cr_lst_cl_prefix : nonkeyword ISOP",
	"cr_lst : cr_lst_elem",
	"cr_lst : cr_lst COMMA cr_lst_elem",
	"cr_lst_elem : NAME",
	"defcursor : defcursorstmnt retrieve updclause readclause",
	"defcursorstmnt : OPENCURSOR QUERYID",
	"updclause : updword uplist",
	"updclause : /* empty */",
	"updword : DEFERUPD nonkeyword",
	"updword : DIRECTUPD nonkeyword",
	"updword : FORUPDATE",
	"uplist : uplist_of LPAREN updlist RPAREN",
	"uplist : /* empty */",
	"uplist_of : OF",
	"updlist : updname",
	"updlist : updlist COMMA updname",
	"updname : NAME",
	"readclause : FORREADONLY",
	"readclause : /* empty */",
	"defref : defrefstmnt NAME WITH defrefclauses",
	"defrefstmnt : DEFREF",
	"defrefclauses : defrefclause",
	"defrefclauses : defrefclauses COMMA defrefclause",
	"defrefclause : nonkeyword ISOP NAME COLON COLON NAME COLON NAME",
	"defrefclause : nonkeyword",
	"defrptcurs : defrptcursstmnt retrieve updclause readclause",
	"defrptcursstmnt : DEFCURSOR QUERYID",
	"defloc : defloc_stmt NAME WITH nonkeyword ISOP nonkeyword COMMA nonkeyword ISOP strconst",
	"defloc_stmt : DEFLOC",
	"delete : delstmnt delvar qualclause",
	"delstmnt : DELETE",
	"delvar : var",
	"delvar : nonkeyword PERIOD NAME",
	"destinteg : destintegstmnt NAME destqymlist",
	"destintegstmnt : DESTINTEG",
	"destqymlist : destint",
	"destqymlist : destqymlist COMMA destint",
	"destqymlist : ALL",
	"destint : intconst",
	"destperm : destpermstmnt NAME destqymlist",
	"destpermstmnt : DESTPERM",
	"destref : DESTREF NAME",
	"destroy : destmnt relationlist",
	"destmnt : DESTROY",
	"relationlist : destrname",
	"relationlist : relationlist COMMA destrname",
	"destrname : NAME",
	"etransact : ENDXACT",
	"execute : execstmt LPAREN paramclause RPAREN",
	"execute : execstmt",
	"execstmt : EXECUTE NAME",
	"paramclause : eparam",
	"paramclause : paramclause COMMA eparam",
	"eparam : QDATA",
	"index : instmnt LPAREN indexcols RPAREN index_with",
	"instmnt : indexq ON NAME index_is indexlocname",
	"indexq : INDEX index_unique",
	"index_unique : UNIQUE",
	"index_unique : /* empty */",
	"index_is : IS",
	"index_is : ISOP",
	"indexlocname : NAME COLON NAME",
	"indexlocname : NAME",
	"indexcols : indexcolname",
	"indexcols : indexcols COMMA indexcolname",
	"indexcolname : NAME",
	"index_with : WITH index_withopts",
	"index_with : WITHNULL",
	"index_with : /* empty */",
	"index_withopts : index_withopt",
	"index_withopts : index_withopts COMMA index_withopt",
	"index_withopt : index_woname",
	"index_withopt : index_wonum",
	"index_withopt : index_wolst",
	"index_withopt : index_woword",
	"index_woname : nonkeyword ISOP nonkeyword",
	"index_wonum : nonkeyword ISOP intconst",
	"index_wolst : index_lst_prefix LPAREN index_lst RPAREN",
	"index_lst_prefix : nonkeyword ISOP",
	"index_lst : index_lst_elem",
	"index_lst : index_lst COMMA index_lst_elem",
	"index_lst_elem : NAME",
	"index_woword : nonkeyword",
	"defintegrity : integstmnt integnoise intvar integis qual",
	"integstmnt : DEFINTEG",
	"integnoise : ON",
	"integnoise : /* empty */",
	"integis : IS",
	"integis : ISOP",
	"integis : /* empty */",
	"intvar : var",
	"modify : modstmnt modtable modnoise modstorage modkeys modwith",
	"modstmnt : MODIFY",
	"modtable : NAME",
	"modnoise : TO",
	"modnoise : /* empty */",
	"modstorage : modstorname",
	"modstorage : modstorname UNIQUE",
	"modstorname : modopt_num_cl",
	"modstorname : nonkeyword",
	"modstorname : RELOCATE",
	"modkeys : ON modrptkey",
	"modkeys : /* empty */",
	"modrptkey : modbasekey",
	"modrptkey : modrptkey COMMA modbasekey",
	"m_asc_desc : COLON nonkeyword",
	"modbasekey : modkeyname",
	"modbasekey : modkeyname m_asc_desc",
	"modkeyname : NAME",
	"modwith : modwith_kwd modopts",
	"modwith : WITHNULL",
	"modwith : /* empty */",
	"modwith_kwd : WHERE",
	"modwith_kwd : WITH",
	"modopts : modopt",
	"modopts : modopts COMMA modopt",
	"modopt : modopt_num_cl",
	"modopt : modopt_lst_cl",
	"modopt : modopt_word",
	"modopt_num_cl : nonkeyword ISOP intconst",
	"modopt_lst_cl : modopt_lst_prefix LPAREN modopt_lst RPAREN",
	"modopt_lst_prefix : nonkeyword ISOP",
	"modopt_lst : modopt_lst_elem",
	"modopt_lst : modopt_lst COMMA modopt_lst_elem",
	"modopt_lst_elem : nonkeyword",
	"modopt_word : nonkeyword",
	"defpermit : permit_prefix permtd qualclause",
	"permit_prefix : permstmnt permlist",
	"permit_prefix : permstmnt permlist permword permvar permtarg permwho permplace",
	"permstmnt : DEFPERM",
	"permlist : permxlist",
	"permlist : permlist COMMA permxlist",
	"permxlist : ALL",
	"permxlist : RETRIEVE",
	"permxlist : DELETE",
	"permxlist : APPEND",
	"permxlist : REPLACE",
	"permxlist : nonkeyword",
	"permword : ON",
	"permword : OF",
	"permword : TO",
	"permvar : var",
	"permtarg : LPAREN permtlist RPAREN",
	"permtarg : EXCLUDING LPAREN permtlist RPAREN",
	"permtarg : /* empty */",
	"permtlist : permtelm",
	"permtlist : permtlist COMMA permtelm",
	"permtelm : generic_ident",
	"permwho : TO NAME",
	"permwho : TO SCONST",
	"permwho : TO ALL",
	"permplace : AT NAME",
	"permplace : AT ALL",
	"permplace : /* empty */",
	"permtd : permtime permday",
	"permtd : permday",
	"permtd : permtime",
	"permtd : /* empty */",
	"permtime : FROM timeofday TO timeofday",
	"timeofday : intconst COLON intconst",
	"permday : ON NAME TO NAME",
	"query : query_key dmlstmt",
	"dmlstmt : append",
	"dmlstmt : delete",
	"dmlstmt : replace",
	"dmlstmt : /* empty */",
	"dmlstmt : retrieve",
	"query_key : DEFQRY NAME is",
	"query_key : DEFQRY QUERYID IS",
	"range : rngstmt OF rngclause",
	"range : rngstmt rngclause",
	"rngclause : rngclause COMMA rngelm",
	"rngclause : rngelm",
	"rngelm : NAME is NAME",
	"rngelm : NAME ISNULL",
	"rngelm : NAME is nonkeyword PERIOD NAME",
	"rngstmt : RANGE",
	"relocate : RELOCATE NAME TO NAME",
	"replace : repstmnt repvar tlclause qualclause",
	"repstmnt : REPLACE",
	"repvar : var",
	"repvar : nonkeyword PERIOD NAME",
	"replcursor : replcursorstmnt tlclause",
	"replcursorstmnt : REPCURSOR QUERYID",
	"retcursor : RETCURSOR QUERYID",
	"retrieve : retstmnt intoclause uniqueclause tlclause qualclause sortclause retwithcls",
	"retstmnt : RETRIEVE",
	"intoclause : retkwd locationname",
	"intoclause : /* empty */",
	"uniqueclause : UNIQUE",
	"uniqueclause : /* empty */",
	"retkwd : retkwd_1",
	"retkwd_1 : INTO",
	"retkwd_1 : TO",
	"retkwd_1 : /* empty */",
	"retwithcls : WITH",
	"retwithcls : WITH retwiths",
	"retwithcls : WITHNULL",
	"retwithcls : /* empty */",
	"retwiths : retwith",
	"retwiths : retwiths COMMA retwith",
	"retwith : rt_nm_eq_nm",
	"retwith : rt_nm_eq_no",
	"retwith : rt_single_kwd",
	"retwith : rt_lst_cl",
	"rt_nm_eq_nm : nonkeyword ISOP nonkeyword",
	"rt_nm_eq_no : nonkeyword ISOP intconst",
	"rt_single_kwd : nonkeyword",
	"rt_lst_cl : rt_lst_cl_prefix LPAREN rt_lst RPAREN",
	"rt_lst_cl_prefix : nonkeyword ISOP",
	"rt_lst : rt_lst_elem",
	"rt_lst : rt_lst COMMA rt_lst_elem",
	"rt_lst_elem : NAME",
	"sortclause : sortkwd sortlist",
	"sortclause : /* empty */",
	"sortkwd : SORT",
	"sortkwd : SORT BY",
	"sortkwd : ORDERBY",
	"sortlist : sortkey",
	"sortlist : sortlist COMMA sortkey",
	"sortkey : sortvar",
	"sortkey : sortvar COLON nonkeyword",
	"sortvar : sortrng PERIOD NAME",
	"sortvar : NAME",
	"sortrng : NAME",
	"save : savstmnt sname UNTIL date",
	"save : savstmnt sname",
	"sname : NAME",
	"savstmnt : SAVE",
	"date : month intconst intconst",
	"month : nonkeyword",
	"month : intconst",
	"savepoint : spoint sp_name",
	"spoint : SAVEPOINT",
	"sp_name : NAME",
	"sp_name : intconst",
	"setaggr : SETAGGR strsetparam",
	"setautocommit : SETAUTOCOMMIT st_auto",
	"st_auto : ON",
	"st_auto : nonkeyword",
	"setcpufact : SETCPUFACT intsetparam",
	"setdatefmt : SETDATEFMT strconst",
	"setdecimal : SETDECIMAL strconst",
	"setiotrace : SETIOTRACE",
	"setjoinop : SETJOINOP",
	"setjournal : SETJOURNAL",
	"setjournal : SETJOURNAL ON NAME",
	"setjtimeout : SETJOINOP nonkeyword setjint",
	"setjint : int2_int4",
	"setjint : /* empty */",
	"setltrace : SETLTRACE",
	"setlogging : SETLOGGING",
	"setlogtrace : SETLOGTRACE",
	"setlockmode : setlockstmnt setlockscope WHERE setlockparms",
	"setlockstmnt : SETLOCKMODE",
	"setlockscope : nonkeyword",
	"setlockscope : ON NAME",
	"setlockparms : setlockparm",
	"setlockparms : setlockparms COMMA setlockparm",
	"setlockparm : setlockkey ISOP nonkeyword",
	"setlockparm : setlockkey ISOP intsetparam",
	"setlockkey : nonkeyword",
	"setlockkey : LEVEL",
	"setmnyfmt : SETMNYFMT strconst",
	"setmnyprec : SETMNYPREC intsetparam",
	"setmxio : setmxio_kwd intsetparam",
	"setmxio : setmxio_kwd",
	"setmxio_kwd : SETMXIO",
	"setmxrow : setmxrow_kwd intsetparam",
	"setmxrow : setmxrow_kwd",
	"setmxrow_kwd : SETMXROW",
	"setmxcpu : setmxcpu_kwd intsetparam",
	"setmxcpu : setmxcpu_kwd",
	"setmxcpu_kwd : SETMXCPU",
	"setmxpage : setmxpage_kwd intsetparam",
	"setmxpage : setmxpage_kwd",
	"setmxpage_kwd : SETMXPAGE",
	"setmxcost : setmxcost_kwd intsetparam",
	"setmxcost : setmxcost_kwd",
	"setmxcost_kwd : SETMXCOST",
	"setprintqry : SETPRINTQRY",
	"setoptimizeonly : SETOPTIMIZEONLY",
	"setqep : SETQEP",
	"setflatten : SETFLATTEN",
	"sethash : SETHASH",
	"setojflatten : SETOJFLATTEN",
	"setparallel : SETPARALLEL parconst",
	"parconst : int2_int4",
	"parconst : /* empty */",
	"setrandomseed : SETRANDOMSEED intsetparam",
	"setrandomseed : SETRANDOMSEED",
	"setretinto : SETRETINTO strsetparam",
	"setsession : setsessstmt setsessparm",
	"setsessstmt : SETSESSION",
	"setsessparm : WITH setsesswith",
	"setsessparm : setsessam",
	"setsessparm : ISOLATION LEVEL isolation_level",
	"setsesswith : setsesstype ISOP setsessaction",
	"setsesstype : nonkeyword",
	"setsessaction : nonkeyword nonkeyword",
	"setsessam : READ ONLY",
	"setsessam : READ WRITE",
	"setsql : SETSQL",
	"setrstruct : SETRSTRUCT strsetparam",
	"setstats : SETSTATS strsetparam",
	"settrace : settracestmnt strsetparam",
	"settrace : settracestmnt strsetparam strsetparam",
	"settrace : settracestmnt strsetparam strsetparam intconst",
	"settrace : settracestmnt strsetparam strsetparam intconst intconst",
	"settracestmnt : SETTRACE",
	"settransaction : settranstmnt settranmodes",
	"settranstmnt : SETTRANSACTION",
	"settranmodes : settranmode",
	"settranmodes : settranmode COMMA settranmode",
	"settranmode : settranisolation_level",
	"settranmode : settranaccess_mode",
	"settranisolation_level : ISOLATION LEVEL isolation_level",
	"isolation_level : READ COMMITTED",
	"isolation_level : READ UNCOMMITTED",
	"isolation_level : REPEATABLE READ",
	"isolation_level : SERIALIZABLE",
	"settranaccess_mode : access_mode",
	"access_mode : READ ONLY",
	"access_mode : READ WRITE",
	"intsetparam : intconst",
	"intsetparam : sconst",
	"strsetparam : strconst",
	"strsetparam : NAME",
	"setupdrowcnt : SETUPDROWCNT changed_or_qualified",
	"changed_or_qualified : nonkeyword",
	"defview : viewclause tlclause qualclause",
	"viewclause : viewstmnt crname",
	"viewstmnt : DEFVIEW",
	"tlclause : LPAREN tlist RPAREN",
	"tlist : tlelm",
	"tlist : tlist COMMA tlelm",
	"tlelm : NAME is afcn",
	"tlelm : attrib",
	"tlelm : var PERIOD ALL",
	"is : IS",
	"is : BY",
	"is : ISOP",
	"qualclause : where qual",
	"qualclause : /* empty */",
	"where : WHERE",
	"qual : LPAREN qual RPAREN",
	"qual : NOT qual",
	"qual : qual LBOP qual",
	"qual : clause",
	"clause : afcn relop afcn",
	"clause : attrib ISNULL",
	"clause : attrib IS NOTNULL",
	"relop : EOP",
	"relop : ISOP",
	"relop : BDOP",
	"afcn : aggrfcn",
	"afcn : attribfcn",
	"afcn : afcn BAOP afcn",
	"afcn : afcn BAOPH afcn",
	"afcn : afcn UAOP afcn",
	"afcn : LPAREN afcn RPAREN",
	"afcn : uop afcn",
	"afcn : func LPAREN afcn RPAREN",
	"afcn : func LPAREN afcn COMMA afcn RPAREN",
	"afcn : func LPAREN RPAREN",
	"afcn : func LPAREN afcn COMMA afcn COMMA afcn RPAREN",
	"func : NAME",
	"aggrfcn : AGOP LPAREN aggtarg BY domseq aggfcnwhere qual RPAREN",
	"aggrfcn : AGOP LPAREN aggtarg BY domseq RPAREN",
	"aggrfcn : AGOP LPAREN aggtarg qualclause RPAREN",
	"aggtarg : afcn",
	"aggtarg : var PERIOD ALL",
	"aggfcnwhere : WHERE",
	"aggfcnwhere : ONLYWHERE",
	"domseq : targdom",
	"domseq : domseq COMMA targdom",
	"targdom : afcn",
	"targdom : var PERIOD ALL",
	"attrib : var PERIOD NAME",
	"attrib : var PERIOD COLNO",
	"var : NAME",
	"var : VAR",
	"attribfcn : I2CONST",
	"attribfcn : I4CONST",
	"attribfcn : DECCONST",
	"attribfcn : F4CONST",
	"attribfcn : F8CONST",
	"attribfcn : SCONST",
	"attribfcn : HEXCONST",
	"attribfcn : QDATA",
	"attribfcn : NAME",
	"attribfcn : param",
	"attribfcn : attrib",
	"param : DOLLAR I2CONST is NAME",
	"param : DOLLAR I2CONST ISOP QDATA",
	"uop : UAOP",
	"locationname : NAME COLON crname",
	"locationname : crname",
	"intconst : int2_int4",
	"intconst : QDATA",
	"int2_int4 : I4CONST",
	"int2_int4 : I2CONST",
	"sconst : SCONST",
	"strconst : sconst",
	"strconst : QDATA",
	"generic_ident : NAME",
	"generic_ident : DELIM_IDENT",
	"nonkeyword : NAME",
};
#endif /* YYDEBUG */
FUNC_EXTERN	i4	psl1iftn();
FUNC_EXTERN	i4	psl2iftn();
FUNC_EXTERN	i4	psl3iftn();
FUNC_EXTERN	i4	psl4iftn();
FUNC_EXTERN	i4	psl5iftn();
FUNC_EXTERN	i4	psl6iftn();
FUNC_EXTERN	i4	psl7iftn();
FUNC_EXTERN	i4	psl8iftn();
FUNC_EXTERN	i4	psl9iftn();
FUNC_EXTERN	i4	psl10iftn();
FUNC_EXTERN	i4	psl11iftn();
FUNC_EXTERN	i4	psl12iftn();
FUNC_EXTERN	i4	psl13iftn();


extern	i4	(*pslfunc[])();
/*
**Copyright (c) 2004 Ingres Corporation
*/
/*
** Skeleton parser driver for yacc output
**
**
** History
**
** 07-apr-93 (smc)
**	Commented out YYREENTER text after endif.
** 10-jan-1996 (toumi01; from 1.1 axp_osf port)
**	Cast (long) 3rd... EXsignal args to avoid truncation.
** 17-nov-99 (inkdo01)
**	Changed YYACCEPT symbol to NOT EXsignal (because of kernel 
**	overhead).
** 17-dec-1999 (somsa01)
**	Removed extra EXdelete() from YYACCEPT that was added.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	19-sep-01 (inkdo01)
**	    Added code to error handling to retry reserved words as 
**	    equivalent identifiers.
**	25-jan-06 (dougi)
**	    Changed action calls to pass YYVARS** instead of YYVARS* to
**	    allow recursion on derived tables (subselects in FROM clauses).
*/

/*
** yacc user known macros and defines
*/
#ifdef		YYFUNCTIONS
#	ifdef    VMS
#		define  YYACCVAL	MAXI4
#		define  YYABORTVAL	E_DB_ERROR
#	endif
#	ifdef    UNIX
#		define  YYACCVAL	MAXI4
#		define  YYABORTVAL	E_DB_ERROR
#	endif
#	ifdef    DESKTOP
#		define  YYACCVAL	MAXI4
#		define  YYABORTVAL	E_DB_ERROR
#	endif
/*
** ysstatusp is a pointer, and EXsignal expects i4, so this solution
** is not very clean; there is no other option, however, so we will go
** ahead and use it, should be OK for now.
** This WAS OK until we ported to axp_osf, where i4 and i4 are 32 bits
** and pointers are 64 bits.  EXsignal was (of course) modified as well.
*/
#	define YYERROR	(VOID)EXsignal(EX_PSF_LJMP, 2, (long)1,yystatusp)
#	define YYACCEPT	\
    {\
	EXdelete();\
	return(E_DB_OK);\
    }
#	define YYABORT	(VOID)EXsignal(EX_PSF_LJMP, 2, (long)YYABORTVAL,yystatusp)
#else
#	define YYERROR	goto yyerrlab
#	define YYACCVAL	E_DB_OK
#	define YYACCEPT	return(YYACCVAL)
#	define YYABORT	return(E_DB_ERROR)
#endif

#define YYBACKUP( newtoken, newvalue )\
{\
	if ( yacc_cb->yychar >= 0 || ( pslr2[ yacc_cb->yytmp ] >> 1 ) != 1 )\
	{\
		(*yacc_cb->yyerror)(1, cb, psq_cb);\
		goto yyerrlab;\
	}\
	yacc_cb->yychar = newtoken;\
	yacc_cb->yystate = *yacc_cb->yyps;\
	yacc_cb->yylval = newvalue;\
	goto yynewstate;\
}
#define YYRECOVERING()	(!!yacc_cb->yyerrflag)

/*
** user known globals
*/
#ifndef	YYREENTER
i4  psldebug;			/* set to 1 to get debugging */
#endif

/*
** driver internal defines
*/
#define YYFLAG		(-1000)

/* 
** If not defined we need to define YYVARS, even though it won't be used,
** because -S option requires it.
*/
#ifdef YACC_VARS
#define	    YYVARS	PSS_YYVARS
#else
typedef	i4	YYVARS;
#endif

#ifdef		YYFUNCTIONS
/*{
** Name: YY_1HANDLER, YY_2HANDLER	- Exception handlers for the YACC generated
** parser.
**
** Description:
**      This function is the exception handler for quel and SQL parsers.
**	Exceptions are caught here only if -S flag is specified to YACC.
**	YY_2HANDLER handles exceptions that occur within YY_1HANDLER.
**
** Inputs:
**      exargs                          The exception handler args, as defined
**					by the CL spec.
**
** Outputs:
**      None
**	Returns:
**	    EXDECLARE
**	Exceptions:
**	    none
**
** Side Effects:
**	    Can send error message to terminal and/or log file
**
** History:
**	11-nov-87 (stec)
**          written
*/

i4
YY_2HANDLER(exargs)
EX_ARGS            *exargs;
{
	return(EXDECLARE);
}

i4
YY_1HANDLER(exargs)
EX_ARGS            *exargs;
{
    EX_CONTEXT	context;
    i4		status;

    /*
    ** Handle exception handler exceptions here.
    */
    if (EXdeclare(YY_2HANDLER, &context) != OK)
    {
	EXdelete();
	return(EXRESIGNAL);
    }

    if (EXmatch(EX_PSF_LJMP, 1, exargs->exarg_num))
    {
	if (exargs->exarg_count == 2)
	{
	    DB_STATUS *statp;

	    statp =  (DB_STATUS *) exargs->exarg_array[1];
	    *statp = (DB_STATUS) exargs->exarg_array[0];
	    status = EXDECLARE;
	}
	else
	{
	    /* unexpected exceptions handled by psf_handle */
	    status = EXRESIGNAL;
	}
    }
    else
    {
	/* unexpected exceptions handled by psf_handle */
	status = EXRESIGNAL;
    }
    EXdelete();
    return(status);
}
#endif

/*
** yyparse - return E_DB_OK if worked, E_DB_ERROR if syntax error not recovered
**	     from
*/
DB_STATUS
pslparse(cb,psq_cb)
PSS_SESBLK	*cb;
PSQ_CB *	psq_cb;
{
#ifndef	YYREENTER
	register YYSTYPE *yypvt;	/* top of value stack for $vars */
	i4		 yyerr_decl;
	YYSTYPE yyv[ YYMAXDEPTH ];	/* value stack */
	i4 yys[ YYMAXDEPTH ];		/* state stack */

	YYSTYPE *yypv;			/* top of value stack */
	i4 *yyps;			/* top of state stack */

	i4 yystate;			/* current state */
	i4 yytmp;			/* extra var (lasts between blocks) */

	i4 yynerrs;			/* number of errors */
	i4 yyerrflag;			/* error recovery flag */
	i4 yychar;			/* current input token number */
#else
	YACC_CB *yacc_cb = (YACC_CB *) (cb->pss_yacc);
#endif

#ifndef YACC_VARS
	YYVARS	yyvars, *yyvarsp = &yyvars, **yyvarspp = &yyvarsp;
#endif
#ifdef	YYFUNCTIONS
	EX_CONTEXT	context;
	DB_STATUS	yystatus, *yystatusp = &yystatus;

#ifdef YACC_VARS
#include "yyvarsinit.h"	
#endif
	/*
	** Handle all exceptions here, They will be primarily generated
	** by execution of YYERROR, YYABORT, YYACCEPT.
	*/
	if (EXdeclare(YY_1HANDLER, &context) != OK)
	{
		if (yystatus == YYACCVAL)
		{
			EXdelete();
			/* E_DB_OK should be returned */
			yystatus = E_DB_OK;
			return (yystatus);
		}
		else if (yystatus == YYABORTVAL)
		{
			EXdelete();
			/* E_DB_ERROR should be returned */
			yystatus = E_DB_ERROR;
			return (yystatus);
		}
		else if (yystatus == 1)		/* YYERROR */
		{
			goto yyerrlab;
		}
		else	/* Should never be reached */
		{
			EXdelete();
			/* E_DB_SEVERE should be returned */
			yystatus = E_DB_SEVERE;
			return (yystatus);
		}
	}
#else
#ifdef YACC_VARS
#include "yyvarsinit.h"	
#endif
#endif

	/*
	** Initialize externals - yyparse may be called more than once
	*/
	yacc_cb->yypv = &yacc_cb->yyv[-1];
	yacc_cb->yyps = &yacc_cb->yys[-1];
	yacc_cb->yystate = 0;
	yacc_cb->yytmp = 0;
	yacc_cb->yynerrs = 0;
	yacc_cb->yyerrflag = 0;
	yacc_cb->yychar = -2;

	goto yystack;
	{
		register YYSTYPE *yy_pv;	/* top of value stack */
		register i4  *yy_ps;		/* top of state stack */
		register i4  yy_state;		/* current state */
		register i4   yy_n;		/* internal state number info */

		/*
		** get globals into registers.
		** branch to here only if YYBACKUP was called.
		*/
	yynewstate:
		yy_pv = yacc_cb->yypv;
		yy_ps = yacc_cb->yyps;
		yy_state = yacc_cb->yystate;
		goto yy_newstate;

		/*
		** get globals into registers.
		** either we just started, or we just finished a reduction
		*/
	yystack:
		yy_pv = yacc_cb->yypv;
		yy_ps = yacc_cb->yyps;
		yy_state = yacc_cb->yystate;

		/*
		** top of for (;;) loop while no reductions done
		*/
	yy_stack:
		/*
		** put a state and value onto the stacks
		*/
#ifdef	xDEBUG
		/*
		** if debugging, look up token value in list of value vs.
		** name pairs.  0 and negative (-1) are special values.
		** Note: linear search is used since time is not a real
		** consideration while debugging.
		*/
		if ( yacc_cb->yydebug )
		{
			register i4  yy_i;

			YYPRINTF( "State %d, token ", yy_state );
			if ( yacc_cb->yychar == 0 )
				YYPRINTF( "end-of-file\n" );
			else if ( yacc_cb->yychar < 0 )
				YYPRINTF( "-none-\n" );
			else
			{
				for ( yy_i = 0; psltoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( psltoks[yy_i].t_val == yacc_cb->yychar )
						break;
				}
				YYPRINTF( "%s\n", psltoks[yy_i].t_name );
			}
		}
#endif /* xDEBUG */
		if ( ++yy_ps >= &yacc_cb->yys[ YYMAXDEPTH ] )	/* room on stack? */
		{
			(*yacc_cb->yyerror)(2, cb, psq_cb);
			YYABORT;
		}
		*yy_ps = yy_state;
		yy_pv++;
		STRUCT_ASSIGN_MACRO(yacc_cb->yyval, *yy_pv);

		/*
		** we have a new state - find out what to do
		*/
	yy_newstate:
		if ( ( yy_n = pslpact[ yy_state ] ) <= YYFLAG )
			goto yydefault;		/* simple state */
#ifdef	xDEBUG
		/*
		** if debugging, need to mark whether new token grabbed
		*/
		yacc_cb->yytmp = yacc_cb->yychar == -2;
#endif
		if (yacc_cb->yychar == -2)
			yacc_cb->yychar = (*yacc_cb->yylex)(cb,psq_cb);
		if (yacc_cb->yychar == -1)
			goto yyerrlab;
/*		REPLACED WITH ABOVE 4 LINES
/*		if ( ( yacc_cb->yychar == -2 ) && ( ( yacc_cb->yychar = (*yacc_cb->yylex)(cb,psq_cb) ) < 0 ) )
/*			yacc_cb->yychar = 0;		/* reached EOF */
/**/
#ifdef	xDEBUG
		if ( yacc_cb->yydebug && yacc_cb->yytmp )
		{
			register i4  yy_i;

			YYPRINTF( "Received token " );
			if ( yacc_cb->yychar == 0 )
				YYPRINTF( "end-of-file\n" );
			else if ( yacc_cb->yychar < 0 )
				YYPRINTF( "-none-\n" );
			else
			{
				for ( yy_i = 0; psltoks[yy_i].t_val >= 0;
					yy_i++ )
				{
					if ( psltoks[yy_i].t_val == yacc_cb->yychar )
						break;
				}
				YYPRINTF( "%s\n", psltoks[yy_i].t_name );
			}
		}
#endif /* xDEBUG */
		if ( ( ( yy_n += yacc_cb->yychar ) < 0 ) || ( yy_n >= YYLAST ) )
			goto yydefault;
		if ( pslchk[ yy_n = pslact[ yy_n ] ] == yacc_cb->yychar ) /*valid shift*/
		{
			yacc_cb->yychar = -2;
			STRUCT_ASSIGN_MACRO(yacc_cb->yylval, yacc_cb->yyval);
			yy_state = yy_n;
			if ( yacc_cb->yyerrflag > 0 )
				yacc_cb->yyerrflag--;
			goto yy_stack;
		}

	yydefault:
		if ( ( yy_n = psldef[ yy_state ] ) == -2 )
		{
#ifdef	xDEBUG
			yacc_cb->yytmp = yacc_cb->yychar < 0;
#endif
			if (yacc_cb->yychar == -2)
				yacc_cb->yychar = (*yacc_cb->yylex)(cb,psq_cb);
			if (yacc_cb->yychar == -1)
				goto yyerrlab;
/*			REPLACED WITH ABOVE 4 LINES
/*			if ( ( yacc_cb->yychar < 0 ) &&
/*			    ( ( yacc_cb->yychar = (*yacc_cb->yylex)(cb,psq_cb) ) < 0 ) )
/*			{
/*				yacc_cb->yychar = 0;		/* reached EOF */
/*			}
/**/
#ifdef	xDEBUG
			if ( yacc_cb->yydebug && yacc_cb->yytmp )
			{
				register i4  yy_i;

				YYPRINTF( "Received token " );
				if ( yacc_cb->yychar == 0 )
					YYPRINTF( "end-of-file\n" );
				else if ( yacc_cb->yychar < 0 )
					YYPRINTF( "-none-\n" );
				else
				{
					for ( yy_i = 0;
						psltoks[yy_i].t_val >= 0;
						yy_i++ )
					{
						if ( psltoks[yy_i].t_val
							== yacc_cb->yychar )
						{
							break;
						}
					}
					YYPRINTF( "%s\n", psltoks[yy_i].t_name );
				}
			}
#endif /* xDEBUG */
			/*
			** look through exception table
			*/
			{
				register i4  *yyxi = (i4*)pslexca;

				while ( ( *yyxi != -1 ) ||
					( yyxi[1] != yy_state ) )
				{
					yyxi += 2;
				}
				while ( ( *(yyxi += 2) >= 0 ) &&
					( *yyxi != yacc_cb->yychar ) )
					;
				if ( ( yy_n = yyxi[1] ) < 0 )
					YYACCEPT;
			}
		}

		/*
		** check for syntax error
		*/
		if ( yy_n == 0 )	/* have an error */
		{
			/* no worry about speed here! */
			switch ( yacc_cb->yyerrflag )
			{
			case 0:		/* new error */
				/* First try the new keyword-as-ident trick. */
				if (yacc_cb->yyreswd)
				{
				    yacc_cb->yyrestok = yacc_cb->yychar;  /* save reswd token */
				    yacc_cb->yyreswd = FALSE;	/* reset flag */
				    yacc_cb->yylval.psl_strtype =
					yacc_cb->yy_rwdstr;	/* retry as IDENT */
				    yacc_cb->yychar = NAME;		/* new token type */
				    goto yy_newstate;	/* try again */
				}
				/* Then, if we just tried it and still got error
				** resend original reserved word. */
				if (yacc_cb->yyrestok > 0)
				{
				    yacc_cb->yylval.psl_tytype = PSL_GOVAL;
				    yacc_cb->yychar = yacc_cb->yyrestok;
				    yacc_cb->yyrestok = 0;
				    goto yy_newstate;
				}
				(*yacc_cb->yyerror)(3, cb, psq_cb);
				goto skip_init;
			yyerrlab:
				/*
				** get globals into registers.
				** we have a user generated syntax type error
				*/
				yy_pv = yacc_cb->yypv;
				yy_ps = yacc_cb->yyps;
				yy_state = yacc_cb->yystate;
				yacc_cb->yynerrs++;
			skip_init:
			case 1:
			case 2:		/* incompletely recovered error */
					/* try again... */
				yacc_cb->yyerrflag = 3;
				/*
				** find state where "error" is a legal
				** shift action
				*/
/*
** This comment supercedes the comment above.
** VERY IMPORTANT! Based on the assumption that there will be never more than
one statement to be parsed at a time, we have taken a different approach to
error reporting/recovery; we have concluded that:
- recovery is not needed
- for certain most common errors `error' productions have been defined in the
SQL grammar. This means that when a valid action cannot be taken, parser will
conceptually recognize seeing a special `error' token and will perform an action
specified. Reduction will follow, as if the rule was recognized. The `while'
loop below searches that state stack backwards for the first state which allows
handling of error token, this resulted in executing the `error' action for
unrelated errors and display of messages that were meaningless. We have,
therefore, decided to commnet out the `while' loop and let the code determine
whether error handling is allowed for current state only. This caused some extra
work with forming productions in such a way that `error' actions get executed
when needed, but that was not tough to do, and we have achieved the desired
result.
*/
/*
** Fix for 2 bugs:  09-jun-89 (neil)
** Both the following bugs occur if the scanner returns -1.  There are many
** cases to force this (ie, a names too long, invalid operators).
**	Bug 1: AV: If nothing has been reduced yet and yylex returned -1 then
**	       yy_ps was not yet correctly set to point at any "saved
**	       reduction stack".  The original "while" loop would never
**	       have let us into this code.  Therefore I modified the commented
**	       out "while" loop to a real "if" statement with the same
**	       condition to check.  AV's occurring to statements such as
**		    rollback aaaaaaaaaaaaaaaaaaaaaaa (> db_maxname chars)
**	       which returned -1 and had no saved "reduction" stack yet no
**	       longer occur as yy_ps is not yet pointing at a valid stack entry.
**	Bug 2: Hangs: Some error rules would loop forever when trying to recover
**	       to a YACC "error" rule in a list construct that occurred not
**	       because of a real syntax error (which had pushed a syntax
**	       state), such as:
**		    create table tx (i varchar(abcde))
**	       but because of a lex error in a list construct (-1 returned,
**	       and pushed nothing):
**		    create table tx (i integer, j varchar(^))
**	       These latter types of errors all cause -1 to be returned.
**	       The fix to this is to not even try the error recovery if the
**	       yychar is -1 as the scanner has already reported an error.
** Note both the above could be fixed by just checking yychar != -1, but
** the extra check to confirm a valid value for yy_ps is done just a safety
** check and will usually apply whenever yychar == -1.
*/
/*				while ( yy_ps >= yacc_cb->yys ) */
				if (   yacc_cb->yychar != -1
				    && yy_ps >= yacc_cb->yys )
				{
					yy_n = pslpact[ *yy_ps ] + YYERRCODE;
					if ( yy_n >= 0 && yy_n < YYLAST &&
					    pslchk[pslact[yy_n]] == YYERRCODE)
					{
						/*
						** simulate shift of "error"
						*/
						yy_state = pslact[ yy_n ];
						goto yy_stack;
					}
					/*
					** current state has no shift on
					** "error", pop stack
					*/
#ifdef	xDEBUG
#	define _POP_ "Error recovery pops state %d, uncovers state %d\n"
				/*	if ( yacc_cb->yydebug )
						YYPRINTF( _POP_, *yy_ps,
							yy_ps[-1] ); */
#	undef _POP_
#endif
				/*	yy_ps--;
					yy_pv--; */
				}
				/*
				** there is no state on stack with "error" as
				** a valid shift.  give up.
				*/
				YYABORT;
			case 3:		/* no shift yet; eat a token */
#ifdef	xDEBUG
				/*
				** if debugging, look up token in list of
				** pairs.  0 and negative shouldn't occur,
				** but since timing doesn't matter when
				** debugging, it doesn't hurt to leave the
				** tests here.
				*/
				if ( yacc_cb->yydebug )
				{
					register i4  yy_i;

					YYPRINTF( "Error recovery discards " );
					if ( yacc_cb->yychar == 0 )
						YYPRINTF("token end-of-file\n");
					else if ( yacc_cb->yychar < 0 )
						YYPRINTF( "token -none-\n" );
					else
					{
						for ( yy_i = 0;
							psltoks[yy_i].t_val >= 0;
							yy_i++ )
						{
							if ( psltoks[yy_i].t_val
								== yacc_cb->yychar )
							{
								break;
							}
						}
						YYPRINTF( "token %s\n",
							psltoks[yy_i].t_name );
					}
				}
#endif /* xDEBUG */
				if ( yacc_cb->yychar == 0 )	/* reached EOF. quit */
					YYABORT;
				yacc_cb->yychar = -2;
				goto yy_newstate;
			}
		}/* end if ( yy_n == 0 ) */
		/*
		** reduction by production yy_n
		** put stack tops, etc. so things right after switch
		*/
#ifdef	xDEBUG
		/*
		** if debugging, print the string that is the user's
		** specification of the reduction which is just about
		** to be done.
		*/
		if ( yacc_cb->yydebug )
			YYPRINTF( "Reduce by (%d) \"%s\"\n",
				yy_n, pslreds[ yy_n ] );
#endif
		yacc_cb->yytmp = yy_n;			/* value to switch over */
		yacc_cb->yypvt = yy_pv;			/* $vars top of value stack */
		/*
		** Look in goto table for next state
		** Sorry about using yy_state here as temporary
		** register variable, but why not, if it works...
		** If pslr2[ yy_n ] doesn't have the low order bit
		** set, then there is no action to be done for
		** this reduction.  So, no saving & unsaving of
		** registers done.  The only difference between the
		** code just after the if and the body of the if is
		** the goto yy_stack in the body.  This way the test
		** can be made before the choice of what to do is needed.
		*/
		{
			/* length of production doubled with extra bit */
			register i4  yy_len = pslr2[ yy_n ];

			if ( !( yy_len & 01 ) )
			{
				yy_len >>= 1;
				/* $$ = $1 */
				yy_pv -= yy_len;
				STRUCT_ASSIGN_MACRO(yacc_cb->yypv[1], yacc_cb->yyval);
				yy_state = pslpgo[ yy_n = pslr1[ yy_n ] ] +
					*( yy_ps -= yy_len ) + 1;
				if ( yy_state >= YYLAST ||
					pslchk[ yy_state =
					pslact[ yy_state ] ] != -yy_n )
				{
					yy_state = pslact[ pslpgo[ yy_n ] ];
				}
				goto yy_stack;
			}
			yy_len >>= 1;
			/* $$ = $1 */
			yy_pv -= yy_len;
			STRUCT_ASSIGN_MACRO(yy_pv[1], yacc_cb->yyval);
			yy_state = pslpgo[ yy_n = pslr1[ yy_n ] ] +
				*( yy_ps -= yy_len ) + 1;
			if ( yy_state >= YYLAST ||
				pslchk[ yy_state = pslact[ yy_state ] ] != -yy_n )
			{
				yy_state = pslact[ pslpgo[ yy_n ] ];
			}
		}
					/* save until reenter driver code */
		yacc_cb->yystate = yy_state;
		yacc_cb->yyps = yy_ps;
		yacc_cb->yypv = yy_pv;
	}
#ifdef		YYFUNCTIONS
	if (pslfunc[yacc_cb->yytmp] != (i4 (*)()) NULL)
#ifdef		YYREENTER
	    yacc_cb->yyerr_decl = (*pslfunc[yacc_cb->yytmp])(yacc_cb, yacc_cb->yytmp,
		yystatusp, yyvarspp, cb,psq_cb);
#else
# ifdef rtp_us5
	{
	    /* AIX cc needs temp to avoid "compiler loop" fatal (daveb) */
	    i4  (*f)() = pslfunc[yacc_cb->yytmp];
	    yacc_cb->yyerr_decl = (*f)(yacc.cb, yacc_cb->yytmp, yystatusp, yyvarspp, cb,psq_cb);
	}
# else	
	    yacc_cb->yyerr_decl = (*pslfunc[yacc_cb->yytmp])(yacc_cb->yypvt, &yacc_cb->yyval,
		yacc_cb->yytmp, yystatusp, cb,psq_cb);
# endif
#endif		/* YYREENTER */
	else
	    yacc_cb->yyerr_decl = 0;

	if (yacc_cb->yyerr_decl == YYACCVAL)	/* TRUE iff YYACCEPT action was taken */
	    YYACCEPT;

	/* In case of RTI any other non-zero value indicates an error 
	** returned from a semantic routine, so no error recovery (in YACC
	** sense of the word) is needed, just report the error to caller.
	*/
	if (yacc_cb->yyerr_decl)		/* Production asked for */
	{
	     EXdelete();
	     return (yacc_cb->yyerr_decl);
	}
	/*    goto yyerrlab;		 * Error recovery */
#else
	/*
	** code supplied by user is placed in this switch
	*/
	switch( yacc_cb->yytmp )
	{
		
	}
#endif
	goto yystack;		/* reset registers in driver code */
}


i4
psl1iftn(yacc_cb, yyrule, yystatusp, yyvarspp, cb, psq_cb)
YACC_CB		*yacc_cb;
i4		yyrule;
PSS_SESBLK	*cb;
DB_STATUS	*yystatusp;
YYVARS		**yyvarspp;
PSQ_CB *	psq_cb;
{
    YYVARS	*yyvarsp = *yyvarspp;
	switch(yyrule)
	{
case 68:
if (!((*yyvarspp)->bypass_actions))
/* # line 1413 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	YYABORT;
    } break;

case 69:
if (!((*yyvarspp)->bypass_actions))
/* # line 1419 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	QEF_RCB                *qef_rcb;
	DB_STATUS	       status;
	DB_SP_NAME	       *spname;

	psq_cb->psq_mode = PSQ_ABSAVE;

	/* Allocate memory for savepoint name */
	status = psf_malloc(cb, &cb->pss_ostream, (i4) sizeof(DB_SP_NAME), 
	    (PTR *) &spname, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	/* Give QEF pointer to name */
	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(DB_SP_NAME), (char *) spname);
	qef_rcb = (QEF_RCB *) cb->pss_object;
	qef_rcb->qef_spoint = spname;
    } break;

case 70:
if (!((*yyvarspp)->bypass_actions))
/* # line 1438 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	QEF_RCB                *qef_rcb;
	DB_STATUS	       status;
	DB_SP_NAME	       *spname;
	char		       numbuf[25];

	psq_cb->psq_mode = PSQ_ABSAVE;

	/* Allocate memory for savepoint name */
	status = psf_malloc(cb, &cb->pss_ostream, (i4) sizeof(DB_SP_NAME), 
	    (PTR *) &spname, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	/* Give QEF pointer to name */
	CVla((i4) yacc_cb->yypvt[-0].psl_inttype, (PTR) numbuf);
	STmove(numbuf, ' ', sizeof(DB_SP_NAME), (char *) spname);
	qef_rcb = (QEF_RCB *) cb->pss_object;
	qef_rcb->qef_spoint = spname;
    } break;

case 71:
if (!((*yyvarspp)->bypass_actions))
/* # line 1459 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	QEF_RCB                *qef_rcb;

	psq_cb->psq_mode = PSQ_ABORT;
	qef_rcb = (QEF_RCB *) cb->pss_object;
	qef_rcb->qef_spoint = (DB_SP_NAME *) NULL;
    } break;

case 72:
if (!((*yyvarspp)->bypass_actions))
/* # line 1469 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;
	QEF_RCB		       *qef_rcb;

	/* Create the QEF control block for abort */
	status = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(QEF_RCB), &cb->pss_object,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_mroot(cb, &cb->pss_ostream, cb->pss_object,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	/* Fill in the QEF control block */
    	qef_rcb = (QEF_RCB *) cb->pss_object;
	qef_rcb->qef_length = sizeof(QEF_RCB);
	qef_rcb->qef_type = QEFRCB_CB;
	qef_rcb->qef_owner = (PTR)DB_PSF_ID;
	qef_rcb->qef_ascii_id = QEFRCB_ASCII_ID;
	qef_rcb->qef_sess_id = cb->pss_sessid;
    } break;

case 75:
if (!((*yyvarspp)->bypass_actions))
/* # line 1501 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_QNODE		*node, *rsdmnode;
	DB_STATUS		status;
	PST_RT_NODE		root;
	PST_QTREE		*tree;
	i4			err_code;
	PST_PROCEDURE		*pnode;
	i4			mask;
	i4			mask2;
	i4			qrymod_resp_mask;

	root.pst_rtuser = TRUE;
	root.pst_union.pst_next  = 0;
	root.pst_dups   = PST_ALLDUPS;

	status = pst_node(cb, &cb->pss_ostream, yacc_cb->yypvt[-1].psl_trtype, yacc_cb->yypvt[-0].psl_trtype, PST_ROOT, (PTR) &root,
	    sizeof(root), DB_NODT, (i2) 0, (i4) 0, (DB_ANYTYPE *) NULL, &node,
	    &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);	/* non-zero return means error */
	}

	/* Now check the target list to see if there are any
	** resdoms with pst_ttargtype of PST_USER, if so
	** convert them to PST_ATTNO.
	*/
	for (rsdmnode = node->pst_left;
	 rsdmnode != (PST_QNODE *) NULL && 
	 rsdmnode->pst_sym.pst_type == PST_RESDOM;
	 rsdmnode = rsdmnode->pst_left)
	{
	    if (rsdmnode->pst_sym.pst_value.pst_s_rsdm.pst_ttargtype
		== PST_USER)
	    {
		rsdmnode->pst_sym.pst_value.pst_s_rsdm.pst_ttargtype
		    = PST_ATTNO;
	    }
	}

	if (cb->pss_defqry == PSQ_DEFQRY)
	{
	    /*
	    ** now call psl_shareable() to determine if the query satisfies SQL
	    ** definition of shareability (i.e. all tables used in the query are
	    ** catalogs or are owned by the DBA; QUEL user cannot explicitly
	    ** qualify them with an owner name)
	    */
	    status = psl_shareable(psq_cb, cb, &yyvarsp->qp_shareable, &yyvarsp->shr_qryinfo);
	    if (DB_FAILURE_MACRO(status))
		return(status);
	}

	/* Apply qrymod (views, permits, & integrities) */
	status = psy_qrymod(node, cb, psq_cb, &yyvarsp->join_id, &qrymod_resp_mask);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	/*
	** if this is a repeat query and it is shareable according to SQL
	** definition of shareability, apply QUEL-specific checking
	*/
	if (cb->pss_defqry == PSQ_DEFQRY && yyvarsp->qp_shareable)
	{
	    psl_quel_shareable(cb, yyvarsp->qry_mask, &yyvarsp->qp_shareable);
	}

	/* make sure all mandatory columns have been specified,
	** and build default CONST nodes for any columns
	** that haven't been specified
	*/
	status = psl_check_defaults(cb, cb->pss_resrng, node,
				    FALSE, (DB_TAB_NAME *) NULL, 
				    &psq_cb->psq_error);
	if DB_FAILURE_MACRO(status)
	    return(status);
    
	mask = cb->pss_resrng->pss_tabdesc->tbl_status_mask;
	mask2 = cb->pss_resrng->pss_tabdesc->tbl_2_status_mask;

	/* Make sure base table is updateable;
	** In case of an index or a non-extended catalog,
	** catalog update privilege must be on.
	** In case of security catalog session must have
	** security privilege active.
	** Bug 34079, since 6.0 you were never supposed to be 
        ** able to update a secondary index.  Fixed it so 
        ** the same error is returned whether you have update 
        ** syscat or not (jennifer)
	** 
	** if PSS_REPAIR_SYSCAT and PSS_CATUPD are set in cb->pss_ses_flag,
	** allow APPEND to indexes which are non-extended catalogs
	*/
	if (mask & DMT_IDX)
	{
	    if (!(   mask & DMT_CATALOG && ~mask & DMT_EXTENDED_CAT
	          && cb->pss_ses_flag & PSS_CATUPD
		  && cb->pss_ses_flag & PSS_REPAIR_SYSCAT
		 )
	       )
	    {
		_VOID_ psf_error(2106L, 0L, PSF_USERERR, &err_code,
		    &psq_cb->psq_error, 1,
		    psf_trmwhite(sizeof(DB_TAB_NAME),
			(char *) &cb->pss_resrng->pss_tabname),
		    &cb->pss_resrng->pss_tabname);
		return(E_DB_ERROR);
	    }
	}
	else if ( ( (mask & DMT_CATALOG || mask2 & DMT_TEXTENSION)
		 && ~mask & DMT_EXTENDED_CAT
		 && !(cb->pss_ses_flag & PSS_CATUPD))
	       ||
	        ((mask & DMT_SECURE) &&
	          !(cb->pss_ustat & DU_USECURITY)))
	{
		i4 local_status;
		DB_ERROR	    e_error;

		if ( Psf_srvblk->psf_capabilities & PSF_C_C2SECURE )
		{
		    local_status = psy_secaudit(FALSE, cb,
			    (char *)&cb->pss_resrng->pss_tabname,
			    &cb->pss_resrng->pss_tabdesc->tbl_owner,
			    sizeof(DB_TAB_NAME), 
			    SXF_E_TABLE,
			    I_SX2020_PROT_REJECT, 
			    SXF_A_FAIL|SXF_A_INSERT, 
			    &e_error);

		    if (local_status > status)
			status = local_status;
		}

		/* process security_alarms */
		local_status = psy_do_alarm_fail(cb, 
				cb->pss_resrng, PSQ_APPEND, &e_error);
		if (local_status > status)
		    status = local_status;
	    _VOID_ psf_error(2107L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 2, sizeof(cb->pss_lineno), &cb->pss_lineno,
		psf_trmwhite(sizeof(DB_TAB_NAME), 
		    (char *) &cb->pss_resrng->pss_tabname),
		&cb->pss_resrng->pss_tabname);
	    return (E_DB_ERROR);
	}

	/* Create the query tree header */
	status = pst_header(cb, psq_cb, PST_UNSPECIFIED, (PST_QNODE *) NULL,
	    node, &tree, &pnode, PST_0FULL_HEADER, (PSS_Q_XLATE *) NULL);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	/* set pst_numjoins.  $Yjoin_id contains the highest join id */
	tree->pst_numjoins = yyvarsp->join_id;

	/* Fix the root in QSF */
	status = psf_mroot(cb, &cb->pss_ostream, (PTR) pnode, &psq_cb->psq_error);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_qttype = tree;
    } break;

case 76:
if (!((*yyvarspp)->bypass_actions))
/* # line 1673 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;

	psq_cb->psq_mode = PSQ_APPEND;
	if ((status = psf_mopen(cb, QSO_QTREE_OBJ, &cb->pss_ostream,
	    &psq_cb->psq_error)) != E_DB_OK)
	{
	    return (status);	    /* non-zero return means error */
	}
    } break;

case 79:
if (!((*yyvarspp)->bypass_actions))
/* # line 1690 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;
	PSS_RNGTAB	       *resrange;
	DB_TAB_NAME	       tabname;
	i4		       err_code;
	i4		       rngvar_info;

	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(tabname), tabname.db_tab_name);
	status = psl0_rngent(&cb->pss_usrrange, -1, "", &tabname, cb,
	    FALSE, &resrange, psq_cb->psq_mode, &psq_cb->psq_error,
	    PSS_USRTBL | PSS_DBATBL | PSS_INGTBL, &rngvar_info, 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}
	if (!resrange)
	{
	    (VOID) psf_error(2108L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 2, sizeof(cb->pss_lineno), &cb->pss_lineno,
		(i4) STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);
	}

	cb->pss_resrng = resrange;
    } break;

case 80:
if (!((*yyvarspp)->bypass_actions))
/* # line 1716 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;
	PSS_RNGTAB	       *resrange;
	DB_TAB_NAME	       tabname;
	i4		       err_code;
	i4		       rngvar_info;

	if (STcasecmp(yacc_cb->yypvt[-2].psl_strtype, "session"))
	{
	    (VOID) psf_error(5153, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1,
		psf_trmwhite(sizeof(DB_OWN_NAME), 
		    (char *) yacc_cb->yypvt[-2].psl_strtype), yacc_cb->yypvt[-2].psl_strtype);
	    return (E_DB_ERROR);
	}
	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(tabname), tabname.db_tab_name);
	status = psl0_rngent(&cb->pss_usrrange, -1, "", &tabname, cb,
	    FALSE, &resrange, psq_cb->psq_mode, &psq_cb->psq_error,
	    PSS_SESTBL, &rngvar_info, 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}
	if (!resrange)
	{
	    (VOID) psf_error(2108L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 2, sizeof(cb->pss_lineno), &cb->pss_lineno,
		(i4) STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);
	}

	cb->pss_resrng = resrange;
    } break;

case 81:
if (!((*yyvarspp)->bypass_actions))
/* # line 1752 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;
	QEF_RCB		       *qef_rcb;

	psq_cb->psq_mode = PSQ_BGNTRANS;

	/* Allocate QEF control block for begin transaction */
	status = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(QEF_RCB), &cb->pss_object,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_mroot(cb, &cb->pss_ostream, cb->pss_object,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	/* Fill in the QEF control block */
    	qef_rcb = (QEF_RCB *) cb->pss_object;
	qef_rcb->qef_length = sizeof(QEF_RCB);
	qef_rcb->qef_type = QEFRCB_CB;
	qef_rcb->qef_owner = (PTR)DB_PSF_ID;
	qef_rcb->qef_ascii_id = QEFRCB_ASCII_ID;
	qef_rcb->qef_modifier = QEF_MSTRAN;
	qef_rcb->qef_sess_id = cb->pss_sessid;
	qef_rcb->qef_db_id = cb->pss_dbid;
	qef_rcb->qef_flag = 0;
    } break;

case 82:
if (!((*yyvarspp)->bypass_actions))
/* # line 1785 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSC_CURBLK             *cursor;
	DB_STATUS	       status;
	i4		       err_code;
	QEF_RCB		       *qef_rcb;

	psq_cb->psq_mode = PSQ_CLSCURS;

	/* Allocate a QEF control block for doing the close */
	status = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(QEF_RCB), &cb->pss_object,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_mroot(cb, &cb->pss_ostream, cb->pss_object,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	/* Fill in the QEF control block */
    	qef_rcb = (QEF_RCB *) cb->pss_object;
	qef_rcb->qef_length = sizeof(QEF_RCB);
	qef_rcb->qef_type = QEFRCB_CB;
	qef_rcb->qef_owner = (PTR)DB_PSF_ID;
	qef_rcb->qef_ascii_id = QEFRCB_ASCII_ID;
	qef_rcb->qef_sess_id = cb->pss_sessid;

	/* Hand cursor id back to caller */
	STRUCT_ASSIGN_MACRO(*(yacc_cb->yypvt[-0].psl_cursid), psq_cb->psq_cursid);

	/* Look up the cursor */
	status = psq_crfind(cb, yacc_cb->yypvt[-0].psl_cursid, &cursor, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	/* Complain if not found */
	if (cursor == (PSC_CURBLK *) NULL || cursor->psc_open == FALSE)
	{
	    (VOID) psf_error(2205L, 0L, PSF_USERERR,
		&err_code, &psq_cb->psq_error, 2, sizeof(cb->pss_lineno),
		&cb->pss_lineno, DB_MAXNAME, yacc_cb->yypvt[-0].psl_cursid->db_cur_name);
	    return (E_DB_ERROR);
	}

	/* Get rid of the cursor control block */
	status = psq_crclose(cursor, &cb->pss_curstab, &cb->pss_memleft,
	    &psq_cb->psq_error);
 	if (status != E_DB_OK)
	    return (status);
    } break;

case 83:
if (!((*yyvarspp)->bypass_actions))
/* # line 1840 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_cp1_copy(cb, psq_cb, &yyvarsp->xlated_qry,
		(PTR) NULL, &yyvarsp->with_clauses, &psq_cb->psq_error);
	if (DB_FAILURE_MACRO(status))
	    return (status);
    } break;

case 84:
if (!((*yyvarspp)->bypass_actions))
/* # line 1851 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_cp2_copstmnt( cb, &psq_cb->psq_mode, &yyvarsp->with_clauses,
				   &psq_cb->psq_error);
	if (DB_FAILURE_MACRO(status))
	    return (status);
    } break;

case 85:
if (!((*yyvarspp)->bypass_actions))
/* # line 1862 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;
	DB_TAB_NAME		tabname;
	PSS_RNGTAB		*resrange;
	i4		       rngvar_info;

	/* Look up table id and timestamp */
	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(DB_TAB_NAME), (char *) &tabname);
	status = psl_rngent(&cb->pss_auxrng, -1, "", &tabname, cb,
	    FALSE, &resrange, psq_cb->psq_mode, &psq_cb->psq_error,
	    &rngvar_info);
	if (status != E_DB_OK)
	    return (status);

	status = psl_cp3_copytable(cb, resrange, &tabname,
		(PSS_Q_XLATE *) NULL, (DD_LDB_DESC *) NULL,
		&psq_cb->psq_error);

	if (DB_FAILURE_MACRO(status))
	    return (status);
    } break;

case 87:
if (!((*yyvarspp)->bypass_actions))
/* # line 1887 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	status;

	status = psl_cp4_coparam(cb, (PSS_Q_XLATE *) NULL, &psq_cb->psq_error);
	if (DB_FAILURE_MACRO(status))
	    return (status);
    } break;

case 88:
if (!((*yyvarspp)->bypass_actions))
/* # line 1897 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_cp5_cospecs(cb, (PSS_Q_XLATE *) NULL, yacc_cb->yypvt[-2].psl_strtype,
		(PTR) NULL, &psq_cb->psq_error);

	if (DB_FAILURE_MACRO(status))
	    return (status);
    } break;

case 89:
if (!((*yyvarspp)->bypass_actions))
/* # line 1907 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_cp5_cospecs(cb, (PSS_Q_XLATE *) NULL, yacc_cb->yypvt[-2].psl_strtype,
		(PTR) NULL, &psq_cb->psq_error);

	if (DB_FAILURE_MACRO(status))
	    return (status);
    } break;

case 90:
if (!((*yyvarspp)->bypass_actions))
/* # line 1919 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	status;

	status = psl_cp6_entname(cb, yacc_cb->yypvt[-0].psl_strtype, &psq_cb->psq_error, (PTR *) NULL);

	if (DB_FAILURE_MACRO(status))
	    return (status);

	cb->pss_stmt_flags &= ~PSS_CP_DUMMY_COL;
	yacc_cb->yyval.psl_strtype = yacc_cb->yypvt[-0].psl_strtype;
    } break;

case 92:
if (!((*yyvarspp)->bypass_actions))
/* # line 1934 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_cp7_fmtspec(cb, &psq_cb->psq_error, FALSE);

	if (DB_FAILURE_MACRO(status))
	    return (status);

    } break;

case 93:
if (!((*yyvarspp)->bypass_actions))
/* # line 1944 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_cp7_fmtspec(cb, &psq_cb->psq_error, TRUE);

	if (DB_FAILURE_MACRO(status))
	    return (status);
    } break;

case 94:
if (!((*yyvarspp)->bypass_actions))
/* # line 1955 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	char	    *letter;
	/*
	** Translate nonkeywords to lower case if regular ids are not always 
	** lowercased
	*/
	if (~*cb->pss_dbxlate & CUI_ID_REG_L)
	{
	    for (letter =  yacc_cb->yypvt[-0].psl_strtype; *letter != EOS; CMnext(letter))
	        CMtolower(letter, letter);
        }

	yyvarsp->id_type = PSS_ID_NONKEY;
	yacc_cb->yyval.psl_strtype = yacc_cb->yypvt[-0].psl_strtype;
    } break;

case 95:
if (!((*yyvarspp)->bypass_actions))
/* # line 1971 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yyvarsp->id_type = PSS_ID_SCONST;
	yacc_cb->yyval.psl_strtype = sconvert(yacc_cb->yypvt[-0].psl_textype);
    } break;

case 96:
if (!((*yyvarspp)->bypass_actions))
/* # line 1978 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	/* no delimiter */
	yacc_cb->yyval.psl_strtype = (char *) NULL;
    } break;

case 97:
if (!((*yyvarspp)->bypass_actions))
/* # line 1983 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_strtype = yacc_cb->yypvt[-0].psl_strtype;
    } break;

case 98:
if (!((*yyvarspp)->bypass_actions))
/* # line 1989 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	/*
	** This is a possibly quoted column type, possibly with a delimiter.
	**     - i4
	**     - c0tab
	**     - date
	**     - "d0null"
	*/

	status = psl_cp8_coent(cb, psq_cb, (bool) TRUE, yacc_cb->yypvt[-0].psl_strtype, 0, (i4 *) NULL,
	    (char *) NULL);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 99:
if (!((*yyvarspp)->bypass_actions))
/* # line 2006 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;
	i4			len = (i4) yacc_cb->yypvt[-2].psl_inttype;

	/*
	** This is a column type, length and possibly a (possibly quoted)
	** delimiter.
	**     - text(40)[["]nl["]]
	*/

	status = psl_cp8_coent(cb, psq_cb, (bool) FALSE, yacc_cb->yypvt[-4].psl_strtype, 1, &len, yacc_cb->yypvt[-0].psl_strtype);

	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 100:
if (!((*yyvarspp)->bypass_actions))
/* # line 2022 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;
	i4			len[2];

	/*
	** This is a column type, length and possibly a (possibly quoted)
	** delimiter.
	**     - decimal(15,2)[["]nl["]]
	*/

	len[0] = yacc_cb->yypvt[-4].psl_inttype;
	len[1] = yacc_cb->yypvt[-2].psl_inttype;
	status = psl_cp8_coent(cb, psq_cb, (bool) FALSE, yacc_cb->yypvt[-6].psl_strtype, 2, len, yacc_cb->yypvt[-0].psl_strtype);

	if (DB_FAILURE_MACRO(status))
	    return(status);
     } break;

case 101:
if (!((*yyvarspp)->bypass_actions))
/* # line 2042 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	QEF_RCB		*qef_rcb;
	QEU_CPDOMD	*cpdom_desc;

	qef_rcb = (QEF_RCB *) cb->pss_object;
	cpdom_desc = qef_rcb->qeu_copy->qeu_fdesc->cp_cur_fdd;

	if (yacc_cb->yypvt[-1].psl_tytype == ADI_MINUS_OP)
	    *yacc_cb->yypvt[-0].psl_i2type = -(*yacc_cb->yypvt[-0].psl_i2type);
	cpdom_desc->cp_nulldbv.db_datatype = DB_INT_TYPE;
	cpdom_desc->cp_nulldbv.db_prec = 0;
	cpdom_desc->cp_nulldbv.db_length = sizeof(i2);
	cpdom_desc->cp_nulldbv.db_data = (PTR) yacc_cb->yypvt[-0].psl_i2type;
    } break;

case 102:
if (!((*yyvarspp)->bypass_actions))
/* # line 2057 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	QEF_RCB		*qef_rcb;
	QEU_CPDOMD	*cpdom_desc;

	qef_rcb = (QEF_RCB *) cb->pss_object;
	cpdom_desc = qef_rcb->qeu_copy->qeu_fdesc->cp_cur_fdd;

	if (yacc_cb->yypvt[-1].psl_tytype == ADI_MINUS_OP)
	    *yacc_cb->yypvt[-0].psl_i4type = -(*yacc_cb->yypvt[-0].psl_i4type);
	cpdom_desc->cp_nulldbv.db_datatype = DB_INT_TYPE;
	cpdom_desc->cp_nulldbv.db_prec = 0;
	cpdom_desc->cp_nulldbv.db_length = sizeof(i4);
	cpdom_desc->cp_nulldbv.db_data = (PTR) yacc_cb->yypvt[-0].psl_i4type;

    } break;

case 103:
if (!((*yyvarspp)->bypass_actions))
/* # line 2073 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	QEF_RCB		*qef_rcb;
	QEU_CPDOMD	*cpdom_desc;
	i2		prec_scale;
	i4		prec;
	i4		scale;
	u_char		tmp[DB_MAX_DECLEN];
	PTR		d;

	qef_rcb = (QEF_RCB *) cb->pss_object;
	cpdom_desc = qef_rcb->qeu_copy->qeu_fdesc->cp_cur_fdd;

	I2ASSIGN_MACRO(*yacc_cb->yypvt[-0].psl_dectype, prec_scale);
	prec  = DB_P_DECODE_MACRO(prec_scale);
	scale = DB_S_DECODE_MACRO(prec_scale);
	d     = (PTR) ((char *) yacc_cb->yypvt[-0].psl_dectype + sizeof(i2));

	if (yacc_cb->yypvt[-1].psl_tytype == ADI_MINUS_OP)
	{
	    MEcopy(d, DB_PREC_TO_LEN_MACRO(prec), (PTR)tmp);
	    MHpkneg((PTR)tmp, prec, scale, d);
	}
	
	cpdom_desc->cp_nulldbv.db_datatype = DB_DEC_TYPE;
	cpdom_desc->cp_nulldbv.db_prec = prec_scale;
	cpdom_desc->cp_nulldbv.db_length = DB_PREC_TO_LEN_MACRO(prec);
	cpdom_desc->cp_nulldbv.db_data = d;
    } break;
	}
	return(0);
}

i4
psl2iftn(yacc_cb, yyrule, yystatusp, yyvarspp, cb, psq_cb)
YACC_CB		*yacc_cb;
i4		yyrule;
PSS_SESBLK	*cb;
DB_STATUS	*yystatusp;
YYVARS		**yyvarspp;
PSQ_CB *	psq_cb;
{
    YYVARS	*yyvarsp = *yyvarspp;
	switch(yyrule)
	{
case 104:
if (!((*yyvarspp)->bypass_actions))
/* # line 2102 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	QEF_RCB		*qef_rcb;
	QEU_CPDOMD	*cpdom_desc;

	qef_rcb = (QEF_RCB *) cb->pss_object;
	cpdom_desc = qef_rcb->qeu_copy->qeu_fdesc->cp_cur_fdd;

	if (yacc_cb->yypvt[-1].psl_tytype == ADI_MINUS_OP)
	    *yacc_cb->yypvt[-0].psl_f4type = -(*yacc_cb->yypvt[-0].psl_f4type);
	cpdom_desc->cp_nulldbv.db_datatype = DB_FLT_TYPE;
	cpdom_desc->cp_nulldbv.db_prec = 0;
	cpdom_desc->cp_nulldbv.db_length = sizeof(f4);
	cpdom_desc->cp_nulldbv.db_data = (PTR) yacc_cb->yypvt[-0].psl_f4type;

    } break;

case 105:
if (!((*yyvarspp)->bypass_actions))
/* # line 2118 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	QEF_RCB		*qef_rcb;
	QEU_CPDOMD	*cpdom_desc;

	qef_rcb = (QEF_RCB *) cb->pss_object;
	cpdom_desc = qef_rcb->qeu_copy->qeu_fdesc->cp_cur_fdd;

	if (yacc_cb->yypvt[-1].psl_tytype == ADI_MINUS_OP)
	    *yacc_cb->yypvt[-0].psl_f8type = -(*yacc_cb->yypvt[-0].psl_f8type);
	cpdom_desc->cp_nulldbv.db_datatype = DB_FLT_TYPE;
	cpdom_desc->cp_nulldbv.db_prec = 0;
	cpdom_desc->cp_nulldbv.db_length = sizeof(f8);
	cpdom_desc->cp_nulldbv.db_data = (PTR) yacc_cb->yypvt[-0].psl_f8type;

    } break;

case 106:
if (!((*yyvarspp)->bypass_actions))
/* # line 2134 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	QEF_RCB		*qef_rcb;
	QEU_CPDOMD	*cpdom_desc;

	qef_rcb = (QEF_RCB *) cb->pss_object;
	cpdom_desc = qef_rcb->qeu_copy->qeu_fdesc->cp_cur_fdd;

	cpdom_desc->cp_nulldbv.db_datatype = DB_VCH_TYPE;
	cpdom_desc->cp_nulldbv.db_prec = 0;
	cpdom_desc->cp_nulldbv.db_length = (i4)yacc_cb->yypvt[-0].psl_textype->db_t_count + DB_CNTSIZE;
	cpdom_desc->cp_nulldbv.db_data = (PTR) yacc_cb->yypvt[-0].psl_textype;
    } break;

case 107:
if (!((*yyvarspp)->bypass_actions))
/* # line 2147 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	QEF_RCB		*qef_rcb;
	QEU_CPDOMD	*cpdom_desc;

	qef_rcb = (QEF_RCB *) cb->pss_object;
	cpdom_desc = qef_rcb->qeu_copy->qeu_fdesc->cp_cur_fdd;

	cpdom_desc->cp_nulldbv.db_datatype = DB_VCH_TYPE;
	cpdom_desc->cp_nulldbv.db_prec = 0;
	cpdom_desc->cp_nulldbv.db_length = (i4)yacc_cb->yypvt[-0].psl_textype->db_t_count + DB_CNTSIZE;
	cpdom_desc->cp_nulldbv.db_data = (PTR) yacc_cb->yypvt[-0].psl_textype;
    } break;

case 108:
if (!((*yyvarspp)->bypass_actions))
/* # line 2162 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	if (yacc_cb->yypvt[-0].psl_tytype == ADI_SUB_OP)
	    yacc_cb->yyval.psl_tytype = ADI_MINUS_OP;
	else
	    yacc_cb->yyval.psl_tytype = ADI_PLUS_OP;
    } break;

case 109:
if (!((*yyvarspp)->bypass_actions))
/* # line 2169 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_tytype = ADI_PLUS_OP;
    } break;

case 110:
if (!((*yyvarspp)->bypass_actions))
/* # line 2175 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	QEF_RCB			*qef_rcb;
	DB_STATUS	status;

	if (~cb->pss_distrib & DB_3_DDB_SESS)
	{
	    qef_rcb = (QEF_RCB *) cb->pss_object;
	    qef_rcb->qeu_copy->qeu_direction = CPY_INTO;
	}
	else
	{
	    status = psq_x_add(cb, "", &cb->pss_ostream,
				yyvarsp->xlated_qry.pss_buf_size,
				&yyvarsp->xlated_qry.pss_q_list,
				(i4) -1,	
				(char *)NULL, (char *)NULL, "into ", 
				&psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);
	}
    } break;

case 111:
if (!((*yyvarspp)->bypass_actions))
/* # line 2197 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	QEF_RCB			*qef_rcb;
	DB_STATUS	status;

	if (~cb->pss_distrib & DB_3_DDB_SESS)
	{
	    qef_rcb = (QEF_RCB *) cb->pss_object;
	    qef_rcb->qeu_copy->qeu_direction = CPY_FROM;
	}
	else
	{
	    status = psq_x_add(cb, "", &cb->pss_ostream,
				yyvarsp->xlated_qry.pss_buf_size,
				&yyvarsp->xlated_qry.pss_q_list,
				(i4) -1,	
				(char *)NULL, (char *)NULL, "from ", 
				&psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);
	}
    } break;

case 112:
if (!((*yyvarspp)->bypass_actions))
/* # line 2221 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	QEF_RCB		*qef_rcb;
	QEU_COPY	*qe_copy;
	DB_STATUS	status;

	if (cb->pss_distrib & DB_3_DDB_SESS)
	{
	    status = psq_x_add(cb, yacc_cb->yypvt[-0].psl_strtype, &cb->pss_ostream,
				yyvarsp->xlated_qry.pss_buf_size,
				&yyvarsp->xlated_qry.pss_q_list,
				(i4)STlength((char *)yacc_cb->yypvt[-0].psl_strtype),	
				"\'", "\'", (char *) NULL, &psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);
	    yyvarsp->scanbuf_ptr = (PTR) cb->pss_nxtchar;
	}
	else
	{
	    qef_rcb = (QEF_RCB *) cb->pss_object;
	    qe_copy = qef_rcb->qeu_copy;

	    /*
	    ** Copy the filename to the control block.
	    */
	    status = psf_malloc(cb, &cb->pss_ostream, STlength((char *) yacc_cb->yypvt[-0].psl_strtype) + 1,
		(PTR *) &qe_copy->qeu_fdesc->cp_filename, &psq_cb->psq_error);
	    if (status != E_DB_OK)
		return (status);
	    STcopy((char *) yacc_cb->yypvt[-0].psl_strtype, qe_copy->qeu_fdesc->cp_filename);
	} 
    } break;

case 113:
if (!((*yyvarspp)->bypass_actions))
/* # line 2253 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	QEF_RCB		*qef_rcb;
	i4		err_code;

	qef_rcb = (QEF_RCB *) cb->pss_object;

	if (STcompare(yacc_cb->yypvt[-0].psl_strtype, "program") != 0)
	{
	    (VOID) psf_error(5851L, 0L, PSF_USERERR, &err_code,
		    &psq_cb->psq_error, 1, STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);
	}

	qef_rcb->qeu_copy->qeu_stat |= CPY_PROGRAM;
    } break;

case 118:
if (!((*yyvarspp)->bypass_actions))
/* # line 2279 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_cp9_nm_eq_nm(cb, yacc_cb->yypvt[-2].psl_strtype, yacc_cb->yypvt[-0].psl_strtype, &psq_cb->psq_error);

	if (DB_FAILURE_MACRO(status))
	    return(status);

    } break;

case 119:
if (!((*yyvarspp)->bypass_actions))
/* # line 2289 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_cp10_nm_eq_str(cb, yacc_cb->yypvt[-2].psl_strtype, sconvert(yacc_cb->yypvt[-0].psl_textype), &psq_cb->psq_error);

	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 120:
if (!((*yyvarspp)->bypass_actions))
/* # line 2298 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_cp11_nm_eq_no( cb, yacc_cb->yypvt[-2].psl_strtype, yacc_cb->yypvt[-0].psl_inttype, &yyvarsp->with_clauses,
				    &psq_cb->psq_error);

	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 121:
if (!((*yyvarspp)->bypass_actions))
/* # line 2308 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_cp12_nm_eq_qdata(cb, psq_cb, yacc_cb->yypvt[-2].psl_strtype, yacc_cb->yypvt[-0].psl_dbval, &psq_cb->psq_error);

	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 122:
if (!((*yyvarspp)->bypass_actions))
/* # line 2318 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_cp13_nm_single(cb, yacc_cb->yypvt[-0].psl_strtype, &psq_cb->psq_error);

	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 123:
if (!((*yyvarspp)->bypass_actions))
/* # line 2329 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		    status;

	status = psl_ct1_create_table(cb, psq_cb, (PSS_CONS *) NULL);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 124:
if (!((*yyvarspp)->bypass_actions))
/* # line 2339 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS 		    status;

	psq_cb->psq_mode = PSQ_CREATE;
	status = psl_ct10_crt_tbl_kwd(cb, psq_cb, &yyvarsp->with_clauses, 0L);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 125:
if (!((*yyvarspp)->bypass_actions))
/* # line 2350 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_ct9_new_loc_name(cb, psq_cb, yacc_cb->yypvt[-2].psl_strtype, yacc_cb->yypvt[-0].psl_strtype, &yyvarsp->with_clauses,
				      (PSS_Q_XLATE *) NULL);

	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 126:
if (!((*yyvarspp)->bypass_actions))
/* # line 2360 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_ct9_new_loc_name(cb, psq_cb, (char *) NULL, yacc_cb->yypvt[-0].psl_strtype,
				      &yyvarsp->with_clauses, (PSS_Q_XLATE *) NULL);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 127:
if (!((*yyvarspp)->bypass_actions))
/* # line 2371 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS               status;

	/*
	** store the table name into format understood by psl_ct12_crname()
	** which is also used by the SQL crname
	*/

        STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(DB_TAB_NAME), (char *) &yyvarsp->obj_spec.pss_obj_name);
	MEfill(sizeof(DB_OWN_NAME), ' ', (PTR) &yyvarsp->obj_spec.pss_owner);
	yyvarsp->obj_spec.pss_objspec_flags = 0;
	yyvarsp->obj_spec.pss_orig_obj_name = yacc_cb->yypvt[-0].psl_strtype;

	status = psl_ct12_crname(cb, psq_cb, &yyvarsp->obj_spec);
	if (DB_FAILURE_MACRO(status))
	    return(status);

	yacc_cb->yyval.psl_strtype = yacc_cb->yypvt[-0].psl_strtype;
    } break;

case 130:
if (!((*yyvarspp)->bypass_actions))
/* # line 2397 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
        DB_STATUS               status;

        /* schang : change the last arg so that code */
        /*          can be shared by RMS GW */
	status = psl_ct13_newcolname(cb, yacc_cb->yypvt[-0].psl_strtype, psq_cb, (PSS_Q_XLATE *)0, 
					(PTR *)0);
	if (DB_FAILURE_MACRO(status))
	    return (status);
    } break;

case 131:
if (!((*yyvarspp)->bypass_actions))
/* # line 2410 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_ct14_typedesc(cb, yacc_cb->yypvt[-1].psl_strtype, 0, (i4 *) NULL, yacc_cb->yypvt[-0].psl_inttype,
				   (PST_QNODE *) NULL, (DB_TEXT_STRING *) NULL,
				   psq_cb, -1);
	if (DB_FAILURE_MACRO(status))
	{
	    return(status);
	}
    } break;

case 132:
if (!((*yyvarspp)->bypass_actions))
/* # line 2422 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS               status;
	i4			len = yacc_cb->yypvt[-2].psl_inttype;

	status = psl_ct14_typedesc(cb, yacc_cb->yypvt[-4].psl_strtype, 1, &len, yacc_cb->yypvt[-0].psl_inttype,
				   (PST_QNODE *) NULL, (DB_TEXT_STRING *) NULL,
				   psq_cb, -1);
	if (DB_FAILURE_MACRO(status))
	{
	    return(status);
	}
    } break;

case 133:
if (!((*yyvarspp)->bypass_actions))
/* # line 2435 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4			parms[2];
	DB_STATUS               status;

	parms[0] = yacc_cb->yypvt[-4].psl_inttype;
	parms[1] = yacc_cb->yypvt[-2].psl_inttype;
	status = psl_ct14_typedesc(cb, yacc_cb->yypvt[-6].psl_strtype, 2, parms, yacc_cb->yypvt[-0].psl_inttype, 
				   (PST_QNODE *) NULL, (DB_TEXT_STRING *) NULL,
				   psq_cb, -1);
	if (DB_FAILURE_MACRO(status))
	{
	    return(status);
	}
    } break;

case 134:
if (!((*yyvarspp)->bypass_actions))
/* # line 2452 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_strtype = yacc_cb->yypvt[-0].psl_strtype;
    } break;

case 135:
if (!((*yyvarspp)->bypass_actions))
/* # line 2456 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;
	char			*value;

	status = psl_ct11_tname_name_name(cb, psq_cb, yacc_cb->yypvt[-1].psl_strtype, yacc_cb->yypvt[-0].psl_strtype, 
						(char *)NULL, &value);
	if (DB_FAILURE_MACRO(status))
	    return(status);

	yacc_cb->yyval.psl_strtype = value;
    } break;

case 136:
if (!((*yyvarspp)->bypass_actions))
/* # line 2470 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	/*
	** nothing specified implies ->
	**	not null with default
	*/
	yacc_cb->yyval.psl_inttype = PSS_TYPE_NOT_NULL | PSS_TYPE_DEFAULT;
    } break;

case 137:
if (!((*yyvarspp)->bypass_actions))
/* # line 2478 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	/*
	** WITH NULL implies -->
	**	with null with default
	*/
	yacc_cb->yyval.psl_inttype = PSS_TYPE_NULL;      /* don't want PSS_TYPE_DEFAULT here
				 ** because WITH NULL implies a default of NULL
				 ** and the new PSS_TYPE_DEFAULT implies a 
				 ** default of 0 or blank depending on 
				 ** the datatype   (rjb  10/11/92)
				 */
    } break;

case 138:
if (!((*yyvarspp)->bypass_actions))
/* # line 2491 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	/*
	** NOT NULL implies -->
	**	not null with default
	*/
	yacc_cb->yyval.psl_inttype = PSS_TYPE_NOT_NULL | PSS_TYPE_DEFAULT;
    } break;

case 139:
if (!((*yyvarspp)->bypass_actions))
/* # line 2499 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4	    err_code;

	if (STcompare(yacc_cb->yypvt[-0].psl_strtype, "default") != 0)
	{
	    /* create table syntax error message */
	    (VOID) psf_error(2607L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 2, 
		sizeof cb->pss_lineno, &cb->pss_lineno,
		(i4) STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}
	else
	{
	    /*
	    ** NOT NULL WITH DEFAULT implies just that
	    */
	    yacc_cb->yyval.psl_inttype = PSS_TYPE_NOT_NULL | PSS_TYPE_DEFAULT;
	}
    } break;
	}
	return(0);
}

i4
psl3iftn(yacc_cb, yyrule, yystatusp, yyvarspp, cb, psq_cb)
YACC_CB		*yacc_cb;
i4		yyrule;
PSS_SESBLK	*cb;
DB_STATUS	*yystatusp;
YYVARS		**yyvarspp;
PSQ_CB *	psq_cb;
{
    YYVARS	*yyvarsp = *yyvarspp;
	switch(yyrule)
	{
case 140:
if (!((*yyvarspp)->bypass_actions))
/* # line 2520 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4	    err_code;

	if ((STcompare(yacc_cb->yypvt[-0].psl_strtype, "default") != 0))
	{
	    /* create table syntax error message */
	    (VOID) psf_error(2607L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 2, 
		sizeof cb->pss_lineno, &cb->pss_lineno,
		(i4) STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}
	else
	{
	    /*
	    ** NOT NULL NOT DEFAULT implies just that
	    */
	    yacc_cb->yyval.psl_inttype = PSS_TYPE_NOT_NULL | PSS_TYPE_NDEFAULT;
	}
    } break;

case 141:
if (!((*yyvarspp)->bypass_actions))
/* # line 2541 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4	    err_code;

	if ((STcompare(yacc_cb->yypvt[-0].psl_strtype, "default") != 0))
	{
	    /* create table syntax error message */
	    (VOID) psf_error(2607L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 2, 
		sizeof cb->pss_lineno, &cb->pss_lineno,
		(i4) STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}
	else
	{
	    /*
	    ** NOT DEFAULT imples -->
	    **	    not null not default
	    */
	    yacc_cb->yyval.psl_inttype = PSS_TYPE_NOT_NULL | PSS_TYPE_NDEFAULT;
	}
    } break;

case 142:
if (!((*yyvarspp)->bypass_actions))
/* # line 2565 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_ct3_crwith(cb, psq_cb->psq_mode, &yyvarsp->with_clauses,
	    &psq_cb->psq_error);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 143:
if (!((*yyvarspp)->bypass_actions))
/* # line 2574 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_ct3_crwith(cb, psq_cb->psq_mode, &yyvarsp->with_clauses,
	    &psq_cb->psq_error);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 151:
if (!((*yyvarspp)->bypass_actions))
/* # line 2598 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_ct6_cr_single_kwd(cb, yacc_cb->yypvt[-0].psl_strtype, &yyvarsp->with_clauses,
	    psq_cb->psq_mode, &psq_cb->psq_error, (PSS_Q_XLATE *) NULL);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 152:
if (!((*yyvarspp)->bypass_actions))
/* # line 2609 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
        DB_STATUS               status;

        status = psl_nm_eq_no(cb, yacc_cb->yypvt[-2].psl_strtype, yacc_cb->yypvt[-0].psl_inttype, &yyvarsp->with_clauses, psq_cb->psq_mode,
            &psq_cb->psq_error, (PSS_Q_XLATE *) NULL);

        if (DB_FAILURE_MACRO(status))
            return(status);
    } break;

case 154:
if (!((*yyvarspp)->bypass_actions))
/* # line 2625 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_lst_prefix(cb, psq_cb, yacc_cb->yypvt[-1].psl_strtype, yyvarsp);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 157:
if (!((*yyvarspp)->bypass_actions))
/* # line 2639 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;
	char			*letter;

	if (yyvarsp->list_clause != PSS_KEY_CLAUSE)
	{
	    /*
	    ** Translate nonkeywords to lower case
	    */
	    for (letter =  yacc_cb->yypvt[-0].psl_strtype; *letter != EOS; CMnext(letter))
		CMtolower(letter, letter);
	}

	status = psl_ct8_cr_lst_elem(cb, yyvarsp->list_clause, NULL, yacc_cb->yypvt[-0].psl_strtype,
	    (PST_RESKEY **) NULL, &psq_cb->psq_error, (PSS_Q_XLATE *) NULL);
	if (DB_FAILURE_MACRO(status))
	    return (status);
		    
    } break;

case 158:
if (!((*yyvarspp)->bypass_actions))
/* # line 2661 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	status;

	/* Initialize the type of updating in the query tree header */
	yacc_cb->yypvt[-2].psl_qttype->pst_updtmode = yacc_cb->yypvt[-1].psl_tytype;

	status = psl_crsopen(cb, psq_cb, yacc_cb->yypvt[-3].psl_crblk, yacc_cb->yypvt[-2].psl_qttype, yacc_cb->yypvt[-0].psl_tytype,
	    yyvarsp->updcollst, yyvarsp->nonupdt, yyvarsp->join_id, FALSE);

	if (DB_FAILURE_MACRO(status))
	{
	    return (status);
	}
    } break;

case 159:
if (!((*yyvarspp)->bypass_actions))
/* # line 2678 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;
	PSC_CURBLK	       *curblk;
	i4		       err_code;

	/* Indicate that this isn't a repeat cursor */
	psq_cb->psq_mode = PSQ_DEFCURS;
	cb->pss_defqry = 0;

	/* look for cursor by its FE id. It should not exist */
	status = psq_crffind(cb, yacc_cb->yypvt[-0].psl_cursid, &curblk, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	if (curblk != (PSC_CURBLK *) NULL)
	{
	    (VOID) psf_error(2201L, 0L, PSF_USERERR,
		&err_code, &psq_cb->psq_error, 2, sizeof(cb->pss_lineno),
		&cb->pss_lineno, DB_MAXNAME, yacc_cb->yypvt[-0].psl_cursid->db_cur_name);
	    return (E_DB_ERROR);
	}
	/* Create cursor control block */
	status = psq_crcreate(cb, yacc_cb->yypvt[-0].psl_cursid, psq_cb->psq_mode, &curblk,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	/* Hand cursor id back to caller */
	STRUCT_ASSIGN_MACRO(curblk->psc_blkid, psq_cb->psq_cursid);

	yacc_cb->yyval.psl_crblk = curblk;
    } break;

case 160:
if (!((*yyvarspp)->bypass_actions))
/* # line 2712 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	if (yyvarsp->updcollst != (PST_QNODE *) NULL)
	{
	    /* FOR UPDATE clause must have been specified.
	    ** We need to validate column names. We will also
	    ** generate VAR nodes and connect them to resdom nodes.
	    ** This mini target-list will be connected to the main one
	    ** so that qrymod executed from within cursor_open tells us
	    ** what attribute numbers in the underlying base relation(s)
	    ** these update columns map to; we need these att. nos.
	    ** for proper initialization of psc_iupdmap and consequently
	    ** pst_updmap in the tree header passed to OPF.
	    */
	    DB_STATUS		status;
	    DMT_ATT_ENTRY	*coldesc;
	    PST_QNODE		*rsdm, *newnode;
	    char		*name;
	    DB_DATA_VALUE	*dv;
	    PST_VAR_NODE	varnode;
	    i4		err_code;

	    /* deferred is the default update mode */
	    if (yacc_cb->yypvt[-1].psl_tytype == PST_UNSPECIFIED)
	    {
		yacc_cb->yypvt[-1].psl_tytype = PST_DEFER;
	    }

	    for (rsdm = yyvarsp->updcollst;
		 rsdm && rsdm->pst_sym.pst_type == PST_RESDOM;
		 rsdm = rsdm->pst_left
		)
	    {
		name = rsdm->pst_sym.pst_value.pst_s_rsdm.pst_rsname;

		/* Get the column description */
		coldesc = pst_coldesc(cb->pss_resrng, (DB_ATT_NAME *) name);

		if (coldesc == (DMT_ATT_ENTRY *) NULL)
		{
		    (VOID) psf_error(2102L, 0L, PSF_USERERR, &err_code,
			&psq_cb->psq_error, 2,
			(i4) sizeof(cb->pss_lineno), &cb->pss_lineno,
			(i4) psf_trmwhite(DB_MAXNAME, name), name);
		    return (E_DB_ERROR);
		}

		dv = &rsdm->pst_sym.pst_dataval;

		dv->db_data =   (PTR) NULL;
		dv->db_length = coldesc->att_width;
		dv->db_datatype = coldesc->att_type;
		dv->db_prec = coldesc->att_prec;

		/* Set up var node. */
		varnode.pst_vno = cb->pss_resrng->pss_rgno;
		varnode.pst_atno.db_att_id = coldesc->att_number;

		/* Create the node */
		status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
		    (PST_QNODE *) NULL, PST_VAR, (char *) &varnode,
		    sizeof(varnode), (DB_DT_ID) coldesc->att_type,
		    (i2) coldesc->att_prec, (i4) coldesc->att_width,
		    (DB_ANYTYPE *) NULL, &newnode, &psq_cb->psq_error, (i4) 0);
		if (DB_FAILURE_MACRO(status))
		    return (status);

		rsdm->pst_right = newnode;
	    }
	}
	else
	{
	    /* If no column list, FOR and UPDATE are the only
	    ** legal words in the update clause.
	    */
	    if (yacc_cb->yypvt[-1].psl_tytype != PST_UNSPECIFIED)
	    {
		i4 err_code;

		(VOID) psf_error(2200L, 0L, PSF_USERERR, &err_code,
		    &psq_cb->psq_error, 2, sizeof(cb->pss_lineno),
		    &cb->pss_lineno, psf_trmwhite(DB_MAXNAME,
		    cb->pss_crsr->psc_curid.db_cur_name),
		    cb->pss_crsr->psc_curid.db_cur_name);
		return (E_DB_ERROR);
	    }
	    else
	    {
		register i4  i;

		/* Find the possible result table. If there is more than one,
		** and the query is not 'readonly', we will catch the error 
		** later.
		*/
		for (i = 0; i < PST_NUMVARS; i++)
		{
		    if (cb->pss_usrrange.pss_rngtab[i].pss_rdrinfo
			!= (RDR_INFO *) NULL)
		    {
			cb->pss_resrng = &cb->pss_usrrange.pss_rngtab[i];
			break;
		    }
		}

		/* No explicit updates */
		yacc_cb->yypvt[-1].psl_tytype = PST_READONLY;
	    }
	}

	yacc_cb->yyval.psl_tytype = yacc_cb->yypvt[-1].psl_tytype;
    } break;

case 161:
if (!((*yyvarspp)->bypass_actions))
/* # line 2823 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4   i;

	/* Find the possible result table. If there is more 
	** than one, and the query is not 'readonly', we
	** will catch the error later.
	*/
	for (i = 0; i < PST_NUMVARS; i++)
	{
	    if (cb->pss_usrrange.pss_rngtab[i].pss_rdrinfo 
		    != (RDR_INFO *) NULL)
	    {
		cb->pss_resrng = &cb->pss_usrrange.pss_rngtab[i];
		break;
	    }
	}

	/* Update privilege has not been requested */
	yacc_cb->yyval.psl_tytype = PST_READONLY;
    } break;

case 162:
if (!((*yyvarspp)->bypass_actions))
/* # line 2846 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4	    err_code;

	if (STcompare(yacc_cb->yypvt[-0].psl_strtype, "update"))
	{
	    (VOID) psf_error(2615L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 2, 
		sizeof cb->pss_lineno, &cb->pss_lineno,
		(i4) STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);
	}

	yacc_cb->yyval.psl_tytype = PST_DEFER;
    } break;

case 163:
if (!((*yyvarspp)->bypass_actions))
/* # line 2861 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4	    err_code;

	if (STcompare(yacc_cb->yypvt[-0].psl_strtype, "update"))
	{
	    (VOID) psf_error(2615L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 2, 
		sizeof cb->pss_lineno, &cb->pss_lineno,
		(i4) STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);
	}

	yacc_cb->yyval.psl_tytype = PST_DIRECT;
    } break;

case 164:
if (!((*yyvarspp)->bypass_actions))
/* # line 2876 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_tytype = PST_UNSPECIFIED;
    } break;

case 165:
if (!((*yyvarspp)->bypass_actions))
/* # line 2882 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	/* updcollst points to the root of RESDOM tree with
	** names of columns named in the FOR UPDATE clause.
	*/
	yyvarsp->updcollst = yacc_cb->yypvt[-1].psl_trtype;
    } break;

case 166:
if (!((*yyvarspp)->bypass_actions))
/* # line 2889 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	/* No update column list.
	** $Yupdcollst is NULL.
	*/
    } break;

case 167:
if (!((*yyvarspp)->bypass_actions))
/* # line 2897 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4   i;

	/* Find the possible result table. If there is more
	** than one, and the query is not 'readonly', we 
	** will catch the error later.
	*/
	for (i = 0; i < PST_NUMVARS; i++)
	{
	    if (cb->pss_usrrange.pss_rngtab[i].pss_rdrinfo 
		    != (RDR_INFO *) NULL)
	    {
		cb->pss_resrng = &cb->pss_usrrange.pss_rngtab[i];
		break;
	    }
	}
    } break;

case 168:
if (!((*yyvarspp)->bypass_actions))
/* # line 2917 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_trtype= yacc_cb->yypvt[-0].psl_trtype;
    } break;

case 169:
if (!((*yyvarspp)->bypass_actions))
/* # line 2921 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_trtype = pst_tlprpnd(yacc_cb->yypvt[-2].psl_trtype, yacc_cb->yypvt[-0].psl_trtype);
    } break;

case 170:
if (!((*yyvarspp)->bypass_actions))
/* # line 2927 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	    status;
	PST_QNODE	    *resdomnode;
	PST_RSDM_NODE	    resdom;

	/* Normalize the attribute name */
	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(resdom.pst_rsname),
	    (char *) resdom.pst_rsname);

	resdom.pst_rsno = (i4) 1;
	resdom.pst_ntargno = (i4) 0;
	resdom.pst_ttargtype = (i4) PST_USER;
	resdom.pst_rsupdt = (i4) FALSE;
	resdom.pst_rsflags = PST_RS_PRINT;
	resdom.pst_dmuflags = 0;
	status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
	    (PST_QNODE *) NULL, PST_RESDOM, (char *) &resdom,
	    sizeof(PST_RSDM_NODE), DB_NODT, (i2) 0, (i4) 0, (DB_ANYTYPE *) NULL,
	    &resdomnode, &psq_cb->psq_error, (i4) 0);
	if (DB_FAILURE_MACRO(status))
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = resdomnode;
    } break;

case 171:
if (!((*yyvarspp)->bypass_actions))
/* # line 2956 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_tytype = TRUE;
    } break;

case 172:
if (!((*yyvarspp)->bypass_actions))
/* # line 2960 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_tytype = FALSE;
    } break;

case 174:
if (!((*yyvarspp)->bypass_actions))
/* # line 2969 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;

	psq_cb->psq_mode = PSQ_DEFREF;

	/* "define reference" is allowed only in distributed */
	if (cb->pss_distrib == DB_DSTNO)
	{
	    (VOID) psf_error(5205L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 0);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}
    } break;

case 177:
if (!((*yyvarspp)->bypass_actions))
/* # line 2989 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;

	/* The only valid parameter of this form is "pathname" */
	if (STcasecmp(yacc_cb->yypvt[-7].psl_strtype, "pathname" ))
	{
	    (VOID) psf_error(5152L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, (i4) STlength(yacc_cb->yypvt[-7].psl_strtype), yacc_cb->yypvt[-7].psl_strtype);
	    return (E_DB_ERROR);    /* Non-zero return means error */
	}
    } break;

case 178:
if (!((*yyvarspp)->bypass_actions))
/* # line 3001 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;

	/*
	** The recognized parameters of this form are "permanent and
	** "temporary
	*/
	if (!STcasecmp(yacc_cb->yypvt[-0].psl_strtype, "permanent"))
	{
	}
	else if (!STcasecmp(yacc_cb->yypvt[-0].psl_strtype, "temporary"))
	{
	}
	else
	{
	    (VOID) psf_error(5152L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}
    } break;

case 179:
if (!((*yyvarspp)->bypass_actions))
/* # line 3024 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	status;

	/* Initialize the type of updating in the query tree header */
	yacc_cb->yypvt[-2].psl_qttype->pst_updtmode = yacc_cb->yypvt[-1].psl_tytype;

	status = psl_crsopen(cb, psq_cb, yacc_cb->yypvt[-3].psl_crblk, yacc_cb->yypvt[-2].psl_qttype, yacc_cb->yypvt[-0].psl_tytype,
	    yyvarsp->updcollst, yyvarsp->nonupdt, yyvarsp->join_id, FALSE);

	if (DB_FAILURE_MACRO(status))
	{
	    return (status);
	}
    } break;

case 180:
if (!((*yyvarspp)->bypass_actions))
/* # line 3041 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;
	PSC_CURBLK	       *curblk;
	i4		       err_code;

	psq_cb->psq_mode = PSQ_DEFCURS;
	cb->pss_defqry = PSQ_DEFCURS;

	/* Look for FE version of cursor control block.
	** If it doesn't exist, error. The cursor may be opened.
	** The user should try the exec statement first.
	*/
	status = psq_crffind(cb, yacc_cb->yypvt[-0].psl_cursid, &curblk, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	if (curblk != (PSC_CURBLK *) NULL)
	{
	    (VOID) psf_error(2210L, 0L, PSF_USERERR,
		&err_code, &psq_cb->psq_error, 2, sizeof(cb->pss_lineno),
		&cb->pss_lineno, DB_MAXNAME, yacc_cb->yypvt[-0].psl_cursid->db_cur_name);
	    return (E_DB_ERROR);
	}
	status = psq_crcreate(cb, yacc_cb->yypvt[-0].psl_cursid, psq_cb->psq_mode, &curblk,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	/* Hand cursor id back to caller */
	STRUCT_ASSIGN_MACRO(curblk->psc_blkid, psq_cb->psq_cursid);

	yacc_cb->yyval.psl_crblk = curblk;
    } break;

case 181:
if (!((*yyvarspp)->bypass_actions))
/* # line 3080 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;
	i4			err_code;
	DMC_CB			*dmc_cb;
	DMC_LOC_ENTRY		*loc_entry;
	char			*syn_error;

	if (STcompare(yacc_cb->yypvt[-6].psl_strtype, "type"))
	{
	    syn_error = yacc_cb->yypvt[-6].psl_strtype;
	}
	else if (STcompare(yacc_cb->yypvt[-2].psl_strtype, "area"))
	{
	    syn_error = yacc_cb->yypvt[-2].psl_strtype;
	}
	else
	{
	    syn_error = (char *) NULL;
	}

	if (syn_error)
	{
	    (VOID) psf_error(2683L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 2, 
		sizeof cb->pss_lineno, &cb->pss_lineno,
		(i4) STlength(syn_error), syn_error);
	    return (E_DB_ERROR);
	}
	/* check for legal location types
	** DATA is the only legal value
	*/
	if (STcompare(yacc_cb->yypvt[-4].psl_strtype, "data"))
	{
	    (VOID) psf_error(2718L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 2, 
		sizeof cb->pss_lineno, &cb->pss_lineno,
		(i4) STlength(yacc_cb->yypvt[-4].psl_strtype), yacc_cb->yypvt[-4].psl_strtype);
	    return (E_DB_ERROR);
	}		

	/* check for legal location name.
	** Just check length.
	*/
	if (STlength(yacc_cb->yypvt[-0].psl_strtype) > sizeof (loc_entry->loc_extent))
	{
	    (VOID) psf_error(2719L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 2, 
		sizeof cb->pss_lineno, &cb->pss_lineno,
		(i4) STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);
	}

	/* Create control block for DMC_ALTER call for define location */
	status = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(DMC_CB), (PTR *) &dmc_cb,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	status = psf_malloc(cb, &cb->pss_ostream, sizeof(DMC_LOC_ENTRY), 
	    (PTR *) &loc_entry, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	status = psf_mroot(cb, &cb->pss_ostream, (PTR) dmc_cb, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	cb->pss_object	    = (PTR) dmc_cb;
	dmc_cb->type	    = DMC_CONTROL_CB;
	dmc_cb->length	    = sizeof (DMC_CB);
	dmc_cb->dmc_op_type = DMC_DATABASE_OP;
	dmc_cb->dmc_session_id	= (PTR)cb->pss_sessid;
	dmc_cb->dmc_flags_mask = DMC_ALOCATION;
	dmc_cb->dmc_db_id   = cb->pss_dbid;
	dmc_cb->dmc_db_access_mode = 0;
	dmc_cb->dmc_lock_mode	= 0;
	dmc_cb->dmc_db_location.data_address = (char*) loc_entry;
	dmc_cb->dmc_db_location.data_in_size = sizeof (*loc_entry);
	loc_entry->loc_flags = LOC_DATA;
	STmove(yacc_cb->yypvt[-8].psl_strtype, ' ', sizeof(DB_LOC_NAME), (char *) &loc_entry->loc_name);
	loc_entry->loc_l_extent = STlength(yacc_cb->yypvt[-0].psl_strtype);
	MEcopy((PTR) yacc_cb->yypvt[-0].psl_strtype, loc_entry->loc_l_extent, 
	    (PTR) loc_entry->loc_extent);
    } break;

case 182:
if (!((*yyvarspp)->bypass_actions))
/* # line 3170 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	psq_cb->psq_mode = PSQ_DEFLOC;
    } break;
	}
	return(0);
}

i4
psl4iftn(yacc_cb, yyrule, yystatusp, yyvarspp, cb, psq_cb)
YACC_CB		*yacc_cb;
i4		yyrule;
PSS_SESBLK	*cb;
DB_STATUS	*yystatusp;
YYVARS		**yyvarspp;
PSQ_CB *	psq_cb;
{
    YYVARS	*yyvarsp = *yyvarspp;
	switch(yyrule)
	{
case 183:
if (!((*yyvarspp)->bypass_actions))
/* # line 3176 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_QNODE              *tidnode;
	PST_VAR_NODE	       tidvar;
	PST_QNODE	       *rsdmnode;
	PST_RSDM_NODE	       resdom;
	PST_QNODE	       *rootnode;
	PST_RT_NODE	       root;
	PST_QTREE	       *tree;
	DB_STATUS	       status;
	i4		       err_code;
	i4		       mask;
	i4		       mask2;
	i4		       qrymod_resp_mask;
	PST_PROCEDURE	       *pnode;

	/* Set up a var. node representing the tid */
	tidvar.pst_vno = yacc_cb->yypvt[-1].psl_rngtype->pss_rgno;
	tidvar.pst_atno.db_att_id = 0;	    /* tid is always column 0 */

	status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
	    (PST_QNODE *) NULL, PST_VAR, (char *) &tidvar, sizeof(tidvar),
	    DB_TID_TYPE, (i2) 0, (i4) DB_TID8_LENGTH, (DB_ANYTYPE *) NULL,
	    &tidnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);	    /* non-zero return means error */
	}

	/* Set up a resdom result domain representing a tid. */
	resdom.pst_rsno = cb->pss_rsdmno++; /* Count resdoms */
	resdom.pst_ntargno = resdom.pst_rsno;
	resdom.pst_ttargtype = PST_ATTNO;
	resdom.pst_rsupdt = FALSE;
	resdom.pst_rsflags = 0; /* no PST_RS_PRINT */

	status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL, 
	    tidnode, PST_RESDOM, (PTR) &resdom, sizeof(resdom), DB_TID_TYPE,
	    (i2) 0, (i4) DB_TID8_LENGTH, (DB_ANYTYPE *) NULL, &rsdmnode,
	    &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);	/* non-zero return means error */
	}

	/* Make the root node */
	root.pst_rtuser = TRUE;
	root.pst_union.pst_next  = 0;
	root.pst_dups   = PST_ALLDUPS;

	status = pst_node(cb, &cb->pss_ostream, rsdmnode, yacc_cb->yypvt[-0].psl_trtype, PST_ROOT,
	    (PTR) &root, sizeof(PST_RT_NODE), DB_NODT, (i2) 0, (i4) 0,
	    (DB_ANYTYPE *) NULL, &rootnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);	/* non-zero return means error */
	}

	/* Remember the result variable */
	cb->pss_resrng = yacc_cb->yypvt[-1].psl_rngtype;

	if (cb->pss_defqry == PSQ_DEFQRY)
	{
	    /*
	    ** now call psl_shareable() to determine if the query satisfies SQL
	    ** definition of shareability (i.e. all tables used in the query are
	    ** catalogs or are owned by the DBA; QUEL user cannot explicitly
	    ** qualify them with an owner name)
	    */
	    status = psl_shareable(psq_cb, cb, &yyvarsp->qp_shareable, &yyvarsp->shr_qryinfo);
	    if (DB_FAILURE_MACRO(status))
		return(status);
	}

	/* Apply qrymod algorithm (views, permits, integrities) */
	status = psy_qrymod(rootnode, cb, psq_cb, &yyvarsp->join_id,
	    &qrymod_resp_mask);
	if (status != E_DB_OK)
	    return (status);

	/*
	** if this is a repeat query and it is shareable according to SQL
	** definition of shareability, apply QUEL-specific checking
	*/
	if (cb->pss_defqry == PSQ_DEFQRY && yyvarsp->qp_shareable)
	{
	    psl_quel_shareable(cb, yyvarsp->qry_mask, &yyvarsp->qp_shareable);
	}

	mask = cb->pss_resrng->pss_tabdesc->tbl_status_mask;
	mask2 = cb->pss_resrng->pss_tabdesc->tbl_2_status_mask;

	/* Make sure base table is updateable;
	** In case of an index or a non-extended catalog,
	** catalog update privilege must be on.
	** In case of security catalog session must have
	** security privilege active.
	** Bug 34079, since 6.0 you were never supposed to be 
        ** able to update a secondary index.  Fixed it so 
        ** the same error is returned whether you have update 
        ** syscat or not (jennifer)
	**
	** if PSS_REPAIR_SYSCAT and PSS_CATUPD are set in cb->pss_ses_flag,
	** allow DELETE on indexes which are non-extended catalogs
	*/
	if (mask & DMT_IDX)
	{
	    if (!(   mask & DMT_CATALOG && ~mask & DMT_EXTENDED_CAT
	          && cb->pss_ses_flag & PSS_CATUPD
		  && cb->pss_ses_flag & PSS_REPAIR_SYSCAT
		 )
	       )
	    {
		_VOID_ psf_error(2106L, 0L, PSF_USERERR, &err_code,
		    &psq_cb->psq_error, 1,
		    psf_trmwhite(sizeof(DB_TAB_NAME),
			(char *) &cb->pss_resrng->pss_tabname),
		    &cb->pss_resrng->pss_tabname);
		return(E_DB_ERROR);
	    }
	}
	else if ( ( (mask & DMT_CATALOG || mask2 & DMT_TEXTENSION)
		 && ~mask & DMT_EXTENDED_CAT
		 && !(cb->pss_ses_flag & PSS_CATUPD))
	      ||
	     	((mask & DMT_SECURE) &&
	       !(cb->pss_ustat & DU_USECURITY)))
	{
		i4 local_status;
		DB_ERROR	    e_error;

		if ( Psf_srvblk->psf_capabilities & PSF_C_C2SECURE )
		{
		    local_status = psy_secaudit(FALSE, cb,
			    (char *)&cb->pss_resrng->pss_tabname,
			    &cb->pss_resrng->pss_tabdesc->tbl_owner,
			    sizeof(DB_TAB_NAME), 
			    SXF_E_TABLE,
			    I_SX2020_PROT_REJECT, 
			    SXF_A_FAIL|SXF_A_DELETE, 
			    &e_error);

		    if (local_status > status)
			status = local_status;
		}

		/* process security_alarms */
		local_status = psy_do_alarm_fail(cb, 
				cb->pss_resrng, PSQ_DELETE, &e_error);
		if (local_status > status)
		    status = local_status;
	    _VOID_ psf_error(2107L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 2, sizeof(cb->pss_lineno), &cb->pss_lineno,
		psf_trmwhite(sizeof(DB_TAB_NAME), 
		    (char *) &cb->pss_resrng->pss_tabname),
		&cb->pss_resrng->pss_tabname);
	    return (E_DB_ERROR);
	}

	/* Make the header */
	status = pst_header(cb, psq_cb, PST_UNSPECIFIED, (PST_QNODE *) NULL,
	    rootnode, &tree, &pnode, PST_0FULL_HEADER, (PSS_Q_XLATE *) NULL);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	/* set pst_numjoins.  $Yjoin_id contains the highest join id */
	tree->pst_numjoins = yyvarsp->join_id;

	/* Fix the root in QSF */
	status = psf_mroot(cb, &cb->pss_ostream, (PTR) pnode, &psq_cb->psq_error);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_qttype = tree;
    } break;

case 184:
if (!((*yyvarspp)->bypass_actions))
/* # line 3357 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;

	/* Set query mode */
	psq_cb->psq_mode = PSQ_DELETE;

	/* Open memory stream for allocating query tree */
	if ((status = psf_mopen(cb, QSO_QTREE_OBJ, &cb->pss_ostream,
	    &psq_cb->psq_error)) != E_DB_OK)
	{
	    return (status);    /* non-zero return means error */
	}
    } break;

case 185:
if (!((*yyvarspp)->bypass_actions))
/* # line 3373 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_rngtype = yacc_cb->yypvt[-0].psl_rngtype;
    } break;

case 186:
if (!((*yyvarspp)->bypass_actions))
/* # line 3377 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4		    rngvar_info;
	i4		    err_code;
	DB_STATUS           status;
	PSS_RNGTAB	    *rngtab;
	DB_TAB_NAME	    tabname;
	DB_OWN_NAME	    sessname;

	if (STcasecmp(yacc_cb->yypvt[-2].psl_strtype, "session"))
	{
	    (VOID) psf_error(5153, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1,
		psf_trmwhite(sizeof(DB_OWN_NAME), 
		    (char *) yacc_cb->yypvt[-2].psl_strtype), yacc_cb->yypvt[-2].psl_strtype);
	    return (E_DB_ERROR);
	}
	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(tabname), (char *) &tabname);
	STRUCT_ASSIGN_MACRO(cb->pss_sess_owner, sessname);
	status = psl_orngent(&cb->pss_usrrange, -1, yacc_cb->yypvt[-2].psl_strtype, &sessname, &tabname, 
	    cb, FALSE, &rngtab, psq_cb->psq_mode, &psq_cb->psq_error,
	    &rngvar_info);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_rngtype = rngtab;
    } break;

case 187:
if (!((*yyvarspp)->bypass_actions))
/* # line 3408 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSY_CB                 *psy_cb;
	DB_STATUS	       status;
	DB_TAB_NAME	       tabname;
	PSS_RNGTAB	       *resrange;
	DB_TAB_NAME	       *nameplace;
	DB_TAB_ID	       *idplace;
	i4			err_code;
	i4			mask;
	i4			mask2;
	i4			err_num = 0L;
	i4			rngvar_info;

	psy_cb = (PSY_CB *) cb->pss_object;

	/* -1 means "all" has been specified, so change to 0 */
	if (psy_cb->psy_numnms == -1)
	    psy_cb->psy_numnms = 0;

	/* Figure out where table name and id go */
	idplace = &psy_cb->psy_tables[psy_cb->psy_numtabs];
	nameplace = &psy_cb->psy_tabname[psy_cb->psy_numtabs++];

	/* Find table id for this table name */
	STmove(yacc_cb->yypvt[-1].psl_strtype, ' ', sizeof(DB_TAB_NAME), (char *) &tabname);
	status = psl_rngent(&cb->pss_auxrng, -1, "", &tabname, cb,
	    FALSE, &resrange, psq_cb->psq_mode, &psq_cb->psq_error,
	    &rngvar_info);
	if (status != E_DB_OK)
	    return (status);

	mask = resrange->pss_tabdesc->tbl_status_mask;
	mask2 = resrange->pss_tabdesc->tbl_2_status_mask;

	if (mask & (DMT_CATALOG | DMT_EXTENDED_CAT)
	    || mask2 & DMT_TEXTENSION)
	{
	    /* If not an extended catalog and no catalog update
	    ** permission - error.
	    */
	    if (~mask & DMT_EXTENDED_CAT
		&& !(cb->pss_ses_flag & PSS_CATUPD))
	    {
		err_num = 5201L;
	    }
	}
	else if (MEcmp((PTR) &resrange->pss_ownname, 
		(PTR) &cb->pss_user, sizeof(DB_OWN_NAME)))
	{
	    err_num = 5202L;
	}

	if (err_num != 0L)
	{
	    /* Must audit DESTROY INTEGRITY here. */
	    if ( Psf_srvblk->psf_capabilities & PSF_C_C2SECURE )
	    {
		DB_STATUS		local_status;
		DB_ERROR		e_error;

		local_status = psy_secaudit(FALSE, cb,
			    (char *)&resrange->pss_tabdesc->tbl_name,
			    &resrange->pss_tabdesc->tbl_owner,
			    sizeof(DB_TAB_NAME), SXF_E_TABLE,
			    I_SX2036_INTEGRITY_DROP, SXF_A_FAIL | SXF_A_CONTROL,
			    &e_error);
	    }

	    (VOID) psf_error(err_num, 0L, PSF_USERERR,&err_code, 
		&psq_cb->psq_error, 1,
		psf_trmwhite(sizeof(DB_TAB_NAME), 
		    (char *) &resrange->pss_tabname),
		&resrange->pss_tabname);
	    return (E_DB_ERROR);
	}

	/* Put it in the control block, along with the table name */
	STRUCT_ASSIGN_MACRO(resrange->pss_tabid, *idplace);
	STRUCT_ASSIGN_MACRO(resrange->pss_tabname, *nameplace);
    } break;

case 188:
if (!((*yyvarspp)->bypass_actions))
/* # line 3491 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	       status;

	psq_cb->psq_mode = PSQ_DSTINTEG;

	/* Allocate a PSY_CB for destroy integrity */
	status = psy_qminit(cb, psq_cb, &cb->pss_ostream);
	if (status != E_DB_OK)
	    return (status);
    } break;

case 191:
if (!((*yyvarspp)->bypass_actions))
/* # line 3506 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSY_CB                 *psy_cb;

	/*
	** If "all" is specified, set the number of numbers to -1.  It will
	** be set to 0 later.  This way, if the user specifies some numbers
	** along with "all", they will all be destroyed, instead of just
	** the ones after the "all".
	*/
	psy_cb = (PSY_CB *) cb->pss_object;
	psy_cb->psy_numnms = -1;
    } break;

case 192:
if (!((*yyvarspp)->bypass_actions))
/* # line 3521 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSY_CB		       *psy_cb;
	i4		       err_code;

	psy_cb = (PSY_CB *) cb->pss_object;

	/* -1 means "all" has been specified */
	if (psy_cb->psy_numnms != -1)
	{
	    if (++psy_cb->psy_numnms > PSY_MAXTABS)
	    {
		(VOID) psf_error(2113L, 0L, PSF_USERERR, &err_code,
		    &psq_cb->psq_error, 0);
		return (E_DB_ERROR);
	    }
	    psy_cb->psy_numbs[psy_cb->psy_numnms - 1] = yacc_cb->yypvt[-0].psl_inttype;
	}
    } break;

case 193:
if (!((*yyvarspp)->bypass_actions))
/* # line 3542 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSY_CB              *psy_cb;
	DB_STATUS	    status;
	DB_TAB_NAME	    tabname;
	PSS_RNGTAB	    *resrange;
	DB_TAB_NAME	    *nameplace;
	DB_TAB_ID	    *idplace;
	i4		    err_code;
	i4		    mask;
	i4		    mask2;
	i4		    err_num = 0L;
	i4		    rngvar_info;

	psy_cb = (PSY_CB *) cb->pss_object;

	/* destroying permit on a table */
	psy_cb->psy_grant = PSY_TDROP;

	/* -1 means "all" has been specified, so change to 0 */
	if (psy_cb->psy_numnms == -1)
	    psy_cb->psy_numnms = 0;

	/* Figure out where table name and id go */
	idplace = &psy_cb->psy_tables[psy_cb->psy_numtabs];
	nameplace = &psy_cb->psy_tabname[psy_cb->psy_numtabs++];

	/* Find table id for this table name */
	STmove(yacc_cb->yypvt[-1].psl_strtype, ' ', sizeof(DB_TAB_NAME), (char *) &tabname);
	status = psl_rngent(&cb->pss_auxrng, -1, "", &tabname, cb,
	    FALSE, &resrange, psq_cb->psq_mode, &psq_cb->psq_error,
	    &rngvar_info);
	if (status != E_DB_OK)
	    return (status);

	mask = resrange->pss_tabdesc->tbl_status_mask;
	mask2 = resrange->pss_tabdesc->tbl_2_status_mask;
	
	/* Catalog update privilege alone is enough to manipulate
	** catalogs.
	*/
	if (!(mask & (DMT_CATALOG | DMT_EXTENDED_CAT) && 
	     (cb->pss_ses_flag & PSS_CATUPD)))
	{
	    /* Only the dba is allowed to revoke a permit */
	    if (MEcmp((PTR) &cb->pss_user, (PTR) &cb->pss_dba, 
		sizeof(DB_OWN_NAME)))
	    {
		if ( Psf_srvblk->psf_capabilities & PSF_C_C2SECURE )
		{
		    DB_STATUS	    local_status;
		    DB_ERROR	    e_error;

		    /* Must audit DESTROY PERMIT here. */

		    local_status = psy_secaudit(FALSE, cb,
			    (char *)&resrange->pss_tabdesc->tbl_name,
			    &resrange->pss_tabdesc->tbl_owner,
			    sizeof(DB_TAB_NAME), SXF_E_TABLE,
			    I_SX2018_PROT_TAB_DROP, SXF_A_FAIL | SXF_A_CONTROL,
			    &e_error);
		}

		(VOID) psf_error(5210L, 0L, PSF_USERERR,
		    &err_code, &psq_cb->psq_error, 0);
		return (E_DB_ERROR);
	    }
	}

	if (mask & (DMT_CATALOG | DMT_EXTENDED_CAT)
	    || mask2 & DMT_TEXTENSION)
	{
	    /* If not an extended catalog and no catalog update
	    ** permission - error.
	    */
	    if (~mask & DMT_EXTENDED_CAT && !(cb->pss_ses_flag & PSS_CATUPD))
	    {
		err_num = 5201L;
	    }
	}
	else if (MEcmp((PTR) &resrange->pss_ownname, 
		(PTR) &cb->pss_user, sizeof(DB_OWN_NAME)))
	{
	    err_num = 5202L;
	}

	if (err_num != 0L)
	{
	    if ( Psf_srvblk->psf_capabilities & PSF_C_C2SECURE )
	    {
		DB_STATUS	    local_status;
		DB_ERROR	    e_error;

		/* Must audit DESTROY PERMIT here. */

		local_status = psy_secaudit(FALSE, cb,
			    (char *)&resrange->pss_tabdesc->tbl_name,
			    &resrange->pss_tabdesc->tbl_owner,
			    sizeof(DB_TAB_NAME), SXF_E_TABLE,
			    I_SX2018_PROT_TAB_DROP, SXF_A_FAIL | SXF_A_CONTROL,
			    &e_error);
	    }

	    (VOID) psf_error(err_num, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1,
		psf_trmwhite(sizeof(DB_TAB_NAME), 
		    (char *) &resrange->pss_tabname),
		&resrange->pss_tabname);
	    return (E_DB_ERROR);
	}

	/* Put it in the control block, along with the table name */
	STRUCT_ASSIGN_MACRO(resrange->pss_tabid, *idplace);
	STRUCT_ASSIGN_MACRO(resrange->pss_tabname, *nameplace);
    } break;

case 194:
if (!((*yyvarspp)->bypass_actions))
/* # line 3659 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	       status;

	psq_cb->psq_mode = PSQ_DSTPERM;

	/* Allocate a PSY_CB for destroy permit */
	status = psy_qminit(cb, psq_cb, &cb->pss_ostream);
	if (status != E_DB_OK)
	    return (status);
    } break;

case 195:
if (!((*yyvarspp)->bypass_actions))
/* # line 3672 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	psq_cb->psq_mode = PSQ_DSTREF;
    } break;

case 196:
if (!((*yyvarspp)->bypass_actions))
/* # line 3678 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSY_CB		*psy_cb;

	psy_cb = (PSY_CB *) cb->pss_object;
	if (psy_cb->psy_numtabs == 0)
	    return (E_DB_ERROR);
    } break;

case 197:
if (!((*yyvarspp)->bypass_actions))
/* # line 3688 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	       status;

	psq_cb->psq_mode = PSQ_DESTROY;

	/* Create a PSY_CB to do the destruction of tables and views */
	status = psy_qminit(cb, psq_cb, &cb->pss_ostream);
	if (status != E_DB_OK)
	    return (status);
    } break;

case 200:
if (!((*yyvarspp)->bypass_actions))
/* # line 3705 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSY_CB                 *psy_cb;
	i4		       err_code;
	DB_STATUS	       status;
	DB_TAB_NAME	       tabname;
	PSS_RNGTAB	       *resrange;
	DB_TAB_NAME	       *nameplace;
	DB_TAB_ID	       *idplace;
	i4			rngvar_info;

	psy_cb = (PSY_CB *) cb->pss_object;

	/* Figure out where table name and id go */
	idplace = &psy_cb->psy_tables[psy_cb->psy_numtabs];
	nameplace = &psy_cb->psy_tabname[psy_cb->psy_numtabs];

	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(DB_TAB_NAME), (char *) &tabname);

	status = psl0_rngent(&cb->pss_auxrng, -1, "", &tabname, cb,
	    TRUE, &resrange, psq_cb->psq_mode, &psq_cb->psq_error,
	    PSS_USRTBL | PSS_DBATBL | PSS_INGTBL, &rngvar_info, 0);
	if (DB_FAILURE_MACRO(status))
	{
	    return (status);
	}

	if (!resrange)
	{
	    (VOID) psf_error(5202L, 0L, PSF_USERERR,&err_code,
		&psq_cb->psq_error, 1, STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	}
	/*
	** DESTROY synonym_name will be disallowed
	*/
	else if (rngvar_info & PSS_BY_SYNONYM)
	{
	    (VOID) psf_error(E_PS0452_CANT_DROP_SYN, 0L,
		PSF_USERERR, &err_code, &psq_cb->psq_error, 1, 
		STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	}
	else if (resrange->pss_tabdesc->tbl_2_status_mask & DMT_READONLY)
	{
	    (VOID) psf_error( E_US14E8_5352_DROP_READONLY, 0L, PSF_USERERR,
			      &err_code, &psq_cb->psq_error, 2,
			      sizeof( "DESTROY" ), "DESTROY",
			      psf_trmwhite(sizeof(DB_TAB_NAME),
			      tabname.db_tab_name),
			      tabname);
	}
	else
	{
	    i4	err_num = 0L;

	    /* Make sure base table isn't a system table */
	    if (resrange->pss_tabdesc->tbl_status_mask & DMT_CATALOG
		|| resrange->pss_tabdesc->tbl_2_status_mask & DMT_TEXTENSION)
	    {
	        /*							    
		** if object is a core catalog or user can't update	
		** catalogs
		*/
		if (  (!(cb->pss_ses_flag & PSS_CATUPD)) ||
    	              (resrange->pss_tabdesc->tbl_status_mask &
		       DMT_CONCURRENCY)
		   ) 
		{
		    err_num = 5201L;
		}
	    }
	    /*								    
	    ** Object is not a catalog (i.e. it is a table) and is
	    ** not owned by the user
	    */
	    else if (MEcmp((PTR) &resrange->pss_ownname, 
		    (PTR) &cb->pss_user, sizeof(DB_OWN_NAME)))
	    {
		err_num = 5202L;
	    }

	    if (err_num != 0L)
	    {
		if ( Psf_srvblk->psf_capabilities & PSF_C_C2SECURE )
		{
		    DB_STATUS		local_status;
		    DB_ERROR		e_error;
		    i4			msg_id;
		    i4		        accessmask = SXF_A_FAIL;
		    i4			auditevent;
		    
		    /* Must audit DROP Failure. */

		    if (resrange->pss_tabdesc->tbl_status_mask & DMT_VIEW)
		    {
			accessmask |= SXF_A_DROP;
			auditevent = SXF_E_VIEW;
			msg_id     = I_SX2015_VIEW_DROP;
		    }
		    else if (resrange->pss_tabdesc->tbl_status_mask & DMT_IDX)
		    {
			accessmask |= SXF_A_INDEX;
			auditevent = SXF_E_TABLE;
			msg_id = I_SX2010_INDEX_DROP;
		    }
		    else
		    {
			accessmask |= SXF_A_DROP;
			auditevent = SXF_E_TABLE;
			msg_id     = I_SX2025_TABLE_DROP;
		    }

		    local_status = psy_secaudit(FALSE, cb,
			    (char *)&resrange->pss_tabdesc->tbl_name,
			    &resrange->pss_tabdesc->tbl_owner,
			    sizeof(DB_TAB_NAME), auditevent,
			    msg_id, accessmask, 
			    &e_error);
		}

		(VOID) psf_error(err_num, 0L, PSF_USERERR, &err_code,
		    &psq_cb->psq_error, 1,
		    psf_trmwhite(sizeof(DB_TAB_NAME), 
			(char *) &resrange->pss_tabname),
		    &resrange->pss_tabname);
	    }
	    /*
	    ** found no errors; will add table to the list of tables to be
	    ** dropped
	    */
	    else		
	    {
		/* if dropping a view, set a flag for psy_kview() */
		psy_cb->psy_obj_mask[psy_cb->psy_numtabs++] =
		    (resrange->pss_tabdesc->tbl_status_mask & DMT_VIEW)
								? PSY_IS_VIEW
								: 0;
		cb->pss_resrng = resrange;

		/* Check for too many tables to fit into control block */
		if (psy_cb->psy_numtabs > PSY_MAXTABS)
		{
		    (VOID) psf_error(2113L, 0L, PSF_USERERR, &err_code,
			&psq_cb->psq_error, 0);
		    return (E_DB_ERROR);
		}

		/* Put it in the control block, along with the table name */
		STRUCT_ASSIGN_MACRO(resrange->pss_tabid, *idplace);
		STRUCT_ASSIGN_MACRO(resrange->pss_tabname, *nameplace);
	    }
	}
    } break;

case 201:
if (!((*yyvarspp)->bypass_actions))
/* # line 3859 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	QEF_RCB                *qef_rcb;
	DB_STATUS	       status;

	psq_cb->psq_mode = PSQ_ENDTRANS;

	/* Allocate the QEF control block for commit */
	status = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(QEF_RCB), &cb->pss_object,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_mroot(cb, &cb->pss_ostream, cb->pss_object,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	/* Fill in the QEF control block */
    	qef_rcb = (QEF_RCB *) cb->pss_object;
	qef_rcb->qef_length = sizeof(QEF_RCB);
	qef_rcb->qef_type = QEFRCB_CB;
	qef_rcb->qef_owner = (PTR)DB_PSF_ID;
	qef_rcb->qef_ascii_id = QEFRCB_ASCII_ID;
	qef_rcb->qef_sess_id = cb->pss_sessid;
    } break;

case 204:
if (!((*yyvarspp)->bypass_actions))
/* # line 3893 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{

	DB_STATUS	       status;

	QEF_RCB		       *qef_rcb;

	psq_cb->psq_mode = PSQ_EXECQRY;

	/* Hand the query name back to the caller */
	psq_cb->psq_cursid.db_cursor_id[0] = cb->pss_psessid;
	psq_cb->psq_cursid.db_cursor_id[1] = 0;
	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', DB_MAXNAME, psq_cb->psq_cursid.db_cur_name);

	/* Allocate the QEF control block for execute cursor */
	status = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(QEF_RCB), &cb->pss_object,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_mroot(cb, &cb->pss_ostream, cb->pss_object,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	/* Fill in the QEF control block */
    	qef_rcb = (QEF_RCB *) cb->pss_object;
	qef_rcb->qef_length = sizeof(QEF_RCB);
	qef_rcb->qef_type = QEFRCB_CB;
	qef_rcb->qef_owner = (PTR)DB_PSF_ID;
	qef_rcb->qef_ascii_id = QEFRCB_ASCII_ID;
	qef_rcb->qef_sess_id = cb->pss_sessid;
	qef_rcb->qef_modifier = 0;

	/* set the query id in the qef_rcb */
	qef_rcb->qef_qp.db_cursor_id[0] = cb->pss_psessid;
	qef_rcb->qef_qp.db_cursor_id[1] = 0;
	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', DB_MAXNAME, qef_rcb->qef_qp.db_cur_name);

	/* Allocate the input data area */
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(QEF_PARAM),
	    (PTR *) &qef_rcb->qef_param, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	qef_rcb->qef_param->dt_next = (QEF_PARAM *) NULL;
	/* start numbering parameters at 1 */
	qef_rcb->qef_pcount = 0;
	qef_rcb->qef_param->dt_size = 1;
	/* room for 255 parameters */
	status = psf_malloc(cb, &cb->pss_ostream, 256 * sizeof (PTR),
	    (PTR *) &qef_rcb->qef_param->dt_data, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	/*
	** To make things easier, we will cheat, and use one of the
	** fields in qef_rcb to keep track of the number of bytes allocated
	** for the parameter buffer.
	*/
	qef_rcb->qef_rowcount = 256;
    } break;

case 207:
if (!((*yyvarspp)->bypass_actions))
/* # line 3962 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	QEF_RCB                *qef_rcb;
	char		       *bufptr;
	DB_STATUS	       status;
	DB_DATA_VALUE	       *dbval;

	qef_rcb = (QEF_RCB *) cb->pss_object;
	dbval	= yacc_cb->yypvt[-0].psl_dbval;
	/*
	** If there's not enough room in the current buffer, allocate one
	** that's twice as big.
	*/
	if (qef_rcb->qef_param->dt_size >= qef_rcb->qef_rowcount)
	{
	    status = psf_malloc(cb, &cb->pss_ostream, (i4) qef_rcb->qef_rowcount,
		(PTR *) &bufptr, &psq_cb->psq_error);
	    if (status != E_DB_OK)
		return (status);
	    MEcopy((char *) qef_rcb->qef_param->dt_data, 
		qef_rcb->qef_rowcount, bufptr);
	    qef_rcb->qef_param->dt_data = (PTR*) bufptr;
	    qef_rcb->qef_rowcount *= 2;
	}

	qef_rcb->qef_param->dt_data[qef_rcb->qef_param->dt_size++] 
						    = (PTR) dbval->db_data;
	/* increment number of parameters */
	qef_rcb->qef_pcount++;
    } break;

case 208:
if (!((*yyvarspp)->bypass_actions))
/* # line 3994 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_ci1_create_index(cb, &yyvarsp->with_clauses, yyvarsp->unique,
					&psq_cb->psq_error);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 209:
if (!((*yyvarspp)->bypass_actions))
/* # line 4005 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	/*
	** store the table name into format understood by psl_ci3_indexrel()
	** which is also used by the SQL indexrel:
	*/

        STmove(yacc_cb->yypvt[-2].psl_strtype, ' ', sizeof(DB_TAB_NAME), (char *) &yyvarsp->obj_spec.pss_obj_name);
	MEfill(sizeof(DB_OWN_NAME), ' ', (PTR) &yyvarsp->obj_spec.pss_owner);
	yyvarsp->obj_spec.pss_objspec_flags = 0;
	yyvarsp->obj_spec.pss_orig_obj_name = yacc_cb->yypvt[-2].psl_strtype;

	status = psl_ci3_indexrel(cb, &yyvarsp->obj_spec, &psq_cb->psq_error,
	    psq_cb->psq_mode);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 210:
if (!((*yyvarspp)->bypass_actions))
/* # line 4026 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

        /*
	** schang : need to set psq_cb->psq_mode to PSQ_INDEX to share code with
	** RMSGW
	*/ 
        psq_cb->psq_mode = PSQ_INDEX;
	cb->pss_object = (PTR) 0;
	cb->pss_save_qeucb = (PTR) 0;

	status = psl_ci2_index_prefix(cb, psq_cb, &yyvarsp->with_clauses, yacc_cb->yypvt[-0].psl_tytype);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 211:
if (!((*yyvarspp)->bypass_actions))
/* # line 4044 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_tytype = TRUE;
	yyvarsp->unique = TRUE;
    } break;

case 212:
if (!((*yyvarspp)->bypass_actions))
/* # line 4049 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_tytype = FALSE;
	yyvarsp->unique = FALSE;
    } break;

case 215:
if (!((*yyvarspp)->bypass_actions))
/* # line 4060 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	/*
	** store the index name into format understood by psl_ci5_indexlocname()
	** which is also used by the SQL indexlocname:
	*/

        STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(DB_TAB_NAME), (char *) &yyvarsp->obj_spec.pss_obj_name);
	MEfill(sizeof(DB_OWN_NAME), ' ', (PTR) &yyvarsp->obj_spec.pss_owner);
	yyvarsp->obj_spec.pss_objspec_flags = 0;
	yyvarsp->obj_spec.pss_orig_obj_name = yacc_cb->yypvt[-0].psl_strtype;

        /* schang : last arg is psq_cb so that RMS GW can use the same code */
	status = psl_ci5_indexlocname(cb, yacc_cb->yypvt[-2].psl_strtype, &yyvarsp->obj_spec, &yyvarsp->with_clauses,
	    psq_cb);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 216:
if (!((*yyvarspp)->bypass_actions))
/* # line 4080 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	/*
	** store the index name into format understood by psl_ci5_indexlocname()
	** which is also used by the SQL indexlocname:
	*/

        STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(DB_TAB_NAME), (char *) &yyvarsp->obj_spec.pss_obj_name);
	MEfill(sizeof(DB_OWN_NAME), ' ', (PTR) &yyvarsp->obj_spec.pss_owner);
	yyvarsp->obj_spec.pss_objspec_flags = 0;
	yyvarsp->obj_spec.pss_orig_obj_name = yacc_cb->yypvt[-0].psl_strtype;

        /* schang : last arg is psq_cb so that RMS GW can use the same code */
	status = psl_ci5_indexlocname(cb, (char *) NULL, &yyvarsp->obj_spec,
	    &yyvarsp->with_clauses, psq_cb);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 219:
if (!((*yyvarspp)->bypass_actions))
/* # line 4106 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_ci4_indexcol(cb, yacc_cb->yypvt[-0].psl_strtype, &psq_cb->psq_error, FALSE);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 229:
if (!((*yyvarspp)->bypass_actions))
/* # line 4131 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_nm_eq_nm(cb, yacc_cb->yypvt[-2].psl_strtype, yacc_cb->yypvt[-0].psl_strtype, &yyvarsp->with_clauses, psq_cb->psq_mode,
	    &psq_cb->psq_error);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 230:
if (!((*yyvarspp)->bypass_actions))
/* # line 4142 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_nm_eq_no(cb, yacc_cb->yypvt[-2].psl_strtype, yacc_cb->yypvt[-0].psl_inttype, &yyvarsp->with_clauses, psq_cb->psq_mode,
	    &psq_cb->psq_error, (PSS_Q_XLATE *) NULL);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 231:
if (!((*yyvarspp)->bypass_actions))
/* # line 4153 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yyvarsp->list_clause = 0;	/* done with this clause */
    } break;

case 232:
if (!((*yyvarspp)->bypass_actions))
/* # line 4159 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_lst_prefix(cb, psq_cb, yacc_cb->yypvt[-1].psl_strtype, yyvarsp);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 235:
if (!((*yyvarspp)->bypass_actions))
/* # line 4173 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_lst_elem(cb, yyvarsp->list_clause, NULL, yacc_cb->yypvt[-0].psl_strtype, PSQ_INDEX,
	    &psq_cb->psq_error);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;
	}
	return(0);
}

i4
psl5iftn(yacc_cb, yyrule, yystatusp, yyvarspp, cb, psq_cb)
YACC_CB		*yacc_cb;
i4		yyrule;
PSS_SESBLK	*cb;
DB_STATUS	*yystatusp;
YYVARS		**yyvarspp;
PSQ_CB *	psq_cb;
{
    YYVARS	*yyvarsp = *yyvarspp;
	switch(yyrule)
	{
case 236:
if (!((*yyvarspp)->bypass_actions))
/* # line 4184 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_ci7_index_woword(cb, yacc_cb->yypvt[-0].psl_strtype, &yyvarsp->with_clauses,
	    &psq_cb->psq_error);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 237:
if (!((*yyvarspp)->bypass_actions))
/* # line 4195 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;
	PST_RT_NODE	       root;
	PST_QNODE	       *rootnode;
	PST_QTREE	       *tree;
	PSY_CB		       *psy_cb;
	i4		       err_code;
	PST_PROCEDURE	       *pnode;

	cb->pss_qualdepth--;

	/* Make the ROOT node */
	root.pst_rtuser = TRUE;
	root.pst_union.pst_next  = 0;
	root.pst_dups   = PST_ALLDUPS;
	status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL, yacc_cb->yypvt[-0].psl_trtype,
	    PST_ROOT, (PTR) &root, sizeof(PST_RT_NODE), DB_NODT, (i2) 0, (i4) 0,
	    (DB_ANYTYPE *) NULL, &rootnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	    return (status);

	/* make sure no tids in the integrity */
	if (psy_qscan(rootnode, cb->pss_resrng->pss_rgno, DB_IMTID))
	{
	    (VOID) psf_error(3496L, 0L, PSF_USERERR, &err_code, 
		&psq_cb->psq_error, 0);
	    return (E_DB_ERROR);
	}
	/* Make the query tree header */
	status = pst_header(cb, psq_cb, PST_UNSPECIFIED, (PST_QNODE *) NULL,
	    rootnode, &tree, &pnode, PST_0FULL_HEADER, (PSS_Q_XLATE *) NULL);
	if (status != E_DB_OK)
	    return (status);

	/*
	** pst_numjoins was set to PST_NOJOIN in pst_header(), so we don't need
	** to do it here.
	*/

	/* Make sure there are no aggregates in qualification */
	if (tree->pst_agintree)
	{
	    (VOID) psf_error(3490L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, sizeof(DB_TAB_NAME),
		&cb->pss_resrng->pss_tabname);
	    return (E_DB_ERROR);
	}

	/* Make sure the integrity has only one range variable */
	if (cb->pss_usrrange.pss_maxrng >= 1)
	{
	    (VOID) psf_error(3491L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, sizeof(DB_TAB_NAME),
		&cb->pss_resrng->pss_tabname);
	    return (E_DB_ERROR);
	}

	/* Set the root in QSF */
	status = psf_mroot(cb, &cb->pss_ostream, (PTR) pnode, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);


	/*
	** We want to give the control block back to the caller.  However,
	** the control block has been allocated using the control block
	** stream, and it's the output stream that gets handed back to the
	** caller.  Therefore, we must swap the control block and output
	** streams.
	*/
	/* Copy the query tree stream so we can re-use it */
	STRUCT_ASSIGN_MACRO(cb->pss_ostream, cb->pss_cbstream);

	/* Create the PSY_CB for the integrity definition */
	status = psy_qminit(cb, psq_cb, &cb->pss_ostream);
	if (status != E_DB_OK)
	    return (status);

	psy_cb = (PSY_CB *) cb->pss_object;

	/* Fill in QSF object ids for query tree and modified query text */
	STRUCT_ASSIGN_MACRO(cb->pss_cbstream.psf_mstream, psy_cb->psy_intree);

	/* Fill in table id and table name */
	STRUCT_ASSIGN_MACRO(yacc_cb->yypvt[-2].psl_rngtype->pss_tabid, psy_cb->psy_tables[0]);
	STRUCT_ASSIGN_MACRO(yacc_cb->yypvt[-2].psl_rngtype->pss_tabname, psy_cb->psy_tabname[0]);

	/* Put the query text in a contiguous block in QSF */
	status = psq_tout(cb, &cb->pss_usrrange, cb->pss_tchain, &cb->pss_tstream,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	STRUCT_ASSIGN_MACRO(cb->pss_tstream.psf_mstream, psy_cb->psy_qrytext);
    } break;

case 238:
if (!((*yyvarspp)->bypass_actions))
/* # line 4292 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	       status;
	PTR		       piece;

	psq_cb->psq_mode = PSQ_INTEG;

	/* Open the memory stream for allocating the query tree */
	status = psf_mopen(cb, QSO_QTREE_OBJ, &cb->pss_ostream,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	/*
	** The "define integrity" statement requires query text to be stored in
	** the iiqrytext relation.  Open a text chain and put the initial
	** words "define integrity " in it.  The text chain will be coalesced
	** into a contiguous block later.
	*/
	cb->pss_stmt_flags |= PSS_TXTEMIT;	    /* Indicate to emit text */
	status = psq_topen(&cb->pss_tchain, &cb->pss_memleft,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psq_tadd(cb->pss_tchain, (u_char *) "define integrity ",
	    sizeof("define integrity ") - 1, &piece, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
    } break;

case 241:
if (!((*yyvarspp)->bypass_actions))
/* # line 4327 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	cb->pss_qualdepth++;
    } break;

case 242:
if (!((*yyvarspp)->bypass_actions))
/* # line 4331 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	cb->pss_qualdepth++;
    } break;

case 243:
if (!((*yyvarspp)->bypass_actions))
/* # line 4335 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	cb->pss_qualdepth++;
    } break;

case 244:
if (!((*yyvarspp)->bypass_actions))
/* # line 4341 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4     err_code;
	i4	    mask;
	i4	    mask2;

	/*
	** On an integrity definition, remember what the result range variable
	** is.  This can't be stored in the result range variable slot in
	** the user range table, because that is not for "real" range variables,
	** only for result tables (as in an append).
	*/
	cb->pss_resrng = yacc_cb->yypvt[-0].psl_rngtype;

	mask = cb->pss_resrng->pss_tabdesc->tbl_status_mask;
	mask2 = cb->pss_resrng->pss_tabdesc->tbl_2_status_mask;

	/*
	** Check for integrities on views.
	*/
	if (mask & DMT_VIEW)
	{
	    (VOID) psf_error(3493L, 0L, PSF_USERERR,
		&err_code, &psq_cb->psq_error, 1, 
		psf_trmwhite(sizeof(DB_TAB_NAME), 
		    (char *) &cb->pss_resrng->pss_tabname),
		&cb->pss_resrng->pss_tabname);
	    return (E_DB_ERROR);
	}

	if (mask & (DMT_CATALOG | DMT_EXTENDED_CAT)
	    || mask2 & DMT_TEXTENSION)
	{
	    /* If not an extended catalog and no catalog update
	    ** permission - error.
	    */
	    if (~mask & DMT_EXTENDED_CAT
		&& !(cb->pss_ses_flag & PSS_CATUPD))
	    {
		if ( Psf_srvblk->psf_capabilities & PSF_C_C2SECURE )
		{
		    DB_STATUS	local_status;
		    DB_ERROR	e_error;
		    /*
		    ** Audit failure to define integrity
		    */
		    local_status = psy_secaudit( FALSE, cb, 
			    cb->pss_resrng->pss_tabname.db_tab_name,
			    &cb->pss_resrng->pss_ownname,
			    sizeof(cb->pss_resrng->pss_tabname),
			    SXF_E_TABLE, I_SX203D_INTEGRITY_CREATE,
			    SXF_A_FAIL | SXF_A_CONTROL,
			    &e_error);
		}

		(VOID) psf_error(3498L, 0L, PSF_USERERR,
		    &err_code, &psq_cb->psq_error, 2, 
		    sizeof(cb->pss_lineno), &cb->pss_lineno,
		    psf_trmwhite(sizeof(DB_TAB_NAME),
			(char *) &cb->pss_resrng->pss_tabname),
		    &cb->pss_resrng->pss_tabname);
		return (E_DB_ERROR);
	    }
	}
	/* Check for integrity on table owned by someone else. */
	else if (MEcmp((PTR) &cb->pss_user, 
	    (PTR) &cb->pss_resrng->pss_tabdesc->tbl_owner, sizeof(DB_OWN_NAME)))
	{
	    if ( Psf_srvblk->psf_capabilities & PSF_C_C2SECURE )
	    {
		DB_STATUS	local_status;
		DB_ERROR	e_error;
		/*
		** Audit failure to define integrity
		*/
		local_status = psy_secaudit( FALSE, cb, 
		    cb->pss_resrng->pss_tabname.db_tab_name,
		    &cb->pss_resrng->pss_ownname,
		    sizeof(cb->pss_resrng->pss_tabname),
		    SXF_E_TABLE, I_SX203D_INTEGRITY_CREATE,
		    SXF_A_FAIL | SXF_A_CONTROL,
		    &e_error);
	    }

	    (VOID) psf_error(3494L, 0L, PSF_USERERR,
		&err_code, &psq_cb->psq_error, 1,
		psf_trmwhite(sizeof(DB_TAB_NAME),
		    (char *) &cb->pss_resrng->pss_tabname),
		&cb->pss_resrng->pss_tabname);
	    return (E_DB_ERROR);
	}

	yacc_cb->yyval.psl_rngtype = yacc_cb->yypvt[-0].psl_rngtype;
    } break;

case 245:
if (!((*yyvarspp)->bypass_actions))
/* # line 4437 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		    status;

	status = psl_md1_modify(cb, yyvarsp, &psq_cb->psq_error);

	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 246:
if (!((*yyvarspp)->bypass_actions))
/* # line 4448 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_md2_modstmnt(cb, psq_cb, yyvarsp);

	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 247:
if (!((*yyvarspp)->bypass_actions))
/* # line 4459 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	/*
	** store the index name into format understood by psl_md8_modtable()
	** which is also used by the SQL modtable:
	*/

        STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(DB_TAB_NAME), (char *) &yyvarsp->obj_spec.pss_obj_name);
	MEfill(sizeof(DB_OWN_NAME), ' ', (PTR) &yyvarsp->obj_spec.pss_owner);
	yyvarsp->obj_spec.pss_objspec_flags = 0;
	yyvarsp->obj_spec.pss_orig_obj_name = yacc_cb->yypvt[-0].psl_strtype;

	status = psl_md8_modtable(cb, &yyvarsp->obj_spec, &psq_cb->psq_error);

	if (DB_FAILURE_MACRO(status))
	{
	    return(status);
	}
    } break;

case 251:
if (!((*yyvarspp)->bypass_actions))
/* # line 4487 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_md3_modstorage(cb, yyvarsp, &psq_cb->psq_error);

	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 253:
if (!((*yyvarspp)->bypass_actions))
/* # line 4499 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_md4_modstorname(cb, yacc_cb->yypvt[-0].psl_strtype, yyvarsp, &psq_cb->psq_error);

	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 254:
if (!((*yyvarspp)->bypass_actions))
/* # line 4508 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_md4_modstorname(cb, "relocate", yyvarsp, &psq_cb->psq_error);

	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 256:
if (!((*yyvarspp)->bypass_actions))
/* # line 4520 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_md5_modkeys(cb, yyvarsp, &psq_cb->psq_error);

	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 259:
if (!((*yyvarspp)->bypass_actions))
/* # line 4535 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	/* Decode sortorder.  Error if unknown. */
	if (!STcompare(yacc_cb->yypvt[-0].psl_strtype, "a") || !STcompare(yacc_cb->yypvt[-0].psl_strtype, "ascending"))
	{
	    yacc_cb->yyval.psl_tytype = PSS_ASCENDING;
	}
	else if (!STcompare(yacc_cb->yypvt[-0].psl_strtype, "d") || !STcompare(yacc_cb->yypvt[-0].psl_strtype, "descending"))
	{
	    yacc_cb->yyval.psl_tytype = PSS_DESCENDING;
	}
	else
	{
	    psl_yerror(3, cb, psq_cb);
	    return (E_DB_ERROR);
	}
    } break;

case 261:
if (!((*yyvarspp)->bypass_actions))
/* # line 4555 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		    status;

	status = psl_md6_modbasekey(cb, (i4) (yacc_cb->yypvt[-0].psl_tytype == PSS_ASCENDING),
	    yyvarsp->is_heapsort, &psq_cb->psq_error);

	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 262:
if (!((*yyvarspp)->bypass_actions))
/* # line 4567 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_md7_modkeyname(cb, yacc_cb->yypvt[-0].psl_strtype, yyvarsp, &psq_cb->psq_error);

	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 273:
if (!((*yyvarspp)->bypass_actions))
/* # line 4596 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_nm_eq_no(cb, yacc_cb->yypvt[-2].psl_strtype, yacc_cb->yypvt[-0].psl_inttype, &yyvarsp->with_clauses, PSQ_MODIFY,
	    &psq_cb->psq_error, (PSS_Q_XLATE *) NULL);

	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 274:
if (!((*yyvarspp)->bypass_actions))
/* # line 4608 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yyvarsp->list_clause = 0;	/* done with this clause */
    } break;

case 275:
if (!((*yyvarspp)->bypass_actions))
/* # line 4614 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_lst_prefix(cb, psq_cb, yacc_cb->yypvt[-1].psl_strtype, yyvarsp);

	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 278:
if (!((*yyvarspp)->bypass_actions))
/* # line 4629 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_lst_elem(cb, yyvarsp->list_clause, NULL, yacc_cb->yypvt[-0].psl_strtype, PSQ_MODIFY,
	    &psq_cb->psq_error);

	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 279:
if (!((*yyvarspp)->bypass_actions))
/* # line 4641 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_md9_modopt_word(cb, yacc_cb->yypvt[-0].psl_strtype, &yyvarsp->with_clauses,
	    &psq_cb->psq_error);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 280:
if (!((*yyvarspp)->bypass_actions))
/* # line 4652 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;
	PSF_MSTREAM	       tempstream;
	PST_QNODE	       *rootnode;
	PST_RT_NODE	       root;
	PSY_CB		       *psy_cb;
	PST_QTREE	       *tree;
	PST_PROCEDURE	       *pnode;

	psy_cb = (PSY_CB *) cb->pss_object;

	/* mark if we have a qualification */
	if ((yacc_cb->yypvt[-0].psl_trtype)->pst_sym.pst_type != PST_QLEND)
	{
	    psy_cb->psy_istree = TRUE;

	    /* Make the ROOT node */
	    root.pst_rtuser = TRUE;
	    root.pst_union.pst_next  = 0;
	    root.pst_dups   = PST_ALLDUPS;
	    status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL, yacc_cb->yypvt[-0].psl_trtype, 
		PST_ROOT, (PTR) &root, sizeof(PST_RT_NODE), DB_NODT, (i2) 0,
		(i4) 0, (DB_ANYTYPE *) NULL, &rootnode, &psq_cb->psq_error,
		(i4) 0);
	    if (status != E_DB_OK)
		return (status);

	    /* Make the query tree header */
	    status = pst_header(cb, psq_cb, PST_UNSPECIFIED, (PST_QNODE *) NULL,
		rootnode, &tree, &pnode, PST_0FULL_HEADER,
		(PSS_Q_XLATE *) NULL);
	    if (status != E_DB_OK)
		return (status);

	    /* pst_numjoins was set to PST_NOJOIN in pst_header. */

	    /* Set the root in QSF */
	    status = psf_mroot(cb, &cb->pss_ostream, (PTR) pnode, 
		&psq_cb->psq_error);
	    if (status != E_DB_OK)
		return (status);
	    /*
	    ** Copy the object ids for the query tree and query text to the
	    ** control block.
	    */
	   STRUCT_ASSIGN_MACRO(cb->pss_ostream.psf_mstream, psy_cb->psy_intree);
	}
	else
	{
	    /* There is no QTREE, so close QTREE stream */
	    status = psf_mclose(cb, &cb->pss_ostream, &psq_cb->psq_error);
	    if (status != E_DB_OK)
		return (status);
	    cb->pss_ostream.psf_mstream.qso_handle = (PTR) NULL;
	}


	/*
	** We want to give the control block back to the caller.  However,
	** the control block has been allocated using the control block
	** stream, and it's the output stream that gets handed back to the
	** caller.  Therefore, we must swap the control block and output
	** streams.
	*/
	if (psy_cb->psy_istree)
	{
	    STRUCT_ASSIGN_MACRO(cb->pss_ostream, tempstream);
	    STRUCT_ASSIGN_MACRO(cb->pss_cbstream, cb->pss_ostream);
	    STRUCT_ASSIGN_MACRO(tempstream, cb->pss_cbstream);
	}
	else
	{
	    STRUCT_ASSIGN_MACRO(cb->pss_cbstream, cb->pss_ostream);
	    cb->pss_cbstream.psf_mstream.qso_handle = (PTR) NULL;
	}

	/* Put the query text in a contiguous block in QSF */
	status = psq_tout(cb, &cb->pss_usrrange, cb->pss_tchain, &cb->pss_tstream,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	STRUCT_ASSIGN_MACRO(cb->pss_tstream.psf_mstream, psy_cb->psy_qrytext);
    } break;

case 281:
if (!((*yyvarspp)->bypass_actions))
/* # line 4738 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PTR		piece;
	DB_STATUS	status;
	char		*str;
	PSY_CB          *psy_cb = (PSY_CB *) cb->pss_object;
	i4		err_code;

	/*
	** The "define permit" statement requires query text to be stored in
	** the iiqrytext relation.  Open a text chain and put the initial
	** string in it.  If a permit represents more than one privilege,
	** the initial string will contain a placeholder big enough to
	** accomodate any of privilege names (i.e. RETRIEVE, REPLACE, APPEND or
	** DELETE); otherwise, the actual privilege name will be inserted.
	** Initial string will also contain word ON since it was already seen by
	** the scanner.  The text chain will be coalesced into a contiguous
	** block later.
	*/
	cb->pss_stmt_flags |= PSS_TXTEMIT; 	    /* Indicate to emit text */

	status = psq_topen(&cb->pss_tchain, &cb->pss_memleft,
	    &psq_cb->psq_error);
	if (DB_FAILURE_MACRO(status))
	    return (status);

	if (BTcount((char *) &psy_cb->psy_opmap, BITS_IN(psy_cb->psy_opmap))
	        > 1)
	{
	    str = "define permit ???????? on";
	}
	else if (psy_cb->psy_opmap == DB_RETRIEVE)
	{
	    str = "define permit retrieve on";
	}
	else if (psy_cb->psy_opmap == DB_REPLACE)
	{
	    str = "define permit replace on";
	}
	else if (psy_cb->psy_opmap == DB_DELETE)
	{
	    str = "define permit delete on";
	}
	else if (psy_cb->psy_opmap == DB_APPEND)
	{
	    str = "define permit append on";
	}
	else
	{
            _VOID_ psf_error(9346L, 0L, PSF_USERERR, &err_code,
            &psq_cb->psq_error, 0);
            return (E_DB_ERROR);
        }

	status = psq_tadd(cb->pss_tchain, (u_char *) str, STlength(str), &piece,
	    &psq_cb->psq_error);
	if (DB_FAILURE_MACRO(status))
	    return (status);
    } break;

case 283:
if (!((*yyvarspp)->bypass_actions))
/* # line 4800 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	       status;
	PSY_CB		       *psy_cb;

	psq_cb->psq_mode = PSQ_PROT;

	/* Open the memory stream for allocating the query tree */
	status = psf_mopen(cb, QSO_QTREE_OBJ, &cb->pss_ostream,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	/* Allocate the PSY_CB for the define permit statement */
	status = psy_qminit(cb, psq_cb, &cb->pss_cbstream);
	if (status != E_DB_OK)
	    return (status);

	psy_cb = (PSY_CB *) cb->pss_object;

	/* Set control block fields to defaults and initial values */
	psy_cb->psy_grant   = PSY_CPERM;    /* this is not GRANT */

	/* Default terminal name is "" */
	MEfill(sizeof(psy_cb->psy_terminal), ' ', (PTR) &psy_cb->psy_terminal);

	/* default user name is " " */
	MEfill(sizeof(psy_cb->psy_user), ' ', (PTR) &psy_cb->psy_user);
	
	/*
	** Default time is all day, starting at 00:00 and ending at 24:00,
	** given as minutes after 00:00.
	*/
	psy_cb->psy_timbgn = 0;
	psy_cb->psy_timend = 1440;

	/*
	** Default days are all week, starting with Sunday and ending with
	** Saturday.  Sunday = 0, Saturday = 6.
	*/
	psy_cb->psy_daybgn = 0;
	psy_cb->psy_dayend = 6;
    } break;

case 286:
if (!((*yyvarspp)->bypass_actions))
/* # line 4849 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSY_CB		*psy_cb = (PSY_CB *) cb->pss_object;

	psy_cb->psy_opmap = psy_cb->psy_opctl =
	    DB_RETRIEVE | DB_APPEND | DB_REPLACE | DB_DELETE;
    } break;

case 287:
if (!((*yyvarspp)->bypass_actions))
/* # line 4856 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSY_CB		*psy_cb = (PSY_CB *) cb->pss_object;

	psy_cb->psy_opmap |= DB_RETRIEVE;
	psy_cb->psy_opctl |= DB_RETRIEVE;
    } break;

case 288:
if (!((*yyvarspp)->bypass_actions))
/* # line 4863 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSY_CB		*psy_cb = (PSY_CB *) cb->pss_object;

	psy_cb->psy_opmap |= DB_DELETE;
	psy_cb->psy_opctl |= DB_DELETE;
    } break;

case 289:
if (!((*yyvarspp)->bypass_actions))
/* # line 4870 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSY_CB		*psy_cb = (PSY_CB *) cb->pss_object;

	psy_cb->psy_opmap |= DB_APPEND;
	psy_cb->psy_opctl |= DB_APPEND;
    } break;

case 290:
if (!((*yyvarspp)->bypass_actions))
/* # line 4877 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSY_CB		*psy_cb = (PSY_CB *) cb->pss_object;

	psy_cb->psy_opmap |= DB_REPLACE;
	psy_cb->psy_opctl |= DB_REPLACE;
    } break;
	}
	return(0);
}

i4
psl6iftn(yacc_cb, yyrule, yystatusp, yyvarspp, cb, psq_cb)
YACC_CB		*yacc_cb;
i4		yyrule;
PSS_SESBLK	*cb;
DB_STATUS	*yystatusp;
YYVARS		**yyvarspp;
PSQ_CB *	psq_cb;
{
    YYVARS	*yyvarsp = *yyvarspp;
	switch(yyrule)
	{
case 291:
if (!((*yyvarspp)->bypass_actions))
/* # line 4884 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSY_CB		*psy_cb = (PSY_CB *) cb->pss_object;

	if(!STcasecmp(yacc_cb->yypvt[-0].psl_strtype, "COPY_INTO")==0)
	{
		psy_cb->psy_opmap |= DB_COPY_INTO;
		psy_cb->psy_opctl |= DB_COPY_INTO;
	}
	else if(!STcasecmp(yacc_cb->yypvt[-0].psl_strtype, "COPY_FROM")==0)
	{
		psy_cb->psy_opmap |= DB_COPY_FROM;
		psy_cb->psy_opctl |= DB_COPY_FROM;
	}
	else
	{
		return E_DB_ERROR;
	}
    } break;

case 295:
if (!((*yyvarspp)->bypass_actions))
/* # line 4910 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSY_CB		    *psy_cb;
	i4		    err_code;
	i4		    mask;
	PSY_TBL		    *psy_tbl;
	DB_STATUS	    status;

	cb->pss_resrng = yacc_cb->yypvt[-0].psl_rngtype;

	mask = cb->pss_resrng->pss_tabdesc->tbl_status_mask;

	/*
	** unless the object is a catalog and the user has CATUPD, the following
	** must be true:
	**   - current user must be the DBA and
	**   - object must be owned by the current user (the DBA) or must be an
	**     extended catalog
	*/

	if (!psy_permit_ok(mask, cb, &cb->pss_resrng->pss_ownname))
	{
	    i4	    err_num;

	    if ( Psf_srvblk->psf_capabilities & PSF_C_C2SECURE )
	    {
		DB_STATUS	    local_status;
		DB_ERROR	    e_error;

		/* Must audit CREATE PERMIT failure. */
		local_status = psy_secaudit(FALSE, cb,
			    (char *)&cb->pss_resrng->pss_tabdesc->tbl_name,
			    &cb->pss_resrng->pss_tabdesc->tbl_owner,
			    sizeof(DB_TAB_NAME), SXF_E_TABLE,
			    I_SX2016_PROT_TAB_CREATE, SXF_A_FAIL | SXF_A_CONTROL,
			    &e_error);
	    }

	    err_num = (MEcmp((PTR) &cb->pss_user, (PTR) &cb->pss_dba,
			     sizeof(DB_OWN_NAME))) ? 3595L : 3592L;

	    _VOID_ psf_error(err_num, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1,
		psf_trmwhite(sizeof(DB_TAB_NAME), 
		    (char *) &cb->pss_resrng->pss_tabname),
		&cb->pss_resrng->pss_tabname);
	    return (E_DB_ERROR);
	}

	/* Don't allow permits on views */
	if (mask & DMT_VIEW)
	{
	    _VOID_ psf_error(3593L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1,
		psf_trmwhite(sizeof(DB_TAB_NAME), 
		    (char *) &cb->pss_resrng->pss_tabname),
		&cb->pss_resrng->pss_tabname);
	    return (E_DB_ERROR);
	}

	/* Copy table id and table name to control block */
	psy_cb = (PSY_CB *) cb->pss_object;

	/* Allocate memory for a table entry */
	status = psf_malloc(cb, &cb->pss_cbstream, (i4) sizeof(PSY_TBL),
	    (PTR *) &psy_tbl, &psq_cb->psq_error);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	/*
	** Copy table id, table name, and owner name in table entry.
	*/
	STRUCT_ASSIGN_MACRO(cb->pss_resrng->pss_tabid, psy_tbl->psy_tabid);
	STRUCT_ASSIGN_MACRO(cb->pss_resrng->pss_tabname, psy_tbl->psy_tabnm);
	STRUCT_ASSIGN_MACRO(cb->pss_resrng->pss_ownname, psy_tbl->psy_owner);
	
	/* remember the object type */
	psy_tbl->psy_mask = (mask & DMT_IDX) ? PSY_OBJ_IS_INDEX
					     : PSY_OBJ_IS_TABLE;

	/*
	** Attach element to the list.
	*/
	QUinsert((QUEUE *) psy_tbl, (QUEUE *) &psy_cb->psy_tblq);

	yacc_cb->yyval.psl_rngtype = yacc_cb->yypvt[-0].psl_rngtype;
    } break;

case 297:
if (!((*yyvarspp)->bypass_actions))
/* # line 5002 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSY_CB                 *psy_cb = (PSY_CB *) cb->pss_object;

	psy_cb->psy_flags |= PSY_EXCLUDE_COLUMNS;
    } break;

case 301:
if (!((*yyvarspp)->bypass_actions))
/* # line 5015 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DMT_ATT_ENTRY	       *attribute;
	DB_ATT_NAME	       attname;
	i4		       err_code;
	PSY_CB		       *psy_cb;

	/* Look up the attribute number */
	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(DB_ATT_NAME), (char *) &attname);
	attribute = pst_coldesc(cb->pss_resrng, &attname);

	/* Check for attribute not found */
	if (attribute == (DMT_ATT_ENTRY *) NULL)
	{
	    (VOID) psf_error(2100L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 4, cb->pss_lineno, &cb->pss_lineno,
		sizeof(DB_TAB_NAME), &cb->pss_resrng->pss_tabname,
		sizeof(DB_OWN_NAME), &cb->pss_resrng->pss_ownname,
		STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);
	}

	/* Copy the attribute number to the control block */
	psy_cb = (PSY_CB *) cb->pss_object;
	if (psy_cb->psy_numcols >= DB_MAX_COLS)
	{
	    (VOID) psf_error(2113L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 0);
	    return (E_DB_ERROR);
	}

	psy_cb->psy_cols[psy_cb->psy_numcols++].db_att_id =
	    (i2) attribute->att_number;
    } break;

case 302:
if (!((*yyvarspp)->bypass_actions))
/* # line 5051 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSY_CB                 *psy_cb;

	/* {@fix_me@} */	/* Check validity of user name? */
	psy_cb = (PSY_CB *) cb->pss_object;
	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(DB_OWN_NAME), (char *) &psy_cb->psy_user);

	/* granted to a user */
	psy_cb->psy_gtype = DBGR_USER;
    } break;

case 303:
if (!((*yyvarspp)->bypass_actions))
/* # line 5062 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSY_CB          *psy_cb;
	char		*str;

	str = sconvert(yacc_cb->yypvt[-0].psl_textype);

	/* {@fix_me@} */	/* Check validity of user name? */
	psy_cb = (PSY_CB *) cb->pss_object;
	STmove(str, ' ', sizeof(DB_OWN_NAME), (char *) &psy_cb->psy_user);

	/* granted to a user */
	psy_cb->psy_gtype = DBGR_USER;
    } break;

case 304:
if (!((*yyvarspp)->bypass_actions))
/* # line 5076 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSY_CB          *psy_cb = (PSY_CB *) cb->pss_object;

	/* granted to public */
	psy_cb->psy_gtype = DBGR_PUBLIC;

	/* psy_cb->psy_user has already been filled with blanks */
    } break;

case 305:
if (!((*yyvarspp)->bypass_actions))
/* # line 5087 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSY_CB                 *psy_cb;

	psy_cb = (PSY_CB *) cb->pss_object;

#ifdef NOT_NEEDED
	/* Check validity of terminal name */
	if (!TEvalid(yacc_cb->yypvt[-0].psl_strtype))
	{
	    i4		       err_code;

	    (VOID) psf_error(3590L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);
	}
#endif
	/* Copy terminal name to control block */
	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(DB_TERM_NAME), (char *) &psy_cb->psy_terminal);
    } break;

case 312:
if (!((*yyvarspp)->bypass_actions))
/* # line 5117 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSY_CB                 *psy_cb;

	psy_cb = (PSY_CB *) cb->pss_object;
	psy_cb->psy_timbgn = yacc_cb->yypvt[-2].psl_inttype;
	psy_cb->psy_timend = yacc_cb->yypvt[-0].psl_inttype;
    } break;

case 313:
if (!((*yyvarspp)->bypass_actions))
/* # line 5127 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;

	/* Check validity of hours */
	if (yacc_cb->yypvt[-2].psl_inttype > 24 || yacc_cb->yypvt[-2].psl_inttype < 0)
	{
	    (VOID) psf_error(2136L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 2, sizeof (cb->pss_lineno), &cb->pss_lineno,
		sizeof(yacc_cb->yypvt[-2].psl_inttype), &(yacc_cb->yypvt[-2].psl_inttype));
	    return (E_DB_ERROR);
	}

	/* Check validity of minutes */
	if (yacc_cb->yypvt[-0].psl_inttype > 59 || yacc_cb->yypvt[-0].psl_inttype < 0)
	{
	    (VOID) psf_error(2137L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 2, sizeof (cb->pss_lineno), &cb->pss_lineno,
		sizeof(yacc_cb->yypvt[-0].psl_inttype), &(yacc_cb->yypvt[-0].psl_inttype));
	    return (E_DB_ERROR);
	}

	/* If hours are 24, make sure minutes are 0 (e.g. 24:10 illegal) */
	if (yacc_cb->yypvt[-2].psl_inttype == 24 && yacc_cb->yypvt[-0].psl_inttype != 0)
	{
	    (VOID) psf_error(2138L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 2, sizeof (cb->pss_lineno), &cb->pss_lineno,
		sizeof(yacc_cb->yypvt[-0].psl_inttype), &(yacc_cb->yypvt[-0].psl_inttype));
	    return (E_DB_ERROR);
	}

	yacc_cb->yyval.psl_inttype = (yacc_cb->yypvt[-2].psl_inttype * 60) + yacc_cb->yypvt[-0].psl_inttype;
    } break;

case 314:
if (!((*yyvarspp)->bypass_actions))
/* # line 5162 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSY_CB                 *psy_cb;
	i4		       err_code;
	i4		       daynum;

	psy_cb = (PSY_CB *) cb->pss_object;

	/* Convert beginning day of week string to number */
	daynum = psq_cvtdow(yacc_cb->yypvt[-2].psl_strtype);
	if (daynum < 0)
	{
	    (VOID) psf_error(3594L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 2, sizeof(DB_TAB_NAME),
		&cb->pss_resrng->pss_tabname, STlength(yacc_cb->yypvt[-2].psl_strtype), yacc_cb->yypvt[-2].psl_strtype);
	    return (E_DB_ERROR);
	}
	psy_cb->psy_daybgn = daynum;

	/* Convert ending day of week string to number */
	daynum = psq_cvtdow(yacc_cb->yypvt[-0].psl_strtype);
	if (daynum < 0)
	{
	    (VOID) psf_error(3594L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 2, sizeof(DB_TAB_NAME),
		&cb->pss_resrng->pss_tabname, STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);
	}
	psy_cb->psy_dayend = daynum;
    } break;

case 315:
if (!((*yyvarspp)->bypass_actions))
/* # line 5194 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4	    save_qry_mode = psq_cb->psq_mode;

	psq_cb->psq_mode = PSQ_DEFQRY;

	/*
	** Added code to support shareable QUEL repeat queries.
	** NOTE: 5.0-style repeat queries will not be shared
	*/
	if (cb->pss_defqry == PSQ_DEFQRY && yyvarsp->qp_shareable)
	{
	    DB_STATUS		    status;
	    DB_SHR_RPTQRY_INFO	    *qry_info;

	    /* allocate memory for DB_SHR_RPTQRY_INFO structure */
	    status = psf_malloc(cb, &cb->pss_ostream,
		(i4) sizeof(DB_SHR_RPTQRY_INFO), (PTR *) &qry_info,
		&psq_cb->psq_error);
	    if (DB_FAILURE_MACRO(status))
		return (status);

	    /*
	    ** first collect information for this repeat query which would
	    ** enable us to effectively determine if an existing query plan can
	    ** indeed be shared.
	    */

	    /*
	    ** put text the repeat query acumulated so far into a contiguous
	    ** block of memory
	    */
	    status = psq_1rptqry_out(cb, cb->pss_tchain, &cb->pss_ostream,
		&qry_info->db_qry_len, &qry_info->db_qry, &psq_cb->psq_error);

	    if (DB_FAILURE_MACRO(status))
	    {
		return(status);
	    }

	    /* build a list of table ids of tables used in the query */

	    status = psl_rptqry_tblids(cb, &cb->pss_usrrange, &cb->pss_ostream,
		save_qry_mode, &qry_info->db_num_tblids,
		&qry_info->db_tblids, &psq_cb->psq_error);

	    if (DB_FAILURE_MACRO(status))
	    {
		return(status);
	    }    

	    /*
	    ** Note that psl_repeat_qry_id() may change psq_cb->psq_mode to
	    ** PSQ_QRYDEFED if the QEP is already in memory
	    */
	    status = psl_repeat_qry_id(psq_cb, cb, &yyvarsp->fe_cursor_id, yacc_cb->yypvt[-0].psl_qttype,
		qry_info);

	    if (DB_FAILURE_MACRO(status))
	    {
		return(status);
	    }
	}

	cb->pss_defqry = PSQ_DEFQRY;
    } break;

case 316:
if (!((*yyvarspp)->bypass_actions))
/* # line 5262 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_qttype = yacc_cb->yypvt[-0].psl_qttype;
    } break;

case 317:
if (!((*yyvarspp)->bypass_actions))
/* # line 5266 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_qttype = yacc_cb->yypvt[-0].psl_qttype;
    } break;

case 318:
if (!((*yyvarspp)->bypass_actions))
/* # line 5270 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_qttype = yacc_cb->yypvt[-0].psl_qttype;
    } break;

case 319:
if (!((*yyvarspp)->bypass_actions))
/* # line 5274 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yyvarsp->check_for_vlups = TRUE;
    } break;

case 320:
if (!((*yyvarspp)->bypass_actions))
/* # line 5278 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_qttype = yacc_cb->yypvt[-0].psl_qttype;
    } break;

case 321:
if (!((*yyvarspp)->bypass_actions))
/* # line 5289 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4         val1 = 0, val2 = 0;
        char		trbuf[PSF_MAX_TEXT + 1]; /* last char for `\n' */

	psq_cb->psq_mode = PSQ_DEFQRY;
	cb->pss_defqry = PSQ_50DEFQRY;
	/* return the query name. The session id and name uniquely
	** identify the query.
	*/
	psq_cb->psq_cursid.db_cursor_id[0] = cb->pss_psessid;
	psq_cb->psq_cursid.db_cursor_id[1] = 0;
	STmove(yacc_cb->yypvt[-1].psl_strtype, ' ', DB_MAXNAME, psq_cb->psq_cursid.db_cur_name);

	if (ult_check_macro(&cb->pss_trace, 4, &val1, &val2))
	{
	    psf_display(psf_scctrace, 0, trbuf, sizeof(trbuf) - 1,
		"\n5.0-style QUEL repeat query cannot be shared.\n");
	}
    } break;

case 322:
if (!((*yyvarspp)->bypass_actions))
/* # line 5309 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	status;

	psq_cb->psq_mode = PSQ_DEFQRY;
	cb->pss_defqry = PSQ_DEFQRY;

	/* For shared QEP's save the original FE id. */
	STRUCT_ASSIGN_MACRO(*(yacc_cb->yypvt[-1].psl_cursid), yyvarsp->fe_cursor_id);

	/* 
	** Hand cursor id back to caller. PSF fills in a part of cursor id.
	** The part consists of two i4's. The only requirement here is that
	** they be unique. So we use pss_sessid which is always unique and
	** a counter (pss_crsid) within each session. We used TMnow before
	** but it turns out that the granularity of the system clock is not
	** good enough to make the timestamp unique.
	*/
	STRUCT_ASSIGN_MACRO(*(yacc_cb->yypvt[-1].psl_cursid), psq_cb->psq_cursid);
	psq_cb->psq_cursid.db_cursor_id[0] = (i4) cb->pss_psessid;
	psq_cb->psq_cursid.db_cursor_id[1] = (i4) ++cb->pss_crsid;

	/*
	** Determining if a quel repeat query is shareable requires that its
	** text be stored along with the range table
	*/
	cb->pss_stmt_flags |= PSS_QUEL_RPTQRY_TEXT;

	/* open the text chain */
	status = psq_topen(&cb->pss_tchain, &cb->pss_memleft,
	    &psq_cb->psq_error);

	if (DB_FAILURE_MACRO(status))
	{
	    return(status);
	}
    } break;

case 327:
if (!((*yyvarspp)->bypass_actions))
/* # line 5356 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4		    rngvar_info;
	DB_STATUS           status;
	PSS_RNGTAB	    *resrange;
	DB_TAB_NAME	    tabname;

	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(tabname), (char *) &tabname);
	status = psl_rngent(&cb->pss_usrrange, -1, yacc_cb->yypvt[-2].psl_strtype, &tabname, cb,
	    TRUE, &resrange, psq_cb->psq_mode, &psq_cb->psq_error,
	    &rngvar_info);
	if (status != E_DB_OK)
	{
	    return (status);
	}
    } break;

case 328:
if (!((*yyvarspp)->bypass_actions))
/* # line 5372 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS           status;
	i4		    rngvar_info;
	PSS_RNGTAB	    *resrange;
	DB_TAB_NAME	    tabname;

	/* takes care of a case where table name happens to be "null" */
	STmove("null", ' ', sizeof(tabname), (char *) &tabname);
	status = psl_rngent(&cb->pss_usrrange, -1, yacc_cb->yypvt[-1].psl_strtype, &tabname, cb,
	    TRUE, &resrange, psq_cb->psq_mode, &psq_cb->psq_error,
	    &rngvar_info);
	if (status != E_DB_OK)
	{
	    return (status);
	}
    } break;

case 329:
if (!((*yyvarspp)->bypass_actions))
/* # line 5389 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4		    rngvar_info;
	i4		    err_code;
	DB_STATUS           status;
	PSS_RNGTAB	    *resrange;
	DB_TAB_NAME	    tabname;
	DB_OWN_NAME	    sessname;

	if (STcasecmp(yacc_cb->yypvt[-2].psl_strtype, "session"))
	{
	    (VOID) psf_error(5153, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1,
		psf_trmwhite(sizeof(DB_OWN_NAME), 
		    (char *) yacc_cb->yypvt[-2].psl_strtype), yacc_cb->yypvt[-2].psl_strtype);
	    return (E_DB_ERROR);
	}
	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(tabname), (char *) &tabname);
	STRUCT_ASSIGN_MACRO(cb->pss_sess_owner, sessname);
	status = psl_orngent(&cb->pss_usrrange, -1, yacc_cb->yypvt[-4].psl_strtype, &sessname, &tabname, 
	    cb, TRUE, &resrange, psq_cb->psq_mode, &psq_cb->psq_error,
	    &rngvar_info);
	if (status != E_DB_OK)
	{
	    return (status);
	}
    } break;

case 330:
if (!((*yyvarspp)->bypass_actions))
/* # line 5418 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	psq_cb->psq_mode = PSQ_RANGE;
    } break;

case 331:
if (!((*yyvarspp)->bypass_actions))
/* # line 5424 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4             err_code;
	i4		    err_num = 0L;
	bool		    must_audit = FALSE;
	DB_STATUS	    status;
	i4		    rngvar_info;
	QEU_CB		    *qeu_cb;
	DMU_CB		    *dmu_cb;
	DB_TAB_NAME	    tabname;
	PSS_RNGTAB	    *resrange;
	bool		    leave_loop = TRUE;

	psq_cb->psq_mode = PSQ_RELOCATE;

	/* Allocate QEU_CB for RELOCATE and initialize its header */
	status = psl_qeucb(cb, DMU_RELOCATE_TABLE, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	qeu_cb = (QEU_CB *) cb->pss_object;

	/* Allocate a DMU control block */
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(DMU_CB),
	    (PTR *) &dmu_cb, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	MEfill(sizeof(DMU_CB), '\0', dmu_cb);
	qeu_cb->qeu_d_cb = (PTR) dmu_cb;

	/* Fill in the DMU control block header */
	dmu_cb->type = DMU_UTILITY_CB;
	dmu_cb->length = sizeof(DMU_CB);
	dmu_cb->dmu_flags_mask = 0;
	dmu_cb->dmu_db_id = (char*) cb->pss_dbid;

	/* Look up the table to be relocated */
	STmove(yacc_cb->yypvt[-2].psl_strtype, ' ', sizeof(DB_TAB_NAME), (char *) &tabname);
	status = psl_rngent(&cb->pss_auxrng, -1, "", &tabname, cb,
	    FALSE, &resrange, psq_cb->psq_mode, &psq_cb->psq_error,
	    &rngvar_info);
	if (status != E_DB_OK)
	    return (status);

	dmu_cb->dmu_nphys_parts = resrange->pss_tabdesc->tbl_nparts;

	do	    /* something to break out of */
	{
	    /*
	    ** If table resides in multiple locations MODIFY RELOCATE
	    ** must be used.
	    */
	    if (resrange->pss_tabdesc->tbl_status_mask & DMT_MULTIPLE_LOC)
	    {
		err_num = 6207L;
		break;
	    }

	    cb->pss_resrng = resrange;

	    /* Put it in the control block */
	    STRUCT_ASSIGN_MACRO(resrange->pss_tabid, dmu_cb->dmu_tbl_id);

	    /*
	    ** Allocate 1 location entry (`RELOCATE' is not supposed to handle
	    ** multiple locations - this is done by `MODIFY'.
	    */
	    status = psf_malloc(cb, &cb->pss_ostream, sizeof(DB_LOC_NAME),
		(PTR *) &dmu_cb->dmu_location.data_address, &psq_cb->psq_error);
	    if (status != E_DB_OK)
		return (status);

	    /* Store the new location name in the control block */
	    STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(DB_LOC_NAME),
		(char *) dmu_cb->dmu_location.data_address);
	    dmu_cb->dmu_location.data_in_size = sizeof(DB_LOC_NAME);

	    /* Indicate no old locations. */
	    dmu_cb->dmu_olocation.data_in_size = 0;

	    /* Don't allow relocation of someone else's table */
	    if (MEcmp((char *) &resrange->pss_ownname,
		(char *) &cb->pss_user, sizeof(DB_OWN_NAME)))
	    {
		must_audit = TRUE;
		err_num    = 6204L;
	    }

	    /* Don't allow relocation of views */
	    else if (resrange->pss_tabdesc->tbl_status_mask & DMT_VIEW)
	    {
		err_num = 6205L;
	    }
	    
	    /* leave_loop has already been set to TRUE */
	} while (!leave_loop);

	if (err_num != 0L)
	{
	    if ( Psf_srvblk->psf_capabilities & PSF_C_C2SECURE && must_audit )
	    {
		/* Must audit RELOCATE failure. */
		DB_STATUS   local_status;
		DB_ERROR    e_error;
		
	    	local_status = psy_secaudit(FALSE, cb,
	    		(char *)&resrange->pss_tabdesc->tbl_name,
			&resrange->pss_tabdesc->tbl_owner,
	    		sizeof(DB_TAB_NAME), SXF_E_TABLE,
	      		I_SX2026_TABLE_ACCESS, SXF_A_FAIL | SXF_A_RELOCATE,
	      		&e_error);
	    }

	    (VOID) psf_error(err_num, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1,
		psf_trmwhite(sizeof(DB_TAB_NAME), 
		    (char *) &resrange->pss_tabname),
		&resrange->pss_tabname);
	    return (E_DB_ERROR);
	}
    } break;

case 332:
if (!((*yyvarspp)->bypass_actions))
/* # line 5547 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_QNODE              *tidnode;
	PST_VAR_NODE	       tidvar;
	PST_QNODE	       *rsdmnode;
	PST_RSDM_NODE	       resdom;
	PST_QNODE	       *rootnode;
	PST_RT_NODE	       root;
	PST_QTREE	       *tree;
	DB_STATUS	       status;
	i4		       err_code;
	i4		       mask;
	i4		       mask2;
	i4		       qrymod_resp_mask;
	PST_PROCEDURE	       *pnode;
	i4                mask3;

	/* Set up a var. node representing the tid */
	tidvar.pst_vno = yacc_cb->yypvt[-2].psl_rngtype->pss_rgno;
	tidvar.pst_atno.db_att_id = 0;	    /* tid is always column 0 */
	STmove(((*cb->pss_dbxlate & CUI_ID_REG_U) ? "TID" : "tid"),
	       ' ', DB_MAXNAME, tidvar.pst_atname.db_att_name);

	status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
	    (PST_QNODE *) NULL, PST_VAR, (char *) &tidvar, sizeof(tidvar),
	    DB_TID_TYPE, (i2) 0, (i4) DB_TID8_LENGTH, (DB_ANYTYPE *) NULL,
	    &tidnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);	    /* non-zero return means error */
	}

	/* Set up a resdom result domain representing a tid. */
	resdom.pst_rsno = cb->pss_rsdmno++; /* Count resdoms */
	resdom.pst_ntargno = resdom.pst_rsno;
	resdom.pst_ttargtype = PST_ATTNO;
	resdom.pst_rsupdt = FALSE;
	resdom.pst_rsflags = 0; /* no PST_RS_PRINT */

	status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
	    tidnode, PST_RESDOM, (PTR) &resdom, sizeof(resdom), DB_TID_TYPE,
	    (i2) 0, (i4) DB_TID8_LENGTH, (DB_ANYTYPE *) NULL, &rsdmnode,
	    &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);	/* non-zero return means error */
	}

	/* prepend the tid node to the target list */
	(VOID) pst_tlprpnd(rsdmnode, yacc_cb->yypvt[-1].psl_trtype);

	/* Make the root node */
	root.pst_rtuser = TRUE;
	root.pst_union.pst_next  = 0;
	root.pst_dups   = PST_ALLDUPS;

	status = pst_node(cb, &cb->pss_ostream, yacc_cb->yypvt[-1].psl_trtype, yacc_cb->yypvt[-0].psl_trtype, PST_ROOT,
	    (PTR) &root, sizeof(PST_RT_NODE), DB_INT_TYPE, (i2) 0, (i4) 4,
	    (DB_ANYTYPE *) NULL, &rootnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);	/* non-zero return means error */
	}

	/* Now check the target list to see if there are any
	** resdoms with pst_ttargtype of PST_USER, if so
	** convert them to PST_ATTNO.
	*/
	for (rsdmnode = rootnode->pst_left;
	 rsdmnode != (PST_QNODE *) NULL && 
	 rsdmnode->pst_sym.pst_type == PST_RESDOM;
	 rsdmnode = rsdmnode->pst_left)
	{
	    if (rsdmnode->pst_sym.pst_value.pst_s_rsdm.pst_ttargtype
		== PST_USER)
	    {
		rsdmnode->pst_sym.pst_value.pst_s_rsdm.pst_ttargtype
		    = PST_ATTNO;
	    }
	}

	if (cb->pss_defqry == PSQ_DEFQRY)
	{
	    /*
	    ** now call psl_shareable() to determine if the query satisfies SQL
	    ** definition of shareability (i.e. all tables used in the query are
	    ** catalogs or are owned by the DBA; QUEL user cannot explicitly
	    ** qualify them with an owner name)
	    */
	    status = psl_shareable(psq_cb, cb, &yyvarsp->qp_shareable, &yyvarsp->shr_qryinfo);
	    if (DB_FAILURE_MACRO(status))
		return(status);
	}

	/* Apply the qrymod algorithm (views, permits, integrities) */
	status = psy_qrymod(rootnode, cb, psq_cb, &yyvarsp->join_id,
	    &qrymod_resp_mask);
	if (status != E_DB_OK)
	    return (status);

	/*
	** if this is a repeat query and it is shareable according to SQL
	** definition of shareability, apply QUEL-specific checking
	*/
	if (cb->pss_defqry == PSQ_DEFQRY && yyvarsp->qp_shareable)
	{
	    psl_quel_shareable(cb, yyvarsp->qry_mask, &yyvarsp->qp_shareable);
	}

	mask = cb->pss_resrng->pss_tabdesc->tbl_status_mask;
	mask2 = cb->pss_resrng->pss_tabdesc->tbl_2_status_mask;

	/* Make sure base table is updateable;
	** In case of an index or a non-extended catalog,
	** catalog update privilege must be on.
	** In case of security catalog session must have security 
	** privilege active.
	** Bug 34079, since 6.0 you were never supposed to be 
        ** able to update a secondary index.  Fixed it so 
        ** the same error is returned whether you have update 
        ** syscat or not (jennifer)
	**
	** if PSS_REPAIR_SYSCAT and PSS_CATUPD are set in cb->pss_ses_flag,
	** allow REPLACE on indexes which are non-extended catalogs
	*/
	if (mask & DMT_IDX)
	{
	    if (!(   mask & DMT_CATALOG && ~mask & DMT_EXTENDED_CAT
	          && cb->pss_ses_flag & PSS_CATUPD
		  && cb->pss_ses_flag & PSS_REPAIR_SYSCAT
		 )
	       )
	    {
		_VOID_ psf_error(2106L, 0L, PSF_USERERR, &err_code,
		    &psq_cb->psq_error, 1,
		    psf_trmwhite(sizeof(DB_TAB_NAME),
			(char *) &cb->pss_resrng->pss_tabname),
		    &cb->pss_resrng->pss_tabname);
		return(E_DB_ERROR);
	    }
	}
	else if ( ( (mask & DMT_CATALOG || mask2 & DMT_TEXTENSION)
		 && ~mask & DMT_EXTENDED_CAT
		 && !(cb->pss_ses_flag & PSS_CATUPD))
	     ||
	        ((mask & DMT_SECURE) &&
	         !(cb->pss_ustat & DU_USECURITY)))
	{
		i4 local_status;
		DB_ERROR	    e_error;

		if ( Psf_srvblk->psf_capabilities & PSF_C_C2SECURE )
		{
		    local_status = psy_secaudit(FALSE, cb,
			    (char *)&cb->pss_resrng->pss_tabname,
			    &cb->pss_resrng->pss_tabdesc->tbl_owner,
			    sizeof(DB_TAB_NAME), 
			    SXF_E_TABLE,
			    I_SX2020_PROT_REJECT, 
			    SXF_A_FAIL|SXF_A_UPDATE, 
			    &e_error);

		    if (local_status > status)
			status = local_status;
		}

		/* process security_alarms */
		local_status = psy_do_alarm_fail(cb, 
				cb->pss_resrng, PSQ_REPLACE, &e_error);
		if (local_status > status)
		    status = local_status;
	    _VOID_ psf_error(2107L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 2, sizeof(cb->pss_lineno), &cb->pss_lineno,
		psf_trmwhite(sizeof(DB_TAB_NAME), 
		    (char *) &cb->pss_resrng->pss_tabname),
		&cb->pss_resrng->pss_tabname);
	    return (E_DB_ERROR);
	}

	mask3=PST_0FULL_HEADER,

	/* Make the header */
	status = pst_header(cb, psq_cb, PST_UNSPECIFIED, (PST_QNODE *) NULL,
	    rootnode, &tree, &pnode, mask3, (PSS_Q_XLATE *) NULL);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	/* set pst_numjoins.  $Yjoin_id contains the highest join id */
	tree->pst_numjoins = yyvarsp->join_id;

	/* Fix the root in QSF */
	status = psf_mroot(cb, &cb->pss_ostream, (PTR) pnode, &psq_cb->psq_error);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_qttype = tree;
    } break;

case 333:
if (!((*yyvarspp)->bypass_actions))
/* # line 5750 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;

	psq_cb->psq_mode = PSQ_REPLACE;

	/* Open memory stream for allocating query tree */
	if ((status = psf_mopen(cb, QSO_QTREE_OBJ, &cb->pss_ostream,
	    &psq_cb->psq_error)) != E_DB_OK)
	{
	    return (status);    /* non-zero return means error */
	}
    } break;

case 334:
if (!((*yyvarspp)->bypass_actions))
/* # line 5765 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4	err_code;

	/* Updates on secondary index tables that are btrees
	** are not allowed.
	*/
	if (yacc_cb->yypvt[-0].psl_rngtype->pss_tabdesc->tbl_status_mask & DMT_IDX
	    &&
	    yacc_cb->yypvt[-0].psl_rngtype->pss_tabdesc->tbl_storage_type == DMT_BTREE_TYPE
	   )
	{
	    (VOID) psf_error(2124L, 0L, PSF_USERERR,
		&err_code, &psq_cb->psq_error, 2,
		sizeof(cb->pss_lineno), &cb->pss_lineno,
		psf_trmwhite(sizeof(DB_TAB_NAME), (char *) &yacc_cb->yypvt[-0].psl_rngtype->pss_tabname),
		&yacc_cb->yypvt[-0].psl_rngtype->pss_tabname);
	    return(E_DB_ERROR);	    
	}

	/*
	** On a replace, remember what the result range variable is.
	** This can't be stored in the result range variable slot in
	** the user range table, because that is not for "real" range
	** variables, only for result tables (as in an append).
	*/
	cb->pss_resrng = yacc_cb->yypvt[-0].psl_rngtype;
	yacc_cb->yyval.psl_rngtype = yacc_cb->yypvt[-0].psl_rngtype;
    } break;

case 335:
if (!((*yyvarspp)->bypass_actions))
/* # line 5794 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4		    rngvar_info;
	i4		    err_code;
	DB_STATUS           status;
	PSS_RNGTAB	    *rngtab;
	DB_TAB_NAME	    tabname;
	DB_OWN_NAME	    sessname;

	if (STcasecmp(yacc_cb->yypvt[-2].psl_strtype, "session"))
	{
	    (VOID) psf_error(5153, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1,
		psf_trmwhite(sizeof(DB_OWN_NAME), 
		    (char *) yacc_cb->yypvt[-2].psl_strtype), yacc_cb->yypvt[-2].psl_strtype);
	    return (E_DB_ERROR);
	}
	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(tabname), (char *) &tabname);
	STRUCT_ASSIGN_MACRO(cb->pss_sess_owner, sessname);
	status = psl_orngent(&cb->pss_usrrange, -1, yacc_cb->yypvt[-2].psl_strtype, &sessname, &tabname, 
	    cb, FALSE, &rngtab, psq_cb->psq_mode, &psq_cb->psq_error,
	    &rngvar_info);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	/*
	** On a replace, remember what the result range variable is.
	** This can't be stored in the result range variable slot in
	** the user range table, because that is not for "real" range
	** variables, only for result tables (as in an append).
	*/
	cb->pss_resrng = rngtab;
	yacc_cb->yyval.psl_rngtype = rngtab;
    } break;

case 336:
if (!((*yyvarspp)->bypass_actions))
/* # line 5832 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;
	PST_RT_NODE	       root;
	PST_QNODE	       *rootnode, *rsdmnode;
	PST_QTREE	       *tree;
	i4		       err_code;
	i4		       qrymod_resp_mask;
	PST_PROCEDURE	       *pnode;

	/* make sure that no range vars were used in the target list */
	if (cb->pss_usrrange.pss_maxrng != -1)
	{
	    (VOID) psf_error(2212L, 0L, PSF_USERERR,
		&err_code, &psq_cb->psq_error, 1, sizeof(cb->pss_lineno),
		&cb->pss_lineno);
	    return (E_DB_ERROR);
	}
	/* Allocate root node */
	root.pst_rtuser = TRUE;
	root.pst_union.pst_next  = 0;
	root.pst_dups   = PST_ALLDUPS;

	status = pst_node(cb, &cb->pss_ostream, yacc_cb->yypvt[-0].psl_trtype, (PST_QNODE *) NULL,
	    PST_ROOT, (PTR) &root, sizeof(root), DB_NODT, (i2) 0, (i4) 0,
	    (DB_ANYTYPE *) NULL, &rootnode, &psq_cb->psq_error, (i4) 0);
	if (DB_FAILURE_MACRO(status))
	{
	    return (status);
	}

	/* Now check the target list to see if there are any
	** resdoms with pst_ttargtype of PST_USER, if so
	** convert them to PST_ATTNO.
	*/
	for (rsdmnode = rootnode->pst_left;
	 rsdmnode != (PST_QNODE *) NULL && 
	 rsdmnode->pst_sym.pst_type == PST_RESDOM;
	 rsdmnode = rsdmnode->pst_left)
	{
	    if (rsdmnode->pst_sym.pst_value.pst_s_rsdm.pst_ttargtype
		== PST_USER)
	    {
		rsdmnode->pst_sym.pst_value.pst_s_rsdm.pst_ttargtype
		    = PST_ATTNO;
	    }
	}

	status = psy_qrymod(rootnode, cb, psq_cb, &yyvarsp->join_id,
	    &qrymod_resp_mask);
	if (DB_FAILURE_MACRO(status))
	    return (status);

	/*
	** if processing REPLACE CURSOR, we delayed replacing PST_VAR with
	** PST_CURVAL until now to make life easier for psy_view()
	*/
	for (rsdmnode = rootnode->pst_left;
	     rsdmnode != (PST_QNODE *) NULL &&
		rsdmnode->pst_sym.pst_type == PST_RESDOM;
	     rsdmnode = rsdmnode->pst_left)
	{
	    status = psl_curval(cb, psq_cb, cb->pss_crsr, &rsdmnode->pst_right);
	    if (DB_FAILURE_MACRO(status))
	    {
		return (status);
	    }
	}

	/* Generate query tree header */
	status = pst_header(cb, psq_cb, PST_UNSPECIFIED, (PST_QNODE *) NULL,
	    rootnode, &tree, &pnode, PST_0FULL_HEADER, (PSS_Q_XLATE *) NULL);
	if (DB_FAILURE_MACRO(status))
	    return (status);

	/* set pst_numjoins.  $Yjoin_id contains the highest join id */
	tree->pst_numjoins = yyvarsp->join_id;

	/* Fix the root in QSF */
	status = psf_mroot(cb, &cb->pss_ostream, (PTR) pnode, &psq_cb->psq_error);
	if (DB_FAILURE_MACRO(status))
	    return (status);
    } break;

case 337:
if (!((*yyvarspp)->bypass_actions))
/* # line 5917 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSC_CURBLK          *cursor;
	DB_STATUS	    status;
	i4		    err_code;
	i4		    rngvar_info;

	psq_cb->psq_mode = PSQ_REPCURS;

	/* Look up the cursor */
	status = psq_crfind(cb, yacc_cb->yypvt[-0].psl_cursid, &cursor, &psq_cb->psq_error);
	if (DB_FAILURE_MACRO(status))
	    return (status);

	/* Complain if not found */
	if (cursor == (PSC_CURBLK *) NULL || cursor->psc_open == FALSE)
	{
	    (VOID) psf_error(2205L, 0L, PSF_USERERR,
		&err_code, &psq_cb->psq_error, 2, sizeof(cb->pss_lineno),
		&cb->pss_lineno, DB_MAXNAME, yacc_cb->yypvt[-0].psl_cursid->db_cur_name);
	    return (E_DB_ERROR);
	}

	/* Check if cursor updateable */
	if (cursor->psc_forupd == FALSE)
	{
	    (VOID) psf_error(2217L, 0L, PSF_USERERR,
		&err_code, &psq_cb->psq_error, 1,
		psf_trmwhite(DB_MAXNAME, cursor->psc_blkid.db_cur_name),
		cursor->psc_blkid.db_cur_name);
	    return (E_DB_ERROR);
	}

	/* Open memory stream in QSF for query tree */
	status = psf_mopen(cb, QSO_QTREE_OBJ, &cb->pss_ostream,
	    &psq_cb->psq_error);
	if (DB_FAILURE_MACRO(status))
	{
	    return (status);
	}

	/*
	** obtain description of the table over which the cursor was defined and
	** make cb->pss_resrng point at it - will come in real handy when  we
	** encounter column references on the RHS of assignment + in
	** psy_view(), psy_protect(), and psy_rules() 
	*/
	status = psl_orngent(&cb->pss_auxrng, -1, "!",
	    &cursor->psc_ownnm, &cursor->psc_tabnm,
	    cb, FALSE, &cb->pss_resrng, psq_cb->psq_mode,
	    &psq_cb->psq_error, &rngvar_info);
	if (DB_FAILURE_MACRO(status))
	    return (status);

	/* Hand cursor id back to caller */
	STRUCT_ASSIGN_MACRO(*(yacc_cb->yypvt[-0].psl_cursid), psq_cb->psq_cursid);
    } break;
	}
	return(0);
}

i4
psl7iftn(yacc_cb, yyrule, yystatusp, yyvarspp, cb, psq_cb)
YACC_CB		*yacc_cb;
i4		yyrule;
PSS_SESBLK	*cb;
DB_STATUS	*yystatusp;
YYVARS		**yyvarspp;
PSQ_CB *	psq_cb;
{
    YYVARS	*yyvarsp = *yyvarspp;
	switch(yyrule)
	{
case 338:
if (!((*yyvarspp)->bypass_actions))
/* # line 5976 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSC_CURBLK             *cursor;
	DB_STATUS	       status;
	i4		       err_code;
	QEF_RCB		       *qef_rcb;

	psq_cb->psq_mode = PSQ_RETCURS;

	/* Look up the cursor */
	status = psq_crfind(cb, yacc_cb->yypvt[-0].psl_cursid, &cursor, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	/* Complain if not found */
	if (cursor == (PSC_CURBLK *) NULL || cursor->psc_open == FALSE)
	{
	    (VOID) psf_error(2205L, 0L, PSF_USERERR,
		&err_code, &psq_cb->psq_error, 2, sizeof(cb->pss_lineno),
		&cb->pss_lineno, DB_MAXNAME, yacc_cb->yypvt[-0].psl_cursid->db_cur_name);
	    return (E_DB_ERROR);
	}

	/* Hand the cursor id back to the caller */
	STRUCT_ASSIGN_MACRO(*(yacc_cb->yypvt[-0].psl_cursid), psq_cb->psq_cursid);

	/* Allocate the QEF control block for fetch cursor */
	status = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(QEF_RCB), &cb->pss_object,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_mroot(cb, &cb->pss_ostream, cb->pss_object,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	/* Fill in the QEF control block */
    	qef_rcb = (QEF_RCB *) cb->pss_object;
	qef_rcb->qef_length = sizeof(QEF_RCB);
	qef_rcb->qef_type = QEFRCB_CB;
	qef_rcb->qef_owner = (PTR)DB_PSF_ID;
	qef_rcb->qef_ascii_id = QEFRCB_ASCII_ID;
	qef_rcb->qef_sess_id = cb->pss_sessid;
    } break;

case 339:
if (!((*yyvarspp)->bypass_actions))
/* # line 6026 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS       status, local_status;
	DB_ERROR	e_error;
	PST_QNODE	*rootnode;
	PST_QNODE	*ssnode;
	PST_RESKEY	*reskey;
	PST_RT_NODE	root;
	PST_QTREE	*tree;
	i2		dup_val;
	i4		err_code;
	i4		error = 0;
	char		*withentry;
	PST_PROCEDURE   *pnode;
	PST_QNODE	*resdom;
	bool		 found;
	char		obj_name[sizeof(DB_TAB_NAME)];
 
	/* Verify CREATE_TABLE privileges for RETRIEVE INTO*/
	
	if ((psq_cb->psq_mode == PSQ_RETINTO) &&
	    ((status = psy_ckdbpr(psq_cb, (u_i4)DBPR_TAB_CREATE))
		    != E_DB_OK)
	   )
	{
	    if ( Psf_srvblk->psf_capabilities & PSF_C_C2SECURE )
	    {
		STmove(yacc_cb->yypvt[-0].psl_strtype, 
		       ' ', sizeof(DB_TAB_NAME), 
		       obj_name);
		
		local_status = psy_secaudit(FALSE, cb,
			    obj_name,
			    &cb->pss_user,
			    sizeof(DB_TAB_NAME), SXF_E_TABLE,
			    I_SX201E_TABLE_CREATE, SXF_A_FAIL | SXF_A_CONTROL,
			    &e_error);
	    }

	    (VOID) psf_error(6245L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 0);
	    return (status);
	}

	/* Make sure there is a location in case of a RETRIEVE INTO. */
	if ((psq_cb->psq_mode == PSQ_RETINTO) &&
	    !cb->pss_restab.pst_resloc.data_in_size
	   )
	{
	    /* Default location */
	    STmove("$default", ' ', sizeof(DB_LOC_NAME),
		(char *) cb->pss_restab.pst_resloc.data_address);
	    cb->pss_restab.pst_resloc.data_in_size = sizeof(DB_LOC_NAME);
	}

	/* Make the root node */
	root.pst_rtuser = TRUE;
	root.pst_union.pst_next  = 0;

	/* with_dups is set in retwithcls to establish duplicate characteristics
	** for retrieve into statements (this initial value may be altered
	** below.
	*/
	root.pst_dups   = yyvarsp->with_dups;

	status = pst_node(cb, &cb->pss_ostream, yacc_cb->yypvt[-3].psl_trtype, yacc_cb->yypvt[-2].psl_trtype, PST_ROOT, (PTR) &root,
	    sizeof(PST_RT_NODE), DB_NODT, (i2) 0, (i4) 0, (DB_ANYTYPE *) NULL,
	    &rootnode, &psq_cb->psq_error, (i4) 0);
	if (DB_FAILURE_MACRO(status))
	    return (status);

	if (psq_cb->psq_mode == PSQ_RETINTO)
	{
	    PST_QNODE	    *res2;
	    PSS_RNGTAB	    *rngtabp;
	    DMT_ATT_ENTRY   *attribute;
	    char	    *name;

	    for (resdom = rootnode->pst_left;
	     resdom != (PST_QNODE *) NULL && 
	     resdom->pst_sym.pst_type == PST_RESDOM;
	     resdom = resdom->pst_left)
	    {
		/* Now check the target list to see if there are any
		** resdoms with pst_ttargtype of PST_USER, if so
		** convert them to PST_ATTNO.
		*/
		if (resdom->pst_sym.pst_value.pst_s_rsdm.pst_ttargtype
		    == PST_USER)
		{
		    resdom->pst_sym.pst_value.pst_s_rsdm.pst_ttargtype
			= PST_ATTNO;
		}

		/* check for duplicate names */
		for (res2 = resdom->pst_left; res2 != (PST_QNODE*) NULL && 
		    res2->pst_sym.pst_type == PST_RESDOM; res2 = res2->pst_left)
		{
		    if (!MEcmp(resdom->pst_sym.pst_value.pst_s_rsdm.pst_rsname, 
			res2->pst_sym.pst_value.pst_s_rsdm.pst_rsname,
			DB_MAXNAME))
		    {
			(VOID) psf_error(5105L, 0L, PSF_USERERR, &err_code,
			    &psq_cb->psq_error, 2, 
			    sizeof(cb->pss_restab.pst_resname),
			    (char*)&cb->pss_restab.pst_resname,
		    sizeof(resdom->pst_sym.pst_value.pst_s_rsdm.pst_rsname),
		    (char*)resdom->pst_sym.pst_value.pst_s_rsdm.pst_rsname);
			    return (E_DB_ERROR);
			
		    }
		}

		/* To avoid creation of columns of LONG TEXT type in case of
		** retrieve into tab (col = NULL) following check is performed.
		*/
		if ((resdom->pst_right->pst_sym.pst_type == PST_CONST) &&
		    (abs(resdom->pst_right->pst_sym.pst_dataval.db_datatype)
							    == DB_LTXT_TYPE))
		{
		    (VOID) psf_error(2142L, 0L, PSF_USERERR,
			&err_code, &psq_cb->psq_error, 0);
		    return (E_DB_ERROR);
		}

		/*
		** Set up defaultability.
		**
		** If the resdom is based directly on a column
		** (i.e. is a PST_VAR), we copy that column's default value;
		** otherwise the resdom must be based on a calculated column or
		** expression, so set up the 'default' QUEL default.
		*/
		if (resdom->pst_right->pst_sym.pst_type != PST_VAR)
		{
		    /* resdom based on expression--use the 'default'
		    ** QUEL default:
		    **     unspecified (null)  for nullable columns
		    **     WITH DEFAULT        for non-nullable columns
		    */ 
		    DB_DATA_VALUE  *dbdata;
		    DMU_ATTR_ENTRY attr;

		    dbdata = &resdom->pst_right->pst_sym.pst_dataval;
		    
		    if (dbdata->db_datatype < 0)
		    {
			/* datatype is nullable;
			** use unspecified default (which is null)
			*/
			SET_CANON_DEF_ID(
				resdom->pst_sym.pst_value.pst_s_rsdm.pst_defid, 
				DB_DEF_NOT_SPECIFIED);
		    }
		    else
		    {
			/* datatype is non-nullable;
			** set up attribute descriptor and get ingres default
			** id for this tuple
			*/
			MEfill(sizeof(attr), 0, (PTR) &attr);
	
			attr.attr_type      = dbdata->db_datatype;
			attr.attr_size      = dbdata->db_length;
			attr.attr_precision = dbdata->db_prec;
			
			status = psl_2col_ingres_default(cb, &attr, 
							 &psq_cb->psq_error);
			if (DB_FAILURE_MACRO(status))
			    return(status);
			
			COPY_DEFAULT_ID(attr.attr_defaultID,
			       resdom->pst_sym.pst_value.pst_s_rsdm.pst_defid);
		    }  /* end else non-nullable datatype */
		}  
		else	/* resdom is based on a column of underlying table */
		{
		    name = resdom->pst_sym.pst_value.pst_s_rsdm.pst_rsname;

		    /* find range table entry 
		     */
		    status = pst_rgfind(
					&cb->pss_usrrange, &rngtabp,
					resdom->pst_right->pst_sym
						.pst_value.pst_s_var.pst_vno,
					&psq_cb->psq_error);
		    if (status != E_DB_OK)
		    {
			(VOID) psf_error(E_PS0909_NO_RANGE_ENTRY, 
					 0L, PSF_INTERR,
					 &err_code, &psq_cb->psq_error, 1,
					 psf_trmwhite(DB_MAXNAME, name), name);
			return (E_DB_ERROR);
		    }

		    /* Look up the attribute 
		     */
		    attribute = pst_coldesc(rngtabp,
					    &resdom->pst_right->pst_sym
					       .pst_value.pst_s_var.pst_atname);

		    /* Check for attribute not found 
		     */
		    if (attribute == (DMT_ATT_ENTRY *) NULL)
		    {
			(VOID) psf_error(E_PS090A_NO_ATTR_ENTRY, 0L, PSF_INTERR,
					 &err_code, &psq_cb->psq_error, 1,
					 psf_trmwhite(DB_MAXNAME, name), name);
			return (E_DB_ERROR);
		    }
		    COPY_DEFAULT_ID(attribute->att_defaultID,
				resdom->pst_sym.pst_value.pst_s_rsdm.pst_defid);
	    
		} /* end else resdom is based on a column of underlying table */

	    }  /* end for resdom->pst_sym.pst_type == PST_RESDOM */

	    /* min or max pages requires hashed relation
	    ** data fillfactor can't use heap.
	    ** leaffill and index fill require btree.
	    */
	    if ((cb->pss_restab.pst_minpgs > 0) &&
		(cb->pss_restab.pst_struct != DB_HASH_STORE))
	    {
		error++;
		withentry = "MINPAGES";
	    }
	    else if ((cb->pss_restab.pst_maxpgs > 0) &&
		(cb->pss_restab.pst_struct != DB_HASH_STORE))
	    {
		error++;
		withentry = "MAXPAGES";
	    }
	    else if ((cb->pss_restab.pst_fillfac > 0) && 
		 (cb->pss_restab.pst_struct == DB_HEAP_STORE))
	    {
		error++;
		withentry = "FILLFACTOR";
	    }
	    else if ((cb->pss_restab.pst_leaff > 0) &&
		(cb->pss_restab.pst_struct != DB_BTRE_STORE))
	    {
		error++;
		withentry = "LEAFFILL or INDEXFILL";
	    }
	    else if ((cb->pss_restab.pst_nonleaff > 0) &&
		(cb->pss_restab.pst_struct != DB_BTRE_STORE))
	    {
		error++;
		withentry = "NONLEAFFILL";
	    }
	    else if ((cb->pss_restab.pst_reskey != NULL) &&
		(cb->pss_restab.pst_struct == DB_HEAP_STORE))
	    {
		error++;
		withentry = "KEY";
	    }

	    if (error)
	    {
		(VOID) psf_error(2152L, 0L, PSF_USERERR,
		    &err_code, &psq_cb->psq_error, 1,
		    (i4) STlength(withentry), withentry);
		return (E_DB_ERROR);
	    }
	}
	else if (yyvarsp->check_for_vlups)
	{
	    /* we need to check for VLUPs in the target list of RETRIEVE */
	    for (resdom = rootnode->pst_left;
	         resdom != (PST_QNODE *) NULL && 
		 resdom->pst_sym.pst_type == PST_RESDOM;
		 resdom = resdom->pst_left)
	    {
		if (resdom->pst_sym.pst_dataval.db_length == ADE_LEN_UNKNOWN)
		{
		    /* We have found a VLUP that will cause an AV.
		    ** Flag it for now.
		    */
		    (VOID) psf_error(2218L, 0L, PSF_USERERR,
			&err_code, &psq_cb->psq_error, 0);
		    return (E_DB_ERROR);
		}
	    }
	}

	/*
	** Check whether KEY clause has been specified, if so validate it.
	*/
	if (cb->pss_restab.pst_reskey != (PST_RESKEY *) NULL)
	{
	    for (reskey = cb->pss_restab.pst_reskey, found = FALSE;
		 reskey;
		 reskey = reskey->pst_nxtkey)
	    {
                for (ssnode = rootnode->pst_left;
                     ssnode->pst_sym.pst_type != PST_TREE;
                     ssnode = ssnode->pst_left
                    )
                {
                    /* compare domain names */
                    if (MEcmp((PTR) &reskey->pst_attname,
                        (PTR) ssnode->pst_sym.pst_value.pst_s_rsdm.pst_rsname,
                         sizeof (reskey->pst_attname)) == 0)
                    {
                        found++;
                        break;
                    }
                }
                if (found == FALSE)
                {   /* bogus column name */
                    (VOID) psf_error(4611L, 0L, PSF_USERERR, &err_code,
                        &psq_cb->psq_error, 0);
                    return (E_DB_ERROR);
                }
		status = psl_check_key(cb, &psq_cb->psq_error,
			    (DB_DT_ID) ssnode->pst_sym.pst_dataval.db_datatype);
		if (status)
		{
		    (VOID) psf_error(2179L, 0L, PSF_USERERR, &err_code,
			&psq_cb->psq_error, 2,
			sizeof(cb->pss_lineno), &cb->pss_lineno,
			psf_trmwhite(sizeof(DB_ATT_NAME),
			    (char*) &reskey->pst_attname),
			(char *) &reskey->pst_attname);
			return (E_DB_ERROR);
		}
	    }
	}
	else if (cb->pss_restab.pst_struct &&
		    (cb->pss_restab.pst_struct != DB_HEAP_STORE))
	{
	    /* Must check that implicit key is keyable */

	    /* First, find it */
	    
	    for (ssnode = rootnode->pst_left;
		 ssnode->pst_sym.pst_type != PST_TREE;
		 ssnode = ssnode->pst_left
		)
	    {
		if (	(ssnode->pst_sym.pst_type == PST_RESDOM)
		    &&	(ssnode->pst_sym.pst_value.pst_s_rsdm.pst_rsno == 1))
		{
		    status = psl_check_key(cb, &psq_cb->psq_error,
				(DB_DT_ID) ssnode->pst_sym.pst_dataval.db_datatype);
		    if (status)
		    {
			(VOID) psf_error(2179L, 0L, PSF_USERERR, &err_code,
			    &psq_cb->psq_error, 2,
			    sizeof(cb->pss_lineno), &cb->pss_lineno,
			    psf_trmwhite(sizeof(DB_ATT_NAME),
				(char*) &ssnode->pst_sym.pst_value.pst_s_rsdm.pst_rsname),
			    (char *) &ssnode->pst_sym.pst_value.pst_s_rsdm.pst_rsname);
			    return (E_DB_ERROR);
		    }
		    break;
		}
	    }
	}

	/* No result range variable on a retrieve */
	cb->pss_resrng = (PSS_RNGTAB *) NULL;

	/*
	** Apply the qrymod algorithm (views, permits, and integrities).
	** If the query mode isn't PSQ_RETRIEVE, this must be a cursor
	** statement, in which case we want to defer qrymod until we know
	** which columns are for update.
	*/
	if (psq_cb->psq_mode == PSQ_RETRIEVE ||
	    psq_cb->psq_mode == PSQ_RETINTO)
	{
	    i4		qrymod_resp_mask;

	    if (cb->pss_defqry == PSQ_DEFQRY)
	    {
		/*
		** now call psl_shareable() to determine if the query satisfies
		** SQL definition of shareability (i.e. all tables used in the
		** query are catalogs or are owned by the DBA; QUEL user cannot
		** explicitly qualify them with an owner name)
		*/
		status = psl_shareable(psq_cb, cb, &yyvarsp->qp_shareable,
		    &yyvarsp->shr_qryinfo);
		if (DB_FAILURE_MACRO(status))
		    return(status);
	    }

	    status = psy_qrymod(rootnode, cb, psq_cb, &yyvarsp->join_id,
		&qrymod_resp_mask);
	    if (status != E_DB_OK)
		return (status);

	    /*
	    ** if this is a repeat query and it is shareable according to SQL
	    ** definition of shareability, apply QUEL-specific checking
	    */
	    if (cb->pss_defqry == PSQ_DEFQRY && yyvarsp->qp_shareable)
	    {
		psl_quel_shareable(cb, yyvarsp->qry_mask, &yyvarsp->qp_shareable);
	    }
	}

        /* Make and fill in the query tree header */
        status = pst_header(cb, psq_cb, PST_UNSPECIFIED, yacc_cb->yypvt[-1].psl_trtype, rootnode,
	    &tree, &pnode, PST_0FULL_HEADER, (PSS_Q_XLATE *) NULL);
    	if (DB_FAILURE_MACRO(status))
	    return (status);

	/* Make sure the procedure gets updated later if cursor. */
	if (psq_cb->psq_mode == PSQ_RETRIEVE ||
	    psq_cb->psq_mode == PSQ_RETINTO)
	{
	    psq_cb->psq_pnode = (PTR)NULL;
	}
	else 
	    psq_cb->psq_pnode = (PTR)pnode;

	/* Tell whether retrieve unique */
	if (yacc_cb->yypvt[-4].psl_inttype == TRUE || yyvarsp->sort_by)
	    dup_val = PST_NODUPS;
	else if (yyvarsp->with_dups == PST_DNTCAREDUPS)
	    dup_val = PST_ALLDUPS;
	else
	    dup_val = yyvarsp->with_dups;
	
	for (;rootnode; rootnode = rootnode->
	     pst_sym.pst_value.pst_s_root.pst_union.pst_next
	    )
	{
	    rootnode->pst_sym.pst_value.pst_s_root.pst_dups = dup_val;
	}
	    
        yacc_cb->yyval.psl_qttype = tree;

	/* set pst_numjoins.  $Yjoin_id contains the highest join id */
	tree->pst_numjoins = yyvarsp->join_id;

	/* Fix the root in QSF */
	status = psf_mroot(cb, &cb->pss_ostream, (PTR) pnode, &psq_cb->psq_error);
        if (DB_FAILURE_MACRO(status))
	    return (status);
    } break;

case 340:
if (!((*yyvarspp)->bypass_actions))
/* # line 6472 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;

	/* A "define cursor" statement shouldn't be changed to a retrieve */
	if (psq_cb->psq_mode != PSQ_DEFCURS)
	    psq_cb->psq_mode = PSQ_RETRIEVE;

	/* Open memory stream for allocating query tree */
	if ((status = psf_mopen(cb, QSO_QTREE_OBJ, &cb->pss_ostream,
	    &psq_cb->psq_error)) != E_DB_OK)
	{
	    return (status);    /* non-zero return means error */
	}
    } break;

case 341:
if (!((*yyvarspp)->bypass_actions))
/* # line 6489 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	/* "Retrieve into" not allowed for cursors */
	if (psq_cb->psq_mode == PSQ_DEFCURS)
	{
	    i4	err_code;

	    (VOID) psf_error(2209L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 2, sizeof(cb->pss_lineno), &cb->pss_lineno,
		DB_MAXNAME, psq_cb->psq_cursid.db_cur_name);
	    return (E_DB_ERROR);
	}
	else
	{
	    yyvarsp->with_dups = PST_DNTCAREDUPS;
	}
    } break;

case 342:
if (!((*yyvarspp)->bypass_actions))
/* # line 6506 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yyvarsp->with_dups = PST_ALLDUPS;
    } break;

case 343:
if (!((*yyvarspp)->bypass_actions))
/* # line 6512 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_inttype = TRUE;
    } break;

case 344:
if (!((*yyvarspp)->bypass_actions))
/* # line 6516 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_inttype = FALSE; 
    } break;

case 345:
if (!((*yyvarspp)->bypass_actions))
/* # line 6522 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	       status;

	/*
	** Allocate the location entries.  Assume DM_LOC_MAX, although it's
	** probably fewer.  This is because we don't know how many locations
	** we have at this point, and it would be a big pain to allocate
	** less and then run into problems.
	*/
	status = psf_malloc(cb, &cb->pss_ostream, DM_LOC_MAX * sizeof(DB_LOC_NAME),
	    (PTR *) &cb->pss_restab.pst_resloc.data_address,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	cb->pss_restab.pst_resloc.data_in_size = 0; /* Start with 0 locations */

	/* processing RETRIEVE INTO */
	psq_cb->psq_mode = PSQ_RETINTO;
    } break;

case 349:
if (!((*yyvarspp)->bypass_actions))
/* # line 6549 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4     err_code;

	/* User specified WITH clause only allowed on a RETRIEVE INTO */
	if (psq_cb->psq_mode != PSQ_RETINTO)
	{
	    (VOID) psf_error(2141L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 0);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}
    } break;

case 350:
if (!((*yyvarspp)->bypass_actions))
/* # line 6561 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4	    err_code;
	i4	    minp;
	i4	    maxp;

	minp = cb->pss_restab.pst_minpgs;
	maxp = cb->pss_restab.pst_maxpgs;

	if (minp > 0 && maxp > 0 && minp > maxp)
	{
	    (VOID) psf_error(2143L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, (i4) sizeof(minp), &minp);
	    return (E_DB_ERROR);	/* non-zero return means error */
	}

	/* If user has not specified [NO]JOURNALING clause
	** session default should be used.
	*/
	if (!yyvarsp->with_journaling && (cb->pss_ses_flag & PSS_JOURNALING))
	    cb->pss_restab.pst_resjour = TRUE;

	/* If user has NOT specified [NO]DUPLICATES clause,
	** language default should be used.
	*/
	if (yyvarsp->with_dups == PST_DNTCAREDUPS)
	    cb->pss_restab.pst_resdup = FALSE;

    } break;

case 351:
if (!((*yyvarspp)->bypass_actions))
/* # line 6590 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	/* User specified WITH clause only allowed on a RETRIEVE INTO */
	if (psq_cb->psq_mode != PSQ_RETINTO)
	{
	    i4     err_code;

	    (VOID) psf_error(2141L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 0);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}

	if (cb->pss_ses_flag & PSS_JOURNALING)
	{
	    cb->pss_restab.pst_resjour = TRUE;
	}

	/* If user has NOT specified [NO]DUPLICATES clause
	** language default should be used.
	*/
	cb->pss_restab.pst_resdup = FALSE;
    } break;

case 352:
if (!((*yyvarspp)->bypass_actions))
/* # line 6612 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	if (cb->pss_ses_flag & PSS_JOURNALING)
	{
	    cb->pss_restab.pst_resjour = TRUE;
	}

	/* If user has NOT specified [NO]DUPLICATES clause
	** language default should be used.
	*/
	cb->pss_restab.pst_resdup = FALSE;
    } break;

case 359:
if (!((*yyvarspp)->bypass_actions))
/* # line 6636 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4	    err_code;
	i4	    storestruct;
	i4	    compressed = FALSE;

	if (STcompare(yacc_cb->yypvt[-2].psl_strtype, "structure") == 0)
	{
	    char    *ssname = yacc_cb->yypvt[-0].psl_strtype;

	    /* Decode storage structure */
	    if (*ssname == 'c')
	    {
		compressed = TRUE;
		ssname++;
	    }

	    storestruct = uld_struct_xlate(ssname);
	    if (storestruct == 0 || storestruct == DB_RTRE_STORE
	      || storestruct == DB_SORT_STORE)
	    {
		(VOID) psf_error(2144L, 0L, PSF_USERERR, &err_code,
			  &psq_cb->psq_error, 1, (i4) STtrmwhite(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
		return (E_DB_ERROR);    /* non-zero return means error */
	    }

	    cb->pss_restab.pst_struct   = storestruct;
	    cb->pss_restab.pst_compress = compressed;
	}
	else
	{
	    (VOID) psf_error(2139L, 0L, PSF_USERERR,
		&err_code, &psq_cb->psq_error, 2, (i4) sizeof(cb->pss_lineno),
		&cb->pss_lineno, STtrmwhite(yacc_cb->yypvt[-2].psl_strtype), yacc_cb->yypvt[-2].psl_strtype);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}
    } break;

case 360:
if (!((*yyvarspp)->bypass_actions))
/* # line 6675 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4		    err_code;

	if (!STcasecmp(yacc_cb->yypvt[-2].psl_strtype, "fillfactor"))
	{
	    /* Fillfactor must be between 1% and 100% */
	    if (yacc_cb->yypvt[-0].psl_inttype < 1 || yacc_cb->yypvt[-0].psl_inttype > 100)
	    {
		(VOID) psf_error(2145L, 0L, PSF_USERERR, &err_code,
		    &psq_cb->psq_error, 1, (i4) sizeof(yacc_cb->yypvt[-0].psl_inttype), &yacc_cb->yypvt[-0].psl_inttype);
		return (E_DB_ERROR);	/* non-zero return means error */
	    }
	    cb->pss_restab.pst_fillfac = yacc_cb->yypvt[-0].psl_inttype;
	}
	else if (!STcasecmp(yacc_cb->yypvt[-2].psl_strtype, "minpages"))
	{
	    /* minpages must be greater than zero */
            if ((yacc_cb->yypvt[-0].psl_inttype <= 0) || (yacc_cb->yypvt[-0].psl_inttype > DB_MAX_PAGENO))
	    {
		(VOID) psf_error(2146L, 0L, PSF_USERERR, &err_code,
		    &psq_cb->psq_error, 1, (i4) sizeof(yacc_cb->yypvt[-0].psl_inttype), &yacc_cb->yypvt[-0].psl_inttype);
		return (E_DB_ERROR);	/* non-zero return means error */
	    }
	    cb->pss_restab.pst_minpgs = yacc_cb->yypvt[-0].psl_inttype;
	}
	else if (!STcasecmp(yacc_cb->yypvt[-2].psl_strtype, "maxpages"))
	{
	    /* maxpages must be greater than zero */
            if ((yacc_cb->yypvt[-0].psl_inttype <= 0) || (yacc_cb->yypvt[-0].psl_inttype > DB_MAX_PAGENO))
	    {
		(VOID) psf_error(2147L, 0L, PSF_USERERR, &err_code,
		    &psq_cb->psq_error, 1, (i4) sizeof(yacc_cb->yypvt[-0].psl_inttype), &yacc_cb->yypvt[-0].psl_inttype);
		return (E_DB_ERROR);	/* non-zero return means error */
	    }
	    cb->pss_restab.pst_maxpgs = yacc_cb->yypvt[-0].psl_inttype;
	}
	else if (!STcasecmp(yacc_cb->yypvt[-2].psl_strtype, "nonleaffill"))
	{
	    /* Nonleaffill must be between 1% and 100% */
	    if (yacc_cb->yypvt[-0].psl_inttype < 1 || yacc_cb->yypvt[-0].psl_inttype > 100)
	    {
		(VOID) psf_error(2148L, 0L, PSF_USERERR, &err_code,
		    &psq_cb->psq_error, 1, (i4) sizeof(yacc_cb->yypvt[-0].psl_inttype), &yacc_cb->yypvt[-0].psl_inttype);
		return (E_DB_ERROR);	/* non-zero return means error */
	    }
	    cb->pss_restab.pst_nonleaff = yacc_cb->yypvt[-0].psl_inttype;
	}
	else if (!STcasecmp(yacc_cb->yypvt[-2].psl_strtype, "leaffill") ||
		 !STcasecmp(yacc_cb->yypvt[-2].psl_strtype, "indexfill")   )
	{
	    /* Leaffill/indexfill must be between 1% and 100% */
	    if (yacc_cb->yypvt[-0].psl_inttype < 1 || yacc_cb->yypvt[-0].psl_inttype > 100)
	    {
		(VOID) psf_error(2149L, 0L, PSF_USERERR, &err_code,
		    &psq_cb->psq_error, 1, (i4) sizeof(yacc_cb->yypvt[-0].psl_inttype), &yacc_cb->yypvt[-0].psl_inttype);
		return (E_DB_ERROR);	/* non-zero return means error */
	    }
	    cb->pss_restab.pst_leaff = yacc_cb->yypvt[-0].psl_inttype;
	}
	else if (!STcasecmp(yacc_cb->yypvt[-2].psl_strtype, "maxindexfill"))
	{
	    /* ignore */
	}
        else if (!STcasecmp(yacc_cb->yypvt[-2].psl_strtype, "allocation"))
        {
            /* Nonleaffill must be between 4 and 8388607 */
            if (yacc_cb->yypvt[-0].psl_inttype < 4 || yacc_cb->yypvt[-0].psl_inttype > 8388607)
            {
                (VOID) psf_error(5555L, 0L, PSF_USERERR, &err_code,
                    &psq_cb->psq_error, 1, (i4) sizeof(yacc_cb->yypvt[-0].psl_inttype), &yacc_cb->yypvt[-0].psl_inttype);
                return (E_DB_ERROR);    /* non-zero return means error */
            }
            cb->pss_restab.pst_alloc = yacc_cb->yypvt[-0].psl_inttype;
        }
        else if (!STcasecmp(yacc_cb->yypvt[-2].psl_strtype, "extend"))
        {
            /* Nonleaffill must be between 1 and 8388607 */
            if (yacc_cb->yypvt[-0].psl_inttype < 1 || yacc_cb->yypvt[-0].psl_inttype > 8388607)
            {
                (VOID) psf_error(5556L, 0L, PSF_USERERR, &err_code,
                    &psq_cb->psq_error, 1, (i4) sizeof(yacc_cb->yypvt[-0].psl_inttype), &yacc_cb->yypvt[-0].psl_inttype);
                return (E_DB_ERROR);    /* non-zero return means error */
            }
            cb->pss_restab.pst_extend = yacc_cb->yypvt[-0].psl_inttype;
        }
	else	/* Unknown parameter */
	{
	    (VOID) psf_error(2140L, 0L, PSF_USERERR,
		&err_code, &psq_cb->psq_error, 2, (i4) sizeof(cb->pss_lineno),
		&cb->pss_lineno, STtrmwhite(yacc_cb->yypvt[-2].psl_strtype), yacc_cb->yypvt[-2].psl_strtype);
	    return (E_DB_ERROR);
	}
    } break;

case 361:
if (!((*yyvarspp)->bypass_actions))
/* # line 6771 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4			err_code;

	if (STcompare((char*) yacc_cb->yypvt[-0].psl_strtype, "journaling") == 0 ||
	    STcompare((char*) yacc_cb->yypvt[-0].psl_strtype, "logging") == 0)
    	{
	    yyvarsp->with_journaling = 1;
	    cb->pss_restab.pst_resjour = TRUE;
	}
	else if (STcompare((char*) yacc_cb->yypvt[-0].psl_strtype, "nojournaling") == 0 ||
	    STcompare((char*) yacc_cb->yypvt[-0].psl_strtype, "nologging") == 0)
	{
	    yyvarsp->with_journaling = 1;
	    cb->pss_restab.pst_resjour = FALSE;
	}
	else if (STcompare((char*) yacc_cb->yypvt[-0].psl_strtype, "duplicates") == 0)
	{
	    yyvarsp->with_dups = PST_ALLDUPS;
	    cb->pss_restab.pst_resdup = TRUE;
	}
	else if (STcompare((char*) yacc_cb->yypvt[-0].psl_strtype, "noduplicates") == 0)
	{
	    yyvarsp->with_dups = PST_NODUPS;
	    cb->pss_restab.pst_resdup = FALSE;
	}
	else
	{
	    (VOID) psf_error(2154L, 0L, PSF_USERERR,
		&err_code, &psq_cb->psq_error, 2, (i4) sizeof(cb->pss_lineno),
		&cb->pss_lineno, STtrmwhite(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);	/* non-zero return means error */
	}
    } break;

case 362:
if (!((*yyvarspp)->bypass_actions))
/* # line 6807 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	if (yyvarsp->list_clause == PSS_KEY_CLAUSE)
	    cb->pss_restab.pst_reskey = yacc_cb->yypvt[-1].psl_reskey;

	yyvarsp->list_clause = 0;	/* done with this clause */
    } break;

case 363:
if (!((*yyvarspp)->bypass_actions))
/* # line 6816 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;

	if (STcompare((char*) yacc_cb->yypvt[-1].psl_strtype, "key") == 0)
	    yyvarsp->list_clause = PSS_KEY_CLAUSE;
	else if (STcompare(yacc_cb->yypvt[-1].psl_strtype, "location") == 0)
	{
	    yyvarsp->list_clause = PSS_NLOC_CLAUSE;
	    /* check if user specified a locationname */
	    if (yyvarsp->with_location)
	    {
		(VOID) psf_error(2114L, 0L, PSF_USERERR,
		    &err_code, &psq_cb->psq_error, 1, sizeof(cb->pss_lineno),
		    &cb->pss_lineno);
		return (E_DB_ERROR);
	    }
	    yyvarsp->with_location = 1;
	}
	else
	{
	    (VOID) psf_error(2150L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, (i4) STlength(yacc_cb->yypvt[-1].psl_strtype), yacc_cb->yypvt[-1].psl_strtype);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}
    } break;

case 364:
if (!((*yyvarspp)->bypass_actions))
/* # line 6844 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_reskey = yacc_cb->yypvt[-0].psl_reskey;
    } break;

case 365:
if (!((*yyvarspp)->bypass_actions))
/* # line 6848 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	if (yyvarsp->list_clause == PSS_KEY_CLAUSE)
	{
	    PST_RESKEY	*reskey;
	    i4		err_code;

	    /*
	    ** Check for duplicate names.
	    */
	    reskey = yacc_cb->yypvt[-2].psl_reskey;	
	    do {
		if (!MEcmp((char *) &(yacc_cb->yypvt[-0].psl_reskey)->pst_attname,
		    (char *) &reskey->pst_attname,
		    sizeof(reskey->pst_attname))
		   )
		{
		    (VOID) psf_error(2151L, 0L, PSF_USERERR,
				     &err_code, &psq_cb->psq_error, 1,
				     STlength(yacc_cb->yypvt[-0].psl_reskey->pst_attname.db_att_name), 
				     yacc_cb->yypvt[-0].psl_reskey->pst_attname.db_att_name); 
		    return (E_DB_ERROR);
		}
		reskey = reskey->pst_nxtkey;
	    } while (reskey != (PST_RESKEY *) NULL);
     
	    /*
	    ** Find the last key in the list.
	    */
	    for (reskey = yacc_cb->yypvt[-2].psl_reskey; reskey->pst_nxtkey; reskey = reskey->pst_nxtkey)
	    ;

	    reskey->pst_nxtkey = yacc_cb->yypvt[-0].psl_reskey;

	    yacc_cb->yyval.psl_reskey = yacc_cb->yypvt[-2].psl_reskey;
	}
    } break;

case 366:
if (!((*yyvarspp)->bypass_actions))
/* # line 6887 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	if (yyvarsp->list_clause == PSS_KEY_CLAUSE)
	{
	    DB_STATUS	status;
	    PST_RESKEY	*reskey;

	    /* Allocate memory for a key entry */
	    status = psf_malloc(cb, &cb->pss_ostream, (i4) sizeof(PST_RESKEY),
		(PTR *) &reskey, &psq_cb->psq_error);
	    if (status != E_DB_OK)
	    {
		return (status);
	    }

	    /*
	    ** Copy column name to column entry.
	    */
	    STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(reskey->pst_attname),
		(char *) &reskey->pst_attname);
	    reskey->pst_nxtkey = (PST_RESKEY *) NULL;

	    yacc_cb->yyval.psl_reskey = reskey;
	}
	else
	{
	    register DB_LOC_NAME *loc, *lim;
	    i4	err_code;

	    /* See if there is space for 1 more */
	    if (cb->pss_restab.pst_resloc.data_in_size/sizeof(DB_LOC_NAME) >=
		DM_LOC_MAX)
	    {
		/* Too many locations */
		(VOID) psf_error(2115L, 0L, PSF_USERERR,
		    &err_code, &psq_cb->psq_error, 1, sizeof(cb->pss_lineno),
		    &cb->pss_lineno);
		return (E_DB_ERROR);
	    }

	    lim = (DB_LOC_NAME *) (cb->pss_restab.pst_resloc.data_address +
		    cb->pss_restab.pst_resloc.data_in_size);

	    /* Store normalized location name */
	    STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', (u_i4) DB_MAXNAME, (char *) lim);
	    cb->pss_restab.pst_resloc.data_in_size += sizeof(DB_LOC_NAME);

	    /* See if not a duplicate */
	    for (loc = (DB_LOC_NAME *) cb->pss_restab.pst_resloc.data_address;
		 loc < lim;
		 loc++
		)
	    {
		if (!MEcmp((char *) loc, (char *) lim, sizeof(DB_LOC_NAME)))
		{
		    /* A duplicate was found */
		    (VOID) psf_error(2116L, 0L, PSF_USERERR,
			&err_code, &psq_cb->psq_error, 2,
			sizeof(cb->pss_lineno), &cb->pss_lineno,
			psf_trmwhite(DB_MAXNAME, yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
		    return (E_DB_ERROR);
		}
	    }
	}
    } break;

case 367:
if (!((*yyvarspp)->bypass_actions))
/* # line 6954 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_trtype = yyvarsp->sort_list;
    } break;

case 368:
if (!((*yyvarspp)->bypass_actions))
/* # line 6958 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	/* No sort clause generates empty sort list */
	yacc_cb->yyval.psl_trtype = (PST_QNODE *) NULL;
    } break;

case 369:
if (!((*yyvarspp)->bypass_actions))
/* # line 6965 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	  yyvarsp->sort_by = 1;
    } break;

case 370:
if (!((*yyvarspp)->bypass_actions))
/* # line 6969 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	  yyvarsp->sort_by = 1;
    } break;

case 371:
if (!((*yyvarspp)->bypass_actions))
/* # line 6973 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	  yyvarsp->sort_by = 0;
    } break;

case 375:
if (!((*yyvarspp)->bypass_actions))
/* # line 6984 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	/*
	** if the same attribute appears more than once in the sort list, we
	** will disregard all but the first occurrence (since those occurrences
	** have no effect on the result of the query); as a result, pst_adsort()
	** may return NULL, in which case we should not worry about the
	** direction of the sort
	*/
	if (yacc_cb->yypvt[-2].psl_trtype != (PST_QNODE *) NULL)
	{
	    DB_STATUS   status;

	    /* Indicate "ascending" or "descending" in sort node */
	    status = pst_sdir(yacc_cb->yypvt[-2].psl_trtype, yacc_cb->yypvt[-0].psl_strtype, &psq_cb->psq_error);
	    if (status != E_DB_OK)
	    {
		return (status);
	    }
	}
    } break;

case 376:
if (!((*yyvarspp)->bypass_actions))
/* # line 7007 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_QNODE              *sortnode;
	DB_STATUS	       status;

	/* Allocate a sort node by range variable */
	status = pst_varsort(cb, &cb->pss_ostream, cb->pss_tlist, &yyvarsp->sort_list,
	    yacc_cb->yypvt[-2].psl_rngtype, yacc_cb->yypvt[-0].psl_strtype, &sortnode, psq_cb);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = sortnode;
    } break;

case 377:
if (!((*yyvarspp)->bypass_actions))
/* # line 7022 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_QNODE              *sortnode;
	DB_STATUS	       status;

	/* Allocate a sort node by expression name */
	status = pst_expsort(cb, &cb->pss_ostream, cb->pss_tlist, &yyvarsp->sort_list,
	    yacc_cb->yypvt[-0].psl_strtype, &sortnode, psq_cb);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = sortnode;
    } break;

case 378:
if (!((*yyvarspp)->bypass_actions))
/* # line 7039 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSS_RNGTAB             *rngvar;
	DB_STATUS	       status;
	i4		       err_code;

	status = pst_rglook(cb, &cb->pss_usrrange, -1, yacc_cb->yypvt[-0].psl_strtype, FALSE, 
	    &rngvar, psq_cb->psq_mode, &psq_cb->psq_error);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	if (rngvar == (PSS_RNGTAB *) NULL)
	{
	    (VOID) psf_error(2109L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 2, sizeof(cb->pss_lineno), &cb->pss_lineno,
		(i4) STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);
	}

	yacc_cb->yyval.psl_rngtype = rngvar;
    } break;

case 379:
if (!((*yyvarspp)->bypass_actions))
/* # line 7064 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	QEU_CB	*qeu_cb;
	DMT_CB	*dmt_cb;

	qeu_cb = (QEU_CB *) cb->pss_object;
	dmt_cb = (DMT_CB *) qeu_cb->qeu_d_cb;
	STRUCT_ASSIGN_MACRO(yacc_cb->yypvt[-2].psl_rngtype->pss_tabid, dmt_cb->dmt_id);
    } break;
	}
	return(0);
}

i4
psl8iftn(yacc_cb, yyrule, yystatusp, yyvarspp, cb, psq_cb)
YACC_CB		*yacc_cb;
i4		yyrule;
PSS_SESBLK	*cb;
DB_STATUS	*yystatusp;
YYVARS		**yyvarspp;
PSQ_CB *	psq_cb;
{
    YYVARS	*yyvarsp = *yyvarspp;
	switch(yyrule)
	{
case 380:
if (!((*yyvarspp)->bypass_actions))
/* # line 7073 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	QEU_CB	*qeu_cb;
	DMT_CB	*dmt_cb;

	qeu_cb = (QEU_CB *) cb->pss_object;
	dmt_cb = (DMT_CB *) qeu_cb->qeu_d_cb;
	STRUCT_ASSIGN_MACRO(yacc_cb->yypvt[-0].psl_rngtype->pss_tabid, dmt_cb->dmt_id);
    } break;

case 381:
if (!((*yyvarspp)->bypass_actions))
/* # line 7084 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4		    err_code, err_num = 0L;
	DB_STATUS           status;
	i4		    rngvar_info;
	QEU_CB		    *qeu_cb;
	DMT_CB		    *dmt_cb;
	PSS_RNGTAB	    *rngtab;
	DB_TAB_NAME	    tabname;

	qeu_cb = (QEU_CB *) cb->pss_object;
	dmt_cb = (DMT_CB *) qeu_cb->qeu_d_cb;

	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(DB_TAB_NAME), (char *) &tabname);

	status = psl0_rngent(&cb->pss_auxrng, -1, "", &tabname, cb,
	    FALSE, &rngtab, psq_cb->psq_mode, &psq_cb->psq_error,
	    PSS_USRTBL | PSS_DBATBL | PSS_INGTBL, &rngvar_info, 0);
	if (status != E_DB_OK)
	    return (status);

	if (rngtab == (PSS_RNGTAB *) NULL)
	{
	    (VOID) psf_error(5604L, 0L, PSF_USERERR,
		&err_code, &psq_cb->psq_error, 1,
		STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);
	}

	/* See if system catalog */
	if (rngtab->pss_tabdesc->tbl_status_mask &
	    	    	    	(DMT_CATALOG | DMT_EXTENDED_CAT)
	    || rngtab->pss_tabdesc->tbl_2_status_mask & DMT_TEXTENSION)
	{
	    err_num = 5600L;
	}
	else if (MEcmp((PTR) &rngtab->pss_ownname, (PTR) &cb->pss_user,
		       sizeof(DB_OWN_NAME)))
	{
	    err_num = 5604L;
	}

	if (err_num != 0L)
	{
	    (VOID) psf_error(err_num, 0L, PSF_USERERR,
		&err_code, &psq_cb->psq_error, 1,
		psf_trmwhite(sizeof(DB_TAB_NAME), 
		    (char *) &rngtab->pss_tabname),
		&rngtab->pss_tabname);
	    return (E_DB_ERROR);
	}

	yacc_cb->yyval.psl_rngtype = rngtab;
    } break;

case 382:
if (!((*yyvarspp)->bypass_actions))
/* # line 7140 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	QEU_CB		       *qeu_cb;
	DMT_CB		       *dmt_cb;
	DB_STATUS	       status;
	DMT_CHAR_ENTRY	       *chr;

	psq_cb->psq_mode = PSQ_SAVE;

	/* Allocate QEU_CB for SAVE and initialize its header */
	status = psl_qeucb(cb, DMT_ALTER, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	qeu_cb = (QEU_CB *) cb->pss_object;

	/* Allocate a DMT control block */
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(DMT_CB),
	    (PTR *) &dmt_cb, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	qeu_cb->qeu_d_cb = (PTR) dmt_cb;

	/* Fill in the DMT control block header */
	dmt_cb->type = DMT_TABLE_CB;
	dmt_cb->length = sizeof(DMT_CB);
	dmt_cb->dmt_db_id = (char*) cb->pss_dbid;
        dmt_cb->dmt_mustlock = FALSE;

	/* Allocate the characteristics array */
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(DMT_CHAR_ENTRY),
	    (PTR *) &dmt_cb->dmt_char_array.data_address, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	dmt_cb->dmt_char_array.data_in_size = sizeof(DMT_CHAR_ENTRY);

	/* Make the characteristic being changed the save date */

	chr = (DMT_CHAR_ENTRY *) dmt_cb->dmt_char_array.data_address;
	chr->char_id	= DMT_C_SAVEDATE;
	chr->char_value = 0;
    } break;

case 383:
if (!((*yyvarspp)->bypass_actions))
/* # line 7184 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4		       err_code;
	register i4	       i;
	register i4       date;
	i4		       value;
	ADF_CB		       *adf_scb = (ADF_CB*) cb->pss_adfcb;
	QEU_CB		       *qeu_cb;
	DMT_CB		       *dmt_cb;
	DMT_CHAR_ENTRY	       *chr;

	/* Check out the year */
	if (yacc_cb->yypvt[-0].psl_inttype < 1970 || yacc_cb->yypvt[-0].psl_inttype > 2035)
	{
	    (VOID) psf_error(5603L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, sizeof(yacc_cb->yypvt[-0].psl_inttype), &(yacc_cb->yypvt[-0].psl_inttype));
	    return (E_DB_ERROR);
	}

	/* Check out the day */
	if (yacc_cb->yypvt[-1].psl_inttype < 1 || psq_monsize((i4) yacc_cb->yypvt[-2].psl_inttype, (i4) yacc_cb->yypvt[-0].psl_inttype) < yacc_cb->yypvt[-1].psl_inttype)
	{
	    (VOID) psf_error(5602L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, sizeof(yacc_cb->yypvt[-1].psl_inttype), &(yacc_cb->yypvt[-1].psl_inttype));
	    return (E_DB_ERROR);
	}

	/* Convert date */

	/* "date" will be number of days from 1970 for a while */
	date = 0;

	/* Do year conversion */
	date += (yacc_cb->yypvt[-0].psl_inttype - 1970) * 365;
	if (yacc_cb->yypvt[-0].psl_inttype > 1970)
	    date += (yacc_cb->yypvt[-0].psl_inttype - 1973) / 4 + 1;    /* Add one day for each leap year */

	/* Do month conversion */
	for (i = 1; i < yacc_cb->yypvt[-2].psl_inttype; i++)
	    date += psq_monsize((i4)i, (i4)yacc_cb->yypvt[-0].psl_inttype);

	/* Do date conversion */
	date += yacc_cb->yypvt[-1].psl_inttype - 1;

	/* Convert the date to be the number of hours since 1970 */
	date *= 24;

	/* NOTE: NO DAYLIGHT SAVINGS TIME COMPUTATIONS */

	/* Convert to minutes */
	date *= 60;

	/* Convert to seconds */
	date *= 60;

	/* Adjust to GMT */

    	if( date > MAXI4)
    	    value = MAXI4;
    	else
    	    value = date;
    
    	date -=	TMtz_search(adf_scb->adf_tzcb, TM_TIMETYPE_LOCAL,
    	    	    	    value);



	/* Store number of seconds in control block */
	qeu_cb = (QEU_CB *) cb->pss_object;
	dmt_cb = (DMT_CB *) qeu_cb->qeu_d_cb;
	chr = (DMT_CHAR_ENTRY *) dmt_cb->dmt_char_array.data_address;
	chr->char_value = date;
    } break;

case 384:
if (!((*yyvarspp)->bypass_actions))
/* # line 7259 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;
	i4		       monthno;
	
	/* Translate month name to month number */
	status = psq_month(yacc_cb->yypvt[-0].psl_strtype, &monthno, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	yacc_cb->yyval.psl_inttype = monthno;
    } break;

case 385:
if (!((*yyvarspp)->bypass_actions))
/* # line 7271 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4		       err_code;
	char		       err[100];

	if (yacc_cb->yypvt[-0].psl_inttype <= 0 || yacc_cb->yypvt[-0].psl_inttype > 12)
	{
	    CVla(yacc_cb->yypvt[-0].psl_inttype, err);
	    (VOID) psf_error(5601L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, STlength(err), err);
	    return (E_DB_ERROR);
	}

	yacc_cb->yyval.psl_inttype = yacc_cb->yypvt[-0].psl_inttype;
    } break;

case 386:
if (!((*yyvarspp)->bypass_actions))
/* # line 7288 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	/* done */
    } break;

case 387:
if (!((*yyvarspp)->bypass_actions))
/* # line 7294 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;
	QEF_RCB		       *qef_rcb;

	psq_cb->psq_mode = PSQ_SVEPOINT;

	/* Allocate QEF control block for savepoint */
	status = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(QEF_RCB), &cb->pss_object,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_mroot(cb, &cb->pss_ostream, cb->pss_object,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	/* Fill in the QEF control block */
    	qef_rcb = (QEF_RCB *) cb->pss_object;
	qef_rcb->qef_length = sizeof(QEF_RCB);
	qef_rcb->qef_type = QEFRCB_CB;
	qef_rcb->qef_owner = (PTR)DB_PSF_ID;
	qef_rcb->qef_ascii_id = QEFRCB_ASCII_ID;
	qef_rcb->qef_modifier = QEF_MSTRAN;
	qef_rcb->qef_flag   = DB_QUEL;
	qef_rcb->qef_sess_id = cb->pss_sessid;
	qef_rcb->qef_db_id  = cb->pss_dbid;

	/* Allocate the savepoint name */
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(DB_SP_NAME),
	    (PTR *) &qef_rcb->qef_spoint, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

    } break;

case 388:
if (!((*yyvarspp)->bypass_actions))
/* # line 7334 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	QEF_RCB		       *qef_rcb;

	/* Fill in the QEF control block */
    	qef_rcb = (QEF_RCB *) cb->pss_object;

	/* Fill in the savepoint name */
	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(DB_SP_NAME), (char *) qef_rcb->qef_spoint);

    } break;

case 389:
if (!((*yyvarspp)->bypass_actions))
/* # line 7345 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	QEF_RCB		       *qef_rcb;
	char		       numbuf[25];

	/* Fill in the QEF control block */
    	qef_rcb = (QEF_RCB *) cb->pss_object;

	/* Fill in the savepoint name */
	CVla((i4) yacc_cb->yypvt[-0].psl_inttype, numbuf);
	STmove(numbuf, ' ', sizeof(DB_SP_NAME), (char *) qef_rcb->qef_spoint);
    } break;

case 390:
if (!((*yyvarspp)->bypass_actions))
/* # line 7359 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;

	psq_cb->psq_mode = PSQ_SAGGR;

	/* Possible parameters are "project" and "noproject" */
	if (!STcasecmp(yacc_cb->yypvt[-0].psl_strtype, "project"))
	{
	    cb->pss_ses_flag |= PSS_PROJECT;  /* pss_project = TRUE */
	}
	else if (!STcasecmp(yacc_cb->yypvt[-0].psl_strtype, "noproject"))
	{
	    cb->pss_ses_flag &= ~PSS_PROJECT;	/* pss_project = FALSE */
	}
	else
	{
	    (VOID) psf_error(5971L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, (i4) STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}
    } break;

case 391:
if (!((*yyvarspp)->bypass_actions))
/* # line 7383 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;

	/* "set autocommit" is not legal in QUEL. Issue warning */
	psq_cb->psq_mode = PSQ_OBSOLETE;

	if (cb->pss_ses_flag & PSS_WARNINGS)
	{
	    /* "set autocommit" is not supported for quel */
	    (VOID) psf_error(5974L, 0L, PSF_USERERR,
		&err_code, &psq_cb->psq_error, 0);
	}
    } break;

case 392:
if (!((*yyvarspp)->bypass_actions))
/* # line 7399 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_inttype = 1;
    } break;

case 393:
if (!((*yyvarspp)->bypass_actions))
/* # line 7403 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4	    err_code;

	if (STcompare(yacc_cb->yypvt[-0].psl_strtype, "off") == 0)
	    yacc_cb->yyval.psl_inttype = 2;
	else  /* syntax error */
	{
	    /* set autocommit syntax error */
	    (VOID) psf_error(2677L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, (i4) STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}
    } break;

case 394:
if (!((*yyvarspp)->bypass_actions))
/* # line 7419 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	       status;
	OPF_CB		       *opf_cb;

	psq_cb->psq_mode = PSQ_SCPUFACT;

	/* Create control block for setting cpu factor in OPF */
	status = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(OPF_CB), (PTR *) &opf_cb,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	status = psf_mroot(cb, &cb->pss_ostream, (PTR) opf_cb, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	/* Fill in OPF control block */
	opf_cb->opf_length = sizeof(opf_cb);
	opf_cb->opf_type = OPFCB_CB;
	opf_cb->opf_owner = (PTR)DB_PSF_ID;
	opf_cb->opf_ascii_id = OPFCB_ASCII_ID;
	opf_cb->opf_level = OPF_SESSION;
	opf_cb->opf_alter = OPF_CPUFACTOR;
	opf_cb->opf_value = yacc_cb->yypvt[-0].psl_inttype;
    } break;

case 395:
if (!((*yyvarspp)->bypass_actions))
/* # line 7450 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;
	i4		  date_fmt;

	psq_cb->psq_mode = PSQ_SDATEFMT;

	/*
	** Possible formats determined in adu_date_format (kibro01) b119318
	*/
	date_fmt = adu_date_format((char*)yacc_cb->yypvt[-0].psl_strtype);
	if (date_fmt == -1)
	{
	    (VOID) psf_error(5934L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, (i4) STlength((char *) yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}
	psq_cb->psq_dtefmt = date_fmt;
    } break;

case 396:
if (!((*yyvarspp)->bypass_actions))
/* # line 7471 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;

	psq_cb->psq_mode = PSQ_SDECIMAL;

	/* Period and comma are only allowable decimal markers */
	if (STcompare((char *) yacc_cb->yypvt[-0].psl_strtype, ",") && STcompare((char *) yacc_cb->yypvt[-0].psl_strtype, "."))
	{
	    (VOID) psf_error(5936L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, (i4) STlength((char *) yacc_cb->yypvt[-0].psl_strtype),
		(char *) yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}

	/* Give new decimal character back to caller */
	psq_cb->psq_decimal.db_decspec = TRUE;
	psq_cb->psq_decimal.db_decimal = *((char *) yacc_cb->yypvt[-0].psl_strtype);

	/* Change decimal character in session control block */
	cb->pss_decimal = *((char *) yacc_cb->yypvt[-0].psl_strtype);
    } break;

case 397:
if (!((*yyvarspp)->bypass_actions))
/* # line 7495 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	    ret_val;
	DB_DEBUG_CB	    *debug_cb;

	psq_cb->psq_mode = PSQ_DMFTRACE;

	/* Allocate the debug control block */
	ret_val = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream, &psq_cb->psq_error);
	if (ret_val != E_DB_OK)
	    return (ret_val);
	ret_val = psf_malloc(cb, &cb->pss_ostream, sizeof(DB_DEBUG_CB),
	    (PTR *) &debug_cb, &psq_cb->psq_error);
	if (ret_val != E_DB_OK)
	    return (ret_val);
	ret_val = psf_mroot(cb, &cb->pss_ostream, (PTR) debug_cb,
	    &psq_cb->psq_error);
	if (ret_val != E_DB_OK)
	    return (ret_val);
	debug_cb->db_trace_point = DM_IOTRACE;
	debug_cb->db_value_count = 0;
	if (yacc_cb->yypvt[-0].psl_tytype == PSL_ONSET)
	    debug_cb->db_trswitch = DB_TR_ON;
	else
	    debug_cb->db_trswitch = DB_TR_OFF;
    } break;

case 398:
if (!((*yyvarspp)->bypass_actions))
/* # line 7523 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;

	psq_cb->psq_mode = PSQ_OBSOLETE;

	if (cb->pss_ses_flag & PSS_WARNINGS)
	{
	    /* Set [no]joinop no longer supported */
	    (VOID) psf_error(5933L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 0);
	}
    } break;

case 399:
if (!((*yyvarspp)->bypass_actions))
/* # line 7538 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	psq_cb->psq_mode = PSQ_ALLJOURNAL;

	if (yacc_cb->yypvt[-0].psl_tytype == PSL_ONSET)
	    cb->pss_ses_flag |= PSS_JOURNALING; /* pss_journaling = TRUE */
	else
	    cb->pss_ses_flag &= ~PSS_JOURNALING; /* pss_journaling = FALSE */
    } break;

case 400:
if (!((*yyvarspp)->bypass_actions))
/* # line 7547 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4             err_code, err_num = 0L;
	DB_STATUS	    status;
	i4		    rngvar_info;
	QEU_CB		    *qeu_cb;
	DMT_CB		    *dmt_cb;
	PSS_RNGTAB	    *rngvar;
	DB_TAB_NAME	    tabname;
	DMT_CHAR_ENTRY	    *chr;

	psq_cb->psq_mode = PSQ_SJOURNAL;

	/* Look up the table id */
	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(DB_TAB_NAME), (char *) &tabname);
	status = psl_rngent(&cb->pss_auxrng, -1, "", &tabname, cb,
	    FALSE, &rngvar, psq_cb->psq_mode, &psq_cb->psq_error, &rngvar_info);
	if (DB_FAILURE_MACRO(status))
	    return (status);

	/* Disallow SET JOURNALING on catalogs. */
	if (rngvar->pss_tabdesc->tbl_status_mask &
	    	    (DMT_CATALOG | DMT_EXTENDED_CAT)
	    || rngvar->pss_tabdesc->tbl_2_status_mask)
	{
	    err_num = 2762L;
	}
	/* Disallow SET JOURNALING on indices */
	else if (rngvar->pss_tabdesc->tbl_status_mask & DMT_IDX)
	{
	    err_num = 2766L;
	}
	else if (MEcmp((PTR) &rngvar->pss_ownname, (PTR) &cb->pss_user,
	               sizeof(DB_OWN_NAME)))
	{
	    /* disallow SET [NO]JOURNALING on someone else's tables */
	    (VOID) psf_error(2753L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 2,
		sizeof("SET [NO]JOURNALING") - 1, "SET [NO]JOURNALING",
		STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return(E_DB_ERROR);
	}

	if (err_num != 0L)
	{
	    (VOID) psf_error(err_num, 0L, PSF_USERERR,&err_code,
		&psq_cb->psq_error, 1,
		psf_trmwhite(sizeof(DB_TAB_NAME), 
		    (char *) &rngvar->pss_tabname),
		&rngvar->pss_tabname);
	    return (E_DB_ERROR);
	}

	/* Allocate QEU_CB for SET JOURNALING and initialize its header */
	status = psl_qeucb(cb, DMT_ALTER, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	qeu_cb = (QEU_CB *) cb->pss_object;

	/* Allocate a DMT control block */
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(DMT_CB),
	    (PTR *) &dmt_cb, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	qeu_cb->qeu_d_cb = (PTR) dmt_cb;

	/* Fill in the DMT control block header */
	dmt_cb->type = DMT_TABLE_CB;
	dmt_cb->length = sizeof(DMT_CB);
	dmt_cb->dmt_db_id = (char*) cb->pss_dbid;
        dmt_cb->dmt_mustlock = FALSE;

	/* Allocate the characteristics array */
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(DMT_CHAR_ENTRY),
	    (PTR *) &dmt_cb->dmt_char_array.data_address, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	dmt_cb->dmt_char_array.data_in_size = sizeof(DMT_CHAR_ENTRY);

	/* Make the characteristic being changed the journaled bit */
	chr = (DMT_CHAR_ENTRY *) dmt_cb->dmt_char_array.data_address;
	chr->char_id = DMT_C_JOURNALED;
	if (yacc_cb->yypvt[-2].psl_tytype == PSL_ONSET)
	    chr->char_value = DMT_C_ON;
	else
	    chr->char_value = DMT_C_OFF;

	STRUCT_ASSIGN_MACRO(rngvar->pss_tabid, dmt_cb->dmt_id);
    } break;

case 401:
if (!((*yyvarspp)->bypass_actions))
/* # line 7639 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	       status;
	OPF_CB		       *opf_cb;
	i4			flag;

	psq_cb->psq_mode = PSQ_JTIMEOUT;

	if (STcompare(yacc_cb->yypvt[-1].psl_strtype, "timeout") == 0)
	    flag = OPF_TIMEOUT;
	else if (STcompare(yacc_cb->yypvt[-1].psl_strtype, "notimeout") == 0)
	    flag = OPF_NOTIMEOUT;
	else if ((STcompare(yacc_cb->yypvt[-1].psl_strtype, "newenum") == 0 ||
		STcompare(yacc_cb->yypvt[-1].psl_strtype, "greedy") == 0) && yacc_cb->yypvt[-0].psl_inttype == 0)
	    flag = OPF_NEWENUM;
	else if ((STcompare(yacc_cb->yypvt[-1].psl_strtype, "nonewenum") == 0 ||
		STcompare(yacc_cb->yypvt[-1].psl_strtype, "nogreedy") == 0) && yacc_cb->yypvt[-0].psl_inttype == 0)
	    flag = OPF_NONEWENUM;
	else
	{
	    i4	    err_code;

	    /* set joinop [no]timeout syntax error */
	    (VOID) psf_error(2682L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 2,
		(i4) sizeof(cb->pss_lineno), &cb->pss_lineno,
		(i4) STlength(yacc_cb->yypvt[-1].psl_strtype), yacc_cb->yypvt[-1].psl_strtype);
	    return (E_DB_ERROR);
	}
	/* Create control block for setting cpu factor in OPF */
	status = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(OPF_CB), (PTR *) &opf_cb,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_mroot(cb, &cb->pss_ostream, (PTR) opf_cb,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	/* Fill in OPF control block */
	opf_cb->opf_length = sizeof(opf_cb);
	opf_cb->opf_type = OPFCB_CB;
	opf_cb->opf_owner = (PTR)DB_PSF_ID;
	opf_cb->opf_ascii_id = OPFCB_ASCII_ID;
	opf_cb->opf_level = OPF_SESSION;
	opf_cb->opf_alter = flag;
	opf_cb->opf_value = yacc_cb->yypvt[-0].psl_inttype;
    } break;

case 403:
if (!((*yyvarspp)->bypass_actions))
/* # line 7693 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_inttype = 0;
    } break;

case 404:
if (!((*yyvarspp)->bypass_actions))
/* # line 7699 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	    ret_val;
	DB_DEBUG_CB	    *debug_cb;

	psq_cb->psq_mode = PSQ_DMFTRACE;

	/* Allocate the debug control block */
	ret_val = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream, &psq_cb->psq_error);
	if (ret_val != E_DB_OK)
	    return (ret_val);
	ret_val = psf_malloc(cb, &cb->pss_ostream, sizeof(DB_DEBUG_CB),
	    (PTR *) &debug_cb, &psq_cb->psq_error);
	if (ret_val != E_DB_OK)
	    return (ret_val);
	ret_val = psf_mroot(cb, &cb->pss_ostream, (PTR) debug_cb,
	    &psq_cb->psq_error);
	if (ret_val != E_DB_OK)
	    return (ret_val);
	debug_cb->db_trace_point = DM_LOCKTRACE;
	debug_cb->db_value_count = 0;
	if (yacc_cb->yypvt[-0].psl_tytype == PSL_ONSET)
	    debug_cb->db_trswitch = DB_TR_ON;
	else
	    debug_cb->db_trswitch = DB_TR_OFF;
    } break;

case 405:
if (!((*yyvarspp)->bypass_actions))
/* # line 7743 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	    status;
	DMC_CB		    *dmc_cb;
	DMC_CHAR_ENTRY	    *dmc_char;
	i4		    err_code;

	psq_cb->psq_mode = PSQ_SLOGGING;

	/*
	** Check for priviledge to turn off logging.
	** User must be the DBA to enable or disable logging.
	*/
	if (MEcmp((PTR)&cb->pss_user, (PTR)&cb->pss_dba,
	       sizeof(DB_OWN_NAME)))
	{
	    _VOID_ psf_error(E_PS0F81_SETLG_PRIV_ERR, 0L, PSF_USERERR,
			     &err_code, &psq_cb->psq_error, 0);
	    return (E_DB_ERROR);
	}

	/*
	** Create control block for DMC_ALTER call for Set [No]Logging
	*/
	status = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream, &psq_cb->psq_error);
	if (DB_FAILURE_MACRO(status))
	    return (status);

	status = psf_malloc(cb, &cb->pss_ostream, sizeof(DMC_CB), (PTR *) &dmc_cb,
	    &psq_cb->psq_error);
	if (DB_FAILURE_MACRO(status))
	    return (status);

	status = psf_mroot(cb, &cb->pss_ostream, (PTR) dmc_cb, &psq_cb->psq_error);
	if (DB_FAILURE_MACRO(status))
	    return (status);

	/*
	** Format Alter Control Block.
	*/
	cb->pss_object	    = (PTR) dmc_cb;
	dmc_cb->type	    = DMC_CONTROL_CB;
	dmc_cb->length	    = sizeof (DMC_CB);
	dmc_cb->dmc_op_type = DMC_SESSION_OP;
	dmc_cb->dmc_session_id	= (PTR)cb->pss_sessid;
	dmc_cb->dmc_db_id   = cb->pss_dbid;
	dmc_cb->dmc_flags_mask = 0;

	/*
	** Allocate characteristics array with entry for Set Logging.
	*/
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(DMC_CHAR_ENTRY),
	     (PTR *) &dmc_char, &psq_cb->psq_error);
	if (DB_FAILURE_MACRO(status))
	    return (status);

	dmc_cb->dmc_char_array.data_in_size = sizeof(DMC_CHAR_ENTRY);
	dmc_cb->dmc_char_array.data_address = (char *) dmc_char;
	dmc_char->char_id = DMC_C_LOGGING;
	dmc_char->char_value = (yacc_cb->yypvt[-0].psl_tytype == PSL_ONSET) ? DMC_C_ON : DMC_C_OFF;
    } break;

case 406:
if (!((*yyvarspp)->bypass_actions))
/* # line 7821 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	    status;
	DB_DEBUG_CB	    *debug_cb;

	psq_cb->psq_mode = PSQ_DMFTRACE;

	/* Allocate the debug control block */
	status = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream, &psq_cb->psq_error);
	if (DB_FAILURE_MACRO(status))
	    return (status);

	status = psf_malloc(cb, &cb->pss_ostream, sizeof(DB_DEBUG_CB),
	    (PTR *) &debug_cb, &psq_cb->psq_error);
	if (DB_FAILURE_MACRO(status))
	    return (status);

	status = psf_mroot(cb, &cb->pss_ostream, (PTR) debug_cb,
	    &psq_cb->psq_error);
	if (DB_FAILURE_MACRO(status))
	    return (status);

	debug_cb->db_trace_point = DM_LOGTRACE;
	debug_cb->db_value_count = 0;

	debug_cb->db_trswitch = (yacc_cb->yypvt[-0].psl_tytype == PSL_ONSET) ? DB_TR_ON : DB_TR_OFF;
    } break;

case 408:
if (!((*yyvarspp)->bypass_actions))
/* # line 7853 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_lm1_setlockstmnt(cb, psq_cb);

	if (DB_FAILURE_MACRO(status))
	{
	    return(status);
	}
    } break;

case 409:
if (!((*yyvarspp)->bypass_actions))
/* # line 7866 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4			err_code;
	DMC_CB			*dmc_cb;

	/* "session" is the only parameter acceptable here */
	if (STcasecmp(yacc_cb->yypvt[-0].psl_strtype, "session"))
	{
	    (VOID) psf_error(5970L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, (i4) STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}
	dmc_cb	= (DMC_CB*) cb->pss_object;
	dmc_cb->dmc_sl_scope = DMC_SL_SESSION;
    } break;

case 410:
if (!((*yyvarspp)->bypass_actions))
/* # line 7881 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;
	i4			rngvar_info;
	DMC_CB			*dmc_cb;
	PSS_RNGTAB	       *rngvar;
	DB_TAB_NAME	       tabname;

	dmc_cb	= (DMC_CB*) cb->pss_object;
	dmc_cb->dmc_sl_scope = DMC_SL_TABLE;

	/* Look up the table id */
	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(DB_TAB_NAME), (char *) &tabname);
	status = psl_rngent(&cb->pss_auxrng, -1, "", &tabname, cb,
	    FALSE, &rngvar, psq_cb->psq_mode, &psq_cb->psq_error, &rngvar_info);
	if (status != E_DB_OK)
	    return (status);
	STRUCT_ASSIGN_MACRO(rngvar->pss_tabid, dmc_cb->dmc_table_id);
    } break;

case 413:
if (!((*yyvarspp)->bypass_actions))
/* # line 7906 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_lm3_setlockparm_name(yacc_cb->yypvt[-2].psl_tytype, yacc_cb->yypvt[-0].psl_strtype, cb, &psq_cb->psq_error);

	if (DB_FAILURE_MACRO(status))
	{
	    return(status);
	}
    } break;
	}
	return(0);
}

i4
psl9iftn(yacc_cb, yyrule, yystatusp, yyvarspp, cb, psq_cb)
YACC_CB		*yacc_cb;
i4		yyrule;
PSS_SESBLK	*cb;
DB_STATUS	*yystatusp;
YYVARS		**yyvarspp;
PSQ_CB *	psq_cb;
{
    YYVARS	*yyvarsp = *yyvarspp;
	switch(yyrule)
	{
case 414:
if (!((*yyvarspp)->bypass_actions))
/* # line 7917 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS		status;

	status = psl_lm4_setlockparm_num(yacc_cb->yypvt[-2].psl_tytype, yacc_cb->yypvt[-0].psl_inttype, cb, &psq_cb->psq_error);

	if (DB_FAILURE_MACRO(status))
	{
	    return(status);
	}
    } break;

case 415:
if (!((*yyvarspp)->bypass_actions))
/* # line 7930 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4			    lockkey_type;
	DB_STATUS		    status;

	status = psl_lm2_setlockkey(cb, yacc_cb->yypvt[-0].psl_strtype, &lockkey_type, &psq_cb->psq_error);

	if (DB_FAILURE_MACRO(status))
	{
	    return(status);
	}
	else
	{
	    yacc_cb->yyval.psl_tytype = lockkey_type;
	}
    } break;

case 416:
if (!((*yyvarspp)->bypass_actions))
/* # line 7946 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
        i4                      lockkey_type;
        DB_STATUS               status;
 
        status = psl_lm2_setlockkey(cb, "level", &lockkey_type,
                                    &psq_cb->psq_error);
 
        if (DB_FAILURE_MACRO(status))
        {
            return(status);
        }
        else
        {
            yacc_cb->yyval.psl_tytype = lockkey_type;
        }
    } break;

case 417:
if (!((*yyvarspp)->bypass_actions))
/* # line 7965 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;

	psq_cb->psq_mode = PSQ_SMNFMT;

	/* First character of parameter must specify leading or trailing */
	if (((char *) yacc_cb->yypvt[-0].psl_strtype)[0] == 't' || ((char *) yacc_cb->yypvt[-0].psl_strtype)[0] == 'T')
	{
	    /* Give trailing indicator back to caller */
	    psq_cb->psq_mnyfmt.db_mny_lort = DB_TRAIL_MONY;
	}
	else if (((char *) yacc_cb->yypvt[-0].psl_strtype)[0] == 'l' || ((char *) yacc_cb->yypvt[-0].psl_strtype)[0] == 'L')
	{
	    /* Give leading indicator back to caller */
	    psq_cb->psq_mnyfmt.db_mny_lort = DB_LEAD_MONY;
	}
	else if ( (((char *) yacc_cb->yypvt[-0].psl_strtype)[0] == 'n' || ((char *) yacc_cb->yypvt[-0].psl_strtype)[0] == 'N') &&
	          (((char *) yacc_cb->yypvt[-0].psl_strtype)[1] == 'o' || ((char *) yacc_cb->yypvt[-0].psl_strtype)[1] == 'O') &&
	          (((char *) yacc_cb->yypvt[-0].psl_strtype)[2] == 'n' || ((char *) yacc_cb->yypvt[-0].psl_strtype)[2] == 'N') &&
	          (((char *) yacc_cb->yypvt[-0].psl_strtype)[3] == 'e' || ((char *) yacc_cb->yypvt[-0].psl_strtype)[3] == 'E') )
	{
	    /* Give leading indicator back to caller */
	    psq_cb->psq_mnyfmt.db_mny_lort = DB_NONE_MONY;
	    psq_cb->psq_mnyfmt.db_mny_sym[0] = EOS;
	}
	else
	{
	    (VOID) psf_error(5935L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, STlength((char *) yacc_cb->yypvt[-0].psl_strtype), (char *) yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}

 	if ( (psq_cb->psq_mnyfmt.db_mny_lort == DB_LEAD_MONY) ||
 	     (psq_cb->psq_mnyfmt.db_mny_lort == DB_TRAIL_MONY) )
	{
	    /* Second character of parameter must be a colon */
	    if ( ((char *) yacc_cb->yypvt[-0].psl_strtype)[1] != ':') 
	    {
	        (VOID) psf_error(5935L, 0L, PSF_USERERR, &err_code,
		    &psq_cb->psq_error, 1, STlength((char *) yacc_cb->yypvt[-0].psl_strtype), (char *) yacc_cb->yypvt[-0].psl_strtype);
	        return (E_DB_ERROR);    /* non-zero return means error */
	    }

	    /* Next 0 - 4 characters is the currency symbol */
	    if (STlength(&((char *) yacc_cb->yypvt[-0].psl_strtype)[2]) > DB_MAXMONY)
	    {
	        (VOID) psf_error(5935L, 0L, PSF_USERERR, &err_code,
		    &psq_cb->psq_error, 1, STlength((char *) yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	        return (E_DB_ERROR);
	    }

	    /* Give money symbol back to caller */
	    STcopy(&((char *) yacc_cb->yypvt[-0].psl_strtype)[2], psq_cb->psq_mnyfmt.db_mny_sym);
	}
    } break;

case 418:
if (!((*yyvarspp)->bypass_actions))
/* # line 8023 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;

	psq_cb->psq_mode = PSQ_SMNPREC;

	/* Specified precision must be between 0 and 2, inclusive */
	if (yacc_cb->yypvt[-0].psl_inttype < 0 || yacc_cb->yypvt[-0].psl_inttype > 2)
	{
	    (VOID) psf_error(5937L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, sizeof(yacc_cb->yypvt[-0].psl_inttype), &yacc_cb->yypvt[-0].psl_inttype);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}

	/* Give money precision back to caller */
	psq_cb->psq_mnyfmt.db_mny_prec = yacc_cb->yypvt[-0].psl_inttype;
    } break;

case 419:
if (!((*yyvarspp)->bypass_actions))
/* # line 8042 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;
	PSY_CB		       *psy_cb;

	psy_cb = (PSY_CB *) cb->pss_object;

	if (yacc_cb->yypvt[-1].psl_tytype != PSL_ONSET)
	{
	    (VOID) psf_error(6236L, 0L, PSF_USERERR, 
		&err_code, &psq_cb->psq_error, 0);
	    return (E_DB_ERROR);
	}

	psy_cb->psy_ctl_dbpriv |= DBPR_QDIO_LIMIT;
	psy_cb->psy_fl_dbpriv  |= DBPR_QDIO_LIMIT;
	psy_cb->psy_qdio_limit  = yacc_cb->yypvt[-0].psl_inttype;

    } break;

case 420:
if (!((*yyvarspp)->bypass_actions))
/* # line 8061 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;
	PSY_CB		       *psy_cb;

	psy_cb = (PSY_CB *) cb->pss_object;

	if (yacc_cb->yypvt[-0].psl_tytype != PSL_OFFSET)
	{
	    (VOID) psf_error(6237L, 0L, PSF_USERERR, 
		&err_code, &psq_cb->psq_error, 0);
	    return (E_DB_ERROR);
	}

	psy_cb->psy_ctl_dbpriv  = DBPR_QDIO_LIMIT;
    } break;

case 421:
if (!((*yyvarspp)->bypass_actions))
/* # line 8079 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	       status;

	psq_cb->psq_mode = PSQ_MXQUERY;

	if ((status = psy_qminit(cb, psq_cb, &cb->pss_ostream)) != E_DB_OK)
	    return(status);

	yacc_cb->yyval.psl_tytype = yacc_cb->yypvt[-0].psl_tytype;
    } break;

case 422:
if (!((*yyvarspp)->bypass_actions))
/* # line 8092 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;
	PSY_CB		       *psy_cb;

	psy_cb = (PSY_CB *) cb->pss_object;

	if (yacc_cb->yypvt[-1].psl_tytype != PSL_ONSET)
	{
	    (VOID) psf_error(6236L, 0L, PSF_USERERR, 
		&err_code, &psq_cb->psq_error, 0);
	    return (E_DB_ERROR);
	}

	psy_cb->psy_ctl_dbpriv |= DBPR_QROW_LIMIT;
	psy_cb->psy_fl_dbpriv  |= DBPR_QROW_LIMIT;
	psy_cb->psy_qrow_limit  = yacc_cb->yypvt[-0].psl_inttype;

    } break;

case 423:
if (!((*yyvarspp)->bypass_actions))
/* # line 8111 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;
	PSY_CB		       *psy_cb;

	psy_cb = (PSY_CB *) cb->pss_object;

	if (yacc_cb->yypvt[-0].psl_tytype != PSL_OFFSET)
	{
	    (VOID) psf_error(6237L, 0L, PSF_USERERR, 
		&err_code, &psq_cb->psq_error, 0);
	    return (E_DB_ERROR);
	}

	psy_cb->psy_ctl_dbpriv  |= DBPR_QROW_LIMIT;
    } break;

case 424:
if (!((*yyvarspp)->bypass_actions))
/* # line 8129 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	       status;

	psq_cb->psq_mode = PSQ_MXQUERY;

	if ((status = psy_qminit(cb, psq_cb, &cb->pss_ostream)) != E_DB_OK)
	    return(status);

	yacc_cb->yyval.psl_tytype = yacc_cb->yypvt[-0].psl_tytype;
    } break;

case 425:
if (!((*yyvarspp)->bypass_actions))
/* # line 8142 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;
	PSY_CB		       *psy_cb;

	psy_cb = (PSY_CB *) cb->pss_object;

	if (yacc_cb->yypvt[-1].psl_tytype != PSL_ONSET)
	{
	    (VOID) psf_error(6236L, 0L, PSF_USERERR, 
		&err_code, &psq_cb->psq_error, 0);
	    return (E_DB_ERROR);
	}

	psy_cb->psy_ctl_dbpriv |= DBPR_QCPU_LIMIT;
	psy_cb->psy_fl_dbpriv  |= DBPR_QCPU_LIMIT;
	psy_cb->psy_qcpu_limit  = yacc_cb->yypvt[-0].psl_inttype;

    } break;

case 426:
if (!((*yyvarspp)->bypass_actions))
/* # line 8161 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;
	PSY_CB		       *psy_cb;

	psy_cb = (PSY_CB *) cb->pss_object;

	if (yacc_cb->yypvt[-0].psl_tytype != PSL_OFFSET)
	{
	    (VOID) psf_error(6237L, 0L, PSF_USERERR, 
		&err_code, &psq_cb->psq_error, 0);
	    return (E_DB_ERROR);
	}

	psy_cb->psy_ctl_dbpriv  |= DBPR_QCPU_LIMIT;
    } break;

case 427:
if (!((*yyvarspp)->bypass_actions))
/* # line 8179 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	       status;

	psq_cb->psq_mode = PSQ_MXQUERY;

	if ((status = psy_qminit(cb, psq_cb, &cb->pss_ostream)) != E_DB_OK)
	    return(status);

	yacc_cb->yyval.psl_tytype = yacc_cb->yypvt[-0].psl_tytype;
    } break;

case 428:
if (!((*yyvarspp)->bypass_actions))
/* # line 8192 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;
	PSY_CB		       *psy_cb;

	psy_cb = (PSY_CB *) cb->pss_object;

	if (yacc_cb->yypvt[-1].psl_tytype != PSL_ONSET)
	{
	    (VOID) psf_error(6236L, 0L, PSF_USERERR, 
		&err_code, &psq_cb->psq_error, 0);
	    return (E_DB_ERROR);
	}

	psy_cb->psy_ctl_dbpriv |= DBPR_QPAGE_LIMIT;
	psy_cb->psy_fl_dbpriv  |= DBPR_QPAGE_LIMIT;
	psy_cb->psy_qpage_limit = yacc_cb->yypvt[-0].psl_inttype;

    } break;

case 429:
if (!((*yyvarspp)->bypass_actions))
/* # line 8211 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;
	PSY_CB		       *psy_cb;

	psy_cb = (PSY_CB *) cb->pss_object;

	if (yacc_cb->yypvt[-0].psl_tytype != PSL_OFFSET)
	{
	    (VOID) psf_error(6237L, 0L, PSF_USERERR, 
		&err_code, &psq_cb->psq_error, 0);
	    return (E_DB_ERROR);
	}

	psy_cb->psy_ctl_dbpriv  |= DBPR_QPAGE_LIMIT;
    } break;

case 430:
if (!((*yyvarspp)->bypass_actions))
/* # line 8229 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	       status;

	psq_cb->psq_mode = PSQ_MXQUERY;

	if ((status = psy_qminit(cb, psq_cb, &cb->pss_ostream)) != E_DB_OK)
	    return(status);

	yacc_cb->yyval.psl_tytype = yacc_cb->yypvt[-0].psl_tytype;
    } break;

case 431:
if (!((*yyvarspp)->bypass_actions))
/* # line 8242 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;
	PSY_CB		       *psy_cb;

	psy_cb = (PSY_CB *) cb->pss_object;

	if (yacc_cb->yypvt[-1].psl_tytype != PSL_ONSET)
	{
	    (VOID) psf_error(6236L, 0L, PSF_USERERR, 
		&err_code, &psq_cb->psq_error, 0);
	    return (E_DB_ERROR);
	}

	psy_cb->psy_ctl_dbpriv |= DBPR_QCOST_LIMIT;
	psy_cb->psy_fl_dbpriv  |= DBPR_QCOST_LIMIT;
	psy_cb->psy_qcost_limit = yacc_cb->yypvt[-0].psl_inttype;

    } break;

case 432:
if (!((*yyvarspp)->bypass_actions))
/* # line 8261 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;
	PSY_CB		       *psy_cb;

	psy_cb = (PSY_CB *) cb->pss_object;

	if (yacc_cb->yypvt[-0].psl_tytype != PSL_OFFSET)
	{
	    (VOID) psf_error(6237L, 0L, PSF_USERERR, 
		&err_code, &psq_cb->psq_error, 0);
	    return (E_DB_ERROR);
	}

	psy_cb->psy_ctl_dbpriv  |= DBPR_QCOST_LIMIT;
    } break;

case 433:
if (!((*yyvarspp)->bypass_actions))
/* # line 8279 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	       status;

	psq_cb->psq_mode = PSQ_MXQUERY;

	if ((status = psy_qminit(cb, psq_cb, &cb->pss_ostream)) != E_DB_OK)
	    return(status);

	yacc_cb->yyval.psl_tytype = yacc_cb->yypvt[-0].psl_tytype;
    } break;

case 434:
if (!((*yyvarspp)->bypass_actions))
/* # line 8292 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_DEBUG_CB	       *debug_cb;
	DB_STATUS	       ret_val;

	/* Allocate the debug control block */
	ret_val = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream,
	    &psq_cb->psq_error);
	if (ret_val != E_DB_OK)
	    return (ret_val);
	ret_val = psf_malloc(cb, &cb->pss_ostream, sizeof(DB_DEBUG_CB),
	    (PTR *) &debug_cb, &psq_cb->psq_error);
	if (ret_val != E_DB_OK)
	    return (ret_val);
	ret_val = psf_mroot(cb, &cb->pss_ostream, (PTR) debug_cb,
	    &psq_cb->psq_error);
	if (ret_val != E_DB_OK)
	    return (ret_val);
	debug_cb->db_value_count = 0;
	if (yacc_cb->yypvt[-0].psl_tytype == PSL_ONSET)
	    debug_cb->db_trswitch = DB_TR_ON;
	else
	    debug_cb->db_trswitch = DB_TR_OFF;

	debug_cb->db_trace_point = 129;	    /* 128 + 1 - first trace flag
					    ** of session trace flags.
					    */
	psq_cb->psq_mode = PSQ_PSFTRACE;
    } break;

case 435:
if (!((*yyvarspp)->bypass_actions))
/* # line 8324 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	/*
	** It has been decided to implement thsi statement by acting as if user
	** entered "set [no]trace point op160"
	*/

	DB_DEBUG_CB	*debug_cb;
	DB_STATUS	ret_val;

	/* Allocate the debug control block */
	ret_val = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream, &psq_cb->psq_error);
	if (ret_val != E_DB_OK)
	    return (ret_val);
	ret_val = psf_malloc(cb, &cb->pss_ostream, sizeof(DB_DEBUG_CB),
	    (PTR *) &debug_cb, &psq_cb->psq_error);
	if (ret_val != E_DB_OK)
	    return (ret_val);
	ret_val = psf_mroot(cb, &cb->pss_ostream, (PTR) debug_cb,
			    &psq_cb->psq_error);
	if (ret_val != E_DB_OK)
	    return (ret_val);
	debug_cb->db_value_count = 0;

	debug_cb->db_trswitch = (yacc_cb->yypvt[-0].psl_tytype == PSL_ONSET) ? DB_TR_ON : DB_TR_OFF;

	/*
	** since we are supplying the tracepoint name, we'd like to hope that
	** getfacil() doesn't get too excited over it.  It will supply us with
	** both the query mode and the trace point number.
	*/
	(VOID) getfacil("op160", &psq_cb->psq_mode,
			(i4 *) &debug_cb->db_trace_point);
    } break;

case 436:
if (!((*yyvarspp)->bypass_actions))
/* # line 8360 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	       status;
	OPF_CB		       *opf_cb;

	psq_cb->psq_mode = PSQ_SQEP;

	/* Create control block for setting cpu factor in OPF */
	status = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(OPF_CB), (PTR *) &opf_cb,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_mroot(cb, &cb->pss_ostream, (PTR) opf_cb,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	/* Fill in OPF control block */
	opf_cb->opf_length = sizeof(opf_cb);
	opf_cb->opf_type = OPFCB_CB;
	opf_cb->opf_owner = (PTR)DB_PSF_ID;
	opf_cb->opf_ascii_id = OPFCB_ASCII_ID;
	opf_cb->opf_level = OPF_SESSION;
	if (yacc_cb->yypvt[-0].psl_tytype == PSL_ONSET)
	    opf_cb->opf_alter = OPF_QEP;
	else
	    opf_cb->opf_alter = OPF_NOQEP;
} break;

case 437:
if (!((*yyvarspp)->bypass_actions))
/* # line 8392 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	       status;
	OPF_CB		       *opf_cb;

	psq_cb->psq_mode = PSQ_SQEP;	/* NOTE: SQEP suffices for all
					** OPF session alter values */

	/* Create control block for setting subselect flattening in OPF */
	status = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(OPF_CB), (PTR *) &opf_cb,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_mroot(cb, &cb->pss_ostream, (PTR) opf_cb,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	/* Fill in OPF control block */
	opf_cb->opf_length = sizeof(opf_cb);
	opf_cb->opf_type = OPFCB_CB;
	opf_cb->opf_owner = (PTR)DB_PSF_ID;
	opf_cb->opf_ascii_id = OPFCB_ASCII_ID;
	opf_cb->opf_level = OPF_SESSION;
	if (yacc_cb->yypvt[-0].psl_tytype == PSL_ONSET)
	    opf_cb->opf_alter = OPF_NOSUBSELECT; 
	else
	    opf_cb->opf_alter = OPF_SUBSELECT;
} break;

case 438:
if (!((*yyvarspp)->bypass_actions))
/* # line 8425 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	       status;
	OPF_CB		       *opf_cb;

	psq_cb->psq_mode = PSQ_SQEP;	/* NOTE: PSQ_SQEP does for any OPF
					** alter values */

	/* Create control block for setting hash operations indicator in OPF */
	status = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(OPF_CB), (PTR *) &opf_cb,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_mroot(cb, &cb->pss_ostream, (PTR) opf_cb,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	/* Fill in OPF control block */
	opf_cb->opf_length = sizeof(opf_cb);
	opf_cb->opf_type = OPFCB_CB;
	opf_cb->opf_owner = (PTR)DB_PSF_ID;
	opf_cb->opf_ascii_id = OPFCB_ASCII_ID;
	opf_cb->opf_level = OPF_SESSION;
	if (yacc_cb->yypvt[-0].psl_tytype == PSL_ONSET)
	    opf_cb->opf_alter = OPF_HASH;
	else
	    opf_cb->opf_alter = OPF_NOHASH;
    } break;

case 439:
if (!((*yyvarspp)->bypass_actions))
/* # line 8459 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	       status;
	OPF_CB		       *opf_cb;

	psq_cb->psq_mode = PSQ_SQEP;	/* NOTE: PSQ_SQEP does for any OPF
					** alter values */

	/* Create control block for setting OJ flattening indicator in OPF */
	status = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(OPF_CB), (PTR *) &opf_cb,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_mroot(cb, &cb->pss_ostream, (PTR) opf_cb,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	/* Fill in OPF control block */
	opf_cb->opf_length = sizeof(opf_cb);
	opf_cb->opf_type = OPFCB_CB;
	opf_cb->opf_owner = (PTR)DB_PSF_ID;
	opf_cb->opf_ascii_id = OPFCB_ASCII_ID;
	opf_cb->opf_level = OPF_SESSION;
	if (yacc_cb->yypvt[-0].psl_tytype == PSL_ONSET)
	    opf_cb->opf_alter = OPF_OJSUBSELECT;
	else
	    opf_cb->opf_alter = OPF_NOOJSUBSELECT;
    } break;

case 440:
if (!((*yyvarspp)->bypass_actions))
/* # line 8493 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	       status;
	OPF_CB		       *opf_cb;

	psq_cb->psq_mode = PSQ_SQEP;	/* NOTE: PSQ_SQEP does for any OPF
					** alter values */

	/* Create control block for setting parallel query indicator in OPF */
	status = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(OPF_CB), (PTR *) &opf_cb,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_mroot(cb, &cb->pss_ostream, (PTR) opf_cb,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	/* Fill in OPF control block */
	opf_cb->opf_length = sizeof(opf_cb);
	opf_cb->opf_type = OPFCB_CB;
	opf_cb->opf_owner = (PTR)DB_PSF_ID;
	opf_cb->opf_ascii_id = OPFCB_ASCII_ID;
	opf_cb->opf_level = OPF_SESSION;
	opf_cb->opf_value = yacc_cb->yypvt[-0].psl_inttype;
	if (yacc_cb->yypvt[-1].psl_tytype == PSL_ONSET)
	    opf_cb->opf_alter = OPF_PARALLEL;
	else
	{
	    opf_cb->opf_alter = OPF_NOPARALLEL;
	    opf_cb->opf_value = -1;
	}
    } break;

case 441:
if (!((*yyvarspp)->bypass_actions))
/* # line 8531 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
    } break;

case 442:
if (!((*yyvarspp)->bypass_actions))
/* # line 8534 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_inttype = 4;
    } break;

case 443:
if (!((*yyvarspp)->bypass_actions))
/* # line 8541 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	psq_cb->psq_mode = PSQ_SETRANDOMSEED;
	psq_cb->psq_random_seed = yacc_cb->yypvt[-0].psl_inttype;
    } break;
	}
	return(0);
}

i4
psl10iftn(yacc_cb, yyrule, yystatusp, yyvarspp, cb, psq_cb)
YACC_CB		*yacc_cb;
i4		yyrule;
PSS_SESBLK	*cb;
DB_STATUS	*yystatusp;
YYVARS		**yyvarspp;
PSQ_CB *	psq_cb;
{
    YYVARS	*yyvarsp = *yyvarspp;
	switch(yyrule)
	{
case 444:
if (!((*yyvarspp)->bypass_actions))
/* # line 8546 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	u_i4	tmppid;
	u_i4	tmpnow = TMsecs();

	PCpid(&tmppid);
	psq_cb->psq_mode = PSQ_SETRANDOMSEED;
	psq_cb->psq_random_seed = tmppid * tmpnow; 
    } break;

case 445:
if (!((*yyvarspp)->bypass_actions))
/* # line 8557 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;
	DB_STATUS	       status;
	OPF_CB		       *opf_cb;
	char		*val = yacc_cb->yypvt[-0].psl_strtype;

	psq_cb->psq_mode = PSQ_SRINTO;

	/* Allocate OPF control block for set ret_into command */
	status = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(OPF_CB), (PTR *) &opf_cb,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	status = psf_mroot(cb, &cb->pss_ostream, (PTR) opf_cb,
	    &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	/* Fill in OPF control block */
	opf_cb->opf_length = sizeof(opf_cb);
	opf_cb->opf_type = OPFCB_CB;
	opf_cb->opf_owner = (PTR)DB_PSF_ID;
	opf_cb->opf_ascii_id = OPFCB_ASCII_ID;
	opf_cb->opf_alter = OPF_RET_INTO;
	opf_cb->opf_level = OPF_SESSION;
	opf_cb->opf_compressed = FALSE;

	/* Decode storage structure.  Error if unknown. */

	if (CMcmpnocase(val, "c") == 0)
	{
	    opf_cb->opf_compressed = TRUE;
	    val = CMnext(val);
	}
	opf_cb->opf_value = uld_struct_xlate(val);
	if (opf_cb->opf_value == 0 || opf_cb->opf_value == DB_RTRE_STORE)
	{
	    (VOID) psf_error(5915L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, (i4) STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	}
    } break;

case 447:
if (!((*yyvarspp)->bypass_actions))
/* # line 8615 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
        DB_STATUS               status;
        QEF_RCB                 *qef_rcb;

        psq_cb->psq_mode = PSQ_SET_SESSION;

        /* Allocate control block for QEC_ALTER call for set session */
        status = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream, &psq_cb->psq_error);
        if (DB_FAILURE_MACRO(status))
            return (status);

        status = psf_malloc(cb, &cb->pss_ostream, sizeof(QEF_RCB), (PTR *) &qef_rcb,
            &psq_cb->psq_error);
        if (DB_FAILURE_MACRO(status))
            return (status);

        status = psf_mroot(cb, &cb->pss_ostream, (PTR) qef_rcb, &psq_cb->psq_error);
        if (DB_FAILURE_MACRO(status))
            return (status);

        cb->pss_object = (PTR) qef_rcb;

        /*
        ** Fill in the QEF control block.
        ** The acutal session alter parameters will be formatted in
        ** productions below.
        */
        qef_rcb->qef_length     = sizeof(QEF_RCB);
        qef_rcb->qef_type       = QEFRCB_CB;
        qef_rcb->qef_owner      = (PTR)DB_PSF_ID;
        qef_rcb->qef_ascii_id   = QEFRCB_ASCII_ID;
        qef_rcb->qef_eflag      = QEF_INTERNAL;
        qef_rcb->qef_asize      = 0;
	qef_rcb->qef_sess_id    = cb->pss_sessid;
    } break;

case 450:
if (!((*yyvarspp)->bypass_actions))
/* # line 8655 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	psq_cb->psq_ret_flag |= PSQ_SET_ISOLATION;
	psq_cb->psq_isolation = yacc_cb->yypvt[-0].psl_inttype;
    } break;

case 452:
if (!((*yyvarspp)->bypass_actions))
/* # line 8665 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
        DB_STATUS               status;
        i4                 err_code;
        QEF_RCB                 *qef_rcb;

        qef_rcb = (QEF_RCB *) cb->pss_object;

        /*
        ** Validate session option.
        **
        ** Currently, the only supported option is "on_error".
        */
        if (STcasecmp(yacc_cb->yypvt[-0].psl_strtype, "on_error"))
        {
            _VOID_ psf_error(E_PS0F82_SET_SESSION_STMT, 0L, PSF_USERERR,
                &err_code, &psq_cb->psq_error, 1, (i4) STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
            return (E_DB_ERROR);    /* non-zero return means error */
        }

        /*
        ** Can only specify this once
        */
        if(psq_cb->psq_ret_flag & PSQ_SET_ON_ERROR)
        {
            (VOID) psf_error(6351L, 0L, PSF_USERERR, &err_code,
                &psq_cb->psq_error, 2,
                (i4) sizeof("SET SESSION")-1, "SET SESSION",
                (i4) sizeof("ON_ERROR")-1, "ON_ERROR");
 
            return E_DB_ERROR;
        }
 
        psq_cb->psq_ret_flag|= PSQ_SET_ON_ERROR;
 

        /*
        ** Allocate memory for alter control block.
        */
        status = psf_malloc(cb, &cb->pss_ostream, (i4) sizeof(QEF_ALT),
            (PTR *) &qef_rcb->qef_alt, &psq_cb->psq_error);
        if (DB_FAILURE_MACRO(status))
            return (status);

        qef_rcb->qef_asize = 1;
        qef_rcb->qef_alt->alt_code = 0;
    } break;

case 453:
if (!((*yyvarspp)->bypass_actions))
/* # line 8714 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
        QEF_RCB                 *qef_rcb;
        i4                 err_code;

        /*
        ** Rollback statement on error option.
        */
        qef_rcb = (QEF_RCB *) cb->pss_object;

	/*
	** Verify that specified option is ROLLBACK.
	*/
        if (STcasecmp(yacc_cb->yypvt[-1].psl_strtype, "rollback") != 0)
	{
            _VOID_ psf_error(E_PS0F84_SET_ONERROR_STMT, 0L, PSF_USERERR,
                &err_code, &psq_cb->psq_error, 1, (i4) STlength(yacc_cb->yypvt[-1].psl_strtype), yacc_cb->yypvt[-1].psl_strtype);
            return (E_DB_ERROR);
	}

        /*
        ** Validate rollback type - must be STATEMENT or TRANSACTION.
        ** Fill in on_error type in qef_rcb control block.
        */
        if (STcasecmp(yacc_cb->yypvt[-0].psl_strtype, "statement") == 0)
        {
            qef_rcb->qef_alt->alt_code = QEC_ONERROR_STATE;
            qef_rcb->qef_alt->alt_value.alt_i4 = QEF_STMT_ROLLBACK;
        }
        else if (STcasecmp(yacc_cb->yypvt[-0].psl_strtype, "transaction") == 0)
        {
            qef_rcb->qef_alt->alt_code = QEC_ONERROR_STATE;
            qef_rcb->qef_alt->alt_value.alt_i4 = QEF_XACT_ROLLBACK;
        }
        else
        {
            _VOID_ psf_error(E_PS0F84_SET_ONERROR_STMT, 0L, PSF_USERERR,
                &err_code, &psq_cb->psq_error, 1, (i4) STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
            return (E_DB_ERROR);
        }
    } break;

case 454:
if (!((*yyvarspp)->bypass_actions))
/* # line 8757 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	    psq_cb->psq_ret_flag |=  PSQ_SET_ACCESSMODE;
	    psq_cb->psq_accessmode = DMC_C_READ_ONLY;
	} break;

case 455:
if (!((*yyvarspp)->bypass_actions))
/* # line 8762 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	    psq_cb->psq_ret_flag |=  PSQ_SET_ACCESSMODE;
	    psq_cb->psq_accessmode = DMC_C_READ_WRITE;
	} break;

case 456:
if (!((*yyvarspp)->bypass_actions))
/* # line 8769 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	psq_cb->psq_mode = PSQ_SSQL;

	/* Change query language in session control block */
	cb->pss_lang = DB_SQL;
    } break;

case 457:
if (!((*yyvarspp)->bypass_actions))
/* # line 8778 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;

	/* "set result_structure" is not legal in QUEL. Issue warning */
	psq_cb->psq_mode = PSQ_OBSOLETE;

	if (cb->pss_ses_flag & PSS_WARNINGS)
	{
	    /* "set result_structure" is not supported for quel */
	    (VOID) psf_error(5977L, 0L, PSF_USERERR,
		&err_code, &psq_cb->psq_error, 0);
	}
    } break;

case 458:
if (!((*yyvarspp)->bypass_actions))
/* # line 8794 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	    status;
	i4		    rngvar_info;
	QEU_CB		    *qeu_cb;
	DMT_CB		    *dmt_cb;
	PSS_RNGTAB	    *rngvar;
	DB_TAB_NAME	    tabname;
	DMT_CHAR_ENTRY	    *chr;
	DB_ERROR	    e_error;
	i4		    err_code;

	psq_cb->psq_mode = PSQ_SSTATS;

	status = psy_ckdbpr(psq_cb, (u_i4) DBPR_TABLE_STATS);

	if (DB_FAILURE_MACRO(status))
	{
	    /*
	    ** Not allowed to change table stats
	    */
	    if ( Psf_srvblk->psf_capabilities & PSF_C_C2SECURE )
	    {
		(VOID)  psy_secaudit(FALSE, cb, 
			yacc_cb->yypvt[-0].psl_strtype,
			NULL, 
			STlength(yacc_cb->yypvt[-0].psl_strtype),
			SXF_E_TABLE, I_SX273C_TABLE_STATS,
			SXF_A_FAIL | SXF_A_CONTROL, 
			&e_error);
	    }

	     (VOID) psf_error(E_PS035B_CANT_ACCESS_TBL_STATS, 
			0L, PSF_USERERR, &err_code, &psq_cb->psq_error, 0);
	    return E_DB_ERROR;
	}

	/* Allocate QEU_CB for SET STATISTICS and initialize its header */
	status = psl_qeucb(cb, DMT_ALTER, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);

	qeu_cb = (QEU_CB *) cb->pss_object;

	/* Allocate a DMT control block */
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(DMT_CB),
	    (PTR *) &dmt_cb, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	qeu_cb->qeu_d_cb = (PTR) dmt_cb;

	/* Fill in the DMT control block header */
	dmt_cb->type = DMT_TABLE_CB;
	dmt_cb->length = sizeof(DMT_CB);
	dmt_cb->dmt_db_id = (char*) cb->pss_dbid;
        dmt_cb->dmt_mustlock = FALSE;

	/* Allocate the characteristics array */
	status = psf_malloc(cb, &cb->pss_ostream, sizeof(DMT_CHAR_ENTRY),
	    (PTR *) &dmt_cb->dmt_char_array.data_address, &psq_cb->psq_error);
	if (status != E_DB_OK)
	    return (status);
	dmt_cb->dmt_char_array.data_in_size = sizeof(DMT_CHAR_ENTRY);

	/* Make the characteristic being changed the journaled bit */
	chr = (DMT_CHAR_ENTRY *) dmt_cb->dmt_char_array.data_address;
	chr->char_id = DMT_C_ZOPTSTATS;
	if (yacc_cb->yypvt[-1].psl_tytype == PSL_ONSET)
	    chr->char_value = DMT_C_ON;
	else
	    chr->char_value = DMT_C_OFF;

	/* Look up the table id */
	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(DB_TAB_NAME), (char *) &tabname);
	status = psl_rngent(&cb->pss_auxrng, -1, "", &tabname, cb,
	    FALSE, &rngvar, psq_cb->psq_mode, &psq_cb->psq_error, &rngvar_info);
	if (status != E_DB_OK)
	    return (status);
	STRUCT_ASSIGN_MACRO(rngvar->pss_tabid, dmt_cb->dmt_id);
    } break;

case 459:
if (!((*yyvarspp)->bypass_actions))
/* # line 8876 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;
	SCF_CB		       scf_cb;
	SCF_SCI		       sci_list;
	DB_TERM_NAME	       termname;
	CL_ERR_DESC	       cl_error;
	DB_STATUS	       status;

	psq_cb->psq_mode = PSQ_STRACE;

	/*
	** Possible single parameters are "terminal", "output", "input",
	** "point".  Error is anything else.
	*/
	if (!STcasecmp(yacc_cb->yypvt[-0].psl_strtype, "terminal"))
	{
	    if (yacc_cb->yypvt[-1].psl_tytype == PSL_ONSET)
	    {
		/* Get user's terminal name */
	        scf_cb.scf_length = sizeof(SCF_CB);
	        scf_cb.scf_type = SCF_CB_TYPE;
	        scf_cb.scf_facility = DB_PSF_ID;
	        scf_cb.scf_session = DB_NOSESSION;
		scf_cb.scf_len_union.scf_ilength = 1;
		/* may cause lint message */
		scf_cb.scf_ptr_union.scf_sci = (SCI_LIST*) &sci_list;
		sci_list.sci_code = SCI_UTTY;
		sci_list.sci_length = sizeof(termname);
		sci_list.sci_aresult = (PTR) &termname;
		sci_list.sci_rlength = NULL;
		status = scf_call(SCU_INFORMATION, &scf_cb);
		if (status == E_DB_OK)
		{
		    /* set trace terminal */
                    if (TRset_file(TR_T_OPEN, termname.db_term_name,
                        sizeof(termname), &cl_error))
                    {
                        (VOID) psf_error(5988L, 0L, PSF_USERERR, &err_code,
                                &psq_cb->psq_error, 1,
                                sizeof (termname.db_term_name),
                                termname.db_term_name);
                    }
		}
		else
		{
		    (VOID) psf_error(E_PS0378_SCF_TERM_ERR,
			scf_cb.scf_error.err_code, PSF_INTERR,
			&err_code, &psq_cb->psq_error, 0);
		}
	    }
	    else
	    {
		/* set notrace terminal */
		TRset_file(TR_T_CLOSE, NULL, 0, &cl_error);
	    }
	}
	else if (!STcasecmp(yacc_cb->yypvt[-0].psl_strtype, "output"))
	{
	    /* "set trace output" without filename not allowed */
	    if (yacc_cb->yypvt[-1].psl_tytype == PSL_ONSET)
	    {
		(VOID) psf_error(5973L, 0L, PSF_USERERR, &err_code,
		    &psq_cb->psq_error, 0);
		return (E_DB_ERROR);	/* non-zero return means error */
	    }
	    else
	    {
		/* set notrace output */
		TRset_file(TR_F_CLOSE, NULL, 0, &cl_error);
	    }
	}
	else if (!STcasecmp(yacc_cb->yypvt[-0].psl_strtype, "input"))
	{
	    /* "set trace input" without filename not allowed */
	    if (yacc_cb->yypvt[-1].psl_tytype == PSL_ONSET)
	    {
		(VOID) psf_error(5973L, 0L, PSF_USERERR, &err_code,
		    &psq_cb->psq_error, 0);
		return (E_DB_ERROR);	/* non-zero return means error */
	    }
	    else
	    {
		/* set notrace input */
		TRset_file(TR_I_CLOSE, NULL, 0, &cl_error);
	    }
	}
	else if (!STcasecmp(yacc_cb->yypvt[-0].psl_strtype, "point"))
	{
	    /* "set trace point" without facility code not allowed */
	    (VOID) psf_error(5965L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 0);
	    return (E_DB_ERROR);	/* non-zero return means error */
	}
	else
	{
	    (VOID) psf_error(5962L, 0L,  PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, (i4) STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);	/* non-zero return means error */
	}
    } break;

case 460:
if (!((*yyvarspp)->bypass_actions))
/* # line 8977 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;
	CL_ERR_DESC	       cl_error;

	psq_cb->psq_mode = PSQ_STRACE;

	/*
	** Valid parameters are "terminal", "output", "input", "point".
	** Error if none of these.
	*/
	if (!STcasecmp(yacc_cb->yypvt[-1].psl_strtype, "terminal"))
	{
	    /* "set notrace terminal" must not have a second parm */
	    if (yacc_cb->yypvt[-2].psl_tytype == PSL_OFFSET)
	    {
		(VOID) psf_error(5963L, 0L, PSF_USERERR, &err_code,
		    &psq_cb->psq_error, 0);
		return (E_DB_ERROR);	/* non-zero return means error */
	    }
	    else
	    {
		/* set trace terminal termname */
                if (TRset_file(TR_T_OPEN, yacc_cb->yypvt[-0].psl_strtype, STlength(yacc_cb->yypvt[-0].psl_strtype), &cl_error))
                {
                    (VOID) psf_error(5988L, 0L, PSF_USERERR, &err_code,
                        &psq_cb->psq_error, 1,
                        STlength (yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
                }
	    }
	}
	else if (!STcasecmp(yacc_cb->yypvt[-1].psl_strtype, "output"))
	{
	    /* "set notrace output" must not have a second parm */
	    if (yacc_cb->yypvt[-2].psl_tytype == PSL_OFFSET)
	    {
		(VOID) psf_error(5964L, 0L, PSF_USERERR, &err_code,
		    &psq_cb->psq_error, 0);
		return (E_DB_ERROR);	/* non-zero return means error */
	    }
	    else
	    {
		/* set trace output filename */
                if (TRset_file(TR_F_SAFEOPEN, yacc_cb->yypvt[-0].psl_strtype, STlength(yacc_cb->yypvt[-0].psl_strtype), &cl_error))
                {
                    (VOID) psf_error(5988L, 0L, PSF_USERERR, &err_code,
                        &psq_cb->psq_error, 1,
                        STlength (yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
                }
	    }
	}
	else if (!STcasecmp(yacc_cb->yypvt[-1].psl_strtype, "input"))
	{
	    /* "set notrace input" must not have a second parm */
	    if (yacc_cb->yypvt[-2].psl_tytype == PSL_OFFSET)
	    {
		(VOID) psf_error(5964L, 0L, PSF_USERERR, &err_code,
		    &psq_cb->psq_error, 0);
		return (E_DB_ERROR);	/* non-zero return means error */
	    }
	    else
	    {
		/* set trace input filename */
                if (TRset_file(TR_I_OPEN, yacc_cb->yypvt[-0].psl_strtype, STlength(yacc_cb->yypvt[-0].psl_strtype), &cl_error))
                {
                    (VOID) psf_error(5988L, 0L, PSF_USERERR, &err_code,
                        &psq_cb->psq_error, 1,
                        STlength (yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
                }
	    }
	}
	else if (!STcasecmp(yacc_cb->yypvt[-1].psl_strtype, "point"))
	{
	    i4                 facility;	/* facility code */
	    i4		       flagno;		/* flag number w/i facility */
	    i4	       status;
	    DB_DEBUG_CB	       *debug_cb;
	    DB_STATUS	       ret_val;

	    /* Allocate the debug control block */
	    ret_val = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream,
		&psq_cb->psq_error);
	    if (ret_val != E_DB_OK)
		return (ret_val);
	    ret_val = psf_malloc(cb, &cb->pss_ostream, sizeof(DB_DEBUG_CB),
		(PTR *) &debug_cb, &psq_cb->psq_error);
	    if (ret_val != E_DB_OK)
		return (ret_val);
	    ret_val = psf_mroot(cb, &cb->pss_ostream, (PTR) debug_cb,
		&psq_cb->psq_error);
	    if (ret_val != E_DB_OK)
		return (ret_val);
	    debug_cb->db_value_count = 0;
	    if (yacc_cb->yypvt[-2].psl_tytype == PSL_ONSET)
		debug_cb->db_trswitch = DB_TR_ON;
	    else
		debug_cb->db_trswitch = DB_TR_OFF;

	    /*
	    ** Decode the facility code.  getfacil() returns an error code
	    ** for psf_error if the facility code doesn't make sense.
	    ** FIXME - facility code is not set */
	    if ((status = getfacil(yacc_cb->yypvt[-0].psl_strtype, &facility, &flagno)) != E_PS0000_OK)
	    {
		(VOID) psf_error(status, 0L, PSF_USERERR, &err_code,
		    &psq_cb->psq_error, 1, (i4) STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
		return (E_DB_ERROR);	/* non-zero return means error */
	    }
	    else
	    {
		debug_cb->db_trace_point = flagno;
		psq_cb->psq_mode = facility;

	    	if (   facility == PSQ_QEFTRACE && flagno == QEF_T_NORULES
		    && MEcmp((PTR)&cb->pss_user, (PTR)&cb->pss_dba,
			    sizeof(DB_OWN_NAME)) != 0
		   )
		{
		    _VOID_ psf_error(E_US18AA_6314_SET_NORULES, 0L, PSF_USERERR,
				     &err_code, &psq_cb->psq_error, 0);
		    return (E_DB_ERROR);
		}
	    }
	}
	else
	{
	    (VOID) psf_error(5962L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, (i4) STlength(yacc_cb->yypvt[-1].psl_strtype), yacc_cb->yypvt[-1].psl_strtype);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}
    } break;

case 461:
if (!((*yyvarspp)->bypass_actions))
/* # line 9108 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4	    err_code;

	/*
	** This form of the "set [no]trace command makes sense only with
	** "set [no]trace point".
	*/
	if (!STcasecmp(yacc_cb->yypvt[-2].psl_strtype, "terminal") ||
	    !STcasecmp(yacc_cb->yypvt[-2].psl_strtype, "output")   ||
	    !STcasecmp(yacc_cb->yypvt[-2].psl_strtype, "input"))
	{
	    (VOID) psf_error(5968L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 0);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}
	else if (!STcasecmp(yacc_cb->yypvt[-2].psl_strtype, "point"))
	{
	    i4		       facility;	/* facility code */
	    i4		       flagno;		/* flag number w/i facility */
	    i4	       status;
	    DB_STATUS	       ret_val;
	    DB_DEBUG_CB	       *debug_cb;

	    /* Allocate the debug control block */
	    ret_val = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream,
		&psq_cb->psq_error);
	    if (ret_val != E_DB_OK)
		return (ret_val);
	    ret_val = psf_malloc(cb, &cb->pss_ostream, sizeof(DB_DEBUG_CB),
		(PTR *) &debug_cb, &psq_cb->psq_error);
	    if (ret_val != E_DB_OK)
		return (ret_val);
	    ret_val = psf_mroot(cb, &cb->pss_ostream, (PTR) debug_cb,
		&psq_cb->psq_error);
	    if (ret_val != E_DB_OK)
		return (ret_val);
	    debug_cb->db_value_count = 1;
	    debug_cb->db_vals[0] = yacc_cb->yypvt[-0].psl_inttype;
	    if (yacc_cb->yypvt[-3].psl_tytype == PSL_ONSET)
		debug_cb->db_trswitch = DB_TR_ON;
	    else
		debug_cb->db_trswitch = DB_TR_OFF;

	    /*
	    ** Decode the facility code.  getfacil() returns an error code
	    ** for psf_error if the facility code doesn't make sense.
	    */
	    if ((status = getfacil(yacc_cb->yypvt[-1].psl_strtype, &facility, &flagno)) != E_PS0000_OK)
	    {
		(VOID) psf_error(status, 0L, PSF_USERERR, &err_code,
		    &psq_cb->psq_error, 1, (i4) STlength(yacc_cb->yypvt[-1].psl_strtype), yacc_cb->yypvt[-1].psl_strtype);
		return (E_DB_ERROR);
	    }
	    else
	    {
		psq_cb->psq_mode = facility;
		debug_cb->db_trace_point = flagno;

	    	if (   facility == PSQ_QEFTRACE && flagno == QEF_T_NORULES
		    && MEcmp((PTR)&cb->pss_user, (PTR)&cb->pss_dba,
			    sizeof(DB_OWN_NAME)) != 0
		   )
		{
		    _VOID_ psf_error(E_US18AA_6314_SET_NORULES, 0L, PSF_USERERR,
				     &err_code, &psq_cb->psq_error, 0);
		    return (E_DB_ERROR);
		}
	    }
	}
	else
	{
	    (VOID) psf_error(5962L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, (i4) STlength(yacc_cb->yypvt[-2].psl_strtype), yacc_cb->yypvt[-2].psl_strtype);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}
    } break;

case 462:
if (!((*yyvarspp)->bypass_actions))
/* # line 9185 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4	    err_code;

	/*
	** This form of the "set [no]trace" command makes sense only with
	** "set [no]trace point".
	*/
	if (!STcasecmp(yacc_cb->yypvt[-3].psl_strtype, "terminal") ||
	    !STcasecmp(yacc_cb->yypvt[-3].psl_strtype, "output")   ||
	    !STcasecmp(yacc_cb->yypvt[-3].psl_strtype, "input"))
	{
	    (VOID) psf_error(5968L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 0);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}
	else if (!STcasecmp(yacc_cb->yypvt[-3].psl_strtype, "point"))
	{
	    i4		       facility;	/* facility code */
	    i4		       flagno;		/* flag number w/i facility */
	    i4	       status;
	    DB_STATUS	       ret_val;
	    DB_DEBUG_CB	       *debug_cb;

	    /* Allocate the debug control block */
	    ret_val = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream,
		&psq_cb->psq_error);
	    if (ret_val != E_DB_OK)
		return (ret_val);
	    ret_val = psf_malloc(cb, &cb->pss_ostream, sizeof(DB_DEBUG_CB),
		(PTR *) &debug_cb, &psq_cb->psq_error);
	    if (ret_val != E_DB_OK)
		return (ret_val);
	    ret_val = psf_mroot(cb, &cb->pss_ostream, (PTR) debug_cb,
		&psq_cb->psq_error);
	    if (ret_val != E_DB_OK)
		return (ret_val);
	    debug_cb->db_value_count = 2;
	    debug_cb->db_vals[0] = yacc_cb->yypvt[-1].psl_inttype;
	    debug_cb->db_vals[1] = yacc_cb->yypvt[-0].psl_inttype;
	    if (yacc_cb->yypvt[-4].psl_tytype == PSL_ONSET)
		debug_cb->db_trswitch = DB_TR_ON;
	    else
		debug_cb->db_trswitch = DB_TR_OFF;

	    /*
	    ** Decode the facility code.  getfacil() returns an error code
	    ** for psf_error if the facility code doesn't make sense.
	    */
	    if ((status = getfacil(yacc_cb->yypvt[-2].psl_strtype, &facility, &flagno)) != E_PS0000_OK)
	    {
		(VOID) psf_error(status, 0L, PSF_USERERR, &err_code,
		    &psq_cb->psq_error, 1, (i4) STlength(yacc_cb->yypvt[-2].psl_strtype), yacc_cb->yypvt[-2].psl_strtype);
		return (E_DB_ERROR);
	    }
	    else
	    {
		psq_cb->psq_mode = facility;
		debug_cb->db_trace_point = flagno;
	    }
	}
	else
	{
	    (VOID) psf_error(5962L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, (i4) STlength(yacc_cb->yypvt[-3].psl_strtype), yacc_cb->yypvt[-3].psl_strtype);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}
    } break;

case 463:
if (!((*yyvarspp)->bypass_actions))
/* # line 9255 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	psq_cb->psq_mode = PSQ_STRACE;

	/*
	** "set trace" is allowed in distributed, even though the current
	** spec doesn't list it.
	*/
	yacc_cb->yyval.psl_tytype = yacc_cb->yypvt[-0].psl_tytype;
    } break;

case 464:
if (!((*yyvarspp)->bypass_actions))
/* # line 9267 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
        DB_STATUS               status;
 
        status = psl_st0_settransaction(cb, psq_cb);
        if (DB_FAILURE_MACRO(status))
            return(status);
    } break;

case 465:
if (!((*yyvarspp)->bypass_actions))
/* # line 9276 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
        DB_STATUS               status;
 
        status = psl_st1_settranstmnt(cb, psq_cb);
        if (DB_FAILURE_MACRO(status))
            return(status);
    } break;

case 470:
if (!((*yyvarspp)->bypass_actions))
/* # line 9294 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
        DB_STATUS               status;
 
        status = psl_st2_settranisolation_level(cb, &psq_cb->psq_error, 
		(i4)yacc_cb->yypvt[-0].psl_inttype);
        if (DB_FAILURE_MACRO(status))
            return(status);
    } break;

case 471:
if (!((*yyvarspp)->bypass_actions))
/* # line 9305 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_inttype = (i4)DMC_C_READ_COMMITTED;
    } break;

case 472:
if (!((*yyvarspp)->bypass_actions))
/* # line 9309 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_inttype = (i4)DMC_C_READ_UNCOMMITTED;
    } break;

case 473:
if (!((*yyvarspp)->bypass_actions))
/* # line 9313 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_inttype = (i4)DMC_C_REPEATABLE_READ;
    } break;

case 474:
if (!((*yyvarspp)->bypass_actions))
/* # line 9317 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_inttype = (i4)DMC_C_SERIALIZABLE;
    } break;

case 475:
if (!((*yyvarspp)->bypass_actions))
/* # line 9323 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
        DB_STATUS               status;
 
        status = psl_st3_accessmode(cb, &psq_cb->psq_error, yacc_cb->yypvt[-0].psl_inttype);
        if (DB_FAILURE_MACRO(status))
            return(status);
    } break;

case 476:
if (!((*yyvarspp)->bypass_actions))
/* # line 9333 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_inttype = (i4)DMC_C_READ_ONLY;
    } break;

case 477:
if (!((*yyvarspp)->bypass_actions))
/* # line 9337 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_inttype = (i4)DMC_C_READ_WRITE; 
    } break;

case 478:
if (!((*yyvarspp)->bypass_actions))
/* # line 9343 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_inttype = yacc_cb->yypvt[-0].psl_inttype;
    } break;

case 479:
if (!((*yyvarspp)->bypass_actions))
/* # line 9347 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                     num;
	i4		       err_code;

	/*
	** If a string used in place of an integer can't be converted to an
	** integer, give an error.
	*/
	if (CVal(sconvert(yacc_cb->yypvt[-0].psl_textype), &num) != OK)
        {
	    (VOID) psf_error(5969L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, STlength((char *) yacc_cb->yypvt[-0].psl_textype), (char *) yacc_cb->yypvt[-0].psl_textype);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}

	yacc_cb->yyval.psl_inttype = num;
    } break;

case 480:
if (!((*yyvarspp)->bypass_actions))
/* # line 9367 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_strtype = yacc_cb->yypvt[-0].psl_strtype;
    } break;

case 481:
if (!((*yyvarspp)->bypass_actions))
/* # line 9371 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_strtype = yacc_cb->yypvt[-0].psl_strtype;
    } break;
	}
	return(0);
}

i4
psl11iftn(yacc_cb, yyrule, yystatusp, yyvarspp, cb, psq_cb)
YACC_CB		*yacc_cb;
i4		yyrule;
PSS_SESBLK	*cb;
DB_STATUS	*yystatusp;
YYVARS		**yyvarspp;
PSQ_CB *	psq_cb;
{
    YYVARS	*yyvarsp = *yyvarspp;
	switch(yyrule)
	{
case 482:
if (!((*yyvarspp)->bypass_actions))
/* # line 9377 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
        DB_STATUS               status;
        QEF_RCB                 *qef_rcb;

        psq_cb->psq_mode = PSQ_UPD_ROWCNT;

        /* Allocate QEF control block for update_rowcount */
        status = psf_mopen(cb, QSO_QP_OBJ, &cb->pss_ostream, &psq_cb->psq_error);
        if (status != E_DB_OK)
           return (status);
        status = psf_malloc(cb, &cb->pss_ostream, sizeof(QEF_RCB), &cb->pss_object,
                &psq_cb->psq_error);
        if (status != E_DB_OK)
           return (status);
        status = psf_mroot(cb, &cb->pss_ostream, cb->pss_object,
                        &psq_cb->psq_error);
        if (status != E_DB_OK)
           return (status);

        /* Fill in the QEF control block */
        qef_rcb = (QEF_RCB *) cb->pss_object;
        qef_rcb->qef_length = sizeof(QEF_RCB);
        qef_rcb->qef_type = QEFRCB_CB;
        qef_rcb->qef_owner = (PTR)DB_PSF_ID;
        qef_rcb->qef_ascii_id = QEFRCB_ASCII_ID;
	qef_rcb->qef_sess_id = cb->pss_sessid;
        qef_rcb->qef_upd_rowcnt = yacc_cb->yypvt[-0].psl_inttype;
      } break;

case 483:
if (!((*yyvarspp)->bypass_actions))
/* # line 9408 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
        i4         err_code;

        if (STcompare(yacc_cb->yypvt[-0].psl_strtype, "changed") == 0)
           yacc_cb->yyval.psl_inttype = QEF_ROWCHGD;

        else if (STcompare(yacc_cb->yypvt[-0].psl_strtype, "qualified") == 0)
           yacc_cb->yyval.psl_inttype = QEF_ROWQLFD;

        else  /* syntax error */
        {
            /* set update_rowcount syntax error */
            (void) psf_error(E_PS0F85_SET_UPDATE_ROWCOUNT_STMT, 0L, PSF_USERERR, &err_code,
                &psq_cb->psq_error, 1, (i4) STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
            return (E_DB_ERROR);    /* non-zero return means error */
        }
    } break;

case 484:
if (!((*yyvarspp)->bypass_actions))
/* # line 9428 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;
	PST_QNODE	       *rootnode;
	PST_RT_NODE	       root;

	/* Make the ROOT node */
	root.pst_rtuser = TRUE;
	root.pst_union.pst_next  = 0;
	root.pst_dups   = PST_ALLDUPS;

	status = pst_node(cb, &cb->pss_ostream, yacc_cb->yypvt[-1].psl_trtype, yacc_cb->yypvt[-0].psl_trtype, PST_ROOT, (PTR) &root,
	    sizeof(PST_RT_NODE), DB_NODT, (i2) 0, (i4) 0, (DB_ANYTYPE *) NULL,
	    &rootnode, &psq_cb->psq_error, (i4) 0);
	if (DB_FAILURE_MACRO(status))
	    return(status);

	status = psl_cv1_create_view(cb, psq_cb, (PST_QNODE *) NULL, rootnode,
	    (i4) TRUE, yyvarsp);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 485:
if (!((*yyvarspp)->bypass_actions))
/* # line 9452 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_STATEMENT	    *snode = (PST_STATEMENT *) cb->pss_object;
	PST_CREATE_VIEW     *crt_view = &snode->pst_specific.pst_create_view;
	QEUQ_CB		    *qeuq_cb = (QEUQ_CB *) crt_view->pst_qeuqcb;
	DMU_CB		    *dmu_cb = (DMU_CB *) qeuq_cb->qeuq_dmf_cb;

	/* Copy the table name into PST_CREATE_VIEW and DMU_CB */
	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(DB_TAB_NAME), (char *) &crt_view->pst_view_name);
	STRUCT_ASSIGN_MACRO(crt_view->pst_view_name, dmu_cb->dmu_table_name);
    } break;

case 486:
if (!((*yyvarspp)->bypass_actions))
/* # line 9465 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	       status;

	status = psl_cv2_viewstmnt(cb, psq_cb, yyvarsp);
	if (DB_FAILURE_MACRO(status))
	    return(status);
    } break;

case 487:
if (!((*yyvarspp)->bypass_actions))
/* # line 9475 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	    status;

	if (status = pst_trfix(cb, &cb->pss_ostream, yacc_cb->yypvt[-1].psl_trtype, &psq_cb->psq_error))
	    return (status);

	yacc_cb->yyval.psl_trtype = yacc_cb->yypvt[-1].psl_trtype;
    } break;

case 489:
if (!((*yyvarspp)->bypass_actions))
/* # line 9487 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	/*
	** Attach bulk of target list to leftmost node of new element.
	*/
	yacc_cb->yyval.psl_trtype = pst_tlprpnd(yacc_cb->yypvt[-2].psl_trtype, yacc_cb->yypvt[-0].psl_trtype);
    } break;

case 490:
if (!((*yyvarspp)->bypass_actions))
/* # line 9502 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;
	PST_QNODE	       *resdomnode;
	PST_QNODE	       *varnode;
        DB_ATT_NAME	       colname;

	/* normalize the attribute name */
	STmove((char *)yacc_cb->yypvt[-2].psl_strtype, ' ', (u_i4)sizeof(DB_ATT_NAME), (char *)&colname);

	varnode = yacc_cb->yypvt[-0].psl_trtype;
	if (cb->pss_parser_compat & PSQ_I4_TIDS)
	{
	    if ( psq_cb->psq_mode == PSQ_RETRIEVE
	      && varnode->pst_sym.pst_type == PST_VAR
	      && varnode->pst_sym.pst_value.pst_s_var.pst_atno.db_att_id == DB_IMTID
	      && varnode->pst_sym.pst_dataval.db_datatype == DB_TID8_TYPE)
	    {
		varnode->pst_sym.pst_dataval.db_length = DB_TID_LENGTH;
	    }
	}
	status = pst_adresdom((char *) &colname, (PST_QNODE *) NULL, yacc_cb->yypvt[-0].psl_trtype, cb, 
	    psq_cb, &resdomnode);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = resdomnode;
    } break;

case 491:
if (!((*yyvarspp)->bypass_actions))
/* # line 9532 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	status;
	PST_QNODE	*resdomnode;
	PST_QNODE	*varnode;

	varnode = yacc_cb->yypvt[-0].psl_trtype;
	if (cb->pss_parser_compat & PSQ_I4_TIDS)
	{
	    if ( psq_cb->psq_mode == PSQ_RETRIEVE
	      && varnode->pst_sym.pst_type == PST_VAR
	      && varnode->pst_sym.pst_value.pst_s_var.pst_atno.db_att_id == DB_IMTID
	      && varnode->pst_sym.pst_dataval.db_datatype == DB_TID8_TYPE)
	    {
		varnode->pst_sym.pst_dataval.db_length = DB_TID_LENGTH;
	    }
	}
	status =
	  pst_adresdom(yacc_cb->yypvt[-0].psl_trtype->pst_sym.pst_value.pst_s_var.pst_atname.db_att_name,
	    (PST_QNODE *) NULL, yacc_cb->yypvt[-0].psl_trtype, cb, psq_cb, &resdomnode);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = resdomnode;
    } break;

case 492:
if (!((*yyvarspp)->bypass_actions))
/* # line 9559 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;
	PST_VAR_NODE	       var;
	DMT_ATT_ENTRY	       **attribute;
	i4		       i;
	PST_QNODE	       *varnode;
	PST_QNODE	       *rsdmnode = (PST_QNODE *) NULL;
	DB_STATUS	       status;

	/* ".all" not allowed in target list for replace */
	if (psq_cb->psq_mode == PSQ_REPLACE)
	{
	    (VOID) psf_error(2123L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, (i4) sizeof(cb->pss_lineno),
		&cb->pss_lineno);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}

	/* Start at first attribute */
	for (i = 0, attribute = yacc_cb->yypvt[-2].psl_rngtype->pss_attdesc + 1;
	     i < yacc_cb->yypvt[-2].psl_rngtype->pss_tabdesc->tbl_attr_count;
	     i++, attribute++)
	{
	    if ((*attribute)->att_flags & DMT_F_HIDDEN)
		continue;

	    /* Set up var node for copy into tree */
	    var.pst_vno = yacc_cb->yypvt[-2].psl_rngtype->pss_rgno;
	    var.pst_atno.db_att_id = (*attribute)->att_number;
	    STRUCT_ASSIGN_MACRO((*attribute)->att_name, var.pst_atname);

	    /* Create the PST_VAR_NODE */
	    status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
		(PST_QNODE *) NULL, PST_VAR, (char *) &var, sizeof(var),
		(DB_DT_ID) (*attribute)->att_type, (i2) (*attribute)->att_prec,
		(i4) (*attribute)->att_width, (DB_ANYTYPE *) NULL, &varnode,
		&psq_cb->psq_error, (i4) 0);
	    if (status != E_DB_OK)
	    {
		return (status);
	    }

	    /* Create the corresponding result domain node */
	    status = pst_adresdom((char *) &(*attribute)->att_name, rsdmnode,
		varnode, cb, psq_cb, &rsdmnode);
	    if (status != E_DB_OK)
	    {
		return (status);
	    }
	}

	yacc_cb->yyval.psl_trtype = rsdmnode;
    } break;

case 496:
if (!((*yyvarspp)->bypass_actions))
/* # line 9620 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	cb->pss_qualdepth--;

	yacc_cb->yyval.psl_trtype = yacc_cb->yypvt[-0].psl_trtype;
    } break;

case 497:
if (!((*yyvarspp)->bypass_actions))
/* # line 9626 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS	status;
	PST_QNODE	*nonode = (PST_QNODE *) NULL;

	status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
	    (PST_QNODE *) NULL, PST_QLEND, (PTR) NULL, 0, DB_NODT, (i2) 0,
	    (i4) 0, (DB_ANYTYPE *) NULL, &nonode, &psq_cb->psq_error, (i4) 0);
	if (DB_FAILURE_MACRO(status))
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = nonode;
    } break;

case 498:
if (!((*yyvarspp)->bypass_actions))
/* # line 9643 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	cb->pss_qualdepth++;
    } break;

case 499:
if (!((*yyvarspp)->bypass_actions))
/* # line 9649 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_trtype = yacc_cb->yypvt[-1].psl_trtype;
    } break;

case 500:
if (!((*yyvarspp)->bypass_actions))
/* # line 9653 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_QNODE              *newnode;
	DB_STATUS	       status;

	status = pst_node(cb, &cb->pss_ostream, yacc_cb->yypvt[-0].psl_trtype, (PST_QNODE *) NULL, PST_NOT,
	    (PTR) NULL, 0, DB_NODT, (i2) 0, (i4) 0,
	    (DB_ANYTYPE *) NULL, &newnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 501:
if (!((*yyvarspp)->bypass_actions))
/* # line 9668 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_QNODE              *newnode;
	DB_STATUS	       status;

	status = pst_node(cb, &cb->pss_ostream, yacc_cb->yypvt[-2].psl_trtype, yacc_cb->yypvt[-0].psl_trtype, yacc_cb->yypvt[-1].psl_tytype,
	    (PTR) NULL, sizeof(PST_OP_NODE), DB_NODT, (i2) 0, (i4) 0,
	    (DB_ANYTYPE *) NULL, &newnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 503:
if (!((*yyvarspp)->bypass_actions))
/* # line 9686 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_QNODE              *newnode;
	PST_OP_NODE	       opnode;
	DB_STATUS	       status;

	opnode.pst_opno = yacc_cb->yypvt[-1].psl_tytype;
	opnode.pst_opmeta = PST_NOMETA;
	opnode.pst_isescape = PST_DOESNT_APPLY;
	status = pst_node(cb, &cb->pss_ostream, yacc_cb->yypvt[-2].psl_trtype, yacc_cb->yypvt[-0].psl_trtype, PST_BOP,
	    (char *) &opnode, sizeof(opnode), DB_NODT, (i2) 0, (i4) 0,
	    (DB_ANYTYPE *) NULL, &newnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 504:
if (!((*yyvarspp)->bypass_actions))
/* # line 9705 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_QNODE              *newnode;
	PST_OP_NODE	       opnode;
	DB_STATUS	       status;

	opnode.pst_opno = ADI_ISNUL_OP;
	opnode.pst_opmeta = PST_NOMETA;
	opnode.pst_isescape = PST_DOESNT_APPLY;
	status = pst_node(cb, &cb->pss_ostream, yacc_cb->yypvt[-1].psl_trtype, (PST_QNODE*) NULL, PST_UOP,
	    (char *) &opnode, sizeof(opnode), DB_NODT, (i2) 0, (i4) 0,
	    (DB_ANYTYPE *) NULL, &newnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 505:
if (!((*yyvarspp)->bypass_actions))
/* # line 9724 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_QNODE              *newnode;
	PST_OP_NODE	       opnode;
	DB_STATUS	       status;

	opnode.pst_opno = ADI_NONUL_OP;
	opnode.pst_opmeta = PST_NOMETA;
	opnode.pst_isescape = PST_DOESNT_APPLY;
	status = pst_node(cb, &cb->pss_ostream, yacc_cb->yypvt[-2].psl_trtype, (PST_QNODE*) NULL, PST_UOP,
	    (char *) &opnode, sizeof(opnode), DB_NODT, (i2) 0, (i4) 0,
	    (DB_ANYTYPE *) NULL, &newnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 506:
if (!((*yyvarspp)->bypass_actions))
/* # line 9745 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_tytype = yacc_cb->yypvt[-0].psl_tytype;
    } break;

case 507:
if (!((*yyvarspp)->bypass_actions))
/* # line 9749 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_tytype = yacc_cb->yypvt[-0].psl_tytype;
    } break;

case 508:
if (!((*yyvarspp)->bypass_actions))
/* # line 9753 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_tytype = yacc_cb->yypvt[-0].psl_tytype;
    } break;

case 509:
if (!((*yyvarspp)->bypass_actions))
/* # line 9759 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	  /* Indicate that there is an aggregate in this tree */
	  cb->pss_stmt_flags |= PSS_AGINTREE;  /* pss_agintree = TRUE */

	/*
	** if processing DEFINE VIEW and this aggregate occurred outside of a
	** view's qualification, remember that this view will be non-updatable
	** and why
	*/
	if (psq_cb->psq_mode == PSQ_VIEW && cb->pss_qualdepth == 0)
	{
	    yyvarsp->nonupdt = TRUE;
	    
	    /* ok, ok - so we ARE using SQL terminology... */
	    yyvarsp->nonupdt_reason |= PSS_SET_FUNC_IN_OUTERMOST_SUBSEL;
	}
    } break;

case 511:
if (!((*yyvarspp)->bypass_actions))
/* # line 9778 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_QNODE              *newnode;
	PST_OP_NODE	       opnode;
	DB_STATUS	       status;

	opnode.pst_opno = yacc_cb->yypvt[-1].psl_tytype;
	opnode.pst_opmeta = PST_NOMETA;
	opnode.pst_isescape = PST_DOESNT_APPLY;
	status = pst_node(cb, &cb->pss_ostream, yacc_cb->yypvt[-2].psl_trtype, yacc_cb->yypvt[-0].psl_trtype, PST_BOP,
	    (char *) &opnode, sizeof(opnode), DB_NODT, (i2) 0, (i4) 0,
	    (DB_ANYTYPE *) NULL, &newnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}


	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 512:
if (!((*yyvarspp)->bypass_actions))
/* # line 9798 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_QNODE              *newnode;
	PST_OP_NODE	       opnode;
	DB_STATUS	       status;

	opnode.pst_opno = yacc_cb->yypvt[-1].psl_tytype;
	opnode.pst_opmeta = PST_NOMETA;
	opnode.pst_isescape = PST_DOESNT_APPLY;
	status = pst_node(cb, &cb->pss_ostream, yacc_cb->yypvt[-2].psl_trtype, yacc_cb->yypvt[-0].psl_trtype, PST_BOP,
	    (char *) &opnode, sizeof(opnode), DB_NODT, (i2) 0, (i4) 0,
	    (DB_ANYTYPE *) NULL, &newnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 513:
if (!((*yyvarspp)->bypass_actions))
/* # line 9817 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_QNODE              *newnode;
	PST_OP_NODE	       opnode;
	DB_STATUS	       status;

	opnode.pst_opno = yacc_cb->yypvt[-1].psl_tytype;
	opnode.pst_opmeta = PST_NOMETA;
	opnode.pst_isescape = PST_DOESNT_APPLY;
	status = pst_node(cb, &cb->pss_ostream, yacc_cb->yypvt[-2].psl_trtype, yacc_cb->yypvt[-0].psl_trtype, PST_BOP,
	    (char *) &opnode, sizeof(opnode), DB_NODT, (i2) 0, (i4) 0,
	    (DB_ANYTYPE *) NULL, &newnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}


	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 514:
if (!((*yyvarspp)->bypass_actions))
/* # line 9837 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_trtype = yacc_cb->yypvt[-1].psl_trtype;
    } break;

case 515:
if (!((*yyvarspp)->bypass_actions))
/* # line 9841 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_QNODE              *newnode;
	PST_OP_NODE	       opnode;
	DB_STATUS	       status;

	opnode.pst_opno = yacc_cb->yypvt[-1].psl_tytype;
	opnode.pst_opmeta = PST_NOMETA;
	opnode.pst_isescape = PST_DOESNT_APPLY;
	status = pst_node(cb, &cb->pss_ostream, yacc_cb->yypvt[-0].psl_trtype, (PST_QNODE *) NULL, PST_UOP,
	    (char *) &opnode, sizeof(opnode), DB_NODT, (i2) 0, (i4) 0,
	    (DB_ANYTYPE *) NULL, &newnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 516:
if (!((*yyvarspp)->bypass_actions))
/* # line 9860 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_OP_NODE	       opnode;
	PST_QNODE	       *newnode;
	PST_QNODE	       *dec_node = NULL;
	DB_STATUS	       status;


	/* This is a necessary hack for setting default precision and scale
	** for the decimal() conversion function.
	*/
	if (yacc_cb->yypvt[-3].psl_tytype == ADI_DEC_OP)
	{
	    PST_CNST_NODE	cconst;
	    DB_DT_ID		type;
	    i4			len;
	    i2			ps;
	    i4		err_code;


	    type = yacc_cb->yypvt[-1].psl_trtype->pst_sym.pst_dataval.db_datatype;
	    len  = yacc_cb->yypvt[-1].psl_trtype->pst_sym.pst_dataval.db_length - ((type < 0) ? 1 : 0);

	    switch (abs(type))
	    {
	      case DB_FLT_TYPE:
	      case DB_DEC_TYPE:
	      case DB_MNY_TYPE:
	        ps = DB_PS_ENCODE_MACRO(AD_FP_TO_DEC_PREC, AD_FP_TO_DEC_SCALE);
	    	break;

	      case DB_INT_TYPE:
		switch (len)
		{
		  case 1:
		    ps = DB_PS_ENCODE_MACRO(AD_I1_TO_DEC_PREC,
					    AD_I1_TO_DEC_SCALE);
		    break;
		  case 2:
		    ps = DB_PS_ENCODE_MACRO(AD_I2_TO_DEC_PREC,
					    AD_I2_TO_DEC_SCALE);
		    break;
		  case 4:
		    ps = DB_PS_ENCODE_MACRO(AD_I4_TO_DEC_PREC,
					    AD_I4_TO_DEC_SCALE);
		    break;
		}
		break;

	      default:
	        {
		    ADI_DT_NAME		dt_name;

		    /* can't default precision/scale for this datatype */
		    status = adi_tyname((ADF_CB *) cb->pss_adfcb, 
					abs(type), &dt_name);

		    (VOID) psf_error(E_PS0C80_NO_DEFLT_ON_DT, 0L, PSF_USERERR,
			&err_code, &psq_cb->psq_error, 1,
			psf_trmwhite(sizeof(ADI_DT_NAME), (char *) &dt_name), 
			&dt_name);
		    return (E_DB_ERROR);
		}
	    }

	    /* Now we have the default precision and scale; build a const
	    ** node and put prec_scale in as an i2
	    */
	    cconst.pst_tparmtype = PST_USER;
	    cconst.pst_parm_no = 0;
	    cconst.pst_pmspec  = PST_PMNOTUSED;
	    cconst.pst_cqlang = DB_SQL;
	    cconst.pst_origtxt = (char *) NULL;
	    status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
		(PST_QNODE *) NULL, PST_CONST, (char *) &cconst,
		sizeof(cconst), DB_INT_TYPE, (i2) 0, (i4) 2,
		(DB_ANYTYPE *) &ps, &dec_node, &psq_cb->psq_error, (i4) 0);
	    if (status != E_DB_OK)
	    {
		return (status);
	    }
	}
	    
	opnode.pst_opno = yacc_cb->yypvt[-3].psl_tytype;
	opnode.pst_opmeta = PST_NOMETA;
	opnode.pst_isescape = PST_DOESNT_APPLY;
	status = pst_node(cb, &cb->pss_ostream, yacc_cb->yypvt[-1].psl_trtype, dec_node,
	    (dec_node == NULL)  ?  PST_UOP  :  PST_BOP,
	    (char *) &opnode, sizeof(opnode), DB_NODT, (i2) 0, (i4) 0,
	    (DB_ANYTYPE *) NULL, &newnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}


	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 517:
if (!((*yyvarspp)->bypass_actions))
/* # line 9958 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_OP_NODE	       opnode;
	PST_QNODE	       *newnode;
	DB_STATUS	       status;

    	/*
        ** The second argument of the two argument version of
    	** ascii(), text(), char() or varchar() functions have to
    	** be a constant.
    	*/
    	if (yacc_cb->yypvt[-5].psl_tytype == ADI_ASCII_OP || yacc_cb->yypvt[-5].psl_tytype == ADI_TEXT_OP || yacc_cb->yypvt[-5].psl_tytype == ADI_CHAR_OP ||
    	    yacc_cb->yypvt[-5].psl_tytype == ADI_VARCH_OP)
        {
    	    DB_DT_ID  type;
    	    i4   err_code;


    	    type  = yacc_cb->yypvt[-1].psl_trtype->pst_sym.pst_dataval.db_datatype;

    	    if (yacc_cb->yypvt[-1].psl_trtype->pst_sym.pst_type != PST_CONST || abs(type) != DB_INT_TYPE)
    	    {
    	    	(VOID) psf_error(E_PS0C90_MUSTBE_CONST_INT, 0L,
    	    	    PSF_USERERR, &err_code, &psq_cb->psq_error, 0);
    	    	    return (E_DB_ERROR);
    	    }
    	}

	/* This is a necessary hack for setting default scale for the
	** decimal() conversion function.
	*/
	if (yacc_cb->yypvt[-5].psl_tytype == ADI_DEC_OP)
	{
	    DB_DT_ID		type;
	    i4			plen;
	    i4			len;
	    PTR			pdata;
	    i2			ps;
	    i4		err_code;


	    /* must be a const node */
	    if (yacc_cb->yypvt[-1].psl_trtype->pst_sym.pst_type != PST_CONST)
	    {
		(VOID) psf_error(E_PS0C81_MUST_BE_CONST, 0L, PSF_USERERR,
		    &err_code, &psq_cb->psq_error, 0);
		return (E_DB_ERROR);
	    }

	    type  = yacc_cb->yypvt[-1].psl_trtype->pst_sym.pst_dataval.db_datatype;
	    plen  = yacc_cb->yypvt[-1].psl_trtype->pst_sym.pst_dataval.db_length - ((type < 0) ? 1 : 0);
	    pdata = yacc_cb->yypvt[-1].psl_trtype->pst_sym.pst_dataval.db_data;

	    /* must be an i2 with valid precision */
	    if (    abs(type) != DB_INT_TYPE  ||  plen != 2
		||  *(i2 *)pdata < 1  ||  *(i2 *)pdata > DB_MAX_DECPREC)
	    {
		(VOID) psf_error(E_PS0C82_BAD_PRECISION, 0L, PSF_USERERR,
		    &err_code, &psq_cb->psq_error, 0);
		return (E_DB_ERROR);
	    }

	    /* check input type and get default scale based on it */
	    type = yacc_cb->yypvt[-3].psl_trtype->pst_sym.pst_dataval.db_datatype;
	    len  = yacc_cb->yypvt[-3].psl_trtype->pst_sym.pst_dataval.db_length - ((type < 0) ? 1 : 0);

	    switch (abs(type))
	    {
	      case DB_FLT_TYPE:
	      case DB_DEC_TYPE:
	      case DB_MNY_TYPE:
	        ps = DB_PS_ENCODE_MACRO(*(i2 *)pdata, AD_FP_TO_DEC_SCALE);
		break;

	      case DB_INT_TYPE:
		switch (len)
		{
		  case 1:
		    ps = DB_PS_ENCODE_MACRO(*(i2 *)pdata, AD_I1_TO_DEC_SCALE);
		    break;
		  case 2:
		    ps = DB_PS_ENCODE_MACRO(*(i2 *)pdata, AD_I2_TO_DEC_SCALE);
		    break;
		  case 4:
		    ps = DB_PS_ENCODE_MACRO(*(i2 *)pdata, AD_I4_TO_DEC_SCALE);
		    break;
		}
		break;

	      default:
	        {
		    ADI_DT_NAME		dt_name;

		    /* can't default precision/scale for this datatype */
		    status = adi_tyname((ADF_CB *) cb->pss_adfcb, 
					abs(type), &dt_name);

		    (VOID) psf_error(E_PS0C80_NO_DEFLT_ON_DT, 0L, PSF_USERERR,
			&err_code, &psq_cb->psq_error, 1,
			psf_trmwhite(sizeof(ADI_DT_NAME), (char *) &dt_name), 
			&dt_name);
		    return (E_DB_ERROR);
		}
	    }

	    /* store precision and scale back in const node */
	    *(i2 *)pdata = ps;
	}

	opnode.pst_opno = yacc_cb->yypvt[-5].psl_tytype;
	opnode.pst_opmeta = PST_NOMETA;
	opnode.pst_isescape = PST_DOESNT_APPLY;
	status = pst_node(cb, &cb->pss_ostream, yacc_cb->yypvt[-3].psl_trtype, yacc_cb->yypvt[-1].psl_trtype, PST_BOP,
	    (char *) &opnode, sizeof(opnode), DB_NODT, (i2) 0, (i4) 0,
	    (DB_ANYTYPE *) NULL, &newnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = newnode;
    } break;
	}
	return(0);
}

i4
psl12iftn(yacc_cb, yyrule, yystatusp, yyvarspp, cb, psq_cb)
YACC_CB		*yacc_cb;
i4		yyrule;
PSS_SESBLK	*cb;
DB_STATUS	*yystatusp;
YYVARS		**yyvarspp;
PSQ_CB *	psq_cb;
{
    YYVARS	*yyvarsp = *yyvarspp;
	switch(yyrule)
	{
case 518:
if (!((*yyvarspp)->bypass_actions))
/* # line 10080 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_OP_NODE	       opnode;
	PST_QNODE	       *newnode;
	DB_STATUS	       status;

	opnode.pst_opno = yacc_cb->yypvt[-2].psl_tytype;
	opnode.pst_opmeta = PST_NOMETA;
	opnode.pst_isescape = PST_DOESNT_APPLY;
	status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
	    (PST_QNODE *) NULL, PST_COP, (char *) &opnode, sizeof(opnode),
	    DB_NODT, (i2) 0, (i4) 0, (DB_ANYTYPE *) NULL, &newnode,
	    &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}


	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 519:
if (!((*yyvarspp)->bypass_actions))
/* # line 10101 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_OP_NODE	       opnode;
	PST_QNODE	       *newnode;
	DB_STATUS	       status;

	/* This is a necessary hack for getting precision and scale
	** combined into one const node for the decimal() conversion function.
	*/
	if (yacc_cb->yypvt[-7].psl_tytype == ADI_DEC_OP)
	{
	    DB_DT_ID		type;
	    i4			len;
	    PTR			pdata;
	    PTR			sdata;
	    i4		err_code;


	    /* must be a const node */
	    if (    yacc_cb->yypvt[-3].psl_trtype->pst_sym.pst_type != PST_CONST
		||  yacc_cb->yypvt[-1].psl_trtype->pst_sym.pst_type != PST_CONST
	       )
	    {
		(VOID) psf_error(E_PS0C81_MUST_BE_CONST, 0L, PSF_USERERR,
		    &err_code, &psq_cb->psq_error, 0);
		return (E_DB_ERROR);
	    }

	    type  = yacc_cb->yypvt[-3].psl_trtype->pst_sym.pst_dataval.db_datatype;
	    len  = yacc_cb->yypvt[-3].psl_trtype->pst_sym.pst_dataval.db_length - ((type < 0) ? 1 : 0);
	    pdata = yacc_cb->yypvt[-3].psl_trtype->pst_sym.pst_dataval.db_data;

	    /* must be an i2 with valid precision */
	    if (    abs(type) != DB_INT_TYPE  ||  len != 2
		||  *(i2 *)pdata < 1  ||  *(i2 *)pdata > DB_MAX_DECPREC)
	    {
		(VOID) psf_error(E_PS0C82_BAD_PRECISION, 0L, PSF_USERERR,
		    &err_code, &psq_cb->psq_error, 0);
		return (E_DB_ERROR);
	    }

	    type  = yacc_cb->yypvt[-1].psl_trtype->pst_sym.pst_dataval.db_datatype;
	    len   = yacc_cb->yypvt[-1].psl_trtype->pst_sym.pst_dataval.db_length - ((type < 0) ? 1 : 0);
	    sdata = yacc_cb->yypvt[-1].psl_trtype->pst_sym.pst_dataval.db_data;

	    /* must be an i2 with valid scale */
	    if (    abs(type) != DB_INT_TYPE  ||  len != 2
		||  *(i2 *)sdata < 0  ||  *(i2 *)sdata > *(i2 *)pdata)
	    {
		(VOID) psf_error(E_PS0C83_BAD_SCALE, 0L, PSF_USERERR,
		    &err_code, &psq_cb->psq_error, 1, sizeof(i2), pdata);
		return (E_DB_ERROR);
	    }

	    /* combine prec and scale into second node and orphan the
	    ** scale's CONST node
	    */
	    *(i2 *)pdata = DB_PS_ENCODE_MACRO(*(i2 *)pdata, *(i2 *)sdata);
	}

	opnode.pst_opno = yacc_cb->yypvt[-7].psl_tytype;
	opnode.pst_opmeta = PST_NOMETA;
	opnode.pst_isescape = PST_DOESNT_APPLY;
	status = pst_node(cb, &cb->pss_ostream, yacc_cb->yypvt[-5].psl_trtype, yacc_cb->yypvt[-3].psl_trtype, PST_BOP,
	    (char *) &opnode, sizeof(opnode), DB_NODT, (i2) 0, (i4) 0,
	    (DB_ANYTYPE *) NULL, &newnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 520:
if (!((*yyvarspp)->bypass_actions))
/* # line 10176 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	ADI_OP_NAME	       funcname;
	ADI_OP_ID	       opid;
	ADF_CB		       *adf_scb = (ADF_CB*) cb->pss_adfcb;
	DB_STATUS	       status;
	i4		       err_code;

	STmove(yacc_cb->yypvt[-0].psl_strtype, '\0', sizeof(ADI_OP_NAME), (char *) &funcname);
	status = adi_opid(adf_scb, &funcname, &opid);
	if (status != E_DB_OK)
	{
	    (VOID) psf_error(2906L, 0L, PSF_USERERR,
		&err_code, &psq_cb->psq_error, 2, sizeof(cb->pss_lineno),
		&cb->pss_lineno, (i4) STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);
	}

	yacc_cb->yyval.psl_tytype = opid;
    } break;

case 521:
if (!((*yyvarspp)->bypass_actions))
/* # line 10198 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_OP_NODE            operator;
	PST_RT_NODE	       aghead;
	PST_QNODE	       *agopnode;
	PST_QNODE	       *byheadnode;
	PST_QNODE	       *agheadnode;
	PST_QNODE	       *treenode;
	DB_STATUS	       status;
	ADI_OP_NAME	       funcname;
	ADI_OP_ID	       opid;
	ADF_CB		       *adf_scb = (ADF_CB*) cb->pss_adfcb;
	i4		       err_code;

	/* mark that aggregate function exists */
	yyvarsp->agg_func = 1;

	cb->pss_qualdepth--;

	/* Assign the result domain numbers */
	pst_windup(yacc_cb->yypvt[-3].psl_trtype);

	/* Create the aggregate operator node */

	/* Get the operator id for this function */
	STmove(yacc_cb->yypvt[-7].psl_strtype, '\0', sizeof(ADI_OP_NAME), (char *) &funcname);
	status = adi_opid(adf_scb, &funcname, &opid);
	if (status != E_DB_OK)
	{
	    (VOID) psf_error(2906L, 0L, PSF_USERERR,
		&err_code, &psq_cb->psq_error, 2, sizeof(cb->pss_lineno),
		&cb->pss_lineno, (i4) STlength(yacc_cb->yypvt[-7].psl_strtype), yacc_cb->yypvt[-7].psl_strtype);
	    return (E_DB_ERROR);
	}

	operator.pst_opno = opid;
	operator.pst_retnull = FALSE;
	operator.pst_opmeta = PST_NOMETA;
	operator.pst_isescape = PST_DOESNT_APPLY;
	if ((yacc_cb->yypvt[-7].psl_strtype)[STlength(yacc_cb->yypvt[-7].psl_strtype)-1] == 'u')
	    operator.pst_distinct = PST_DISTINCT;
	else if (!MEcmp((PTR) yacc_cb->yypvt[-7].psl_strtype,(PTR) "min", 3) ||
		 !MEcmp((PTR) yacc_cb->yypvt[-7].psl_strtype,(PTR) "max", 3)	||
		 !MEcmp((PTR) yacc_cb->yypvt[-7].psl_strtype,(PTR) "any", 3)
		)
	    operator.pst_distinct = PST_DONTCARE;
	else
	    operator.pst_distinct = PST_NDISTINCT;

	status = pst_node(cb, &cb->pss_ostream, yacc_cb->yypvt[-5].psl_trtype, (PST_QNODE *) NULL, PST_AOP,
	    (char *) &operator, sizeof(operator), DB_NODT, (i2) 0, (i4) 0,
	    (DB_ANYTYPE *) NULL, &agopnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	/* Create the byhead node */
	status = pst_node(cb, &cb->pss_ostream, yacc_cb->yypvt[-3].psl_trtype, agopnode, PST_BYHEAD,
	    (PTR) NULL, sizeof(PST_RSDM_NODE),
	    agopnode->pst_sym.pst_dataval.db_datatype,
	    agopnode->pst_sym.pst_dataval.db_prec,
	    agopnode->pst_sym.pst_dataval.db_length,
	    (DB_ANYTYPE *) NULL, &byheadnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	/* Create the aghead node */
	aghead.pst_union.pst_next  = 0;
	aghead.pst_dups   = PST_ALLDUPS;

	status = pst_node(cb, &cb->pss_ostream, byheadnode, yacc_cb->yypvt[-1].psl_trtype, PST_AGHEAD,
	    (PTR) &aghead, sizeof(PST_RT_NODE),
	    agopnode->pst_sym.pst_dataval.db_datatype,
	    agopnode->pst_sym.pst_dataval.db_prec,
	    agopnode->pst_sym.pst_dataval.db_length,
	    (DB_ANYTYPE *) NULL, &agheadnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	/* pst_project is set to FALSE in pst_node(); here we reset it to $6 */
	agheadnode->pst_sym.pst_value.pst_s_root.pst_project = yacc_cb->yypvt[-2].psl_inttype;

	/* Create the tree node */
	status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
	    (PST_QNODE *) NULL, PST_TREE, (PTR) NULL, 0, DB_NODT, (i2) 0,
	    (i4) 0, (DB_ANYTYPE *) NULL, &treenode, &psq_cb->psq_error,
	    (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	/* Prepend the tree node to the aggregate result domain list */
	yacc_cb->yyval.psl_trtype = pst_tlprpnd(treenode, agheadnode);
    } break;

case 522:
if (!((*yyvarspp)->bypass_actions))
/* # line 10298 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_OP_NODE            operator;
	PST_RT_NODE	       aghead;
	PST_QNODE	       *agopnode;
	PST_QNODE	       *byheadnode;
	PST_QNODE	       *agheadnode;
	PST_QNODE	       *treenode;
	PST_QNODE	       *nonode = (PST_QNODE *) NULL;
	DB_STATUS	       status;
	ADI_OP_NAME	       funcname;
	ADI_OP_ID	       opid;
	ADF_CB		       *adf_scb = (ADF_CB*) cb->pss_adfcb;
	i4		       err_code;

	/* mark that aggregate function exists */
	yyvarsp->agg_func = 1;
	/* Assign the result domain numbers */
	pst_windup(yacc_cb->yypvt[-1].psl_trtype);

	/* Create the aggregate operator node */

	/* Get the operator id for this function */
	STmove(yacc_cb->yypvt[-5].psl_strtype, '\0', sizeof(ADI_OP_NAME), (char *) &funcname);
	status = adi_opid(adf_scb, &funcname, &opid);
	if (status != E_DB_OK)
	{
	    (VOID) psf_error(2906L, 0L, PSF_USERERR,
		&err_code, &psq_cb->psq_error, 2, sizeof(cb->pss_lineno),
		&cb->pss_lineno, (i4) STlength(yacc_cb->yypvt[-5].psl_strtype), yacc_cb->yypvt[-5].psl_strtype);
	    return (E_DB_ERROR);
	}

	operator.pst_opno = opid;
	operator.pst_retnull = FALSE;
	operator.pst_opmeta = PST_NOMETA;
	operator.pst_isescape = PST_DOESNT_APPLY;
	if ((yacc_cb->yypvt[-5].psl_strtype)[STlength(yacc_cb->yypvt[-5].psl_strtype)-1] == 'u')
	    operator.pst_distinct = PST_DISTINCT;
	else if (!MEcmp((PTR) yacc_cb->yypvt[-5].psl_strtype,(PTR) "min", 3) ||
		!MEcmp((PTR) yacc_cb->yypvt[-5].psl_strtype,(PTR) "max", 3))
	    operator.pst_distinct = PST_DONTCARE;
	else
	    operator.pst_distinct = PST_NDISTINCT;
	status = pst_node(cb, &cb->pss_ostream, yacc_cb->yypvt[-3].psl_trtype, (PST_QNODE *) NULL, PST_AOP,
	    (char *) &operator, sizeof(operator), DB_NODT, (i2) 0, (i4) 0,
	    (DB_ANYTYPE *) NULL, &agopnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	/* Create the byhead node */
	status = pst_node(cb, &cb->pss_ostream, yacc_cb->yypvt[-1].psl_trtype, agopnode, PST_BYHEAD,
	    (PTR) NULL, sizeof(PST_RSDM_NODE),
	    agopnode->pst_sym.pst_dataval.db_datatype,
	    agopnode->pst_sym.pst_dataval.db_prec,
	    agopnode->pst_sym.pst_dataval.db_length,
	    (DB_ANYTYPE *) NULL, &byheadnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
	    (PST_QNODE *) NULL, PST_QLEND, (PTR) NULL, 0, DB_NODT, (i2) 0,
	    (i4) 0, (DB_ANYTYPE *) NULL, &nonode, &psq_cb->psq_error, (i4) 0);
	if (DB_FAILURE_MACRO(status))
	{
	    return (status);
	}

	/* Create the aghead node */
	/* don't project since there is no qualification in aggregate */
	aghead.pst_union.pst_next  = 0;
	aghead.pst_dups   = PST_ALLDUPS;

	status = pst_node(cb, &cb->pss_ostream, byheadnode, nonode, PST_AGHEAD,
	    (PTR) &aghead, sizeof(PST_RT_NODE),
	    agopnode->pst_sym.pst_dataval.db_datatype,
	    agopnode->pst_sym.pst_dataval.db_prec,
	    agopnode->pst_sym.pst_dataval.db_length,
	    (DB_ANYTYPE *) NULL, &agheadnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	/* Create the tree node */
	status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
	    (PST_QNODE *) NULL, PST_TREE, (PTR) NULL, 0, DB_NODT, (i2) 0,
	    (i4) 0, (DB_ANYTYPE *) NULL, &treenode, &psq_cb->psq_error,
	    (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	/* Prepend the tree node to the aggregate result domain list */
	yacc_cb->yyval.psl_trtype = pst_tlprpnd(treenode, agheadnode);
    } break;

case 523:
if (!((*yyvarspp)->bypass_actions))
/* # line 10399 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_OP_NODE            operator;
	PST_QNODE	       *agopnode;
	PST_QNODE	       *aghdnode;
	PST_RT_NODE	       aghead;
        DB_STATUS	       status;
	ADI_OP_NAME	       funcname;
	ADI_OP_ID	       opid;
	ADF_CB		       *adf_scb = (ADF_CB*) cb->pss_adfcb;
	i4		       err_code;

	/* Allocate the aggregate operator node */
	STmove(yacc_cb->yypvt[-4].psl_strtype, '\0', sizeof(ADI_OP_NAME), (char *) &funcname);
	status = adi_opid(adf_scb, &funcname, &opid);
	if (status != E_DB_OK)
	{
	    (VOID) psf_error(2906L, 0L, PSF_USERERR,
		&err_code, &psq_cb->psq_error, 2, sizeof(cb->pss_lineno),
		&cb->pss_lineno, (i4) STlength(yacc_cb->yypvt[-4].psl_strtype), yacc_cb->yypvt[-4].psl_strtype);
	    return (E_DB_ERROR);
	}

	operator.pst_opno = opid;
	operator.pst_retnull = FALSE;
	operator.pst_opmeta = PST_NOMETA;
	operator.pst_isescape = PST_DOESNT_APPLY;
	if ((yacc_cb->yypvt[-4].psl_strtype)[STlength(yacc_cb->yypvt[-4].psl_strtype)-1] == 'u')
	    operator.pst_distinct = PST_DISTINCT;
	else if (!MEcmp((PTR) yacc_cb->yypvt[-4].psl_strtype,(PTR) "min", 3) ||
		!MEcmp((PTR) yacc_cb->yypvt[-4].psl_strtype,(PTR) "max", 3))
	    operator.pst_distinct = PST_DONTCARE;
	else
	    operator.pst_distinct = PST_NDISTINCT;
	status = pst_node(cb, &cb->pss_ostream, yacc_cb->yypvt[-2].psl_trtype, (PST_QNODE *) NULL, PST_AOP,
	    (char *) &operator, sizeof(operator), DB_NODT, (i2) 0, (i4) 0,
	    (DB_ANYTYPE *) NULL, &agopnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	aghead.pst_union.pst_next  = 0;
	aghead.pst_dups   = PST_ALLDUPS;

	/* Allocate the aghead node */
	status = pst_node(cb, &cb->pss_ostream, agopnode, yacc_cb->yypvt[-1].psl_trtype, PST_AGHEAD, 
	    (PTR) &aghead,
	    sizeof(PST_RT_NODE), agopnode->pst_sym.pst_dataval.db_datatype,
	    agopnode->pst_sym.pst_dataval.db_prec,
	    agopnode->pst_sym.pst_dataval.db_length, (DB_ANYTYPE *) NULL,
	    &aghdnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = aghdnode;
    } break;

case 524:
if (!((*yyvarspp)->bypass_actions))
/* # line 10460 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_trtype = yacc_cb->yypvt[-0].psl_trtype;
    } break;

case 525:
if (!((*yyvarspp)->bypass_actions))
/* # line 10464 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_VAR_NODE	       var;
	DMT_ATT_ENTRY	       **attribute;
	PST_QNODE	       *varnode;
	DB_STATUS	       status;

	/* just use first attribute */
	attribute = &yacc_cb->yypvt[-2].psl_rngtype->pss_attdesc[1];
	/* Set up var node for copy into tree */
	var.pst_vno = yacc_cb->yypvt[-2].psl_rngtype->pss_rgno;
	var.pst_atno.db_att_id = (*attribute)->att_number;
	STRUCT_ASSIGN_MACRO((*attribute)->att_name, var.pst_atname);

	/* Create the PST_VAR_NODE */
	status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
	    (PST_QNODE *) NULL, PST_VAR, (char *) &var, sizeof(var),
	    (DB_DT_ID) (*attribute)->att_type,
	    (i2) (*attribute)->att_prec, (i4) (*attribute)->att_width,
	    (DB_ANYTYPE *) NULL, &varnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}
	yacc_cb->yyval.psl_trtype = varnode;
    } break;

case 526:
if (!((*yyvarspp)->bypass_actions))
/* # line 10492 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	cb->pss_qualdepth++;
	yacc_cb->yyval.psl_inttype = (cb->pss_ses_flag & PSS_PROJECT);
    } break;

case 527:
if (!((*yyvarspp)->bypass_actions))
/* # line 10497 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	cb->pss_qualdepth++;
	yacc_cb->yyval.psl_inttype = FALSE;
    } break;

case 529:
if (!((*yyvarspp)->bypass_actions))
/* # line 10505 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_trtype = pst_tlprpnd(yacc_cb->yypvt[-2].psl_trtype, yacc_cb->yypvt[-0].psl_trtype);
    } break;

case 530:
if (!((*yyvarspp)->bypass_actions))
/* # line 10511 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_QNODE              *newnode;
	DB_STATUS	       status;
	PST_RSDM_NODE	       rsdom;

	rsdom.pst_rsno = 0;
	rsdom.pst_ntargno = rsdom.pst_rsno;
	rsdom.pst_ttargtype = PST_USER;
	rsdom.pst_rsupdt = FALSE;
	rsdom.pst_rsflags = PST_RS_PRINT;
	rsdom.pst_dmuflags = 0;
	status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL, yacc_cb->yypvt[-0].psl_trtype,
	    PST_RESDOM, (PTR) &rsdom, sizeof(PST_RSDM_NODE),
	    yacc_cb->yypvt[-0].psl_trtype->pst_sym.pst_dataval.db_datatype, 
	    yacc_cb->yypvt[-0].psl_trtype->pst_sym.pst_dataval.db_prec,
	    yacc_cb->yypvt[-0].psl_trtype->pst_sym.pst_dataval.db_length, (DB_ANYTYPE *) NULL,
	    &newnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 531:
if (!((*yyvarspp)->bypass_actions))
/* # line 10536 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_VAR_NODE	       var;
	DMT_ATT_ENTRY	       **attribute;
	i4		       i;
	PST_QNODE	       *varnode;
	PST_QNODE	       *rsdmnode = (PST_QNODE *) NULL;
	DB_STATUS	       status;
	PST_RSDM_NODE	       rsdom;

	rsdom.pst_rsno = 0;
	rsdom.pst_ntargno = rsdom.pst_rsno;
	rsdom.pst_ttargtype = PST_USER;
	rsdom.pst_rsupdt = FALSE;
	rsdom.pst_rsflags = PST_RS_PRINT;
	rsdom.pst_dmuflags = 0;

	/* Start at first attribute */
	for (i = 0, attribute = yacc_cb->yypvt[-2].psl_rngtype->pss_attdesc + 1;
	     i < yacc_cb->yypvt[-2].psl_rngtype->pss_tabdesc->tbl_attr_count;
	     i++, attribute++)
	{
	    if ((*attribute)->att_flags & DMT_F_HIDDEN)
		continue;

	    /* Set up var node for copy into tree */
	    var.pst_vno = yacc_cb->yypvt[-2].psl_rngtype->pss_rgno;
	    var.pst_atno.db_att_id = (*attribute)->att_number;
	    STRUCT_ASSIGN_MACRO((*attribute)->att_name, var.pst_atname);

	    /* Create the PST_VAR_NODE */
	    status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
		(PST_QNODE *) NULL, PST_VAR, (char *) &var, sizeof(var),
		(DB_DT_ID) (*attribute)->att_type,
		(i2) (*attribute)->att_prec, (i4) (*attribute)->att_width,
		(DB_ANYTYPE *) NULL, &varnode, &psq_cb->psq_error, (i4) 0);
	    if (status != E_DB_OK)
	    {
		return (status);
	    }

	    /* Create the corresponding result domain node */
	    status = pst_node(cb, &cb->pss_ostream, rsdmnode, varnode,
		PST_RESDOM, (PTR) &rsdom, sizeof(PST_RSDM_NODE),
		(DB_DT_ID) (*attribute)->att_type,
		(i2) (*attribute)->att_prec, (i4) (*attribute)->att_width,
		(DB_ANYTYPE *) NULL, &rsdmnode,	&psq_cb->psq_error, (i4) 0);
	    if (status != E_DB_OK)
	    {
		return (status);
	    }
	}

	yacc_cb->yyval.psl_trtype = rsdmnode;
    } break;

case 532:
if (!((*yyvarspp)->bypass_actions))
/* # line 10593 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DMT_ATT_ENTRY          *attribute;
	PST_VAR_NODE	       varnode;
	PST_QNODE	       *newnode;
	i4		       err_code;
	DB_STATUS	       status;

	/* Normalize the attribute name */
	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(DB_ATT_NAME),
	    (char *) &varnode.pst_atname);

	/* Look up the attribute */
	attribute = pst_coldesc(yacc_cb->yypvt[-2].psl_rngtype, &varnode.pst_atname);

	/* Check for attribute not found */
	if (attribute == (DMT_ATT_ENTRY *) NULL)
	{
	    (VOID) psf_error(2100L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 4, (i4) sizeof(cb->pss_lineno),
		&cb->pss_lineno, (i4) sizeof(DB_TAB_NAME),
		&(yacc_cb->yypvt[-2].psl_rngtype->pss_tabname), (i4) sizeof(DB_OWN_NAME),
		&(yacc_cb->yypvt[-2].psl_rngtype->pss_ownname), (i4) STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
	    return (E_DB_ERROR);
	}
		
	/* Set up var node for copy into tree */
	varnode.pst_vno = yacc_cb->yypvt[-2].psl_rngtype->pss_rgno;
	varnode.pst_atno.db_att_id = attribute->att_number;

	/* Create the node */
	status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
	    (PST_QNODE *) NULL, PST_VAR, (char *) &varnode, sizeof(varnode),
	    (DB_DT_ID) attribute->att_type,
	    (i2) attribute->att_prec, (i4) attribute->att_width,
	    (DB_ANYTYPE *) NULL, &newnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 533:
if (!((*yyvarspp)->bypass_actions))
/* # line 10636 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DMT_ATT_ENTRY          *attribute;
	PST_VAR_NODE	       varnode;
	PST_QNODE	       *newnode;
	DB_STATUS	       status;

	/* (schka24) I don't think there's any way to generate a COLNO
	** token???  so I don't think you can get here...
	*/

	/* Get the attribute */
	attribute = yacc_cb->yypvt[-2].psl_rngtype->pss_attdesc[yacc_cb->yypvt[-0].psl_inttype];

	/* Normalize the attribute name */
	STmove(attribute->att_name.db_att_name, ' ', sizeof(DB_ATT_NAME),
	    (char *) &varnode.pst_atname);
		
	/* Set up var node for copy into tree */
	varnode.pst_vno = yacc_cb->yypvt[-2].psl_rngtype->pss_rgno;
	varnode.pst_atno.db_att_id = attribute->att_number;

	/* Create the node */
	status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
	    (PST_QNODE *) NULL, PST_VAR, (char *) &varnode, sizeof(varnode),
	    (DB_DT_ID) attribute->att_type,
	    (i2) attribute->att_prec, (i4) attribute->att_width,
	    (DB_ANYTYPE *) NULL, &newnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 534:
if (!((*yyvarspp)->bypass_actions))
/* # line 10673 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;
	PSS_RNGTAB	       *rngtab;
	DB_TAB_NAME	       tabname;

	/* Look for an already-existing range var of this name */
	status = pst_rglook(cb, &cb->pss_usrrange, -1, yacc_cb->yypvt[-0].psl_strtype, TRUE, 
	    &rngtab, psq_cb->psq_mode, &psq_cb->psq_error);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	/* If not found, try entering it */
	if (rngtab == (PSS_RNGTAB *) NULL)
	{
	    i4	    rngvar_info;
	    
	    /* normalize the table name */
	    STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(DB_TAB_NAME), (char *) &tabname);
	    status = psl_rngent(&cb->pss_usrrange, -1, yacc_cb->yypvt[-0].psl_strtype, &tabname, cb,
		FALSE, &rngtab, psq_cb->psq_mode, &psq_cb->psq_error,
		&rngvar_info);
	    if (status != E_DB_OK)
	    {
		return (status);
	    }
	}
	yacc_cb->yyval.psl_rngtype = rngtab;
    } break;

case 535:
if (!((*yyvarspp)->bypass_actions))
/* # line 10704 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PSS_RNGTAB	       *rngvar = cb->pss_usrrange.pss_rngtab;
	i4			i;
	i4			err_code;

	/* Scan the range table looking for the range variable */
	for (i = 0; i < PST_NUMVARS; i++)
	{
	    if (rngvar->pss_used && rngvar->pss_rgno == *yacc_cb->yypvt[-0].psl_i4type)
	    {
		yacc_cb->yyval.psl_rngtype = rngvar;
		break;
	    }

	    rngvar++;
	}

	/* Internal error if we didn't find it */
	if (i >= PST_NUMVARS)
	{
	    (VOID) psf_error(E_PS0908_RANGE_VAR_MISSING, 0L, PSF_INTERR,
		&err_code, &psq_cb->psq_error, 2, sizeof(cb->pss_lineno),
		&cb->pss_lineno, sizeof(*yacc_cb->yypvt[-0].psl_i4type), yacc_cb->yypvt[-0].psl_i4type);
	    return (E_DB_SEVERE);
	}
    } break;

case 536:
if (!((*yyvarspp)->bypass_actions))
/* # line 10733 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;
	PST_CNST_NODE	       cconst;
	PST_QNODE	       *newnode;

	/* Non-parameter i2 constant node */
	cconst.pst_tparmtype = PST_USER;
	cconst.pst_parm_no = 0;
	cconst.pst_pmspec  = PST_PMNOTUSED;
	cconst.pst_cqlang = DB_QUEL;
	cconst.pst_origtxt = (char *) NULL;
	status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
	    (PST_QNODE *) NULL, PST_CONST, (char *) &cconst,
	    sizeof(cconst), DB_INT_TYPE, (i2) 0, (i4) 2, (DB_ANYTYPE *) yacc_cb->yypvt[-0].psl_i2type,
	    &newnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 537:
if (!((*yyvarspp)->bypass_actions))
/* # line 10756 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;
	PST_CNST_NODE	       cconst;
	PST_QNODE	       *newnode;

	/* Non-parameter i4 constant node */
	cconst.pst_tparmtype = PST_USER;
	cconst.pst_parm_no = 0;
	cconst.pst_pmspec  = PST_PMNOTUSED;
	cconst.pst_cqlang = DB_QUEL;
	cconst.pst_origtxt = (char *) NULL;
	status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
	    (PST_QNODE *) NULL, PST_CONST, (char *) &cconst,
	    sizeof(cconst), DB_INT_TYPE, (i2) 0, (i4) 4, (DB_ANYTYPE *) yacc_cb->yypvt[-0].psl_i4type,
	    &newnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 538:
if (!((*yyvarspp)->bypass_actions))
/* # line 10779 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;
	PST_CNST_NODE	       cconst;
	PST_QNODE	       *newnode;
	i2			prec;

	/* Non-parameter decimal constant node */
	I2ASSIGN_MACRO(*yacc_cb->yypvt[-0].psl_dectype, prec);
	cconst.pst_tparmtype = PST_USER;
	cconst.pst_parm_no = 0;
	cconst.pst_pmspec  = PST_PMNOTUSED;
	cconst.pst_cqlang = DB_QUEL;
	cconst.pst_origtxt = (char *) NULL;
	status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
	    (PST_QNODE *) NULL, PST_CONST, (char *) &cconst,
	    sizeof(cconst), DB_DEC_TYPE, (i2) prec,
	    (i4) DB_PREC_TO_LEN_MACRO(DB_P_DECODE_MACRO(prec)),
	    (DB_ANYTYPE *) ((u_char *)yacc_cb->yypvt[-0].psl_dectype + sizeof(i2)),
	    &newnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 539:
if (!((*yyvarspp)->bypass_actions))
/* # line 10806 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;
	PST_CNST_NODE	       cconst;
	PST_QNODE	       *newnode;

	/* Non-parameter f4 constant node */
	cconst.pst_tparmtype = PST_USER;
	cconst.pst_parm_no = 0;
	cconst.pst_pmspec  = PST_PMNOTUSED;
	cconst.pst_cqlang = DB_QUEL;
	cconst.pst_origtxt = (char *) NULL;
	status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
	    (PST_QNODE *) NULL, PST_CONST, (char *) &cconst,
	    sizeof(cconst), DB_FLT_TYPE, (i2) 0, (i4) 4, (DB_ANYTYPE *) yacc_cb->yypvt[-0].psl_f4type,
	    &newnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 540:
if (!((*yyvarspp)->bypass_actions))
/* # line 10829 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;
	PST_CNST_NODE	       cconst;
	PST_QNODE	       *newnode;

	/* Non-parameter f8 constant node */
	cconst.pst_tparmtype = PST_USER;
	cconst.pst_parm_no = 0;
	cconst.pst_pmspec  = PST_PMNOTUSED;
	cconst.pst_cqlang = DB_QUEL;
	cconst.pst_origtxt = (char *) NULL;
	status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
	    (PST_QNODE *) NULL, PST_CONST, (char *) &cconst,
	    sizeof(cconst), DB_FLT_TYPE, (i2) 0, (i4) 8, (DB_ANYTYPE *) yacc_cb->yypvt[-0].psl_f8type,
	    &newnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 541:
if (!((*yyvarspp)->bypass_actions))
/* # line 10852 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_CNST_NODE	cconst;
	PST_QNODE	*newnode;
	i4		reqs = 0;
	i4		lines = 0;
	bool		pmchars;
	i4		err_code;
	DB_STATUS	status;
	DB_DATA_VALUE	dv;
	DB_TEXT_STRING	*str;
	ADF_CB		*adf_scb = (ADF_CB*) cb->pss_adfcb;

	str = yacc_cb->yypvt[-0].psl_textype;

	dv.db_data = (PTR) str;
	dv.db_length = str->db_t_count + DB_CNTSIZE;
	dv.db_datatype = DB_VCH_TYPE;
	dv.db_prec = 0;

	reqs |= (ADI_DO_BACKSLASH | ADI_DO_MOD_LEN);
	if (cb->pss_qualdepth)
	{
	    reqs |= ADI_DO_PM;
	}

	status = adi_pm_encode(adf_scb, reqs, &dv, &lines, &pmchars);
	cb->pss_lineno += lines;

	if (DB_FAILURE_MACRO(status))
	{
	    switch (adf_scb->adf_errcb.ad_errcode)
	    {
		case E_AD1015_BAD_RANGE:
		case E_AD3050_BAD_CHAR_IN_STRING:
		    {
			(VOID) psf_adf_error(&adf_scb->adf_errcb, 
					     &psq_cb->psq_error, cb);
			break;
		    }
		default:
		    {
			(VOID) psf_error(E_PS0377_ADI_PM_ERR,
			    adf_scb->adf_errcb.ad_errcode, PSF_INTERR,
			    &err_code, &psq_cb->psq_error, 0);
		    }
	    }
	    return (status);
	}

	/* Non-parameter text constant node */
	cconst.pst_tparmtype = PST_USER;
	cconst.pst_parm_no = 0;
	cconst.pst_pmspec  = PST_PMNOTUSED;
	cconst.pst_cqlang = DB_QUEL;
	cconst.pst_origtxt = (char *) NULL;

	if (pmchars == TRUE)
	{
	    cconst.pst_pmspec  = PST_PMUSED;
	}

	status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
	    (PST_QNODE *) NULL, PST_CONST, (char *) &cconst,
	    sizeof(cconst), DB_VCH_TYPE, (i2) 0,
	    (i4) (yacc_cb->yypvt[-0].psl_textype->db_t_count + DB_CNTSIZE), (DB_ANYTYPE *) yacc_cb->yypvt[-0].psl_textype, &newnode,
	    &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 542:
if (!((*yyvarspp)->bypass_actions))
/* # line 10926 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_STATUS              status;
	PST_CNST_NODE	       cconst;
	PST_QNODE	       *newnode;

	/* hex constant node */
	cconst.pst_tparmtype = PST_USER;
	cconst.pst_parm_no = 0;
	cconst.pst_pmspec  = PST_PMNOTUSED;
	cconst.pst_cqlang = DB_QUEL;
	cconst.pst_origtxt = (char *) NULL;
	status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
	    (PST_QNODE *) NULL, PST_CONST, (char *) &cconst,
	    sizeof(cconst), DB_VCH_TYPE, (i2) 0,
	    (i4) (yacc_cb->yypvt[-0].psl_textype->db_t_count + DB_CNTSIZE),
	    (DB_ANYTYPE *) yacc_cb->yypvt[-0].psl_textype, &newnode, &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 543:
if (!((*yyvarspp)->bypass_actions))
/* # line 10950 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_CNST_NODE	cconst;
	PST_QNODE	*newnode;
	DB_DATA_VALUE	*dbval;
	i4		reqs = 0;
	i4		lines = 0;
	bool		pmchars = FALSE;
	i4		err_code;
	DB_STATUS	status;
	ADF_CB		*adf_scb = (ADF_CB*) cb->pss_adfcb;

	dbval = (DB_DATA_VALUE*) yacc_cb->yypvt[-0].psl_dbval;

	/*
	** In case of data coming from host lang. variable, no
	** modifications must be made, except for PM in qualifications.
	*/
	if (cb->pss_qualdepth)
	{
	    reqs |= ADI_DO_PM;
	    status = adi_pm_encode(adf_scb, reqs, dbval, &lines, &pmchars);
	    cb->pss_lineno += lines;

	    if (DB_FAILURE_MACRO(status))
	    {
		switch (adf_scb->adf_errcb.ad_errcode)
		{
		    case E_AD1015_BAD_RANGE:
		    case E_AD3050_BAD_CHAR_IN_STRING:
			{
			    (VOID) psf_adf_error(&adf_scb->adf_errcb,
						 &psq_cb->psq_error, cb);
			    break;
			}
		    default:
			{
			    (VOID) psf_error(E_PS0377_ADI_PM_ERR,
				adf_scb->adf_errcb.ad_errcode, PSF_INTERR,
				&err_code, &psq_cb->psq_error, 0);
			}
		}
		return (status);
	    }
	}

	/* Non-parameter arbitrary constant node */
	cconst.pst_tparmtype = PST_USER;
	cconst.pst_parm_no = 0;
	cconst.pst_pmspec  = (pmchars) ? PST_PMUSED : PST_PMNOTUSED;
	cconst.pst_cqlang = DB_QUEL;
	cconst.pst_origtxt = (char *) NULL;

	status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
	    (PST_QNODE *) NULL, PST_CONST, (char *) &cconst,
	    sizeof(cconst), dbval->db_datatype, dbval->db_prec,
	    (i4) (dbval->db_length), (DB_ANYTYPE *) dbval->db_data, &newnode,
	    &psq_cb->psq_error, (i4) 0);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	/*
	** if parsing a repeat query, remember that it contained a non-repeat
	** parameter - this query will not be shareable
	*/
	if (cb->pss_defqry == PSQ_DEFQRY)
	{
	    yyvarsp->qry_mask |= PSS_NONREPEAT_PARAM;

	    /*
	    ** As a part of the algorithm for verifying shareability of QUEL
	    ** repeat queries, we save query text; once we know that the query
	    ** will not be shareable, we might as well stop saving its text
	    */
	    cb->pss_stmt_flags &= ~PSS_QUEL_RPTQRY_TEXT;
	}

	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 544:
if (!((*yyvarspp)->bypass_actions))
/* # line 11031 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	PST_OP_NODE	       opnode;
	PST_QNODE	       *newnode = (PST_QNODE *) NULL;
	ADI_OP_NAME	       funcname;
	ADF_CB		       *adf_scb = (ADF_CB*) cb->pss_adfcb;
	DB_STATUS	       status;
        i4		       err_code;

	/*
	** If we are doing a "replace cursor", look for a column in the
	** table/view over which the cursor was defined (as opposed to the
	** column in the FOR UPDATE list, as it used to do)
	*/
	if (psq_cb->psq_mode == PSQ_REPCURS)
	{
	    DB_ATT_NAME		colname;
	    DMT_ATT_ENTRY	*att_entry;

	    STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(DB_ATT_NAME), (char *) &colname);
	    att_entry = pst_coldesc(cb->pss_resrng, &colname);

	    if (att_entry != (DMT_ATT_ENTRY *) NULL)
	    {
		PST_VAR_NODE	    varnode;

		varnode.pst_vno	= cb->pss_resrng->pss_rgno;
		varnode.pst_atno.db_att_id = att_entry->att_number;

		status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
		    (PST_QNODE *) NULL, PST_VAR, (char *) &varnode,
		    sizeof(varnode), (DB_DT_ID) att_entry->att_type,
		    (i2) att_entry->att_prec, (i4) att_entry->att_width,
		    (DB_ANYTYPE *) NULL, &newnode, &psq_cb->psq_error, (i4) 0);
		if (status != E_DB_OK)
		{
		    return (status);
		}
	    }
	}

	/* check to see if NULL */
	if (!cb->pss_qualdepth && MEcmp((PTR) "null", (PTR) yacc_cb->yypvt[-0].psl_strtype, 4) == 0)
	{
	    DB_STATUS               status;
	    PST_CNST_NODE	    cconst;
	    DB_DATA_VALUE	    db_data;
	    char		    buf[5];

	    db_data.db_datatype = -DB_LTXT_TYPE;
	    db_data.db_prec	= 0;
	    db_data.db_length	= DB_CNTSIZE + 1;
	    db_data.db_data	= (PTR) buf;

	    status = adc_getempty ((ADF_CB *) cb->pss_adfcb, &db_data);
	    if (status != E_DB_OK)
	    {
		return (status);
	    }
	    /* NULL constant node */
	    cconst.pst_tparmtype = PST_USER;
	    cconst.pst_parm_no = 0;
	    cconst.pst_pmspec  = PST_PMNOTUSED;
	    cconst.pst_cqlang = DB_QUEL;
	    cconst.pst_origtxt = (char *) NULL;
	    status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
		(PST_QNODE *) NULL, PST_CONST, (char *) &cconst,
		sizeof(cconst), -DB_LTXT_TYPE, (i2) 0, (i4) (1 + DB_CNTSIZE),
		(DB_ANYTYPE *) db_data.db_data, &newnode, &psq_cb->psq_error,
		(i4) 0);
	    if (status != E_DB_OK)
	    {
		return (status);
	    }
	}
	/* If it's not a cursor column, look for a non-ary function */
	else if (newnode == (PST_QNODE *) NULL)
	{
	    STmove(yacc_cb->yypvt[-0].psl_strtype, '\0', sizeof(ADI_OP_NAME), (char *) &funcname);
	    status = adi_opid(adf_scb, &funcname, &opnode.pst_opno);
	    if (status != E_DB_OK)
	    {
		(VOID) psf_error(2906L, 0L, PSF_USERERR,
		    &err_code, &psq_cb->psq_error, 2,
		    (i4) sizeof(cb->pss_lineno), &cb->pss_lineno,
		    (i4) STlength(yacc_cb->yypvt[-0].psl_strtype), yacc_cb->yypvt[-0].psl_strtype);
		return (E_DB_ERROR);
	    }

	    opnode.pst_opmeta = PST_NOMETA;
	    opnode.pst_isescape = PST_DOESNT_APPLY;
	    status = pst_node(cb, &cb->pss_ostream, (PST_QNODE *) NULL,
	        (PST_QNODE *) NULL, PST_COP, (char *) &opnode,
		sizeof(opnode), DB_NODT, (i2) 0, (i4) 0, (DB_ANYTYPE *) NULL,
		&newnode, &psq_cb->psq_error, (i4) 0);
	    if (status != E_DB_OK)
	    {
	        return (status);
	    }

	}

	yacc_cb->yyval.psl_trtype = newnode;
    } break;

case 547:
if (!((*yyvarspp)->bypass_actions))
/* # line 11139 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;
	PST_QNODE	       *parmnode;
	DB_STATUS	       status;

	/* parameters not allowed in non-repeat queries */
	if (cb->pss_defqry == 0)
	{
	    (VOID) psf_error(2110L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, (i4) sizeof(cb->pss_lineno),
		&cb->pss_lineno);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}

	/* Allocate a constant node with a parameter number */
	status = pst_adparm(cb, psq_cb, &cb->pss_ostream, *yacc_cb->yypvt[-2].psl_i2type + 1, yacc_cb->yypvt[-0].psl_strtype,
	    &parmnode, &cb->pss_highparm);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	yacc_cb->yyval.psl_trtype = parmnode;
    } break;

case 548:
if (!((*yyvarspp)->bypass_actions))
/* # line 11164 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4                err_code;
	PST_QNODE	       *parmnode;
	DB_STATUS	       status;

	/* parameters not allowed in non-repeat queries */
	if (cb->pss_defqry == 0)
	{
	    (VOID) psf_error(2110L, 0L, PSF_USERERR, &err_code,
		&psq_cb->psq_error, 1, (i4) sizeof(cb->pss_lineno),
		&cb->pss_lineno);
	    return (E_DB_ERROR);    /* non-zero return means error */
	}

	/* Allocate a constant node with a parameter number */
	status = pst_2adparm(cb, psq_cb, &cb->pss_ostream, *yacc_cb->yypvt[-2].psl_i2type, yacc_cb->yypvt[-0].psl_dbval,
	    &parmnode, &cb->pss_highparm);
	if (status != E_DB_OK)
	{
	    return (status);
	}

	/*
	** if this is a repeat query, remember if the current parameter contains
	** pattern-matching characters; if so, the query will NOT be shareable
	*/
	if (cb->pss_defqry == PSQ_DEFQRY &&
	    parmnode->pst_sym.pst_value.pst_s_cnst.pst_pmspec == PST_PMUSED)
	{
	    yyvarsp->qry_mask |= PSS_PM_IN_REPEAT_PARAM;

	    /*
	    ** As a part of the algorithm for verifying shareability of QUEL
	    ** repeat queries, we save query text; once we know that the query
	    ** will not be shareable, we might as well stop saving its text
	    */
	    cb->pss_stmt_flags &= ~PSS_QUEL_RPTQRY_TEXT;
	}

	yacc_cb->yyval.psl_trtype = parmnode;
    } break;

case 549:
if (!((*yyvarspp)->bypass_actions))
/* # line 11208 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	/*
	** The scanner is unable to distinguish between unary and binary +,
	** and unary and binary -.  Therefore, translate the binary operator
	** ids to unary ones here.
	*/
	if (yacc_cb->yypvt[-0].psl_tytype == ADI_ADD_OP)
	    yacc_cb->yyval.psl_tytype = ADI_PLUS_OP;
	else if (yacc_cb->yypvt[-0].psl_tytype == ADI_SUB_OP)
	    yacc_cb->yyval.psl_tytype = ADI_MINUS_OP;
    } break;

case 550:
if (!((*yyvarspp)->bypass_actions))
/* # line 11222 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yyvarsp->with_location = 1;

 	/* Remember location and table name */

	STmove(yacc_cb->yypvt[-2].psl_strtype, ' ', sizeof(DB_LOC_NAME),
	    (char *) cb->pss_restab.pst_resloc.data_address);
	cb->pss_restab.pst_resloc.data_in_size = sizeof(DB_LOC_NAME);

 	/* Remember table name */
	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(DB_TAB_NAME),
	    (char *) &cb->pss_restab.pst_resname);

    } break;
	}
	return(0);
}

i4
psl13iftn(yacc_cb, yyrule, yystatusp, yyvarspp, cb, psq_cb)
YACC_CB		*yacc_cb;
i4		yyrule;
PSS_SESBLK	*cb;
DB_STATUS	*yystatusp;
YYVARS		**yyvarspp;
PSQ_CB *	psq_cb;
{
    YYVARS	*yyvarsp = *yyvarspp;
	switch(yyrule)
	{
case 551:
if (!((*yyvarspp)->bypass_actions))
/* # line 11237 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
 	/* Remember table name */
	STmove(yacc_cb->yypvt[-0].psl_strtype, ' ', sizeof(DB_TAB_NAME),
	    (char *) &cb->pss_restab.pst_resname);
    } break;

case 552:
if (!((*yyvarspp)->bypass_actions))
/* # line 11245 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_inttype = yacc_cb->yypvt[-0].psl_inttype;
    } break;

case 553:
if (!((*yyvarspp)->bypass_actions))
/* # line 11249 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4		    value;
	ADF_CB		    adf_cb;
	ADF_CB		    *adf_scb = (ADF_CB*) cb->pss_adfcb;
	DB_DATA_VALUE	    tdv;
	ADI_DT_NAME	    dt_fname;
	ADI_DT_NAME	    dt_tname;
	i4		    err_code;
	
	/* Copy the session ADF block into local one */
	STRUCT_ASSIGN_MACRO(*adf_scb, adf_cb);

	adf_cb.adf_errcb.ad_ebuflen = 0;
	adf_cb.adf_errcb.ad_errmsgp = 0;
	tdv.db_length		    = 4;
	tdv.db_prec		    = 0;
	tdv.db_datatype		    = DB_INT_TYPE;
	tdv.db_data		    = (PTR) &value;
	    
	if (adc_cvinto(&adf_cb, yacc_cb->yypvt[-0].psl_dbval, &tdv) != E_DB_OK)
	{
	    (VOID) adi_tyname(&adf_cb, yacc_cb->yypvt[-0].psl_dbval->db_datatype, &dt_fname);
	    (VOID) adi_tyname(&adf_cb, (DB_DT_ID) DB_INT_TYPE, &dt_tname);
	    (VOID) psf_error(2911L, 0L, PSF_USERERR,
		&err_code, &psq_cb->psq_error, 3, sizeof (cb->pss_lineno),
		&cb->pss_lineno, sizeof (dt_fname), &dt_fname, 
		sizeof (dt_tname), &dt_tname);
	    return (E_DB_ERROR);
	}
	yacc_cb->yyval.psl_inttype = value;
    } break;

case 554:
if (!((*yyvarspp)->bypass_actions))
/* # line 11283 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_inttype = *yacc_cb->yypvt[-0].psl_i4type;
    } break;

case 555:
if (!((*yyvarspp)->bypass_actions))
/* # line 11287 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_inttype = *yacc_cb->yypvt[-0].psl_i2type;
    } break;

case 556:
if (!((*yyvarspp)->bypass_actions))
/* # line 11293 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	i4		reqs = 0;
	i4		lines = 0;
	bool		pmchars;
	i4		err_code;
	DB_STATUS	status;
	DB_DATA_VALUE	dv;
	DB_TEXT_STRING	*str;
	ADF_CB		*adf_scb = (ADF_CB*) cb->pss_adfcb;

	str = yacc_cb->yypvt[-0].psl_textype;

	dv.db_data = (PTR) str;
	dv.db_length = str->db_t_count + DB_CNTSIZE;
	dv.db_datatype = DB_VCH_TYPE;
	dv.db_prec = 0;

	reqs |= (ADI_DO_BACKSLASH | ADI_DO_MOD_LEN);
	if (cb->pss_qualdepth)
	{
	    reqs |= ADI_DO_PM;
	}

	status = adi_pm_encode(adf_scb, reqs, &dv, &lines, &pmchars);
	cb->pss_lineno += lines;

	if (DB_FAILURE_MACRO(status))
	{
	    switch (adf_scb->adf_errcb.ad_errcode)
	    {
		case E_AD1015_BAD_RANGE:
		case E_AD3050_BAD_CHAR_IN_STRING:
		    {
			(VOID) psf_adf_error(&adf_scb->adf_errcb,
					     &psq_cb->psq_error, cb);
			break;
		    }
		default:
		    {
			(VOID) psf_error(E_PS0377_ADI_PM_ERR,
			    adf_scb->adf_errcb.ad_errcode, PSF_INTERR,
			    &err_code, &psq_cb->psq_error, 0);
		    }
	    }
	    return (status);
	}

	yacc_cb->yyval.psl_textype = yacc_cb->yypvt[-0].psl_textype;
    } break;

case 557:
if (!((*yyvarspp)->bypass_actions))
/* # line 11345 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yacc_cb->yyval.psl_strtype = sconvert(yacc_cb->yypvt[-0].psl_textype);
    } break;

case 558:
if (!((*yyvarspp)->bypass_actions))
/* # line 11349 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	DB_TEXT_STRING		*value;
	DB_STATUS		status;
	DB_DT_ID		totype;
	    
	/*
	** before we call qdata_cvt() to convert input value into longtext, make
	** sure that the input value is of "character type", i.e. c, char, text,
	** varchar, or longtext
	*/

	status = psl_must_be_string(cb, yacc_cb->yypvt[-0].psl_dbval, &psq_cb->psq_error);
	if (DB_FAILURE_MACRO(status))
	    return(status);

	/*
	** if the input datatype was nullable, so must be the output datatype
	*/

	totype = (yacc_cb->yypvt[-0].psl_dbval->db_datatype < 0) ? -DB_LTXT_TYPE : DB_LTXT_TYPE;

	if (qdata_cvt(cb, psq_cb, yacc_cb->yypvt[-0].psl_dbval, totype, (PTR *) &value) != E_DB_OK)
	{
	    return (E_DB_ERROR);
	}

	yacc_cb->yyval.psl_strtype = sconvert(value);
    } break;

case 559:
if (!((*yyvarspp)->bypass_actions))
/* # line 11384 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yyvarsp->id_type = PSS_ID_REG;
	yacc_cb->yyval.psl_strtype = yacc_cb->yypvt[-0].psl_strtype;
    } break;

case 560:
if (!((*yyvarspp)->bypass_actions))
/* # line 11389 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	yyvarsp->id_type = PSS_ID_DELIM;
	yacc_cb->yyval.psl_strtype = yacc_cb->yypvt[-0].psl_strtype;
    } break;

case 561:
if (!((*yyvarspp)->bypass_actions))
/* # line 11396 "/home/ingres/SANDBOX/Ingres_SVN/server/src/back/psf/psl/pslgram.yi" */
{
	char	*letter;
	/*
	** Translate nonkeywords to lower case
	*/
	for (letter =  yacc_cb->yypvt[-0].psl_strtype; *letter != EOS; CMnext(letter))
	    CMtolower(letter, letter);

	yacc_cb->yyval.psl_strtype = yacc_cb->yypvt[-0].psl_strtype;
    } break;
}
	return(0);
}

