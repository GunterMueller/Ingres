/*
** Copyright (c) 2004 Ingres Corporation
*/

#include    <compat.h>
#include    <gl.h>
#include    <cv.h>
#include    <me.h>
#include    <tr.h>
#include    <sl.h>
#include    <st.h>
#include    <iicommon.h>
#include    <adf.h>
#include    <adumoney.h>
#include    <adudate.h>
#include    <adp.h>
#include    <ulf.h>
#include    <adfint.h>
#include    <aduint.h>
#include    <ade.h>

/**
**
**  Name: ADUPRINT.C - prints a DB_DATA_VALUE for debugging.
**
**  Description:
**	  This routine formats and prints a DB_DATA_VALUE struct for
**      debugging.
**
**	  This file defines:
**          adu_prdv() - print a DB_DATA_VALUE struct.
**	    adu_prtype() - print just the data type name of a DB_DATA_VALUE.
**          adu_prvalue() - print just the data value in a DB_DATA_VALUE.
**	    adu_prinstr() - print the name of an instruction.
**
**
**  History:
**      23-apr-86 (ericj)
**          Initially written.
**	26-sep-86 (thurston)
**	    Made adu_prdv() work for char, varchar, and longtext.
**	31-oct-86 (thurston)
**	    Made adu_prdv() a little more robust when it receives bad data.
**	18-nov-86 (thurston)
**	    Made the display of datatype names more readable in adu_prdv().
**      8-dec-86 (eric)
**          added adu_prvalue() and adu_prtype() and changed adu_prdv() 
**	    to use it. Added adu_prinstr(). All of these changes are needed
**	    to support ade_cxbrief_print().
**	24-feb-87 (thurston)
**	    Added support for nullable datatypes.
**	26-feb-87 (thurston)
**	    Made changes to use server CB instead of GLOBALDEFs.
**	12-mar-87 (thurston)
**	    Added support for `bit fields'.  These are currently only used
**	    by ADF to reference the null byte on `nullable pre-instructions'.
**	23-jun-87 (thurston)
**	    Added READONLYs and WSCREADONLYs to static variables as needed.
**	10-feb-88 (thurston)
**	    Added code to adu_prinstr() to display several of the newer (now
**	    old) CX special instruction names.
**	01-jul-88 (thurston)
**	    Undid the change to adu_pr2value() that got rid of the "%.#s"
**	    format for printing out strings by taking two arguments:  length
**	    and string.  I believe EricL must have changed this for some
**	    reason, but it sure does not work the way I had intended it too.
**	13-jul-88 (thurston)
**	    Now, in adu_2prvalue(), we dynamically create the format for
**	    printing out strings so that the "%.#s" style formats don't have to
**	    be used.
**	06-nov-88 (thurston)
**	    Removed some `READONLY' and `WSCREADONLY' stuff from statics.
**	28-feb-89 (fred)
**	    Altered references to Adi_fi_lkup global variable to be thru
**	    Adf_globs for UDADT support.
**	16-mar-89 (jrb)
**	    Added support for decimal.
**	21-apr-89 (mikem)
**	    Logical key development.  Added support for DB_LOGKEY_TYPE and
**	    DB_TABKEY_TYPE.
**	03-aug-89 (jrb)
**	    More decimal support and removed AD_BIT_TYPE support since it is
**	    obsolete.
**	11-dec-89 (fred)
**	    Added support for the ADE_REDEEM operation
**	02-jan-90 (jrb)
**	    Fixed alignment problems.
**	29-jan-1990 (fred)
**	    Added support for the "long varchar"/DB_LVCH_TYPE type.
**	20-nov-91 (seg)
**	    Can't do arithmetic or dereferencing on PTR types.
**      05-jan-1993 (stevet)
**          Added function prototypes.
**      13-Apr-1993 (fred)
**          Added support for bit & byte datatypes.  fixed bug in
**          DB_LVCH_TYPE case.
**	14-jul-93 (ed)
**	    replacing <dbms.h> by <gl.h> <sl.h> <iicommon.h> <dbdbms.h>
**	25-aug-93 (ed)
**	    remove dbdbms.h
**	01-apr-1996 (toumi01)
**	    For axp_osf add cpn_id[5] to debugging prints for expanded
**	    64-bit coupon.
**	26-apr-1999 (hanch04)
**	    Added casts to quite compiler warnings
**      22-Nov-1999 (hweho01)
**          Extended change for axp_osf to ris_u64 (AIX 64-bit platform).
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      16-apr-1999 (hanch04)
**	    For LP64 add cpn_id[5] to debugging prints for expanded
**	    64-bit coupon.
**      08-Sep-2000 (hanje04)
**          Extended change for axp_osf to axp_lnx (Alpha Linux).
**	04-Apr-2001 (gupsh01)
**	    Added Support for long nvarchar.
**	10-Jan-2004 (schka24)
**	    Add i8.
**/

/*
[@forward_type_references@]
[@forward_function_references@]
[@#defines_of_other_constants@]
[@type_definitions@]
[@global_variable_definitions@]
*/


/*
**  Definition of static variables and forward static functions.
*/

#define		ADU_PDTSMALL	0   /* Minimum datatype in this table */

static char	*type_name[] = {
/* 0 - 19 */
    NULL,	    "DB_BOO_TYPE",  NULL,	    "DB_DTE_TYPE",
    NULL,	    "DB_MNY_TYPE",  NULL,	    NULL,
    NULL,	    NULL,	    "DB_DEC_TYPE",  "DB_LOGKEY_TYPE",
    "DB_TABKEY_TYPE",
		    NULL,	    "DB_BIT_TYPE",  "DB_VBIT_TYPE",
    "DB_LBIT_TYPE", "DB_CPN_TYPE",  NULL,	    NULL,
/* 20 - 39 */
    "DB_CHA_TYPE",  "DB_VCH_TYPE",  "DB_LVCH_TYPE", "DB_BYTE_TYPE",
    "DB_VBYTE_TYPE","DB_LBYTE_TYPE",NULL,	    NULL,
    NULL,	    NULL,	    "DB_INT_TYPE",  "DB_FLT_TYPE",
    "DB_CHR_TYPE",  NULL,	    NULL,	    NULL,
    NULL,	    "DB_TXT_TYPE",  NULL,	    NULL,
/* 40 - 59 */
    NULL,	    "DB_LTXT_TYPE", NULL,	    NULL,
    NULL,	    NULL,	    NULL,	    NULL,
    NULL,	    NULL,	    NULL,	    NULL,
    NULL,	    NULL,	    "DB_LOGKEY_TYPE", "DB_TABKEY_TYPE",
    NULL,	    NULL,	    NULL,	    NULL,
/* 60 - 79 */
    NULL,	    NULL,	    NULL,	    NULL,
    NULL,	    NULL,	    NULL,	    NULL,
    NULL,	    NULL,	    NULL,	    NULL,
    NULL,	    NULL,	    NULL,	    NULL,
    NULL,	    NULL,	    NULL,	    NULL,
/* 80 - 99 */
    NULL,	    NULL,	    NULL,	    NULL,
    NULL,	    NULL,	    NULL,	    NULL,
    NULL,	    NULL,	    NULL,	    NULL,
    NULL,	    NULL,	    NULL,	    NULL,
    NULL,	    NULL,	    NULL,	    NULL
};

#define		ADU_PDTBIG     99   /* Maximum datatype in this table */


/*
**  Array of hex characters used when display a bit field:
*/

static char	ad0_hexchars[16] = { '0','1','2','3','4','5','6','7',
				     '8','9','A','B','C','D','E','F' };
/*
[@static_variable_or_function_definition@]...
*/


/*{
** Name: adu_prdv() - Print a DB_DATA_VALUE struct for debugging.
**
** Description:
**        This routine formats and prints a DB_DATA_VALUE for debugging.
**
** Inputs:
**      db_dv                           pointer to the DB_DATA_VALUE to be
**					formated and printed.
**
** Outputs:
**	none
**
**	Returns:
**	    none
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	23-apr-86 (ericj)
**          Initially written.
**	26-sep-86 (thurston)
**	    Made to work for char, varchar, and longtext.
**	31-oct-86 (thurston)
**	    Made a little more robust when handed bad data.
**	18-nov-86 (thurston)
**	    Made the display of datatype names more readable.
**      8-dec-86 (eric)
**          changed to use adu_prtype(), and adu_prvalue().
**	24-feb-87 (thurston)
**	    Added support for nullable datatypes.
**	16-mar-89 (jrb)
**	    Output precision along with type and length (for decimal).
**	17-jul-2006 (gupsh01)
**	    Added support for new ANSI datetime data type.
*/  

VOID
adu_prdv(
DB_DATA_VALUE      *db_dv)
{
    DB_DT_ID	dt;
    DB_DT_ID	bdt;
    i4		len;
    i4		blen;
    PTR		data;

    if (db_dv == NULL)
    {
	TRdisplay("< Pointer to DB_DATA_VALUE is NULL >\n");
	return;
    }

    dt   = bdt  = db_dv->db_datatype;
    len  = blen = db_dv->db_length;
    data        = db_dv->db_data;
    if (dt < 0)
    {
	bdt = -dt;
	blen--;
    }

    TRdisplay("type: %d (", (i4) dt);
    adu_prtype(db_dv);
    TRdisplay(");    prec: [%d,%d]   length: %d\n",
		(i4)DB_P_DECODE_MACRO(db_dv->db_prec),
		(i4)DB_S_DECODE_MACRO(db_dv->db_prec),
		(i4) len);

    if (data == NULL)
    {
	TRdisplay("< pointer to data is NULL >\n");
    }
    else if (ADI_ISNULL_MACRO(db_dv))
    {
	TRdisplay("value: < NULL >\n");
    }
    else
    {
	switch (bdt)
	{
	  case DB_CHA_TYPE:
	  case DB_CHR_TYPE:
	  case DB_DEC_TYPE:
	  case DB_FLT_TYPE:
	  case DB_INT_TYPE:
	  case DB_MNY_TYPE:
	  case DB_BOO_TYPE:
	  case DB_LOGKEY_TYPE:
	  case DB_TABKEY_TYPE:
	  case DB_BYTE_TYPE:
	  case DB_NCHR_TYPE:
	    TRdisplay("value: ");
	    break;

	  case DB_VCH_TYPE:
	    TRdisplay("varchar count: %d\n", 
				((DB_TEXT_STRING *) data)->db_t_count);
	    break;

	  case DB_VBYTE_TYPE:
	    TRdisplay("varbyte count: %d\n", 
				((DB_TEXT_STRING *) data)->db_t_count);
	    break;

	  case DB_TXT_TYPE:
	    TRdisplay("text count: %d\n",
				((DB_TEXT_STRING *) data)->db_t_count);
	    break;

	  case DB_LTXT_TYPE:
	    TRdisplay("longtext count: %d\n", 
				((DB_TEXT_STRING *) data)->db_t_count);
	    break;

	case DB_NVCHR_TYPE:
	    TRdisplay("nvarchar count: %d\n",
                                ((DB_NVCHR_STRING *) data)->count);

	  case DB_DTE_TYPE:
	  case DB_ADTE_TYPE:
	  case DB_TMWO_TYPE:
	  case DB_TMW_TYPE:
	  case DB_TME_TYPE:
	  case DB_TSWO_TYPE:
	  case DB_TSW_TYPE:
	  case DB_TSTMP_TYPE:
	  case DB_INYM_TYPE:
	  case DB_INDS_TYPE:
	  default:
	    break;
	}

	adu_prvalue(db_dv);
	TRdisplay("\n");
    }

    return;
}

/*{
** Name: adu_prtype() - Print the data type of a DB_DATA_VALUE for debugging.
**
** Description:
**        This routine formats and prints the type of a DB_DATA_VALUE 
**	for debugging.
**
** Inputs:
**      db_dv                           pointer to the DB_DATA_VALUE to be
**					formated and printed.
**
** Outputs:
**	none
**
**	Returns:
**	    none
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      8-dec-86 (eric)
**          created, from adu_prdv mostly.
**	24-feb-87 (thurston)
**	    Added support for nullable datatypes.
*/

VOID
adu_prtype(
DB_DATA_VALUE      *db_dv)
{
    DB_DT_ID		dt  = db_dv->db_datatype;
    DB_DT_ID		bdt = abs(dt);


    if (bdt < ADU_PDTSMALL || bdt > ADU_PDTBIG || type_name[bdt] == NULL) 
    {
	TRdisplay("<dt %d>", (i4) dt);
    }       
    else
    {
	if (dt < 0)
	    TRdisplay("Nullable ");
	TRdisplay(type_name[bdt]);
    }

    return;
}


/*{
** Name: adu_prvalue() - Print out just the data value of a DB_DATA_VALUE
**
** Description:
**        This routine formats and prints just the data value of a 
**	DB_DATA_VALUE for debugging. No extra blanks, tabs, new lines, etc.
**	will be added, except for a leading and trailing ' as a delimiter as
**	needed.
**
** Inputs:
**      db_dv                           pointer to the DB_DATA_VALUE to be
**					formated and printed.
**
** Outputs:
**	none
**
**	Returns:
**	    none
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      8-dec-86 (eric)
**          created, from adu_prdv mostly.
**	24-feb-87 (thurston)
**	    Added support for nullable datatypes.
**	12-mar-87 (thurston)
**	    Added support for `bit fields'.  These are currently only used
**	    by ADF to reference the null byte on `nullable pre-instructions'.
**	4-aug-87 (daved)
**	    make this routine use adu_2prvalue.
*/

VOID
adu_prvalue(
DB_DATA_VALUE      *db_dv)
{
    adu_2prvalue(TRdisplay, db_dv);
    return;
}

/*{
** Name: adu_prvalarg() - Print out just the data value of a DB_DATA_VALUE
**
** Description:
**        This routine formats and prints just the data value of a 
**	DB_DATA_VALUE for debugging. No extra blanks, tabs, new lines, etc.
**	will be added, except for a leading and trailing ' as a delimiter as
**	needed.
**
** Inputs:
**      db_dv                           pointer to the DB_DATA_VALUE to be
**					formated and printed.
**	fcn				function to print strings
**
**	arg				Argument to be passed to fcn
**
** Outputs:
**	none
**
**	Returns:
**	    none
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**      7-jul-93 (robf)
**          Created
**	22-mar-94 (robf)
**          Check for printing empty strings, printing a 0-length string
**	    could fail with querytext auditing.
**      04-Apr-2001 (gupsh01)
**          Added Support for long nvarchar.
**	10-Jan-2004 (schka24)
**	    Add i8.
**	17-jul-2006 (gupsh01)
**	    Added support for new ANSI datetime types.
*/

VOID
adu_prvalarg(
i4	(*fcn)(),
PTR	arg,
DB_DATA_VALUE      *db_dv)
{
    char			num_buf[64];
    i2				flt_wid;
    f8				f8_tmp;
    AD_DATENTRNL		*datep;
    AD_NEWDTNTRNL 		newdt;
    AD_NEWDTNTRNL		*ndatep;
    DB_TAB_LOGKEY_INTERNAL  	*tab_logkey;
    DB_OBJ_LOGKEY_INTERNAL  	*obj_logkey;
    DB_DT_ID			bdt;
    i4				blen;
    i4				prec;
    char			*data;
    char			fmt[16];
    i4			tmp;
    i4			tcnt;
    i4				i;
    i4				res_wid;
    i8				i8_tmp;
    f4				f4_tmp;
    i4				i4_tmp;
    i2				i2_tmp;
    i1				i1_tmp;


    if (db_dv == NULL)
    {
	(*fcn)(arg,"< Pointer to DB_DATA_VALUE is NULL >");
	return;
    }

    bdt  = abs(db_dv->db_datatype);
    blen = db_dv->db_length - (db_dv->db_datatype < 0);
    prec = db_dv->db_prec;
    data = db_dv->db_data;

    if (data == NULL)
    {
	(*fcn)(arg,"< pointer to data is NULL >\n");
    }
    else if (ADI_ISNULL_MACRO(db_dv))
    {
	(*fcn)(arg,"< NULL >\n");
    }
    else
    {
	switch (bdt)
	{
	  case DB_LOGKEY_TYPE:

	    obj_logkey = (DB_OBJ_LOGKEY_INTERNAL *) data;

	    (*fcn)(arg,"olk_db_id = %x\tolk_rel_id = %x\n",
		    obj_logkey->olk_db_id, obj_logkey->olk_rel_id);
	    (*fcn)(arg,"olk_high_id = %x\tolk_low_id = %x\n",
		    obj_logkey->olk_high_id, obj_logkey->olk_low_id);
	    
	    break;

	  case DB_TABKEY_TYPE:

	    tab_logkey = (DB_TAB_LOGKEY_INTERNAL *) data;

	    (*fcn)(arg,"tlk_high_id = %x\ttlk_low_id = %x\n",
		    tab_logkey->tlk_high_id, tab_logkey->tlk_low_id);

	    break;

	  case DB_CHA_TYPE:
	  case DB_CHR_TYPE:
	  case DB_BYTE_TYPE:
	    tmp = blen;
	    i = 0;
	    fmt[i++] = '\'';
	    fmt[i++] = '%';
	    if (blen >= 1000)
	    {
		fmt[i++] = (tmp / 1000) + '0';
		tmp = tmp % 1000;
	    }
	    if (blen >= 100)
	    {
		fmt[i++] = (tmp / 100) + '0';
		tmp = tmp % 100;
	    }
	    if (blen >= 10)
	    {
		fmt[i++] = (tmp / 10) + '0';
		tmp = tmp % 10;
	    }
	    fmt[i++] = tmp + '0';
	    fmt[i++] = 's';
	    fmt[i++] = '\'';
	    fmt[i] = EOS;
	    if(blen==0)
	    	(*fcn)(arg,"<empty string>", "");
	    else
	    	(*fcn)(arg,fmt, (u_char *) data);
	    break;

	  case DB_DTE_TYPE:
	    datep = (AD_DATENTRNL *) data;
	    (*fcn)(arg,"dn_status = %x\tdn_highday = %d\tdn_time = %d\n",
		      datep->dn_status, datep->dn_highday, datep->dn_time);
	    (*fcn)(arg,"dn_year = %d\tdn_month = %d\tdn_lowday = %d\n",
		      datep->dn_year, datep->dn_month, datep->dn_lowday);
	    break;

	  case DB_ADTE_TYPE:
	    ndatep = &newdt;
	    adu_6to_dtntrnl(db_dv, ndatep);
	    (*fcn)(arg,"dn_year = %d\tdn_month = %d\tdn_day = %d\n",
		      ndatep->dn_year, ndatep->dn_month, ndatep->dn_day);
	    break;
	  case DB_TMWO_TYPE:
	  case DB_TMW_TYPE:
	  case DB_TME_TYPE:
	    ndatep = &newdt;
	    adu_6to_dtntrnl(db_dv, ndatep);
	    (*fcn)(arg,"dn_time = %d\tdn_nsecond = %d\n",
		      ndatep->dn_time, ndatep->dn_nsecond );
	    (*fcn)(arg,"dn_tzhour\tdn_tzminute = %d\n",
		      ndatep->dn_tzhour, ndatep->dn_tzminute);
	    break;

	  case DB_INYM_TYPE:
	    ndatep = &newdt;
	    adu_6to_dtntrnl(db_dv, ndatep);
	    (*fcn)(arg,"dn_year = %d\tdn_month = %d\n",
		      ndatep->dn_year, ndatep->dn_month);
	    break;

	  case DB_INDS_TYPE:
	    ndatep = &newdt;
	    adu_6to_dtntrnl(db_dv, ndatep);
	    (*fcn)(arg,"dn_day = %d\tdn_time = %d\tdn_nsecond = %d\n",
		      ndatep->dn_day, ndatep->dn_time, ndatep->dn_nsecond);
	    break;

	  case DB_TSWO_TYPE:
	  case DB_TSW_TYPE:
	  case DB_TSTMP_TYPE:
	    ndatep = &newdt;
	    adu_6to_dtntrnl(db_dv, ndatep);
	    (*fcn)(arg,"dn_year = %d\tdn_month = %d\tdn_day = %d\n",
		      ndatep->dn_year, ndatep->dn_month, ndatep->dn_day);
	    (*fcn)(arg,"dn_time = %d\tdn_nsecond = %d\n",
		      ndatep->dn_time, ndatep->dn_nsecond );
	    (*fcn)(arg,"dn_tzhour\tdn_tzminute = %d\n",
		      ndatep->dn_tzhour, ndatep->dn_tzminute);
	    break;

	  case DB_DEC_TYPE:
	    CVpka(data,
		(i4)DB_P_DECODE_MACRO(prec),
		(i4)DB_S_DECODE_MACRO(prec),
		'.',
		(i4)DB_P_DECODE_MACRO(prec) + 5,
		(i4)DB_S_DECODE_MACRO(prec),
		CV_PKLEFTJUST,
		num_buf,
		&res_wid);
		
	    tmp = res_wid;
	    i = 0;
	    fmt[i++] = '%';
	    if (tmp >= 10)
	    {
		fmt[i++] = (tmp / 10) + '0';
		tmp = tmp % 10;
	    }
	    fmt[i++] = tmp + '0';
	    fmt[i++] = 's';
	    fmt[i] = EOS;
	    (*fcn)(arg,fmt, num_buf);
	    break;
	    
	  case DB_FLT_TYPE:
	    if (blen == 4)
	    {
		F4ASSIGN_MACRO(*data, f4_tmp);
		f8_tmp = f4_tmp;
	    }
	    else
		F8ASSIGN_MACRO(*data, f8_tmp);

	    CVfa(f8_tmp, (i4) 32, (i4) 4, 'f', '.', num_buf, &flt_wid);
	    tmp = flt_wid;
	    i = 0;
	    fmt[i++] = '%';
	    if (flt_wid >= 100)
	    {
		fmt[i++] = (tmp / 100) + '0';
		tmp = tmp % 100;
	    }
	    if (flt_wid >= 10)
	    {
		fmt[i++] = (tmp / 10) + '0';
		tmp = tmp % 10;
	    }
	    fmt[i++] = tmp + '0';
	    fmt[i++] = 's';
	    fmt[i] = EOS;
	    (*fcn)(arg,fmt, num_buf);
	    break;

	  case DB_INT_TYPE:
	    if (blen == 4)
	    {
		I4ASSIGN_MACRO(*data, i4_tmp);
	    }
	    else if (blen == 2)
	    {
		I2ASSIGN_MACRO(*data, i2_tmp);
		i4_tmp = i2_tmp;
	    }
	    else if (blen == 1)
	    {
		i4_tmp = I1_CHECK_MACRO(*(i1 *)data);
	    }
	    else /* blen == 8 */
	    {
		/* Depending on host, printing i8's might be %ld (lp64)
		** or %lld (lpi32).  Avoid issues by converting to string.
		*/
		I8ASSIGN_MACRO(*data,i8_tmp);
		CVla8(i8_tmp,&num_buf[0]);
		(*fcn)(arg,"%s",num_buf);
		break;
	    }
	    (*fcn)(arg,"%d", i4_tmp);
	    break;

	  case DB_MNY_TYPE:
	    f8_tmp = ((AD_MONEYNTRNL *) data)->mny_cents / 100.0;
	    CVfa(f8_tmp, 32, 2, 'f', '.', num_buf, &flt_wid);
	    tmp = flt_wid;
	    i = 0;
	    fmt[i++] = '%';
	    if (flt_wid >= 100)
	    {
		fmt[i++] = (tmp / 100) + '0';
		tmp = tmp % 100;
	    }
	    if (flt_wid >= 10)
	    {
		fmt[i++] = (tmp / 10) + '0';
		tmp = tmp % 10;
	    }
	    fmt[i++] = tmp + '0';
	    fmt[i++] = 's';
	    fmt[i] = EOS;
	    (*fcn)(arg,fmt, num_buf);
	    break;

	  case DB_VCH_TYPE:
	  case DB_TXT_TYPE:
	  case DB_LTXT_TYPE:
	  case DB_VBYTE_TYPE:
	    I2ASSIGN_MACRO(((DB_TEXT_STRING *)data)->db_t_count, i2_tmp);
	    tmp = tcnt = i2_tmp;
	    i = 0;
	    fmt[i++] = '\'';
	    fmt[i++] = '%';
	    if (tcnt >= 1000)
	    {
		fmt[i++] = (tmp / 1000) + '0';
		tmp = tmp % 1000;
	    }
	    if (tcnt >= 100)
	    {
		fmt[i++] = (tmp / 100) + '0';
		tmp = tmp % 100;
	    }
	    if (tcnt >= 10)
	    {
		fmt[i++] = (tmp / 10) + '0';
		tmp = tmp % 10;
	    }
	    fmt[i++] = tmp + '0';
	    fmt[i++] = 's';
	    fmt[i++] = '\'';
	    fmt[i] = EOS;
	    if(tcnt==0)
		    (*fcn)(arg,"<empty string>", "");
	    else
		    (*fcn)(arg,fmt, ((DB_TEXT_STRING *) data)->db_t_text);
	    break;

	  case DB_BOO_TYPE:
	    if (*((bool *) data))
		(*fcn)(arg,"TRUE");
	    else
		(*fcn)(arg,"FALSE");
	    break;

	  case DB_LVCH_TYPE:
  	  case DB_LBYTE_TYPE:
	  case DB_LNVCHR_TYPE:
	    {
		ADP_PERIPHERAL	    *p = (ADP_PERIPHERAL *) data;
		ADP_COUPON	    *cpn;
		
		(*fcn)(arg,"%s/length = (%d., %0d.):  ",
			(p->per_tag == ADP_P_COUPON ? "ADP_P_COUPON" :
				    "ADP_P_DATA"),
			p->per_length0, p->per_length1);
		if (p->per_tag == ADP_P_COUPON)
		{
		    cpn = &p->per_value.val_coupon;
		    
#if defined(axp_osf) || defined(ris_u64) || defined(LP64) || \
    defined(axp_lnx)
		    (*fcn)(arg,"(%d.,%d.,%d.,%d.,%d.,%d.)",
			cpn->cpn_id[0],cpn->cpn_id[1],
			cpn->cpn_id[2],cpn->cpn_id[3],
			cpn->cpn_id[4],cpn->cpn_id[5]);
#else
		    (*fcn)(arg,"(%d.,%d.,%d.,%d.,%d.)",
			cpn->cpn_id[0],cpn->cpn_id[1],
			cpn->cpn_id[2],cpn->cpn_id[3],
			cpn->cpn_id[4]);
#endif
		}
		else
		{
		    (*fcn)(arg,"(first %d. of value) '%t'", min(100, p->per_length1),
			    min(100, p->per_length1), p->per_value.val_value);
		}
			
	    }
	    break;
	  default:
	    (*fcn)(arg, "< unknown type: don't know how to interpret data >");
	    break;
	}
    }
    return;
}

/*{
** Name: adu_2prvalue() - Print out just the data value of a DB_DATA_VALUE
**
** Description:
**        This routine formats and prints just the data value of a 
**	DB_DATA_VALUE for debugging. No extra blanks, tabs, new lines, etc.
**	will be added, except for a leading and trailing ' as a delimiter as
**	needed.
**
** Inputs:
**	fcn				function to print strings
**      db_dv                           pointer to the DB_DATA_VALUE to be
**					formated and printed.
**
** Outputs:
**	none
**
**	Returns:
**	    none
**
**	Exceptions:
**	    none
**
** Side Effects:
**	    none
**
** History:
**	04-aug-87 (daved)
**	    written
**	01-jul-88 (thurston)
**	    Undid the change that got rid of the "%.#s" format for printing out
**	    strings by taking two arguments:  length and string.  I believe
**	    EricL must have changed this for some reason, but it sure does not
**	    work the way I had intended it too.
**	13-jul-88 (thurston)
**	    Dynamically created the format for printing out strings so that the
**	    "%.#s" style formats don't have to be used.
**	16-mar-89 (jrb)
**	    Added decimal datatype.
**	24-jul-89 (jrb)
**	    Change interface for CVpka.
**	02-jan-90 (jrb)
**	    Fixed alignment problems.
**	20-nov-91 (seg)
**	    Can't do arithmetic or dereferencing on PTR types.
**      13-Apr-1993 (fred)
**          Added support for byte datatypes.  fixed long varchar bug
**          (lack of `break' statement).
**	11-oct-93 (swm)
**	    Bug #56448
**	    Unfortunately, not all fcn function pointers passed to
**	    adu_2prvalue() are varargs functions. So for those that
**	    not, format the output string here using STprintf (which is
**	    a varargs function) and pass the resulting formatted output
**	    as a single argument to (*fcn)().
**	22-may-01 (hayke02)
**	    Added newline character ('\n') to end of NULL pointer message.
**	10-Jan-2004 (schka24)
**	    Add i8.
**	07-Sep-2006 (gupsh01)
**	    Added support for ANSI date/time types.
*/

VOID
adu_2prvalue(
i4		    (*fcn)(char *, ...),
DB_DATA_VALUE      *db_dv)
{
    char			num_buf[64];
    i2				flt_wid;
    f8				f8_tmp;
    AD_DATENTRNL		*datep;
    AD_NEWDTNTRNL 		newdt;
    AD_NEWDTNTRNL		*ndatep;
    DB_TAB_LOGKEY_INTERNAL  	*tab_logkey;
    DB_OBJ_LOGKEY_INTERNAL  	*obj_logkey;
    DB_DT_ID			bdt;
    i4				blen;
    i4				prec;
    char			*data;
    char			fmt[16];
    i4			tmp;
    i4			tcnt;
    i4				i;
    i4				res_wid;
    i8				i8_tmp;
    f4				f4_tmp;
    i4				i4_tmp;
    i2				i2_tmp;
    i1				i1_tmp;
    char			stbuf[2048];


    if (db_dv == NULL)
    {
	(*fcn)("< Pointer to DB_DATA_VALUE is NULL >\n");
	return;
    }

    bdt  = abs(db_dv->db_datatype);
    blen = db_dv->db_length - (db_dv->db_datatype < 0);
    prec = db_dv->db_prec;
    data = db_dv->db_data;

    if (data == NULL)
    {
	(*fcn)("< pointer to data is NULL >\n");
    }
    else if (ADI_ISNULL_MACRO(db_dv))
    {
	(*fcn)("< NULL >\n");
    }
    else
    {
	switch (bdt)
	{
	  case DB_LOGKEY_TYPE:

	    obj_logkey = (DB_OBJ_LOGKEY_INTERNAL *) data;

	    (*fcn)(STprintf(stbuf, "olk_db_id = %x\tolk_rel_id = %x\n",
		    obj_logkey->olk_db_id, obj_logkey->olk_rel_id));
	    (*fcn)(STprintf(stbuf, "olk_high_id = %x\tolk_low_id = %x\n",
		    obj_logkey->olk_high_id, obj_logkey->olk_low_id));
	    
	    break;

	  case DB_TABKEY_TYPE:

	    tab_logkey = (DB_TAB_LOGKEY_INTERNAL *) data;

	    (*fcn)(STprintf(stbuf, "tlk_high_id = %x\ttlk_low_id = %x\n",
		    tab_logkey->tlk_high_id, tab_logkey->tlk_low_id));

	    break;

	  case DB_CHA_TYPE:
	  case DB_CHR_TYPE:
	  case DB_BYTE_TYPE:
	    tmp = blen;
	    i = 0;
	    fmt[i++] = '\'';
	    fmt[i++] = '%';
	    if (blen >= 1000)
	    {
		fmt[i++] = (tmp / 1000) + '0';
		tmp = tmp % 1000;
	    }
	    if (blen >= 100)
	    {
		fmt[i++] = (tmp / 100) + '0';
		tmp = tmp % 100;
	    }
	    if (blen >= 10)
	    {
		fmt[i++] = (tmp / 10) + '0';
		tmp = tmp % 10;
	    }
	    fmt[i++] = tmp + '0';
	    fmt[i++] = 's';
	    fmt[i++] = '\'';
	    fmt[i] = EOS;
	    (*fcn)(STprintf(stbuf, fmt, (u_char *) data));
	    break;

	  case DB_DTE_TYPE:
	    datep = (AD_DATENTRNL *) data;
	    (*fcn)(STprintf(stbuf,
		      "dn_status = %x\tdn_highday = %d\tdn_time = %d\n",
		      datep->dn_status, datep->dn_highday, datep->dn_time));
	    (*fcn)(STprintf(stbuf,
		      "dn_year = %d\tdn_month = %d\tdn_lowday = %d",
		      datep->dn_year, datep->dn_month, datep->dn_lowday));
	    break;

	  case DB_ADTE_TYPE:
	    ndatep = &newdt;
	    adu_6to_dtntrnl(db_dv, ndatep);
	    (*fcn)(STprintf(stbuf,"dn_year = %d\tdn_month = %d\tdn_day = %d\n",
		      ndatep->dn_year, ndatep->dn_month, ndatep->dn_day));
	    break;

	  case DB_TMWO_TYPE:
	  case DB_TMW_TYPE:
	  case DB_TME_TYPE:
	    ndatep = &newdt;
	    adu_6to_dtntrnl(db_dv, ndatep);
	    (*fcn)(STprintf(stbuf,"dn_time = %d\tdn_nsecond = %d\n",
		      ndatep->dn_time, ndatep->dn_nsecond ));
	    (*fcn)(STprintf(stbuf,"dn_tzhour\tdn_tzminute = %d\n",
		      ndatep->dn_tzhour, ndatep->dn_tzminute));
	    break;

	  case DB_INYM_TYPE:
	    ndatep = &newdt;
	    adu_6to_dtntrnl(db_dv, ndatep);
	    (*fcn)(STprintf(stbuf,"dn_year = %d\tdn_month = %d\n",
		      ndatep->dn_year, ndatep->dn_month));
	    break;

	  case DB_INDS_TYPE:
	    ndatep = &newdt;
	    adu_6to_dtntrnl(db_dv, ndatep);
	    (*fcn)(STprintf(stbuf,"dn_day = %d\tdn_time = %d\tdn_nsecond = %d\n",
		      ndatep->dn_day, ndatep->dn_time, ndatep->dn_nsecond));
	    break;

	  case DB_TSWO_TYPE:
	  case DB_TSW_TYPE:
	  case DB_TSTMP_TYPE:
	    ndatep = &newdt;
	    adu_6to_dtntrnl(db_dv, ndatep);
	    (*fcn)(STprintf(stbuf,"dn_year = %d\tdn_month = %d\tdn_day = %d\n",
		      ndatep->dn_year, ndatep->dn_month, ndatep->dn_day));
	    (*fcn)(STprintf(stbuf,"dn_time = %d\tdn_nsecond = %d\n",
		      ndatep->dn_time, ndatep->dn_nsecond ));
	    (*fcn)(STprintf(stbuf,"dn_tzhour\tdn_tzminute = %d\n",
		      ndatep->dn_tzhour, ndatep->dn_tzminute));
	    break;

	  case DB_DEC_TYPE:
	    CVpka(data,
		(i4)DB_P_DECODE_MACRO(prec),
		(i4)DB_S_DECODE_MACRO(prec),
		'.',
		(i4)DB_P_DECODE_MACRO(prec) + 5,
		(i4)DB_S_DECODE_MACRO(prec),
		CV_PKLEFTJUST,
		num_buf,
		&res_wid);
		
	    tmp = res_wid;
	    i = 0;
	    fmt[i++] = '%';
	    if (tmp >= 10)
	    {
		fmt[i++] = (tmp / 10) + '0';
		tmp = tmp % 10;
	    }
	    fmt[i++] = tmp + '0';
	    fmt[i++] = 's';
	    fmt[i] = EOS;
	    (*fcn)(STprintf(stbuf, fmt, num_buf));
	    break;
	    
	  case DB_FLT_TYPE:
	    if (blen == 4)
	    {
		F4ASSIGN_MACRO(*data, f4_tmp);
		f8_tmp = f4_tmp;
	    }
	    else
		F8ASSIGN_MACRO(*data, f8_tmp);

	    CVfa(f8_tmp, (i4) 32, (i4) 4, 'f', '.', num_buf, &flt_wid);
	    tmp = flt_wid;
	    i = 0;
	    fmt[i++] = '%';
	    if (flt_wid >= 100)
	    {
		fmt[i++] = (tmp / 100) + '0';
		tmp = tmp % 100;
	    }
	    if (flt_wid >= 10)
	    {
		fmt[i++] = (tmp / 10) + '0';
		tmp = tmp % 10;
	    }
	    fmt[i++] = tmp + '0';
	    fmt[i++] = 's';
	    fmt[i] = EOS;
	    (*fcn)(STprintf(stbuf, fmt, num_buf));
	    break;

	  case DB_INT_TYPE:
	    if (blen == 4)
	    {
		I4ASSIGN_MACRO(*data, i4_tmp);
	    }
	    else if (blen == 2)
	    {
		I2ASSIGN_MACRO(*data, i2_tmp);
		i4_tmp = i2_tmp;
	    }
	    else if (blen == 1)
	    {
		i4_tmp = I1_CHECK_MACRO(*(i1 *)data);
	    }
	    else /* blen == 8 */
	    {
		/* Depending on host, printing i8's might be %ld (lp64)
		** or %lld (lpi32).  Avoid issues by converting to string.
		*/
		I8ASSIGN_MACRO(*data,i8_tmp);
		CVla8(i8_tmp,&num_buf[0]);
		(*fcn)(num_buf);
		break;
	    }
	    (*fcn)(STprintf(stbuf, "%d", i4_tmp));
	    break;

	  case DB_MNY_TYPE:
	    f8_tmp = ((AD_MONEYNTRNL *) data)->mny_cents / 100.0;
	    CVfa(f8_tmp, 32, 2, 'f', '.', num_buf, &flt_wid);
	    tmp = flt_wid;
	    i = 0;
	    fmt[i++] = '%';
	    if (flt_wid >= 100)
	    {
		fmt[i++] = (tmp / 100) + '0';
		tmp = tmp % 100;
	    }
	    if (flt_wid >= 10)
	    {
		fmt[i++] = (tmp / 10) + '0';
		tmp = tmp % 10;
	    }
	    fmt[i++] = tmp + '0';
	    fmt[i++] = 's';
	    fmt[i] = EOS;
	    (*fcn)(STprintf(stbuf, fmt, num_buf));
	    break;

	  case DB_VCH_TYPE:
	  case DB_TXT_TYPE:
	  case DB_LTXT_TYPE:
	  case DB_VBYTE_TYPE:
	    I2ASSIGN_MACRO(((DB_TEXT_STRING *)data)->db_t_count, i2_tmp);
	    tmp = tcnt = i2_tmp;
	    i = 0;
	    fmt[i++] = '\'';
	    fmt[i++] = '%';
	    if (tcnt >= 1000)
	    {
		fmt[i++] = (tmp / 1000) + '0';
		tmp = tmp % 1000;
	    }
	    if (tcnt >= 100)
	    {
		fmt[i++] = (tmp / 100) + '0';
		tmp = tmp % 100;
	    }
	    if (tcnt >= 10)
	    {
		fmt[i++] = (tmp / 10) + '0';
		tmp = tmp % 10;
	    }
	    fmt[i++] = tmp + '0';
	    fmt[i++] = 's';
	    fmt[i++] = '\'';
	    fmt[i] = EOS;
	    (*fcn)(STprintf(stbuf, fmt, ((DB_TEXT_STRING *) data)->db_t_text));
	    break;

	  case DB_BOO_TYPE:
	    if (*((bool *) data))
		(*fcn)("TRUE");
	    else
		(*fcn)("FALSE");
	    break;

	  case DB_NCHR_TYPE:
	    for(i=0; i< blen/2; i++)
	    {
                I2ASSIGN_MACRO(data[i], i2_tmp);
                i4_tmp = i2_tmp;
                (*fcn)(STprintf(stbuf,"%x ,", i4_tmp));
	     }
             break;

          case DB_NVCHR_TYPE:
            I2ASSIGN_MACRO(((DB_NVCHR_STRING *)data)->count, i2_tmp);
	    for(i=0; i< i2_tmp; i++)
            {
                I2ASSIGN_MACRO(((DB_NVCHR_STRING *)data)->element_array[i], tmp);
                i4_tmp = tmp;
		(*fcn)(STprintf(stbuf,"%x ,", i4_tmp));
             }
             break;

	  case DB_LVCH_TYPE:
  	  case DB_LBYTE_TYPE:
	  case DB_LNVCHR_TYPE:
	    {
		ADP_PERIPHERAL	    *p = (ADP_PERIPHERAL *) data;
		ADP_COUPON	    *cpn;
		
		(*fcn)(STprintf(stbuf, "%s/length = (%d., %0d.):  ",
			(p->per_tag == ADP_P_COUPON ? "ADP_P_COUPON" :
				    "ADP_P_DATA"),
			p->per_length0, p->per_length1));
		if (p->per_tag == ADP_P_COUPON)
		{
		    cpn = &p->per_value.val_coupon;
		    
#if defined(axp_osf) || defined(ris_u64) || defined(LP64) || \
    defined(axp_lnx)
		    (*fcn)(STprintf(stbuf, "(%d.,%d.,%d.,%d.,%d.,%d.)",
			cpn->cpn_id[0],cpn->cpn_id[1],
			cpn->cpn_id[2],cpn->cpn_id[3],
			cpn->cpn_id[4],cpn->cpn_id[5]));
#else
		    (*fcn)(STprintf(stbuf, "(%d.,%d.,%d.,%d.,%d.)",
			cpn->cpn_id[0],cpn->cpn_id[1],
			cpn->cpn_id[2],cpn->cpn_id[3],
			cpn->cpn_id[4]));
#endif
		}
		else
		{
		    (*fcn)(STprintf(stbuf, "(first %d. of value) '%t'",
			    min(100, p->per_length1),
			    min(100, p->per_length1),
			    p->per_value.val_value));
		}
			
	    }
	    break;
	  default:
	    (*fcn)("< unknown type: don't know how to interpret data >");
	    break;
	}
    }

    return;
}


/*{
** Name: ADU_PRINSTR	- Print out the name of an instruction
**
** Description:
**      This routine takes a ADE_INSTRUCTION and prints out the name of the
**      instruction for debugging. For an example, see ade_ibrief_print() 
**      in adedebug.c
**
** Inputs:
**      instr                           The instruction to print.
**
** Outputs:
**	none.
**
**	Returns:
**	    none
**	Exceptions:
**	    none
**
** Side Effects:
**	    Trace output is produced.
**
** History:
**      8-dec-86 (eric)
**          initially created.
**	12-mar-87 (thurston)
**	    Added code to print out new CX special instructions that deal with
**	    nullable datatype.
**	10-feb-88 (thurston)
**	    Added code to display several of the newer (now old) CX special
**	    instruction names.
*/

VOID
adu_prinstr(
ADI_FI_ID	    *instr)
{
    ADI_OPRATION        *op;
    ADI_OP_ID           adi_oid;
    ADI_FI_DESC         *fi;
    ADI_FI_ID		adi_fid = *instr;

    if (adi_fid < 0)
    {
	/* print out the special CX instruction. */
	switch (adi_fid)
	{
	 case ADE_AND:
	    TRdisplay("ADE_AND");
	    break;

	 case ADE_OR:
	    TRdisplay("ADE_OR");
	    break;

	 case ADE_NOT:
	    TRdisplay("ADE_NOT");
	    break;

	 case ADE_MECOPY:
	    TRdisplay("ADE_MECOPY");
	    break;

	 case ADE_AGBGN:
	    TRdisplay("ADE_AGBGN");
	    break;

	 case ADE_AGEND:
	    TRdisplay("ADE_AGEND");
	    break;

	 case ADE_KEYBLD:
	    TRdisplay("ADE_KEYBLD");
	    break;

	 case ADE_CALCLEN:
	    TRdisplay("ADE_CALCLEN");
	    break;

	 case ADE_COMPARE:
	    TRdisplay("ADE_COMPARE");
	    break;

	 case ADE_NAGEND:
	    TRdisplay("ADE_NAGEND");
	    break;

	 case ADE_PMQUEL:
	    TRdisplay("ADE_PMQUEL");
	    break;

	 case ADE_NO_PMQUEL:
	    TRdisplay("ADE_NO_PMQUEL");
	    break;

	 case ADE_PMFLIPFLOP:
	    TRdisplay("ADE_PMFLIPFLOP");
	    break;

	 case ADE_PMENCODE:
	    TRdisplay("ADE_PMENCODE");
	    break;

	 case ADE_BYCOMPARE:
	    TRdisplay("ADE_BYCOMPARE");
	    break;

	 case ADE_0CXI_IGNORE:
	    TRdisplay("ADE_0CXI_IGNORE");
	    break;

	 case ADE_1CXI_SET_SKIP:
	    TRdisplay("ADE_1CXI_SET_SKIP");
	    break;

	 case ADE_3CXI_CMP_SET_SKIP:
	    TRdisplay("ADE_3CXI_CMP_SET_SKIP");
	    break;

	 case ADE_4CXI_CMP_SET_SKIP:
	    TRdisplay("ADE_4CXI_CMP_SET_SKIP");
	    break;

	 case ADE_2CXI_SKIP:
	    TRdisplay("ADE_2CXI_SKIP");
	    break;

	 case ADE_REDEEM:
	    TRdisplay("ADE_REDEEM");
	    break;

	  default:
	    TRdisplay("< unknown CX instr %d >", (i4)adi_fid);
	    break;
	}
	return;
    }

    /* get the fi_desc for this fiid */
    fi = (ADI_FI_DESC *) NULL;
    if (adi_fid >= (ADI_FI_ID) 0 && adi_fid <= Adf_globs->Adi_fi_biggest)
    {
	fi = (ADI_FI_DESC *)
		Adf_globs->Adi_fi_lkup[ADI_LK_MAP_MACRO((i4) adi_fid)].adi_fi;
    }

    /* get the op id from the fi_desc */
    if (fi == NULL)
    {
	TRdisplay("< unknown fiid %d >", (i4) adi_fid);
	return;
    }
    else
    {
	adi_oid = fi->adi_fiopid;
    }

    /* get the opname from the opid */
    for (op = &Adf_globs->Adi_operations[0]; op->adi_opid != ADI_NOOP; op++)
    {
	if (op->adi_opid == adi_oid)
	{
	    TRdisplay(op->adi_opname.adi_opname);
	    return;
	}
    }

    TRdisplay("< unknown operator %d>", adi_oid);
    return;
}
